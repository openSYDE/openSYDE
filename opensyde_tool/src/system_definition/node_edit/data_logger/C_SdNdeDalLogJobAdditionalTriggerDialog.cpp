//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog to edit the additional trigger condition

   Dialog to edit the additional trigger condition

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "stwerrors.hpp"

#include "C_SdNdeDalLogJobAdditionalTriggerDialog.hpp"
#include "ui_C_SdNdeDalLogJobAdditionalTriggerDialog.h"

#include "../../../system_views/dashboards/properties/C_SyvDaPeDataElementBrowse.hpp"
#include "TglUtils.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_SdNdeDalTriggerCheckHelper.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_OgeWiUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;
using namespace stw::errors;
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

   \param[in,out] orc_Parent        Optional pointer to parent
   \param[in]     orc_LogJobName    Name of the log job
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobAdditionalTriggerDialog::C_SdNdeDalLogJobAdditionalTriggerDialog(
   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const QString & orc_LogJobName) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDalLogJobAdditionalTriggerDialog),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(0),
   mu32_DataLoggerJobIndex(0),
   mq_TriggerConditionValid(false)
{
   this->mpc_Ui->setupUi(this);
   this->mc_LogJobName = orc_LogJobName;

   this->InitStaticNames();

   // register the widget for display
   this->mrc_ParentDialog.SetWidget(this);

   // Set no padding for this button
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_PbAddDataElement, "Align", "right");

   // Add data element
   const QSize c_SIZE(13, 13);
   this->mpc_Ui->pc_PbAddDataElement->setIconSize(c_SIZE);
   this->mpc_Ui->pc_PbAddDataElement->SetCustomIcon("://images/IconAddEnabled.svg", "://images/IconAddDisabled.svg");
   this->mpc_Ui->pc_PbAddDataElement->SetTextRightAligned();

   connect(this->mpc_Ui->pc_PushButtonCancel_2, &QPushButton::clicked, this,
           &C_SdNdeDalLogJobAdditionalTriggerDialog::m_CancelClicked);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this,
           &C_SdNdeDalLogJobAdditionalTriggerDialog::m_OkClicked);

   connect(this->mpc_Ui->pc_PbAddDataElement, &QPushButton::clicked, this,
           &C_SdNdeDalLogJobAdditionalTriggerDialog::m_AddDataElementClicked);

   connect(this->mpc_Ui->pc_TextEditTriggerCondition, &QTextEdit::textChanged, this,
           &C_SdNdeDalLogJobAdditionalTriggerDialog::m_ValidateTriggerCondition);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobAdditionalTriggerDialog::~C_SdNdeDalLogJobAdditionalTriggerDialog()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerDialog::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText(this->mc_LogJobName.toStdString().c_str()));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Additional Trigger"));
   this->mpc_Ui->pc_LabelSettings->setText(C_GtGetText::h_GetText("Additional Trigger"));
   this->mpc_Ui->pc_LabelSample->setText(C_GtGetText::h_GetText(
                                            "Supported expressions: >, <, ==, >=, <=, !, &&, ||, (). \n"
                                            "Example: (NodeA::DP1::List1::Var1 > NodeB::DP2::List2::Var2) && Node3::DP3::List3::Var3 == 1"));
   this->mpc_Ui->pc_LabelNamingCheck->setText(C_GtGetText::h_GetText("Data Elements Naming Check"));
   this->mpc_Ui->pc_LabelSyntaxCheck->setText(C_GtGetText::h_GetText("Condition Syntax Check"));
   this->mpc_Ui->pc_PbAddDataElement->setText(C_GtGetText::h_GetText("Add Data Element"));
   this->mpc_Ui->pc_LabelNamingCheckResult->setText(C_GtGetText::h_GetText("-"));
   this->mpc_Ui->pc_LabelSyntaxCheckResult->setText(C_GtGetText::h_GetText("-"));
   this->mpc_Ui->pc_LabelTriggerCondition->setText(C_GtGetText::h_GetText("Condition"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerDialog::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for Ok button click

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerDialog::m_OkClicked()
{
   // Valid trigger condition
   if (this->m_SaveTriggerCondition() == true)
   {
      this->mrc_ParentDialog.accept();
   }
   else
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);

      // Show message
      c_Message.SetHeading(C_GtGetText::h_GetText("Trigger Condition"));
      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "Trigger condition checks have failed. Please correct the condition."));
      c_Message.SetCustomMinHeight(180, 270);
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new node data logger indices
 *
 *  Note : this is generally done when a new node is selected and correspondingly new data is to be loaded

   \param[in]  ou32_NodeIndex             Index of node
   \param[in]  ou32_DataLoggerJobIndex    Data logger job index
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1762,9175}
void C_SdNdeDalLogJobAdditionalTriggerDialog::SetNodeDataLoggerJob(const uint32_t ou32_NodeIndex,
                                                                   const uint32_t ou32_DataLoggerJobIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mu32_DataLoggerJobIndex = ou32_DataLoggerJobIndex;

   this->m_GetAdditionalTriggerCondition();
   this->m_ValidateTriggerCondition();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save the trigger condition

   \retval   true    if both data element and syntax checks pass
             false   if any of the checks fails
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalLogJobAdditionalTriggerDialog::m_SaveTriggerCondition()
{
   bool q_ReturnValue = false;

   if (this->mq_TriggerConditionValid == true)
   {
      // Note: The expression may be empty, in that case an empty condition will be stored.
      const C_SclString c_TriggerCondition = this->mpc_Ui->pc_TextEditTriggerCondition->toPlainText().toStdString();

      // save the expression
      C_PuiSdHandler::h_GetInstance()->SetDataLoggerAdditionalTriggerExpertModeString(this->mu32_NodeIndex,
                                                                                      this->mu32_DataLoggerJobIndex,
                                                                                      c_TriggerCondition);

      q_ReturnValue = true;
   }
   return q_ReturnValue;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for add data element button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerDialog::m_AddDataElementClicked()
{
   // Dialog opens up for data element selection
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);

   // Single element selection only
   C_SyvDaPeDataElementBrowse * const pc_Dialog = new C_SyvDaPeDataElementBrowse(*c_New, 0U, false, false, false, true,
                                                                                 true, true, NULL, false,
                                                                                 this->mu32_NodeIndex, true);

   //Resize
   c_New->SetSize(QSize(800, 800));

   // Save the selected data elements to system definition
   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      std::vector<C_PuiSvDbNodeDataPoolListElementId> c_DataElements = pc_Dialog->GetSelectedDataElements();

      //Cursor
      QApplication::setOverrideCursor(Qt::WaitCursor);

      // Data element was selected from the popup dialog
      // Since it is single selection, only using the first item in the list of data elements i.e c_DataElements[0]
      if (c_DataElements.size() > 0)
      {
         if (c_DataElements[0].GetIsValid() == true)
         {
            tgl_assert(C_PuiSdHandler::h_GetInstance()->CheckAndHandleNewElement(
                          c_DataElements[0]) == C_NO_ERR);

            const C_OscNodeDataPoolListElementOptArrayId & rc_ConfiguredElementOptArrayId = c_DataElements[0];

            const C_OscNodeDataPoolListElement * const pc_Element =
               C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(
                  rc_ConfiguredElementOptArrayId.u32_NodeIndex,
                  rc_ConfiguredElementOptArrayId.u32_DataPoolIndex,
                  rc_ConfiguredElementOptArrayId.u32_ListIndex,
                  rc_ConfiguredElementOptArrayId.u32_ElementIndex);

            // Fetch the existing trigger condition
            QString c_TriggerCondition = this->mpc_Ui->pc_TextEditTriggerCondition->toPlainText();
            if (pc_Element != NULL)
            {
               // Add the selected data element to the condition
               const QString c_ElementName = C_PuiSdUtil::h_GetNamespaceDatapoolElement(rc_ConfiguredElementOptArrayId);
               //if no previous trigger condition exists
               if (c_TriggerCondition.isEmpty())
               {
                  c_TriggerCondition = c_ElementName;
               }
               // if previous trigger condition exists, append the data element
               else
               {
                  const QString c_DataElement = static_cast<QString>(C_GtGetText::h_GetText("%1")).arg(
                     c_ElementName);
                  c_TriggerCondition.append(c_DataElement);
               }
               this->mpc_Ui->pc_TextEditTriggerCondition->setText(c_TriggerCondition);
            }
         }
      }

      //Cursor
      QApplication::restoreOverrideCursor();
   }

   if (c_New != NULL)
   {
      pc_Dialog->SaveUserSettings();
      pc_Dialog->PrepareCleanUp();
      c_New->HideOverlay();
      c_New->deleteLater();
   }
   //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks the validity of the trigger condition expression

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerDialog::m_ValidateTriggerCondition()
{
   bool q_IsSyntaxValid = false;
   bool q_AreVariablesValid = false;
   const C_SclString c_TriggerCondition = this->mpc_Ui->pc_TextEditTriggerCondition->toPlainText().toStdString();

   std::string c_ErrorString("");

   // if no trigger condition exists
   if (c_TriggerCondition.IsEmpty() == false)
   {
      const bool q_ReturnVal = C_SdNdeDalTriggerCheckHelper::h_Check(this->mu32_NodeIndex,
                                                                     c_TriggerCondition,
                                                                     &c_ErrorString,
                                                                     &q_AreVariablesValid, &q_IsSyntaxValid);

      if (q_ReturnVal == true)
      {
         this->mq_TriggerConditionValid = true;
      }
      else
      {
         this->mq_TriggerConditionValid = false;
      }

      if (q_AreVariablesValid == true)
      {
         this->mpc_Ui->pc_LabelNamingCheckResult->setText(C_GtGetText::h_GetText("PASS"));
      }
      else
      {
         this->mpc_Ui->pc_LabelNamingCheckResult->setText(static_cast<QString>(C_GtGetText::h_GetText("FAIL: %1")).arg(
                                                             c_ErrorString.c_str()));
      }

      if (q_IsSyntaxValid == true)
      {
         this->mpc_Ui->pc_LabelSyntaxCheckResult->setText(C_GtGetText::h_GetText("PASS"));
      }
      else
      {
         this->mpc_Ui->pc_LabelSyntaxCheckResult->setText(static_cast<QString>(C_GtGetText::h_GetText("FAIL: %1")).arg(
                                                             c_ErrorString.c_str()));
      }
   }
   else
   {
      this->mpc_Ui->pc_LabelNamingCheckResult->setText(C_GtGetText::h_GetText(
                                                          "-"));
      this->mpc_Ui->pc_LabelSyntaxCheckResult->setText(C_GtGetText::h_GetText(
                                                          "-"));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fetch the additional trigger condition and display it in the dialog

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerDialog::m_GetAdditionalTriggerCondition()
{
   // Fetch the trigger condition string and set it to the edit box
   const C_SclString c_TriggerCondition = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(this->mu32_NodeIndex,
                                                                                            this->mu32_DataLoggerJobIndex)
                                          ->c_Properties.
                                          c_AdditionalTriggerProperties.c_ExpertMode.c_TriggerConfiguration;

   disconnect(this->mpc_Ui->pc_TextEditTriggerCondition, &QTextEdit::textChanged, this,
              &C_SdNdeDalLogJobAdditionalTriggerDialog::m_ValidateTriggerCondition);
   if (c_TriggerCondition.IsEmpty() == false)
   {
      this->mpc_Ui->pc_TextEditTriggerCondition->setText(c_TriggerCondition.c_str());
      this->mpc_Ui->pc_TextEditTriggerCondition->moveCursor(QTextCursor::End);
   }

   connect(this->mpc_Ui->pc_TextEditTriggerCondition, &QTextEdit::textChanged, this,
           &C_SdNdeDalLogJobAdditionalTriggerDialog::m_ValidateTriggerCondition);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually

   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->m_OkClicked();
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
