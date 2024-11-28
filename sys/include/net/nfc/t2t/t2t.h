/*
 * Copyright (C) 2024 Max Stolze <max_karl.stolze@mailbox.tu-dresden.de>
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


// typedefs
typedef struct {

} t2t_uid_t;

typedef struct {

} t2t_sn_t;

typedef struct {

} t2t_cc_t;

typedef struct {

} t2t_lock_bytes_t;
    

typedef struct{
    t2t_uid_t *uid;
    t2t_sn_t *sn;
    t2t_cc_t *cc;
    t2t_lock_bytes_t *lb;
    tlv_t *data;
    // t2t_dynamic_t *extra;
} nfc_t2t_t;

//functions


#endif
