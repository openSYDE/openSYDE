//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for IP Address configuration (implementation)

   Widget for IP Address configuration

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeIpAddressConfigurationWidget.h"
#include "TGLUtils.h"
#include "ui_C_SdNdeIpAddressConfigurationWidget.h"
#include "C_OgeWiCustomMessage.h"
#include "C_OgeWiUtil.h"
// QLineEdit is multiple times included with the following include, but cannot do anything else then suppress lint
// warning
//lint -save -e451
#include "C_OgeLeIpAddress.h"
//lint -restore

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent Optional pointer to parent
   \param[in]     ou32_Index Node index
   \param[in]     ou32_ComIf Node COM Interface Number
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeIpAddressConfigurationWidget::C_SdNdeIpAddressConfigurationWidget(
   stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32 ou32_NodeIndex, const uint32 ou32_ComIf) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeIpAddressConfigurationWidget),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(ou32_NodeIndex),
   mu32_ComIf(ou32_ComIf)
{
   mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   this->mrc_ParentDialog.SetTitle(static_cast<QString>(C_GtGetText::h_GetText("IP Address")));
   this->mrc_ParentDialog.SetSubTitle(static_cast<QString>(C_GtGetText::h_GetText("Settings")));

   //Init
   InitStaticNames();

   m_LoadData();

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this,
           &C_SdNdeIpAddressConfigurationWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked, this,
           &C_SdNdeIpAddressConfigurationWidget::m_CancelClicked);

   std::vector<QLineEdit *> c_LeIpAddress =  this->mpc_Ui->pc_WidgetIpAddress->GetLineEdits();
   for (std::vector<QLineEdit *>::iterator c_Iter = c_LeIpAddress.begin(); c_Iter != c_LeIpAddress.end(); ++c_Iter)
   {
      connect(dynamic_cast<C_OgeLeIpAddress *>(*c_Iter),
              &C_OgeLeIpAddress::SignalTabKey,
              this, &C_SdNdeIpAddressConfigurationWidget::SlotTabKey);
   }

   std::vector<QLineEdit *> c_LeSubNetMask =  this->mpc_Ui->pc_WidgetSubNetMask->GetLineEdits();
   for (std::vector<QLineEdit *>::iterator c_Iter = c_LeSubNetMask.begin(); c_Iter != c_LeSubNetMask.end(); ++c_Iter)
   {
      connect(dynamic_cast<C_OgeLeIpAddress *>(*c_Iter),
              &C_OgeLeIpAddress::SignalTabKey,
              this, &C_SdNdeIpAddressConfigurationWidget::SlotTabKey);
   }

   std::vector<QLineEdit *> c_LeDefaultGateway =  this->mpc_Ui->pc_WidgetDefaultGateway->GetLineEdits();
   for (std::vector<QLineEdit *>::iterator c_Iter = c_LeDefaultGateway.begin(); c_Iter != c_LeDefaultGateway.end();
        ++c_Iter)
   {
      connect(dynamic_cast<C_OgeLeIpAddress *>(*c_Iter),
              &C_OgeLeIpAddress::SignalTabKey,
              this, &C_SdNdeIpAddressConfigurationWidget::SlotTabKey);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeIpAddressConfigurationWidget::~C_SdNdeIpAddressConfigurationWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeIpAddressConfigurationWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LabelDataElement->setText(C_GtGetText::h_GetText("IP Address Settings"));
   this->mpc_Ui->pc_LabelIpAddress->setText(C_GtGetText::h_GetText("IP Address"));
   this->mpc_Ui->pc_LabelSubNetMask->setText(C_GtGetText::h_GetText("Sub Net Mask"));
   this->mpc_Ui->pc_LabelDefaultGateway->setText(C_GtGetText::h_GetText("Default Gateway"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot to provide 'Tab' key behaviour for the three IP address fields.

   When user presses 'Tab' the next IP address line or button should be focused.

   \param[in]       opc_LineEdit     One line edit part of one IP address line where 'Tab' key was pressed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeIpAddressConfigurationWidget::SlotTabKey(const QLineEdit * const opc_LineEdit)
{
   if (this->mpc_Ui->pc_WidgetIpAddress->IsLineEditPartOfWidget(opc_LineEdit) == true)
   {
      this->mpc_Ui->pc_WidgetSubNetMask->SetCursorPosition(0);
   }
   else if (this->mpc_Ui->pc_WidgetSubNetMask->IsLineEditPartOfWidget(opc_LineEdit) == true)
   {
      this->mpc_Ui->pc_WidgetDefaultGateway->SetCursorPosition(0);
   }
   else
   {
      if (this->mpc_Ui->pc_WidgetDefaultGateway->IsLineEditPartOfWidget(opc_LineEdit) == true)
      {
         this->mpc_Ui->pc_BushButtonCancel->setFocus();
      }
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeIpAddressConfigurationWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
/*! \brief   Trigger load data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeIpAddressConfigurationWidget::m_LoadData(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OSCNodeComInterfaceSettings & rc_CurInterface = pc_Node->c_Properties.c_ComInterfaces[this->mu32_ComIf];

      this->mpc_Ui->pc_WidgetIpAddress->SetIPAddress(rc_CurInterface.c_Ip.au8_IpAddress);
      this->mpc_Ui->pc_WidgetSubNetMask->SetIPAddress(rc_CurInterface.c_Ip.au8_NetMask);
      this->mpc_Ui->pc_WidgetDefaultGateway->SetIPAddress(rc_CurInterface.c_Ip.au8_DefaultGateway);

      // directly start typing without cursor move
      this->mpc_Ui->pc_WidgetIpAddress->SetCursorPosition(0);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click

   - write current settings to data. If IP Address is conflicting user may confirm the conflict (error engine will
     show conflict anyways).
   - exit with accept state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeIpAddressConfigurationWidget::m_OkClicked(void)
{
   const bool q_IpValid = C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().CheckIpAddressIsValid(
      this->mu32_NodeIndex, this->mu32_ComIf,
      this->mpc_Ui->pc_WidgetIpAddress->GetIPAddress());
   bool q_WriteToData = true;

   //set invalid text property
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_WidgetIpAddress, "Valid", q_IpValid);

   if (q_IpValid == false)
   {
      C_OgeWiCustomMessage c_Error(this, C_OgeWiCustomMessage::E_Type::eERROR);
      c_Error.SetHeading(C_GtGetText::h_GetText("IP Address Settings"));
      c_Error.SetDescription(C_GtGetText::h_GetText(
                                "IP Address does conflict with another node on the same Ethernet. Are you sure you want to assign this IP Address?"));
      c_Error.SetOKButtonText(C_GtGetText::h_GetText("Continue"));
      c_Error.SetCancelButtonText(C_GtGetText::h_GetText("Cancel"));
      c_Error.SetCustomMinHeight(180, 250);

      if (c_Error.Execute() == C_OgeWiCustomMessage::eCANCEL)
      {
         q_WriteToData = false;
         this->mpc_Ui->pc_WidgetIpAddress->setFocus();
      }
   }

   if (q_WriteToData)
   {
      //write current settings to data
      C_PuiSdHandler::h_GetInstance()->SetOSCNodeEthernetConfiguration(
         this->mu32_NodeIndex, this->mu32_ComIf,
         this->mpc_Ui->pc_WidgetIpAddress->GetIPAddress(),
         this->mpc_Ui->pc_WidgetSubNetMask->GetIPAddress(),
         this->mpc_Ui->pc_WidgetDefaultGateway->GetIPAddress());
      //close dialog after data write
      this->mrc_ParentDialog.accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeIpAddressConfigurationWidget::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}
