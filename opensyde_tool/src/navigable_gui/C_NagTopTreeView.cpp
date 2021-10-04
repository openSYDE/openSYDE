//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view for topology item navigation tree (implementation)

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>

#include "stwtypes.h"
#include "constants.h"
#include "C_NagTopTreeView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out]  opc_Parent    Parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagTopTreeView::C_NagTopTreeView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent)
{
   this->C_NagTopTreeView::setModel(&mc_Model);
   this->setItemDelegate(&this->mc_Delegate);

   this->setHeaderHidden(true);
   this->setSelectionMode(QAbstractItemView::SingleSelection);

   connect(this->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &C_NagTopTreeView::m_OnItemSelected);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item types are nodes flag

   \param[in]  oq_IsNode   Flag if items are nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeView::SetTypeNode(const bool oq_IsNode)
{
   this->mc_Model.SetTypeNode(oq_IsNode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update selected index and scroll there

   \param[in]  os32_Selected  Selected index (-1 if no selection)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeView::SetSelectedIndexAndScroll(const sint32 os32_Selected)
{
   if (os32_Selected >= 0)
   {
      const QModelIndex & rc_Index = this->mc_Model.GetModelIndexFromFlatIndex(static_cast<uint32>(os32_Selected));
      this->selectionModel()->select(rc_Index, QItemSelectionModel::Select);
      this->scrollTo(rc_Index);
   }
   else
   {
      this->setCurrentIndex(QModelIndex()); // reset current index (this also clears selection)
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set tree content

   \param[in]  orc_Content       List of content names
   \param[in]  orc_SubContent    List of sub content names (corresponding to list of content names; or empty if none)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeView::SetContent(const std::vector<QString> & orc_Content,
                                  const std::vector<std::vector<QString> > & orc_SubContent)
{
   this->mc_Model.SetContent(orc_Content, orc_SubContent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set list errors

   \param[in]  orc_Error   List error states
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeView::SetError(const std::vector<bool> & orc_Error)
{
   this->mc_Model.SetError(orc_Error);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update text of a single item

   \param[in]  ou32_Index     Index
   \param[in]  orc_Content    Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeView::UpdateItem(const uint32 ou32_Index, const QString & orc_Content)
{
   this->mc_Model.UpdateItem(ou32_Index, orc_Content);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden drawBranches method

   Here: Move arrow icon horizontally to the right

   \param[in,out]  opc_Painter   Painter
   \param[in]      orc_Rect      Rectangle
   \param[in]      orc_Index     Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeView::drawBranches(QPainter * const opc_Painter, const QRect & orc_Rect,
                                    const QModelIndex & orc_Index) const
{
   QRect c_Rect = orc_Rect;

   c_Rect.moveLeft(6);
   C_OgeTreeViewToolTipBase::drawBranches(opc_Painter, c_Rect, orc_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Emit clicked signal when row got selected

   \param[in]  orc_Index   Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeView::m_OnItemSelected(const QModelIndex & orc_Index)
{
   if (orc_Index.isValid() == true)
   {
      Q_EMIT (this->SigClicked(C_NagTopTreeModel::h_GetFlatIndexFromModelIndex(orc_Index)));
   }
}
