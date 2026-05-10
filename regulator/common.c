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

#include "zcu_regs.h"
#include "etm_regs.h"
#include "cti_regs.h"
#include "pmu_regs.h"
#include "dbg_regs.h"
#include "common.h"

/* disable cpuidle for the given core
 *
 * returns 0 on success, error otherwise
 */
int linux_disable_cpuidle_cpu(unsigned int cpu) //
{
	char s[80];
	ssize_t w;
	int err;
	int fd;

	sprintf(s, "/sys/devices/system/cpu/cpu%u/cpuidle/state1/disable", cpu);

	fd = open(s, O_RDWR);
	if (fd == -1) {
		err = errno;
		if (err == ENOENT) {
			/* on "file-not-found", cpuidle is already deactivated */
			err = 0;
		}
		return err;
	}

	w = write(fd, "1", 1);
	err = errno;

	close(fd);

	return w == 1 ? 0 : err;
}

volatile void* cs_register(enum component comp)
{
    volatile void* ptr = NULL;
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("Cannot open /dev/mem\n");
        exit(1);
    }

    switch(comp) {
        case A53_0_etm:
            ptr = mmap(NULL, sizeof(ETM_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_0_ETM);
            break;
        case A53_0_pmu:
            ptr = mmap(NULL, sizeof(PMU_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_0_PMU);
            break;
        case A53_0_cti:
            ptr = mmap(NULL, sizeof(CTI_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_0_CTI);
            break;
        case A53_0_debug:
            ptr = mmap(NULL, sizeof(CTI_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_0_DEBUG);
            break;
        case A53_1_etm:
            ptr = mmap(NULL, sizeof(ETM_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_1_ETM);
            break;
        case A53_1_pmu:
            ptr = mmap(NULL, sizeof(PMU_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_1_PMU);
            break;
        case A53_1_cti:
            ptr = mmap(NULL, sizeof(CTI_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_1_CTI);
            break;
        case A53_1_debug:
            ptr = mmap(NULL, sizeof(CTI_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_1_DEBUG);
            break;
        case A53_2_etm:
            ptr = mmap(NULL, sizeof(ETM_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_2_ETM);
            break;
        case A53_2_pmu:
            ptr = mmap(NULL, sizeof(PMU_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_2_PMU);
            break;
        case A53_2_cti:
            ptr = mmap(NULL, sizeof(CTI_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_2_CTI);
            break;
        case A53_2_debug:
            ptr = mmap(NULL, sizeof(CTI_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_2_DEBUG);
            break;
        case A53_3_etm:
            ptr = mmap(NULL, sizeof(ETM_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_3_ETM);
            break;
        case A53_3_pmu:
            ptr = mmap(NULL, sizeof(PMU_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_3_PMU);
            break;
        case A53_3_cti:
            ptr = mmap(NULL, sizeof(CTI_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_3_CTI);
            break;
        case A53_3_debug:
            ptr = mmap(NULL, sizeof(CTI_interface), PROT_READ | PROT_WRITE, MAP_SHARED, fd, CS_BASE + A53_3_DEBUG);
            break;
        default:
            fprintf(stderr, "Unimplemented component %d\n", comp);
            exit(1);
            break;
    }

	if (ptr == MAP_FAILED)
		fprintf(stderr,"mmap to component %d failed!\n", comp);
	close(fd);
    return ptr;
}

