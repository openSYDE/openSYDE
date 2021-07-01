//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List widget for topology widgets (implementation)

   List widget for topology widgets

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_NagTopList.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagTopList::C_NagTopList(QWidget * const opc_Parent) :
   C_OgeListViewToolTipBase(opc_Parent)
{
   this->C_NagTopList::setModel(&mc_Model);
   this->setItemDelegate(&mc_Delegate);

   this->setSelectionMode(QAbstractItemView::NoSelection);

   connect(this, &C_NagTopList::clicked, this, &C_NagTopList::m_HandleClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item types are nodes flag

   \param[in] oq_IsNode Flag if items are nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopList::SetTypeNode(const bool oq_IsNode)
{
   this->mc_Model.SetTypeNode(oq_IsNode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update selected index and scroll there

   \param[in] os32_Selected Selected index (is any)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopList::SetSelectedIndexAndScroll(const sint32 os32_Selected)
{
   this->mc_Delegate.SetSelectedIndex(os32_Selected);
   this->mc_Model.SetSelectedIndex(os32_Selected);
   if (os32_Selected >= 0)
   {
      this->scrollTo(this->mc_Model.index(os32_Selected));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set list content

   \param[in] orc_Content List content names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopList::SetContent(const std::vector<QString> & orc_Content)
{
   this->mc_Model.SetContent(orc_Content);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set list errors

   \param[in] orc_Error List error states
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopList::SetError(const std::vector<bool> & orc_Error)
{
   this->mc_Model.SetError(orc_Error);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle index click

   \param[in] orc_Index Clicked index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopList::m_HandleClicked(const QModelIndex & orc_Index)
{
   if (orc_Index.isValid() == true)
   {
      Q_EMIT this->SigClicked(orc_Index.row());
   }
}
