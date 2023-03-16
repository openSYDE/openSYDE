//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project node datapool list user settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_USNODEDATAPOOLLIST_H
#define C_USNODEDATAPOOLLIST_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsNodeDatapoolList
{
public:
   C_UsNodeDatapoolList(void);

   const std::vector<int32_t> & GetColumnWidths(void) const;
   void SetColumnSizes(const std::vector<int32_t> & orc_ColumnWidths);

private:
   std::vector<int32_t> mc_ColumnWidth;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
