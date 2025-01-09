#include "net/nfc/ndef/ndef.h"

#include "errno.h"
#include "log.h"
#include <stdbool.h>
#include <assert.h>

#define IDENTIFIER_CODE_LENGTH 1

uint8_t const ndef_uri_record_type[] = {'U'};

int ndef_add_uri_record(ndef_t *message, ndef_uri_identifier_code_t identifier_code, const char *uri, uint32_t uri_length) {
    uint32_t payload_length = IDENTIFIER_CODE_LENGTH + uri_length;
    assert(payload_length <= 65535);

    
    ndef_add_record(message, ndef_uri_record_type, sizeof(ndef_uri_record_type), 0, 0, NULL, payload_length, 
    0, 1, 0, 0, 0, TNF_WELL_KNOWN);

    ndef_write_to_buffer(message, &identifier_code, IDENTIFIER_CODE_LENGTH);
    ndef_write_to_buffer(message, (uint8_t*) uri, uri_length);

    return 0;
}