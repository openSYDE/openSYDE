//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view for system view dashboard data element update mode (implementation)

   Table view for system view dashboard data element update mode

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QScrollBar>
#include "C_OgeWiUtil.hpp"
#include "C_SyvDaPeUpdateModeTableView.hpp"
#include "C_SyvDaPeUpdateModeTableModel.hpp"
#include "C_SyvDaPeUpdateModeTableHeaderView.hpp"
#include "constants.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]     ou32_ViewIndex     View index
   \param[in,out] opc_TreeWidget     Complete tree widget
   \param[in,out] opc_TreeWidgetItem Tree widget item
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeUpdateModeTableView::C_SyvDaPeUpdateModeTableView(const uint32_t ou32_ViewIndex,
                                                           QTreeWidget * const opc_TreeWidget,
                                                           QTreeWidgetItem * const opc_TreeWidgetItem) :
   C_TblViewScroll(),
   mpc_TreeWidget(opc_TreeWidget),
   mpc_TreeWidgetItem(opc_TreeWidgetItem),
   mc_Delegate(ou32_ViewIndex)
{
   //UI Settings
   this->setCornerButtonEnabled(false);
   this->setSortingEnabled(false);
   this->setGridStyle(Qt::NoPen);
   this->setShowGrid(false);
   this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
   this->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
   this->setAlternatingRowColors(true);
   this->setDragDropMode(QAbstractItemView::NoDragDrop);
   this->setDefaultDropAction(Qt::DropAction::MoveAction);
   this->setDragEnabled(false);
   this->setLineWidth(0);
   this->setFrameShadow(QAbstractItemView::Shadow::Plain);
   this->setFrameShape(QAbstractItemView::Shape::NoFrame);
   this->setEditTriggers(
      QAbstractItemView::DoubleClicked /* | QAbstractItemView::CurrentChanged*/ | QAbstractItemView::AnyKeyPressed |
      QAbstractItemView::EditKeyPressed);
   //Consider all elements for resize
   this->setHorizontalHeader(new C_SyvDaPeUpdateModeTableHeaderView(Qt::Horizontal));
   this->horizontalHeader()->setResizeContentsPrecision(-1);
   this->horizontalHeader()->setStretchLastSection(true);
   this->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
   this->horizontalHeader()->setFixedHeight(40);
   //Row Height
   this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   this->verticalHeader()->setMinimumSectionSize(30);
   this->verticalHeader()->setMaximumSectionSize(30);
   this->verticalHeader()->setDefaultSectionSize(30);
   //Icon
   this->setIconSize(QSize(16, 16));

   //Hide vertical header
   this->verticalHeader()->hide();

   //Delegate
   this->setItemDelegate(&mc_Delegate);

   //Avoid styling table inside
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->verticalScrollBar(), "C_SyvDaPeUpdateModeTableView", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->horizontalScrollBar(), "C_SyvDaPeUpdateModeTableView", true);

   //Hover event
   this->setMouseTracking(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeUpdateModeTableView::~C_SyvDaPeUpdateModeTableView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init default columns
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeTableView::InitColumns(void)
{
   this->setColumnWidth(C_SyvDaPeUpdateModeTableModel::h_EnumToColumn(C_SyvDaPeUpdateModeTableModel::eICON), 26);
   this->setColumnWidth(C_SyvDaPeUpdateModeTableModel::h_EnumToColumn(C_SyvDaPeUpdateModeTableModel::eINDEX), 26);
   this->setColumnWidth(C_SyvDaPeUpdateModeTableModel::h_EnumToColumn(C_SyvDaPeUpdateModeTableModel::eNAME), 400);
   this->setColumnWidth(C_SyvDaPeUpdateModeTableModel::h_EnumToColumn(
                           C_SyvDaPeUpdateModeTableModel::eTRANSMISSION_MODE), 150);
   this->setColumnWidth(C_SyvDaPeUpdateModeTableModel::h_EnumToColumn(
                           C_SyvDaPeUpdateModeTableModel::eCYCLIC_INTERVAL), 150);
   this->setColumnWidth(C_SyvDaPeUpdateModeTableModel::h_EnumToColumn(C_SyvDaPeUpdateModeTableModel::eTHRESHOLD), 150);
   this->setColumnWidth(C_SyvDaPeUpdateModeTableModel::h_EnumToColumn(C_SyvDaPeUpdateModeTableModel::eUSAGE), 103);

   //Always hide those
   this->setColumnWidth(C_SyvDaPeUpdateModeTableModel::h_EnumToColumn(C_SyvDaPeUpdateModeTableModel::eVALUE_TYPE), 85);
   this->setColumnWidth(C_SyvDaPeUpdateModeTableModel::h_EnumToColumn(C_SyvDaPeUpdateModeTableModel::eARRAY_SIZE), 85);
   this->setColumnWidth(C_SyvDaPeUpdateModeTableModel::h_EnumToColumn(C_SyvDaPeUpdateModeTableModel::eDATA_POOL), 200);
   this->hideColumn(C_SyvDaPeUpdateModeTableModel::h_EnumToColumn(C_SyvDaPeUpdateModeTableModel::eVALUE_TYPE));
   this->hideColumn(C_SyvDaPeUpdateModeTableModel::h_EnumToColumn(C_SyvDaPeUpdateModeTableModel::eARRAY_SIZE));
   this->hideColumn(C_SyvDaPeUpdateModeTableModel::h_EnumToColumn(C_SyvDaPeUpdateModeTableModel::eDATA_POOL));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust widget size to items

   \param[in]  orq_Initial    Flag if initial call
   \param[in]  oq_Last        Flag if last call
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeTableView::AdjustToItems(const bool & orq_Initial, const bool oq_Last)
{
   if (((this->mpc_TreeWidgetItem != NULL) && (this->mpc_TreeWidget != NULL)) && (this->model() != NULL))
   {
      //Configure
      const int32_t s32_CONST_OFFSET = 50;
      const int32_t s32_ITEM_OFFSET = 30;
      const uint32_t u32_VISIBLE_ITEM_COUNT = 7U;
      const uint32_t u32_ItemCount = this->model()->rowCount();
      int32_t s32_Height = s32_CONST_OFFSET;

      //Adapt dynamic elements
      if ((u32_ItemCount <= u32_VISIBLE_ITEM_COUNT) || (oq_Last == true))
      {
         s32_Height += s32_ITEM_OFFSET * static_cast<int32_t>(u32_ItemCount);
      }
      else
      {
         s32_Height += s32_ITEM_OFFSET * static_cast<int32_t>(u32_VISIBLE_ITEM_COUNT);
      }
      //Change size
      this->setMinimumHeight(s32_Height);
      this->setMaximumHeight(s32_Height);
      this->mpc_TreeWidgetItem->setSizeHint(0, QSize(this->mpc_TreeWidget->width(), s32_Height));
      if (orq_Initial == false)
      {
         this->mpc_TreeWidget->adjustSize();
      }
   }
}
