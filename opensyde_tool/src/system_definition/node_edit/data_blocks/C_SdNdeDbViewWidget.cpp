//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing all node applications of a specific node (implementation)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_UsHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiProject.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeDbViewWidget.hpp"
#include "C_SdNdeDbProperties.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "ui_C_SdNdeDbViewWidget.h"
#include "C_PopSaveAsDialogWidget.hpp"
#include "C_SdNdeProgrammingOptions.hpp"
#include "C_OscHalcDefFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbViewWidget::C_SdNdeDbViewWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDbViewWidget),
   mu32_NodeIndex(0)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   // init icons
   this->mpc_Ui->pc_PushButtonAdd->SetCustomIcons("://images/IconAddEnabled.svg", "://images/IconAddHovered.svg",
                                                  "://images/IconAddClicked.svg", "://images/IconAddDisabled.svg");
   this->mpc_Ui->pc_PushButtonCodeGenerationOptions->SetSvg("://images/system_definition/IconGenerateCodeSettings.svg",
                                                            "://images/system_definition/IconGenerateCodeSettings.svg");

   //Connects
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDbListWidget::SigDelete, this,
           &C_SdNdeDbViewWidget::m_OnDelete);
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDbListWidget::SigErrorChange, this,
           &C_SdNdeDbViewWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDbListWidget::SigAppDisplay, this,
           &C_SdNdeDbViewWidget::m_OnAppDisplay);
   connect(this->mpc_Ui->pc_PushButtonAdd, &stw::opensyde_gui_elements::C_OgePubIconOnly::clicked, this,
           &C_SdNdeDbViewWidget::AddApp);
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDbListWidget::SigOwnedDataPoolsChanged, this,
           &C_SdNdeDbViewWidget::SigOwnedDataPoolsChanged);
   connect(this->mpc_Ui->pc_PushButtonCodeGenerationOptions, &QPushButton::clicked, this,
           &C_SdNdeDbViewWidget::m_ProgrammingOptions);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbViewWidget::~C_SdNdeDbViewWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::InitStaticNames(void) const
{
   //Tool tips
   this->mpc_Ui->pc_LabelApplicationName->SetToolTipInformation(
      C_GtGetText::h_GetText("Data Blocks"),
      C_GtGetText::h_GetText(
         "Data Blocks specify files that must be updated to the node.\n\n"
         "Those files can represent any kind of application or data. They can be either created by another tool chain "
         "or developed by using the openSYDE file generation feature. The second is available for devices with "
         "programming support or with NVM-based hardware configuration. "));

   this->mpc_Ui->pc_PushButtonAdd->SetToolTipInformation(C_GtGetText::h_GetText("Add"),
                                                         C_GtGetText::h_GetText("Add new Data Block."));

   this->mpc_Ui->pc_PushButtonCodeGenerationOptions->SetToolTipInformation(C_GtGetText::h_GetText(
                                                                              "Source Code Generation Settings"),
                                                                           C_GtGetText::h_GetText(
                                                                              "Edit source code generation settings."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node index

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::SetNodeIndex(const uint32_t ou32_NodeIndex)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);

   this->mu32_NodeIndex = ou32_NodeIndex;
   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      this->mpc_Ui->pc_ListWidget->SetIndex(ou32_NodeIndex);

      this->mpc_Ui->pc_ListWidget->clear();
      for (uint32_t u32_ItApp = 0; u32_ItApp < pc_Node->c_Applications.size(); ++u32_ItApp)
      {
         this->mpc_Ui->pc_ListWidget->AddApplication(ou32_NodeIndex, u32_ItApp);
      }
   }

   this->m_UpdateCount();
   this->m_HandleNoDatablocksLabel();
   this->m_HandleAddButtonAvailability();
   this->m_HandleCodeGenerationSettingsButtonAvailability();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Scrolls to the application with the index ou32_ApplicationIndex

   \param[in]  ou32_ApplicationIndex   Index of application
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::ShowApplication(const uint32_t ou32_ApplicationIndex) const
{
   this->mpc_Ui->pc_ListWidget->scrollToItem(
      this->mpc_Ui->pc_ListWidget->item(static_cast<int32_t>(ou32_ApplicationIndex)));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add a new application
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::AddApp(void)
{
   bool q_Continue = true;

   // check if empty path (new unsaved project)
   if (C_PuiProject::h_GetInstance()->IsEmptyProject() == true)
   {
      // warn user
      C_OgeWiCustomMessage c_MessageBox(this);
      c_MessageBox.SetType(C_OgeWiCustomMessage::eWARNING);
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Add Data Blocks"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                     "This project is not saved yet. Adding Data Blocks might cause "
                                     "problems with file or directory paths."));
      c_MessageBox.SetDetails(C_GtGetText::h_GetText(
                                 "Paths that are handled as relative to *.syde file can not be resolved correctly!"));
      c_MessageBox.SetOkButtonText(C_GtGetText::h_GetText("Continue"));
      c_MessageBox.SetCustomMinHeight(230, 270);
      c_MessageBox.SetCancelButtonText(C_GtGetText::h_GetText("Cancel"));
      if (c_MessageBox.Execute() != C_OgeWiCustomMessage::eOK)
      {
         q_Continue = false;
      }
   }

   if (q_Continue)
   {
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         if (pc_Node->IsAnyUpdateAvailable() == true)
         {
            this->m_AddManualApplication();
         }
         else
         {
            //no fbl support
            C_OgeWiCustomMessage c_MessageBox(this);
            c_MessageBox.SetType(C_OgeWiCustomMessage::E_Type::eWARNING);
            c_MessageBox.SetHeading(C_GtGetText::h_GetText("Add Datablocks"));
            c_MessageBox.SetDescription(C_GtGetText::h_GetText("There is no Flashloader support for this device type. "
                                                               "Data Blocks cannot be added."));
            c_MessageBox.SetCustomMinHeight(180, 180);
            c_MessageBox.Execute();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update applications
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::UpdateApplications(void) const
{
   this->mpc_Ui->pc_ListWidget->UpdateApplications();
   this->m_OnAppDisplay();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new project action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::AddFromTsp(void)
{
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_SdNdeDbAddNewProject * const pc_Dialog = new C_SdNdeDbAddNewProject(this->mu32_NodeIndex, *c_New);
   C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNode(this->mu32_NodeIndex);

   //Help
   //connect(pc_New, &C_OgePopUpDialog::SigHelp, pc_SettingsWidget, &C_GiSyLineWidget::HandleHelp);

   //Resize
   const QSize c_SIZE(970, 745);

   c_New->SetSize(c_SIZE);

   //init
   pc_Dialog->SetTspPath(C_UsHandler::h_GetInstance()->GetProjSdTopologyLastKnownTspPath());

   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      const QString c_HalcFileName = pc_Dialog->GetHalcDefinitionFileName();
      C_OgeWiCustomMessage c_Message(this);
      c_Message.SetHeading(C_GtGetText::h_GetText("Import TSP"));
      QString c_Description = "";
      if (c_HalcFileName == "")
      {
         c_Description = static_cast<QString>(C_GtGetText::h_GetText("Successfully created %1 Data Block(s).")).
                         arg(pc_Dialog->GetTspApplicationCount());
      }
      else
      {
         c_Description = static_cast<QString>(C_GtGetText::h_GetText("Successfully created %1 Data Block(s).\n"
                                                                     "Hardware Configurator: Hardware Definition File (%2) selected."))
                         .arg(pc_Dialog->GetTspApplicationCount()).arg(c_HalcFileName);
      }
      QString c_Details = "";
      c_Message.SetCustomMinHeight(180, 180);
      c_Message.SetCustomMinWidth(650);

      // check if there are existing datablocks
      if ((pc_Node != NULL) && (pc_Node->c_Applications.empty() == false))
      {
         m_DeleteAllDatablocks(mu32_NodeIndex, pc_Node->c_Applications);
      }

      for (uint32_t u32_It = 0; u32_It < pc_Dialog->GetTspApplicationCount(); ++u32_It)
      {
         C_OscNodeApplication c_Tmp;
         c_Tmp.e_Type = C_OscNodeApplication::ePROGRAMMABLE_APPLICATION;
         c_Tmp.q_Active = true;
         pc_Dialog->AddSelectedProject(u32_It, c_Tmp, c_Details);
         m_AddApplication(c_Tmp);
      }
      pc_Dialog->HandleCodeGenerationConfig();
      // last step, import halc definition from TSP
      AddHalcDefFromTsp(pc_Dialog);
      // Add warnings if any
      if (c_Details.isEmpty() == false)
      {
         c_Message.SetType(C_OgeWiCustomMessage::eWARNING);
         c_Description += C_GtGetText::h_GetText(" Some warnings occured. See details for more information.");
         c_Message.SetCustomMinHeight(180, 300);
         c_Message.SetDetails(c_Details);
      }

      c_Message.SetDescription(c_Description);
      c_Message.Execute();
   }

   if (c_New != NULL)
   {
      C_UsHandler::h_GetInstance()->SetProjSdTopologyLastKnownTspPath(pc_Dialog->GetTspPath());
      c_New->HideOverlay();
      c_New->deleteLater();
   }
} //lint !e593  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  imports HALC definition from TSP

 *          Loads syde_halc_def from TSP, loads it, clears HALC config, if there is any and then
 *          sets the new config. Emits a signal to C_SdNdeNodeEditWidget to run the magician and update datapool
 *          and HALC tab.

   \param[in]  opc_Dialog  TSP Import dialog widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::AddHalcDefFromTsp(C_SdNdeDbAddNewProject * const opc_Dialog)
{
   bool q_IsClear = false;

   const QString c_HalcPath = C_PuiProject::h_GetInstance()->GetFolderPath() +
                              opc_Dialog->GetProcessedHalcDefinitionPath();

   tgl_assert(C_PuiSdHandler::h_GetInstance()->IsHalcClear(this->mu32_NodeIndex, q_IsClear) == C_NO_ERR);

   uint32_t u32_NotHalcDpCount = 0UL;
   {
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
      if (pc_Node != NULL)
      {
         for (uint32_t u32_ItDp = 0UL; u32_ItDp < pc_Node->c_DataPools.size(); ++u32_ItDp)
         {
            const C_OscNodeDataPool & rc_Dp = pc_Node->c_DataPools[u32_ItDp];
            if (rc_Dp.e_Type != C_OscNodeDataPool::eHALC)
            {
               ++u32_NotHalcDpCount;
            }
         }
      }
   }
   if (u32_NotHalcDpCount > (C_OscNode::hu32_MAX_NUMBER_OF_DATA_POOLS_PER_NODE - 2UL))
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eERROR);

      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Select Hardware Description"));
      c_MessageBox.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                          "Cannot select hardware description,\n"
                                                          "because HAL Datapools may not be created,\n"
                                                          "as the max Datapool count (%1) would be exceeded.")).arg(
                                     C_OscNode::hu32_MAX_NUMBER_OF_DATA_POOLS_PER_NODE));
      c_MessageBox.SetCustomMinHeight(200, 270);
      c_MessageBox.Execute();
   }
   else
   {
      C_OscHalcConfig c_HalcConfig;

      const int32_t s32_LoadResult = C_OscHalcDefFiler::h_LoadFile(c_HalcConfig, c_HalcPath.toStdString().c_str());

      if (s32_LoadResult == C_NO_ERR)
      {
         // clean up if necessary
         if (q_IsClear == false)
         {
            // Clear configuration
            tgl_assert(C_PuiSdHandler::h_GetInstance()->ClearHalcConfig(this->mu32_NodeIndex) == C_NO_ERR);

            // Remove HAL Datapools
            tgl_assert(C_PuiSdHandler::h_GetInstance()->HalcRemoveDatapools(this->mu32_NodeIndex) == C_NO_ERR);
         }

         // set the HALC definition
         tgl_assert(C_PuiSdHandler::h_GetInstance()->SetHalcConfig(this->mu32_NodeIndex, c_HalcConfig) == C_NO_ERR);

         // run HALC magician and update GUI
         Q_EMIT (this->SigHalcLoadedFromTsp());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show/hide no data blocks declared label
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::m_HandleNoDatablocksLabel(void) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if ((pc_Node != NULL) && (pc_Node->IsAnyUpdateAvailable() == true))
   {
      //are there any data blocks?
      if (pc_Node->c_Applications.empty() == true)
      {
         //no data blocks display label
         this->mpc_Ui->pc_LabelNoDatablocks->setVisible(true);
         this->mpc_Ui->pc_LabelNoDatablocks->setText(C_GtGetText::h_GetText(
                                                        "No Data Block declared. \nAdd any via the '+' button"));
      }
      else
      {
         //data blocks present, no label required
         this->mpc_Ui->pc_LabelNoDatablocks->setVisible(false);
      }
   }
   else
   {
      this->mpc_Ui->pc_LabelNoDatablocks->setVisible(true);
      this->mpc_Ui->pc_LabelNoDatablocks->setText(C_GtGetText::h_GetText(
                                                     "No openSYDE/KEFEX protocol support. Data Blocks cannot be added."));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle add button availability
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::m_HandleAddButtonAvailability(void) const
{
   bool q_Enabled = true;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      if (pc_Node->IsAnyUpdateAvailable() == true)
      {
         q_Enabled = true;
      }
      else
      {
         q_Enabled = false;
      }
   }
   else
   {
      q_Enabled = false;
   }
   this->mpc_Ui->pc_PushButtonAdd->setEnabled(q_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new application

   \param[in]  oe_Type  Application type

   \return
   Application position (in node)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeDbViewWidget::m_AddApplication(const C_OscNodeApplication::E_Type oe_Type) const
{
   C_OscNodeApplication c_Appl;
   uint32_t u32_Retval;

   // add datablock
   c_Appl.c_Name = "DataBlock";
   c_Appl.c_Comment = "";
   c_Appl.q_Active = true;
   c_Appl.e_Type = oe_Type;
   u32_Retval = m_AddApplication(c_Appl);

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new application

   \param[in,out]  orc_Application  Application content

   \return
   Application position (in node)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeDbViewWidget::m_AddApplication(C_OscNodeApplication & orc_Application) const
{
   uint32_t u32_Retval = 0UL;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      u32_Retval = pc_Node->c_Applications.size();
      tgl_assert(C_PuiSdHandler::h_GetInstance()->InsertApplication(this->mu32_NodeIndex, u32_Retval,
                                                                    orc_Application) == C_NO_ERR);
      //No reload required
      this->mpc_Ui->pc_ListWidget->AddApplication(this->mu32_NodeIndex, u32_Retval);
      this->m_UpdateCount();
      this->m_HandleAddButtonAvailability();
      this->m_HandleCodeGenerationSettingsButtonAvailability();
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On App Display
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::m_OnAppDisplay() const
{
   this->m_UpdateCount();
   this->m_HandleNoDatablocksLabel();
   this->m_HandleAddButtonAvailability();
   this->m_HandleCodeGenerationSettingsButtonAvailability();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle delete

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::m_OnDelete(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex)
{
   tgl_assert(this->mu32_NodeIndex == ou32_NodeIndex);
   tgl_assert(C_PuiSdHandler::h_GetInstance()->RemoveApplication(ou32_NodeIndex, ou32_ApplicationIndex) == C_NO_ERR);

   //Trigger reload (also important for index update)
   this->SetNodeIndex(ou32_NodeIndex);

   // inform about changes of owned Datapools because they are now not assigned anymore
   Q_EMIT (this->SigOwnedDataPoolsChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Deletes all the node's applications

   \param[in]  ou32_NodeIndex    Index of selected node
   \param[in]  orc_Applications  Array of the node's applications
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::m_DeleteAllDatablocks(const uint32_t ou32_NodeIndex,
                                                const std::vector<C_OscNodeApplication> & orc_Applications)
{
   tgl_assert(this->mu32_NodeIndex == ou32_NodeIndex);
   while (orc_Applications.size() > 0)
   {
      // as the vector gets smaller, we just remove the first element till it's empty
      tgl_assert(C_PuiSdHandler::h_GetInstance()->RemoveApplication(ou32_NodeIndex, 0) == C_NO_ERR);
   }
   this->SetNodeIndex(ou32_NodeIndex);

   // inform about changes of owned Datapools because they are now not assigned anymore
   Q_EMIT (this->SigOwnedDataPoolsChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update data block count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::m_UpdateCount(void) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      this->mpc_Ui->pc_LabelApplicationName->setText(static_cast<QString>("Data Blocks (%1)").arg(pc_Node->
                                                                                                  c_Applications.size()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of programming options button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::m_ProgrammingOptions(void) const
{
   const std::vector<const C_OscNodeApplication *> c_ProgrammableApplications =
      C_PuiSdHandler::h_GetInstance()->GetProgrammableApplications(this->mu32_NodeIndex);

   if (c_ProgrammableApplications.size() > 0)
   {
      //Set parent for better hierarchy handling via window manager
      const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this->parentWidget(), this->parentWidget());
      const QPointer<const C_SdNdeProgrammingOptions> c_Dialog = new C_SdNdeProgrammingOptions(*c_New,
                                                                                               this->mu32_NodeIndex);

      //Help
      //connect(pc_New, &C_OgePopUpDialog::SigHelp, pc_SettingsWidget, &C_GiSyLineWidget::HandleHelp);

      //Resize
      c_New->SetSize(QSize(810, 573));

      if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
      {
         if (c_Dialog.isNull() == false)
         {
            c_Dialog->Save();
         }
      }
      //Hide overlay after dialog is not relevant anymore
      if (c_New.isNull() == false)
      {
         c_New->HideOverlay();
         c_New->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_New and pc_Dialog and the Qt memory management
   else
   {
      C_OgeWiCustomMessage c_Message(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eWARNING);
      c_Message.SetHeading(C_GtGetText::h_GetText("File generation settings"));
      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "File generation settings are not available. "
                                  "\nThere are no Data Blocks with active file generation declared."));
      c_Message.SetCustomMinHeight(180, 180);
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new manual application
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::m_AddManualApplication(void)
{
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_SdNdeDbProperties * const pc_Dialog = new C_SdNdeDbProperties(this->mu32_NodeIndex, -1, *c_New);

   //Resize
   c_New->SetSize(C_SdNdeDbProperties::h_GetBinaryWindowSize());

   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      C_OscNodeApplication c_Tmp;

      uint32_t u32_Index;
      pc_Dialog->ApplyNewData(c_Tmp);
      u32_Index = m_AddApplication(c_Tmp);
      pc_Dialog->HandleDataPools(u32_Index);
      // Inform about change (this also triggers this->UpdateApplications())
      Q_EMIT (this->SigOwnedDataPoolsChanged());
   }

   if (c_New != NULL)
   {
      c_New->HideOverlay();
      c_New->deleteLater();
   }
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle visibility of button "File generation settings"
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbViewWidget::m_HandleCodeGenerationSettingsButtonAvailability(void) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      tgl_assert(pc_Node->pc_DeviceDefinition != NULL);
      if (pc_Node->pc_DeviceDefinition != NULL)
      {
         tgl_assert(pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size());
         if (pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size())
         {
            if ((pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].q_ProgrammingSupport ==
                 true) &&
                (C_PuiSdHandler::h_GetInstance()->GetProgrammableApplications(this->mu32_NodeIndex).size() > 0UL))
            {
               this->mpc_Ui->pc_PushButtonCodeGenerationOptions->setVisible(true);
            }
            else
            {
               this->mpc_Ui->pc_PushButtonCodeGenerationOptions->setVisible(false);
            }
         }
      }
   }
}
