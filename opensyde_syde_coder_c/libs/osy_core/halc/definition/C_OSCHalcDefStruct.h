//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition generic struct part
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFSTRUCT_H
#define C_OSCHALCDEFSTRUCT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCHalcDefElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcDefStruct :
   public C_OSCHalcDefElement
{
public:
   C_OSCHalcDefStruct(void);
   virtual ~C_OSCHalcDefStruct(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   std::vector<C_OSCHalcDefElement> c_StructElements; ///< Optional data elements of this struct
   // If these are zero: assume plain type
   // If these are non-zero: assume struct type (don't use initial, min and max value in this case)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
