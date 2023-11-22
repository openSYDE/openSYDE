//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for data pool UI part (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDNODEDATAPOOL_HPP
#define C_PUISDNODEDATAPOOL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>

#include "stwtypes.hpp"

#include "C_PuiSdNodeDataPoolList.hpp"
#include "C_OscXmlParser.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeDataPool
{
public:
   C_PuiSdNodeDataPool(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   std::vector<C_PuiSdNodeDataPoolList> c_DataPoolLists;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
