//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core AES text encryption utility

   \class       stw::opensyde_core::C_OscSecurityAesCbc

   Wrapper for the AES library by SergeyBel.
   Provides functions to encrypt/decrypt text with AES-128 CBC with PKCS#7 and initialization vector.

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSECURITYAESCBC_HPP
#define C_OSCSECURITYAESCBC_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSecurityAesCbc
{
public:
   static const uint32_t hu32_KEY_LENGTH = 16U;
   static const uint32_t hu32_IV_LENGTH = 16U;

   static int32_t h_Encrypt(const uint8_t(&orau8_Key)[hu32_KEY_LENGTH],
                            const uint8_t(&orau8_InitVector)[hu32_IV_LENGTH], const std::vector<uint8_t> & orc_Input,
                            std::vector<uint8_t> & orc_Output);
   static int32_t h_Decrypt(const uint8_t(&orau8_Key)[hu32_KEY_LENGTH],
                            const uint8_t(&orau8_InitVector)[hu32_IV_LENGTH], const std::vector<uint8_t> & orc_Input,
                            std::vector<uint8_t> & orc_Output);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
