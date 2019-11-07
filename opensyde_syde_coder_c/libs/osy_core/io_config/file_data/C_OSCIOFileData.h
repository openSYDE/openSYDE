//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       IO description
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOFILEDATA_H
#define C_OSCIOFILEDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCIOFileDataBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOFileData :
   public C_OSCIOFileDataBase
{
public:
   C_OSCIOFileData(void);

   virtual stw_types::uint32 GetDomainSize(void) const;
   virtual const C_OSCIOFileDataDomain * GetDomainFileDataConst(const stw_types::uint32 ou32_Index) const;
   virtual void AddDomain(const C_OSCIOFileDataDomain & orc_Domain);

private:
   std::vector<C_OSCIOFileDataDomain> c_Domains; ///< Domains of IO description
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
