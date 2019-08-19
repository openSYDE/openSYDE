//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for simple data element tree structure

   Class for simple data element tree structure

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <stddef.h>
#include <algorithm>

#include "stwtypes.h"
#include "C_TblTreSimpleItem.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreSimpleItem::C_TblTreSimpleItem() :
   pc_Parent(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreSimpleItem::~C_TblTreSimpleItem()
{
   this->ClearChildren();
   pc_Parent = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add child and take ownership

   \param[in,out] opc_Child Child to add & store
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreSimpleItem::AddChild(C_TblTreSimpleItem * const opc_Child)
{
   if (opc_Child != NULL)
   {
      opc_Child->pc_Parent = this;
      this->c_Children.push_back(opc_Child);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert child and take ownership

   \param[in]     ou32_Index Index for new child
   \param[in,out] opc_Child  Child to add & store
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreSimpleItem::InsertChild(const uint32 ou32_Index, C_TblTreSimpleItem * const opc_Child)
{
   if (opc_Child != NULL)
   {
      opc_Child->pc_Parent = this;
      this->c_Children.insert(this->c_Children.begin() + ou32_Index, opc_Child);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle children space reservation (performance)

   \param[in] ou32_Space Number of items to reserve for
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreSimpleItem::ReserveChildrenSpace(const uint32 ou32_Space)
{
   this->c_Children.reserve(ou32_Space);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set child and take ownership

   Warning: this will delete the previous item at this position

   \param[in]     ou32_Index Position to access
   \param[in,out] opc_Child  Child to set & store
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreSimpleItem::SetChild(const uint32 ou32_Index, C_TblTreSimpleItem * const opc_Child)
{
   if (opc_Child != NULL)
   {
      opc_Child->pc_Parent = this;
      delete this->c_Children[ou32_Index];
      this->c_Children[ou32_Index] = opc_Child;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get index of this item in parent item

   \return
   Index of this item in parent item
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_TblTreSimpleItem::GetIndexInParentNumber(void) const
{
   sint32 s32_Retval = -1;

   if (this->pc_Parent != NULL)
   {
      s32_Retval = C_TblTreSimpleItem::h_GetIndex(this->pc_Parent->c_Children, this);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear and delete all children
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreSimpleItem::ClearChildren(void)
{
   for (std::vector<C_TblTreSimpleItem *>::const_iterator c_It = this->c_Children.begin();
        c_It != this->c_Children.end();
        ++c_It)
   {
      delete (*c_It);
   }
   this->c_Children.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get index of item in vector

   \param[in] orc_Vector Vector
   \param[in] opc_Item   Item

   \return
   -1   Not found
   Else Valid index
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_TblTreSimpleItem::h_GetIndex(const std::vector<C_TblTreSimpleItem *> & orc_Vector,
                                      const C_TblTreSimpleItem * const opc_Item)
{
   sint32 s32_Retval = -1;

   if (opc_Item != NULL)
   {
      //lint -e{10,26,50,64,119,155,1024,1703} C++11 construct
      const std::vector<C_TblTreSimpleItem *>::const_iterator c_It = std::find_if(
         orc_Vector.begin(),
         orc_Vector.end(),
         [&] (const std::vector<C_TblTreSimpleItem *>::value_type & orc_Test)
      {
         return orc_Test == opc_Item;
      }
         );
      if (c_It != orc_Vector.end())
      {
         s32_Retval = c_It - orc_Vector.begin();
      }
   }
   return s32_Retval;
}
