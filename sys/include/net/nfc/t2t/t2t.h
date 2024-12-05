/*
 * Copyright (C) 2024 Technische Universität Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys/net/nfc
 * @{
 *
 * @file
 * @brief       Typedefs and function definitions for NFC Forum Type 2 Tags.
 *
 * Typedefs and function definitions for NFC Forum Type 2 Tags.
 *
 * @author      Max Stolze <max_karl.stolze@mailbox.tu-dresden.de>
 *
 */

#ifndef NET_NFC_TYPE_2_TAG_H
#define NET_NFC_TYPE_2_TAG_H

#include <stdint.h>
#include <stdbool.h>

// NFC specific constants
#define NFC_T2T_CASCADE_TAG_BYTE 0x88
#define NFC_T2T_VERSION_1_1 0x11
#define NFC_T2T_CC_MAGIC_NUMBER 0xE1
#define NFC_T2T_CC_READ_WRITE 0x00
#define NFC_T2T_CC_READ_ONLY 0x0F
#define NFC_T2T_LOCK_BYTES_READ_ONLY 0xFF
#define NFC_T2T_LOCK_BYTES_READ_WRITE 0x00
//sizes
#define NFC_T2T_BLOCK_SIZE 4
#define NFC_T2T_BLOCKS_PER_SECTOR 256
#define NFC_T2T_STATIC_MEMORY_SIZE 64
#define NFC_T2T_STATIC_MEMORY_DATA_AREA_SIZE 64
#define NFC_T2T_DEFAULT_MEM_SIZE NFC_T2T_STATIC_MEMORY_SIZE
#define NFC_T2T_SIZE_UID 10
#define NFC_T2T_SIZE_STATIC_LOCK_BYTES 2
#define NFC_T2T_SIZE_CC 4
#define NFC_T2T_READ_RETURN_BYTES 16

//TLV
#define NFC_TLV_NULL_TLV_TYPE 0x00

// typedefs

typedef struct {
    uint8_t UID0;
    uint8_t UID1;
    uint8_t UID2;
    uint8_t UID3;
    uint8_t UID4; 
    uint8_t UID5; 
    uint8_t UID6; 
    uint8_t UID7;
    uint8_t UID8;
    uint8_t UID9;
} t2t_uid_t; // 10 byte UID


typedef struct {
    uint8_t SN0;
    uint8_t SN1;
    uint8_t SN2;
    uint8_t SN3;
    uint8_t SN4;
    uint8_t SN5;
    uint8_t SN6;
    uint8_t SN7;
    uint8_t SN8;
    uint8_t SN9;
} t2t_sn_t; // 10 Byte SN
/* TODO - find standard that says what UIDs/SNs for T2Ts*/

typedef struct {
    uint8_t lock_byte1;
    uint8_t lock_byte2;
} t2t_static_lock_bytes_t;

typedef struct {
    uint8_t magic_number; //EH1 for NDEF
    uint8_t version_number; // 4 bit major, 4 bit minor -> 0x11
    uint8_t memory_size; // data area size / 8
    uint8_t read_write_access; // 4 bit read, 4 bit write,
} t2t_cc_t;

typedef struct {
    uint8_t type;
    union{
        uint8_t length_one_B;
        uint8_t length_three_B[3];
    };
    uint8_t *value;
} nfc_tlv_t;
    

typedef struct{
    uint8_t *memory;
    uint8_t memory_size;
    bool dynamic_layout;
    uint8_t current_sector;
    t2t_sn_t sn; //this is not part of the t2t mem layout - inside uid
    t2t_uid_t *uid;
    t2t_static_lock_bytes_t *lb;
    t2t_cc_t *cc;
    uint8_t data_area_size;
    nfc_tlv_t *data_area;
    // TODO t2t_dynamic_t *extra;
} nfc_t2t_t;

typedef struct 
{
    int filler;
    //TODO - fill me/move me
}nfc_t2t_write_command_t;

//functions
nfc_t2t_t create_type_2_tag(t2t_sn_t *sn, t2t_cc_t *cc, t2t_static_lock_bytes_t *lb, 
                                uint32_t memory_size, uint8_t *memory);

uint8_t t2t_get_size(nfc_t2t_t *tag); //TODO - or remove, is in struct
bool t2t_write_block(nfc_t2t_t *tag, nfc_t2t_write_command_t data); //TODO
uint8_t * t2t_read_block(nfc_t2t_t *tag, uint8_t block_no, uint8_t *buf);
bool t2t_is_writeable(nfc_t2t_t *tag); //TODO
bool t2t_set_writeable(nfc_t2t_t *tag); //TODO
bool t2t_add_tlv(nfc_t2t_t *tag, nfc_tlv_t *tlv); //TODO
bool t2t_clear_mem(nfc_t2t_t *tag); //TODO

t2t_uid_t * t2t_create_4_byte_uid(nfc_t2t_t *sn);
t2t_uid_t * t2t_create_7_byte_uid(nfc_t2t_t *sn);
t2t_uid_t * t2t_create_10_byte_uid(nfc_t2t_t *tag);

t2t_sn_t t2t_create_default_sn(void);
//t2t_sn_t t2t_create_random_sn(uint8_t length);
/*TODO - only accept predefined? offer to roll one? */

t2t_cc_t * t2t_create_cc(bool readable, bool writeable, uint32_t size_data_area, nfc_t2t_t *tag);
t2t_cc_t * t2t_set_cc(t2t_cc_t * cap_cont, nfc_t2t_t *tag);
t2t_static_lock_bytes_t * t2t_create_static_lock_bytes(bool read_write, nfc_t2t_t *tag);
t2t_static_lock_bytes_t * t2t_set_static_lock_bytes(t2t_static_lock_bytes_t * lock_bytes, nfc_t2t_t *tag);
#endif



//Remove me

/*typedef struct {
    uint8_t SN0;
    uint8_t SN1;
    uint8_t SN2;
    uint8_t SN3;
} t2t_sn_t; // 4Byte SN

typedef struct {
    uint8_t SN0;
    uint8_t SN1;
    uint8_t SN2;
    uint8_t SN3;
    uint8_t SN4;
    uint8_t SN5;
    uint8_t SN6;
} t2t_sn_t; // 7Byte SN*/
/*
typedef struct {
    uint8_t UID0;
    uint8_t UID1;
    uint8_t UID2;
    uint8_t UID4;
    uint8_t BCC0; // BCC = UID0 ^ UID1 ^ UID2 ^ UID3
    uint8_t Internal5; // 0xFF
    uint8_t Internal6; // 0xFF
    uint8_t Internal7; // 0xFF
    uint8_t Internal8; // 0xFF
    uint8_t NFC_lib_version; // NFC-Lib Version
} t2t_4_B_uid_t; // 4 byte UID

typedef struct {
    uint8_t UID0;
    uint8_t UID1;
    uint8_t UID2;
    uint8_t BCC0; // BCC0 = CASCADE_TAG_BYTE ^ UID0 ^ UID1 ^ UID2
    uint8_t UID3;
    uint8_t UID4; 
    uint8_t UID5; 
    uint8_t UID6; 
    uint8_t BCC1; // BCC1 = UID3 ^ UID4 ^ UID5 ^ UID6
    uint8_t NFC_lib_version; // NFC-Lib Version
} t2t_7_B_uid_t; // 7 byte UID

typedef struct {
    uint8_t UID0;
    uint8_t UID1;
    uint8_t UID2;
    uint8_t UID3;
    uint8_t UID4; 
    uint8_t UID5; 
    uint8_t UID6; 
    uint8_t UID7;
    uint8_t UID8;
    uint8_t UID9;
} t2t_10_B_uid_t; // 10 byte UID
*/
