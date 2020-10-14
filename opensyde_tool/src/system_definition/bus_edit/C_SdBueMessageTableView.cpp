//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message table view (implementation)

   Message table view

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QHeaderView>
#include <QMouseEvent>
#include <QScrollBar>

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_SdBueMessageTableView.h"
#include "C_SdBueMessageSignalTableDelegate.h"
#include "C_SdNdeSingleHeaderView.h"
#include "constants.h"

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
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageTableView::C_SdBueMessageTableView(QWidget * const opc_Parent) :
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
   this->horizontalHeader()->resizeSection(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eICON),
                                           42);
   this->setIconSize(mc_ICON_SIZE_20);

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
C_SdBueMessageTableView::~C_SdBueMessageTableView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings

   \param[in]  orc_Values  Values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageTableView::LoadUserSettings(const std::vector<sint32> & orc_Values)
{
   if (this->m_SetColumnWidths(orc_Values) == false)
   {
      m_InitColumns();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings

   \param[in,out]  orc_Values    Values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageTableView::SaveUserSettings(std::vector<sint32> & orc_Values) const
{
   orc_Values = this->m_GetColumnWidths();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message sync manager

   \param[in,out]  opc_Value  Message sync manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageTableView::SetMessageSyncManager(
   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value)
{
   this->mc_Model.SetMessageSyncManager(opc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal data changes to table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageTableView::UpdateData(void)
{
   this->mc_Model.UpdateData();
   this->sortByColumn(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eINDEX), Qt::AscendingOrder);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the number of rows

   \return
   Count of items in table
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdBueMessageTableView::GetCountRows(void) const
{
   return this->mc_Model.rowCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Track mouse hover

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageTableView::mouseMoveEvent(QMouseEvent * const opc_Event)
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
void C_SdBueMessageTableView::leaveEvent(QEvent * const opc_Event)
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
void C_SdBueMessageTableView::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   const QModelIndex c_ClickedIndexProxy = this->indexAt(opc_Event->pos());

   QTableView::mouseDoubleClickEvent(opc_Event);
   if (c_ClickedIndexProxy.isValid() == true)
   {
      const QModelIndex c_ClickedIndex = this->mc_SortProxyModel.mapToSource(c_ClickedIndexProxy);
      if (c_ClickedIndex.isValid() == true)
      {
         C_OSCCanMessageIdentificationIndices c_MessageId;
         if (this->mc_Model.ConvertRowToMessage(c_ClickedIndex.row(), c_MessageId) == C_NO_ERR)
         {
            Q_EMIT this->SigMessageSelected(c_MessageId);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init default columns
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageTableView::m_InitColumns(void)
{
   this->setColumnWidth(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eINDEX), 40);
   this->setColumnWidth(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eICON), 26);
   this->setColumnWidth(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eNAME), 206);
   this->setColumnWidth(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eCOMMENT), 206);
   this->setColumnWidth(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eEXTENDED), 71);
   this->setColumnWidth(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eCAN_ID), 72);
   this->setColumnWidth(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eDLC), 44);
   this->setColumnWidth(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eTX_METHOD), 90);
   this->setColumnWidth(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eCYCLE_TIME), 105);
   this->setColumnWidth(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eNOT_EARLIER_THAN), 135);
   this->setColumnWidth(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eNOT_LATER_THAN), 145);
   this->setColumnWidth(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eTRANSMITTER), 90);
   this->setColumnWidth(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eRECEIVER), 106);
}
