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
   ms32_LastUsedLocalDataIndex(0),
   ms32_LastUsedInterfaceNumberIndex(0)

{
   this->mpc_Ui->setupUi(this);
   this->mpc_Ui->pc_LabelIconFileFormat->SetForegroundColor(7);
   this->mpc_Ui->pc_LabelIconFileFormat->SetFontPixel(36, true);
   this->mpc_Ui->pc_LabelIcon->SetSvg("://images/IconLogging.svg");

   this->mpc_Ui->pc_SpinBoxMaxFileSize->SetMinimumCustom(100);
   this->mpc_Ui->pc_SpinBoxMaxFileSize->SetMaximumCustom(1024);

   // name length restriction
   this->mpc_Ui->pc_LineEditName->setMaxLength(C_PuiSdHandler::h_GetInstance()->GetNameMaxCharLimit());

   //Hiding Max File Size for now
   this->mpc_Ui->pc_LabelMaxFileSize->setVisible(false);
   this->mpc_Ui->pc_SpinBoxMaxFileSize->setVisible(false);
   this->mpc_Ui->horizontalSpacer_9->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
   this->mpc_Ui->horizontalSpacer_8->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);

   //Set mode to disable for now
   this->mpc_Ui->pc_ComboBoxLocalData->setDisabled(true);
   this->mpc_Ui->pc_ComboBoxLocalData->setCurrentText("On Change");

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
   connect(this->mpc_Ui->pc_ComboBoxClientInterface,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_SdNdeDalLogJobPropertiesWidget::m_OnClientInterfaceChanged);
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
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
   const C_OscDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;

   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mu32_DataLoggerJobIndex = ou32_DataLoggerJobIndex;
   this->m_GetSupportedLogFileFormats();
   this->m_GetSupportedLocalData();
   this->m_GetSupportedClientInterfaces();

   if (pc_Retval != NULL)
   {
      //this->mpc_Ui->pc_LineEditName->setText(pc_Retval->c_Properties.c_Name.c_str()); //AC: Disabled for fixed name
      this->mpc_Ui->pc_LineEditName->setText("LogJob1");
      this->mpc_Ui->pc_LineEditName->setDisabled(true);
      this->mpc_Ui->pc_TextEditComment->setText(pc_Retval->c_Properties.c_Comment.c_str());
      this->mpc_Ui->pc_ComboBoxLogFileFormat->setCurrentIndex(static_cast<int32_t>(pc_Retval->c_Properties.
                                                                                   e_LogFileFormat));
      this->mpc_Ui->pc_ComboBoxLocalData->setCurrentIndex(
         static_cast<int32_t>(pc_Retval->c_Properties.e_LocalLogTrigger));
      this->mpc_Ui->pc_SpinBoxMaxFileSize->setValue(static_cast<int32_t>(pc_Retval->c_Properties.u32_MaxLogFileSizeMb));

      if (pc_Retval->c_Properties.e_ConnectedInterfaceType == C_OscSystemBus::E_Type::eETHERNET)
      {
         this->mpc_Ui->pc_ComboBoxClientInterface->setCurrentIndex(static_cast<uint8_t>(pc_Retval->c_Properties.
                                                                                        u8_ConnectedInterfaceNumber +
                                                                                        pc_DevDef->u8_NumCanBusses));
      }
      else
      {
         this->mpc_Ui->pc_ComboBoxClientInterface->setCurrentIndex(static_cast<uint8_t>(pc_Retval->c_Properties.
                                                                                        u8_ConnectedInterfaceNumber));
      }
   }
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
/*! \brief  Add client interfaces (CAN/ETHERNET) into related combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_GetSupportedClientInterfaces() const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
   const C_OscDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;

   std::vector<QString> c_InterfaceNames;
   QString c_InterfaceName;

   for (uint8_t u8_Iterator = 0; u8_Iterator < pc_DevDef->u8_NumCanBusses; ++u8_Iterator)
   {
      c_InterfaceName = C_PuiSdUtil::h_GetInterfaceName(C_OscSystemBus::eCAN, u8_Iterator);
      c_InterfaceNames.push_back(c_InterfaceName);
   }

   for (uint8_t u8_Iterator = 0; u8_Iterator < pc_DevDef->u8_NumEthernetBusses; ++u8_Iterator)
   {
      c_InterfaceName = C_PuiSdUtil::h_GetInterfaceName(C_OscSystemBus::eETHERNET, u8_Iterator);
      c_InterfaceNames.push_back(c_InterfaceName);
   }

   for (uint32_t u32_InterfaceNamesCounter = 0U;
        u32_InterfaceNamesCounter < static_cast<uint32_t>(c_InterfaceNames.size());
        ++u32_InterfaceNamesCounter)
   {
      this->mpc_Ui->pc_ComboBoxClientInterface->addItem(c_InterfaceNames[u32_InterfaceNamesCounter]);
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

   if (pc_Retval != NULL)
   {
      C_OscDataLoggerJobProperties rc_Properties = pc_Retval->c_Properties;

      rc_Properties.c_Name = this->mpc_Ui->pc_LineEditName->text().toStdString().c_str();
      C_PuiSdHandler::h_GetInstance()->SetDataLoggerPropertiesWithoutInterfaceChanges(mu32_NodeIndex,
                                                                                      mu32_DataLoggerJobIndex,
                                                                                      rc_Properties);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for SigEditingFinished on comment property
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_OnCommentEdited()
{
   const C_OscDataLoggerJob * const pc_Retval = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(mu32_NodeIndex,
                                                                                                  mu32_DataLoggerJobIndex);

   if (pc_Retval != NULL)
   {
      C_OscDataLoggerJobProperties rc_Properties = pc_Retval->c_Properties;

      rc_Properties.c_Comment = this->mpc_Ui->pc_TextEditComment->toPlainText().toStdString().c_str();
      C_PuiSdHandler::h_GetInstance()->SetDataLoggerPropertiesWithoutInterfaceChanges(mu32_NodeIndex,
                                                                                      mu32_DataLoggerJobIndex,
                                                                                      rc_Properties);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for valueChanged on file size spin box property
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_OnFileSizeChanged()
{
   const C_OscDataLoggerJob * const pc_Retval = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(mu32_NodeIndex,
                                                                                                  mu32_DataLoggerJobIndex);

   if (pc_Retval != NULL)
   {
      C_OscDataLoggerJobProperties rc_Properties = pc_Retval->c_Properties;

      rc_Properties.u32_MaxLogFileSizeMb = static_cast<uint32_t>(this->mpc_Ui->pc_SpinBoxMaxFileSize->value());
      C_PuiSdHandler::h_GetInstance()->SetDataLoggerPropertiesWithoutInterfaceChanges(mu32_NodeIndex,
                                                                                      mu32_DataLoggerJobIndex,
                                                                                      rc_Properties);
   }
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
      if (pc_Retval != NULL)
      {
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
            static_cast<C_OscDataLoggerJobProperties::E_LogFileFormat>(this->mpc_Ui->pc_ComboBoxLogFileFormat->
                                                                       currentIndex());
         C_PuiSdHandler::h_GetInstance()->SetDataLoggerPropertiesWithoutInterfaceChanges(mu32_NodeIndex,
                                                                                         mu32_DataLoggerJobIndex,
                                                                                         rc_Properties);
         this->ms32_LastUsedLogFileFormatIndex = this->mpc_Ui->pc_ComboBoxLogFileFormat->currentIndex();
      }
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
      if (pc_Retval != NULL)
      {
         C_OscDataLoggerJobProperties rc_Properties = pc_Retval->c_Properties;

         rc_Properties.e_LocalLogTrigger =
            static_cast<C_OscDataLoggerJobProperties::E_LocalLogTrigger>(this->mpc_Ui->pc_ComboBoxLocalData->
                                                                         currentIndex());
         C_PuiSdHandler::h_GetInstance()->SetDataLoggerPropertiesWithoutInterfaceChanges(mu32_NodeIndex,
                                                                                         mu32_DataLoggerJobIndex,
                                                                                         rc_Properties);
         this->ms32_LastUsedLocalDataIndex = this->mpc_Ui->pc_ComboBoxLocalData->currentIndex();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for client interface combo box index change.

   \param[in]  os32_NewIndex  New combo box index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobPropertiesWidget::m_OnClientInterfaceChanged(const int32_t os32_NewIndex)
{
   if (this->ms32_LastUsedInterfaceNumberIndex != os32_NewIndex)
   {
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
      const C_OscDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;
      uint8_t u8_ConnectedInterfaceNumber;
      C_OscSystemBus::E_Type e_ConnectedInterfaceType;

      if (os32_NewIndex < pc_DevDef->u8_NumCanBusses)
      {
         e_ConnectedInterfaceType = C_OscSystemBus::E_Type::eCAN;
         u8_ConnectedInterfaceNumber = static_cast<uint8_t>(os32_NewIndex);
      }
      else
      {
         e_ConnectedInterfaceType = C_OscSystemBus::E_Type::eETHERNET;
         u8_ConnectedInterfaceNumber = static_cast<uint8_t>(os32_NewIndex) - pc_DevDef->u8_NumCanBusses;
      }
      C_PuiSdHandler::h_GetInstance()->SetDataLoggerInterface(mu32_NodeIndex, mu32_DataLoggerJobIndex,
                                                              e_ConnectedInterfaceType, u8_ConnectedInterfaceNumber);
      this->ms32_LastUsedInterfaceNumberIndex = this->mpc_Ui->pc_ComboBoxClientInterface->currentIndex();
      Q_EMIT this->SigReloadDataLoggerDataElements();
   }
}
