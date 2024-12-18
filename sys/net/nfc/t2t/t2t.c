#include "net/nfc/t2t/t2t.h"
#include "net/nfc/ndef.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Sets dynamic lock bits and decreases data area.
 *
 * Sets the dynamic lock bits to 0x0 at the end of the data area as defined
 * for the default behavior in Type 2 Tag Specification 2.2.2
 *
 * @param tag the type 2 tag to be altered
 * @return 0 in case of success, negative number in case of problems
 */
static int create_default_dynamic_memory_layout(nfc_t2t_t *tag, bool read_only){
// calculate size of dynamic lock bytes and amnt of lock bits
    if(tag->memory_size <= NFC_T2T_STATIC_MEMORY_SIZE){
        return -1;
    }
    uint32_t dyn_lock_bytes;
    uint32_t dyn_lock_bits; // only needed if setting to 1 needed
    if(((tag->data_area_size - NFC_T2T_SIZE_STATIC_DATA_AREA) % 8) > 0){
        dyn_lock_bytes= ((tag->data_area_size - 48) / 64) + 1;
        dyn_lock_bits= ((tag->data_area_size - 48) / 8) + 1;  
    }else{
        dyn_lock_bytes = (tag->data_area_size - 48) / 64;
        dyn_lock_bits = (tag->data_area_size - 48) / 8;
    }
    
// calculate position of dynamic lock bits -> new end of user usable data area
    uint32_t start_lock_bits = tag->data_area_size - 1 - dyn_lock_bytes;
// write lock control tlv to data area - apparently unnecessary for default
// write lock bits to data area
    for(uint32_t i = 0; i < dyn_lock_bytes; i++){
        if(read_only){
            tag->data_area_start[start_lock_bits + i] = 0xFF; //initiate to read_only
        }else{
            tag->data_area_start[start_lock_bits + i] = 0x00; //initiate to read_write
        } 
    }
    // in case last byte is just partially filled with lock bits
    if(read_only && dyn_lock_bits % 8 != 0){
        // only as many bits shall be set to 1 as there are lock bits - remaining bits to 0x0
        // e.g. 70 Byte data area -> 22 byte dyn -> ceil(22/8) = 3bit -> 1110 0000 --> 1111 1111 << (8-3=5) 
        tag->data_area_start[start_lock_bits + dyn_lock_bytes] = (uint8_t) 0xFF << (8-(dyn_lock_bits %8));
    }
    //tag->data_area_size = new_data_area_size;
    return 0;
}

static bool isWriteable(nfc_t2t_t *tag){
    return (tag->cc->read_write_access == (uint8_t) 0x00);
}

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
        tag->data_area_size = memory_size-(NFC_T2T_SIZE_UID+NFC_T2T_SIZE_CC+NFC_T2T_SIZE_STATIC_LOCK_BYTES);
    }else{
        tag->dynamic_layout = false;
        tag->data_area_size = NFC_T2T_SIZE_STATIC_DATA_AREA;
    }
    tag->data_area_start = memory + NFC_T2T_START_STATIC_DATA_AREA;
    tag->data_area_cursor = tag->data_area_start;
    if(tag->dynamic_layout){
        create_default_dynamic_memory_layout(tag, false); //sets dynamic lock bits at end of data area and lowers data_area_size
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

    return 0;
}

int create_type_2_tag_with_ndef(nfc_t2t_t *tag, t2t_sn_t *sn, t2t_cc_t *cc, t2t_static_lock_bytes_t *lb, 
                                uint32_t memory_size, uint8_t *memory, ndef_t *msg){
    int error = 0;
    error = create_type_2_tag(tag, sn, cc, lb, memory_size, memory);
    if(error != 0) return error;
    error = t2t_add_ndef_msg(tag, msg);

    return error;

}

//ambigous name - handles read command and doesn't just read one block
int t2t_handle_read(nfc_t2t_t *tag, uint8_t block_no, uint8_t *buf){
    uint8_t *block_address = block_no_to_address(block_no, tag);
    memcpy(buf, block_address, NFC_T2T_READ_RETURN_BYTES);
    return 0;
}

int t2t_handle_write(nfc_t2t_t *tag, uint8_t block_no, uint8_t const *buf){
    if(!isWriteable(tag)){
        return -1;
    }
    uint8_t *block_address = block_no_to_address(block_no, tag);
    memcpy(block_address, buf, NFC_T2T_BLOCK_SIZE);
    return 0;
    
}


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
    uint8_t pos = 0;
    tag->data_area_cursor[pos] = NFC_TLV_TYPE_NDEF_MSG;
    pos++;
    pos += write_tlv_length(&tag->data_area_cursor[pos], length);
    if(pos < 2){ // write_tlv returns -1 if smth went wrong
        return -1;
    }
    tag->data_area_cursor += pos;
    return pos; //either 1 or 4
}

int t2t_add_ndef_msg(nfc_t2t_t *tag, ndef_t const *msg){
    int tlv_header_size = 0;
    tlv_header_size = t2t_create_ndef_tlv(tag, msg->buffer.cursor);
    if (tlv_header_size < 0){
        return -1;
    }
    //write msg into tag mem
    memcpy(tag->data_area_cursor, msg->buffer.memory,msg->buffer.cursor);
    tag->data_area_cursor += msg->buffer.cursor;
    //add terminator
    t2t_create_terminator_tlv(tag);
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

int t2t_clear_data_area(nfc_t2t_t *tag){
    memset(tag->data_area_start, 0x00, tag->data_area_size);
    tag->data_area_cursor = tag->data_area_start;
    return 0;
}

int t2t_clear_mem(nfc_t2t_t *tag){
    tag->uid = NULL;
    tag->lb = NULL;
    tag->cc = NULL;
    tag->data_area_start = NULL;
    tag->data_area_cursor = NULL;
    tag->data_area_size = 0;
    memset(tag->memory, 0x00, tag->memory_size);
    return 0;
}

