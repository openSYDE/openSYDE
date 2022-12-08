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

#include <tomcrypt.h>

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
bool C_OscSecurityRsa::mhq_IsInitialized = false;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */
//lint -e{526, 2701,8010,8047} //Required by libtomcrypt
extern const ltc_math_descriptor ltm_desc;

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSecurityRsa::C_OscSecurityRsa()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize RSA library
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSecurityRsa::h_Init()
{
   //Set math lib for libtomcrypt
   ltc_mp = ltm_desc;
   C_OscSecurityRsa::mhq_IsInitialized = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sign signature

   \param[in]      orc_PrivateKey         Private key
   \param[in,out]  orc_Message            Message
   \param[in,out]  orc_EncryptedMessage   Encrypted message

   \return
   STW error codes

   \retval   C_NO_ERR   Message encrypted
   \retval   C_RANGE    Invalid key
   \retval   C_CONFIG   Class uninitialized
   \retval   C_NOACT    Could not encrypt message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityRsa::h_SignSignature(const std::vector<uint8_t> & orc_PrivateKey,
                                          const std::vector<uint8_t> & orc_Message,
                                          std::vector<uint8_t> & orc_EncryptedMessage)
{
   int32_t s32_Retval = C_NO_ERR;

   if (C_OscSecurityRsa::mhq_IsInitialized)
   {
      rsa_key c_PrivKeyRsa;

      try
      {
         //using library type for API compatibility:
         const unsigned long x_KeySize = static_cast<unsigned long>(orc_PrivateKey.size()); //lint !e8080 !e970

         if (rsa_import_pkcs8(&orc_PrivateKey[0], x_KeySize, NULL, 0, &c_PrivKeyRsa) == CRYPT_OK)
         {
            //ENCRYPTION
            orc_EncryptedMessage.resize(C_OscSecurityRsa::mhu32_DEFAULT_BUFFER_SIZE);
            //lint -save -e970 -e8080 //using types to match library interface
            unsigned long x_EncryptedMessageCount = static_cast<unsigned long>(orc_EncryptedMessage.size());
            const unsigned long x_MessageSize = static_cast<unsigned long>(orc_Message.size());

            if (rsa_sign_hash_ex(&orc_Message[0], x_MessageSize, &orc_EncryptedMessage[0],
                                 &x_EncryptedMessageCount,
                                 static_cast<int>(LTC_PKCS_1_V1_5_NA1),
                                 NULL, 0, 0, 0, &c_PrivKeyRsa) == CRYPT_OK)
            //lint -restore
            {
               orc_EncryptedMessage.resize(x_EncryptedMessageCount);
            }
            else
            {
               s32_Retval = C_NOACT;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      catch (...)
      {
         s32_Retval = C_UNKNOWN_ERR;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Verify signature

   \param[in]   orc_PublicKey          Public key
   \param[in]   orc_Message            Message
   \param[in]   orc_EncryptedMessage   Encrypted message
   \param[out]  orq_Valid              Valid

   \return
   STW error codes

   \retval   C_NO_ERR   Message decrypted
   \retval   C_RANGE    Invalid key
   \retval   C_CONFIG   Class uninitialized
   \retval   C_NOACT    Could not decrypt message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityRsa::h_VerifySignature(const std::vector<uint8_t> & orc_PublicKey,
                                            const std::vector<uint8_t> & orc_Message,
                                            const std::vector<uint8_t> & orc_EncryptedMessage, bool & orq_Valid)
{
   int32_t s32_Retval = C_NO_ERR;

   if (C_OscSecurityRsa::mhq_IsInitialized)
   {
      rsa_key c_PubKeyRsa;

      try
      {
         //using type to match library interface
         const unsigned long x_KexSize = static_cast<unsigned long>(orc_PublicKey.size()); //lint !e8080 !e970
         if (rsa_import_x509(&orc_PublicKey[0], x_KexSize, &c_PubKeyRsa) == CRYPT_OK)
         {
            //lint -save -e970 -e8080 //using types to match library interface
            int x_IsValid;
            const unsigned long x_Message1Size = static_cast<unsigned long>(orc_EncryptedMessage.size());
            const unsigned long x_Message2Size = static_cast<unsigned long>(orc_Message.size());
            if (rsa_verify_hash_ex(&orc_EncryptedMessage[0], x_Message1Size, &orc_Message[0],
                                   x_Message2Size,
                                   static_cast<int>(LTC_PKCS_1_V1_5_NA1),
                                   0, 0,
                                   &x_IsValid, &c_PubKeyRsa) == CRYPT_OK)
            //lint -restore
            {
               if (x_IsValid == 1)
               {
                  orq_Valid = true;
               }
               else
               {
                  orq_Valid = false;
               }
            }
            else
            {
               s32_Retval = C_NOACT;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      catch (...)
      {
         s32_Retval = C_UNKNOWN_ERR;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}
