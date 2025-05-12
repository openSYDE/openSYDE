//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Access to ECDSA signature handling

   Access to ECDSA signature handling

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstring>

#include "openssl/x509.h"
#include "openssl/evp.h"
#include "openssl/ecdsa.h"
#include "openssl/pem.h"
#include "openssl/core_names.h"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscSecurityEcdsa.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::scl;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Clear elements.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSecurityEcdsa::C_Ecdsa256Signature::C_Ecdsa256Signature()
{
   this->u8_NumBytesUsedRpart = 0U;
   this->u8_NumBytesUsedSpart = 0U;
   (void)std::memset(&au8_Rpart[0], 0, hu32_SECP256R1_SIGNATURE_PART_LENGTH_MAX);
   (void)std::memset(&au8_Spart[0], 0, hu32_SECP256R1_SIGNATURE_PART_LENGTH_MAX);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert ECDSA signature to DER encoded string

   Convert the signature data to an DER encoded string.

   The bytes in the result will look like this:
   * one byte 0x30      "Sequence"
   * one byte <size_t>  Total number of bytes that will follow
   * one byte 0x02      "Integer"
   * one byte <size_r>  Size of the R part
   * <size_r> bytes     R part (with inserted leading zero padding if MSB of first byte is set)
   * one byte <size_s>  Size of the S part
   * <size_s> bytes     S part (with inserted leading zero padding if MSB of first byte is set)

   \param[out]  orc_Signature   string representation of DER encoded signature

   \retval   C_NO_ERR   No problems
   \retval   C_CONFIG   Held data not valid; conversion failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdsa::C_Ecdsa256Signature::GetAsDerString(C_SclString & orc_Signature) const
{
   int32_t s32_Return = C_CONFIG;

   orc_Signature = "";

   if ((this->u8_NumBytesUsedRpart != 0) && (this->u8_NumBytesUsedSpart != 0))
   {
      BIGNUM * const pc_Rpart = BN_bin2bn(&this->au8_Rpart[0], this->u8_NumBytesUsedRpart, NULL);
      BIGNUM * const pc_Spart = BN_bin2bn(&this->au8_Spart[0], this->u8_NumBytesUsedSpart, NULL);

      if ((pc_Rpart != NULL) && (pc_Spart != NULL))
      {
         ECDSA_SIG * const pc_Signature = ECDSA_SIG_new();
         if (pc_Signature != NULL)
         {
            //set signature from BIGNUMs:
            int x_Result = ECDSA_SIG_set0(pc_Signature, pc_Rpart, pc_Spart); //lint !e970 !e8080 //using type to
                                                                             // match library interface
            if (x_Result == 1)
            {
               //convert to string
               //first dry run to get size of required buffer:
               const int x_BufferSize = i2d_ECDSA_SIG(pc_Signature, NULL); //lint !e970 !e8080 //using type to
                                                                           // match library interface
               if (x_BufferSize > 0)
               {
                  uint8_t * pu8_Buffer = new uint8_t[x_BufferSize];
                  uint8_t * const pu8_OriginalBuffer = pu8_Buffer;

                  //now fill in the data:
                  x_Result = i2d_ECDSA_SIG(pc_Signature, &pu8_Buffer);
                  tgl_assert(x_Result == x_BufferSize);

                  //convert to string:
                  for (uint32_t u32_Character = 0U; u32_Character < static_cast<uint32_t>(x_BufferSize);
                       u32_Character++)
                  {
                     orc_Signature += C_SclString::IntToHex(pu8_OriginalBuffer[u32_Character], 2U);
                  }

                  delete[] pu8_OriginalBuffer;

                  s32_Return = C_NO_ERR;
               }
            }
            ECDSA_SIG_free(pc_Signature);
         }
         else
         {
            //only free manually if not done already indirectly via ECDSA_SIG_free
            BN_clear_free(pc_Rpart);
            BN_clear_free(pc_Spart);
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set signature from DER encoded string

   See comment in GetAsDerString for expected format.

   \param[in]  orc_Signature   string representation of DER encoded signature

   \retval   C_NO_ERR   No problems
   \retval   C_RANGE    No valid signature in string; conversion failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdsa::C_Ecdsa256Signature::SetFromDerString(const C_SclString & orc_Signature)
{
   int32_t s32_Return = C_RANGE;
   //maximum size for DER encoded: 72 bytes
   const uint32_t u32_MAX_LENGTH_DER_ENCODED_BYTES = 72U;

   //less than 7 bytes makes no sense
   //length must be even as two characters represent
   if ((orc_Signature.Length() > (7U * 2U)) &&
       (orc_Signature.Length() <= (u32_MAX_LENGTH_DER_ENCODED_BYTES * 2U)) &&
       ((orc_Signature.Length() % 2U) == 0U))
   {
      uint8_t au8_BinarySignature[u32_MAX_LENGTH_DER_ENCODED_BYTES];
      bool q_Error = false;

      const uint8_t * pu8_Data = &au8_BinarySignature[0];
      const uint8_t u8_NumBytes = static_cast<uint8_t>(orc_Signature.Length() / 2U);
      for (uint32_t u32_Byte = 0U; u32_Byte < u8_NumBytes; u32_Byte++)
      {
         try
         {
            au8_BinarySignature[u32_Byte] =
               static_cast<uint8_t>(("0x" + orc_Signature.SubString(1U + (u32_Byte * 2U), 2U)).ToInt());
         }
         catch (...)
         {
            q_Error = true;
            break;
         }
      }
      if (q_Error == false)
      {
         ECDSA_SIG * const pc_Signature = d2i_ECDSA_SIG(NULL, &pu8_Data, orc_Signature.Length());
         if (pc_Signature != NULL)
         {
            const BIGNUM * const pc_SignatureRpart = ECDSA_SIG_get0_r(pc_Signature);
            const BIGNUM * const pc_SignatureSpart = ECDSA_SIG_get0_s(pc_Signature);

            if ((pc_SignatureRpart != NULL) || (pc_SignatureSpart != NULL) ||
                (BN_num_bytes(pc_SignatureRpart) > 32) || (BN_num_bytes(pc_SignatureSpart) > 32))
            {
               //operation successful -> place signature into output array
               int x_Result = BN_bn2bin(pc_SignatureRpart, &this->au8_Rpart[0]); //lint !e970 !e8080 //using type to
                                                                                 // match library interface
               tgl_assert(x_Result == BN_num_bytes(pc_SignatureRpart));          //we already checked size above; this
                                                                                 // would
                                                                                 // be unexpected
               this->u8_NumBytesUsedRpart = static_cast<uint8_t>(x_Result);

               x_Result = BN_bn2bin(pc_SignatureSpart, &this->au8_Spart[0]);
               tgl_assert(x_Result == BN_num_bytes(pc_SignatureSpart)); //we already checked size above; this would
                                                                        // be unexpected
               this->u8_NumBytesUsedSpart = static_cast<uint8_t>(x_Result);
               s32_Return = C_NO_ERR;
            }
            ECDSA_SIG_free(pc_Signature);
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSecurityEcdsa::C_OscSecurityEcdsa() :
   mpc_MdContext(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Destructor

   Clean up
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSecurityEcdsa::~C_OscSecurityEcdsa()
{
   if (mpc_MdContext != NULL)
   {
      EVP_MD_CTX_free(mpc_MdContext);
      mpc_MdContext = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility to extract ESDSA public key from X509 certificate data

   Formats:
   orc_X509:
   * Is expected to be provided in X.509 format.
   * When looking at a .PEM file:
   ** effectively everything between the "BEGIN CERTIFICATE" and "END CERTIFICATE" lines converted from base64 to binary.

   orau8_Binary:
   * binary (concatenated "x" and "y" parts; 32 bytes each)

   The function will report an error if a key of the expected type and size cannot be extracted.

   \param[in]    orc_X509              certificate data
   \param[in]    orau8_Binary          public key in binary format

   \retval   C_NO_ERR   Conversion done
   \retval   C_RANGE    Could not parse key from certificate
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdsa::h_ExtractPublicKeyFromX509Certificate(const std::vector<uint8_t> & orc_X509, uint8_t(
                                                                     &orau8_Binary)[C_OscSecurityEcdsa::hu32_SECP256R1_PUBLIC_KEY_LENGTH])
{
   int32_t s32_Result = C_RANGE;

   if (orc_X509.size() != 0)
   {
      const uint8_t * pu8_KeyData = &orc_X509[0];
      //Convert the binary key data to X509 certificate format:
      X509 * const pc_X509Data = d2i_X509(NULL, &pu8_KeyData,
                                          static_cast<long>(orc_X509.size())); //lint !e970 //using type to match
                                                                               // library interface
      if (pc_X509Data != NULL)
      {
         //Get key in EVP_PKEY format:
         EVP_PKEY * const pc_EvpKey = X509_get_pubkey(pc_X509Data);
         X509_free(pc_X509Data);

         //check if the key is really an elliptic curve key
         if ((pc_EvpKey != NULL) && (EVP_PKEY_base_id(pc_EvpKey) == EVP_PKEY_EC))
         {
            //convert into EC KEY object
            EC_GROUP * const pc_EcGroup = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
            if (pc_EcGroup != NULL)
            {
               EC_POINT * const pc_PublicKey = EC_POINT_new(pc_EcGroup);
               if (pc_PublicKey != NULL)
               {
                  std::vector<uint8_t> c_PublicKeyOctets;
                  size_t x_NumBytesPublicKey; //lint !e970 !e8080 //use API type

                  //get public key as octet string
                  //Whether this is compressed or uncompressed is up to the OpenSSL library.
                  //So first ask the library about the required buffer size.
                  int x_ResultGetPublicKey; //lint !e970 !e8080 //use API type

                  x_ResultGetPublicKey = EVP_PKEY_get_octet_string_param(pc_EvpKey,
                                                                         OSSL_PKEY_PARAM_PUB_KEY,
                                                                         NULL,
                                                                         0,
                                                                         &x_NumBytesPublicKey);
                  if (x_ResultGetPublicKey == 1)
                  {
                     c_PublicKeyOctets.resize(x_NumBytesPublicKey);
                     x_ResultGetPublicKey =  EVP_PKEY_get_octet_string_param(pc_EvpKey,
                                                                             OSSL_PKEY_PARAM_PUB_KEY,
                                                                             &c_PublicKeyOctets[0],
                                                                             c_PublicKeyOctets.size(),
                                                                             &x_NumBytesPublicKey);
                  }
                  if ((x_ResultGetPublicKey == 1) && (x_NumBytesPublicKey == c_PublicKeyOctets.size()))
                  {
                     //convert from octet string to EC_POINT
                     const int x_ResultOct2Point = //lint !e970 !e8080 //use API type
                                                   EC_POINT_oct2point(pc_EcGroup, pc_PublicKey, &c_PublicKeyOctets[0],
                                                                      x_NumBytesPublicKey, NULL);
                     if (x_ResultOct2Point == 1)
                     {
                        BIGNUM * const pc_Xpart = BN_new();
                        BIGNUM * const pc_Ypart = BN_new();
                        //get unencoded x and y parts:
                        int x_Result = //lint !e970 !e8080 //use API type
                                       EC_POINT_get_affine_coordinates(
                           pc_EcGroup, pc_PublicKey, pc_Xpart, pc_Ypart,
                           NULL);
                        if ((x_Result == 1) && (BN_num_bytes(pc_Xpart) == 32) && (BN_num_bytes(pc_Ypart) == 32))
                        {
                           //put x and y into array:
                           x_Result = BN_bn2bin(pc_Xpart, &orau8_Binary[0]);
                           tgl_assert(x_Result == 32); //size already checked; this would be unexpected
                           x_Result = BN_bn2bin(pc_Ypart, &orau8_Binary[32]);
                           tgl_assert(x_Result == 32); //size already checked; this would be unexpected
                           s32_Result = C_NO_ERR;
                        }
                        BN_clear_free(pc_Xpart);
                        BN_clear_free(pc_Ypart);
                     }
                  }
                  EC_POINT_clear_free(pc_PublicKey);
               }
               EC_GROUP_free(pc_EcGroup);
            }
         }

         EVP_PKEY_free(pc_EvpKey);
      }
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates a ECDSA signature using a secp256r1 curve and a private key over a SHA encrypted input.

   The SHA encrypted input is a stream of data (see SHA256 utility functions in this class)

   We provide the hashing and signing steps in separate functions on purpose.
   Thus it is possible to create the hash (and therefore signature) over non-continuous data by streaming in
    multiple blocks into the SHA calculation one by one before calculating the signature over the final digest.

   Formats:
   Digest:
   * binary result of SHA256 algorithm
   Key:
   * binary (length must be 32 characters)
   Signature:
   * see structure for documentation

   \param[in]    orau8_Digest          Result of the SHA256 calculation of the data we want to sign
   \param[in]    orau8_PrivateKey      Private key
   \param[out]   orc_Signature         The signature over the digest.
   \param[out]   orc_ErrorMessage      Error feedback for caller

   \retval   C_NO_ERR   Signature successfully generated
   \retval   C_RANGE    Invalid key; technically this should not happen as we do not explicitly check the key content
   \retval   C_NOACT    Could not generate signature
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdsa::h_CalcEcdsaSecp256r1Signature(const uint8_t (&orau8_Digest)[hu32_SHA256_FINAL_LENGTH],
                                                          const uint8_t(&orau8_PrivateKey)[hu32_SECP256R1_PRIVATE_KEY_LENGTH], C_Ecdsa256Signature & orc_Signature,
                                                          stw::scl::C_SclString & orc_ErrorMessage)
{
   int32_t s32_Return = C_RANGE;

   //converting the binary data of private key back to openSSL internal structures
   BIGNUM * const pc_BigNum = BN_bin2bn(&orau8_PrivateKey[0], hu32_SECP256R1_PRIVATE_KEY_LENGTH, NULL);

   if (pc_BigNum != NULL)
   {
      //the key parameter for EC_KEY_set_private_key shall not be NULL so we first need a "dummy" key
      //the prime256v1 curve is equivalent to secp256r1, that has no specific NID in openssl library. See:
      //https://stackoverflow.com/questions/41950056/openssl1-1-0-b-is-not-support-secp256r1openssl-ecparam-list-curves
      EC_KEY * const pc_EcdsaKey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
      if (pc_EcdsaKey != NULL)
      {
         int x_Result = EC_KEY_set_private_key(pc_EcdsaKey, pc_BigNum); //lint !e970 !e8080 //using type to
         if (x_Result == 1)
         {
            //Checking for the validity of the private key is not so simple.
            //EC_KEY_check_key will fail as we have not explicitly set a public key.
            //Technically the private key must be in a certain range, but this is rather broad.
            //So we will not perform an additional check for validity.
            s32_Return = C_NOACT;

            BN_clear_free(pc_BigNum);

            //sign the digest
            ECDSA_SIG * const pc_Signature = ECDSA_do_sign(orau8_Digest, hu32_SHA256_FINAL_LENGTH, pc_EcdsaKey);
            EC_KEY_set_private_key(pc_EcdsaKey, NULL);
            EC_KEY_free(pc_EcdsaKey);

            if (pc_Signature != NULL)
            {
               const BIGNUM * const pc_SignatureRpart = ECDSA_SIG_get0_r(pc_Signature);
               const BIGNUM * const pc_SignatureSpart = ECDSA_SIG_get0_s(pc_Signature);

               if ((pc_SignatureRpart == NULL) || (pc_SignatureSpart == NULL) ||
                   (BN_num_bytes(pc_SignatureRpart) > 32) || (BN_num_bytes(pc_SignatureSpart) > 32))
               {
                  orc_ErrorMessage = "ECDSA signature size is greater than the expected maximum size.";
               }
               else
               {
                  //operation successful -> place signature into output array

                  x_Result = BN_bn2bin(pc_SignatureRpart, &orc_Signature.au8_Rpart[0]);
                  tgl_assert(x_Result == BN_num_bytes(pc_SignatureRpart)); //we already checked size above; this would
                                                                           // be unexpected
                  orc_Signature.u8_NumBytesUsedRpart = static_cast<uint8_t>(x_Result);

                  x_Result = BN_bn2bin(pc_SignatureSpart, &orc_Signature.au8_Spart[0]);
                  tgl_assert(x_Result == BN_num_bytes(pc_SignatureSpart)); //we already checked size above; this would
                                                                           // be unexpected
                  orc_Signature.u8_NumBytesUsedSpart = static_cast<uint8_t>(x_Result);
                  s32_Return = C_NO_ERR;
               }
               ECDSA_SIG_free(pc_Signature);
            }
            else
            {
               //could not sign the digest
               orc_ErrorMessage = "openSSL unable to sign the data. Check inputs: digest and private key.";
            }
         }
      }
   }
   else
   {
      //should not occur if pem file is valid AND contains private key
      orc_ErrorMessage = "Could not convert private key from data. Maybe check pem file.";
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks if an ECDSA signature that was calculated with a secp256r1 curve by the method
            h_CalcEcdsaSecp256r1Signature is valid

   Formats:
   Key:
   * binary (concatenated "x" and "y" parts; 32 bytes each)
   Signature:
   * see structure for documentation
   Digest:
   * binary result of SHA256 algorithm

   \param[in]      orau8_PublicKey  ECDSA public key
   \param[in]      orc_Signature    ECDSA signature
   \param[in]      orau8_Digest     Result of the SHA256 calculation of the data we did sign
   \param[out]     orq_Valid        true: signature valid
                                    false: signature not valid

   \retval   C_NO_ERR   Signature was checked (result in orq_Valid)
   \retval   C_RANGE    Invalid input
                        (key and/or signature in unexpected format)
   \retval   C_NOACT    Signature verification failed
                        (internal error returned by used security library; should not happen in regular use cases)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdsa::h_VerifyEcdsaSecp256r1Signature(
   const uint8_t(&orau8_PublicKey)[hu32_SECP256R1_PUBLIC_KEY_LENGTH], const C_Ecdsa256Signature & orc_Signature,
   const uint8_t (&orau8_Digest)[hu32_SHA256_FINAL_LENGTH], bool & orq_Valid)
{
   int32_t s32_Return = C_RANGE;

   //set default if caller did not
   orq_Valid = false;

   if ((orc_Signature.u8_NumBytesUsedRpart > 0U) && (orc_Signature.u8_NumBytesUsedSpart > 0U))
   {
      //convert binary signature data back to openSSL structure
      BIGNUM * const pc_Rpart = BN_bin2bn(&orc_Signature.au8_Rpart[0], orc_Signature.u8_NumBytesUsedRpart, NULL);
      BIGNUM * const pc_Spart = BN_bin2bn(&orc_Signature.au8_Spart[0], orc_Signature.u8_NumBytesUsedSpart, NULL);
      BIGNUM * const pc_Xpart = BN_bin2bn(&orau8_PublicKey[0], 32, NULL);
      BIGNUM * const pc_Ypart = BN_bin2bn(&orau8_PublicKey[32], 32, NULL);

      if ((pc_Rpart != NULL) && (pc_Spart != NULL) && (pc_Xpart != NULL) && (pc_Ypart != NULL))
      {
         ECDSA_SIG * const pc_Signature = ECDSA_SIG_new();
         if (pc_Signature != NULL)
         {
            int x_Result = ECDSA_SIG_set0(pc_Signature, pc_Rpart, pc_Spart); //lint !e970 !e8080 //using type to
                                                                             // match library interface
            if (x_Result == 1)
            {
               //create EC KEY object
               EC_KEY * const pc_EcdsaKey = EC_KEY_new();

               if (pc_EcdsaKey != NULL)
               {
                  EC_GROUP * const pc_EcGroup = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
                  if (pc_EcGroup != NULL)
                  {
                     x_Result = EC_KEY_set_group(pc_EcdsaKey, pc_EcGroup);
                     if (x_Result == 1)
                     {
                        //set public key from bignum data:
                        x_Result = EC_KEY_set_public_key_affine_coordinates(pc_EcdsaKey, pc_Xpart, pc_Ypart);
                     }
                     if (x_Result == 1)
                     {
                        //verify signature
                        x_Result = ECDSA_do_verify(&orau8_Digest[0],
                                                   hu32_SHA256_FINAL_LENGTH,
                                                   pc_Signature,
                                                   pc_EcdsaKey);
                        switch (x_Result)
                        {
                        case 0: //check done; result stays "false"
                           s32_Return = C_NO_ERR;
                           break;
                        case 1: //check done; result becomes "true"
                           s32_Return = C_NO_ERR;
                           orq_Valid = true;
                           break;
                        case -1: //function reports an error
                        default: //undefined function result
                           s32_Return = C_NOACT;
                           break;
                        }
                     }
                     EC_GROUP_free(pc_EcGroup);
                  }
                  EC_KEY_set_private_key(pc_EcdsaKey, NULL);
                  EC_KEY_free(pc_EcdsaKey);
               }
            }
            ECDSA_SIG_free(pc_Signature);
         }
         else
         {
            //only free R and S parts manually if they were not indirectly freed via ECDSA_SIG_free
            BN_clear_free(pc_Rpart);
            BN_clear_free(pc_Spart);
         }
         BN_clear_free(pc_Xpart);
         BN_clear_free(pc_Ypart);
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize SHA2 256bit engine

   Sequence of function calls to perform SHA2 calculation:
   * Sha256Init (once)
   * Sha256Update (at least once)
   * Sha256GetDigest (once)

   \retval   C_NO_ERR   Engine initialized
   \retval   C_NOACT    Initialization failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdsa::Sha256Init()
{
   int32_t s32_Return = C_NOACT;

   //defensive measure: if context was not finalized then clear first:
   if (mpc_MdContext != NULL)
   {
      EVP_MD_CTX_free(mpc_MdContext);
   }
   //get new context:
   mpc_MdContext = EVP_MD_CTX_new();
   //get algorithm definition:
   const EVP_MD * const pc_Algorithm = EVP_sha256();
   //initialize context:
   const int x_Result = EVP_DigestInit_ex(mpc_MdContext, pc_Algorithm, NULL); //lint !e8080 !e970  //using type to
                                                                              // match library interface
   if (x_Result == 1)
   {
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Stream data into SHA calculation

   As a result the context will be updated depending on the supplied data and previous context.

   \param[in]  opu8_Data        Data to calculate the SHA value over
   \param[in]  ou32_NumBytes    Size of memory referenced by opu8_Data in bytes

   \retval   C_NO_ERR   Data streamed into SHA calculation
   \retval   C_RANGE    opu8_Data is NULL
   \retval   C_CONFIG   Engine was not initialized with Sha256Init
   \retval   C_NOACT    Calculation failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdsa::Sha256Update(const uint8_t * const opu8_Data, const uint32_t ou32_NumBytes)
{
   int32_t s32_Return = C_RANGE;

   if (opu8_Data != NULL)
   {
      if (mpc_MdContext == NULL)
      {
         s32_Return = C_CONFIG;
      }
      else
      {
         //stream data into algorithm:
         const int x_Result = EVP_DigestUpdate(mpc_MdContext, opu8_Data, ou32_NumBytes); //lint !e8080 !e970  //using
                                                                                         // type to match library
                                                                                         // interface
         if (x_Result != 1)
         {
            s32_Return = C_NOACT;
         }
         else
         {
            s32_Return = C_NO_ERR;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get result of SHA-2 256bit calculation

   Returns the 256bit message digest calculated through calls of Sha256Update.

   \param[out]  orau8_Digest   Resulting digest value

   \retval   C_NO_ERR   No problems
   \retval   C_CONFIG   Engine was not initialized with Sha256Init
   \retval   C_NOACT    Getting digest value failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityEcdsa::Sha256GetDigest(uint8_t (&orau8_Digest)[hu32_SHA256_FINAL_LENGTH])
{
   int32_t s32_Return = C_NO_ERR;

   if (mpc_MdContext == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      unsigned int x_Size; //lint !e8080 !e970  //using type to match library interface
      //get final digest value:
      const int x_Result = EVP_DigestFinal_ex(mpc_MdContext,  orau8_Digest, &x_Size); //lint !e8080 !e970  //using type
                                                                                      // to match library interface
      if ((x_Result != 1) || (x_Size != hu32_SHA256_FINAL_LENGTH))
      {
         s32_Return = C_NOACT;
      }
      //clean up:
      EVP_MD_CTX_free(mpc_MdContext);
      mpc_MdContext = NULL;
   }

   return s32_Return;
}
