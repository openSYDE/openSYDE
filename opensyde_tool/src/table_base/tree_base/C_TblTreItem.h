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
#include <vector>
#include <QIcon>
#include <QString>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreItem
{
public:
   C_TblTreItem(void);
   ~C_TblTreItem(void);

   void AddChild(C_TblTreItem * const opc_Child);
   void ReserveChildrenSpace(const stw_types::uint32 ou32_Space);
   void SetChild(const stw_types::uint32 ou32_Index, C_TblTreItem * const opc_Child);
   stw_types::sint32 GetIndexInParentNumber(void);
   static stw_types::sint32 h_GetIndex(const std::vector<C_TblTreItem *> & orc_Vector,
                                       const C_TblTreItem * const opc_Item);
   C_TblTreItem * GetItem(const stw_types::uint32 ou32_Index);

   stw_types::uint32 u32_Index;
   QString c_Name;
   QString c_ToolTipHeading;
   QString c_ToolTipContent;
   QIcon c_Icon;
   bool q_Enabled;
   bool q_Selectable;
   C_TblTreItem * pc_Parent;               ///< If this one is NULL you have the invisible root item
   std::vector<C_TblTreItem *> c_Children; ///< If this one is empty you have a data element leaf
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
