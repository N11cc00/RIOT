#include "net/nfc/nfct.h"
#include "net/nfc/ndef.h"

#include <cstddef>


void create_tag(nfct_t2t_emulator_t* dev, const ndef_t* message, tag_type_t tag_type) {
    if (tag_type == TYPE_2_TAG) {
        nfc_t2t_t tag;
        create_type_2_tag(&tag, NULL, NULL, NULL, NFC_T2T_STATIC_MEMORY_SIZE, NULL);
        t2t_add_ndef_msg(&tag, message);
        dev->initialize(&tag);
        dev->enable();
    }
}