//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle content of PEM files for SecureUpdate feature

   Content and thus handling of SecureUpdate PEM files is slightly different than the PEM files of the
   SecureAuthentication feature.

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "openssl/pem.h"
#include "openssl/evp.h"

#include "stwerrors.hpp"
#include "stwtypes.hpp"
#include "C_OscUtils.hpp"
#include "TglFile.hpp"
#include "C_OscSecurityPemSecUpdate.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::errors;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSecurityPemSecUpdate::C_OscSecurityPemSecUpdate() :
   C_OscSecurityPemBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load from file

   Loads information from PEM file using the base class.
   Checks for specific conditions required for PEM files used for secure update:
   * key usage flag digitalSignature must be set
   * extended key usage flag id-kp-emailProtection must be set

   Results will be stored in mc_KeyInfo and mc_MetaInfo.

   \param[in]      orc_FileName        File name
   \param[in,out]  orc_ErrorMessage    Error message (does not include file name)

   \return
   STW error codes

   \retval   C_NO_ERR    Information extracted
   \retval   C_RANGE     File not found
   \retval   C_CONFIG    Invalid file content
   \retval   C_OVERFLOW  usage flags not set as expected
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityPemSecUpdate::LoadFromFile(const std::string & orc_FileName, std::string & orc_ErrorMessage)
{
   int32_t s32_Result = C_OscSecurityPemBase::LoadFromFile(orc_FileName, orc_ErrorMessage);

   if (s32_Result == C_NO_ERR)
   {
      const C_OscSecurityPemKeyInfo::C_CertificateKeyUsageInformation c_UsageInfo = mc_KeyInfo.GetKeyUsageInformation();
      if ((c_UsageInfo.q_KeyUsageDefined == false) || (c_UsageInfo.q_KeyUsageDigitalSignature == false) ||
          (c_UsageInfo.q_ExtendedKeyUsageDefined == false) || (c_UsageInfo.q_ExtendedKeyUsageEmailProtection == false))
      {
         s32_Result = C_OVERFLOW;
         orc_ErrorMessage = "Certificate: Key usage flags not as expected in PEM file used for secure update "
                            "configuration. Expected flags \"digitalSignature\" and \"id-kp-emailProtection\""
                            "to be set.";
      }
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Read private key

   Here: we expect an elliptic curve private key

   \param[in]      orc_FileContent     File content
   \param[in,out]  orc_ErrorMessage    Error message

   \return
   C_NO_ERR   Information extracted
   C_CONFIG   Invalid file content
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityPemSecUpdate::m_ReadPrivateKey(const std::vector<uint8_t> & orc_FileContent,
                                                    std::string & orc_ErrorMessage)
{
   int32_t s32_Retval = C_NO_ERR;

   const int x_ContentSize = static_cast<int>(orc_FileContent.size()); //lint !e970 !e8080 //use type expected by API
   //read pem file content into a BIO (= openSSL I/O stream)
   BIO * const pc_PrivKeyFile = BIO_new_mem_buf(&orc_FileContent[0], x_ContentSize);

   if (pc_PrivKeyFile != NULL)
   {
      //read the private key portion from the BIO
      EVP_PKEY * const pc_PrivKey = PEM_read_bio_PrivateKey(pc_PrivKeyFile, NULL, NULL, NULL);

      BIO_free(pc_PrivKeyFile);
      if (pc_PrivKey != NULL)
      {
         //extract the ECDSA key from the private key portion
         EC_KEY * const pc_EcdsaKey = EVP_PKEY_get1_EC_KEY(pc_PrivKey);
         //get the private key as BIGNUM (needed for later conversion)
         const BIGNUM * const pc_PrivBigNum = EC_KEY_get0_private_key(pc_EcdsaKey);

         if (pc_PrivBigNum != NULL)
         {
            const int x_Size = BN_num_bytes(pc_PrivBigNum); //lint !e970 !e8080 //use type expected by API
            std::vector<uint8_t> c_PrivKey(x_Size);

            //convert BIGNUM to byte array
            BN_bn2bin(pc_PrivBigNum, &c_PrivKey[0]);

            //write private key to our internal structure
            this->mc_KeyInfo.SetPrivateKey(c_PrivKey);

            EVP_PKEY_free(pc_PrivKey);
            EC_KEY_set_private_key(pc_EcdsaKey, NULL);
            EC_KEY_free(pc_EcdsaKey);
         }
      }
      else
      {
         //No error as private key part is only sometimes present, but info could be useful in layers above
         orc_ErrorMessage = "No private key present in given file.";
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
      orc_ErrorMessage = "Private key: could not read file content.";
   }

   return s32_Retval;
}
