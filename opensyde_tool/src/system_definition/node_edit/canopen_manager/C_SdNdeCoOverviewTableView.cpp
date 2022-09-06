//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view widget with current CANopen configuration of Manager and devices on the used interfaces

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMouseEvent>

#include "precomp_headers.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_UsHandler.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeCoOverviewTableView.h"
#include "C_SdNdeCoOverviewTableModel.h"
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
C_SdNdeCoOverviewTableView::C_SdNdeCoOverviewTableView(QWidget * const opc_Parent) :
   C_TblViewScroll(opc_Parent)
{
   QItemSelectionModel * const pc_LastSelectionModel = this->selectionModel();

   this->mc_SortProxyModel.setSourceModel(&mc_Model);
   this->mc_SortProxyModel.setSortRole(static_cast<sintn>(Qt::EditRole));
   this->C_SdNdeCoOverviewTableView::setModel(&mc_SortProxyModel);
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
C_SdNdeCoOverviewTableView::~C_SdNdeCoOverviewTableView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableView::LoadUserSettings(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mc_Model.GetNodeIndex());

   if (pc_Node != NULL)
   {
      const C_UsNode c_Node = C_UsHandler::h_GetInstance()->GetProjSdNode(pc_Node->c_Properties.c_Name.c_str());
      if (this->m_SetColumnWidths(c_Node.GetCANopenOverviewColumnWidth()) == false)
      {
         m_InitColumns();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableView::SaveUserSettings(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mc_Model.GetNodeIndex());

   if (pc_Node != NULL)
   {
      C_UsHandler::h_GetInstance()->SetProjSdNodeCANopenOverviewColumnWidth(
         pc_Node->c_Properties.c_Name.c_str(), this->m_GetColumnWidths());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableView::UpdateData()
{
   this->mc_Model.UpdateData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableView::SetNodeIndex(const stw_types::uint32 ou32_NodeIndex)
{
   this->mc_Model.SetNodeIndex(ou32_NodeIndex);
   this->sortByColumn(static_cast<sintn>(C_SdNdeCoOverviewTableModel::eINDEX), Qt::AscendingOrder);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the number of rows

   \return
   Number of items in table
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sintn C_SdNdeCoOverviewTableView::GetCountRows() const
{
   return this->mc_Model.rowCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Track mouse hover

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableView::mouseMoveEvent(QMouseEvent * const opc_Event)
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
void C_SdNdeCoOverviewTableView::leaveEvent(QEvent * const opc_Event)
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
void C_SdNdeCoOverviewTableView::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   const QModelIndex c_ClickedIndexProxy = this->indexAt(opc_Event->pos());

   QTableView::mouseDoubleClickEvent(opc_Event);
   if (c_ClickedIndexProxy.isValid() == true)
   {
      const QModelIndex c_ClickedIndex = this->mc_SortProxyModel.mapToSource(c_ClickedIndexProxy);
      if (c_ClickedIndex.isValid() == true)
      {
         uint8 u8_InterfaceNumber;
         stw_opensyde_core::C_OSCCanInterfaceId c_CanInterfaceId;
         uint32 u32_UseCaseIndex;
         C_SdNdeCoOverviewTableModel::E_NodeType e_NodeType;

         const sint32 s32_Error = this->mc_Model.MapRowToNodeConfig(
            c_ClickedIndex.row(), e_NodeType, u8_InterfaceNumber, c_CanInterfaceId);

         if (s32_Error == C_NO_ERR)
         {
            if (e_NodeType == C_SdNdeCoOverviewTableModel::E_NodeType::eMANAGER)
            {
               Q_EMIT (this->SigInterfaceSelected(u8_InterfaceNumber));
            }
            else
            {
               u32_UseCaseIndex = 0U; // for the config page
               Q_EMIT (this->SigDeviceSelected(u8_InterfaceNumber, c_CanInterfaceId, u32_UseCaseIndex));
            }
         }
         else
         {
            // take default
            u8_InterfaceNumber = 0U;
            Q_EMIT (this->SigInterfaceSelected(u8_InterfaceNumber));
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init default columns
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewTableView::m_InitColumns()
{
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoOverviewTableModel::eINDEX), 44);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoOverviewTableModel::eINTERFACE), 120);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoOverviewTableModel::eNODE), 140);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoOverviewTableModel::eROLE), 120);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoOverviewTableModel::eCANOPENNODEID), 120);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoOverviewTableModel::eTPDOS), 90);
   this->setColumnWidth(static_cast<sintn>(C_SdNdeCoOverviewTableModel::eRPDOS), 90);
}
