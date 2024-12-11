#include "net/nfc/t2t/t2t.h"
#include <stdio.h>
#include <string.h>

// TODO - this looks like it could be done in a nicer way 
t2t_sn_t t2t_create_default_sn(void){
    t2t_sn_t sn = NFC_T2T_4_BYTE_DEFAULT_UID;
    return sn;
}

static uint8_t * block_no_to_address(uint8_t block_no, nfc_t2t_t *tag){
    return (uint8_t*) &tag->memory[block_no * 4];
}

t2t_uid_t * t2t_create_uid(nfc_t2t_t *tag){
    t2t_sn_t *sn = &tag->sn;
    t2t_uid_t *uid = (t2t_uid_t*) &tag->memory[0];
    uid->uid[0] = sn->sn[0];
    uid->uid[1] = sn->sn[1];
    uid->uid[2] = sn->sn[2];
    if(sn->sn_length == NFC_ISO14443A_UID_SINGLE_SIZE){
        uid->uid[3] = sn->sn[3];
        uid->uid[4] = uid->uid[0] ^ uid->uid[1] ^ uid->uid[2] ^ uid->uid[3];
        uid->uid[5] = 0XFF;
        uid->uid[6] = 0XFF;
        uid->uid[7] = 0XFF;
        uid->uid[8] = 0XFF;
        uid->uid[9] = NFC_T2T_VERSION_1_1; //find a norm to verify this
    }else if(sn->sn_length == NFC_ISO14443A_UID_DOUBLE_SIZE){
        uid->uid[3] = NFC_T2T_CASCADE_TAG_BYTE ^ uid->uid[0] ^ uid->uid[1] ^ uid->uid[2];
        uid->uid[4] = sn->sn[3];
        uid->uid[5] = sn->sn[4];
        uid->uid[6] = sn->sn[5];
        uid->uid[7] = sn->sn[6];
        uid->uid[8] = uid->uid[4]^ uid->uid[5] ^ uid->uid[6] ^ uid->uid[7];
        uid->uid[9] = NFC_T2T_VERSION_1_1; //find a norm to verify this
    }else{
        uid->uid[3] = sn->sn[3];
        uid->uid[4] = sn->sn[4];
        uid->uid[5] = sn->sn[5];
        uid->uid[6] = sn->sn[6];
        uid->uid[7] = sn->sn[7];
        uid->uid[8] = sn->sn[8];
        uid->uid[9] = sn->sn[9];
    }
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


int create_type_2_tag(nfc_t2t_t *tag, t2t_sn_t *sn, t2t_cc_t *cc, t2t_static_lock_bytes_t *lb, 
                            uint32_t memory_size, uint8_t *memory)
{
    tag->memory = memory;
    tag->memory_size = memory_size;
    memset(tag->memory, 0x00, tag->memory_size);
    // TODO - from here on we know that we need dynamic or static memory layout
    // TODO - add code for dynamic layout
    if(memory_size > 64){ //TODO
        tag->dynamic_layout = true;
        tag->data_area_size = memory_size-(NFC_T2T_SIZE_UID+NFC_T2T_SIZE_CC+NFC_T2T_SIZE_STATIC_LOCK_BYTES+NFC_T2T_SIZE_DYNAMIC_LOCK_BYTES);
    }else{
        tag->dynamic_layout = false;
        tag->data_area_size = NFC_T2T_SIZE_STATIC_DATA_AREA;
        tag->data_area_start = memory + NFC_T2T_START_STATIC_DATA_AREA;
        tag->data_area_cursor = tag->data_area_start;
    }
    //initialize to sector 0
    tag->current_sector = 0;
    
    //create/use sn
    if(!sn){
        tag->sn = t2t_create_default_sn();
    }else{
        tag->sn = *sn;
    }
    //create uid
    tag->uid = t2t_create_uid(tag);
    //create lock_bytes
    if(!lb){
        tag->lb = t2t_create_static_lock_bytes(true, tag);
    }else{
        tag->lb = t2t_set_static_lock_bytes(lb, tag);
    }
    
    //create cc
    if(!cc){
        tag->cc = t2t_create_cc(true, true, tag->data_area_size, tag);
    }else{
        tag->cc = t2t_set_cc(cc, tag);
    }

    //add data - TODO

    return 0;
}

//ambigous name - handles read command and doesn't just read one block
uint8_t * t2t_read_block(nfc_t2t_t *tag, uint8_t block_no, uint8_t *buf){
    uint8_t *block_address = block_no_to_address(block_no, tag);
    memcpy(buf, block_address, NFC_T2T_READ_RETURN_BYTES);
    return block_address;
}


/*TODO - is there a general preference to handle such functions with 
- ENUMs and one function or
- multiple functions, each with descriptive names*/
int t2t_create_null_tlv(nfc_t2t_t *tag){
    tag->data_area_cursor[0] = NFC_TLV_TYPE_NULL_TLV;
    tag->data_area_cursor++;
    return 0;
}

int t2t_create_terminator_tlv(nfc_t2t_t *tag){
    tag->data_area_cursor[0] = NFC_TLV_TYPE_TERMINATOR;
    tag->data_area_cursor++;
    return 0;
}

int write_tlv_length(uint8_t *buf, uint16_t length){
    printf("enter write_tlv_length\n");
    if(length > 0xFF){ //==> length 0xFF and to be interpreted as 2 bytes
        buf[0] = 0xFF;
        buf[1] = (length >> 8) & 0x00FF; //most significant Byte - left 8 bit
        buf[2] = (length & 0x00FF); //lsB - right 8 bit
        return 3;
    }else{
        printf("writing length\n");
        buf[0] = (uint8_t) length;
        return 1;
    }
}

int t2t_create_ndef_tlv(nfc_t2t_t *tag, uint16_t length){
    printf("enter create ndef_tlv\n");
    uint8_t pos = 0;
    tag->data_area_cursor[pos] = NFC_TLV_TYPE_NDEF_MSG;
    printf("wrote tlv type\n");
    pos++;
    pos += write_tlv_length(&tag->data_area_cursor[pos], length);
    printf("wrote length to tag\n");
    if(pos < 2){ // write_tlv returns -1 if smth went wrong
        return -1;
    }
    tag->data_area_cursor += pos;
    return pos; //either 1 or 4
}

//Ich brauch entweder Anfang und Ende vom speicher, dann weiß ich nicht was drin steht oder der user muss tlvs erstellen, wenn ich die eben haben will
// oder der Tag braucht nen link auf seinen tlv
// aber ich kann die Speicherstruktur nicht dynamisch erweitern
int t2t_add_ndef_msg(nfc_t2t_t *tag, nfc_ndef_msg_t *msg){
    int tlv_header_size = 0;
    printf("enter add_ndef\n");
    tlv_header_size = t2t_create_ndef_tlv(tag, msg->size);
    printf("created tlv\n");
    if (tlv_header_size < 0){
        return -1;
    }
    //write msg into tag mem
    memcpy(tag->data_area_cursor,msg->start_address,msg->size);
    tag->data_area_cursor += msg->size;
    //add terminator
    t2t_create_terminator_tlv(tag);
    printf("leave add_ndef\n");
    return 0;
}


void t2t_dump_tag_memory(nfc_t2t_t *tag){
    for(int i=0; i < tag->memory_size; i++){
        if(tag->memory[i] >= 65 && tag->memory[i] <= 122){
            printf("%c\t", tag->memory[i]);
        }else{
            printf("%#04x\t", tag->memory[i]);
        }        
        if(((i+1)%4) == 0){
            printf("\n");
        }
        
    }
    
}

//remove me 
/*
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
*/
