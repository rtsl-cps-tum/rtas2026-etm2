#include <stdio.h>
#include <stdint.h>

#include "common.h"
#include "setup.h"
#include "regulators.h"
#include "etm_regs.h"


void trc_setup_pr(volatile ETM_interface* etm, unsigned int budget, unsigned int cycles)
{
	/* trace unit resource selection:
	 * - selector  0: always FALSE (hardcoded)
	 * - selector  1: always TRUE (hardcoded)
	 * - selector  2: external inputs from PMU (architecture specific)
	 * - selector  3: grp=2, bit 0: counter0 for time <- selector 1 (always TRUE)
	 * - selector  4: grp=2, bit 1: counter1 <- EXT inputs
	 * - selector  5: grp=2, bit 7: sequencer state 3
	 *
	 * We don't use any selector pairs here.
	 */
	etm->TRCRSCTLR[3] =  TRCRSCTLR_GROUP_CNT_SEQ | 0x01;
	etm->TRCRSCTLR[4] =  TRCRSCTLR_GROUP_CNT_SEQ | 0x02;
	etm->TRCRSCTLR[5] =  TRCRSCTLR_GROUP_CNT_SEQ | 0x80;

    etm->TRCRSCTLR[6] = TRCRSCTLR_GROUP_CNT_SEQ | 0x10;
    etm->TRCRSCTLR[7] = TRCRSCTLR_GROUP_CNT_SEQ | 0x01;
	/* counter 0: counting time (the always TRUE selector), resets periodically */
	etm->TRCCNTRLDVR[0] = cycles;
	etm->TRCCNTVR[0] = cycles;
	etm->TRCCNTCTLR[0] = TRCCNTCTLR_RLDSELF | (SEL_0 << 8) | (SEL_1 << 0);

	/* counter 1: counting L2 cache PMU events (selector 2), resets on counter0 (sel 3) */
	etm->TRCCNTRLDVR[1] = budget;
	etm->TRCCNTVR[1] = budget;
	etm->TRCCNTCTLR[1] = TRCCNTCTLR_RLDSELF | (SEL_3 << 8) | (SEL_2 << 0);

	/* program sequencer state transitions for a Periodic Replinishment regulation:
	 *
	 * - state 0 "within budget":
	 *                                v sel_4 (PMU)    ^ sel_3 (timer)
	 * - state 3 "budget exceeded":
	 *
	 * The state transitions resemble MemGuard with periodic replenish
	 * due to the reset to state 0 by the periotimer sel_3.
	 *
	 * NOTE: states 1 and 2 are not used and jumped over
	 *
	 * The core's bandwidth is counter_1_reload
	 * before the CTI interrupt is triggered.
	 *
	 * NOTE: bits 15..8 backward direction, bits 7..0 forward direction
	 */
	etm->TRCSEQEVR[0] = (SEL_3 << 8) | (SEL_4 << 0);
	etm->TRCSEQEVR[1] = (SEL_3 << 8) | (SEL_4 << 0);
	etm->TRCSEQEVR[2] = (SEL_3 << 8) | (SEL_4 << 0);
	etm->TRCSEQRSTEVR = 0;

	/* program sequencer: start in state 0 */
	etm->TRCSEQSTR = 0;

	/* select sequencer state 3 (sel 5) as CTIIRQ trigger output on EXT3 */
	etm->TRCEVENTCTL0R = (SEL_5 << 24);
}


void trc_setup_periodic_replenish(volatile ETM_interface* etm, unsigned int budget, unsigned int cycles)
{
	/* We do not reset the budget counter when the timer expires.
	 * This pushes any overuse of budgets it into the next period.
	 *
	 * We additionally setup:
	 * - selector  6: grp=2, bit 4: sequencer state 0        \__ selector pair 3
	 * - selector  7: grp=2, bit 0: counter0 for time        /
	 *
	 * Now, we use selector pair 3 as reset condition, i.e.:
	 * we only reset the budget when we are below our budget.
	 */
    trc_setup_pr(etm, budget, cycles);
    etm->TRCCNTCTLR[1] = TRCCNTCTLR_RLDSELF | (SEL_PAIR_3 << 8) | (SEL_2 << 0);
}

/** setup a MemPol-like regulation regime (3 token buckets, 1 exceed bucket) */
void trc_setup_token_bucket_3_1(volatile ETM_interface* etm, unsigned int budget, unsigned int cycles)
{
	/* trace unit resource selection:
	 * - selector  0: always FALSE (hardcoded)
	 * - selector  1: always TRUE (hardcoded)
	 * - selector  2: external inputs from PMU (architecture specific)
	 * - selector  3: grp=2, bit 0:   counter0 for time <- selector 1 (always TRUE)
	 * - selector  4: grp=2, bit 4:   sequencer state 0      \__ selector pair 2
	 * - selector  5: grp=2, bit 1:   counter1 <- EXT inputs /
	 * - selector  6: grp=2, bit 5:   sequencer state 1      \__ selector pair 3
	 * - selector  7: grp=2, bit 1:   counter1 <- EXT inputs /
	 * - selector  8: grp=2, bit 6:   sequencer state 2      \__ selector pair 4
	 * - selector  9: grp=2, bit 1:   counter1 <- EXT inputs /
	 * - selector 10: grp=2, bit 7:   sequencer state 3      \__ selector pair 5
	 * - selector 11: grp=2, bit 0:   counter0 for time      /
	 * - selector 12: grp=2, bit 6:   sequencer state 2      \__ selector pair 6
	 * - selector 13: grp=2, bit 0:   counter0 for time      /
	 * - selector 14: grp=2, bit 5:   sequencer state 1      \__ selector pair 7
	 * - selector 15: grp=2, bit 0:   counter0 for time      /
	 *
	 * The selector pairs provide the logical AND of their two inputs.
	 * Pairs 2, 3 and 4 push the ETM state machine forwards, while
	 * pairs 4, 5 and 7 push backwards.
	 */
	etm->TRCRSCTLR[3] =  TRCRSCTLR_GROUP_CNT_SEQ | 0x01;
	etm->TRCRSCTLR[4] =  TRCRSCTLR_GROUP_CNT_SEQ | 0x10;
	etm->TRCRSCTLR[5] =  TRCRSCTLR_GROUP_CNT_SEQ | 0x02;
	etm->TRCRSCTLR[6] =  TRCRSCTLR_GROUP_CNT_SEQ | 0x20;
	etm->TRCRSCTLR[7] =  TRCRSCTLR_GROUP_CNT_SEQ | 0x02;
	etm->TRCRSCTLR[8] =  TRCRSCTLR_GROUP_CNT_SEQ | 0x40;
	etm->TRCRSCTLR[9] =  TRCRSCTLR_GROUP_CNT_SEQ | 0x02;
	etm->TRCRSCTLR[10] = TRCRSCTLR_GROUP_CNT_SEQ | 0x80;
	etm->TRCRSCTLR[11] = TRCRSCTLR_GROUP_CNT_SEQ | 0x01;
	etm->TRCRSCTLR[12] = TRCRSCTLR_GROUP_CNT_SEQ | 0x40;
	etm->TRCRSCTLR[13] = TRCRSCTLR_GROUP_CNT_SEQ | 0x01;
	etm->TRCRSCTLR[14] = TRCRSCTLR_GROUP_CNT_SEQ | 0x20;
	etm->TRCRSCTLR[15] = TRCRSCTLR_GROUP_CNT_SEQ | 0x01;

	/* counter 0: counting time (the always TRUE selector), resets periodically */
	etm->TRCCNTRLDVR[0] = cycles;
	etm->TRCCNTVR[0] = cycles;
	etm->TRCCNTCTLR[0] = TRCCNTCTLR_RLDSELF | (SEL_0 << 8) | (SEL_1 << 0);

	/* counter 1: counting L2 cache PMU events (selector 2), reloads automatically */
	etm->TRCCNTRLDVR[1] = budget;
	etm->TRCCNTVR[1] = budget;
	etm->TRCCNTCTLR[1] = TRCCNTCTLR_RLDSELF | (SEL_0 << 8) | (SEL_2 << 0);

	/* program sequencer state transitions:
	 *
	 * - state 0:
	 *             v sel_pair_2   ^ sel_pair_7
	 * - state 1:
	 *             v sel_pair_3   ^ sel_pair_6
	 * - state 2:
	 *             v sel_pair_4   ^ sel_pair_5
	 * - state 3:
	 *
	 * The state transitions resemble MemPol with periodic replenishment.
	 * The core's peak bandwidth is 3x counter_1_reload
	 * before the CTI interrupt is triggered.
	 *
	 * NOTE: bits 15..8 backward direction, bits 7..0 forward direction
	 */
	etm->TRCSEQEVR[0] = (SEL_PAIR_7 << 8) | (SEL_PAIR_2 << 0);
	etm->TRCSEQEVR[1] = (SEL_PAIR_6 << 8) | (SEL_PAIR_3 << 0);
	etm->TRCSEQEVR[2] = (SEL_PAIR_5 << 8) | (SEL_PAIR_4 << 0);
	etm->TRCSEQRSTEVR = SEL_0;

	/* program sequencer: start in state 0 */
	etm->TRCSEQSTR = 0;

	/* select sequencer state 3 (sel 10) as CTIIRQ trigger output on EXT3 */
	etm->TRCEVENTCTL0R = (SEL_10 << 24);
}

/** setup a MemPol-like regulation regime (2 token buckets, 2 exceed buckets) */
void trc_setup_token_bucket_2_2(volatile ETM_interface* etm, unsigned int budget, unsigned int cycles)
{
	trc_setup_token_bucket_3_1(etm, budget, cycles);
	/* select sequencer states 2 and 3 (sel 8 + 10) as CTIIRQ trigger output on EXT3 */
	etm->TRCEVENTCTL0R = (SEL_10 << 24) | (SEL_8 << 16);
}

/** setup a MemPol-like regulation regime (1 token bucket, 3 exceed buckets) */
void trc_setup_token_bucket_1_3(volatile ETM_interface* etm, unsigned int budget, unsigned int cycles)
{
	trc_setup_token_bucket_3_1(etm, budget, cycles);
	/* select sequencer states 1, 2 and 3 (sel 6 + sel 8 + 10) as CTIIRQ trigger output on EXT3 */
	etm->TRCEVENTCTL0R = (SEL_10 << 24) | (SEL_8 << 16) | (SEL_6 << 8);
}
