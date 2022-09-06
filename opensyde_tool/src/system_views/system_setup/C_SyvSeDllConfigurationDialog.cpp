//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for choosing and configuring the CAN communication DLL

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#include "precomp_headers.h"

#include <QFile>
#include <QFileInfo>
#include <QFileDialog>

#include "stwtypes.h"
#include "stwerrors.h"

#include "C_SyvSeDllConfigurationDialog.h"
#include "ui_C_SyvSeDllConfigurationDialog.h"

#include "C_OSCUtils.h"
#include "C_Uti.h"
#include "C_GtGetText.h"
#include "CCAN.h"
#include "C_OgeWiCustomMessage.h"
#include "C_ImpUtil.h"
#include "C_PuiUtil.h"
#include "C_OgeWiUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;
using namespace stw_can;

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
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvSeDllConfigurationDialog::C_SyvSeDllConfigurationDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvSeDllConfigurationDialog),
   mrc_ParentDialog(orc_Parent),
   mu64_Bitrate(0U)
{
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_LineEditCustomDllPath->SetDragAndDropActiveForFile("dll");

   this->mrc_ParentDialog.SetWidget(this);

   this->InitText();

   this->m_ShowCustomDllPath(false);

   // Remove "..." and "ABC" string
   this->mpc_Ui->pc_PushButtonBrowse->setText("");
   this->mpc_Ui->pc_PushButtonVariables->setText("");

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
   connect(this->mpc_Ui->pc_PushButtonBrowse, &C_OgePubOpen::clicked,
           this, &C_SyvSeDllConfigurationDialog::m_OnBrowse);
   connect(this->mpc_Ui->pc_PushButtonVariables, &C_OgePubPathVariables::SigVariableSelected,
           this->mpc_Ui->pc_LineEditCustomDllPath, &C_OgeLeFilePath::InsertVariable);
   connect(this->mpc_Ui->pc_LineEditCustomDllPath, &C_OgeLeFilePath::SigPathDropped,
           this, &C_SyvSeDllConfigurationDialog::m_OnDroppedDllPath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvSeDllConfigurationDialog::~C_SyvSeDllConfigurationDialog()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
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
                                                 "CAN bitrate will be applied automatically."));
   this->mpc_Ui->pc_RadioButtonOther->setText(C_GtGetText::h_GetText("Other"));

   this->mpc_Ui->pc_PushButtonBrowse->SetToolTipInformation(
      C_GtGetText::h_GetText("Browse"),
      C_GtGetText::h_GetText("Browse for custom DLL file."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the CAN DLL type.

   \param[in]     oe_Type       CAN DLL type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::SetDllType(const C_PuiSvPc::E_CANDllType oe_Type) const
{
   // toggle radio button depending on type
   switch (oe_Type)
   {
   case C_PuiSvPc::ePEAK:
      // PEAK
      this->mpc_Ui->pc_RadioButtonPeak->setChecked(true);
      this->m_ShowCustomDllPath(false);
      break;
   case C_PuiSvPc::eVECTOR:
      // Vector
      this->mpc_Ui->pc_RadioButtonVector->setChecked(true);
      this->m_ShowCustomDllPath(false);
      break;
   case C_PuiSvPc::eOTHER:
      // Other
      this->mpc_Ui->pc_RadioButtonOther->setChecked(true);
      this->m_ShowCustomDllPath(true);
      break;
   default:
      // Default is PEAK
      this->mpc_Ui->pc_RadioButtonPeak->setChecked(true);
      this->m_ShowCustomDllPath(false);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the custom CAN DLL path.

   \param[in]     orc_Path       Path of the CAN DLL
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::SetCustomDllPath(const QString & orc_Path) const
{
   // set line edit text to last known custom path
   this->mpc_Ui->pc_LineEditCustomDllPath->SetPath(orc_Path, C_Uti::h_GetExePath());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the bitrate for the test connection in bit/s

   \param[in]     ou64_Bitrate     Bitrate for test connection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::SetBitrate(const uint64 ou64_Bitrate)
{
   this->mu64_Bitrate = ou64_Bitrate;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get CAN DLL type.

   \return     CAN DLL type (PEAK/VECTOR/Other)
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvPc::E_CANDllType C_SyvSeDllConfigurationDialog::GetDllType(void) const
{
   C_PuiSvPc::E_CANDllType e_Type;

   if (this->mpc_Ui->pc_RadioButtonVector->isChecked() == true)
   {
      e_Type = C_PuiSvPc::E_CANDllType::eVECTOR;
   }
   else if (this->mpc_Ui->pc_RadioButtonOther->isChecked() == true)
   {
      e_Type = C_PuiSvPc::E_CANDllType::eOTHER;
   }
   else
   {
      e_Type = C_PuiSvPc::E_CANDllType::ePEAK;
   }

   return e_Type;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get custom CAN DLL path.

   \return     path of custom CAN DLL
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvSeDllConfigurationDialog::GetCustomDllPath(void) const
{
   QString c_Path;

   c_Path = this->mpc_Ui->pc_LineEditCustomDllPath->GetPath();

   return c_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of OK button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_OkClicked(void) const
{
   if (this->m_CheckCustomDllPath() == true)
   {
      this->mrc_ParentDialog.accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_CancelClicked(void) const
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_ConfigureDllClicked(void) const
{
   // check path for invalid signs for custom DLL
   if (this->m_CheckCustomDllPath() == true)
   {
      const QString c_Path = this->m_GetAbsoluteDllPath();

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
            c_MessageBox.SetCustomMinHeight(180, 180);
            c_MessageBox.Execute();
         }
         (void)c_Can.DLL_Close();
      }
      else
      {
         C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eWARNING);
         c_MessageBox.SetHeading(C_GtGetText::h_GetText("PC CAN Interface configuration"));
         c_MessageBox.SetDescription(C_GtGetText::h_GetText("CAN DLL not found."));
         c_MessageBox.SetCustomMinHeight(180, 180);
         c_MessageBox.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_TestConnectionClicked(void) const
{
   // check path for invalid signs for custom DLL
   if (this->m_CheckCustomDllPath() == true)
   {
      // 3 of 4 message cases are of type "failed"
      C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eWARNING);
      QString c_Description;
      const QString c_Path = this->m_GetAbsoluteDllPath();
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
               const uint64 u64_BitrateKbit = this->mu64_Bitrate / 1000U;
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
               c_Description =
                  C_GtGetText::h_GetText("CAN bus initialization not successful: could not initialize bus.");
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
      c_MessageBox.SetCustomMinHeight(180, 180);
      c_MessageBox.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_ConcretDllClicked(void) const
{
   this->m_ShowCustomDllPath(false);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_OtherDllClicked(void) const
{
   this->m_ShowCustomDllPath(true);
   this->mpc_Ui->pc_LineEditCustomDllPath->setFocus();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for browse button click.

   Browse for CAN DLL.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_OnBrowse(void) const
{
   const QString c_Folder = C_PuiUtil::h_GetResolvedAbsPathFromExe(this->mpc_Ui->pc_LineEditCustomDllPath->GetPath());
   const QString c_Filter = static_cast<QString>(C_GtGetText::h_GetText("CAN DLL ")) + "(*.dll)";
   QFileDialog c_Dialog(this->parentWidget(), C_GtGetText::h_GetText("Select CAN DLL"), c_Folder, c_Filter);

   c_Dialog.setDefaultSuffix(".dll");

   if (c_Dialog.exec() == static_cast<sintn>(QDialog::Accepted))
   {
      const QString c_Path = c_Dialog.selectedFiles().at(0);

      if (c_Path != "")
      {
         this->m_SetCustomDllPath(c_Path);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle a dropped folder path in line edit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_OnDroppedDllPath(void)
{
   this->m_SetCustomDllPath(this->mpc_Ui->pc_LineEditCustomDllPath->GetPath());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setter for full create in path.

   \param[in] orc_New New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_SetCustomDllPath(const QString & orc_New) const
{
   // check if relative path is possible and appreciated
   const QString c_Path = C_ImpUtil::h_AskUserToSaveRelativePath(this->parentWidget(), orc_New, C_Uti::h_GetExePath());

   if (c_Path != "")
   {
      this->mpc_Ui->pc_LineEditCustomDllPath->SetPath(c_Path, C_Uti::h_GetExePath());
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeDllConfigurationDialog::m_ShowCustomDllPath(const bool oq_Active) const
{
   this->mpc_Ui->pc_LabelCustomDllPath->setVisible(oq_Active);
   this->mpc_Ui->pc_LineEditCustomDllPath->setVisible(oq_Active);
   this->mpc_Ui->pc_PushButtonBrowse->setVisible(oq_Active);
   this->mpc_Ui->pc_PushButtonVariables->setVisible(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  If custom DLL is selected check if path contains invalid characters
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvSeDllConfigurationDialog::m_CheckCustomDllPath(void) const
{
   bool q_Return = true;

   if (this->GetDllType() == C_PuiSvPc::eOTHER)
   {
      const QString c_ResolvedPath =  C_PuiUtil::h_ResolvePlaceholderVariables(this->GetCustomDllPath());
      if (c_ResolvedPath.isEmpty() == true)
      {
         C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eERROR);
         c_MessageBox.SetHeading(C_GtGetText::h_GetText("PC CAN Interface configuration"));
         c_MessageBox.SetDescription(C_GtGetText::h_GetText("CAN DLL path is empty. Please choose a valid path."));
         c_MessageBox.SetCustomMinHeight(180, 180);
         c_MessageBox.Execute();
         q_Return = false;
      }
      else
      {
         if (C_OSCUtils::h_CheckValidFilePath(c_ResolvedPath.toStdString().c_str()) == false)
         {
            C_OgeWiUtil::h_ShowPathInvalidError(this->parentWidget(), c_ResolvedPath);
            q_Return = false;
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the absolute CAN DLL path of currently selected DLL.

   \return
   Absolute CAN DLL path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvSeDllConfigurationDialog::m_GetAbsoluteDllPath(void) const
{
   QString c_Return;

   switch (this->GetDllType())
   {
   case C_PuiSvPc::ePEAK:
      c_Return = stw_opensyde_gui::mc_DLL_PATH_PEAK;
      break;
   case C_PuiSvPc::eVECTOR:
      c_Return = stw_opensyde_gui::mc_DLL_PATH_VECTOR;
      break;
   case C_PuiSvPc::eOTHER:
      c_Return = this->GetCustomDllPath();
      break;
   default:
      c_Return = stw_opensyde_gui::mc_DLL_PATH_PEAK;
      break;
   }

   // resolve variables and make absolute if it is relative (only if not empty)
   if (c_Return.isEmpty() == false)
   {
      c_Return = C_PuiUtil::h_GetResolvedAbsPathFromExe(c_Return);
   }
   return c_Return;
}
