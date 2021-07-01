//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View for project file table (implementation)

   View for project file table

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
#include "C_PopFileTableView.h"
#include "constants.h"
#include "C_OgeWiUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;

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
C_PopFileTableView::C_PopFileTableView(QWidget * const opc_Parent) :
   C_TblViewToolTipBase(opc_Parent)
{
   //Model
   this->C_PopFileTableView::setModel(&mc_Model);

   //Drawing delegate
   this->mc_Delegate.SetModel(&mc_Model);
   this->setItemDelegate(&mc_Delegate);

   //UI Settings
   this->setCornerButtonEnabled(false);
   this->setSortingEnabled(false);
   this->setGridStyle(Qt::NoPen);
   this->setShowGrid(false);
   this->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
   this->setDragEnabled(false);
   this->setLineWidth(0);
   this->setFrameShadow(QAbstractItemView::Shadow::Plain);
   this->setFrameShape(QAbstractItemView::Shape::NoFrame);
   this->setEditTriggers(EditTrigger::NoEditTriggers);
   //Consider all elements for resize
   this->horizontalHeader()->setResizeContentsPrecision(-1);
   //Columns
   this->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   this->horizontalHeader()->setSectionResizeMode(C_PopFileTableModel::h_EnumToColumn(
                                                     C_PopFileTableModel::ePATH), QHeaderView::Stretch);
   this->setColumnWidth(C_PopFileTableModel::h_EnumToColumn(C_PopFileTableModel::eNAME), 315);
   this->setColumnWidth(C_PopFileTableModel::h_EnumToColumn(C_PopFileTableModel::eVERSION), 80);
   this->setColumnWidth(C_PopFileTableModel::h_EnumToColumn(C_PopFileTableModel::ePATH), 200);

   //Row Height
   this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   this->verticalHeader()->setDefaultSectionSize(40); // note: text only gets wrapped if a second line fits in this size

   //Hide vertical header
   this->verticalHeader()->hide();
   this->horizontalHeader()->hide();

   // no horizontal scrollbar needed --> do not reserve space for it
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

   //Hover event
   this->setMouseTracking(true);

   //update style of scrollbars (they wont get styled at all without this)
   this->verticalScrollBar()->style()->unpolish(this->verticalScrollBar());
   this->verticalScrollBar()->style()->polish(this->verticalScrollBar());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal data changes to table

   \param[in]  orc_RecentFilePaths  Recent file paths
   \param[in]  orc_RecentProjects   Recent projects
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopFileTableView::UpdateData(const std::vector<QString> & orc_RecentFilePaths,
                                    const std::vector<stw_opensyde_core::C_OSCProject> & orc_RecentProjects)
{
   this->mc_Model.UpdateData(orc_RecentFilePaths, orc_RecentProjects);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert row to message

   \param[in]   ors32_Row     Model row
   \param[out]  orc_FilePath  File path

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PopFileTableView::ConvertRowToFile(const sint32 & ors32_Row, QString & orc_FilePath) const
{
   return this->mc_Model.ConvertRowToFile(ors32_Row, orc_FilePath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Track mouse hover

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopFileTableView::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   sint32 s32_HoveredRow = -1;
   const QModelIndex c_HoveredIndex = this->indexAt(opc_Event->pos());

   C_TblViewToolTipBase::mouseMoveEvent(opc_Event);

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
/*! \brief   Overwritten mouse press event slot

   Here: Paint background

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopFileTableView::mousePressEvent(QMouseEvent * const opc_Event)
{
   sint32 s32_ClickedRow = -1;
   const QModelIndex c_ClickedIndex = this->indexAt(opc_Event->pos());

   if (opc_Event->button() == Qt::LeftButton)
   {
      if (c_ClickedIndex.isValid() == true)
      {
         s32_ClickedRow = c_ClickedIndex.row();
      }
      if (this->mc_Delegate.SetClickedRow(s32_ClickedRow) == true)
      {
         this->update();
      }
   }

   C_TblViewToolTipBase::mousePressEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse release event slot

   Here: Paint background

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------

void C_PopFileTableView::mouseReleaseEvent(QMouseEvent * const opc_Event)
{
   C_TblViewToolTipBase::mouseReleaseEvent(opc_Event);

   if (this->mc_Delegate.SetClickedRow(-1) == true)
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
void C_PopFileTableView::leaveEvent(QEvent * const opc_Event)
{
   C_TblViewToolTipBase::leaveEvent(opc_Event);
   if ((this->mc_Delegate.SetHoveredRow(-1) == true))
   {
      this->update();
   }
}
