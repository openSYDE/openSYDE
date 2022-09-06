//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing system view update

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#include <QApplication>
#include <QFileInfo>

#include "C_SyvUpUpdateWidget.h"
#include "ui_C_SyvUpUpdateWidget.h"

#include "stwerrors.h"
#include "constants.h"
#include "CSCLString.h"
#include "TGLTime.h"
#include "TGLFile.h"
#include "C_Uti.h"
#include "C_GtGetText.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCUtils.h"
#include "C_SyvUtil.h"
#include "C_OgeWiUtil.h"
#include "C_UsHandler.h"
#include "C_SyvUpDeviceInfo.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSvData.h"
#include "C_PuiSdHandler.h"
#include "DLLocalize.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_scl;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvUpUpdateWidget::mhc_TEMP_FOLDER = "system_update_temp";
const sintn C_SyvUpUpdateWidget::mhsn_WIDGET_BORDER = 12;
const sintn C_SyvUpUpdateWidget::mhsn_TOOLBOX_INIT_POS_Y = 150;
const stw_types::uint32 C_SyvUpUpdateWidget::mhu32_WAIT_TIME = 5100U;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      ou32_ViewIndex         View index
   \param[in]      opc_ProgressLogParent  Optional pointer to progress log parent
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdateWidget::C_SyvUpUpdateWidget(const uint32 ou32_ViewIndex, QWidget * const opc_ProgressLogParent,
                                         QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpUpdateWidget),
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_ApplicationIndex(0U),
   mpc_Scene(NULL),
   mpc_ProgressLog(NULL),
   mpc_FixMinimizedProgressLog(NULL),
   mpc_ProgressLogParent(opc_ProgressLogParent),
   mpc_ProgressLogContent(NULL),
   mpc_UpSequences(NULL),
   me_Step(C_SyvUpSequences::eNOT_ACTIVE),
   mq_Connected(false),
   mq_ClearProgressLog(true),
   mq_StartUpdateAfterConnect(false),
   mq_ConnectFailed(false),
   mq_ErrorDetected(false),
   mq_NodesPreconditionError(false),
   mu32_DisconnectTime(0U),
   mu32_UpdateTime(0U)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   QString c_Name;

   mpc_Ui->setupUi(this);

   //Style error label
   this->mpc_Ui->pc_ErrorLabelTitle->SetForegroundColor(24);
   this->mpc_Ui->pc_ErrorLabelTitle->SetFontPixel(14, false, true);
   this->mpc_Ui->pc_ErrorLabelIcon->SetSvg("://images/Error_iconV2.svg");

   this->m_InitToolBox();

   // Initialize the update summary screen and update package
   this->mpc_Ui->pc_WiUpdateInformation->SetViewIndex(this->mu32_ViewIndex);

   //Splitter
   this->mpc_Ui->pc_SplitterHori->SetMargins(20, 20);
   this->mpc_Ui->pc_SplitterHori->setStretchFactor(0, 1);

   // create scene for graphics view
   this->mpc_Scene = new C_SyvUpScene(this->mu32_ViewIndex);

   // configure scene
   this->mpc_Scene->setSceneRect(0.0, 0.0,
                                 static_cast<float64>(this->mpc_Ui->pc_GraphicsView->width()),
                                 static_cast<float64>(this->mpc_Ui->pc_GraphicsView->height()));
   this->mpc_Ui->pc_GraphicsView->SetSceneAndConnect(this->mpc_Scene);
   this->mpc_Scene->SetDrawingBackground(false);
   this->mpc_Ui->pc_GraphicsView->SetDrawingBackground(false);

   //Update all items with initial zoom & pos value
   if (pc_View != NULL)
   {
      c_Name = pc_View->GetName();
   }
   else
   {
      c_Name = "";
   }
   this->mpc_Ui->pc_GraphicsView->SetZoomValue(C_UsHandler::h_GetInstance()->GetProjSvSetupView(
                                                  c_Name).sn_UpdateViewZoom, true);
   this->mpc_Ui->pc_GraphicsView->SetViewPos(C_UsHandler::h_GetInstance()->GetProjSvSetupView(c_Name).c_UpdateViewPos);

   // Initialize the GUI
   this->mpc_Ui->pc_PbConnect->setEnabled(false);
   this->mpc_Ui->pc_PbUpdate->setEnabled(false);
   this->mpc_Ui->pc_PbDisconnnect->setEnabled(false);
   this->mpc_Ui->pc_PbCancel->setVisible(false);

   this->InitText();
   this->InitIcon();

   //Connects for tool tip
   connect(this->mpc_Scene, &C_SebScene::SigShowToolTip, this->mpc_Ui->pc_GraphicsView,
           &C_SebGraphicsView::ShowToolTip);
   connect(this->mpc_Scene, &C_SebScene::SigHideToolTip, this->mpc_Ui->pc_GraphicsView,
           &C_SebGraphicsView::HideToolTip);
   connect(this->mpc_Ui->pc_GraphicsView, &C_SebGraphicsView::SigShowToolTip, this->mpc_Scene,
           &C_SebScene::DisplayToolTip);
   // Connects
   connect(this->mpc_Ui->pc_PbConnect, &stw_opensyde_gui_elements::C_OgePubUpdate::clicked, this,
           &C_SyvUpUpdateWidget::m_Connect);
   connect(this->mpc_Ui->pc_PbUpdate, &stw_opensyde_gui_elements::C_OgePubUpdate::clicked, this,
           &C_SyvUpUpdateWidget::m_Update);
   connect(this->mpc_Ui->pc_PbDisconnnect, &stw_opensyde_gui_elements::C_OgePubUpdate::clicked, this,
           &C_SyvUpUpdateWidget::m_Disconnect);
   connect(this->mpc_Ui->pc_PbCancel, &stw_opensyde_gui_elements::C_OgePubUpdate::clicked, this,
           &C_SyvUpUpdateWidget::m_Cancel);
   connect(this->mpc_Scene, &C_SyvUpScene::SigDiscardInfo, this, &C_SyvUpUpdateWidget::m_DiscardInfo);
   connect(this, &C_SyvUpUpdateWidget::SigNodeConnectStates, this->mpc_Scene, &C_SyvUpScene::SetNodeConnectStates);
   connect(this, &C_SyvUpUpdateWidget::SigNodeUpdateStates, this->mpc_Scene, &C_SyvUpScene::SetNodeUpdateStates);

   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SyvUpUpdateWidget::m_Timer);

   // Progress signals
   // We need a signal which is emitted in the other thread
   // We have to register the type and use the queued connections. Auto is default, but did not work.
   qRegisterMetaType<uint32>("uint32");
   qRegisterMetaType<sint32>("sint32");
   qRegisterMetaType<uint8>("uint8");

   connect(this->mpc_Ui->pc_WiUpdateInformation, &C_SyvUpInformationWidget::SigUpdatePackageState,
           this, &C_SyvUpUpdateWidget::m_UpdatePackageState);

   this->mc_Timer.setInterval(25);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_Scene and the Qt memory management
C_SyvUpUpdateWidget::~C_SyvUpUpdateWidget()
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   // store configuration of the view
   if (pc_View != NULL)
   {
      //Splitter
      const QList<sintn> c_Sizes = this->mpc_Ui->pc_SplitterHori->sizes();
      if (c_Sizes.count() > 1)
      {
         C_UsHandler::h_GetInstance()->SetProjSvUpdateHorizontalSplitterY(pc_View->GetName(), c_Sizes.at(1));
      }

      //Scene
      C_UsHandler::h_GetInstance()->SetProjSvUpdateViewZoom(
         pc_View->GetName(), this->mpc_Ui->pc_GraphicsView->GetZoomValue());
      C_UsHandler::h_GetInstance()->SetProjSvUpdateViewPos(
         pc_View->GetName(), this->mpc_Ui->pc_GraphicsView->GetViewPos());
   }
   m_CleanUpProgressLog();
   delete mpc_Ui;
   mpc_Ui = NULL;

   this->m_CleanUpSequence();

   mpc_ProgressLog = NULL;
   mpc_FixMinimizedProgressLog = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::InitText(void) const
{
   // button text
   this->mpc_Ui->pc_PbUpdate->setText(static_cast<QString>(C_GtGetText::h_GetText("Update System")));
   this->mpc_Ui->pc_PbConnect->setText(static_cast<QString>(C_GtGetText::h_GetText("Enter Update Mode")));
   this->mpc_Ui->pc_PbDisconnnect->setText(static_cast<QString>(C_GtGetText::h_GetText("Exit Update Mode")));
   this->mpc_Ui->pc_PbCancel->setText(static_cast<QString>(C_GtGetText::h_GetText("Abort System Update")));

   // tooltips
   this->mpc_Ui->pc_PbUpdate->SetToolTipInformation(
      C_GtGetText::h_GetText("Update System"),
      C_GtGetText::h_GetText("Updates all devices that have a different version compared to "
                             "the version in the Update Package. \n"
                             "Triggers automatically \"Enter Update Mode\" first, if not done yet."));
   this->mpc_Ui->pc_PbConnect->SetToolTipInformation(C_GtGetText::h_GetText("Enter Update Mode"),
                                                     C_GtGetText::h_GetText(
                                                        "Puts all devices into flashloader mode and gets current "
                                                        "versions of each device."));
   this->mpc_Ui->pc_PbDisconnnect->SetToolTipInformation(C_GtGetText::h_GetText("Exit Update Mode"),
                                                         C_GtGetText::h_GetText(
                                                            "Puts all devices back into application mode."));
   this->mpc_Ui->pc_PbCancel->SetToolTipInformation(C_GtGetText::h_GetText("Abort System Update"),
                                                    C_GtGetText::h_GetText(
                                                       "Forces the update to abort."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to prepare closing the widget

   \return
   true     Can be closed.
   false    Can not be closed.
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::PrepareToClose(void)
{
   bool q_Return = true;

   if (this->mpc_UpSequences != NULL)
   {
      bool q_Disconnect = false;

      q_Return = false;

      if (this->me_Step == C_SyvUpSequences::eNOT_ACTIVE)
      {
         if (this->mq_Connected == true)
         {
            q_Disconnect = true;
         }

         q_Return = true;
      }
      else if (this->me_Step == C_SyvUpSequences::eUPDATE_SYSTEM)
      {
         // Update sequence can be aborted
         C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
         C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;

         c_MessageBox.SetHeading(C_GtGetText::h_GetText("System Update abort"));
         c_MessageBox.SetDescription(
            C_GtGetText::h_GetText("System Update is running. Do you really want to abort the current System Update?"));
         c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Continue Update"));
         c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Abort Update"));
         c_MessageBox.SetCustomMinWidth(650);
         c_MessageBox.SetCustomMinHeight(180, 180);
         e_ReturnMessageBox = c_MessageBox.Execute();

         if (e_ReturnMessageBox == C_OgeWiCustomMessage::eNO)
         {
            this->mpc_UpSequences->AbortCurrentProgress();
            q_Return = true;
            q_Disconnect = true;

            // Wait till the thread has finished and process the signals from the thread
            do
            {
               QApplication::processEvents();
            }
            while (this->me_Step == C_SyvUpSequences::eUPDATE_SYSTEM);
         }
      }
      else
      {
         // Sequence is not interruptable
         q_Return = false;
      }

      if (q_Disconnect == true)
      {
         // Make a disconnect
         sint32 s32_Result = this->mpc_UpSequences->StartResetSystem();

         if (s32_Result == C_NO_ERR)
         {
            sint32 s32_SequenceResult;
            const QString c_Text =  C_GtGetText::h_GetText("Disconnect: Start");

            this->m_UpdateReportText(c_Text);

            this->mpc_Ui->pc_PbDisconnnect->setEnabled(false);
            this->mpc_Ui->pc_PbUpdate->setEnabled(false);

            // Wait for the result
            do
            {
               s32_Result = this->mpc_UpSequences->GetResults(s32_SequenceResult);
            }
            while (s32_Result != C_NO_ERR);

            // In the event queue could be some emitted signals from thread left.
            QApplication::processEvents();
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delayed load of scene
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::LoadScene(void)
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->Load();

      this->m_CheckError();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Load splitter position

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::showEvent(QShowEvent * const opc_Event)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   // store configuration of the view
   if (pc_View != NULL)
   {
      const C_UsSystemView c_UserView = C_UsHandler::h_GetInstance()->GetProjSvSetupView(pc_View->GetName());
      sint32 s32_LastSegmentWidth = c_UserView.GetUpdateHorizontalSplitterY();

      //Revert to default if necessary
      if (s32_LastSegmentWidth <= 0)
      {
         s32_LastSegmentWidth = 400;
      }

      this->mpc_Ui->pc_SplitterHori->SetSecondSegment(s32_LastSegmentWidth);
   }
   QWidget::showEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten hide event slot

   Here: hide and delete toolbox

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::hideEvent(QHideEvent * const opc_Event)
{
   QWidget::hideEvent(opc_Event);
   m_CleanUpProgressLog();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event

   Move the toolbox.

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   if (this->mpc_ProgressLog != NULL)
   {
      Q_UNUSED(opc_Event)

      // only resize if scene is active to avoid bad toolbox geometry in case of "open tool with *.syde double click"
      //      if ((this->mpc_Scene != NULL) && (this->mpc_Scene->isActive() == true))
      //      {
      QPoint c_Point = this->mpc_ProgressLog->pos();
      QSize c_Size = this->mpc_ProgressLog->size();

      QPoint c_PointFixMiniProgressLog = this->mpc_FixMinimizedProgressLog->pos();

      QWidget * pc_Widget = this->mpc_ProgressLogParent;

      if (pc_Widget == NULL)
      {
         // if no parent exist use this widget
         pc_Widget = this;
      }

      // would the toolbox be outside of the widget in x direction
      if ((this->mpc_ProgressLog->x() + this->mpc_ProgressLog->width() + mhsn_WIDGET_BORDER) > pc_Widget->width())
      {
         // is the toolbox to big?
         if ((this->mpc_ProgressLog->width() + (2 * mhsn_WIDGET_BORDER)) > pc_Widget->width())
         {
            c_Size.setWidth(pc_Widget->width() - (2 * mhsn_WIDGET_BORDER));
         }
         else
         {
            // adapt position of toolbox
            c_Point.setX((pc_Widget->width() - this->mpc_ProgressLog->width()) - mhsn_WIDGET_BORDER);
         }
      }

      // would the toolbox be outside of the widget in y direction
      if ((this->mpc_ProgressLog->y() + this->mpc_ProgressLog->height() + mhsn_WIDGET_BORDER) > pc_Widget->height())
      {
         // is the toolbox to big?
         if ((this->mpc_ProgressLog->height() + (2 * mhsn_WIDGET_BORDER)) > pc_Widget->height())
         {
            c_Size.setHeight(pc_Widget->height() - (2 * mhsn_WIDGET_BORDER));
         }
         else
         {
            // adapt position of toolbox
            c_Point.setY((pc_Widget->height() - this->mpc_ProgressLog->height()) - mhsn_WIDGET_BORDER);
         }
      }

      // adapt position of fix minimized toolbox
      c_PointFixMiniProgressLog.setX((pc_Widget->width() - this->mpc_FixMinimizedProgressLog->width()) -
                                     mhsn_WIDGET_BORDER);

      this->mpc_ProgressLog->setGeometry(QRect(c_Point, c_Size));
      this->mpc_FixMinimizedProgressLog->setGeometry(QRect(c_PointFixMiniProgressLog,
                                                           this->mpc_FixMinimizedProgressLog->size()));
      //      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update system setup error label
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_CheckError(void)
{
   QString c_ErrorText;
   QString c_ErrorTextHeading;
   QString c_ErrorTextTooltip;

   C_NagToolTip::E_Type e_ToolTipType;
   QString c_IconPath;
   sintn sn_ColorID;
   this->mq_ErrorDetected = C_SyvUtil::h_GetViewStatusLabelInfo(
      this->mu32_ViewIndex, ms32_SUBMODE_SYSVIEW_UPDATE, c_ErrorTextHeading, c_ErrorText, c_ErrorTextTooltip,
      e_ToolTipType, c_IconPath, sn_ColorID);

   if (this->mq_ErrorDetected == true)
   {
      //Disable error if info only
      this->mq_ErrorDetected = e_ToolTipType == C_NagToolTip::eERROR;

      this->mpc_Ui->pc_ErrorLabelIcon->SetSvg(c_IconPath);
      this->mpc_Ui->pc_ErrorLabelIcon->SetToolTipInformation(C_GtGetText::h_GetText("Invalid"),
                                                             c_ErrorTextTooltip, e_ToolTipType);
      this->mpc_Ui->pc_ErrorLabelTitle->SetForegroundColor(sn_ColorID);
      this->mpc_Ui->pc_ErrorLabelTitle->setText(c_ErrorTextHeading);
      this->mpc_Ui->pc_ErrorLabelTitle->SetToolTipInformation(C_GtGetText::h_GetText("Invalid"),
                                                              c_ErrorTextTooltip, e_ToolTipType);
      this->mpc_Ui->pc_ErrorLabel->SetForegroundColor(sn_ColorID);
      this->mpc_Ui->pc_ErrorLabel->SetCompleteText(c_ErrorText, c_ErrorTextTooltip, e_ToolTipType);
      this->mpc_Ui->pc_GroupBoxErrorContent->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxErrorContent->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize system update sequence

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpUpdateWidget::m_InitSequence(void)
{
   bool q_IsEthernet = false;
   QString c_Message;
   sint32 s32_Retval;

   //Check if ethernet
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      if (pc_View->GetPcData().GetConnected())
      {
         const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
            pc_View->GetPcData().GetBusIndex());
         if (pc_Bus != NULL)
         {
            if (pc_Bus->e_Type == C_OSCSystemBus::eETHERNET)
            {
               q_IsEthernet = true;
            }
         }
      }
   }

   // Sequence initialization

   if (this->mpc_UpSequences == NULL)
   {
      this->mpc_UpSequences = new C_SyvUpSequences();

      connect(this->mpc_UpSequences, &C_SyvUpSequences::SigReportProgress, this,
              &C_SyvUpUpdateWidget::m_ReportProgress, Qt::QueuedConnection);
      connect(this->mpc_UpSequences, &C_SyvUpSequences::SigReportProgressForServer, this,
              &C_SyvUpUpdateWidget::m_ReportProgressForServer, Qt::QueuedConnection);
      connect(this->mpc_UpSequences, &C_SyvUpSequences::SigReportOpenSydeFlashloaderInformationRead, this,
              &C_SyvUpUpdateWidget::m_ReportOpenSydeFlashloaderInformationRead);
      connect(this->mpc_UpSequences, &C_SyvUpSequences::SigReportStwFlashloaderInformationRead, this,
              &C_SyvUpUpdateWidget::m_ReportStwFlashloaderInformationRead);
   }

   if (this->mpc_UpSequences->IsInitialized() == false)
   {
      s32_Retval = this->mpc_UpSequences->InitUpSequences(this->mu32_ViewIndex);
   }
   else
   {
      s32_Retval = this->mpc_UpSequences->ReinitDispatcher();
   }

   C_OgeWiCustomMessage c_MessageBox(this);

   switch (s32_Retval)
   {
   case C_NO_ERR:
      break;
   case C_CONFIG:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("Invalid SYSTEM DEFINITION/View configuration."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_RD_WR:
      c_Message =
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "Configured communication DLL does not exist or DLL could not be opened."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_OVERFLOW:
      c_Message =
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "Unknown transport protocol or unknown diagnostic server for at least one node."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_NOACT:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("System View is invalid. Action cannot be performed."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_COM:
      if (q_IsEthernet)
      {
         c_Message =
            static_cast<QString>(
               C_GtGetText::h_GetText(
                  "Ethernet initialization failed. Check your Ethernet adapter settings in Windows system network configuration."));
         c_MessageBox.SetCustomMinHeight(180, 180);
      }
      else
      {
         c_Message =
            static_cast<QString>(
               C_GtGetText::h_GetText(
                  "CAN initialization failed. Check your PC CAN interface configuration (System View setup - "
                  "double-click on PC)."));
         c_MessageBox.SetCustomMinHeight(200, 200);
      }
      break;
   case C_CHECKSUM:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("Internal buffer overflow detected."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_RANGE:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("Routing configuration failed."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_UNKNOWN_ERR:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("Wrapped error of internal function call."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_WARN:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("Internal error."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   case C_BUSY:
      c_Message = static_cast<QString>(
         C_GtGetText::h_GetText(
            "System View is invalid. Action cannot be performed. Fix the issues and retry."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   default:
      c_Message = static_cast<QString>(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(s32_Retval));
      c_MessageBox.SetCustomMinHeight(180, 180);
      break;
   }

   if (s32_Retval != C_NO_ERR)
   {
      c_MessageBox.SetType(C_OgeWiCustomMessage::E_Type::eERROR);
      c_MessageBox.SetDescription(c_Message);
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Enter update mode"));
      c_MessageBox.Execute();

      // Simplify return value
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_CleanUpSequence(void)
{
   if (this->mpc_UpSequences != NULL)
   {
      this->mpc_UpSequences->CloseDispatcher();

      disconnect(this->mpc_UpSequences, &C_SyvUpSequences::SigReportProgress, this,
                 &C_SyvUpUpdateWidget::m_ReportProgress);
      disconnect(this->mpc_UpSequences, &C_SyvUpSequences::SigReportProgressForServer, this,
                 &C_SyvUpUpdateWidget::m_ReportProgressForServer);
      disconnect(this->mpc_UpSequences, &C_SyvUpSequences::SigReportOpenSydeFlashloaderInformationRead, this,
                 &C_SyvUpUpdateWidget::m_ReportOpenSydeFlashloaderInformationRead);
      disconnect(this->mpc_UpSequences, &C_SyvUpSequences::SigReportStwFlashloaderInformationRead, this,
                 &C_SyvUpUpdateWidget::m_ReportStwFlashloaderInformationRead);

      delete mpc_UpSequences;
      this->mpc_UpSequences = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the state of the buttons depending on the update package and the view error state

   \param[in]  os32_State  State of update package
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_UpdatePackageState(const sint32 os32_State)
{
   if ((os32_State == C_NO_ERR) &&
       (this->mq_ErrorDetected == false))
   {
      this->mpc_Ui->pc_PbConnect->setEnabled(true);
      this->mpc_Ui->pc_PbUpdate->setEnabled(true);
   }
   else
   {
      this->mpc_Ui->pc_PbConnect->setEnabled(false);
      this->mpc_Ui->pc_PbUpdate->setEnabled(false);
   }
   //Update scene as well
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->CheckUpdateDisabledState();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reporting slot for system update

   \param[in]  ou32_Step      Step of node configuration
   \param[in]  os32_Result    Result of service
   \param[in]  ou8_Progress   Progress of sequence in percentage (goes from 0..100 for each function)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_ReportProgress(const uint32 ou32_Step, const sint32 os32_Result, const uint8 ou8_Progress)
{
   const C_OSCSuSequences::E_ProgressStep e_Step = static_cast<C_OSCSuSequences::E_ProgressStep>(ou32_Step);

   (void)os32_Result;  //details not reported by this layer
   (void)ou8_Progress; //[...]

   //Stop animation
   if (e_Step == C_OSCSuSequences::eUPDATE_SYSTEM_FINISHED)
   {
      if (this->mpc_Scene != NULL)
      {
         this->mpc_Scene->StopProgressAnimation(false, 0, true);
      }
      //Signal summary
      this->mpc_Ui->pc_WiUpdateInformation->SetHeading("://images/system_views/UpdateSummaryOk.svg",
                                                       C_GtGetText::h_GetText(
                                                          "System Update successful!"), false, true);
      this->mpc_Ui->pc_WiUpdateInformation->StopElapsedTimer();
      this->mpc_Ui->pc_WiUpdateInformation->UpdateProgress(100, true);
      //Signal progress log
      if (this->mpc_ProgressLogContent != NULL)
      {
         this->mpc_ProgressLogContent->AddSubHeading(C_GtGetText::h_GetText("System Update successful!"));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reporting slot for system update for specific server

   \param[in]  ou32_Step            Step of node configuration
   \param[in]  os32_Result          Result of service
   \param[in]  ou8_Progress         Progress of sequence in percentage (goes from 0..100 for each function)
                                    Progress invalid: 255
   \param[in]  ou8_BusIdentifier    Affected sub net
   \param[in]  ou8_NodeIdentifier   Node Id of affected node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_ReportProgressForServer(const uint32 ou32_Step, const sint32 os32_Result,
                                                    const uint8 ou8_Progress, const uint8 ou8_BusIdentifier,
                                                    const uint8 ou8_NodeIdentifier)
{
   const C_OSCSuSequences::E_ProgressStep e_Step = static_cast<C_OSCSuSequences::E_ProgressStep>(ou32_Step);
   uint32 u32_NodeIndex;
   uint32 u32_BusIndex;
   const sint32 s32_Result = C_SyvUtil::h_GetIndicesFromBusId(ou8_BusIdentifier, ou8_NodeIdentifier, u32_NodeIndex,
                                                              u32_BusIndex);

   (void)os32_Result;

   //Report progress
   if (s32_Result == C_NO_ERR)
   {
      bool q_IsParam;
      bool q_IsPemFile;
      if (C_SyvUpUpdateWidget::mh_IsConnectionStart(e_Step) == true)
      {
         //Signal progress log
         m_AddProgressLogConnectEntry(u32_NodeIndex);
      }
      else if (C_SyvUpUpdateWidget::mh_IsUpdateAppStart(e_Step, q_IsParam, q_IsPemFile) == true)
      {
         this->mpc_Ui->pc_WiUpdateInformation->SetUpdateApplicationStarted(u32_NodeIndex, q_IsParam, q_IsPemFile);
      }
      else if (C_SyvUpUpdateWidget::mh_IsUpdateNodeStart(e_Step) == true)
      {
         const C_PuiSdNode * const pc_UINode = C_PuiSdHandler::h_GetInstance()->GetUINode(u32_NodeIndex);
         this->mu32_ApplicationIndex = 0;
         //Signal progress
         this->mpc_Ui->pc_WiUpdateInformation->SetUpdateNodeStarted(u32_NodeIndex);
         //Signal scene
         if (this->mpc_Scene != NULL)
         {
            // Stop the previous animation first
            this->mpc_Scene->StopProgressAnimation(false, 0, false);
            this->mpc_Scene->StartProgressAnimation(u32_NodeIndex);
         }
         //Scroll scene
         if (pc_UINode != NULL)
         {
            this->mpc_Ui->pc_GraphicsView->ScrollTo(pc_UINode->c_UIPosition,
                                                    QSizeF(pc_UINode->f64_Width, pc_UINode->f64_Height));
         }
         //Signal progress log
         if (this->mpc_ProgressLogContent != NULL)
         {
            const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodeIndex);
            if (pc_Node != NULL)
            {
               const QString c_Text = static_cast<QString>(C_GtGetText::h_GetText("Update %1...")).arg(
                  pc_Node->c_Properties.c_Name.c_str());
               this->mpc_ProgressLogContent->AddEntry(u32_NodeIndex, c_Text, true);
            }
         }
      }
      else if (C_SyvUpUpdateWidget::mh_IsConnectionFailure(e_Step) == true)
      {
         //Signal progress log
         if (this->mpc_ProgressLogContent != NULL)
         {
            this->mpc_ProgressLogContent->UpdateStatus(u32_NodeIndex, C_GtGetText::h_GetText("FAIL"), false);
         }
         //Signal scene
         if (this->mpc_Scene != NULL)
         {
            this->mpc_Scene->SetNodeError(u32_NodeIndex);
         }
      }
      else if (C_SyvUpUpdateWidget::mh_IsConnectionSuccess(e_Step) == true)
      {
         //Signal progress log
         if (this->mpc_ProgressLogContent != NULL)
         {
            this->mpc_ProgressLogContent->UpdateStatus(u32_NodeIndex, C_GtGetText::h_GetText("OK"), false);
         }
      }
      else if (C_SyvUpUpdateWidget::mh_IsUpdateFailure(e_Step) == true)
      {
         //Signal other widgets (signals might be missing)
         this->mpc_Ui->pc_WiUpdateInformation->SetUpdateApplicationError(u32_NodeIndex);
         //Signal progress log
         if (this->mpc_ProgressLogContent != NULL)
         {
            this->mpc_ProgressLogContent->UpdateStatus(u32_NodeIndex, C_GtGetText::h_GetText("FAIL"), true);
         }
         //Signal scene
         if (this->mpc_Scene != NULL)
         {
            this->mpc_Scene->SetNodeError(u32_NodeIndex);
            this->mpc_Scene->StopProgressAnimation(true, this->mu32_ApplicationIndex, true);
         }
      }
      else if (C_SyvUpUpdateWidget::mh_IsUpdateAbort(e_Step) == true)
      {
         this->mpc_Ui->pc_WiUpdateInformation->SetUpdateApplicationError(u32_NodeIndex);
         //Signal progress log
         if (this->mpc_ProgressLogContent != NULL)
         {
            this->mpc_ProgressLogContent->UpdateStatus(u32_NodeIndex, C_GtGetText::h_GetText("USER CANCELED"), true);
         }
         //Signal scene
         if (this->mpc_Scene != NULL)
         {
            this->mpc_Scene->StopProgressAnimation(true, this->mu32_ApplicationIndex, true);
         }
         //Signal summary
         this->mpc_Ui->pc_WiUpdateInformation->SetHeading("://images/system_views/UpdateSummaryFailure.svg",
                                                          C_GtGetText::h_GetText("System Update failed!"), true, false);
         this->mpc_Ui->pc_WiUpdateInformation->StopElapsedTimer();
      }
      else if (C_SyvUpUpdateWidget::mh_IsUpdateAppSuccess(e_Step, q_IsParam, q_IsPemFile) == true)
      {
         ++this->mu32_ApplicationIndex;
         this->mpc_Ui->pc_WiUpdateInformation->SetUpdateApplicationFinished(u32_NodeIndex, q_IsParam, q_IsPemFile);
      }
      else if (C_SyvUpUpdateWidget::mh_IsUpdateNodeSuccess(e_Step) == true)
      {
         //Signal progress
         this->mpc_Ui->pc_WiUpdateInformation->SetUpdateNodeSuccess(u32_NodeIndex);
         //Signal progress log
         if (this->mpc_ProgressLogContent != NULL)
         {
            this->mpc_ProgressLogContent->UpdateStatus(u32_NodeIndex, C_GtGetText::h_GetText("OK"), true);
         }
      }
      else if (this->me_Step == C_SyvUpSequences::eUPDATE_SYSTEM)
      {
         switch (e_Step) //lint !e788 //we do not handle all steps here
         {
         case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_START: // The normal flash progress
         case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_START:     // The normal flash progress
         case C_OSCSuSequences::eXFL_PROGRESS:                                         // The normal flash progress
            if (ou8_Progress != 0xFF)
            {
               this->mpc_Ui->pc_WiUpdateInformation->SetNodeProgress(u32_NodeIndex, ou8_Progress);
               //Signal summary
               this->mpc_Ui->pc_WiUpdateInformation->UpdateProgress(static_cast<uint16>(ou8_Progress), false);
            }

            break;
         default:
            break;
         }
      }
      else
      {
         // Nothing to do
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reporting slot for read openSYDE flashloader information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_ReportOpenSydeFlashloaderInformationRead(void)
{
   if (this->mpc_UpSequences != NULL)
   {
      std::vector<uint32> c_NodeIndexes;
      std::vector<C_OSCSuSequences::C_OsyDeviceInformation> c_DeviceInformation;
      std::vector<C_SyvUpDeviceInfo> c_Devices;
      uint32 u32_Counter;

      this->mpc_UpSequences->GetOsyDeviceInformation(c_NodeIndexes, c_DeviceInformation);

      for (u32_Counter = 0U; u32_Counter < c_NodeIndexes.size(); ++u32_Counter)
      {
         const C_OSCSuSequences::C_OsyDeviceInformation & rc_Info = c_DeviceInformation[u32_Counter];
         QString c_Temp;

         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "openSYDE device information found for node with index %1")).arg(
               c_NodeIndexes[u32_Counter]));
         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText("Device name: %1")).arg(rc_Info.c_DeviceName.c_str()));
         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText("Number of applications: %1")).arg(rc_Info.c_Applications.
                                                                                           size()));

         for (uint8 u8_Application = 0U; u8_Application < rc_Info.c_Applications.size(); u8_Application++)
         {
            this->m_UpdateReportText(
               static_cast<QString>(C_GtGetText::h_GetText("Application %1")).arg(static_cast<uint32>(u8_Application)));
            this->m_UpdateReportText(
               static_cast<QString>(C_GtGetText::h_GetText(" Name: %1")).arg(
                  rc_Info.c_Applications[u8_Application].c_ApplicationName.c_str()));
            this->m_UpdateReportText(
               static_cast<QString>(C_GtGetText::h_GetText(" Version: %1")).arg(
                  rc_Info.c_Applications[u8_Application].c_ApplicationVersion.c_str()));
            this->m_UpdateReportText(
               static_cast<QString>(C_GtGetText::h_GetText(" Build date: %1")).arg(
                  rc_Info.c_Applications[u8_Application].c_BuildDate.c_str()));
            this->m_UpdateReportText(
               static_cast<QString>(C_GtGetText::h_GetText(" Build time: %1")).arg(
                  rc_Info.c_Applications[u8_Application].c_BuildTime.c_str()));
            this->m_UpdateReportText(
               static_cast<QString>(C_GtGetText::h_GetText(" Block start address: 0x%1")).arg(
                  QString::number(rc_Info.c_Applications[u8_Application].u32_BlockStartAddress, 16).rightJustified(
                     8, '0')));
            this->m_UpdateReportText(
               static_cast<QString>(C_GtGetText::h_GetText(" Block end address: 0x%1")).arg(
                  QString::number(rc_Info.c_Applications[u8_Application].u32_BlockEndAddress, 16).rightJustified(
                     8, '0')));
            this->m_UpdateReportText(
               static_cast<QString>(C_GtGetText::h_GetText(" Signature valid: %1")).arg((rc_Info.c_Applications[
                                                                                            u8_Application].
                                                                                         u8_SignatureValid ==
                                                                                         0) ? "yes" : "no"));
            this->m_UpdateReportText(static_cast<QString>(C_GtGetText::h_GetText(" Additional information: %1")).arg(
                                        rc_Info.c_Applications[u8_Application].c_AdditionalInformation.c_str()));
         }

         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "Flashloader software version: V%1.%2r%3")).arg(
               QString::number(rc_Info.c_MoreInformation.au8_FlashloaderSoftwareVersion[0]).rightJustified(2, '0'),
               QString::number(rc_Info.c_MoreInformation.au8_FlashloaderSoftwareVersion[1]).rightJustified(2, '0'),
               QString::number(rc_Info.c_MoreInformation.au8_FlashloaderSoftwareVersion[2]).rightJustified(2, '0')));
         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "Flashloader protocol version: V%1.%2r%3")).arg(
               QString::number(rc_Info.c_MoreInformation.au8_FlashloaderProtocolVersion[0]).rightJustified(2, '0'),
               QString::number(rc_Info.c_MoreInformation.au8_FlashloaderProtocolVersion[1]).rightJustified(2, '0'),
               QString::number(rc_Info.c_MoreInformation.au8_FlashloaderProtocolVersion[2]).rightJustified(2, '0')));
         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText("Protocol version: V%1.%2r%3")).arg(
               QString::number(rc_Info.c_MoreInformation.au8_ProtocolVersion[0]).rightJustified(2, '0'),
               QString::number(rc_Info.c_MoreInformation.au8_ProtocolVersion[1]).rightJustified(2, '0'),
               QString::number(rc_Info.c_MoreInformation.au8_ProtocolVersion[2]).rightJustified(2, '0')));
         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText("Flash count: %1")).arg(rc_Info.c_MoreInformation.
                                                                                u32_FlashCount));
         if (rc_Info.c_MoreInformation.c_AvailableFeatures.q_ExtendedSerialNumberModeImplemented == false)
         {
            c_Temp = C_GtGetText::h_GetText("(Format: Standard)");
         }
         else
         {
            c_Temp = static_cast<QString>(C_GtGetText::h_GetText("(Format: Extended with Manufacturer Format %1)")).arg(
               QString::number(static_cast<uint32>(rc_Info.c_MoreInformation.c_SerialNumber.
                                                   u8_SerialNumberManufacturerFormat)));
         }
         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText("Device serial number: %1 %2")).arg(
               rc_Info.c_MoreInformation.GetEcuSerialNumber().c_str(), c_Temp));
         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText("Device article number: %1")).arg(rc_Info.
                                                                                          c_MoreInformation.
                                                                                          u32_EcuArticleNumber));
         this->m_UpdateReportText(static_cast<QString>(C_GtGetText::h_GetText("Device article version: %1")).arg(
                                     rc_Info.c_MoreInformation.c_EcuHardwareVersionNumber.c_str()));
         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText("Flash fingerprint date: %1-%2-%3 (yy-mm-dd)")).arg(
               QString::number(static_cast<uint32>(
                                  rc_Info.c_MoreInformation.au8_FlashFingerprintDate[0])).rightJustified(2, '0'),
               QString::number(static_cast<uint32>(
                                  rc_Info.c_MoreInformation.au8_FlashFingerprintDate[1])).rightJustified(2, '0'),
               QString::number(static_cast<uint32>(
                                  rc_Info.c_MoreInformation.au8_FlashFingerprintDate[2])).rightJustified(2, '0')));
         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText("Flash fingerprint time: %1:%2:%3")).arg(
               QString::number(static_cast<uint32>(rc_Info.c_MoreInformation.au8_FlashFingerprintTime[0])).
               rightJustified(2, '0'),
               QString::number(static_cast<uint32>(rc_Info.c_MoreInformation.au8_FlashFingerprintTime[1])).
               rightJustified(2, '0'),
               QString::number(static_cast<uint32>(rc_Info.c_MoreInformation.au8_FlashFingerprintTime[2])).
               rightJustified(2, '0')));
         this->m_UpdateReportText(static_cast<QString>(C_GtGetText::h_GetText("Flash fingerprint username: %1")).arg(
                                     rc_Info.c_MoreInformation.c_FlashFingerprintUserName.c_str()));

         // Information about available flashloader features
         this->m_UpdateReportText(static_cast<QString>(C_GtGetText::h_GetText("Available features:")));
         if (rc_Info.c_MoreInformation.c_AvailableFeatures.q_MaxNumberOfBlockLengthAvailable == true)
         {
            this->m_UpdateReportText(
               static_cast<QString>(C_GtGetText::h_GetText(
                                       " Maximum block size information available: 1 (%1 Bytes)")).arg(
                  QString::number(rc_Info.c_MoreInformation.u16_MaxNumberOfBlockLength)));
         }
         else
         {
            this->m_UpdateReportText(static_cast<QString>(C_GtGetText::h_GetText(
                                                             " Maximum block size information available: 0")));
         }
         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText(" NVM writing available: %1")).arg(
               QString::number(static_cast<uint8>(rc_Info.c_MoreInformation.c_AvailableFeatures.
                                                  q_FlashloaderCanWriteToNvm))));
         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText(" Security supported: %1")).arg(
               QString::number(static_cast<uint8>(rc_Info.c_MoreInformation.c_AvailableFeatures.
                                                  q_SupportsSecurity))));
         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText(" Disabling debugger supported: %1")).arg(
               QString::number(static_cast<uint8>(rc_Info.c_MoreInformation.c_AvailableFeatures.
                                                  q_SupportsDebuggerOff))));
         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText(" Enabling debugger supported: %1")).arg(
               QString::number(static_cast<uint8>(rc_Info.c_MoreInformation.c_AvailableFeatures.
                                                  q_SupportsDebuggerOn))));
         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText(
                                    " Ethernet2Ethernet routing supported: %1")).arg(
               QString::number(static_cast<uint8>(rc_Info.c_MoreInformation.c_AvailableFeatures.
                                                  q_EthernetToEthernetRoutingSupported))));
      }

      //Convert to same basic class
      c_Devices.reserve(c_DeviceInformation.size());
      for (uint32 u32_ItDevice = 0; u32_ItDevice < c_DeviceInformation.size(); ++u32_ItDevice)
      {
         C_SyvUpDeviceInfo c_NewDevice;
         const C_OSCSuSequences::C_OsyDeviceInformation & rc_Info = c_DeviceInformation[u32_ItDevice];
         c_NewDevice.pc_OSYDevice = &rc_Info;
         c_Devices.push_back(c_NewDevice);
      }

      // Check for problems
      this->m_CheckOpenSydeFlashloaderInformation(c_NodeIndexes, c_DeviceInformation);

      //Signal scene
      if (this->mpc_Scene != NULL)
      {
         this->mpc_Scene->UpdateDeviceInformation(c_NodeIndexes, c_Devices);
      }
      // Signal update package
      this->mpc_Ui->pc_WiUpdateInformation->UpdateDeviceInformation(c_NodeIndexes, c_Devices);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reporting slot for read STW flashloader information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_ReportStwFlashloaderInformationRead(void)
{
   if (this->mpc_UpSequences != NULL)
   {
      std::vector<uint32> c_NodeIndexes;
      std::vector<C_OSCSuSequences::C_XflDeviceInformation> c_DeviceInformation;
      std::vector<C_SyvUpDeviceInfo> c_Devices;
      uint32 u32_Counter;

      this->mpc_UpSequences->GetXflDeviceInformation(c_NodeIndexes, c_DeviceInformation);

      for (u32_Counter = 0U; u32_Counter < c_NodeIndexes.size(); ++u32_Counter)
      {
         C_SCLStringList c_Strings;

         this->m_UpdateReportText(
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "STW Flashloader device information found for node with index %1")).arg(
               c_NodeIndexes[u32_Counter]));

         C_OSCSuSequences::h_StwFlashloaderInformationToText(c_DeviceInformation[u32_Counter], c_Strings);

         for (uint32 u32_StringIndex = 0U; u32_StringIndex < c_Strings.GetCount(); u32_StringIndex++)
         {
            this->m_UpdateReportText(c_Strings.Strings[u32_StringIndex].c_str());
         }
      }

      //Convert to same basic class
      c_Devices.reserve(c_DeviceInformation.size());
      for (uint32 u32_ItDevice = 0; u32_ItDevice < c_DeviceInformation.size(); ++u32_ItDevice)
      {
         C_SyvUpDeviceInfo c_NewDevice;
         const C_OSCSuSequences::C_XflDeviceInformation & rc_Info = c_DeviceInformation[u32_ItDevice];
         c_NewDevice.pc_STWDevice = &rc_Info;
         c_Devices.push_back(c_NewDevice);
      }

      //Signal scene
      if (this->mpc_Scene != NULL)
      {
         this->mpc_Scene->UpdateDeviceInformation(c_NodeIndexes, c_Devices);
      }
      // Signal update package
      this->mpc_Ui->pc_WiUpdateInformation->UpdateDeviceInformation(c_NodeIndexes, c_Devices);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check the read openSYDE Flashloader information for problems

   Checks:
   - If a NVM write is necessary, the available Flashloader will be checked

   \param[in]  orc_OsyNodeIndexes         All node indexes
   \param[in]  orc_OsyDeviceInformation   All device information associated to the node index in the same order
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_CheckOpenSydeFlashloaderInformation(const std::vector<uint32> & orc_OsyNodeIndexes,
                                                                const std::vector<C_OSCSuSequences::C_OsyDeviceInformation> & orc_OsyDeviceInformation)
{
   uint32 u32_NodesInformationCounter;

   tgl_assert(orc_OsyNodeIndexes.size() == orc_OsyDeviceInformation.size());

   for (u32_NodesInformationCounter = 0U; u32_NodesInformationCounter < orc_OsyNodeIndexes.size();
        ++u32_NodesInformationCounter)
   {
      uint32 u32_NodesPackageCounter;
      // u32_NodesPackageCounter equals node index in system definition
      for (u32_NodesPackageCounter = 0U; u32_NodesPackageCounter < this->mc_NodesToFlash.size();
           ++u32_NodesPackageCounter)
      {
         if (orc_OsyNodeIndexes[u32_NodesInformationCounter] == u32_NodesPackageCounter)
         {
            // Same node index
            const C_OSCSuSequences::C_OsyDeviceInformation & rc_Info =
               orc_OsyDeviceInformation[u32_NodesInformationCounter];
            const C_OSCNode * const pc_Node =
               C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodesPackageCounter);
            stw_scl::C_SCLString c_NodeName;

            if (pc_Node != NULL)
            {
               c_NodeName = pc_Node->c_Properties.c_Name;
            }

            if (this->mc_NodesToFlash[u32_NodesPackageCounter].c_FilesToWriteToNvm.size() > 0)
            {
               // Node must have features for NVM writing in Flashloader
               if ((rc_Info.c_MoreInformation.c_AvailableFeatures.q_FlashloaderCanWriteToNvm == false) ||
                   (rc_Info.c_MoreInformation.c_AvailableFeatures.q_MaxNumberOfBlockLengthAvailable == false))
               {
                  // Node does not have the ability
                  this->mq_NodesPreconditionError = true;
                  this->mc_NodePreconditionErrors.c_NvmWriteError.push_back(u32_NodesPackageCounter);

                  osc_write_log_error("Update Node",
                                      "Node " + c_NodeName +
                                      " has not the Flashloader features to write a Parameter Set Image file.");
               }
            }

            if (this->mc_NodesToFlash[u32_NodesPackageCounter].c_PemFile != "")
            {
               // Node must have features for PEM writing in Flashloader
               if (rc_Info.c_MoreInformation.c_AvailableFeatures.q_SupportsSecurity == false)
               {
                  // Node does not have the ability
                  this->mq_NodesPreconditionError = true;
                  this->mc_NodePreconditionErrors.c_PemWriteError.push_back(u32_NodesPackageCounter);

                  osc_write_log_error(
                     "Update Node",
                     "Node " + c_NodeName +
                     " has not the Flashloader features to support security and to write PEM files.");
               }

               if (this->mc_NodesToFlash[u32_NodesPackageCounter].q_SendDebuggerEnabledState == true)
               {
                  if ((rc_Info.c_MoreInformation.c_AvailableFeatures.q_SupportsDebuggerOn == false) &&
                      (this->mc_NodesToFlash[u32_NodesPackageCounter].q_DebuggerEnabled == true))
                  {
                     // Node does not have the ability
                     this->mq_NodesPreconditionError = true;
                     this->mc_NodePreconditionErrors.c_DebuggerEnableError.push_back(u32_NodesPackageCounter);

                     osc_write_log_error(
                        "Update Node",
                        "Node " + c_NodeName +
                        " has not the Flashloader features to support enabling the debugger.");
                  }

                  if ((rc_Info.c_MoreInformation.c_AvailableFeatures.q_SupportsDebuggerOff == false) &&
                      (this->mc_NodesToFlash[u32_NodesPackageCounter].q_DebuggerEnabled == false))
                  {
                     // Node does not have the ability
                     this->mq_NodesPreconditionError = true;
                     this->mc_NodePreconditionErrors.c_DebuggerDisableError.push_back(u32_NodesPackageCounter);

                     osc_write_log_error(
                        "Update Node",
                        "Node " + c_NodeName +
                        " has not the Flashloader features to support disabling the debugger.");
                  }
               }
            }

            if ((rc_Info.c_MoreInformation.c_AvailableFeatures.q_EthernetToEthernetRoutingSupported == false) &&
                (this->mpc_UpSequences->IsEthToEthRoutingNecessary(u32_NodesPackageCounter) == true))
            {
               // Node does not support Ethernet to Ethernet routing but it is necessary for at least one route
               this->mq_NodesPreconditionError = true;
               this->mc_NodePreconditionErrors.c_EthToEthError.push_back(u32_NodesPackageCounter);

               osc_write_log_error("Update Node",
                                   "Node " + c_NodeName +
                                   " has not the Flashloader features for Ethernet to Ethernet routing.");
            }

            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start connecting to nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_Connect(void)
{
   sint32 s32_Return;

   QApplication::setOverrideCursor(Qt::WaitCursor);

   //Buttons
   this->mpc_Ui->pc_PbConnect->setEnabled(false);
   this->mpc_Ui->pc_PbUpdate->setEnabled(false);

   if (this->mpc_ProgressLogContent != NULL)
   {
      this->mpc_ProgressLogContent->Clear();

      if (this->mq_StartUpdateAfterConnect == false)
      {
         this->mpc_ProgressLogContent->SetHeading(C_GtGetText::h_GetText("Enter Update Mode"));
      }
      // Let the user know, when the tool is occupied with itself before communicating with the bus
      this->mpc_ProgressLogContent->AddSubHeading(C_GtGetText::h_GetText("Initializing communication interface..."));
      this->mpc_ProgressLogContent->update();
      // For updating the progress log
      QApplication::processEvents();
   }

   // Is a new connect already possible
   while ((this->mu32_DisconnectTime + mhu32_WAIT_TIME) > TGL_GetTickCount())
   {
      // Wait till it is possible
   }

   s32_Return = this->m_InitSequence();

   //Always reset flag
   this->mq_ConnectFailed = false;
   this->mq_NodesPreconditionError = false;
   this->mc_NodePreconditionErrors.Clear();

   if (s32_Return == C_NO_ERR)
   {
      //Signal summary
      this->mpc_Ui->pc_WiUpdateInformation->SetHeading("", C_GtGetText::h_GetText("Entering update mode..."));
      //Signal scene
      if (this->mpc_Scene != NULL)
      {
         //Reset connected status (required if in system disconnected state due to no node response)
         this->mpc_Scene->SetConnected(false);
         this->mpc_Scene->SetConnected(true);
         this->mpc_Scene->StartConnectionAnimation();
      }

      // And Update package. After this call the timer will be disabled
      this->mpc_Ui->pc_WiUpdateInformation->SetConnected();

      // Due to the timer of the update package and the signal used, timing could be a problem.
      // Let the update package timer and its signal finish in any case and deactivate the buttons again just in case
      QApplication::processEvents();
      this->mpc_Ui->pc_PbConnect->setEnabled(false);
      this->mpc_Ui->pc_PbUpdate->setEnabled(false);

      // When connected, no drag and drop is allowed on top level
      Q_EMIT (this->SigBlockDragAndDrop(true));

      // Prepare the update package
      this->mc_NodesToFlash.clear();
      this->mc_NodesOrder.clear();
      this->mc_NodesWithAllApplications.clear();
      this->mc_NodesWithAllApplicationsAndTempPath.clear();
      // Get the update package with the update configuration
      s32_Return = this->mpc_Ui->pc_WiUpdateInformation->GetUpdatePackage(this->mc_NodesToFlash,
                                                                          this->mc_NodesOrder,
                                                                          &this->mc_NodesWithAllApplications);

      if (s32_Return == C_NO_ERR)
      {
         if (C_SyvUpUpdateWidget::mh_IsSecurityWarningNecessary(this->mc_NodesToFlash))
         {
            C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eWARNING);
            C_OgeWiCustomMessage::E_Outputs e_Output;
            c_MessageBox.SetHeading(C_GtGetText::h_GetText("Transferring PEM file"));
            c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                           "The Update Package includes PEM file(s). After transferring the public certificate and activating the security flag on the target,\n"
                                           "access is only possible with a valid private key.\n"
                                           "\n"
                                           "Are you sure you want to continue?\n"));
            c_MessageBox.SetCustomMinHeight(240, 240);
            c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Continue"));
            c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Cancel"));
            e_Output = c_MessageBox.Execute();
            switch (e_Output)
            {
            case C_OgeWiCustomMessage::eYES:
               //Continue
               break;
            case C_OgeWiCustomMessage::eCANCEL:
            case C_OgeWiCustomMessage::eNO:
            default:
               //Do not continue
               s32_Return = C_RANGE;
               break;
            }
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         QString c_ErrorPath;
         const C_SCLString c_ExePath = C_Uti::h_GetExePath().toStdString().c_str();
         const C_SCLString c_TemporaryPath = c_ExePath + "/" + mhc_TEMP_FOLDER.toStdString().c_str() + "/";

         // Copy the application paths
         // Saving both, the original and temporary, paths will help to map changed relevant applications
         // between connect and update by discarding application information blocks
         this->mc_NodesWithAllApplicationsAndTempPath = this->mc_NodesWithAllApplications;

         // Copy all files to a temporary folder to have them "safe"
         s32_Return = this->mpc_UpSequences->SyvUpCreateTemporaryFolder(c_TemporaryPath,
                                                                        this->mc_NodesWithAllApplicationsAndTempPath,
                                                                        c_ErrorPath);

         // Adapt paths of mc_NodesToFlash to the temporary folder
         this->m_ReplaceOriginalWithTempPaths();

         if (s32_Return != C_NO_ERR)
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
            QString c_MessageText;
            QString c_CompleteMessageText;
            switch (s32_Return)
            {
            case C_CONFIG:
               c_MessageText = C_GtGetText::h_GetText("View is invalid or initialization was not finished.");
               break;
            case C_OVERFLOW:
               c_MessageText =
                  C_GtGetText::h_GetText("Number of applications to write or number of active nodes is invalid.");
               break;
            case C_NOACT:
               c_MessageText =
                  C_GtGetText::h_GetText("Update package contains at least one file for an inactive node or number of "
                                         "files does not match number of Data Blocks.");
               break;
            case C_RANGE:
               c_MessageText = C_GtGetText::h_GetText("File does not exist or target path does not "
                                                      "end in \"\\\" or \"/\": \n") + c_ErrorPath;
               break;
            case C_BUSY:
               c_MessageText = C_GtGetText::h_GetText("Could not erase pre-existing target path "
                                                      "(can result in partially erased target path):\n") + c_ErrorPath;
               break;
            case C_RD_WR:
               c_MessageText =  C_GtGetText::h_GetText("Could not copy file:\n") + c_ErrorPath;
               break;
            case C_TIMEOUT:
               c_MessageText =  C_GtGetText::h_GetText("Could not create target directory:\n") + c_ErrorPath;
               break;
            default:
               c_MessageText =
                  static_cast<QString>(C_GtGetText::h_GetText("Unknown: %1")).arg(C_Uti::h_StwError(s32_Return));
               break;
            }
            c_CompleteMessageText =
               static_cast<QString>(C_GtGetText::h_GetText("Enter update mode: Error on creating temporary"
                                                           " folder: %1")).arg(c_MessageText);
            this->m_UpdateReportText(c_CompleteMessageText);
            c_Message.SetHeading(C_GtGetText::h_GetText("System Update"));
            c_Message.SetDescription("Enter update mode: Error on creating temporary folder.");
            c_Message.SetDetails(c_MessageText);
            c_Message.SetCustomMinHeight(180, 250);
            c_Message.Execute();
         }
         else
         {
            this->mpc_Ui->pc_WiUpdateInformation->InitUpdatePackage(this->mc_NodesToFlash, this->mc_NodesOrder,
                                                                    m_GetIsFileBasedFlagForEach(), false);
         }
      }
      else if (s32_Return == C_NOACT)
      {
         //nothing to flash: this is not a connection problem
         s32_Return = C_NO_ERR;
      }
      else
      {
         this->m_UpdateReportText(static_cast<QString>(C_GtGetText::h_GetText(
                                                          "Activate Flashloader: Cannot start. Error on creation temporary files: %1")).
                                  arg(C_Uti::h_StwError(s32_Return)));
      }

      if (s32_Return == C_NO_ERR)
      {
         // Start the connect sequence
         this->m_UpdateReportText(C_GtGetText::h_GetText("Activate Flashloader: Started"));

         s32_Return = this->mpc_UpSequences->StartActivateFlashloader();

         if (s32_Return == C_NO_ERR)
         {
            //Signal progress log
            if (this->mpc_ProgressLogContent != NULL)
            {
               // The thread was started, the communication is running...
               this->mpc_ProgressLogContent->AddSubHeading(C_GtGetText::h_GetText("Entering update mode..."));
            }

            //Don't set step if timer still active
            this->me_Step = C_SyvUpSequences::eACTIVATE_FLASHLOADER;
            //Timer
            this->mc_Timer.start();
            //Signal scene
            if (this->mpc_Scene != NULL)
            {
               this->mpc_Scene->SetConnecting(true);
            }
         }
         else
         {
            //Thread access possible
            this->m_UpdateReportText(static_cast<QString>(
                                        C_GtGetText::h_GetText(
                                           "Activate Flashloader: Cannot start. Error on initialization: %1")).
                                     arg(C_Uti::h_StwError(s32_Return)));
         }
      }

      if (s32_Return != C_NO_ERR)
      {
         // Handle failure
         this->mq_StartUpdateAfterConnect = false;

         //Signal summary
         this->mpc_Ui->pc_WiUpdateInformation->SetHeading("", C_GtGetText::h_GetText("Disconnected!"));
         this->mpc_Ui->pc_WiUpdateInformation->SetDisconnected();

         //Signal scene
         if (this->mpc_Scene != NULL)
         {
            // Stop the previous animation first
            this->mpc_Scene->StopProgressAnimation(false, 0, true);
         }
      }
   }
   else
   {
      this->m_UpdateReportText(static_cast<QString>(
                                  C_GtGetText::h_GetText(
                                     "Activate Flashloader: Cannot start. Error on initialization: %1")).
                               arg(C_Uti::h_StwError(s32_Return)));
   }

   if (s32_Return != C_NO_ERR)
   {
      //Signal progress log
      if (this->mpc_ProgressLogContent != NULL)
      {
         this->mpc_ProgressLogContent->AddSubHeading(C_GtGetText::h_GetText("Enter Update Mode failed!"));
         this->mpc_ProgressLogContent->AddLogHyperlink();
      }

      this->mpc_Ui->pc_PbUpdate->setEnabled(true);
      this->mpc_Ui->pc_PbConnect->setEnabled(true);
   }

   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start updating of nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_Update(void)
{
   if ((this->mq_Connected == true) &&
       (this->mq_ConnectFailed == true))
   {
      // A connect was done and it failed
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("System Update"));
      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "Update process cannot be started. There are active nodes, which are not responding. \n"
                                  "Check the nodes connection and try again."));
      c_Message.SetCustomMinHeight(180, 180);
      c_Message.SetCustomMinWidth(650);
      c_Message.Execute();
   }
   else
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (this->mq_Connected == true)
      {
         // Reset summary
         this->mpc_Ui->pc_WiUpdateInformation->ResetSummary();
         this->mpc_Ui->pc_WiUpdateInformation->SetHeading("", C_GtGetText::h_GetText("Update mode active!"));
         if (this->mpc_UpSequences != NULL)
         {
            QApplication::setOverrideCursor(Qt::WaitCursor);

            if (pc_View != NULL)
            {
               sint32 s32_Return;

               this->m_UpdateReportText(C_GtGetText::h_GetText("Update System started"));

               // Is a new update already possible
               while ((this->mu32_UpdateTime + mhu32_WAIT_TIME) > TGL_GetTickCount())
               {
                  // Wait till it is possible
               }

               // Get the state of the update package
               // Prepare the update package
               this->mc_NodesToFlash.clear();
               this->mc_NodesOrder.clear();
               // Get the update package with the update configuration
               s32_Return = this->mpc_Ui->pc_WiUpdateInformation->GetUpdatePackage(this->mc_NodesToFlash,
                                                                                   this->mc_NodesOrder);

               // Adapt paths of mc_NodesToFlash to the temporary folder
               this->m_ReplaceOriginalWithTempPaths();

               if (s32_Return == C_NOACT)
               {
                  // No applications to update
                  C_OgeWiCustomMessage c_Message(this);
                  c_Message.SetHeading(C_GtGetText::h_GetText("System Update"));
                  c_Message.SetDescription(C_GtGetText::h_GetText(
                                              "No System Update needed. All applications for the devices match the ones in the "
                                              "Update Package. To update anyway see details."));
                  c_Message.SetDetails(C_GtGetText::h_GetText(
                                          "For updating a device nevertheless, there is an option to trigger a \"Force Update\" "
                                          "in the \"Device Status Information\" dialog (double click on node)."));
                  c_Message.SetCustomMinHeight(230, 270);
                  c_Message.Execute();
               }
               else if (s32_Return != C_NO_ERR)
               {
                  m_HandleUpdateFailure();
               }
               else
               {
                  // Nothing to do
               }

               if (s32_Return == C_NO_ERR)
               {
                  this->me_Step = C_SyvUpSequences::eUPDATE_SYSTEM;
                  s32_Return = this->mpc_UpSequences->StartUpdateSystem(this->mc_NodesToFlash, this->mc_NodesOrder);

                  if (s32_Return == C_NO_ERR)
                  {
                     this->mpc_Ui->pc_PbDisconnnect->setEnabled(false);
                     this->mpc_Ui->pc_PbUpdate->setVisible(false);
                     this->mpc_Ui->pc_PbCancel->setVisible(true);
                     this->mpc_Ui->pc_PbCancel->setEnabled(true);
                     this->mc_Timer.start();
                     //Signal scene
                     if (this->mpc_Scene != NULL)
                     {
                        this->mpc_Scene->SetUpdating(true);
                     }
                     // And update package
                     this->mpc_Ui->pc_WiUpdateInformation->SetUpdateStarted();

                     //Signal progress log
                     if (this->mpc_ProgressLogContent != NULL)
                     {
                        this->mpc_ProgressLogContent->SetHeading(C_GtGetText::h_GetText("System Update"));
                     }
                     if (this->mq_ClearProgressLog == true)
                     {
                        if (this->mpc_ProgressLogContent != NULL)
                        {
                           this->mpc_ProgressLogContent->Clear();
                        }
                     }
                     else
                     {
                        this->mq_ClearProgressLog = true;
                        if (this->mpc_ProgressLogContent != NULL)
                        {
                           this->mpc_ProgressLogContent->AddSpacer();
                        }
                     }
                     if (this->mpc_ProgressLogContent != NULL)
                     {
                        this->mpc_ProgressLogContent->AddSubHeading(C_GtGetText::h_GetText("Updating System..."));
                     }
                     //Signal summary
                     //Reinit necessary if abort and update again
                     this->mpc_Ui->pc_WiUpdateInformation->InitUpdatePackage(this->mc_NodesToFlash, this->mc_NodesOrder,
                                                                             m_GetIsFileBasedFlagForEach(), true);
                     this->mpc_Ui->pc_WiUpdateInformation->SetHeading("", C_GtGetText::h_GetText("Updating System..."));
                     this->mpc_Ui->pc_WiUpdateInformation->StartElapsedTimer();
                  }
                  else
                  {
                     this->m_UpdateReportText(static_cast<QString>(
                                                 C_GtGetText::h_GetText(
                                                    "Update System: Cannot start. Thread is still busy.")).
                                              arg(C_Uti::h_StwError(s32_Return)));

                     m_HandleUpdateFailure();
                  }
               }
            }
            else
            {
               this->m_UpdateReportText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                "Update System: Cannot start. Error on System View.")));

               m_HandleUpdateFailure();
            }

            QApplication::restoreOverrideCursor();
         }
      }
      else
      {
         //Signal progress log
         if (this->mpc_ProgressLogContent != NULL)
         {
            this->mpc_ProgressLogContent->SetHeading(C_GtGetText::h_GetText("System Update"));
         }
         // Connect in the first step and start the update after the successful connecting sequence
         this->mq_StartUpdateAfterConnect = true;
         this->m_Connect();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger system reset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_Disconnect(void)
{
   m_DisconnectAction();
   //Always reset flag
   this->mq_ConnectFailed = false;
   this->mq_NodesPreconditionError = false;
   this->mc_NodePreconditionErrors.Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resets the system
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_DisconnectAction(void)
{
   if (this->mpc_UpSequences != NULL)
   {
      sint32 s32_Return;

      this->me_Step = C_SyvUpSequences::eRESET_SYSTEM;
      s32_Return = this->mpc_UpSequences->StartResetSystem();

      if (s32_Return == C_NO_ERR)
      {
         //Buttons
         this->mpc_Ui->pc_PbDisconnnect->setEnabled(false);
         this->mpc_Ui->pc_PbConnect->setEnabled(false);
         this->mpc_Ui->pc_PbUpdate->setVisible(true);
         this->mpc_Ui->pc_PbUpdate->setEnabled(false);

         this->mc_Timer.start();

         this->m_UpdateReportText(C_GtGetText::h_GetText("Disconnect: Start"));

         //Signal scene
         if (this->mpc_Scene != NULL)
         {
            this->mpc_Scene->SetConnected(false);
         }
         //Signal log
         if (this->mpc_ProgressLogContent != NULL)
         {
            this->mpc_ProgressLogContent->Clear();
         }

         //Signal summary
         this->mpc_Ui->pc_WiUpdateInformation->ResetSummary();
         this->mpc_Ui->pc_WiUpdateInformation->SetHeading("", C_GtGetText::h_GetText("Exiting Update Mode..."));
      }
      else
      {
         this->m_UpdateReportText(static_cast<QString>(C_GtGetText::h_GetText(
                                                          "Disconnect: Cannot start. Thread is still busy.")));
         this->m_CleanUpSequence();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Aborts the current sequence
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_Cancel(void)
{
   if (this->mpc_UpSequences != NULL)
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eQUESTION);

      C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;

      c_MessageBox.SetHeading(C_GtGetText::h_GetText("System Update"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                     "System Update is running. Do you really want to abort the current System Update?"));
      c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Continue Update"));
      c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Abort Update"));
      c_MessageBox.SetCustomMinWidth(650);
      c_MessageBox.SetCustomMinHeight(180, 180);
      e_ReturnMessageBox = c_MessageBox.Execute();

      if (e_ReturnMessageBox == C_OgeWiCustomMessage::eNO)
      {
         this->mpc_UpSequences->AbortCurrentProgress();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_Timer(void)
{
   sint32 s32_Result = C_CONFIG;
   sint32 s32_SequenceResult;
   QString c_Message = "";

   if (this->mpc_UpSequences != NULL)
   {
      s32_Result = this->mpc_UpSequences->GetResults(s32_SequenceResult);
   }

   if (s32_Result == C_NO_ERR)
   {
      // In the event queue could be some emitted signals from thread left.
      // We need all information for showing the result
      QApplication::processEvents();

      // Thread finished
      this->mc_Timer.stop();

      switch (this->me_Step)
      {
      case C_SyvUpSequences::eACTIVATE_FLASHLOADER:
         if (s32_SequenceResult == C_NO_ERR)
         {
            // First step of two steps finished.
            // Start the next one.
            this->me_Step = C_SyvUpSequences::eREAD_DEVICE_INFORMATION;
            s32_Result = this->mpc_UpSequences->StartReadDeviceInformation();

            if (s32_Result == C_NO_ERR)
            {
               this->m_UpdateReportText(C_GtGetText::h_GetText("Read Device information: Start"));

               this->mc_Timer.start();
            }
            else
            {
               this->m_UpdateReportText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                "Read Device information: Cannot start. Thread is still busy.")));
            }
         }
         else
         {
            std::vector<stw_opensyde_core::C_OSCSuSequencesNodeConnectStates> c_NodeStates;

            // Sequence failed
            this->me_Step = C_SyvUpSequences::eNOT_ACTIVE;
            this->mq_StartUpdateAfterConnect = false;

            this->mpc_Ui->pc_PbUpdate->setVisible(true);
            this->mpc_Ui->pc_PbUpdate->setEnabled(true);
            this->mpc_Ui->pc_PbConnect->setEnabled(true);

            // Get the connect status of the nodes
            tgl_assert(this->mpc_UpSequences->GetConnectStates(c_NodeStates) == C_NO_ERR);
            Q_EMIT (this->SigNodeConnectStates(c_NodeStates, this->mc_NodePreconditionErrors));

            m_HandleConnectionFailure();
         }
         break;
      case C_SyvUpSequences::eREAD_DEVICE_INFORMATION:
         // Sequence finished
         this->me_Step = C_SyvUpSequences::eNOT_ACTIVE;
         {
            // Get the connect status of the nodes and inform the UI
            std::vector<stw_opensyde_core::C_OSCSuSequencesNodeConnectStates> c_NodeStates;

            tgl_assert(this->mpc_UpSequences->GetConnectStates(c_NodeStates) == C_NO_ERR);
            Q_EMIT (this->SigNodeConnectStates(c_NodeStates, this->mc_NodePreconditionErrors));
         }

         this->mpc_Ui->pc_PbUpdate->setVisible(true);
         this->mpc_Ui->pc_PbDisconnnect->setEnabled(true);

         if ((s32_SequenceResult == C_NO_ERR) &&
             (this->mq_NodesPreconditionError == false))
         {
            this->mq_Connected = true;

            this->mpc_Ui->pc_PbConnect->setEnabled(false);
            this->mpc_Ui->pc_PbUpdate->setEnabled(true);

            //Signal summary
            this->mpc_Ui->pc_WiUpdateInformation->SetHeading("", C_GtGetText::h_GetText("Update mode active!"));
            //Signal scene
            if (this->mpc_Scene != NULL)
            {
               // Stop the previous animation first
               this->mpc_Scene->StopProgressAnimation(false, 0, true);
            }

            //Signal progress log
            if (this->mpc_ProgressLogContent != NULL)
            {
               this->mpc_ProgressLogContent->AddSubHeading(C_GtGetText::h_GetText("\nUpdate mode activated."));
            }

            if (this->mq_StartUpdateAfterConnect == true)
            {
               // In the event queue could be some emitted signals from thread left.
               // We need all information for showing the result before starting the update
               QApplication::processEvents();

               this->mq_ClearProgressLog = false;
               this->m_Update();
            }
            else
            {
               // Only necessary if not directly updating because updating will do this step automatically
               // Update the state of the update package (now includes the device status)
               // Prepare the update package
               this->mc_NodesToFlash.clear();
               this->mc_NodesOrder.clear();
               // Get the update package with the update configuration
               this->mpc_Ui->pc_WiUpdateInformation->GetUpdatePackage(this->mc_NodesToFlash,
                                                                      this->mc_NodesOrder);
               this->mpc_Ui->pc_WiUpdateInformation->InitUpdatePackage(this->mc_NodesToFlash, this->mc_NodesOrder,
                                                                       m_GetIsFileBasedFlagForEach(), true);
            }
         }
         else
         {
            this->mq_ConnectFailed = true;
            this->mpc_Ui->pc_PbUpdate->setEnabled(false);

            // Show the original message box only in the case of a sequence error
            m_HandleConnectionFailure((s32_SequenceResult == C_NO_ERR) ? this->mq_NodesPreconditionError : false);

            // Show a message box in the special case
            // with a not capable node for NVM/PEM writing or Ethernet routing
            if (this->mq_NodesPreconditionError == true)
            {
               // Error for NVM writing
               if (this->mc_NodePreconditionErrors.c_NvmWriteError.size() > 0)
               {
                  this->m_HandlePreconditionErrorType(
                     this->mc_NodePreconditionErrors.c_NvmWriteError,
                     C_GtGetText::h_GetText(
                        "Enter update mode failed.\n"
                        "There are nodes, which do not support writing"
                        " parameter set image files."),
                     C_GtGetText::h_GetText(
                        "Following node(s) do not support writing parameter set image files:\n"));
               }

               // Error for PEM writing
               if (this->mc_NodePreconditionErrors.c_PemWriteError.size() > 0)
               {
                  this->m_HandlePreconditionErrorType(
                     this->mc_NodePreconditionErrors.c_PemWriteError,
                     C_GtGetText::h_GetText(
                        "Enter update mode failed.\n"
                        "There are nodes, which do not support security"
                        " and writing PEM files."),
                     C_GtGetText::h_GetText(
                        "Following node(s) do not support security and writing PEM files:\n"));
               }

               // Error for debugger enabling
               if (this->mc_NodePreconditionErrors.c_DebuggerEnableError.size() > 0)
               {
                  this->m_HandlePreconditionErrorType(
                     this->mc_NodePreconditionErrors.c_DebuggerEnableError,
                     C_GtGetText::h_GetText(
                        "Enter update mode failed.\n"
                        "There are nodes, which do not support"
                        " enabling the debugger."),
                     C_GtGetText::h_GetText(
                        "Following node(s) do not support enabling the debugger:\n"));
               }

               // Error for debugger disabling
               if (this->mc_NodePreconditionErrors.c_DebuggerDisableError.size() > 0)
               {
                  this->m_HandlePreconditionErrorType(
                     this->mc_NodePreconditionErrors.c_DebuggerDisableError,
                     C_GtGetText::h_GetText(
                        "Enter update mode failed.\n"
                        "There are nodes, which do not support"
                        " disabling the debugger."),
                     C_GtGetText::h_GetText(
                        "Following node(s) do not support disabling the debugger:\n"));
               }

               // Error for Ethernet to Ethernet routing
               if (this->mc_NodePreconditionErrors.c_EthToEthError.size() > 0)
               {
                  this->m_HandlePreconditionErrorType(
                     this->mc_NodePreconditionErrors.c_EthToEthError,
                     C_GtGetText::h_GetText(
                        "Enter update mode failed.\n"
                        "There are nodes, which do not support Ethernet"
                        " to Ethernet routing."),
                     C_GtGetText::h_GetText(
                        "Following node(s) do not support Ethernet to Ethernet routing:\n"));
               }
            }
         }

         this->mq_StartUpdateAfterConnect = false;
         break;
      case C_SyvUpSequences::eUPDATE_SYSTEM:
         // Sequence finished
         this->me_Step = C_SyvUpSequences::eNOT_ACTIVE;

         {
            // Get the connect status of the nodes and inform the UI
            std::vector<stw_opensyde_core::C_OSCSuSequencesNodeUpdateStates> c_NodeStates;

            tgl_assert(this->mpc_UpSequences->GetUpdateStates(c_NodeStates) == C_NO_ERR);
            Q_EMIT (this->SigNodeUpdateStates(c_NodeStates));
         }

         this->mpc_Ui->pc_PbDisconnnect->setEnabled(true);
         this->mpc_Ui->pc_PbUpdate->setVisible(true);
         this->mpc_Ui->pc_PbUpdate->setEnabled(true);
         this->mpc_Ui->pc_PbCancel->setVisible(false);

         // User aborted sequence
         switch (s32_SequenceResult)
         {
         case C_NO_ERR:
            c_Message = C_GtGetText::h_GetText("Update successful.");
            break;
         case C_CONFIG:
            c_Message = C_GtGetText::h_GetText(
               "Initialization of update mechanism invalid or no signature block found in file.");
            break;
         case C_OVERFLOW:
            c_Message = C_GtGetText::h_GetText("Node configuration is invalid.");
            break;
         case C_NOACT:
            c_Message = C_GtGetText::h_GetText("File configuration is invalid.");
            break;
         case C_RD_WR:
            c_Message = C_GtGetText::h_GetText("File does not exist.");
            break;
         case C_COM:
            c_Message = C_GtGetText::h_GetText("Communication problem.");
            break;
         case C_BUSY:
            c_Message = C_GtGetText::h_GetText("User aborted the update.");
            break;
         case C_WARN:
            c_Message = C_GtGetText::h_GetText("A PEM file could be extracted.");
            break;
         case C_DEFAULT:
            c_Message = C_GtGetText::h_GetText("At least one of the *.syde_psi file has an invalid checksum.");
            break;
         case C_RANGE:
            c_Message = C_GtGetText::h_GetText(
               "At least one necessary feature of the openSYDE Flashloader is not available for NVM writing.");
            break;
         case C_CHECKSUM:
            c_Message = C_GtGetText::h_GetText(
               "Authentication between openSYDE Tool and device(s) has failed. Access denied.");
            break;
         default:
            c_Message = C_GtGetText::h_GetText("Unknown error occurred.");
            break;
         }

         if (s32_SequenceResult != C_NO_ERR)
         {
            uint32 u32_ErrorNodeIndex;
            uint32 u32_ErrorFileIndex;
            uint32 u32_Position;

            // Adapt the update configuration. Remove all applications which are updated successfully
            // in case of a retry without new connect
            if (this->mpc_UpSequences->GetLastUpdatePosition(u32_ErrorNodeIndex, u32_ErrorFileIndex) == C_NO_ERR)
            {
               // Remove all applications of the finished nodes
               for (u32_Position = 0U; u32_Position < this->mc_NodesOrder.size(); u32_Position++)
               {
                  const uint32 u32_NodeIndex = this->mc_NodesOrder[u32_Position];

                  if (u32_NodeIndex != u32_ErrorNodeIndex)
                  {
                     // Previous and finished node
                     if (u32_NodeIndex < this->mc_NodesToFlash.size())
                     {
                        this->mc_NodesToFlash[u32_NodeIndex].c_FilesToFlash.clear();
                     }
                  }
                  else
                  {
                     // Error node
                     break;
                  }
               }

               // Remove the finished nodes from the order
               this->mc_NodesOrder.erase(this->mc_NodesOrder.begin(), this->mc_NodesOrder.begin() + u32_Position);

               // Remove the applications before the conflicted file of the same node
               if ((u32_ErrorFileIndex > 0U) &&
                   (u32_ErrorNodeIndex < this->mc_NodesToFlash.size()) &&
                   (u32_ErrorFileIndex < this->mc_NodesToFlash[u32_ErrorNodeIndex].c_FilesToFlash.size()))
               {
                  this->mc_NodesToFlash[u32_ErrorNodeIndex].
                  c_FilesToFlash.erase(this->mc_NodesToFlash[u32_ErrorNodeIndex].c_FilesToFlash.begin(),
                                       this->mc_NodesToFlash[u32_ErrorNodeIndex].c_FilesToFlash.begin() +
                                       u32_ErrorFileIndex);
               }
            }

            //Signal other widgets (signals might be missing)
            this->mpc_Ui->pc_WiUpdateInformation->SetUpdateApplicationError(u32_ErrorNodeIndex);
            //Signal progress log
            if (this->mpc_ProgressLogContent != NULL)
            {
               this->mpc_ProgressLogContent->UpdateStatus(u32_ErrorNodeIndex, C_GtGetText::h_GetText("FAIL"), true);
            }
            //Signal scene
            if (this->mpc_Scene != NULL)
            {
               this->mpc_Scene->SetNodeError(u32_ErrorNodeIndex);
               this->mpc_Scene->StopProgressAnimation(true, this->mu32_ApplicationIndex, true);
            }
            // Need to be written before m_HandleUpdateFailure due to the link in the message box
            this->m_UpdateReportText(static_cast<QString>(C_GtGetText::h_GetText("Update System: %1")).arg(c_Message));

            m_HandleUpdateFailure();
         }
         else
         {
            this->m_UpdateReportText(static_cast<QString>(C_GtGetText::h_GetText("Update System: %1")).arg(c_Message));
         }

         this->mpc_Ui->pc_WiUpdateInformation->SetUpdateFinished();
         //Update estimated time
         this->m_UpdateUpdatePackageStatus();

         // Save the time of update
         this->mu32_UpdateTime = TGL_GetTickCount();

         break;
      case C_SyvUpSequences::eRESET_SYSTEM:
         // Sequence finished
         this->me_Step = C_SyvUpSequences::eNOT_ACTIVE;

         this->mq_Connected = false;

         this->mpc_Ui->pc_PbDisconnnect->setEnabled(false);
         this->mpc_Ui->pc_PbConnect->setEnabled(true);
         this->mpc_Ui->pc_PbUpdate->setVisible(true);
         this->mpc_Ui->pc_PbUpdate->setEnabled(true);

         //Signal summary
         this->mpc_Ui->pc_WiUpdateInformation->ResetSummary();
         //This one will implicitly enable the buttons therefore it cannot be informed at an earlier time
         // And update package
         this->mpc_Ui->pc_WiUpdateInformation->SetDisconnected();
         Q_EMIT (this->SigBlockDragAndDrop(false));

         // Save the time of disconnect
         this->mu32_DisconnectTime = TGL_GetTickCount();

         // Close sequence
         this->m_CleanUpSequence();
         break;
      case C_SyvUpSequences::eNOT_ACTIVE:
      default:
         break;
      }
      // A common handling for all cases could be useful
      if (s32_SequenceResult == C_CHECKSUM)
      {
         C_OgeWiCustomMessage c_MessageAuth(this, C_OgeWiCustomMessage::E_Type::eERROR);

         C_OSCLoggingHandler::h_Flush();
         c_MessageAuth.SetHeading(C_GtGetText::h_GetText("System Update"));
         c_MessageAuth.SetDescription(C_GtGetText::h_GetText(
                                         "Authentication between openSYDE Tool and device(s) has failed. Access denied."));
         c_MessageAuth.SetDetails(C_GtGetText::h_GetText("Possible reasons:<br/>"
                                                         "- Associated private key (*.pem) not found in /certificates folder (most common)<br/>"
                                                         "- Failure during authenfication process<br/>"
                                                         "For more information see ") +
                                  C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLESHEET_GUIDE_COLOR_LINK,
                                                   C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()) +
                                  C_GtGetText::h_GetText("."));
         c_MessageAuth.SetCustomMinHeight(200, 300);
         c_MessageAuth.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handles a precondition error type

   \param[in]       orc_ErrorNodeIndexes     Node indexes in system definition of all nodes with error of the same type
   \param[out]      orc_Description          Description text for the message box
   \param[in,out]   orc_DetailsStart         Beginning of the details text of the message box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_HandlePreconditionErrorType(const std::vector<uint32> & orc_ErrorNodeIndexes,
                                                        const QString & orc_Description,
                                                        const QString & orc_DetailsStart)
{
   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);

   c_Message.SetHeading(C_GtGetText::h_GetText("System Update"));
   QString c_Details = orc_DetailsStart;
   uint32 u32_NodeCounter;

   for (u32_NodeCounter = 0U; u32_NodeCounter < orc_ErrorNodeIndexes.size(); ++u32_NodeCounter)
   {
      this->m_HandleNodePreconditionError(
         c_Details,
         orc_ErrorNodeIndexes[u32_NodeCounter]);
   }

   // Nodes precondition error
   c_Message.SetDescription(orc_Description);
   c_Message.SetDetails(c_Details);
   c_Message.SetCustomMinHeight(200, 300);
   c_Message.Execute();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Extends the error text with the node name and informs the progress log and scene about the specific node

   \param[in,out]   orc_ErrorText           Error text for extending by node name
   \param[in]       ou32_ErrorNodeIndex     Node index in system definition of node with error
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_HandleNodePreconditionError(QString & orc_ErrorText, const uint32 ou32_ErrorNodeIndex)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
      ou32_ErrorNodeIndex);

   if (pc_Node != NULL)
   {
      orc_ErrorText += "- ";
      orc_ErrorText += pc_Node->c_Properties.c_Name.c_str();
      orc_ErrorText += "\n";
   }

   //Signal progress log
   if (this->mpc_ProgressLogContent != NULL)
   {
      this->mpc_ProgressLogContent->UpdateStatus(ou32_ErrorNodeIndex, C_GtGetText::h_GetText(
                                                    "FAIL"), false);
   }
   //Signal scene
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetNodeError(ou32_ErrorNodeIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_UpdateReportText(const QString & orc_NewTextPart) const
{
   osc_write_log_info("Update Node", orc_NewTextPart.toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle connection failure

   \param[in]  oq_SuppressMessageBox   Flag if message box shall be suppressed and not be showed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_HandleConnectionFailure(const bool oq_SuppressMessageBox)
{
   //Signal summary
   if (this->mq_NodesPreconditionError == false)
   {
      this->mpc_Ui->pc_WiUpdateInformation->SetHeading("",
                                                       C_GtGetText::h_GetText("Update mode active. Errors detected!"));
   }
   else
   {
      this->mpc_Ui->pc_WiUpdateInformation->SetHeading("",
                                                       C_GtGetText::h_GetText("Update mode active. Nodes not capable!"));
   }
   //Signal progress log
   if (this->mpc_ProgressLogContent != NULL)
   {
      this->mpc_ProgressLogContent->AddLogHyperlink();
   }
   //Signal scene
   if (this->mpc_Scene != NULL)
   {
      // Stop the previous animation first
      this->mpc_Scene->StopProgressAnimation(false, 0, true);
   }

   if ((this->mpc_Scene != NULL) && (this->mpc_ProgressLogContent != NULL))
   {
      uint32 u32_AddedNodes = 0;
      //All nodes with any response
      const std::vector<uint32> c_RespondedNodes = this->mpc_ProgressLogContent->GetConnectNodeEntryIndices();
      //All nodes which require an update
      const std::vector<uint32> c_ActiveNoneThirdPartyNodeIndices =
         this->mpc_Scene->GetActiveNoneThirdPartyNodeIndices();
      //All nodes which require a response
      std::vector<uint32> c_NodeIndicesWhichRequireAResponse;

      //Check which nodes are either directly required or indirectly required for routing
      c_NodeIndicesWhichRequireAResponse.reserve(
         c_ActiveNoneThirdPartyNodeIndices.size() * c_ActiveNoneThirdPartyNodeIndices.size());
      for (uint32 u32_ItNoneThirdPartyNode = 0; u32_ItNoneThirdPartyNode < c_ActiveNoneThirdPartyNodeIndices.size();
           ++u32_ItNoneThirdPartyNode)
      {
         const C_SyvRoRouteCalculation c_Calc(this->mu32_ViewIndex,
                                              c_ActiveNoneThirdPartyNodeIndices[u32_ItNoneThirdPartyNode],
                                              C_OSCRoutingCalculation::eUPDATE);
         const C_OSCRoutingRoute * const pc_Route = c_Calc.GetBestRoute();
         //Directly required
         c_NodeIndicesWhichRequireAResponse.push_back(c_ActiveNoneThirdPartyNodeIndices[u32_ItNoneThirdPartyNode]);
         if (pc_Route != NULL)
         {
            for (uint32 u32_ItRoute = 0; u32_ItRoute < pc_Route->c_VecRoutePoints.size(); ++u32_ItRoute)
            {
               const C_OSCRoutingRoutePoint & rc_RountingPoint = pc_Route->c_VecRoutePoints[u32_ItRoute];
               //Indirectly required
               c_NodeIndicesWhichRequireAResponse.push_back(rc_RountingPoint.u32_NodeIndex);
            }
         }
      }
      C_Uti::h_Uniqueify(c_NodeIndicesWhichRequireAResponse);

      //Add missing routing nodes
      for (uint32 u32_ItActiveNode = 0; u32_ItActiveNode < c_NodeIndicesWhichRequireAResponse.size();
           ++u32_ItActiveNode)
      {
         const uint32 & ru32_CurActiveNodeIndex = c_NodeIndicesWhichRequireAResponse[u32_ItActiveNode];
         //Check if active
         bool q_AlreadyAdded = false;
         //Check if already added
         for (uint32 u32_AddedNode = 0; u32_AddedNode < c_RespondedNodes.size(); ++u32_AddedNode)
         {
            if (c_RespondedNodes[u32_AddedNode] == ru32_CurActiveNodeIndex)
            {
               q_AlreadyAdded = true;
            }
         }
         //Add missing nodes
         if (q_AlreadyAdded == false)
         {
            //Signal progress log
            m_AddProgressLogConnectEntry(ru32_CurActiveNodeIndex);

            this->mpc_ProgressLogContent->UpdateStatus(ru32_CurActiveNodeIndex, C_GtGetText::h_GetText("FAIL"),
                                                       false);

            //Signal scene
            this->mpc_Scene->SetNodeError(ru32_CurActiveNodeIndex);
            //Always remember
            ++u32_AddedNodes;
         }
      }

      //Display message
      if (oq_SuppressMessageBox == false)
      {
         const QString c_Log = C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str();
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("System Update"));
         c_Message.SetDescription(C_GtGetText::h_GetText(
                                     "Enter update mode failed."
                                     " Details see node progress log and log file."));
         c_Message.SetDetails(C_GtGetText::h_GetText("For more information see ") +
                              C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"),
                                               mc_STYLESHEET_GUIDE_COLOR_LINK,
                                               c_Log) + C_GtGetText::h_GetText("."));
         c_Message.SetCustomMinHeight(180, 230);
         c_Message.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle update failure
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_HandleUpdateFailure(void)
{
   //Signal summary
   this->mpc_Ui->pc_WiUpdateInformation->SetHeading("://images/system_views/UpdateSummaryFailure.svg",
                                                    C_GtGetText::h_GetText("System Update failed!"), true, false);
   this->mpc_Ui->pc_WiUpdateInformation->StopElapsedTimer();
   //Signal progress log
   if (this->mpc_ProgressLogContent != NULL)
   {
      this->mpc_ProgressLogContent->AddLogHyperlink();
   }

   //Display message
   {
      const QString c_Log = C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str();
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("System Update"));
      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "Update failed."
                                  " Details see node progress log and log file."));
      c_Message.SetDetails(C_GtGetText::h_GetText("For more information see ") +
                           C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"),
                                            mc_STYLESHEET_GUIDE_COLOR_LINK,
                                            c_Log) + C_GtGetText::h_GetText("."));
      c_Message.SetCustomMinHeight(180, 230);
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init toolbox widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_InitToolBox(void)
{
   QString c_ViewName = "";

   C_UsSystemView c_ViewSettings;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      c_ViewName = pc_View->GetName();
   }
   c_ViewSettings = C_UsHandler::h_GetInstance()->GetProjSvSetupView(c_ViewName);

   // create toolbox
   if (this->mpc_ProgressLog == NULL)
   {
      mpc_ProgressLogContent = new C_SyvUpProgressLog();
      mpc_ProgressLog = new C_OgeWiHover(*mpc_ProgressLogContent, C_GtGetText::h_GetText("PROGRESS LOG"),
                                         ":images/system_views/IconProgressLog.svg", false, this->mpc_ProgressLogParent,
                                         this->mpc_ProgressLogParent);
   }

   // create fix minimized progress log
   if (this->mpc_FixMinimizedProgressLog == NULL)
   {
      this->mpc_FixMinimizedProgressLog = new C_OgeWiFixPosition(C_GtGetText::h_GetText("PROGRESS LOG"),
                                                                 ":images/system_views/IconProgressLog.svg",
                                                                 QRect(1449, 14, 190, 36), this->mpc_ProgressLogParent);
   }

   // check for saved default values for toolbox
   if (c_ViewSettings.GetUpdateProgressLogPos().x() < 0)
   {
      if (this->mpc_ProgressLogParent == NULL)
      {
         // default value in this error case
         this->mpc_ProgressLog->move(mhsn_WIDGET_BORDER, mhsn_TOOLBOX_INIT_POS_Y);
      }
      else
      {
         // default value
         this->mpc_ProgressLog->setGeometry(QRect(QPoint((((this->mpc_ProgressLogParent->width() -
                                                            this->mpc_ProgressLog->width()) -
                                                           mhsn_WIDGET_BORDER) -  static_cast<sintn> (100)),
                                                         mhsn_TOOLBOX_INIT_POS_Y),
                                                  QSize(400, 319)));

         this->mpc_ProgressLog->SetMaximizedHeight(319);
      }
   }
   else
   {
      // restore last position and size
      this->mpc_ProgressLog->setGeometry(QRect(c_ViewSettings.GetUpdateProgressLogPos(),
                                               c_ViewSettings.GetUpdateProgressLogSize()));
      this->mpc_ProgressLog->SetMaximizedHeight(c_ViewSettings.GetUpdateProgressLogSize().height());
   }

   if (c_ViewSettings.GetUpdateProgressLogMaximized() == false)
   {
      this->m_WiHoverMinBtnClicked();
   }
   else
   {
      this->mpc_ProgressLog->show();
   }

   connect(this->mpc_ProgressLog, &C_OgeWiHover::SigWiHoverMinBtnClicked,
           this, &C_SyvUpUpdateWidget::m_WiHoverMinBtnClicked);
   connect(this->mpc_FixMinimizedProgressLog, &C_OgeWiFixPosition::SigWiFixPosMaxBtnClicked,
           this, &C_SyvUpUpdateWidget::m_WiFixPosMaxBtnClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up progress log
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_CleanUpProgressLog(void)
{
   if (this->mpc_ProgressLog != NULL)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
      if (pc_View != NULL)
      {
         QSize c_Size;
         // save position, size and state of toolbox
         c_Size.setWidth(this->mpc_ProgressLog->width());
         c_Size.setHeight(this->mpc_ProgressLog->GetMaximizedHeight());
         C_UsHandler::h_GetInstance()->SetProjSvUpdateProgressLog(pc_View->GetName(),
                                                                  this->mpc_ProgressLog->pos(), c_Size,
                                                                  this->mpc_ProgressLog->GetMaximized());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add progress log connecting to node entry

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_AddProgressLogConnectEntry(const uint32 ou32_NodeIndex)
{
   if (this->mpc_ProgressLogContent != NULL)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);
      if (pc_Node != NULL)
      {
         const QString c_Text = static_cast<QString>(C_GtGetText::h_GetText("Get status from %1...")).arg(
            pc_Node->c_Properties.c_Name.c_str());
         this->mpc_ProgressLogContent->AddEntry(ou32_NodeIndex, c_Text, false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt paths of mc_NodesToFlash to the temporary folder

   In case of an update of mc_NodesToFlash, the paths must be adapted again to the temporary path with the
   copied files.
   This function will adapt the paths by using the saved paths in mc_NodesWithAllApplications and
   mc_NodesWithAllApplicationsAndTempPath.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_ReplaceOriginalWithTempPaths(void)
{
   uint32 u32_NodeCounter;

   // All vector must have the size of the count of nodes in the system
   tgl_assert(this->mc_NodesToFlash.size() == this->mc_NodesWithAllApplications.size());
   tgl_assert(this->mc_NodesToFlash.size() == this->mc_NodesWithAllApplicationsAndTempPath.size());

   for (u32_NodeCounter = 0U; u32_NodeCounter < this->mc_NodesToFlash.size(); ++u32_NodeCounter)
   {
      uint32 u32_FileCounter;

      // Files for updating
      for (u32_FileCounter = 0U;
           u32_FileCounter < this->mc_NodesToFlash[u32_NodeCounter].c_FilesToFlash.size();
           ++u32_FileCounter)
      {
         uint32 u32_AllApplCounter;

         for (u32_AllApplCounter = 0U;
              u32_AllApplCounter < this->mc_NodesWithAllApplications[u32_NodeCounter].c_FilesToFlash.size();
              ++u32_AllApplCounter)
         {
            // mc_NodesWithAllApplications has a 1:1 mapping to mc_NodesWithAllApplicationsAndTempPath
            // Searching of the correct temp path in mc_NodesWithAllApplicationsAndTempPath for mc_NodesToFlash
            if (this->mc_NodesToFlash[u32_NodeCounter].c_FilesToFlash[u32_FileCounter] ==
                this->mc_NodesWithAllApplications[u32_NodeCounter].c_FilesToFlash[u32_AllApplCounter])
            {
               // Same path, replace the original by the temporary path
               this->mc_NodesToFlash[u32_NodeCounter].c_FilesToFlash[u32_FileCounter] =
                  this->mc_NodesWithAllApplicationsAndTempPath[u32_NodeCounter].c_FilesToFlash[u32_AllApplCounter];
               break;
            }
         }
      }

      // Paramset files
      for (u32_FileCounter = 0U;
           u32_FileCounter < this->mc_NodesToFlash[u32_NodeCounter].c_FilesToWriteToNvm.size();
           ++u32_FileCounter)
      {
         uint32 u32_AllParamFileCounter;

         for (u32_AllParamFileCounter = 0U;
              u32_AllParamFileCounter < this->mc_NodesWithAllApplications[u32_NodeCounter].c_FilesToWriteToNvm.size();
              ++u32_AllParamFileCounter)
         {
            // mc_NodesWithAllApplications has a 1:1 mapping to mc_NodesWithAllApplicationsAndTempPath
            // Searching of the correct temp path in mc_NodesWithAllApplicationsAndTempPath for mc_NodesToFlash
            if (this->mc_NodesToFlash[u32_NodeCounter].c_FilesToWriteToNvm[u32_FileCounter] ==
                this->mc_NodesWithAllApplications[u32_NodeCounter].c_FilesToWriteToNvm[u32_AllParamFileCounter])
            {
               // Same path, replace the original by the temporary path
               this->mc_NodesToFlash[u32_NodeCounter].c_FilesToWriteToNvm[u32_FileCounter] =
                  this->mc_NodesWithAllApplicationsAndTempPath[u32_NodeCounter].
                  c_FilesToWriteToNvm[u32_AllParamFileCounter];
               break;
            }
         }
      }

      // PEM file
      // Same path, replace the original by the temporary path
      if (this->mc_NodesToFlash[u32_NodeCounter].c_PemFile ==
          this->mc_NodesWithAllApplications[u32_NodeCounter].c_PemFile)
      {
         this->mc_NodesToFlash[u32_NodeCounter].c_PemFile =
            this->mc_NodesWithAllApplicationsAndTempPath[u32_NodeCounter].c_PemFile;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle trigger for application information discard

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_DiscardInfo(const uint32 ou32_NodeIndex)
{
   this->mpc_Ui->pc_WiUpdateInformation->DiscardApplicationStatus(ou32_NodeIndex);

   this->m_UpdateUpdatePackageStatus();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get vector with is file based flag for each node

   \return
   Vector with is file based flag for each node
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<bool> C_SyvUpUpdateWidget::m_GetIsFileBasedFlagForEach(void) const
{
   std::vector<bool> c_Retval;
   std::vector<uint8> c_NodeActiveFlags;
   const sint32 s32_FuncRetval = C_PuiSvHandler::h_GetInstance()->GetNodeActiveFlagsWithSquadAdaptions(
      this->mu32_ViewIndex,
      c_NodeActiveFlags);

   if (s32_FuncRetval == C_NO_ERR)
   {
      for (uint32 u32_ItActiveNode = 0UL; u32_ItActiveNode < c_NodeActiveFlags.size(); ++u32_ItActiveNode)
      {
         if (c_NodeActiveFlags[u32_ItActiveNode] == true)
         {
            const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_ItActiveNode);
            if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL) &&
                (pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size()))
            {
               if (pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].
                   q_FlashloaderOpenSydeIsFileBased == true)
               {
                  c_Retval.push_back(true);
               }
               else
               {
                  c_Retval.push_back(false);
               }
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current step connection start

   \param[in]  oe_Step  Current step

   \return
   True  Connection start step
   False Irrelevant step
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsConnectionStart(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   if (oe_Step == C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START)
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current step connection success

   \param[in]  oe_Step  Current step

   \return
   True  Connection success step
   False Irrelevant step
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsConnectionSuccess(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step) //lint !e788 //we do not handle all steps here
   {
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FINISHED:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_XFL_FINISHED:
      q_Retval = true;
      break;
   default:
      q_Retval = false;
      break;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current step connection failure

   \param[in]  oe_Step  Current step

   \return
   True  Connection failure step
   False Irrelevant step
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsConnectionFailure(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step) //lint !e788 //we do not handle all steps here
   {
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_ECU_RESET_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_XFL_ECU_RESET_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_BC_ENTER_PRE_PROGRAMMING_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_XFL_BC_FLASH_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_RECONNECT_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_SET_SESSION_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_XFL_WAKEUP_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_ROUTING_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_ROUTING_AVAILABLE_FEATURE_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_RECONNECT_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_SET_SESSION_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_DEVICE_NAME_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_SECURITY_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FLASHLOADER_CHECK_SECURITY_ACTIVATION_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_XFL_WAKEUP_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_XFL_READING_INFORMATION_ERROR:
      q_Retval = true;
      break;
   default:
      q_Retval = false;
      break;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current step update application start

   \param[in]   oe_Step       Current step
   \param[out]  orq_IsParam   Flag if current step was a parameter file step (only valid if return is true!)
   \param[out]  orq_IsPemFile Flag if current step was a PEM file step (only valid if return is true!)

   \return
   True  Update application start step
   False Irrelevant step
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsUpdateAppStart(const C_OSCSuSequences::E_ProgressStep oe_Step, bool & orq_IsParam,
                                              bool & orq_IsPemFile)
{
   bool q_Retval;

   switch (oe_Step) //lint !e788 //we do not handle all steps here
   {
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_START:  // Application start state
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_START: // Application start state
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_START:  // Application start state
      q_Retval = true;
      orq_IsParam = false;
      orq_IsPemFile = false;
      break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_START:
      q_Retval = true;
      orq_IsParam = true;
      orq_IsPemFile = false;
      break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_START:
      q_Retval = true;
      orq_IsParam = false;
      orq_IsPemFile = true;
      break;
   default:
      q_Retval = false;
      orq_IsParam = false;
      orq_IsPemFile = false;
      break;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current step update application success

   \param[in]   oe_Step       Current step
   \param[out]  orq_IsParam   Flag if current step was a parameter file step (only valid if return is true!)
   \param[out]  orq_IsPemFile Flag if current step was a PEM file step (only valid if return is true!)

   \return
   True  Update application success step
   False Irrelevant step
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsUpdateAppSuccess(const C_OSCSuSequences::E_ProgressStep oe_Step, bool & orq_IsParam,
                                                bool & orq_IsPemFile)
{
   bool q_Retval;

   switch (oe_Step) //lint !e788 //we do not handle all steps here
   {
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_FINISHED:  // Finished application state
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_FINISHED: // Finished application state
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_FINISHED:  // Finished application state
      orq_IsParam = false;
      q_Retval = true;
      orq_IsPemFile = false;
      break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_FILE_FINISHED:
      q_Retval = true;
      orq_IsParam = true;
      orq_IsPemFile = false;
      break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_FINISHED:
      q_Retval = true;
      orq_IsParam = false;
      orq_IsPemFile = true;
      break;
   default:
      orq_IsParam = false;
      q_Retval = false;
      orq_IsPemFile = false;
      break;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current step update start

   \param[in]  oe_Step  Current step

   \return
   True  Update start step
   False Irrelevant step
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsUpdateNodeStart(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step) //lint !e788 //we do not handle all steps here
   {
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_START: // Application start state
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_START: // Application start state
      q_Retval = true;
      break;
   default:
      q_Retval = false;
      break;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current step update success

   \param[in]  oe_Step  Current step

   \return
   True  Update success step
   False Irrelevant step
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsUpdateNodeSuccess(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step) //lint !e788 //we do not handle all steps here
   {
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINISHED: // Finished application state
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_FINISHED: // Finished application state
      q_Retval = true;
      break;
   default:
      q_Retval = false;
      break;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current step update failure

   \param[in]  oe_Step  Current step

   \return
   True  Update failure step
   False Irrelevant step
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsUpdateFailure(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step) //lint !e788 //we do not handle all steps here
   {
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_READ_FEATURE_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_HEX_SIGNATURE_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_RECONNECT_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_COMM_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_FILE_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_MATCH_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_SESSION_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_FILE_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_NOT_OK:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_NAME_NOT_READABLE:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_ERASE_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_AVAILABLE_FEATURE_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_SESSION_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_MAX_SIZE_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_OPEN_FILE_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_WRITE_FILE_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_AVAILABLE_FEATURE_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_SESSION_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_OPEN_FILE_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_EXTRACT_KEY_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_SEND_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_SEND_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_AVAILABLE_FEATURE_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_SESSION_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_SEND_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_AVAILABLE_FEATURE_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_SESSION_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_ERROR:
      q_Retval = true;
      break;
   default:
      q_Retval = false;
      break;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current step update abort

   \param[in]  oe_Step  Current step

   \return
   True  Update abort step
   False Irrelevant step
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsUpdateAbort(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   if (oe_Step == C_OSCSuSequences::eUPDATE_SYSTEM_ABORTED)
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is security warning necessary

   \param[in]  orc_NodesToFlash  Nodes to flash

   \return
   Flags

   \retval   True    Is necessary
   \retval   False   Not necessary
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsSecurityWarningNecessary(
   const std::vector<C_OSCSuSequences::C_DoFlash> & orc_NodesToFlash)
{
   bool q_Retval = false;

   for (uint32 u32_It = 0UL; u32_It < orc_NodesToFlash.size(); ++u32_It)
   {
      const C_OSCSuSequences::C_DoFlash & rc_Flash = orc_NodesToFlash[u32_It];
      if (!rc_Flash.c_PemFile.IsEmpty())
      {
         q_Retval = true;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update update package status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_UpdateUpdatePackageStatus()
{
   // Prepare the update package
   this->mc_NodesToFlash.clear();
   this->mc_NodesOrder.clear();
   // Get the update package with the update configuration
   this->mpc_Ui->pc_WiUpdateInformation->GetUpdatePackage(this->mc_NodesToFlash, this->mc_NodesOrder);
   // Adapt paths of mc_NodesToFlash to the temporary folder
   this->m_ReplaceOriginalWithTempPaths();

   //Update summary widget with new info
   this->mpc_Ui->pc_WiUpdateInformation->InitUpdatePackage(this->mc_NodesToFlash, this->mc_NodesOrder,
                                                           m_GetIsFileBasedFlagForEach(), true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function of fix minimized toolbox widget for button maximizing click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_WiFixPosMaxBtnClicked(void)
{
   this->mpc_ProgressLog->setVisible(true);
   this->mpc_ProgressLog->SetMaximized(true);
   this->mpc_FixMinimizedProgressLog->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function of hover widget for button minimizing click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_WiHoverMinBtnClicked(void)
{
   this->mpc_ProgressLog->setVisible(false);
   this->mpc_ProgressLog->SetMaximized(false);
   this->mpc_FixMinimizedProgressLog->setVisible(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize UpdateButton Iconsg
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdateWidget::InitIcon(void) const
{
   this->mpc_Ui->pc_PbUpdate->InitCustomIcon("://images//system_views/IconUpdate.svg",
                                             "://images//system_views/IconUpdateDisabled.svg");
   this->mpc_Ui->pc_PbDisconnnect->InitCustomIcon(":/images/system_views/IconDisconnect.svg",
                                                  "://images/system_views/IconDisconnectDisabled.svg");
   this->mpc_Ui->pc_PbConnect->InitCustomIcon("://images/system_views/IconConnectWhite.svg",
                                              "://images/system_views/IconConnectGrey.svg");
   this->mpc_Ui->pc_PbCancel->InitCustomIcon("://images/system_views/IconCancelBright.svg",
                                             "://images/system_views/IconCancelDisabled.svg");
}
