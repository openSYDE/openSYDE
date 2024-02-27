//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Hex file view

   Hex file view

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_FlaUpHexFileView.hpp"
#include "ui_C_FlaUpHexFileView.h"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_FlaUpHexFileView::mhc_START_TD = "<td style=\"padding: 0 9px 0 0;\">";
const QString C_FlaUpHexFileView::mhc_CONTINUE_TD = "<td style=\"padding: 0 9px 0 9px;\">";
/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent        Optional pointer to parent
   \param[in,out] orc_HexFileInfo   Hex file info to view
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUpHexFileView::C_FlaUpHexFileView(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                       const C_FlaUpHexFileInfo & orc_HexFileInfo) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_FlaUpHexFileView),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);
   this->mrc_ParentDialog.SetWidget(this);
   this->mrc_ParentDialog.SetSubTitle(orc_HexFileInfo.c_HexFileInfo.c_FileName);

   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Data Block File"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("File Information"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_TextEditContent->setReadOnly(true);

   this->m_LoadInfo(&orc_HexFileInfo);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_FlaUpHexFileView::m_OkClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpHexFileView::m_OkClicked()
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load all HEX information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpHexFileView::m_LoadInfo(const C_FlaUpHexFileInfo * const opc_HexFileInfo) const
{
   QString c_Text = "<html><body>";

   if (opc_HexFileInfo->c_HexFileInfo.c_TimeStamp != "No information available")
   {
      //Adding file information section
      c_Text += "<h3>" + static_cast<QString>(C_GtGetText::h_GetText("File Information")) + "</h3>";
      c_Text += "<table>";
      c_Text += "<tr>";
      c_Text += C_FlaUpHexFileView::mhc_START_TD;
      c_Text += C_GtGetText::h_GetText("Timestamp:");
      c_Text += "</td>";
      c_Text += C_FlaUpHexFileView::mhc_CONTINUE_TD;
      c_Text += opc_HexFileInfo->c_HexFileInfo.c_TimeStamp;
      c_Text += "</td>";
      c_Text += "</tr>";
      c_Text += "<tr>";
      c_Text += C_FlaUpHexFileView::mhc_START_TD;
      c_Text += C_GtGetText::h_GetText("Size:");
      c_Text += "</td>";
      c_Text += C_FlaUpHexFileView::mhc_CONTINUE_TD;
      c_Text += opc_HexFileInfo->c_HexFileInfo.c_Size;
      c_Text += "</td>";
      c_Text += "</tr>";
      c_Text += "<tr>";
      c_Text += C_FlaUpHexFileView::mhc_START_TD;
      c_Text += C_GtGetText::h_GetText("MD5 checksum:");
      c_Text += "</td>";
      c_Text += C_FlaUpHexFileView::mhc_CONTINUE_TD;
      c_Text += opc_HexFileInfo->c_HexFileInfo.c_Checksum;
      c_Text += "</td>";
      c_Text += "</tr>";
      c_Text += "</table>";

      //Adding data information
      c_Text += "<h3>" + static_cast<QString>(C_GtGetText::h_GetText("Data Information")) + "</h3>";
      c_Text += "<table>";
      c_Text += "<tr>";
      c_Text += C_FlaUpHexFileView::mhc_START_TD;
      c_Text += C_GtGetText::h_GetText("Number of bytes:");
      c_Text += "</td>";
      c_Text += C_FlaUpHexFileView::mhc_CONTINUE_TD;
      c_Text += opc_HexFileInfo->c_HexFileInfo.c_NumberOfBytes;
      c_Text += "</td>";
      c_Text += "</tr>";
      c_Text += "<tr>";
      c_Text += C_FlaUpHexFileView::mhc_START_TD;
      c_Text += C_GtGetText::h_GetText("32bit CRC:");
      c_Text += "</td>";
      c_Text += C_FlaUpHexFileView::mhc_CONTINUE_TD;
      c_Text += opc_HexFileInfo->c_HexFileInfo.c_BitCrc;
      c_Text += "</td>";
      c_Text += "</tr>";
      c_Text += "</table>";

      //Adding application information
      c_Text += "<h3>" + static_cast<QString>(C_GtGetText::h_GetText("File Information Blocks")) + "</h3>";
      c_Text += "<table>";
      c_Text += "<tr>";
      c_Text += C_FlaUpHexFileView::mhc_START_TD;
      c_Text += C_GtGetText::h_GetText("Number of blocks:");
      c_Text += "</td>";
      c_Text += "<td>";
      c_Text += QString::number(opc_HexFileInfo->c_HexFileInfo.s32_NumberOfBlocks);
      c_Text += "</td>";
      c_Text += "</tr>";
      c_Text += "</table>";
      for (int32_t s32_ItAppl = 0UL; s32_ItAppl < opc_HexFileInfo->c_HexFileInfo.s32_NumberOfBlocks; ++s32_ItAppl)
      {
         c_Text += "<h3>" +
                   static_cast<QString>(C_GtGetText::h_GetText("Block %1")).arg(s32_ItAppl + 1, 2, 10, QChar('0')) +
                   "</h3>";
         c_Text += "<table>";
         c_Text += "<tr>";
         c_Text += C_FlaUpHexFileView::mhc_START_TD;
         c_Text += C_GtGetText::h_GetText("Block type:");
         c_Text += "</td>";
         c_Text += C_FlaUpHexFileView::mhc_CONTINUE_TD;
         c_Text += opc_HexFileInfo->c_BlockInfo[s32_ItAppl].c_BlockType;
         c_Text += "</td>";
         c_Text += "</tr>";
         c_Text += "<tr>";
         c_Text += C_FlaUpHexFileView::mhc_START_TD;
         c_Text += C_GtGetText::h_GetText("Version:");
         c_Text += "</td>";
         c_Text += C_FlaUpHexFileView::mhc_CONTINUE_TD;
         c_Text += opc_HexFileInfo->c_BlockInfo[s32_ItAppl].c_Version;
         c_Text += "</td>";
         c_Text += "</tr>";
         c_Text += "<tr>";
         c_Text += C_FlaUpHexFileView::mhc_START_TD;
         c_Text += C_GtGetText::h_GetText("DeviceID:");
         c_Text += "</td>";
         c_Text += C_FlaUpHexFileView::mhc_CONTINUE_TD;
         if (!opc_HexFileInfo->c_BlockInfo[s32_ItAppl].c_DeviceId.isEmpty())
         {
            c_Text += opc_HexFileInfo->c_BlockInfo[s32_ItAppl].c_DeviceId;
         }
         else
         {
            c_Text += C_GtGetText::h_GetText("Unknown");
         }
         c_Text += "</td>";
         c_Text += "</tr>";
         c_Text += "<tr>";
         c_Text += C_FlaUpHexFileView::mhc_START_TD;
         c_Text += C_GtGetText::h_GetText("Date:");
         c_Text += "</td>";
         c_Text += C_FlaUpHexFileView::mhc_CONTINUE_TD;
         if (!opc_HexFileInfo->c_BlockInfo[s32_ItAppl].c_Date.isEmpty())
         {
            c_Text += opc_HexFileInfo->c_BlockInfo[s32_ItAppl].c_Date;
         }
         else
         {
            c_Text += C_GtGetText::h_GetText("Unknown");
         }
         c_Text += "</td>";
         c_Text += "</tr>";
         c_Text += "<tr>";
         c_Text += C_FlaUpHexFileView::mhc_START_TD;
         c_Text += C_GtGetText::h_GetText("Time:");
         c_Text += "</td>";
         c_Text += C_FlaUpHexFileView::mhc_CONTINUE_TD;
         if (!opc_HexFileInfo->c_BlockInfo[s32_ItAppl].c_Time.isEmpty())
         {
            c_Text += opc_HexFileInfo->c_BlockInfo[s32_ItAppl].c_Time;
         }
         else
         {
            c_Text += C_GtGetText::h_GetText("Unknown");
         }
         c_Text += "</td>";
         c_Text += "</tr>";
         c_Text += "<tr>";
         c_Text += C_FlaUpHexFileView::mhc_START_TD;
         c_Text += C_GtGetText::h_GetText("Project name:");
         c_Text += "</td>";
         c_Text += C_FlaUpHexFileView::mhc_CONTINUE_TD;
         if (!opc_HexFileInfo->c_BlockInfo[s32_ItAppl].c_ProjectName.isEmpty())
         {
            c_Text += opc_HexFileInfo->c_BlockInfo[s32_ItAppl].c_ProjectName;
         }
         else
         {
            c_Text += C_GtGetText::h_GetText("Unknown");
         }
         c_Text += "</td>";
         c_Text += "</tr>";
         c_Text += "<tr>";
         c_Text += C_FlaUpHexFileView::mhc_START_TD;
         c_Text += C_GtGetText::h_GetText("Project version:");
         c_Text += "</td>";
         c_Text += C_FlaUpHexFileView::mhc_CONTINUE_TD;
         if (!opc_HexFileInfo->c_BlockInfo[s32_ItAppl].c_ProjectVersion.isEmpty())
         {
            c_Text += opc_HexFileInfo->c_BlockInfo[s32_ItAppl].c_ProjectVersion;
         }
         else
         {
            c_Text += C_GtGetText::h_GetText("Unknown");
         }
         c_Text += "</td>";
         c_Text += "</tr>";
         c_Text += "<tr>";
         c_Text += C_FlaUpHexFileView::mhc_START_TD;
         c_Text += C_GtGetText::h_GetText("Additional info:");
         c_Text += "</td>";
         c_Text += C_FlaUpHexFileView::mhc_CONTINUE_TD;
         if (!opc_HexFileInfo->c_BlockInfo[s32_ItAppl].c_AdditionalInfo.isEmpty())
         {
            c_Text += opc_HexFileInfo->c_BlockInfo[s32_ItAppl].c_AdditionalInfo;
         }
         else
         {
            c_Text += C_GtGetText::h_GetText("Unknown");
         }
         c_Text += "</td>";
         c_Text += "</tr>";
         c_Text += "</table>";
      }
   }
   else
   {
      c_Text += C_GtGetText::h_GetText("No information available");
      c_Text += opc_HexFileInfo->c_HexFileInfo.c_FilePath;
      c_Text += ".<br>";
      c_Text += C_GtGetText::h_GetText("Please make sure it is an valid HEX file.");
   }
   c_Text += "</body></html>";
   this->mpc_Ui->pc_TextEditContent->setHtml(c_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUpHexFileView::~C_FlaUpHexFileView()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpHexFileView::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
