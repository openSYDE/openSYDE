//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group config data for IO config
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOCONFIGDATA_H
#define C_OSCIOCONFIGDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCIOFileDataBase.h"
#include "C_OSCIOConfigDataDomain.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOConfigData :
   public C_OSCIOFileDataBase
{
public:
   C_OSCIOConfigData(void);

   virtual stw_types::uint32 GetDomainSize(void) const;
   const C_OSCIOConfigDataDomain * GetDomainConfigDataConst(const stw_types::uint32 ou32_Index) const;
   virtual const C_OSCIOFileDataDomain * GetDomainFileDataConst(const stw_types::uint32 ou32_Index) const;
   virtual void AddDomain(const C_OSCIOFileDataDomain & orc_Domain);
   stw_types::sint32 SetDomainConfig(const stw_types::uint32 ou32_Index, const C_OSCIOConfigDataDomain & orc_Domain);

private:
   std::vector<C_OSCIOConfigDataDomain> c_Domains; ///< Domains of IO description (synced with C_OSCIOFileDataBase)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
