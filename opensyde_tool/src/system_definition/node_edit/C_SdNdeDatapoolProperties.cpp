//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Properties dialog for datapool properties

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdNdeDatapoolProperties.h"
#include "ui_C_SdNdeDatapoolProperties.h"
#include "C_GtGetText.h"
#include "C_OSCUtils.h"
#include "C_PuiSdHandler.h"
#include "C_OgeWiUtil.h"
#include "C_OSCNode.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCDeviceDefinition.h"
#include "C_PuiSdUtil.h"
#include "TGLUtils.h"
#include "constants.h"
#include "C_OgeWiCustomMessage.h"
#include "C_OSCNodeDataPoolId.h"
#include "C_PuiSdSharedDatapools.h"
#include "C_SdNdeDataPoolUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_types;
using namespace stw_scl;
using namespace stw_tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent                Reference to parent
   \param[in]     orc_Name                  Name of the bus for the title
   \param[in,out] opc_OSCDataPool           Pointer to the actual core datapool object
   \param[in,out] pc_UiDataPool             Pointer to the actual UI datapool object
   \param[in]     oe_DatapoolType           Data pool type
   \param[in]     os32_DataPoolIndex        Flag for new datapool (-1 is new datapool, >= 0 is existing datapool)
   \param[in]     oru32_NodeIndex           Node index
   \param[in]     oq_SelectName             Selects the datapool name for instant editing
   \param[in]     oq_ShowApplicationSection Flag to show or hide application section
   \param[in]     opc_SharedDatapoolId      In case of a new shared Datapool, the Id is the shared Datapool of the new
                                            Datapool. In case of an edited or stand alone Datapool the pointer is NULL
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDatapoolProperties::C_SdNdeDatapoolProperties(C_OgePopUpDialog & orc_Parent, const QString & orc_Name,
                                                     C_OSCNodeDataPool * const opc_OSCDataPool,
                                                     C_PuiSdNodeDataPool * const pc_UiDataPool,
                                                     C_OSCCanProtocol::E_Type * const ope_ComProtocolType,
                                                     const C_OSCNodeDataPool::E_Type oe_DatapoolType,
                                                     const sint32 os32_DataPoolIndex, const uint32 & oru32_NodeIndex,
                                                     const bool oq_SelectName, const bool oq_ShowApplicationSection,
                                                     const C_OSCNodeDataPoolId * const opc_SharedDatapoolId) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDatapoolProperties()),
   mpc_ParentDialog(&orc_Parent),
   me_DatapoolType(oe_DatapoolType),
   mpc_OSCDataPool(opc_OSCDataPool),
   mpc_UiDataPool(pc_UiDataPool),
   mpe_ComProtocolType(ope_ComProtocolType),
   mu32_NodeIndex(oru32_NodeIndex),
   ms32_DataPoolIndex(os32_DataPoolIndex)
{
   bool q_IsShared;

   std::vector<QString> c_DatapoolGroup;

   // init UI
   mpc_Ui->setupUi(this);

   //Ui restriction
   this->mpc_Ui->pc_SpinBoxSize->SetMaximumCustom(32768);

   //Ui restriction
   this->mpc_Ui->pc_LineEditDatapoolName->setMaxLength(msn_C_ITEM_MAX_CHAR_COUNT);

   // register the widget for showing
   this->mpc_ParentDialog->SetWidget(this);

   // set main title
   this->mpc_ParentDialog->SetTitle(orc_Name);

   //BEFORE load
   InitStaticNames();

   if (this->mpc_OSCDataPool != NULL)
   {
      const sintn sn_Major = static_cast<sintn>(this->mpc_OSCDataPool->au8_Version[0]);
      const sintn sn_Minor = static_cast<sintn>(this->mpc_OSCDataPool->au8_Version[1]);
      const sintn sn_Revision = static_cast<sintn>(this->mpc_OSCDataPool->au8_Version[2]);
      const QString c_Version =
         QString("v%1.%2r%3").arg(sn_Major, 2, 10, QChar('0')).arg(sn_Minor, 2, 10, QChar('0')).arg(sn_Revision, 2, 10, QChar(
                                                                                                       '0'));
      // load actual datapool values
      this->mpc_Ui->pc_LineEditDatapoolName->setText(this->mpc_OSCDataPool->c_Name.c_str());
      this->mpc_Ui->pc_CommentText->setText(this->mpc_OSCDataPool->c_Comment.c_str());
      this->mpc_Ui->pc_CheckBoxSafety->setChecked(this->mpc_OSCDataPool->q_IsSafety);

      //Warning: this section has to be last, due to data update triggers
      //Application
      m_LoadCodeGenerationAndApplication();

      this->m_InitSpinBox();

      if (this->me_DatapoolType == C_OSCNodeDataPool::eCOM)
      {
         this->mpc_Ui->pc_LineEditDatapoolName->setEnabled(false);
         if ((this->ms32_DataPoolIndex >= 0) && (this->mpe_ComProtocolType != NULL))
         {
            // get the actual protocol type of the datapool
            const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
            if (pc_Node != NULL)
            {
               uint32 u32_Counter;

               for (u32_Counter = 0U; u32_Counter < pc_Node->c_ComProtocols.size(); ++u32_Counter)
               {
                  if (pc_Node->c_ComProtocols[u32_Counter].u32_DataPoolIndex ==
                      static_cast<uint32>(this->ms32_DataPoolIndex))
                  {
                     *this->mpe_ComProtocolType = pc_Node->c_ComProtocols[u32_Counter].e_Type;
                  }
               }
            }

            this->m_InitComboBoxProtocols(false, *this->mpe_ComProtocolType);
         }
         else
         {
            C_OSCCanProtocol::E_Type e_Type;
            this->m_InitComboBoxProtocols(true);
            //Get selected type after combo box was initialized
            e_Type = m_GetSelectedProtocol();
            //Adapt initial flag status (only if new)
            switch (e_Type)
            {
            case C_OSCCanProtocol::eLAYER2:
               this->mpc_Ui->pc_CheckBoxSafety->setChecked(false);
               break;
            case C_OSCCanProtocol::eECES:
               this->mpc_Ui->pc_CheckBoxSafety->setChecked(true);
               break;
            case C_OSCCanProtocol::eCAN_OPEN_SAFETY:
               this->mpc_Ui->pc_CheckBoxSafety->setChecked(true);
               break;
            }
         }
         //Initial name generation
         m_OnComTypeChange();
      }

      this->m_CheckDatapoolName();

      //Insert version
      this->mpc_Ui->pc_LineEditVersion_2->setText(c_Version);
   }

   this->m_UpdateSizePrediction();

   if (oq_SelectName == true)
   {
      this->mpc_Ui->pc_LineEditDatapoolName->setFocus();
      this->mpc_Ui->pc_LineEditDatapoolName->selectAll();
   }

   //Application section
   this->mpc_Ui->pc_GroupBoxApplication->setVisible(oq_ShowApplicationSection);

   // Share Datapool configuration
   if (this->ms32_DataPoolIndex >= 0)
   {
      const C_PuiSdSharedDatapools & rc_SharedDatapools = C_PuiSdHandler::h_GetInstance()->GetSharedDatapoolsConst();
      uint32 u32_SharedDatapoolGroup;
      C_OSCNodeDataPoolId c_DpId(this->mu32_NodeIndex,
                                 static_cast<uint32>(this->ms32_DataPoolIndex));

      q_IsShared = rc_SharedDatapools.IsSharedDatapool(c_DpId, &u32_SharedDatapoolGroup);

      if (q_IsShared == true)
      {
         // Scenario 1: Edit of a shared Datapool
         C_SdNdeDataPoolUtil::GetSharedDatapoolGroup(u32_SharedDatapoolGroup, c_DpId, this->mu32_NodeIndex,
                                                     c_DatapoolGroup);
      }
   }
   else if (opc_SharedDatapoolId != NULL)
   {
      const C_PuiSdSharedDatapools & rc_SharedDatapools = C_PuiSdHandler::h_GetInstance()->GetSharedDatapoolsConst();
      uint32 u32_SharedDatapoolGroup;
      const bool q_ShareGroupExists = rc_SharedDatapools.IsSharedDatapool(*opc_SharedDatapoolId,
                                                                          &u32_SharedDatapoolGroup);

      if (q_ShareGroupExists == true)
      {
         // Scenario 2: New shared Datapool and the shared Datapool was already shared with an other Datapool
         // The group exist. We want all group member. The second parameter for ignoring a concrete Datapool must be
         // invalid. The new Datapool is not registered in shared Datapools yet.
         C_SdNdeDataPoolUtil::GetSharedDatapoolGroup(u32_SharedDatapoolGroup,
                                                     C_OSCNodeDataPoolId(0xFFFFFFFFU, 0xFFFFFFFFU),
                                                     this->mu32_NodeIndex,
                                                     c_DatapoolGroup);
      }
      else
      {
         // Scenario 3: New shared Datapool. The shared Datapool was stand alone yet
         // The group does not exist and both Datapools are not registered yet.
         // The list entry must be created manually
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
            opc_SharedDatapoolId->u32_NodeIndex);

         tgl_assert(pc_Node != NULL);
         if ((pc_Node != NULL) &&
             (opc_SharedDatapoolId->u32_DataPoolIndex < pc_Node->c_DataPools.size()))
         {
            QString c_Text = "";
            if (opc_SharedDatapoolId->u32_NodeIndex != this->mu32_NodeIndex)
            {
               // Add node as namespace
               c_Text += QString(pc_Node->c_Properties.c_Name.c_str()) + QString("::");
            }

            c_Text += pc_Node->c_DataPools[opc_SharedDatapoolId->u32_DataPoolIndex].c_Name.c_str();

            c_DatapoolGroup.push_back(c_Text);
         }
      }

      // New shared Datapool. Deactivate the break relation button
      this->mpc_Ui->pc_BushButtonBreakRelation->setEnabled(false);

      q_IsShared = true;
   }
   else
   {
      // New Datapool is not shared
      q_IsShared = false;
   }

   //apply datapool style
   this->m_ApplyType(q_IsShared);

   if (q_IsShared == true)
   {
      uint32 u32_DatapoolCounter;

      // Fill the lists with the shared Datapools
      for (u32_DatapoolCounter = 0U; u32_DatapoolCounter < c_DatapoolGroup.size(); ++u32_DatapoolCounter)
      {
         this->mpc_Ui->pc_ListWidgetSharedDatapoolInfo->addItem(c_DatapoolGroup[u32_DatapoolCounter]);
      }

      // Adapt the labels of all shared information
      this->mpc_Ui->pc_LabelVersion->setText(this->mpc_Ui->pc_LabelVersion->text() +
                                             C_GtGetText::h_GetText(" (Shared property)"));
      this->mpc_Ui->pc_LabelDatapoolSize->setText(this->mpc_Ui->pc_LabelDatapoolSize->text() +
                                                  C_GtGetText::h_GetText(" (Shared property)"));
   }

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_SdNdeDatapoolProperties::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked,
           this, &C_SdNdeDatapoolProperties::m_CancelClicked);
   connect(this->mpc_Ui->pc_LineEditDatapoolName, &QLineEdit::textChanged, this,
           &C_SdNdeDatapoolProperties::m_CheckDatapoolName);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxSize, static_cast<void (QSpinBox::*)(stw_types::sintn)>(&QSpinBox::valueChanged),
           this, &C_SdNdeDatapoolProperties::m_SpinBoxChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxProtocol, static_cast<void (QComboBox::*)(
                                                             sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SdNdeDatapoolProperties::m_OnComTypeChange);
   connect(this->mpc_Ui->pc_CheckBoxSafety, &C_OgeChxTristateTransparentToggle::toggled, this,
           &C_SdNdeDatapoolProperties::m_OnSafetyChange);
   connect(this->mpc_Ui->pc_BushButtonBreakRelation, &C_OgePubConfigure::clicked, this,
           &C_SdNdeDatapoolProperties::m_BreakSharedRelation);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDatapoolProperties::~C_SdNdeDatapoolProperties(void)
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_ParentDialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::InitStaticNames(void)
{
   this->mpc_ParentDialog->SetSubTitle(C_GtGetText::h_GetText("Properties"));
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_CommentText->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here ..."));
   this->mpc_Ui->pc_LabelName->setText(C_GtGetText::h_GetText("Name"));
   this->mpc_Ui->pc_LabelComment->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_LabelVersion->setText(C_GtGetText::h_GetText("Version"));
   this->mpc_Ui->pc_LabelConfigurationHeading->setText(C_GtGetText::h_GetText("Configuration"));
   this->mpc_Ui->pc_LabelSafetyContent->setText(C_GtGetText::h_GetText("Safety Relevant Content"));
   this->mpc_Ui->pc_LabelRelatedApplication->setText(C_GtGetText::h_GetText(
                                                        "Related Programmable Application"));
   this->mpc_Ui->pc_LabelComProt->setText(C_GtGetText::h_GetText("Communication Protocol"));
   this->mpc_Ui->pc_LabelDatapoolSize->setText(C_GtGetText::h_GetText("Datapool Size"));
   this->mpc_Ui->pc_LabelDataPoolUsage->setText(C_GtGetText::h_GetText("Resulting Datapool Usage"));
   this->mpc_Ui->pc_LabelDataPoolReservation->setText(C_GtGetText::h_GetText("Resulting Node NVM Reservation"));
   this->mpc_Ui->pc_LabelDatapoolShareConfiguration->setText(C_GtGetText::h_GetText(
                                                                "Share Configuration with other Datapools:"));
   this->mpc_Ui->pc_BushButtonBreakRelation->setText(C_GtGetText::h_GetText("Break Relation"));

   //Tool tips
   this->mpc_Ui->pc_LabelName->SetToolTipInformation(C_GtGetText::h_GetText("Name"),
                                                     C_GtGetText::h_GetText(
                                                        "Symbolic Datapool name. Unique within a node."
                                                        "\nFollowing C naming conventions are required:"
                                                        "\n - must not be empty"
                                                        "\n - only alphanumeric characters + \"_\""
                                                        "\n - should not be longer than 31 characters"));

   this->mpc_Ui->pc_LabelComment->SetToolTipInformation(C_GtGetText::h_GetText("Comment"),
                                                        C_GtGetText::h_GetText("Comment for this Datapool."));

   this->mpc_Ui->pc_LabelVersion->SetToolTipInformation(C_GtGetText::h_GetText("Version"),
                                                        C_GtGetText::h_GetText(
                                                           "Datapool version. (Example: v01.00r00)"));

   this->mpc_Ui->pc_LabelSafetyContent->SetToolTipInformation(C_GtGetText::h_GetText("Safety Relevant Content"),
                                                              C_GtGetText::h_GetText(
                                                                 "Does this Datapool contain safety relevant elements?"
                                                                 "\nThis property is relevant for code generation."));

   this->mpc_Ui->pc_LabelRelatedApplication->SetToolTipInformation(
      C_GtGetText::h_GetText("Related Programmable Application"),
      C_GtGetText::h_GetText("Which programmable application owns this Datapool? "
                             "\nOnly relevant if code generation is enabled."));

   this->mpc_Ui->pc_LabelComProt->SetToolTipInformation(
      C_GtGetText::h_GetText("Communication Protocol"),
      C_GtGetText::h_GetText("Type of process data exchange CAN protocol:"
                             "\n - OSI Layer 2 - Layer 2 CAN data"
                             "\n - ECeS - ESX CAN efficient safety protocol"
                             "\n - ECoS - ESX CANopen safety protocol"));

   this->mpc_Ui->pc_LabelDatapoolSize->SetToolTipInformation(
      C_GtGetText::h_GetText("Datapool Size"),
      C_GtGetText::h_GetText("Reserved size in bytes for this Datapool."));

   this->mpc_Ui->pc_LabelDataPoolUsage->SetToolTipInformation(
      C_GtGetText::h_GetText("Resulting Datapool Usage"),
      C_GtGetText::h_GetText("Resulting Datapool usage in percent based on Datapool reservation size and "
                             "\ncurrent Datapool usage (size of declared parameters)"
                             "\nValues greater than 100% will cause Datapool reservation size error."));

   this->mpc_Ui->pc_LabelDataPoolReservation->SetToolTipInformation(
      C_GtGetText::h_GetText("Resulting Node NVM Reservation"),
      C_GtGetText::h_GetText("Resulting node NVM reservation in percent based on Datapool reservation "
                             "\nsizes of all Datapools declared on this node."));

   this->mpc_Ui->pc_LabelDatapoolShareConfiguration->SetToolTipInformation(
      C_GtGetText::h_GetText("Shared Datapool Configuration"),
      C_GtGetText::h_GetText("The Datapool is a shared Datapool. All connected Datapools are in the list below."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         m_OkClicked();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::m_OkClicked(void)
{
   std::vector<C_SCLString> c_ExistingDatapoolNames;
   //Check name
   if (this->m_CheckDatapoolNameNotDuplicate(&c_ExistingDatapoolNames) == false)
   {
      //No accept allowed
      const QString c_Description = QString(C_GtGetText::h_GetText(
                                               "A Datapool with the name \"%1\" already exists. Choose another name.")).
                                    arg(
         this->mpc_Ui->pc_LineEditDatapoolName->text());
      QString c_Details;
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Datapool naming"));
      c_Message.SetDescription(c_Description);
      c_Details.append(C_GtGetText::h_GetText("Used Datapool names:\n"));
      for (uint32 u32_ItExistingName = 0UL; u32_ItExistingName < c_ExistingDatapoolNames.size(); ++u32_ItExistingName)
      {
         const C_SCLString & rc_Name = c_ExistingDatapoolNames[u32_ItExistingName];
         c_Details.append(QString("\"%1\"\n").arg(rc_Name.c_str()));
      }
      c_Message.SetDetails(c_Details);
      c_Message.Execute();
   }
   else
   {
      //Save possible, continue
      if (this->mpc_OSCDataPool != NULL)
      {
         QString c_Version = this->mpc_Ui->pc_LineEditVersion_2->text();
         //adapt data
         this->mpc_OSCDataPool->c_Name = this->mpc_Ui->pc_LineEditDatapoolName->text().toStdString().c_str();
         this->mpc_OSCDataPool->c_Comment = this->mpc_Ui->pc_CommentText->toPlainText().toStdString().c_str();
         this->mpc_OSCDataPool->q_IsSafety = this->mpc_Ui->pc_CheckBoxSafety->isChecked();
         this->mpc_OSCDataPool->u32_NvMSize = static_cast<uint32>(this->mpc_Ui->pc_SpinBoxSize->value());

         m_HandleDataPoolSafetyAdaptation();

         //Application
         if (this->mpc_Ui->pc_ComboBoxApplication->currentIndex() > 0)
         {
            const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
            if (pc_Node != NULL)
            {
               sint32 s32_ApplicationIndex = -1;
               sint32 s32_Counter = 0;
               //Map combo box index to data block index
               for (uint32 u32_ItDataBlock = 0; u32_ItDataBlock < pc_Node->c_Applications.size(); ++u32_ItDataBlock)
               {
                  const C_OSCNodeApplication & rc_DataBlock = pc_Node->c_Applications[u32_ItDataBlock];
                  if (rc_DataBlock.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
                  {
                     if (static_cast<sint32>(this->mpc_Ui->pc_ComboBoxApplication->currentIndex() - 1) == s32_Counter)
                     {
                        s32_ApplicationIndex = static_cast<sint32>(u32_ItDataBlock);
                     }
                     //Important iterator step for counter
                     ++s32_Counter;
                  }
               }
               this->mpc_OSCDataPool->s32_RelatedDataBlockIndex = s32_ApplicationIndex;
            }
         }
         else
         {
            //Invalid application
            this->mpc_OSCDataPool->s32_RelatedDataBlockIndex = -1;
         }

         //Parse version
         //Format: v99.99r99;0
         tgl_assert(c_Version.size() <= 9);
         if (c_Version.size() <= 9)
         {
            const QRegExp c_Separators("(\\.|r)");
            const QStringList c_Parts = c_Version.remove(0, 1).split(c_Separators);
            tgl_assert(c_Parts.size() == 3);
            if (c_Parts.size() == 3)
            {
               this->mpc_OSCDataPool->au8_Version[0] = static_cast<uint8>(c_Parts[0].toInt());
               this->mpc_OSCDataPool->au8_Version[1] = static_cast<uint8>(c_Parts[1].toInt());
               this->mpc_OSCDataPool->au8_Version[2] = static_cast<uint8>(c_Parts[2].toInt());
            }
         }

         if (me_DatapoolType == C_OSCNodeDataPool::eCOM)
         {
            if (this->mpe_ComProtocolType != NULL)
            {
               const C_OSCCanProtocol::E_Type e_Protocol = this->m_GetSelectedProtocol();

               *this->mpe_ComProtocolType = e_Protocol;
            }
         }

         if (this->mpc_ParentDialog != NULL)
         {
            this->mpc_ParentDialog->accept();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::m_CancelClicked(void)
{
   if (this->mpc_ParentDialog != NULL)
   {
      this->mpc_ParentDialog->reject();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply datapool type
   - Load correct picture
   - show correct interaction elements

    \param[in]    oq_SharedDatapool    Flag if Datapool is a shared Datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::m_ApplyType(const bool oq_SharedDatapool)
{
   QString c_Pic;

   if (this->me_DatapoolType == C_OSCNodeDataPool::eDIAG)
   {
      if (oq_SharedDatapool == true)
      {
         c_Pic = "://images/system_definition/IconDataPoolBigShared.svg";
      }
      else
      {
         c_Pic = "://images/system_definition/IconDataPoolBig.svg";
      }
      this->mpc_Ui->pc_ComProtGroup->setVisible(false);
      this->mpc_Ui->pc_SizeGroup->setVisible(false);
      this->mpc_Ui->pc_SizeInfoUsageGroup->setVisible(false);
      this->mpc_Ui->pc_SizeInfoReservedGroup->setVisible(false);
      this->mpc_Ui->pc_LabDatapoolType->setText(C_GtGetText::h_GetText("DIAG"));
   }
   else if (this->me_DatapoolType == C_OSCNodeDataPool::eNVM)
   {
      if (oq_SharedDatapool == true)
      {
         c_Pic = "://images/system_definition/IconDataPoolBigShared.svg";
      }
      else
      {
         c_Pic = "://images/system_definition/IconDataPoolBig.svg";
      }
      this->mpc_Ui->pc_ComProtGroup->setVisible(false);
      this->mpc_Ui->pc_LabDatapoolType->setText(C_GtGetText::h_GetText("NVM"));
   }
   else
   {
      if (oq_SharedDatapool == true)
      {
         c_Pic = "://images/system_definition/IconDataPoolBigShared.svg";
      }
      else
      {
         c_Pic = "://images/system_definition/IconDataPoolBig.svg";
      }
      this->mpc_Ui->pc_SizeGroup->setVisible(false);
      this->mpc_Ui->pc_SizeInfoUsageGroup->setVisible(false);
      this->mpc_Ui->pc_SizeInfoReservedGroup->setVisible(false);
      //option is not needed for com datapools
      this->mpc_Ui->pc_CheckBoxSafety->setChecked(false);
      this->mpc_Ui->pc_LabDatapoolType->setText(C_GtGetText::h_GetText("COMM"));
   }

   this->mpc_Ui->pc_GroupBoxShareConfiguration->setVisible(oq_SharedDatapool);
   this->mpc_Ui->pc_GroupBoxShareConfigurationList->setVisible(oq_SharedDatapool);

   //set pic
   this->mpc_Ui->pc_LabDatapoolImage->SetSvg(c_Pic);

   //adapt size (set min height will auto update the height to visible elements
   this->setMinimumHeight(0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load code generation flag and application data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::m_LoadCodeGenerationAndApplication(void) const
{
   sint32 s32_DataBlockIndex = 0;

   this->mpc_Ui->pc_ComboBoxApplication->clear();
   this->mpc_Ui->pc_ComboBoxApplication->addItem(C_GtGetText::h_GetText("<not assigned>"));

   if (this->mpc_OSCDataPool != NULL)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

      if (pc_Node != NULL)
      {
         sint32 s32_Counter = 1;
         //Add all programmable applications and map data block index to combo box index
         for (uint32 u32_ItDataBlock = 0; u32_ItDataBlock < pc_Node->c_Applications.size(); ++u32_ItDataBlock)
         {
            const C_OSCNodeApplication & rc_DataBlock = pc_Node->c_Applications[u32_ItDataBlock];
            if (rc_DataBlock.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
            {
               this->mpc_Ui->pc_ComboBoxApplication->addItem(rc_DataBlock.c_Name.c_str());
               if ((this->mpc_OSCDataPool->s32_RelatedDataBlockIndex >= 0) &&
                   (static_cast<uint32>(this->mpc_OSCDataPool->s32_RelatedDataBlockIndex) == u32_ItDataBlock))
               {
                  s32_DataBlockIndex = s32_Counter;
               }
               //Important iteration step
               ++s32_Counter;
            }
         }
      }
   }
   this->mpc_Ui->pc_ComboBoxApplication->setCurrentIndex(s32_DataBlockIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::m_SpinBoxChanged(const stw_types::sintn osn_Value) const
{
   Q_UNUSED(osn_Value)

   this->m_UpdateSizePrediction();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::m_UpdateSizePrediction(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
      this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OSCDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;
      tgl_assert(pc_DevDef != NULL);
      if (pc_DevDef != NULL)
      {
         uint32 u32_SizeOfAllNvmDatapools = 0;
         uint32 u32_PercentageReservation;
         uint32 u32_PercentageUsage;
         uint32 u32_DpCounter;
         const uint32 u32_ActSize = static_cast<uint32>(this->mpc_Ui->pc_SpinBoxSize->value());

         // get the entire size of all NVM datapools. check all datapools of the lists
         for (u32_DpCounter = 0; u32_DpCounter < pc_Node->c_DataPools.size(); ++u32_DpCounter)
         {
            if (pc_Node->c_DataPools[u32_DpCounter].e_Type == C_OSCNodeDataPool::eNVM)
            {
               if (this->ms32_DataPoolIndex < 0)
               {
                  // it is a new datapool
                  u32_SizeOfAllNvmDatapools += pc_Node->c_DataPools[u32_DpCounter].u32_NvMSize;
               }
               else
               {
                  if (u32_DpCounter != static_cast<uint32>(this->ms32_DataPoolIndex))
                  {
                     // it is not the actual edited datapool
                     u32_SizeOfAllNvmDatapools += pc_Node->c_DataPools[u32_DpCounter].u32_NvMSize;
                  }
                  else
                  {
                     // it is the actual edited datapool. do not use the old value
                     u32_SizeOfAllNvmDatapools += u32_ActSize;
                  }
               }
            }
         }
         if (this->ms32_DataPoolIndex < 0)
         {
            // it is a new datapool. add the new value separately
            u32_SizeOfAllNvmDatapools += u32_ActSize;
         }

         // update the label with reservation prediction
         if ((pc_DevDef->u32_UserEepromSizeBytes > 0U) &&
             (u32_SizeOfAllNvmDatapools > 0U))
         {
            u32_PercentageReservation = (u32_SizeOfAllNvmDatapools * 100U) / pc_DevDef->u32_UserEepromSizeBytes;
         }
         else
         {
            u32_PercentageReservation = 0U;
         }

         this->mpc_Ui->pc_LabelDataPoolReservation->setText(C_GtGetText::h_GetText("Resulting Node NVM Reservation: ") +
                                                            QString::number(u32_PercentageReservation) +
                                                            QString("%") +
                                                            QString(" (Node NVM size = ") +
                                                            QString::number(pc_DevDef->u32_UserEepromSizeBytes) +
                                                            QString(" Bytes)"));

         // update the label with usage prediction
         if ((u32_ActSize > 0U) && (this->mpc_OSCDataPool != NULL))
         {
            u32_PercentageUsage = (this->mpc_OSCDataPool->GetNumBytesUsed() * 100U) / u32_ActSize;
         }
         else
         {
            u32_PercentageUsage = 100;
         }

         this->mpc_Ui->pc_LabelDataPoolUsage->setText(C_GtGetText::h_GetText("Resulting Datapool Usage: ") +
                                                      QString::number(u32_PercentageUsage) +
                                                      QString("%"));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check datapool name
   - check input
   - show/hide invalid icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::m_CheckDatapoolName(void) const
{
   QString c_Content;
   const stw_scl::C_SCLString c_Name = this->mpc_Ui->pc_LineEditDatapoolName->text().toStdString().c_str();

   //check
   bool q_NameIsValid = C_OSCUtils::h_CheckValidCName(c_Name);

   if (q_NameIsValid == false)
   {
      c_Content += C_GtGetText::h_GetText("- is empty or contains invalid characters\n");
   }

   if (this->m_CheckDatapoolNameNotDuplicate(NULL) == false)
   {
      q_NameIsValid = false;
      c_Content += C_GtGetText::h_GetText("- is already in use\n");
   }

   //set invalid text property
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditDatapoolName, "Valid", q_NameIsValid);
   if (q_NameIsValid == true)
   {
      this->mpc_Ui->pc_LineEditDatapoolName->SetToolTipInformation("", "", C_NagToolTip::eDEFAULT);
   }
   else
   {
      const QString c_Heading = C_GtGetText::h_GetText("Datapool Name");
      this->mpc_Ui->pc_LineEditDatapoolName->SetToolTipInformation(c_Heading, c_Content, C_NagToolTip::eERROR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for duplicate data pool name

   \param[in,out] opc_ExistingDatapoolNames Optional parameter to list all OTHER existing datapool names

   \return
   True  Name valid
   False Name in conflict
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDatapoolProperties::m_CheckDatapoolNameNotDuplicate(
   std::vector<C_SCLString> * const opc_ExistingDatapoolNames) const
{
   bool q_NameIsValid = true;
   const stw_scl::C_SCLString c_Name = this->mpc_Ui->pc_LineEditDatapoolName->text().toStdString().c_str();

   if (this->ms32_DataPoolIndex >= 0)
   {
      const uint32 u32_DataPoolIndex = static_cast<uint32>(this->ms32_DataPoolIndex);
      if (C_PuiSdHandler::h_GetInstance()->CheckNodeDataPoolNameAvailable(this->mu32_NodeIndex,
                                                                          c_Name, &u32_DataPoolIndex,
                                                                          opc_ExistingDatapoolNames) == false)
      {
         q_NameIsValid = false;
      }
   }
   else
   {
      if (C_PuiSdHandler::h_GetInstance()->CheckNodeDataPoolNameAvailable(this->mu32_NodeIndex,
                                                                          c_Name, NULL,
                                                                          opc_ExistingDatapoolNames) == false)
      {
         q_NameIsValid = false;
      }
   }
   return q_NameIsValid;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::m_InitSpinBox(void) const
{
   if (this->mpc_OSCDataPool != NULL)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      if (pc_Node != NULL)
      {
         const C_OSCDeviceDefinition * const pc_Device = pc_Node->pc_DeviceDefinition;
         tgl_assert(pc_Device != NULL);
         if (pc_Device != NULL)
         {
            const uint32 u32_Maximum = pc_Device->u32_UserEepromSizeBytes;

            //Init spin box minimum
            this->mpc_Ui->pc_SpinBoxSize->SetMinimumCustom(1);
            //Init spin box maximum
            this->mpc_Ui->pc_SpinBoxSize->SetMaximumCustom(u32_Maximum);
            this->mpc_Ui->pc_SpinBoxSize->setValue(static_cast<sint32>(this->mpc_OSCDataPool->u32_NvMSize));
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::m_InitComboBoxProtocols(const bool oq_NewDataPool,
                                                        const C_OSCCanProtocol::E_Type oe_ComProtocolType) const
{
   bool q_Add;
   bool q_SelectActualItem;
   bool q_Selected = false;

   this->mpc_Ui->pc_ComboBoxProtocol->clear();

   // check Layer 2
   if ((oq_NewDataPool == true) ||
       (oe_ComProtocolType != C_OSCCanProtocol::eLAYER2))
   {
      q_Add = this->m_IsProtocolSelectable(C_OSCCanProtocol::eLAYER2);
      q_SelectActualItem = false;
   }
   else
   {
      q_Add = true;
      q_SelectActualItem = true;
   }
   // add Layer 2
   if (q_Add == true)
   {
      const sintn sn_Index = this->mpc_Ui->pc_ComboBoxProtocol->count();
      this->mpc_Ui->pc_ComboBoxProtocol->addItem(C_PuiSdUtil::h_ConvertProtocolTypeToString(C_OSCCanProtocol::eLAYER2));
      if (q_SelectActualItem == true)
      {
         this->mpc_Ui->pc_ComboBoxProtocol->setCurrentIndex(sn_Index);
         q_Selected = true;
      }
   }

   // check ECeS
   if ((oq_NewDataPool == true) ||
       (oe_ComProtocolType != C_OSCCanProtocol::eECES))
   {
      q_Add = this->m_IsProtocolSelectable(C_OSCCanProtocol::eECES);
      q_SelectActualItem = false;
   }
   else
   {
      q_Add = true;
      q_SelectActualItem = true;
   }
   // add ECeS
   if (q_Add == true)
   {
      const sintn sn_Index = this->mpc_Ui->pc_ComboBoxProtocol->count();
      this->mpc_Ui->pc_ComboBoxProtocol->addItem(C_PuiSdUtil::h_ConvertProtocolTypeToString(C_OSCCanProtocol::eECES));
      if (q_SelectActualItem == true)
      {
         this->mpc_Ui->pc_ComboBoxProtocol->setCurrentIndex(sn_Index);
         q_Selected = true;
      }
   }

   // check CANopen Safety
   if ((oq_NewDataPool == true) ||
       (oe_ComProtocolType != C_OSCCanProtocol::eCAN_OPEN_SAFETY))
   {
      q_Add = this->m_IsProtocolSelectable(C_OSCCanProtocol::eCAN_OPEN_SAFETY);
      q_SelectActualItem = false;
   }
   else
   {
      q_Add = true;
      q_SelectActualItem = true;
   }
   // add CANopen Safety
   if (q_Add == true)
   {
      const sintn sn_Index = this->mpc_Ui->pc_ComboBoxProtocol->count();
      this->mpc_Ui->pc_ComboBoxProtocol->addItem(C_PuiSdUtil::h_ConvertProtocolTypeToString(C_OSCCanProtocol::
                                                                                            eCAN_OPEN_SAFETY));
      if (q_SelectActualItem == true)
      {
         this->mpc_Ui->pc_ComboBoxProtocol->setCurrentIndex(sn_Index);
         q_Selected = true;
      }
   }

   if ((q_Selected == false) &&
       (this->mpc_Ui->pc_ComboBoxProtocol->count() > 0))
   {
      // set something
      this->mpc_Ui->pc_ComboBoxProtocol->setCurrentIndex(0);
   }
}

//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDatapoolProperties::m_IsProtocolSelectable(const C_OSCCanProtocol::E_Type oe_ComProtocolType) const
{
   bool q_Return = true;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      uint32 u32_Counter;

      for (u32_Counter = 0U; u32_Counter < pc_Node->c_ComProtocols.size(); ++u32_Counter)
      {
         if (pc_Node->c_ComProtocols[u32_Counter].e_Type == oe_ComProtocolType)
         {
            q_Return = false;
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected protocol

   \return
   Currently selected protocol
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanProtocol::E_Type C_SdNdeDatapoolProperties::m_GetSelectedProtocol(void) const
{
   C_OSCCanProtocol::E_Type e_Retval;
   const QString c_ActualProtocol = this->mpc_Ui->pc_ComboBoxProtocol->currentText();

   if (c_ActualProtocol == C_PuiSdUtil::h_ConvertProtocolTypeToString(C_OSCCanProtocol::eECES))
   {
      e_Retval = C_OSCCanProtocol::eECES;
   }
   else if (c_ActualProtocol == C_PuiSdUtil::h_ConvertProtocolTypeToString(C_OSCCanProtocol::eCAN_OPEN_SAFETY))
   {
      e_Retval = C_OSCCanProtocol::eCAN_OPEN_SAFETY;
   }
   else
   {
      e_Retval = C_OSCCanProtocol::eLAYER2;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle com type change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::m_OnComTypeChange(void) const
{
   const C_OSCCanProtocol::E_Type e_Type = m_GetSelectedProtocol();

   this->mpc_Ui->pc_LineEditDatapoolName->setText(C_PuiSdUtil::h_ConvertProtocolTypeToDatapoolNameString(e_Type));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle on safety flag state change

   \param[in] oq_IsSafety Is safety activated
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::m_OnSafetyChange(const bool oq_IsSafety) const
{
   if (oq_IsSafety == true)
   {
      C_OgeWiCustomMessage c_Message(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eQUESTION);
      C_OgeWiCustomMessage::E_Outputs e_Output;
      c_Message.SetHeading(C_GtGetText::h_GetText("Safety property enable"));
      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "When enabling the safety property, the access permission of all existing data "
                                  "elements of this Datapool will be changed to RO (read only).\n"
                                  "Do you want to enable the safety property?"));
      c_Message.SetOKButtonText(C_GtGetText::h_GetText("Enable"));
      c_Message.SetNOButtonText(C_GtGetText::h_GetText("Cancel"));
      e_Output = c_Message.Execute();
      switch (e_Output)
      {
      case C_OgeWiCustomMessage::eYES:
         break;
      default:
         //Reset
         this->mpc_Ui->pc_CheckBoxSafety->setChecked(false);
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle all required actions for change of data pool to contain safety data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::m_HandleDataPoolSafetyAdaptation(void)
{
   if ((this->mpc_OSCDataPool != NULL) && (this->mpc_OSCDataPool->q_IsSafety == true))
   {
      for (uint32 u32_ItList = 0; u32_ItList < this->mpc_OSCDataPool->c_Lists.size(); ++u32_ItList)
      {
         C_OSCNodeDataPoolList & rc_CurList = this->mpc_OSCDataPool->c_Lists[u32_ItList];
         for (uint32 u32_ItElement = 0; u32_ItElement < rc_CurList.c_Elements.size(); ++u32_ItElement)
         {
            C_OSCNodeDataPoolListElement & rc_CurElement = rc_CurList.c_Elements[u32_ItElement];
            rc_CurElement.e_Access = C_OSCNodeDataPoolListElement::eACCESS_RO;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Breaks the relation of the shared Datapool to the connected Datapools
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDatapoolProperties::m_BreakSharedRelation(void)
{
   if (this->ms32_DataPoolIndex >= 0)
   {
      C_PuiSdSharedDatapools & rc_SharedDatapools = C_PuiSdHandler::h_GetInstance()->GetSharedDatapools();
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eQUESTION);

      c_Message.SetHeading(C_GtGetText::h_GetText("Break Shared Datapool Relation"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Do you really want to delete the relation to other Datapool(s)\n"
                                                      "This is not revertible.\n\n"
                                                      "After relation break, the Datapool acts like a stand-alone Datapool."));

      if (c_Message.Execute() == C_OgeWiCustomMessage::eYES)
      {
         rc_SharedDatapools.RemoveSharedDatapool(C_OSCNodeDataPoolId(this->mu32_NodeIndex,
                                                                     static_cast<uint32>(this->ms32_DataPoolIndex)));

         this->mpc_Ui->pc_BushButtonBreakRelation->setEnabled(false);
         this->mpc_Ui->pc_ListWidgetSharedDatapoolInfo->clear();

         // Reinit the labels and the ui
         this->InitStaticNames();
         this->m_UpdateSizePrediction();

         this->m_ApplyType(false);
         // The parent dialog must be resized to the minimum.This will cause an automatic adaption of the dialog
         // to the correct size
         this->mpc_ParentDialog->setMinimumHeight(0);
         this->mpc_ParentDialog->setMaximumHeight(0);
      }
   }
}
