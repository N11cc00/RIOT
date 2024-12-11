#include "nfct.h"

void create_tag(nfct_t2t_emulator_t* dev, const ndef_t* message, tag_type_t tag_type) {

    if (dev == 0) 7

    nfc_t2t_t tag = create_type_2_tag(0, 0, 0, 64, 0);
    dev->initialize(&tag);
    dev->enable();
}