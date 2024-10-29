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
    .data_size = sizeof(data_buffer),
    .p_data = data_buffer
};

/*
static void print_state(void) {
    printf("");
} 
*/
/*
static uint32_t min(uint32_t a, uint32_t b) {
    if (a > b) {
        return b;
    } else {
        return a;
    }
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

/*
static void test_send_ndef(void) {
    uint8_t ndef_buffer[256];
    size_t ndef_length;

    create_ndef_text_record("Hello, NFC!", ndef_buffer, &ndef_length);

    data_desc_tx.data_size = ndef_length;
    data_desc_tx.p_data = ndef_buffer;

    nrfx_err_t error = nrfx_nfct_tx(&data_desc_tx, NRF_NFCT_FRAME_DELAY_MODE_FREERUN);
    assert(error == NRFX_SUCCESS);
} 
*/

static void print_buffer_as_hex(const uint8_t* buffer, const uint32_t size) {
    // prints each byte as hexadecimal values (0x33);
    printf("Buffer: ");
    for (uint32_t i = 0; i < size; i++) {
        printf("0x%02X ", buffer[i]);
    }
}


void event_printer(nrfx_nfct_evt_t const * event) {
    printf("Event caught: " );
    if (event->evt_id == NRFX_NFCT_EVT_FIELD_DETECTED) {
        printf("Field detected");
        // test_send_ndef();
    } else if (event->evt_id == NRFX_NFCT_EVT_FIELD_LOST) {
        printf("Field lost");
    } else if (event->evt_id == NRFX_NFCT_EVT_SELECTED) {
        printf("Field selected");
    } else if (event->evt_id == NRFX_NFCT_EVT_RX_FRAMESTART) {
        printf("RX Framestart");
    } else if (event->evt_id == NRFX_NFCT_EVT_RX_FRAMEEND) {
        printf("RX Framend\n");
        printf("RX Status: %lX\n", event->params.rx_frameend.rx_status);
        printf("RX Data Size: %lu\n", event->params.rx_frameend.rx_data.data_size);
        print_buffer_as_hex(event->params.rx_frameend.rx_data.p_data, event->params.rx_frameend.rx_data.data_size);
    } else if (event->evt_id == NRFX_NFCT_EVT_TX_FRAMESTART) {
        printf("TX Framestart");
    } else if (event->evt_id == NRFX_NFCT_EVT_TX_FRAMEEND) {
        printf("TX Frameend");
    } else if (event->evt_id == NRFX_NFCT_EVT_ERROR) {
        printf("Error Event\n");
        printf("Error Reason: %d", event->params.error.reason);
    } else {
        printf("UNKNOWN EVENT");
    }
    printf("\n");
    //printf("EVENT: %d, %X\n", event->evt_id, event->evt_id);
}


static void test_init(void) {
    puts("[TEST]: Initializing driver");
    nrfx_nfct_config_t config = {
        .rxtx_int_mask = 0 /* NRFX_NFCT_EVT_FIELD_DETECTED |
                         NRFX_NFCT_EVT_ERROR          |
                         NRFX_NFCT_EVT_SELECTED 
                          |
                         NRFX_NFCT_EVT_FIELD_LOST |
                         NRFX_NFCT_EVT_RX_FRAMEEND |
                         NRFX_NFCT_EVT_RX_FRAMESTART |
                         NRFX_NFCT_EVT_TX_FRAMEEND |
                         NRFX_NFCT_EVT_TX_FRAMESTART */
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

void test_receive(void) {
    nrfx_err_t error = nrfx_nfct_rx(&data_desc_rx);
    assert(error == NRFX_SUCCESS);
}

int main(void) {
    ztimer_sleep(ZTIMER_SEC, 3);
    puts("Starting tests");
    test_init();

    nrfx_nfct_autocolres_enable();

    test_enable();   
    
    nrf_nfct_shorts_enable(NRF_NFCT, NRF_NFCT_SHORT_FIELDDETECTED_ACTIVATE_MASK | NRF_NFCT_SHORT_TXFRAMEEND_ENABLERXDATA_MASK);

    // nrfx_nfct_state_force(NRFX_NFCT_STATE_ACTIVATED);

    // ztimer_sleep(ZTIMER_SEC, 2);

    // test_send_ndef();

    test_receive();
    
    // nrfx_nfct_state_force(NRFX_NFCT_STATE_ACTIVATED);
    
    // test_scan_for_field();

    while(1) {
        print_buffer_as_hex(data_desc_rx.p_data, data_desc_rx.data_size);
        printf("\n");
        printf("State: %X\n", nrfy_nfct_tag_state_get(NRF_NFCT));
        ztimer_sleep(ZTIMER_MSEC, 500);
    };
    return 0;
}