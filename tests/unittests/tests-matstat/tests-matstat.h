/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the Matstat library
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @brief   The entry point of this test suite.
*/
void tests_matstat(void);

/**
 * @brief   Generates tests for matstat
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_matstat_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
