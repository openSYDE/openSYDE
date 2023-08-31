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

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSecurityPemKeyInfo
{
public:
   C_OscSecurityPemKeyInfo();

   void Clear();

   const std::vector<uint8_t> & GetPrivKeyTextDecoded(void) const;
   const std::vector<uint8_t> & GetPubKeyTextDecoded(void) const;
   const std::vector<uint8_t> & GetPubKeySerialNumber(void) const;

   bool CheckValidKey(std::string & orc_ErrorMessage, const bool oq_CheckSerialNumber) const;
   void SetPrivKeyTextDecoded(const std::vector<uint8_t> & orc_Value);
   void SetPubKeyTextDecoded(const std::vector<uint8_t> & orc_Value);
   void SetPubKeySerialNumber(const std::vector<uint8_t> & orc_Value);

private:
   std::vector<uint8_t> mc_PrivKeyTextDecoded;
   std::vector<uint8_t> mc_PubKeyTextDecoded;
   std::vector<uint8_t> mc_PubKeySerialNumber;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
