//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Access to RSA signature handling
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSECURITYRSA_H
#define C_OSCSECURITYRSA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCSecurityRsa
{
public:
   C_OSCSecurityRsa();

   static void h_Init();

   static stw_types::sint32 h_SignSignature(const std::vector<stw_types::uint8> & orc_PrivateKey,
                                            const std::vector<stw_types::uint8> & orc_Message,
                                            std::vector<stw_types::uint8> & orc_EncryptedMessage);
   static stw_types::sint32 h_VerifySignature(const std::vector<stw_types::uint8> & orc_PublicKey,
                                              const std::vector<stw_types::uint8> & orc_Message,
                                              const std::vector<stw_types::uint8> & orc_EncryptedMessage,
                                              bool & orq_Valid);

private:
   static const stw_types::uint32 mhu32_DEFAULT_BUFFER_SIZE;

   static bool mhq_IsInitialized;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
