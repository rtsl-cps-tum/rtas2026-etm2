#ifndef __PMU_REGS_H__
#define __PMU_REGS_H__

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "common.h"

#define PMU_SOFTWARE_ACCESS_LOCK        0xC5ACCE55
#define PMCR_X				            0x10	/* export of events to ETM */


typedef struct __attribute__((__packed__)) pmu_interface {
    uint32_t PMEVCNTR0_EL0;
    PAD(0x4, 0x8);
    uint32_t PMEVCNTR1_EL0;
    PAD(0xc,0x10);
    uint32_t PMEVCNTR2_EL0;
    PAD(0x14,0x18);
    uint32_t PMEVCNTR3_EL0;
    PAD(0x1c,0x20);
    uint32_t PMEVCNTR4_EL0;
    PAD(0x24,0x28);
    uint32_t PMEVCNTR5_EL0;
    PAD(0x2c,0xf8);
    uint64_t PMCCNTR_EL0;
    PAD(0x100,0x400);
    uint32_t PMEVTYPER0_EL0;
    uint32_t PMEVTYPER1_EL0;
    uint32_t PMEVTYPER2_EL0;
    uint32_t PMEVTYPER3_EL0;
    uint32_t PMEVTYPER4_EL0;
    uint32_t PMEVTYPER5_EL0;
    PAD(0x418,0x47c);
    uint32_t PMCCFILTR_EL0;
    PAD(0x480,0xc00);
    uint32_t PMCNTENSET_EL0;
    PAD(0xc04,0xc20);
    uint32_t PMCNTENCLR_EL0;
    PAD(0xc24,0xc40);
    uint32_t PMINTENSET_EL1;
    PAD(0xc44,0xc60);
    uint32_t PMINTENCLR_EL1;
    PAD(0xc64,0xc80);
    uint32_t PMOVSCLR_EL0;
    PAD(0xc84,0xca0);
    uint32_t PMSWINC_EL0;
    PAD(0xca4,0xcc0);
    uint32_t PMOVSSET_EL0;
    PAD(0xcc4,0xe00);
    uint32_t PMCFGR;
    uint32_t PMCR_EL0;
    PAD(0xe08,0xe20);
    uint32_t PMCEID0_EL0;
    uint32_t PMCEID1_EL0;
    PAD(0xe28,0xfa8);
    int32_t PMDEVAFF0;
    int32_t PMDEVAFF1;
    int32_t PMLAR;
    int32_t PMLSR;
    int32_t PMAUTHSTATUS;
    int32_t PMDEVARCH;
    PAD(0xfc0,0xfcc);
    int32_t PMDEVTYPE;
    int32_t PMPIDR4;
    int32_t PMPIDR5;
    int32_t PMPIDR6;
    int32_t PMPIDR7;
    int32_t PMPIDR0;
    int32_t PMPIDR1;
    int32_t PMPIDR2;
    int32_t PMPIDR3;
    int32_t PMCIDR0;
    int32_t PMCIDR1;
    int32_t PMCIDR2;
    int32_t PMCIDR3;
} PMU_interface ;


static inline void pmu_unlock(volatile PMU_interface* pmu)
{
    assert(pmu->PMCIDR1 == CORESIGHT_CLASS_CORESIGHT);
    assert(pmu->PMDEVTYPE == CORESIGHT_DEVTYPE_PMU_CPU);

    /* Clear SW lock*/
    pmu->PMLAR = PMU_SOFTWARE_ACCESS_LOCK;

    /* Reading lock status to ensure unlock took place*/
    assert((pmu->PMLSR == 0x1) || (pmu->PMLSR == 0x1));
}


static inline void pmu_enable_export_events(volatile PMU_interface* pmu)
{
    unsigned int val = pmu->PMCR_EL0;
    
    val |= PMCR_X;
    pmu->PMCR_EL0 = val;
}

#endif //__PMU_REGS_H__
