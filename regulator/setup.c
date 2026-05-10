#include "setup.h"
#include "regulators.h"
#include "zcu_regs.h"
#include "cti_regs.h"
#include "etm_regs.h"

/**
 * Set Up the CTI routing from ETM to the PE CTIIRQ
 */
void cti_setup(volatile CTI_interface* cti)
{
	/* CTI setup
	 *
	 * CTI inputs (signals from PE to CTI):
	 * - 0     DGBTRIGGER    pulsed on entry to debug state
	 * - 1     PMUIRQ        PMU generated interrupt
	 * - 4..7  EXTOUT[0..3]  ETM signals
	 *
	 * CTI outputs (signals from CTI to PE)
	 * - 0     EDBGRQ        enter debug state
	 * - 1     DBGRESTART    leave debug state
	 * - 2     CTIIRQ        CTI interrupt pending
	 * - 4..7  EXTIN[0..3]   ETM signals
	 *
	 * We can observe the sequencer state machine of the ETM on EXTOUT[0..3].
	 * We need to route EXTOUT3 to CTIIRQ to forward the interrupt.
	 * In the following code, we use channel 3 for the local routing.
	 * ETM TB uses CTI channels 0 and 1.
	 */

	/* map CTIINEN7 (EXTOUT3) and CTIOUTEN2 (CTIIRQ) to channel 3 */
	cti->CTIINEN[5] =	(1u << 3);	/* EXTOUT1 for ETM TB 1:3 */
	cti->CTIINEN[6] = 	(1u << 3);	/* EXTOUT2 for ETM TB 2:2 */
    cti->CTIINEN[7] =	(1u << 3);
    cti->CTIOUTEN[2] = 	(1u << 3);
}


////////////////////////////////////////////////////////////////////////////////


/**
 * Set Up the ETM Trace 
 */
void trc_setup_common(volatile ETM_interface* etm)
{
	unsigned int val;
	
    /* trace configuration register -- clear all features */
    etm->TRCCONFIGR = TRCCONFIGR_RES1;

	/* auxiliary control register -- A53 specific, disable all FIFO stuff */
    etm->TRCAUXCTLR = 0;

	/* disable any instruction stalls */
    etm->TRCSTALLCTLR = 0;

	/* disable periodic synchronization requests */
    etm->TRCSYNCPR = 0;

	/* instruction cycle count threshold (12 bit) */
    etm->TRCCCCTLR = 0;

	/* trace ID (7 bit) */
    etm->TRCTRACEIDR = 0;

	/* ViewInst main control register, controls instruction tracing */
    etm->TRCVICTLR = 0;

	/* ViewInst include-exclude control register */
    etm->TRCVIIECTLR = 0;

	/* ViewInst start-stop control register */
    etm->TRCVISSCTLR = 0;

	/* counter 0: disabled */
	etm->TRCCNTCTLR[0] = 0;
	/* counter 1: disabled */
	etm->TRCCNTCTLR[1] = 0;

    /* External input select register and selector 2
	 *
	 * We always use selector 2 to select the specific external inputs.
	 * The combination is the logical OR of the raw PMU input signals.
	 *
	 * Cortex-A53:
	 * - EXT0: L2D_CACHE_REFILL event 21
	 * - EXT1: L2D_CACHE_WB     event 22
	 * - selector 2: grp=0, bits 0|1
	 *
	 * NOTE: +4 is needed for PMU counter events to skip over the four CTI events
	 */
#if defined CORTEX_A53
	etm->TRCEXTINSELR = ((4+22) << 8) | ((4+21) << 0);
	etm->TRCRSCTLR[2] =  TRCRSCTLR_GROUP_EXT | 0x03;
#else
#error CORTEX_Axxx not defined
#endif
	/* address range comparators:
	 * - comp_pair 0 matches all             EL0   \__ use if regulator is in OS
	 * - comp_pair 1 matches all EL3+EL2+EL1       /
	 * - comp_pair 2 matches all         EL1+EL0   \__ use if regulator is in HV
	 * - comp_pair 3 matches all EL3+EL2           /
	 * The bits in the TRCACATR registers are inverted and set both NS and S.
	 *
	 * We let the address pairs always match, 0 <= addr <= 0xffffffffffffffff.
	 * The address range comparators provide an edge-triggered signal
	 * each time an instruction is fetched.
	 */
	etm->TRCACVR[0] = 0;
	etm->TRCACVR[1] = 0xffffffffffffffff;
	etm->TRCACVR[2] = 0;
	etm->TRCACVR[3] = 0xffffffffffffffff;
	etm->TRCACVR[4] = 0;
	etm->TRCACVR[5] = 0xffffffffffffffff;
	etm->TRCACVR[6] = 0;
	etm->TRCACVR[7] = 0xffffffffffffffff;

	etm->TRCACATR[0] = 0xee00;
	etm->TRCACATR[1] = 0xee00;
	etm->TRCACATR[2] = 0x1100;
	etm->TRCACATR[3] = 0x1100;
	etm->TRCACATR[4] = 0xcc00;
	etm->TRCACATR[5] = 0xcc00;
	etm->TRCACATR[6] = 0x3300;
	etm->TRCACATR[7] = 0x3300;

	/* In "off" mode, we configure the two counters in chained mode
	 * to just count the number of observed external PMU events.
	 * We can use this handy tool for validating the correctness.
	 */

	/* counter 0: counting L2 cache PMU events (selector 2), reload itself */
	etm->TRCCNTRLDVR[0] = 0xffff;
	etm->TRCCNTVR[0] = 0xffff;
	etm->TRCCNTCTLR[0] = TRCCNTCTLR_RLDSELF | (SEL_0 << 8) | (SEL_2 << 0);

	/* counter 1: counting when counter 0 reloads (chain mode),  */
	etm->TRCCNTRLDVR[1] = 0xffff;
	etm->TRCCNTVR[1] = 0xffff;
	etm->TRCCNTCTLR[1] = TRCCNTCTLR_RLDSELF | TRCCNTCTLR_CHAIN;

	/* program sequencer state transitions: disable */
	etm->TRCSEQEVR[0] = 0;
	etm->TRCSEQEVR[1] = 0;
	etm->TRCSEQEVR[2] = 0;
	etm->TRCSEQRSTEVR = 0;

	/* program sequencer: start in state 0 */
	etm->TRCSEQSTR = 0;

	/* ETM outputs: disabled */
	etm->TRCEVENTCTL0R = 0;
	/* disable all events in trace stream */

	val = etm->TRCIDR5;
	if (0 && (val & TRCIDR5_LPOVERRIDE) != 0) {
		/* FIXME: to disable low power mode on Cortex A cores
		 */
		val = TRCEVENTCTL1R_LPOVERRIDE | 0x0;
	} else {
		val = 0x0;
	}
	etm->TRCEVENTCTL1R = val;
}


////////////////////////////////////////////////////////////////////////////////
