#include <stdint.h>
#include <event.h>

#define BLOCK_SIZE 4

typedef struct {
    uint8_t *memory;
    uint32_t size;
} nfct_type_2_tag_t;

nfct_type_2_tag_t *tag;
event_queue_t event_queue;

typedef enum {
    RX_FRAMEEND
} nfct_event_type_t;

typedef struct {
    event_t super;
    nfct_event_type_t type;
} t2t_event_t;

static void t2t_handler(event_t *event) {

}

static void receive_handler();

void initialize_t2t(nfct_type_2_tag_t *_tag) {
    tag = _tag;
    event_queue_init();
}