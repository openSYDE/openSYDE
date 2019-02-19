//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Class for data element tree structure (implementation)

   Class for data element tree structure

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_TblTreItem.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_TblTreItem::C_TblTreItem(void) :
   u32_Index(0),
   q_Enabled(true),
   q_Selectable(true),
   pc_Parent(NULL)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_TblTreItem::~C_TblTreItem(void)
{
   for (std::vector<C_TblTreItem *>::const_iterator c_It = this->c_Children.begin();
        c_It != this->c_Children.end();
        ++c_It)
   {
      delete (*c_It);
   }
   pc_Parent = NULL;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add child and take ownership

   \param[in,out] opc_Child Child to add & store

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreItem::AddChild(C_TblTreItem * const opc_Child)
{
   if (opc_Child != NULL)
   {
      opc_Child->pc_Parent = this;
      this->c_Children.push_back(opc_Child);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle children space reservation (performance)

   \param[in] ou32_Space Number of items to reserve for

   \created     05.12.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreItem::ReserveChildrenSpace(const uint32 ou32_Space)
{
   this->c_Children.reserve(ou32_Space);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set child and take ownership

   Warning: this will delete the previous item at this position

   \param[in]     ou32_Index Position to access
   \param[in,out] opc_Child  Child to set & store

   \created     05.09.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblTreItem::SetChild(const uint32 ou32_Index, C_TblTreItem * const opc_Child)
{
   if (opc_Child != NULL)
   {
      opc_Child->pc_Parent = this;
      delete this->c_Children[ou32_Index];
      this->c_Children[ou32_Index] = opc_Child;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get index of this item in parent item

   \return
   Index of this item in parent item

   \created     29.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_TblTreItem::GetIndexInParentNumber(void)
{
   sint32 s32_Retval = -1;

   if (this->pc_Parent != NULL)
   {
      s32_Retval = C_TblTreItem::h_GetIndex(this->pc_Parent->c_Children, this);
   }
   return s32_Retval;
   //lint -e{1762} This function cannot be const, I  tried (probably changes in interface from qt 5.6 to qt 5.7)
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get index of item in vector

   \param[in] orc_Vector Vector
   \param[in] opc_Item   Item

   \return
   -1   Not found
   Else Valid index

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_TblTreItem::h_GetIndex(const std::vector<C_TblTreItem *> & orc_Vector, const C_TblTreItem * const opc_Item)
{
   sint32 s32_Retval = -1;

   if (opc_Item != NULL)
   {
      //lint -e{10,26,50,64,119,155,1024,1703} C++11 construct
      const std::vector<C_TblTreItem *>::const_iterator c_It = std::find_if(
         orc_Vector.begin(),
         orc_Vector.end(),
         [&] (const std::vector<C_TblTreItem *>::value_type & orc_Test)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Search for index in child items

   Info: this function only searches for this index in the first layer

   \param[in] ou32_Index Index to search for in child items

   \return
   NULL Index not found
   Else Valid item

   \created     25.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_TblTreItem * C_TblTreItem::GetItem(const uint32 ou32_Index)
{
   C_TblTreItem * pc_Retval = NULL;

   for (std::vector<C_TblTreItem *>::const_iterator c_It = this->c_Children.begin();
        c_It != this->c_Children.end(); ++c_It)
   {
      C_TblTreItem * const pc_CurItem = *c_It;
      if ((pc_CurItem != NULL) && (pc_CurItem->u32_Index == ou32_Index))
      {
         pc_Retval = pc_CurItem;
         break;
      }
   }

   return pc_Retval;
}
