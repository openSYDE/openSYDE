//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for data element tree structure (implementation)

   Class for data element tree structure

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_TblTreItem.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreItem::C_TblTreItem(void) :
   C_TblTreSimpleItem(),
   u32_Index(0),
   q_Enabled(true),
   q_Selectable(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Search for index in child items

   Info: this function only searches for this index in the first layer

   \param[in] ou32_Index Index to search for in child items

   \return
   NULL Index not found
   Else Valid item
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreItem * C_TblTreItem::GetItem(const uint32_t ou32_Index)
{
   C_TblTreItem * pc_Retval = NULL;

   for (std::vector<C_TblTreSimpleItem *>::const_iterator c_It = this->c_Children.begin();
        c_It != this->c_Children.end(); ++c_It)
   {
      C_TblTreSimpleItem * const pc_Item = *c_It;

      C_TblTreItem * const pc_CurItem = dynamic_cast<C_TblTreItem * const>(pc_Item);
      if ((pc_CurItem != NULL) && (pc_CurItem->u32_Index == ou32_Index))
      {
         pc_Retval = pc_CurItem;
         break;
      }
   }

   return pc_Retval;
}
