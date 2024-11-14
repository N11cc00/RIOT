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

#include "t2t.h"
#include <stdint.h>
#include <event.h>
#include "panic.h"
#include "cpu.h"

int main(void) {
    nfct_type_2_tag_t tag;
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
    tag.memory = tag_memory;
    tag.size = sizeof(tag_memory);

    uint8_t nfcid[] = {'\x01', '\x02', '\x03', '\xFF'};




    initialize_t2t(&tag, false, nfcid, sizeof(nfcid));

    enable_t2t();

    start_event_loop();
}