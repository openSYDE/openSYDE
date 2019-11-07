//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for all displaying and interacting with manually configured messages (implementation)

   Widget for all displaying and interacting with manually configured messages

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_GtGetText.h"
#include "C_CamProHandler.h"

#include "C_CamGenMessagesWidget.h"
#include "ui_C_CamGenMessagesWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamGenMessagesWidget::C_CamGenMessagesWidget(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamGenMessagesWidget)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   this->m_OnItemCountChanged(0UL);

   this->SetBackgroundColor(11);
   this->mpc_Ui->pc_LabelHeading->SetForegroundColor(4);
   this->mpc_Ui->pc_LabelHeading->SetFontPixel(14, true, false);
   this->mpc_Ui->pc_LabelNoMessages->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelNoMessages->SetFontPixel(12, false, false);

   m_InitButtons();

   //Deactivate move (kept for fast reactivation if necessary)
   this->mpc_Ui->pc_PushButtonMoveUp->setVisible(false);
   this->mpc_Ui->pc_PushButtonMoveDown->setVisible(false);

   //Connections
   connect(this->mpc_Ui->pc_TableView, &C_CamGenTableView::SigSelected, this, &C_CamGenMessagesWidget::SigSelected);
   connect(this->mpc_Ui->pc_TableView, &C_CamGenTableView::SigItemCountChanged, this,
           &C_CamGenMessagesWidget::m_OnItemCountChanged);
   connect(this->mpc_Ui->pc_TableView, &C_CamGenTableView::SigUpdateMessageDLC, this,
           &C_CamGenMessagesWidget::SigUpdateMessageDLC);
   connect(this->mpc_Ui->pc_TableView, &C_CamGenTableView::SigSendMessage, this,
           &C_CamGenMessagesWidget::SigSendMessage);
   connect(this->mpc_Ui->pc_TableView, &C_CamGenTableView::SigRegisterCyclicMessage, this,
           &C_CamGenMessagesWidget::SigRegisterCyclicMessage);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamGenMessagesWidget::~C_CamGenMessagesWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenMessagesWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelNoMessages->setText(C_GtGetText::h_GetText(
                                                "No messages are declared, add any via the '+' button."));
   //Tooltips
   this->mpc_Ui->pc_PushButtonAdd->SetToolTipInformation(C_GtGetText::h_GetText("Add Message"), "");
   this->mpc_Ui->pc_PushButtonAddFromDatabase->SetToolTipInformation(C_GtGetText::h_GetText(
                                                                        "Add Message from Database"), "");
   this->mpc_Ui->pc_PushButtonCopy->SetToolTipInformation(C_GtGetText::h_GetText("Copy"), "");
   this->mpc_Ui->pc_PushButtonPaste->SetToolTipInformation(C_GtGetText::h_GetText("Paste"), "");
   this->mpc_Ui->pc_PushButtonCut->SetToolTipInformation(C_GtGetText::h_GetText("Cut"), "");
   this->mpc_Ui->pc_PushButtonDelete->SetToolTipInformation(C_GtGetText::h_GetText("Delete"), "");
   this->mpc_Ui->pc_PushButtonMoveUp->SetToolTipInformation(C_GtGetText::h_GetText("Move Up"), "");
   this->mpc_Ui->pc_PushButtonMoveDown->SetToolTipInformation(C_GtGetText::h_GetText("Move Down"), "");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenMessagesWidget::SaveUserSettings(void) const
{
   this->mpc_Ui->pc_TableView->SaveUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenMessagesWidget::LoadUserSettings(void) const
{
   this->mpc_Ui->pc_TableView->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove all messages for the specified file

   \param[in] orc_File Database to remove all messages for
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenMessagesWidget::RemoveMessagesForFile(const QString & orc_File) const
{
   this->mpc_Ui->pc_TableView->RemoveMessagesForFile(orc_File);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Signal communication start
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenMessagesWidget::SetCommunicationStarted(void) const
{
   this->mpc_Ui->pc_TableView->SetCommunicationStarted();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Signal communication stop
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenMessagesWidget::SetCommunicationStopped(void) const
{
   this->mpc_Ui->pc_TableView->SetCommunicationStopped();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if the key needs to be handled by this widget

   \param[in] orc_Input Key input interpreted as text

   \return
   True  Handled by this widget
   False No handling
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamGenMessagesWidget::CheckAndHandleKey(const QString & orc_Input) const
{
   return this->mpc_Ui->pc_TableView->CheckAndHandleKey(orc_Input);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update message data

   \param[in] ou32_MessageIndex Message index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenMessagesWidget::UpdateMessageData(const stw_types::uint32 ou32_MessageIndex) const
{
   this->mpc_Ui->pc_TableView->UpdateMessageData(ou32_MessageIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Request trigger of model function for update cyclic message

   Reason: have one central point for each check which has to be done when changing an existing message

   \param[in] ou32_MessageIndex Message index
   \param[in] oq_Active         Change of cyclic message state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenMessagesWidget::TriggerModelUpdateCyclicMessage(const stw_types::uint32 ou32_MessageIndex,
                                                             const bool oq_Active) const
{
   this->mpc_Ui->pc_TableView->TriggerModelUpdateCyclicMessage(ou32_MessageIndex, oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle buttons init state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenMessagesWidget::m_InitButtons(void) const
{
   QString c_Icon;

   c_Icon = "://images/IconAddDataBaseBlue";
   this->mpc_Ui->pc_PushButtonAddFromDatabase->SetSvg(c_Icon + ".svg", "", c_Icon + "Hover.svg", "", "", "",
                                                      c_Icon + "Pressed.svg");
   c_Icon = "://images/IconAddEnabled";
   this->mpc_Ui->pc_PushButtonAdd->SetSvg(c_Icon + ".svg", "", c_Icon + "Hover.svg", "", "", "",
                                          c_Icon + "Pressed.svg");
   c_Icon = "://images/IconCopy";
   this->mpc_Ui->pc_PushButtonCopy->SetSvg(c_Icon + ".svg", "", c_Icon + "Hover.svg", "", "", "",
                                           c_Icon + "Pressed.svg");
   c_Icon = "://images/IconPaste";
   this->mpc_Ui->pc_PushButtonPaste->SetSvg(c_Icon + ".svg", "", c_Icon + "Hover.svg", "", "", "",
                                            c_Icon + "Pressed.svg");
   c_Icon = "://images/IconCut";
   this->mpc_Ui->pc_PushButtonCut->SetSvg(c_Icon + ".svg", "", c_Icon + "Hover.svg", "", "", "",
                                          c_Icon + "Pressed.svg");
   c_Icon = "://images/IconClear";
   this->mpc_Ui->pc_PushButtonDelete->SetSvg(c_Icon + ".svg", "", c_Icon + "Hover.svg", "", "", "",
                                             c_Icon + "Pressed.svg");
   c_Icon = "://images/IconMoveUp";
   this->mpc_Ui->pc_PushButtonMoveUp->SetSvg(c_Icon + ".svg", "", c_Icon + "Hover.svg", "", "", "",
                                             c_Icon + "Pressed.svg");
   c_Icon = "://images/IconMoveDown";
   this->mpc_Ui->pc_PushButtonMoveDown->SetSvg(c_Icon + ".svg", "", c_Icon + "Hover.svg", "", "", "",
                                               c_Icon + "Pressed.svg");
   //Connects
   connect(this->mpc_Ui->pc_PushButtonAddFromDatabase, &C_OgePubSvgIconOnly::clicked, this->mpc_Ui->pc_TableView,
           &C_CamGenTableView::AddMessageFromDatabase);
   connect(this->mpc_Ui->pc_PushButtonAdd, &C_OgePubSvgIconOnly::clicked, this->mpc_Ui->pc_TableView,
           &C_CamGenTableView::AddMessage);
   connect(this->mpc_Ui->pc_PushButtonCopy, &C_OgePubSvgIconOnly::clicked, this->mpc_Ui->pc_TableView,
           &C_CamGenTableView::CopyMessage);
   connect(this->mpc_Ui->pc_PushButtonPaste, &C_OgePubSvgIconOnly::clicked, this->mpc_Ui->pc_TableView,
           &C_CamGenTableView::PasteMessage);
   connect(this->mpc_Ui->pc_PushButtonCut, &C_OgePubSvgIconOnly::clicked, this->mpc_Ui->pc_TableView,
           &C_CamGenTableView::CutMessage);
   connect(this->mpc_Ui->pc_PushButtonDelete, &C_OgePubSvgIconOnly::clicked, this->mpc_Ui->pc_TableView,
           &C_CamGenTableView::DeleteMessage);
   connect(this->mpc_Ui->pc_PushButtonMoveUp, &C_OgePubSvgIconOnly::clicked, this->mpc_Ui->pc_TableView,
           &C_CamGenTableView::MoveMessageUp);
   connect(this->mpc_Ui->pc_PushButtonMoveDown, &C_OgePubSvgIconOnly::clicked, this->mpc_Ui->pc_TableView,
           &C_CamGenTableView::MoveMessageDown);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update dynamic heading
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenMessagesWidget::m_UpdateHeading(void) const
{
   this->mpc_Ui->pc_LabelHeading->setText(QString(C_GtGetText::h_GetText("Messages (%1)")).arg(C_CamProHandler::
                                                                                               h_GetInstance()->
                                                                                               GetMessages().size()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Register item count change

   \param[in] ou32_NewItemCount New item count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenMessagesWidget::m_OnItemCountChanged(const stw_types::uint32 ou32_NewItemCount) const
{
   m_UpdateHeading();
   if (ou32_NewItemCount > 0UL)
   {
      this->mpc_Ui->pc_GroupBoxNoMessages->setVisible(false);
      this->mpc_Ui->pc_TableView->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxNoMessages->setVisible(true);
      this->mpc_Ui->pc_TableView->setVisible(false);
   }
}
