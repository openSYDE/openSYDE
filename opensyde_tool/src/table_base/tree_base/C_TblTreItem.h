//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for data element tree structure (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLTREITEM_H
#define C_TBLTREITEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QIcon>
#include <QString>
#include "C_TblTreSimpleItem.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreItem :
   public C_TblTreSimpleItem
{
public:
   C_TblTreItem(void);

   C_TblTreItem * GetItem(const stw_types::uint32 ou32_Index);

   stw_types::uint32 u32_Index;
   QString c_ToolTipHeading;
   QString c_ToolTipContent;
   QIcon c_Icon;
   QString c_Name;
   bool q_Enabled;
   bool q_Selectable;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
