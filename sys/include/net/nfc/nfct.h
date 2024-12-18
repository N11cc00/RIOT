#ifndef NFCT_H
#define NFCT_H

#include "ndef.h"
#include "nfct_t2t_emulator.h"
#include "net/nfc/t2t/t2t.h"

typedef enum {
    TYPE_2_TAG,
    TYPE_4_TAG
} tag_type_t;

#if MODULE_NRFX_NFCT
#include "nrfx_nfct_t2t_emulator.h"
nfct_t2t_emulator_t default_t2t_emulator_dev = {
    .initialize = initialize_t2t,
    .enable = enable_t2t,
    .disable = disable_t2t,
    .uninit = uninitialize_t2t
};
#endif 


void create_tag(nfct_t2t_emulator_t* dev, const ndef_t* message, tag_type_t tag_type);

void delete_tag(void);

void create_tag_with_ndef(void);

#endif