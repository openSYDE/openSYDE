//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific PEM file added to other files of a node

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_SyvUpPacListNodeItemPemFileWidget.hpp"
#include "ui_C_SyvUpPacListNodeItemWidget.h"
#include "C_SyvUpPacPemFileInfoPopUp.hpp"
#include "C_OscSecurityPem.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
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

   \param[in]     ou32_ViewIndex       View index
   \param[in]     ou32_NodeIndex       Node index
   \param[in]     orc_DeviceName       Name of device for comparing with application block
   \param[in]     oq_FileBased         Flag if node is a file based device
   \param[in]     oq_StwFlashloader    Flag if node has not an openSYDE Flashloader but a STW Flashloader
   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacListNodeItemPemFileWidget::C_SyvUpPacListNodeItemPemFileWidget(const uint32_t ou32_ViewIndex,
                                                                         const uint32_t ou32_NodeIndex,
                                                                         const QString & orc_DeviceName,
                                                                         const bool oq_FileBased,
                                                                         const bool oq_StwFlashloader,
                                                                         QWidget * const opc_Parent) :
   C_SyvUpPacListNodeItemWidget(ou32_ViewIndex, ou32_NodeIndex, orc_DeviceName, oq_FileBased,
                                oq_StwFlashloader, opc_Parent),
   me_StateSecurity(C_OscViewNodeUpdate::eST_SEC_NO_CHANGE),
   me_StateDebugger(C_OscViewNodeUpdate::eST_DEB_NO_CHANGE)
{
   this->mpc_Ui->pc_LabelSecurity->setText(C_GtGetText::h_GetText("Security UNKNOWN"));
   this->mpc_Ui->pc_LabelSecurity->SetFontPixel(10);
   this->mpc_Ui->pc_LabelSecurity->SetForegroundColor(0);
   this->mpc_Ui->pc_LabelSecurity->SetBackgroundColor(9);
   this->mpc_Ui->pc_LabelSecurity->setVisible(false);

   this->mpc_Ui->pc_LabelDebugger->setText(C_GtGetText::h_GetText("Debugger UNKNOWN"));
   this->mpc_Ui->pc_LabelDebugger->SetFontPixel(10);
   this->mpc_Ui->pc_LabelDebugger->SetForegroundColor(0);
   this->mpc_Ui->pc_LabelDebugger->SetBackgroundColor(9);
   this->mpc_Ui->pc_LabelDebugger->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of the item

   \return
   Type of return values, e.g. STW error codes
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvUpPacListNodeItemPemFileWidget::GetType(void) const
{
   return mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PEM;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Shows file information

   Handle view PEM file info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemPemFileWidget::ViewFileInfo(void)
{
   if (this->GetAppFilePath().compare("") != 0)
   {
      const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
      C_SyvUpPacPemFileInfoPopUp * const pc_InfoDialog =
         new C_SyvUpPacPemFileInfoPopUp(*c_New,
                                        this->GetAppAbsoluteFilePath());

      Q_UNUSED(pc_InfoDialog)

      //Resize
      c_New->SetSize(QSize(1000, 761));

      if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
      {
         //No confirmation
      }

      if (c_New != NULL)
      {
         c_New->HideOverlay();
         c_New->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_InfoDialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the flag if it is possible to show file information

   Return false if path is empty.

   \retval   true   View file info can show information
   \retval   false  View file info can not show information
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacListNodeItemPemFileWidget::IsViewFileInfoPossible(void) const
{
   return this->GetAppFilePath().compare("") != 0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set pem states

   \param[in]  oe_StateSecurity  Security state of node
   \param[in]  oe_StateDebugger  Debugger state of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemPemFileWidget::SetPemStates(const C_OscViewNodeUpdate::E_StateSecurity oe_StateSecurity,
                                                       const C_OscViewNodeUpdate::E_StateDebugger oe_StateDebugger)
{
   this->me_StateDebugger = oe_StateDebugger;
   this->me_StateSecurity = oe_StateSecurity;

   //lint -e{9042} Warning wanted if new cases are added
   switch (this->me_StateSecurity)
   {
   case C_OscViewNodeUpdate::eST_SEC_NO_CHANGE:
      this->mpc_Ui->pc_LabelSecurity->setText(C_GtGetText::h_GetText("Security UNKNOWN"));
      break;
   case C_OscViewNodeUpdate::eST_SEC_ACTIVATE:
      this->mpc_Ui->pc_LabelSecurity->setText(C_GtGetText::h_GetText("Security ON"));
      break;
   case C_OscViewNodeUpdate::eST_SEC_DEACTIVATE:
      this->mpc_Ui->pc_LabelSecurity->setText(C_GtGetText::h_GetText("Security OFF"));
      break;
   }

   //lint -e{9042} Warning wanted if new cases are added
   switch (this->me_StateDebugger)
   {
   case C_OscViewNodeUpdate::eST_DEB_NO_CHANGE:
      this->mpc_Ui->pc_LabelDebugger->setText(C_GtGetText::h_GetText("Debugger UNKNOWN"));
      break;
   case C_OscViewNodeUpdate::eST_DEB_ACTIVATE:
      this->mpc_Ui->pc_LabelDebugger->setText(C_GtGetText::h_GetText("Debugger ON"));
      break;
   case C_OscViewNodeUpdate::eST_DEB_DEACTIVATE:
      this->mpc_Ui->pc_LabelDebugger->setText(C_GtGetText::h_GetText("Debugger OFF"));
      break;
   }

   this->mpc_Ui->pc_LabelSecurity->setVisible(this->me_StateSecurity != C_OscViewNodeUpdate::eST_SEC_NO_CHANGE);
   this->mpc_Ui->pc_LabelDebugger->setVisible(this->me_StateDebugger != C_OscViewNodeUpdate::eST_DEB_NO_CHANGE);

   //Adapt file path label to changes in layout
   this->m_UpateFilePathLabel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the states of the node

   \param[out]      ore_StateSecurity   Security state of node
   \param[out]      ore_StateDebugger   Debugger state of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemPemFileWidget::GetPemStates(C_OscViewNodeUpdate::E_StateSecurity & ore_StateSecurity,
                                                       C_OscViewNodeUpdate::E_StateDebugger & ore_StateDebugger) const
{
   ore_StateDebugger = this->me_StateDebugger;
   ore_StateSecurity = this->me_StateSecurity;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load application information.

   \param[out]    orq_FileExists          true: file exists  false: file not found
   \param[out]    orq_FlashwareWarning    true: flashware warning occured
                                                (only relevant for STW Flashloader; wrong device name in hex file)
                                          false: no warnings occured
   \param[out]    orq_TriggerRemove       true: remove item widget
                                                (only relevant if user discards file)
                                          false: keep widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemPemFileWidget::m_LoadFileInformation(bool & orq_FileExists, bool & orq_FlashwareWarning,
                                                                bool & orq_TriggerRemove)
{
   C_SyvUpPacListNodeItemWidget::m_LoadFileInformation(orq_FileExists, orq_FlashwareWarning,
                                                       orq_TriggerRemove);

   if (orq_FileExists == true)
   {
      std::string c_ErrorMessage;
      std::vector<uint8_t> c_Modulus;
      std::vector<uint8_t> c_Exponent;
      int32_t s32_Result = stw::opensyde_core::C_OscSecurityPem::h_ExtractModulusAndExponentFromFile(
         this->GetAppAbsoluteFilePath().toStdString(), c_Modulus, c_Exponent, c_ErrorMessage);

      if (s32_Result == C_NO_ERR)
      {
         // Check the content of the PEM file
         // Public key must be 1024 bit and the exponent must be smaller or equal than 32 bit
         if ((c_Modulus.size() != 128) ||
             (c_Exponent.size() == 0) ||
             (c_Exponent.size() > 4))
         {
            s32_Result = C_OVERFLOW;
         }
      }

      if (s32_Result != C_NO_ERR)
      {
         QString c_Details = static_cast<QString>(C_GtGetText::h_GetText("File path: %1")).arg(
            this->GetAppAbsoluteFilePath()) +
                             "\nReason: ";
         //New file cannot be read
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         switch (s32_Result)
         {
         case C_RANGE:
            c_Details +=
               static_cast<QString>(C_GtGetText::h_GetText("File could not be opened. PEM error message: %1")).arg(
                  c_ErrorMessage.c_str());
            break;
         case C_CONFIG:
            c_Details +=
               static_cast<QString>(C_GtGetText::h_GetText("Invalid file content. PEM error message: %1")).arg(
                  c_ErrorMessage.c_str());
            break;
         case C_CHECKSUM:
            c_Details +=
               static_cast<QString>(C_GtGetText::h_GetText(
                                       "Could not parse modulus and exponent from key. PEM error message: %1")).arg(
                  c_ErrorMessage.c_str());
            break;
         case C_OVERFLOW:
            c_Details += static_cast<QString>(
               C_GtGetText::h_GetText(
                  "The public key or its exponent of the PEM file does not have the expected size.\n"
                  "The public key must be 1024 Bit."
                  "The exponent of the public key must be less than or equal to 32 bits."));
            break;
         default:
            c_Details += static_cast<QString>(C_GtGetText::h_GetText("Unknown reason. PEM error message: %1")).arg(
               c_ErrorMessage.c_str());
            break;
         }

         c_Message.SetHeading(C_GtGetText::h_GetText("Update package configuration"));
         c_Message.SetDescription(C_GtGetText::h_GetText("File is not a valid PEM file."));
         c_Message.SetDetails(c_Details);
         c_Message.SetCustomMinHeight(180, 250);
         c_Message.Execute();
         //File invalid
         orq_TriggerRemove = true;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Creates and returns the title specific tool tip content

   \return
   Tool tip title
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacListNodeItemPemFileWidget::m_CreateToolTipTitle(void) const
{
   QString c_Title = static_cast<QString>("%1: PEM File").
                     arg(this->mc_OwnerSectionName);

   return c_Title;
}
