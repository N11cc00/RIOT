#include "net/nfc/ndef.h"

#include "net/nfc/ndef_text_payload.h"
#include "log.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>


#define SHORT_RECORD_PAYLOAD_LENGTH 255
#define LONG_RECORD_PAYLOAD_LENGTH 65535

#define SHORT_RECORD_PAYLOAD_LENGTH_SIZE 1
#define LONG_RECORD_PAYLOAD_LENGTH_SIZE 4

/* Structure of the header byte */
#define RECORD_MB_MASK 0x80
#define RECORD_ME_MASK 0x40
#define RECORD_CF_MASK 0x20
#define RECORD_SR_MASK 0x10
#define RECORD_IL_MASK 0x08
#define RECORD_TNF_MASK 0x07

/**
 * @brief Converts the payload length field to a uint32_t.
 * 
 * @note The payload length can be either 1 or 4 bytes depending on the SR flag
 * in the NDEF record header. Analogously, the @ref convert_uint32_to_payload_length
 * does the opposite.
 * 
 * @param[in] bytes
 * @param[in] size
 * 
 * @return Converted uint32_t
 */
static uint32_t convert_payload_length_to_uint32(uint8_t const *bytes, uint32_t size) {
    assert(size == NDEF_RECORD_SHORT_PAYLOAD_LENGTH_SIZE || size == NDEF_RECORD_LONG_PAYLOAD_LENGTH_SIZE);
    
    if (size == NDEF_RECORD_SHORT_PAYLOAD_LENGTH_SIZE) {
        return (uint32_t) bytes[0];
    } else {
        uint32_t return_value = 0;
        return (return_value | bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    }
    return 0;
}

/**
 * @brief Converts a uint32_t to the payload length field.
 * 
 * @note Does the opposite of @ref convert_payload_length_to_uint32.
 * 
 * @param[out] bytes
 * @param[in] size
 * @param[in] uint32
 */
static void convert_uint32_to_payload_length(uint8_t* bytes, uint32_t size, uint32_t uint32) {
    assert(size == NDEF_RECORD_SHORT_PAYLOAD_LENGTH_SIZE || size == NDEF_RECORD_LONG_PAYLOAD_LENGTH_SIZE);
    if (size == NDEF_RECORD_SHORT_PAYLOAD_LENGTH_SIZE) {
        bytes[0] = uint32;
    } else {
        bytes[0] = (uint32 >> 24) & 0xFF;
        bytes[1] = (uint32 >> 16) & 0xFF;
        bytes[2] = (uint32 >> 8) & 0xFF;
        bytes[3] = uint32 & 0xFF;
    }
}

size_t get_ndef_size(ndef_t const *ndef) {
    return ndef->buffer.cursor;
}

void pretty_print_ndef(ndef_t const *ndef) {
    printf("----------------\n");
    printf("NDEF Printing\n");
    printf("NDEF message records: %lu\n", ndef->record_count);
    printf("\n");
    for (size_t i = 0; i < (size_t) ndef->record_count; ++i) {
        ndef_record_desc_t record = ndef->records[i];
        printf("Record %d\n", i);
        printf("----\n");
        printf("TNF: %d\n", record.header[0] & RECORD_TNF_MASK);
        printf("Type length: %d\n", record.type_length[0]);
        printf("Type: ");
        for (int j = 0; j < record.type_length[0]; ++j) {
            printf("%c", record.type[j]);
        }
        printf("\n");

        printf("Payload length size: %u\n", record.payload_length_size); 
        uint32_t payload_length = convert_payload_length_to_uint32(record.payload_length, record.payload_length_size);
        printf("Payload length: %lu\n", payload_length);
        printf("Payload: ");
        for (size_t j = 0; j < (size_t) payload_length; ++j) {
            printf("0x%02x ", record.payload[j]);
        }
        printf("\n\n");
    }
    printf("----------------");
    printf("\n");
}

uint8_t* write_to_ndef_buffer(ndef_t* ndef, uint8_t const *data, uint32_t data_length) {
    uint8_t *before_write = &(ndef->buffer.memory[ndef->buffer.cursor]); 
    if (ndef->buffer.cursor + data_length > ndef->buffer.memory_size) {
        LOG_ERROR("NDEF buffer overflow\n");
        return NULL;
    }
    memcpy(&ndef->buffer.memory[ndef->buffer.cursor], data, data_length);
    ndef->buffer.cursor += data_length;
    return before_write;
}

void initialize_ndef_message(ndef_t *message, uint8_t *buffer, uint32_t buffer_size) {
    assert(message != NULL);
    assert(buffer != NULL);
    message->buffer.memory = buffer;
    message->buffer.memory_size = buffer_size;
    message->buffer.cursor = 0;
    message->record_count = 0;
}

int add_ndef_record(ndef_t *message, uint8_t const *type, uint8_t type_length, uint8_t const *id, uint8_t id_length, uint8_t const *payload, uint32_t payload_length, bool mb, bool me, bool cf, bool sr, bool il, ndef_record_tnf_t tnf) {
    ndef_record_desc_t record;

    assert(message != NULL);
    
    if (message->record_count >= MAX_NDEF_RECORD_COUNT) {
        LOG_ERROR("NDEF record count exceeds maximum\n");
        return -1;
    }

    /** The ME flag can only be set on the last record of the NDEF message.
     *  The second to last record should have the ME flag cleared.
     */
    if (message->record_count >= 1) {
        message->records[message->record_count - 1].header[0] &= ~RECORD_ME_MASK;
        mb = false;
    } else {
        mb = true;
    }

    // message end has to be true because records are only added at the end
    me = true;

    uint8_t header_byte = 0;
    header_byte |= mb ? RECORD_MB_MASK : 0;
    header_byte |= me ? RECORD_ME_MASK : 0;
    header_byte |= cf ? RECORD_CF_MASK : 0;
    header_byte |= sr ? RECORD_SR_MASK : 0;
    header_byte |= il ? RECORD_IL_MASK : 0;
    header_byte |= tnf;

    assert(payload_length > 0);
    assert(payload_length <= LONG_RECORD_PAYLOAD_LENGTH);
    assert((payload_length <= SHORT_RECORD_PAYLOAD_LENGTH && sr == 1) ||
           (payload_length > SHORT_RECORD_PAYLOAD_LENGTH && sr == 0));
    

    // the size of the payload length field can be 1 or 4 depending on the SR (short record) flag
    uint8_t payload_length_byte_count = sr ? SHORT_RECORD_PAYLOAD_LENGTH_SIZE : LONG_RECORD_PAYLOAD_LENGTH_SIZE;

    // this array either holds 4 or 1 bytes depending on the SR flag
    uint8_t payload_length_bytes[4];
    convert_uint32_to_payload_length(payload_length_bytes, payload_length_byte_count, payload_length);

    record.payload_length_size = payload_length_byte_count;

    record.header = write_to_ndef_buffer(message, &header_byte, 1);
    record.start = record.header;
    record.type_length = write_to_ndef_buffer(message, &type_length, 1);
    record.payload_length = write_to_ndef_buffer(message, payload_length_byte_count == LONG_RECORD_PAYLOAD_LENGTH_SIZE ? 
    payload_length_bytes : &payload_length_bytes[0], payload_length_byte_count);


    if (id) {
        record.id_length = write_to_ndef_buffer(message, &id_length, 1);
    }

    record.type = write_to_ndef_buffer(message, (uint8_t*) type, type_length);

    if (id) {
        record.id = write_to_ndef_buffer(message, id, id_length);
    }

    /** this needs to be done so the payload points to the correct position 
     *  for further writes 
     * */
    record.payload = &message->buffer.memory[message->buffer.cursor];

    // the payload has to be written by the calling function
    (void) payload;

    message->records[message->record_count] = record;
    message->record_count += 1;

    return 0;
}

/*
int calculate_ndef_record_size(ndef_record_t const *record) {
    // we assume no ID and no chunking
    int record_size = 1; // TNF and flags

    record_size += 1; // Type length
    record_size += record->type_length; // Type

    if (record->payload_length <= SHORT_RECORD_PAYLOAD_LENGTH && record->payload_length > 0) {
        record_size += 1; // Payload length
    } else if (record->payload_length <= LONG_RECORD_PAYLOAD_LENGTH && record->payload_length > 0) {
        record_size += 4; // Payload length
    } else {
        record_size += 0;
    }
    record_size += record->payload_length; // Payload

    record_size += 0; // ID length
    record_size += 0; // ID

    return record_size;
}
*/

/*
int calculate_ndef_size(ndef_t const *message) {
    int ndef_size = 0;
    for (int i = 0; i < message->record_count; ++i) {
        ndef_record_t const *record = &message->records[i];
        int record_size = calculate_ndef_record_size(record);
        ndef_size += record_size;
    }
    return ndef_size;
}
*/

/*
int encode_ndef_record(ndef_record_t const *record, ndef_record_location_t record_location, uint8_t *buffer, uint32_t len) {
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

    memcpy(&buffer[byte_position], record->type, record->type_length);

    return 0;
}
*/

/*
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
*/

/*
int create_ndef_message_with_text(ndef_t *message, char const *text, uint32_t text_length) {
    ndef_t msg = {
        .record_count = 1,
        .records = {
            {
                .tnf = TNF_WELL_KNOWN,
                .type_length = 1,
                .type = {0x54}, // T
                .id_length = 0,
                .id = 0,
                .payload_length = text_length,
                .payload = text
            }
        }
    };
}
*/