//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition generic struct part
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFSTRUCT_HPP
#define C_OSCHALCDEFSTRUCT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscHalcDefElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcDefStruct :
   public C_OscHalcDefElement
{
public:
   C_OscHalcDefStruct(void);
   virtual ~C_OscHalcDefStruct(void);

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   std::vector<C_OscHalcDefElement> c_StructElements; ///< Optional data elements of this struct
   // If these are zero: assume plain type
   // If these are non-zero: assume struct type (don't use initial, min and max value in this case)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
