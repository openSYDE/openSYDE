//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Access to RSA signature handling

   Access to RSA signature handling

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <tomcrypt.h>

#include "stwerrors.h"
#include "C_OSCSecurityRsa.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::uint32 C_OSCSecurityRsa::mhu32_DEFAULT_BUFFER_SIZE = 1024;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
bool C_OSCSecurityRsa::mhq_IsInitialized = false;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */
//lint -e{526, 2701,8010,8047} //Required by libtomcrypt
extern const ltc_math_descriptor ltm_desc;

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCSecurityRsa::C_OSCSecurityRsa()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize RSA library
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSecurityRsa::h_Init()
{
   //Set math lib for libtomcrypt
   ltc_mp = ltm_desc;
   C_OSCSecurityRsa::mhq_IsInitialized = true;
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
sint32 C_OSCSecurityRsa::h_SignSignature(const std::vector<stw_types::uint8> & orc_PrivateKey,
                                         const std::vector<stw_types::uint8> & orc_Message,
                                         std::vector<stw_types::uint8> & orc_EncryptedMessage)
{
   sint32 s32_Retval = C_NO_ERR;

   if (C_OSCSecurityRsa::mhq_IsInitialized)
   {
      rsa_key c_PrivKeyRsa;

      try
      {
         if (rsa_import_pkcs8(&orc_PrivateKey[0], orc_PrivateKey.size(), NULL, 0, &c_PrivKeyRsa) == CRYPT_OK)
         {
            //ENCRYPTION
            orc_EncryptedMessage.resize(C_OSCSecurityRsa::mhu32_DEFAULT_BUFFER_SIZE);
            uint32 u32_EncryptedMessageCount = orc_EncryptedMessage.size();
            if (rsa_sign_hash_ex(&orc_Message[0], orc_Message.size(), &orc_EncryptedMessage[0],
                                 &u32_EncryptedMessageCount,
                                 static_cast<sintn>(LTC_PKCS_1_V1_5_NA1),
                                 NULL, 0, 0, 0, &c_PrivKeyRsa) == CRYPT_OK)
            {
               orc_EncryptedMessage.resize(u32_EncryptedMessageCount);
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
stw_types::sint32 C_OSCSecurityRsa::h_VerifySignature(const std::vector<stw_types::uint8> & orc_PublicKey,
                                                      const std::vector<stw_types::uint8> & orc_Message,
                                                      const std::vector<stw_types::uint8> & orc_EncryptedMessage,
                                                      bool & orq_Valid)
{
   sint32 s32_Retval = C_NO_ERR;

   if (C_OSCSecurityRsa::mhq_IsInitialized)
   {
      rsa_key c_PubKeyRsa;

      try
      {
         if (rsa_import_x509(&orc_PublicKey[0], orc_PublicKey.size(),
                             &c_PubKeyRsa) == CRYPT_OK)
         {
            sintn sn_IsValid;
            if (rsa_verify_hash_ex(&orc_EncryptedMessage[0], orc_EncryptedMessage.size(), &orc_Message[0],
                                   orc_Message.size(), static_cast<sintn>(LTC_PKCS_1_V1_5_NA1), 0, 0,
                                   &sn_IsValid, &c_PubKeyRsa) == CRYPT_OK)
            {
               if (sn_IsValid == 1)
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
