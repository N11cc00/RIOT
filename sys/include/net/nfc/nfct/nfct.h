#ifndef NFCT_H
#define NFCT_H

#include "net/nfc/ndef/ndef.h"
#include "nfct_t2t_emulator.h"
#include "net/nfc/t2t/t2t.h"
#include "modules.h"

typedef enum {
    TYPE_2_TAG,
    TYPE_4_TAG
} tag_type_t;

#if IS_USED(MODULE_NRFX_NFCT)
#include "nrfx_nfct_t2t_emulator.h"
#define DEFAULT_T2T_EMULATOR_DEV  (nfct_t2t_emulator_t) {   \
    .init = initialize_t2t,                 \
    .enable = enable_t2t,                   \
    .disable = disable_t2t,                 \
    .uninit = uninitialize_t2t              \
}
#endif



void nfct_create_tag(nfct_t2t_emulator_t* dev, void* tag, const ndef_t* message, tag_type_t tag_type);

void nfct_delete_tag(nfct_t2t_emulator_t* dev);

#endif