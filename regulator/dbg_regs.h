#ifndef __DBG_REGS_H__
#define __DBG_REGS_H__

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "common.h"

#define DBG_SOFTWARE_ACCESS_LOCK        0xC5ACCE55

typedef struct __attribute__((__packed__)) dbg_interface {
    PAD(0x0, 0x20);
    uint32_t EDESR;
    uint32_t EDECR;
    PAD(0x28, 0x30);
    uint32_t EDWAR_31_0;
    uint32_t EDWAR_63_32;
    PAD(0x38, 0x80);
    uint32_t DBGDTRRX_EL0;
    uint32_t EDITR;
    uint32_t EDSCR;
    uint32_t DBGDTRTX_EL0;
    uint32_t EDRCR;
    uint32_t EDACR;
    uint32_t EDECCR;
    uint32_t res0;
    uint32_t EDPCSRlo;
    uint32_t EDCIDSR;
    uint32_t EDVIDSR;
    uint32_t EDPCSRhi;
    PAD(0xB0, 0x300);
    uint32_t OSLAR_EL1;
    PAD(0x304, 0x310);
    uint32_t EDPRCR;
    uint32_t EDPRSR;
    PAD(0x318, 0x400);
    uint32_t DBGBVR0_EL1_31_0;
    uint32_t DBGBVR0_EL1_63_32;
    uint32_t DBGBCR0_EL1;
    uint32_t res1;
    uint32_t DBGBVR1_EL1_31_0;
    uint32_t DBGBVR1_EL1_63_32;
    uint32_t DBGBCR1_EL1;
    uint32_t res2;
    uint32_t DBGBVR2_EL1_31_0;
    uint32_t DBGBVR2_EL1_63_32;
    uint32_t DBGBCR2_EL1;
    uint32_t res3;
    uint32_t DBGBVR3_EL1_31_0;
    uint32_t DBGBVR3_EL1_63_32;
    uint32_t DBGBCR3_EL1;
    uint32_t res4;
    uint32_t DBGBVR4_EL1_31_0;
    uint32_t DBGBVR4_EL1_63_32;
    uint32_t DBGBCR4_EL1;
    uint32_t res5;
    uint32_t DBGBVR5_EL1_31_0;
    uint32_t DBGBVR5_EL1_63_32;
    uint32_t DBGBCR5_EL1;
    PAD(0x45C, 0x800);
    uint32_t DBGWVR0_EL1_31_0;
    uint32_t DBGWVR0_EL1_63_32;
    uint32_t DBGWCR0_EL1;
    uint32_t res6;
    uint32_t DBGWVR1_EL1_31_0;
    uint32_t DBGWVR1_EL1_63_32;
    uint32_t DBGWCR1_EL1;
    uint32_t res7;
    uint32_t DBGWVR2_EL1_31_0;
    uint32_t DBGWVR2_EL1_63_32;
    uint32_t DBGWCR2_EL1;
    uint32_t res8;
    uint32_t DBGWVR3_EL1_31_0;
    uint32_t DBGWVR3_EL1_63_32;
    uint32_t DBGWCR3_EL1;
    PAD(0x83c, 0xD00);
    uint32_t MIDR_EL1;
    PAD(0xD04, 0xD20);
    uint32_t EDPFR0_EL1_31_0;
    uint32_t EDPFR0_EL1_63_32;
    uint32_t EDDFR0_EL1_31_0;
    uint32_t EDDFR0_EL1_63_32;
    uint32_t ID_AA64ISAR0_EL1_31_0;
    uint32_t ID_AA64ISAR0_EL1_63_32;
    uint32_t ID_AA64MMFR0_EL1_31_0;
    uint32_t ID_AA64MMFR0_EL1_63_32;
    PAD(0xD40, 0xF00);
    uint32_t EDITCTRL;
    PAD(0xF04, 0xFA0);
    uint32_t DBGCLAIMSET_EL1;
    uint32_t DBGCLAIMCLR_EL1;
    uint32_t EDDEVAFF0;
    uint32_t EDDEVAFF1;
    uint32_t EDLAR;
    uint32_t EDLSR;
    uint32_t DBGAUTHSTATUS_EL1;
    uint32_t EDDEVARCH;
    uint32_t EDDEVID2;
    uint32_t EDDEVID1;
    uint32_t EDDEVID;
    uint32_t EDDEVTYPE;
    uint32_t EDPIDR4;
    uint32_t EDPIDR5;
    uint32_t EDPIDR6;
    uint32_t EDPIDR7;
    uint32_t EDPIDR0;
    uint32_t EDPIDR1;
    uint32_t EDPIDR2;
    uint32_t EDPIDR3;
    uint32_t EDCIDR0;
    uint32_t EDCIDR1;
    uint32_t EDCIDR2;
    uint32_t EDCIDR3;
} DBG_interface ;


static inline void dbg_init_minimal(volatile DBG_interface* dbg)
{
    assert((dbg->EDCIDR1 & 0xff) == CORESIGHT_CLASS_CORESIGHT); 
    assert((dbg->EDDEVTYPE & 0xff) == CORESIGHT_DEVTYPE_DEBUG_CPU);

    /* Unlock device if initally locked */
    dbg->EDLAR = DBG_SOFTWARE_ACCESS_LOCK;
    __asm__ volatile ("dsb sy" : : : "memory");
    /* reading the lock status ensures that the unlock operation takes place */
    assert((dbg->EDLSR == 0x0) || (dbg->EDLSR == 0x1));

    /* Clear any OS locks (this also unlocks the PMU) */
    dbg->OSLAR_EL1 = 0;
    __asm__ volatile ("dsb sy" : : : "memory");
}

#endif //__DBG_REGS_H__
