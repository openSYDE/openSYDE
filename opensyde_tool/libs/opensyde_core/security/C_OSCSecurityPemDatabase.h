//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class to handle parsing of PEM file folder and stores all necessary information about all read PEM files
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSECURITYPEMDATABASE_H
#define C_OSCSECURITYPEMDATABASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <string>
#include <vector>

#include "stwtypes.h"
#include "C_OSCSecurityPemKeyInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCSecurityPemDatabase
{
public:
   C_OSCSecurityPemDatabase();

   stw_types::uint32 GetSizeOfDatabase(void) const;
   const C_OSCSecurityPemKeyInfo * GetPemFileBySerialNumber(const std::vector<stw_types::uint8> & orc_SerialNumber)
   const;

   stw_types::sint32 ParseFolder(const std::string & orc_FolderPath);

private:
   std::vector<C_OSCSecurityPemKeyInfo> mc_StoredPemFiles;

   void m_TryAddKey(const C_OSCSecurityPemKeyInfo & orc_NewKey, std::string & orc_ErrorMessage);
   static std::vector<std::string> mh_GetPEMFiles(const std::string & orc_FolderPath);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
