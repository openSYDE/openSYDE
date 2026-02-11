//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Elliptic curve diffie hellman algorithms

   Software implementation of elliptic curve diffie hellman algorithms using secp256r1 curve.

   Features:
   * generate private and public ec keys
   * derive shared secret using ECDH with SHA-256 as KDF

   Note that the used OpenSSL is most likely not available on bare metal targets, but a HW module usually is.

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
/*STW Deviation Coding Rules v4*/
/*Violated Rule: no explicit rule*/
/*Circumstances: Library header includes same file twice.*/
/*Reason: Decision of that library.*/
/*Potential risks: Bad build performance; unexpected behavior.*/
/*Prevention of risks: Library was tested by supplier "as is"*/
/* PRQA S 0838 L1 */ //For unknown reasons "++" "--" syntax does not work here
#include <openssl/evp.h>
/* PRQA L:L1 */
#include <openssl/ec.h>
#include <openssl/core_names.h>
#include <openssl/sha.h>

#include "stwtypes.h"
#include "stwerrors.h"
#include "osy_crypto_algorithm_ecdh.h"

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
static uint8 mau8_PublicKey[OSY_CRY_ALG_ECDH_SIZE_PUBLIC_KEY]; //in compressed format
static uint8 mau8_PrivateKey[OSY_CRY_ALG_ECDH_SIZE_PRIVATE_KEY];

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */
static sint16 m_extract_compressed_public_key(const EVP_PKEY * const opt_Key);
static sint16 m_extract_private_key(const EVP_PKEY * const opt_Key);
static EVP_PKEY * m_create_evp_pkey_from_raw_pubkey(const uint8 opu8_PublicKey[OSY_CRY_ALG_ECDH_SIZE_PUBLIC_KEY]);
static EVP_PKEY * m_create_evp_pkey_from_raw_private_key(
   const uint8 opu8_PrivateKey[OSY_CRY_ALG_ECDH_SIZE_PRIVATE_KEY]);

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Extract public key

   Get public key in compressed format and store locally.

   \param[in]        pt_Key                   OpenSSL key structure

   \retval  C_NO_ERR    key available
   \retval  C_NOACT     error trying to extract key
*/
//----------------------------------------------------------------------------------------------------------------------
static sint16 m_extract_compressed_public_key(const EVP_PKEY * const opt_Key)
{
   size_t un_KeyLength;
   sint16 s16_Result = C_NOACT;

   //We did configure compressed format in osy_udc_trg_sec_get_ec_keys already. So just grab the data.
   const sintn sn_Result = EVP_PKEY_get_octet_string_param(opt_Key, OSSL_PKEY_PARAM_PUB_KEY, &mau8_PublicKey[0],
                                                           OSY_CRY_ALG_ECDH_SIZE_PUBLIC_KEY, &un_KeyLength);

   if ((sn_Result == 1) && (un_KeyLength == OSY_CRY_ALG_ECDH_SIZE_PUBLIC_KEY))
   {
      //sizes other than 33 should not happen; a compressed key should always be 33 bytes
      s16_Result = C_NO_ERR;
   }
   return s16_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Extract private key

   Get private key in binary format and store locally.

   \param[in]        pt_Key                   OpenSSL key structure

   \retval  C_NO_ERR    key available
   \retval  C_NOACT     error trying to extract key
*/
//----------------------------------------------------------------------------------------------------------------------
static sint16 m_extract_private_key(const EVP_PKEY * const opt_Key)
{
   sint16 s16_Result = C_NOACT;

   //Using deprecated API as the new EVP_PKEY_get_bn_param API does not support extraction of the private key (yet?)
   //See https://github.com/openssl/openssl/issues/16081
   const EC_KEY * const pt_EcKey = EVP_PKEY_get0_EC_KEY(opt_Key);

   if (pt_EcKey != NULL)
   {
      //We did configure compressed format in osy_udc_trg_sec_get_ec_keys already. So just grab the data.
      const BIGNUM * const pt_PrivateKeyBn = EC_KEY_get0_private_key(pt_EcKey);
      if (pt_PrivateKeyBn != NULL)
      {
         //The key can occasionally be smaller than 32 bytes (due to leading zeroes).
         //In this case BN_bn2bin will only copy over the bytes that are occupied.
         //Storage is big endian. So we need the leading zeroes on the left.
         //So fill in values "to the right" of the leading zeroes, memset leading zeroes explicitly

         /*STW Deviation Coding Rules v4*/
         /*Violated Rule: no explicit rule*/
         /*Circumstances: Using macro that does integer division*/
         /*Reason: macro is documented approach to using API*/
         /*Potential risks: Undefined behavior*/
         /*Prevention of risks: Macro is implemented with consideration of UB*/
         /*PRQA S 3103 1*/
         const sintn sn_NumBytes = BN_num_bytes(pt_PrivateKeyBn);
         //sanity check:
         if (sn_NumBytes <= (sintn)OSY_CRY_ALG_ECDH_SIZE_PRIVATE_KEY)
         {
            const sintn sn_OffsetToWriteTo = ((sintn)OSY_CRY_ALG_ECDH_SIZE_PRIVATE_KEY - sn_NumBytes);
            const sintn sn_CopiedLength = BN_bn2bin(pt_PrivateKeyBn, &mau8_PrivateKey[sn_OffsetToWriteTo]);
            if (sn_CopiedLength == sn_NumBytes)
            {
               sintn sn_LeadingZeroIndex;
               //set leading zeroes to zero:
               for (sn_LeadingZeroIndex = 0; sn_LeadingZeroIndex < sn_OffsetToWriteTo; sn_LeadingZeroIndex++)
               {
                  mau8_PrivateKey[sn_LeadingZeroIndex] = 0U;
               }
               s16_Result = C_NO_ERR;
            }
         }
      }
   }
   return s16_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create and provide elliptic curve key pair with secp256r1 curve

   Here: SW implementation using OpenSSL.
   The implementation is generic and can be used for ECDA key generation in general.

   The function implements a state machine to make sure the keys are only created once until the node is reset.
   But creation can be forces using the ou8_ForceCreation parameter.

   \param[in]     ou8_ForceCreation        0: regular operation; create key only once at first call
                                           1: force re-creation of key
   \param[out]    opu8_PublicKey           If NULL: nothing
                                           If not NULL: Public key in compressed format
                                             (caller must provide 33 bytes of buffer)
   \param[out]    opu8_PrivateKey          If NULL: nothing
                                           If not NULL: Private key
                                             (caller must provide 32 bytes of buffer)

   \retval  C_NO_ERR    keys available (and returned if requested)
   \retval  C_NOACT     error trying to perform operation
*/
//----------------------------------------------------------------------------------------------------------------------
sint16 osy_cry_alg_ecdh_get_ec_keys(const uint8 ou8_ForceCreation,
                                    uint8 opu8_PublicKey[OSY_CRY_ALG_ECDH_SIZE_PUBLIC_KEY],
                                    uint8 opu8_PrivateKey[OSY_CRY_ALG_ECDH_SIZE_PRIVATE_KEY])
{
   static uint8 hu8_KeysInitialized = 0U;
   sint16 s16_Result = C_NOACT;

   if ((hu8_KeysInitialized == 0U) || (ou8_ForceCreation == 1U))
   {
      EVP_PKEY_CTX * const pt_ParamContext = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
      if (pt_ParamContext != NULL)
      {
         EVP_PKEY_CTX * pt_KeyContext = NULL;
         EVP_PKEY * pt_KeyParams = NULL;
         sintn sn_Result = EVP_PKEY_paramgen_init(pt_ParamContext);
         if (sn_Result == 1)
         {
            // select the curve
            sn_Result = EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pt_ParamContext, NID_X9_62_prime256v1);
         }
         if (sn_Result == 1)
         {
            // generate parameters object
            sn_Result = EVP_PKEY_paramgen(pt_ParamContext, &pt_KeyParams);
         }
         EVP_PKEY_CTX_free(pt_ParamContext); //not needed any longer
         if ((sn_Result == 1) && (pt_KeyParams != NULL))
         {
            // create key generation context
            pt_KeyContext = EVP_PKEY_CTX_new(pt_KeyParams, NULL);
            EVP_PKEY_free(pt_KeyParams);
         }
         if (pt_KeyContext != NULL)
         {
            EVP_PKEY * pt_TheKey = NULL;

            sn_Result = EVP_PKEY_keygen_init(pt_KeyContext);
            if (sn_Result == 1)
            {
               // generate the keypair
               sn_Result = EVP_PKEY_keygen(pt_KeyContext, &pt_TheKey);
               EVP_PKEY_CTX_free(pt_KeyContext); //not needed any longer
            }
            if (sn_Result == 1)
            {
               //we want to make sure we get the public key data in 33 byte compressed format when reading it
               // subsequently (for compact transfer to the client):
               sn_Result = EVP_PKEY_set_utf8_string_param(pt_TheKey,
                                                          OSSL_PKEY_PARAM_EC_POINT_CONVERSION_FORMAT,
                                                          OSSL_PKEY_EC_POINT_CONVERSION_FORMAT_COMPRESSED);
            }
            if ((sn_Result == 1) && (pt_TheKey != NULL))
            {
               //extract binary keys and remember:
               s16_Result = m_extract_compressed_public_key(pt_TheKey);
               if (s16_Result == C_NO_ERR)
               {
                  s16_Result = m_extract_private_key(pt_TheKey);
               }
               if (s16_Result == C_NO_ERR)
               {
                  hu8_KeysInitialized = 1U;
                  s16_Result = C_NO_ERR;
               }
               EVP_PKEY_free(pt_TheKey);
            }
         }
      }
   }
   //return keys if initialized and desired by caller:
   if (hu8_KeysInitialized == 1U)
   {
      uint8 u8_Index;
      s16_Result = C_NO_ERR;
      if (opu8_PublicKey != NULL)
      {
         //use loop instead of memcpy to be independent of external libraries
         for (u8_Index = 0U; u8_Index < OSY_CRY_ALG_ECDH_SIZE_PUBLIC_KEY; u8_Index++)
         {
            opu8_PublicKey[u8_Index] = mau8_PublicKey[u8_Index];
         }
      }
      if (opu8_PrivateKey != NULL)
      {
         for (u8_Index = 0U; u8_Index < OSY_CRY_ALG_ECDH_SIZE_PRIVATE_KEY; u8_Index++)
         {
            opu8_PrivateKey[u8_Index] = mau8_PrivateKey[u8_Index];
         }
      }
   }
   return s16_Result;
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

   \param[in]    opu8_PublicKey      Public key in compressed format (33 bytes of buffer)

   \retval  NULL     could not create key
   \retval  C_NOACT  created key (needs to be EVP_PKEY_free's by caller)
*/
//----------------------------------------------------------------------------------------------------------------------
static EVP_PKEY * m_create_evp_pkey_from_raw_pubkey(const uint8 opu8_PublicKey[OSY_CRY_ALG_ECDH_SIZE_PUBLIC_KEY])
{
   EVP_PKEY * pt_Pkey = NULL;
   EC_KEY * pt_EcKey = NULL;
   EC_POINT * pt_Point = NULL;
   sintn sn_Result = -1;

   // Create group for prime256v1
   EC_GROUP * const pt_Group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);

   if (pt_Group != NULL)
   {
      pt_EcKey = EC_KEY_new();
   }
   if (pt_EcKey != NULL)
   {
      sn_Result = EC_KEY_set_group(pt_EcKey, pt_Group);
   }
   if (sn_Result == 1)
   {
      // Create EC_POINT for public key
      pt_Point = EC_POINT_new(pt_Group);
   }
   if (pt_Point != NULL)
   {
      sn_Result = EC_POINT_oct2point(pt_Group, pt_Point, opu8_PublicKey, OSY_CRY_ALG_ECDH_SIZE_PUBLIC_KEY, NULL);
   }
   if (sn_Result == 1)
   {
      sn_Result = EC_KEY_set_public_key(pt_EcKey, pt_Point);
   }
   if (sn_Result == 1)
   {
      // Create EVP_PKEY and assign EC_KEY
      pt_Pkey = EVP_PKEY_new();
   }
   if (pt_Pkey != NULL)
   {
      sn_Result = EVP_PKEY_set1_EC_KEY(pt_Pkey, pt_EcKey);
      if (sn_Result != 1)
      {
         EVP_PKEY_free(pt_Pkey);
         pt_Pkey = NULL;
      }
   }

   //no action if parameters are NULL
   EC_POINT_free(pt_Point);
   EC_KEY_free(pt_EcKey);
   EC_GROUP_free(pt_Group);

   return pt_Pkey;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create OpenSSL EVP_PKEY from raw RC private key

   See comment at m_create_evp_pkey_from_raw_pubkey regarding usage of deprecated functions.

   \param[in]    opu8_PrivateKey      Private key in binary format (32 bytes of buffer)

   \retval  NULL     could not create key
   \retval  C_NOACT  created key (needs to be EVP_PKEY_free's by caller)
*/
//----------------------------------------------------------------------------------------------------------------------
static EVP_PKEY * m_create_evp_pkey_from_raw_private_key(const uint8 opu8_PrivateKey[OSY_CRY_ALG_ECDH_SIZE_PRIVATE_KEY])
{
   EVP_PKEY * pt_Pkey = NULL;
   BIGNUM * pt_TheBigNum = NULL;
   sintn sn_Result = -1;

   // create group for prime256v1
   EC_KEY * const pt_EcKey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);

   if (pt_EcKey != NULL)
   {
      pt_TheBigNum = BN_bin2bn(opu8_PrivateKey, (sintn)OSY_CRY_ALG_ECDH_SIZE_PRIVATE_KEY, NULL);
   }
   if (pt_TheBigNum != NULL)
   {
      sn_Result = EC_KEY_set_private_key(pt_EcKey, pt_TheBigNum);
      BN_clear_free(pt_TheBigNum);
   }
   if (sn_Result == 1)
   {
      // create EVP_PKEY and assign EC_KEY
      pt_Pkey = EVP_PKEY_new();
   }
   if (pt_Pkey != NULL)
   {
      sn_Result = EVP_PKEY_set1_EC_KEY(pt_Pkey, pt_EcKey);
      if (sn_Result != 1)
      {
         EVP_PKEY_free(pt_Pkey);
         pt_Pkey = NULL;
      }
   }
   //no action if parameters are NULL
   EC_KEY_free(pt_EcKey);

   return pt_Pkey;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Perform elliptic curve diffie hellman algorithm to create AES encryption key

   Here: SW implementation using OpenSSL.

   Steps:
   * perform ECDH algorithm to create a shared secret from the private and public keys
   * use SHA-256 as a key derivation function (KDF) to create an hash value over the shared secret
   * return the first 16 bytes of the SHA-256 hash as an AES-KEY

   \param[in]    opu8_OthersPublicKey    Public key in compressed format (caller will provide 33 bytes of buffer)
   \param[in]    opu8_OwnPrivateKey      Private key in binary format (caller will provide 32 bytes of buffer)
   \param[out]   opu8_AesKey             Derived AES key (caller will provide 16 bytes of buffer)

   \retval  C_NO_ERR    derivation done; key is in opu8_AesKey
   \retval  C_NOACT     error trying to perform operation
*/
//----------------------------------------------------------------------------------------------------------------------
sint16 osy_cry_alg_ecdh_derive_aes_key(const uint8 opu8_OthersPublicKey[OSY_CRY_ALG_ECDH_SIZE_PUBLIC_KEY],
                                       const uint8 opu8_OwnPrivateKey[OSY_CRY_ALG_ECDH_SIZE_PRIVATE_KEY],
                                       uint8 opu8_AesKey[OSY_CRY_ALG_ECDH_SIZE_AES_KEY])
{
   sint16 s16_Result = C_NOACT;

   // set up private key from binary key data:
   EVP_PKEY * const pt_OwnKey = m_create_evp_pkey_from_raw_private_key(opu8_OwnPrivateKey);

   if (pt_OwnKey != NULL)
   {
      EVP_PKEY_CTX * const pt_DeriveContext = EVP_PKEY_CTX_new(pt_OwnKey, NULL);
      EVP_PKEY_free(pt_OwnKey); //not needed any more

      if (pt_DeriveContext != NULL)
      {
         size_t un_SharedSecredSize;
         uint8 au8_SharedSecret[32U];

         sintn sn_Result = EVP_PKEY_derive_init(pt_DeriveContext);
         if (sn_Result == 1)
         {
            // set up a key from the binary public key data
            EVP_PKEY * const pt_PeerKey = m_create_evp_pkey_from_raw_pubkey(opu8_OthersPublicKey);
            if (pt_PeerKey != NULL)
            {
               sn_Result = EVP_PKEY_derive_set_peer(pt_DeriveContext, pt_PeerKey);
            }
            else
            {
               sn_Result = -1;
            }
            EVP_PKEY_free(pt_PeerKey); //not needed any more
         }

         if (sn_Result == 1)
         {
            // derive shared secret
            un_SharedSecredSize = sizeof(au8_SharedSecret);
            sn_Result = EVP_PKEY_derive(pt_DeriveContext, &au8_SharedSecret[0], &un_SharedSecredSize);
         }
         EVP_PKEY_CTX_free(pt_DeriveContext);

         if (sn_Result == 1)
         {
            uint8 au8_Sha256Digest[SHA256_DIGEST_LENGTH];
            const uint8 * const pu8_Value = SHA256(&au8_SharedSecret[0], un_SharedSecredSize, &au8_Sha256Digest[0]);

            if (pu8_Value != NULL)
            {
               //use first 16bytes as AES key:
               //use loop instead of memcpy to be independent of external libraries
               uint8 u8_Index;
               for (u8_Index = 0U; u8_Index < OSY_CRY_ALG_ECDH_SIZE_AES_KEY; u8_Index++)
               {
                  opu8_AesKey[u8_Index] = au8_Sha256Digest[u8_Index];
               }

               s16_Result = C_NO_ERR;
            }
            else
            {
               s16_Result  = C_NOACT;
            }
         }
      }
   }
   return s16_Result;
}
