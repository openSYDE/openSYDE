//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with all configuration and controlling elements for the trace
 (implementation)

   Widget with all configuration and controlling elements for the trace

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QLineEdit>

#include "C_CamMetControlBarWidget.hpp"
#include "ui_C_CamMetControlBarWidget.h"

#include "C_UsHandler.hpp"
#include "C_GtGetText.hpp"

#include "C_OgePopUpDialog.hpp"
#include "C_CamMetSettingsPopup.hpp"

#include "C_OgeWiCustomMessage.hpp"

#include <QDebug>

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_CamMetControlBarWidget::mhs32_MAX_COUNT_COMBO_BOX = 7;

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
C_CamMetControlBarWidget::C_CamMetControlBarWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_CamMetControlBarWidget),
   mq_DisplayTimestampAbsoluteTimeOfDay(false),
   mu32_TraceBufferSize(1000U)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   // Set maximum + 1, so the user can add a new element although the combo box is full. The last element will
   // be removed manually in the slot functions.
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_ComboBoxSearch->setMaxCount(mhs32_MAX_COUNT_COMBO_BOX + 1);
   this->mpc_Ui->pc_ComboBoxSearch->setInsertPolicy(QComboBox::InsertAtTop);

   this->mpc_Ui->pc_PushButtonTogglePlay->setCheckable(true);
   this->mpc_Ui->pc_PushButtonToggleHex->setCheckable(true);
   this->mpc_Ui->pc_PushButtonToggleTimeMode->setCheckable(true);
   this->mpc_Ui->pc_PushButtonToggleDisplayMode->setCheckable(true);
   this->mpc_Ui->pc_PushButtonTogglePlay->SetSvg("://images/IconToggleToStatePlaying.svg", "",
                                                 "://images/IconToggleToStatePlayingHover.svg",
                                                 "://images/IconToggleToStatePausing.svg", "",
                                                 "://images/IconToggleToStatePausingHover.svg",
                                                 "://images/IconToggleToStatePlayingPressed.svg",
                                                 "://images/IconToggleToStatePausingPressed.svg");
   this->mpc_Ui->pc_PushButtonStop->setEnabled(false);
   this->mpc_Ui->pc_PushButtonStop->SetSvg("://images/IconStopEnabled.svg", "://images/IconStopDisabled.svg",
                                           "://images/IconStopEnabledHover.svg", "", "", "",
                                           "://images/IconStopEnabledPressed.svg");
   this->mpc_Ui->pc_PushButtoneSearchNext->SetSvg("://images/IconSearchDownEnabled.svg", "",
                                                  "://images/IconSearchDownHover.svg",
                                                  "", "", "",
                                                  "://images/IconSearchDownPressed.svg");
   this->mpc_Ui->pc_PushButtoneSearchPrev->SetSvg("://images/IconSearchUpEnabled.svg", "",
                                                  "://images/IconSearchUpHover.svg",
                                                  "", "", "",
                                                  "://images/IconSearchUpPressed.svg");
   this->mpc_Ui->pc_PushButtonClear->SetSvg("://images/IconWipeAwayEnabled.svg", "", "://images/IconWipeAwayHover.svg",
                                            "", "", "",
                                            "://images/IconWipeAwayPressed.svg");
   this->mpc_Ui->pc_PushButtonTraceSettings->SetSvg("://images/IconSettingsEnabled.svg", "",
                                                    "://images/IconSettingsHover.svg",
                                                    "", "", "",
                                                    "://images/IconSettingsPressed.svg");
   this->mpc_Ui->pc_PushButtonToggleHex->SetSvg("://images/IconNumberFormatHex.svg", "",
                                                "://images/IconNumberFormatHexHover.svg",
                                                "://images/IconNumberFormatDec.svg", "",
                                                "://images/IconNumberFormatDecHover.svg",
                                                "://images/IconNumberFormatHexPressed.svg",
                                                "://images/IconNumberFormatDecPressed.svg");

   this->mpc_Ui->pc_PushButtonToggleTimeMode->SetSvg("://images/IconTimeModeRelative.svg", "",
                                                     "://images/IconTimeModeRelativeHover.svg",
                                                     "://images/IconTimeModeAbsolute.svg", "",
                                                     "://images/IconTimeModeAbsoluteHover.svg",
                                                     "://images/IconTimeModeRelativePressed.svg",
                                                     "://images/IconTimeModeAbsolutePressed.svg");

   this->mpc_Ui->pc_PushButtonToggleDisplayMode->SetSvg("://images/IconToggleToUniqueOn.svg", "",
                                                        "://images/IconToggleToUniqueOnHover.svg",
                                                        "://images/IconToggleToUniqueOff.svg", "",
                                                        "://images/IconToggleToUniqueOffHover.svg",
                                                        "://images/IconToggleToUniqueOnPressed.svg",
                                                        "://images/IconToggleToUniqueOffPressed.svg");

   connect(this->mpc_Ui->pc_PushButtonTogglePlay, &QPushButton::toggled, this,
           &C_CamMetControlBarWidget::m_HandleTogglePlay);
   connect(this->mpc_Ui->pc_PushButtonStop, &QPushButton::clicked, this,
           &C_CamMetControlBarWidget::m_HandleStop);
   connect(this->mpc_Ui->pc_PushButtonToggleDisplayMode, &QPushButton::toggled, this,
           &C_CamMetControlBarWidget::m_HandleToggleDisplayMode);
   connect(this->mpc_Ui->pc_ComboBoxProtocol,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_CamMetControlBarWidget::m_OnProtocolIndexChange);

   connect(this->mpc_Ui->pc_PushButtonClear, &QPushButton::clicked,
           this, &C_CamMetControlBarWidget::SigClearData);
   connect(this->mpc_Ui->pc_PushButtonToggleHex, &QPushButton::toggled,
           this, &C_CamMetControlBarWidget::SigDisplayAsHex);
   connect(this->mpc_Ui->pc_PushButtonToggleTimeMode, &QPushButton::toggled,
           this, &C_CamMetControlBarWidget::SigDisplayTimestampRelative);

   connect(this->mpc_Ui->pc_PushButtoneSearchNext, &QPushButton::clicked,
           this, &C_CamMetControlBarWidget::SearchNext);
   connect(this->mpc_Ui->pc_PushButtoneSearchPrev, &QPushButton::clicked,
           this, &C_CamMetControlBarWidget::SearchPrev);

   connect(this->mpc_Ui->pc_ComboBoxSearch->lineEdit(), &QLineEdit::returnPressed,
           this, &C_CamMetControlBarWidget::m_SearchComboBoxChanged);

   connect(this->mpc_Ui->pc_PushButtonTraceSettings, &QPushButton::clicked,
           this, &C_CamMetControlBarWidget::m_OpenTraceSettings);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetControlBarWidget::~C_CamMetControlBarWidget()
{
   // save position
   this->SaveUserSettings();

   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::InitStaticNames() const
{
   //Protocols
   this->mpc_Ui->pc_ComboBoxProtocol->clear();
   this->mpc_Ui->pc_ComboBoxProtocol->addItem("L7-Protocol: CAN Layer 2");
   this->mpc_Ui->pc_ComboBoxProtocol->addItem("L7-Protocol: STW openSYDE");
   this->mpc_Ui->pc_ComboBoxProtocol->addItem("L7-Protocol: CANopen");
   this->mpc_Ui->pc_ComboBoxProtocol->addItem("L7-Protocol: STW KEFEX");
   this->mpc_Ui->pc_ComboBoxProtocol->addItem("L7-Protocol: STW Flashloader");
   this->mpc_Ui->pc_ComboBoxProtocol->addItem("L7-Protocol: STW SHIP-IP!/IVA");
   this->mpc_Ui->pc_ComboBoxProtocol->addItem("L7-Protocol: SAE J1939 06/2006");

   this->mpc_Ui->pc_PushButtonTogglePlay->SetToolTipInformation(
      C_GtGetText::h_GetText("Start / Pause Measurement"),
      C_GtGetText::h_GetText("Start Measurement: \n"
                             "   - Initialize CAN DLL, start CAN communication\n"
                             "   - Display CAN messages in trace window\n"
                             "   - Send CAN messages configured in \"Message Generator\" window\n"
                             "Pause Measurement:\n"
                             "   - Freeze trace window content\n"
                             "   - Stop sending messages configured in \"Message Generator\" window\n"
                             "   - CAN communication will not be closed. CAN acknowledge is still enabled."));

   this->mpc_Ui->pc_PushButtonStop->SetToolTipInformation(
      C_GtGetText::h_GetText("Stop Measurement"),
      C_GtGetText::h_GetText("The CAN communication will be stopped and closed."));

   this->mpc_Ui->pc_PushButtonToggleTimeMode->SetToolTipInformation(
      C_GtGetText::h_GetText("Toggle Time Mode"),
      C_GtGetText::h_GetText("The displaying of the time can be switched between absolute and relative values."));

   this->mpc_Ui->pc_PushButtonToggleHex->SetToolTipInformation(
      C_GtGetText::h_GetText("Toggle Number Format"),
      C_GtGetText::h_GetText("The number format of CAN messages can be switched between decimal and hexadecimal."));

   this->mpc_Ui->pc_PushButtonToggleDisplayMode->SetToolTipInformation(
      C_GtGetText::h_GetText("Toggle Display Mode"),
      C_GtGetText::h_GetText("The display mode of CAN messages can be switched between static and continuous."));

   this->mpc_Ui->pc_ComboBoxProtocol->SetToolTipInformation(
      C_GtGetText::h_GetText("Layer 7-Protocol Interpretation"),
      C_GtGetText::h_GetText("Layer 7 protocol interpretation of CAN messages."));

   this->mpc_Ui->pc_PushButtonClear->SetToolTipInformation(
      C_GtGetText::h_GetText("Clear Trace"),
      C_GtGetText::h_GetText("Clear trace window content."));

   this->mpc_Ui->pc_PushButtonTraceSettings->SetToolTipInformation(
      C_GtGetText::h_GetText("Trace Settings"),
      C_GtGetText::h_GetText("Show and edit trace settings."));

   this->mpc_Ui->pc_ComboBoxSearch->lineEdit()->setPlaceholderText(C_GtGetText::h_GetText("Search..."));

   this->mpc_Ui->pc_ComboBoxSearch->SetToolTipInformation(
      C_GtGetText::h_GetText("Trace Search"),
      C_GtGetText::h_GetText("Search quickly and simply for text within the Trace.\n"
                             "Available only when Trace is paused or stopped."));

   this->mpc_Ui->pc_PushButtoneSearchNext->SetToolTipInformation(
      C_GtGetText::h_GetText("Trace Search"),
      C_GtGetText::h_GetText("Find next (F3)."));

   this->mpc_Ui->pc_PushButtoneSearchPrev->SetToolTipInformation(
      C_GtGetText::h_GetText("Trace Search"),
      C_GtGetText::h_GetText("Find previous (Shift + F3)."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::LoadUserSettings(void)
{
   //Buttons
   this->mpc_Ui->pc_PushButtonToggleHex->setChecked(C_UsHandler::h_GetInstance()->GetButtonHexActive());
   this->mpc_Ui->pc_PushButtonToggleTimeMode->setChecked(
      C_UsHandler::h_GetInstance()->GetButtonRelativeTimeStampActive());
   this->mpc_Ui->pc_PushButtonToggleDisplayMode->setChecked(C_UsHandler::h_GetInstance()->GetButtonUniqueViewActive());
   this->mpc_Ui->pc_ComboBoxProtocol->setCurrentIndex(C_UsHandler::h_GetInstance()->GetSelectedProtocolIndex());

   // Trace settings
   this->mq_DisplayTimestampAbsoluteTimeOfDay =
      C_UsHandler::h_GetInstance()->GetTraceSettingDisplayTimestampAbsoluteTimeOfDay();
   this->mu32_TraceBufferSize = C_UsHandler::h_GetInstance()->GetTraceSettingBufferSize();

   Q_EMIT (this->SigDisplayTimestampTimeOfDay(this->mq_DisplayTimestampAbsoluteTimeOfDay));
   Q_EMIT (this->SigTraceBufferSize(this->mu32_TraceBufferSize));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::SaveUserSettings(void) const
{
   //Buttons
   C_UsHandler::h_GetInstance()->SetButtonHexActive(this->mpc_Ui->pc_PushButtonToggleHex->isChecked());
   C_UsHandler::h_GetInstance()->SetButtonRelativeTimeStampActive(this->mpc_Ui->pc_PushButtonToggleTimeMode->isChecked());
   C_UsHandler::h_GetInstance()->SetButtonUniqueViewActive(this->mpc_Ui->pc_PushButtonToggleDisplayMode->isChecked());
   C_UsHandler::h_GetInstance()->SetSelectedProtocolIndex(this->mpc_Ui->pc_ComboBoxProtocol->currentIndex());

   // Trace settings
   C_UsHandler::h_GetInstance()->SetTraceSettingDisplayTimestampAbsoluteTimeOfDay(
      this->mq_DisplayTimestampAbsoluteTimeOfDay);
   C_UsHandler::h_GetInstance()->SetTraceSettingBufferSize(this->mu32_TraceBufferSize);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Stopping logging in case of an error
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::StopLogging(void)
{
   this->m_HandleStop();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the focus on SearchBar. If Trace is currently running, user gets a message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::SearchBarFocus()
{
   if (this->mpc_Ui->pc_PushButtonTogglePlay->isChecked() == true)
   {
      this->m_MessageSearchWhileTracing();
   }
   else
   {
      this->mpc_Ui->pc_ComboBoxSearch->lineEdit()->setFocus();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Triggers searching the next match
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::SearchNext(void)
{
   if ((this->mpc_Ui->pc_PushButtonStop->isEnabled() == false) ||
       (this->mpc_Ui->pc_PushButtonTogglePlay->isChecked() == false))
   {
      Q_EMIT (this->SigSearchTrace(this->mpc_Ui->pc_ComboBoxSearch->currentText(), true));

      this->m_UpdateSearchComboBox();
   }
   else
   {
      this->m_MessageSearchWhileTracing();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Triggers searching the previous match
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::SearchPrev(void)
{
   if ((this->mpc_Ui->pc_PushButtonStop->isEnabled() == false) ||
       (this->mpc_Ui->pc_PushButtonTogglePlay->isChecked() == false))
   {
      Q_EMIT (this->SigSearchTrace(this->mpc_Ui->pc_ComboBoxSearch->currentText(), false));

      this->m_UpdateSearchComboBox();
   }
   else
   {
      this->m_MessageSearchWhileTracing();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle toggle of action playing state

   \param[in] oq_Playing Is playing flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::m_HandleTogglePlay(const bool oq_Playing)
{
   if (oq_Playing == false)
   {
      //Pause if not stopped already
      if (this->mpc_Ui->pc_PushButtonStop->isEnabled() == true)
      {
         Q_EMIT this->SigPauseLogging();
      }
   }
   else
   {
      if (this->mpc_Ui->pc_PushButtonStop->isEnabled() == false)
      {
         //Signal state: active
         this->mpc_Ui->pc_PushButtonStop->setEnabled(true);

         Q_EMIT this->SigStartLogging();
      }
      else
      {
         //Continue
         Q_EMIT this->SigContinueLogging();
      }
   }
   //In all cases handle tree visibility
   m_HandleTreeVisibility();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle stop button press
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::m_HandleStop(void)
{
   //Signal state: not active
   this->mpc_Ui->pc_PushButtonStop->setEnabled(false);
   //Signal not playing anymore
   this->mpc_Ui->pc_PushButtonTogglePlay->setChecked(false);

   Q_EMIT (this->SigStopLogging());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle selected protocol change

   \param[in] os32_Index Selected protocol index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::m_OnProtocolIndexChange(const int32_t os32_Index)
{
   Q_EMIT this->SigChangeProtocol(C_CamMetControlBarWidget::mh_GetProtocolFromIndex(os32_Index));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the protocol type

   \param[in] os32_Protocol Index of protocol

   \return
   Protocol type as enum
*/
//----------------------------------------------------------------------------------------------------------------------
stw::cmon_protocol::e_CanMonL7Protocols C_CamMetControlBarWidget::mh_GetProtocolFromIndex(const int32_t os32_Protocol)
{
   stw::cmon_protocol::e_CanMonL7Protocols e_Retval;

   switch (os32_Protocol)
   {
   case 1:
      e_Retval = stw::cmon_protocol::CMONL7ProtocolOpenSYDE;
      break;
   case 2:
      e_Retval = stw::cmon_protocol::CMONL7ProtocolCANopen;
      break;
   case 3:
      e_Retval = stw::cmon_protocol::CMONL7ProtocolKEFEX;
      break;
   case 4:
      e_Retval = stw::cmon_protocol::CMONL7ProtocolXFL;
      break;
   case 5:
      e_Retval = stw::cmon_protocol::CMONL7Protocol_SHIP_IP_IVA;
      break;
   case 6:
      e_Retval = stw::cmon_protocol::CMONL7ProtocolJ1939;
      break;
   default:
      e_Retval = stw::cmon_protocol::CMONL7ProtocolNone;
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle the tree visibility
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::m_HandleTreeVisibility(void)
{
   //Either if currently no communication traced or if the display mode is static
   if ((this->mpc_Ui->pc_PushButtonTogglePlay->isChecked() == false) ||
       (this->mpc_Ui->pc_PushButtonToggleDisplayMode->isChecked() == true))
   {
      //Display tree in these cases
      Q_EMIT (this->SigDisplayTree(true));
   }
   else
   {
      //Default: tree not visible
      Q_EMIT (this->SigDisplayTree(false));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set display mode: display unique messages

   \param[in] oq_Active Active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::m_HandleToggleDisplayMode(const bool oq_Active)
{
   Q_EMIT (this->SigDisplayUniqueMessages(oq_Active));
   //In all cases handle tree visibility
   this->m_HandleTreeVisibility();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The text

   \param[in]       orc_Text    Current text of combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::m_SearchComboBoxChanged(void) const
{
   if ((this->mpc_Ui->pc_PushButtonStop->isEnabled() == false) ||
       (this->mpc_Ui->pc_PushButtonTogglePlay->isChecked() == false))
   {
      if (this->mpc_Ui->pc_ComboBoxSearch->count() > mhs32_MAX_COUNT_COMBO_BOX)
      {
         this->mpc_Ui->pc_ComboBoxSearch->removeItem(mhs32_MAX_COUNT_COMBO_BOX);
      }

      Q_EMIT (this->SigSearchTrace(this->mpc_Ui->pc_ComboBoxSearch->currentText(), true));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds the searched string, if it is not already in the list
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::m_UpdateSearchComboBox(void) const
{
   int32_t s32_Index;
   const QString c_CurrentText = this->mpc_Ui->pc_ComboBoxSearch->currentText();
   bool q_Found = false;

   for (s32_Index = 0U; s32_Index < this->mpc_Ui->pc_ComboBoxSearch->count(); ++s32_Index)
   {
      if (this->mpc_Ui->pc_ComboBoxSearch->itemText(s32_Index) == c_CurrentText)
      {
         q_Found = true;
      }
   }

   if (q_Found == false)
   {
      this->mpc_Ui->pc_ComboBoxSearch->insertItem(0, c_CurrentText);

      // Remove the last item if there are to many entries
      if (this->mpc_Ui->pc_ComboBoxSearch->count() > mhs32_MAX_COUNT_COMBO_BOX)
      {
         this->mpc_Ui->pc_ComboBoxSearch->removeItem(mhs32_MAX_COUNT_COMBO_BOX);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Opens the trace settings dialog, saves and activates the changed configurations
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::m_OpenTraceSettings(void)
{
   //Set parent for better hierarchy handling via window manager
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this->parentWidget(), this->parentWidget());
   C_CamMetSettingsPopup * const pc_Dialog = new C_CamMetSettingsPopup(*c_New);

   //Resize
   c_New->SetSize(QSize(600, 350));

   pc_Dialog->SetValues(this->mq_DisplayTimestampAbsoluteTimeOfDay, this->mu32_TraceBufferSize);

   // Update settings on accept
   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      this->mq_DisplayTimestampAbsoluteTimeOfDay = pc_Dialog->GetDisplayTimestampAbsoluteTimeOfDay();
      this->mu32_TraceBufferSize = pc_Dialog->GetTraceBufferSize();

      Q_EMIT (this->SigDisplayTimestampTimeOfDay(this->mq_DisplayTimestampAbsoluteTimeOfDay));
      Q_EMIT (this->SigTraceBufferSize(this->mu32_TraceBufferSize));
   }

   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
} //lint !e429  no memory leak because of the parent pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Generates a message for the user. User may not use Search while Trace is running
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::m_MessageSearchWhileTracing()
{
   C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eINFORMATION);

   c_MessageBox.SetHeading(C_GtGetText::h_GetText("Trace Search"));
   c_MessageBox.SetDescription(C_GtGetText::h_GetText("Available only if Trace is paused or stopped"));
   c_MessageBox.Execute();
}
