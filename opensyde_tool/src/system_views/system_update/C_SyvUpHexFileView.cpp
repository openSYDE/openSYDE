//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for HEX file details (implementation)

   Widget for HEX file details

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.06.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QDateTime>
#include <QFileInfo>
#include <QCryptographicHash>
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_GtGetText.h"
#include "C_PuiProject.h"
#include "C_SyvUpHexFileView.h"
#include "ui_C_SyvUpHexFileView.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */
const QString C_SyvUpHexFileView::mhc_StartTD = "<td style=\"padding: 0 9px 0 0;\">";
const QString C_SyvUpHexFileView::mhc_ContinueTD = "<td style=\"padding: 0 9px 0 9px;\">";

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent Reference to parent
   \param[in]     orc_File   The file to look at

   \created     25.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvUpHexFileView::C_SyvUpHexFileView(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                       const QString & orc_File) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvUpHexFileView),
   mrc_ParentDialog(orc_Parent),
   mc_File(orc_File)
{
   const QFileInfo c_Info(orc_File);

   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   this->mrc_ParentDialog.SetSubTitle(c_Info.fileName());

   this->mpc_Ui->pc_TextEditContent->setReadOnly(true);

   m_LoadInfo();

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SyvUpHexFileView::m_OkClicked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     25.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvUpHexFileView::~C_SyvUpHexFileView(void)
{
   delete this->mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     25.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpHexFileView::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("HEX File"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("File Information"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information

   \created     25.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpHexFileView::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
   \brief   Load all HEX information

   \created     25.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpHexFileView::m_LoadInfo(void) const
{
   QString c_Text = "<html><body>";
   const QFileInfo c_InvalidFileInfo(this->mc_File);
   C_OsyHexFile c_HexFile;
   uint32 u32_Result;

   stw_scl::C_SCLString c_Path;
   if (c_InvalidFileInfo.isAbsolute() == true)
   {
      c_Path = this->mc_File.toStdString().c_str();
   }
   else
   {
      c_Path = (C_PuiProject::h_GetInstance()->GetFolderPath() + '/' + this->mc_File).toStdString().c_str();
   }
   //Application is relative to project files
   u32_Result = c_HexFile.LoadFromFile(c_Path.c_str());
   if (u32_Result == stw_hex_file::NO_ERR)
   {
      mh_AddFileSection(c_Path.c_str(), c_Text);
      mh_AddDataInformation(c_HexFile, c_Text);
      mh_AddApplicationInformation(c_HexFile, c_Text);
   }
   c_Text += "</body></html>";
   this->mpc_Ui->pc_TextEditContent->setHtml(c_Text);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Ok button click

   \created     25.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpHexFileView::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle file information section

   \param[in]     orc_Path    HEX file path
   \param[in,out] orc_Content Text to append to

   \created     25.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpHexFileView::mh_AddFileSection(const QString & orc_Path, QString & orc_Content)
{
   const QFileInfo c_FileInfo(orc_Path);

   orc_Content += "<h3>" + QString(C_GtGetText::h_GetText("File Information")) + "</h3>";
   orc_Content += "<table>";
   orc_Content += "<tr>";
   orc_Content += C_SyvUpHexFileView::mhc_StartTD;
   orc_Content += C_GtGetText::h_GetText("Timestamp:");
   orc_Content += "</td>";
   orc_Content += C_SyvUpHexFileView::mhc_ContinueTD;
   orc_Content += c_FileInfo.lastModified().toString("dd.MM.yyyy HH:mm:ss");
   orc_Content += "</td>";
   orc_Content += "</tr>";
   orc_Content += "<tr>";
   orc_Content += C_SyvUpHexFileView::mhc_StartTD;
   orc_Content += C_GtGetText::h_GetText("Size:");
   orc_Content += "</td>";
   orc_Content += C_SyvUpHexFileView::mhc_ContinueTD;
   orc_Content += QString("%1 bytes").arg(c_FileInfo.size());
   orc_Content += "</td>";
   orc_Content += "</tr>";
   orc_Content += "<tr>";
   orc_Content += C_SyvUpHexFileView::mhc_StartTD;
   orc_Content += C_GtGetText::h_GetText("MD5 checksum:");
   orc_Content += "</td>";
   orc_Content += C_SyvUpHexFileView::mhc_ContinueTD;
   orc_Content += mh_GetMD5Hex(orc_Path);
   orc_Content += "</td>";
   orc_Content += "</tr>";
   orc_Content += "</table>";
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get MD5 checksum for file encoded in hex

   \param[in] orc_Path File path

   \return
   MD5 checksum for file encoded in hex

   \created     25.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_SyvUpHexFileView::mh_GetMD5Hex(const QString & orc_Path)
{
   QString c_Retval;
   QFile c_File(orc_Path);

   if (c_File.open(QFile::ReadOnly))
   {
      QCryptographicHash c_Md5Algorithm(QCryptographicHash::Md5);
      if (c_Md5Algorithm.addData(&c_File))
      {
         c_Retval = c_Md5Algorithm.result().toHex();
      }
      c_File.close();
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add data information

   \param[in]     orc_HexFile Hex file info
   \param[in,out] orc_Content Text to append to

   \created     25.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpHexFileView::mh_AddDataInformation(C_OsyHexFile & orc_HexFile, QString & orc_Content)
{
   const uint32 u32_Bytes = orc_HexFile.ByteCount();
   uint32 u32_CRC;

   orc_HexFile.CalcFileChecksum(u32_CRC);

   orc_Content += "<h3>" + QString(C_GtGetText::h_GetText("Data Information")) + "</h3>";
   orc_Content += "<table>";
   orc_Content += "<tr>";
   orc_Content += C_SyvUpHexFileView::mhc_StartTD;
   orc_Content += C_GtGetText::h_GetText("Number of bytes:");
   orc_Content += "</td>";
   orc_Content += C_SyvUpHexFileView::mhc_ContinueTD;
   orc_Content += QString::number(u32_Bytes);
   orc_Content += "</td>";
   orc_Content += "</tr>";
   orc_Content += "<tr>";
   orc_Content += C_SyvUpHexFileView::mhc_StartTD;
   orc_Content += C_GtGetText::h_GetText("32bit CRC:");
   orc_Content += "</td>";
   orc_Content += C_SyvUpHexFileView::mhc_ContinueTD;
   orc_Content += QString("0x%1").arg(u32_CRC, 0, 16);
   orc_Content += "</td>";
   orc_Content += "</tr>";
   orc_Content += "</table>";
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add application information

   \param[in]     orc_HexFile Hex file info
   \param[in,out] orc_Content Text to append to

   \created     25.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpHexFileView::mh_AddApplicationInformation(C_OsyHexFile & orc_HexFile, QString & orc_Content)
{
   stw_scl::SCLDynamicArray<stw_diag_lib::C_XFLECUInformation> c_InfoBlocks;
   orc_HexFile.GetECUInformationBlocks(c_InfoBlocks, 0UL, false, false, false);
   orc_Content += "<h3>" + QString(C_GtGetText::h_GetText("File Information Blocks")) + "</h3>";
   orc_Content += "<table>";
   orc_Content += "<tr>";
   orc_Content += C_SyvUpHexFileView::mhc_StartTD;
   orc_Content += C_GtGetText::h_GetText("Number of blocks:");
   orc_Content += "</td>";
   orc_Content += "<td>";
   orc_Content += QString::number(c_InfoBlocks.GetLength());
   orc_Content += "</td>";
   orc_Content += "</tr>";
   orc_Content += "</table>";
   for (sint32 s32_ItAppl = 0UL; s32_ItAppl < c_InfoBlocks.GetLength(); ++s32_ItAppl)
   {
      const stw_diag_lib::C_XFLECUInformation & rc_CurInfo = c_InfoBlocks[s32_ItAppl];
      orc_Content += "<h3>" +
                     QString(C_GtGetText::h_GetText("Block %1")).arg(s32_ItAppl + 1L, 2, 10, QChar('0')) + "</h3>";
      orc_Content += "<table>";
      orc_Content += "<tr>";
      orc_Content += C_SyvUpHexFileView::mhc_StartTD;
      orc_Content += C_GtGetText::h_GetText("Block type:");
      orc_Content += "</td>";
      orc_Content += C_SyvUpHexFileView::mhc_ContinueTD;
      orc_Content += rc_CurInfo.GetInfoLevelAsString().c_str();
      orc_Content += "</td>";
      orc_Content += "</tr>";
      orc_Content += "<tr>";
      orc_Content += C_SyvUpHexFileView::mhc_StartTD;
      orc_Content += C_GtGetText::h_GetText("Version:");
      orc_Content += "</td>";
      orc_Content += C_SyvUpHexFileView::mhc_ContinueTD;
      orc_Content += QString::number(rc_CurInfo.u8_StructVersion);
      orc_Content += "</td>";
      orc_Content += "</tr>";
      orc_Content += "<tr>";
      orc_Content += C_SyvUpHexFileView::mhc_StartTD;
      orc_Content += C_GtGetText::h_GetText("DeviceID:");
      orc_Content += "</td>";
      orc_Content += C_SyvUpHexFileView::mhc_ContinueTD;
      if (rc_CurInfo.ContainsDeviceID() == true)
      {
         orc_Content += rc_CurInfo.GetDeviceID().c_str();
      }
      else
      {
         orc_Content += C_GtGetText::h_GetText("Unknown");
      }
      orc_Content += "</td>";
      orc_Content += "</tr>";
      orc_Content += "<tr>";
      orc_Content += C_SyvUpHexFileView::mhc_StartTD;
      orc_Content += C_GtGetText::h_GetText("Date:");
      orc_Content += "</td>";
      orc_Content += C_SyvUpHexFileView::mhc_ContinueTD;
      if (rc_CurInfo.ContainsDateAndTime() == true)
      {
         orc_Content += rc_CurInfo.GetDate().c_str();
      }
      else
      {
         orc_Content += C_GtGetText::h_GetText("Unknown");
      }
      orc_Content += "</td>";
      orc_Content += "</tr>";
      orc_Content += "<tr>";
      orc_Content += C_SyvUpHexFileView::mhc_StartTD;
      orc_Content += C_GtGetText::h_GetText("Time:");
      orc_Content += "</td>";
      orc_Content += C_SyvUpHexFileView::mhc_ContinueTD;
      if (rc_CurInfo.ContainsDateAndTime() == true)
      {
         orc_Content += rc_CurInfo.GetTime().c_str();
      }
      else
      {
         orc_Content += C_GtGetText::h_GetText("Unknown");
      }
      orc_Content += "</td>";
      orc_Content += "</tr>";
      orc_Content += "<tr>";
      orc_Content += C_SyvUpHexFileView::mhc_StartTD;
      orc_Content += C_GtGetText::h_GetText("Project name:");
      orc_Content += "</td>";
      orc_Content += C_SyvUpHexFileView::mhc_ContinueTD;
      if (rc_CurInfo.ContainsProjectName() == true)
      {
         orc_Content += rc_CurInfo.GetProjectName().c_str();
      }
      else
      {
         orc_Content += C_GtGetText::h_GetText("Unknown");
      }
      orc_Content += "</td>";
      orc_Content += "</tr>";
      orc_Content += "<tr>";
      orc_Content += C_SyvUpHexFileView::mhc_StartTD;
      orc_Content += C_GtGetText::h_GetText("Project version:");
      orc_Content += "</td>";
      orc_Content += C_SyvUpHexFileView::mhc_ContinueTD;
      if (rc_CurInfo.ContainsProjectVersion() == true)
      {
         orc_Content += rc_CurInfo.GetProjectVersion().c_str();
      }
      else
      {
         orc_Content += C_GtGetText::h_GetText("Unknown");
      }
      orc_Content += "</td>";
      orc_Content += "</tr>";
      orc_Content += "<tr>";
      orc_Content += C_SyvUpHexFileView::mhc_StartTD;
      orc_Content += C_GtGetText::h_GetText("Additional info:");
      orc_Content += "</td>";
      orc_Content += C_SyvUpHexFileView::mhc_ContinueTD;
      if (rc_CurInfo.ContainsAdditionalInfo() == true)
      {
         orc_Content += rc_CurInfo.GetAdditionalInfo().c_str();
      }
      else
      {
         orc_Content += C_GtGetText::h_GetText("Unknown");
      }
      orc_Content += "</td>";
      orc_Content += "</tr>";
      orc_Content += "</table>";
   }
}
