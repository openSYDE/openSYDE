//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for editing bus properties

   Widget for editing bus properties

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QSpinBox>

#include "stwerrors.hpp"
#include "C_SdBueBusEditPropertiesWidget.hpp"
#include "C_GtGetText.hpp"
#include "C_OscUtils.hpp"
#include "C_SdUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "TglUtils.hpp"
#include "ui_C_SdBueBusEditPropertiesWidget.h"
#include "constants.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::scl;
using namespace stw::tgl;

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
C_SdBueBusEditPropertiesWidget::C_SdBueBusEditPropertiesWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueBusEditPropertiesWidget),
   mu32_BusIndex(0)
{
   bool q_UsableForRoutingDisabled;

   // init UI
   mpc_Ui->setupUi(this);

   //Ui restriction
   this->mpc_Ui->pc_SpinBoxBusId->SetMaximumCustom(15);

   this->mpc_Ui->pc_ComboBoxBitRate->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                           C_GtGetText::h_GetText(""),
                                                           C_NagToolTip::eDEFAULT);

   InitStaticNames();

   // Usable for Routing
   q_UsableForRoutingDisabled = (this->mpc_Ui->pc_CheckBoxUsableForRouting->isChecked() == false);
   this->m_UpdateUsableForRoutingUi(q_UsableForRoutingDisabled);
   this->mpc_Ui->pc_HorizontalLayoutBusId->setAlignment(this->mpc_Ui->pc_SpinBoxBusId, Qt::AlignLeft);

   //Name restriction
   this->mpc_Ui->pc_LineEditBusName->setMaxLength(ms32_C_ITEM_MAX_CHAR_COUNT);

   // connects
   connect(this->mpc_Ui->pc_LineEditBusName, &QLineEdit::textChanged, this,
           &C_SdBueBusEditPropertiesWidget::m_CheckBusName);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueBusEditPropertiesWidget::~C_SdBueBusEditPropertiesWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditPropertiesWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelName->setText(C_GtGetText::h_GetText("Name"));
   this->mpc_Ui->pc_LabelComment->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_LabelConfiguration->setText(C_GtGetText::h_GetText("Configuration"));
   this->mpc_Ui->pc_LabelBusId->setText(C_GtGetText::h_GetText("Bus ID"));
   this->mpc_Ui->pc_LabelBitRate->setText(C_GtGetText::h_GetText("Bitrate"));

   this->mpc_Ui->pc_TextEditComment->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here ..."));

   //Tool tips
   this->mpc_Ui->pc_LabelName->SetToolTipInformation(C_GtGetText::h_GetText("Name"),
                                                     C_GtGetText::h_GetText(
                                                        "Symbolic bus name. Unique within Network Topology.\n"
                                                        "\nC naming conventions must be followed:"
                                                        "\n - must not be empty"
                                                        "\n - must not start with digits"
                                                        "\n - only alphanumeric characters and \"_\""
                                                        "\n - should not be longer than 31 characters"));
   this->mpc_Ui->pc_LabelComment->SetToolTipInformation(C_GtGetText::h_GetText("Comment"),
                                                        C_GtGetText::h_GetText("Comment for this bus."));
   this->mpc_Ui->pc_LabelBusId->SetToolTipInformation(C_GtGetText::h_GetText("Bus ID"),
                                                      C_GtGetText::h_GetText(
                                                         "Unique within Network Topology. Range: 0-15."
                                                         "\nThe ID is used for addressing in the communication protocol."));
   this->mpc_Ui->pc_LabelBitRate->SetToolTipInformation(C_GtGetText::h_GetText("Bitrate"),
                                                        C_GtGetText::h_GetText("CAN bus bitrate"));
   this->mpc_Ui->pc_CheckBoxUsableForRouting->SetToolTipInformation(C_GtGetText::h_GetText("Usable for Routing"),
                                                                    C_GtGetText::h_GetText(
                                                                       "If disabled, the bus is no more usable for routing."));
   this->mpc_Ui->pc_LabelNotRelevant->SetToolTipInformation(C_GtGetText::h_GetText("Bus ID not relevant"),
                                                            C_GtGetText::h_GetText(
                                                               "Bus ID is not relevant when the option "
                                                               "\"Usable for Routing\" is disabled. \nUniqueness check "
                                                               "is also disabled in this state."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Selects the node name in the text edit for fast editing
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditPropertiesWidget::SelectName(void) const
{
   this->mpc_Ui->pc_LineEditBusName->setFocus();
   this->mpc_Ui->pc_LineEditBusName->selectAll();
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Bus ID setter

   Sets the private bus id of widget

   \param[in] ou32_BusIndex New bus id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditPropertiesWidget::SetBusId(const uint32_t ou32_BusIndex)
{
   this->mu32_BusIndex = ou32_BusIndex;

   //load bus data
   this->m_LoadFromData();

   //Initial check
   this->m_CheckBusName();
   this->m_CheckBusId();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load bus information

   Load bus information from core data using bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditPropertiesWidget::m_LoadFromData(void)
{
   const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(this->mu32_BusIndex);

   tgl_assert(pc_Bus != NULL);

   if (pc_Bus != NULL)
   {
      //disconnect RegisterChange
      disconnect(this->mpc_Ui->pc_LineEditBusName, &QLineEdit::editingFinished, this,
                 &C_SdBueBusEditPropertiesWidget::m_RegisterNameChange);
      disconnect(this->mpc_Ui->pc_TextEditComment, &QTextEdit::textChanged, this,
                 &C_SdBueBusEditPropertiesWidget::m_RegisterChange);
      //lint -e{929} Cast required to avoid ambiguous signal of qt interface
      disconnect(this->mpc_Ui->pc_SpinBoxBusId, static_cast<void (QSpinBox::*)(
                                                               int32_t)>(&C_OgeSpxNumber::valueChanged), this,
                 &C_SdBueBusEditPropertiesWidget::m_RegisterIdChange);
      disconnect(this->mpc_Ui->pc_CheckBoxUsableForRouting, &QCheckBox::stateChanged, this,
                 &C_SdBueBusEditPropertiesWidget::m_RegisterIdChange);
      //lint -e{929} Cast required to avoid ambiguous signal of qt interface
      disconnect(this->mpc_Ui->pc_ComboBoxBitRate, static_cast<void (QComboBox::*)(
                                                                  int32_t)>(&C_OgeCbxText::currentIndexChanged), this,
                 &C_SdBueBusEditPropertiesWidget::m_RegisterChange);
      disconnect(this->mpc_Ui->pc_ComboBoxBitRate, &C_OgeCbxText::SigErrorFixed, this,
                 &C_SdBueBusEditPropertiesWidget::m_CanBitrateFixed);
      disconnect(this->mpc_Ui->pc_CheckBoxUsableForRouting, &QCheckBox::toggled, this,
                 &C_SdBueBusEditPropertiesWidget::m_HandleUsableForRoutingEnableChanged);

      //name
      this->mpc_Ui->pc_LineEditBusName->setText(pc_Bus->c_Name.c_str());

      //comment
      this->mpc_Ui->pc_TextEditComment->setText(pc_Bus->c_Comment.c_str());

      //Bus id
      this->mpc_Ui->pc_SpinBoxBusId->setValue(static_cast<int32_t>(pc_Bus->u8_BusId));

      //Bit rate
      if (pc_Bus->e_Type == C_OscSystemBus::eCAN)
      {
         std::vector<uint32_t> c_ConnectedNodes;
         std::vector<uint32_t> c_ConnectedInterfaces;
         std::vector<uint32_t> c_SupportedBitrates;
         uint32_t u32_BitrateCounter;
         const uint32_t u32_CurrentSetBitrate = static_cast<uint32_t>(pc_Bus->u64_BitRate / 1000ULL);
         const QString c_CurrentSetBitrate = this->m_GetComboBoxString(u32_CurrentSetBitrate);
         bool q_CurrentSetBitrateFound = false;

         C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(this->mu32_BusIndex,
                                                                                            c_ConnectedNodes,
                                                                                            c_ConnectedInterfaces);
         C_PuiSdHandler::h_GetInstance()->GetSupportedCanBitrates(c_ConnectedNodes, c_SupportedBitrates);

         //Fill combo box
         for (u32_BitrateCounter = 0U; u32_BitrateCounter < c_SupportedBitrates.size(); ++u32_BitrateCounter)
         {
            this->mpc_Ui->pc_ComboBoxBitRate->addItem(this->m_GetComboBoxString(c_SupportedBitrates[u32_BitrateCounter]));

            if (c_SupportedBitrates[u32_BitrateCounter] == u32_CurrentSetBitrate)
            {
               q_CurrentSetBitrateFound = true;
            }
         }

         if (q_CurrentSetBitrateFound == true)
         {
            this->mpc_Ui->pc_ComboBoxBitRate->setCurrentText(c_CurrentSetBitrate);
         }
         else
         {
            // Error case
            this->mpc_Ui->pc_ComboBoxBitRate->SetTemporaryText(c_CurrentSetBitrate, true);
         }
         if (q_CurrentSetBitrateFound == true)
         {
            this->mpc_Ui->pc_ComboBoxBitRate->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                                    C_GtGetText::h_GetText(""),
                                                                    C_NagToolTip::eDEFAULT);
         }
         else
         {
            QString c_Content;
            const QString c_Heading = C_GtGetText::h_GetText(
               "Selected bitrate not supported by following connected nodes:");
            std::vector<QString> c_InvalidNodesForBitRate;
            C_PuiSdHandler::h_GetInstance()->CheckBusConflictDetailed(this->mu32_BusIndex, NULL, NULL, NULL,
                                                                      &c_InvalidNodesForBitRate, NULL);
            for (uint32_t u32_ItNode = 0UL; (u32_ItNode < c_InvalidNodesForBitRate.size()) &&
                 (u32_ItNode < mu32_TOOL_TIP_MAXIMUM_ITEMS); ++u32_ItNode)
            {
               c_Content += "- " + c_InvalidNodesForBitRate[u32_ItNode] + "\n";
            }
            if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidNodesForBitRate.size())
            {
               c_Content += static_cast<QString>("+%1\n").arg(
                  static_cast<uint32_t>(c_InvalidNodesForBitRate.size()) - mu32_TOOL_TIP_MAXIMUM_ITEMS);
            }
            this->mpc_Ui->pc_ComboBoxBitRate->SetToolTipInformation(c_Heading, c_Content, C_NagToolTip::eERROR);
         }

         this->mpc_Ui->pc_ComboBoxBitRate->setVisible(true);
         this->mpc_Ui->pc_LabelBitRate->setVisible(true);
      }
      else
      {
         // As dummy for saving
         this->mpc_Ui->pc_ComboBoxBitRate->addItem(this->m_GetComboBoxString(1000));
         this->mpc_Ui->pc_ComboBoxBitRate->setCurrentIndex(0);

         this->mpc_Ui->pc_ComboBoxBitRate->setVisible(false);
         this->mpc_Ui->pc_LabelBitRate->setVisible(false);
      }

      //Picture / text
      this->mpc_Ui->pc_BusType->setText(C_PuiSdUtil::h_ConvertBusTypeToStringUppercase(pc_Bus->e_Type));
      if (pc_Bus->e_Type == C_OscSystemBus::eCAN)
      {
         this->mpc_Ui->pc_LabBusImage->SetSvg("://images/system_definition/BusEdit/ImageBus.svg");
      }
      else
      {
         this->mpc_Ui->pc_LabBusImage->SetSvg("://images/system_definition/BusEdit/ImageBusEth.svg");
      }

      // Usable for Routing
      this->mpc_Ui->pc_CheckBoxUsableForRouting->setChecked(pc_Bus->q_UseableForRouting);
      this->m_UpdateUsableForRoutingUi(!pc_Bus->q_UseableForRouting);

      //connects for RegisterChange
      connect(this->mpc_Ui->pc_LineEditBusName, &QLineEdit::editingFinished, this,
              &C_SdBueBusEditPropertiesWidget::m_RegisterNameChange);
      connect(this->mpc_Ui->pc_TextEditComment, &QTextEdit::textChanged, this,
              &C_SdBueBusEditPropertiesWidget::m_RegisterChange);
      //lint -e{929} Cast required to avoid ambiguous signal of qt interface
      connect(this->mpc_Ui->pc_SpinBoxBusId, static_cast<void (QSpinBox::*)(
                                                            int32_t)>(&C_OgeSpxNumber::valueChanged), this,
              &C_SdBueBusEditPropertiesWidget::m_RegisterIdChange);
      connect(this->mpc_Ui->pc_CheckBoxUsableForRouting, &QCheckBox::stateChanged, this,
              &C_SdBueBusEditPropertiesWidget::m_RegisterIdChange);
      //lint -e{929} Cast required to avoid ambiguous signal of qt interface
      connect(this->mpc_Ui->pc_ComboBoxBitRate, static_cast<void (QComboBox::*)(
                                                               int32_t)>(&C_OgeCbxText::currentIndexChanged), this,
              &C_SdBueBusEditPropertiesWidget::m_RegisterChange);
      connect(this->mpc_Ui->pc_ComboBoxBitRate, &C_OgeCbxText::SigErrorFixed, this,
              &C_SdBueBusEditPropertiesWidget::m_CanBitrateFixed);
      connect(this->mpc_Ui->pc_CheckBoxUsableForRouting, &QCheckBox::toggled, this,
              &C_SdBueBusEditPropertiesWidget::m_HandleUsableForRoutingEnableChanged);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditPropertiesWidget::m_CanBitrateFixed(void) const
{
   this->mpc_Ui->pc_ComboBoxBitRate->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                           C_GtGetText::h_GetText(""),
                                                           C_NagToolTip::eDEFAULT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle "Usable for Routing" state of the bus

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditPropertiesWidget::m_HandleUsableForRoutingEnableChanged()
{
   bool q_UsableForRoutingDisabled = (this->mpc_Ui->pc_CheckBoxUsableForRouting->isChecked() == false);

   if (q_UsableForRoutingDisabled == true)
   {
      // Confirm if the user really wants to disable the routing for the bus
      if (this->m_ConfirmDisableRouting() == false)
      {
         disconnect(this->mpc_Ui->pc_CheckBoxUsableForRouting, &QCheckBox::toggled, this,
                    &C_SdBueBusEditPropertiesWidget::m_HandleUsableForRoutingEnableChanged);

         this->mpc_Ui->pc_CheckBoxUsableForRouting->setChecked(true);
         q_UsableForRoutingDisabled =  (this->mpc_Ui->pc_CheckBoxUsableForRouting->isChecked() == false);

         connect(this->mpc_Ui->pc_CheckBoxUsableForRouting, &QCheckBox::toggled, this,
                 &C_SdBueBusEditPropertiesWidget::m_HandleUsableForRoutingEnableChanged);
      }
   }
   this->m_UpdateUsableForRoutingUi(q_UsableForRoutingDisabled);
   this->m_RegisterChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update the GUI when the user modifies the "Usable for Routing" setting

  \param[in]       oq_Enabled     True / False
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditPropertiesWidget::m_UpdateUsableForRoutingUi(const bool oq_Enabled)
{
   // Bus Id and the label are modified accordingly
   this->mpc_Ui->pc_LabelNotRelevant->setVisible(oq_Enabled);
   this->mpc_Ui->pc_SpinBoxBusId->setEnabled(!oq_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Prompt user when routing is disabled for the bus

   \return
   true if user selects button with text "Yes"
   false otherwise
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueBusEditPropertiesWidget::m_ConfirmDisableRouting()
{
   bool q_Return = false;

   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eQUESTION);

   c_Message.SetHeading(C_GtGetText::h_GetText("Usable for Routing"));
   c_Message.SetDescription(C_GtGetText::h_GetText("Do you really want to disable routing for this bus?"));
   c_Message.SetDetails(C_GtGetText::h_GetText("This will disable the routing flags of all node interfaces "
                                               "connected to this bus."));
   c_Message.SetCustomMinHeight(180, 270);

   c_Message.SetNoButtonText(C_GtGetText::h_GetText("Cancel"));
   c_Message.SetOkButtonText(C_GtGetText::h_GetText("Disable Routing"));

   if (c_Message.Execute() == C_OgeWiCustomMessage::eYES)
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save ui data to bus

   Is called from outside
      - on system definition save
      - on page change

   \return   false:
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditPropertiesWidget::SaveToData(void) const
{
   const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(this->mu32_BusIndex);

   tgl_assert(pc_Bus != NULL);
   if (pc_Bus != NULL)
   {
      //copy current bus
      C_OscSystemBus c_NewBus = *pc_Bus;

      //name
      //Only accept new name if not in conflict
      if (C_PuiSdHandler::h_GetInstance()->CheckBusNameAvailable(
             this->mpc_Ui->pc_LineEditBusName->text().toStdString().c_str(), &this->mu32_BusIndex, NULL))
      {
         c_NewBus.c_Name = this->mpc_Ui->pc_LineEditBusName->text().toStdString().c_str();
      }
      else
      {
         //Restore previous name
         c_NewBus.c_Name = pc_Bus->c_Name.c_str();
      }

      //comment
      c_NewBus.c_Comment = this->mpc_Ui->pc_TextEditComment->toPlainText().toStdString().c_str();

      //Bus id
      c_NewBus.u8_BusId = static_cast<uint8_t>(this->mpc_Ui->pc_SpinBoxBusId->value());

      //Bit rate
      if (c_NewBus.e_Type == C_OscSystemBus::eCAN)
      {
         // Bitrate is shown as kbit/s
         c_NewBus.u64_BitRate =
            static_cast<uint64_t>(this->m_GetBitrateFromComboBoxString(this->mpc_Ui->pc_ComboBoxBitRate->currentText()))
            *
            1000ULL;
      }
      else
      {
         // As dummy
         c_NewBus.u64_BitRate = 1000000ULL;
      }

      // Usable for Routing
      c_NewBus.q_UseableForRouting = this->mpc_Ui->pc_CheckBoxUsableForRouting->isChecked();

      //save new bus
      C_PuiSdHandler::h_GetInstance()->SetOscBus(this->mu32_BusIndex, c_NewBus);

      // Disable routing settings for all nodes connected to this bus.
      // NOTE: Only "disabling" routing for the bus is rendered to all nodes. Enabling routing for the bus
      // will not reverse the action for all nodes
      C_PuiSdHandler::h_GetInstance()->SetAutomaticBusRoutingSettings(this->mu32_BusIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
QString C_SdBueBusEditPropertiesWidget::m_GetComboBoxString(const uint32_t ou32_Bitrate) const
{
   const QString c_Text = QString::number(ou32_Bitrate) + static_cast<QString>(" kbit/s");

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdBueBusEditPropertiesWidget::m_GetBitrateFromComboBoxString(const QString & orc_Entry) const
{
   QString c_Bitrate = orc_Entry;
   // In case of an error it returns 0
   const uint32_t u32_Bitrate = c_Bitrate.remove(" kbit/s").toInt();

   return u32_Bitrate;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check bus name
   - check input
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditPropertiesWidget::m_CheckBusName(void)
{
   const QString c_Text = this->mpc_Ui->pc_LineEditBusName->text();
   const bool q_NameIsUnique = C_PuiSdHandler::h_GetInstance()->CheckBusNameAvailable(
      c_Text.toStdString().c_str(), &this->mu32_BusIndex, NULL);
   const bool q_NameIsValid = C_OscUtils::h_CheckValidCeName(
      this->mpc_Ui->pc_LineEditBusName->text().toStdString().c_str());

   //set invalid text property
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditBusName, "Valid", q_NameIsUnique && q_NameIsValid);

   if ((q_NameIsUnique == true) && (q_NameIsValid == true))
   {
      this->mpc_Ui->pc_LineEditBusName->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                              C_GtGetText::h_GetText(""),
                                                              C_NagToolTip::eDEFAULT);
   }
   else
   {
      const QString c_Heading = C_GtGetText::h_GetText("Bus Name");
      QString c_Content;
      if (q_NameIsUnique == false)
      {
         c_Content += C_GtGetText::h_GetText("- is already in use");
      }
      if (q_NameIsValid == false)
      {
         c_Content += C_GtGetText::h_GetText("- is empty or contains invalid characters.");
      }
      this->mpc_Ui->pc_LineEditBusName->SetToolTipInformation(c_Heading, c_Content, C_NagToolTip::eERROR);
   }

   Q_EMIT this->SigChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check bus id
   - check input
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditPropertiesWidget::m_CheckBusId(void) const
{
   bool q_IdIsValid;

   //check
   tgl_assert(C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().CheckErrorBus(this->mu32_BusIndex,
                                                                                           NULL,
                                                                                           NULL, &q_IdIsValid,
                                                                                           NULL) == C_NO_ERR);
   //Result signals error
   q_IdIsValid = !q_IdIsValid;

   //set invalid text property
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxBusId, "Valid", q_IdIsValid);

   if (q_IdIsValid == true)
   {
      this->mpc_Ui->pc_SpinBoxBusId->SetToolTipAdditionalInfo("", C_NagToolTip::eDEFAULT);
   }
   else
   {
      const std::vector<uint32_t> c_UsedIds = C_SdUtil::h_GetUsedBusIdsUniqueAndSortedAscending(this->mu32_BusIndex);
      const QString c_Content = C_SdUtil::h_InitUsedIdsString(c_UsedIds, "", "", true);
      this->mpc_Ui->pc_SpinBoxBusId->SetToolTipAdditionalInfo(c_Content, C_NagToolTip::eERROR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trimm bus name

   Remove whitespaces at the beginning and end of the string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditPropertiesWidget::m_TrimmBusName(void) const
{
   this->mpc_Ui->pc_LineEditBusName->setText(this->mpc_Ui->pc_LineEditBusName->text().trimmed());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register Change

   Function where ui elements register a change. Change will be sent via a signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditPropertiesWidget::m_RegisterChange(void)
{
   SaveToData();
   //signal
   Q_EMIT this->SigChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle name change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditPropertiesWidget::m_RegisterNameChange(void)
{
   //This function can somehow be called twice, so ... let's avoid that!
   static bool hq_InProgress = false;

   if (hq_InProgress == false)
   {
      std::vector<stw::scl::C_SclString> c_ExistingNames;
      hq_InProgress = true;
      if (C_PuiSdHandler::h_GetInstance()->CheckBusNameAvailable(this->mpc_Ui->pc_LineEditBusName->text().toStdString()
                                                                 .
                                                                 c_str(), &this->mu32_BusIndex,
                                                                 &c_ExistingNames) == false)
      {
         const QString c_Description = static_cast<QString>(C_GtGetText::h_GetText(
                                                               "A bus with the name \"%1\" already exists. Choose another name."))
                                       .
                                       arg(this->mpc_Ui->pc_LineEditBusName->text());
         QString c_Details;
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Bus naming"));
         c_Message.SetDescription(c_Description);
         c_Details.append(C_GtGetText::h_GetText("Used bus names:\n"));
         for (uint32_t u32_ItExistingName = 0UL; u32_ItExistingName < c_ExistingNames.size(); ++u32_ItExistingName)
         {
            const C_SclString & rc_Name = c_ExistingNames[u32_ItExistingName];
            c_Details.append(static_cast<QString>("\"%1\"\n").arg(rc_Name.c_str()));
         }
         c_Message.SetDetails(c_Details);
         c_Message.SetCustomMinHeight(180, 300);
         c_Message.Execute();
         //Restore previous name
         {
            const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(this->mu32_BusIndex);
            if (pc_Bus != NULL)
            {
               this->mpc_Ui->pc_LineEditBusName->setText(pc_Bus->c_Name.c_str());
            }
         }
      }
      else
      {
         m_TrimmBusName();
         m_RegisterChange();
         Q_EMIT (this->SigNameChanged(C_GtGetText::h_GetText(
                                         "NETWORK TOPOLOGY"), this->mpc_Ui->pc_LineEditBusName->text(), false));
      }
      hq_InProgress = false; //lint !e838 its static and could be used on strange second call
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle id change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditPropertiesWidget::m_RegisterIdChange(void)
{
   m_RegisterChange();
   m_CheckBusId();
}
