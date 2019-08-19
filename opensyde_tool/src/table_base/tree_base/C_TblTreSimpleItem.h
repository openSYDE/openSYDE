//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for simple data element tree structure
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLTRESIMPLEITEM_H
#define C_TBLTRESIMPLEITEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreSimpleItem
{
public:
   C_TblTreSimpleItem(void);
   virtual ~C_TblTreSimpleItem(void);

   void AddChild(C_TblTreSimpleItem * const opc_Child);
   void InsertChild(const stw_types::uint32 ou32_Index, C_TblTreSimpleItem * const opc_Child);
   void ReserveChildrenSpace(const stw_types::uint32 ou32_Space);
   void SetChild(const stw_types::uint32 ou32_Index, C_TblTreSimpleItem * const opc_Child);
   stw_types::sint32 GetIndexInParentNumber(void) const;
   void ClearChildren(void);
   static stw_types::sint32 h_GetIndex(const std::vector<C_TblTreSimpleItem *> & orc_Vector,
                                       const C_TblTreSimpleItem * const opc_Item);

   C_TblTreSimpleItem * pc_Parent;               ///< If this one is NULL you have the invisible root item
   std::vector<C_TblTreSimpleItem *> c_Children; ///< If this one is empty you have a data element leaf
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
