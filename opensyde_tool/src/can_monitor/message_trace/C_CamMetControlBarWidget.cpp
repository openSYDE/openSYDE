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
#include "precomp_headers.h"

#include "C_CamMetControlBarWidget.h"
#include "ui_C_CamMetControlBarWidget.h"

#include "C_UsHandler.h"
#include "C_GtGetText.h"

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
C_CamMetControlBarWidget::C_CamMetControlBarWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_CamMetControlBarWidget)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

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
   this->mpc_Ui->pc_PushButtonClear->SetSvg("://images/IconWipeAwayEnabled.svg", "", "://images/IconWipeAwayHover.svg",
                                            "", "", "",
                                            "://images/IconWipeAwayPressed.svg");
   this->mpc_Ui->pc_PushButtonToggleHex->SetSvg("://images/IconNumberFormatDec.svg", "",
                                                "://images/IconNumberFormatDecHover.svg",
                                                "://images/IconNumberFormatHex.svg", "",
                                                "://images/IconNumberFormatHexHover.svg",
                                                "://images/IconNumberFormatDecPressed.svg",
                                                "://images/IconNumberFormatHexPressed.svg");
   this->mpc_Ui->pc_PushButtonToggleTimeMode->SetSvg("://images/IconTimeModeAbsolute.svg", "",
                                                     "://images/IconTimeModeAbsoluteHover.svg",
                                                     "://images/IconTimeModeRelative.svg", "",
                                                     "://images/IconTimeModeRelativeHover.svg",
                                                     "://images/IconTimeModeAbsolutePressed.svg",
                                                     "://images/IconTimeModeRelativePressed.svg");
   this->mpc_Ui->pc_PushButtonToggleDisplayMode->SetSvg("://images/IconToggleToUniqueOff.svg", "",
                                                        "://images/IconToggleToUniqueOffHover.svg",
                                                        "://images/IconToggleToUniqueOn.svg", "",
                                                        "://images/IconToggleToUniqueOnHover.svg",
                                                        "://images/IconToggleToUniqueOffPressed.svg",
                                                        "://images/IconToggleToUniqueOnPressed.svg");

   connect(this->mpc_Ui->pc_PushButtonTogglePlay, &QPushButton::toggled, this,
           &C_CamMetControlBarWidget::m_HandleTogglePlay);
   connect(this->mpc_Ui->pc_PushButtonStop, &QPushButton::clicked, this,
           &C_CamMetControlBarWidget::m_HandleStop);
   connect(this->mpc_Ui->pc_PushButtonToggleDisplayMode, &QPushButton::toggled, this,
           &C_CamMetControlBarWidget::m_HandleToggleDisplayMode);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxProtocol, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
           this, &C_CamMetControlBarWidget::m_OnProtocolIndexChange);

   connect(this->mpc_Ui->pc_PushButtonClear, &QPushButton::clicked,
           this, &C_CamMetControlBarWidget::SigClearData);
   connect(this->mpc_Ui->pc_PushButtonToggleHex, &QPushButton::toggled,
           this, &C_CamMetControlBarWidget::SigDisplayAsHex);
   connect(this->mpc_Ui->pc_PushButtonToggleTimeMode, &QPushButton::toggled,
           this, &C_CamMetControlBarWidget::SigDisplayTimestampRelative);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetControlBarWidget::~C_CamMetControlBarWidget()
{
   // save position
   this->SaveUserSettings();

   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
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
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::LoadUserSettings(void) const
{
   //Buttons
   this->mpc_Ui->pc_PushButtonToggleHex->setChecked(C_UsHandler::h_GetInstance()->GetButtonHexActive());
   this->mpc_Ui->pc_PushButtonToggleTimeMode->setChecked(
      C_UsHandler::h_GetInstance()->GetButtonRelativeTimeStampActive());
   this->mpc_Ui->pc_PushButtonToggleDisplayMode->setChecked(C_UsHandler::h_GetInstance()->GetButtonUniqueViewActive());
   this->mpc_Ui->pc_ComboBoxProtocol->setCurrentIndex(C_UsHandler::h_GetInstance()->GetSelectedProtocolIndex());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::SaveUserSettings(void) const
{
   //Buttons
   C_UsHandler::h_GetInstance()->SetButtonHexActive(this->mpc_Ui->pc_PushButtonToggleHex->isChecked());
   C_UsHandler::h_GetInstance()->SetButtonRelativeTimeStampActive(this->mpc_Ui->pc_PushButtonToggleTimeMode->isChecked());
   C_UsHandler::h_GetInstance()->SetButtonUniqueViewActive(this->mpc_Ui->pc_PushButtonToggleDisplayMode->isChecked());
   C_UsHandler::h_GetInstance()->SetSelectedProtocolIndex(this->mpc_Ui->pc_ComboBoxProtocol->currentIndex());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stopping logging in case of an error
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::StopLogging(void)
{
   this->m_HandleStop();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle toggle of action playing state

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
/*! \brief   Handle stop button press
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
/*! \brief   Handle selected protocol change

   \param[in] osn_Index Selected protocol index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::m_OnProtocolIndexChange(const sintn osn_Index)
{
   Q_EMIT this->SigChangeProtocol(C_CamMetControlBarWidget::mh_GetProtocolFromIndex(osn_Index));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the protocol type

   \param[in]     os32_Protocol         Index of protocol

   \return
   Protocol type as enum
*/
//----------------------------------------------------------------------------------------------------------------------
stw_cmon_protocol::e_CMONL7Protocols C_CamMetControlBarWidget::mh_GetProtocolFromIndex(const sint32 os32_Protocol)
{
   stw_cmon_protocol::e_CMONL7Protocols e_Retval;

   switch (os32_Protocol)
   {
   case 1:
      e_Retval = stw_cmon_protocol::CMONL7ProtocolOpenSYDE;
      break;
   case 2:
      e_Retval = stw_cmon_protocol::CMONL7ProtocolCANopen;
      break;
   case 3:
      e_Retval = stw_cmon_protocol::CMONL7ProtocolKEFEX;
      break;
   case 4:
      e_Retval = stw_cmon_protocol::CMONL7ProtocolXFL;
      break;
   case 5:
      e_Retval = stw_cmon_protocol::CMONL7Protocol_SHIP_IP_IVA;
      break;
   case 6:
      e_Retval = stw_cmon_protocol::CMONL7ProtocolJ1939;
      break;
   default:
      e_Retval = stw_cmon_protocol::CMONL7ProtocolNone;
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle the tree visibility
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::m_HandleTreeVisibility(void)
{
   //Either if currently no communication traced or if the display mode is static
   if ((this->mpc_Ui->pc_PushButtonTogglePlay->isChecked() == false) ||
       (this->mpc_Ui->pc_PushButtonToggleDisplayMode->isChecked() == true))
   {
      //Display tree in these cases
      Q_EMIT this->SigDisplayTree(true);
   }
   else
   {
      //Default: tree not visible
      Q_EMIT this->SigDisplayTree(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set display mode: display unique messages

   \param[in] oq_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetControlBarWidget::m_HandleToggleDisplayMode(const bool oq_Active)
{
   Q_EMIT this->SigDisplayUniqueMessages(oq_Active);
   //In all cases handle tree visibility
   this->m_HandleTreeVisibility();
}
