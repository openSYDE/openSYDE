//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for log job properties configuration

   Widget for log job properties configuration

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SdNdeDalLogJobPropertiesWidget.hpp"
#include "ui_C_SdNdeDalLogJobPropertiesWidget.h"

#include "C_PuiSdHandler.hpp"
#include "stwerrors.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OscUtils.hpp"
#include "C_OgeWiCustomMessage.hpp"

#include <cmath>

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::tgl;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::scl;

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
C_SdNdeDalLogJobPropertiesWidget::C_SdNdeDalLogJobPropertiesWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDalLogJobPropertiesWidget),
   mu32_NodeIndex(0),
   mu32_DataLoggerJobIndex(0)

{
   this->mpc_Ui->setupUi(this);
   this->mpc_Ui->pc_LabelIconFileFormat->SetForegroundColor(7);
   this->mpc_Ui->pc_LabelIconFileFormat->SetFontPixel(36, true);
   this->mpc_Ui->pc_LabelIcon->SetSvg("://images/IconLogging.svg");

   // name length restriction
   this->mpc_Ui->pc_LineEditName->setMaxLength(C_PuiSdHandler::h_GetInstance()->GetNameMaxCharLimit());

   this->m_GetSupportedLogFileFormats();
   this->m_GetSupportedLocalData();
   this->m_GetSupportedLogJobUseCase();

   this->InitStaticNames();

   // set LoggingInterval min/max
   this->mpc_Ui->pc_SpinBoxLoggingInterval->SetMinimumCustom(0);
   this->mpc_Ui->pc_SpinBoxLoggingInterval->SetMaximumCustom(0x7FFFFFFF);

   // set LogDuration min/max
   this->mpc_Ui->pc_SpinBoxLogDuration->SetMinimumCustom(0);
   this->mpc_Ui->pc_SpinBoxLogDuration->SetMaximumCustom(0x7FFFFFFF);

   // set MaxLogEntries min/max
   this->mpc_Ui->pc_SpxBoxMaxLogEntries->SetMinimumCustom(0);
   this->mpc_Ui->pc_SpxBoxMaxLogEntries->SetMaximumCustom(0x7FFFFFFF);

   connect(this->mpc_Ui->pc_LineEditName, &C_OgeLePropertiesName::editingFinished,
           this, &C_SdNdeDalLogJobPropertiesWidget::m_TrimLogJobName);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobPropertiesWidget::~C_SdNdeDalLogJobPropertiesWidget() noexcept
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new node data logger indices

   \param[in]  ou32_NodeIndex             Index of node
   \param[in]  ou32_DataLoggerJobIndex    Data logger job index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::SetNodeDataLoggerJob(const uint32_t ou32_NodeIndex,
                                                            const uint32_t ou32_DataLoggerJobIndex)
{
   const C_OscDataLoggerJob * const pc_Retval = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(ou32_NodeIndex,
                                                                                                  ou32_DataLoggerJobIndex);

   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mu32_DataLoggerJobIndex = ou32_DataLoggerJobIndex;
   this->m_DisconnectChangeTriggers();
   if (pc_Retval != NULL)
   {
      this->mpc_Ui->pc_LineEditName->setText(pc_Retval->c_Properties.c_Name.c_str());
      this->mpc_Ui->pc_TextEditComment->setText(pc_Retval->c_Properties.c_Comment.c_str());
      this->mpc_Ui->pc_ComboBoxLogJobUseCase->setCurrentIndex(static_cast<int32_t>(pc_Retval->c_Properties.e_UseCase));
      this->mpc_Ui->pc_ComboBoxLogFileFormat->setCurrentIndex(static_cast<int32_t>(pc_Retval->c_Properties.
                                                                                   e_LogFileFormat));
      this->mpc_Ui->pc_LineEditLogDestination->setText(static_cast<QString>(pc_Retval->c_Properties.
                                                                            c_LogDestinationDirectory.c_str()));
      this->mpc_Ui->pc_ComboBoxLocalData->setCurrentIndex(
         static_cast<int32_t>(pc_Retval->c_Properties.e_LocalLogTrigger));
      this->mpc_Ui->pc_SpinBoxLoggingInterval->setValue(pc_Retval->c_Properties.u32_LogIntervalMs);
      this->mpc_Ui->pc_SpinBoxLogDuration->setValue(pc_Retval->c_Properties.u32_MaxLogDurationSec);
      this->mpc_Ui->pc_SpxBoxMaxLogEntries->setValue(pc_Retval->c_Properties.u32_MaxLogEntries);
      this->m_OnLogJobUseCaseChanged(this->mpc_Ui->pc_ComboBoxLogJobUseCase->currentIndex());
      this->m_OnLocalDataChanged(this->mpc_Ui->pc_ComboBoxLocalData->currentIndex());
   }
   this->m_ReconnectChangeTriggers();
   this->mpc_Ui->pc_WidgetAdditionalTriggerProperties->SetNodeDataLoggerJob(ou32_NodeIndex, ou32_DataLoggerJobIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::Save() const
{
   C_PuiSdHandler::h_GetInstance()->SetDataLoggerProperties(mu32_NodeIndex,
                                                            mu32_DataLoggerJobIndex,
                                                            this->mc_Properties.c_Name,
                                                            this->mc_Properties.c_Comment,
                                                            this->mc_Properties.e_UseCase,
                                                            this->mc_Properties.e_LogFileFormat,
                                                            this->mc_Properties.u32_MaxLogEntries,
                                                            this->mc_Properties.u32_MaxLogDurationSec,
                                                            this->mc_Properties.u32_LogIntervalMs,
                                                            this->mc_Properties.e_LocalLogTrigger,
                                                            this->mc_Properties.c_LogDestinationDirectory);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} TODO
void C_SdNdeDalLogJobPropertiesWidget::SaveUserSettings() const
{
   //TODO
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::InitStaticNames() const
{
   // properties section
   this->mpc_Ui->pc_LabelName->setText(C_GtGetText::h_GetText("Name"));
   this->mpc_Ui->pc_LabelComment->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_TextEditComment->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here..."));
   this->mpc_Ui->pc_LabelGeneral->setText(C_GtGetText::h_GetText("General"));
   this->mpc_Ui->pc_LabelLogFileFormat->setText(C_GtGetText::h_GetText("Log File Format"));
   this->mpc_Ui->pc_LabelMode->setText(C_GtGetText::h_GetText("Mode"));
   this->mpc_Ui->pc_LabelGeneral->setText(C_GtGetText::h_GetText("General"));
   this->mpc_Ui->pc_LabelLogJobUseCase->setText(C_GtGetText::h_GetText("Log Job Use-Case"));
   this->mpc_Ui->pc_LabelLogFileFormat->setText(C_GtGetText::h_GetText("Log File Format"));
   this->mpc_Ui->pc_LabelLogDestination->setText(C_GtGetText::h_GetText("Log Destination"));
   this->mpc_Ui->pc_LineEditLogDestination->setPlaceholderText(C_GtGetText::h_GetText("./ExampleSubFolder"));
   this->mpc_Ui->pc_LabelMode->setText(C_GtGetText::h_GetText("Mode"));
   this->mpc_Ui->pc_LabelLoggingInterval->setText(C_GtGetText::h_GetText("Logging Interval"));
   this->mpc_Ui->pc_LabelLogDuration->setText(C_GtGetText::h_GetText("Log Duration"));
   this->mpc_Ui->pc_LabelMaxLogEntries->setText(C_GtGetText::h_GetText("Max Log Entries"));

   //tooltips
   this->mpc_Ui->pc_LabelName->SetToolTipInformation(C_GtGetText::h_GetText(
                                                        "Name"),
                                                     static_cast<QString>(C_GtGetText::h_GetText(
                                                                             "Symbolic log job name. Unique within Data Logger.\n"
                                                                             "\nFollowing naming conventions are required:"
                                                                             "\n - must not be empty"
                                                                             "\n - must not start with digits"
                                                                             "\n - only alphanumeric characters and \"_\""
                                                                             "\n - should not be longer than %1 (= project setting) characters")).arg(
                                                        C_PuiSdHandler::h_GetInstance()->GetNameMaxCharLimit()));
   this->mpc_Ui->pc_LabelComment->SetToolTipInformation(C_GtGetText::h_GetText("Comment"),
                                                        C_GtGetText::h_GetText("Comment for this log job."));

   this->mpc_Ui->pc_LabelLogJobUseCase->SetToolTipInformation(C_GtGetText::h_GetText("Log Job Use-Case"),
                                                              C_GtGetText::h_GetText(
                                                                 "Select the use-case of the log job:\n"
                                                                 "- Manual: Select custom log destination and file format.\n"
                                                                 "- AWS: Logging for AWS cloud\n"
                                                                 "- maschines.cloud: Logging for maschines.cloud"));
   this->mpc_Ui->pc_LabelLogFileFormat->SetToolTipInformation(C_GtGetText::h_GetText("Log File Format"),
                                                              C_GtGetText::h_GetText(
                                                                 "Log File Format, relevant when log job use-case 'Manual' is selected."));
   this->mpc_Ui->pc_LabelLogDestination->SetToolTipInformation(C_GtGetText::h_GetText("Log Destination"),
                                                               C_GtGetText::h_GetText(
                                                                  "Log Destination, relevant when log job use-case 'Manual' is selected.\n"
                                                                  "Sub-folder name where the log files are stored."));

   this->mpc_Ui->pc_LabelMode->SetToolTipInformation(C_GtGetText::h_GetText("Mode"),
                                                     C_GtGetText::h_GetText(
                                                        "Log Mode:\n"
                                                        "- On Change: On every value change, the data element is logged to file. \n"
                                                        "- Interval: Every logging interval all logging data of a log job are logged to file."));
   this->mpc_Ui->pc_LabelLoggingInterval->SetToolTipInformation(C_GtGetText::h_GetText("Logging Interval"),
                                                                C_GtGetText::h_GetText(
                                                                   "Relevant when log mode 'Interval' is selected.\n"
                                                                   "Every logging interval all logging data of a log job are logged to file."));
   this->mpc_Ui->pc_LabelLogDuration->SetToolTipInformation(C_GtGetText::h_GetText("Log Duration"),
                                                            C_GtGetText::h_GetText(
                                                               "Relevant when log mode 'Interval' is selected.\n"
                                                               "Max log duration of a file before starting a new one. \n\n"
                                                               "Value shall be greater than the logging interval."));
   this->mpc_Ui->pc_LabelMaxLogEntries->SetToolTipInformation(C_GtGetText::h_GetText("Max Log Entries"),
                                                              C_GtGetText::h_GetText(
                                                                 "Relevant when log mode 'Interval' is selected.\n"
                                                                 "Max log entries in a file before starting a new one."));

   // Icon section
   this->mpc_Ui->pc_LabelIconFileFormat->setText(
      mh_ConvertTxMethodToName(static_cast<C_OscDataLoggerJobProperties::E_UseCase>(
                                  this->mpc_Ui->pc_ComboBoxLogJobUseCase->currentIndex())));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reload additional trigger
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::ReloadAdditionalTrigger()
{
   this->mpc_Ui->pc_WidgetAdditionalTriggerProperties->Reload();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add Log file formats into related combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_GetSupportedLogFileFormats() const
{
   for (uint32_t u32_LogFileFormatCounter = 0U;
        u32_LogFileFormatCounter < static_cast<int32_t>(E_LogFileFormat::eNUM_OF_FORMATS);
        ++u32_LogFileFormatCounter)
   {
      switch (u32_LogFileFormatCounter)
      {
      case 0:
         this->mpc_Ui->pc_ComboBoxLogFileFormat->addItem("CSV");
         break;
      case 1:
         this->mpc_Ui->pc_ComboBoxLogFileFormat->addItem("Parquet");
         break;
      default:
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add Local data into related combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_GetSupportedLocalData() const
{
   for (uint32_t u32_ModeCounter = 0U;
        u32_ModeCounter < static_cast<int32_t>(E_LogTriggerMode::eNUM_OF_LOG_TRIGGERS);
        ++u32_ModeCounter)
   {
      switch (u32_ModeCounter)
      {
      case 0:
         this->mpc_Ui->pc_ComboBoxLocalData->addItem("On Change");
         break;
      case 1:
         this->mpc_Ui->pc_ComboBoxLocalData->addItem("Interval");
         break;
      default:
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add supported log job use cases into related combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_GetSupportedLogJobUseCase() const
{
   // Must be same order as defined in core
   this->mpc_Ui->pc_ComboBoxLogJobUseCase->addItem(mh_ConvertTxMethodToName(C_OscDataLoggerJobProperties::eUC_MANUAL));
   this->mpc_Ui->pc_ComboBoxLogJobUseCase->addItem(mh_ConvertTxMethodToName(C_OscDataLoggerJobProperties::eUC_AWS));
   this->mpc_Ui->pc_ComboBoxLogJobUseCase->addItem(mh_ConvertTxMethodToName(C_OscDataLoggerJobProperties::
                                                                            eUC_MACHINES_CLOUD));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert tx method to name

   \param[in]  ore_Type    Tx method

   \return
   Name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDalLogJobPropertiesWidget::mh_ConvertTxMethodToName(
   const C_OscDataLoggerJobProperties::E_UseCase & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case C_OscDataLoggerJobProperties::eUC_MANUAL:
      c_Retval = C_GtGetText::h_GetText("Manual");
      break;
   case C_OscDataLoggerJobProperties::eUC_AWS:
      c_Retval = C_GtGetText::h_GetText("AWS");
      break;
   case C_OscDataLoggerJobProperties::eUC_MACHINES_CLOUD:
      c_Retval = C_GtGetText::h_GetText("machines.cloud");
      break;
   default:
      c_Retval = "";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for log job use case combo box index change.

   \param[in]  os32_NewIndex  New combo box index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_OnLogJobUseCaseChanged(const int32_t os32_NewIndex)
{
   const C_OscDataLoggerJobProperties::E_UseCase e_UseCase =
      static_cast<C_OscDataLoggerJobProperties::E_UseCase>(os32_NewIndex);

   switch (e_UseCase)
   {
   case C_OscDataLoggerJobProperties::eUC_AWS:
      this->mpc_Ui->pc_LabelIconFileFormat->setText(C_GtGetText::h_GetText("AWS"));

      this->mpc_Ui->pc_GbxFileFormat->setVisible(false);
      this->mpc_Ui->pc_LabelLogDestination->setVisible(false);
      this->mpc_Ui->pc_LineEditLogDestination->setVisible(false);
      break;

   case C_OscDataLoggerJobProperties::eUC_MACHINES_CLOUD:
      this->mpc_Ui->pc_LabelIconFileFormat->setText(C_GtGetText::h_GetText("machines.cloud"));

      this->mpc_Ui->pc_GbxFileFormat->setVisible(false);
      this->mpc_Ui->pc_LabelLogDestination->setVisible(false);
      this->mpc_Ui->pc_LineEditLogDestination->setVisible(false);
      break;

   case C_OscDataLoggerJobProperties::eUC_MANUAL:
      this->mpc_Ui->pc_LabelIconFileFormat->setText(C_GtGetText::h_GetText("Manual"));

      this->mpc_Ui->pc_GbxFileFormat->setVisible(true);
      this->mpc_Ui->pc_LabelLogDestination->setVisible(true);
      this->mpc_Ui->pc_LineEditLogDestination->setVisible(true);
      break;

   default:
      break;
   }
   this->m_OnXappSettingsChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for Local data combo box index change.

   \param[in]  os32_NewIndex  New combo box index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_OnLocalDataChanged(const int32_t os32_NewIndex)
{
   switch (os32_NewIndex)
   {
   case 0:
      this->mpc_Ui->pc_GbxLoggingInterval->setVisible(false);
      this->mpc_Ui->pc_GbxLogDuration->setVisible(false);
      this->mpc_Ui->pc_GroupBoxMaxLogEntries->setVisible(true);
      this->mpc_Ui->pc_WidgetAdditionalTriggerProperties->setVisible(false);
      break;
   case 1:
      this->mpc_Ui->pc_GbxLoggingInterval->setVisible(true);
      this->mpc_Ui->pc_GbxLogDuration->setVisible(true);
      this->mpc_Ui->pc_GroupBoxMaxLogEntries->setVisible(false);
      this->mpc_Ui->pc_WidgetAdditionalTriggerProperties->setVisible(true);
      this->mpc_Ui->pc_WidgetAdditionalTriggerProperties->Reload();
      break;
   default:
      break;
   }

   this->m_OnXappSettingsChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for X-App settings data value changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_OnXappSettingsChanged()
{
   const C_OscDataLoggerJob * const pc_Retval = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(mu32_NodeIndex,
                                                                                                  mu32_DataLoggerJobIndex);

   if (pc_Retval != NULL)
   {
      this->mc_Properties = pc_Retval->c_Properties;

      this->mc_Properties.c_Name = this->mpc_Ui->pc_LineEditName->text().toStdString().c_str();
      this->mc_Properties.c_Comment = this->mpc_Ui->pc_TextEditComment->toPlainText().toStdString().c_str();
      this->mc_Properties.e_UseCase =
         static_cast<C_OscDataLoggerJobProperties::E_UseCase>(this->mpc_Ui->pc_ComboBoxLogJobUseCase->currentIndex());
      this->mc_Properties.e_LogFileFormat =
         static_cast<C_OscDataLoggerJobProperties::E_LogFileFormat>(this->mpc_Ui->pc_ComboBoxLogFileFormat->
                                                                    currentIndex());
      this->mc_Properties.c_LogDestinationDirectory =
         this->mpc_Ui->pc_LineEditLogDestination->text().toStdString().c_str();
      this->mc_Properties.e_LocalLogTrigger =
         static_cast<C_OscDataLoggerJobProperties::E_LocalLogTrigger>(this->mpc_Ui->pc_ComboBoxLocalData->
                                                                      currentIndex());
      const float64_t f64_DurationSec =
         (static_cast<float64_t>(this->mpc_Ui->pc_SpinBoxLoggingInterval->value()) + 999.0) / 1000.0;
      this->mpc_Ui->pc_SpinBoxLogDuration->SetMinimumCustom(static_cast<int32_t>(f64_DurationSec));

      this->mc_Properties.u32_LogIntervalMs = this->mpc_Ui->pc_SpinBoxLoggingInterval->value();
      this->mc_Properties.u32_MaxLogDurationSec = this->mpc_Ui->pc_SpinBoxLogDuration->value();
      this->Save();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check data logger name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_CheckDataLoggerName()
{
   //check
   const C_SclString c_LogJobName = this->mpc_Ui->pc_LineEditName->text().toStdString().c_str();
   const bool q_IsLogJobNameUnique = C_PuiSdUtil::h_CheckNodeDataLoggerNameAvailable(this->mu32_NodeIndex, c_LogJobName,
                                                                                     &this->mu32_DataLoggerJobIndex,
                                                                                     NULL);
   const bool q_IsLoggerNameValid = C_OscUtils::h_CheckValidCeName(c_LogJobName);

   //set invalid text property
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditName, "Valid",
                                          q_IsLogJobNameUnique && q_IsLoggerNameValid);

   if ((q_IsLogJobNameUnique == true) && (q_IsLoggerNameValid == true))
   {
      this->mpc_Ui->pc_LineEditName->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                           C_GtGetText::h_GetText(""),
                                                           C_NagToolTip::eDEFAULT);
   }
   else
   {
      const QString c_Heading = C_GtGetText::h_GetText("Data Logger Job Name");
      QString c_Content;
      if (q_IsLogJobNameUnique == false)
      {
         c_Content += C_GtGetText::h_GetText("- is already in use\n");
      }
      if (q_IsLoggerNameValid == false)
      {
         c_Content += C_GtGetText::h_GetText("- is empty or contains invalid characters.\n");
      }
      this->mpc_Ui->pc_LineEditName->SetToolTipInformation(c_Heading, c_Content, C_NagToolTip::eERROR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Disconnect change triggers
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_DisconnectChangeTriggers() const
{
   disconnect(this->mpc_Ui->pc_LineEditName, &C_OgeLePropertiesName::textChanged,
              this, &C_SdNdeDalLogJobPropertiesWidget::m_CheckDataLoggerName);
   disconnect(this->mpc_Ui->pc_LineEditName, &C_OgeLePropertiesName::editingFinished,
              this, &C_SdNdeDalLogJobPropertiesWidget::m_OnNameEditingFinished);
   disconnect(this->mpc_Ui->pc_TextEditComment, &C_OgeTedPropertiesComment::SigEditingFinished,
              this, &C_SdNdeDalLogJobPropertiesWidget::m_OnXappSettingsChanged);

   disconnect(this->mpc_Ui->pc_ComboBoxLogJobUseCase,
              static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
              this, &C_SdNdeDalLogJobPropertiesWidget::m_OnLogJobUseCaseChanged);
   disconnect(this->mpc_Ui->pc_ComboBoxLogFileFormat,
              static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
              this, &C_SdNdeDalLogJobPropertiesWidget::m_OnXappSettingsChanged);
   disconnect(this->mpc_Ui->pc_LineEditLogDestination, &QLineEdit::editingFinished, this,
              &C_SdNdeDalLogJobPropertiesWidget::m_OnXappSettingsChanged);

   disconnect(this->mpc_Ui->pc_ComboBoxLocalData,
              static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
              this, &C_SdNdeDalLogJobPropertiesWidget::m_OnLocalDataChanged);
   disconnect(this->mpc_Ui->pc_SpinBoxLoggingInterval,
              static_cast<void (QSpinBox::*)(int32_t)>(&QSpinBox::valueChanged),
              this, &C_SdNdeDalLogJobPropertiesWidget::m_OnXappSettingsChanged);
   disconnect(this->mpc_Ui->pc_SpinBoxLogDuration,
              static_cast<void (QSpinBox::*)(int32_t)>(&QSpinBox::valueChanged),
              this, &C_SdNdeDalLogJobPropertiesWidget::m_OnXappSettingsChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reconnect change triggers
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_ReconnectChangeTriggers() const
{
   connect(this->mpc_Ui->pc_LineEditName, &C_OgeLePropertiesName::textChanged,
           this, &C_SdNdeDalLogJobPropertiesWidget::m_CheckDataLoggerName);
   connect(this->mpc_Ui->pc_LineEditName, &C_OgeLePropertiesName::editingFinished,
           this, &C_SdNdeDalLogJobPropertiesWidget::m_OnNameEditingFinished);
   connect(this->mpc_Ui->pc_TextEditComment, &C_OgeTedPropertiesComment::SigEditingFinished,
           this, &C_SdNdeDalLogJobPropertiesWidget::m_OnXappSettingsChanged);

   connect(this->mpc_Ui->pc_ComboBoxLogJobUseCase,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_SdNdeDalLogJobPropertiesWidget::m_OnLogJobUseCaseChanged);
   connect(this->mpc_Ui->pc_ComboBoxLogFileFormat,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_SdNdeDalLogJobPropertiesWidget::m_OnXappSettingsChanged);
   connect(this->mpc_Ui->pc_LineEditLogDestination, &QLineEdit::editingFinished, this,
           &C_SdNdeDalLogJobPropertiesWidget::m_OnXappSettingsChanged);

   connect(this->mpc_Ui->pc_ComboBoxLocalData,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_SdNdeDalLogJobPropertiesWidget::m_OnLocalDataChanged);
   connect(this->mpc_Ui->pc_SpinBoxLoggingInterval,
           static_cast<void (QSpinBox::*)(int32_t)>(&QSpinBox::valueChanged),
           this, &C_SdNdeDalLogJobPropertiesWidget::m_OnXappSettingsChanged);
   connect(this->mpc_Ui->pc_SpinBoxLogDuration,
           static_cast<void (QSpinBox::*)(int32_t)>(&QSpinBox::valueChanged),
           this, &C_SdNdeDalLogJobPropertiesWidget::m_OnXappSettingsChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On name editing finished an error Message Box shown on existing same log job name
   Trim name on any white spaces
   Save changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_OnNameEditingFinished()
{
   //This function can somehow be called twice, so ... let's avoid that!
   static bool hq_InProgress = false;

   if (hq_InProgress == false)
   {
      std::vector<stw::scl::C_SclString> c_ExistingLogJobNames;
      const C_SclString c_LogJobName = this->mpc_Ui->pc_LineEditName->text().toStdString().c_str();
      hq_InProgress = true;
      if (C_PuiSdUtil::h_CheckNodeDataLoggerNameAvailable(this->mu32_NodeIndex, c_LogJobName,
                                                          &this->mu32_DataLoggerJobIndex,
                                                          &c_ExistingLogJobNames) == false)
      {
         const QString c_Description =
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "A Log Job with the name \"%1\" already exists. Choose another name.")).
            arg(c_LogJobName.c_str());
         QString c_Details;
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Log Job naming"));
         c_Message.SetDescription(c_Description);
         c_Details.append(C_GtGetText::h_GetText("Used Log Job names:\n"));
         for (uint32_t u32_ItExistingName = 0UL; u32_ItExistingName < c_ExistingLogJobNames.size();
              ++u32_ItExistingName)
         {
            const C_SclString & rc_Name = c_ExistingLogJobNames[u32_ItExistingName];
            c_Details.append(static_cast<QString>("\"%1\"\n").arg(rc_Name.c_str()));
         }
         c_Message.SetDetails(c_Details);
         c_Message.SetCustomMinHeight(220, 350);
         c_Message.Execute();
         //Restore previous name
         {
            const C_OscDataLoggerJob * const pc_Retval = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(
               this->mu32_NodeIndex,
               this->mu32_DataLoggerJobIndex);
            if (pc_Retval != NULL)
            {
               this->mpc_Ui->pc_LineEditName->setText(pc_Retval->c_Properties.c_Name.c_str());
            }
         }
      }
      else if (C_OscUtils::h_CheckValidCeName(c_LogJobName) == false)
      {
         const QString c_Description =
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "Data Logger Job name \"%1\" is empty or contains invalid characters.")).
            arg(c_LogJobName.c_str());
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Log Job naming"));
         c_Message.SetDescription(c_Description);
         c_Message.SetCustomMinHeight(220, 350);
         c_Message.Execute();
         //Restore previous name
         {
            const C_OscDataLoggerJob * const pc_Retval = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(
               this->mu32_NodeIndex,
               this->mu32_DataLoggerJobIndex);
            if (pc_Retval != NULL)
            {
               this->mpc_Ui->pc_LineEditName->setText(pc_Retval->c_Properties.c_Name.c_str());
            }
         }
      }
      else
      {
         this->m_OnXappSettingsChanged();
         Q_EMIT this->SigLogJobNameModified();
      }
      hq_InProgress = false; //lint !e838 its static and could be used on strange second call
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trim Log Job name

   Remove whitespaces at the beginning and end of the string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_TrimLogJobName() const
{
   this->mpc_Ui->pc_LineEditName->setText(this->mpc_Ui->pc_LineEditName->text().trimmed());
}
