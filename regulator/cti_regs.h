#ifndef __CTI_REGS_H__
#define __CTI_REGS_H__

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "common.h"


#define CTI_SOFTWARE_ACCESS_LOCK        0xC5ACCE55

typedef struct __attribute__((__packed__)) cti_interface {
    uint32_t CTICONTROL;
    PAD(0x4, 0x10);
    uint32_t CTIINTACK;
    uint32_t CTIAPPSET;
    uint32_t CTIAPPCLEAR;
    uint32_t CTIAPPPULSE;
    uint32_t CTIINEN[8];
    PAD(0x40, 0xa0);
    uint32_t CTIOUTEN[8];
    PAD(0xc0, 0x130);
    uint32_t CTITRIGINSTATUS;
    uint32_t CTITRIGOUTSTATUS;
    uint32_t CTICHINSTATUS;
    uint32_t CTICHOUTSTATUS;
    uint32_t CTIGATE;
    uint32_t ASICCTL ;
    PAD(0x148, 0xedc);
    uint32_t ITCHINACK;
    uint32_t ITTRIGINACK;
    uint32_t ITCHOUT;
    uint32_t ITTRIGOUT;
    uint32_t ITCHOUTACK;
    uint32_t ITTRIGOUTACK;
    uint32_t ITCHIN ;
    uint32_t ITTRIGIN;
    uint32_t res0;
    uint32_t ITCTRL;
    PAD(0xf04, 0xfa0);
    uint32_t claim_tag_set ;
    uint32_t claim_tag_clear ;
    PAD(0xfa8, 0xfb0);
    uint32_t lock_access_register ;
    uint32_t lock_status_register ;
    uint32_t authentication_status ;
    PAD(0xfbc, 0xfc8);
    uint32_t device_id;
    uint32_t device_type_identifier;
    uint32_t PeripheralID4;
    uint32_t PeripheralID5;
    uint32_t PeripheralID6;
    uint32_t PeripheralID7;
    uint32_t PeripheralID0;
    uint32_t PeripheralID1;
    uint32_t PeripheralID2;
    uint32_t PeripheralID3;
    uint32_t ComponentID0;
    uint32_t ComponentID1;
    uint32_t ComponentID2;
    uint32_t ComponentID3;
} CTI_interface ;


static inline void cti_init(volatile CTI_interface* cti)
{
    assert((cti->ComponentID1 & 0xff) == CORESIGHT_CLASS_CORESIGHT);
    assert((cti->device_type_identifier) == CORESIGHT_DEVTYPE_DEBUG_TRIGGER);
    
    /* Unlock device */
    cti->lock_access_register = CTI_SOFTWARE_ACCESS_LOCK;

    assert((cti->lock_status_register == 0x1) || (cti->lock_status_register == 0x0)); /* device unlocked or software lock not implemented */

    /* Disable CTI before programming */
    cti->CTICONTROL = 0x0u;

    /* Disable all input and output triggers from channels */
    cti->CTIINEN[0] = 0;
    cti->CTIINEN[1] = 0;
    cti->CTIINEN[2] = 0;
    cti->CTIINEN[3] = 0;
    cti->CTIINEN[4] = 0;
    cti->CTIINEN[5] = 0;
    cti->CTIINEN[6] = 0;
    cti->CTIINEN[7] = 0;

    cti->CTIOUTEN[0] = 0;
    cti->CTIOUTEN[1] = 0;
    cti->CTIOUTEN[2] = 0;
    cti->CTIOUTEN[3] = 0;
    cti->CTIOUTEN[4] = 0;
    cti->CTIOUTEN[5] = 0;
    cti->CTIOUTEN[6] = 0;
    cti->CTIOUTEN[7] = 0;

    /* Keep PE/CTIs "off grid" -- keep CTIGATE bits cleared */
    cti->CTIGATE = 0;

    /* Ack all prviously pending requests */
    cti->CTIINTACK = 0;

    /* enable CTI after programming */
    cti->CTICONTROL = 0x1u;
}

#endif //__CTI_REGS_H__
