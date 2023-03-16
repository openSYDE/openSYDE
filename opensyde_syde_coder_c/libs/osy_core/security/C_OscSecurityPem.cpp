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

#include <openssl/pem.h>

#include "TglFile.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_OscSecurityPem.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint32_t C_OscSecurityPem::mhu32_DEFAULT_BUFFER_SIZE = 2048;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSecurityPem::C_OscSecurityPem()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSecurityPem::Clear()
{
   this->mc_KeyInfo.Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get key info

   \return
   Key info
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscSecurityPemKeyInfo & C_OscSecurityPem::GetKeyInfo() const
{
   return this->mc_KeyInfo;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get meta infos

   \return
   Meta infos
*/
//----------------------------------------------------------------------------------------------------------------------
const std::string & C_OscSecurityPem::GetMetaInfos() const
{
   return this->mc_MetaInfo;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load from file

   \param[in]      orc_FileName        File name
   \param[in,out]  orc_ErrorMessage    Error message (does not include file name)

   \return
   STW error codes

   \retval   C_NO_ERR   Information extracted
   \retval   C_RANGE    File not found
   \retval   C_CONFIG   Invalid file content
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityPem::LoadFromFile(const std::string & orc_FileName, std::string & orc_ErrorMessage)
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
/*! \brief  Read modulus and exponent from file

   \param[in]      orc_FileName        File name
   \param[in,out]  orc_Modulus         Modulus
   \param[in,out]  orc_Exponent        Exponent
   \param[in,out]  orc_ErrorMessage    Error message (does not include file name)

   \return
   STW error codes

   \retval   C_NO_ERR   Information extracted
   \retval   C_RANGE    File not found
   \retval   C_CONFIG   Invalid file content
   \retval   C_CHECKSUM Could not parse modulus and exponent from key
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityPem::h_ExtractModulusAndExponentFromFile(const std::string & orc_FileName,
                                                              std::vector<uint8_t> & orc_Modulus,
                                                              std::vector<uint8_t> & orc_Exponent,
                                                              std::string & orc_ErrorMessage)
{
   C_OscSecurityPem c_Pem;
   int32_t s32_Retval = c_Pem.LoadFromFile(orc_FileName, orc_ErrorMessage);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OscSecurityPem::h_ExtractModulusAndExponent(c_Pem.GetKeyInfo().GetPubKeyTextDecoded(),
                                                                 orc_Modulus, orc_Exponent, orc_ErrorMessage);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Read modulus and exponent

   \param[in]      orc_PubKeyTextDecoded  Public key text decoded
   \param[in,out]  orc_Modulus            Modulus
   \param[in,out]  orc_Exponent           Exponent
   \param[in,out]  orc_ErrorMessage       Error message

   \return
   STW error codes

   \retval   C_NO_ERR   Information extracted
   \retval   C_CHECKSUM Could not parse modulus and exponent from key
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityPem::h_ExtractModulusAndExponent(const std::vector<uint8_t> & orc_PubKeyTextDecoded,
                                                      std::vector<uint8_t> & orc_Modulus,
                                                      std::vector<uint8_t> & orc_Exponent,
                                                      std::string & orc_ErrorMessage)
{
   int32_t s32_Retval = C_NO_ERR;
   X509 * pc_X509Key = X509_new();
   const uint8_t * pu8_DataPointer = &orc_PubKeyTextDecoded[0];

   const long x_DecodedSize = static_cast<long>(orc_PubKeyTextDecoded.size()); //lint !e970 !e8080 //use API type

   pc_X509Key = d2i_X509(&pc_X509Key, &pu8_DataPointer, x_DecodedSize);
   if (pc_X509Key != NULL)
   {
      EVP_PKEY * const pc_PubKey = X509_get0_pubkey(pc_X509Key);
      if (pc_PubKey != NULL)
      {
         const RSA * const pc_RsaKey = EVP_PKEY_get0_RSA(pc_PubKey);
         if (pc_RsaKey != NULL)
         {
            const BIGNUM * const pc_Modulus = RSA_get0_n(pc_RsaKey);
            const BIGNUM * const pc_Exponent = RSA_get0_e(pc_RsaKey);
            orc_Modulus.resize(C_OscSecurityPem::mhu32_DEFAULT_BUFFER_SIZE);
            orc_Exponent.resize(C_OscSecurityPem::mhu32_DEFAULT_BUFFER_SIZE);
            {
               const int32_t s32_SizeModulus = BN_bn2bin(pc_Modulus, &orc_Modulus[0]);
               const int32_t s32_SizeExponent = BN_bn2bin(pc_Exponent, &orc_Exponent[0]);
               if ((s32_SizeModulus > 0) && (s32_SizeExponent > 0))
               {
                  orc_Modulus.resize(s32_SizeModulus);
                  orc_Exponent.resize(s32_SizeExponent);
               }
               else
               {
                  s32_Retval = C_CHECKSUM;
                  orc_ErrorMessage = "Could not read modulus or exponent";
               }
            }
         }
         else
         {
            s32_Retval = C_CHECKSUM;
            orc_ErrorMessage = "Could not get RSA part from public key";
         }
      }
      else
      {
         s32_Retval = C_CHECKSUM;
         orc_ErrorMessage = "Could not get public key from X509 input";
      }
      X509_free(pc_X509Key);
   }
   else
   {
      s32_Retval = C_CHECKSUM;
      orc_ErrorMessage = "Could not read X509 input";
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Read public key

   \param[in]      orc_FileContent     File content
   \param[in,out]  orc_ErrorMessage    Error message

   \return
   STW error codes

   \retval   C_NO_ERR   Information extracted
   \retval   C_CONFIG   Invalid file content
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityPem::m_ReadPublicKey(const std::vector<uint8_t> & orc_FileContent, std::string & orc_ErrorMessage)
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
         c_PubKeyTextDecoded.resize(C_OscSecurityPem::mhu32_DEFAULT_BUFFER_SIZE);
         {
            uint8_t * pu8_PubKeyTextDecodedPointer = &c_PubKeyTextDecoded[0];
            const uint32_t u32_PubKeyTextDecodedOpensslCount = i2d_X509(pc_RsaPub, &pu8_PubKeyTextDecodedPointer);
            c_PubKeyTextDecoded.resize(u32_PubKeyTextDecodedOpensslCount);
            this->mc_KeyInfo.SetPubKeyTextDecoded(c_PubKeyTextDecoded);
            if (u32_PubKeyTextDecodedOpensslCount > 0)
            {
               //Serial number
               ASN1_INTEGER * const pc_SerialNumberOpenssl = X509_get_serialNumber(pc_RsaPub);
               if (pc_SerialNumberOpenssl != NULL)
               {
                  std::vector<uint8_t> c_PubKeySerialNumber;
                  c_PubKeySerialNumber.resize(C_OscSecurityPem::mhu32_DEFAULT_BUFFER_SIZE);
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
                              //Erase first two elments
                              c_PubKeySerialNumber.erase(c_PubKeySerialNumber.begin());
                              c_PubKeySerialNumber.erase(c_PubKeySerialNumber.begin());
                              //Add serial number
                              this->mc_KeyInfo.SetPubKeySerialNumber(c_PubKeySerialNumber);
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
int32_t C_OscSecurityPem::m_ReadMetaInfos(const std::vector<uint8_t> & orc_FileContent, std::string & orc_ErrorMessage)
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Read private key

   \param[in]      orc_FileContent     File content
   \param[in,out]  orc_ErrorMessage    Error message

   \return
   STW error codes

   \retval   C_NO_ERR   Information extracted
   \retval   C_CONFIG   Invalid file content
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityPem::m_ReadPrivateKey(const std::vector<uint8_t> & orc_FileContent, std::string & orc_ErrorMessage)
{
   int32_t s32_Retval = C_NO_ERR;
   //Private key
   const int x_ContentSize = static_cast<int>(orc_FileContent.size()); //lint !e970 !e8080 //use type expected by API
   BIO * const pc_PrivKeyFile = BIO_new_mem_buf(&orc_FileContent[0], x_ContentSize);

   if (pc_PrivKeyFile != NULL)
   {
      PKCS8_PRIV_KEY_INFO * const pc_RsaPriv = PEM_read_bio_PKCS8_PRIV_KEY_INFO(pc_PrivKeyFile, NULL, NULL,
                                                                                NULL);
      if (pc_RsaPriv != NULL)
      {
         std::vector<uint8_t> c_PrivKeyTextDecoded;
         c_PrivKeyTextDecoded.resize(C_OscSecurityPem::mhu32_DEFAULT_BUFFER_SIZE);
         {
            uint8_t * pu8_PrivKeyTextPointer = &c_PrivKeyTextDecoded[0];
            const uint32_t u32_PrivKeyTextCount = i2d_PKCS8_PRIV_KEY_INFO(pc_RsaPriv, &pu8_PrivKeyTextPointer);
            c_PrivKeyTextDecoded.resize(u32_PrivKeyTextCount);
            this->mc_KeyInfo.SetPrivKeyTextDecoded(c_PrivKeyTextDecoded);
            if (u32_PrivKeyTextCount == 0)
            {
               s32_Retval = C_CONFIG;
               orc_ErrorMessage = "Private key: could not convert private key section into bytes";
            }
         }
         PKCS8_PRIV_KEY_INFO_free(pc_RsaPriv);
      }
      else
      {
         //No error as private key part is only sometimes present
      }
      BIO_free(pc_PrivKeyFile);
   }
   else
   {
      s32_Retval = C_CONFIG;
      orc_ErrorMessage = "Private key: could not read file content";
   }
   return s32_Retval;
}
