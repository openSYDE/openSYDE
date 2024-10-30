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

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::tgl;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

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
   mu32_DataLoggerJobIndex(0),
   ms32_LastUsedLogFileFormatIndex(0),
   ms32_LastUsedLocalDataIndex(0)
{
   this->mpc_Ui->setupUi(this);
   this->mpc_Ui->pc_LabelIconFileFormat->SetForegroundColor(7);
   this->mpc_Ui->pc_LabelIconFileFormat->SetFontPixel(36, true);
   this->mpc_Ui->pc_LabelIcon->SetSvg("://images/IconLogging.svg");

   // name length restriction
   this->mpc_Ui->pc_LineEditName->setMaxLength(C_PuiSdHandler::h_GetInstance()->GetNameMaxCharLimit());

   this->InitStaticNames();

   connect(this->mpc_Ui->pc_LineEditName, &C_OgeLePropertiesName::editingFinished,
           this, &C_SdNdeDalLogJobPropertiesWidget::m_OnNameEdited);
   connect(this->mpc_Ui->pc_TextEditComment, &C_OgeTedPropertiesComment::SigEditingFinished,
           this, &C_SdNdeDalLogJobPropertiesWidget::m_OnCommentEdited);
   connect(this->mpc_Ui->pc_ComboBoxLogFileFormat,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_SdNdeDalLogJobPropertiesWidget::m_OnLogFileFormatChanged);
   connect(this->mpc_Ui->pc_ComboBoxLocalData,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_SdNdeDalLogJobPropertiesWidget::m_OnLocalDataChanged);
   connect(this->mpc_Ui->pc_SpinBoxMaxFileSize, static_cast<void (QSpinBox::*)(
                                                               int32_t)>(&C_OgeSpxNumber::valueChanged), this,
           &C_SdNdeDalLogJobPropertiesWidget::m_OnFileSizeChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobPropertiesWidget::~C_SdNdeDalLogJobPropertiesWidget()
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
   this->m_GetSupportedLogFileFormats();
   this->m_GetSupportedLocalData();

   this->mpc_Ui->pc_LineEditName->setText(pc_Retval->c_Properties.c_Name.c_str());
   this->mpc_Ui->pc_TextEditComment->setText(pc_Retval->c_Properties.c_Comment.c_str());
   this->mpc_Ui->pc_ComboBoxLogFileFormat->setCurrentIndex(static_cast<int32_t>(pc_Retval->c_Properties.e_LogFileFormat));
   this->mpc_Ui->pc_ComboBoxLocalData->setCurrentIndex(static_cast<int32_t>(pc_Retval->c_Properties.e_LocalLogTrigger));
   this->mpc_Ui->pc_SpinBoxMaxFileSize->setValue(static_cast<int32_t>(pc_Retval->c_Properties.u32_MaxLogFileSizeMb));
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
   this->mpc_Ui->pc_LabelLogTriggerCondition->setText(C_GtGetText::h_GetText("Log Trigger Condition"));
   this->mpc_Ui->pc_LabelLogFileFormat->setText(C_GtGetText::h_GetText("Log File Format"));
   this->mpc_Ui->pc_LabelMaxFileSize->setText(C_GtGetText::h_GetText("Max. File Size"));
   this->mpc_Ui->pc_LabelMode->setText(C_GtGetText::h_GetText("Mode"));

   // Icon section
   switch (this->mpc_Ui->pc_ComboBoxLogFileFormat->currentIndex())
   {
   case 0:
      this->mpc_Ui->pc_LabelIconFileFormat->setText(C_GtGetText::h_GetText("CSV"));
      break;
   case 1:
      this->mpc_Ui->pc_LabelIconFileFormat->setText(C_GtGetText::h_GetText("Parquet"));
      break;
   default:
      break;
   }
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
/*! \brief  Slot for editing finished on name property
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_OnNameEdited()
{
   const C_OscDataLoggerJob * const pc_Retval = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(mu32_NodeIndex,
                                                                                                  mu32_DataLoggerJobIndex);
   C_OscDataLoggerJobProperties rc_Properties = pc_Retval->c_Properties;

   rc_Properties.c_Name = this->mpc_Ui->pc_LineEditName->text().toStdString().c_str();
   C_PuiSdHandler::h_GetInstance()->SetDataLoggerProperties(mu32_NodeIndex, mu32_DataLoggerJobIndex, rc_Properties);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for SigEditingFinished on comment property
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_OnCommentEdited()
{
   const C_OscDataLoggerJob * const pc_Retval = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(mu32_NodeIndex,
                                                                                                  mu32_DataLoggerJobIndex);
   C_OscDataLoggerJobProperties rc_Properties = pc_Retval->c_Properties;

   rc_Properties.c_Comment = this->mpc_Ui->pc_TextEditComment->toPlainText().toStdString().c_str();
   C_PuiSdHandler::h_GetInstance()->SetDataLoggerProperties(mu32_NodeIndex, mu32_DataLoggerJobIndex, rc_Properties);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for valueChanged on file size spin box property
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_OnFileSizeChanged()
{
   const C_OscDataLoggerJob * const pc_Retval = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(mu32_NodeIndex,
                                                                                                  mu32_DataLoggerJobIndex);
   C_OscDataLoggerJobProperties rc_Properties = pc_Retval->c_Properties;

   rc_Properties.u32_MaxLogFileSizeMb = static_cast<uint32_t>(this->mpc_Ui->pc_SpinBoxMaxFileSize->value());
   C_PuiSdHandler::h_GetInstance()->SetDataLoggerProperties(mu32_NodeIndex, mu32_DataLoggerJobIndex, rc_Properties);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for File format combo box index change.

   \param[in]  os32_NewIndex  New combo box index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_OnLogFileFormatChanged(const int32_t os32_NewIndex)
{
   if (this->ms32_LastUsedLogFileFormatIndex != os32_NewIndex)
   {
      const C_OscDataLoggerJob * const pc_Retval = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(mu32_NodeIndex,
                                                                                                     mu32_DataLoggerJobIndex);
      switch (os32_NewIndex)
      {
      case 0:
         this->mpc_Ui->pc_LabelIconFileFormat->setText(C_GtGetText::h_GetText("CSV"));
         break;
      case 1:
         this->mpc_Ui->pc_LabelIconFileFormat->setText(C_GtGetText::h_GetText("Parquet"));
         break;
      default:
         break;
      }
      C_OscDataLoggerJobProperties rc_Properties = pc_Retval->c_Properties;

      rc_Properties.e_LogFileFormat =
         static_cast<C_OscDataLoggerJobProperties::E_LogFileFormat>(this->mpc_Ui->pc_ComboBoxLogFileFormat->currentIndex());
      C_PuiSdHandler::h_GetInstance()->SetDataLoggerProperties(mu32_NodeIndex, mu32_DataLoggerJobIndex, rc_Properties);
      this->ms32_LastUsedLogFileFormatIndex = this->mpc_Ui->pc_ComboBoxLogFileFormat->currentIndex();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for Local data combo box index change.

   \param[in]  os32_NewIndex  New combo box index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_OnLocalDataChanged(const int32_t os32_NewIndex)
{
   if (this->ms32_LastUsedLocalDataIndex != os32_NewIndex)
   {
      const C_OscDataLoggerJob * const pc_Retval = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(mu32_NodeIndex,
                                                                                                     mu32_DataLoggerJobIndex);
      C_OscDataLoggerJobProperties rc_Properties = pc_Retval->c_Properties;

      rc_Properties.e_LocalLogTrigger =
         static_cast<C_OscDataLoggerJobProperties::E_LocalLogTrigger>(this->mpc_Ui->pc_ComboBoxLocalData->currentIndex());
      C_PuiSdHandler::h_GetInstance()->SetDataLoggerProperties(mu32_NodeIndex, mu32_DataLoggerJobIndex, rc_Properties);
      this->ms32_LastUsedLocalDataIndex = this->mpc_Ui->pc_ComboBoxLocalData->currentIndex();
   }
}
