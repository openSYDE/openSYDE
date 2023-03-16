//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class to handle parsing of PEM file folder and stores all necessary information about all read PEM files
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSECURITYPEMDATABASE_HPP
#define C_OSCSECURITYPEMDATABASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <string>
#include <vector>

#include "stwtypes.hpp"
#include "C_OscSecurityPemKeyInfo.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSecurityPemDatabase
{
public:
   C_OscSecurityPemDatabase();

   uint32_t GetSizeOfDatabase(void) const;
   const C_OscSecurityPemKeyInfo * GetPemFileBySerialNumber(const std::vector<uint8_t> & orc_SerialNumber)
   const;

   int32_t ParseFolder(const std::string & orc_FolderPath);

private:
   std::vector<C_OscSecurityPemKeyInfo> mc_StoredPemFiles;

   void m_TryAddKey(const C_OscSecurityPemKeyInfo & orc_NewKey, std::string & orc_ErrorMessage);
   static std::vector<std::string> mh_GetPemFiles(const std::string & orc_FolderPath);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
