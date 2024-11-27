#include "nrfx_nfct_t2t_emulator.h"


int main(void) {
    uint8_t tag_memory[] = {
    '\x01', '\x02', '\x03', '\xFF', // Internal
    '\x11', '\x22', '\x33', '\x44', // Serial Number
    '\x55', '\x66', '\x00', '\x00', // Internal / Lock
    '\xE1', '\x10', '\x06', '\x00', // Capability Container
    '\x03', '\x03', '\xD0', '\x00', // Data
    '\x00', '\x00', '\x00', '\x00',  // Data
    '\x00', '\x00', '\x00', '\x00',  // Data
    '\x00', '\x00', '\x00', '\x00',  // Data
    '\x00', '\x00', '\x00', '\x00',  // Data
    '\x00', '\x00', '\x00', '\x00',  // Data
    '\x00', '\x00', '\x00', '\x00',  // Data
    '\x00', '\x00', '\x00', '\x00',  // Data
    '\x00', '\x00', '\x00', '\x00',  // Data
    '\x00', '\x00', '\x00', '\x00',  // Data
    '\x00', '\x00', '\x00', '\x00',  // Data
    '\x00', '\x00', '\x00', '\x00',  // Data
    };
    uint8_t tag_memory_size = sizeof(tag_memory);

    uint8_t uid[] = {'\x01', '\x02', '\x03', '\xFF'};
    uint8_t uid_size = sizeof(uid);

    nfct_type_2_tag_t tag = {
        .memory = tag_memory,
        .memory_size = tag_memory_size
    };

    initialize_t2t(&tag, true, uid, uid_size);
    enable_t2t();

}