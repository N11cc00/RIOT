#ifndef _T2T_H_
#define _T2T_H_

#include <event.h>

#define T2T_BLOCK_SIZE 4

// Command set
typedef enum {
    T2T_READ_COMMAND = 0x30,
    T2T_HALT_COMMAND = 0x50,
    T2T_NACK_COMMAND = 0x00
} t2t_command_t;

typedef struct {
    uint8_t *memory;
    uint32_t size;
} nfct_type_2_tag_t;

typedef struct {
    event_t super;
    uint32_t size; 
} t2t_receive_event_t;

typedef struct {
    event_t super;
    uint32_t size;
} t2t_transmit_event_t;


void initialize_t2t(nfct_type_2_tag_t *_tag, bool autocolres, uint8_t *nfcid1, uint8_t nfcid1_size);

void enable_t2t(void);

void start_event_loop(void);

// void irq_event_handler(nrfx_nfct_evt_t const * event);

#endif // _T2T_H_