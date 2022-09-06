//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view for showing the PDO configuration of a specific device on a specific interface of the CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QMouseEvent>

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_UsHandler.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeCoPdoTableView.h"
#include "C_SdNdeCoPdoTableModel.h"
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
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoPdoTableView::C_SdNdeCoPdoTableView(QWidget * const opc_Parent) :
   C_TblViewScroll(opc_Parent)
{
   QItemSelectionModel * const pc_LastSelectionModel = this->selectionModel();

   this->mc_SortProxyModel.setSourceModel(&mc_Model);
   this->mc_SortProxyModel.setSortRole(static_cast<sintn>(Qt::EditRole));
   this->C_SdNdeCoPdoTableView::setModel(&mc_SortProxyModel);
   this->mc_Delegate.SetModel(&mc_Model);
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
C_SdNdeCoPdoTableView::~C_SdNdeCoPdoTableView()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoTableView::LoadUserSettings(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
      this->mc_Model.GetDeviceNodeIndex());

   if (pc_Node != NULL)
   {
      const C_UsNode c_Node = C_UsHandler::h_GetInstance()->GetProjSdNode(pc_Node->c_Properties.c_Name.c_str());
      if (this->m_SetColumnWidths(c_Node.GetCANopenPdoOverviewColumnWidth()) == false)
      {
         m_InitColumns();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoTableView::SaveUserSettings(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
      this->mc_Model.GetDeviceNodeIndex());

   if (pc_Node != NULL)
   {
      C_UsHandler::h_GetInstance()->SetProjSdNodeCANopenPdoOverviewColumnWidth(
         pc_Node->c_Properties.c_Name.c_str(), this->m_GetColumnWidths());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoTableView::UpdateData()
{
   this->mc_Model.UpdateData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index

   \param[in]  ou32_ManagerNodeIndex              Node index
   \param[in]  ou8_ManagerInterfaceId             Interface index
   \param[in]  orc_DeviceInterfaceId              CAN node interface id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoTableView::SetNodeIndexAndInterfaceId(const stw_types::uint32 ou32_ManagerNodeIndex,
                                                       const stw_types::uint8 ou8_ManagerInterfaceId,
                                                       const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceInterfaceId)
{
   this->mc_Model.SetNodeIndexAndInterfaceId(ou32_ManagerNodeIndex, ou8_ManagerInterfaceId, orc_DeviceInterfaceId);
   this->sortByColumn(static_cast<sintn>(C_SdNdeCoPdoTableModel::eINDEX), Qt::AscendingOrder);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the number of rows

   \return
   Number of items in table
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sintn C_SdNdeCoPdoTableView::GetCountRows() const
{
   return this->mc_Model.rowCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Track mouse hover

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoTableView::mouseMoveEvent(QMouseEvent * const opc_Event)
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
void C_SdNdeCoPdoTableView::leaveEvent(QEvent * const opc_Event)
{
   QTableView::leaveEvent(opc_Event);
   if (this->mc_Delegate.SetHoveredRow(-1) == true)
   {
      this->update();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse double click event slot

   Here: Send navigation to message in COMM messages signal

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoTableView::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   const QModelIndex c_ClickedIndexProxy = this->indexAt(opc_Event->pos());

   QTableView::mouseDoubleClickEvent(opc_Event);

   if (c_ClickedIndexProxy.isValid() == true)
   {
      const QModelIndex c_ClickedIndex = this->mc_SortProxyModel.mapToSource(c_ClickedIndexProxy);
      if (c_ClickedIndex.isValid() == true)
      {
         stw_opensyde_core::C_OSCCanMessageIdentificationIndices c_MsgId;
         if (this->mc_Model.MapRowToMsgId(c_ClickedIndex.row(), c_MsgId) == C_NO_ERR)
         {
            Q_EMIT (this->SigSwitchToBusProtocolMessage(c_MsgId));
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init default columns
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoTableView::m_InitColumns(void)
{
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoPdoTableModel::eINDEX), 44);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoPdoTableModel::eENABLED), 60);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoPdoTableModel::eNAME), 180);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoPdoTableModel::eCOMMENT), 200);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoPdoTableModel::eCOTYPE), 60);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoPdoTableModel::eEXTENDED), 60);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoPdoTableModel::eCOBID), 170);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoPdoTableModel::eDLC), 50);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoPdoTableModel::eTXMETHOD), 170);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoPdoTableModel::eINHIBITTIME), 70);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoPdoTableModel::eEVENTTIME), 70);
}
