#ifndef NRFX_NFCT_T2T_EMULATOR_H
#define NRFX_NFCT_T2T_EMULATOR_H

#include "kernel_defines.h"
#include "event.h"
#include "thread.h"

#if !IS_USED(MODULE_NRFX_NFCT)
#error Please use the nrfx_nfct module to enable \
    the functionality on this device
#endif

#include "nrfx_nfct.h"
#include "net/nfc/t2t/t2t.h"

typedef struct {
    event_t super;
    uint32_t size; 
} nrfx_t2t_receive_event_t;

typedef struct {
    event_t super;
    uint32_t size;
} nrfx_t2t_transmit_event_t;


void initialize_t2t(nfc_t2t_t* tag);

void uninitialize_t2t(void);

void enable_t2t(void);

void disable_t2t(void);

#endif