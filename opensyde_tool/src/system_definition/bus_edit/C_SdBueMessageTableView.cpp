//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message table view (implementation)

   Message table view

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QHeaderView>
#include <QMouseEvent>
#include <QScrollBar>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SdBueMessageTableView.hpp"
#include "C_SdBueMessageSignalTableDelegate.hpp"
#include "C_SdNdeSingleHeaderView.hpp"
#include "constants.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

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
   C_TblViewScroll(opc_Parent),
   mpc_SyncManager(NULL)
{
   QItemSelectionModel * const pc_LastSelectionModel = this->selectionModel();

   this->mc_SortProxyModel.setSourceModel(&mc_Model);
   this->mc_SortProxyModel.setSortRole(static_cast<int32_t>(Qt::EditRole));
   this->C_SdBueMessageTableView::setModel(&mc_SortProxyModel);
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
//lint -e{1540} never took ownership of any item
C_SdBueMessageTableView::~C_SdBueMessageTableView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings

   \param[in]  orc_Values  Values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageTableView::LoadUserSettings(const std::vector<int32_t> & orc_Values)
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
void C_SdBueMessageTableView::SaveUserSettings(std::vector<int32_t> & orc_Values) const
{
   const std::map<C_SdBueMessageTableModel::E_Columns,
                  uint32_t> c_DefaultColumnWidths = C_SdBueMessageTableView::mh_GetDefaultColumnWidths();

   orc_Values = this->m_GetColumnWidths();
   for (uint32_t u32_It = 0UL; u32_It < orc_Values.size(); ++u32_It)
   {
      if (orc_Values[u32_It] == 0L)
      {
         const std::map<C_SdBueMessageTableModel::E_Columns,
                        uint32_t>::const_iterator c_It = c_DefaultColumnWidths.find(C_SdBueMessageTableModel::h_ColumnToEnum(
                                                                                       u32_It));
         if (c_It != c_DefaultColumnWidths.cend())
         {
            //Column hidden, use default instead
            orc_Values[u32_It] = static_cast<int32_t>(c_It->second);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message sync manager

   \param[in,out]  opc_Value  Message sync manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageTableView::SetMessageSyncManager(
   stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value)
{
   this->mpc_SyncManager = opc_Value;
   this->mc_Model.SetMessageSyncManager(opc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal data changes to table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageTableView::UpdateData(void)
{
   this->mc_Model.UpdateData();
   this->m_HandleColumnVisibility();
   this->sortByColumn(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eINDEX), Qt::AscendingOrder);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the number of rows

   \return
   Count of items in table
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueMessageTableView::GetCountRows(void) const
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
   int32_t s32_HoveredRow = -1;
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
         C_OscCanMessageIdentificationIndices c_MessageId;
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
   const
   std::map<C_SdBueMessageTableModel::E_Columns,
            uint32_t> c_ColumnWidths = C_SdBueMessageTableView::mh_GetDefaultColumnWidths();

   for (std::map<C_SdBueMessageTableModel::E_Columns,
                 uint32_t>::const_iterator c_ItEntry = c_ColumnWidths.cbegin(); c_ItEntry != c_ColumnWidths.cend();
        ++c_ItEntry)
   {
      this->setColumnWidth(C_SdBueMessageTableModel::h_EnumToColumn(c_ItEntry->first), c_ItEntry->second);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get default column widths

   \return
   Default column widths
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<C_SdBueMessageTableModel::E_Columns, uint32_t> C_SdBueMessageTableView::mh_GetDefaultColumnWidths()
{
   std::map<C_SdBueMessageTableModel::E_Columns, uint32_t> c_ColumnWidths;
   c_ColumnWidths[C_SdBueMessageTableModel::eINDEX] = 40;
   c_ColumnWidths[C_SdBueMessageTableModel::eICON] = 26;
   c_ColumnWidths[C_SdBueMessageTableModel::eENABLED] = 54;
   c_ColumnWidths[C_SdBueMessageTableModel::eNAME] = 206;
   c_ColumnWidths[C_SdBueMessageTableModel::eCOMMENT] = 206;
   c_ColumnWidths[C_SdBueMessageTableModel::eJ1939_PGN] = 56;
   c_ColumnWidths[C_SdBueMessageTableModel::eJ1939_PRIORITY] = 50;
   c_ColumnWidths[C_SdBueMessageTableModel::eJ1939_SOURCE_ADDRESS] = 94;
   c_ColumnWidths[C_SdBueMessageTableModel::eJ1939_DESTINATION_ADDRESS] = 118;
   c_ColumnWidths[C_SdBueMessageTableModel::eJ1939_FORMAT] = 50;
   c_ColumnWidths[C_SdBueMessageTableModel::eJ1939_EDP_AND_DP] = 55;
   c_ColumnWidths[C_SdBueMessageTableModel::eCAN_OPEN_INDEX] = 54;
   c_ColumnWidths[C_SdBueMessageTableModel::eEXTENDED] = 71;
   c_ColumnWidths[C_SdBueMessageTableModel::eCAN_ID] = 84;
   c_ColumnWidths[C_SdBueMessageTableModel::eCOB_ID] = 228;
   c_ColumnWidths[C_SdBueMessageTableModel::eDLC] = 44;
   c_ColumnWidths[C_SdBueMessageTableModel::eTX_METHOD] = 90;
   c_ColumnWidths[C_SdBueMessageTableModel::eCYCLE_TIME] = 105;
   c_ColumnWidths[C_SdBueMessageTableModel::eNOT_EARLIER_THAN] = 135;
   c_ColumnWidths[C_SdBueMessageTableModel::eNOT_LATER_THAN] = 145;
   c_ColumnWidths[C_SdBueMessageTableModel::eTRANSMITTER] = 90;
   c_ColumnWidths[C_SdBueMessageTableModel::eRECEIVER] = 106;
   return c_ColumnWidths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle column visibility
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageTableView::m_HandleColumnVisibility(void)
{
   if (this->mpc_SyncManager != NULL)
   {
      bool q_ShowEnabled;
      bool q_ShowCanOpenIndex;
      bool q_ShowCanId;
      bool q_ShowCobId;
      bool q_ShowJ1939;
      bool q_ShowCycleTime;
      if (this->mpc_SyncManager->GetCurrentComProtocol() == C_OscCanProtocol::eCAN_OPEN)
      {
         q_ShowEnabled = true;
         q_ShowCanOpenIndex = true;
         q_ShowCanId = false;
         q_ShowCobId = true;
         q_ShowCycleTime = false;
         q_ShowJ1939 = false;
      }
      else if (this->mpc_SyncManager->GetCurrentComProtocol() == C_OscCanProtocol::eJ1939)
      {
         q_ShowEnabled = false;
         q_ShowCanOpenIndex = false;
         q_ShowCanId = true;
         q_ShowCobId = false;
         q_ShowCycleTime = true;
         q_ShowJ1939 = true;
      }
      else
      {
         q_ShowEnabled = false;
         q_ShowCanOpenIndex = false;
         q_ShowCanId = true;
         q_ShowCobId = false;
         q_ShowCycleTime = true;
         q_ShowJ1939 = false;
      }
      this->setColumnHidden(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eENABLED),
                            !q_ShowEnabled);
      this->setColumnHidden(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eCAN_OPEN_INDEX),
                            !q_ShowCanOpenIndex);
      this->setColumnHidden(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eCAN_ID), !q_ShowCanId);
      this->setColumnHidden(C_SdBueMessageTableModel::h_EnumToColumn(C_SdBueMessageTableModel::eCOB_ID), !q_ShowCobId);
      this->setColumnHidden(C_SdBueMessageTableModel::h_EnumToColumn(
                               C_SdBueMessageTableModel::eCYCLE_TIME), !q_ShowCycleTime);
      this->setColumnHidden(C_SdBueMessageTableModel::h_EnumToColumn(
                               C_SdBueMessageTableModel::eJ1939_PGN), !q_ShowJ1939);
      this->setColumnHidden(C_SdBueMessageTableModel::h_EnumToColumn(
                               C_SdBueMessageTableModel::eJ1939_PRIORITY), !q_ShowJ1939);
      this->setColumnHidden(C_SdBueMessageTableModel::h_EnumToColumn(
                               C_SdBueMessageTableModel::eJ1939_SOURCE_ADDRESS), !q_ShowJ1939);
      this->setColumnHidden(C_SdBueMessageTableModel::h_EnumToColumn(
                               C_SdBueMessageTableModel::eJ1939_DESTINATION_ADDRESS), !q_ShowJ1939);
      this->setColumnHidden(C_SdBueMessageTableModel::h_EnumToColumn(
                               C_SdBueMessageTableModel::eJ1939_FORMAT), !q_ShowJ1939);
      this->setColumnHidden(C_SdBueMessageTableModel::h_EnumToColumn(
                               C_SdBueMessageTableModel::eJ1939_EDP_AND_DP), !q_ShowJ1939);
   }
}
