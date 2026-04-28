//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle PEM content

   Handle PEM content

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <fstream>
#include <sstream>

#include "openssl/x509.h"
#include "openssl/x509v3.h"
#include "openssl/pem.h"

#include "TglFile.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_OscSecurityPemBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint32_t C_OscSecurityPemBase::mhu32_DEFAULT_BUFFER_SIZE = 2048U;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSecurityPemBase::C_OscSecurityPemBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSecurityPemBase::~C_OscSecurityPemBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSecurityPemBase::Clear()
{
   this->mc_KeyInfo.Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get key info

   \return
   Key info
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscSecurityPemKeyInfo & C_OscSecurityPemBase::GetKeyInfo() const
{
   return this->mc_KeyInfo;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get meta infos

   \return
   Meta infos
*/
//----------------------------------------------------------------------------------------------------------------------
const std::string & C_OscSecurityPemBase::GetMetaInfos() const
{
   return this->mc_MetaInfo;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load from file

   Will load information from PEM file using the following sequence:
   * load certificate information including public key
   * load meta information
   * load private key

   Results will be stored in mc_KeyInfo and mc_MetaInfo.

   \param[in]      orc_FileName        File name
   \param[in,out]  orc_ErrorMessage    Error message (does not include file name)

   \return
   STW error codes

   \retval   C_NO_ERR   Information extracted
   \retval   C_RANGE    File not found
   \retval   C_CONFIG   Invalid file content
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityPemBase::LoadFromFile(const std::string & orc_FileName, std::string & orc_ErrorMessage)
{
   int32_t s32_Retval = C_NO_ERR;

   this->Clear();

   if (TglFileExists(orc_FileName.c_str()))
   {
      stw::scl::C_SclString c_FileContent;
      C_OscUtils::h_FileToString(orc_FileName.c_str(), c_FileContent);
      {
         const std::string c_PemFileContent(c_FileContent.c_str());

         std::vector<uint8_t> c_BufferFile;
         c_BufferFile.resize(c_PemFileContent.size());
         memcpy(&c_BufferFile[0], c_PemFileContent.data(), c_PemFileContent.size());
         s32_Retval = this->m_ReadPublicKey(c_BufferFile, orc_ErrorMessage);
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = this->m_ReadMetaInfos(c_BufferFile, orc_ErrorMessage);
         }
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = this->m_ReadPrivateKey(c_BufferFile, orc_ErrorMessage);
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
      orc_ErrorMessage += "could not read file";
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Read public key

   Reads information from X509 certificate:
   * public key
   * certificate serial number
   * key usage flags
   * extended key usage flags

   Read information will be stored in mc_KeyInfo.

   \param[in]      orc_FileContent     File content
   \param[in,out]  orc_ErrorMessage    Error message

   \return
   STW error codes

   \retval   C_NO_ERR   Information extracted
   \retval   C_CONFIG   Invalid file content
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityPemBase::m_ReadPublicKey(const std::vector<uint8_t> & orc_FileContent,
                                              std::string & orc_ErrorMessage)
{
   int32_t s32_Retval = C_NO_ERR;

   const int x_ContentSize = static_cast<int>(orc_FileContent.size()); //lint !e970 !e8080 //use type expected by API
   BIO * const pc_PubKeyFile = BIO_new_mem_buf(&orc_FileContent[0], x_ContentSize);

   if (pc_PubKeyFile != NULL)
   {
      //Public key
      X509 * const pc_RsaPub = PEM_read_bio_X509(pc_PubKeyFile, NULL, NULL, NULL);
      if (pc_RsaPub != NULL)
      {
         std::vector<uint8_t> c_PubKeyTextDecoded;
         c_PubKeyTextDecoded.resize(C_OscSecurityPemBase::mhu32_DEFAULT_BUFFER_SIZE);
         {
            uint8_t * pu8_PubKeyTextDecodedPointer = &c_PubKeyTextDecoded[0];
            const uint32_t u32_PubKeyTextDecodedOpensslCount = i2d_X509(pc_RsaPub, &pu8_PubKeyTextDecodedPointer);
            c_PubKeyTextDecoded.resize(u32_PubKeyTextDecodedOpensslCount);
            this->mc_KeyInfo.SetX509CertificateData(c_PubKeyTextDecoded);
            if (u32_PubKeyTextDecodedOpensslCount > 0)
            {
               //Get flags
               C_OscSecurityPemKeyInfo::C_CertificateKeyUsageInformation c_Info;

               const uint32_t u32_ExtensionFlags = X509_get_extension_flags(pc_RsaPub);
               if ((u32_ExtensionFlags & static_cast<uint32_t>(EXFLAG_KUSAGE)) == EXFLAG_KUSAGE)
               {
                  //get key usage flags
                  const uint32_t u32_KeyUsage = X509_get_key_usage(pc_RsaPub);
                  c_Info.q_KeyUsageDefined = true;

                  if ((u32_KeyUsage & static_cast<uint32_t>(KU_DIGITAL_SIGNATURE)) == KU_DIGITAL_SIGNATURE)
                  {
                     c_Info.q_KeyUsageDigitalSignature = true;
                  }
                  if ((u32_KeyUsage & static_cast<uint32_t>(KU_NON_REPUDIATION)) == KU_NON_REPUDIATION)
                  {
                     c_Info.q_KeyUsageNonRepudiation = true;
                  }
                  if ((u32_KeyUsage & static_cast<uint32_t>(KU_KEY_ENCIPHERMENT)) == KU_KEY_ENCIPHERMENT)
                  {
                     c_Info.q_KeyUsageKeyEncipherment = true;
                  }
                  if ((u32_KeyUsage & static_cast<uint32_t>(KU_DATA_ENCIPHERMENT)) == KU_DATA_ENCIPHERMENT)
                  {
                     c_Info.q_KeyUsageDataEncipherment = true;
                  }
                  if ((u32_KeyUsage & static_cast<uint32_t>(KU_KEY_AGREEMENT)) == KU_KEY_AGREEMENT)
                  {
                     c_Info.q_KeyUsageKeyAgreement = true;
                  }
                  if ((u32_KeyUsage & static_cast<uint32_t>(KU_KEY_CERT_SIGN)) == KU_KEY_CERT_SIGN)
                  {
                     c_Info.q_KeyUsageKeyCertSign = true;
                  }
                  if ((u32_KeyUsage & static_cast<uint32_t>(KU_CRL_SIGN)) == KU_CRL_SIGN)
                  {
                     c_Info.q_KeyUsageCrlSign = true;
                  }
                  if ((u32_KeyUsage & static_cast<uint32_t>(KU_ENCIPHER_ONLY)) == KU_ENCIPHER_ONLY)
                  {
                     c_Info.q_KeyUsageEncipherOnly = true;
                  }
                  if ((u32_KeyUsage & static_cast<uint32_t>(KU_DECIPHER_ONLY)) == KU_DECIPHER_ONLY)
                  {
                     c_Info.q_KeyUsageDecipherOnly = true;
                  }
               }
               if ((u32_ExtensionFlags & static_cast<uint32_t>(EXFLAG_XKUSAGE)) == EXFLAG_XKUSAGE)
               {
                  //get extended key usage flags
                  const uint32_t u32_ExtendedKeyUsage = X509_get_extended_key_usage(pc_RsaPub);
                  c_Info.q_ExtendedKeyUsageDefined = true;

                  if ((u32_ExtendedKeyUsage & static_cast<uint32_t>(XKU_SSL_SERVER)) == XKU_SSL_SERVER)
                  {
                     c_Info.q_ExtendedKeyUsageServerAuth = true;
                  }
                  if ((u32_ExtendedKeyUsage & static_cast<uint32_t>(XKU_SSL_CLIENT)) == XKU_SSL_CLIENT)
                  {
                     c_Info.q_ExtendedKeyUsageClientAuth = true;
                  }
                  if ((u32_ExtendedKeyUsage & static_cast<uint32_t>(XKU_SMIME)) == XKU_SMIME)
                  {
                     c_Info.q_ExtendedKeyUsageEmailProtection = true;
                  }
                  if ((u32_ExtendedKeyUsage & static_cast<uint32_t>(XKU_CODE_SIGN)) == XKU_CODE_SIGN)
                  {
                     c_Info.q_ExtendedKeyUsageCodeSigning = true;
                  }
                  if ((u32_ExtendedKeyUsage & static_cast<uint32_t>(XKU_OCSP_SIGN)) == XKU_OCSP_SIGN)
                  {
                     c_Info.q_ExtendedKeyUsageOcspSigning = true;
                  }
                  if ((u32_ExtendedKeyUsage & static_cast<uint32_t>(XKU_TIMESTAMP)) == XKU_TIMESTAMP)
                  {
                     c_Info.q_ExtendedKeyUsageTimeStamping = true;
                  }
                  if ((u32_ExtendedKeyUsage & static_cast<uint32_t>(XKU_DVCS)) == XKU_DVCS)
                  {
                     c_Info.q_ExtendedKeyUsageDvcs = true;
                  }
                  if ((u32_ExtendedKeyUsage & static_cast<uint32_t>(XKU_ANYEKU)) == XKU_ANYEKU)
                  {
                     c_Info.q_ExtendedKeyUsageAnyExtendedKeyUsage = true;
                  }
               }
               mc_KeyInfo.SetKeyUsageInformation(c_Info);

               {
                  //Get serial number
                  ASN1_INTEGER * const pc_SerialNumberOpenssl = X509_get_serialNumber(pc_RsaPub);
                  if (pc_SerialNumberOpenssl != NULL)
                  {
                     std::vector<uint8_t> c_PubKeySerialNumber;
                     c_PubKeySerialNumber.resize(C_OscSecurityPemBase::mhu32_DEFAULT_BUFFER_SIZE);
                     {
                        uint8_t * pu8_SerialNumberPointer = &c_PubKeySerialNumber[0];
                        const uint32_t u32_SerialNumberCount = i2d_ASN1_INTEGER(pc_SerialNumberOpenssl,
                                                                                &pu8_SerialNumberPointer);
                        c_PubKeySerialNumber.resize(u32_SerialNumberCount);
                        if (u32_SerialNumberCount == 0)
                        {
                           s32_Retval = C_CONFIG;
                           orc_ErrorMessage = "Public key: could not convert serial number into bytes";
                        }
                        else
                        {
                           //Validate serial number
                           if ((c_PubKeySerialNumber.size() > 22UL) || (c_PubKeySerialNumber.size() < 3UL))
                           {
                              std::stringstream c_Stream;
                              c_Stream << c_PubKeySerialNumber.size();
                              s32_Retval = C_CONFIG;
                              orc_ErrorMessage = "Public key: serial number has invalid size, should be [3,22], is: " +
                                                 c_Stream.str();
                           }
                           else
                           {
                              //Check expected serial number format, for details see https://en.wikipedia.org/wiki/ASN.1
                              //1. byte: 0x2 == Type tag indicating integer
                              //2. byte: length of serial number
                              if ((c_PubKeySerialNumber[0UL] == 0x02U) &&
                                  (c_PubKeySerialNumber[1UL] == (c_PubKeySerialNumber.size() - 2UL)))
                              {
                                 //Erase first two elements
                                 c_PubKeySerialNumber.erase(c_PubKeySerialNumber.begin());
                                 c_PubKeySerialNumber.erase(c_PubKeySerialNumber.begin());

                                 //Strip leading zeroes for compatibility (see #100795 for details)
                                 while ((c_PubKeySerialNumber.size() > 0) && (c_PubKeySerialNumber[0] == 0x00U))
                                 {
                                    c_PubKeySerialNumber.erase(c_PubKeySerialNumber.begin());
                                 }

                                 //Add serial number
                                 this->mc_KeyInfo.SetCertificateSerialNumber(c_PubKeySerialNumber);
                              }
                              else
                              {
                                 s32_Retval = C_CONFIG;
                                 orc_ErrorMessage = "Public key: serial number format unknown";
                              }
                           }
                        }
                     }
                  }
                  else
                  {
                     s32_Retval = C_CONFIG;
                     orc_ErrorMessage = "Public key: could not get serial number";
                  }
               }
            }
            else
            {
               s32_Retval = C_CONFIG;
               orc_ErrorMessage = "Public key: could not convert X509 section into bytes";
            }
         }
         X509_free(pc_RsaPub);
      }
      else
      {
         s32_Retval = C_CONFIG;
         orc_ErrorMessage = "Public key: X509 section not found";
      }
      BIO_free(pc_PubKeyFile);
   }
   else
   {
      s32_Retval = C_CONFIG;
      orc_ErrorMessage = "Public key: could not read file content";
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Read meta infos

   \param[in]      orc_FileContent     File content
   \param[in,out]  orc_ErrorMessage    Error message

   \return
   STW error codes

   \retval   C_NO_ERR   Information extracted
   \retval   C_CONFIG   Invalid file content
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityPemBase::m_ReadMetaInfos(const std::vector<uint8_t> & orc_FileContent,
                                              std::string & orc_ErrorMessage)
{
   int32_t s32_Retval = C_NO_ERR;

   const int x_ContentSize = static_cast<int>(orc_FileContent.size()); //lint !e970 !e8080 //use type expected by API
   BIO * const pc_PubKeyFile = BIO_new_mem_buf(&orc_FileContent[0], x_ContentSize);

   if (pc_PubKeyFile != NULL)
   {
      //Public key
      X509 * const pc_RsaPub = PEM_read_bio_X509(pc_PubKeyFile, NULL, NULL, NULL);
      if (pc_RsaPub != NULL)
      {
         BIO * const pc_PrintBuffer = BIO_new(BIO_s_mem());
         if (pc_PrintBuffer != NULL)
         {
            if (X509_print(pc_PrintBuffer, pc_RsaPub) == 1)
            {
               BUF_MEM * pc_MemPtr;
               //lint -e{1924,9176} OpenSSL interface
               BIO_get_mem_ptr(pc_PrintBuffer, &pc_MemPtr); //lint !e970 OpenSSL interface
               if (pc_MemPtr != NULL)
               {
                  this->mc_MetaInfo = "";
                  for (uint32_t u32_It = 0UL; u32_It < pc_MemPtr->length; ++u32_It)
                  {
                     this->mc_MetaInfo += pc_MemPtr->data[u32_It];
                  }
               }
               else
               {
                  s32_Retval = C_CONFIG;
                  orc_ErrorMessage = "Public key: could not get mem pointer for buffer object";
               }
            }
            else
            {
               s32_Retval = C_CONFIG;
               orc_ErrorMessage = "Public key: could not print meta options";
            }
            BIO_free(pc_PrintBuffer);
         }
         else
         {
            s32_Retval = C_CONFIG;
            orc_ErrorMessage = "Public key: could not create buffer object";
         }
         X509_free(pc_RsaPub);
      }
      else
      {
         s32_Retval = C_CONFIG;
         orc_ErrorMessage = "Public key: X509 section not found";
      }
      BIO_free(pc_PubKeyFile);
   }
   else
   {
      s32_Retval = C_CONFIG;
      orc_ErrorMessage = "Public key: could not read file content";
   }
   return s32_Retval;
}
