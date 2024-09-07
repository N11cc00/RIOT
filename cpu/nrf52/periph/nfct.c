/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf52
 * @{
 *
 * @file
 * @brief       NFC Tag peripheral driver implementation
 *
 * @author      Nico Behrens <nico.behrens@mailbox.tu-dresden.de>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/nfct.h"
#include "periph_conf.h"

/**
 * @brief   Lock to prevent concurrency issues when used from different threads
 */
static mutex_t lock = MUTEX_INIT;

static nfct_state_t state = NFCT_STATE_DISABLE;

void ifs_nfct(void) {
    // TODO: Implement the interrupt handler for the NFCT peripheral
    if (NFCT->EVENTS_FIELDDETECTED) {
        NFCT->EVENTS_FIELDDETECTED = 0;    
    }

    if (NFCT->EVENTS_FIELDLOST) {
        NFCT->EVENTS_FIELDLOST = 0;
    }

    if (NFCT->EVENTS_SELECTED) {
        NFCT->EVENTS_SELECTED = 0;
    }

    if (NFCT->EVENTS_RXFRAMESTART) {
        NFCT->EVENTS_RXFRAMESTART = 0;
    }

}

int activate(void) {
    assert(state == NFCT_STATE_DISABLE);
    NRF_NFCT->TASK_ACTIVATE = 1;

    state = NFCT_IDLERU
    // enable interrupts
    NVIC_EnableIRQ(NFCT_IRQn);
    NRF_RADIO->INTENSET = NFCT_INTENSET_READY_Msk |
                          NFCT_INTENSET_RXERROR_Msk |
                          NFCT_INTENSET_RXFRAMEEND_Msk |
                          NFCT_INTENSET_TXFRAMEEND_Msk;
}

int disable(void) {
    assert(state != NFCT_DISABLE);

    // disable interrupts
    NVIC_DisableIRQ(NFCT_IRQn);

    NRF_NFCT->TASK_DISABLE = 1;
}




