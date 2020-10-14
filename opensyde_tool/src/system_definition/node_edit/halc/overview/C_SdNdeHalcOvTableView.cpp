//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC overview table view

   HALC overview table view

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QMouseEvent>

#include "constants.h"
#include "stwerrors.h"
#include "C_UsHandler.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeHalcOvTableView.h"
#include "C_SdNdeSingleHeaderView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
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
C_SdNdeHalcOvTableView::C_SdNdeHalcOvTableView(QWidget * const opc_Parent) :
   C_TblViewScroll(opc_Parent)
{
   QItemSelectionModel * const pc_LastSelectionModel = this->selectionModel();

   this->mc_SortProxyModel.setSourceModel(&mc_Model);
   this->mc_SortProxyModel.setSortRole(static_cast<sintn>(Qt::EditRole));
   this->setModel(&mc_SortProxyModel);
   //Delete last selection model, see Qt documentation for setModel
   delete pc_LastSelectionModel;

   //Drawing delegate
   this->mc_Delegate.SetModel(&mc_Model);
   this->setItemDelegate(&mc_Delegate);

   //UI Settings
   this->setCornerButtonEnabled(false);
   this->setSortingEnabled(true);
   this->setGridStyle(Qt::NoPen);
   this->setShowGrid(false);
   this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
   this->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
   this->setAlternatingRowColors(true);
   this->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
   this->setDefaultDropAction(Qt::DropAction::MoveAction);
   this->setDragEnabled(false);
   this->setLineWidth(0);
   this->setFrameShadow(QAbstractItemView::Shadow::Plain);
   this->setFrameShape(QAbstractItemView::Shape::NoFrame);
   this->setEditTriggers(EditTrigger::NoEditTriggers);
   //Consider all elements for resize
   this->setHorizontalHeader(new C_SdNdeSingleHeaderView(Qt::Horizontal));
   this->horizontalHeader()->setResizeContentsPrecision(-1);
   this->horizontalHeader()->setFixedHeight(35);
   this->horizontalHeader()->setStretchLastSection(true);
   this->horizontalHeader()->setSectionsClickable(true);
   m_InitColumns();
   //Row Height
   this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   this->verticalHeader()->setMinimumSectionSize(25);
   this->verticalHeader()->setMaximumSectionSize(25);
   this->verticalHeader()->setDefaultSectionSize(25);
   //Icon
   // make sure to resize to icon size
   this->horizontalHeader()->resizeSection(C_SdNdeHalcOvTableModel::h_EnumToColumn(C_SdNdeHalcOvTableModel::eTYPE),
                                           42);
   this->setIconSize(QSize(21, 21));

   //Hide vertical header
   this->verticalHeader()->hide();

   //Hover event
   this->setMouseTracking(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcOvTableView::~C_SdNdeHalcOvTableView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableView::LoadUserSettings(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mc_Model.GetNodeIndex());

   if (pc_Node != NULL)
   {
      const C_UsNode c_Node = C_UsHandler::h_GetInstance()->GetProjSdNode(pc_Node->c_Properties.c_Name.c_str());
      if (this->m_SetColumnWidths(c_Node.GetHalcOverviewColumnWidth()) == false)
      {
         m_InitColumns();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableView::SaveUserSettings(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mc_Model.GetNodeIndex());

   if (pc_Node != NULL)
   {
      C_UsHandler::h_GetInstance()->SetProjSdNodeHalcOverviewColumnWidth(
         pc_Node->c_Properties.c_Name.c_str(), this->m_GetColumnWidths());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableView::UpdateData(void)
{
   this->mc_Model.UpdateData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableView::SetNodeIndex(const stw_types::uint32 ou32_NodeIndex)
{
   this->mc_Model.SetNodeIndex(ou32_NodeIndex);
   this->sortByColumn(C_SdNdeHalcOvTableModel::h_EnumToColumn(C_SdNdeHalcOvTableModel::eINDEX), Qt::AscendingOrder);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the number of rows

   \return
   Count of items in table
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeHalcOvTableView::GetCountRows() const
{
   return this->mc_Model.rowCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Track mouse hover

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableView::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   sint32 s32_HoveredRow = -1;
   const QModelIndex c_HoveredIndex = this->indexAt(opc_Event->pos());

   QTableView::mouseMoveEvent(opc_Event);

   if (c_HoveredIndex.isValid())
   {
      s32_HoveredRow = c_HoveredIndex.row();
   }
   if (this->mc_Delegate.SetHoveredRow(s32_HoveredRow) == true)
   {
      this->update();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse leave event slot

   Here: Handle hover effect change

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableView::leaveEvent(QEvent * const opc_Event)
{
   QTableView::leaveEvent(opc_Event);
   if (this->mc_Delegate.SetHoveredRow(-1) == true)
   {
      this->update();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse double click event slot

   Here: Send selection signal

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableView::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   const QModelIndex c_ClickedIndexProxy = this->indexAt(opc_Event->pos());

   QTableView::mouseDoubleClickEvent(opc_Event);
   if (c_ClickedIndexProxy.isValid() == true)
   {
      const QModelIndex c_ClickedIndex = this->mc_SortProxyModel.mapToSource(c_ClickedIndexProxy);
      if (c_ClickedIndex.isValid() == true)
      {
         uint32 u32_DomainNum;
         if (this->mc_Model.MapRowToDomainIndex(c_ClickedIndex.row(), u32_DomainNum) == C_NO_ERR)
         {
            uint32 u32_ChanNum;
            if (this->mc_Model.MapRowToChannelNum(c_ClickedIndex.row(), u32_ChanNum) == C_NO_ERR)
            {
               Q_EMIT (this->SigChannelSelected(u32_DomainNum, u32_ChanNum, true));
            }
            else
            {
               Q_EMIT (this->SigChannelSelected(u32_DomainNum, 0UL, false));
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init default columns
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableView::m_InitColumns(void)
{
   this->setColumnWidth(C_SdNdeHalcOvTableModel::h_EnumToColumn(C_SdNdeHalcOvTableModel::eINDEX), 44);
   this->setColumnWidth(C_SdNdeHalcOvTableModel::h_EnumToColumn(C_SdNdeHalcOvTableModel::eTYPE), 44);
   this->setColumnWidth(C_SdNdeHalcOvTableModel::h_EnumToColumn(C_SdNdeHalcOvTableModel::eDOMAIN), 85);
   this->setColumnWidth(C_SdNdeHalcOvTableModel::h_EnumToColumn(C_SdNdeHalcOvTableModel::eCHANNEL), 144);
   this->setColumnWidth(C_SdNdeHalcOvTableModel::h_EnumToColumn(C_SdNdeHalcOvTableModel::eNAME), 174);
   this->setColumnWidth(C_SdNdeHalcOvTableModel::h_EnumToColumn(C_SdNdeHalcOvTableModel::eCOMMENT), 174);
   this->setColumnWidth(C_SdNdeHalcOvTableModel::h_EnumToColumn(C_SdNdeHalcOvTableModel::eUSE_CASE), 139);
   this->setColumnWidth(C_SdNdeHalcOvTableModel::h_EnumToColumn(C_SdNdeHalcOvTableModel::eLINKED_WITH), 103);
   this->setColumnWidth(C_SdNdeHalcOvTableModel::h_EnumToColumn(C_SdNdeHalcOvTableModel::eSAFETY_RELEVANT), 59);
   this->setColumnWidth(C_SdNdeHalcOvTableModel::h_EnumToColumn(C_SdNdeHalcOvTableModel::eDATAPOOL), 115);
   this->setColumnWidth(C_SdNdeHalcOvTableModel::h_EnumToColumn(C_SdNdeHalcOvTableModel::eDATABLOCK_ASSIGNMENT), 162);
}
