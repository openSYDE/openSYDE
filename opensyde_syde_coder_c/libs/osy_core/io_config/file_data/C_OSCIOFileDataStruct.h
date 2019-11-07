//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       IO description generic struct part
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOFILEDATASTRUCT_H
#define C_OSCIOFILEDATASTRUCT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCIOFileDataElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOFileDataStruct :
   public C_OSCIOFileDataElement
{
public:
   C_OSCIOFileDataStruct(void);
   virtual ~C_OSCIOFileDataStruct(void);

   std::vector<C_OSCIOFileDataElement> c_StructElements; ///< Optional data elements of this struct
   // If these are zero: assume plain type
   // If these are non-zero: assume struct type (don't use initial,min and max value in this case)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
