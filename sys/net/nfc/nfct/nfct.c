#include "net/nfc/nfct/nfct.h"
#include "net/nfc/ndef/ndef.h"

#include "modules.h"

#if IS_USED(MODULE_NFC_T2T)
void nfct_create_tag(nfct_t2t_emulator_t* dev, void *tag, const ndef_t* message, const tag_type_t tag_type) {
    if (tag_type == TYPE_2_TAG) {
        nfc_t2t_t *t2t = (nfc_t2t_t *) tag; 
        t2t_add_ndef_msg(t2t, message);
        dev->init(t2t);
        dev->enable();
    }
}

void nfct_delete_tag(nfct_t2t_emulator_t* dev) {
    dev->disable();
    dev->uninit();
}
#endif