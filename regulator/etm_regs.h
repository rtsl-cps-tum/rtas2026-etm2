#ifndef __ETM_REGS_H__
#define __ETM_REGS_H__
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "common.h"

#define TRC_SOFTWARE_ACCESS_LOCK        0xC5ACCE55

#define TRCPRGCTLR_EN		            0x01
#define TRCSTATR_IDLE		            0x01


#define TRCCONFIGR_RES1	0x0001
#define TRCCONFIGR_BB	0x0008
#define TRCCONFIGR_CCI	0x0010
#define TRCCONFIGR_CID	0x0040
#define TRCCONFIGR_VMID	0x0080
#define TRCCONFIGR_TS	0x0800
#define TRCCONFIGR_RS	0x1000



#define TRCRSCTLR_PAIRINV		(1 << 21)	/* inverts the result of combined pair of resources, only for lower */
#define TRCRSCTLR_INV			(1 << 20)	/* invert resource */
#define TRCRSCTLR_GROUP_EXT		(0 << 16)	/* sel 0..3: external input selectors */
#define TRCRSCTLR_GROUP_COMP	(1 << 16)	/* sel 0..7: PE comparator inputs */
#define TRCRSCTLR_GROUP_CNT_SEQ	(2 << 16)	/* sel 0..3: counter at zero, 4..7: sequencer states */
#define TRCRSCTLR_GROUP_SSC		(3 << 16)	/* sel 0..7: single-shot comparator */
#define TRCRSCTLR_GROUP_SAC		(4 << 16)	/* sel 0..15: single address comparator */
#define TRCRSCTLR_GROUP_ARC		(5 << 16)	/* sel 0..7: address range comparator */
#define TRCRSCTLR_GROUP_CID		(6 << 16)	/* sel 0..7: context ID comparator */
#define TRCRSCTLR_GROUP_VMID	(7 << 16)	/* sel 0..7: VMID ID comparator */


#define TRCCNTCTLR_RLDSELF	0x10000	/* self-reload mode */
#define TRCCNTCTLR_CHAIN	0x20000	/* chain mode (counters 1 and 3) */
#define TRCIDR5_LPOVERRIDE	(1u << 23)	/* low-power override supported */
#define TRCEVENTCTL1R_LPOVERRIDE	0x1000 /* low-power override */

enum rs_group {External_input, PE_comparator, Counter_Seq, Single_shot, Single_addr, Contextid, Virtual_context};


typedef struct __attribute__((__packed__)) etm_interface {
    PAD(0x0, 0x4);
    uint32_t TRCPRGCTLR;
    uint32_t TRCPROCSELR;
    uint32_t TRCSTATR;
    uint32_t TRCCONFIGR;
    uint32_t res1;
    uint32_t TRCAUXCTLR;
    uint32_t res2;
    uint32_t TRCEVENTCTL0R;
    uint32_t TRCEVENTCTL1R;
    uint32_t res3;
    uint32_t TRCSTALLCTLR;
    uint32_t TRCTSCTLR;
    uint32_t TRCSYNCPR;
    uint32_t TRCCCCTLR;
    uint32_t TRCBBCTLR;
    uint32_t TRCTRACEIDR;
    uint32_t TRCQCTLR;
    PAD(0x48, 0x080);
    uint32_t TRCVICTLR;
    uint32_t TRCVIIECTLR;
    uint32_t TRCVISSCTLR;
    uint32_t TRCVIPCSSCTLR;
    PAD(0x90, 0xa0);
    uint32_t TRCVDCTLR;
    uint32_t TRCVDSACCTLR;
    uint32_t TRCVDARCCTLR;
    PAD(0xac, 0x100);
    uint32_t TRCSEQEVR[3];
    PAD(0x10c, 0x118);
    uint32_t TRCSEQRSTEVR;
    uint32_t TRCSEQSTR;
    uint32_t TRCEXTINSELR;
    PAD(0x124, 0x140);
    uint32_t TRCCNTRLDVR[4];
    uint32_t TRCCNTCTLR[4];
    uint32_t TRCCNTVR[4];
    PAD(0x170, 0x180);
    uint32_t TRCIDR8;
    uint32_t TRCIDR9;
    uint32_t TRCIDR10;
    uint32_t TRCIDR11;
    uint32_t TRCIDR12;
    uint32_t TRCIDR13;
    PAD(0x198, 0x1c0);
    uint32_t TRCIMSPEC[8] ;     
    uint32_t TRCIDR0;
    uint32_t TRCIDR1;
    uint32_t TRCIDR2;
    uint32_t TRCIDR3;
    uint32_t TRCIDR4;
    uint32_t TRCIDR5;
    uint32_t TRCIDR6;
    uint32_t TRCIDR7;
    uint32_t TRCRSCTLR[32];
    uint32_t TRCSSCCR[8];
    uint32_t TRCSSCSR[8];
    uint32_t TRCSSPCICR[8];
    PAD(0x2e0, 0x300);
    uint32_t TRCOSLAR;
    uint32_t TRCOSLSR;
    PAD(0x308, 0x310);
    uint32_t TRCPDCR;
    uint32_t TRCPDSR;
    PAD(0x318, 0x380);
    uint32_t res_blk7 [32];
    uint64_t TRCACVR[16];
    uint64_t TRCACATR[16];
    
    /*
    Data Trace Block.
    Ignore them since Cortex-A53 does not support Data Trace
    */

    uint64_t TRCDVCVR[16];
    uint64_t TRCDVCMR[16];
    uint64_t TRCCIDCVR[8];
    uint64_t TRCVMIDCVR[8];
    uint32_t TRCCIDCCTLR0;
    uint32_t TRCCIDCCTLR1;
    uint32_t TRCVMIDCCTLR0;
    uint32_t TRCVMIDCCTLR1;
    uint32_t res_blk13 [28];
    uint32_t res_blk14_28 [480];
    uint32_t res_imp_def_topology_detection [32];
    uint32_t TRCITCTRL ;
    uint32_t res_blk30 [31];
    uint32_t res_blk31_a [8];
    uint32_t TRCCLAIMSET ;
    uint32_t TRCCLAIMCLR ;
    uint32_t TRCDEVAFF0;
    uint32_t TRCDEVAFF1;
    uint32_t TRCLAR ;
    uint32_t TRCLSR ;
    uint32_t TRCAUTHSTATUS ;
    uint32_t TRCDEVARCH ;
    uint32_t TRCDEVID2;
    uint32_t TRCDEVID1;
    uint32_t TRCDEVID;
    uint32_t TRCDEVTYPE;
    uint32_t TRCPIDR4 ;
    uint32_t TRCPIDR5 ;
    uint32_t TRCPIDR6 ;
    uint32_t TRCPIDR7 ;
    uint32_t TRCPIDR0 ;
    uint32_t TRCPIDR1 ;
    uint32_t TRCPIDR2 ;
    uint32_t TRCPIDR3 ;
    uint32_t TRCCIDR0 ;
    uint32_t TRCCIDR1 ;
    uint32_t TRCCIDR2 ;
    uint32_t TRCCIDR3 ;

} ETM_interface ;



/* Unlock TRC for futher programming */
static inline void trc_unlock(volatile ETM_interface *etm)
{
    assert(etm->TRCCIDR1 == CORESIGHT_CLASS_CORESIGHT);
    assert(etm->TRCDEVTYPE == CORESIGHT_DEVTYPE_TRACE_CPU);

    /* Clear SW lock*/
    etm->TRCLAR = TRC_SOFTWARE_ACCESS_LOCK;

    /* Reading lock status to ensure unlock took place*/
    assert((etm->TRCLSR == 0x1) || (etm->TRCLSR == 0x0));

    /* power up the TRC */
    if ((etm->TRCPDSR & 0x1) == 0) {
        etm->TRCPDCR = 0x8; // bit 3 to turn on power

        while ((etm->TRCPDSR & 0x1) == 0);
    }

    /* Clear OS lock */
    if ((etm->TRCOSLSR & 0x2) != 0) {
        etm->TRCOSLAR = 0x00;
    }
}

/* Disable TRC and wait for it to enter idle state */
static inline void trc_disable(volatile ETM_interface* etm)
{
    etm->TRCPRGCTLR = 0;

    while ((etm->TRCSTATR & TRCSTATR_IDLE) == 0);
}

/* Enable TRC and wait for it to enter running state*/
static inline void trc_enable(volatile ETM_interface* etm)
{
    etm->TRCPRGCTLR = TRCPRGCTLR_EN;

    while ((etm->TRCSTATR & TRCSTATR_IDLE) != 0);
}

/* Reset TRC registers (must be in disabled/idle state) */
static inline void trc_reset(volatile ETM_interface* etm)
{
    /* trace config register -- clear all features */
    etm->TRCCONFIGR = TRCCONFIGR_RES1;

    /* auxiliary control register -- A53 specific, disable all FIFO stuff */
    etm->TRCAUXCTLR = 0;

    etm->TRCEVENTCTL0R = 0;
    etm->TRCEVENTCTL1R = 0;
    etm->TRCSTALLCTLR = 0;
    etm->TRCTSCTLR = 0;
    etm->TRCSYNCPR = 0;

    etm->TRCCCCTLR = 0;
    etm->TRCBBCTLR = 0;
    etm->TRCTRACEIDR = 0;
    etm->TRCQCTLR = 0;

    etm->TRCVICTLR = 0;
    etm->TRCVIIECTLR = 0;
    etm->TRCVISSCTLR = 0;

    etm->TRCVDCTLR = 0;
    etm->TRCVDARCCTLR = 0;
    etm->TRCVDSACCTLR = 0;

    etm->TRCSEQEVR[0] = 0;
    etm->TRCSEQEVR[1] = 0;
    etm->TRCSEQEVR[2] = 0;
    etm->TRCSEQRSTEVR = 0;
    etm->TRCSEQSTR = 0;
    etm->TRCEXTINSELR = 0;

    etm->TRCCNTCTLR[0] = 0;
    etm->TRCCNTCTLR[1] = 0;
    etm->TRCCNTCTLR[2] = 0;
    etm->TRCCNTCTLR[3] = 0;
}

#endif //__ETM_REGS_H__
