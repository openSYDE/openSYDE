//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for data pool list UI part (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDNODEDATAPOOLLIST_H
#define C_PUISDNODEDATAPOOLLIST_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>

#include "stwtypes.hpp"

#include "C_PuiSdNodeDataPoolListElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeDataPoolList
{
public:
   C_PuiSdNodeDataPoolList(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   std::vector<C_PuiSdNodeDataPoolListElement> c_DataPoolListElements;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
