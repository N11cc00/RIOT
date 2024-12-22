//#include "../../../sys/include/net/nfc/t2t/t2t.h"
#include "net/nfc/t2t/t2t.h"
#include "net/nfc/nfct.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "net/nfc/ndef.h"
#include "net/nfc/ndef_text_payload.h"

static void print_ndef_as_hex(ndef_t const *message) {
    printf("NDEF message size: %lu\n", message->buffer.cursor);
    for (uint32_t i = 0; i < message->buffer.cursor; ++i) {
        if (i % 4 == 0 && i != 0) {
            printf("\n");
        }
        printf("0x%02x ", message->buffer.memory[i]);
    }
    printf("\n\n");
}

static bool test_t2t(void) {
    uint8_t tag_mem[NFC_T2T_STATIC_MEMORY_SIZE];
    //uint8_t ndef_message_content[] = {'H','e','l','l','o',' ','W','o','r','l','d'};
    ndef_t ndef_msg;
    uint8_t buffer[1024];
    nfc_t2t_t tag;

    ndef_init(&ndef_msg, buffer, 1024);
    ndef_add_text_record(&ndef_msg, "Hello World", 11, "en", 2, UTF8);


    int error = 0;
    error = create_type_2_tag(&tag, NULL, NULL, NULL, sizeof(tag_mem), tag_mem);
    if(error){
        printf("Error while creating the tag\n");
    }
    printf("Tag created\n");
    uint8_t buf[16];
    t2t_handle_read(&tag, 0,buf);
    printf("read block\n");
    //nfc_tlv_t null_block = {0};
    t2t_create_null_tlv(&tag);
    t2t_create_null_tlv(&tag);
    t2t_create_null_tlv(&tag);
    t2t_create_null_tlv(&tag);

    t2t_add_ndef_msg(&tag, &ndef_msg);
    printf("added ndef\n");

    uint8_t write_buf[4] = {'H','E','L','O'};
    t2t_handle_write(&tag, 10, write_buf);
    //manual set write_only
    t2t_handle_read(&tag, 3, buf);
    memcpy(write_buf, buf, 4);
    write_buf[3] = (uint8_t) 0x0F;
    error = t2t_handle_write(&tag, 3, write_buf);
    if(error != 0) printf("should not happen\n");
    write_buf[0] = 'U';
    write_buf[1] = 'U';
    write_buf[2] = 'U';
    write_buf[3] = 'U';
    error = t2t_handle_write(&tag, 11, write_buf);
    if(error != 0) printf("that's fine\n");
    t2t_dump_tag_memory(&tag);
    return true;
}

static bool test_ndef_text_record(void) {
    ndef_t message;
    uint8_t buffer[1024];

    ndef_init(&message, buffer, 1024);
    ndef_add_text_record(&message, "Hello World", 11, "en", 2, UTF8);
    print_ndef_as_hex(&message);
    ndef_pretty_print(&message);
    return true;
}

static bool test_two_ndef_text_records(void) {
    ndef_t message;
    uint8_t buffer[1024];
    ndef_init(&message, buffer, 1024);
    ndef_add_text_record(&message, "Hello World", 11, "en", 2, UTF8);
    ndef_add_text_record(&message, "Hej Verden", 10, "da", 2, UTF8);
    ndef_pretty_print(&message);
    return true;
}

/*
bool test_nfct(void) {
    ndef_t ndef_message;
    uint8_t buffer[1024];

    initialize_ndef_message(&ndef_message, buffer, 1024);
    ndef_add_text_record(&ndef_message, "Hello World", 11, "en", 2, UTF8);
    create_tag(&DEFAULT_T2T_EMULATOR_DEV, &ndef_message, TYPE_2_TAG);
    return true;
}

*/
int main(void){
    test_t2t();
    test_ndef_text_record();
    test_two_ndef_text_records();
    // test_nfct();
}
