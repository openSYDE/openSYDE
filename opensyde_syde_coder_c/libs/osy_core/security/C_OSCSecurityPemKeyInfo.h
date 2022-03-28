//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       PEM key info part
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSECURITYPEMKEYINFO_H
#define C_OSCSECURITYPEMKEYINFO_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <string>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCSecurityPemKeyInfo
{
public:
   C_OSCSecurityPemKeyInfo();

   void Clear();

   const std::vector<stw_types::uint8> & GetPrivKeyTextDecoded(void) const;
   const std::vector<stw_types::uint8> & GetPubKeyTextDecoded(void) const;
   const std::vector<stw_types::uint8> & GetPubKeySerialNumber(void) const;

   void SetPrivKeyTextDecoded(const std::vector<stw_types::uint8> & orc_Value);
   void SetPubKeyTextDecoded(const std::vector<stw_types::uint8> & orc_Value);
   void SetPubKeySerialNumber(const std::vector<stw_types::uint8> & orc_Value);

private:
   std::vector<stw_types::uint8> mc_PrivKeyTextDecoded;
   std::vector<stw_types::uint8> mc_PubKeyTextDecoded;
   std::vector<stw_types::uint8> mc_PubKeySerialNumber;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
