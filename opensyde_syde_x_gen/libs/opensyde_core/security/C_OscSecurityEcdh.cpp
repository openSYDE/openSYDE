//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core ECDH encryption utility

   see header in .hpp file for details.

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

extern "C" {
#include "osy_crypto_algorithm_ecdh.h"
}

#include "C_OscSecurityEcdh.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace std;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get public and private key.

   Wrapper for osy_cry_alg_ecdh_get_ec_keys().

   \param[out]  orau8_PublicKey    Public key
   \param[out]  orau8_PrivateKey   Private key

   \retval  C_NO_ERR    keys available (and returned if requested)
   \retval  else        error occurred
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdh::h_GetKeys(uint8_t (&orau8_PublicKey)[hu32_PUBLIC_KEY_LENGTH],
                                     uint8_t (&orau8_PrivateKey)[hu32_PRIVATE_KEY_LENGTH])
{
   // always force creation on client side
   return osy_cry_alg_ecdh_get_ec_keys(1U, orau8_PublicKey, orau8_PrivateKey);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Derive AES key.

   Wrapper for osy_cry_alg_ecdh_derive_aes_key().

   \param[in]   orau8_OthersPublicKey    Others public key
   \param[in]   orau8_OwnPrivateKey      Own private key
   \param[out]  orau8_AesKey             Aes key

   \retval   C_NO_ERR   success, key is orau8_AesKey
   \retval   else       error occured
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdh::h_DeriveKey(const uint8_t (&orau8_OthersPublicKey)[hu32_PUBLIC_KEY_LENGTH],
                                       const uint8_t (&orau8_OwnPrivateKey)[hu32_PRIVATE_KEY_LENGTH],
                                       uint8_t (&orau8_AesKey)[hu32_AES_KEY_LENGTH])
{
   return osy_cry_alg_ecdh_derive_aes_key(orau8_OthersPublicKey, orau8_OwnPrivateKey, orau8_AesKey);
}
