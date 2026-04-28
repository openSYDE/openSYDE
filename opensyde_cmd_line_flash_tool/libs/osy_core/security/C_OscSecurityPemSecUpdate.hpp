//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle content of PEM files for SecureUpdate feature
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSECURITYPEMSECUPDATE_HPP
#define C_OSCSECURITYPEMSECUPDATE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscSecurityPemBase.hpp"
/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSecurityPemSecUpdate :
   public C_OscSecurityPemBase
{
public:
   C_OscSecurityPemSecUpdate();

   virtual int32_t LoadFromFile(const std::string & orc_FileName, std::string & orc_ErrorMessage);

protected:
   virtual int32_t m_ReadPrivateKey(const std::vector<uint8_t> & orc_FileContent, std::string & orc_ErrorMessage);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
