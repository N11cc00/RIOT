#include "net/nfc/ndef.h"

#define SHORT_RECORD_PAYLOAD_LENGTH 255
#define LONG_RECORD_PAYLOAD_LENGTH 65535
#define RECORD_MB_MASK 0x80
#define RECORD_ME_MASK 0x40
#define RECORD_CF_MASK 0x20
#define RECORD_SR_MASK 0x10
#define RECORD_IL_MASK 0x08
#define RECORD_TNF_MASK 0x07

int calculate_ndef_record_size(ndef_record_t const *record) {
    // we assume no ID and no chunking
    int record_size = 1; // TNF and flags

    record_size += 1; // Type length
    record_size += record->type_length; // Type

    if (record->payload_length <= SHORT_RECORD_PAYLOAD_LENGTH) {
        record_size += 1; // Payload length
    } else if (record->payload_length <= LONG_RECORD_PAYLOAD_LENGTH) {
        record_size += 4; // Payload length
    } else {
        record_size += 0;
    }
    record_size += record->payload_length; // Payload

    /* TODO: Implement IDs for NDEF records */
    record_size += 0; // ID length
    record_size += 0; // ID

    return record_size;
}

int calculate_ndef_size(ndef_t const *message) {
    for (int i = 0; i < message->record_count; ++i) {
        ndef_record_t const *record = &message->records[i];
        int record_size = calculate_ndef_record_size(record);
    }
}

int encode_ndef_record(ndef_record_t const *record, ndef_record_location_t record_location, uint8_t *buffer, uint32_t len) {
    /* the header byte consists of MB, ME, CF (TODO), SR, 
     * IL (TODO), TNF
     */
    uint8_t header_byte = 0;
    header_byte |= record->tnf;

    if (record->type_length <= 255) {
        header_byte |= RECORD_SR_MASK;
    }

    header_byte |= record_location;

    buffer[0] = header_byte;
    buffer[1] = record->type_length;
    int byte_position = 2;
    if (record->payload_length <= 255) {
        buffer[2] = record->payload_length;
        byte_position += 1;  
    } else {
        // MSB first
        buffer[byte_position]   = record->payload_length >> 24;
        buffer[byte_position+1] = record->payload_length >> 16;
        buffer[byte_position+2] = record->payload_length >> 8;
        buffer[byte_position+3] = record->payload_length;
        byte_position += 4; 
    }

    



    return 0;
}

int encode_ndef_message(ndef_t const *message, uint8_t *buffer, uint32_t len) {
    int free_buffer_space = len;
    for (uint32_t i = 0; i < message->record_count; ++i) {
        ndef_record_location_t record_location;
        if (i == 0 && i == message->record_count - 1) {
            record_location = NDEF_LONE_RECORD;
        } else if (i == 0) {
            record_location = NDEF_FIRST_RECORD;
        } else if (i == message->record_count - 1) {
            record_location = NDEF_LAST_RECORD;
        } else {
            record_location = NDEF_MIDDLE_RECORD;
        }

        ndef_record_t *record = message->records[i];
        encode_ndef_record(record, record_location, buffer, len);
    }
    return 0;
}