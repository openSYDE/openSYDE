//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling system view

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QApplication>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"

#include "C_SyvHandlerWidget.hpp"
#include "ui_C_SyvHandlerWidget.h"

#include "C_PopUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_HeHandler.hpp"
#include "C_PuiProject.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_PopErrorHandling.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_TblTreDataElementModel.hpp"

#include "constants.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint32_t C_SyvHandlerWidget::mhu32_USER_INPUT_FUNC_APPLY = 0U;
const uint32_t C_SyvHandlerWidget::mhu32_USER_INPUT_FUNC_SETTINGS = 1U;
const uint32_t C_SyvHandlerWidget::mhu32_USER_INPUT_FUNC_DEVICECONFIG = 2U;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvHandlerWidget::C_SyvHandlerWidget(QWidget * const opc_Parent) :
   C_NagUseCaseWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvHandlerWidget),
   mpc_SetupWidget(NULL),
   mpc_UpdateWidget(NULL),
   mpc_DashboardsWidget(NULL),
   ms32_SubMode(-1),
   mu32_Index(0U),
   mq_DataChanged(false)
{
   mpc_Ui->setupUi(this);

   // configure toolbar functions
   // the order of adding the function is very important
   C_NagToolBarButtonProperties c_ButtonProperties;

   // Function index 0: mhu32_USER_INPUT_FUNC_APPLY
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Save Project");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Save Project");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText("Save project data to file.");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 1: mhu32_USER_INPUT_FUNC_SETTINGS
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Dashboard Configuration");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Configure Dashboard");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText("Edit Dashboard transmission settings.");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 2: mhu32_USER_INPUT_FUNC_DEVICECONFIG
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Device Configuration");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Device Configuration");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText(
      "Scan for devices, assign and configure device interface properties like node ID, bitrate, ...");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_DashboardsWidget and the Qt memory management
C_SyvHandlerWidget::~C_SyvHandlerWidget(void)
{
   //Necessary for use-case switch
   if (this->mc_Interaction.isNull() == false)
   {
      this->mc_Interaction->setVisible(false);
   }

   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for toolbar

   Saves the project.

   \param[in]  ou32_FuncNumber   Number of function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvHandlerWidget::UserInputFunc(const uint32_t ou32_FuncNumber)
{
   //Default
   Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, true);

   switch (ou32_FuncNumber)
   {
   case mhu32_USER_INPUT_FUNC_APPLY:
      //BEFORE save
      if ((this->mpc_DashboardsWidget != NULL) &&
          (this->mpc_DashboardsWidget->GetConnectActive() == true))
      {
         Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, false);
      }

      this->Save();
      break;
   case mhu32_USER_INPUT_FUNC_SETTINGS:
      if (this->mpc_DashboardsWidget != NULL)
      {
         this->mpc_DashboardsWidget->OpenSettings();
      }
      break;
   case mhu32_USER_INPUT_FUNC_DEVICECONFIG:
      if (this->mpc_SetupWidget != NULL)
      {
         this->mpc_SetupWidget->StartDeviceConfiguration();
      }
      break;
   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the actual mode of the system view handler

   Do not call this function internally. Use the signal SigChangeMode to
   inform the entire application about the change.

   \param[in]     os32_SubMode     Actual sub mode
   \param[in]     ou32_Index       The view index
   \param[in]     ou32_Flag        Flag for special functionality
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvHandlerWidget::SetSubMode(const int32_t os32_SubMode, const uint32_t ou32_Index, const uint32_t ou32_Flag)
{
   //Change cursor
   QApplication::setOverrideCursor(Qt::WaitCursor);

   // set the icons. this can not be done in constructor
   QIcon c_IconApply("://images/IconSave.svg");
   c_IconApply.addPixmap(static_cast<QPixmap>("://images/IconSaveDisabled.svg"), QIcon::Disabled);
   Q_EMIT (this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_APPLY, c_IconApply));
   QIcon c_IconSettings("://images/SettingsIcon.svg");
   c_IconSettings.addPixmap(static_cast<QPixmap>("://images/SettingsIconDisabled.svg"), QIcon::Disabled);
   Q_EMIT (this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, c_IconSettings));

   // if the submode is setup it can be the initial call or a kind of refresh because
   // of the start screen
   //Flag values of 1 indicate the current widget index might not point to the same view anymore
   if (((os32_SubMode != this->ms32_SubMode) ||
        (ou32_Index != this->mu32_Index)) || (ou32_Flag == 1UL))
   {
      int32_t s32_Index;

      //Store hash during load
      C_PuiSvHandler::h_GetInstance()->SetAllowViewHashCache(true);

      // Deactivate all buttons in the first step
      Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, false));
      Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_DEVICECONFIG, false));

      // delete or remove from the layout the other widgets if necessary
      if (this->mpc_SetupWidget != NULL)
      {
         disconnect(this->mpc_SetupWidget, &C_SyvSeSetupWidget::SigChanged, this, &C_SyvHandlerWidget::m_DataChanged);
         disconnect(this->mpc_SetupWidget, &C_SyvSeSetupWidget::SigCheckViewError, this,
                    &C_SyvHandlerWidget::m_ErrorChanged);
         disconnect(this->mpc_SetupWidget, &C_SyvSeSetupWidget::SigEnableConfiguration,
                    this, &C_SyvHandlerWidget::m_EnableConfiguration);
         this->mpc_Ui->pc_VerticalLayout->removeWidget(this->mpc_SetupWidget);
         delete this->mpc_SetupWidget;
         this->mpc_SetupWidget = NULL;
      }
      if (this->mpc_UpdateWidget != NULL)
      {
         disconnect(this->mpc_UpdateWidget, &C_SyvUpUpdateWidget::SigChanged, this, &C_SyvHandlerWidget::m_DataChanged);
         disconnect(this->mpc_UpdateWidget, &C_SyvUpUpdateWidget::SigBlockDragAndDrop, this,
                    &C_SyvHandlerWidget::SigBlockDragAndDrop);
         this->mpc_Ui->pc_VerticalLayout->removeWidget(this->mpc_UpdateWidget);
         delete this->mpc_UpdateWidget;
         this->mpc_UpdateWidget = NULL;
      }
      if (this->mpc_DashboardsWidget != NULL)
      {
         //Necessary for submode switch
         if (this->mc_Interaction.isNull() == false)
         {
            this->mc_Interaction->setVisible(false);
         }
         disconnect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigChanged, this,
                    &C_SyvHandlerWidget::m_DataChanged);
         disconnect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigNumberDashboardsChanged, this,
                    &C_SyvHandlerWidget::m_DashboardCountChanged);
         disconnect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigSetDarkModePushButtonIcon,
                    this, &C_SyvHandlerWidget::m_SetPushButtonDarkIconSvg);
         disconnect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigSetConnectPushButtonIcon,
                    this, &C_SyvHandlerWidget::m_SetConnectPushButtonIcon);
         disconnect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigSetConfigurationAvailable,
                    this, &C_SyvHandlerWidget::m_SetConfigurationAvailable);
         disconnect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigBlockDragAndDrop,
                    this, &C_SyvHandlerWidget::SigBlockDragAndDrop);
         this->mpc_Ui->pc_VerticalLayout->removeWidget(this->mpc_DashboardsWidget);
         delete this->mpc_DashboardsWidget;
         this->mpc_DashboardsWidget = NULL;

         // Clear the model of the element selector dialog. The model must refreshed in case of an other
         // or changed view
         // Do not clean it up in destructor of dashboard widget. The order of children destructor and this clean up
         // can cause problems.
         C_TblTreDataElementModel::h_CleanUp();
      }

      // create the new widget
      if (os32_SubMode == ms32_SUBMODE_SYSVIEW_SETUP)
      {
         //Handle buttons
         Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_APPLY, true));
         Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_DEVICECONFIG, true));

         QIcon c_IconDevice("://images/system_views/Device.svg");
         c_IconDevice.addPixmap(static_cast<QPixmap>("://images/system_views/DeviceDisabled.svg"), QIcon::Disabled);
         Q_EMIT (this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_DEVICECONFIG, c_IconDevice));

         this->mpc_SetupWidget = new C_SyvSeSetupWidget(ou32_Index, this);

         connect(this->mpc_SetupWidget, &C_SyvSeSetupWidget::SigChanged, this, &C_SyvHandlerWidget::m_DataChanged);
         connect(this->mpc_SetupWidget, &C_SyvSeSetupWidget::SigCheckViewError, this,
                 &C_SyvHandlerWidget::m_ErrorChanged);
         connect(this->mpc_SetupWidget, &C_SyvSeSetupWidget::SigEnableConfiguration,
                 this, &C_SyvHandlerWidget::m_EnableConfiguration);
         this->mpc_Ui->pc_VerticalLayout->addWidget(this->mpc_SetupWidget);

         s32_Index = this->mpc_Ui->pc_VerticalLayout->indexOf(this->mpc_SetupWidget);
         this->mpc_Ui->pc_VerticalLayout->setStretch(s32_Index, 1);

         this->mpc_SetupWidget->LoadScene();
         this->mpc_SetupWidget->show();
      }
      if (os32_SubMode == ms32_SUBMODE_SYSVIEW_UPDATE)
      {
         //Handle buttons
         Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_APPLY, true));

         this->mpc_UpdateWidget = new C_SyvUpUpdateWidget(ou32_Index, this->parentWidget(), this);

         connect(this->mpc_UpdateWidget, &C_SyvUpUpdateWidget::SigChanged, this, &C_SyvHandlerWidget::m_DataChanged);
         connect(this->mpc_UpdateWidget, &C_SyvUpUpdateWidget::SigBlockDragAndDrop, this,
                 &C_SyvHandlerWidget::SigBlockDragAndDrop);
         this->mpc_Ui->pc_VerticalLayout->addWidget(this->mpc_UpdateWidget);

         s32_Index = this->mpc_Ui->pc_VerticalLayout->indexOf(this->mpc_UpdateWidget);
         this->mpc_Ui->pc_VerticalLayout->setStretch(s32_Index, 1);

         this->mpc_UpdateWidget->LoadScene();
         this->mpc_UpdateWidget->show();
      }
      if (os32_SubMode == ms32_SUBMODE_SYSVIEW_DASHBOARD)
      {
         const bool q_ServiceModeActive = C_PuiSvHandler::h_GetInstance()->GetServiceModeActive();
         this->mc_Interaction = new C_SyvDaDashboardInteraction();
         this->mc_Interaction->SetPushButtonConnectSvg("://images/system_views/IconDisconnected.svg", false);
         connect(this->mc_Interaction, &C_SyvDaDashboardInteraction::SigPushButtonDarkModePressed, this,
                 &C_SyvHandlerWidget::OnPushButtonIconPress);
         connect(this->mc_Interaction, &C_SyvDaDashboardInteraction::SigPushButtonConnectPressed, this,
                 &C_SyvHandlerWidget::m_OnPushButtonConnectPress);
         Q_EMIT (this->SigSetInteractionWidget(this->mc_Interaction));
         //Handle buttons
         Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_APPLY, true));
         Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, true));

         Q_EMIT (this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, !q_ServiceModeActive));

         this->mpc_DashboardsWidget = new C_SyvDaDashboardsWidget(ou32_Index, this->parentWidget(), this);

         connect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigChanged, this,
                 &C_SyvHandlerWidget::m_DataChanged);
         connect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigNumberDashboardsChanged, this,
                 &C_SyvHandlerWidget::m_DashboardCountChanged);
         connect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigSetDarkModePushButtonIcon,
                 this, &C_SyvHandlerWidget::m_SetPushButtonDarkIconSvg);
         connect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigSetConnectPushButtonIcon,
                 this, &C_SyvHandlerWidget::m_SetConnectPushButtonIcon);
         connect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigSetConfigurationAvailable,
                 this, &C_SyvHandlerWidget::m_SetConfigurationAvailable);
         connect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigBlockDragAndDrop,
                 this, &C_SyvHandlerWidget::SigBlockDragAndDrop);
         this->mpc_Ui->pc_VerticalLayout->addWidget(this->mpc_DashboardsWidget);

         s32_Index = this->mpc_Ui->pc_VerticalLayout->indexOf(this->mpc_DashboardsWidget);
         this->mpc_Ui->pc_VerticalLayout->setStretch(s32_Index, 1);

         this->mpc_DashboardsWidget->LoadDarkMode();
         //Do error check AFTER connections are up
         this->mpc_DashboardsWidget->CheckError();
         this->mpc_DashboardsWidget->show();
      }

      //Reset
      C_PuiSvHandler::h_GetInstance()->SetAllowViewHashCache(false);

      // update of the index values
      this->ms32_SubMode = os32_SubMode;
      this->mu32_Index = ou32_Index;
   }
   //Change cursor
   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvHandlerWidget::Save(void)
{
   const uint16_t u16_Timer = osc_write_log_performance_start();

   QApplication::setOverrideCursor(Qt::WaitCursor);

   // save all changes of the active edit widgets to the core
   if (this->mpc_SetupWidget != NULL)
   {
      this->mpc_SetupWidget->Save();
   }
   if (this->mpc_DashboardsWidget != NULL)
   {
      this->mpc_DashboardsWidget->Save();
   }
   if (C_PopUtil::h_CheckCriticalNamingConflict(this, true) == false)
   {
      //Check if valid save
      if (C_PuiProject::h_GetInstance()->IsEmptyProject() == true)
      {
         // open project save
         this->SaveAs();
      }
      else
      {
         //Save to file
         C_PopErrorHandling::h_ProjectSaveErr(C_PuiProject::h_GetInstance()->Save(), this);

         this->mq_DataChanged = false;
      }
   }
   QApplication::restoreOverrideCursor();
   osc_write_log_performance_stop(u16_Timer, "System Commisioning Save");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to prepare closing the widget

   Prepare each widget for close and do specific actions to prepare the widget for close action

   \return
   true     Preparation successful. Can be closed.
   false    Preparation not finished. Can not be closed.
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvHandlerWidget::PrepareToClose(void)
{
   bool q_Return = true;

   if (this->mpc_SetupWidget != NULL)
   {
      //Save necessary for user settings
      this->mpc_SetupWidget->Save();
      this->mpc_SetupWidget->PrepareToClose();
   }

   if (this->mpc_DashboardsWidget != NULL)
   {
      //Save necessary for user settings
      this->mpc_DashboardsWidget->Save();
      q_Return = this->mpc_DashboardsWidget->PrepareToClose();
   }

   if (this->mpc_UpdateWidget != NULL)
   {
      q_Return = this->mpc_UpdateWidget->PrepareToClose();
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle for global key press event

   \param[in,out] opc_Event Event identification and information

   \return
   True  Handled
   False Not handled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvHandlerWidget::GlobalUserKeyPress(QKeyEvent * const opc_Event)
{
   bool q_Retval = false;

   if (opc_Event->key() == static_cast<int32_t>(Qt::Key_Escape))
   {
      if (this->mpc_SetupWidget != NULL)
      {
         if (this->mpc_SetupWidget->GetEditMode() == true)
         {
            this->mpc_SetupWidget->Abort();
            //Accept to properly signal a handled escape
            opc_Event->accept();
            q_Retval = true;
         }
      }
      if (this->mpc_DashboardsWidget != NULL)
      {
         if (this->mpc_DashboardsWidget->GetEditMode() == true)
         {
            this->Save();
            //Accept to properly signal a handled escape
            opc_Event->accept();
            q_Retval = true;
         }
      }
   }
   else if ((opc_Event->key() == static_cast<int32_t>(Qt::Key_S)) &&
            (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true))
   {
      this->Save();
   }
   else if (opc_Event->key() == static_cast<int32_t>(Qt::Key_F12))
   {
      // Save as is not allowed for service projects
      if (C_PuiSvHandler::h_GetInstance()->GetServiceModeActive() == false)
      {
         // open project save
         this->SaveAs();
      }
   }
   else if (opc_Event->key() == static_cast<int32_t>(Qt::Key_F8))
   {
      // open color picker
      this->OpenColorPicker();
   }
   else
   {
      //Not handled
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle generic push button icon press
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvHandlerWidget::OnPushButtonIconPress(void)
{
   if (this->mpc_DashboardsWidget != NULL)
   {
      this->mpc_DashboardsWidget->ToggleDarkMode();
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvHandlerWidget::m_DataChanged(void)
{
   this->mq_DataChanged = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle dashboard count change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvHandlerWidget::m_DashboardCountChanged(void)
{
   Q_EMIT this->SigTriggerUpdateTitle();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward error changed signal

   \param[in] ou32_Index Item index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvHandlerWidget::m_ErrorChanged(const uint32_t ou32_Index)
{
   Q_EMIT this->SigErrorChanged(ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvHandlerWidget::m_EnableConfiguration(const bool oq_State)
{
   Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_DEVICECONFIG, oq_State);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set icon path for dark mode push button

   \param[in] orc_Path Push button icon path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvHandlerWidget::m_SetPushButtonDarkIconSvg(const QString & orc_Path)
{
   if (this->mc_Interaction.isNull() == false)
   {
      this->mc_Interaction->SetPushButtonDarkSvg(orc_Path);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set push button connect SVG icon path

   \param[in] orc_IconPath     Push button connect SVG icon path
   \param[in] oq_ShowAnimation Flag to request show of animation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvHandlerWidget::m_SetConnectPushButtonIcon(const QString & orc_IconPath, const bool oq_DisplayAnimation)
{
   if (this->mc_Interaction.isNull() == false)
   {
      this->mc_Interaction->SetPushButtonConnectSvg(orc_IconPath, oq_DisplayAnimation);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle connect push button press
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvHandlerWidget::m_OnPushButtonConnectPress(void)
{
   if (this->mpc_DashboardsWidget != NULL)
   {
      this->mpc_DashboardsWidget->OnPushButtonConnectPress();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle configuration available state

   \param[in] oq_State requested state for configuration option
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvHandlerWidget::m_SetConfigurationAvailable(const bool oq_State)
{
   const bool q_ServiceModeActive = C_PuiSvHandler::h_GetInstance()->GetServiceModeActive();

   Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, oq_State && (!q_ServiceModeActive));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call specific help pages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvHandlerWidget::CallHelp(void)
{
   //get sub mode
   if (this->ms32_SubMode == ms32_SUBMODE_SYSVIEW_SETUP)
   {
      stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(
         "stw::opensyde_gui::C_SyvSeSetupWidget");
   }
   else if (this->ms32_SubMode == ms32_SUBMODE_SYSVIEW_UPDATE)
   {
      stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(
         "stw::opensyde_gui::C_SyvUpUpdateWidget");
   }
   else if (this->ms32_SubMode == ms32_SUBMODE_SYSVIEW_DASHBOARD)
   {
      stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(
         "stw::opensyde_gui::C_SyvDaDashboardsWidget");
   }
   else
   {
      // Nothing to do
   }
}
