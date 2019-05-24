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
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsNodeDatapoolList
{
public:
   C_UsNodeDatapoolList(void);

   const std::vector<stw_types::sint32> & GetColumnWidths(void) const;
   void SetColumnSizes(const std::vector<stw_types::sint32> & orc_ColumnWidths);

private:
   std::vector<stw_types::sint32> mc_ColumnWidth;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
