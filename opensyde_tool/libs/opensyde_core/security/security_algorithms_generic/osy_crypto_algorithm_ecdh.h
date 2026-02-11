//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Elliptic curve diffie hellman generic algorithms implementation
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef OSY_CRYPTO_ALGORITHM_ECDH_H
#define OSY_CRYPTO_ALGORITHM_ECDH_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

/* -- Defines ---------------------------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif

#define OSY_CRY_ALG_ECDH_SIZE_PUBLIC_KEY  (33U) //in compressed format
#define OSY_CRY_ALG_ECDH_SIZE_PRIVATE_KEY (32U)
#define OSY_CRY_ALG_ECDH_SIZE_AES_KEY     (16U)

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables --------------------------------------------------------------------------------------- */

/* -- Function Prototypes --------------------------------------------------------------------------------------- */
extern sint16 osy_cry_alg_ecdh_get_ec_keys(const uint8 ou8_ForceCreation,
                                           uint8 opu8_PublicKey[OSY_CRY_ALG_ECDH_SIZE_PUBLIC_KEY],
                                           uint8 opu8_PrivateKey[OSY_CRY_ALG_ECDH_SIZE_PRIVATE_KEY]);
extern sint16 osy_cry_alg_ecdh_derive_aes_key(const uint8 opu8_OthersPublicKey[OSY_CRY_ALG_ECDH_SIZE_PUBLIC_KEY],
                                              const uint8 opu8_OwnPrivateKey[OSY_CRY_ALG_ECDH_SIZE_PRIVATE_KEY],
                                              uint8 opu8_AesKey[OSY_CRY_ALG_ECDH_SIZE_AES_KEY]);
/* -- Implementation --------------------------------------------------------------------------------------- */

#ifdef __cplusplus
} //end of extern "C"
#endif

#endif
