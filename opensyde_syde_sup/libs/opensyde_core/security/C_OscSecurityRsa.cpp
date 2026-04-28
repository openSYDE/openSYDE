//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Access to RSA signature handling

   Access to RSA signature handling

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstring>

#include "openssl/x509.h"
#include "openssl/rsa.h"
#include "openssl/pem.h"

#include "stwerrors.hpp"
#include "C_OscSecurityRsa.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create signature

   Create signature over message data using private key.

   Formats:
   Key:
   * binary in PKCS#8 format
   Message:
   * binary data
   Signature:
   * binary data

   \param[in]      orc_PrivateKey     Private key in PKCS#8 format
   \param[in]      orc_Message        Message
   \param[out]     orc_Signature      Created signature

   \return
   STW error codes

   \retval   C_NO_ERR   Message encrypted
   \retval   C_RANGE    Invalid key; key or message have zero length
   \retval   C_NOACT    Could not encrypt message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityRsa::h_SignSignature(const std::vector<uint8_t> & orc_PrivateKey,
                                          const std::vector<uint8_t> & orc_Message,
                                          std::vector<uint8_t> & orc_Signature)
{
   int32_t s32_Retval = C_RANGE;

   if ((orc_PrivateKey.size() > 0) && (orc_Message.size() > 0))
   {
      const uint8_t * pu8_Data = &orc_PrivateKey[0];

      //Get private key information from PKCS#8 dump:
      PKCS8_PRIV_KEY_INFO * const pc_Key = d2i_PKCS8_PRIV_KEY_INFO(
         NULL, &pu8_Data,
         static_cast<long>(orc_PrivateKey.size())); //lint !e970 //using type to match library interface
      if (pc_Key != NULL)
      {
         //Convert PKCS#8 key to EVP_PKEY:
         EVP_PKEY * const pc_EvpKey = EVP_PKCS82PKEY(pc_Key);
         PKCS8_PRIV_KEY_INFO_free(pc_Key);

         if (pc_EvpKey != NULL)
         {
            s32_Retval = C_NOACT;
            EVP_PKEY_CTX * const pc_SignCtx = EVP_PKEY_CTX_new(pc_EvpKey, NULL);
            EVP_PKEY_free(pc_EvpKey);

            if (pc_SignCtx != NULL)
            {
               int x_Result; //lint !e970 !e8080 //using type to match library interface

               x_Result = EVP_PKEY_sign_init(pc_SignCtx);
               if (x_Result > 0)
               {
                  // Raw RSA PKCS#1 v1.5 operation on the passed message)
                  x_Result = EVP_PKEY_CTX_set_rsa_padding(pc_SignCtx, RSA_PKCS1_PADDING);
                  if (x_Result > 0)
                  {
                     size_t x_SignatureSize = 0; //lint !e8080  //using type to match library interface

                     //first call without output buffer to get size of required buffer:
                     x_Result = EVP_PKEY_sign(pc_SignCtx, NULL, &x_SignatureSize, &orc_Message[0], orc_Message.size());
                     if ((x_Result > 0) && (x_SignatureSize > 0))
                     {
                        //dimension buffer based on result of first call
                        orc_Signature.resize(x_SignatureSize);

                        //second call: compute the signature:
                        x_Result = EVP_PKEY_sign(pc_SignCtx, &orc_Signature[0], &x_SignatureSize,
                                                 &orc_Message[0], orc_Message.size());
                        if (x_Result > 0)
                        {
                           //this should really be the same value as before, but let's be defensive
                           orc_Signature.resize(x_SignatureSize);
                           s32_Retval = C_NO_ERR;
                        }
                        else
                        {
                           orc_Signature.clear();
                        }
                     }
                  }
               }
               EVP_PKEY_CTX_free(pc_SignCtx);
            }
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Verify signature

   Extracts data from specified signature using a public key.
   Compares extracted data to provided original data.

   Formats:
   Key:
   * is expected to be provided in X.509 format.
   * When looking at a .PEM file:
   ** effectively everything between the "BEGIN CERTIFICATE" and "END CERTIFICATE" lines converted from base64 to binary.
   ExpectedMessage:
   * binary data
   Signature:
   * binary data

   \param[in]   orc_PublicKey          Public key in X509 format
   \param[in]   orc_ExpectedMessage    Expected message to compare against
   \param[in]   orc_Signature          Signature to parse
   \param[out]  orq_Valid              true: expected message identical to data extracted from orc_EncryptedMessage
                                       false: not the thing above

   \return
   STW error codes

   \retval   C_NO_ERR   Operation done; check for result
   \retval   C_RANGE    Invalid key; key, message or encrypted message have zero length
   \retval   C_NOACT    Could not decrypt message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityRsa::h_VerifySignature(const std::vector<uint8_t> & orc_PublicKey,
                                            const std::vector<uint8_t> & orc_ExpectedMessage,
                                            const std::vector<uint8_t> & orc_Signature, bool & orq_Valid)
{
   int32_t s32_Retval = C_RANGE;

   orq_Valid = false;

   if ((orc_PublicKey.size() > 0) && (orc_ExpectedMessage.size() > 0) && (orc_Signature.size() > 0))
   {
      //get RSA structure from binary key:
      const uint8_t * pu8_Data = &orc_PublicKey[0];

      //Extract X509 data from binary key data:
      X509 * const pc_X509Data = d2i_X509(
         NULL, &pu8_Data,
         static_cast<long>(orc_PublicKey.size())); //lint !e970 //using type to match library interface
      if (pc_X509Data != NULL)
      {
         //Get key in EVP_PKEY format:
         EVP_PKEY * const pc_EvpKey = X509_get_pubkey(pc_X509Data);
         X509_free(pc_X509Data);

         if (pc_EvpKey != NULL)
         {
            EVP_PKEY_CTX * const pc_VerifyCtx = EVP_PKEY_CTX_new(pc_EvpKey, NULL);
            EVP_PKEY_free(pc_EvpKey);
            s32_Retval = C_NOACT;

            if (pc_VerifyCtx != NULL)
            {
               int x_Result; //lint !e970 !e8080 //using type to match library interface
               x_Result = EVP_PKEY_verify_recover_init(pc_VerifyCtx);
               if (x_Result > 0)
               {
                  // Raw RSA PKCS#1 v1.5 operation on the passed message
                  x_Result = EVP_PKEY_CTX_set_rsa_padding(pc_VerifyCtx, RSA_PKCS1_PADDING);
                  if (x_Result > 0)
                  {
                     //get original data from signed data
                     size_t x_DecryptedSize = 0; //lint !e8080  //using type to match library interface

                     //first call: get buffer size needed for result
                     x_Result = EVP_PKEY_verify_recover(pc_VerifyCtx, NULL, &x_DecryptedSize, &orc_Signature[0],
                                                        orc_Signature.size());
                     if (x_Result > 0)
                     {
                        std::vector<uint8_t> c_DecryptedMessage;

                        if (x_DecryptedSize > 0)
                        {
                           c_DecryptedMessage.resize(x_DecryptedSize);
                           //second call: get original data
                           x_Result = EVP_PKEY_verify_recover(pc_VerifyCtx, &c_DecryptedMessage[0], &x_DecryptedSize,
                                                              &orc_Signature[0], orc_Signature.size());
                           if (x_Result > 0)
                           {
                              s32_Retval = C_NO_ERR;
                              //this should really be the same value as before, but let's be defensive
                              c_DecryptedMessage.resize(x_DecryptedSize);

                              //compare original data with expected message:
                              if (c_DecryptedMessage.size() == orc_ExpectedMessage.size())
                              {
                                 const int x_DiffResult = //lint !e970 !e8080 //using type to match library interface
                                                          std::memcmp(&c_DecryptedMessage[0], &orc_ExpectedMessage[0],
                                                                      orc_ExpectedMessage.size());
                                 if (x_DiffResult == 0)
                                 {
                                    orq_Valid = true; //we have a winner
                                 }
                              }
                           }
                        }
                     }
                  }
               }
               EVP_PKEY_CTX_free(pc_VerifyCtx);
            }
         }
      }
   }

   return s32_Retval;
}
