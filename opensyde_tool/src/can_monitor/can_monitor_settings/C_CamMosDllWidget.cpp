//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling CAN DLL configuration
 (implementation)

   Widget for handling CAN DLL configuration

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFileInfo>

#include "C_CamMosDllWidget.h"
#include "ui_C_CamMosDllWidget.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "CCAN.h"
#include "C_OSCUtils.h"
#include "C_GtGetText.h"
#include "C_OgeWiCustomMessage.h"
#include "C_Uti.h"
#include "C_CamProHandler.h"
#include "cam_constants.h"
#include "C_OgeWiUtil.h"
#include "C_UsHandler.h"
#include "C_CamUti.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosDllWidget::C_CamMosDllWidget(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamMosDllWidget)
{
   this->mpc_Ui->setupUi(this);

   this->m_InitUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosDllWidget::~C_CamMosDllWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load all user settings.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidget::LoadUserSettings(void) const
{
   this->mpc_Ui->pc_WiHeader->SetExpanded(C_UsHandler::h_GetInstance()->GetWiDllConfigExpanded());
   this->m_OnExpand(C_UsHandler::h_GetInstance()->GetWiDllConfigExpanded());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare widget for menu view in collapsed state resp. expanded view.

   \param[in]     oq_Expand       true: prepare for expanded, false: prepare for collapsed menu
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidget::PrepareForExpanded(const bool oq_Expand) const
{
   if (oq_Expand == false)
   {
      this->mpc_Ui->pc_WiContent->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_WiContent->setVisible(C_UsHandler::h_GetInstance()->GetWiDllConfigExpanded());
   }

   // hide arrow button
   this->mpc_Ui->pc_WiHeader->ShowExpandButton(oq_Expand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for communication change signal.

   \param[in]  oq_Online    Online/offline flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidget::OnCommunicationStarted(const bool oq_Online) const
{
   this->mpc_Ui->pc_RadioButtonOther->setDisabled(oq_Online);
   this->mpc_Ui->pc_RadioButtonPeak->setDisabled(oq_Online);
   this->mpc_Ui->pc_RadioButtonVector->setDisabled(oq_Online);
   this->mpc_Ui->pc_WidgetCustomDll->setDisabled(oq_Online);
   this->mpc_Ui->pc_PushButtonConfigure->setDisabled(oq_Online);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize GUI elements
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidget::m_InitUi(void)
{
   // initialize background color
   this->SetBackgroundColor(5);

   // initialize title widget
   this->mpc_Ui->pc_WiHeader->SetTitle(C_GtGetText::h_GetText("PC CAN Interface Configuration"));
   this->mpc_Ui->pc_WiHeader->SetIcon("://images/IconConfig.svg");
   this->mpc_Ui->pc_WiHeader->SetToggle(false);

   // initialize text
   this->mpc_Ui->pc_PushButtonConfigure->setText(C_GtGetText::h_GetText("Configure Interface"));
   this->mpc_Ui->pc_PushButtonConfigure->SetToolTipInformation(C_GtGetText::h_GetText("Configure Interface"),
                                                               C_GtGetText::h_GetText("Configure CAN DLL interface."));
   this->mpc_Ui->pc_LabelCustomDllPath->setText(C_GtGetText::h_GetText("DLL path"));
   this->mpc_Ui->pc_RadioButtonOther->setText(C_GtGetText::h_GetText("Other"));
   this->mpc_Ui->pc_PushButtonBrowse->setText("");
   this->mpc_Ui->pc_PushButtonVariables->setText("");

   // initialize button icon
   QIcon c_Icon;
   c_Icon.addFile("://images/IconEditHovered.svg", QSize(), QIcon::Normal);
   c_Icon.addFile("://images/IconEditDisabled.svg", QSize(), QIcon::Disabled);
   this->mpc_Ui->pc_PushButtonConfigure->setIconSize(QSize(16, 16));
   this->mpc_Ui->pc_PushButtonConfigure->setIcon(c_Icon);

   // initialize label
   this->mpc_Ui->pc_LabelCustomDllPath->SetForegroundColor(0);
   this->mpc_Ui->pc_LabelCustomDllPath->SetFontPixel(13);

   // initialize color of separator
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameTop, "HasColor8Background", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameBottom, "HasColor8Background", true);

   // initialize radio buttons style
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_RadioButtonPeak, "DarkBackground", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_RadioButtonVector, "DarkBackground", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_RadioButtonOther, "DarkBackground", true);

   // hide line edit
   this->mpc_Ui->pc_WidgetCustomDll->setVisible(false);

   // connects
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigExpandSection,
           this, &C_CamMosDllWidget::m_OnExpand);
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigHide,
           this, &C_CamMosDllWidget::SigHide);

   // behavior of DLL selection (Other vs. PEAK and Vector)
   connect(this->mpc_Ui->pc_RadioButtonPeak, &stw_opensyde_gui_elements::C_OgeRabProperties::clicked,
           this, &C_CamMosDllWidget::m_ConcreteDllClicked);
   connect(this->mpc_Ui->pc_RadioButtonVector, &stw_opensyde_gui_elements::C_OgeRabProperties::clicked,
           this, &C_CamMosDllWidget::m_ConcreteDllClicked);
   connect(this->mpc_Ui->pc_RadioButtonOther, &stw_opensyde_gui_elements::C_OgeRabProperties::clicked,
           this, &C_CamMosDllWidget::m_OtherDllClicked);
   connect(this->mpc_Ui->pc_LineEditCustomDllPath, &C_CamOgeLeFilePath::editingFinished, this,
           &C_CamMosDllWidget::m_OnCustomDllEdited);

   // path actions
   connect(this->mpc_Ui->pc_PushButtonBrowse, &C_CamOgePubDarkBrowse::clicked, this, &C_CamMosDllWidget::m_OnBrowse);
   connect(this->mpc_Ui->pc_PushButtonVariables, &C_CamOgePubPathVariables::SigVariableSelected,
           this, &C_CamMosDllWidget::m_InsertPathVar);

   // button configure
   connect(this->mpc_Ui->pc_PushButtonConfigure, &QPushButton::clicked,
           this, &C_CamMosDllWidget::m_ConfigureDllClicked);

   // configuration load
   connect(C_CamProHandler::h_GetInstance(), &C_CamProHandler::SigNewConfiguration,
           this, &C_CamMosDllWidget::m_LoadConfig);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load configuration (i.e. DLL paths)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidget::m_LoadConfig(void) const
{
   const C_CamProHandler::E_CANDllType e_CANDllType = C_CamProHandler::h_GetInstance()->GetCANDllType();

   // check radio button depending on type
   switch (e_CANDllType)
   {
   case C_CamProHandler::ePEAK:
      // PEAK
      this->mpc_Ui->pc_RadioButtonPeak->setChecked(true);
      this->mpc_Ui->pc_WidgetCustomDll->setVisible(false);
      break;
   case C_CamProHandler::eVECTOR:
      // Vector
      this->mpc_Ui->pc_RadioButtonVector->setChecked(true);
      this->mpc_Ui->pc_WidgetCustomDll->setVisible(false);
      break;
   case C_CamProHandler::eOTHER:
      // Other
      this->mpc_Ui->pc_RadioButtonOther->setChecked(true);
      this->mpc_Ui->pc_WidgetCustomDll->setVisible(true);
      break;
   default:
      // Default is PEAK
      this->mpc_Ui->pc_RadioButtonPeak->setChecked(true);
      this->mpc_Ui->pc_WidgetCustomDll->setVisible(false);
      break;
   }

   // set line edit text to last known custom path
   QString c_Path = C_CamProHandler::h_GetInstance()->GetCustomCANDllPath();
   if (c_Path == "")
   {
      this->mpc_Ui->pc_LineEditCustomDllPath->SetPath("");
   }
   else
   {
      this->mpc_Ui->pc_LineEditCustomDllPath->SetPath(c_Path, C_Uti::h_GetExePath());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of configure DLL button.

   Open CAN DLL and try to call interactive setup.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidget::m_ConfigureDllClicked(void)
{
   QString c_Path;

   // Get absolute DLL path (resolve variables and make absolute if it is relative ant not empty)
   c_Path = C_CamProHandler::h_GetInstance()->GetCANDllPath();
   if (c_Path.isEmpty() == false)
   {
      c_Path = C_CamUti::h_GetResolvedAbsolutePathFromExe(c_Path);
   }

   if (QFile::exists(c_Path) == true)
   {
      C_CAN c_Can;
      const sint32 s32_Return = c_Can.DLL_Open(c_Path.toStdString().c_str());

      if (s32_Return == C_NO_ERR)
      {
         // let the user configure the DLL
         c_Can.CAN_InteractiveSetup();

         // inform about possible changes
         Q_EMIT (this->SigCANDllConfigured());
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of radio buttons PEAK and Vector.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidget::m_ConcreteDllClicked(void) const
{
   this->mpc_Ui->pc_WidgetCustomDll->setVisible(false);
   this->m_UpdateCANDllPath();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for other DLL radio button.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidget::m_OtherDllClicked(void) const
{
   this->mpc_Ui->pc_WidgetCustomDll->setVisible(true);
   if (this->mpc_Ui->pc_LineEditCustomDllPath->GetPath().isEmpty() == true)
   {
      this->mpc_Ui->pc_LineEditCustomDllPath->setFocus();
   }
   this->m_UpdateCANDllPath();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new CAN DLL path in data handling.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidget::m_UpdateCANDllPath(void) const
{
   C_CamProHandler::E_CANDllType e_Type;

   if (this->mpc_Ui->pc_RadioButtonVector->isChecked() == true)
   {
      e_Type = C_CamProHandler::E_CANDllType::eVECTOR;
   }
   else if (this->mpc_Ui->pc_RadioButtonOther->isChecked() == true)
   {
      QString c_Path = this->mpc_Ui->pc_LineEditCustomDllPath->GetPath();

      // remember path in data handling
      C_CamProHandler::h_GetInstance()->SetCustomCANDllPath(c_Path);

      // set type
      e_Type = C_CamProHandler::E_CANDllType::eOTHER;
   }
   else
   {
      e_Type = C_CamProHandler::E_CANDllType::ePEAK;
   }

   // set CAN DLL type in data handling
   C_CamProHandler::h_GetInstance()->SetCANDllType(e_Type);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of expand signal.

   \param[in] oq_Expand    true: expand; false: collapse
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidget::m_OnExpand(const bool oq_Expand) const
{
   this->mpc_Ui->pc_WiContent->setVisible(oq_Expand);
   C_UsHandler::h_GetInstance()->SetWiDllConfigExpanded(oq_Expand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for browse button click.

   Browse for CAN DLL.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidget::m_OnBrowse(void)
{
   QString c_Path;
   QString c_Folder = C_CamProHandler::h_GetInstance()->GetCustomCANDllPath();
   const QString c_Filter = QString(C_GtGetText::h_GetText("CAN DLL ")) + "(*.dll)";
   QFileDialog c_Dialog(this, C_GtGetText::h_GetText("Select CAN DLL"), c_Folder, c_Filter);

   if ((c_Folder == "") || (QFileInfo(c_Folder).dir().exists() == false))
   {
      // use executable path if no existing directory is given
      c_Folder = C_Uti::h_GetExePath();
   }
   else
   {
      // get directory of dll path
      c_Folder = QFileInfo(c_Folder).dir().absolutePath();
   }

   c_Dialog.setDirectory(c_Folder);
   c_Dialog.setDefaultSuffix(".dll");

   if (c_Dialog.exec() == static_cast<sintn>(QDialog::Accepted))
   {
      c_Path = c_Dialog.selectedFiles().at(0);
      c_Path = C_CamUti::h_AskUserToSaveRelativePath(this, c_Path, C_Uti::h_GetExePath());

      // if path contains invalid characters this returned empty
      if (c_Path.isEmpty() == false)
      {
         this->mpc_Ui->pc_LineEditCustomDllPath->SetPath(c_Path, C_Uti::h_GetExePath());
         this->m_UpdateCANDllPath();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Insert selected path variable in DLL path string.

   \param[in]       orc_Variable     path variable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidget::m_InsertPathVar(const QString & orc_Variable)
{
   this->mpc_Ui->pc_LineEditCustomDllPath->InsertVariable(orc_Variable);
   this->m_OnCustomDllEdited();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for editing finished signal of line edit for custom DLL.

   Check if inserted path is empty or contains invalid characters.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidget::m_OnCustomDllEdited(void)
{
   const QString c_Path =  this->mpc_Ui->pc_LineEditCustomDllPath->GetPath();
   const QString c_ResolvedPath = C_CamUti::h_ResolvePlaceholderVariables(c_Path);

   if ((stw_opensyde_core::C_OSCUtils::h_CheckValidFilePath(c_ResolvedPath.toStdString().c_str()) == true) ||
       (c_ResolvedPath.isEmpty() == true)) // actually empty path is not valid but feedback on "play" is good enough
   {
      // update data handling
      this->m_UpdateCANDllPath();
   }
   else
   {
      // Invalid name: revert and inform user
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      const QString c_Heading = C_GtGetText::h_GetText("PC CAN Interface configuration");
      const QString c_MessageText =
         C_GtGetText::h_GetText("CAN DLL path contains invalid characters.");
      QString c_Details;

      c_Details = C_GtGetText::h_GetText("Path:\n");
      c_Details += c_Path;

      if (c_Path != c_ResolvedPath)
      {
         c_Details += C_GtGetText::h_GetText(" (resolved: ") + c_ResolvedPath + ")";
      }

      // disconnect to ignore editingFinished signal on focus lose because of popup
      disconnect(this->mpc_Ui->pc_LineEditCustomDllPath, &C_CamOgeLeFilePath::editingFinished,
                 this, &C_CamMosDllWidget::m_OnCustomDllEdited);
      c_Message.SetHeading(c_Heading);
      c_Message.SetDescription(c_MessageText);
      c_Message.SetDetails(c_Details);
      c_Message.Execute();

      this->m_LoadConfig();

      if (this->mpc_Ui->pc_LineEditCustomDllPath->hasFocus() == true)
      {
         this->mpc_Ui->pc_LineEditCustomDllPath->UpdateText();
      }

      // reconnect
      connect(this->mpc_Ui->pc_LineEditCustomDllPath, &C_CamOgeLeFilePath::editingFinished,
              this, &C_CamMosDllWidget::m_OnCustomDllEdited);
   }
}
