/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       nrfx package test application
 *
 * @author      Nico Behrens <nifrabe@outlook.de>
 *
 * @}
 */

#include "nrfx_nfct.h"
#include <assert.h>
#include <stdio.h>
#include "ztimer.h"


const uint8_t data[] = {'H','E','L','L','O',' ','W','O','R','L','D','\0'};
const uint32_t size = 12;
nrfx_nfct_data_desc_t data_desc = {
    .data_size = size,
    .p_data = data

};

static void print_state(void) {
    printf("Current state is: %d\n", nrfy_nfct_tag_state_get(NRF_NFCT));
} 

static void event_printer(const nrfx_nfct_evt_t *event) {
    printf("event id: %d\n", event->evt_id);
}

static void test_init(void) {
    puts("[TEST]: Initializing driver");
    nrfx_nfct_config_t config = {
        .rxtx_int_mask = NRFX_NFCT_EVT_FIELD_DETECTED | 
        NRFX_NFCT_EVT_FIELD_LOST | 
        NRFX_NFCT_EVT_ERROR | 
        NRFX_NFCT_EVT_SELECTED |
        NRFX_NFCT_EVT_RX_FRAMESTART |
        NRFX_NFCT_EVT_RX_FRAMEEND |
        NRFX_NFCT_EVT_TX_FRAMEEND |
        NRFX_NFCT_EVT_TX_FRAMESTART,
        .cb = event_printer,
        .irq_priority = 1
    };
    nrfx_err_t error = nrfx_nfct_init(&config);
    printf("status code is: %d\n", error);
    print_state();
    assert(error == NRFX_SUCCESS);
    puts("Initialization successful!");

    /*
    printf("error code is: %d\n", error);
    error = nrfx_nfct_init(&config);
    assert(error == NRFX_ERROR_);
    puts("Initialized twice");
    */
}

static void test_enable(void) {
    puts("[TEST]: Enabling nfc");
    nrfx_nfct_enable();
    print_state();
}
/*
static void test_transmit_frame(void) {
    puts("[TEST]: Transmitting frame");
    nrfx_err_t error = nrfx_nfct_tx(&data_desc, NRF_NFCT_FRAME_DELAY_MODE_WINDOW);
    assert(error == NRFX_SUCCESS);
}
*/

/* TODO:
static void test_receive_frame(void) {
 
}
*/


static void test_scan_for_field(void) {
    puts("[TEST]: Infinite scanning");
    while(1) {
        bool sensed = nrfx_nfct_field_check();
        print_state();
        if (sensed) {
            puts("SENSED SOMETHING");
            break;
        }
    }
}



int main(void) {
    ztimer_sleep(ZTIMER_SEC, 3);
    puts("Starting tests");
    test_init();
    test_enable();

    test_scan_for_field();
    // test_transmit_frame();
    while(1) {};
    return 0;
}