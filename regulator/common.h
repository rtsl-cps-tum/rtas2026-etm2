#ifndef __COMMON_H__
#define __COMMON_H__

#define CHECK(r,p) ( (r & 0x1 << (p) ) >> (p) )
#define SET(r,p) ( r |= 0x1 << (p) )
#define CLEAR(r,p) ( r &= ~(0x1 << (p) ) )

#define PAD( start, end ) JOIN( char pad , __COUNTER__ [end - start] )
#define JOIN( symbol1, symbol2 ) _DO_JOIN( symbol1, symbol2 )
#define _DO_JOIN( symbol1, symbol2 ) symbol1##symbol2

/* coresight classes */
#define CORESIGHT_CLASS_VERIFICATION	0x00
#define CORESIGHT_CLASS_ROM_TABLE		0x10
#define CORESIGHT_CLASS_CORESIGHT		0x90
#define CORESIGHT_CLASS_TEST_BLOCK		0xb0
#define CORESIGHT_CLASS_GENERIC_IP		0xe0
#define CORESIGHT_CLASS_OTHER			0xf0

/* coresight device types*/
#define CORESIGHT_DEVTYPE_MAJOR_MISC			0x0	/* miscellaneous */
#define CORESIGHT_DEVTYPE_MAJOR_TRACE_SINK		0x1	/* trace sink */
#define CORESIGHT_DEVTYPE_MAJOR_TRACE_LINK		0x2	/* trace link */
#define CORESIGHT_DEVTYPE_MAJOR_TRACE_SOURCE	0x3	/* trace source */
#define CORESIGHT_DEVTYPE_MAJOR_DEBUG_LINK		0x4	/* debug control */
#define CORESIGHT_DEVTYPE_MAJOR_DEBUG_LOGIC		0x5	/* debug control */
#define CORESIGHT_DEVTYPE_MAJOR_PERF			0x6	/* debug control */

#define CORESIGHT_DEVTYPE_TRACE_PORT	0x11
#define CORESIGHT_DEVTYPE_TRACE_BUFFER	0x21
#define CORESIGHT_DEVTYPE_TRACE_ROUTER	0x31

#define CORESIGHT_DEVTYPE_TRACE_FUNNEL	0x12
#define CORESIGHT_DEVTYPE_TRACE_FILTER	0x22
#define CORESIGHT_DEVTYPE_TRACE_FIFO	0x32

#define CORESIGHT_DEVTYPE_TRACE_CPU		0x13
#define CORESIGHT_DEVTYPE_TRACE_DSP		0x23
#define CORESIGHT_DEVTYPE_TRACE_COPROC	0x33
#define CORESIGHT_DEVTYPE_TRACE_BUS		0x43
#define CORESIGHT_DEVTYPE_TRACE_SW		0x63

#define CORESIGHT_DEVTYPE_DEBUG_TRIGGER	0x14
#define CORESIGHT_DEVTYPE_DEBUG_AUTH	0x24
#define CORESIGHT_DEVTYPE_DEBUG_POWER	0x34

#define CORESIGHT_DEVTYPE_DEBUG_CPU		0x15
#define CORESIGHT_DEVTYPE_DEBUG_DSP		0x25
#define CORESIGHT_DEVTYPE_DEBUG_COPROC	0x35
#define CORESIGHT_DEVTYPE_DEBUG_BUS		0x45
#define CORESIGHT_DEVTYPE_DEBUG_MEM		0x55

#define CORESIGHT_DEVTYPE_PMU_CPU		0x16
#define CORESIGHT_DEVTYPE_PMU_DSP		0x26
#define CORESIGHT_DEVTYPE_PMU_COPROC	0x36
#define CORESIGHT_DEVTYPE_PMU_BUS		0x46
#define CORESIGHT_DEVTYPE_PMU_SMMU		0x56

#include "zcu_regs.h"

int linux_disable_cpuidle_cpu(unsigned int cpu);
volatile void* cs_register(enum component comp);

#endif //__COMMON_H__
