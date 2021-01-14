//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for import of a HALC configuration file

   Dialog for import of a HALC configuration file

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLString.h"
#include "C_GtGetText.h"
#include "C_Uti.h"
#include "C_OgeWiCustomMessage.h"
#include "C_SdNdeHalcConfigImportDialog.h"
#include "ui_C_SdNdeHalcConfigImportDialog.h"

#include "C_PuiSdHandler.h"
#include "C_OSCHalcConfig.h"
#include "C_OSCHalcConfigStandaloneFiler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
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
   \param[in]     ou32_NodeIndex      Node index of node for updating the HALC definition and its configuration,
   \param[in]     orc_ImportFileName  File path of HALC configuration import file
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigImportDialog::C_SdNdeHalcConfigImportDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                             const uint32 ou32_NodeIndex,
                                                             const QString & orc_ImportFileName) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeHalcConfigImportDialog),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(ou32_NodeIndex),
   mc_ImportFileName(orc_ImportFileName),
   ms32_Result(C_NOACT)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdNdeHalcConfigImportDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &C_OgePubCancel::clicked, this,
           &C_SdNdeHalcConfigImportDialog::m_OnCancel);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigImportDialog::~C_SdNdeHalcConfigImportDialog(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigImportDialog::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Hardware Configuration"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Import"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Import Configuration"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks the preconditions for the import

   \param[out] orc_ErrorDetails   String with details of error. Empty string in case of no error

   \retval   C_NO_ERR    All ok, dialog can be opened
   \retval   C_RANGE     specified file does not exist
   \retval   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   \retval   C_CONFIG    HALC configuration content is invalid or incomplete
   \retval   C_CHECKSUM  HALC configuration config version mismatch
   \retval   C_RANGE     HALC configuration of node does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeHalcConfigImportDialog::PrepareDialog(QString & orc_ErrorDetails)
{
   sint32 s32_Return = C_NO_ERR;
   const C_OSCHalcConfig * pc_Config = C_PuiSdHandler::h_GetInstance()->GetHALCConfig(this->mu32_NodeIndex);

   orc_ErrorDetails = "";

   if (pc_Config != NULL)
   {
      // Load the configuration only once
      s32_Return = C_OSCHalcConfigStandaloneFiler::h_LoadFileStandalone(this->mc_ImportConfig,
                                                                        this->mc_ImportFileName.toStdString().c_str());

      if (s32_Return == C_NO_ERR)
      {
         if (this->mc_ImportConfig.c_DeviceType != pc_Config->c_DeviceName)
         {
            s32_Return = C_CHECKSUM;
            orc_ErrorDetails =
               C_GtGetText::h_GetText(("Imported Hardware configuration is not compatible to this device type.\n"
                                       "Current device type: " + pc_Config->c_DeviceName + "\n"
                                       "Device type of imported Hardware configuration: " +
                                       this->mc_ImportConfig.c_DeviceType + "\n").c_str());
         }
         else if (this->mc_ImportConfig.u32_DefinitionContentVersion != pc_Config->u32_ContentVersion)
         {
            s32_Return = C_CHECKSUM;
            orc_ErrorDetails =
               C_GtGetText::h_GetText(("Imported Hardware configuration version does not match the current "
                                       "used Hardware configuration version of this node.\n"
                                       "Current used version: " +
                                       C_SCLString::IntToStr(pc_Config->u32_ContentVersion) + "\n"
                                       "Version of imported Hardware configuration: " +
                                       C_SCLString::IntToStr(this->mc_ImportConfig.u32_DefinitionContentVersion) +
                                       "\n").c_str());
         }
         else
         {
            // Prepare dialog
            s32_Return = this->mpc_Ui->pc_TreeView->Init(*pc_Config, this->mc_ImportConfig);

            if (s32_Return != C_NO_ERR)
            {
               switch (s32_Return)
               {
               case C_NOACT:
                  orc_ErrorDetails = C_GtGetText::h_GetText("Imported Hardware configuration does not have any elements "
                                                            "which does match to the current configuration. "
                                                            "Nothing to import.");
                  break;
               default:
                  orc_ErrorDetails =
                     static_cast<QString>(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(s32_Return));
                  break;
               }
            }
         }
      }
      else
      {
         switch (s32_Return)
         {
         case C_RANGE:
            orc_ErrorDetails = C_GtGetText::h_GetText("The selected file does not exist.");
            break;
         case C_NOACT:
            orc_ErrorDetails = C_GtGetText::h_GetText("The selected file does not match the expected format.");
            break;
         case C_CONFIG:
            orc_ErrorDetails = C_GtGetText::h_GetText("The Hardware configuration content of the file is invalid.");
            break;
         default:
            orc_ErrorDetails = static_cast<QString>(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(s32_Return));
            break;
         }
      }
   }
   else
   {
      orc_ErrorDetails = C_GtGetText::h_GetText(
         "Current Hardware configuration of node is invalid. File can not be imported");
      s32_Return = C_RANGE;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Gets the result of the saving of the imported configuration in case of a click on the Ok button

   \param[out] orc_ErrorDetails   String with details of error. Empty string in case of no error

   \retval   C_NO_ERR   All ok, changes saved
   \retval   C_NOACT    Nothing done, dialog was not open or accepted yet
   \retval   C_RANGE    Node index is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeHalcConfigImportDialog::GetResult(QString & orc_ErrorDetails) const
{
   switch (this->ms32_Result)
   {
   case C_NO_ERR:
      orc_ErrorDetails = "";
      break;
   case C_RANGE:
      orc_ErrorDetails = C_GtGetText::h_GetText("The current node is invalid.");
      break;
   case C_NOACT:
      orc_ErrorDetails = C_GtGetText::h_GetText("The import dialog was not accepted or opened.");
      break;
   default:
      orc_ErrorDetails = static_cast<QString>(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(this->ms32_Result));
      break;
   }

   return this->ms32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigImportDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigImportDialog::m_OkClicked(void)
{
   std::vector<uint32> c_DomainIndices;
   std::vector<std::vector<uint32> > c_MissingChannelIndices;

   if (this->mpc_Ui->pc_TreeView->IsSelectionOfLinkedChannelsValid(c_DomainIndices, c_MissingChannelIndices) == false)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eQUESTION);
      c_Message.SetHeading(C_GtGetText::h_GetText("Invalid selection"));
      c_Message.SetDescription(
         C_GtGetText::h_GetText(
            "Your selection contains channels that are or will get linked to another channel which is not selected. "
            "Do you want to select all linked channels too?"));
      c_Message.SetOKButtonText(C_GtGetText::h_GetText("Select Linked Channels"));
      c_Message.SetNOButtonText(C_GtGetText::h_GetText("Cancel"));
      if (c_Message.Execute() == C_OgeWiCustomMessage::eOK)
      {
         this->mpc_Ui->pc_TreeView->CheckChannels(c_DomainIndices, c_MissingChannelIndices);
      }
   }
   else
   {
      C_OSCHalcConfig c_AdaptedConfig;

      this->mpc_Ui->pc_TreeView->GetAdaptedConfiguration(c_AdaptedConfig);

      this->ms32_Result = C_PuiSdHandler::h_GetInstance()->SetHALCConfig(this->mu32_NodeIndex, c_AdaptedConfig);

      this->mrc_ParentDialog.accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigImportDialog::m_OnCancel(void)
{
   this->mrc_ParentDialog.reject();
}
