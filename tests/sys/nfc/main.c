//#include "../../../sys/include/net/nfc/t2t/t2t.h"
#include "net/nfc/t2t/t2t.h"
#include "net/nfc/nfct.h"
#include <stddef.h>
#include <stdio.h>

int main(void){
    uint8_t tag_mem[NFC_T2T_STATIC_MEMORY_SIZE];
    //uint8_t ndef_message_content[] = {'H','e','l','l','o',' ','W','o','r','l','d'};
    uint8_t msg_content[] = {'\xD1', '\x01', '\x08', '\x54', '\x02', 'e', 'n', 'H', 'e', 'l', 'l', 'o'};
    nfc_ndef_msg_t ndef_msg = {0};
    ndef_msg.size = sizeof(msg_content);
    ndef_msg.start_address = msg_content;
    nfc_t2t_t tag = {0};
    int error = 0;
    error = create_type_2_tag(&tag, NULL, NULL, NULL, NFC_T2T_STATIC_MEMORY_SIZE, tag_mem);
    if(error){
        printf("Error while creating the tag\n");
    }
    printf("Tag created\n");
    uint8_t buf[16];
    t2t_read_block(&tag, 0,buf);
    printf("read block\n");
    //nfc_tlv_t null_block = {0};
    t2t_create_null_tlv(&tag);
    t2t_create_null_tlv(&tag);
    t2t_create_null_tlv(&tag);
    t2t_create_null_tlv(&tag);
    //t2t_add_tlv(&tag, &null_block);
    //nfc_ndef_msg_t msg;
    //t2t_create_ndef_msg(&msg);
    //nfc_tlv_t ndef_block;
    //t2t_create_ndef_tlv(&ndef_block, &ndef_msg);
    t2t_add_ndef_msg(&tag, &ndef_msg);
    printf("added ndef\n");
    t2t_dump_tag_memory(&tag);

}

