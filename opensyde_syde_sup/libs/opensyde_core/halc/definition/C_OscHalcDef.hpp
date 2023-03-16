//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEF_HPP
#define C_OSCHALCDEF_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscHalcDefBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcDef :
   public C_OscHalcDefBase
{
public:
   C_OscHalcDef(void);

   virtual uint32_t GetDomainSize(void) const;
   virtual const C_OscHalcDefDomain * GetDomainDefDataConst(const uint32_t ou32_Index) const;
   virtual void AddDomain(const C_OscHalcDefDomain & orc_Domain);
   virtual void HandleFileLoadPostProcessing(void);

private:
   std::vector<C_OscHalcDefDomain> mc_Domains; ///< Domains of HALC definition
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
