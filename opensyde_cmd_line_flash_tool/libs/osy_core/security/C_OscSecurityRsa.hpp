//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Access to RSA signature handling
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSECURITYRSA_HPP
#define C_OSCSECURITYRSA_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSecurityRsa
{
public:
   C_OscSecurityRsa();

   static int32_t h_SignSignature(const std::vector<uint8_t> & orc_PrivateKey, const std::vector<uint8_t> & orc_Message,
                                  std::vector<uint8_t> & orc_EncryptedMessage);
   static int32_t h_VerifySignature(const std::vector<uint8_t> & orc_PublicKey,
                                    const std::vector<uint8_t> & orc_Message,
                                    const std::vector<uint8_t> & orc_EncryptedMessage, bool & orq_Valid);

private:
   static const uint32_t mhu32_DEFAULT_BUFFER_SIZE;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
