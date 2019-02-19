//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for IP Address configuration (implementation)

   Widget for IP Address configuration

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.02.2018  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeIpAddressConfigurationWidget.h"
#include "TGLUtils.h"
#include "ui_C_SdNdeIpAddressConfigurationWidget.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent Optional pointer to parent
   \param[in]     ou32_Index Node index
   \param[in]     ou32_ComIf Node COM Interface Number

   \created     26.02.2018  STW/S.Singer
*/
//-----------------------------------------------------------------------------
C_SdNdeIpAddressConfigurationWidget::C_SdNdeIpAddressConfigurationWidget(
   stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32 ou32_NodeIndex, const uint32 ou32_ComIf) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeIpAddressConfigurationWidget),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(ou32_NodeIndex),
   mu32_ComIf(ou32_ComIf)
{
   //set validator for ip address (source:
   // https://stackoverflow.com/questions/23166283/how-to-set-input-mask-and-qvalidator-to-a-qlineedit-at-a-time-in-qt
   // https://forum.qt.io/topic/83468/inputmask-and-validator-not-working-together/3)
   const QString c_Range =
      "(([ 0]+)|"                                     // only spaces or zeros
      "([ 0]*[0-9] *)|"                               // one digit, e.g. "9  " or " 08" or " 1 "; but not " 10"
      "([0-9][0-9] )|([ 0][0-9][0-9])|([0-9] [0-9])|" // two digits, e.g. "99 " or " 25" or "087" or "8 3"
      "(1[0-9][0-9])|([2][0-4][0-9])|(25[0-5]))";     // three digits, e.g. "255" or "179"; but not "678"
   const QRegularExpression c_Regex("^" + c_Range +
                                    "\\." + c_Range +
                                    "\\." + c_Range +
                                    "\\." + c_Range + "$");

   mpc_Ui->setupUi(this);

   this->mpc_Validator = new QRegularExpressionValidator(c_Regex, this);
   this->mpc_Ui->pc_LineEditIpAddress->setValidator(mpc_Validator);
   this->mpc_Ui->pc_LineEditSubNetMask->setValidator(mpc_Validator);
   this->mpc_Ui->pc_LineEditDefaultGateway->setValidator(mpc_Validator);

   // use a mask in addition to the validator
   this->mpc_Ui->pc_LineEditIpAddress->setInputMask("000.000.000.000");
   this->mpc_Ui->pc_LineEditSubNetMask->setInputMask("000.000.000.000");
   this->mpc_Ui->pc_LineEditDefaultGateway->setInputMask("000.000.000.000");

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   this->mrc_ParentDialog.SetTitle(QString(C_GtGetText::h_GetText("IP Address")));
   this->mrc_ParentDialog.SetSubTitle(QString(C_GtGetText::h_GetText("Settings")));

   //Init
   InitStaticNames();

   m_LoadData();

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this,
           &C_SdNdeIpAddressConfigurationWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked, this,
           &C_SdNdeIpAddressConfigurationWidget::m_CancelClicked);

   connect(this->mpc_Ui->pc_LineEditSubNetMask, &C_OgeLeFilePath::SigFocus, this,
           &C_SdNdeIpAddressConfigurationWidget::m_OnSubNetMaskFocusIn);
   connect(this->mpc_Ui->pc_LineEditSubNetMask, &C_OgeLeFilePath::SigFocusOut, this,
           &C_SdNdeIpAddressConfigurationWidget::m_OnSubNetMaskFocusOut);
   connect(this->mpc_Ui->pc_LineEditDefaultGateway, &C_OgeLeFilePath::SigFocus, this,
           &C_SdNdeIpAddressConfigurationWidget::m_OnGatewayMaskFocusIn);
   connect(this->mpc_Ui->pc_LineEditDefaultGateway, &C_OgeLeFilePath::SigFocusOut, this,
           &C_SdNdeIpAddressConfigurationWidget::m_OnGatewayMaskFocusOut);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     26.02.2018  STW/S.Singer
*/
//-----------------------------------------------------------------------------
C_SdNdeIpAddressConfigurationWidget::~C_SdNdeIpAddressConfigurationWidget(void)
{
   delete mpc_Ui;
   delete mpc_Validator;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     26.02.2018  STW/S.Singer
*/
//-----------------------------------------------------------------------------
void C_SdNdeIpAddressConfigurationWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LabelDataElement->setText(C_GtGetText::h_GetText("IP Address Settings"));
   this->mpc_Ui->pc_LabelIpAddress->setText(C_GtGetText::h_GetText("IP Address"));
   this->mpc_Ui->pc_LabelSubNetMask->setText(C_GtGetText::h_GetText("Sub Net Mask"));
   this->mpc_Ui->pc_LabelDefaultGateway->setText(C_GtGetText::h_GetText("Default Gateway"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information

   \created     02.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger load data

   \created     26.02.2018  STW/S.Singer
*/
//-----------------------------------------------------------------------------
void C_SdNdeIpAddressConfigurationWidget::m_LoadData(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   QString c_HelpString;

   if (pc_Node != NULL)
   {
      const C_OSCNodeComInterfaceSettings & rc_CurInterface = pc_Node->c_Properties.c_ComInterfaces[this->mu32_ComIf];
      // set ip address (padded with blanks)
      c_HelpString = "";
      c_HelpString.append(QString("%1").arg(rc_CurInterface.c_Ip.au8_IpAddress[0], 3, 10, QChar(' ')));
      c_HelpString.append(".");
      c_HelpString.append(QString("%1").arg(rc_CurInterface.c_Ip.au8_IpAddress[1], 3, 10, QChar(' ')));
      c_HelpString.append(".");
      c_HelpString.append(QString("%1").arg(rc_CurInterface.c_Ip.au8_IpAddress[2], 3, 10, QChar(' ')));
      c_HelpString.append(".");
      c_HelpString.append(QString("%1").arg(rc_CurInterface.c_Ip.au8_IpAddress[3], 3, 10, QChar(' ')));

      // show default with spaces instead of zeros
      // this is done with IP address for better input experience, but for subnetmask and gateway the lineedit
      // is also for showing the value and therefore should really show the zeros
      if (c_HelpString == "  0.  0.  0.  0")
      {
         c_HelpString = "";
      }

      this->mpc_Ui->pc_LineEditIpAddress->setText(c_HelpString);
      this->mpc_Ui->pc_LineEditIpAddress->setCursorPosition(0); // directly start typing without cursor move

      //set sub net mask (padded with blanks)
      c_HelpString = "";
      c_HelpString.append(QString("%1").arg(rc_CurInterface.c_Ip.au8_NetMask[0], 3, 10, QChar(' ')));
      c_HelpString.append(".");
      c_HelpString.append(QString("%1").arg(rc_CurInterface.c_Ip.au8_NetMask[1], 3, 10, QChar(' ')));
      c_HelpString.append(".");
      c_HelpString.append(QString("%1").arg(rc_CurInterface.c_Ip.au8_NetMask[2], 3, 10, QChar(' ')));
      c_HelpString.append(".");
      c_HelpString.append(QString("%1").arg(rc_CurInterface.c_Ip.au8_NetMask[3], 3, 10, QChar(' ')));

      this->mpc_Ui->pc_LineEditSubNetMask->setText(c_HelpString);

      //set default gateway (padded with blanks)
      c_HelpString = "";
      c_HelpString.append(QString("%1").arg(rc_CurInterface.c_Ip.au8_DefaultGateway[0], 3, 10, QChar(' ')));
      c_HelpString.append(".");
      c_HelpString.append(QString("%1").arg(rc_CurInterface.c_Ip.au8_DefaultGateway[1], 3, 10, QChar(' ')));
      c_HelpString.append(".");
      c_HelpString.append(QString("%1").arg(rc_CurInterface.c_Ip.au8_DefaultGateway[2], 3, 10, QChar(' ')));
      c_HelpString.append(".");
      c_HelpString.append(QString("%1").arg(rc_CurInterface.c_Ip.au8_DefaultGateway[3], 3, 10, QChar(' ')));

      this->mpc_Ui->pc_LineEditDefaultGateway->setText(c_HelpString);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Ok button click

   - write current settings to data
   - exit with accept state

   \created     26.02.2018  STW/S.Singer
*/
//-----------------------------------------------------------------------------
void C_SdNdeIpAddressConfigurationWidget::m_OkClicked(void)
{
   QString c_HelpString;
   QStringList c_Parts;

   // set the IP address string with blanks only to 0.0.0.0
   if (this->mpc_Ui->pc_LineEditIpAddress->text() == "...")
   {
      this->mpc_Ui->pc_LineEditIpAddress->setText("  0.  0.  0.  0");
   }

   if (this->mq_VerifyInput() == true)
   {
      //write current settings to data
      std::vector<sint32> c_Ip;
      std::vector<sint32> c_NetMask;
      std::vector<sint32> c_DefaultGateway;

      //write ip address
      c_HelpString = this->mpc_Ui->pc_LineEditIpAddress->text();
      c_Parts = c_HelpString.split(".", QString::SkipEmptyParts);
      tgl_assert(c_Parts.size() == 4);
      if (c_Parts.size() == 4)
      {
         c_Ip.push_back(c_Parts[0].toInt());
         c_Ip.push_back(c_Parts[1].toInt());
         c_Ip.push_back(c_Parts[2].toInt());
         c_Ip.push_back(c_Parts[3].toInt());
      }

      //write sub net mask
      c_HelpString = this->mpc_Ui->pc_LineEditSubNetMask->text();
      c_Parts = c_HelpString.split(".", QString::SkipEmptyParts);
      tgl_assert(c_Parts.size() == 4);
      if (c_Parts.size() == 4)
      {
         c_NetMask.push_back(c_Parts[0].toInt());
         c_NetMask.push_back(c_Parts[1].toInt());
         c_NetMask.push_back(c_Parts[2].toInt());
         c_NetMask.push_back(c_Parts[3].toInt());
      }

      //write default gateway
      c_HelpString = this->mpc_Ui->pc_LineEditDefaultGateway->text();
      c_Parts = c_HelpString.split(".", QString::SkipEmptyParts);
      tgl_assert(c_Parts.size() == 4);
      if (c_Parts.size() == 4)
      {
         c_DefaultGateway.push_back(c_Parts[0].toInt());
         c_DefaultGateway.push_back(c_Parts[1].toInt());
         c_DefaultGateway.push_back(c_Parts[2].toInt());
         c_DefaultGateway.push_back(c_Parts[3].toInt());
      }

      //save new node
      C_PuiSdHandler::h_GetInstance()->SetOSCNodeEthernetConfiguration(this->mu32_NodeIndex, this->mu32_ComIf, c_Ip,
                                                                       c_NetMask, c_DefaultGateway);

      //close dialog after data write
      this->mrc_ParentDialog.accept();
   }
   else
   {
      //invalid input
      //don't write data
      //don't exit dialog
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Cancel button

   \created     26.02.2018  STW/S.Singer
*/
//-----------------------------------------------------------------------------
void C_SdNdeIpAddressConfigurationWidget::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Verify ip address and sub net mask input

   \return
     true:   no errors
     false:  invalid input detected

   \created     26.02.2018  STW/S.Singer
*/
//-----------------------------------------------------------------------------
bool C_SdNdeIpAddressConfigurationWidget::mq_VerifyInput() const
{
   C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eERROR);
   QString c_HelpString;
   bool q_InputIsValid = true;
   QStringList c_Parts;

   // message box heading
   c_MessageBox.SetHeading(C_GtGetText::h_GetText("IP address setting"));

   //check ip address text
   c_HelpString = this->mpc_Ui->pc_LineEditIpAddress->text();
   c_Parts = c_HelpString.split(".", QString::SkipEmptyParts);

   if (c_Parts.size() != 4)
   {
      q_InputIsValid = false;

      //show message box
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("Invalid IP address. Check your input and retry."));
      c_MessageBox.Execute();
      this->mpc_Ui->pc_LineEditIpAddress->setFocus();
      this->mpc_Ui->pc_LineEditIpAddress->selectAll();
   }

   //check sub net mask text (only if no error detected so far)
   if (q_InputIsValid == true)
   {
      c_HelpString = this->mpc_Ui->pc_LineEditSubNetMask->text();
      c_Parts = c_HelpString.split(".", QString::SkipEmptyParts);

      if (c_Parts.size() != 4)
      {
         q_InputIsValid = false;

         //show message box

         c_MessageBox.SetDescription(C_GtGetText::h_GetText("Invalid sub net mask. Check your input and retry."));
         c_MessageBox.Execute();
         this->mpc_Ui->pc_LineEditSubNetMask->setFocus();
         this->mpc_Ui->pc_LineEditSubNetMask->selectAll();
      }
   }

   //check default gateway text (only if no error detected so far)
   if (q_InputIsValid == true)
   {
      c_HelpString = this->mpc_Ui->pc_LineEditDefaultGateway->text();
      c_Parts = c_HelpString.split(".", QString::SkipEmptyParts);

      if (c_Parts.size() != 4)
      {
         q_InputIsValid = false;

         //show message box

         c_MessageBox.SetDescription(C_GtGetText::h_GetText("Invalid default gateway. Check your input and retry."));
         c_MessageBox.Execute();
         this->mpc_Ui->pc_LineEditDefaultGateway->setFocus();
         this->mpc_Ui->pc_LineEditDefaultGateway->selectAll();
      }
   }

   return q_InputIsValid;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot to show empty sub net mask on editing instead of "  0.  0.  0.  0."

   \created     06.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_SdNdeIpAddressConfigurationWidget::m_OnSubNetMaskFocusIn(void) const
{
   if ((this->mpc_Ui->pc_LineEditSubNetMask->isReadOnly() == false) &&
       (this->mpc_Ui->pc_LineEditSubNetMask->text() == "0.0.0.0"))
   {
      this->mpc_Ui->pc_LineEditSubNetMask->clear();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot to replace empty strings with 0 in sub net mask.

   \created     06.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_SdNdeIpAddressConfigurationWidget::m_OnSubNetMaskFocusOut(void) const
{
   QStringList c_Parts = this->mpc_Ui->pc_LineEditSubNetMask->text().split(".");

   // replace *..* with *.  0.*
   for (sint32 s32_Pos = 0; s32_Pos < c_Parts.size(); s32_Pos++)
   {
      c_Parts[s32_Pos] = c_Parts[s32_Pos].rightJustified(3, ' ');
   }
   this->mpc_Ui->pc_LineEditSubNetMask->setText(c_Parts.join("."));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot to show empty default gateway on editing instead of "  0.  0.  0.  0."

   \created     06.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_SdNdeIpAddressConfigurationWidget::m_OnGatewayMaskFocusIn(void) const
{
   if ((this->mpc_Ui->pc_LineEditDefaultGateway->isReadOnly() == false) &&
       (this->mpc_Ui->pc_LineEditDefaultGateway->text() == "0.0.0.0"))
   {
      this->mpc_Ui->pc_LineEditDefaultGateway->clear();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief    Slot to replace empty strings with 0 in default gateway.

   \created     06.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_SdNdeIpAddressConfigurationWidget::m_OnGatewayMaskFocusOut(void) const
{
   QStringList c_Parts = this->mpc_Ui->pc_LineEditDefaultGateway->text().split(".");

   // replace *..* with *.  0.*, *.2.* with *.  2.* and *.23.* with *. 23.*
   for (sint32 s32_Pos = 0; s32_Pos < c_Parts.size(); s32_Pos++)
   {
      c_Parts[s32_Pos] = c_Parts[s32_Pos].rightJustified(3, ' ');
   }
   this->mpc_Ui->pc_LineEditDefaultGateway->setText(c_Parts.join("."));
}
