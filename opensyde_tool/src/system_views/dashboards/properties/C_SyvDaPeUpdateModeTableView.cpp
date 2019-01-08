//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Table view for system view dashboard data element update mode (implementation)

   Table view for system view dashboard data element update mode

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QScrollBar>
#include "C_OgeWiUtil.h"
#include "C_SyvDaPeUpdateModeTableView.h"
#include "C_SyvDaPeUpdateModeTableModel.h"
#include "C_SyvDaPeUpdateModeTableHeaderView.h"
#include "constants.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in]     ou32_ViewIndex     View index
   \param[in,out] opc_TreeWidget     Complete tree widget
   \param[in,out] opc_TreeWidgetItem Tree widget item

   \created     11.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaPeUpdateModeTableView::C_SyvDaPeUpdateModeTableView(const uint32 ou32_ViewIndex,
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
   //Row Height
   this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   this->verticalHeader()->setMinimumSectionSize(40);
   this->verticalHeader()->setMaximumSectionSize(40);
   this->verticalHeader()->setDefaultSectionSize(40);
   //Icon
   this->setIconSize(mc_ICON_SIZE_20);

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

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     05.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaPeUpdateModeTableView::~C_SyvDaPeUpdateModeTableView(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Init default columns

   \created     12.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Adjust widget size to items

   \param[in] orq_Initial Flag if initial call

   \created     08.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeUpdateModeTableView::AdjustToItems(const bool & orq_Initial, const bool oq_Last)
{
   if (((this->mpc_TreeWidgetItem != NULL) && (this->mpc_TreeWidget != NULL)) && (this->model() != NULL))
   {
      //Configure
      const sintn sn_ConstOffset = 60;
      const sintn sn_ItemOffset = 40;
      const uint32 u32_VisibleItemCount = 7;
      const uint32 u32_ItemCount = this->model()->rowCount();
      sintn sn_Height = sn_ConstOffset;

      //Adapt dynamic elements
      if ((u32_ItemCount <= u32_VisibleItemCount) || (oq_Last == true))
      {
         sn_Height += sn_ItemOffset * static_cast<sintn>(u32_ItemCount);
      }
      else
      {
         sn_Height += sn_ItemOffset * static_cast<sintn>(u32_VisibleItemCount);
      }
      //Change size
      this->setMinimumHeight(sn_Height);
      this->setMaximumHeight(sn_Height);
      this->mpc_TreeWidgetItem->setSizeHint(0, QSize(this->mpc_TreeWidget->width(), sn_Height));
      if (orq_Initial == false)
      {
         this->mpc_TreeWidget->adjustSize();
      }
   }
}
