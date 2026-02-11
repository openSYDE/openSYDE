//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       AES-128-CBC encryption/decryption generic algorithm implementation
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef OSY_CRYPTO_ALGORITHM_AES_H
#define OSY_CRYPTO_ALGORITHM_AES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

/* -- Defines ---------------------------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif

#define OSY_CRY_ALG_AES_SIZE_AES_KEY     (16U)
#define OSY_CRY_ALG_AES_SIZE_INIT_VECTOR (16U)

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables --------------------------------------------------------------------------------------- */

/* -- Function Prototypes --------------------------------------------------------------------------------------- */
extern sint16 osy_cry_alg_aes_128_cbc_encrypt(const uint8 opu8_Key[OSY_CRY_ALG_AES_SIZE_AES_KEY],
                                              const uint8 opu8_InitVector[OSY_CRY_ALG_AES_SIZE_INIT_VECTOR],
                                              uint8 * const opu8_Data, uint16 * const opu16_NumBytes);
extern sint16 osy_cry_alg_aes_128_cbc_decrypt(const uint8 opu8_Key[OSY_CRY_ALG_AES_SIZE_AES_KEY],
                                              const uint8 opu8_InitVector[OSY_CRY_ALG_AES_SIZE_INIT_VECTOR],
                                              uint8 * const opu8_Data, uint16 * const opu16_NumBytes);
/* -- Implementation --------------------------------------------------------------------------------------- */

#ifdef __cplusplus
} //end of extern "C"
#endif

#endif
