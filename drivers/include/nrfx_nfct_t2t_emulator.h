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
#include "t2t.h"

#include "nrfx_nfct_t2t_emulator.h"

// TODO: check for nrfx package in use here

typedef struct {
    event_t super;
    uint32_t size; 
} nrfx_t2t_receive_event_t;

typedef struct {
    event_t super;
    uint32_t size;
} nrfx_t2t_transmit_event_t;


void initialize_t2t(nfc_t2t_t* tag);

void uninitialize_t2t();

void enable_t2t(void);

void disable_t2t(void);



void start_event_loop(void);



#endif