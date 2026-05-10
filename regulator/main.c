#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sched.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#include "zcu_regs.h"
#include "etm_regs.h"
#include "cti_regs.h"
#include "pmu_regs.h"
#include "dbg_regs.h"
#include "setup.h"
#include "common.h"
#include "regulators.h"

/**
 * Intended Platform: ZynqMP based SoCs
 * which have 4 Cortex-A53 Cores
 */

#define NUM_CORES               4


volatile ETM_interface *etms[NUM_CORES];
volatile CTI_interface *ctis[NUM_CORES];
volatile DBG_interface *dbgs[NUM_CORES];
volatile PMU_interface *pmus[NUM_CORES];

struct etm_config config_per_cpu[NUM_CORES];

volatile sig_atomic_t keep_running = 1;

/* Signal Handler to Exit at CTRL^C*/
static void sig_handler(int _) {
    (void)_; 
    keep_running = 0;
}

void usage()
{
    printf("\n"
        "ETM Regulator Usage\n"
        "--budget(-b):  sets the combined budget in ETM counter for PMU\n"
        "--cycle(-c):   sets the regulation period\n"
        "--mode(-m)\n"
        "\t 0 for No Regulation \n"
        "\t 1 for ETM PR algorithm\n"
        "\t 2 for ETM TB 1:3 algorithm\n"
        "\t 3 for ETM TB 2:2 algorithm\n"
        "\t 4 for ETM TB 3:1 algorithm\n"
        "number of x after --budget(-b) is equal to number of cores\n"
        "cores without a budget are essentially not regulated\n"
        "./etm_regulator --budget x x x x --cycles x --mode x\n"
        "Eg: ./etm_regulator --budget 31 62 125 250 --cycles 24000 --mode 1\n"
        "Or\n"
        "Eg: ./etm_regulator -b 31 62 125 250 -c 24000 -m 2\n"
        "This sets up regulation on Core 0 to 3 at 100, 200, 400, 800 MB/s respectively\n"
    );
}
int main(int argc, char* argv[])
{
    int mode = -1;
    int cycles = -1;
    int budgets[NUM_CORES] = {0};
    int budget_count = 0;

    for (int c = 0; c < NUM_CORES; c++)
        linux_disable_cpuidle_cpu(c);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--budget") == 0 || strcmp(argv[i], "-b") == 0) {
            i++;
            while (i < argc && argv[i][0] != '-') {
                if (budget_count >= NUM_CORES) {
                    printf("Error: Too many budget values\n");
                    usage();
                    return -1;
                }
                budgets[budget_count++] = atoi(argv[i]);
                i++;
            }
            i--;
        }
        else if (strcmp(argv[i], "--cycle") == 0 || strcmp(argv[i], "-c") == 0) {
            if (i + 1 >= argc) {
                printf("Error: Missing cycle value\n");
                usage();
                return -1;
            }
            cycles = atoi(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "--mode") == 0 || strcmp(argv[i], "-m") == 0) {
            if (i + 1 >= argc) {
                printf("Error: Missing mode value\n");
                usage();
                return -1;
            }
            mode = atoi(argv[i + 1]);
            i++;
        }
    }

    if (mode < 0 || mode > 4) {
        printf("Error: Invalid mode (must be 0 - 4)\n");
        usage();
        return -1;
    }
    if (cycles > 0xffff) {
        printf("Error: Invalid cycles value\n");
        usage();
        return -1;
    } else if (cycles == 0) {
        printf("Warning: No cycle count provided. Setting to 65535\n");
        cycles = 0xffff;
    }
    if (budget_count == 0) {
        printf("Warning: No budget values provided. Setting to all to 65535\n");
    }

    for (int c = 0; c < NUM_CORES; c++) {
        config_per_cpu[c].mode = mode;
        config_per_cpu[c].cycles = cycles;
        config_per_cpu[c].budget = (c < budget_count) ? budgets[c] : 0xffff;
        config_per_cpu[c].throttle_count = 0; // Initialize to 0, to be used in IRQ (in kernel module)

        if (config_per_cpu[c].budget == 0) {
            config_per_cpu[c].budget = 0xffff;
        }
    }

    printf("Configuration:\n");
    printf("Mode: %d\n", config_per_cpu[0].mode);
    printf("Cycles: %d\n", config_per_cpu[0].cycles);
    printf("Budgets per core:\n");
    for (int c = 0; c < NUM_CORES; c++) {
        printf("Core %d: %u\n", c, config_per_cpu[c].budget);
    }

    for (int c = 0; c < NUM_CORES; c++) {
        etms[c] = (volatile ETM_interface *) cs_register(A53_0_etm + c);
        ctis[c] = (volatile CTI_interface *) cs_register(A53_0_cti + c);
        dbgs[c] = (volatile DBG_interface *) cs_register(A53_0_debug + c);
        pmus[c] = (volatile PMU_interface *) cs_register(A53_0_pmu + c);
    }

    /**
     * Setup ETM regulation on all cores
     */ 
    for (int c = 0; c < NUM_CORES; c++) {
        printf("Setup Core %d\n", c);
        /* init debug */
        dbg_init_minimal(dbgs[c]);

        /* Set up the PMU and export PMU events */
        pmu_unlock(pmus[c]);
        pmu_enable_export_events(pmus[c]);


        /* init and program all CTIs */
        cti_init(ctis[c]);
        cti_setup(ctis[c]);

        /* Program the ETM for all cores */
        trc_unlock(etms[c]);
        trc_disable(etms[c]);
        trc_reset(etms[c]);
        trc_setup_common(etms[c]);
        if (config_per_cpu[c].mode == ETM_MODE_OFF) {
            /* Do nothing */
        } else if (config_per_cpu[c].mode == ETM_MODE_PR) {
            trc_setup_periodic_replenish(etms[c], config_per_cpu[c].budget, config_per_cpu[c].cycles);
        } else if (config_per_cpu[c].mode == ETM_MODE_TB_1_3) {
            trc_setup_token_bucket_1_3(etms[c], config_per_cpu[c].budget, config_per_cpu[c].cycles);
        } else if (config_per_cpu[c].mode == ETM_MODE_TB_2_2) {
            trc_setup_token_bucket_2_2(etms[c], config_per_cpu[c].budget, config_per_cpu[c].cycles);
        } else if (config_per_cpu[c].mode == ETM_MODE_TB_3_1) {
            trc_setup_token_bucket_3_1(etms[c], config_per_cpu[c].budget, config_per_cpu[c].cycles);
        } 

        trc_enable(etms[c]);
    }
    
    printf("Enabled ETM Regulator\nUse CTRL^C to disable regulation and exit\n");

    signal(SIGINT, sig_handler);
    uint64_t i = 0;
    while (keep_running) {
        usleep(10000);
        i++; 
    }
    printf("\ni = %ld\n", i);

    for (int c = 0; c < NUM_CORES; c++) {
        trc_disable(etms[c]);
        trc_reset(etms[c]);
        munmap((void *)etms[c], sizeof(ETM_interface));
        munmap((void *)ctis[c], sizeof(CTI_interface));
        munmap((void *)dbgs[c], sizeof(DBG_interface));
        munmap((void *)pmus[c], sizeof(PMU_interface));
    }
    
}
