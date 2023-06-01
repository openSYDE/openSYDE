//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for new data block properties (implementation)

   Widget for new data block properties

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFileDialog>

#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_ImpUtil.hpp"
#include "C_OscUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiProject.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeDbProperties.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "ui_C_SdNdeDbProperties.h"
#include "C_SdNdeDbSelectDataPools.hpp"
#include "C_Uti.hpp"
#include "C_PuiUtil.hpp"
#include "C_OgeWiUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SdNdeDbProperties::mhs32_VERSION_INDEX_V1 = 0;
const int32_t C_SdNdeDbProperties::mhs32_VERSION_INDEX_V2 = 1;
const int32_t C_SdNdeDbProperties::mhs32_VERSION_INDEX_V3 = 2;
const int32_t C_SdNdeDbProperties::mhs32_VERSION_INDEX_V4 = 3;
const int32_t C_SdNdeDbProperties::mhs32_VERSION_INDEX_V5 = 4;
const int32_t C_SdNdeDbProperties::mhs32_VERSION_INDEX_V6 = 5;
// when adding new versions do not forget to update mu16_HIGHEST_KNOWN_CODE_STRUCTURE_VERSION

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.
   Warning: uninitialized if os32_ApplicationIndex invalid

   \param[in]      ou32_NodeIndex         Node index
   \param[in]      os32_ApplicationIndex  Application index
   \param[in,out]  orc_Parent             Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbProperties::C_SdNdeDbProperties(const uint32_t ou32_NodeIndex, const int32_t os32_ApplicationIndex,
                                         stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDbProperties),
   mu32_NodeIndex(ou32_NodeIndex),
   ms32_ApplicationIndex(os32_ApplicationIndex),
   me_Type(C_OscNodeApplication::ePROGRAMMABLE_APPLICATION),
   mrc_ParentDialog(orc_Parent)
{
   QStringList c_CodeGeneratorSupportedFiles;

   this->mpc_Ui->setupUi(this);

   //Configure
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_CheckBoxFileGen, "Font", 16);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_CheckBoxFileGen, "SemiBold", false);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_CheckBoxFileGen, "Bold", false);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_CheckBoxFileGen, "Foreground", 3);
   this->mpc_Ui->pc_PushButtonAddDataPool->SetSvg("://images/IconAddEnabled.svg");
   this->mpc_Ui->pc_SpinBoxProcessID->SetMinimumCustom(0);
   this->mpc_Ui->pc_SpinBoxProcessID->SetMaximumCustom(255);
   this->mpc_Ui->pc_ScrollAreaWidgetContents->SetBackgroundColor(-1);
   this->mpc_Ui->pc_ScrollAreaDataPools->DeactivateScrollbarResize();
   this->mpc_Ui->pc_PushButtonRevertToDefault->setIcon(QIcon("://images/IconRevert.svg"));
   this->mpc_Ui->pc_PushButtonRevertToDefault->setMenu(NULL); // remove menu (we only need the style of button class)
   // Provide data block project as placeholder variable for output, source code directory, file generator and IDE call
   this->mpc_Ui->pc_PubMenuOutputFile->AddDatablockSection();
   this->mpc_Ui->pc_PubMenuCodeGenerate->AddDatablockSection();
   this->mpc_Ui->pc_PubMenuCodeGenerator->AddDatablockSection();
   this->mpc_Ui->pc_PubMenuIDE->AddDatablockSection();
   this->mpc_Ui->pc_LineEditName->setMaxLength(ms32_C_ITEM_MAX_CHAR_COUNT); // name restriction

   //Remove debug labels
   this->mpc_Ui->pc_GroupBoxDataPoolsNotEmpty->setTitle("");
   this->mpc_Ui->pc_PushButtonProject->setText("");
   this->mpc_Ui->pc_PushButtonOutputFile->setText("");
   this->mpc_Ui->pc_PushButtonCodeGenerate->setText("");
   this->mpc_Ui->pc_PushButtonFileGenerate->setText("");
   this->mpc_Ui->pc_PushButtonCodeGenerator->setText("");
   this->mpc_Ui->pc_PushButtonIDE->setText("");
   this->mpc_Ui->pc_PubMenuProject->setText("");
   this->mpc_Ui->pc_PubMenuOutputFile->setText("");
   this->mpc_Ui->pc_PubMenuCodeGenerate->setText("");
   this->mpc_Ui->pc_PubMenuFileGenerate->setText("");
   this->mpc_Ui->pc_PubMenuCodeGenerator->setText("");
   this->mpc_Ui->pc_PubMenuIDE->setText("");
   this->mpc_Ui->pc_PushButtonRevertToDefault->setText("");
   this->mpc_Ui->pc_LabelSafeFile->setText("");
   this->mpc_Ui->pc_LabelSafeFileValue->setText("");
   this->mpc_Ui->pc_LabelNonSafeFile->setText("");
   this->mpc_Ui->pc_LabelNonSafeFileValue->setText("");

   // Activate drag and drop for path/file line edits
   this->mpc_Ui->pc_LineEditProject->SetDragAndDropActiveForFolder(true);
   this->mpc_Ui->pc_LineEditOutputFile->SetDragAndDropActiveForFile("hex");

   c_CodeGeneratorSupportedFiles.push_back("exe");
   c_CodeGeneratorSupportedFiles.push_back("bat");
   this->mpc_Ui->pc_LineEditCodeGenerator->SetDragAndDropActiveForFile(true, &c_CodeGeneratorSupportedFiles);

   this->mpc_Ui->pc_LineEditCodeGenerate->SetDragAndDropActiveForFolder(true);
   this->mpc_Ui->pc_LineEditFileGenerate->SetDragAndDropActiveForFolder(true);

   // Init non empty names
   InitStaticNames();

   // Load data
   m_LoadData();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // general connects
   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdNdeDbProperties::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this, &C_SdNdeDbProperties::m_CancelClicked);
   connect(this->mpc_Ui->pc_LineEditName, &C_OgeLePropertiesName::textChanged,
           this, &C_SdNdeDbProperties::m_OnNameEdited);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxProcessID, static_cast<void (QSpinBox::*)(int32_t)>(&QSpinBox::valueChanged), this,
           &C_SdNdeDbProperties::m_OnProcessIdChanged);
   connect(this->mpc_Ui->pc_PushButtonAddDataPool, &QPushButton::clicked, this,
           &C_SdNdeDbProperties::m_HandleAddDataPools);
   connect(this->mpc_Ui->pc_PushButtonRevertToDefault, &QPushButton::clicked, this,
           &C_SdNdeDbProperties::m_HandleRevertCodeGenerator);
   connect(this->mpc_Ui->pc_CheckBoxFileGen, &QCheckBox::stateChanged,
           this, &C_SdNdeDbProperties::m_OnFileGenerationChanged);

   // browse actions
   connect(this->mpc_Ui->pc_PushButtonProject, &QPushButton::clicked, this, &C_SdNdeDbProperties::m_OnClickProject);
   connect(this->mpc_Ui->pc_PushButtonCodeGenerate, &QPushButton::clicked,
           this, &C_SdNdeDbProperties::m_OnClickCodeGeneration);
   connect(this->mpc_Ui->pc_PushButtonFileGenerate, &QPushButton::clicked,
           this, &C_SdNdeDbProperties::m_OnClickFileGeneration);
   connect(this->mpc_Ui->pc_PushButtonCodeGenerator, &QPushButton::clicked,
           this, &C_SdNdeDbProperties::m_OnClickGenerator);
   connect(this->mpc_Ui->pc_PushButtonOutputFile, &QPushButton::clicked, this, &C_SdNdeDbProperties::m_OnClickOutput);
   connect(this->mpc_Ui->pc_PushButtonIDE, &QPushButton::clicked, this, &C_SdNdeDbProperties::m_OnClickIde);

   // path variables actions
   connect(this->mpc_Ui->pc_PubMenuProject, &C_OgePubPathVariables::SigVariableSelected,
           this->mpc_Ui->pc_LineEditProject, &C_OgeLeFilePath::InsertVariable);
   connect(this->mpc_Ui->pc_PubMenuProject, &C_OgePubPathVariables::SigVariableSelected,
           this, &C_SdNdeDbProperties::m_UpdatePathsRelativeToProject);
   connect(this->mpc_Ui->pc_PubMenuOutputFile, &C_OgePubPathVariables::SigVariableSelected,
           this->mpc_Ui->pc_LineEditOutputFile, &C_OgeLeFilePath::InsertVariable);
   connect(this->mpc_Ui->pc_PubMenuCodeGenerate, &C_OgePubPathVariables::SigVariableSelected,
           this->mpc_Ui->pc_LineEditCodeGenerate, &C_OgeLeFilePath::InsertVariable);
   connect(this->mpc_Ui->pc_PubMenuFileGenerate, &C_OgePubPathVariables::SigVariableSelected,
           this->mpc_Ui->pc_LineEditFileGenerate, &C_OgeLeFilePath::InsertVariable);
   connect(this->mpc_Ui->pc_PubMenuFileGenerate, &C_OgePubPathVariables::SigVariableSelected,
           this, &C_SdNdeDbProperties::m_UpdatePathsRelativeToGeneratedDir);
   connect(this->mpc_Ui->pc_PubMenuCodeGenerator, &C_OgePubPathVariables::SigVariableSelected,
           this->mpc_Ui->pc_LineEditCodeGenerator, &C_OgeLeFilePath::InsertVariable);
   connect(this->mpc_Ui->pc_PubMenuIDE, &C_OgePubPathVariables::SigVariableSelected,
           this->mpc_Ui->pc_LineEditIDE, &C_OgeLePlaceholderVar::InsertVariable);

   // connect for updating paths that are relative to project path
   connect(this->mpc_Ui->pc_LineEditProject, &C_OgeLeFilePath::editingFinished,
           this, &C_SdNdeDbProperties::m_UpdatePathsRelativeToProject);
   connect(this->mpc_Ui->pc_LineEditFileGenerate, &C_OgeLeFilePath::editingFinished,
           this, &C_SdNdeDbProperties::m_UpdatePathsRelativeToGeneratedDir);

   // connect for drag and drop path handling
   connect(this->mpc_Ui->pc_LineEditProject, &C_OgeLeFilePath::SigPathDropped,
           this, &C_SdNdeDbProperties::m_OnDroppedProject);
   connect(this->mpc_Ui->pc_LineEditOutputFile, &C_OgeLeFilePath::SigPathDropped,
           this, &C_SdNdeDbProperties::m_OnDroppedOutput);
   connect(this->mpc_Ui->pc_LineEditCodeGenerator, &C_OgeLeFilePath::SigPathDropped,
           this, &C_SdNdeDbProperties::m_OnDroppedGenerator);
   connect(this->mpc_Ui->pc_LineEditCodeGenerate, &C_OgeLeFilePath::SigPathDropped,
           this, &C_SdNdeDbProperties::m_OnDroppedCodeGeneration);
   connect(this->mpc_Ui->pc_LineEditFileGenerate, &C_OgeLeFilePath::SigPathDropped,
           this, &C_SdNdeDbProperties::m_OnDroppedFileGeneration);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbProperties::~C_SdNdeDbProperties(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get window size for binary case

   \return
   Window size for binary case
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_SdNdeDbProperties::h_GetBinaryWindowSize(void)
{
   const QSize c_SIZE(1152, 761);

   return c_SIZE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get window size for default case

   \return
   Window size for default case
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_SdNdeDbProperties::h_GetDefaultWindowSize(void)
{
   const QSize c_SIZE(1200, 942);

   return c_SIZE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Data Block"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Properties"));
   this->mpc_Ui->pc_LabelName->setText(C_GtGetText::h_GetText("Name"));
   this->mpc_Ui->pc_LabelComment->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_LabelHeadingProject->setText(C_GtGetText::h_GetText("Project"));
   this->mpc_Ui->pc_CheckBoxFileGen->setText(C_GtGetText::h_GetText("File Generation"));
   this->mpc_Ui->pc_LabelFileGenerationDisabled->setText(C_GtGetText::h_GetText("File Generation disabled."));
   this->mpc_Ui->pc_LabelSafeFile->setText(C_GtGetText::h_GetText("Safe File"));
   this->mpc_Ui->pc_LabelNonSafeFile->setText(C_GtGetText::h_GetText("Non Safe File"));
   this->mpc_Ui->pc_LabelSafeFileNone->setText(C_GtGetText::h_GetText("<safe configuration not available>"));
   this->mpc_Ui->pc_LabelNonSafeFileNone->setText(C_GtGetText::h_GetText("<non-safe configuration not available>"));
   this->mpc_Ui->pc_LabelProject->setText(C_GtGetText::h_GetText("Project Path"));
   this->mpc_Ui->pc_LabelProcessID->setText(C_GtGetText::h_GetText("Process ID"));
   this->mpc_Ui->pc_LabelOutputFile->setText(C_GtGetText::h_GetText("Output File"));
   this->mpc_Ui->pc_LabelIDE->setText(C_GtGetText::h_GetText("IDE Call"));
   this->mpc_Ui->pc_LabelCodeGenerator->setText(C_GtGetText::h_GetText("Generator"));
   this->mpc_Ui->pc_LabelCodeGenerate->setText(C_GtGetText::h_GetText("Gen. Directory"));
   this->mpc_Ui->pc_LabelFileGenerate->setText(C_GtGetText::h_GetText("Gen. Directory"));
   this->mpc_Ui->pc_LabelCodeStructure->setText(C_GtGetText::h_GetText("Structure"));
   this->mpc_Ui->pc_ComboBoxCode->addItem("dummy");
   this->mpc_Ui->pc_ComboBoxCode->addItem("dummy");
   this->mpc_Ui->pc_ComboBoxCode->addItem("dummy");
   this->mpc_Ui->pc_ComboBoxCode->addItem("dummy");
   this->mpc_Ui->pc_ComboBoxCode->addItem("dummy");
   this->mpc_Ui->pc_ComboBoxCode->addItem("dummy");
   this->mpc_Ui->pc_ComboBoxCode->setItemText(mhs32_VERSION_INDEX_V1, C_GtGetText::h_GetText("Version 1"));
   this->mpc_Ui->pc_ComboBoxCode->setItemText(mhs32_VERSION_INDEX_V2, C_GtGetText::h_GetText("Version 2"));
   this->mpc_Ui->pc_ComboBoxCode->setItemText(mhs32_VERSION_INDEX_V3, C_GtGetText::h_GetText("Version 3"));
   this->mpc_Ui->pc_ComboBoxCode->setItemText(mhs32_VERSION_INDEX_V4, C_GtGetText::h_GetText("Version 4"));
   this->mpc_Ui->pc_ComboBoxCode->setItemText(mhs32_VERSION_INDEX_V5, C_GtGetText::h_GetText("Version 5"));
   this->mpc_Ui->pc_ComboBoxCode->setItemText(mhs32_VERSION_INDEX_V6, C_GtGetText::h_GetText("Version 6"));
   this->mpc_Ui->pc_LabelHeadingDatapools->setText(C_GtGetText::h_GetText("Owned Datapools"));
   this->mpc_Ui->pc_LabelDataPoolsEmpty->setText("No assigned Datapools, \nadd any via the '+' button");
   this->mpc_Ui->pc_CommentText->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here ..."));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));

   //Tool tips
   this->mpc_Ui->pc_LabelName->SetToolTipInformation(
      C_GtGetText::h_GetText("Name"),
      C_GtGetText::h_GetText("Symbolic Data Block name. Unique within node\n"
                             "\nFollowing C naming conventions are required:"
                             "\n - must not be empty"
                             "\n - must not start with digits"
                             "\n - only alphanumeric characters and \"_\""
                             "\n - should not be longer than 31 characters"));
   this->mpc_Ui->pc_LabelComment->SetToolTipInformation(C_GtGetText::h_GetText("Comment"),
                                                        C_GtGetText::h_GetText("Comment for this Data Block."));

   this->mpc_Ui->pc_LabelHeadingDatapools->SetToolTipInformation(
      C_GtGetText::h_GetText("Owned Datapools"),
      C_GtGetText::h_GetText("List of all Datapool(s) which are mapped to this Data Block"
                             "\n(Relevant for file generation)."));

   this->mpc_Ui->pc_PushButtonAddDataPool->SetToolTipInformation(C_GtGetText::h_GetText("Add"),
                                                                 C_GtGetText::h_GetText("Add Datapool(s) to list."));

   this->mpc_Ui->pc_CheckBoxFileGen->SetToolTipInformation(
      C_GtGetText::h_GetText("File Generation"),
      C_GtGetText::h_GetText("Generate files for this Data Block with an external code or file generator tool."));

   this->mpc_Ui->pc_LabelSafeFile->SetToolTipInformation(
      C_GtGetText::h_GetText("Safe File"),
      C_GtGetText::h_GetText("File name of generated parameter set image for safe hardware configuration data."));
   this->mpc_Ui->pc_LabelNonSafeFile->SetToolTipInformation(
      C_GtGetText::h_GetText("Non Safe File"),
      C_GtGetText::h_GetText("File name of generated parameter set image for non-safe hardware configuration data."));
   this->mpc_Ui->pc_LabelSafeFileNone->SetToolTipInformation(
      "Safe Channel",
      C_GtGetText::h_GetText("The hardware definition for this node does not support safe channel configuration."));
   this->mpc_Ui->pc_LabelNonSafeFileNone->SetToolTipInformation(
      "Non Safe Channel",
      C_GtGetText::h_GetText("The hardware definition for this node does not support non-safe channel configuration."));

   this->mpc_Ui->pc_LabelProcessID->SetToolTipInformation(
      C_GtGetText::h_GetText("Process ID"),
      C_GtGetText::h_GetText(
         "This is required by remote Datapool configurations. "
         "For devices that only can run one process at a time this setting is not relevant. "
         "For information how the IDs are used see the device's user manual."));

   this->mpc_Ui->pc_LabelProject->SetToolTipInformation(
      C_GtGetText::h_GetText("Project Path"),
      C_GtGetText::h_GetText("Root directory of the target project. Absolute or relative to *.syde."));
   this->mpc_Ui->pc_LabelOutputFile->SetToolTipInformation(
      C_GtGetText::h_GetText("Output File"),
      C_GtGetText::h_GetText("Location of output file. Absolute or relative to project path."));
   this->mpc_Ui->pc_LabelCodeGenerate->SetToolTipInformation(
      C_GtGetText::h_GetText("Generation Directory"),
      C_GtGetText::h_GetText("Location of the directory for openSYDE generated source code files. "
                             "Absolute or relative to project path."));
   this->mpc_Ui->pc_LabelFileGenerate->SetToolTipInformation(
      C_GtGetText::h_GetText("Generation Directory"),
      C_GtGetText::h_GetText("Location of the directory for openSYDE generated parameter set image files. "
                             "Absolute or relative to *.syde."));
   this->mpc_Ui->pc_LabelCodeGenerator->SetToolTipInformation(
      C_GtGetText::h_GetText("Generator"),
      C_GtGetText::h_GetText("Location of file generator executable (*.exe / *.bat). "
                             "Absolute or relative to openSYDE.exe."));
   this->mpc_Ui->pc_LabelCodeStructure->SetToolTipInformation(
      C_GtGetText::h_GetText("Structure Version"),
      static_cast<QString>(C_GtGetText::h_GetText(
                              "Version of structure for generated output files. Which version to select depends on what "
                              "your physical target supports. See target documentation for further information about "
                              "supported versions.\n"
                              " - %1: Compatibility mode for previous versions of provided tools \n"
                              "   (might not take all supported project properties into account)\n"
                              " - %2: Support of COMM messages with multiplexing\n"
                              " - %3: Support for flexible placement of embedded RAM variables\n"
                              " - %4: Support for public scope of Datapool content\n"
                              " - %5: Optimization of sequences where the openSYDE server keeps a thread/process lock on\n"
                              " - %6: CANopen configuration: Support for SYNC PDOs and restoring device configuration at startup"))
      .arg(this->mpc_Ui->pc_ComboBoxCode->itemText(mhs32_VERSION_INDEX_V1),
           this->mpc_Ui->pc_ComboBoxCode->itemText(mhs32_VERSION_INDEX_V2),
           this->mpc_Ui->pc_ComboBoxCode->itemText(mhs32_VERSION_INDEX_V3),
           this->mpc_Ui->pc_ComboBoxCode->itemText(mhs32_VERSION_INDEX_V4),
           this->mpc_Ui->pc_ComboBoxCode->itemText(mhs32_VERSION_INDEX_V5),
           this->mpc_Ui->pc_ComboBoxCode->itemText(mhs32_VERSION_INDEX_V6)));
   this->mpc_Ui->pc_LabelIDE->SetToolTipInformation(
      C_GtGetText::h_GetText("IDE Call"),
      C_GtGetText::h_GetText("Command line IDE call. Absolute or relative to working directory. Make sure to escape "
                             "paths that contain blanks with quotation marks and avoid non-printable characters."));

   this->mpc_Ui->pc_PushButtonRevertToDefault->SetToolTipInformation(
      C_GtGetText::h_GetText("Revert to Default"),
      C_GtGetText::h_GetText("Set path to default openSYDE file generator \"SYDE Coder C\"."));

   this->mpc_Ui->pc_PushButtonProject->SetToolTipInformation(
      C_GtGetText::h_GetText("Browse"),
      C_GtGetText::h_GetText("Browse for project path of this Data Block."));
   this->mpc_Ui->pc_PushButtonOutputFile->SetToolTipInformation(
      C_GtGetText::h_GetText("Browse"),
      C_GtGetText::h_GetText("Browse for output file of this Data Block."));
   this->mpc_Ui->pc_PushButtonCodeGenerate->SetToolTipInformation(
      C_GtGetText::h_GetText("Browse"),
      C_GtGetText::h_GetText("Browse for location for openSYDE generated source code files."));
   this->mpc_Ui->pc_PushButtonFileGenerate->SetToolTipInformation(
      C_GtGetText::h_GetText("Browse"),
      C_GtGetText::h_GetText("Browse for location for openSYDE generated parameter set image files."));
   this->mpc_Ui->pc_PushButtonCodeGenerator->SetToolTipInformation(
      C_GtGetText::h_GetText("Browse"),
      C_GtGetText::h_GetText("Browse for file generator of this Data Block."));
   this->mpc_Ui->pc_PushButtonIDE->SetToolTipInformation(
      C_GtGetText::h_GetText("Browse"),
      C_GtGetText::h_GetText("Browse for IDE executable."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply new data block properties

   \param[in,out]  orc_Application  Application to apply to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::ApplyNewData(C_OscNodeApplication & orc_Application) const
{
   orc_Application.c_Name = this->mpc_Ui->pc_LineEditName->text().toStdString().c_str();
   orc_Application.c_Comment = this->mpc_Ui->pc_CommentText->toPlainText().toStdString().c_str();
   orc_Application.e_Type = this->me_Type;
   orc_Application.u8_ProcessId = static_cast<uint8_t>(this->mpc_Ui->pc_SpinBoxProcessID->value());
   switch (this->me_Type)
   {
   case C_OscNodeApplication::ePROGRAMMABLE_APPLICATION:
   case C_OscNodeApplication::eBINARY:
      orc_Application.c_ProjectPath = this->mpc_Ui->pc_LineEditProject->GetPath().toStdString().c_str();
      orc_Application.c_ResultPaths.resize(1);
      orc_Application.c_ResultPaths[0] = this->mpc_Ui->pc_LineEditOutputFile->GetPath().toStdString().c_str();
      break;
   case C_OscNodeApplication::ePARAMETER_SET_HALC:
      orc_Application.c_ProjectPath = this->mpc_Ui->pc_LineEditFileGenerate->GetPath().toStdString().c_str();

      if (this->mpc_Ui->pc_LabelSafeFileValue->text() == "")
      {
         // all non-safe
         orc_Application.c_ResultPaths.resize(1);
         orc_Application.c_ResultPaths[0] = this->mpc_Ui->pc_LabelNonSafeFileValue->text().toStdString().c_str();
      }
      else if (this->mpc_Ui->pc_LabelNonSafeFileValue->text() == "")
      {
         // all safe
         orc_Application.c_ResultPaths.resize(1);
         orc_Application.c_ResultPaths[0] = this->mpc_Ui->pc_LabelSafeFileValue->text().toStdString().c_str();
      }
      else
      {
         //safe and non-safe
         orc_Application.c_ResultPaths.resize(2);
         orc_Application.c_ResultPaths[0] = this->mpc_Ui->pc_LabelSafeFileValue->text().toStdString().c_str();
         orc_Application.c_ResultPaths[1] = this->mpc_Ui->pc_LabelNonSafeFileValue->text().toStdString().c_str();
      }
      break;
   default:
      tgl_assert(false);
      break;
   }
   orc_Application.c_IdeCall = this->mpc_Ui->pc_LineEditIDE->text().toStdString().c_str();
   orc_Application.c_CodeGeneratorPath =
      this->mpc_Ui->pc_LineEditCodeGenerator->GetPath().toStdString().c_str();
   orc_Application.c_GeneratePath = this->mpc_Ui->pc_LineEditCodeGenerate->GetPath().toStdString().c_str();
   switch (this->mpc_Ui->pc_ComboBoxCode->currentIndex())
   {
   case mhs32_VERSION_INDEX_V1:
      orc_Application.u16_GenCodeVersion = 1U;
      break;
   case mhs32_VERSION_INDEX_V2:
      orc_Application.u16_GenCodeVersion = 2U;
      break;
   case mhs32_VERSION_INDEX_V3:
      orc_Application.u16_GenCodeVersion = 3U;
      break;
   case mhs32_VERSION_INDEX_V4:
      orc_Application.u16_GenCodeVersion = 4U;
      break;
   case mhs32_VERSION_INDEX_V5:
      orc_Application.u16_GenCodeVersion = 5U;
      break;
   case mhs32_VERSION_INDEX_V6:
      orc_Application.u16_GenCodeVersion = 6U;
      break;
   default:
      orc_Application.u16_GenCodeVersion = 6U;
      tgl_assert(false); // combo box has unknown index should never happen
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle data pools of application

   \param[in]  ou32_ApplicationIndex   Index of application to apply data pools to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::HandleDataPools(const uint32_t ou32_ApplicationIndex) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      for (uint32_t u32_ItDataPool = 0UL; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
      {
         bool q_Found = false;
         const C_OscNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
         for (std::set<uint32_t>::const_iterator c_ItSelect = this->mc_SelectedDataPools.begin();
              c_ItSelect != this->mc_SelectedDataPools.end(); ++c_ItSelect)
         {
            if (*c_ItSelect == u32_ItDataPool)
            {
               q_Found = true;
            }
         }
         //Hint: these operations should not change the vector so no reload of nodes necessary
         if (q_Found == true)
         {
            //Always assign this one
            tgl_assert(C_PuiSdHandler::h_GetInstance()->
                       AssignDataPoolApplication(this->mu32_NodeIndex, u32_ItDataPool,
                                                 static_cast<int32_t>(ou32_ApplicationIndex)) == C_NO_ERR);
         }
         else
         {
            if ((rc_DataPool.s32_RelatedDataBlockIndex >= 0) &&
                (static_cast<uint32_t>(rc_DataPool.s32_RelatedDataBlockIndex) == ou32_ApplicationIndex))
            {
               //Unassign this one
               tgl_assert(C_PuiSdHandler::h_GetInstance()->UnassignDataPoolApplication(this->mu32_NodeIndex,
                                                                                       u32_ItDataPool) == C_NO_ERR);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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

   Only allow OK if application name and process ID are valid, if process ID error: list existing ones!
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OkClicked(void)
{
   const QString c_ErrorName = this->m_CheckName();
   const QString c_ErrorId = this->m_CheckId();
   const QString c_ErrorPaths = this->m_CheckPaths();

   const bool q_NameIsValid = (c_ErrorName == "");
   const bool q_IdIsValid = (c_ErrorId == "");
   const bool q_PathsAreValid = (c_ErrorPaths == "");

   if ((q_NameIsValid == true) && (q_IdIsValid == true) && (q_PathsAreValid == true))
   {
      // clear unused data
      switch (this->me_Type)
      {
      case C_OscNodeApplication::ePROGRAMMABLE_APPLICATION:
         this->mpc_Ui->pc_LabelSafeFileValue->setText("");
         this->mpc_Ui->pc_LabelNonSafeFileValue->setText("");
         this->mpc_Ui->pc_LineEditFileGenerate->SetPath("");
         break;
      case C_OscNodeApplication::eBINARY:
         this->mc_SelectedDataPools.clear();
         this->mpc_Ui->pc_LineEditCodeGenerator->SetPath("");
         this->mpc_Ui->pc_ComboBoxCode->setCurrentIndex(0);
         this->mpc_Ui->pc_SpinBoxProcessID->setValue(0);
         this->mpc_Ui->pc_LineEditCodeGenerate->SetPath("");
         this->mpc_Ui->pc_LabelSafeFileValue->setText("");
         this->mpc_Ui->pc_LabelNonSafeFileValue->setText("");
         break;
      case C_OscNodeApplication::ePARAMETER_SET_HALC:
         this->mpc_Ui->pc_ComboBoxCode->setCurrentIndex(0);
         this->mpc_Ui->pc_SpinBoxProcessID->setValue(0);
         this->mpc_Ui->pc_LineEditCodeGenerate->SetPath("");
         this->mpc_Ui->pc_LineEditProject->SetPath("");
         this->mpc_Ui->pc_LineEditOutputFile->SetPath("");
         this->mpc_Ui->pc_LineEditIDE->setText("");
         break;
      default:
         tgl_assert(false);
         break;
      }

      this->mrc_ParentDialog.accept();
   }
   else
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      const QString c_Heading = C_GtGetText::h_GetText("Data Block Properties");
      const QString c_MessageText =
         C_GtGetText::h_GetText("Invalid content detected. For more information see details.");
      QString c_Details = "";
      if (q_NameIsValid == false)
      {
         c_Details +=
            static_cast<QString>(C_GtGetText::h_GetText("Name \"%1\" is invalid: ")).arg(
               this->mpc_Ui->pc_LineEditName->text());
         c_Details += "\n";
         c_Details += c_ErrorName;
         c_Details += "\n\n";
      }
      if (q_IdIsValid == false)
      {
         c_Details += C_GtGetText::h_GetText("Process ID is already used. ");
         c_Details += "\n";
         c_Details += c_ErrorId;
         c_Details += "\n\n";
      }
      if (q_PathsAreValid == false)
      {
         c_Details += C_GtGetText::h_GetText("Invalid paths (e.g. containing invalid characters):\n");
         c_Details += c_ErrorPaths;
      }

      c_Message.SetHeading(c_Heading);
      c_Message.SetDescription(c_MessageText);
      c_Message.SetDetails(c_Details);
      c_Message.SetCustomMinHeight(180, 270);
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_CancelClicked()
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check data block name

   \return
   error text of name check (empty if no error occured)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDbProperties::m_CheckName() const
{
   QString c_Return = "";

   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      //Check Name
      bool q_DuplicateNameError = false;
      const bool q_InvalidNameError =
         !C_OscUtils::h_CheckValidCeName(this->mpc_Ui->pc_LineEditName->text().toStdString().c_str());

      for (uint32_t u32_ItApplication = 0UL; u32_ItApplication < pc_Node->c_Applications.size(); ++u32_ItApplication)
      {
         //Avoid conflict with itself
         if ((this->ms32_ApplicationIndex >= 0) &&
             (static_cast<uint32_t>(this->ms32_ApplicationIndex) == u32_ItApplication))
         {
            //Skip itself
         }
         else
         {
            const C_OscNodeApplication & rc_Application = pc_Node->c_Applications[u32_ItApplication];
            //Check name
            if (this->mpc_Ui->pc_LineEditName->text().compare(rc_Application.c_Name.c_str()) == 0)
            {
               q_DuplicateNameError = true;
            }
         }
      }

      if (q_DuplicateNameError == true)
      {
         c_Return += C_GtGetText::h_GetText("Name is already in use.");
      }
      if (q_InvalidNameError == true)
      {
         c_Return +=
            C_GtGetText::h_GetText("Name is empty or contains invalid characters.");
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check process ID

   \return
   error text of process ID check (empty if no error occured)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDbProperties::m_CheckId(void) const
{
   QString c_Return = "";

   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      //Check ID
      bool q_ProcessIdError = false;
      std::vector<uint32_t> c_UsedProcessIds;
      for (uint32_t u32_ItApplication = 0UL; u32_ItApplication < pc_Node->c_Applications.size(); ++u32_ItApplication)
      {
         //Avoid conflict with itself
         if ((this->ms32_ApplicationIndex >= 0) &&
             (static_cast<uint32_t>(this->ms32_ApplicationIndex) == u32_ItApplication))
         {
            //Skip itself
         }
         else
         {
            const C_OscNodeApplication & rc_Application = pc_Node->c_Applications[u32_ItApplication];
            //Only check programmable applications
            if (rc_Application.e_Type == C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
            {
               //Only check process IDs if current application is programmable
               if (this->me_Type == C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
               {
                  //Check process ID
                  c_UsedProcessIds.push_back(rc_Application.u8_ProcessId);
                  if (static_cast<uint8_t>(this->mpc_Ui->pc_SpinBoxProcessID->value()) == rc_Application.u8_ProcessId)
                  {
                     q_ProcessIdError = true;
                  }
               }
            }
         }
      }
      if (q_ProcessIdError == true)
      {
         if (c_UsedProcessIds.size() > 0)
         {
            QString c_Ids;
            for (uint32_t u32_ItId = 0; u32_ItId < (static_cast<uint32_t>(c_UsedProcessIds.size()) - 1UL); ++u32_ItId)
            {
               c_Ids += static_cast<QString>("%1,").arg(c_UsedProcessIds[u32_ItId]);
            }
            c_Ids +=
               QString::number(c_UsedProcessIds[static_cast<std::vector<uint32_t>::size_type>(c_UsedProcessIds.size() -
                                                                                              1UL)]);
            c_Return += static_cast<QString>(C_GtGetText::h_GetText("Already used Process IDs: %1")).arg(c_Ids);
         }
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check all paths for invalid characters

   \return
   error text of paths
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDbProperties::m_CheckPaths(void) const
{
   QString c_Return = "";

   //Project Path
   c_Return += this->m_CheckPath(this->mpc_Ui->pc_LineEditProject->GetPath());

   // Output File
   c_Return += this->m_CheckPath(this->mpc_Ui->pc_LineEditOutputFile->GetPath());

   // Source Code Generation Directory
   c_Return += this->m_CheckPath(this->mpc_Ui->pc_LineEditCodeGenerate->GetPath());

   // PSI File Generation Directory
   c_Return += this->m_CheckPath(this->mpc_Ui->pc_LineEditFileGenerate->GetPath());
   // empty path not allowed for PSI file generation
   if ((this->me_Type == C_OscNodeApplication::ePARAMETER_SET_HALC) &&
       (this->mpc_Ui->pc_LineEditFileGenerate->GetPath().isEmpty() == true))
   {
      c_Return += C_GtGetText::h_GetText("Generation directory path must not be empty.\n");
   }

   // Generator
   c_Return += this->m_CheckPath(this->mpc_Ui->pc_LineEditCodeGenerator->GetPath());

   // no logic for IDE call because this can be anything callable on a command line, so the user is responsible for a
   // correct call

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check path for invalid characters

   See C_OscUtils for details about valid characters.

   \param[in]  orc_Path    Path

   \return
   error path plus newline (empty if no error occured)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDbProperties::m_CheckPath(const QString & orc_Path) const
{
   QString c_Return = "";

   // empty is allowed and resolved with relative path
   if (orc_Path.isEmpty() == false)
   {
      const QString c_ProjectPath =
         C_PuiUtil::h_GetResolvedAbsPathFromProject(this->mpc_Ui->pc_LineEditProject->GetPath());

      const QString c_ResolvedPath = C_PuiUtil::h_ResolvePlaceholderVariables(orc_Path, c_ProjectPath);

      // use resolve engine to check resulting path
      if (C_OscUtils::h_CheckValidFilePath(c_ResolvedPath.toStdString().c_str()) == false)
      {
         c_Return = orc_Path;
         if (orc_Path != c_ResolvedPath)
         {
            c_Return += C_GtGetText::h_GetText(" (resolved: ") + c_ResolvedPath + ")";
         }
         c_Return += "\n";
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get path for file browse dialog

   \param[in]  orc_CurrentPath   Current absolute (!) path of file or directory

   \return
   Existing path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDbProperties::m_GetDialogPath(const QString & orc_CurrentPath) const
{
   QString c_Folder;
   QFileInfo c_FileInfo;
   bool q_FoundPath = false;

   // First favorite: current path (if existing)
   c_FileInfo.setFile(orc_CurrentPath);

   if (orc_CurrentPath.isEmpty() == false)
   {
      if (c_FileInfo.exists() == true)
      {
         // input path is an existing directory -> use this directory
         if (c_FileInfo.isDir() == true)
         {
            c_Folder = orc_CurrentPath;
            q_FoundPath = true;
         }

         else if (c_FileInfo.isFile() == true)
         {
            // input path is an existing file -> use parent directory
            c_Folder = c_FileInfo.dir().absolutePath();
            q_FoundPath = true;
         }
         else
         {
            // do nothing
         }
      }
      // c_FileInfo.isFile() and c_FileInfo.isDir() do not work as expected when the file does not exist
      // so just check if the file name contains a '.' to avoid going to parent directory of a not existing directory
      else if ((c_FileInfo.fileName().contains('.') == true) && (c_FileInfo.dir().exists() == true))
      {
         c_Folder = c_FileInfo.dir().absolutePath();
         q_FoundPath = true;
      }
      else
      {
         // do nothing
      }
   }

   // Second favorite: application project path
   if (q_FoundPath == false)
   {
      c_Folder = this->mpc_Ui->pc_LineEditProject->GetPath();
      if (c_Folder.isEmpty() == false)
      {
         c_Folder = C_PuiUtil::h_GetResolvedAbsPathFromProject(c_Folder);
         c_FileInfo.setFile(c_Folder);
         if (c_FileInfo.exists() == true)
         {
            q_FoundPath = true;
         }
      }
   }

   // Third favorite: openSYDE project files
   if (q_FoundPath == false)
   {
      if (C_PuiProject::h_GetInstance()->IsEmptyProject() == false)
      {
         c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();
         q_FoundPath = true;
      }
   }

   // Fourth favorite: openSYDE.exe
   if (q_FoundPath == false)
   {
      c_Folder = C_Uti::h_GetExePath();
   }

   return c_Folder;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load displayed data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_LoadData(void)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      //Add existing selected datapools
      this->mc_SelectedDataPools.clear();
      for (uint32_t u32_ItDataPool = 0UL; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
      {
         const C_OscNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
         if ((rc_DataPool.s32_RelatedDataBlockIndex >= 0) &&
             (this->ms32_ApplicationIndex >= 0) &&
             (rc_DataPool.s32_RelatedDataBlockIndex == this->ms32_ApplicationIndex))
         {
            this->mc_SelectedDataPools.insert(u32_ItDataPool);
         }
      }

      //Handle file generation checkbox
      if ((pc_Node->pc_DeviceDefinition != NULL) &&
          (pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size()))
      {
         bool q_FileGenEnabled = true;
         if (pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].q_ProgrammingSupport == false)
         {
            q_FileGenEnabled = false;

            // check for HALC NVM file generation (psi files)
            if ((pc_Node->c_HalcConfig.q_NvmBasedConfig == true) && (pc_Node->c_HalcConfig.IsClear() == false))
            {
               q_FileGenEnabled = true;

               // check if there already exists a Data Block of type PSI generation
               for (uint32_t u32_ItDataBlock = 0UL; u32_ItDataBlock < pc_Node->c_Applications.size(); ++u32_ItDataBlock)
               {
                  const C_OscNodeApplication & rc_CurDataBlock = pc_Node->c_Applications[u32_ItDataBlock];
                  if ((rc_CurDataBlock.e_Type == C_OscNodeApplication::ePARAMETER_SET_HALC) &&
                      (u32_ItDataBlock != static_cast<uint32_t>(this->ms32_ApplicationIndex)))
                  {
                     q_FileGenEnabled = false;
                     break;
                  }
               }

               // set visibility of output file labels
               if (pc_Node->c_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eONE_LEVEL_ALL_NON_SAFE)
               {
                  // no safe file
                  this->mpc_Ui->pc_LabelSafeFileValue->setVisible(false);
                  this->mpc_Ui->pc_LabelSafeFileNone->setVisible(true);
               }
               else
               {
                  this->mpc_Ui->pc_LabelSafeFileNone->setVisible(false);
                  this->mpc_Ui->pc_LabelSafeFileValue->setVisible(true);
               }

               if (pc_Node->c_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eONE_LEVEL_ALL_SAFE)
               {
                  // no non-safe file
                  this->mpc_Ui->pc_LabelNonSafeFileValue->setVisible(false);
                  this->mpc_Ui->pc_LabelNonSafeFileNone->setVisible(true);
               }
               else
               {
                  this->mpc_Ui->pc_LabelNonSafeFileNone->setVisible(false);
                  this->mpc_Ui->pc_LabelNonSafeFileValue->setVisible(true);
               }
            }
         }

         this->mpc_Ui->pc_CheckBoxFileGen->setEnabled(q_FileGenEnabled);

         if (q_FileGenEnabled == true)
         {
            if (pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].q_ProgrammingSupport == true)
            {
               this->mpc_Ui->pc_CheckBoxFileGen->setText(C_GtGetText::h_GetText("File Generation (Source Code)"));
            }
            else
            {
               this->mpc_Ui->pc_CheckBoxFileGen->setText(
                  C_GtGetText::h_GetText("File Generation (Parameter Set Image)"));
            }
         }
         else
         {
            this->mpc_Ui->pc_CheckBoxFileGen->setText(C_GtGetText::h_GetText("File Generation"));
         }
      }

      // load Data Block stuff
      this->m_LoadDataBlock();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load from specified data block
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_LoadDataBlock(void)
{
   // get application / use default
   const C_OscNodeApplication * const pc_Appl =
      C_PuiSdHandler::h_GetInstance()->GetApplication(this->mu32_NodeIndex, this->ms32_ApplicationIndex);
   C_OscNodeApplication c_Default;

   if (this->ms32_ApplicationIndex < 0)
   {
      //default
      c_Default.c_Name = C_PuiSdHandler::h_GetInstance()->GetUniqueApplicationName(this->mu32_NodeIndex, "DataBlock");
      c_Default.u8_ProcessId = C_PuiSdHandler::h_GetInstance()->GetUniqueApplicationProcessId(this->mu32_NodeIndex, 0);
      c_Default.q_Active = true;
   }
   const C_OscNodeApplication & rc_Application = (pc_Appl != NULL) ? *pc_Appl : c_Default;

   //Section Name and Comment
   this->mpc_Ui->pc_LineEditName->setText(rc_Application.c_Name.c_str());
   this->mpc_Ui->pc_CommentText->setText(rc_Application.c_Comment.c_str());
   this->me_Type = rc_Application.e_Type;
   this->mpc_Ui->pc_CheckBoxFileGen->setChecked(rc_Application.e_Type != C_OscNodeApplication::eBINARY);

   //Section Content (order is relevant: first set project path and then output file and gen. source code directory)
   this->mpc_Ui->pc_LineEditProject->SetPath(rc_Application.c_ProjectPath.c_str(),
                                             C_PuiProject::h_GetInstance()->GetFolderPath());
   this->mpc_Ui->pc_LineEditFileGenerate->SetPath(rc_Application.c_ProjectPath.c_str(),
                                                  C_PuiProject::h_GetInstance()->GetFolderPath());
   const QString c_ProjectPath =
      C_PuiUtil::h_GetResolvedAbsPathFromProject(this->mpc_Ui->pc_LineEditProject->GetPath());
   this->mpc_Ui->pc_SpinBoxProcessID->setValue(rc_Application.u8_ProcessId);
   this->mpc_Ui->pc_LineEditOutputFile->SetDbProjectPath(c_ProjectPath);
   this->m_LoadOutputFilePaths(rc_Application, c_ProjectPath);
   this->mpc_Ui->pc_LineEditIDE->setText(rc_Application.c_IdeCall.c_str());
   this->mpc_Ui->pc_LineEditCodeGenerator->SetDbProjectPath(c_ProjectPath);
   this->mpc_Ui->pc_LineEditCodeGenerator->SetPath(rc_Application.c_CodeGeneratorPath.c_str(), C_Uti::h_GetExePath());
   this->mpc_Ui->pc_LineEditCodeGenerate->SetDbProjectPath(c_ProjectPath);
   this->mpc_Ui->pc_LineEditCodeGenerate->SetPath(rc_Application.c_GeneratePath.c_str(), c_ProjectPath);
   this->mpc_Ui->pc_LineEditIDE->SetDbProjectPath(c_ProjectPath);
   this->m_UpdatePathsRelativeToGeneratedDir(); // after setting output labels text and file generation directory!
   switch (rc_Application.u16_GenCodeVersion)
   {
   case 1U:
      this->mpc_Ui->pc_ComboBoxCode->setCurrentIndex(mhs32_VERSION_INDEX_V1);
      break;
   case 2U:
      this->mpc_Ui->pc_ComboBoxCode->setCurrentIndex(mhs32_VERSION_INDEX_V2);
      break;
   case 3U:
      this->mpc_Ui->pc_ComboBoxCode->setCurrentIndex(mhs32_VERSION_INDEX_V3);
      break;
   case 4U:
      this->mpc_Ui->pc_ComboBoxCode->setCurrentIndex(mhs32_VERSION_INDEX_V4);
      break;
   case 5U:
      this->mpc_Ui->pc_ComboBoxCode->setCurrentIndex(mhs32_VERSION_INDEX_V5);
      break;
   case 6U:
      this->mpc_Ui->pc_ComboBoxCode->setCurrentIndex(mhs32_VERSION_INDEX_V6);
      break;
   default:
      // latest is greatest
      this->mpc_Ui->pc_ComboBoxCode->setCurrentIndex(mhs32_VERSION_INDEX_V6);
      break;
   }

   this->m_SetVisibilityOfContentWidgets(rc_Application.e_Type);

   //Section Datapools
   this->m_InitDataPoolsSection();

   // initial check
   this->m_CheckName();
   this->m_CheckId();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show output file paths in corresponding label / line edit

   \param[in]  orc_Application   Application
   \param[in]  orc_ProjectPath   Project path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_LoadOutputFilePaths(const C_OscNodeApplication & orc_Application,
                                                const QString & orc_ProjectPath)
{
   // get HALC information (necessary for PSI file generation)
   const C_OscHalcConfig * const pc_Halc = C_PuiSdHandler::h_GetInstance()->GetHalcConfig(this->mu32_NodeIndex);

   switch (orc_Application.e_Type)
   {
   case C_OscNodeApplication::eBINARY:
   case C_OscNodeApplication::ePROGRAMMABLE_APPLICATION:
      if (orc_Application.c_ResultPaths.size() > 0) // defensive check
      {
         this->mpc_Ui->pc_LineEditOutputFile->SetPath(orc_Application.c_ResultPaths[0].c_str(), orc_ProjectPath);
      }
      break;
   case C_OscNodeApplication::ePARAMETER_SET_HALC:
      if (pc_Halc != NULL)
      {
         QString c_SafeFile = "";
         QString c_NonSafeFile = "";
         switch (pc_Halc->e_SafetyMode)
         {
         case C_OscHalcDefBase::eONE_LEVEL_ALL_SAFE:
            if (orc_Application.c_ResultPaths.size() > 0) // defensive check
            {
               c_SafeFile = orc_Application.c_ResultPaths[0].c_str();
            }
            break;
         case C_OscHalcDefBase::eONE_LEVEL_ALL_NON_SAFE:
            if (orc_Application.c_ResultPaths.size() > 0) // defensive check
            {
               c_NonSafeFile = orc_Application.c_ResultPaths[0].c_str();
            }
            break;
         case C_OscHalcDefBase::eTWO_LEVELS_WITH_DROPPING:
         case C_OscHalcDefBase::eTWO_LEVELS_WITHOUT_DROPPING:
            if (orc_Application.c_ResultPaths.size() > 1) // defensive check
            {
               c_SafeFile = orc_Application.c_ResultPaths[0].c_str();
               c_NonSafeFile = orc_Application.c_ResultPaths[1].c_str();
            }
            break;
         default:
            tgl_assert(false);
            break;
         }
         this->mpc_Ui->pc_LabelSafeFileValue->setText(c_SafeFile);
         this->mpc_Ui->pc_LabelNonSafeFileValue->setText(c_NonSafeFile);
      }
      break;
   default:
      tgl_assert(false);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set visibility of properties that are different for each Data Block type

   \param[in]  ore_Type    Type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_SetVisibilityOfContentWidgets(const C_OscNodeApplication::E_Type & ore_Type) const
{
   // first set all invisible and then all visible items
   switch (ore_Type)
   {
   case C_OscNodeApplication::ePROGRAMMABLE_APPLICATION:
      this->mpc_Ui->pc_LabelFileGenerationDisabled->setVisible(false);
      this->mpc_Ui->pc_WidgetPsiFiles->setVisible(false);
      this->mpc_Ui->pc_PushButtonAddDataPool->setVisible(true);
      this->mpc_Ui->pc_GroupBoxProject->setVisible(true);
      this->mpc_Ui->pc_WidgetFileGenContent->setVisible(true);
      this->mpc_Ui->pc_WidgetCode->setVisible(true);
      break;
   case C_OscNodeApplication::eBINARY:
      this->mpc_Ui->pc_WidgetFileGenContent->setVisible(false);
      this->mpc_Ui->pc_GroupBoxProject->setVisible(true);
      this->mpc_Ui->pc_LabelFileGenerationDisabled->setVisible(true);
      break;
   case C_OscNodeApplication::ePARAMETER_SET_HALC:
      this->mpc_Ui->pc_GroupBoxProject->setVisible(false);
      this->mpc_Ui->pc_WidgetCode->setVisible(false);
      this->mpc_Ui->pc_LabelFileGenerationDisabled->setVisible(false);
      this->mpc_Ui->pc_PushButtonAddDataPool->setVisible(false);
      this->mpc_Ui->pc_WidgetFileGenContent->setVisible(true);
      this->mpc_Ui->pc_WidgetPsiFiles->setVisible(true);
      break;
   default:
      tgl_assert(false);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On file generation checked

   \param[in]  os32_State   State
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnFileGenerationChanged(const int32_t os32_State)
{
   // reset
   if (this->me_Type == C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
   {
      mrc_ParentDialog.SetSize(C_SdNdeDbProperties::h_GetBinaryWindowSize());
   }
   else
   {
      mrc_ParentDialog.SetSize(C_SdNdeDbProperties::h_GetDefaultWindowSize());
   }

   this->me_Type = C_OscNodeApplication::eBINARY;

   if (static_cast<Qt::CheckState>(os32_State) == Qt::Checked)
   {
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

      if (pc_Node != NULL)
      {
         // file generation depends on programmable flag and loaded hardware configuration
         if ((pc_Node->pc_DeviceDefinition != NULL) &&
             (pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size()))
         {
            bool q_FileGenerationType = false;
            if (pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].q_ProgrammingSupport == true)
            {
               this->me_Type = C_OscNodeApplication::ePROGRAMMABLE_APPLICATION;
               q_FileGenerationType = true;
            }
            else
            {
               if ((pc_Node->c_HalcConfig.q_NvmBasedConfig == true) && (pc_Node->c_HalcConfig.IsClear() == false))
               {
                  // set type
                  this->me_Type = C_OscNodeApplication::ePARAMETER_SET_HALC;
                  q_FileGenerationType = true;

                  // set default file names
                  if (this->mpc_Ui->pc_LabelSafeFileValue->text() == "")
                  {
                     // only if safe data is allowed
                     if (pc_Node->c_HalcConfig.e_SafetyMode != C_OscHalcDefBase::eONE_LEVEL_ALL_NON_SAFE)
                     {
                        this->mpc_Ui->pc_LabelSafeFileValue->setText("halc_parameters_safe.syde_psi");
                     }
                  }
                  if (this->mpc_Ui->pc_LabelNonSafeFileValue->text() == "")
                  {
                     // only if non-safe data is allowed
                     if (pc_Node->c_HalcConfig.e_SafetyMode != C_OscHalcDefBase::eONE_LEVEL_ALL_SAFE)
                     {
                        this->mpc_Ui->pc_LabelNonSafeFileValue->setText("halc_parameters_non_safe.syde_psi");
                     }
                  }

                  // set default generation directory if empty
                  if (this->mpc_Ui->pc_LineEditFileGenerate->GetPath().isEmpty() == true)
                  {
                     this->mpc_Ui->pc_LineEditFileGenerate->SetPath(
                        C_ImpUtil::h_GetDefaultGeneratedDir(this->mpc_Ui->pc_LineEditName->text().toStdString().c_str(),
                                                            pc_Node->c_Properties.c_Name));
                  }

                  // update for tooltips
                  this->m_UpdatePathsRelativeToGeneratedDir(); // after setting label text and generation directory!

                  // add all NVM HALC Datapools for displaying
                  // those can be kept even if type is changed back again - they will be removed on dialog ok click
                  for (uint32_t u32_ItDataPool = 0UL; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
                  {
                     const C_OscNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
                     if (rc_DataPool.e_Type == C_OscNodeDataPool::eHALC_NVM)
                     {
                        this->mc_SelectedDataPools.insert(u32_ItDataPool);
                     }
                  }
                  this->m_InitDataPoolsSection();
               }
            }

            if (q_FileGenerationType == true)
            {
               // revert file generator if none is set
               if (this->mpc_Ui->pc_LineEditCodeGenerator->GetPath() == "")
               {
                  this->m_HandleRevertCodeGenerator();
               }
            }
         }
      }
   }

   this->m_SetVisibilityOfContentWidgets(this->me_Type);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle click on browse button for "Project Path" section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnClickProject(void)
{
   const QString c_FolderName =
      this->m_GetDialogPath(C_PuiUtil::h_GetResolvedAbsPathFromProject(this->mpc_Ui->pc_LineEditProject->GetPath()));

   const QString c_Path =
      QFileDialog::getExistingDirectory(this, C_GtGetText::h_GetText("Select Target Project Location"),
                                        c_FolderName, QFileDialog::ShowDirsOnly);

   this->m_AskUserToSaveRelativePath(c_Path,
                                     C_PuiProject::h_GetInstance()->GetFolderPath(), this->mpc_Ui->pc_LineEditProject);

   if (this->mpc_Ui->pc_LineEditProject->GetPath() != "")
   {
      // update tooltips of line edits with paths relative to project
      this->m_UpdatePathsRelativeToProject();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle a dropped url on "Project Path" section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnDroppedProject(void)
{
   this->m_AskUserToSaveRelativePath(this->mpc_Ui->pc_LineEditProject->GetPath(),
                                     C_PuiProject::h_GetInstance()->GetFolderPath(), this->mpc_Ui->pc_LineEditProject);
   if (this->mpc_Ui->pc_LineEditProject->GetPath() != "")
   {
      // update tooltips of line edits with paths relative to project
      this->m_UpdatePathsRelativeToProject();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle click on browse button for "Output File" section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnClickOutput(void)
{
   const QString c_ProjectPath =
      C_PuiUtil::h_GetResolvedAbsPathFromProject(this->mpc_Ui->pc_LineEditProject->GetPath());
   const QString c_FolderName = this->m_GetDialogPath(
      C_PuiUtil::h_GetResolvedAbsPathFromDbProject(c_ProjectPath, this->mpc_Ui->pc_LineEditOutputFile->GetPath()));
   const QString c_FilterName = static_cast<QString>(C_GtGetText::h_GetText("HEX file (*.hex);;Others (*.*)"));

   // Do not use QFileDialog::getOpenFileName because it does not support default suffix
   // File path check is done by h_AskUserToSaveRelativePath(), so no need to use C_OgeWiUtil::h_GetOpenFileName()
   QFileDialog c_Dialog(this,  C_GtGetText::h_GetText("Select Output File"), c_FolderName, c_FilterName);

   c_Dialog.setDefaultSuffix("hex");

   if (c_Dialog.exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      QString c_FilePath;
      c_FilePath = c_Dialog.selectedFiles().at(0); // multi-selection is not possible

      // check if relative path is possible and appreciated
      this->m_AskUserToSaveRelativePath(c_FilePath, c_ProjectPath, this->mpc_Ui->pc_LineEditOutputFile);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle a dropped url on "Output File" section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnDroppedOutput(void)
{
   const QString c_ProjectPath =
      C_PuiUtil::h_GetResolvedAbsPathFromProject(this->mpc_Ui->pc_LineEditProject->GetPath());

   this->m_AskUserToSaveRelativePath(this->mpc_Ui->pc_LineEditOutputFile->GetPath(),
                                     c_ProjectPath, this->mpc_Ui->pc_LineEditOutputFile);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle click on browse button for "Generator" section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnClickGenerator(void)
{
   const QString c_ProjectPath =
      C_PuiUtil::h_GetResolvedAbsPathFromProject(this->mpc_Ui->pc_LineEditProject->GetPath());
   const QString c_FolderName =
      C_PuiUtil::h_GetResolvedAbsPathFromExe(this->mpc_Ui->pc_LineEditCodeGenerator->GetPath(), c_ProjectPath);
   // if generator line edit is empty this results in executable path - we use this as feature

   const QString c_FilterName = static_cast<QString>(C_GtGetText::h_GetText("Executable (*.exe *.bat);;Others (*.*)"));

   // File path check is done by h_AskUserToSaveRelativePath(), so no need to use C_OgeWiUtil::h_GetOpenFileName()
   const QString c_FilePath = QFileDialog::getOpenFileName(this, C_GtGetText::h_GetText("Select Generator"),
                                                           c_FolderName, c_FilterName, NULL);

   this->m_AskUserToSaveRelativePath(c_FilePath, C_Uti::h_GetExePath(), this->mpc_Ui->pc_LineEditCodeGenerator);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle a dropped url on "Generator" section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnDroppedGenerator(void)
{
   this->m_AskUserToSaveRelativePath(this->mpc_Ui->pc_LineEditCodeGenerator->GetPath(),
                                     C_Uti::h_GetExePath(), this->mpc_Ui->pc_LineEditCodeGenerator);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle click on browse button for "Generation Directory" section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnClickCodeGeneration(void)
{
   const QString c_ProjectPath =
      C_PuiUtil::h_GetResolvedAbsPathFromProject(this->mpc_Ui->pc_LineEditProject->GetPath());
   const QString c_FolderName = this->m_GetDialogPath(
      C_PuiUtil::h_GetResolvedAbsPathFromDbProject(c_ProjectPath, this->mpc_Ui->pc_LineEditCodeGenerate->GetPath()));

   const QString c_Path =
      QFileDialog::getExistingDirectory(this, C_GtGetText::h_GetText("Select Directory for Generated Files"),
                                        c_FolderName, QFileDialog::ShowDirsOnly);

   this->m_AskUserToSaveRelativePath(c_Path, c_ProjectPath, this->mpc_Ui->pc_LineEditCodeGenerate);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle a dropped url on "Generation Directory" section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnDroppedCodeGeneration(void)
{
   const QString c_ProjectPath =
      C_PuiUtil::h_GetResolvedAbsPathFromProject(this->mpc_Ui->pc_LineEditProject->GetPath());

   this->m_AskUserToSaveRelativePath(this->mpc_Ui->pc_LineEditCodeGenerate->GetPath(),
                                     c_ProjectPath, this->mpc_Ui->pc_LineEditCodeGenerate);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle click on browse button for file generator output directory section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnClickFileGeneration(void)
{
   const QString c_FolderName = this->m_GetDialogPath(
      C_PuiUtil::h_GetResolvedAbsPathFromProject(this->mpc_Ui->pc_LineEditFileGenerate->GetPath()));

   const QString c_Path =
      QFileDialog::getExistingDirectory(this, C_GtGetText::h_GetText("Select Directory for Generated Files"),
                                        c_FolderName, QFileDialog::ShowDirsOnly);

   this->m_AskUserToSaveRelativePath(c_Path,
                                     C_PuiProject::h_GetInstance()->GetFolderPath(),
                                     this->mpc_Ui->pc_LineEditFileGenerate);

   if (this->mpc_Ui->pc_LineEditFileGenerate->GetPath() != "")
   {
      // update tooltips of line edits with paths relative to project
      this->m_UpdatePathsRelativeToGeneratedDir();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle a dropped url on "file generator output directory" section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnDroppedFileGeneration(void)
{
   this->m_AskUserToSaveRelativePath(this->mpc_Ui->pc_LineEditFileGenerate->GetPath(),
                                     C_PuiProject::h_GetInstance()->GetFolderPath(),
                                     this->mpc_Ui->pc_LineEditFileGenerate);
   if (this->mpc_Ui->pc_LineEditFileGenerate->GetPath() != "")
   {
      // update tooltips of line edits with paths relative to project
      this->m_UpdatePathsRelativeToGeneratedDir();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle click on browse button for "IDE Call" section.

   Replaces current IDE call with selected executable path. It is ok to eventually forget flags.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnClickIde(void)
{
   const QString c_ProjectPath =
      C_PuiUtil::h_GetResolvedAbsPathFromProject(this->mpc_Ui->pc_LineEditProject->GetPath());
   const QString c_FilterName = static_cast<QString>(C_GtGetText::h_GetText("Executable (*.exe);;Others (*.*)"));
   QString c_IdeCall =
      static_cast<QString>(C_PuiUtil::h_ResolvePlaceholderVariables(this->mpc_Ui->pc_LineEditIDE->text(),
                                                                    c_ProjectPath));
   QString c_Path;
   QString c_FolderName;
   QStringList c_SplittedString;

   // get path from command line call for browse location start
   if (c_IdeCall.startsWith('\"'))
   {
      c_SplittedString = c_IdeCall.split('\"', Qt::SplitBehaviorFlags::SkipEmptyParts);
      if (c_SplittedString.size() > 0)
      {
         c_IdeCall = c_SplittedString[0];
      }
   }
   else
   {
      c_SplittedString = c_IdeCall.split(' ');
      if (c_SplittedString.size() > 0)
      {
         c_IdeCall = c_SplittedString[0];
      }
   }

   c_FolderName = this->m_GetDialogPath(c_IdeCall);
   // File path check is done by h_AskUserToSaveRelativePath(), so no need to use C_OgeWiUtil::h_GetOpenFileName()
   c_Path = QFileDialog::getOpenFileName(this, C_GtGetText::h_GetText("Select IDE Executable"), c_FolderName,
                                         c_FilterName, NULL);

   if (c_Path != "")
   {
      // put quotation marks around path for command line call handling of blanks in paths
      if (c_Path.contains(' '))
      {
         c_Path = "\"" + c_Path + "\"";
      }
      this->mpc_Ui->pc_LineEditIDE->setText(c_Path);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for text changed signal of line edit name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnNameEdited(void) const
{
   const QString c_ErrorText = this->m_CheckName();
   const bool q_Error = (c_ErrorText == "");

   //set valid text property flag
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditName, "Valid", q_Error);

   if (q_Error)
   {
      this->mpc_Ui->pc_LineEditName->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                           C_GtGetText::h_GetText(""),
                                                           C_NagToolTip::eDEFAULT);
   }
   else
   {
      const QString c_Heading = C_GtGetText::h_GetText("Data Block Name");
      this->mpc_Ui->pc_LineEditName->SetToolTipInformation(c_Heading, c_ErrorText, C_NagToolTip::eERROR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for value changed signal of process ID spin box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnProcessIdChanged(void) const
{
   const QString c_ErrorText = this->m_CheckId();
   const bool q_Error = (c_ErrorText == "");

   //set valid text property flag
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxProcessID, "Valid", q_Error);

   if (q_Error)
   {
      this->mpc_Ui->pc_SpinBoxProcessID->SetToolTipAdditionalInfo("", C_NagToolTip::eDEFAULT);
   }
   else
   {
      this->mpc_Ui->pc_SpinBoxProcessID->SetToolTipAdditionalInfo(c_ErrorText, C_NagToolTip::eERROR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up all data pool widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_CleanUpDataPoolWidgets(void)
{
   for (std::vector<C_SdNdeDbDataPoolEntry *>::const_iterator c_It = this->mc_DataPoolWidgets.begin();
        c_It != this->mc_DataPoolWidgets.end(); ++c_It)
   {
      C_SdNdeDbDataPoolEntry * const pc_Entry = *c_It;
      if (pc_Entry != NULL)
      {
         this->mpc_Ui->pc_VerticalLayoutDataPools->removeWidget(pc_Entry);
         pc_Entry->hide();
      }
   }
   this->mc_DataPoolWidgets.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update owned datapools count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_UpdateOwnedDpsCount(void) const
{
   //update owned dp count
   this->mpc_Ui->pc_LabelHeadingDatapools->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                           "Owned Datapools (%1)")).arg(
                                                      this->mc_SelectedDataPools.size()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update tool tips for paths that are relative to project path

   Slot for project path editing finished signal.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_UpdatePathsRelativeToProject(void) const
{
   const QString c_ProjectPath =
      C_PuiUtil::h_GetResolvedAbsPathFromProject(this->mpc_Ui->pc_LineEditProject->GetPath());
   QString c_Temp;

   // output file path
   c_Temp = this->mpc_Ui->pc_LineEditOutputFile->GetPath();
   this->mpc_Ui->pc_LineEditOutputFile->SetDbProjectPath(c_ProjectPath);
   this->mpc_Ui->pc_LineEditOutputFile->SetPath(c_Temp, c_ProjectPath);

   // generated source code directory
   c_Temp = this->mpc_Ui->pc_LineEditCodeGenerate->GetPath();
   this->mpc_Ui->pc_LineEditCodeGenerate->SetDbProjectPath(c_ProjectPath);
   this->mpc_Ui->pc_LineEditCodeGenerate->SetPath(c_Temp, c_ProjectPath);

   // file generator (not relative but might contain Data Block path as placeholder variable)
   c_Temp = this->mpc_Ui->pc_LineEditCodeGenerator->GetPath();
   this->mpc_Ui->pc_LineEditCodeGenerator->SetDbProjectPath(c_ProjectPath);
   this->mpc_Ui->pc_LineEditCodeGenerator->SetPath(c_Temp, C_Uti::h_GetExePath());

   // IDE call path (for resolving only)
   this->mpc_Ui->pc_LineEditIDE->SetDbProjectPath(c_ProjectPath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update tool tips for paths that are relative to generated files directory

   Slot for generation directory path editing finished signal.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_UpdatePathsRelativeToGeneratedDir(void) const
{
   const QString c_ProjectPath =
      C_PuiUtil::h_GetResolvedAbsPathFromProject(this->mpc_Ui->pc_LineEditFileGenerate->GetPath());

   this->mpc_Ui->pc_LabelSafeFileValue->
   SetToolTipInformation(C_GtGetText::h_GetText("Safe File Value"),
                         C_Uti::h_ConcatPathIfNecessary(c_ProjectPath, this->mpc_Ui->pc_LabelSafeFileValue->text()));
   this->mpc_Ui->pc_LabelNonSafeFileValue->
   SetToolTipInformation(C_GtGetText::h_GetText("Non Safe File Value"),
                         C_Uti::h_ConcatPathIfNecessary(c_ProjectPath, this->mpc_Ui->pc_LabelNonSafeFileValue->text()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize the data pools section (based on member variable)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_InitDataPoolsSection(void)
{
   m_CleanUpDataPoolWidgets();
   int32_t s32_Count = 0;
   for (std::set<uint32_t>::const_iterator c_It = this->mc_SelectedDataPools.begin();
        c_It != this->mc_SelectedDataPools.end(); ++c_It)
   {
      const uint32_t u32_Val = *c_It;
      C_SdNdeDbDataPoolEntry * const pc_Entry = new C_SdNdeDbDataPoolEntry(this->mu32_NodeIndex, u32_Val, this);

      connect(pc_Entry, &C_SdNdeDbDataPoolEntry::SigDeleteDataPool, this, &C_SdNdeDbProperties::m_HandleDeleteDataPool);

      this->mpc_Ui->pc_VerticalLayoutDataPools->insertWidget(s32_Count, pc_Entry);
      ++s32_Count;
      this->mc_DataPoolWidgets.push_back(pc_Entry);
   } //lint !e429  //no memory leak because of the parent of pc_Entry and the Qt memory management
   if (this->mc_SelectedDataPools.size() > 0UL)
   {
      this->mpc_Ui->pc_LabelDataPoolsEmpty->setVisible(false);
      this->mpc_Ui->pc_GroupBoxDataPoolsNotEmpty->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxDataPoolsNotEmpty->setVisible(false);
      this->mpc_Ui->pc_LabelDataPoolsEmpty->setVisible(true);
   }

   //update owned dp count
   this->m_UpdateOwnedDpsCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle add data pools operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_HandleAddDataPools(void)
{
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   const std::vector<uint32_t> c_SelectedDatapools(this->mc_SelectedDataPools.begin(),
                                                   this->mc_SelectedDataPools.end());
   const C_SdNdeDbSelectDataPools * const pc_Dialog = new C_SdNdeDbSelectDataPools(this->mu32_NodeIndex,
                                                                                   this->ms32_ApplicationIndex,
                                                                                   c_SelectedDatapools, *c_New);

   //Resize
   c_New->SetSize(QSize(800, 800));

   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      const std::vector<C_PuiSvDbNodeDataPoolListElementId> c_SelectedDataPools = pc_Dialog->GetSelectedDataPools();
      for (uint32_t u32_It = 0UL; u32_It < c_SelectedDataPools.size(); ++u32_It)
      {
         const C_PuiSvDbNodeDataPoolListElementId & rc_CurItem = c_SelectedDataPools[u32_It];
         this->mc_SelectedDataPools.insert(rc_CurItem.u32_DataPoolIndex);
      }
      m_InitDataPoolsSection();
   }

   if (c_New != NULL)
   {
      c_New->HideOverlay();
      c_New->deleteLater();
   }
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle revert to default file generator action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_HandleRevertCodeGenerator(void) const
{
   this->mpc_Ui->pc_LineEditCodeGenerator->SetPath(C_ImpUtil::h_GetSydeCoderCePath(), C_Uti::h_GetExePath());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle delete datapool entry

   \param[in,out]  opc_Source    Source widget
   \param[in]      ou32_Index    Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_HandleDeleteDataPool(C_SdNdeDbDataPoolEntry * const opc_Source, const uint32_t ou32_Index)
{
   if (opc_Source != NULL)
   {
      for (std::set<uint32_t>::iterator c_It = this->mc_SelectedDataPools.begin();
           c_It != this->mc_SelectedDataPools.end(); ++c_It)
      {
         if (*c_It == ou32_Index)
         {
            this->mc_SelectedDataPools.erase(c_It);
            break;
         }
      }
      for (std::vector<C_SdNdeDbDataPoolEntry *>::iterator c_It = this->mc_DataPoolWidgets.begin();
           c_It != this->mc_DataPoolWidgets.end(); ++c_It)
      {
         if (*c_It == opc_Source)
         {
            this->mc_DataPoolWidgets.erase(c_It);
            break;
         }
      }
      this->mpc_Ui->pc_VerticalLayoutDataPools->removeWidget(opc_Source);
      opc_Source->hide();
      if (this->mc_SelectedDataPools.size() == 0UL)
      {
         this->mpc_Ui->pc_GroupBoxDataPoolsNotEmpty->setVisible(false);
         this->mpc_Ui->pc_LabelDataPoolsEmpty->setVisible(true);
      }
   }

   //update owned dp count
   this->m_UpdateOwnedDpsCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Short function description

   Detailed function description (optional). The function shall be described detailed if it is not described completely
   by short function description and parameter description.

   \param[in]       ou8_Aa     Detailed input parameter description
   \param[out]      opu32_Bb   Detailed output parameter description
   \param[in,out]   opu16_Cc   Detailed input/output parameter description

   \return
   Type of return values, e.g. STW error codes

   \retval   Return value 1   Detailed description of 1st return value
   \retval   Return value 2   Detailed description of 2nd return value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_AskUserToSaveRelativePath(const QString & orc_Path,
                                                      const QString & orc_AbsoluteReferenceDir,
                                                      stw::opensyde_gui_elements::C_OgeLeFilePath * const opc_PathLineEdit)
{
   if (orc_Path != "")
   {
      // check if relative path is possible and appreciated
      const QString c_AdaptedPath = C_ImpUtil::h_AskUserToSaveRelativePath(this, orc_Path, orc_AbsoluteReferenceDir);

      if (c_AdaptedPath != "")
      {
         tgl_assert(opc_PathLineEdit != NULL);
         if (opc_PathLineEdit != NULL)
         {
            opc_PathLineEdit->SetPath(c_AdaptedPath, orc_AbsoluteReferenceDir);
         }
      }
   }
}
