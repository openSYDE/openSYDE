//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition data base
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFBASE_H
#define C_OSCHALCDEFBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCHalcDefDomain.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcDefBase
{
public:
   C_OSCHalcDefBase(void);
   virtual ~C_OSCHalcDefBase(void);

   bool CheckIdsUnique(std::vector<stw_scl::C_SCLString> & orc_DuplicateIds) const;

   stw_types::uint32 u32_ContentVersion;
   stw_scl::C_SCLString c_DeviceName;
   stw_scl::C_SCLString c_FileString;

   virtual void Clear(void);
   virtual bool IsClear(void) const;

   virtual stw_types::uint32 GetDomainSize(void) const = 0;
   virtual const C_OSCHalcDefDomain * GetDomainDefDataConst(const stw_types::uint32 ou32_Index) const = 0;
   virtual void AddDomain(const C_OSCHalcDefDomain & orc_Domain) = 0;

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

private:
   static void mh_AggregateIds(const std::vector<C_OSCHalcDefStruct> & orc_Items,
                               std::vector<stw_scl::C_SCLString> & orc_DuplicateIds);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
