//#include "../../../sys/include/net/nfc/t2t/t2t.h"
#include "net/nfc/t2t/t2t.h"
#include "net/nfc/nfct.h"
#include <stddef.h>
#include <stdio.h>

bool test_t2t_reading(void) {
    uint8_t mem[NFC_T2T_STATIC_MEMORY_SIZE];
    //uint8_t ndef_message_content[] = {'H','e','l','l','o',' ','W','o','r','l','d'};
    nfc_t2t_t tag = create_type_2_tag(NULL, NULL, NULL, NFC_T2T_STATIC_MEMORY_SIZE, mem);
    printf("Tag created\n");
    uint8_t buf[16];
    t2t_read_block(&tag, 0,buf);
    printf("read block\n");
    for(int i=0; i<16;i++){
        printf("Block 0: %#04X\n", buf[i]);
    }
    return true;
}

bool test_t2t_emulator() {
    create_tag(&default_t2t_emulator_dev, NULL, TYPE_2_TAG);
}

int main(void) {
    test_t2t_creation_and_reading();
    test_t2t_emulator();
}

