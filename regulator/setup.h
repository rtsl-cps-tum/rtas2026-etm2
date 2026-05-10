#ifndef __SETUP_H__
#define __SETUP_H__

#include <stdint.h>
#include "etm_regs.h"
#include "cti_regs.h"

#define CORTEX_A53 /* For ZynqMP SoCs */

void cti_setup(volatile CTI_interface* cti);
void trc_setup_common(volatile ETM_interface* etm);


#endif //__SETUP_H__
