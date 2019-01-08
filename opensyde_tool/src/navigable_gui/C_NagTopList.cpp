//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       List widget for topology widgets (implementation)

   List widget for topology widgets

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     31.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_NagTopList.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     31.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_NagTopList::C_NagTopList(QWidget * const opc_Parent) :
   C_OgeListViewToolTipBase(opc_Parent)
{
   this->setModel(&mc_Model);
   this->setItemDelegate(&mc_Delegate);

   this->setSelectionMode(QAbstractItemView::NoSelection);

   connect(this, &C_NagTopList::clicked, this, &C_NagTopList::m_HandleClicked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set item types are nodes flag

   \param[in] oq_IsNode Flag if items are nodes

   \created     01.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagTopList::SetTypeNode(const bool oq_IsNode)
{
   this->mc_Model.SetTypeNode(oq_IsNode);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update selected index

   \param[in] os32_Selected Selected index (is any)

   \created     31.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagTopList::SetSelectedIndex(const sint32 os32_Selected)
{
   this->mc_Model.SetSelectedIndex(os32_Selected);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set list content

   \param[in] orc_Content List content names

   \created     31.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagTopList::SetContent(const std::vector<QString> & orc_Content)
{
   this->mc_Model.SetContent(orc_Content);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set list errors

   \param[in] orc_Error List error states

   \created     31.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagTopList::SetError(const std::vector<bool> & orc_Error)
{
   this->mc_Model.SetError(orc_Error);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle index click

   \param[in] orc_Index Clicked index

   \created     31.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagTopList::m_HandleClicked(const QModelIndex & orc_Index)
{
   if (orc_Index.isValid() == true)
   {
      Q_EMIT this->SigClicked(orc_Index.row());
   }
}
