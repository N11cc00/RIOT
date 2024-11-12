#include "nrfx_nfct.h"
#include "container.h"
#include "log.h"
#include "panic.h"
#include "cpu.h"

#include "t2t.h"

#define BUFFER_SIZE 200

static nfct_type_2_tag_t *tag;
static event_queue_t event_queue;

static uint8_t receive_data_size = 0;
static uint8_t transmit_data_size = 0;

static uint8_t data_buffer_rx[BUFFER_SIZE];
static uint8_t data_buffer_tx[BUFFER_SIZE];
static const uint32_t buffer_size = BUFFER_SIZE;

// static nrfx_nfct_data_desc_t data_desc_tx;
// static nrfx_nfct_data_desc_t data_desc_rx;

static t2t_receive_event_t receive_event;
static t2t_transmit_event_t transmit_event;
static event_t select_event;
static event_t end_of_transmission_event;

static void configure_autocolres(uint8_t *nfctid1, uint8_t nfctid1_size) {
    nrfx_nfct_nfcid1_t nfcid1 = {
        .p_id = nfctid1,
        .id_size = nfctid1_size,
    };

    nrfx_nfct_param_t param = {
        .id = NRFX_NFCT_PARAM_ID_NFCID1,
        .data.nfcid1 = nfcid1
    };
    nrfx_nfct_parameter_set(&param);
    nrfx_nfct_autocolres_enable();
}

// send the data at the given block address
// always sends 16 bytes
static void process_read_command(uint8_t block_address) {
    uint8_t start_address = block_address * T2T_BLOCK_SIZE;

    // Copy 16 bytes of data from the tag's memory to the response buffer
    // unless there are no more bytes left in the tag's memory
    for (uint32_t i = 0; i < 16; i++) {
        data_buffer_rx[i] = tag->memory[start_address + i];
    }

    // Set up the data descriptor for the response
    nrfx_nfct_data_desc_t data_desc_tx = {
        .p_data = data_buffer_tx,
        .data_size = 16
    };

    // Transmit the response
    nrfx_err_t error = nrfx_nfct_tx(&data_desc_tx, NRF_NFCT_FRAME_DELAY_MODE_FREERUN);
    assert(error == NRFX_SUCCESS);
} 

static void parse_received_data(const uint8_t *data, uint32_t size) {
    if (size == 0) {
        LOG_DEBUG("Received empty data\n");
        return;
    }

    if (data[0] == T2T_READ_COMMAND) {
        if (size != 2) {
            LOG_DEBUG("Invalid read command received\n");
            return;
        }
        uint8_t block_address = data[1];
        LOG_DEBUG("Read command received for block %d\n", block_address);
        process_read_command(block_address);
    } else if (data[0] == T2T_HALT_COMMAND) {
        LOG_DEBUG("Halt command received\n");
    } else {
        LOG_DEBUG("Unknown command received (0x%02X)\n", data[0]);
    }
}

static void receive_handler(event_t * event) {
    t2t_receive_event_t *e = container_of(event, t2t_receive_event_t, super);
    parse_received_data(data_buffer_rx, e->size);
}

static void transmit_handler(event_t * event) {
    t2t_transmit_event_t *e = container_of(event, t2t_transmit_event_t, super);
    nrfx_nfct_data_desc_t data_desc_tx = {
        .p_data = data_buffer_tx,
        .data_size = e->size
    };

    nrfx_err_t error = nrfx_nfct_tx(&data_desc_tx, NRF_NFCT_FRAME_DELAY_MODE_WINDOW);
}

static void end_of_tx_handler(event_t * event) {
    nrfx_nfct_data_desc_t data_desc_rx = {
        .p_data = data_buffer_rx,
        .data_size = buffer_size
    };
    nrfx_nfct_rx(&data_desc_rx);
}

static void selected_handler(event_t * event) {
    nrfx_nfct_data_desc_t data_desc_rx = {
        .p_data = data_buffer_rx,
        .data_size = buffer_size
    };
    nrfx_nfct_rx(&data_desc_rx);
}

// this is the irq callback function that is called by the nfct driver
void irq_event_handler(nrfx_nfct_evt_t const * event) {
    if (event->evt_id == NRFX_NFCT_EVT_RX_FRAMEEND) {
        LOG_DEBUG("Received frame of size %lu\n", event->params.rx_frameend.rx_data.data_size);
        receive_event.super.handler = receive_handler;
        receive_event.size = event->params.rx_frameend.rx_data.data_size;
        event_post(&event_queue, (event_t *) &receive_event);   
    } else if (event->evt_id == NRFX_NFCT_EVT_TX_FRAMEEND) {
        LOG_DEBUG("End of transmission\n");
        end_of_transmission_event.handler = end_of_tx_handler;
        event_post(&event_queue, &end_of_transmission_event);
    } else if (event->evt_id == NRFX_NFCT_EVT_SELECTED) {
        LOG_DEBUG("Field has been selected\n");
        select_event.handler = selected_handler;
        event_post(&event_queue, &select_event);
    } else if (event->evt_id == NRFX_NFCT_EVT_TX_FRAMESTART) {
        LOG_DEBUG("Transmitting frame of size %lu\n", event->params.tx_framestart.tx_data.data_size);
    } else if (event->evt_id == NRFX_NFCT_EVT_RX_FRAMESTART) {
        LOG_DEBUG("Receiving frame\n");
    } else if (event->evt_id == NRFX_NFCT_EVT_ERROR) {
        LOG_DEBUG("Error occurred\n");
    }
}




void initialize_t2t(nfct_type_2_tag_t *_tag, bool autocolres, uint8_t *nfcid1, uint8_t nfcid1_size) {
    tag = _tag;
    event_queue_init(&event_queue);
    nrfx_nfct_config_t config = {
        .rxtx_int_mask = /*NRFX_NFCT_EVT_FIELD_DETECTED |
                         NRFX_NFCT_EVT_ERROR          |
                         NRFX_NFCT_EVT_SELECTED       |
                         NRFX_NFCT_EVT_FIELD_LOST     |  */
                         NRFX_NFCT_EVT_RX_FRAMEEND    |
                         NRFX_NFCT_EVT_RX_FRAMESTART  |
                         NRFX_NFCT_EVT_TX_FRAMEEND    |
                         NRFX_NFCT_EVT_TX_FRAMESTART
                        ,
        .cb = irq_event_handler,
        .irq_priority = 2
    };

    nrfx_err_t error = nrfx_nfct_init(&config);
    assert(error == NRFX_SUCCESS);

    LOG_DEBUG("Initialization of nfct driver successful!\n");

    if (autocolres) {
        configure_autocolres(nfcid1, nfcid1_size);
    }

    nrf_nfct_shorts_enable(NRF_NFCT, NRF_NFCT_SHORT_FIELDDETECTED_ACTIVATE_MASK | NRF_NFCT_SHORT_FIELDLOST_SENSE_MASK);

    enable_t2t();

    LOG_DEBUG("Starting event loop.\n");
    event_loop(&event_queue);
}

void enable_t2t(void) {
    nrfx_nfct_enable();
}