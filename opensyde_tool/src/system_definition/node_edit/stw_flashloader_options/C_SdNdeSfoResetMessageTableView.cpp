//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QScrollBar>
#include <QHeaderView>
#include "C_SdNdeSfoResetMessageTableView.h"
#include "constants.h"

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
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeSfoResetMessageTableView::C_SdNdeSfoResetMessageTableView(QWidget * const opc_Parent) :
   C_TblViewScroll(opc_Parent)
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
   //Row Height
   this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   this->verticalHeader()->setMinimumSectionSize(30);
   this->verticalHeader()->setMaximumSectionSize(30);
   this->verticalHeader()->setDefaultSectionSize(30);

   //Hide vertical header
   this->verticalHeader()->hide();

   this->setModel(&mc_Model);
   this->setItemDelegate(&mc_Delegate);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Specify associated node

   \param[in] ou32_NodeIndex Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeSfoResetMessageTableView::SetNode(const stw_types::uint32 ou32_NodeIndex)
{
   this->mc_Model.SetIndex(ou32_NodeIndex);

   m_InitColumns();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize columns size
*/
//----------------------------------------------------------------------------------------------------------------------
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
