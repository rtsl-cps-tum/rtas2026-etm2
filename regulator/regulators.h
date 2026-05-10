#ifndef _REGULATOR_H_
#define _REGULATOR_H

#include <stdint.h>
#include "etm_regs.h"

#define ETM_MODE_OFF		0
#define ETM_MODE_PR			1
#define ETM_MODE_TB_1_3		2
#define ETM_MODE_TB_2_2		3
#define ETM_MODE_TB_3_1		4


struct etm_config {
	unsigned int mode;				/* 0: OFF, 1: ETM PR, 2: ETM TB 1:3 ... */
	unsigned int budget;			/* number of cachelines */
	unsigned int cycles;			/* CPU cycles */
	unsigned int throttle_count;	/* throttling loop count */
};

/* IDs for single selectors and selector pairs */
#define SEL_0	0x00
#define SEL_1	0x01
#define SEL_2	0x02
#define SEL_3	0x03
#define SEL_4	0x04
#define SEL_5	0x05
#define SEL_6	0x06
#define SEL_7	0x07
#define SEL_8	0x08
#define SEL_9	0x09
#define SEL_10	0x0a
#define SEL_11	0x0b
#define SEL_12	0x0c
#define SEL_13	0x0d
#define SEL_14	0x0e
#define SEL_15	0x0f
#define SEL_PAIR_1	0x81
#define SEL_PAIR_2	0x82
#define SEL_PAIR_3	0x83
#define SEL_PAIR_4	0x84
#define SEL_PAIR_5	0x85
#define SEL_PAIR_6	0x86
#define SEL_PAIR_7	0x87

void trc_setup_periodic_replenish(volatile ETM_interface* etm, unsigned int budget, unsigned int cycles);
void trc_setup_token_bucket_3_1(volatile ETM_interface* etm, unsigned int budget, unsigned int cycles);
void trc_setup_token_bucket_2_2(volatile ETM_interface* etm, unsigned int budget, unsigned int cycles);
void trc_setup_token_bucket_1_3(volatile ETM_interface* etm, unsigned int budget, unsigned int cycles);
#endif
