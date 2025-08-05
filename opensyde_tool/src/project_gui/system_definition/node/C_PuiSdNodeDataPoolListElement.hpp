//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for data pool list element UI part (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDNODEDATAPOOLLISTELEMENT_HPP
#define C_PUISDNODEDATAPOOLLISTELEMENT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeDataPoolListElement
{
public:
   C_PuiSdNodeDataPoolListElement(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   bool q_AutoMinMaxActive;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
