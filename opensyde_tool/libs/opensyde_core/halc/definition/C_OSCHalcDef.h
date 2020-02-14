//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEF_H
#define C_OSCHALCDEF_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCHalcDefBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcDef :
   public C_OSCHalcDefBase
{
public:
   C_OSCHalcDef(void);

   virtual stw_types::uint32 GetDomainSize(void) const;
   virtual const C_OSCHalcDefDomain * GetDomainDefDataConst(const stw_types::uint32 ou32_Index) const;
   virtual void AddDomain(const C_OSCHalcDefDomain & orc_Domain);

private:
   std::vector<C_OSCHalcDefDomain> mc_Domains; ///< Domains of HALC definition
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
