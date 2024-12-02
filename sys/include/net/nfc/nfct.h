#ifndef NFCT_H
#define NFCT_H

#include "ndef.h"

typedef enum {
    TYPE_2_TAG,
    TYPE_4_TAG
} tag_type_t;

void create_tag(const ndef_t* message, tag_type_t tag_type,  nfct_type_2_tag* dev);

void delete_tag();

void create_tag_with_ndef();

#endif