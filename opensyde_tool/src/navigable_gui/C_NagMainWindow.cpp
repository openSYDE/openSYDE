//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main window with all sub widgets

   The main window handles all sub widgets, their connects and the switching of
   the use cases.
   This widget is designed in a ui file.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <iostream>

#include <QApplication>
#include <QFileInfo>
#include <QDesktopWidget>
#include <QMimeData>

#include "C_NagMainWindow.h"
#include "ui_C_NagMainWindow.h"

#include "constants.h"

#include "TGLUtils.h"
#include "C_OSCUtils.h"
#include "C_OSCLoggingHandler.h"
#include "stwerrors.h"
#include "C_SyvUtil.h"
#include "C_HeHandler.h"
#include "C_SdTopologyWidget.h"
#include "C_SdNdeDpEditWidget.h"
#include "C_SdHandlerWidget.h"
#include "C_SyvHandlerWidget.h"
#include "C_UsHandler.h"
#include "C_OgeOverlay.h"
#include "C_SdNdeNodeEditWidget.h"
#include "C_PuiProject.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_GtGetText.h"
#include "C_OSCNode.h"
#include "C_OSCSystemBus.h"
#include "C_PopErrorHandling.h"
#include "C_OSCSystemDefinition.h"
#include "C_Uti.h"
#include "C_NagToolTip.h"
#include "C_PopUtil.h"
#include "C_TblTreDataElementModel.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
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
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagMainWindow::C_NagMainWindow(void) :
   QMainWindow(NULL),
   mpc_Ui(new Ui::C_NagMainWindow),
   mpc_ActiveWidget(NULL),
   mq_InitialProjectLoaded(false),
   mq_BlockDragAndDrop(false),
   mq_StartView(true),
   ms32_Mode(0),
   ms32_SubMode(-1),
   mu32_Index(0U),
   ms32_SdSubMode(ms32_SUBMODE_SYSDEF_TOPOLOGY),
   mu32_SdIndex(0U),
   mu32_SdFlag(0U),
   ms32_SvSubMode(ms32_SUBMODE_SYSVIEW_SETUP),
   mu32_SvIndex(0U),
   mu32_SvFlag(0U)
{
   // Performance measurement
   // Switch on and off because using flag from user settings would load them, hence this loading time would not count
   C_OSCLoggingHandler::h_SetMeasurePerformanceActive(true);
   const uint16 u16_TimerId = osc_write_log_performance_start();
   C_OSCLoggingHandler::h_SetMeasurePerformanceActive(false);

   mpc_Ui->setupUi(this);

   //Configure collapse-ability
   mpc_Ui->pc_Splitter->setCollapsible(0, true);
   mpc_Ui->pc_Splitter->setCollapsible(1, false);
   // if there is more space stretch right widget (i.e. index 1)
   this->mpc_Ui->pc_Splitter->setStretchFactor(0, 0);
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 1);

   // load devices so they are known to UI
   stw_opensyde_core::C_OSCSystemDefinition::hc_Devices.LoadFromFile(
      C_Uti::h_GetAbsolutePathFromExe("../devices/devices.ini").toStdString().c_str(), false);

   stw_opensyde_core::C_OSCSystemDefinition::hc_Devices.LoadFromFile(
      C_Uti::h_GetAbsolutePathFromExe("../devices/user_devices.ini").toStdString().c_str(), true);

   this->mpc_MainWidget = new C_NagMainWidget(this->mpc_Ui->pc_workAreaWidget);
   this->mpc_UseCaseWidget = new C_NagUseCaseViewWidget(this->mpc_Ui->pc_workAreaWidget);

   this->m_ShowStartView();

   // connect to C_NagNaviBarWidget
   connect(this->mpc_Ui->pc_NaviBar, &C_NagNaviBarWidget::SigStartViewClicked, this, &C_NagMainWindow::m_ShowStartView);
   connect(this->mpc_Ui->pc_NaviBar, &C_NagNaviBarWidget::SigChangeMode, this,
           &C_NagMainWindow::m_ChangeMode);

   // connect to C_NagMainWidget
   connect(this->mpc_MainWidget, &C_NagMainWidget::SigChangeMode, this, &C_NagMainWindow::m_ChangeMode);

   // connect to C_NagToolBarWidget
   connect(this->mpc_Ui->pc_TopToolBar, &C_NagToolBarWidget::SigChangeMode, this, &C_NagMainWindow::m_ChangeMode);
   connect(this->mpc_Ui->pc_TopToolBar, &C_NagToolBarWidget::SigOpenDetail, this, &C_NagMainWindow::m_OpenDetail);

   // connect to C_SyvManager
   connect(&this->mc_SystemViewManager, &C_SyvManager::SigChangeMode, this, &C_NagMainWindow::m_ChangeMode);
   connect(&this->mc_SystemViewManager, &C_SyvManager::SigReloadNaviBarSystemViewNames, this->mpc_Ui->pc_NaviBar,
           &C_NagNaviBarWidget::UpdateNames);
   connect(&this->mc_SystemViewManager, &C_SyvManager::SigReloadNaviBarSystemViewContent, this->mpc_Ui->pc_NaviBar,
           &C_NagNaviBarWidget::InitSysView);
   connect(this->mpc_Ui->pc_NaviBar, &C_NagNaviBarWidget::SigChangeUseCase, this, &C_NagMainWindow::m_ChangeUseCase);
   connect(this->mpc_Ui->pc_NaviBar, &C_NagNaviBarWidget::SigSysDefRequest, this,
           &C_NagMainWindow::m_HandleSysDefRequest);
   connect(this->mpc_Ui->pc_NaviBar, &C_NagNaviBarWidget::SigCheckSysViews, this->mpc_Ui->pc_NaviBar,
           &C_NagNaviBarWidget::UpdateAllViewsIcons);
   connect(this->mpc_Ui->pc_NaviBar, &C_NagNaviBarWidget::SigRenameView, this, &C_NagMainWindow::m_HandleRenameView);
   connect(this->mpc_Ui->pc_NaviBar, &C_NagNaviBarWidget::SigAddViewClicked, this,
           &C_NagMainWindow::m_HandleAddViewRequest);
   connect(this->mpc_Ui->pc_NaviBar, &C_NagNaviBarWidget::SigMoveView, this, &C_NagMainWindow::m_HandleMoveViewRequest);
   connect(this->mpc_Ui->pc_NaviBar, &C_NagNaviBarWidget::SigDeleteView, this,
           &C_NagMainWindow::m_HandleDeleteSysViewRequest);
   connect(this->mpc_Ui->pc_NaviBar, &C_NagNaviBarWidget::SigDuplicateView, this,
           &C_NagMainWindow::m_HandleDuplicateSysViewRequest);

   // connect to main widget
   connect(this->mpc_MainWidget, &C_NagMainWidget::SigBeforeOtherProjectLoad, this,
           &C_NagMainWindow::m_CloseAndPrepareProjectLoad);
   connect(this->mpc_MainWidget, &C_NagMainWidget::SigOtherProjectLoaded, this,
           &C_NagMainWindow::m_ProjectLoaded);

   //Register MainWindow for overlay
   C_OgeOverlay::h_SetTopWidget(this);
   this->mpc_Ui->pc_TopToolBar->SetTopWidget(this);

   // adapt start position
   {
      QPoint c_Position = C_UsHandler::h_GetInstance()->GetScreenPos();
      QSize c_Size = C_UsHandler::h_GetInstance()->GetAppSize();
      C_OgeWiUtil::h_CheckAndFixDialogPositionAndSize(c_Position, c_Size, QSize(1000, 700), true);
      this->setGeometry(c_Position.x(), c_Position.y(), c_Size.width(), c_Size.height());
   }

   // Performance time measurement
   C_OSCLoggingHandler::h_SetMeasurePerformanceActive(C_UsHandler::h_GetInstance()->GetPerformanceActive());

   //Drag & drop of *.syde files
   this->setAcceptDrops(true);

   //Window title
   this->setWindowTitle("openSYDE");
   connect(this->mpc_MainWidget, &C_NagMainWidget::SigNewApplicationName, this, &C_NagMainWindow::setWindowTitle);

   //Window icon
   C_OgeWiUtil::h_SetWindowIcon(this);

   //Time measurement log
   osc_write_log_performance_stop(u16_TimerId, "Main");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagMainWindow::~C_NagMainWindow()
{
   // save position
   this->m_SaveScreenProperties();
   // store (only if useful!)
   if ((this->mpc_MainWidget == NULL) || (this->mpc_MainWidget->isVisible() == false))
   {
      this->mpc_Ui->pc_Splitter->StoreUserSettings();
      this->mpc_Ui->pc_NaviBar->SaveUserSettings();
   }

   // deactivate use case view widget
   this->m_RemoveUseCaseWidget();

   delete mpc_Ui;

   //Clean up singleton
   stw_opensyde_gui_logic::C_PuiProject::h_Destroy();
   C_UsHandler::h_Destroy();
   C_TblTreDataElementModel::h_CleanUp();

   //lint -e{1579}  no memory leak because of the parent all elements and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Return to start view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_ShowStartView()
{
   bool q_Continue;

   if (this->mpc_ActiveWidget != NULL)
   {
      // a change of mode or submode will happen
      // is a change possible at the moment?
      q_Continue = this->mpc_ActiveWidget->PrepareToClose();
   }
   else
   {
      q_Continue = true;
   }
   if (q_Continue)
   {
      //Store user settings (only if useful!)
      if ((this->mpc_MainWidget == NULL) || (this->mpc_MainWidget->isVisible() == false))
      {
         this->mpc_Ui->pc_Splitter->StoreUserSettings();
         this->mpc_Ui->pc_NaviBar->SaveUserSettings();
      }

      // deactivate use case view widget
      this->mpc_Ui->pc_NaviBar->setVisible(false);
      this->mpc_Ui->pc_TopToolBar->HideAllButtonsAndStoreState();
      this->mpc_Ui->pc_TopToolBar->SetDarkTheme();
      this->mpc_UseCaseWidget->setVisible(false);

      // activate main widget
      this->mpc_Ui->pc_workAreaWidget->layout()->addWidget(this->mpc_MainWidget);
      this->mpc_MainWidget->setVisible(true);

      this->mq_StartView = true;

      // update recent projects
      this->mpc_MainWidget->UpdateRecentProjects();
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_ChangeUseCase(const sint32 os32_Mode, const sint32 os32_SubIndex, const bool oq_ChangeUseCase)
{
   bool q_Worked;
   QString c_Name;
   QString c_SubItemName;

   //Get appropriate default heading
   C_NagMainWindow::h_GetHeadingNames(os32_Mode, os32_SubIndex, 0UL, c_Name, c_SubItemName);

   //Change use-case
   q_Worked = m_ChangeMode(os32_Mode, os32_SubIndex, 0U, c_Name, c_SubItemName, 0U, oq_ChangeUseCase);

   if (q_Worked == false)
   {
      this->mpc_Ui->pc_NaviBar->ResetUseCaseAfterChangeFailure(this->ms32_Mode);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle system definition request

   \param[in]  os32_Mode      Mode
   \param[in]  os32_SubMode   Sub mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_HandleSysDefRequest(const sint32 os32_Mode, const sint32 os32_SubMode)
{
   const bool q_ChangeUseCase = (this->ms32_Mode != ms32_MODE_SYSDEF);

   m_ChangeUseCase(os32_Mode, os32_SubMode, q_ChangeUseCase);
}

//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_OpenDetail(const sint32 os32_Index, const sint32 os32_SubIndex, const sint32 os32_SubSubIndex,
                                   const sint32 os32_SubSubSubIndex, const sint32 os32_Flag)
{
   if (this->mpc_ActiveWidget != NULL)
   {
      this->mpc_ActiveWidget->OpenDetail(os32_Index, os32_SubIndex, os32_SubSubIndex, os32_SubSubSubIndex, os32_Flag);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Loading initial project

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::showEvent(QShowEvent * const opc_Event)
{
   if (C_UsHandler::h_GetInstance()->GetAppMaximized() == true)
   {
      // Call here maximized to avoid a further show event in the constructor
      this->showMaximized();
   }

   QMainWindow::showEvent(opc_Event);

   if (this->mq_InitialProjectLoaded == false)
   {
      this->mq_InitialProjectLoaded = true;

      // Load initial project (must be called always at start to have a valid project!)
      // Call it with a delay to make sure the UI is finished. The maximized state can cause a not reliable
      // size information of the QWidgets or QMainWindow when the first events will caused (Polished, Paint if called,
      // Show and WindowStateChange if called)
      QTimer::singleShot(50, this, &C_NagMainWindow::m_LoadInitialProject);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Trigger help key press handling

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_ToolTipHidden1 = false;
   bool q_ToolTipHidden2 = false;

   if (stw_opensyde_gui_logic::C_HeHandler::h_CheckHelpKey(opc_KeyEvent) == true)
   {
      if (this->mq_StartView == false)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SdHandlerWidget * pc_Handler = dynamic_cast<C_SdHandlerWidget *>(this->mpc_ActiveWidget);

         //SD is active
         if (pc_Handler != NULL)
         {
            pc_Handler->CallHelp();
         }

         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SyvHandlerWidget * pc_HandlerSv = dynamic_cast<C_SyvHandlerWidget *>(this->mpc_ActiveWidget);

         //SV is active
         if (pc_HandlerSv != NULL)
         {
            pc_HandlerSv->CallHelp();
         }
      }
      else
      {
         stw_opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(
            this->mpc_MainWidget->metaObject()->className());
      }
   }

   //Give widget the option to handle global key event
   if (this->mpc_ActiveWidget != NULL)
   {
      q_ToolTipHidden1 = this->mpc_ActiveWidget->GlobalUserKeyPress(opc_KeyEvent);
   }
   else
   {
      // Save on Ctrl+S
      if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_S)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true))
      {
         //Check if valid save
         if (C_PuiProject::h_GetInstance()->IsEmptyProject() == true)
         {
            // open save as dialog
            this->mpc_MainWidget->OnSaveProjAs();
         }
         else
         {
            C_PopErrorHandling::mh_ProjectSaveErr(C_PuiProject::h_GetInstance()->Save(), this);
            this->mpc_MainWidget->UpdateRecentProjects();
         }
      }
      // Save as on F12
      else if (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_F12))
      {
         // open save as dialog
         this->mpc_MainWidget->OnSaveProjAs();
      }
      // Open color picker
      else if (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_F8))
      {
         // open color picker dialog
         this->mpc_MainWidget->OpenColorPicker();
      }
      else
      {
         // nothing to do
      }
   }
   //Trigger tool tip hide if necessary
   if (opc_KeyEvent->key() == Qt::Key_Escape)
   {
      q_ToolTipHidden2 = C_NagToolTip::h_HideAll();
   }
   QMainWindow::keyPressEvent(opc_KeyEvent);
   if ((((opc_KeyEvent->key() == Qt::Key_Escape) && (opc_KeyEvent->isAccepted() == false)) &&
        (q_ToolTipHidden1 == false)) && (q_ToolTipHidden2 == false))
   {
      switch (this->ms32_Mode)
      {
      case ms32_MODE_SYSDEF:
         switch (this->ms32_SdSubMode)
         {
         case ms32_SUBMODE_SYSDEF_NODEEDIT:
            //One higher
            m_ChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_TOPOLOGY, 0, C_GtGetText::h_GetText("NETWORK TOPOLOGY"));
            break;
         case ms32_SUBMODE_SYSDEF_BUSEDIT:
            //One higher
            m_ChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_TOPOLOGY, 0, C_GtGetText::h_GetText("NETWORK TOPOLOGY"));
            break;
         case ms32_SUBMODE_SYSDEF_TOPOLOGY:
            //One higher
            //Check if already at main screen?
            if ((this->mpc_MainWidget == NULL) || (this->mpc_MainWidget->isVisible() == false))
            {
               m_ShowStartView();
            }
            break;
         default:
            //No action available
            break;
         }
         break;
      case ms32_MODE_SYSVIEW:
         //One higher
         //Check if already at main screen?
         if ((this->mpc_MainWidget == NULL) || (this->mpc_MainWidget->isVisible() == false))
         {
            m_ShowStartView();
         }
         break;
      default:
         //No action available
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten close event slot

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::closeEvent(QCloseEvent * const opc_Event)
{
   bool q_CallCloseEvent = false;

   bool q_Continue;

   C_PuiProject::h_HandlePendingEvents();

   if (this->mpc_ActiveWidget != NULL)
   {
      q_Continue = this->mpc_ActiveWidget->PrepareToClose();
   }
   else
   {
      q_Continue = true;
   }

   if (q_Continue == true)
   {
      if (this->m_CheckProjectForChanges() == true)
      {
         const bool q_Close = C_PopUtil::h_AskUserToContinue(this);

         if (q_Close == true)
         {
            q_CallCloseEvent = true;
         }
         else
         {
            opc_Event->ignore();
         }
      }
      else
      {
         q_CallCloseEvent = true;
      }
   }
   else
   {
      // The active widget can not be closed at the moment
      opc_Event->ignore();
   }

   if (q_CallCloseEvent == true)
   {
      C_UsHandler::h_GetInstance()->SetProjLastScreenMode(this->ms32_SdSubMode, this->mu32_SdIndex, this->mu32_SdFlag,
                                                          this->ms32_SvSubMode, this->mu32_SvIndex, this->mu32_SvFlag);
      C_UsHandler::h_GetInstance()->SetProjLastMode(this->ms32_Mode);
      QMainWindow::closeEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag enter event slot

   Here: Accept external *.syde file

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::dragEnterEvent(QDragEnterEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   QMainWindow::dragEnterEvent(opc_Event);
   if ((this->mq_BlockDragAndDrop == false) &&
       (mh_CheckMime(pc_MimeData) == true))
   {
      opc_Event->accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag move event slot

   Here: Accept external *.syde file

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::dragMoveEvent(QDragMoveEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   QMainWindow::dragMoveEvent(opc_Event);
   if ((this->mq_BlockDragAndDrop == false) &&
       (mh_CheckMime(pc_MimeData) == true))
   {
      opc_Event->accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drop event slot

   Here: Handle dropped *.syde file

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::dropEvent(QDropEvent * const opc_Event)
{
   if (C_PopUtil::h_AskUserToContinue(this) == true)
   {
      bool q_CallCloseEvent = true;
      //Change view
      this->m_ShowStartView();

      if (this->mpc_ActiveWidget != NULL)
      {
         q_CallCloseEvent = this->mpc_ActiveWidget->PrepareToClose();

         if (q_CallCloseEvent == true)
         {
            this->m_RemoveUseCaseWidget();
         }
      }

      if (q_CallCloseEvent == true)
      {
         QString c_FilePath;
         const QMimeData * const pc_MimeData = opc_Event->mimeData();

         if ((this->mq_BlockDragAndDrop == false) &&
             (mh_CheckMime(pc_MimeData, &c_FilePath) == true))
         {
            // Check if path is a valid path with no irregular characters
            if (C_OSCUtils::h_CheckValidFilePath(c_FilePath.toStdString().c_str()) == false)
            {
               C_OgeWiUtil::h_ShowPathInvalidError(this, c_FilePath);
            }
            else
            {
               this->mpc_MainWidget->LoadProject(c_FilePath);
            }
         }
      }
   }
   QMainWindow::dropEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Perform initial project load.

   Instantiate C_PuiProject, perform initial project load, handle errors and update GUI.

   Do not try to load other recent projects, just open an empty project.
   (If the last recent project is invalid, all others might be invalid too, e.g. because of invalid
   devices.ini. In this case the user would be forced to close n message boxes if there are n recent projects
   in user settings - very annoying).
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_LoadInitialProject(void)
{
   uint16 u16_Version;
   QString c_LoadedProject;

   // Initial load (command line or first recent project)
   const sint32 s32_Error = C_PuiProject::h_GetInstance()->LoadInitialProject(&u16_Version, c_LoadedProject);

   C_PopErrorHandling::mh_ProjectLoadErr(s32_Error, c_LoadedProject, this->mpc_MainWidget, u16_Version);

   // Handle use case switch
   this->m_ProjectLoaded(C_PuiProject::h_GetInstance()->GetSwitchUseCaseFlag());

   // Initialize dynamic text of main widget and window title
   this->mpc_MainWidget->UpdateRecentProjects();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Preparation for showing a specific widget in the 'work area'

   \param[in]  oq_DeleteActualWidget   Flag for deleting the previous widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_PrepareForSpecificWidget(const bool oq_DeleteActualWidget)
{
   QRect c_Rect;

   // save actual size
   c_Rect = this->geometry();

   // deactivate main widget
   this->mpc_Ui->pc_workAreaWidget->layout()->removeWidget(this->mpc_MainWidget);
   this->mpc_MainWidget->setVisible(false);

   // delete old widget
   if (oq_DeleteActualWidget == true)
   {
      this->m_RemoveUseCaseWidget();
   }
   else
   {
      // same widget, restore the button configuration
      this->mpc_Ui->pc_TopToolBar->RestoreAllButtonsVisibility();
   }

   // activate the use case view widget and the navigation bar
   this->mpc_Ui->pc_workAreaWidget->layout()->addWidget(this->mpc_UseCaseWidget);
   this->mpc_Ui->pc_NaviBar->setVisible(true);
   this->mpc_Ui->pc_TopToolBar->SetLightTheme();
   this->mpc_UseCaseWidget->setVisible(true);

   // set old width
   this->setGeometry(c_Rect);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Showing a specific widget in the 'work area'

   \param[in]  os32_Mode            Mode
   \param[in]  os32_SubMode         Sub mode
   \param[in]  oc_ItemName          Item name
   \param[in]  orc_SubSubModeName   Sub sub mode name
   \param[in]  ou32_Index           Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_SetNewSpecificWidget(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode,
                                             QString oc_ItemName, const QString & orc_SubSubModeName,
                                             const uint32 ou32_Index)
{
   connect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigChangeMode,
           this, &C_NagMainWindow::m_ChangeMode);
   connect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigTriggerUpdateTitle,
           this, &C_NagMainWindow::m_UpdateTitle);
   connect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigErrorChanged,
           this->mpc_Ui->pc_NaviBar, &C_NagNaviBarWidget::UpdateViewIcons);
   connect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigDataChanged,
           this->mpc_Ui->pc_NaviBar, &C_NagNaviBarWidget::MarkModeForDataChanged);
   connect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigStartViewClicked,
           this, &C_NagMainWindow::m_ShowStartView);
   connect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigSetInteractionWidget,
           this, &C_NagMainWindow::m_SetInteractionWidget);
   connect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigSearch, this->mpc_Ui->pc_TopToolBar,
           &C_NagToolBarWidget::Search);
   connect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigNewUserSettings, this->mpc_MainWidget,
           &C_NagMainWidget::UpdateRecentProjects);
   connect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigBlockDragAndDrop,
           this, &C_NagMainWindow::m_BlockDragAndDrop);

   // configure Navigationbar
   this->mpc_Ui->pc_NaviBar->SetMode(os32_Mode, os32_SubMode, ou32_Index);

   // show the new widget
   this->mpc_UseCaseWidget->SetUseCaseWidget(this->mpc_ActiveWidget, os32_Mode, os32_SubMode, oc_ItemName,
                                             orc_SubSubModeName, os32_Mode == ms32_MODE_SYSVIEW);

   // configure the buttons of the upper toolbar
   this->mpc_Ui->pc_TopToolBar->ConfigureButtons(this->mpc_ActiveWidget);
}

//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_RemoveUseCaseWidget(void)
{
   this->mpc_Ui->pc_TopToolBar->ResetButtons();
   this->mpc_UseCaseWidget->RemoveUseCaseWidget();
   if (this->mpc_ActiveWidget != NULL)
   {
      disconnect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigChangeMode,
                 this, &C_NagMainWindow::m_ChangeMode);
      disconnect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigTriggerUpdateTitle,
                 this, &C_NagMainWindow::m_UpdateTitle);
      disconnect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigErrorChanged,
                 this->mpc_Ui->pc_NaviBar, &C_NagNaviBarWidget::UpdateViewIcons);
      disconnect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigDataChanged,
                 this->mpc_Ui->pc_NaviBar, &C_NagNaviBarWidget::MarkModeForDataChanged);
      disconnect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigStartViewClicked,
                 this, &C_NagMainWindow::m_ShowStartView);
      disconnect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigSetInteractionWidget,
                 this, &C_NagMainWindow::m_SetInteractionWidget);
      disconnect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigSearch, this->mpc_Ui->pc_TopToolBar,
                 &C_NagToolBarWidget::Search);
      disconnect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigNewUserSettings, this->mpc_MainWidget,
                 &C_NagMainWidget::UpdateRecentProjects);
      disconnect(this->mpc_ActiveWidget, &C_NagUseCaseWidget::SigBlockDragAndDrop,
                 this, &C_NagMainWindow::m_BlockDragAndDrop);
      delete this->mpc_ActiveWidget;
      this->mpc_ActiveWidget = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the parameter if a switch from main widget to system definition or system view was triggered.

   The last used parameter for the use case will be used.

   \param[in]      os32_Mode           Next chosen use case
   \param[in,out]  ors32_SubMode       Next sub mode
   \param[in,out]  oru32_Index         Next index
   \param[in,out]  orc_Name            Name of next screen
   \param[in,out]  orc_SubItemName     Selected sub sub mode name
   \param[in,out]  oru32_Flag          Flag value for next screen
   \param[in]      oq_ChangeUseCase    Flag to signal use case change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_AdaptParameter(const sint32 os32_Mode, sint32 & ors32_SubMode, uint32 & oru32_Index,
                                       QString & orc_Name, QString & orc_SubItemName, uint32 & oru32_Flag,
                                       const bool oq_ChangeUseCase)
{
   if ((this->mq_StartView == true) || (oq_ChangeUseCase == true))
   {
      // The name of must be adapted dynamically. It could be changed.
      if (os32_Mode == ms32_MODE_SYSDEF)
      {
         ors32_SubMode = this->ms32_SdSubMode;
         oru32_Index = this->mu32_SdIndex;
         oru32_Flag = this->mu32_SdFlag;
      }
      else if (os32_Mode == ms32_MODE_SYSVIEW)
      {
         ors32_SubMode = this->ms32_SvSubMode;
         oru32_Index = this->mu32_SvIndex;
         oru32_Flag = this->mu32_SvFlag;

         //Handle new views if necessary
         this->mc_SystemViewManager.HandleInitialSystemView();
      }
      else
      {
         // Nothing to do
      }

      //Get names (last as indices need to be up to date)
      C_NagMainWindow::h_GetHeadingNames(os32_Mode, ors32_SubMode, oru32_Index, orc_Name, orc_SubItemName);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get System Definition display name

   \param[in]      os32_Mode        Mode
   \param[out]     ors32_SubMode    Sub mode
   \param[in]      ou32_Index       Index
   \param[in,out]  orc_SubMode      Sub mode
   \param[in,out]  orc_SubSubMode   Sub sub mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::h_GetHeadingNames(const sint32 os32_Mode, const sint32 & ors32_SubMode, const uint32 ou32_Index,
                                        QString & orc_SubMode, QString & orc_SubSubMode)
{
   if (os32_Mode == ms32_MODE_SYSDEF)
   {
      if (ors32_SubMode == ms32_SUBMODE_SYSDEF_NODEEDIT)
      {
         const stw_opensyde_core::C_OSCNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_Index);
         if (pc_Node != NULL)
         {
            orc_SubSubMode = pc_Node->c_Properties.c_Name.c_str();
         }
         else
         {
            orc_SubSubMode = "Node";
         }
         orc_SubMode = C_GtGetText::h_GetText("NETWORK TOPOLOGY");
      }
      else if (ors32_SubMode == ms32_SUBMODE_SYSDEF_BUSEDIT)
      {
         const stw_opensyde_core::C_OSCSystemBus * const pc_Bus =
            C_PuiSdHandler::h_GetInstance()->GetOSCBus(ou32_Index);
         if (pc_Bus != NULL)
         {
            orc_SubSubMode = pc_Bus->c_Name.c_str();
         }
         else
         {
            orc_SubSubMode = C_GtGetText::h_GetText("NETWORK TOPOLOGY");
         }
         orc_SubMode = C_GtGetText::h_GetText("NETWORK TOPOLOGY");
      }
      else
      {
         orc_SubMode = C_GtGetText::h_GetText("NETWORK TOPOLOGY");
         orc_SubSubMode = "";
      }
   }
   else if (os32_Mode == ms32_MODE_SYSVIEW)
   {
      C_SyvUtil::h_GetViewDisplayName(ou32_Index, ors32_SubMode, orc_SubMode, orc_SubSubMode);
   }
   else
   {
      // Nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   show node edit

   \param[in]  os32_SubMode   Sub mode
   \param[in]  ou32_Index     Index
   \param[in]  orc_Name       Name
   \param[in]  orc_SubName    Sub name
   \param[in]  ou32_Flag      Flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_ShowSysDefItem(const sint32 os32_SubMode, const uint32 ou32_Index, const QString & orc_Name,
                                       const QString & orc_SubName, const uint32 ou32_Flag)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SdHandlerWidget * pc_Handler = dynamic_cast<C_SdHandlerWidget *>(this->mpc_ActiveWidget);

   // Special case: Restore last screen when coming from main
   if (this->mq_StartView == false)
   {
      // Change of the shown screen. Save the new parameter.
      this->ms32_SdSubMode = os32_SubMode;
      this->mu32_SdIndex = ou32_Index;
      this->mu32_SdFlag = ou32_Flag;
   }

   if (pc_Handler == NULL)
   {
      // Sd Handler was not active yet
      this->m_PrepareForSpecificWidget();
      pc_Handler = new C_SdHandlerWidget(NULL);
      this->mpc_ActiveWidget = pc_Handler;
      this->m_SetNewSpecificWidget(ms32_MODE_SYSDEF, os32_SubMode, orc_Name, orc_SubName, ou32_Index);
   }
   else if (this->mq_StartView == true)
   {
      // Sd Handler was active before the start view
      this->m_PrepareForSpecificWidget(false);
   }
   else
   {
      if (os32_SubMode == ms32_SUBMODE_SYSDEF_TOPOLOGY)
      {
         this->mpc_UseCaseWidget->UpdateUseCaseWidget(os32_SubMode, orc_Name, orc_SubName, false);
      }
      else
      {
         this->mpc_UseCaseWidget->UpdateUseCaseWidget(os32_SubMode, C_GtGetText::h_GetText(
                                                         "NETWORK TOPOLOGY"), orc_Name, false);
      }
   }

   pc_Handler->SetSubMode(os32_SubMode, ou32_Index, ou32_Flag);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show system view item

   \param[in,out]  ors32_SubMode       Target submode (may be changed if necessary for PC reconnection feature)
   \param[in]      ou32_Index          View index
   \param[in]      orc_Name            Display name
   \param[in]      orc_SubSubModeName  Selected sub sub mode name
   \param[in]      ou32_Flag           Currently unused flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_ShowSysViewItem(sint32 & ors32_SubMode, const uint32 ou32_Index, const QString & orc_Name,
                                        const QString & orc_SubSubModeName, const uint32 ou32_Flag)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SyvHandlerWidget * pc_Handler = dynamic_cast<C_SyvHandlerWidget *>(this->mpc_ActiveWidget);

   // Special case: Handle system view PC reconnect
   if (ors32_SubMode != ms32_SUBMODE_SYSVIEW_SETUP)
   {
      bool q_Reconnect;
      tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckViewReconnectNecessary(ou32_Index, q_Reconnect) == C_NO_ERR);
      if (q_Reconnect == true)
      {
         ors32_SubMode = ms32_SUBMODE_SYSVIEW_SETUP;
      }
   }

   // Special case: Restore last screen when coming from main
   if (this->mq_StartView == false)
   {
      // Change of the shown screen. Save the new parameter.
      this->ms32_SvSubMode = ors32_SubMode;
      this->mu32_SvIndex = ou32_Index;
      this->mu32_SvFlag = ou32_Flag;
   }

   if (pc_Handler == NULL)
   {
      // Sd Handler was not active yet
      this->m_PrepareForSpecificWidget();
      pc_Handler = new C_SyvHandlerWidget(NULL);
      this->mpc_ActiveWidget = pc_Handler;
      //Do system view switch specific actions
      C_PuiSvHandler::h_GetInstance()->UpdateSystemDefintionErrors();
      //Do switch
      this->m_SetNewSpecificWidget(ms32_MODE_SYSVIEW, ors32_SubMode, orc_Name, orc_SubSubModeName, ou32_Index);
   }
   else if (this->mq_StartView == true)
   {
      // Sd Handler was active before the start view
      this->m_PrepareForSpecificWidget(false);
   }
   else
   {
      this->mpc_UseCaseWidget->UpdateUseCaseWidget(ors32_SubMode, orc_Name, orc_SubSubModeName, true);
   }

   pc_Handler->SetSubMode(ors32_SubMode, ou32_Index, ou32_Flag);
}

//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_SaveScreenProperties(void) const
{
   C_UsHandler::h_GetInstance()->SetScreenPos(this->normalGeometry().topLeft());
   C_UsHandler::h_GetInstance()->SetAppSize(this->normalGeometry().size());
   C_UsHandler::h_GetInstance()->SetAppMaximized(this->isMaximized());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if mime valid

   \param[in]      opc_Mime      Mime to check
   \param[in,out]  opc_FilePath  Optional parameter for file path output if valid

   \return
   true  Valid
   false Invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_NagMainWindow::mh_CheckMime(const QMimeData * const opc_Mime, QString * const opc_FilePath)
{
   bool q_Retval = false;

   if (opc_Mime != NULL)
   {
      if (opc_Mime->hasUrls() == true)
      {
         QStringList c_PathList;
         QList<QUrl> c_UrlList = opc_Mime->urls();

         // extract the local paths of the files
         for (sintn sn_It = 0; sn_It < c_UrlList.size(); ++sn_It)
         {
            c_PathList.append(c_UrlList.at(sn_It).toLocalFile());
         }
         if (c_PathList.size() == 1)
         {
            QFileInfo c_File;
            c_File.setFile(c_PathList[0]);
            if (c_File.exists() == true)
            {
               if (c_File.suffix().compare("syde") == 0)
               {
                  q_Retval = true;
                  if (opc_FilePath != NULL)
                  {
                     *opc_FilePath = c_PathList[0];
                  }
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Blocking of drag and drop for opening an other project file

   \param[in]  oq_Block    Flag if the drag and drop feature shall be active
                           - true    drag and drop will be blocked
                           - false   drag and drop will be allowed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_BlockDragAndDrop(const bool oq_Block)
{
   this->mq_BlockDragAndDrop = oq_Block;
}

//----------------------------------------------------------------------------------------------------------------------
bool C_NagMainWindow::m_CheckProjectForChanges(void) const
{
   bool q_Result = C_PuiSdHandler::h_GetInstance()->HasHashChanged();

   q_Result = q_Result || C_PuiProject::h_GetInstance()->HasHashChanged();
   q_Result = q_Result || C_PuiSvHandler::h_GetInstance()->HasHashChanged();

   return q_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   An other project will be loaded. Save project specific user settings.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_PrepareProjectLoad(void)
{
   C_UsHandler::h_GetInstance()->SetProjLastScreenMode(this->ms32_SdSubMode, this->mu32_SdIndex, this->mu32_SdFlag,
                                                       this->ms32_SvSubMode, this->mu32_SvIndex, this->mu32_SvFlag);
   C_UsHandler::h_GetInstance()->SetProjLastMode(this->ms32_Mode);
   C_UsHandler::h_GetInstance()->Save();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle change of current project.

   Load project specific user settings and change mode to last known mode (SD/SC),
   where SD always goes to topology and SC also uses last sub mode.
      Cases where mode should switch to the last known mode:
         - open executable with parameters
            (e.g. double click on *.syde, command line with parameters)
         - open project from already running executable
            (e.g. drag-drop *.syde, open-projects-button, select recent project)
      Cases where mode should be main page:
         - open executable without parameters
            (e.g. double click on openSYDE.exe, command line openSYDE.exe only, Qt run)
         - if project load returned an error and therefore the previous one gets restored
            (e.g. missing *.syde_sysdef file)

   \param[in]  orq_SwitchToLastKnownMode  Flag to switch to last known mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_ProjectLoaded(const bool & orq_SwitchToLastKnownMode)
{
   C_UsHandler::h_GetInstance()->LoadActiveProject(C_PuiProject::h_GetInstance()->GetPath());

   C_UsHandler::h_GetInstance()->GetProjLastScreenMode(this->ms32_SdSubMode, this->mu32_SdIndex, this->mu32_SdFlag,
                                                       this->ms32_SvSubMode, this->mu32_SvIndex, this->mu32_SvFlag);

   // reset SD submode always to network topology on project load TODO: remove from user settings save (and remove
   // following index checker)
   this->ms32_SdSubMode = ms32_SUBMODE_SYSDEF_TOPOLOGY;
   this->mu32_SdIndex = 0U;
   this->mu32_SdFlag = 0U;

   // Check the indexes for existence
   if ((this->ms32_SdSubMode == ms32_SUBMODE_SYSDEF_NODEEDIT) &&
       (C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_SdIndex) == NULL))
   {
      this->ms32_SdSubMode = ms32_SUBMODE_SYSDEF_TOPOLOGY;
      this->mu32_SdIndex = 0U;
      this->mu32_SdFlag = 0U;
   }
   else if ((this->ms32_SdSubMode == ms32_SUBMODE_SYSDEF_BUSEDIT) &&
            (C_PuiSdHandler::h_GetInstance()->GetOSCBus(this->mu32_SdIndex) == NULL))
   {
      this->ms32_SdSubMode = ms32_SUBMODE_SYSDEF_TOPOLOGY;
      this->mu32_SdIndex = 0U;
      this->mu32_SdFlag = 0U;
   }
   else
   {
      // Nothing to do
   }

   //Update system views (after project was loaded
   this->mpc_Ui->pc_NaviBar->InitSysView();

   if (C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_SvIndex) == NULL)
   {
      this->ms32_SvSubMode = ms32_SUBMODE_SYSVIEW_SETUP;
      this->mu32_SvIndex = 0U;
      this->mu32_SvFlag = 0U;
   }
   else
   {
      // Special case: Handle system view PC reconnect
      if (this->ms32_SvSubMode != ms32_SUBMODE_SYSVIEW_SETUP)
      {
         bool q_Reconnect;
         tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckViewReconnectNecessary(this->mu32_SvIndex,
                                                                                 q_Reconnect) == C_NO_ERR);
         if (q_Reconnect == true)
         {
            this->ms32_SvSubMode = ms32_SUBMODE_SYSVIEW_SETUP;
         }
      }
   }

   if (orq_SwitchToLastKnownMode == true)
   {
      this->ms32_Mode = C_UsHandler::h_GetInstance()->GetProjLastMode();
      switch (this->ms32_Mode)
      {
      case ms32_MODE_SYSDEF:
         this->ms32_SdSubMode = ms32_SUBMODE_SYSDEF_TOPOLOGY;
         this->m_ChangeUseCase(this->ms32_Mode, this->ms32_SdSubMode);
         break;
      case ms32_MODE_SYSVIEW:
         this->m_ChangeUseCase(this->ms32_Mode, this->ms32_SvSubMode);
      default:
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   An other project will be loaded. Close recent project and save project specific user settings.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_CloseAndPrepareProjectLoad(void)
{
   // close active project
   if (this->mpc_ActiveWidget != NULL)
   {
      if (this->mpc_ActiveWidget->PrepareToClose() == true)
      {
         this->m_RemoveUseCaseWidget();

         // prepare project load
         m_PrepareProjectLoad();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set interaction widget

   \param[in]  opc_Widget  Interaction widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_SetInteractionWidget(QWidget * const opc_Widget)
{
   if (this->mpc_UseCaseWidget != NULL)
   {
      this->mpc_UseCaseWidget->SetInteractionWidget(opc_Widget);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle add new view request
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_HandleAddViewRequest(void)
{
   bool q_Continue;

   if (this->mpc_ActiveWidget != NULL)
   {
      q_Continue = this->mpc_ActiveWidget->PrepareToClose();
   }
   else
   {
      q_Continue = true;
   }
   if (q_Continue == true)
   {
      this->mc_SystemViewManager.AddView();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle rename view action

   Warning: Name assumed to be correct

   \param[in]  ou32_Index  Index identifier
   \param[in]  orc_Name    New name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_HandleRenameView(const uint32 ou32_Index, const QString & orc_Name) const
{
   this->mc_SystemViewManager.RenameView(ou32_Index, orc_Name);
   if (this->mu32_Index == ou32_Index)
   {
      m_UpdateTitle();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update title
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_UpdateTitle(void) const
{
   QString c_SubMode;
   QString c_SubSubMode;

   C_SyvUtil::h_GetViewDisplayName(this->mu32_Index, this->ms32_SubMode, c_SubMode, c_SubSubMode);
   this->mpc_UseCaseWidget->UpdateItemName(c_SubMode, c_SubSubMode, true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle move view request

   \param[in]  ou32_StartIndex   Start index
   \param[in]  ou32_TargetIndex  Target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_HandleMoveViewRequest(const uint32 ou32_StartIndex, const uint32 ou32_TargetIndex)
{
   bool q_Continue;

   if (this->mpc_ActiveWidget != NULL)
   {
      q_Continue = this->mpc_ActiveWidget->PrepareToClose();
   }
   else
   {
      q_Continue = true;
   }
   if (q_Continue == true)
   {
      this->mc_SystemViewManager.MoveView(ou32_StartIndex, ou32_TargetIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle delete view request

   \param[in]  ou32_Index              View index to delete
   \param[in]  os32_SelectedSubMode    Currently selected submode
   \param[in]  ou32_SelectedIndex      Currently selected view index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_HandleDeleteSysViewRequest(const uint32 ou32_Index, const sint32 os32_SelectedSubMode,
                                                   const uint32 ou32_SelectedIndex)
{
   bool q_Continue;

   //Only prepare to delete if: not last view, deleting view before current view & active widget valid
   if (this->mpc_ActiveWidget != NULL)
   {
      q_Continue = this->mpc_ActiveWidget->PrepareToClose();
   }
   else
   {
      q_Continue = true;
   }

   if (q_Continue == true)
   {
      this->mc_SystemViewManager.DeleteSysView(ou32_Index, os32_SelectedSubMode, ou32_SelectedIndex, this);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle duplicate system view request

   \param[in]  ou32_Index  System view index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWindow::m_HandleDuplicateSysViewRequest(const uint32 ou32_Index)
{
   bool q_Continue;

   if (this->mpc_ActiveWidget != NULL)
   {
      q_Continue = this->mpc_ActiveWidget->PrepareToClose();
   }
   else
   {
      q_Continue = true;
   }
   if (q_Continue == true)
   {
      this->mc_SystemViewManager.DuplicateSysView(ou32_Index);
   }
}

//----------------------------------------------------------------------------------------------------------------------
bool C_NagMainWindow::m_ChangeMode(const stw_types::sint32 os32_Mode, const sint32 os32_SubMode,
                                   const uint32 ou32_Index, const QString & orc_Name, const QString & orc_SubSubName,
                                   const uint32 ou32_Flag, const bool oq_ChangeUseCase)
{
   bool q_Continue;
   sint32 s32_SubMode = os32_SubMode;
   uint32 u32_Index = ou32_Index;
   QString c_Name = orc_Name;
   QString c_SubSubName = orc_SubSubName;
   uint32 u32_Flag = ou32_Flag;

   const uint16 u16_TimerId = osc_write_log_performance_start();

   // Get the previous parameter for each use case, if a change from the main widget was triggered
   this->m_AdaptParameter(os32_Mode, s32_SubMode, u32_Index, c_Name, c_SubSubName, u32_Flag, oq_ChangeUseCase);

   if (((os32_Mode != this->ms32_Mode) ||
        (s32_SubMode != this->ms32_SubMode) ||
        (u32_Index != this->mu32_Index)) || (this->mq_StartView == true))
   {
      //Flag values of 1 indicate the prepare to close event was already handled
      if ((this->mpc_ActiveWidget != NULL) && ((os32_Mode != ms32_MODE_SYSVIEW) || (ou32_Flag != 1UL)))
      {
         // a change of mode or submode will happen
         // is a change possible at the moment?
         q_Continue = this->mpc_ActiveWidget->PrepareToClose();
      }
      else
      {
         //New mode
         q_Continue = true;
      }
   }
   else
   {
      //Same mode
      q_Continue = false;
   }

   if (q_Continue == true)
   {
      //We will restore the last known splitter config after change of mode so store the current config (only if
      // useful!)
      if ((this->mpc_MainWidget == NULL) || (this->mpc_MainWidget->isVisible() == false))
      {
         this->mpc_Ui->pc_Splitter->StoreUserSettings();
         this->mpc_Ui->pc_NaviBar->SaveUserSettings();
      }
      // change the mode
      if (os32_Mode == ms32_MODE_SYSDEF)
      {
         // special case: system definition
         this->m_ShowSysDefItem(s32_SubMode, u32_Index, c_Name, c_SubSubName, u32_Flag);
         // update Navigationbar
         this->mpc_Ui->pc_NaviBar->SetMode(os32_Mode, s32_SubMode, u32_Index);
         this->mpc_Ui->pc_TopToolBar->ShowSearch(true);
      }
      else if (os32_Mode == ms32_MODE_SYSVIEW)
      {
         // special case: system view
         this->m_ShowSysViewItem(s32_SubMode, u32_Index, c_Name, c_SubSubName, u32_Flag);
         // update Navigationbar
         this->mpc_Ui->pc_NaviBar->SetMode(os32_Mode, s32_SubMode, u32_Index);
         this->mpc_Ui->pc_TopToolBar->ShowSearch(false);
         //Do explicit update system view items
         this->mpc_Ui->pc_NaviBar->UpdateAllScreenIcons(false);
      }
      else
      {
         // nothing to do
      }

      this->ms32_Mode = os32_Mode;
      this->ms32_SubMode = s32_SubMode;
      this->mu32_Index = u32_Index;

      this->mq_StartView = false;

      //Restore user settings
      this->mpc_Ui->pc_Splitter->LoadUserSettings();
      this->mpc_Ui->pc_NaviBar->LoadUserSettings();
   }

   osc_write_log_performance_stop(u16_TimerId, QString("Switch to mode %1 submode %2 index %3")
                                  .arg(this->ms32_Mode).arg(this->ms32_SubMode).arg(
                                     this->mu32_Index).toStdString().c_str());

   return q_Continue;
}
