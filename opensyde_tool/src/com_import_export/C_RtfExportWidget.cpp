//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for RTF file export (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>
#include <fstream>
#include <QFileInfo>
#include <QDir>
#include <QGraphicsView>
#include <QScreen>
#include <QProcess>
#include <QFileDialog>
#include <QDateTime>
//#include <cstdio> // do not need to include because of indirect include,
// otherwise PCLint warning 451 repeatly included
#include "TGLUtils.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_OSCUtils.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_OSCLoggingHandler.h"
#include "C_RtfExportWidget.h"
#include "TGLFile.h"
#include "C_OSCXMLParser.h"
#include "C_OSCProject.h"
#include "C_PuiProject.h"
#include "C_Uti.h"
#include "C_PuiUtil.h"
#include "C_SdTopologyWidget.h"
#include "C_OgeWiCustomMessage.h"
#include "ui_C_RtfExportWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_scl;
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

   \param[in,out] orc_Parent          Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_RtfExportWidget::C_RtfExportWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_RtfExportWidget),
   mrc_ParentDialog(orc_Parent)
{
   mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // Remove "..." string
   this->mpc_Ui->pc_PushButtonLogoPath->setText("");
   this->mpc_Ui->pc_PushButtonRtfPath->setText("");

   // connects
   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this,
           &C_RtfExportWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this,
           &C_RtfExportWidget::m_CancelClicked);
   connect(this->mpc_Ui->pc_PushButtonRtfPath, &QPushButton::clicked, this,
           &C_RtfExportWidget::m_RtfPathClicked);
   connect(this->mpc_Ui->pc_PushButtonLogoPath, &QPushButton::clicked, this,
           &C_RtfExportWidget::m_LogoPathClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_RtfExportWidget::~C_RtfExportWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_RtfExportWidget::InitStaticNames(void) const
{
   // set main title
   this->mrc_ParentDialog.SetTitle(QString(C_GtGetText::h_GetText("SYSTEM DEFINITION")));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Report"));

   // labels and buttons
   this->mpc_Ui->pc_LabelHeadingReport->setText(C_GtGetText::h_GetText("Report Settings"));
   this->mpc_Ui->pc_LabelPath->setText(C_GtGetText::h_GetText("Path"));
   this->mpc_Ui->pc_LabelCompany->setText(C_GtGetText::h_GetText("Company Name"));
   this->mpc_Ui->pc_LabelLogo->setText(C_GtGetText::h_GetText("Company Logo"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Generate Report"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));

   // set hint for optional entries to appropriate labels
   this->mpc_Ui->pc_EditCompany->setPlaceholderText(C_GtGetText::h_GetText("optional"));
   this->mpc_Ui->pc_EditLogoPath->setPlaceholderText(C_GtGetText::h_GetText("optional"));

   // Tool tips
   this->mpc_Ui->pc_LabelPath->SetToolTipInformation(
      C_GtGetText::h_GetText("RTF File Path"),
      C_GtGetText::h_GetText("Full path to save RTF file documentation (*.rtf)."));

   this->mpc_Ui->pc_LabelCompany->SetToolTipInformation(
      C_GtGetText::h_GetText("Company Name"),
      C_GtGetText::h_GetText("The company name for that the RTF file export is created for.\n(optional parameter)"));

   this->mpc_Ui->pc_LabelLogo->SetToolTipInformation(
      C_GtGetText::h_GetText("Company Logo"),
      C_GtGetText::h_GetText(
         "Location of the file containing the logo of the company for that the RTF file export is created for.\n"
         "File format is JPG or PNG. \n(optional parameter)"));

   this->mpc_Ui->pc_PushButtonLogoPath->SetToolTipInformation(
      C_GtGetText::h_GetText("Browse"),
      C_GtGetText::h_GetText("Browse for company logo file."));
   this->mpc_Ui->pc_PushButtonRtfPath->SetToolTipInformation(
      C_GtGetText::h_GetText("Browse"),
      C_GtGetText::h_GetText("Browse for export path."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Getter of full RTF file path for DocuCreator.

   Mandatory parameter.

   \param[out]    orc_RtfPath    RTF file path for DocuCreator

   \return
   C_NO_ERR    valid RTF file path
   C_CONFIG    directory does not exists
               missing path (empty string "")
   C_NOACT     invalid file format (not .rtf)
   C_WARN      file already exists
   C_CHECKSUM  file name invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_RtfExportWidget::GetRtfPath(C_SCLString & orc_RtfPath) const
{
   sint32 s32_Return = C_CONFIG;

   // get full RTF path of widget Ui
   orc_RtfPath = C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_EditRtfPath->GetPath()).toStdString().c_str();

   // check if directory exists
   if (TGL_DirectoryExists(TGL_ExtractFilePath(orc_RtfPath)) == true)
   {
      // check if file name is valid
      C_SCLString c_FileExtAct = TGL_ExtractFileExtension(orc_RtfPath);
      if (c_FileExtAct.LowerCase() == ".rtf")
      {
         QFileInfo c_Info(orc_RtfPath.c_str());
         if (C_OSCUtils::h_CheckValidCName(c_Info.baseName().toStdString().c_str(),
                                           std::numeric_limits<uint16>::max()))
         {
            if (TGL_FileExists(orc_RtfPath) == true)
            {
               s32_Return = C_WARN;
            }
            else
            {
               s32_Return = C_NO_ERR;
            }
         }
         else
         {
            s32_Return = C_CHECKSUM;
         }
      }
      else
      {
         s32_Return = C_NOACT;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Getter of customer company name for RTF DocuCreator.

   Optional parameter:
   e.g. Sensor-Technik Wiedemann GmbH

   \param[out]    orc_CompanyName    company name (empty string "" if not available)

   \return
   C_NO_ERR    in any case (all signs are allowed)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_RtfExportWidget::GetCompanyName(C_SCLString & orc_CompanyName) const
{
   // get company name of widget Ui
   orc_CompanyName = this->mpc_Ui->pc_EditCompany->text().toStdString().c_str();

   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Getter of full company logo path for DocuCreator.

   \param[out]    orc_CompanyLogoPath    Company logo path for DocuCreator (empty string "" if not available)

   \return
   C_NO_ERR    valid RTF file path or empty string
   C_CONFIG    file does not exist
   C_NOACT     invalid file format (not .jpg or .png)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_RtfExportWidget::GetCompanyLogoPath(C_SCLString & orc_CompanyLogoPath) const
{
   sint32 s32_Return = C_CONFIG;

   // get company logo path
   orc_CompanyLogoPath = this->mpc_Ui->pc_EditLogoPath->GetPath().toStdString().c_str();

   if (orc_CompanyLogoPath != "")
   {
      // make absolute path if necessary
      orc_CompanyLogoPath = C_PuiUtil::h_GetAbsolutePathFromProject(orc_CompanyLogoPath.c_str()).toStdString().c_str();

      // check if file exists
      if (TGL_FileExists(orc_CompanyLogoPath) == true)
      {
         // check if file name is valid
         C_SCLString c_FileExtAct = TGL_ExtractFileExtension(orc_CompanyLogoPath);
         if ((c_FileExtAct.LowerCase() == ".jpg") || (c_FileExtAct.LowerCase() == ".png"))
         {
            s32_Return = C_NO_ERR;
         }
         else
         {
            s32_Return = C_NOACT;
         }
      }
   }
   else
   {
      // optional parameter, empty string is OK
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setter for full RTF File Export path.

   \param[in]     orc_RtfPath    input parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_RtfExportWidget::SetRtfPath(const C_SCLString & orc_RtfPath) const
{
   this->mpc_Ui->pc_EditRtfPath->SetPath(orc_RtfPath.c_str(), C_PuiProject::h_GetInstance()->GetFolderPath());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setter for company name.

   \param[in]     orc_CompanyName    input parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_RtfExportWidget::SetCompanyName(const C_SCLString & orc_CompanyName) const
{
   this->mpc_Ui->pc_EditCompany->setText(orc_CompanyName.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setter for company logo path.

   \param[in]     orc_CompanyLogoPath    path to company logo
*/
//----------------------------------------------------------------------------------------------------------------------
void C_RtfExportWidget::SetCompanyLogoPath(const C_SCLString & orc_CompanyLogoPath) const
{
   this->mpc_Ui->pc_EditLogoPath->SetPath(orc_CompanyLogoPath.c_str(), C_PuiProject::h_GetInstance()->GetFolderPath());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Exports openSYDE system definition as RTF documentation file.

   Assumptions:
   * RTF path and optional company logo path is valid (approved by getters of this class)
   * read/write permission of given folders

   Result of function after successful execution:
   * RTF documentation file with content of the current system definition and screenshot of its 'Network Topology'.

   \param[in]     orc_RtfPath            full RTF file path
   \param[in]     orc_CompanyName        customer company name for RTF file export
   \param[in]     orc_CompanyLogoPath    full path of customer company logo (".jpg" or ".png")
   \param[in]     opc_Widget             access to topology widget for scene screenshot
   \param[out]    orc_WarningMessages    warning messages of RTF File Export (empty list if no warnings)
   \param[out]    orc_ErrorMessage       error message in case of failure (empty string if no error)

   \return
   C_NO_ERR    RTF File Export file successfully created
   C_WARN      some warnings occurred (see warning messages)
   C_NOACT     can't find external "DocuCreator" tool
   C_BUSY      could not save 'Network Topology' screenshot to disk
   C_UNKNOWN   Execution of 'DocuCreator' tool not successful (see error message)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_RtfExportWidget::ExportToRtf(const C_SCLString & orc_RtfPath, const C_SCLString & orc_CompanyName,
                                      const C_SCLString & orc_CompanyLogoPath, C_SdTopologyWidget * const opc_Widget,
                                      C_SCLStringList & orc_WarningMessages, C_SCLString & orc_ErrorMessage)
{
   sint32 s32_Return = C_NO_ERR;

   this->mc_Warnings.Clear(); // reset global warnings
   this->mc_Error = "";       //     -"-      error

   // get paths of 'DocuCreator' tool and its config XML file (which has to be created)
   QString c_DocuCreatorPath = C_Uti::h_GetExePath(); // get device ini location by project path
   QDir c_DirDocuCreatorTmp(c_DocuCreatorPath);
   tgl_assert(c_DirDocuCreatorTmp.cdUp() == true);                      // go one directory up
   c_DocuCreatorPath = c_DirDocuCreatorTmp.absolutePath();              // get current path
   c_DocuCreatorPath += "/connectors/DocuCreator/osy_docu_creator.exe"; // add DocuCreator location
   C_SCLString c_SclStringDocuCreatorPath = c_DocuCreatorPath.toStdString().c_str();
   C_SCLString c_SclStringDocuCreatorConfigPath = c_DirDocuCreatorTmp.absolutePath().toStdString().c_str();
   c_SclStringDocuCreatorConfigPath += "/connectors/DocuCreator/config.xml";

   QString c_PathNetworkTopologyScreenshot; // to save screenshot of network topology

   osc_write_log_info("RTF File Export", "Look for DocuCreator at path \"" + c_SclStringDocuCreatorPath + "\".");

   // check for path of external 'DocuCreator' tool
   if (TGL_FileExists(c_SclStringDocuCreatorPath) == false)
   {
      this->mc_Error = "Can't find external tool 'DocuCreator' for RTF File Export at path \"" +
                       c_SclStringDocuCreatorPath + "\".";
      osc_write_log_error("RTF File Export", this->mc_Error);
      s32_Return = C_NOACT;
   }

   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info("RTF File Export", "DocuCreator found, create screenshot of Network Topology ...");

      // create image of system definition view 'Network Topology'
      // get path, load graphic scene to image via painter and save to file
      c_PathNetworkTopologyScreenshot = c_DirDocuCreatorTmp.absolutePath();
      c_PathNetworkTopologyScreenshot += "/connectors/DocuCreator/network_topology.png";

      sintn sn_Factor; // to get better quality of image
      sintn sn_Width = static_cast<sintn>(opc_Widget->GetScene()->width());
      sintn sn_Height = static_cast<sintn>(opc_Widget->GetScene()->height());
      // factor values for pixels are empirical evaluated and a matter of taste
      // get longer side
      sintn sn_LongerSide = (sn_Width > sn_Height) ? sn_Width : sn_Height;
      sn_Factor = 6000 / sn_LongerSide; // a maximum of round about 6000 pixels for the longer side should be enough
      if (sn_Factor != 0)
      {
         sn_Width *= sn_Factor;
         sn_Height *= sn_Factor;
      }

      QSize c_ImageSize(sn_Width, sn_Height);
      QImage c_Image(c_ImageSize, QImage::Format_ARGB32);
      QPainter c_Painter(&c_Image);
      c_Painter.setRenderHint(QPainter::Antialiasing);
      opc_Widget->GetScene()->render(&c_Painter); // get scene

      //      // add white background to 'Network Topology' screenshot
      //      QImage c_Background(c_ImageSize, QImage::Format_RGB32);
      //      c_Background.fill(Qt::white); // white background for image
      //      c_Painter.setCompositionMode(QPainter::CompositionMode_DestinationAtop);
      //      c_Painter.drawImage(0, 0, c_Background);

      // save image, compression level is default
      if (c_Image.save(c_PathNetworkTopologyScreenshot) == false)
      {
         // could not save 'Network Topology' screenshot to disk
         this->mc_Error = "Could not save Network Topology screenshot to \"" +
                          C_SCLString(c_PathNetworkTopologyScreenshot.toStdString().c_str()) + "\".";
         osc_write_log_error("RTF File Export", this->mc_Error);
         s32_Return = C_BUSY;
      }
      c_Painter.end();
   }

   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info("RTF File Export", "Screenshot created, fill up content for DocuCreator ...");

      // fill up information for DocuCreator
      C_ExportXmlStructure c_ConfigXml;
      QString c_SysDefPathTmp; // get path of system definition file '.syde_sysdef'
      C_PuiProject::h_AdaptProjectPathToSystemDefinition(C_PuiProject::h_GetInstance()->GetPath(), c_SysDefPathTmp);
      QDateTime c_CurrentTime(QDateTime::currentDateTime());

      // Project
      c_ConfigXml.c_Title = "Project Documentation";
      c_ConfigXml.c_Name = C_PuiProject::h_GetInstance()->GetName().toStdString().c_str();
      c_ConfigXml.c_Version = C_PuiProject::h_GetInstance()->c_Version;
      c_ConfigXml.c_Created = C_SCLString(c_CurrentTime.toString("dd.MM.yyyy hh:mm").toStdString().c_str());
      c_ConfigXml.c_Author = C_PuiProject::h_GetInstance()->c_Editor;
      c_ConfigXml.c_SysDefPath = c_SysDefPathTmp.toStdString().c_str();
      c_ConfigXml.c_DevicesIniPath = C_Uti::h_GetAbsolutePathFromExe("../devices/devices.ini").toStdString().c_str();
      c_ConfigXml.c_OutputPath = orc_RtfPath;
      c_ConfigXml.c_NetworkTopologyImage = c_PathNetworkTopologyScreenshot.toStdString().c_str();
      // openSYDE
      c_ConfigXml.c_OpenSydeVersion = C_PuiProject::h_GetInstance()->c_OpenSYDEVersion; // used openSYDE version not
                                                                                        // current APPLICATION_VERSION
      // Company
      c_ConfigXml.c_CompanyName = orc_CompanyName;
      c_ConfigXml.c_CompanyLogoPath = orc_CompanyLogoPath;

      // create config xml file for DocuCreator
      m_CreateConfigXml(c_SclStringDocuCreatorConfigPath, c_ConfigXml);
   }

   // call external 'DocuCreator' tool
   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info("RTF File Export", "Content ready, call DocuCreator ...");

      QStringList c_Arguments;
      c_Arguments.push_back("-c");
      c_Arguments.push_back(QString(c_SclStringDocuCreatorConfigPath.c_str()));

      QProcess * pc_Process = new QProcess(new QObject());
      // execute DocuCreator
      pc_Process->start(QString(c_SclStringDocuCreatorPath.c_str()), c_Arguments);
      // wait until DocuCreator has finished
      bool q_Tmp = pc_Process->waitForFinished(30000); // 30 seconds (default)
      if (q_Tmp == true)
      {
         // evaluate return code of 'DocuCreator'
         if (pc_Process->exitCode() == EXIT_SUCCESS)
         {
            osc_write_log_info("RTF File Export", "Successful execution of DocuCreator.");
         }
         else
         {
            this->mc_Error =
               "DocuCreator reports EXIT_FAILURE. See log of DocuCreator for detailed information. Located in the same directory where your DocuCreator.exe is.";
            osc_write_log_error("RTF File Export", this->mc_Error);
            s32_Return = C_UNKNOWN_ERR;
         }
      }
      else
      {
         QProcess::ProcessError e_Error = pc_Process->error();
         if (e_Error == QProcess::ProcessError::FailedToStart)
         {
            this->mc_Error =
               "Could not start DocuCreator most likely due to insufficient permissions to invoke this program.";
            osc_write_log_error("RTF File Export", this->mc_Error);
         }
         else if (e_Error == QProcess::ProcessError::Crashed)
         {
            this->mc_Error = "DocuCreator crashed some time after starting successfully.";
            osc_write_log_error("RTF File Export", this->mc_Error);
         }
         else if (e_Error == QProcess::ProcessError::Timedout)
         {
            this->mc_Error = "Timeout for DocuCreator after 30 seconds.";
            osc_write_log_error("RTF File Export", this->mc_Error);
         }
         else if (e_Error == QProcess::ProcessError::ReadError)
         {
            this->mc_Error = "Error when attempting to read from DocuCreator process.";
            osc_write_log_error("RTF File Export", this->mc_Error);
         }
         else if (e_Error == QProcess::ProcessError::WriteError)
         {
            this->mc_Error = "Error when attempting to write to the DocuCreator process.";
            osc_write_log_error("RTF File Export", this->mc_Error);
         }
         else
         {
            this->mc_Error = "Execution of DocuCreator process not successful. Can't get any more information.";
            osc_write_log_error("RTF File Export", this->mc_Error);
         }
         s32_Return = C_UNKNOWN_ERR;
      }
      pc_Process->close();
      pc_Process->deleteLater(); // not necessary but does also no harm
      //lint -e{429}  no memory leak because of the Qt memory management
   }

   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info("RTF File Export", "RTF document created, export successful.");
   }

   orc_WarningMessages = this->mc_Warnings; // give user feedback
   orc_ErrorMessage = this->mc_Error;       //        -"-

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates config.xml file as input for DocuCreator RTF File Export.

   Assumption:
   * valid path (as internal function)
   * valid data structure ( -"- )

   \param[out]    orc_Path                 full path for config.xml of DocuCreator
   \param[out]    orc_ExportXmlStructure   data to write in config.xml

   \return
   C_NO_ERR    XML file successfully created
   C_NOACT     could not write data to XML file
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_RtfExportWidget::m_CreateConfigXml(const C_SCLString & orc_Path,
                                            const C_ExportXmlStructure & orc_ExportXmlStructure) const
{
   sint32 s32_Return;

   const C_SCLString c_ROOT_NAME = "config";
   const C_SCLString c_PROJECT = "project";
   const C_SCLString c_OPENSYDE = "opensyde";
   const C_SCLString c_COMPANY = "company";

   // create empty DocuCreator configuration file
   std::fstream c_File;

   c_File.open(orc_Path.c_str(), std::fstream::out);

   // fill DocuCreator configuration definition
   C_OSCXMLParser c_XMLParser;

   //Root Node
   c_XMLParser.CreateNodeChild(c_ROOT_NAME);
   tgl_assert(c_XMLParser.SelectRoot() == c_ROOT_NAME);

   //Project content
   tgl_assert(c_XMLParser.CreateAndSelectNodeChild(c_PROJECT) == c_PROJECT);

   c_XMLParser.CreateNodeChild("title", orc_ExportXmlStructure.c_Title);
   c_XMLParser.CreateNodeChild("name", orc_ExportXmlStructure.c_Name);
   c_XMLParser.CreateNodeChild("version", orc_ExportXmlStructure.c_Version);
   c_XMLParser.CreateNodeChild("created", orc_ExportXmlStructure.c_Created);
   c_XMLParser.CreateNodeChild("author", orc_ExportXmlStructure.c_Author);
   c_XMLParser.CreateNodeChild("sysdefPath", orc_ExportXmlStructure.c_SysDefPath);
   c_XMLParser.CreateNodeChild("devicesIniPath", orc_ExportXmlStructure.c_DevicesIniPath);
   c_XMLParser.CreateNodeChild("outputPath", orc_ExportXmlStructure.c_OutputPath);
   c_XMLParser.CreateNodeChild("networkTopologyImage", orc_ExportXmlStructure.c_NetworkTopologyImage);

   //Return
   tgl_assert(c_XMLParser.SelectNodeParent() == c_ROOT_NAME);

   //openSYDE version content
   tgl_assert(c_XMLParser.CreateAndSelectNodeChild(c_OPENSYDE) == c_OPENSYDE);

   c_XMLParser.CreateNodeChild("version", orc_ExportXmlStructure.c_OpenSydeVersion);

   //Return
   tgl_assert(c_XMLParser.SelectNodeParent() == c_ROOT_NAME);

   //Company details
   tgl_assert(c_XMLParser.CreateAndSelectNodeChild(c_COMPANY) == c_COMPANY);

   if (orc_ExportXmlStructure.c_CompanyName != "")
   {
      c_XMLParser.CreateNodeChild("name", orc_ExportXmlStructure.c_CompanyName);
   }
   else
   {
      c_XMLParser.CreateNodeChild("name", "unknown");
   }

   if (orc_ExportXmlStructure.c_CompanyLogoPath != "")
   {
      c_XMLParser.CreateNodeChild("logo", orc_ExportXmlStructure.c_CompanyLogoPath);
   }
   else
   {
      c_XMLParser.CreateNodeChild("logo", "notused.png");
   }

   //Return
   tgl_assert(c_XMLParser.SelectNodeParent() == c_ROOT_NAME);

   // save DocuCreator configuration file
   s32_Return = c_XMLParser.SaveToFile(orc_Path);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_RtfExportWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of OK button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_RtfExportWidget::m_OkClicked(void)
{
   if (this->m_CheckSettings() == C_NO_ERR)
   {
      this->mrc_ParentDialog.accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_RtfExportWidget::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of RTF path button click to call file selector
*/
//----------------------------------------------------------------------------------------------------------------------
void C_RtfExportWidget::m_RtfPathClicked(void)
{
   QString c_Folder; // for default folder
   C_SCLString c_Tmp =
      C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_EditRtfPath->GetPath()).toStdString().c_str();

   if (TGL_DirectoryExists(TGL_ExtractFilePath(c_Tmp)) == true)
   {
      c_Folder = TGL_ExtractFilePath(c_Tmp).c_str();
   }
   else
   {
      c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   QString c_DefaultFilename = C_PuiProject::h_GetInstance()->GetName();
   c_DefaultFilename += ".rtf";

   const QString c_FilterName = QString(C_GtGetText::h_GetText("RTF file (*.rtf)"));
   const QString c_FullRtfFilePath = C_OgeWiUtil::h_GetSaveFileName(
      this, C_GtGetText::h_GetText("Save File for RTF Export"), c_Folder, c_FilterName, c_DefaultFilename,
      QFileDialog::DontConfirmOverwrite);

   if (c_FullRtfFilePath != "")
   {
      this->SetRtfPath(c_FullRtfFilePath.toStdString().c_str());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of logo path button click to call file selector
*/
//----------------------------------------------------------------------------------------------------------------------
void C_RtfExportWidget::m_LogoPathClicked(void)
{
   QString c_Folder; // for default folder

   C_SCLString c_Tmp =
      C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_EditLogoPath->GetPath()).toStdString().c_str();

   if (TGL_DirectoryExists(TGL_ExtractFilePath(c_Tmp)) == true)
   {
      c_Folder = TGL_ExtractFilePath(c_Tmp).c_str();
   }
   else
   {
      c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   const QString c_FilterName = QString(C_GtGetText::h_GetText("Image file (*.jpg *.png)"));
   const QString c_FullLogoFilePath = QFileDialog::getOpenFileName(
      this,
      C_GtGetText::h_GetText("Select Company Logo"),
      c_Folder,
      c_FilterName,
      NULL);

   if (c_FullLogoFilePath != "")
   {
      this->SetCompanyLogoPath(c_FullLogoFilePath.toStdString().c_str());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check user input settings.

   \return
   C_NO_ERR    settings valid
   !C_NO_ERR   invalid settings
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_RtfExportWidget::m_CheckSettings(void) const
{
   sint32 s32_Return;
   C_SCLString c_RtfPath;
   C_SCLString c_CompanyName;
   C_SCLString c_CompanyLogoPath;

   s32_Return = this->GetRtfPath(c_RtfPath);
   // check if RTF file already exists
   if (s32_Return == C_WARN)
   {
      C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eQUESTION);
      C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;

      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Save RTF file"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you really want to overwrite the existing file?"));
      c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Overwrite"));
      c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Back"));
      e_ReturnMessageBox = c_MessageBox.Execute();

      switch (e_ReturnMessageBox)
      {
      case C_OgeWiCustomMessage::eYES:
         // Delete old file
         if (std::remove(c_RtfPath.c_str()) != 0)
         {
            // Error on deleting file
            C_OgeWiCustomMessage c_MessageBoxErrorRemove(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eERROR);
            c_MessageBoxErrorRemove.SetHeading(C_GtGetText::h_GetText("Save RTF file"));
            c_MessageBoxErrorRemove.SetDescription(C_GtGetText::h_GetText("File cannot be overwritten!"));
            c_MessageBoxErrorRemove.Execute();
         }
         else
         {
            s32_Return = C_NO_ERR;
         }
         break;
      case C_OgeWiCustomMessage::eCANCEL:
         // do not continue and do nothing
         break;
      case C_OgeWiCustomMessage::eNO:
         // do not continue and do nothing
         break;
      default:
         break;
      }
   }

   if ((s32_Return != stw_errors::C_NO_ERR) && (s32_Return != stw_errors::C_WARN))
   {
      stw_scl::C_SCLString c_Description;
      stw_scl::C_SCLString c_Details;
      if (s32_Return == stw_errors::C_CONFIG)
      {
         c_Description = "Directory of given RTF file path invalid.";
      }
      else if (s32_Return == stw_errors::C_CHECKSUM)
      {
         c_Description = "File name invalid. Only alphanumeric characters and \"_\" are allowed.";
      }
      else
      {
         c_Description = "Invalid RTF file format. File format (*.RTF) for DocuCreator required.";
      }
      c_Details = "Given RTF file path: \"" + c_RtfPath + "\"";

      C_OgeWiCustomMessage c_MessageResult(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eERROR);
      c_MessageResult.SetHeading(C_GtGetText::h_GetText("RTF File Export"));
      c_MessageResult.SetDescription(C_GtGetText::h_GetText(c_Description.c_str()));
      c_MessageResult.SetDetails(C_GtGetText::h_GetText(c_Details.c_str()));
      c_MessageResult.Execute();
   }

   if (s32_Return == stw_errors::C_NO_ERR)
   {
      s32_Return = this->GetCompanyName(c_CompanyName); // always no error
   }

   if (s32_Return == stw_errors::C_NO_ERR)
   {
      s32_Return = this->GetCompanyLogoPath(c_CompanyLogoPath);
      if (s32_Return != stw_errors::C_NO_ERR)
      {
         stw_scl::C_SCLString c_Description;
         stw_scl::C_SCLString c_Details;

         if (s32_Return == stw_errors::C_CONFIG)
         {
            c_Description = "File path for company logo invalid.";
         }
         else
         {
            c_Description = "Invalid picture file format for company logo. Only *.jpg or *.png supported.";
         }
         c_Details = "Given company logo file path: \"" + c_CompanyLogoPath + "\"";

         C_OgeWiCustomMessage c_MessageResult(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eERROR);
         c_MessageResult.SetHeading(C_GtGetText::h_GetText("RTF File Export"));
         c_MessageResult.SetDescription(C_GtGetText::h_GetText(c_Description.c_str()));
         c_MessageResult.SetDetails(C_GtGetText::h_GetText(c_Details.c_str()));
         c_MessageResult.Execute();
      }
   }

   return s32_Return;
}
