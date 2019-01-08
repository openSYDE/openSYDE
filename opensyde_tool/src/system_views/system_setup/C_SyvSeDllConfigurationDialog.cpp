//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for choosing and configuring the CAN communication DLL

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.07.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#include "precomp_headers.h"

#include <QFile>
#include <QFileInfo>

#include "stwtypes.h"
#include "stwerrors.h"

#include "C_SyvSeDllConfigurationDialog.h"
#include "ui_C_SyvSeDllConfigurationDialog.h"

#include "C_Uti.h"
#include "C_GtGetText.h"
#include "CCAN.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_can;

/* -- Module Global Constants ---------------------------------------------- */
const QString C_SyvSeDllConfigurationDialog::mhc_DLL_PATH_PEAK = "STW_dlls\\stwpeak2\\stwpeak2.dll";
const QString C_SyvSeDllConfigurationDialog::mhc_DLL_PATH_VECTOR = "STW_dlls\\stwvec32\\stwvec32.dll";

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_parent Optional pointer to parent

   \created     04.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvSeDllConfigurationDialog::C_SyvSeDllConfigurationDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvSeDllConfigurationDialog),
   mrc_ParentDialog(orc_Parent),
   mu64_Bitrate(0U)
{
   mpc_Ui->setupUi(this);

   this->mrc_ParentDialog.SetWidget(this);

   this->InitText();

   this->m_ShowCustomDllPath(false);

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked,
           this, &C_SyvSeDllConfigurationDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked,
           this, &C_SyvSeDllConfigurationDialog::m_CancelClicked);
   connect(this->mpc_Ui->pc_BushButtonTestConnection, &QPushButton::clicked,
           this, &C_SyvSeDllConfigurationDialog::m_TestConnectionClicked);
   connect(this->mpc_Ui->pc_BushButtonConfigureDll, &QPushButton::clicked,
           this, &C_SyvSeDllConfigurationDialog::m_ConfigureDllClicked);
   connect(this->mpc_Ui->pc_RadioButtonPeak, &stw_opensyde_gui_elements::C_OgeRabProperties::clicked,
           this, &C_SyvSeDllConfigurationDialog::m_ConcretDllClicked);
   connect(this->mpc_Ui->pc_RadioButtonVector, &stw_opensyde_gui_elements::C_OgeRabProperties::clicked,
           this, &C_SyvSeDllConfigurationDialog::m_ConcretDllClicked);
   connect(this->mpc_Ui->pc_RadioButtonOther, &stw_opensyde_gui_elements::C_OgeRabProperties::clicked,
           this, &C_SyvSeDllConfigurationDialog::m_OtherDllClicked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     04.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvSeDllConfigurationDialog::~C_SyvSeDllConfigurationDialog()
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initializes all visible strings on the widget

   \created     04.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::InitText(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("PC CAN Interface"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Configuration"));

   this->mpc_Ui->pc_LabelBusHeading->setText(C_GtGetText::h_GetText("Select Interface"));
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_BushButtonConfigureDll->setText(C_GtGetText::h_GetText("Configure"));
   this->mpc_Ui->pc_BushButtonTestConnection->setText(C_GtGetText::h_GetText("Test Connection"));
   this->mpc_Ui->pc_LabelCustomDllPath->setText(C_GtGetText::h_GetText("DLL path"));
   this->mpc_Ui->pc_LabelBitrateInfo->setText(C_GtGetText::h_GetText(
                                                 "CAN Bitrate will be applied automatically."));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the CAN DLL

   \param[in]     orc_Path       Path of the dll

   \created     05.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::SetDllPath(const QString & orc_Path) const
{
   QString c_UsedPath = orc_Path;

   this->mpc_Ui->pc_LineEditCustomDllPath->setText("");
   this->m_ShowCustomDllPath(false);

   if (c_UsedPath.isEmpty() == true)
   {
      // value not set yet, use peak as default
      c_UsedPath = mhc_DLL_PATH_PEAK;
   }

   if (c_UsedPath == mhc_DLL_PATH_PEAK)
   {
      this->mpc_Ui->pc_RadioButtonPeak->setChecked(true);
   }
   else if (c_UsedPath == mhc_DLL_PATH_VECTOR)
   {
      this->mpc_Ui->pc_RadioButtonVector->setChecked(true);
   }
   else
   {
      this->mpc_Ui->pc_RadioButtonOther->setChecked(true);
      this->m_ShowCustomDllPath(true);
      this->mpc_Ui->pc_LineEditCustomDllPath->setText(c_UsedPath);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   short description of function

   possible return value(s) and description

   \created     05.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_SyvSeDllConfigurationDialog::GetDllPath(void) const
{
   QString c_Path;

   if (this->mpc_Ui->pc_RadioButtonPeak->isChecked() == true)
   {
      c_Path = mhc_DLL_PATH_PEAK;
   }
   else if (this->mpc_Ui->pc_RadioButtonVector->isChecked() == true)
   {
      c_Path = mhc_DLL_PATH_VECTOR;
   }
   else
   {
      c_Path = this->mpc_Ui->pc_LineEditCustomDllPath->text();
   }

   return c_Path;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the bitrate for the test connection in bit/s

   \param[in]     ou64_Bitrate     Bitrate for test connection

   \created     07.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::SetBitrate(const uint64 ou64_Bitrate)
{
   this->mu64_Bitrate = ou64_Bitrate;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information

   \created     02.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
         this->mrc_ParentDialog.accept();
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
   \brief   Slot of OK button click

   \created     16.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_OkClicked(void) const
{
   this->mrc_ParentDialog.accept();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Cancel button

   \created     04.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_CancelClicked(void) const
{
   this->mrc_ParentDialog.reject();
}

//-----------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_ConfigureDllClicked(void) const
{
   const QString c_Path = mh_GetAbsolutePathFromExe(this->GetDllPath());

   if (QFile::exists(c_Path) == true)
   {
      C_CAN c_Can;
      const sint32 s32_Return = c_Can.DLL_Open(c_Path.toStdString().c_str());

      if (s32_Return == C_NO_ERR)
      {
         // let the user configure the DLL
         c_Can.CAN_InteractiveSetup();
      }
      else
      {
         C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eWARNING);
         c_MessageBox.SetHeading(C_GtGetText::h_GetText("PC CAN Interface configuration"));
         c_MessageBox.SetDescription(C_GtGetText::h_GetText("CAN DLL initialization not successful."));
         c_MessageBox.Execute();
      }
      (void)c_Can.DLL_Close();
   }
   else
   {
      C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eWARNING);
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("PC CAN Interface configuration"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("CAN DLL not found."));
      c_MessageBox.Execute();
   }
}

//-----------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_TestConnectionClicked(void) const
{
   // 3 of 4 message cases are of type "failed"
   C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eWARNING);
   QString c_Description;
   const QString c_Path = mh_GetAbsolutePathFromExe(this->GetDllPath());
   const QString c_Heading = C_GtGetText::h_GetText("PC CAN Interface configuration");

   if (QFile::exists(c_Path) == true)
   {
      C_CAN c_Can;
      sint32 s32_Return = c_Can.DLL_Open(c_Path.toStdString().c_str());
      if (s32_Return == C_NO_ERR)
      {
         // Test the CAN
         if (this->mu64_Bitrate > 0U)
         {
            uint64 u64_BitrateKbit = this->mu64_Bitrate / 1000U;
            s32_Return = c_Can.CAN_Init(static_cast<sint32>(u64_BitrateKbit));
         }
         else
         {
            s32_Return = c_Can.CAN_Init();
         }

         if (s32_Return == C_NO_ERR)
         {
            c_MessageBox.SetType(C_OgeWiCustomMessage::E_Type::eINFORMATION);
            c_Description = C_GtGetText::h_GetText("Connection test successful. CAN Interface is ready for use.");
         }
         else
         {
            c_Description = C_GtGetText::h_GetText("CAN bus initialization not successful: could not initialize bus.");
         }
         (void)c_Can.CAN_Exit();
      }
      else
      {
         c_Description = C_GtGetText::h_GetText("CAN DLL initialization not successful: could not open DLL.");
      }
      (void)c_Can.DLL_Close();
   }
   else
   {
      c_Description = C_GtGetText::h_GetText("CAN DLL not found.");
   }

   // Show the result
   c_MessageBox.SetHeading(c_Heading);
   c_MessageBox.SetDescription(c_Description);
   c_MessageBox.Execute();
}

//-----------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_ConcretDllClicked(void) const
{
   this->m_ShowCustomDllPath(false);
}

//-----------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_OtherDllClicked(void) const
{
   this->m_ShowCustomDllPath(true);
}

//-----------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_ShowCustomDllPath(const bool oq_Active) const
{
   this->mpc_Ui->pc_LabelCustomDllPath->setVisible(oq_Active);
   this->mpc_Ui->pc_LineEditCustomDllPath->setVisible(oq_Active);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Always get absolute path

   \param[in] orc_Path Absolute or relative path

   \return
   Absolute path

   \created     07.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_SyvSeDllConfigurationDialog::mh_GetAbsolutePathFromExe(const QString & orc_Path)
{
   QString c_Retval;
   const QFileInfo c_Info(orc_Path);

   if (c_Info.isRelative() == true)
   {
      c_Retval = C_Uti::h_GetExePath() + '\\' + orc_Path;
   }
   else
   {
      c_Retval = orc_Path;
   }
   return c_Retval;
}
