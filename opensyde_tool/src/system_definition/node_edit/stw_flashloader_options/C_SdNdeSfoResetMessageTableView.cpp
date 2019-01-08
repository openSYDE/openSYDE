//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       short description (implementation)

   detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.12.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QScrollBar>
#include <QHeaderView>
#include "C_SdNdeSfoResetMessageTableView.h"
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

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     07.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeSfoResetMessageTableView::C_SdNdeSfoResetMessageTableView(QWidget * const opc_Parent) :
   //Do not use common base class because of spacing issue at bottom
   QTableView(opc_Parent)
{
   //UI Settings
   this->setCornerButtonEnabled(false);
   this->setGridStyle(Qt::NoPen);
   this->setShowGrid(false);
   this->setSelectionMode(QAbstractItemView::NoSelection);
   this->setDragDropMode(QAbstractItemView::NoDragDrop);
   this->setDefaultDropAction(Qt::DropAction::MoveAction);
   this->setDragEnabled(false);
   this->setLineWidth(0);
   this->setFrameShadow(QAbstractItemView::Plain);
   this->setFrameShape(QAbstractItemView::NoFrame);
   this->setEditTriggers(
      QAbstractItemView::DoubleClicked | QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked);
   //Consider all elements for resize
   this->horizontalHeader()->setResizeContentsPrecision(-1);
   this->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   this->horizontalHeader()->setStretchLastSection(false);
   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->horizontalScrollBar()->hide();
   //Row Height
   this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   this->verticalHeader()->setMinimumSectionSize(40);
   this->verticalHeader()->setMaximumSectionSize(40);
   this->verticalHeader()->setDefaultSectionSize(40);
   // configure the scrollbar to only show up if size was miscalculated
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

   //Icon
   this->setIconSize(mc_ICON_SIZE_24);

   //Hide vertical header
   this->verticalHeader()->hide();

   this->setModel(&mc_Model);
   this->setItemDelegate(&mc_Delegate);

   connect(this->horizontalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SdNdeSfoResetMessageTableView::m_ScrollBarRangeChangedHor);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Specify associated node

   \param[in] ou32_NodeIndex Node index

   \created     07.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeSfoResetMessageTableView::SetNode(const stw_types::uint32 ou32_NodeIndex)
{
   this->mc_Model.SetIndex(ou32_NodeIndex);

   m_InitColumns();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize columns size

   \created     07.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeSfoResetMessageTableView::m_InitColumns(void)
{
   //Table size
   this->setColumnWidth(C_SdNdeSfoResetMessageTableModel::h_EnumToColumn(C_SdNdeSfoResetMessageTableModel::eID), 90);
   this->setColumnWidth(C_SdNdeSfoResetMessageTableModel::h_EnumToColumn(C_SdNdeSfoResetMessageTableModel::
                                                                         eEXTENDED_FORMAT), 70);
   this->setColumnWidth(C_SdNdeSfoResetMessageTableModel::h_EnumToColumn(C_SdNdeSfoResetMessageTableModel::eDLC), 40);
   this->setColumnWidth(C_SdNdeSfoResetMessageTableModel::h_EnumToColumn(C_SdNdeSfoResetMessageTableModel::eBYTE1), 50);
   this->setColumnWidth(C_SdNdeSfoResetMessageTableModel::h_EnumToColumn(C_SdNdeSfoResetMessageTableModel::eBYTE2), 50);
   this->setColumnWidth(C_SdNdeSfoResetMessageTableModel::h_EnumToColumn(C_SdNdeSfoResetMessageTableModel::eBYTE3), 50);
   this->setColumnWidth(C_SdNdeSfoResetMessageTableModel::h_EnumToColumn(C_SdNdeSfoResetMessageTableModel::eBYTE4), 50);
   this->setColumnWidth(C_SdNdeSfoResetMessageTableModel::h_EnumToColumn(C_SdNdeSfoResetMessageTableModel::eBYTE5), 50);
   this->setColumnWidth(C_SdNdeSfoResetMessageTableModel::h_EnumToColumn(C_SdNdeSfoResetMessageTableModel::eBYTE6), 50);
   this->setColumnWidth(C_SdNdeSfoResetMessageTableModel::h_EnumToColumn(C_SdNdeSfoResetMessageTableModel::eBYTE7), 50);
   this->setColumnWidth(C_SdNdeSfoResetMessageTableModel::h_EnumToColumn(C_SdNdeSfoResetMessageTableModel::eBYTE8), 50);
}

//-----------------------------------------------------------------------------
void C_SdNdeSfoResetMessageTableView::m_ScrollBarRangeChangedHor(const sintn osn_Min, const sintn osn_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((osn_Min == 0) && (osn_Max == 0))
   {
      this->horizontalScrollBar()->hide();
   }
   else
   {
      this->horizontalScrollBar()->show();
   }
}
