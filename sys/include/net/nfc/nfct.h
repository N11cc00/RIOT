#ifndef NFCT_H
#define NFCT_H

#include "ndef.h"
#include "nfct_t2t_emulator.h"
#include "t2t.h"

typedef enum {
    TYPE_2_TAG,
    TYPE_4_TAG
} tag_type_t;

void create_tag(nfct_t2t_emulator_t* dev, const ndef_t* message, tag_type_t tag_type);

void delete_tag();

void create_tag_with_ndef();

#endif