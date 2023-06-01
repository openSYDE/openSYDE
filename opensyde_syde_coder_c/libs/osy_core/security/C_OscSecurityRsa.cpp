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
#include <openssl/rsa.h>
#include <openssl/pem.h>

#include "stwerrors.hpp"
#include "C_OscSecurityRsa.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint32_t C_OscSecurityRsa::mhu32_DEFAULT_BUFFER_SIZE = 1024;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSecurityRsa::C_OscSecurityRsa()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sign signature

   \param[in]      orc_PrivateKey         Private key in PKCS#8 format
   \param[in]      orc_Message            Message
   \param[out]     orc_EncryptedMessage   Encrypted message

   \return
   STW error codes

   \retval   C_NO_ERR   Message encrypted
   \retval   C_RANGE    Invalid key
   \retval   C_NOACT    Could not encrypt message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityRsa::h_SignSignature(const std::vector<uint8_t> & orc_PrivateKey,
                                          const std::vector<uint8_t> & orc_Message,
                                          std::vector<uint8_t> & orc_EncryptedMessage)
{
   int32_t s32_Retval = C_RANGE;
   const uint8_t * pu8_Data = &orc_PrivateKey[0];

   orc_EncryptedMessage.resize(C_OscSecurityRsa::mhu32_DEFAULT_BUFFER_SIZE);

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
         //Get RSA context from EVP_PKEY:
         RSA * const pc_Rsa = EVP_PKEY_get1_RSA(pc_EvpKey);
         EVP_PKEY_free(pc_EvpKey);
         s32_Retval = C_NOACT;

         if (pc_Rsa != NULL)
         {
            //Perform the actual encryption:
            const int x_ResultEncrypt = RSA_private_encrypt( //lint !e970 !e8080 //using type to match library interface
               static_cast<int>(orc_Message.size()),         //lint !e970 //using type to match library interface
               &orc_Message[0], &orc_EncryptedMessage[0], pc_Rsa,
               RSA_PKCS1_PADDING);
            RSA_free(pc_Rsa);

            orc_EncryptedMessage.resize(x_ResultEncrypt);
            if (x_ResultEncrypt != 0)
            {
               s32_Retval = C_NO_ERR;
            }
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Verify signature

   Checks whether an encrypted messages, decrypted with a public key matches an expected message.

   \param[in]   orc_PublicKey          Public key in X509 format
   \param[in]   orc_Message            Expected message
   \param[in]   orc_EncryptedMessage   Encrypted message
   \param[out]  orq_Valid              true: signature valid
                                       false: signature not valid

   \return
   STW error codes

   \retval   C_NO_ERR   Message decrypted
   \retval   C_RANGE    Invalid key
   \retval   C_NOACT    Could not decrypt message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityRsa::h_VerifySignature(const std::vector<uint8_t> & orc_PublicKey,
                                            const std::vector<uint8_t> & orc_Message,
                                            const std::vector<uint8_t> & orc_EncryptedMessage, bool & orq_Valid)
{
   int32_t s32_Retval = C_RANGE;

   //get RSA structure from binary key:
   const uint8_t * pu8_Data = &orc_PublicKey[0];

   orq_Valid = false;

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
         //Get RSA context from EVP_PKEY:
         RSA * const pc_Rsa = EVP_PKEY_get1_RSA(pc_EvpKey);
         EVP_PKEY_free(pc_EvpKey);
         s32_Retval = C_NOACT;

         if (pc_Rsa != NULL)
         {
            std::vector<uint8_t> c_DecryptedMessage;
            c_DecryptedMessage.resize(C_OscSecurityRsa::mhu32_DEFAULT_BUFFER_SIZE);

            //Perform the actual decryption:
            const int x_DecryptResult = //lint !e970 !e8080  //using type to match library interface
                                        RSA_public_decrypt(
               static_cast<int>(orc_EncryptedMessage.size()), //lint !e970 //using type to match library interface
               &orc_EncryptedMessage[0], &c_DecryptedMessage[0], pc_Rsa,
               RSA_PKCS1_PADDING);
            RSA_free(pc_Rsa);

            if (x_DecryptResult != 0)
            {
               s32_Retval = C_NO_ERR;
               c_DecryptedMessage.resize(x_DecryptResult);

               //compare decrypted messages with expected message:
               if (c_DecryptedMessage.size() == orc_Message.size())
               {
                  const int x_DiffResult = //lint !e970 !e8080 //using type to match library interface
                                           std::memcmp(&c_DecryptedMessage[0], &orc_Message[0], orc_Message.size());
                  if (x_DiffResult == 0)
                  {
                     orq_Valid = true; //we have a winner
                  }
               }
            }
         }
      }
   }

   return s32_Retval;
}
