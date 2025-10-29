//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Log Jobs overview table view

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QMouseEvent>

#include "stwtypes.hpp"
#include "C_SdNdeDalLogJobsOvTableView.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_UsHandler.hpp"
#include "C_SdNdeSingleHeaderView.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
/*! \brief  Default constructor

   \param[in,out]  opc_Parent    Parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobsOvTableView::C_SdNdeDalLogJobsOvTableView(QWidget * const opc_Parent) :
   C_TblViewScroll(opc_Parent)
{
   QItemSelectionModel * const pc_LastSelectionModel = this->selectionModel();

   this->mc_SortProxyModel.setSourceModel(&mc_Model);
   this->mc_SortProxyModel.setSortRole(static_cast<int32_t>(Qt::EditRole));
   this->C_SdNdeDalLogJobsOvTableView::setModel(&mc_SortProxyModel);
   //Delete last selection model, see Qt documentation for setModel
   delete pc_LastSelectionModel;

   //Drawing delegate
   this->mc_Delegate.SetModel(&mc_Model);
   this->setItemDelegate(&mc_Delegate);

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->horizontalScrollBar()->hide();

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

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
   this->horizontalHeader()->resizeSection(C_SdNdeDalLogJobsOvTableModel::h_EnumToColumn(C_SdNdeDalLogJobsOvTableModel::
                                                                                         eNAME),
                                           44);
   this->setIconSize(QSize(21, 21));

   //Hide vertical header
   this->verticalHeader()->hide();

   //Hover event
   this->setMouseTracking(true);

   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SdNdeDalLogJobsOvTableView::m_ShowHideVerticalScrollBar);
   connect(this->horizontalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SdNdeDalLogJobsOvTableView::m_ShowHideHorizontalScrollBar);
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobsOvTableView::~C_SdNdeDalLogJobsOvTableView()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data

   \param[in]  orc_AllLoggerJobElementsLocationCount     vector holds all Logjobs Index, Local elements count, remote elements count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvTableView::UpdateData(const std::vector<std::tuple<uint32_t, uint32_t,
                                                                           uint32_t> > & orc_AllLoggerJobElementsLocationCount)
{
   this->mc_Model.UpdateData(orc_AllLoggerJobElementsLocationCount);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the number of rows

   \return
   Count of items in table
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDalLogJobsOvTableView::GetCountRows() const
{
   return this->mc_Model.rowCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvTableView::SetNodeIndex(const uint32_t ou32_NodeIndex)
{
   this->mc_Model.SetNodeIndex(ou32_NodeIndex);
   this->sortByColumn(C_SdNdeDalLogJobsOvTableModel::h_EnumToColumn(
                         C_SdNdeDalLogJobsOvTableModel::eINDEX), Qt::AscendingOrder);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvTableView::LoadUserSettings()
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mc_Model.GetNodeIndex());

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
void C_SdNdeDalLogJobsOvTableView::SaveUserSettings() const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mc_Model.GetNodeIndex());

   if (pc_Node != NULL)
   {
      C_UsHandler::h_GetInstance()->SetProjSdNodeHalcOverviewColumnWidth(
         pc_Node->c_Properties.c_Name.c_str(), this->m_GetColumnWidths());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Track mouse hover

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvTableView::mouseMoveEvent(QMouseEvent * const opc_Event)
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
void C_SdNdeDalLogJobsOvTableView::leaveEvent(QEvent * const opc_Event)
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
void C_SdNdeDalLogJobsOvTableView::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   const QModelIndex c_ClickedIndexProxy = this->indexAt(opc_Event->pos());

   QTableView::mouseDoubleClickEvent(opc_Event);
   if (c_ClickedIndexProxy.isValid() == true)
   {
      const QModelIndex c_ClickedIndex = this->mc_SortProxyModel.mapToSource(c_ClickedIndexProxy);
      if (c_ClickedIndex.isValid() == true)
      {
         const QModelIndex c_FirstColumnIndex = this->mc_Model.index(c_ClickedIndex.row(), 0);
         const QVariant c_Data = this->mc_Model.data(c_FirstColumnIndex, static_cast<int32_t>(Qt::DisplayRole));
         Q_EMIT this->SigLoadSelectedLogJob(static_cast<int32_t>(c_Data.toInt() - 1));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init default columns
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvTableView::m_InitColumns()
{
   this->setColumnWidth(C_SdNdeDalLogJobsOvTableModel::h_EnumToColumn(C_SdNdeDalLogJobsOvTableModel::eINDEX), 44);
   this->setColumnWidth(C_SdNdeDalLogJobsOvTableModel::h_EnumToColumn(C_SdNdeDalLogJobsOvTableModel::eICON), 44);
   this->setColumnWidth(C_SdNdeDalLogJobsOvTableModel::h_EnumToColumn(C_SdNdeDalLogJobsOvTableModel::eNAME), 174);
   this->setColumnWidth(C_SdNdeDalLogJobsOvTableModel::h_EnumToColumn(C_SdNdeDalLogJobsOvTableModel::eCOMMENT), 174);
   this->setColumnWidth(C_SdNdeDalLogJobsOvTableModel::h_EnumToColumn(C_SdNdeDalLogJobsOvTableModel::eUSE_CASE), 139);
   this->setColumnWidth(C_SdNdeDalLogJobsOvTableModel::h_EnumToColumn(C_SdNdeDalLogJobsOvTableModel::eTRIGGERMODE),
                        139);
   this->setColumnWidth(C_SdNdeDalLogJobsOvTableModel::h_EnumToColumn(C_SdNdeDalLogJobsOvTableModel::eLOGGINGDATA),
                        115);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show hide vertical scroll bar

   \param[in]  os32_Min  Minimum range
   \param[in]  os32_Max  Maximum range
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvTableView::m_ShowHideVerticalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((os32_Min == 0) && (os32_Max == 0))
   {
      this->verticalScrollBar()->hide();
   }
   else
   {
      this->verticalScrollBar()->show();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show hide horizontal scroll bar

   \param[in]  os32_Min  Minimum range
   \param[in]  os32_Max  Maximum range
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvTableView::m_ShowHideHorizontalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((os32_Min == 0) && (os32_Max == 0))
   {
      this->horizontalScrollBar()->hide();
   }
   else
   {
      this->horizontalScrollBar()->show();
   }
}
