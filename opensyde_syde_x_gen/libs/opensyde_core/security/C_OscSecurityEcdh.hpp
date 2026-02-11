//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core ECDH encryption utility

   \class       stw::opensyde_core::C_OscSecurityEcdh

   C++ wrapper for the generic elliptic curve diffie hellman algorithm implementation in the UDS stack
   Provides functions to generate private and public ec keys and derive shared secret using ECDH with SHA-256 as KDF.

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSECURITYECDH_HPP
#define C_OSCSECURITYECDH_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSecurityEcdh
{
public:
   static const uint32_t hu32_PUBLIC_KEY_LENGTH = 33U;
   static const uint32_t hu32_PRIVATE_KEY_LENGTH = 32U;
   static const uint32_t hu32_AES_KEY_LENGTH = 16U;

   static int32_t h_GetKeys(uint8_t(&orau8_PublicKey)[hu32_PUBLIC_KEY_LENGTH],
                            uint8_t(&orau8_PrivateKey)[hu32_PRIVATE_KEY_LENGTH]);
   static int32_t h_DeriveKey(const uint8_t(&orau8_OthersPublicKey)[hu32_PUBLIC_KEY_LENGTH],
                              const uint8_t(&orau8_OwnPrivateKey)[hu32_PRIVATE_KEY_LENGTH],
                              uint8_t(&orau8_AesKey)[hu32_AES_KEY_LENGTH]);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
