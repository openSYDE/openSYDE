//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View (display) component for message generator table (implementation)

   View (display) component for message generator table

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QSpinBox>
#include <QFileInfo>
#include <QScrollBar>
#include <QHeaderView>
#include <QApplication>

#include "C_Uti.hpp"
#include "TglTime.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_GtGetText.hpp"
#include "C_UsHandler.hpp"
#include "cam_constants.hpp"
#include "C_CamDbHandler.hpp"
#include "C_CamGenSigUtil.hpp"
#include "C_CamProHandler.hpp"
#include "C_CamGenTableView.hpp"
#include "C_CamGenKeySelect.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_CamMosDatabaseSelectionPopup.hpp"
#include "C_OscNodeDataPoolContentUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_CamGenTableView::mhs32_COL_WIDTH_DATA = 170;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamGenTableView::C_CamGenTableView(QWidget * const opc_Parent) :
   C_TblViewInteraction(opc_Parent),
   mq_CommunicationActive(false),
   mq_CyclicTransmissionActive(true)
{
   QItemSelectionModel * const pc_LastSelectionModel = this->selectionModel();

   this->mc_SortProxyModel.setSourceModel(&mc_Model);
   this->mc_SortProxyModel.setSortRole(ms32_USER_ROLE_SORT);
   this->C_CamGenTableView::setModel(&mc_SortProxyModel);
   //Delete last selection model, see Qt documentation for setModel
   delete pc_LastSelectionModel;

   this->setItemDelegate(&this->mc_Delegate);

   //Configure
   this->setShowGrid(false);
   this->setSortingEnabled(true);
   this->verticalHeader()->setDefaultSectionSize(20);
   this->horizontalHeader()->setFixedHeight(27);

   //Deactivate
   this->verticalHeader()->setVisible(false);
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   m_SetupContextMenu();

   this->setSelectionBehavior(QAbstractItemView::SelectRows);
   this->setSelectionMode(QAbstractItemView::ExtendedSelection);

   //Buttons
   this->mpc_PushButtonScrollTop = new C_OgePubIconOnly(this->verticalScrollBar());
   // Size is necessary for correct drawing of the images
   this->mpc_PushButtonScrollTop->setGeometry(0, 0, 25, 27);
   this->mpc_PushButtonScrollTop->SetCustomIcons("://images/HeadingPart.svg", "", "", "");

   connect(&this->mc_Model, &C_CamGenTableModel::SigItemCountChanged, this, &C_CamGenTableView::SigItemCountChanged);
   connect(&this->mc_Model, &C_CamGenTableModel::SigUpdateMessageDlc, this, &C_CamGenTableView::SigUpdateMessageDlc);
   connect(&this->mc_Model, &C_CamGenTableModel::SigRegisterCyclicMessage, this,
           &C_CamGenTableView::m_ModelRegisterCyclicMessage);
   connect(this, &C_CamGenTableView::SigLinkClicked, this, &C_CamGenTableView::m_HandleLinkClicked);
   connect(&this->mc_Model, &C_CamGenTableModel::SigItemCheck, this, &C_CamGenTableView::m_HandleCheckChange);
   connect(&this->mc_Model, &C_CamGenTableModel::SigReport, this, &C_CamGenTableView::m_Report);
   connect(&this->mc_Model, &C_CamGenTableModel::SigAutoProtocolSupport, this,
           &C_CamGenTableView::SigAutoProtocolSupport);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540} never took ownership of any context menu item or button
C_CamGenTableView::~C_CamGenTableView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle add new message from data base action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::AddMessageFromDatabase(void)
{
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_CamMosDatabaseSelectionPopup * const pc_Dialog = new C_CamMosDatabaseSelectionPopup(*c_New, true);

   //Resize
   c_New->SetSize(QSize(700, 800));

   // Update settings on accept
   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      QApplication::setOverrideCursor(Qt::WaitCursor);
      const std::vector<std::array<QString, 2> > c_SelectedItems = pc_Dialog->GetSelectedDataElements();
      m_AddMessageFromDatabase(c_SelectedItems);
      QApplication::restoreOverrideCursor();
   }

   if (c_New != NULL)
   {
      pc_Dialog->SaveUserSettings();
      c_New->HideOverlay();
   }
} //lint !e429  no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle add new message action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::AddMessage(void)
{
   uint32_t u32_Item;

   QApplication::setOverrideCursor(Qt::WaitCursor);
   u32_Item = this->mc_Model.AddNewItem(this->m_GetSelectedRows());
   //Select & scroll to newest item
   this->m_SelectAndScrollToRow(u32_Item);
   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle copy message action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::CopyMessage(void) const
{
   QApplication::setOverrideCursor(Qt::WaitCursor);
   this->mc_Model.CopySelectedItems(this->m_GetSelectedRows());
   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle paste message action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::PasteMessage(void)
{
   std::vector<uint32_t> c_Items;
   QApplication::setOverrideCursor(Qt::WaitCursor);
   c_Items = this->mc_Model.PasteItems(this->m_GetSelectedRows());
   this->m_HandleNewItemScrollingAndSelection(c_Items);
   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle cut message action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::CutMessage(void)
{
   uint32_t u32_NewSelection;
   const std::vector<uint32_t> c_Selection = this->m_GetSelectedRows();

   QApplication::setOverrideCursor(Qt::WaitCursor);
   m_StopCyclicCommunication(c_Selection);
   u32_NewSelection = this->mc_Model.CutSelectedItems(c_Selection);
   this->m_SelectAndScrollToRow(u32_NewSelection);
   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle delete message action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::DeleteMessage(void)
{
   uint32_t u32_NewSelection;
   const std::vector<uint32_t> c_Selection = this->m_GetSelectedRows();

   QApplication::setOverrideCursor(Qt::WaitCursor);
   m_StopCyclicCommunication(c_Selection);
   u32_NewSelection = this->mc_Model.DeleteSelectedItems(c_Selection);
   this->m_SelectAndScrollToRow(u32_NewSelection);
   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle move message up action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::MoveMessageUp(void)
{
   const std::vector<uint32_t> c_Input = this->m_GetSelectedRows();

   QApplication::setOverrideCursor(Qt::WaitCursor);
   this->mc_Model.MoveSelectedItems(c_Input, true);
   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle move message down action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::MoveMessageDown(void)
{
   const std::vector<uint32_t> c_Input = this->m_GetSelectedRows();

   QApplication::setOverrideCursor(Qt::WaitCursor);
   this->mc_Model.MoveSelectedItems(c_Input, false);
   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restore column widths

   \param[in]  orc_ColumnWidths  Stored column widths (Restores default values if empty)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::SetCurrentColumnWidths(const std::vector<int32_t> & orc_ColumnWidths)
{
   if (orc_ColumnWidths.size() > 0)
   {
      for (uint32_t u32_ItCol = 0; u32_ItCol < orc_ColumnWidths.size(); ++u32_ItCol)
      {
         this->setColumnWidth(static_cast<int32_t>(u32_ItCol), orc_ColumnWidths[u32_ItCol]);
      }
   }
   else
   {
      //Default
      this->setColumnWidth(C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eNAME), 230);
      this->setColumnWidth(C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eID), 60);
      this->setColumnWidth(C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eXTD), 52);
      this->setColumnWidth(C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eRTR), 51);
      this->setColumnWidth(C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eDLC), 51);
      this->setColumnWidth(C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eDATA), mhs32_COL_WIDTH_DATA);
      this->setColumnWidth(C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eCYCLIC_TRIGGER), 63);
      this->setColumnWidth(C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eCYCLIC_TIME), 94);
      this->setColumnWidth(C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eKEY), 60);
      this->setColumnWidth(C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eMANUAL_TRIGGER), 120);
      this->setColumnWidth(C_CamGenTableModel::h_EnumToColumn(C_CamGenTableModel::eAUTO_SUPPORT), 110);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current column widths

   \return
   Current column widths
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<int32_t> C_CamGenTableView::GetCurrentColumnWidths(void) const
{
   std::vector<int32_t> c_Retval;
   c_Retval.reserve(this->model()->columnCount());
   for (int32_t s32_ItCol = 0; s32_ItCol < this->model()->columnCount(); ++s32_ItCol)
   {
      c_Retval.push_back(this->columnWidth(s32_ItCol));
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::SaveUserSettings(void) const
{
   C_UsHandler::h_GetInstance()->SetMessageColWidths(this->GetCurrentColumnWidths());
   C_UsHandler::h_GetInstance()->SetSelectedMessages(C_CamGenTableView::mh_ConvertVector(this->m_GetSelectedRows()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::LoadUserSettings(void)
{
   this->SetCurrentColumnWidths(C_UsHandler::h_GetInstance()->GetMessageColWidths());
   this->m_SetSelectedMessages(C_UsHandler::h_GetInstance()->GetSelectedMessages());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove all messages for the specified file

   \param[in]  orc_File       Database to remove all messages for
   \param[in]  opc_Indices    Optional indices to specify which messages should be deleted instead of all
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::RemoveMessagesForFile(const QString & orc_File, const std::vector<uint32_t> * const opc_Indices)
{
   std::vector<uint32_t> c_Indices;
   QApplication::setOverrideCursor(Qt::WaitCursor);
   if (opc_Indices != NULL)
   {
      c_Indices = *opc_Indices;
   }
   else
   {
      C_CamProHandler::h_GetInstance()->GetAllMessagesFromDatabase(orc_File, NULL, &c_Indices);
   }
   this->m_StopCyclicCommunication(c_Indices);
   this->mc_Model.DeleteSelectedItems(c_Indices);
   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Signal communication start

   \param[in]  oq_Active   Online/offline flag of trace
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::SetCommunicationStarted(const bool oq_Active)
{
   this->mq_CommunicationActive = oq_Active;
   this->m_RegisterAllCyclicMessages();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set cyclic message transmission active

   \param[in]  oq_Active   true: transmit messages; false: block message transmitting
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::SetCyclicActive(const bool oq_Active)
{
   this->mq_CyclicTransmissionActive = oq_Active;
   this->m_RegisterAllCyclicMessages();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update message data

   \param[in]  ou32_MessageIndex    Message index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::UpdateMessageData(const uint32_t ou32_MessageIndex)
{
   this->mc_Model.UpdateMessageData(ou32_MessageIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Request trigger of model function for update cyclic message

   Reason: have one central point for each check which has to be done when changing an existing message

   \param[in]  ou32_MessageIndex    Message index
   \param[in]  oq_Active            Change of cyclic message state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::TriggerModelUpdateCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active)
{
   this->mc_Model.TriggerModelUpdateCyclicMessage(ou32_MessageIndex, oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if the key needs to be handled by this widget

   \param[in]  orc_Input   Key input interpreted as text

   \return
   True  Handled by this widget
   False No handling
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamGenTableView::CheckAndHandleKey(const QString & orc_Input)
{
   bool q_Retval = false;

   //Only relevant if message can be sent
   if (this->mq_CommunicationActive)
   {
      //Check all messages
      for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < C_CamProHandler::h_GetInstance()->GetMessages().size();
           ++u32_ItMessage)
      {
         const C_CamProMessageData * const pc_Message =
            C_CamProHandler::h_GetInstance()->GetMessageConst(u32_ItMessage);
         if (pc_Message != NULL)
         {
            //Only check non empty keys
            if (pc_Message->c_Key.IsEmpty() == false)
            {
               //Compare key, case sensitive!
               if (orc_Input.compare(pc_Message->c_Key.c_str()) == 0)
               {
                  //Don't stop here as multiple messages can have the same key
                  q_Retval = true;
                  //Trigger sending
                  Q_EMIT (this->SigSendMessage(u32_ItMessage, TglGetTickCount() + pc_Message->u32_KeyPressOffset));
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Request trigger of model function for updating message AutoProtocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::TriggerMessageReload()
{
   this->mc_Model.TriggerMessageReload();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten keypress event slot

   Here: add new actions

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::keyPressEvent(QKeyEvent * const opc_Event)
{
   bool q_CallOrig = true;

   switch (opc_Event->key())
   {
   case Qt::Key_C:
      if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOrig = false;
         this->CopyMessage();
         opc_Event->accept();
      }
      break;
   case Qt::Key_X:
      if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOrig = false;
         this->CutMessage();
         opc_Event->accept();
      }
      break;
   case Qt::Key_V:
      if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOrig = false;
         this->PasteMessage();
         opc_Event->accept();
      }
      break;
   case Qt::Key_Delete:
      q_CallOrig = false;
      this->DeleteMessage();
      opc_Event->accept();
      break;
   case Qt::Key_BracketRight: // Qt::Key_BracketRight matches the "Not-Num-Plus"-Key
   case Qt::Key_Plus:
      if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOrig = false;
         this->AddMessage();
         opc_Event->accept();
      }
      break;
   default:
      //Nothing to do
      break;
   }

   if (q_CallOrig == true)
   {
      C_TblViewInteraction::keyPressEvent(opc_Event);
      //If selected: handle on column basis, otherwise always ignore
      if (this->selectedIndexes().size() > 0L)
      {
         //Check if active editor (might invalidate current index)
         const QSpinBox * const pc_SpinBox = dynamic_cast<const QSpinBox * const>(this->focusWidget());
         const QLineEdit * const pc_LineEdit = dynamic_cast<const QLineEdit * const>(this->focusWidget());
         const QComboBox * const pc_ComboBox = dynamic_cast<const QComboBox * const>(this->focusWidget());
         if (((pc_SpinBox == NULL) && (pc_LineEdit == NULL)) && (pc_ComboBox == NULL))
         {
            //If no editor check current index
            bool q_Ignore = false;
            const QModelIndex c_Current = this->mc_SortProxyModel.mapToSource(this->currentIndex());
            const C_CamGenTableModel::E_Columns e_Col = C_CamGenTableModel::h_ColumnToEnum(c_Current.column());
            //Handle depending on currently active column
            switch (e_Col)
            {
            case C_CamGenTableModel::eNAME:
            case C_CamGenTableModel::eID:
            case C_CamGenTableModel::eDLC:
            case C_CamGenTableModel::eCYCLIC_TIME:
               break;
            case C_CamGenTableModel::eXTD:
            case C_CamGenTableModel::eRTR:
            case C_CamGenTableModel::eDATA:
            case C_CamGenTableModel::eCYCLIC_TRIGGER:
            case C_CamGenTableModel::eKEY:
            case C_CamGenTableModel::eMANUAL_TRIGGER:
            case C_CamGenTableModel::eAUTO_SUPPORT:
            default:
               q_Ignore = true;
               break;
            }
            if (q_Ignore)
            {
               //ignore if not relevant cell -> cell can not be edited via single key press (exception: space ignored)
               opc_Event->ignore();
            }
            else
            {
               //Don't ignore event if edit mode might be entered by pressing a key
            }
         }
         else
         {
            //Don't ignore event
            //Check first if the key is relevant for message triggering (so the event might have to be ignored or not)
            if (opc_Event->text().isEmpty() == false)
            {
               //Explicitly accept the event if entering edit mode -> does not natively accept
               opc_Event->accept();
            }
         }
      }
      else
      {
         //Ignore if no active selection
         opc_Event->ignore();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten show event slot

   Here: move scroll bar buttons

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::showEvent(QShowEvent * const opc_Event)
{
   C_TblViewInteraction::showEvent(opc_Event);
   m_RepositionButtons();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten resize event slot

   Here: move scroll bar buttons

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::resizeEvent(QResizeEvent * const opc_Event)
{
   C_TblViewInteraction::resizeEvent(opc_Event);
   m_RepositionButtons();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten selection change event slot

   Here: notify model of change (selection indicator)

   \param[in]  orc_Selected      Selected items
   \param[in]  orc_Deselected    Deselected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected)
{
   std::vector<uint32_t> c_Selection;

   C_TblViewInteraction::selectionChanged(orc_Selected, orc_Deselected);
   c_Selection = this->m_GetSelectedRows();
   if (c_Selection.size() == 1UL)
   {
      const uint32_t u32_Size = c_Selection.size();
      Q_EMIT (this->SigSelected(u32_Size, c_Selection[0UL]));
   }
   else
   {
      Q_EMIT (this->SigSelected(c_Selection.size(), 0UL));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the hint for size column

   Will be used when resizeColumnToContents is used for the specific column.
   resizeColumnToContents is used by double click on the separator in the header too.

   \param[in]  os32_Column  Index of column

   \return
   Width of column in pixel
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamGenTableView::sizeHintForColumn(const int32_t os32_Column) const
{
   int32_t s32_Size;

   if (os32_Column == static_cast<int32_t>(C_CamGenTableModel::eDATA))
   {
      s32_Size = mhs32_COL_WIDTH_DATA;
   }
   else
   {
      s32_Size = C_TblViewInteraction::sizeHintForColumn(os32_Column);
   }

   return s32_Size;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get selected row index

   \return
   Selected row index
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32_t> C_CamGenTableView::m_GetSelectedRows(void) const
{
   std::vector<uint32_t> c_Retval;
   const QModelIndexList c_Indices = this->selectedIndexes();

   c_Retval.reserve(c_Indices.size());
   for (QModelIndexList::const_iterator c_It = c_Indices.begin(); c_It != c_Indices.end(); ++c_It)
   {
      const QModelIndex & rc_Index = *c_It;
      const int32_t s32_Row = this->mc_SortProxyModel.mapToSource(rc_Index).row();
      if (s32_Row >= 0)
      {
         c_Retval.push_back(static_cast<uint32_t>(s32_Row));
      }
   }
   //Unique!
   C_Uti::h_Uniqueify(c_Retval);

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   this->mpc_ActionAddFromDatabase = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                         "Add message from database"), this,
                                                                      &C_CamGenTableView::AddMessageFromDatabase);
   this->mpc_ActionAdd = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                             "Add new message"), this, &C_CamGenTableView::AddMessage,
                                                          static_cast<int32_t>(Qt::CTRL) +
                                                          static_cast<int32_t>(Qt::Key_Plus));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ActionCut = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                             "Cut"), this, &C_CamGenTableView::CutMessage,
                                                          static_cast<int32_t>(Qt::CTRL) +
                                                          static_cast<int32_t>(Qt::Key_X));
   this->mpc_ActionCopy = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                              "Copy"), this, &C_CamGenTableView::CopyMessage,
                                                           static_cast<int32_t>(Qt::CTRL) +
                                                           static_cast<int32_t>(Qt::Key_C));
   this->mpc_ActionPaste = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                               "Paste"), this, &C_CamGenTableView::PasteMessage,
                                                            static_cast<int32_t>(Qt::CTRL) +
                                                            static_cast<int32_t>(Qt::Key_V));

   //Deactivate move (kept for fast reactivation if necessary)
   //   this->mpc_ContextMenu->addSeparator();

   //   this->mpc_ActionMoveUp = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
   //                                                                "Move Up"), this,
   //                                                             &C_CamGenTableView::MoveMessageUp,
   //                                                             static_cast<int32_t>(Qt::CTRL) +
   //                                                             static_cast<int32_t>(Qt::Key_Up));
   //   this->mpc_ActionMoveDown = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
   //                                                                  "Move Down"), this,
   //                                                               &C_CamGenTableView::MoveMessageDown,
   //                                                               static_cast<int32_t>(Qt::CTRL) +
   //                                                               static_cast<int32_t>(Qt::Key_Down));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ActionDelete = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                "Delete"), this, &C_CamGenTableView::DeleteMessage,
                                                             static_cast<int32_t>(Qt::Key_Delete));

   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_CamGenTableView::customContextMenuRequested, this,
           &C_CamGenTableView::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   bool q_SelectionVisibility;

   if (this->selectedIndexes().size() > 0L)
   {
      q_SelectionVisibility = true;
   }
   else
   {
      q_SelectionVisibility = false;
   }
   this->mpc_ActionCopy->setVisible(q_SelectionVisibility);
   this->mpc_ActionCut->setVisible(q_SelectionVisibility);
   this->mpc_ActionDelete->setVisible(q_SelectionVisibility);

   this->mpc_ContextMenu->popup(this->mapToGlobal(orc_Pos));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility function to select the specified rows

   Info: selection is updated, not cleared

   \param[in]  orc_Items   Rows to select, expected sorted ascending
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_SelectRange(const std::vector<uint32_t> & orc_Items)
{
   if (orc_Items.size() > 0UL)
   {
      this->clearSelection();
      for (uint32_t u32_It = 0UL; u32_It < orc_Items.size(); ++u32_It)
      {
         this->m_SelectRow(orc_Items[u32_It], QItemSelectionModel::Select);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility function to select the specified row (clear other selection) and scroll to this row (first column)

   \param[in]  ou32_Row    Row index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_SelectAndScrollToRow(const uint32_t ou32_Row)
{
   const QModelIndex c_Index = this->mc_SortProxyModel.mapFromSource(this->mc_Model.index(ou32_Row, 0));

   this->clearSelection();
   this->setFocus();
   this->setCurrentIndex(c_Index);
   this->selectionModel()->select(c_Index, QItemSelectionModel::Select);
   this->scrollTo(c_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reposition the buttons as necessary
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_RepositionButtons(void)
{
   const QSize c_BUTTON_SIZE_UP(25, 27);

   //Position
   this->mpc_PushButtonScrollTop->setGeometry(QRect(QPoint(0, 0), c_BUTTON_SIZE_UP));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle link clicked signal

   \param[in]  orc_Index   Clicked index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_HandleLinkClicked(const QModelIndex & orc_Index)
{
   const int32_t s32_Row = this->mc_SortProxyModel.mapToSource(orc_Index).row();

   if (s32_Row >= 0)
   {
      if (C_CamGenTableModel::h_ColumnToEnum(this->mc_SortProxyModel.mapToSource(orc_Index).column()) ==
          C_CamGenTableModel::eMANUAL_TRIGGER)
      {
         if (this->mq_CommunicationActive)
         {
            Q_EMIT (this->SigSendMessage(static_cast<uint32_t>(s32_Row), TglGetTickCount()));
         }

         else
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eINFORMATION);
            c_Message.SetHeading(C_GtGetText::h_GetText("Measurement not started"));
            c_Message.SetDescription(C_GtGetText::h_GetText("The transmission of messages is only allowed as long "
                                                            "as the measurement is running. \nClick the play button "
                                                            "in trace to start measurement."));
            c_Message.Execute();
         }
      }
      else if (C_CamGenTableModel::h_ColumnToEnum(this->mc_SortProxyModel.mapToSource(orc_Index).column()) ==
               C_CamGenTableModel::eKEY)
      {
         const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
         C_CamGenKeySelect * const pc_Dialog =
            new C_CamGenKeySelect(*c_New, static_cast<uint32_t>(s32_Row));

         //Resize
         c_New->SetSize(QSize(600, 375));

         // Update settings on accept
         if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
         {
            Q_UNUSED(pc_Dialog)
            this->mc_Model.UpdateMessageKey(static_cast<uint32_t>(s32_Row));
         }
         if (c_New != NULL)
         {
            c_New->HideOverlay();
         }
      } //lint !e429  no memory leak because of the parent pc_Dialog and the Qt memory management
      else
      {
         //Unexpected
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Stop cyclic communication for these items if necessary (if currently is registered)

   \param[in]  orc_Items   Item indices to change the state for
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_StopCyclicCommunication(const std::vector<uint32_t> & orc_Items)
{
   //only relevant if communication active
   if ((this->mq_CommunicationActive == true) && (this->mq_CyclicTransmissionActive == true))
   {
      //Each item
      for (uint32_t u32_It = 0UL; u32_It < orc_Items.size(); ++u32_It)
      {
         const C_CamProMessageData * const pc_Message =
            C_CamProHandler::h_GetInstance()->GetMessageConst(orc_Items[u32_It]);
         if (pc_Message != NULL)
         {
            //Only deactivate if currently active
            if (pc_Message->q_DoCyclicTrigger == true)
            {
               Q_EMIT (this->SigRegisterCyclicMessage(orc_Items[u32_It], false));
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle cyclic change request

   Add check if necessary

   \param[in]  ou32_MessageIndex    Message index
   \param[in]  oq_Active            Flag if cyclic message is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_ModelRegisterCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active)
{
   //only relevant if communication active or while paused
   if ((this->mq_CommunicationActive == true) && (this->mq_CyclicTransmissionActive == true))
   {
      Q_EMIT (this->SigRegisterCyclicMessage(ou32_MessageIndex, oq_Active));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle add message from data base action

   \param[in]  orc_NewItems   New item description
                              array:
                              0: database name
                              1: message name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_AddMessageFromDatabase(const std::vector<std::array<QString, 2> > & orc_NewItems)
{
   std::vector<uint32_t> c_NewItems;
   std::vector<C_CamProMessageData> c_NewMessages;
   for (uint32_t u32_ItNewItem = 0UL; u32_ItNewItem < orc_NewItems.size(); ++u32_ItNewItem)
   {
      const QFileInfo c_FileInfo(orc_NewItems[u32_ItNewItem][0UL]);
      C_CamProMessageData c_NewMessage;

      std::vector<uint8_t> c_Bytes;

      //Common attributes
      c_NewMessage.c_DataBaseFilePath = orc_NewItems[u32_ItNewItem][0UL].toStdString().c_str();
      c_NewMessage.q_ContainsValidHash = true;
      c_NewMessage.c_Key = "";
      c_NewMessage.u32_KeyPressOffset = 0UL;
      c_NewMessage.c_Name = orc_NewItems[u32_ItNewItem][1UL].toStdString().c_str();
      c_NewMessage.SetMessageBoolValue(C_CamProMessageData::eGBODS_RTR, false);
      c_NewMessage.SetMessageBoolValue(C_CamProMessageData::eGBODS_DO_CYCLIC, false);
      c_NewMessage.SetMessageBoolValue(C_CamProMessageData::eGBODS_AUTO_SUPPORT, false);

      if (c_FileInfo.suffix().compare("dbc", Qt::CaseInsensitive) == 0)
      {
         //First prepare C_CamDbHandler
         if (C_CamDbHandler::h_GetInstance()->FindDbcMessage(orc_NewItems[u32_ItNewItem][0UL],
                                                             orc_NewItems[u32_ItNewItem][1UL]) == C_NO_ERR)
         {
            //Get data
            const C_CieConverter::C_CieCanMessage * const pc_Message = C_CamDbHandler::h_GetInstance()->GetDbcMessage(
               orc_NewItems[u32_ItNewItem][0UL], orc_NewItems[u32_ItNewItem][1UL], false, 0UL);
            if (pc_Message != NULL)
            {
               //Copy values
               c_NewMessage.SetMessageBoolValue(C_CamProMessageData::eGBODS_EXTENDED, pc_Message->q_IsExtended);
               c_NewMessage.u16_Dlc = pc_Message->u16_Dlc;
               c_NewMessage.u32_CyclicTriggerTime = pc_Message->u32_CycleTimeMs;
               c_NewMessage.u32_Id = pc_Message->u32_CanId;

               //Cyclic
               if (pc_Message->e_TxMethod == C_OscCanMessage::eTX_METHOD_CYCLIC)
               {
                  c_NewMessage.q_DoCyclicTrigger = true;
               }
               else
               {
                  c_NewMessage.q_DoCyclicTrigger = false;
               }

               //Get bytes AFTER the DLC is valid
               c_Bytes = C_CamGenSigUtil::h_ConvertRawDataFormat(c_NewMessage);
               //Init message data to signal init values
               for (uint32_t u32_ItSignal = 0UL; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
               {
                  const C_CieConverter::C_CieCanSignal & rc_Signal = pc_Message->c_Signals[u32_ItSignal];
                  const C_OscCanSignal c_OscSignal = C_CamGenSigUtil::h_ConvertDbcToOsy(rc_Signal);

                  // set unused multiplexed signals to zero
                  // (i.e. those with mux value != 0, the initial multiplexer value)
                  if ((rc_Signal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL) &&
                      (rc_Signal.u16_MultiplexValue != 0U))
                  {
                     C_OscNodeDataPoolContent c_ZeroContent = rc_Signal.c_Element.c_MinValue; // init appropriately
                     C_OscNodeDataPoolContentUtil::h_ZeroContent(c_ZeroContent);
                     C_CamGenSigUtil::h_DecodeSignalValueToRaw(c_Bytes, c_OscSignal, c_ZeroContent);
                  }
                  else
                  {
                     if (rc_Signal.c_Element.c_DataSetValues.size() > 0UL)
                     {
                        C_CamGenSigUtil::h_DecodeSignalValueToRaw(c_Bytes, c_OscSignal,
                                                                  rc_Signal.c_Element.c_DataSetValues[0UL]);
                     }
                  }
               }

               //Hash
               c_NewMessage.u32_Hash = C_CamGenSigUtil::h_CalcMessageHash(*pc_Message);

               //Set new bytes (after init)
               c_NewMessage.SetMessageDataBytes(c_Bytes);

               //Add complete message
               c_NewMessages.push_back(c_NewMessage);
            }
         }
      }
      else if (c_FileInfo.suffix().compare("syde_sysdef", Qt::CaseInsensitive) == 0)
      {
         //First prepare C_CamDbHandler
         if (C_CamDbHandler::h_GetInstance()->FindOsyMessage(orc_NewItems[u32_ItNewItem][0UL],
                                                             orc_NewItems[u32_ItNewItem][1UL]) == C_NO_ERR)
         {
            //Get data
            const C_OscCanMessage * const pc_Message = C_CamDbHandler::h_GetInstance()->GetOscMessage(
               orc_NewItems[u32_ItNewItem][0UL], orc_NewItems[u32_ItNewItem][1UL], false, 0UL);
            const C_OscNodeDataPoolList * const pc_List = C_CamDbHandler::h_GetInstance()->GetOscList(
               orc_NewItems[u32_ItNewItem][0UL], orc_NewItems[u32_ItNewItem][1UL], false, 0UL);
            if ((pc_Message != NULL) && (pc_List != NULL))
            {
               std::vector<C_OscNodeDataPoolListElement> c_DatapoolPart;

               //Copy values
               c_NewMessage.SetMessageBoolValue(C_CamProMessageData::eGBODS_EXTENDED, pc_Message->q_IsExtended);
               c_NewMessage.u16_Dlc = static_cast<uint8_t>(pc_Message->u16_Dlc);
               c_NewMessage.u32_CyclicTriggerTime = pc_Message->u32_CycleTimeMs;
               c_NewMessage.u32_Id = pc_Message->u32_CanId;

               //Cyclic
               if (pc_Message->IsTransmissionTypeOfCyclicType())
               {
                  c_NewMessage.q_DoCyclicTrigger = true;
               }
               else
               {
                  c_NewMessage.q_DoCyclicTrigger = false;
               }

               //Get bytes AFTER the DLC is valid
               c_Bytes = C_CamGenSigUtil::h_ConvertRawDataFormat(c_NewMessage);
               //Init message data to signal init values
               for (uint32_t u32_ItSignal = 0UL; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
               {
                  const C_OscCanSignal & rc_Signal = pc_Message->c_Signals[u32_ItSignal];

                  if (rc_Signal.u32_ComDataElementIndex < pc_List->c_Elements.size())
                  {
                     const C_OscNodeDataPoolListElement & rc_Element =
                        pc_List->c_Elements[rc_Signal.u32_ComDataElementIndex];
                     c_DatapoolPart.push_back(rc_Element);
                     // set unused multiplexed signals to zero
                     // (i.e. those with mux value != 0, the initial multiplexer value)
                     if ((rc_Signal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL) &&
                         (rc_Signal.u16_MultiplexValue != 0U))
                     {
                        C_OscNodeDataPoolContent c_ZeroContent = rc_Element.c_MinValue; // init appropriately
                        C_OscNodeDataPoolContentUtil::h_ZeroContent(c_ZeroContent);
                        C_CamGenSigUtil::h_DecodeSignalValueToRaw(c_Bytes, rc_Signal, c_ZeroContent);
                     }
                     else
                     {
                        if (rc_Element.c_DataSetValues.size() > 0UL)
                        {
                           C_CamGenSigUtil::h_DecodeSignalValueToRaw(c_Bytes, rc_Signal,
                                                                     rc_Element.c_DataSetValues[0UL]);
                        }
                     }
                  }
               }

               //Hash
               c_NewMessage.u32_Hash = C_CamGenSigUtil::h_CalcMessageHash(*pc_Message, c_DatapoolPart);

               //Set new bytes (after init)
               c_NewMessage.SetMessageDataBytes(c_Bytes);

               //Add complete message
               c_NewMessages.push_back(c_NewMessage);
            }
         }
      }
      else
      {
         //Unknown
      }
   }
   //Add complete message
   c_NewItems = this->mc_Model.AddSpecificNewItems(this->m_GetSelectedRows(), c_NewMessages);
   this->m_HandleNewItemScrollingAndSelection(c_NewItems);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle new item selection and scrolling

   \param[in]  orc_Indices    New item indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_HandleNewItemScrollingAndSelection(const std::vector<uint32_t> & orc_Indices)
{
   //Select newest items
   const std::vector<uint32_t> c_Items = C_Uti::h_UniquifyAndSortAscending(orc_Indices);

   this->clearSelection();
   this->m_SelectRange(c_Items);
   //Scroll to last new item
   if (c_Items.size() > 0UL)
   {
      const uint32_t u32_HighestItem = c_Items[static_cast<std::vector<uint32_t>::size_type>(c_Items.size() - 1UL)];

      if (c_Items.size() > 1)
      {
         const QModelIndex c_Index = this->mc_SortProxyModel.mapFromSource(this->mc_Model.index(u32_HighestItem, 0));
         this->scrollTo(c_Index);
      }
      else
      {
         this->m_SelectAndScrollToRow(u32_HighestItem);
      }
   }
   //only relevant if communication active or while paused
   if ((this->mq_CommunicationActive == true) && (this->mq_CyclicTransmissionActive == true))
   {
      //Activate all copied cyclic items
      for (uint32_t u32_It = 0UL; u32_It < c_Items.size(); ++u32_It)
      {
         const C_CamProMessageData * const pc_Message = C_CamProHandler::h_GetInstance()->GetMessageConst(
            c_Items[u32_It]);
         if (pc_Message != NULL)
         {
            //Only activate if it should be active
            if (pc_Message->q_DoCyclicTrigger == true)
            {
               Q_EMIT (this->SigRegisterCyclicMessage(c_Items[u32_It], true));
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert uint32_t vector to int32_t

   \param[in]  orc_Input   Vector to convert

   \return
   Converted vector
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<int32_t> C_CamGenTableView::mh_ConvertVector(const std::vector<uint32_t> & orc_Input)
{
   std::vector<int32_t> c_Retval;
   c_Retval.reserve(orc_Input.size());
   for (uint32_t u32_ItInput = 0UL; u32_ItInput < orc_Input.size(); ++u32_ItInput)
   {
      c_Retval.push_back(static_cast<int32_t>(orc_Input[u32_ItInput]));
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select the specified items (clear all other selection)

   \param[in]  orc_SelectedIndices  Indices of items to select
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_SetSelectedMessages(const std::vector<int32_t> & orc_SelectedIndices)
{
   this->selectionModel()->clearSelection();
   for (uint32_t u32_ItInput = 0UL; u32_ItInput < orc_SelectedIndices.size(); ++u32_ItInput)
   {
      m_SelectRow(orc_SelectedIndices[u32_ItInput], QItemSelectionModel::Select);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select one row

   \param[in]  os32_Row    Row index
   \param[in]  orc_Flags   Flags
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_SelectRow(const int32_t os32_Row, const QItemSelectionModel::SelectionFlags & orc_Flags)
{
   const QModelIndex c_StartIndex = this->mc_SortProxyModel.mapFromSource(this->mc_Model.index(os32_Row, 0));
   const QModelIndex c_EndIndex =
      this->mc_SortProxyModel.mapFromSource(this->mc_Model.index(os32_Row, this->mc_Model.columnCount() - 1));
   const QItemSelection c_SelectionRange(c_StartIndex, c_EndIndex);

   this->selectionModel()->select(c_SelectionRange, orc_Flags);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle checkbox item change

   \param[in]  orc_Index   Changed item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_HandleCheckChange(const QModelIndex & orc_Index)
{
   if (orc_Index.isValid())
   {
      m_SelectRow(orc_Index.row(), QItemSelectionModel::ClearAndSelect);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Decide if to start or stop cyclic message transmission.

   Combine the two flags "message transmission active" and "trace running"
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_RegisterAllCyclicMessages(void)
{
   if ((this->mq_CyclicTransmissionActive == true) && (this->mq_CommunicationActive == true))
   {
      // register all cyclic messages
      for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < C_CamProHandler::h_GetInstance()->GetMessages().size();
           ++u32_ItMessage)
      {
         const C_CamProMessageData * const pc_Message =
            C_CamProHandler::h_GetInstance()->GetMessageConst(u32_ItMessage);
         if (pc_Message != NULL)
         {
            if (pc_Message->q_DoCyclicTrigger == true)
            {
               //Trigger sending
               Q_EMIT (this->SigRegisterCyclicMessage(u32_ItMessage, true));
            }
         }
      }
   }

   // remove of all current cyclic messages is handled by main window (start/stop) resp. by messages widget (toggle)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report

   \param[in]  oe_Type           Type
   \param[in]  orc_Heading       Heading
   \param[in]  orc_Description   Description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableView::m_Report(const C_OgeWiCustomMessage::E_Type oe_Type, const QString & orc_Heading,
                                 const QString & orc_Description)
{
   C_OgeWiCustomMessage c_Message(this, oe_Type);

   c_Message.SetHeading(orc_Heading);
   c_Message.SetDescription(orc_Description);
   c_Message.Execute();
}
