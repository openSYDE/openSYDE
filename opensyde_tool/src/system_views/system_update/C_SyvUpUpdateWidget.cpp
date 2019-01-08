//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing system view update

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#include <QApplication>
#include <QFileInfo>

#include "C_SyvUpUpdateWidget.h"
#include "ui_C_SyvUpUpdateWidget.h"

#include "stwerrors.h"
#include "C_SyvUtil.h"
#include "TGLTime.h"
#include "C_UsHandler.h"
#include "C_SyvUpDeviceInfo.h"
#include "C_OSCLoggingHandler.h"
#include "C_GtGetText.h"
#include "C_Uti.h"
#include "CSCLString.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSvData.h"
#include "C_OSCLoggingHandler.h"
#include "C_PuiSdHandler.h"
#include "TGLFile.h"
#include "C_OgeWiUtil.h"
#include "DLLocalize.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_scl;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */
const QString C_SyvUpUpdateWidget::mhc_TempFolder = "system_update_temp";
const sintn C_SyvUpUpdateWidget::mhsn_WidgetBorder = 25;
const sintn C_SyvUpUpdateWidget::mhsn_ToolboxInitPosY = 150;
const stw_types::uint32 C_SyvUpUpdateWidget::mhu32_WaitTime = 5100U;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_ViewIndex        View index
   \param[in]     opc_ProgressLogParent Optional pointer to progress log parent
   \param[in,out] opc_Parent            Optional pointer to parent

   \created     19.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpUpdateWidget::C_SyvUpUpdateWidget(const uint32 ou32_ViewIndex, QWidget * const opc_ProgressLogParent,
                                         QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpUpdateWidget),
   mu32_ViewIndex(ou32_ViewIndex),
   mpc_Scene(NULL),
   mpc_ProgressLog(NULL),
   mpc_ProgressLogParent(opc_ProgressLogParent),
   mpc_ProgressLogContent(NULL),
   mpc_UpSequences(NULL),
   me_Step(C_SyvUpSequences::eNOT_ACTIVE),
   mq_Connected(false),
   mq_ClearProgressLog(true),
   mq_StartUpdateAfterConnect(false),
   mq_ConnectFailed(false),
   mu32_DisconnectTime(0U),
   mu32_UpdateTime(0U)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   QString c_Name;

   mpc_Ui->setupUi(this);

   this->m_InitToolBox();

   // Initialize the update package
   this->mpc_Ui->pc_WidgetUpdatePackage->SetViewIndex(this->mu32_ViewIndex);

   // Initialize the update summary screen
   this->mpc_Ui->pc_WidgetUpdateSummary->SetViewIndex(this->mu32_ViewIndex);

   //Splitter
   this->mpc_Ui->pc_Splitter->SetMargins(20, 36);

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

   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SyvUpUpdateWidget::m_Timer);

   // Progress signals
   // We need a signal which is emitted in the other thread
   // We have to register the type and use the queued connections. Auto is default, but did not work.
   qRegisterMetaType<uint32>("uint32");
   qRegisterMetaType<sint32>("sint32");
   qRegisterMetaType<uint8>("uint8");

   connect(this->mpc_Ui->pc_WidgetUpdatePackage, &C_SyvUpUpdatePackageWidget::SigUpdatePackageState,
           this, &C_SyvUpUpdateWidget::m_UpdatePackageState);

   this->mc_Timer.setInterval(25);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     19.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpUpdateWidget::~C_SyvUpUpdateWidget()
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   // store configuration of the view
   if (pc_View != NULL)
   {
      //Splitter
      const QList<sintn> c_Sizes = this->mpc_Ui->pc_Splitter->sizes();
      if (c_Sizes.count() > 1)
      {
         C_UsHandler::h_GetInstance()->SetProjSvUpdateSplitterX(pc_View->GetName(), c_Sizes.at(1));
      }

      //Scene
      C_UsHandler::h_GetInstance()->SetProjSvUpdateViewZoom(
         pc_View->GetName(), this->mpc_Ui->pc_GraphicsView->GetZoomValue());
      C_UsHandler::h_GetInstance()->SetProjSvUpdateViewPos(
         pc_View->GetName(), this->mpc_Ui->pc_GraphicsView->GetViewPos());
   }
   m_CleanUpToolBox();
   delete mpc_Ui;

   this->m_CleanUpSequence();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initializes all visible strings on the widget

   \created     20.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::InitText(void) const
{
   // button text
   this->mpc_Ui->pc_PbUpdate->setText(QString(C_GtGetText::h_GetText("Update")));
   this->mpc_Ui->pc_PbConnect->setText(QString(C_GtGetText::h_GetText("Connect")));
   this->mpc_Ui->pc_PbDisconnnect->setText(QString(C_GtGetText::h_GetText("Disconnect")));
   this->mpc_Ui->pc_PbCancel->setText(QString(C_GtGetText::h_GetText("Abort Update")));

   // tooltips
   this->mpc_Ui->pc_PbUpdate->SetToolTipInformation(C_GtGetText::h_GetText("System Update"),
                                                    C_GtGetText::h_GetText(
                                                       "Updates all devices with version deviations (comparing to Update Package content). If not connected to system yet, a connect is performed first."));
   this->mpc_Ui->pc_PbConnect->SetToolTipInformation(C_GtGetText::h_GetText("Connect to System (optional)"),
                                                     C_GtGetText::h_GetText(
                                                        "Puts all devices into flashloader mode and gets current versions of each device."));
   this->mpc_Ui->pc_PbDisconnnect->SetToolTipInformation(C_GtGetText::h_GetText("Disconnect from System"),
                                                         C_GtGetText::h_GetText(
                                                            "Puts all devices back into application mode."));
   this->mpc_Ui->pc_PbCancel->SetToolTipInformation(C_GtGetText::h_GetText("Abort Update"),
                                                    C_GtGetText::h_GetText(
                                                       "Forces the update to abort."));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function to prepare closing the widget

   \return
   true     Can be closed.
   false    Can not be closed.

   \created     11.01.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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
         c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                        "System Update is running. Do you really want to abort the current System Update?"));
         c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Continue Update"));
         c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Abort Update"));
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

//-----------------------------------------------------------------------------
/*!
   \brief   Delayed load of scene

   \created     04.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::LoadScene(void)
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->Load();

      this->m_CheckError();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten show event slot

   Here: Load splitter position

   \param[in,out] opc_Event Event identification and information

   \created     28.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::showEvent(QShowEvent * const opc_Event)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   // store configuration of the view
   if (pc_View != NULL)
   {
      const C_UsSystemView c_UserView = C_UsHandler::h_GetInstance()->GetProjSvSetupView(pc_View->GetName());
      sint32 s32_LastSegmentWidth = c_UserView.GetUpdateSplitterX();

      //Revert to default if necessary
      if (s32_LastSegmentWidth <= 0)
      {
         s32_LastSegmentWidth = 320;
      }

      this->mpc_Ui->pc_Splitter->SetFirstSegment(this->mpc_Ui->pc_Splitter->width() - s32_LastSegmentWidth);
   }
   QWidget::showEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten hide event slot

   Here: hide and delete toolbox

   \param[in,out] opc_Event Event identification and information

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::hideEvent(QHideEvent * const opc_Event)
{
   QWidget::hideEvent(opc_Event);
   m_CleanUpToolBox();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten resize event

   Move the toolbox.

   \param[in,out] opc_Event Event identification and information

   \created     15.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   if (this->mpc_ProgressLog != NULL)
   {
      QPoint c_Point = this->mpc_ProgressLog->pos();
      QSize c_Size = this->mpc_ProgressLog->size();
      QWidget * pc_Widget = this->mpc_ProgressLogParent;

      Q_UNUSED(opc_Event)

      if (pc_Widget == NULL)
      {
         // if no parent exist use this widget
         pc_Widget = this;
      }

      // would the toolbox be outside of the widget in x direction
      if ((this->mpc_ProgressLog->x() + this->mpc_ProgressLog->width() + mhsn_WidgetBorder) > pc_Widget->width())
      {
         // is the toolbox to big?
         if ((this->mpc_ProgressLog->width() + (2 * mhsn_WidgetBorder)) > pc_Widget->width())
         {
            c_Size.setWidth(pc_Widget->width() - (2 * mhsn_WidgetBorder));
         }
         else
         {
            // adapt position of toolbox
            c_Point.setX((pc_Widget->width() - this->mpc_ProgressLog->width()) - mhsn_WidgetBorder);
         }
      }

      // would the toolbox be outside of the widget in y direction
      if ((this->mpc_ProgressLog->y() + this->mpc_ProgressLog->height() + mhsn_WidgetBorder) > pc_Widget->height())
      {
         // is the toolbox to big?
         if ((this->mpc_ProgressLog->height() + (2 * mhsn_WidgetBorder)) > pc_Widget->height())
         {
            c_Size.setHeight(pc_Widget->height() - (2 * mhsn_WidgetBorder));
         }
         else
         {
            // adapt position of toolbox
            c_Point.setY((pc_Widget->height() - this->mpc_ProgressLog->height()) - mhsn_WidgetBorder);
         }
      }

      this->mpc_ProgressLog->setGeometry(QRect(c_Point, c_Size));
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update system setup error label

   \created     25.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_CheckError(void)
{
   QString c_ErrorText;
   const bool q_ViewSetupError = C_SyvUtil::h_CheckViewSetupError(this->mu32_ViewIndex, c_ErrorText);

   if (q_ViewSetupError == true)
   {
      this->mpc_Ui->pc_ErrorLabel->SetCompleteText(c_ErrorText);
      this->mpc_Ui->pc_ErrorFrame->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_ErrorFrame->setVisible(false);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize system update sequence

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid

   \created     18.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvUpUpdateWidget::m_InitSequence(void)
{
   QString c_Message;
   sint32 s32_Retval;

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

   switch (s32_Retval)
   {
   case C_NO_ERR:
      break;
   case C_CONFIG:
      c_Message = QString(C_GtGetText::h_GetText("Invalid System Definition/View configuration."));
      break;
   case C_RD_WR:
      c_Message =
         QString(C_GtGetText::h_GetText("Configured communication DLL does not exist or DLL could not be opened."));
      break;
   case C_OVERFLOW:
      c_Message =
         QString(C_GtGetText::h_GetText(
                    "Unknown transport protocol or unknown diagnostic server for at least one node."));
      break;
   case C_NOACT:
      c_Message = QString(C_GtGetText::h_GetText("System View is invalid. Action cannot be performed."));
      break;
   case C_COM:
      c_Message =
         QString(C_GtGetText::h_GetText(
                    "CAN initialization failed. Check your PC CAN interface configuration (System View setup - "
                    "double click on PC)."));
      break;
   case C_CHECKSUM:
      c_Message = QString(C_GtGetText::h_GetText("Internal buffer overflow detected."));
      break;
   case C_RANGE:
      c_Message = QString(C_GtGetText::h_GetText("Routing configuration failed."));
      break;
   case C_UNKNOWN_ERR:
      c_Message = QString(C_GtGetText::h_GetText("Wrapped error of internal function call."));
      break;
   case C_WARN:
      c_Message = QString(C_GtGetText::h_GetText("Internal error."));
      break;
   default:
      c_Message = QString(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(s32_Retval));
      break;
   }

   if (s32_Retval != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR, c_Message);
      c_MessageBox.Execute();
   }

   //Simplify return value
   if (s32_Retval != C_NO_ERR)
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Removes the

   \created     30.10.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_UpdatePackageState(const sint32 os32_State)
{
   if (os32_State == C_NO_ERR)
   {
      this->mpc_Ui->pc_PbConnect->setEnabled(true);
      this->mpc_Ui->pc_PbUpdate->setEnabled(true);
   }
   else
   {
      this->mpc_Ui->pc_PbConnect->setEnabled(false);
      this->mpc_Ui->pc_PbUpdate->setEnabled(false);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Reporting slot for system update

   \param[in]     ou32_Step         Step of node configuration
   \param[in]     os32_Result       Result of service
   \param[in]     ou8_Progress      Progress of sequence in percentage (goes from 0..100 for each function)

   \created     18.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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
      this->mpc_Ui->pc_WidgetUpdateSummary->SetHeading("://images/system_views/UpdateSummaryOk.svg",
                                                       C_GtGetText::h_GetText(
                                                          "System Update successful!"), false, true);
      this->mpc_Ui->pc_WidgetUpdateSummary->StopElapsedTimer();
      this->mpc_Ui->pc_WidgetUpdateSummary->SetProgressFile100(100, true);
      //Signal progress log
      if (this->mpc_ProgressLogContent != NULL)
      {
         this->mpc_ProgressLogContent->AddSubHeading(C_GtGetText::h_GetText("System Update successful!"));
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Reporting slot for system update for specific server

   \param[in]     ou32_Step            Step of node configuration
   \param[in]     os32_Result          Result of service
   \param[in]     ou8_BusIdentifier    Affected sub net
   \param[in]     ou8_NodeIdentifier   Node Id of affected node
   \param[in]     ou8_Progress         Progress of sequence in percentage (goes from 0..100 for each function)
                                       Progress invalid: 255

   \created     18.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_ReportProgressForServer(const uint32 ou32_Step, const sint32 os32_Result,
                                                    const uint8 ou8_Progress, const uint8 ou8_BusIdentifier,
                                                    const uint8 ou8_NodeIdentifier)
{
   const C_OSCSuSequences::E_ProgressStep e_Step = static_cast<C_OSCSuSequences::E_ProgressStep>(ou32_Step);
   uint32 u32_NodeIndex;
   uint32 u32_BusIndex;

   (void)os32_Result;
   const sint32 s32_Result = C_SyvUtil::h_GetIndicesFromBusId(ou8_BusIdentifier, ou8_NodeIdentifier, u32_NodeIndex,
                                                              u32_BusIndex);

   //Report progress
   if (s32_Result == C_NO_ERR)
   {
      if (C_SyvUpUpdateWidget::mh_IsConnectionStart(e_Step) == true)
      {
         //Signal progress log
         m_AddProgressLogConnectEntry(u32_NodeIndex);
      }
      else if (C_SyvUpUpdateWidget::mh_IsUpdateAppStart(e_Step) == true)
      {
         this->mpc_Ui->pc_WidgetUpdatePackage->SetUpdateApplicationStarted(u32_NodeIndex);
      }
      else if (C_SyvUpUpdateWidget::mh_IsUpdateNodeStart(e_Step) == true)
      {
         const C_PuiSdNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetUINode(u32_NodeIndex);
         this->mu32_ApplicationIndex = 0;
         //Signal scene
         if (this->mpc_Scene != NULL)
         {
            // Stop the previous animation first
            this->mpc_Scene->StopProgressAnimation(false, 0, false);
            this->mpc_Scene->StartProgressAnimation(u32_NodeIndex);
         }
         //Scroll scene
         if (pc_Node != NULL)
         {
            this->mpc_Ui->pc_GraphicsView->ScrollTo(pc_Node->c_UIPosition,
                                                    QSizeF(pc_Node->f64_Width, pc_Node->f64_Height));
         }
         //Signal progress log
         if (this->mpc_ProgressLogContent != NULL)
         {
            const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodeIndex);
            if (pc_Node != NULL)
            {
               const QString c_Text = QString(C_GtGetText::h_GetText("Update %1...")).arg(
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
            this->mpc_Scene->SetNoResponse(u32_NodeIndex);
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
         m_HandleUpdateFailure();
         this->mpc_Ui->pc_WidgetUpdatePackage->SetUpdateApplicationError(u32_NodeIndex);
         //Signal progress log
         if (this->mpc_ProgressLogContent != NULL)
         {
            this->mpc_ProgressLogContent->UpdateStatus(u32_NodeIndex, C_GtGetText::h_GetText("FAIL"), true);
         }
         //Signal scene
         if (this->mpc_Scene != NULL)
         {
            this->mpc_Scene->StopProgressAnimation(true, this->mu32_ApplicationIndex, true);
         }
      }
      else if (C_SyvUpUpdateWidget::mh_IsUpdateAbort(e_Step) == true)
      {
         this->mpc_Ui->pc_WidgetUpdatePackage->SetUpdateApplicationError(u32_NodeIndex);
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
         this->mpc_Ui->pc_WidgetUpdateSummary->SetHeading("://images/system_views/UpdateSummaryFailure.svg", C_GtGetText::h_GetText(
                                                             "System Update failed!"), true, false);
         this->mpc_Ui->pc_WidgetUpdateSummary->StopElapsedTimer();
      }
      else if (C_SyvUpUpdateWidget::mh_IsUpdateAppSuccess(e_Step) == true)
      {
         ++this->mu32_ApplicationIndex;
         this->mpc_Ui->pc_WidgetUpdatePackage->SetUpdateApplicationFinished(u32_NodeIndex);
         //Signal summary
         this->mpc_Ui->pc_WidgetUpdateSummary->SignalFileFinished();
      }
      else if (C_SyvUpUpdateWidget::mh_IsUpdateNodeSuccess(e_Step) == true)
      {
         //Signal progress log
         if (this->mpc_ProgressLogContent != NULL)
         {
            this->mpc_ProgressLogContent->UpdateStatus(u32_NodeIndex, C_GtGetText::h_GetText("OK"), true);
         }
      }
      else if (this->me_Step == C_SyvUpSequences::eUPDATE_SYSTEM)
      {
         switch (e_Step)
         {
         case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_START: // The normal flash progress
         case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_START:     // The normal flash progress
         case C_OSCSuSequences::eXFL_PROGRESS:                                         // The normal flash progress
            if (ou8_Progress != 0xFF)
            {
               this->mpc_Ui->pc_WidgetUpdatePackage->SetNodeProgress(u32_NodeIndex, ou8_Progress);
               //Signal summary
               if (this->me_Step == C_SyvUpSequences::eUPDATE_SYSTEM)
               {
                  this->mpc_Ui->pc_WidgetUpdateSummary->SetProgressFile100(static_cast<uint16>(ou8_Progress), false);
               }
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

//-----------------------------------------------------------------------------
/*!
   \brief   Reporting slot for read openSYDE flashloader information

   \created     18.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

         this->m_UpdateReportText(QString(C_GtGetText::h_GetText(
                                             "openSYDE device information found for node with index %1")).arg(
                                     c_NodeIndexes
                                     [
                                        u32_Counter
                                     ]));
         this->m_UpdateReportText(QString(C_GtGetText::h_GetText("Device name: %1")).arg(rc_Info.c_DeviceName.c_str()));
         this->m_UpdateReportText(QString(C_GtGetText::h_GetText("Number of applications: %1")).arg(rc_Info.
                                                                                                    c_Applications.
                                                                                                    size()));

         for (uint8 u8_Application = 0U; u8_Application < rc_Info.c_Applications.size(); u8_Application++)
         {
            this->m_UpdateReportText(
               QString(C_GtGetText::h_GetText("Application %1")).arg(static_cast<uint32>(u8_Application)));
            this->m_UpdateReportText(
               QString(C_GtGetText::h_GetText(" Name: %1")).arg(
                  rc_Info.c_Applications[u8_Application].c_ApplicationName.c_str()));
            this->m_UpdateReportText(
               QString(C_GtGetText::h_GetText(" Version: %1")).arg(
                  rc_Info.c_Applications[u8_Application].c_ApplicationVersion.c_str()));
            this->m_UpdateReportText(
               QString(C_GtGetText::h_GetText(" Build date: %1")).arg(
                  rc_Info.c_Applications[u8_Application].c_BuildDate.c_str()));
            this->m_UpdateReportText(
               QString(C_GtGetText::h_GetText(" Build time: %1")).arg(
                  rc_Info.c_Applications[u8_Application].c_BuildTime.c_str()));
            this->m_UpdateReportText(
               QString(C_GtGetText::h_GetText(" Block start address: 0x%1")).arg(
                  QString::number(rc_Info.c_Applications[u8_Application].u32_BlockStartAddress, 16).rightJustified(
                     8, '0')));
            this->m_UpdateReportText(
               QString(C_GtGetText::h_GetText(" Block end address: 0x%1")).arg(
                  QString::number(rc_Info.c_Applications[u8_Application].u32_BlockEndAddress, 16).rightJustified(
                     8, '0')));
            this->m_UpdateReportText(
               QString(C_GtGetText::h_GetText(" Signature valid: %1")).arg((rc_Info.c_Applications[u8_Application].
                                                                            u8_SignatureValid == 0) ? "yes" : "no"));
            this->m_UpdateReportText(QString(C_GtGetText::h_GetText(" Additional information: %1")).arg(
                                        rc_Info.c_Applications[u8_Application].c_AdditionalInformation.c_str()));
         }

         this->m_UpdateReportText(QString(C_GtGetText::h_GetText("Flashloader software version: V%1.%2r%3")).arg(
                                     QString::number(rc_Info.c_MoreInformation.au8_FlashloaderSoftwareVersion[0]).
                                     rightJustified(2, '0'),
                                     QString::number(rc_Info.c_MoreInformation.au8_FlashloaderSoftwareVersion[1]).
                                     rightJustified(2, '0'),
                                     QString::number(rc_Info.c_MoreInformation.au8_FlashloaderSoftwareVersion[2]).
                                     rightJustified(2, '0')));
         this->m_UpdateReportText(QString(C_GtGetText::h_GetText("Flashloader protocol version: V%1.%2r%3")).arg(
                                     QString::number(rc_Info.c_MoreInformation.au8_FlashloaderProtocolVersion[1]).
                                     rightJustified(2, '0'),
                                     QString::number(rc_Info.c_MoreInformation.au8_FlashloaderProtocolVersion[1]).
                                     rightJustified(2, '0'),
                                     QString::number(rc_Info.c_MoreInformation.au8_FlashloaderProtocolVersion[2]).
                                     rightJustified(2, '0')));
         this->m_UpdateReportText(QString(C_GtGetText::h_GetText("Protocol version: V%1.%2r%3")).arg(
                                     QString::number(rc_Info.c_MoreInformation.au8_ProtocolVersion[0]).rightJustified(2,
                                                                                                                      '0'),
                                     QString::number(rc_Info.c_MoreInformation.au8_ProtocolVersion[1]).rightJustified(2,
                                                                                                                      '0'),
                                     QString::number(rc_Info.c_MoreInformation.au8_ProtocolVersion[2]).rightJustified(2,
                                                                                                                      '0')));
         this->m_UpdateReportText(QString(C_GtGetText::h_GetText("Flash count: %1")).arg(rc_Info.c_MoreInformation.
                                                                                         u32_FlashCount));
         this->m_UpdateReportText(
            QString(C_GtGetText::h_GetText("Device serial number: %1")).arg(mh_SerialNumberToString(rc_Info.
                                                                                                    c_MoreInformation.
                                                                                                    au8_EcuSerialNumber)));
         this->m_UpdateReportText(QString(C_GtGetText::h_GetText("Device article number: %1")).arg(rc_Info.
                                                                                                   c_MoreInformation.
                                                                                                   u32_EcuArticleNumber));
         this->m_UpdateReportText(QString(C_GtGetText::h_GetText("Device article version: %1")).arg(
                                     rc_Info.c_MoreInformation.c_EcuHardwareVersionNumber.c_str()));
         this->m_UpdateReportText(
            QString(C_GtGetText::h_GetText("Flash fingerprint date: %1-%2-%3 (yy-mm-dd)")).arg(
               QString::number(static_cast<uint32>(
                                  rc_Info.c_MoreInformation.au8_FlashFingerprintDate[0])).rightJustified(2, '0'),
               QString::number(static_cast<uint32>(
                                  rc_Info.c_MoreInformation.au8_FlashFingerprintDate[1])).rightJustified(2, '0'),
               QString::number(static_cast<uint32>(
                                  rc_Info.c_MoreInformation.au8_FlashFingerprintDate[2])).rightJustified(2, '0')));
         this->m_UpdateReportText(QString(C_GtGetText::h_GetText("Flash fingerprint time: %1:%2:%3")).arg(
                                     QString::number(static_cast<uint32>(
                                                        rc_Info.c_MoreInformation.au8_FlashFingerprintTime[0])).
                                     rightJustified(2, '0'),
                                     QString::number(static_cast<uint32>(
                                                        rc_Info.c_MoreInformation.au8_FlashFingerprintTime[1])).
                                     rightJustified(2, '0'),
                                     QString::number(static_cast<uint32>(
                                                        rc_Info.c_MoreInformation.au8_FlashFingerprintTime[2])).
                                     rightJustified(2, '0')));
         this->m_UpdateReportText(QString(C_GtGetText::h_GetText("Flash fingerprint username: %1")).arg(
                                     rc_Info.c_MoreInformation.c_FlashFingerprintUserName.c_str()));
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

      //Signal scene
      if (this->mpc_Scene != NULL)
      {
         this->mpc_Scene->UpdateDeviceInformation(c_NodeIndexes, c_Devices);
      }
      // Signal update package
      this->mpc_Ui->pc_WidgetUpdatePackage->UpdateDeviceInformation(c_NodeIndexes, c_Devices);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Reporting slot for read STW flashloader information

   \created     13.02.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
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
            QString(C_GtGetText::h_GetText("STW Flashloader device information found for node with index %1")).arg(
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
      this->mpc_Ui->pc_WidgetUpdatePackage->UpdateDeviceInformation(c_NodeIndexes, c_Devices);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start connecting to nodes

   \created     18.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_Connect(void)
{
   sint32 s32_Return;

   QApplication::setOverrideCursor(Qt::WaitCursor);

   // Is a new connect already possible
   while ((this->mu32_DisconnectTime + mhu32_WaitTime) > TGL_GetTickCount())
   {
      // Wait till it is possible
   }

   s32_Return = this->m_InitSequence();

   //Always reset flag
   this->mq_ConnectFailed = false;

   if (s32_Return == C_NO_ERR)
   {
      this->m_UpdateReportText(C_GtGetText::h_GetText("Activate Flashloader: Started"));
      s32_Return = this->mpc_UpSequences->StartActivateFlashloader();

      //Buttons
      this->mpc_Ui->pc_PbConnect->setEnabled(false);
      this->mpc_Ui->pc_PbUpdate->setEnabled(false);

      //Signal progress log
      if (this->mpc_ProgressLogContent != NULL)
      {
         this->mpc_ProgressLogContent->Clear();
         if (this->mq_StartUpdateAfterConnect == false)
         {
            this->mpc_ProgressLogContent->SetHeading(C_GtGetText::h_GetText("System Connect"));
         }
         this->mpc_ProgressLogContent->AddSubHeading(C_GtGetText::h_GetText("Connecting to System..."));
      }

      //Signal summary
      this->mpc_Ui->pc_WidgetUpdateSummary->SetHeading("", C_GtGetText::h_GetText("Connecting to System..."));
      //Signal scene
      if (this->mpc_Scene != NULL)
      {
         //Reset connected status (required if in system disconnected state due to no node response)
         this->mpc_Scene->SetConnected(false, true);
         this->mpc_Scene->SetConnected(true, true);
         this->mpc_Scene->StartConnectionAnimation();
      }
      // And Update package
      this->mpc_Ui->pc_WidgetUpdatePackage->SetConnected();
      // When connected, no drag and drop is allowed on top level
      Q_EMIT this->SigBlockDragAndDrop(true);

      if (s32_Return == C_NO_ERR)
      {
         //Don't set step if timer still active
         this->me_Step = C_SyvUpSequences::eACTIVATE_FLASHLOADER;
         //Timer
         this->mc_Timer.start();
      }
      else
      {
         //Thread access possible
         this->m_UpdateReportText(QString(
                                     C_GtGetText::h_GetText(
                                        "Activate Flashloader: Can not start. Error on initialization: %1")).
                                  arg(C_Uti::h_StwError(s32_Return)));
      }
   }
   else
   {
      this->m_UpdateReportText(QString(
                                  C_GtGetText::h_GetText(
                                     "Activate Flashloader: Can not start. Error on initialization: %1")).
                               arg(C_Uti::h_StwError(s32_Return)));
   }

   QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start updating of nodes

   \created     18.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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
                                  "Please check nodes connection and reconnect."));
      c_Message.Execute();
   }
   else
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (this->mq_Connected == true)
      {
         if (this->mpc_UpSequences != NULL)
         {
            QApplication::setOverrideCursor(Qt::WaitCursor);

            if (pc_View != NULL)
            {
               sint32 s32_Return;

               this->m_UpdateReportText(C_GtGetText::h_GetText("Update System started"));

               // Is a new update already possible
               while ((this->mu32_UpdateTime + mhu32_WaitTime) > TGL_GetTickCount())
               {
                  // Wait till it is possible
               }

               // Get the state of the update package
               // Prepare the update package
               this->mc_NodesToFlash.clear();
               this->mc_NodesOrder.clear();
               // Get the update package with the update configuration
               s32_Return = this->mpc_Ui->pc_WidgetUpdatePackage->GetUpdatePackage(this->mc_NodesToFlash,
                                                                                   this->mc_NodesOrder);

               if (s32_Return == C_NOACT)
               {
                  // No applications to update
                  C_OgeWiCustomMessage c_Message(this);
                  c_Message.SetHeading(C_GtGetText::h_GetText("System Update"));
                  c_Message.SetDescription(C_GtGetText::h_GetText(
                                              "No System Update needed. All applications for the devices match the ones in the "
                                              "update package. To update anyway see details."));
                  c_Message.SetDetails(C_GtGetText::h_GetText(
                                          "For updating a device nevertheless, there is an option to "
                                          "trigger a discard of the application information in the "
                                          "\"Device Status Information\" dialog (double click on node)."));

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
                     this->mpc_Ui->pc_WidgetUpdatePackage->SetUpdateStarted();

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
                     this->mpc_Ui->pc_WidgetUpdateSummary->InitUpdatePackage(this->mc_NodesToFlash, this->mc_NodesOrder,
                                                                             m_GetIsFileBasedFlagForEach());
                     this->mpc_Ui->pc_WidgetUpdateSummary->SetHeading("", C_GtGetText::h_GetText("Updating System..."));
                     this->mpc_Ui->pc_WidgetUpdateSummary->StartElapsedTimer();
                  }
                  else
                  {
                     m_HandleUpdateFailure();

                     this->m_UpdateReportText(QString(
                                                 C_GtGetText::h_GetText(
                                                    "Update System: Can not start. Thread is still busy.")).
                                              arg(C_Uti::h_StwError(s32_Return)));
                  }
               }
            }
            else
            {
               m_HandleUpdateFailure();

               this->m_UpdateReportText(QString(C_GtGetText::h_GetText(
                                                   "Update System: Can not start. Error on System View.")));
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

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger system reset

   \created     21.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_Disconnect(void)
{
   m_DisconnectAction();
   //Always reset flag
   this->mq_ConnectFailed = false;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Resets the system

   \created     20.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_DisconnectAction(const bool oq_ClearLogAndResetScene)
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

         if (oq_ClearLogAndResetScene == true)
         {
            //Signal scene
            if (this->mpc_Scene != NULL)
            {
               this->mpc_Scene->SetConnected(false, true);
            }
            //Signal log
            if (this->mpc_ProgressLogContent != NULL)
            {
               this->mpc_ProgressLogContent->Clear();
            }
         }
         else
         {
            //Signal scene (partially)
            if (this->mpc_Scene != NULL)
            {
               this->mpc_Scene->SetConnected(false, false);
            }
         }
         //Signal summary
         this->mpc_Ui->pc_WidgetUpdateSummary->Reset();
         this->mpc_Ui->pc_WidgetUpdateSummary->SetHeading("", C_GtGetText::h_GetText("Disconnecting from System..."));
      }
      else
      {
         this->m_UpdateReportText(QString(C_GtGetText::h_GetText("Disconnect: Can not start. Thread is still busy.")));
         this->m_CleanUpSequence();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Aborts the current sequence

   \created     20.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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
      e_ReturnMessageBox = c_MessageBox.Execute();

      if (e_ReturnMessageBox == C_OgeWiCustomMessage::eNO)
      {
         this->mpc_UpSequences->AbortCurrentProgress();
      }
   }
}

//-----------------------------------------------------------------------------
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
      bool q_AlreadyDisplayedError = false;
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
               this->m_UpdateReportText(QString(C_GtGetText::h_GetText(
                                                   "Read Device information: Can not start. Thread is still busy.")));
            }
         }
         else
         {
            // Sequence failed
            this->me_Step = C_SyvUpSequences::eNOT_ACTIVE;
            this->mq_StartUpdateAfterConnect = false;

            this->mpc_Ui->pc_PbUpdate->setVisible(true);
            this->mpc_Ui->pc_PbUpdate->setEnabled(true);
            this->mpc_Ui->pc_PbConnect->setEnabled(true);
            m_HandleConnectionFailure();
         }
         break;
      case C_SyvUpSequences::eREAD_DEVICE_INFORMATION:
         // Sequence finished
         this->me_Step = C_SyvUpSequences::eNOT_ACTIVE;

         this->mpc_Ui->pc_PbUpdate->setVisible(true);
         this->mpc_Ui->pc_PbUpdate->setEnabled(true);
         if (s32_SequenceResult == C_NO_ERR)
         {
            this->mq_Connected = true;

            this->mpc_Ui->pc_PbConnect->setEnabled(false);
            this->mpc_Ui->pc_PbDisconnnect->setEnabled(true);

            //Signal summary
            this->mpc_Ui->pc_WidgetUpdateSummary->SetHeading("", C_GtGetText::h_GetText("Connected to System!"));
            //Signal scene
            if (this->mpc_Scene != NULL)
            {
               // Stop the previous animation first
               this->mpc_Scene->StopProgressAnimation(false, 0, true);
            }

            // Prepare the update package
            this->mc_NodesToFlash.clear();
            this->mc_NodesOrder.clear();
            // Get the update package with the update configuration
            s32_Result = this->mpc_Ui->pc_WidgetUpdatePackage->GetUpdatePackage(this->mc_NodesToFlash,
                                                                                this->mc_NodesOrder);
            if (s32_Result == C_NO_ERR)
            {
               QString c_ErrorPath;
               const C_SCLString c_ExePath = C_Uti::h_GetExePath().toStdString().c_str();
               C_SCLString c_TemporaryPath = c_ExePath + "/" + mhc_TempFolder.toStdString().c_str() + "/";

               // Copy all relevant files to a temporary folder to have them "safe"
               s32_Result = this->mpc_UpSequences->SyvUpCreateTemporaryFolder(c_TemporaryPath, this->mc_NodesToFlash,
                                                                              c_ErrorPath);

               if (s32_Result != C_NO_ERR)
               {
                  C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                  QString c_MessageText;
                  QString c_CompleteMessageText;
                  switch (s32_Result)
                  {
                  case C_CONFIG:
                     c_MessageText = "View is invalid or initialization was not finished";
                     break;
                  case C_OVERFLOW:
                     c_MessageText =
                        "Size of orc_ApplicationsToWrite is not the same as the size of nodes in orc_Nodes;\n"
                        "Size of orc_ActiveNodes is not the same as the size of nodes in orc_Nodes";
                     break;
                  case C_NOACT:
                     c_MessageText =
                        "orc_ApplicationsToWrite has non-empty list of files for node that was not set as active in orc_ActiveNodes;\n"
                        "size of files in orc_ApplicationsToWrite[node] is higher than the"
                        "number of applications of the node in orc_Nodes (for an active and address based node)";
                     break;
                  case C_RANGE:
                     c_MessageText = "File referenced by orc_ApplicationsToWrite does not exist\n"
                                     "orc_TargetPath does not end in \"\\\" or \"/\"";
                     break;
                  case C_BUSY:
                     c_MessageText =
                        "Could not erase pre-existing target path (note: can result in partially erased target path)";
                     break;
                  case C_RD_WR:
                     c_MessageText = "Could not copy file:\n" + c_ErrorPath;
                     break;
                  case C_TIMEOUT:
                     c_MessageText = "Could not create target directory:\n" + c_ErrorPath;
                     break;
                  default:
                     c_MessageText = QString("Unknown: %1").arg(C_Uti::h_StwError(s32_Result));
                     break;
                  }
                  c_CompleteMessageText = QString(C_GtGetText::h_GetText(
                                                     "Connect to system: error on creating temporary folder: %1")).arg(
                     c_MessageText);
                  this->m_UpdateReportText(c_CompleteMessageText);
                  c_Message.SetHeading(C_GtGetText::h_GetText("System Update"));
                  c_Message.SetDescription("Connect to system: error on creating temporary folder.");
                  c_Message.SetDetails(c_MessageText);
                  c_Message.Execute();
                  q_AlreadyDisplayedError = true;
               }
               else
               {
                  this->mpc_Ui->pc_WidgetUpdateSummary->InitUpdatePackage(this->mc_NodesToFlash, this->mc_NodesOrder,
                                                                          m_GetIsFileBasedFlagForEach());
               }
            }
            else if (s32_Result == C_NOACT)
            {
               //nothing to flash: this is not a connection problem
               s32_Result = C_NO_ERR;
            }

            if (s32_Result == C_NO_ERR)
            {
               //Signal progress log
               if (this->mpc_ProgressLogContent != NULL)
               {
                  this->mpc_ProgressLogContent->AddSubHeading(C_GtGetText::h_GetText("Connected to System!"));
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
                  this->mpc_Ui->pc_PbDisconnnect->setEnabled(true);
               }
            }
         }

         if ((s32_Result != C_NO_ERR) ||
             (s32_SequenceResult != C_NO_ERR))
         {
            this->mq_ConnectFailed = true;
            this->mpc_Ui->pc_PbUpdate->setEnabled(true);
            this->mpc_Ui->pc_PbDisconnnect->setEnabled(true);
            m_HandleConnectionFailure();
            if ((this->mq_StartUpdateAfterConnect == true) && (q_AlreadyDisplayedError == false))
            {
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
               c_Message.SetHeading(C_GtGetText::h_GetText("System Update"));
               c_Message.SetDescription(C_GtGetText::h_GetText("There are active nodes, which are not responding. \n"
                                                               "Please check nodes connection and reconnect."));
               c_Message.Execute();
            }
         }

         this->mq_StartUpdateAfterConnect = false;
         break;
      case C_SyvUpSequences::eUPDATE_SYSTEM:
         // Sequence finished
         this->me_Step = C_SyvUpSequences::eNOT_ACTIVE;

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
         }

         this->mpc_Ui->pc_WidgetUpdatePackage->SetUpdateFinished();

         this->m_UpdateReportText(QString(C_GtGetText::h_GetText("Update System: %1")).arg(c_Message));

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
         this->mpc_Ui->pc_WidgetUpdateSummary->Reset();
         //This one will implicitly enable the buttons therefore it cannot be informed at an earlier time
         // And update package
         this->mpc_Ui->pc_WidgetUpdatePackage->SetDisconnected();
         Q_EMIT this->SigBlockDragAndDrop(false);

         // Save the time of disconnect
         this->mu32_DisconnectTime = TGL_GetTickCount();

         // Close sequence
         this->m_CleanUpSequence();
         break;
      default:
         break;
      }
   }
}

//-----------------------------------------------------------------------------
QString C_SyvUpUpdateWidget::m_GetTimeStamp(const bool oq_Bold) const
{
   C_TGLDateTime c_DateTime;

   std::string c_DateTimeFormatted;
   QString c_Text;

   Q_UNUSED(oq_Bold)

   TGL_GetDateTimeNow(c_DateTime);
   c_DateTimeFormatted = C_OSCLoggingHandler::h_UtilConvertDateTimeToString(c_DateTime);
   c_Text = c_DateTimeFormatted.c_str();

   return c_Text;
}

//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_UpdateReportText(const QString & orc_NewTextPart) const
{
   osc_write_log_info("Update Node", orc_NewTextPart.toStdString().c_str());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle connection failure

   \created     23.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_HandleConnectionFailure(void)
{
   //Signal summary
   this->mpc_Ui->pc_WidgetUpdateSummary->SetHeading("", C_GtGetText::h_GetText("Connected to System. Nodes missing!"));
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
      //All nodes with no response
      const std::vector<uint32> c_ActiveNoResponseNodeIndices = this->mpc_Scene->GetActiveNoResponseNodeIndices();
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
            this->mpc_Scene->SetNoResponse(ru32_CurActiveNodeIndex);
            //Always remember
            ++u32_AddedNodes;
         }
      }

      //Check existing and added ones then compare with expected number for special case no response
      if ((c_ActiveNoResponseNodeIndices.size() + u32_AddedNodes) == c_NodeIndicesWhichRequireAResponse.size())
      {
         //Handle no nodes case
         m_DisconnectAction(false);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle update failure

   \created     23.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_HandleUpdateFailure(void)
{
   //Signal summary
   this->mpc_Ui->pc_WidgetUpdateSummary->SetHeading("://images/system_views/UpdateSummaryFailure.svg", C_GtGetText::h_GetText(
                                                       "System update failed!"), true, false);
   this->mpc_Ui->pc_WidgetUpdateSummary->StopElapsedTimer();
   //Signal progress log
   if (this->mpc_ProgressLogContent != NULL)
   {
      this->mpc_ProgressLogContent->AddLogHyperlink();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Init toolbox widget

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
      mpc_ProgressLog = new C_OgeWiHover(*mpc_ProgressLogContent, C_GtGetText::h_GetText(
                                            "PROGRESS LOG"), false, this->mpc_ProgressLogParent,
                                         this->mpc_ProgressLogParent);
   }

   // check for saved default values for toolbox
   if (c_ViewSettings.GetUpdateProgressLogPos().x() < 0)
   {
      if (this->mpc_ProgressLogParent == NULL)
      {
         // default value in this error case
         this->mpc_ProgressLog->move(mhsn_WidgetBorder, mhsn_ToolboxInitPosY);
      }
      else
      {
         // default value
         this->mpc_ProgressLog->setGeometry(QRect(QPoint(((this->mpc_ProgressLogParent->width() -
                                                           this->mpc_ProgressLog->width()) -
                                                          mhsn_WidgetBorder -  static_cast<sintn> (100)),
                                                         mhsn_ToolboxInitPosY),
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
      this->mpc_ProgressLog->SetMinimize();
   }

   this->mpc_ProgressLog->show();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clean up toolbox

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_CleanUpToolBox(void)
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
      this->mpc_ProgressLog->hide();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add progress log connecting to node entry

   \param[in] ou32_NodeIndex Node index

   \created     21.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_AddProgressLogConnectEntry(const uint32 ou32_NodeIndex)
{
   if (this->mpc_ProgressLogContent != NULL)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);
      if (pc_Node != NULL)
      {
         const QString c_Text = QString(C_GtGetText::h_GetText("Get status from %1...")).arg(
            pc_Node->c_Properties.c_Name.c_str());
         this->mpc_ProgressLogContent->AddEntry(ou32_NodeIndex, c_Text, false);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle trigger for application information discard

   \param[in] ou32_NodeIndex Node index

   \created     22.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdateWidget::m_DiscardInfo(const uint32 ou32_NodeIndex)
{
   this->mpc_Ui->pc_WidgetUpdatePackage->DiscardApplicationStatus(ou32_NodeIndex);

   // Prepare the update package
   this->mc_NodesToFlash.clear();
   this->mc_NodesOrder.clear();
   // Get the update package with the update configuration
   this->mpc_Ui->pc_WidgetUpdatePackage->GetUpdatePackage(this->mc_NodesToFlash, this->mc_NodesOrder);
   //Update summary widget with new info
   this->mpc_Ui->pc_WidgetUpdateSummary->InitUpdatePackage(this->mc_NodesToFlash, this->mc_NodesOrder,
                                                           m_GetIsFileBasedFlagForEach());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get vector with is file based flag for each node

   \return
   Vector with is file based flag for each node

   \created     12.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<bool> C_SyvUpUpdateWidget::m_GetIsFileBasedFlagForEach(void) const
{
   std::vector<bool> c_Retval;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   if (pc_View != NULL)
   {
      const std::vector<uint8> & rc_ActiveNodes = pc_View->GetNodeActiveFlags();
      for (uint32 u32_ItActiveNode = 0UL; u32_ItActiveNode < rc_ActiveNodes.size(); ++u32_ItActiveNode)
      {
         if (rc_ActiveNodes[u32_ItActiveNode] == true)
         {
            const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_ItActiveNode);
            if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL))
            {
               if (pc_Node->pc_DeviceDefinition->q_FlashloaderOpenSydeIsFileBased == true)
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

//-----------------------------------------------------------------------------
QString C_SyvUpUpdateWidget::mh_SerialNumberToString(const uint8 oau8_SerialNumer[])
{
   QString c_Result = "";
   uint32 u32_Counter;

   for (u32_Counter = 0U; u32_Counter < 6; ++u32_Counter)
   {
      QString c_Part = QString::number(oau8_SerialNumer[u32_Counter], 16);
      c_Part = c_Part.rightJustified(2, '0');
      c_Result += c_Part;
      //Add point after first and fourth byte
      if ((u32_Counter == 0) || (u32_Counter == 3))
      {
         c_Result += ".";
      }
   }

   return c_Result;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if current step animated

   \param[in] oe_Step Current step

   \return
   True  Animated step
   False Irrelevant step

   \created     09.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsAnimated(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step)
   {
   case C_OSCSuSequences::eUPDATE_SYSTEM_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_FINAL_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_FINISHED:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_START:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_FINISHED:
   case C_OSCSuSequences::eXFL_PROGRESS:
      q_Retval = true;
      break;
   default:
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if current step connection start

   \param[in] oe_Step Current step

   \return
   True  Connection start step
   False Irrelevant step

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsConnectionStart(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step)
   {
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START:
      q_Retval = true;
      break;
   default:
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if current step connection success

   \param[in] oe_Step Current step

   \return
   True  Connection success step
   False Irrelevant step

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsConnectionSuccess(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step)
   {
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FINISHED:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_XFL_FINISHED:
      q_Retval = true;
      break;
   default:
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if current step connection failure

   \param[in] oe_Step Current step

   \return
   True  Connection failure step
   False Irrelevant step

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsConnectionFailure(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step)
   {
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_BC_REQUEST_PROGRAMMING_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_BC_ECU_RESET_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_XFL_ECU_RESET_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_BC_ENTER_PRE_PROGRAMMING_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_XFL_BC_FLASH_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_RECONNECT_ERRROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_SET_SESSION_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_XFL_WAKEUP_ERROR:
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_ROUTING_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_RECONNECT_ERRROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_SET_SESSION_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_DEVICE_NAME_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_SECURITY_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_XFL_WAKEUP_ERROR:
   case C_OSCSuSequences::eREAD_DEVICE_INFO_XFL_READING_INFORMATION_ERROR:
      q_Retval = true;
      break;
   default:
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if current step update application start

   \param[in] oe_Step Current step

   \return
   True  Update application start step
   False Irrelevant step

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsUpdateAppStart(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step)
   {
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_START:  // Application start state
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_START: // Application start state
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_START:  // Application start state
      q_Retval = true;
      break;
   default:
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if current step update application success

   \param[in] oe_Step Current step

   \return
   True  Update application success step
   False Irrelevant step

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsUpdateAppSuccess(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step)
   {
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_FINISHED:  // Finished application state
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_FINISHED: // Finished application state
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_FINISHED:  // Finished application state
      q_Retval = true;
      break;
   default:
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if current step update start

   \param[in] oe_Step Current step

   \return
   True  Update start step
   False Irrelevant step

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsUpdateNodeStart(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step)
   {
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_START: // Application start state
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_START: // Application start state
      q_Retval = true;
      break;
   default:
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if current step update success

   \param[in] oe_Step Current step

   \return
   True  Update success step
   False Irrelevant step

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsUpdateNodeSuccess(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step)
   {
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINISHED: // Finished application state
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_FINISHED: // Finished application state
      q_Retval = true;
      break;
   default:
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if current step update failure

   \param[in] oe_Step Current step

   \return
   True  Update failure step
   False Irrelevant step

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsUpdateFailure(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step)
   {
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
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_ERASE_ERRROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_ERRROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_ERROR:
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_ERROR:
      q_Retval = true;
      break;
   default:
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if current step update abort

   \param[in] oe_Step Current step

   \return
   True  Update abort step
   False Irrelevant step

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdateWidget::mh_IsUpdateAbort(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   bool q_Retval;

   switch (oe_Step)
   {
   case C_OSCSuSequences::eUPDATE_SYSTEM_ABORTED:
      q_Retval = true;
      break;
   default:
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert state to readable string

   \param[in] oe_Step State

   \return
   Readable string

   \created     14.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_SyvUpUpdateWidget::mh_GetString(const C_OSCSuSequences::E_ProgressStep oe_Step)
{
   QString c_Retval;

   switch (oe_Step)
   {
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_BC_REQUEST_PROGRAMMING_START:
      c_Retval = "eACTIVATE_FLASHLOADER_OSY_BC_REQUEST_PROGRAMMING_START"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_BC_REQUEST_PROGRAMMING_ERROR:
      c_Retval = "eACTIVATE_FLASHLOADER_OSY_BC_REQUEST_PROGRAMMING_ERROR"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_BC_ECU_RESET_START:
      c_Retval = "eACTIVATE_FLASHLOADER_OSY_BC_ECU_RESET_START"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_BC_ECU_RESET_ERROR:
      c_Retval = "eACTIVATE_FLASHLOADER_OSY_BC_ECU_RESET_ERROR"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_XFL_ECU_RESET_START:
      c_Retval = "eACTIVATE_FLASHLOADER_XFL_ECU_RESET_START"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_XFL_ECU_RESET_ERROR:
      c_Retval = "eACTIVATE_FLASHLOADER_XFL_ECU_RESET_ERROR"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_XFL_BC_ENTER_FLASHLOADER_START:
      c_Retval = "eACTIVATE_FLASHLOADER_OSY_XFL_BC_ENTER_FLASHLOADER_START"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_BC_ENTER_PRE_PROGRAMMING_ERROR:
      c_Retval = "eACTIVATE_FLASHLOADER_OSY_BC_ENTER_PRE_PROGRAMMING_ERROR"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_XFL_BC_FLASH_ERROR:
      c_Retval = "eACTIVATE_FLASHLOADER_XFL_BC_FLASH_ERROR"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START:
      c_Retval = "eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_RECONNECT_ERRROR:
      c_Retval = "eACTIVATE_FLASHLOADER_OSY_RECONNECT_ERRROR"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_OSY_SET_SESSION_ERROR:
      c_Retval = "eACTIVATE_FLASHLOADER_OSY_SET_SESSION_ERROR"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_XFL_WAKEUP_ERROR:
      c_Retval = "eACTIVATE_FLASHLOADER_XFL_WAKEUP_ERROR"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_ROUTING_START:
      c_Retval = "eACTIVATE_FLASHLOADER_ROUTING_START"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_ROUTING_ERROR:
      c_Retval = "eACTIVATE_FLASHLOADER_ROUTING_ERROR"; break;
   case C_OSCSuSequences::eACTIVATE_FLASHLOADER_FINISHED:
      c_Retval = "eACTIVATE_FLASHLOADER_FINISHED"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_START:
      c_Retval = "eREAD_DEVICE_INFO_START"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_RECONNECT_ERRROR:
      c_Retval = "eREAD_DEVICE_INFO_OSY_RECONNECT_ERRROR"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_SET_SESSION_START:
      c_Retval = "eREAD_DEVICE_INFO_OSY_SET_SESSION_START"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_SET_SESSION_ERROR:
      c_Retval = "eREAD_DEVICE_INFO_OSY_SET_SESSION_ERROR"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_DEVICE_NAME_START:
      c_Retval = "eREAD_DEVICE_INFO_OSY_DEVICE_NAME_START"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_DEVICE_NAME_ERROR:
      c_Retval = "eREAD_DEVICE_INFO_OSY_DEVICE_NAME_ERROR"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_START:
      c_Retval = "eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_START"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_SECURITY_ERROR:
      c_Retval = "eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_SECURITY_ERROR"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_ERROR:
      c_Retval = "eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_ERROR"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_START:
      c_Retval = "eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_START"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_ERROR:
      c_Retval = "eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_ERROR"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_XFL_WAKEUP_ERROR:
      c_Retval = "eREAD_DEVICE_INFO_XFL_WAKEUP_ERROR"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_XFL_READING_INFORMATION_START:
      c_Retval = "eREAD_DEVICE_INFO_XFL_READING_INFORMATION_START"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_XFL_READING_INFORMATION_ERROR:
      c_Retval = "eREAD_DEVICE_INFO_XFL_READING_INFORMATION_ERROR"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_FINISHED:
      c_Retval = "eREAD_DEVICE_INFO_FINISHED"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_START:
      c_Retval = "eREAD_DEVICE_INFO_OSY_START"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_XFL_START:
      c_Retval = "eREAD_DEVICE_INFO_XFL_START"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_OSY_FINISHED:
      c_Retval = "eREAD_DEVICE_INFO_OSY_FINISHED"; break;
   case C_OSCSuSequences::eREAD_DEVICE_INFO_XFL_FINISHED:
      c_Retval = "eREAD_DEVICE_INFO_XFL_FINISHED"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_START:
      c_Retval = "eUPDATE_SYSTEM_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_START:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINISHED:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_START:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_ERROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_ERROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_HEX_SIGNATURE_ERROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_HEX_SIGNATURE_ERROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_RECONNECT_ERROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_RECONNECT_ERROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_START:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_COMM_ERROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_COMM_ERROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_FILE_ERROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_FILE_ERROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_MATCH_ERROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_MATCH_ERROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_START:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_SESSION_ERROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_SESSION_ERROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_FILE_ERROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_FILE_ERROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_NOT_OK:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_NOT_OK"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_START:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_NAME_NOT_READABLE:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_NAME_NOT_READABLE"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_ERROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_ERROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_START:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_START:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_ERASE_ERRROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_ERASE_ERRROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_START:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_ERROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_ERROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_START:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_FINAL_START:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_FINAL_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_ERROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_ERROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_FINISHED:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_FINISHED"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_START:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_START:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_ERRROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_ERRROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_START:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_ERROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_ERROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_START:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_ERROR:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_ERROR"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_FINISHED:
      c_Retval = "eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_FINISHED"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_ABORTED:
      c_Retval = "eUPDATE_SYSTEM_ABORTED"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_FINISHED:
      c_Retval = "eUPDATE_SYSTEM_FINISHED"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_START:
      c_Retval = "eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_ERROR:
      c_Retval = "eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_START"; break;
   case C_OSCSuSequences::eXFL_PROGRESS:
      c_Retval = "eXFL_PROGRESS"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_START:
      c_Retval = "eUPDATE_SYSTEM_XFL_NODE_START"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_FINISHED:
      c_Retval = "eUPDATE_SYSTEM_XFL_NODE_FINISHED"; break;
   case C_OSCSuSequences::eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_FINISHED:
      c_Retval = "eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_FINISHED"; break;
   case C_OSCSuSequences::eRESET_SYSTEM_START:
      c_Retval = "eRESET_SYSTEM_START"; break;
   case C_OSCSuSequences::eRESET_SYSTEM_OSY_NODE_ERROR:
      c_Retval = "eRESET_SYSTEM_OSY_NODE_ERROR"; break;
   case C_OSCSuSequences::eRESET_SYSTEM_FINISHED:
      c_Retval = "eRESET_SYSTEM_FINISHED"; break;
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize UpdateButton Iconsg

   \created     21.02.2018  STW/Hollmann
*/
//-----------------------------------------------------------------------------
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
