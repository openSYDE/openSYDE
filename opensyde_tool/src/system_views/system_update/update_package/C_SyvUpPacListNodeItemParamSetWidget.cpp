//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific parameter set image file as part of a node

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"

#include "constants.hpp"
#include "TglUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_OgePopUpDialog.hpp"

#include "C_OgeWiCustomMessage.hpp"
#include "C_SyvUpPacListNodeItemParamSetWidget.hpp"
#include "C_SyvUpPacParamSetFileInfoPopUp.hpp"
#include "C_SyvUpPacParamSetFileAddPopUp.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
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
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_ViewIndex       View index
   \param[in]     ou32_NodeIndex       Node index
   \param[in]     orc_DeviceName       Name of device for comparing with application block
   \param[in]     oq_FileBased         Flag if node is a file based device
   \param[in]     oq_StwFlashloader    Flag if node has not an openSYDE Flashloader but a STW Flashloader
   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacListNodeItemParamSetWidget::C_SyvUpPacListNodeItemParamSetWidget(const uint32_t ou32_ViewIndex,
                                                                           const uint32_t ou32_NodeIndex,
                                                                           const QString & orc_DeviceName,
                                                                           const bool oq_FileBased,
                                                                           const bool oq_StwFlashloader,
                                                                           QWidget * const opc_Parent) :
   C_SyvUpPacListNodeItemWidget(ou32_ViewIndex, ou32_NodeIndex, orc_DeviceName, oq_FileBased,
                                oq_StwFlashloader, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the parameter information
 *
   \param[in]       orc_ParamInfo     New parameter information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemParamSetWidget::SetParamInfo(
   const stw::opensyde_gui_logic::C_PuiSvNodeUpdateParamInfo & orc_ParamInfo)
{
   this->mc_ParamsetInfo = orc_ParamInfo;
   this->SetAppFile(this->mc_ParamsetInfo.GetPath(), (orc_ParamInfo.GetPath() == this->GetDefaultFilePath()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the parameter information

   \return
   Paramterset information
*/
//----------------------------------------------------------------------------------------------------------------------
stw::opensyde_gui_logic::C_PuiSvNodeUpdateParamInfo C_SyvUpPacListNodeItemParamSetWidget::GetParamInfo(void) const
{
   return mc_ParamsetInfo;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of the item

   \return
   Type of return values, e.g. STW error codes
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvUpPacListNodeItemParamSetWidget::GetType(void) const
{
   return mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Shows file information

   Handle view paramset file info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemParamSetWidget::ViewFileInfo(void)
{
   if (this->GetAppFilePath().compare("") != 0)
   {
      const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
      C_SyvUpPacParamSetFileInfoPopUp * const pc_InfoDialog =
         new C_SyvUpPacParamSetFileInfoPopUp(*c_New,
                                             this->GetAppAbsoluteFilePath(),
                                             this->mu32_NodeIndex);

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
bool C_SyvUpPacListNodeItemParamSetWidget::IsViewFileInfoPossible(void) const
{
   return this->GetAppFilePath().compare("") != 0;
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
void C_SyvUpPacListNodeItemParamSetWidget::m_LoadFileInformation(bool & orq_FileExists, bool & orq_FlashwareWarning,
                                                                 bool & orq_TriggerRemove)
{
   C_SyvUpPacListNodeItemWidget::m_LoadFileInformation(orq_FileExists, orq_FlashwareWarning,
                                                       orq_TriggerRemove);

   // Special case: In case of a NVM HALC datablock with used default path, no check is acitve
   if ((this->IsDefaultPathActive() == false) &&
       (orq_FileExists == true))
   {
      //Special handling required
      const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
      C_SyvUpPacParamSetFileAddPopUp * const pc_InfoDialog =
         new C_SyvUpPacParamSetFileAddPopUp(*c_New, this->GetAppAbsoluteFilePath(), this->GetAppFilePath(),
                                            this->mu32_NodeIndex);

      //Read file info
      const int32_t s32_ReadResult = pc_InfoDialog->ReadFile();
      if (s32_ReadResult == C_NO_ERR)
      {
         const C_PuiSvNodeUpdateParamInfo & rc_ParamSetInfo = pc_InfoDialog->GetParamInfo();
         if (rc_ParamSetInfo.GetLastKnownCrc() != this->mc_ParamsetInfo.GetLastKnownCrc())
         {
            //New file has changes
            //Resize
            c_New->SetSize(QSize(1000, 761));

            if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
            {
               //User accepted changed file
               this->SetParamInfo(rc_ParamSetInfo);

               //Update data
               tgl_assert(C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformationParamInfo(this->mu32_ViewIndex,
                                                                                             this->mu32_NodeIndex,
                                                                                             this->GetAppNumber(),
                                                                                             rc_ParamSetInfo) ==
                          C_NO_ERR);
            }
            else
            {
               //User discarded file
               orq_TriggerRemove = true;
            }
         }
         else
         {
            //New file has no relevant changes
         }
      }
      else
      {
         QString c_Details = static_cast<QString>(C_GtGetText::h_GetText("File path: %1")).arg(
            this->GetAppAbsoluteFilePath()) +
                             "\nReason: ";
         //New file cannot be read
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         switch (s32_ReadResult)
         {
         case C_RD_WR:
            c_Details += C_GtGetText::h_GetText(
               "File does not exist or has invalid structure. See log file for details.");
            break;
         case C_CHECKSUM:
            c_Details += C_GtGetText::h_GetText("File CRC is not correct.");
            break;
         case C_CONFIG:
            c_Details += C_GtGetText::h_GetText("File has missing content. See log file for details.");
            break;
         default:
            c_Details += static_cast<QString>(C_GtGetText::h_GetText("Unknown reason. Error code: %1")).arg(
               s32_ReadResult);
            break;
         }

         c_Message.SetHeading(C_GtGetText::h_GetText("Update package configuration"));
         c_Message.SetDescription(C_GtGetText::h_GetText("File is not a valid parameter set image file."));
         c_Message.SetDetails(c_Details);
         c_Message.SetCustomMinHeight(180, 250);
         c_Message.Execute();
         //File invalid
         orq_TriggerRemove = true;
      }

      if (c_New != NULL)
      {
         c_New->HideOverlay();
         c_New->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_InfoDialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Creates and returns the title specific tool tip content

   \return
   Tool tip title
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacListNodeItemParamSetWidget::m_CreateToolTipTitle(void) const
{
   QString c_Title = static_cast<QString>("%1: Parameter Set Image File #%2").
                     arg(this->mc_OwnerSectionName).
                     arg(this->mu32_Number + 1U);

   return c_Title;
}
