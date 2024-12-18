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

static void print_length_field(uint8_t* bytes, bool sr) {
    if (sr) {
        printf("Length field: ");
        printf("0x%02x\n", bytes[0]);
        return;
    } else {
        printf("Length field: ");
        for (int i = 0; i < 4; ++i) {
            printf("0x%02x ", bytes[i]);
        }
        printf("\n");
    }
}

static bool test_t2t(void) {
    uint8_t tag_mem[NFC_T2T_STATIC_MEMORY_SIZE];
    //uint8_t ndef_message_content[] = {'H','e','l','l','o',' ','W','o','r','l','d'};
    uint8_t msg_content[] = {'\xD1', '\x01', '\x08', '\x54', '\x02', 'e', 'n', 'H', 'e', 'l', 'l', 'o'};
    nfc_ndef_msg_t ndef_msg = {0};
    ndef_msg.size = sizeof(msg_content);
    ndef_msg.start_address = msg_content;
    nfc_t2t_t tag = {0};
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

    initialize_ndef_message(&message, buffer, 1024);
    add_ndef_text_record(&message, "Hello World", 11, "en", 2, UTF8);
    print_length_field(message.records[0].payload_length, message.records[0].payload_length_size == 1);
    print_ndef_as_hex(&message);
    pretty_print_ndef(&message);
    return true;
}

static bool test_two_ndef_text_records(void) {
    ndef_t message;
    uint8_t buffer[1024];
    initialize_ndef_message(&message, buffer, 1024);
    add_ndef_text_record(&message, "Hello World", 11, "en", 2, UTF8);
    add_ndef_text_record(&message, "Hej Verden", 10, "da", 2, UTF8);
    pretty_print_ndef(&message);
    return true;
}

bool test_nfct(void) {
    ndef_t ndef_message;
    uint8_t buffer[1024];

    initialize_ndef_message(&ndef_message, buffer, 1024);
    add_ndef_text_record(&ndef_message, "Hello World", 11, "en", 2, UTF8);
    create_tag(&default_t2t_emulator_dev, &ndef_message, TYPE_2_TAG);
    return true;
}

int main(void){
    test_t2t();
    test_ndef_text_record();
    test_two_ndef_text_records();
    test_nfct();

}
