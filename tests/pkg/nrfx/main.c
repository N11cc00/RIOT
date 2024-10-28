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
#include "panic.h"
#include "cpu.h"


uint8_t data_buffer[256];

nrfx_nfct_data_desc_t data_desc_tx;
nrfx_nfct_data_desc_t data_desc_rx = {
    .data_size = 0,
    .p_data = data_buffer
};

/*
static void print_state(void) {
    printf("");
} 
*/
void create_ndef_text_record(const char *text, uint8_t *buffer, size_t *length) {
    const char *language_code = "en";
    size_t text_length = strlen(text);
    size_t language_code_length = strlen(language_code);

    // NDEF record header
    buffer[0] = 0xD1; // MB=1, ME=1, CF=0, SR=1, IL=0, TNF=0x01 (Well-known type)
    buffer[1] = 0x01; // Type Length
    buffer[2] = text_length + language_code_length + 1; // Payload Length
    buffer[3] = 'T'; // Type

    // NDEF record payload
    buffer[4] = language_code_length; // Status byte: UTF-8 encoding, length of language code
    memcpy(&buffer[5], language_code, language_code_length); // Language code
    memcpy(&buffer[5 + language_code_length], text, text_length); // Text

    *length = 5 + language_code_length + text_length;
}


void event_printer(nrfx_nfct_evt_t const * event) {
    printf("EVENT: %d\n", event->evt_id);
}


static void test_init(void) {
    puts("[TEST]: Initializing driver");
    nrfx_nfct_config_t config = {
        .rxtx_int_mask = NRFX_NFCT_EVT_FIELD_DETECTED |
                         NRFX_NFCT_EVT_ERROR          |
                         NRFX_NFCT_EVT_SELECTED /* |
                         
                         NRFX_NFCT_EVT_FIELD_LOST |
                         NRFX_NFCT_EVT_RX_FRAMEEND |
                         NRFX_NFCT_EVT_RX_FRAMESTART |
                         NRFX_NFCT_EVT_TX_FRAMEEND |
                         NRFX_NFCT_EVT_TX_FRAMESTART
                         */
                        ,
        .cb = event_printer,
        .irq_priority = 1
    };
    nrfx_err_t error = nrfx_nfct_init(&config);
    printf("status code is: %d\n", error);
    // print_state();
    assert(error == NRFX_SUCCESS);
    puts("Initialization successful!");
}


static void test_enable(void) {
    puts("[TEST]: Enabling nfc");
    nrfx_nfct_enable();
    // print_state();
}



/*
static void test_transmit_frame(void) {
    puts("[TEST]: Transmitting frame");
    nrfx_err_t error = nrfx_nfct_tx(&data_desc, NRF_NFCT_FRAME_DELAY_MODE_WINDOW);
    printf("Transmission Error: %X\n", error);
    assert(error == NRFX_SUCCESS);
}
*/


/* TODO:
static void test_receive_frame(void) {
 
}
*/

/*
static void test_scan_for_field(void) {
    puts("[TEST]: Infinite scanning");
    while(1) {
        bool sensed = nrfx_nfct_field_check();
        // print_state();
        // printf("%d\n", flag);
        if (sensed) {
            puts("SENSED SOMETHING");
            break;
        }
    }
}
*/

void test_send_ndef(void) {
    uint8_t ndef_buffer[256];
    size_t ndef_length;

    create_ndef_text_record("Hello, NFC!", ndef_buffer, &ndef_length);

    data_desc_tx.data_size = ndef_length;
    data_desc_tx.p_data = ndef_buffer;

    nrfx_err_t error = nrfx_nfct_tx(&data_desc_tx, NRF_NFCT_FRAME_DELAY_MODE_WINDOW);
    assert(error == NRFX_SUCCESS);
} 

void test_receive(void) {
    nrfx_err_t error = nrfx_nfct_rx(&data_desc_rx);
    assert(error == NRFX_SUCCESS);
}

int main(void) {
    ztimer_sleep(ZTIMER_SEC, 3);
    puts("Starting tests");
    test_init();

    nrfx_nfct_autocolres_disable();

    test_enable();

    nrfx_nfct_state_force(NRFX_NFCT_STATE_ACTIVATED);

    ztimer_sleep(ZTIMER_SEC, 2);

    // test_send_ndef();

    test_receive();
    
    // nrfx_nfct_state_force(NRFX_NFCT_STATE_ACTIVATED);
    
    // test_scan_for_field();
    while(1) {
        printf("DATA: %lu, %s\n", data_desc_rx.data_size, data_desc_rx.p_data);
    };
    return 0;
}