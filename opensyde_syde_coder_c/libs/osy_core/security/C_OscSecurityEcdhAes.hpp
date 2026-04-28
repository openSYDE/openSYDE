//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core ECDH/AES encryption utility

   Software implementation of elliptic curve diffie hellman algorithms using secp256r1 curve.
   Uses OpenSSL

   Features:
   * generate private and public ec keys with secp256r1 curve
   * perform ECDH algorithm to derive AES key with SHA-256 as KDF
   * encrypt/decrypt using AES-CBC

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSECURITYECDHAES_HPP
#define C_OSCSECURITYECDHAES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <openssl/evp.h>
#include "stwtypes.hpp"
#include "C_OscSecurityAesCbc.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSecurityEcdhAes
{
public:
   C_OscSecurityEcdhAes();
   virtual ~C_OscSecurityEcdhAes();

   static const uint32_t hu32_PUBLIC_KEY_LENGTH = 33U;
   static const uint32_t hu32_PRIVATE_KEY_LENGTH = 32U;
   static const uint32_t hu32_AES_KEY_LENGTH = 16U;

   int32_t CreateEcKeys(uint8_t(&orau8_PublicKey)[hu32_PUBLIC_KEY_LENGTH]);
   int32_t DeriveAesKey(const uint8_t(&orau8_OthersPublicKey)[hu32_PUBLIC_KEY_LENGTH]);
   int32_t AesEncrypt(const uint8_t (&orau8_AesInitVector)[stw::opensyde_core::C_OscSecurityAesCbc::hu32_IV_LENGTH],
                      const std::vector<uint8_t> & orc_Input, std::vector<uint8_t> & orc_Output) const;
   int32_t AesDecrypt(const uint8_t (&orau8_AesInitVector)[stw::opensyde_core::C_OscSecurityAesCbc::hu32_IV_LENGTH],
                      const std::vector<uint8_t> & orc_Input, std::vector<uint8_t> & orc_Output) const;

   int32_t GetAesKey(uint8_t(&orau8_AesKey)[hu32_AES_KEY_LENGTH]) const;

protected:
   EVP_PKEY * mpc_TheKey;
   uint8_t * mpu8_AesKey;

   int32_t m_ExtractCompressedPublicKey(uint8_t(&orau8_PublicKey)[hu32_PUBLIC_KEY_LENGTH]) const;
   static EVP_PKEY * mh_CreateEvpPkeyFromRawPublicKey(const uint8_t(&orau8_PublicKey)[hu32_PUBLIC_KEY_LENGTH]);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
