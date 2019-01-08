//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for handling system view

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QApplication>
#include "stwtypes.h"
#include "stwerrors.h"
#include "constants.h"

#include "C_SyvHandlerWidget.h"
#include "ui_C_SyvHandlerWidget.h"

#include "C_GtGetText.h"
#include "C_HeHandler.h"
#include "C_PuiProject.h"
#include "C_PopErrorHandling.h"

#include "constants.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */
const uint32 C_SyvHandlerWidget::mhu32_USER_INPUT_FUNC_APPLY = 0U;
const uint32 C_SyvHandlerWidget::mhu32_USER_INPUT_FUNC_EDIT = 1U;
const uint32 C_SyvHandlerWidget::mhu32_USER_INPUT_FUNC_SETTINGS = 2U;
const uint32 C_SyvHandlerWidget::mhu32_USER_INPUT_FUNC_CONNECT = 3U;
const uint32 C_SyvHandlerWidget::mhu32_USER_INPUT_FUNC_DISCONNECT = 4U;
const uint32 C_SyvHandlerWidget::mhu32_USER_INPUT_FUNC_CONNECTING = 5U;
const uint32 C_SyvHandlerWidget::mhu32_USER_INPUT_FUNC_DEVICECONFIG = 6U;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     11.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

   // Function index 1: mhu32_USER_INPUT_FUNC_EDIT
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Edit");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Edit");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText("Edit Dashboard content.");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 2: mhu32_USER_INPUT_FUNC_SETTINGS
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Dashboard Configuration");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Configure Dashboard");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText("Edit Dashboard transmission settings.");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 3: mhu32_USER_INPUT_FUNC_CONNECT
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Connect");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Connect");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText("Connect to active system nodes. (Go Online)");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 4: mhu32_USER_INPUT_FUNC_DISCONNECT
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Disconnect");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Disconnect");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText("Disconnect from system. (Go Offline)");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 5: mhu32_USER_INPUT_FUNC_CONNECTING
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Connecting ...");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Connecting...");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText("Trying to connect to system.");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 6: mhu32_USER_INPUT_FUNC_DEVICECONFIG
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Device Configuration");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Device Configuration");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText(
      "Scan for devices, assign and configure device interface properties like node id, bitrate, ...");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     11.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvHandlerWidget::~C_SyvHandlerWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_DashboardsWidget and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function for toolbar

   Saves the project.

   \param[in]  ou32_FuncNumber   Number of function

   \created     20.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvHandlerWidget::UserInputFunc(const uint32 ou32_FuncNumber)
{
   //Default
   Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, true);
   Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, true);
   Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_CONNECTING, false);

   switch (ou32_FuncNumber)
   {
   case mhu32_USER_INPUT_FUNC_APPLY:
      //BEFORE save
      if ((this->mpc_DashboardsWidget != NULL) &&
          (this->mpc_DashboardsWidget->GetConnectActive() == true))
      {
         Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, false);
         Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, false);
      }

      this->Save();
      break;
   case mhu32_USER_INPUT_FUNC_EDIT:
      if ((this->mpc_SetupWidget != NULL) ||
          (this->mpc_DashboardsWidget != NULL))
      {
         bool q_Edit = false;
         if (this->mpc_SetupWidget != NULL)
         {
            q_Edit = !this->mpc_SetupWidget->GetEditMode();
            this->mpc_SetupWidget->SetEditMode(q_Edit);
         }
         if (this->mpc_DashboardsWidget != NULL)
         {
            q_Edit = !this->mpc_DashboardsWidget->GetEditMode();
            this->mpc_DashboardsWidget->SetEditMode(q_Edit);
         }
         Q_EMIT this->SigShowBackgroundOfUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, q_Edit);
         if (q_Edit == true)
         {
            QIcon c_IconEdit("://images/IconEditSelected.svg");
            c_IconEdit.addPixmap(QPixmap("://images/IconEditDisabled.svg"), QIcon::Disabled);
            Q_EMIT this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, c_IconEdit);
         }
         else
         {
            QIcon c_IconEdit("://images/IconEditActive.svg");
            c_IconEdit.addPixmap(QPixmap("://images/IconEditDisabled.svg"), QIcon::Disabled);
            Q_EMIT this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, c_IconEdit);
         }
      }
      break;
   case mhu32_USER_INPUT_FUNC_SETTINGS:
      if (this->mpc_DashboardsWidget != NULL)
      {
         this->mpc_DashboardsWidget->OpenSettings();
      }
      break;
   case mhu32_USER_INPUT_FUNC_CONNECT:
      if (this->mpc_DashboardsWidget != NULL)
      {
         //Leave eidt mode
         if (this->mpc_DashboardsWidget->GetEditMode() == true)
         {
            this->mpc_DashboardsWidget->SetEditMode(false);
            Q_EMIT this->SigShowBackgroundOfUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, false);
            QIcon c_IconEdit("://images/IconEditActive.svg");
            c_IconEdit.addPixmap(QPixmap("://src/images/IconEditDisabled.svg"), QIcon::Disabled);
            Q_EMIT this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, c_IconEdit);
         }
         //Deactivate edit & config
         Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, false);
         Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, false);
         Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_CONNECTING, true);
         Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_CONNECT, false);
         //While connecting display updated tool bar
         QApplication::processEvents();
         if (this->mpc_DashboardsWidget->SetConnectActive(true) == C_NO_ERR)
         {
            //Success
            Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_DISCONNECT, true);
         }
         else
         {
            //Failure
            //Reactivate edit & config
            Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, true);
            Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, true);
            Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_DISCONNECT, false);
            Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_CONNECT, true);
         }
         //Hide connecting
         Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_CONNECTING, false);
      }
      break;
   case mhu32_USER_INPUT_FUNC_DISCONNECT:
      if (this->mpc_DashboardsWidget != NULL)
      {
         this->mpc_DashboardsWidget->SetConnectActive(false);
         Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_DISCONNECT, false);
         Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_CONNECT, true);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the actual mode of the system view handler

   Do not call this function internally. Use the signal SigChangeMode to
   inform the entire application about the change.

   \param[in]     os32_SubMode     Actual sub mode
   \param[in]     ou32_Index       The view index
   \param[in]     ou32_Flag        Flag for special functionality

   \created     20.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvHandlerWidget::SetSubMode(const sint32 os32_SubMode, const uint32 ou32_Index, const uint32 ou32_Flag)
{
   //Change cursor
   QApplication::setOverrideCursor(Qt::WaitCursor);

   // set the icons. this can not be done in constructor
   QIcon c_IconApply("://images/IconSave.svg");
   c_IconApply.addPixmap(QPixmap("://images/IconSaveDisabled.svg"), QIcon::Disabled);
   Q_EMIT this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_APPLY, c_IconApply); //QIcon(":images/IconSave.svg"));
   QIcon c_IconEdit("://images/IconEditActive.svg");
   c_IconEdit.addPixmap(QPixmap("://images/IconEditDisabled.svg"), QIcon::Disabled);
   Q_EMIT this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, c_IconEdit); //QIcon(":images/IconEditActive.svg"));
   QIcon c_IconSettings("://images/SettingsIcon.svg");
   c_IconSettings.addPixmap(QPixmap("://images/SettingsIconDisabled.svg"), QIcon::Disabled);
   Q_EMIT this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, c_IconSettings);
   Q_EMIT this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_CONNECT,
                                           QIcon(":/images/system_views/ConnectInactive.svg"));
   Q_EMIT this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_DISCONNECT,
                                           QIcon(":/images/system_views/ConnectActive.svg"));
   Q_EMIT this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_CONNECTING,
                                           QIcon(":/images/system_views/ConnectInactive.svg"));

   // if the submode is setup it can be the initial call or a kind of refresh because
   // of the start screen
   //Flag values of 1 indicate the current widget index might not point to the same view anymore
   if (((os32_SubMode != this->ms32_SubMode) ||
        (ou32_Index != this->mu32_Index)) || (ou32_Flag == 1UL))
   {
      sintn sn_Index;

      // Deactivate all buttons in the first step
      Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, false);
      Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_CONNECT, false);
      Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_DISCONNECT, false);
      Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_CONNECTING, false);
      Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, false);
      Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_DEVICECONFIG, false);

      // delete or remove from the layout the other widgets if necessary
      if (this->mpc_SetupWidget != NULL)
      {
         disconnect(this->mpc_SetupWidget, &C_SyvSeSetupWidget::SigChanged, this, &C_SyvHandlerWidget::m_DataChanged);
         disconnect(this->mpc_SetupWidget, &C_SyvSeSetupWidget::SigCheckViewError, this,
                    &C_SyvHandlerWidget::m_ErrorChanged);
         disconnect(this->mpc_SetupWidget, &C_SyvSeSetupWidget::SigEditModeClosed,
                    this, &C_SyvHandlerWidget::m_ActivateEditButton);
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
         disconnect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigChanged, this,
                    &C_SyvHandlerWidget::m_DataChanged);
         disconnect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigEditModeClosed,
                    this, &C_SyvHandlerWidget::m_ActivateEditButton);
         disconnect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigSetPushButtonIcon,
                    this, &C_SyvHandlerWidget::SigSetPushButtonIcon);
         disconnect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigConfigError,
                    this, &C_SyvHandlerWidget::m_SetConfigError);
         disconnect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigBlockDragAndDrop,
                    this, &C_SyvHandlerWidget::SigBlockDragAndDrop);
         this->mpc_Ui->pc_VerticalLayout->removeWidget(this->mpc_DashboardsWidget);
         delete this->mpc_DashboardsWidget;
         this->mpc_DashboardsWidget = NULL;
      }

      // create the new widget
      if (os32_SubMode == ms32_SUBMODE_SYSVIEW_SETUP)
      {
         //Handle buttons
         Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_APPLY, true);
         Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, true);
         Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_DEVICECONFIG, true);

         Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, true);
         QIcon c_IconDevice("://images/system_views/Device.svg");
         c_IconDevice.addPixmap(QPixmap("://images/system_views/DeviceDisabled.svg"), QIcon::Disabled);
         Q_EMIT this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_DEVICECONFIG, c_IconDevice);
         Q_EMIT this->SigShowBackgroundOfUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, false);
         Q_EMIT this->SigSetPushButtonIcon("");

         this->mpc_SetupWidget = new C_SyvSeSetupWidget(ou32_Index, this);
         if (this->mpc_SetupWidget != NULL)
         {
            connect(this->mpc_SetupWidget, &C_SyvSeSetupWidget::SigChanged, this, &C_SyvHandlerWidget::m_DataChanged);
            connect(this->mpc_SetupWidget, &C_SyvSeSetupWidget::SigCheckViewError, this,
                    &C_SyvHandlerWidget::m_ErrorChanged);
            connect(this->mpc_SetupWidget, &C_SyvSeSetupWidget::SigEditModeClosed,
                    this, &C_SyvHandlerWidget::m_ActivateEditButton);
            connect(this->mpc_SetupWidget, &C_SyvSeSetupWidget::SigEnableConfiguration,
                    this, &C_SyvHandlerWidget::m_EnableConfiguration);
            this->mpc_Ui->pc_VerticalLayout->addWidget(this->mpc_SetupWidget);

            sn_Index = this->mpc_Ui->pc_VerticalLayout->indexOf(this->mpc_SetupWidget);
            this->mpc_Ui->pc_VerticalLayout->setStretch(sn_Index, 1);

            this->mpc_SetupWidget->LoadScene();
            this->mpc_SetupWidget->show();

            this->m_ActivateEditButton();
         }
      }
      if (os32_SubMode == ms32_SUBMODE_SYSVIEW_UPDATE)
      {
         //Handle buttons
         Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_APPLY, true);

         Q_EMIT this->SigShowBackgroundOfUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, false);
         Q_EMIT this->SigSetPushButtonIcon("");

         this->mpc_UpdateWidget = new C_SyvUpUpdateWidget(ou32_Index, this->parentWidget(), this);
         if (this->mpc_UpdateWidget != NULL)
         {
            connect(this->mpc_UpdateWidget, &C_SyvUpUpdateWidget::SigChanged, this, &C_SyvHandlerWidget::m_DataChanged);
            connect(this->mpc_UpdateWidget, &C_SyvUpUpdateWidget::SigBlockDragAndDrop, this,
                    &C_SyvHandlerWidget::SigBlockDragAndDrop);
            this->mpc_Ui->pc_VerticalLayout->addWidget(this->mpc_UpdateWidget);

            sn_Index = this->mpc_Ui->pc_VerticalLayout->indexOf(this->mpc_UpdateWidget);
            this->mpc_Ui->pc_VerticalLayout->setStretch(sn_Index, 1);

            this->mpc_UpdateWidget->LoadScene();
            this->mpc_UpdateWidget->show();
         }
      }
      if (os32_SubMode == ms32_SUBMODE_SYSVIEW_DASHBOARD)
      {
         //Handle buttons
         Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_APPLY, true);
         Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, true);
         Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, true);
         Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_CONNECT, true);

         Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, true);
         Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, true);

         Q_EMIT this->SigShowBackgroundOfUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, false);

         this->mpc_DashboardsWidget = new C_SyvDaDashboardsWidget(ou32_Index, this->parentWidget(), this);
         if (this->mpc_DashboardsWidget != NULL)
         {
            connect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigChanged, this,
                    &C_SyvHandlerWidget::m_DataChanged);
            connect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigEditModeClosed,
                    this, &C_SyvHandlerWidget::m_ActivateEditButton);
            connect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigSetPushButtonIcon,
                    this, &C_SyvHandlerWidget::SigSetPushButtonIcon);
            connect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigConfigError,
                    this, &C_SyvHandlerWidget::m_SetConfigError);
            connect(this->mpc_DashboardsWidget, &C_SyvDaDashboardsWidget::SigBlockDragAndDrop,
                    this, &C_SyvHandlerWidget::SigBlockDragAndDrop);
            this->mpc_Ui->pc_VerticalLayout->addWidget(this->mpc_DashboardsWidget);

            sn_Index = this->mpc_Ui->pc_VerticalLayout->indexOf(this->mpc_DashboardsWidget);
            this->mpc_Ui->pc_VerticalLayout->setStretch(sn_Index, 1);

            this->mpc_DashboardsWidget->LoadDarkMode();
            //Do error check AFTER connections are up
            this->mpc_DashboardsWidget->CheckError();
            this->mpc_DashboardsWidget->show();

            this->m_ActivateEditButton();
         }
      }

      // update of the index values
      this->ms32_SubMode = os32_SubMode;
      this->mu32_Index = ou32_Index;
   }
   //Change cursor
   QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save data

   \created     28.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvHandlerWidget::Save(void)
{
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

   //Check if valid save
   if (C_PuiProject::h_GetInstance()->IsEmptyProject() == true)
   {
      // open project save
      this->SaveAs();
   }
   else
   {
      //Save to file
      C_PopErrorHandling::mh_ProjectSaveErr(C_PuiProject::h_GetInstance()->Save(), this);

      this->mq_DataChanged = false;
   }
   QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function to prepare closing the widget

   Prepare each widget for close and do specific actions to prepare the widget for close action

   \return
   true     Preparation successful. Can be closed.
   false    Preparation not finished. Can not be closed.

   \created     22.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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
   }

   if (this->mpc_UpdateWidget != NULL)
   {
      q_Return = this->mpc_UpdateWidget->PrepareToClose();
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle for global key press event

   \param[in,out] opc_Event Event identification and information

   \return
   True  Handled
   False Not handled

   \created     27.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvHandlerWidget::GlobalUserKeyPress(QKeyEvent * const opc_Event)
{
   bool q_Retval = false;

   if (opc_Event->key() == static_cast<sintn>(Qt::Key_Escape))
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
   else if ((opc_Event->key() == static_cast<sintn>(Qt::Key_S)) &&
            (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true))
   {
      this->Save();
   }
   else if (opc_Event->key() == static_cast<sintn>(Qt::Key_F12))
   {
      // open project save
      this->SaveAs();
   }
   else
   {
      //Not handled
   }

   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle generic push button icon press

   \created     02.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvHandlerWidget::OnPushButtonIconPress(void)
{
   if (this->mpc_DashboardsWidget != NULL)
   {
      this->mpc_DashboardsWidget->ToggleDarkMode();
   }
}

//-----------------------------------------------------------------------------
void C_SyvHandlerWidget::m_ActivateEditButton(void)
{
   Q_EMIT this->SigShowBackgroundOfUserInputFunc(mhu32_USER_INPUT_FUNC_EDIT, false);
}

//-----------------------------------------------------------------------------
void C_SyvHandlerWidget::m_DataChanged(void)
{
   this->mq_DataChanged = true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Forward error changed signal

   \param[in] ou32_Index Item index

   \created     11.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvHandlerWidget::m_ErrorChanged(const uint32 ou32_Index)
{
   Q_EMIT this->SigErrorChanged(ou32_Index);
}

//-----------------------------------------------------------------------------
void C_SyvHandlerWidget::m_EnableConfiguration(const bool oq_State)
{
   Q_EMIT this->SigEnableUserInputFunc(mhu32_USER_INPUT_FUNC_DEVICECONFIG, oq_State);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Change config state

   \created     17.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvHandlerWidget::m_SetConfigError(const bool oq_Active)
{
   if (oq_Active == true)
   {
      Q_EMIT this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, QIcon("://images/SettingsIconError.svg"));
   }
   else
   {
      QIcon c_IconSettings("://images/SettingsIcon.svg");
      c_IconSettings.addPixmap(QPixmap("://images/SettingsIconDisabled.svg"), QIcon::Disabled);
      Q_EMIT this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_SETTINGS, c_IconSettings);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call specific help pages

   \created     30.10.2018  STW/S.Singer
*/
//-----------------------------------------------------------------------------
void C_SyvHandlerWidget::CallHelp(void)
{
   //get sub mode
   if (this->ms32_SubMode == ms32_SUBMODE_SYSVIEW_SETUP)
   {
      stw_opensyde_gui_logic::C_HeHandler::GetInstance().CallSpecificHelpPage(
         "stw_opensyde_gui::C_SyvSeSetupWidget");
   }
   else if (this->ms32_SubMode == ms32_SUBMODE_SYSVIEW_UPDATE)
   {
      stw_opensyde_gui_logic::C_HeHandler::GetInstance().CallSpecificHelpPage(
         "stw_opensyde_gui::C_SyvUpUpdateWidget");
   }
   else if (this->ms32_SubMode == ms32_SUBMODE_SYSVIEW_DASHBOARD)
   {
      stw_opensyde_gui_logic::C_HeHandler::GetInstance().CallSpecificHelpPage(
         "stw_opensyde_gui::C_SyvDaDashboardsWidget");
   }
}
