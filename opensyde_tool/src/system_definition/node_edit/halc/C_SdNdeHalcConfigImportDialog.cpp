//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for import of a HALC configuration file

   Dialog for import of a HALC configuration file

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclString.hpp"
#include "C_GtGetText.hpp"
#include "C_Uti.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_SdNdeHalcConfigImportDialog.hpp"
#include "ui_C_SdNdeHalcConfigImportDialog.h"

#include "C_PuiSdHandler.hpp"
#include "C_OscHalcConfig.hpp"
#include "C_OscHalcConfigStandaloneFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::scl;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
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

   \param[in,out]  orc_Parent          Reference to parent
   \param[in]      ou32_NodeIndex      Node index of node for updating the HALC definition and its configuration,
   \param[in]      orc_ImportFileName  File path of HALC configuration import file
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigImportDialog::C_SdNdeHalcConfigImportDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                             const uint32_t ou32_NodeIndex,
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
C_SdNdeHalcConfigImportDialog::~C_SdNdeHalcConfigImportDialog(void) noexcept
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

   \param[out]  orc_ErrorDetails    String with details of error. Empty string in case of no error

   \retval   C_NO_ERR    All ok, dialog can be opened
   \retval   C_RANGE     specified file does not exist
   \retval   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   \retval   C_CONFIG    HALC configuration content is invalid or incomplete
   \retval   C_CHECKSUM  HALC configuration config version mismatch
   \retval   C_RANGE     HALC configuration of node does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeHalcConfigImportDialog::PrepareDialog(QString & orc_ErrorDetails)
{
   int32_t s32_Return = C_NO_ERR;
   const C_OscHalcConfig * const pc_Config = C_PuiSdHandler::h_GetInstance()->GetHalcConfig(this->mu32_NodeIndex);

   orc_ErrorDetails = "";

   if (pc_Config != NULL)
   {
      // Load the configuration only once
      s32_Return = C_OscHalcConfigStandaloneFiler::h_LoadFileStandalone(this->mc_ImportConfig,
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
                                       C_SclString::IntToStr(pc_Config->u32_ContentVersion) + "\n"
                                       "Version of imported Hardware configuration: " +
                                       C_SclString::IntToStr(this->mc_ImportConfig.u32_DefinitionContentVersion) +
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

            //Consistency
            {
               QString c_ErrDetails;
               const bool q_Consistent = C_SdNdeHalcConfigImportDialog::mh_CheckConsistency(pc_Config,
                                                                                            this->mc_ImportConfig,
                                                                                            c_ErrDetails);
               if (q_Consistent == false)
               {
                  s32_Return = C_CONFIG;
                  orc_ErrorDetails = C_GtGetText::h_GetText(
                     "Current Hardware configuration of node is invalid. File does not match the definition. Error:\n");
                  orc_ErrorDetails += c_ErrDetails;
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
            orc_ErrorDetails =
               static_cast<QString>(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(s32_Return));
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

   \param[out]  orc_ErrorDetails    String with details of error. Empty string in case of no error

   \retval   C_NO_ERR   All ok, changes saved
   \retval   C_NOACT    Nothing done, dialog was not open or accepted yet
   \retval   C_RANGE    Node index is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeHalcConfigImportDialog::GetResult(QString & orc_ErrorDetails) const
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
      orc_ErrorDetails =
         static_cast<QString>(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(this->ms32_Result));
      break;
   }

   return this->ms32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigImportDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
   std::vector<uint32_t> c_DomainIndices;
   std::vector<std::vector<uint32_t> > c_MissingChannelIndices;

   if (this->mpc_Ui->pc_TreeView->IsSelectionOfLinkedChannelsValid(c_DomainIndices, c_MissingChannelIndices) == false)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eQUESTION);
      c_Message.SetHeading(C_GtGetText::h_GetText("Invalid selection"));
      c_Message.SetDescription(
         C_GtGetText::h_GetText(
            "Your selection contains channels that are or will get linked to another channel which is not selected. "
            "Do you want to select all linked channels too?"));
      c_Message.SetOkButtonText(C_GtGetText::h_GetText("Select Linked Channels"));
      c_Message.SetNoButtonText(C_GtGetText::h_GetText("Cancel"));
      if (c_Message.Execute() == C_OgeWiCustomMessage::eOK)
      {
         this->mpc_Ui->pc_TreeView->CheckChannels(c_DomainIndices, c_MissingChannelIndices);
      }
   }
   else
   {
      C_OscHalcConfig c_AdaptedConfig;

      this->mpc_Ui->pc_TreeView->GetAdaptedConfiguration(c_AdaptedConfig);

      this->ms32_Result = C_PuiSdHandler::h_GetInstance()->SetHalcConfig(this->mu32_NodeIndex, c_AdaptedConfig);

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check consistency

   \param[in]      opc_Config             Config
   \param[in]      orc_ConfigStandalone   Config standalone
   \param[in,out]  orc_ErrorDetails       Error details

   \return
   Consistency state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcConfigImportDialog::mh_CheckConsistency(const C_OscHalcConfig * const opc_Config,
                                                        const C_OscHalcConfigStandalone & orc_ConfigStandalone,
                                                        QString & orc_ErrorDetails)
{
   bool q_Consistent = true;

   if (opc_Config != NULL)
   {
      if (opc_Config->GetDomainSize() == orc_ConfigStandalone.c_Domains.size())
      {
         for (uint32_t u32_ItDomain = 0UL; u32_ItDomain < opc_Config->GetDomainSize(); ++u32_ItDomain)
         {
            const C_OscHalcConfigDomain * const pc_RefConfig = opc_Config->GetDomainConfigDataConst(
               u32_ItDomain);
            if (pc_RefConfig != NULL)
            {
               const C_OscHalcConfigStandaloneDomain & rc_NewConfig =
                  orc_ConfigStandalone.c_Domains[u32_ItDomain];
               if (pc_RefConfig->c_ChannelConfigs.size() == rc_NewConfig.c_ChannelConfigs.size())
               {
                  for (uint32_t u32_ItChan = 0UL; u32_ItChan < pc_RefConfig->c_ChannelConfigs.size();
                       ++u32_ItChan)
                  {
                     const C_OscHalcConfigChannel & rc_RefChan = pc_RefConfig->c_ChannelConfigs[u32_ItChan];
                     const C_OscHalcConfigChannel & rc_NewChan = rc_NewConfig.c_ChannelConfigs[u32_ItChan];
                     if (rc_RefChan.c_Parameters.size() == rc_NewChan.c_Parameters.size())
                     {
                        for (uint32_t u32_ItPar = 0UL; (u32_ItPar < rc_RefChan.c_Parameters.size()) && q_Consistent;
                             ++u32_ItPar)
                        {
                           const C_OscHalcConfigParameterStruct & rc_RefPar =
                              rc_RefChan.c_Parameters[u32_ItPar];
                           const C_OscHalcConfigParameterStruct & rc_NewPar =
                              rc_NewChan.c_Parameters[u32_ItPar];
                           if (rc_RefPar.c_ParameterElements.size() == rc_NewPar.c_ParameterElements.size())
                           {
                              if (rc_RefPar.c_ParameterElements.size() == 0UL)
                              {
                                 q_Consistent = C_SdNdeHalcConfigImportDialog::mh_CheckConsistencyEl(rc_RefPar,
                                                                                                     rc_NewPar,
                                                                                                     orc_ErrorDetails,
                                                                                                     pc_RefConfig->c_Name.c_str(),
                                                                                                     rc_RefChan.c_Name.c_str(),
                                                                                                     "");
                              }
                              for (uint32_t u32_ItParEl = 0UL;
                                   (u32_ItParEl < rc_RefPar.c_ParameterElements.size()) && q_Consistent;
                                   ++u32_ItParEl)
                              {
                                 const C_OscHalcConfigParameter & rc_RefParEl =
                                    rc_RefPar.c_ParameterElements[u32_ItParEl];
                                 const C_OscHalcConfigParameter & rc_NewParEl =
                                    rc_NewPar.c_ParameterElements[u32_ItParEl];
                                 q_Consistent = C_SdNdeHalcConfigImportDialog::mh_CheckConsistencyEl(rc_RefParEl,
                                                                                                     rc_NewParEl,
                                                                                                     orc_ErrorDetails,
                                                                                                     pc_RefConfig->c_Name.c_str(),
                                                                                                     rc_RefChan.c_Name.c_str(),
                                                                                                     " elements");
                              }
                           }
                           else
                           {
                              q_Consistent = false;
                              //lint -e{1946} Qt interface
                              orc_ErrorDetails +=
                                 QString(
                                    "Num parameter elements differ for domain %1, channel %2 (def: %3 <-> new: %4)\n")
                                 .arg(
                                    pc_RefConfig->c_Name.c_str()).arg(rc_RefChan.c_Name.c_str()).arg(
                                    rc_RefChan.c_Parameters.size()).arg(
                                    rc_NewChan.c_Parameters.size());
                           }
                        }
                     }
                     else
                     {
                        q_Consistent = false;
                        //lint -e{1946} Qt interface
                        orc_ErrorDetails +=
                           QString("Num parameters differ for domain %1, channel %2 (def: %3 <-> new: %4)\n").arg(
                              pc_RefConfig->c_Name.c_str()).arg(rc_RefChan.c_Name.c_str()).arg(
                              rc_RefChan.c_Parameters.size()).arg(
                              rc_NewChan.c_Parameters.size());
                     }
                  }
               }
               else
               {
                  q_Consistent = false;
                  //lint -e{1946} Qt interface
                  orc_ErrorDetails +=
                     QString("Num channel configurations differ for domain %1 (def: %2 <-> new: %3)\n").arg(
                        pc_RefConfig->c_Name.c_str()).arg(pc_RefConfig->c_ChannelConfigs.size()).arg(
                        rc_NewConfig.c_ChannelConfigs.size());
               }
               if (pc_RefConfig->c_Channels.size() != rc_NewConfig.c_Channels.size())
               {
                  q_Consistent = false;
                  //lint -e{1946} Qt interface
                  orc_ErrorDetails += QString("Num channels differ for domain %1 (def: %2 <-> new: %3)\n").arg(
                     pc_RefConfig->c_Name.c_str()).arg(pc_RefConfig->c_Channels.size()).arg(
                     rc_NewConfig.c_Channels.size());
               }
               //Should exist but does not, probably not stored in standalone config
               //if (pc_RefConfig->c_ChannelUseCases.size() != rc_NewConfig.c_ChannelUseCases.size())
               //{
               //   q_Consistent = false;
               //   orc_ErrorDetails += QString("Num channel use-cases differ for domain %1 (def: %2 <-> new:
               // %3)\n").arg(
               //      pc_RefConfig->c_Name.c_str()).arg(pc_RefConfig->c_ChannelUseCases.size()).arg(
               //      rc_NewConfig.c_ChannelUseCases.size());
               //}
               //if (pc_RefConfig->c_ChannelValues.c_InputValues.size() !=
               //    rc_NewConfig.c_ChannelValues.c_InputValues.size())
               //{
               //   q_Consistent = false;
               //   orc_ErrorDetails += QString("Num channel input-values differ for domain %1 (def: %2 <-> new:
               // %3)\n").arg(
               //      pc_RefConfig->c_Name.c_str()).arg(pc_RefConfig->c_ChannelValues.c_InputValues.size()).arg(
               //      rc_NewConfig.c_ChannelValues.c_InputValues.size());
               //}
               //if (pc_RefConfig->c_ChannelValues.c_Parameters.size() !=
               //    rc_NewConfig.c_ChannelValues.c_Parameters.size())
               //{
               //   q_Consistent = false;
               //   orc_ErrorDetails += QString("Num channel param-values differ for domain %1 (def: %2 <-> new:
               // %3)\n").arg(
               //      pc_RefConfig->c_Name.c_str()).arg(pc_RefConfig->c_ChannelValues.c_Parameters.size()).arg(
               //      rc_NewConfig.c_ChannelValues.c_Parameters.size());
               //}
               //if (pc_RefConfig->c_ChannelValues.c_OutputValues.size() !=
               //    rc_NewConfig.c_ChannelValues.c_OutputValues.size())
               //{
               //   q_Consistent = false;
               //   orc_ErrorDetails +=
               //      QString("Num channel output-values differ for domain %1 (def: %2 <-> new: %3)\n").arg(
               //         pc_RefConfig->c_Name.c_str()).arg(pc_RefConfig->c_ChannelValues.c_OutputValues.size()).arg(
               //         rc_NewConfig.c_ChannelValues.c_OutputValues.size());
               //}
               //if (pc_RefConfig->c_ChannelValues.c_StatusValues.size() !=
               //    rc_NewConfig.c_ChannelValues.c_StatusValues.size())
               //{
               //   q_Consistent = false;
               //   orc_ErrorDetails +=
               //      QString("Num channel status-values differ for domain %1 (def: %2 <-> new: %3)\n").arg(
               //         pc_RefConfig->c_Name.c_str()).arg(pc_RefConfig->c_ChannelValues.c_StatusValues.size()).arg(
               //         rc_NewConfig.c_ChannelValues.c_StatusValues.size());
               //}
            }
         }
      }
      else
      {
         q_Consistent = false;
         //lint -e{1946} Qt interface
         orc_ErrorDetails += QString("Num comains differ (def: %1 <-> new: %2)\n").arg(
            opc_Config->GetDomainSize()).arg(orc_ConfigStandalone.c_Domains.size());
      }
   }
   return q_Consistent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check consistency element

   \param[in]      orc_Ref             Ref
   \param[in]      orc_New             New
   \param[in,out]  orc_ErrorDetails    Error details
   \param[in]      orc_Domain          Domain
   \param[in]      orc_Channel         Channel
   \param[in]      orc_CheckAddendum   Check addendum

   \return
   Consistency state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcConfigImportDialog::mh_CheckConsistencyEl(const C_OscHalcConfigParameter & orc_Ref,
                                                          const C_OscHalcConfigParameter & orc_New,
                                                          QString & orc_ErrorDetails, const QString & orc_Domain,
                                                          const QString & orc_Channel,
                                                          const QString & orc_CheckAddendum)
{
   bool q_Consistent = true;
   QString c_Type;
   QString c_Ref;
   QString c_New;

   if (orc_Ref.c_Value.GetArray() != orc_New.c_Value.GetArray())
   {
      c_Type = "array state";
      //lint -e{730,1785} Qt interface
      c_Ref = QString::number(orc_Ref.c_Value.GetArray());
      //lint -e{730,1785} Qt interface
      c_New = QString::number(orc_New.c_Value.GetArray());
      q_Consistent = false;
   }
   if (orc_Ref.c_Value.GetArraySize() != orc_New.c_Value.GetArraySize())
   {
      c_Type = "array size";
      c_Ref = QString::number(orc_Ref.c_Value.GetArraySize());
      c_New = QString::number(orc_New.c_Value.GetArraySize());
      q_Consistent = false;
   }
   if (orc_Ref.c_Value.GetType() != orc_New.c_Value.GetType())
   {
      c_Type = "type";
      c_Ref = C_SdNdeDpUtil::h_ConvertContentTypeToString(orc_Ref.c_Value.GetType());
      c_New = C_SdNdeDpUtil::h_ConvertContentTypeToString(orc_New.c_Value.GetType());
      q_Consistent = false;
   }
   if (not q_Consistent == false)
   {
      //lint -e{1946} Qt interface
      orc_ErrorDetails +=
         QString("Parameter%1 %2 differ for domain %3, channel %4 (def: %5 <-> new: %6)\n")
         .arg(orc_CheckAddendum).arg(c_Type).arg(orc_Domain).arg(orc_Channel).arg(c_Ref).arg(c_New);
   }
   return q_Consistent;
}
