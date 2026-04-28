//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Access to ECDSA signature handling
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSECURITYECDSA_HPP
#define C_OSCSECURITYECDSA_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "C_SclString.hpp"

#include "openssl/evp.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSecurityEcdsa
{
private:
   EVP_MD_CTX * mpc_MdContext; //OpenSSL context for SHA digest

public:
   static const uint32_t hu32_SHA256_FINAL_LENGTH = 32U;
   static const uint32_t hu32_SECP256R1_PRIVATE_KEY_LENGTH = 32U;
   static const uint32_t hu32_SECP256R1_PUBLIC_KEY_LENGTH = 64U;         //concatenated x and y parts; each 32 bytes
   static const uint32_t hu32_SECP256R1_SIGNATURE_PART_LENGTH_MAX = 32U; //maximum number of bytes for r and s parts

   //We wrap up the signature in our own class.
   //Using this approach we allow the application to be flexible about how to serialize/store a signature.
   //Depending on the use case this might be done e.g. in string format, DER encoded, pure binary data, ...
   class C_Ecdsa256Signature
   {
   public:
      C_Ecdsa256Signature();

      uint8_t u8_NumBytesUsedRpart;                                //shows how many bytes of RPart are used
      uint8_t u8_NumBytesUsedSpart;                                //shows how many bytes of SPart are used
      uint8_t au8_Rpart[hu32_SECP256R1_SIGNATURE_PART_LENGTH_MAX]; //r part as binary
      uint8_t au8_Spart[hu32_SECP256R1_SIGNATURE_PART_LENGTH_MAX]; //s part as binary

      int32_t GetAsDerString(stw::scl::C_SclString & orc_Signature) const;
      int32_t SetFromDerString(const stw::scl::C_SclString & orc_Signature);
   };

   C_OscSecurityEcdsa();
   virtual ~C_OscSecurityEcdsa();

   static int32_t h_ExtractPublicKeyFromX509Certificate(const std::vector<uint8_t> &orc_X509,
                                                        uint8_t(&orau8_Binary)[hu32_SECP256R1_PUBLIC_KEY_LENGTH]);

   static int32_t h_CalcEcdsaSecp256r1Signature(const uint8_t(&orau8_Digest)[hu32_SHA256_FINAL_LENGTH],
                                                const uint8_t(&orau8_PrivateKey)[hu32_SECP256R1_PRIVATE_KEY_LENGTH],
                                                C_Ecdsa256Signature & orc_Signature,
                                                stw::scl::C_SclString & orc_ErrorMessage);

   static int32_t h_VerifyEcdsaSecp256r1Signature(const uint8_t(&orau8_PublicKey)[hu32_SECP256R1_PUBLIC_KEY_LENGTH],
                                                  const C_Ecdsa256Signature & orc_Signature,
                                                  const uint8_t(&orau8_Digest)[hu32_SHA256_FINAL_LENGTH],
                                                  bool & orq_Valid);

   //Utility functions for calculating SHA-2 256bits. The result is to be passed to the actual secp256r1 calculation.
   int32_t Sha256Init();
   int32_t Sha256Update(const uint8_t * const opu8_Data, const uint32_t ou32_NumBytes);
   int32_t Sha256GetDigest(uint8_t(&orau8_Digest)[hu32_SHA256_FINAL_LENGTH]);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
