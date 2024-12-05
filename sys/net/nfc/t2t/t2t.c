#include "net/nfc/t2t/t2t.h"
#include <stdio.h>
#include <string.h>

// TODO - this looks like it could be done in a nicer way 
t2t_sn_t t2t_create_default_sn(void){
    t2t_sn_t sn;
    sn.SN0 = 0x01;
    sn.SN1 = 0x02;
    sn.SN2 = 0x03;
    sn.SN3 = 0xFF;
    sn.SN4 = 0x11;
    sn.SN5 = 0x22;
    sn.SN6 = 0x33;
    sn.SN7 = 0x44;
    sn.SN8 = 0x55;
    sn.SN9 = 0x66;
    return sn;
}

static uint8_t * block_no_to_address(uint8_t block_no, nfc_t2t_t *tag){
    return (uint8_t*) &tag->memory[block_no * 4];
}

t2t_uid_t * t2t_create_4_byte_uid(nfc_t2t_t *tag){
    t2t_sn_t *sn = &tag->sn;
    t2t_uid_t *uid = (t2t_uid_t*) &tag->memory[0];
    uid->UID0 = sn->SN0;
    uid->UID1 = sn->SN1;
    uid->UID2 = sn->SN2;
    uid->UID3 = sn->SN3;
    uid->UID4 = uid->UID0 ^ uid->UID1 ^ uid->UID2 ^ uid->UID3;
    uid->UID5 = 0XFF;
    uid->UID6 = 0XFF;
    uid->UID7 = 0XFF;
    uid->UID8 = 0XFF;
    uid->UID9 = NFC_T2T_VERSION_1_1; //find a norm to verify this
    return uid;
}

t2t_uid_t * t2t_create_7_byte_uid(nfc_t2t_t *tag){
    t2t_sn_t *sn = &tag->sn;
    t2t_uid_t *uid = (t2t_uid_t*) &tag->memory[0];
    uid->UID0 = sn->SN0;
    uid->UID1 = sn->SN1;
    uid->UID2 = sn->SN2;
    uid->UID3 = NFC_T2T_CASCADE_TAG_BYTE ^ uid->UID0 ^ uid->UID1 ^ uid->UID2; // BCC0 = CASCADE_TAG_BYTE ^ UID0 ^ UID1 ^ UID2
    uid->UID4 = sn->SN3;
    uid->UID5 = sn->SN4;
    uid->UID6 = sn->SN5;
    uid->UID7 = sn->SN6;
    uid->UID8 = uid->UID3 ^ uid->UID4 ^ uid->UID5 ^ uid->UID6; // BCC1 = UID3 ^ UID4 ^ UID5 ^ UID6
    uid->UID9 = NFC_T2T_VERSION_1_1; //find a norm to verify this
    return uid;
}

t2t_uid_t * t2t_create_10_byte_uid(nfc_t2t_t *tag){
    t2t_sn_t *sn = &tag->sn;
    t2t_uid_t *uid = (t2t_uid_t*) &tag->memory[0];
    uid->UID0 = sn->SN0;
    uid->UID1 = sn->SN1;
    uid->UID2 = sn->SN2;
    uid->UID3 = sn->SN3;
    uid->UID4 = sn->SN4;
    uid->UID5 = sn->SN5;
    uid->UID6 = sn->SN6;
    uid->UID7 = sn->SN7;
    uid->UID8 = sn->SN8;
    uid->UID9 = sn->SN9;
    return uid;
}


t2t_static_lock_bytes_t * t2t_create_static_lock_bytes(bool read_write, nfc_t2t_t *tag){
    t2t_static_lock_bytes_t *lb = (t2t_static_lock_bytes_t*) &tag->memory[NFC_T2T_SIZE_UID];
    if(read_write){
        lb->lock_byte1 = NFC_T2T_LOCK_BYTES_READ_WRITE;
        lb->lock_byte2 = NFC_T2T_LOCK_BYTES_READ_WRITE;
    }else{
        lb->lock_byte1 = NFC_T2T_LOCK_BYTES_READ_ONLY;
        lb->lock_byte2 = NFC_T2T_LOCK_BYTES_READ_ONLY;
    }
    return lb;
}

t2t_static_lock_bytes_t * t2t_set_static_lock_bytes(t2t_static_lock_bytes_t * lock_bytes, nfc_t2t_t *tag){
    t2t_static_lock_bytes_t *lb = (t2t_static_lock_bytes_t*) &tag->memory[NFC_T2T_SIZE_UID];
    lb->lock_byte1 = lock_bytes->lock_byte1;
    lb->lock_byte2 = lock_bytes->lock_byte2;
    return lb;
}

t2t_cc_t * t2t_create_cc(bool readable, bool writeable, uint32_t size_data_area, nfc_t2t_t *tag){
    t2t_cc_t *cc = (t2t_cc_t*) &tag->memory[NFC_T2T_SIZE_UID+NFC_T2T_SIZE_STATIC_LOCK_BYTES];
    cc->magic_number = NFC_T2T_CC_MAGIC_NUMBER;
    cc->version_number = NFC_T2T_VERSION_1_1;
    cc->memory_size = size_data_area / 8;
    if(readable && writeable){
        cc->read_write_access = NFC_T2T_CC_READ_WRITE;
    }else{
        cc->read_write_access = NFC_T2T_CC_READ_ONLY;
    }
    
    return cc;
}

t2t_cc_t * t2t_set_cc(t2t_cc_t * cap_cont, nfc_t2t_t *tag){
    t2t_cc_t *cc = (t2t_cc_t*) &tag->memory[NFC_T2T_SIZE_UID+NFC_T2T_SIZE_STATIC_LOCK_BYTES];
    cc->magic_number = cap_cont->magic_number;
    cc->memory_size = cap_cont->memory_size;
    cc->read_write_access = cap_cont->read_write_access;
    cc->version_number = cap_cont->version_number;
    return cc;
}


//TODO - this is a tad stupid, it just writes the tlv to the beginning of the data area
nfc_tlv_t * create_null_type_tlv(nfc_t2t_t *tag){
    nfc_tlv_t * tlv = (nfc_tlv_t*) &tag->memory[NFC_T2T_SIZE_UID+NFC_T2T_SIZE_STATIC_LOCK_BYTES+NFC_T2T_SIZE_CC];
    tlv->type = NFC_TLV_NULL_TLV_TYPE;
    return tlv;
}

nfc_t2t_t create_type_2_tag(t2t_sn_t *sn, t2t_cc_t *cc, t2t_static_lock_bytes_t *lb, 
                            uint32_t memory_size, uint8_t *memory)
{
    nfc_t2t_t t2t = {0};
    t2t.memory = memory;
    t2t.memory_size = memory_size;
    // TODO - from here on we know that we need dynamic or static memory layout
    // TODO - add code for dynamic layout
    if(memory_size > 64){ //TODO
        t2t.dynamic_layout = true;
        int additional_space_for_lock_bytes = 48;
        t2t.data_area_size = memory_size-(NFC_T2T_SIZE_UID+NFC_T2T_SIZE_CC+NFC_T2T_SIZE_STATIC_LOCK_BYTES+ additional_space_for_lock_bytes);
    }else{
        t2t.dynamic_layout = false;
        t2t.data_area_size = memory_size-(NFC_T2T_SIZE_UID+NFC_T2T_SIZE_CC+NFC_T2T_SIZE_STATIC_LOCK_BYTES);
    }
    //initialize to sector 0
    t2t.current_sector = 0;
    
    //create/use sn
    if(!sn){
        t2t.sn = t2t_create_default_sn();
    }else{
        t2t.sn = *sn;
    }
    //create uid
    t2t.uid = t2t_create_10_byte_uid(&t2t);
    //create lock_bytes
    if(!lb){
        t2t.lb = t2t_create_static_lock_bytes(true, &t2t);
    }else{
        t2t.lb = t2t_set_static_lock_bytes(lb, &t2t);
    }
    
    //create cc
    if(!cc){
        t2t.cc = t2t_create_cc(true, true, t2t.data_area_size, &t2t);
    }else{
        t2t.cc = t2t_set_cc(cc, &t2t);
    }

    //add data - TODO

    return t2t;
}

//ambigous name - handles read command and doesn't just read one block
uint8_t * t2t_read_block(nfc_t2t_t *tag, uint8_t block_no, uint8_t *buf){
    uint8_t *block_address = block_no_to_address(block_no, tag);
    memcpy(buf, block_address, NFC_T2T_READ_RETURN_BYTES);
    return block_address;
}
