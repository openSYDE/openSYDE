//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for RTF file export (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <limits>
#include <fstream>
#include <QFileInfo>
#include <QDir>
#include <QGraphicsView>
#include <QScreen>
#include <QProcess>
#include <QFileDialog>
#include <QDateTime>
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_OscUtils.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_RtfExportWidget.hpp"
#include "TglFile.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscProject.hpp"
#include "C_PuiProject.hpp"
#include "C_Uti.hpp"
#include "C_PuiUtil.hpp"
#include "C_SdTopologyWidget.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "ui_C_RtfExportWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::scl;
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

   \param[in,out] orc_Parent          Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_RtfExportWidget::C_RtfExportWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
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
C_RtfExportWidget::~C_RtfExportWidget(void) noexcept
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
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("SYSTEM DEFINITION"));
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
int32_t C_RtfExportWidget::GetRtfPath(C_SclString & orc_RtfPath) const
{
   int32_t s32_Return = C_CONFIG;

   // get full RTF path of widget Ui
   orc_RtfPath = C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_EditRtfPath->GetPath()).toStdString().c_str();

   // check if path fulfills our path requirements
   if (C_OscUtils::h_CheckValidFilePath(orc_RtfPath) == true)
   {
      // check if directory exists
      if (TglDirectoryExists(TglExtractFilePath(orc_RtfPath)) == true)
      {
         // check if file name is valid
         const C_SclString c_FileExtAct = TglExtractFileExtension(orc_RtfPath);
         if (c_FileExtAct.LowerCase() == ".rtf")
         {
            const QFileInfo c_Info(orc_RtfPath.c_str());
            if (C_OscUtils::h_CheckValidFileName(c_Info.completeBaseName().toStdString().c_str()))
            {
               if (TglFileExists(orc_RtfPath) == true)
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
int32_t C_RtfExportWidget::GetCompanyName(C_SclString & orc_CompanyName) const
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
   C_CONFIG    file path invalid or file  does not exist
   C_NOACT     invalid file format (not .jpg or .png)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_RtfExportWidget::GetCompanyLogoPath(C_SclString & orc_CompanyLogoPath) const
{
   int32_t s32_Return = C_CONFIG;

   // get company logo path
   orc_CompanyLogoPath = this->mpc_Ui->pc_EditLogoPath->GetPath().toStdString().c_str();

   if (orc_CompanyLogoPath == "")
   {
      //nothing to check. Optional parameter, empty string is OK
      s32_Return = C_NO_ERR;
   }
   // check if path fulfills our path requirements
   else if (C_OscUtils::h_CheckValidFilePath(orc_CompanyLogoPath) == true)
   {
      if (orc_CompanyLogoPath != "")
      {
         // make absolute path if necessary
         orc_CompanyLogoPath =
            C_PuiUtil::h_GetAbsolutePathFromProject(orc_CompanyLogoPath.c_str()).toStdString().c_str();

         // check if file exists
         if (TglFileExists(orc_CompanyLogoPath) == true)
         {
            // check if file name is valid
            const C_SclString c_FileExtAct = TglExtractFileExtension(orc_CompanyLogoPath);
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
   }
   else
   {
      // nothing to do
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setter for full RTF File Export path.

   \param[in]     orc_RtfPath    input parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_RtfExportWidget::SetRtfPath(const C_SclString & orc_RtfPath) const
{
   this->mpc_Ui->pc_EditRtfPath->SetPath(orc_RtfPath.c_str(), C_PuiProject::h_GetInstance()->GetFolderPath());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setter for company name.

   \param[in]     orc_CompanyName    input parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_RtfExportWidget::SetCompanyName(const C_SclString & orc_CompanyName) const
{
   this->mpc_Ui->pc_EditCompany->setText(orc_CompanyName.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setter for company logo path.

   \param[in]     orc_CompanyLogoPath    path to company logo
*/
//----------------------------------------------------------------------------------------------------------------------
void C_RtfExportWidget::SetCompanyLogoPath(const C_SclString & orc_CompanyLogoPath) const
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
int32_t C_RtfExportWidget::ExportToRtf(const C_SclString & orc_RtfPath, const C_SclString & orc_CompanyName,
                                       const C_SclString & orc_CompanyLogoPath, C_SdTopologyWidget * const opc_Widget,
                                       C_SclStringList & orc_WarningMessages, C_SclString & orc_ErrorMessage)
{
   int32_t s32_Return = C_NO_ERR;

   this->mc_Warnings.Clear(); // reset global warnings
   this->mc_Error = "";       //     -"-      error

   // get paths of 'DocuCreator' tool and its config XML file (which has to be created)
   QString c_DocuCreatorPath = C_Uti::h_GetExePath(); // get device ini location by project path
   QDir c_DirDocuCreatorTmp(c_DocuCreatorPath);
   tgl_assert(c_DirDocuCreatorTmp.cdUp() == true);                      // go one directory up
   c_DocuCreatorPath = c_DirDocuCreatorTmp.absolutePath();              // get current path
   c_DocuCreatorPath += "/connectors/DocuCreator/osy_docu_creator.exe"; // add DocuCreator location
   const C_SclString c_SclStringDocuCreatorPath = c_DocuCreatorPath.toStdString().c_str();
   C_SclString c_SclStringDocuCreatorConfigPath = c_DirDocuCreatorTmp.absolutePath().toStdString().c_str();
   c_SclStringDocuCreatorConfigPath += "/connectors/DocuCreator/config.xml";

   QString c_PathNetworkTopologyScreenshot; // to save screenshot of network topology

   osc_write_log_info("RTF File Export", "Look for DocuCreator at path \"" + c_SclStringDocuCreatorPath + "\".");

   // check for path of external 'DocuCreator' tool
   if (TglFileExists(c_SclStringDocuCreatorPath) == false)
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

      int32_t s32_Factor; // to get better quality of image
      int32_t s32_Width = static_cast<int32_t>(opc_Widget->GetScene()->width());
      int32_t s32_Height = static_cast<int32_t>(opc_Widget->GetScene()->height());
      // factor values for pixels are empirical evaluated and a matter of taste
      // get longer side
      const int32_t s32_LongerSide = (s32_Width > s32_Height) ? s32_Width : s32_Height;
      s32_Factor = 6000 / s32_LongerSide; // a maximum of round about 6000 pixels for the longer side should be enough
      if (s32_Factor != 0)
      {
         s32_Width *= s32_Factor;
         s32_Height *= s32_Factor;
      }

      const QSize c_ImageSize(s32_Width, s32_Height);
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
                          static_cast<C_SclString>(c_PathNetworkTopologyScreenshot.toStdString().c_str()) + "\".";
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
      const QDateTime c_CurrentTime(QDateTime::currentDateTime());

      // Project
      c_ConfigXml.c_Title = "Project Documentation";
      c_ConfigXml.c_Name = C_PuiProject::h_GetInstance()->GetName().toStdString().c_str();
      c_ConfigXml.c_Version = C_PuiProject::h_GetInstance()->c_Version;
      c_ConfigXml.c_Created =
         static_cast<C_SclString>(c_CurrentTime.toString("dd.MM.yyyy hh:mm").toStdString().c_str());
      c_ConfigXml.c_Author = C_PuiProject::h_GetInstance()->c_Editor;
      c_ConfigXml.c_SysDefPath = c_SysDefPathTmp.toStdString().c_str();
      c_ConfigXml.c_DevicesIniPath = C_Uti::h_GetAbsolutePathFromExe("../devices/devices.ini").toStdString().c_str();
      c_ConfigXml.c_OutputPath = orc_RtfPath;
      c_ConfigXml.c_NetworkTopologyImage = c_PathNetworkTopologyScreenshot.toStdString().c_str();
      // openSYDE
      c_ConfigXml.c_OpenSydeVersion = C_PuiProject::h_GetInstance()->c_OpenSydeVersion; // used openSYDE version not
                                                                                        // current APPLICATION_VERSION
      // Company
      c_ConfigXml.c_CompanyName = orc_CompanyName;
      c_ConfigXml.c_CompanyLogoPath = orc_CompanyLogoPath;

      // create config xml file for DocuCreator
      m_CreateConfigXml(c_SclStringDocuCreatorConfigPath, c_ConfigXml);

      // call external 'DocuCreator' tool
      osc_write_log_info("RTF File Export", "Content ready, call DocuCreator ...");

      QStringList c_Arguments;
      c_Arguments.push_back("-c");
      c_Arguments.push_back(c_SclStringDocuCreatorConfigPath.c_str());

      QProcess * const pc_Process = new QProcess(new QObject());
      // execute DocuCreator
      pc_Process->start(c_SclStringDocuCreatorPath.c_str(), c_Arguments);
      // wait until DocuCreator has finished
      const bool q_Tmp = pc_Process->waitForFinished(30000); // 30 seconds (default)
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
         const QProcess::ProcessError e_Error = pc_Process->error();
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

      osc_write_log_info("RTF File Export", "RTF document created, export successful.");
   } //lint !e429  //no memory leak because of the Qt memory management

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
int32_t C_RtfExportWidget::m_CreateConfigXml(const C_SclString & orc_Path,
                                             const C_ExportXmlStructure & orc_ExportXmlStructure) const
{
   int32_t s32_Return;

   const C_SclString c_ROOT_NAME = "config";
   const C_SclString c_PROJECT = "project";
   const C_SclString c_OPENSYDE = "opensyde";
   const C_SclString c_COMPANY = "company";

   // create empty DocuCreator configuration file
   std::fstream c_File;

   c_File.open(orc_Path.c_str(), std::fstream::out);

   // fill DocuCreator configuration definition
   C_OscXmlParser c_XmlParser;

   //Root Node
   c_XmlParser.CreateNodeChild(c_ROOT_NAME);
   tgl_assert(c_XmlParser.SelectRoot() == c_ROOT_NAME);

   //Project content
   tgl_assert(c_XmlParser.CreateAndSelectNodeChild(c_PROJECT) == c_PROJECT);

   c_XmlParser.CreateNodeChild("title", orc_ExportXmlStructure.c_Title);
   c_XmlParser.CreateNodeChild("name", orc_ExportXmlStructure.c_Name);
   c_XmlParser.CreateNodeChild("version", orc_ExportXmlStructure.c_Version);
   c_XmlParser.CreateNodeChild("created", orc_ExportXmlStructure.c_Created);
   c_XmlParser.CreateNodeChild("author", orc_ExportXmlStructure.c_Author);
   c_XmlParser.CreateNodeChild("sysdefPath", orc_ExportXmlStructure.c_SysDefPath);
   c_XmlParser.CreateNodeChild("devicesIniPath", orc_ExportXmlStructure.c_DevicesIniPath);
   c_XmlParser.CreateNodeChild("outputPath", orc_ExportXmlStructure.c_OutputPath);
   c_XmlParser.CreateNodeChild("networkTopologyImage", orc_ExportXmlStructure.c_NetworkTopologyImage);

   //Return
   tgl_assert(c_XmlParser.SelectNodeParent() == c_ROOT_NAME);

   //openSYDE version content
   tgl_assert(c_XmlParser.CreateAndSelectNodeChild(c_OPENSYDE) == c_OPENSYDE);

   c_XmlParser.CreateNodeChild("version", orc_ExportXmlStructure.c_OpenSydeVersion);

   //Return
   tgl_assert(c_XmlParser.SelectNodeParent() == c_ROOT_NAME);

   //Company details
   tgl_assert(c_XmlParser.CreateAndSelectNodeChild(c_COMPANY) == c_COMPANY);

   if (orc_ExportXmlStructure.c_CompanyName != "")
   {
      c_XmlParser.CreateNodeChild("name", orc_ExportXmlStructure.c_CompanyName);
   }
   else
   {
      c_XmlParser.CreateNodeChild("name", "unknown");
   }

   if (orc_ExportXmlStructure.c_CompanyLogoPath != "")
   {
      c_XmlParser.CreateNodeChild("logo", orc_ExportXmlStructure.c_CompanyLogoPath);
   }
   else
   {
      c_XmlParser.CreateNodeChild("logo", "notused.png");
   }

   //Return
   tgl_assert(c_XmlParser.SelectNodeParent() == c_ROOT_NAME);

   // save DocuCreator configuration file
   s32_Return = c_XmlParser.SaveToFile(orc_Path);

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
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
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
   const C_SclString c_Tmp =
      C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_EditRtfPath->GetPath()).toStdString().c_str();

   if (TglDirectoryExists(TglExtractFilePath(c_Tmp)) == true)
   {
      c_Folder = TglExtractFilePath(c_Tmp).c_str();
   }
   else
   {
      c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   QString c_DefaultFilename = C_PuiProject::h_GetInstance()->GetName();
   c_DefaultFilename += ".rtf";

   const QString c_FilterName = C_GtGetText::h_GetText("RTF file (*.rtf)");
   const QString c_FullRtfFilePath = C_OgeWiUtil::h_GetSaveFileName(
      this, C_GtGetText::h_GetText("Save File for RTF Export"), c_Folder, c_FilterName, c_DefaultFilename,
      QFileDialog::DontConfirmOverwrite); // overwrite is handled later

   if (c_FullRtfFilePath != "")
   {
      this->SetRtfPath(c_FullRtfFilePath.toStdString().c_str());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of logo path button click to call file selector
*/
//----------------------------------------------------------------------------------------------------------------------
void C_RtfExportWidget::m_LogoPathClicked(void) const
{
   const QString c_Filter = C_GtGetText::h_GetText("Image file (*.jpg *.png)");
   QString c_Folder; // for default folder

   const C_SclString c_Tmp =
      C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_EditLogoPath->GetPath()).toStdString().c_str();

   if (TglDirectoryExists(TglExtractFilePath(c_Tmp)) == true)
   {
      c_Folder = TglExtractFilePath(c_Tmp).c_str();
   }
   else
   {
      c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   const QString c_FullLogoFilePath = C_OgeWiUtil::h_GetOpenFileName(this->parentWidget(),
                                                                     C_GtGetText::h_GetText("Select Company Logo"),
                                                                     c_Folder, c_Filter, "*.jpg");
   if (c_FullLogoFilePath.isEmpty() == false)
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
int32_t C_RtfExportWidget::m_CheckSettings(void) const
{
   int32_t s32_Return;
   C_SclString c_RtfPath;
   C_SclString c_CompanyName;
   C_SclString c_CompanyLogoPath;

   s32_Return = this->GetRtfPath(c_RtfPath);
   // check if RTF file already exists
   if (s32_Return == C_WARN)
   {
      C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eQUESTION);
      C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;

      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Save RTF file"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you really want to overwrite the existing file?"));
      c_MessageBox.SetOkButtonText(C_GtGetText::h_GetText("Overwrite"));
      c_MessageBox.SetNoButtonText(C_GtGetText::h_GetText("Back"));
      c_MessageBox.SetCustomMinHeight(180, 180);
      e_ReturnMessageBox = c_MessageBox.Execute();

      if (e_ReturnMessageBox == C_OgeWiCustomMessage::eYES)
      {
         // Delete old file
         if (std::remove(c_RtfPath.c_str()) != 0)
         {
            // Error on deleting file
            C_OgeWiCustomMessage c_MessageBoxErrorRemove(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eERROR);
            c_MessageBoxErrorRemove.SetHeading(C_GtGetText::h_GetText("Save RTF file"));
            c_MessageBoxErrorRemove.SetDescription(C_GtGetText::h_GetText("File cannot be overwritten!"));
            c_MessageBoxErrorRemove.SetCustomMinHeight(180, 180);
            c_MessageBoxErrorRemove.Execute();
         }
         else
         {
            s32_Return = C_NO_ERR;
         }
      }
   }

   if ((s32_Return != stw::errors::C_NO_ERR) && (s32_Return != stw::errors::C_WARN))
   {
      stw::scl::C_SclString c_Description;
      stw::scl::C_SclString c_Details;
      if (s32_Return == stw::errors::C_CONFIG)
      {
         c_Description = "Directory of given RTF file path invalid.";
      }
      else if (s32_Return == stw::errors::C_CHECKSUM)
      {
         c_Description = "File name is invalid.";
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
      c_MessageResult.SetCustomMinHeight(180, 250);
      c_MessageResult.Execute();
   }

   if (s32_Return == stw::errors::C_NO_ERR)
   {
      s32_Return = this->GetCompanyName(c_CompanyName); // always no error
   }

   if (s32_Return == stw::errors::C_NO_ERR)
   {
      s32_Return = this->GetCompanyLogoPath(c_CompanyLogoPath);
      if (s32_Return != stw::errors::C_NO_ERR)
      {
         stw::scl::C_SclString c_Description;
         stw::scl::C_SclString c_Details;

         if (s32_Return == stw::errors::C_CONFIG)
         {
            c_Description = "File path for company logo is invalid.";
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
         c_MessageResult.SetCustomMinHeight(180, 250);
         c_MessageResult.Execute();
      }
   }

   return s32_Return;
}
