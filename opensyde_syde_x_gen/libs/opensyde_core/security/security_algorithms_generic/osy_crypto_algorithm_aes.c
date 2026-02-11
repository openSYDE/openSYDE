//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       AES-128-CBC encryption/decryption algorithm

   Software implementation of algorithm using OpenSSL:
   * AES encryption/decryption
   * using 128 keys / block size
   * CBC mode used
   * PKCS#7 padding/unpadding applied before encryption / after decryption

   Note that the used OpenSSL is most likely not available on bare metal targets, but a HW module usually is.

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#define OPENSSL_NO_DEPRECATED //no deprecated APIs (also prevents an annoying QA-C message regarding header internals)
#include <openssl/evp.h>

#include "stwtypes.h"
#include "stwerrors.h"

#include "osy_crypto_algorithm_aes.h"

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Encrypt with AES-128-CBC

   The caller is responsible to provide valid pointers.
   Specifically it must provide a data buffer rounded up to the next 16byte boundary as the function will pad to that
    boundary.

   \param[in]        opu8_Key                 16byte AES key
   \param[in]        opu8_InitVector          16byte CBC init vector
   \param[in,out]    opu8_Data                in: data to encrypt
                                              out: encrypted data (padded to 16bytes)
   \param[in,out]    opu16_NumBytes           in: number of bytes of opu8_Data used before padding and encryption
                                              out: number of bytes of opu8_Data used after padding and encryption

   \retval  C_NO_ERR    data encrypted
   \retval  C_NOACT     error trying to perform operation
*/
//----------------------------------------------------------------------------------------------------------------------
sint16 osy_cry_alg_aes_128_cbc_encrypt(const uint8 opu8_Key[OSY_CRY_ALG_AES_SIZE_AES_KEY],
                                       const uint8 opu8_InitVector[OSY_CRY_ALG_AES_SIZE_INIT_VECTOR],
                                       uint8 * const opu8_Data, uint16 * const opu16_NumBytes)
{
   sint16 s16_Result = C_NOACT;

   //create context:
   EVP_CIPHER_CTX * const pt_Context = EVP_CIPHER_CTX_new();

   if (pt_Context != NULL)
   {
      // Initialize encryption operation with AES-128-CBC
      sintn sn_Result = EVP_EncryptInit(pt_Context, EVP_aes_128_cbc(), opu8_Key, opu8_InitVector);
      if (sn_Result == 1)
      {
         // Function works inplace, so provide the same buffer for input and output:
         sintn sn_OutLen = 0;
         sn_Result = EVP_EncryptUpdate(pt_Context, opu8_Data, &sn_OutLen, opu8_Data, (sintn)(*opu16_NumBytes));

         if (sn_Result == 1)
         {
            // Finalize encryption (handles padding)
            // 2nd parameter: target for final block of encrypted (and possibly padded) data
            (*opu16_NumBytes) = (uint16)sn_OutLen;

            sn_Result = EVP_EncryptFinal_ex(pt_Context, &opu8_Data[sn_OutLen], &sn_OutLen);
            if (sn_Result == 1)
            {
               s16_Result = C_NO_ERR;
               (*opu16_NumBytes) += (uint16)sn_OutLen;
            }
         }
      }

      EVP_CIPHER_CTX_free(pt_Context);
   }

   return s16_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Decrypt with AES-128-CBC

   The caller is responsible to provide valid pointers and a valid input value for (*opu16_NumBytes).

   \param[in]        opu8_Key                 16byte AES key
   \param[in]        opu8_InitVector          16byte CBC init vector
   \param[in,out]    opu8_Data                in: data to decrypt
                                              out: decrypted data
   \param[in,out]    opu16_NumBytes           in: number of bytes in opu8_Data (must be multiple of 16)
                                              out: number of bytes of opu8_Data used after decryption and unpadding

   \retval  C_NO_ERR    data decrypted
   \retval  C_NOACT     error trying to perform operation
*/
//----------------------------------------------------------------------------------------------------------------------
sint16 osy_cry_alg_aes_128_cbc_decrypt(const uint8 opu8_Key[OSY_CRY_ALG_AES_SIZE_AES_KEY],
                                       const uint8 opu8_InitVector[OSY_CRY_ALG_AES_SIZE_INIT_VECTOR],
                                       uint8 * const opu8_Data, uint16 * const opu16_NumBytes)
{
   sint16 s16_Result = C_NOACT;

   //create context:
   EVP_CIPHER_CTX * const pt_Context = EVP_CIPHER_CTX_new();

   if (pt_Context != NULL)
   {
      // Initialize encryption operation with AES-128-CBC
      sintn sn_Result = EVP_DecryptInit(pt_Context, EVP_aes_128_cbc(), opu8_Key, opu8_InitVector);
      if (sn_Result == 1)
      {
         // Function works inplace, so provide the same buffer for input and output:
         sintn sn_OutLen = 0;
         // Provide the encrypted message to be decrypted
         sn_Result = EVP_DecryptUpdate(pt_Context, opu8_Data, &sn_OutLen, opu8_Data, (sintn)(*opu16_NumBytes));
         if (sn_Result == 1)
         {
            // Finalize encryption (handles padding)
            (*opu16_NumBytes) = (uint16)sn_OutLen;

            sn_Result = EVP_DecryptFinal_ex(pt_Context, &opu8_Data[sn_OutLen], &sn_OutLen);
            if (sn_Result == 1)
            {
               s16_Result = C_NO_ERR;
               (*opu16_NumBytes) += (uint16)sn_OutLen;
            }
         }

         // Clean up
         EVP_CIPHER_CTX_free(pt_Context);
      }
   }

   return s16_Result;
}
