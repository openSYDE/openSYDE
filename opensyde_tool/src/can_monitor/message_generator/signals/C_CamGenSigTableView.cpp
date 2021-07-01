//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View for message generator signals table (implementation)

   View for message generator signals table

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QSpinBox>
#include <QScrollBar>
#include <QHeaderView>
#include "C_OgeWiSpinBoxGroup.h"

#include "stwtypes.h"
#include "C_UsHandler.h"
#include "C_CamGenSigTableView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamGenSigTableView::C_CamGenSigTableView(QWidget * const opc_Parent) :
   C_TblViewToolTipBase(opc_Parent)
{
   QItemSelectionModel * const pc_LastSelectionModel = this->selectionModel();

   this->mc_SortProxyModel.setSourceModel(&mc_Model);
   this->mc_SortProxyModel.setSortRole(static_cast<sintn>(Qt::EditRole));
   this->C_CamGenSigTableView::setModel(&mc_SortProxyModel);
   //Delete last selection model, see Qt documentation for setModel
   delete pc_LastSelectionModel;

   this->setItemDelegate(&this->mc_Delegate);

   this->verticalHeader()->setVisible(false);

   //Configure
   this->setShowGrid(false);
   this->setSelectionMode(QAbstractItemView::SingleSelection);

   //Deactivate
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   //Style
   this->verticalHeader()->setDefaultSectionSize(30);
   this->horizontalHeader()->setFixedHeight(27);

   //Buttons
   this->mpc_PushButtonScrollTop = new C_OgePubIconOnly(this->verticalScrollBar());
   // Size is necessary for correct drawing of the images
   this->mpc_PushButtonScrollTop->setGeometry(0, 0, 25, 27);
   this->mpc_PushButtonScrollTop->SetCustomIcons("://images/HeadingPart.svg", "", "", "");

   connect(&this->mc_Model, &C_CamGenSigTableModel::SigUpdateMessageData, this,
           &C_CamGenSigTableView::SigUpdateMessageData);
   connect(&this->mc_Model, &C_CamGenSigTableModel::SigResetPermanentEditors, this,
           &C_CamGenSigTableView::m_SetPersistentEditorWidgets);
   connect(&this->mc_Model, &C_CamGenSigTableModel::SigTriggerModelUpdateCyclicMessage, this,
           &C_CamGenSigTableView::SigTriggerModelUpdateCyclicMessage);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540} never took ownership of any button
C_CamGenSigTableView::~C_CamGenSigTableView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle signal reload on external trigger
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigTableView::TriggerSignalReload(void)
{
   this->mc_Model.TriggerSignalReload();
   m_SetPersistentEditorWidgets();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set current message index

   \param[in] ou32_Message Message index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigTableView::SetMessage(const uint32 ou32_Message)
{
   this->mc_Model.SetMessage(ou32_Message);
   m_SetPersistentEditorWidgets();
   //Always sort by SB
   //Complex handling for column to handle moved columns
   this->mc_SortProxyModel.sort(this->mc_SortProxyModel.mapFromSource(this->mc_Model.index(0, 0)).column());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update the message DLC

   \param[in] ou32_MessageIndex Message index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigTableView::UpdateMessageDLC(const uint32 ou32_MessageIndex)
{
   this->mc_Model.UpdateMessageDLC(ou32_MessageIndex);
   m_SetPersistentEditorWidgets();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restore column widths

   \param[in] orc_ColumnWidths Stored column widths (Restores default values if empty)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigTableView::SetCurrentColumnWidths(const std::vector<sint32> & orc_ColumnWidths)
{
   sint32 s32_CurColIndex;

   if (orc_ColumnWidths.size() > 0)
   {
      //Restore all saved values
      for (uint32 u32_ItCol = 0; u32_ItCol < orc_ColumnWidths.size(); ++u32_ItCol)
      {
         this->setColumnWidth(static_cast<sintn>(u32_ItCol), orc_ColumnWidths[u32_ItCol]);
      }
      //Also handle the remaining ones
   }
   //Apply default wherever there was no stored value
   s32_CurColIndex = C_CamGenSigTableModel::h_EnumToColumn(C_CamGenSigTableModel::eBIT_POS);
   if ((static_cast<uint32>(s32_CurColIndex) >= orc_ColumnWidths.size()) ||
       (orc_ColumnWidths[static_cast<uint32>(s32_CurColIndex)] == 0))
   {
      this->setColumnWidth(s32_CurColIndex, 50);
   }
   s32_CurColIndex = C_CamGenSigTableModel::h_EnumToColumn(C_CamGenSigTableModel::eNAME);
   if ((static_cast<uint32>(s32_CurColIndex) >= orc_ColumnWidths.size()) ||
       (orc_ColumnWidths[static_cast<uint32>(s32_CurColIndex)] == 0))
   {
      this->setColumnWidth(s32_CurColIndex, 200);
   }
   s32_CurColIndex = C_CamGenSigTableModel::h_EnumToColumn(C_CamGenSigTableModel::eRAW);
   if ((static_cast<uint32>(s32_CurColIndex) >= orc_ColumnWidths.size()) ||
       (orc_ColumnWidths[static_cast<uint32>(s32_CurColIndex)] == 0))
   {
      this->setColumnWidth(s32_CurColIndex, 100);
   }
   s32_CurColIndex = C_CamGenSigTableModel::h_EnumToColumn(C_CamGenSigTableModel::ePHYSICAL);
   if ((static_cast<uint32>(s32_CurColIndex) >= orc_ColumnWidths.size()) ||
       (orc_ColumnWidths[static_cast<uint32>(s32_CurColIndex)] == 0))
   {
      this->setColumnWidth(s32_CurColIndex, 100);
   }
   s32_CurColIndex = C_CamGenSigTableModel::h_EnumToColumn(C_CamGenSigTableModel::eUNIT);
   if ((static_cast<uint32>(s32_CurColIndex) >= orc_ColumnWidths.size()) ||
       (orc_ColumnWidths[static_cast<uint32>(s32_CurColIndex)] == 0))
   {
      this->setColumnWidth(s32_CurColIndex, 100);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current column widths

   \return
   Current column widths
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sint32> C_CamGenSigTableView::GetCurrentColumnWidths(void) const
{
   std::vector<sint32> c_Retval;
   c_Retval.reserve(this->model()->columnCount());
   for (sint32 s32_ItCol = 0; s32_ItCol < this->model()->columnCount(); ++s32_ItCol)
   {
      c_Retval.push_back(this->columnWidth(s32_ItCol));
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigTableView::SaveUserSettings(void) const
{
   C_UsHandler::h_GetInstance()->SetSignalsColWidths(this->GetCurrentColumnWidths());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigTableView::LoadUserSettings(void)
{
   this->SetCurrentColumnWidths(C_UsHandler::h_GetInstance()->GetSignalsColWidths());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten show event slot

   Here: move scroll bar buttons

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigTableView::showEvent(QShowEvent * const opc_Event)
{
   C_TblViewToolTipBase::showEvent(opc_Event);
   m_RepositionButtons();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten key press event slot

   Here: Forward necessary keys

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigTableView::keyPressEvent(QKeyEvent * const opc_Event)
{
   C_TblViewToolTipBase::keyPressEvent(opc_Event);
   //Check if active editor
   const QSpinBox * const pc_SpinBox = dynamic_cast<const QSpinBox * const>(this->focusWidget());
   const C_OgeWiSpinBoxGroup * const pc_SpinBoxGroup =
      dynamic_cast<const C_OgeWiSpinBoxGroup * const>(this->focusWidget());
   const QLineEdit * const pc_LineEdit = dynamic_cast<const QLineEdit * const>(this->focusWidget());
   const QComboBox * const pc_ComboBox = dynamic_cast<const QComboBox * const>(this->focusWidget());
   if ((((pc_SpinBox == NULL) && (pc_LineEdit == NULL)) && (pc_ComboBox == NULL)) && (pc_SpinBoxGroup == NULL))
   {
      //For some reason the tab key needs to be accepted to allow proper tab navigation
      if (opc_Event->key() != static_cast<sintn>(Qt::Key_Tab))
      {
         //Ignore event if no widget can accept it
         opc_Event->ignore();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten resize event slot

   Here: move scroll bar buttons

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigTableView::resizeEvent(QResizeEvent * const opc_Event)
{
   C_TblViewToolTipBase::resizeEvent(opc_Event);
   m_RepositionButtons();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reposition the buttons as necessary
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigTableView::m_RepositionButtons(void)
{
   const QSize c_ButtonSizeUp(25, 27);

   //Position
   this->mpc_PushButtonScrollTop->setGeometry(QRect(QPoint(0, 0), c_ButtonSizeUp));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update the persistent editor widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigTableView::m_SetPersistentEditorWidgets(void)
{
   //Set static editor widgets
   for (sint32 s32_Row = 0; s32_Row < this->mc_Model.rowCount(); ++s32_Row)
   {
      //Each relevant column
      const sint32 s32_Col1 = C_CamGenSigTableModel::h_EnumToColumn(C_CamGenSigTableModel::eRAW);
      const sint32 s32_Col2 = C_CamGenSigTableModel::h_EnumToColumn(C_CamGenSigTableModel::ePHYSICAL);
      const QModelIndex c_Index1 = this->mc_SortProxyModel.mapFromSource(this->mc_Model.index(s32_Row, s32_Col1));
      const QModelIndex c_Index2 = this->mc_SortProxyModel.mapFromSource(this->mc_Model.index(s32_Row, s32_Col2));
      this->openPersistentEditor(c_Index1);
      this->openPersistentEditor(c_Index2);
   }
}
