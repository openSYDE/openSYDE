//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       IO configuration data base
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOFILEDATABASE_H
#define C_OSCIOFILEDATABASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCIOFileDataDomain.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOFileDataBase
{
public:
   C_OSCIOFileDataBase(void);
   virtual ~C_OSCIOFileDataBase(void);

   bool CheckIdsUnique(std::vector<stw_scl::C_SCLString> & orc_DuplicateIds) const;

   stw_types::uint32 u32_ContentVersion;
   stw_scl::C_SCLString c_DeviceName;

   virtual stw_types::uint32 GetDomainSize(void) const = 0;
   virtual const C_OSCIOFileDataDomain * GetDomainFileDataConst(const stw_types::uint32 ou32_Index) const = 0;
   virtual void AddDomain(const C_OSCIOFileDataDomain & orc_Domain) = 0;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
