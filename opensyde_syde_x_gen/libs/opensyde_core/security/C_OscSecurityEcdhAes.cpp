//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core ECDH/AES encryption utility

   see header in .hpp file for details.

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstring>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/core_names.h>
#include <openssl/sha.h>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglUtils.hpp"

#include "C_OscSecurityEcdhAes.hpp"
#include "C_OscSecurityAesCbc.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::errors;
using namespace std;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSecurityEcdhAes::C_OscSecurityEcdhAes() :
   mpc_TheKey(NULL),
   mpu8_AesKey(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSecurityEcdhAes::~C_OscSecurityEcdhAes()
{
   if (mpc_TheKey != NULL)
   {
      EVP_PKEY_free(mpc_TheKey);
      mpc_TheKey = NULL;
   }
   if (mpu8_AesKey != NULL)
   {
      OPENSSL_secure_clear_free(mpu8_AesKey, hu32_AES_KEY_LENGTH);
      mpu8_AesKey = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get AES key

   Note that this function should only be used for testing purposes.
   For production code the key should be kept as "local" as possible.

   \param[out]       orau8_AesKey          AES key

   \retval  C_NO_ERR    key read
   \retval  C_NOACT     no key available
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdhAes::GetAesKey(uint8_t(&orau8_AesKey)[hu32_AES_KEY_LENGTH]) const
{
   int32_t s32_Result = C_NOACT;

   if (C_OscSecurityEcdhAes::mpu8_AesKey != NULL)
   {
      s32_Result = C_NO_ERR;
      (void)std::memcpy(&orau8_AesKey[0], C_OscSecurityEcdhAes::mpu8_AesKey, hu32_AES_KEY_LENGTH);
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Extract public key

   Get public key in compressed format.

   \param[out]       orau8_PublicKey          Public key

   \retval  C_NO_ERR    key available
   \retval  C_NOACT     error trying to extract key
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdhAes::m_ExtractCompressedPublicKey(uint8_t (&orau8_PublicKey)[hu32_PUBLIC_KEY_LENGTH]) const
{
   int32_t s32_Result = C_NOACT;

   if (mpc_TheKey != NULL)
   {
      size_t x_KeyLength; //lint !e8080 //using type to match library interface
      //We did configure compressed format at initialization already. So just grab the data.
      const int x_Result = //lint !e970 !e8080 //using type to match library interface
                           EVP_PKEY_get_octet_string_param(mpc_TheKey, OSSL_PKEY_PARAM_PUB_KEY, &orau8_PublicKey[0],
                                                           hu32_PUBLIC_KEY_LENGTH, &x_KeyLength);
      if ((x_Result == 1) && (x_KeyLength == hu32_PUBLIC_KEY_LENGTH))
      {
         //sizes other than 33 should not happen; a compressed key should always be 33 bytes
         s32_Result = C_NO_ERR;
      }
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create OpenSSL EVP_PKEY from raw RC public key

   Note on usage of deprecated OpenSSL functions:
   The new approach would be to use the "EVP_PKEY_fromdata" API.
   This fails with "error:03000096:digital envelope routines::operation not supported for this keytype"
    (same with public and private keys).
   So constructing an EC key from binary data using this API is not supported with our version of OpenSSL (and possibly
    also versions targets might use).
   So better to use the "old" API until we can rely on OpenSSL providing the functionality with the new API.

   \param[in]    orau8_PublicKey      Public key in compressed format (33 bytes of buffer)

   \retval  NULL     could not create key
   \retval  C_NOACT  created key (needs to be EVP_PKEY_free's by caller)
*/
//----------------------------------------------------------------------------------------------------------------------
EVP_PKEY * C_OscSecurityEcdhAes::mh_CreateEvpPkeyFromRawPublicKey(
   const uint8_t(&orau8_PublicKey)[hu32_PUBLIC_KEY_LENGTH])
{
   EVP_PKEY * pc_Pkey = NULL;
   EC_KEY * pc_EcKey = NULL;
   EC_POINT * pc_Point = NULL;
   int x_Result = -1; //lint !e970 !e8080 //using type to match library interface

   // Create group for prime256v1
   EC_GROUP * const pc_Group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);

   if (pc_Group != NULL)
   {
      pc_EcKey = EC_KEY_new();
   }
   if (pc_EcKey != NULL)
   {
      x_Result = EC_KEY_set_group(pc_EcKey, pc_Group);
   }
   if (x_Result == 1)
   {
      // Create EC_POINT for public key
      pc_Point = EC_POINT_new(pc_Group);
   }
   if (pc_Point != NULL)
   {
      x_Result = EC_POINT_oct2point(pc_Group, pc_Point, orau8_PublicKey, hu32_PUBLIC_KEY_LENGTH, NULL);
   }
   if (x_Result == 1)
   {
      x_Result = EC_KEY_set_public_key(pc_EcKey, pc_Point);
   }
   if (x_Result == 1)
   {
      // Create EVP_PKEY and assign EC_KEY
      pc_Pkey = EVP_PKEY_new();
   }
   if (pc_Pkey != NULL)
   {
      x_Result = EVP_PKEY_set1_EC_KEY(pc_Pkey, pc_EcKey);
      if (x_Result != 1)
      {
         EVP_PKEY_free(pc_Pkey);
         pc_Pkey = NULL;
      }
   }

   //no action if parameters are NULL
   EC_POINT_free(pc_Point);
   EC_KEY_free(pc_EcKey);
   EC_GROUP_free(pc_Group);

   return pc_Pkey;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create elliptic curve key pair with secp256r1 curve

   The created ECDH instance including the private key will be stored in the class instance for further operations.

   \param[out]    orau8_PublicKey          Public key in compressed format

   \retval  C_NO_ERR    keys available
   \retval  C_NOACT     error trying to perform operation
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdhAes::CreateEcKeys(uint8_t (&orau8_PublicKey)[hu32_PUBLIC_KEY_LENGTH])
{
   int32_t s32_Result = C_NOACT;

   if (mpc_TheKey != NULL)
   {
      EVP_PKEY_free(mpc_TheKey);
      mpc_TheKey = NULL;
   }
   if (mpc_TheKey == NULL)
   {
      EVP_PKEY_CTX * const pc_ParamContext = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);

      if (pc_ParamContext != NULL)
      {
         EVP_PKEY_CTX * pc_KeyContext = NULL;
         EVP_PKEY * pc_KeyParams = NULL;
         int x_Result = EVP_PKEY_paramgen_init(pc_ParamContext); //lint !e970 !e8080 //using type to match library
         if (x_Result == 1)
         {
            // select the curve
            x_Result = EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pc_ParamContext, NID_X9_62_prime256v1);
         }
         if (x_Result == 1)
         {
            // generate parameters object
            x_Result = EVP_PKEY_paramgen(pc_ParamContext, &pc_KeyParams);
         }
         EVP_PKEY_CTX_free(pc_ParamContext); //not needed any longer
         if ((x_Result == 1) && (pc_KeyParams != NULL))
         {
            // create key generation context
            pc_KeyContext = EVP_PKEY_CTX_new(pc_KeyParams, NULL);
            EVP_PKEY_free(pc_KeyParams);
         }
         if (pc_KeyContext != NULL)
         {
            x_Result = EVP_PKEY_keygen_init(pc_KeyContext);
            if (x_Result == 1)
            {
               // generate the keypair
               x_Result = EVP_PKEY_keygen(pc_KeyContext, &mpc_TheKey);
               EVP_PKEY_CTX_free(pc_KeyContext); //not needed any longer
            }
            if (x_Result == 1)
            {
               //we want to make sure we get the public key data in 33 byte compressed format when reading it
               // subsequently (for compact transfer to the server):
               x_Result = EVP_PKEY_set_utf8_string_param(mpc_TheKey,
                                                         OSSL_PKEY_PARAM_EC_POINT_CONVERSION_FORMAT,
                                                         OSSL_PKEY_EC_POINT_CONVERSION_FORMAT_COMPRESSED);
            }
            if ((x_Result == 1) && (mpc_TheKey != NULL))
            {
               //extract binary keys and remember:
               s32_Result = m_ExtractCompressedPublicKey(orau8_PublicKey);
            }
         }
      }
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Perform elliptic curve diffie hellman algorithm to create AES encryption key

   Can be called after creating a valid ECDH instance with CreateEcKeys.
   The resulting AES key will be stored in the corresponding class field.

   Steps:
   * perform ECDH algorithm to create a shared secret from the own private and other's public keys
   * use SHA-256 as a key derivation function (KDF) to create a hash value over the shared secret
   * return the first 16 bytes of the SHA-256 hash as an AES-KEY

   \param[in]   orau8_OthersPublicKey    Others public key

   \retval   C_NO_ERR   success, key stored
   \retval   C_NOACT    error occurred
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdhAes::DeriveAesKey(const uint8_t (&orau8_OthersPublicKey)[hu32_PUBLIC_KEY_LENGTH])
{
   int32_t s32_Result = C_NOACT;

   if (mpc_TheKey != NULL)
   {
      EVP_PKEY_CTX * const pc_DeriveContext = EVP_PKEY_CTX_new(mpc_TheKey, NULL);
      if (pc_DeriveContext != NULL)
      {
         size_t x_SharedSecredSize = 0; //lint !e8080 //using type to match library
         uint8_t au8_SharedSecret[32U];

         int x_Result = EVP_PKEY_derive_init(pc_DeriveContext); //lint !e970 !e8080 //using type to match library
         if (x_Result == 1)
         {
            // set up a key from the binary public key data
            EVP_PKEY * const pc_PeerKey = mh_CreateEvpPkeyFromRawPublicKey(orau8_OthersPublicKey);
            if (pc_PeerKey != NULL)
            {
               x_Result = EVP_PKEY_derive_set_peer(pc_DeriveContext, pc_PeerKey);
            }
            else
            {
               x_Result = -1;
            }
            EVP_PKEY_free(pc_PeerKey); //not needed any more
         }

         if (x_Result == 1)
         {
            // derive shared secret
            x_SharedSecredSize = sizeof(au8_SharedSecret);
            x_Result = EVP_PKEY_derive(pc_DeriveContext, &au8_SharedSecret[0], &x_SharedSecredSize);
         }
         EVP_PKEY_CTX_free(pc_DeriveContext);

         if (x_Result == 1)
         {
            uint8_t au8_Sha256Digest[SHA256_DIGEST_LENGTH];
            const uint8_t * const pu8_Value = SHA256(&au8_SharedSecret[0], x_SharedSecredSize, &au8_Sha256Digest[0]);

            if (pu8_Value != NULL)
            {
               if (this->mpu8_AesKey == NULL)
               {
                  //lint -e{9079} //casting is the correct approach to use the API
                  this->mpu8_AesKey = reinterpret_cast<uint8_t *>(OPENSSL_secure_malloc(hu32_AES_KEY_LENGTH));
               }
               if (this->mpu8_AesKey != NULL)
               {
                  //use first 16bytes as AES key:
                  (void)std::memcpy(this->mpu8_AesKey, &au8_Sha256Digest[0], hu32_AES_KEY_LENGTH);
                  s32_Result = C_NO_ERR;
               }
            }
            else
            {
               s32_Result  = C_NOACT;
            }
         }
      }
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Encrypt with AES-128

   Can be called after deriving a valid key with DeriveAesKey

   \param[in]   orau8_AesInitVector    AES init vector to use
   \param[in]   orc_Input              Input data
   \param[out]  orc_Output             Output data

   \retval   C_NO_ERR   encryption done
   \retval   else       encryption failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdhAes::AesEncrypt(const uint8_t (&orau8_AesInitVector)[C_OscSecurityAesCbc::hu32_IV_LENGTH],
                                         const std::vector<uint8_t> & orc_Input,
                                         std::vector<uint8_t> & orc_Output) const
{
   tgl_assert(this->mpu8_AesKey != NULL);
   const uint8_t(&orau8_Key)[C_OscSecurityAesCbc::hu32_IV_LENGTH] =
      reinterpret_cast<const uint8_t ( &)[C_OscSecurityAesCbc::hu32_IV_LENGTH]>(this->mpu8_AesKey[0]);
   return C_OscSecurityAesCbc::h_Encrypt(orau8_Key, orau8_AesInitVector, orc_Input, orc_Output);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Decrypt with AES-128

   Can be called after deriving a valid key with DeriveAesKey

   \param[in]   orau8_AesInitVector    AES init vector to use
   \param[in]   orc_Input              Input data
   \param[out]  orc_Output             Output data

   \retval   C_NO_ERR   decryption done
   \retval   else       decryption failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdhAes::AesDecrypt(const uint8_t (&orau8_AesInitVector)[C_OscSecurityAesCbc::hu32_IV_LENGTH],
                                         const std::vector<uint8_t> & orc_Input,
                                         std::vector<uint8_t> & orc_Output) const
{
   tgl_assert(this->mpu8_AesKey != NULL);
   const uint8_t(&orau8_Key)[C_OscSecurityAesCbc::hu32_IV_LENGTH] =
      reinterpret_cast<const uint8_t ( &)[C_OscSecurityAesCbc::hu32_IV_LENGTH]>(this->mpu8_AesKey[0]);
   return C_OscSecurityAesCbc::h_Decrypt(orau8_Key, orau8_AesInitVector, orc_Input, orc_Output);
}
