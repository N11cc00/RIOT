#ifndef NDEF_H
#define NDEF_H

typedef struct {
    ndef_record_tnf_t tnf;
    uint8_t type_length;
    uint8_t const *type;

    uint8_t id_length;
    uint8_t const *id;

    uint32_t payload_length;
    void const *payload;
} ndef_record_t;

typedef struct {

} ndef_text_record_t;

typedef enum {
	/** The value indicates that there is no type or payload. */
	TNF_EMPTY         = 0x00,
	/** NFC Forum well-known type [NFC RTD]. */
	TNF_WELL_KNOWN    = 0x01,
	/** Media-type as defined in RFC 2046 [RFC 2046]. */
	TNF_MEDIA_TYPE    = 0x02,
	/** Absolute URI as defined in RFC 3986 [RFC 3986]. */
	TNF_ABSOLUTE_URI  = 0x03,
	/** NFC Forum external type [NFC RTD]. */
	TNF_EXTERNAL_TYPE = 0x04,
	/** The value indicates that there is no type associated with this
	 *  record.
	 */
	TNF_UNKNOWN_TYPE  = 0x05,
	/** The value is used for the record chunks used in chunked payload. */
	TNF_UNCHANGED     = 0x06,
	/** The value is reserved for future use. */
	TNF_RESERVED      = 0x07,
} ndef_record_tnf_t;

typedef enum {
	NDEF_FIRST_RECORD  = 0x80, /**< First record. */
	NDEF_MIDDLE_RECORD = 0x00, /**< Middle record. */
	NDEF_LAST_RECORD   = 0x40, /**< Last record. */
	NDEF_LONE_RECORD   = 0xC0  /**< Only one record in the message. */
} ndef_record_location_t;

typedef struct {
    ndef_record_t *records;
    uint32_t record_count; 
} ndef_t;

int encode_ndef_message(ndef_t const *message, uint8_t *buffer, uint32_t const len);

#endif