//#include "../../../sys/include/net/nfc/t2t/t2t.h"
#include "net/nfc/t2t/t2t.h"
#include <stddef.h>
#include <stdio.h>

int main(void){
    uint8_t mem[NFC_T2T_STATIC_MEMORY_SIZE];
    //uint8_t ndef_message_content[] = {'H','e','l','l','o',' ','W','o','r','l','d'};
    nfc_t2t_t tag = {0};
    int error = 0;
    error = create_type_2_tag(&tag, NULL, NULL, NULL, NFC_T2T_STATIC_MEMORY_SIZE, mem);
    if(error){
        printf("Error while creating the tag\n");
    }
    printf("Tag created\n");
    uint8_t buf[16];
    t2t_read_block(&tag, 0,buf);
    printf("read block\n");
    for(int i=0; i<16;i++){
        printf("Block 0: %#04X\n", buf[i]);
    }
}

