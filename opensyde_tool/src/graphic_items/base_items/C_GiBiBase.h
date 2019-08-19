//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common base for all graphics items
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIBIBASE_H
#define C_GIBIBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiBiBase
{
public:
   C_GiBiBase(void);
   virtual ~C_GiBiBase(void);

   virtual void SetZValueCustom(const stw_types::float64 of64_ZValue) = 0;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
