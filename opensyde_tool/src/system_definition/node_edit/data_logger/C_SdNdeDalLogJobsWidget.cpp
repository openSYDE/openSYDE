//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base widget for log jobs list

   Base widget for log jobs list

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QFrame>
#include <QFont>
#include "precomp_headers.hpp"
#include "constants.hpp"
#include "stwerrors.hpp"

#include "C_SdNdeDalLogJobsWidget.hpp"
#include "ui_C_SdNdeDalLogJobsWidget.h"
#include "C_OscDataLoggerJob.hpp"
#include "C_OscDataLoggerJobProperties.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_Uti.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobsWidget::C_SdNdeDalLogJobsWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDalLogJobsWidget),
   mu32_NodeIndex(0),
   mpc_ContextMenu(NULL)
{
   this->mpc_Ui->setupUi(this);
   this->InitStaticNames();
   this->m_SetupContextMenu();

   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "C_SdNdeDalLogJobsWidget", true);

   this->mpc_Ui->pc_WidgetBackground->SetBackgroundColor(11);
   const QSize c_ICON_SIZE(16, 16);
   this->mpc_Ui->pc_PushButtonAdd->setIconSize(c_ICON_SIZE);
   this->mpc_Ui->pc_PushButtonAdd->SetSvg("://images/IconAddEnabled.svg", "://images/IconAddDisabled.svg",
                                          "://images/IconAddHovered.svg", "", "", "", "://images/IconAddClicked.svg",
                                          "");

   this->mpc_Ui->pc_JobsListView->setFrameStyle(static_cast<int32_t>(QFrame::NoFrame));

   //   this->mpc_Ui->pc_LabelLogJobs->setFont(mc_STYLE_GUIDE_FONT_SEMIBOLD_13);
   //   this->mpc_Ui->pc_LabelLogJobs->setFont(C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_SEMIBOLD_24));
   this->mpc_Ui->pc_LabelLogJobs->SetFontPixel(13, true, false);
   this->mpc_Ui->pc_LabelLogJobs->SetForegroundColor(4);

   connect(this->mpc_Ui->pc_JobsListView, &C_SdNdeDalLogJobsListView::SigDataChanged, this,
           &C_SdNdeDalLogJobsWidget::m_OnLogJobStateChanged);
   connect(this->mpc_Ui->pc_JobsListView, &C_SdNdeDalLogJobsListView::SigSelectionChanged, this,
           &C_SdNdeDalLogJobsWidget::SigSelectionChanged);

   connect(this->mpc_Ui->pc_PushButtonAdd, &QPushButton::clicked, this, &C_SdNdeDalLogJobsWidget::m_OnAddLogJob);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_ContextMenu and the Qt memory management
C_SdNdeDalLogJobsWidget::~C_SdNdeDalLogJobsWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::InitStaticNames() const
{
   this->mpc_Ui->pc_LabelLogJobs->setText(C_GtGetText::h_GetText("Log Jobs"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} TODO
void C_SdNdeDalLogJobsWidget::LoadUserSettings() const
{
   //TODO
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} TODO
void C_SdNdeDalLogJobsWidget::SaveUserSettings() const
{
   //TODO
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new node index.

   \param[in]  ou32_NodeIndex    Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::SetNode(const uint32_t ou32_NodeIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;

   this->LoadLogJobs();

   this->mpc_Ui->pc_JobsListView->SetSelection(C_SdNdeDalLogJobsListView::eINITIALIZE);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reload log jobs on data change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::LoadLogJobs(void)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      const uint32_t u32_LogJobCount = pc_Node->c_DataLoggerJobs.size();

      if (pc_Node->c_Properties.q_XappSupport == true)
      {
         this->mpc_Ui->pc_PushButtonAdd->setVisible(true);
         this->mpc_Ui->pc_JobsListView->setVisible(true);

         this->m_UpdateLogJobCountLabel(u32_LogJobCount);

         this->mpc_Ui->pc_JobsListView->LoadLogJobs(this->mu32_NodeIndex);
      }
      else
      {
         this->mpc_Ui->pc_LabelLogJobs->setText(C_GtGetText::h_GetText("Log Jobs"));
         this->mpc_Ui->pc_PushButtonAdd->setVisible(false);
         this->mpc_Ui->pc_JobsListView->setVisible(false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Called when log job name in the properties is edited by user

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::OnLogJobNameModified()
{
   // reload the list when a log job name is  modified
   if (this->mpc_Ui->pc_JobsListView->IsEmpty() == false)
   {
      this->mpc_Ui->pc_JobsListView->LoadLogJobs(this->mu32_NodeIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for Log job state change.
 *
 *  \param[in]  orc_Index     Index of the selected log job in the list view
 *  \param[in]  oq_IsEnabled  Indicates if log job is enabled
 *
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_OnLogJobStateChanged(const QModelIndex orc_Index, const bool oq_IsEnabled)
{
   const C_OscDataLoggerJob * const pc_Retval = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(
      this->mu32_NodeIndex, static_cast<uint32_t>(orc_Index.row()));

   if (pc_Retval != NULL)
   {
      C_PuiSdHandler::h_GetInstance()->SetDataLoggerEnabled(mu32_NodeIndex, orc_Index.row(), oq_IsEnabled);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add data logger job

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_OnAddLogJob()
{
   const C_OscDataLoggerJob c_LogJob;

   tgl_assert(C_PuiSdHandler::h_GetInstance()->AddDataLogger(mu32_NodeIndex, c_LogJob, NULL) == C_NO_ERR);

   this->LoadLogJobs();

   this->mpc_Ui->pc_JobsListView->SetSelection(C_SdNdeDalLogJobsListView::eLOGJOB_ADDED);

   Q_EMIT this->SigNumLogJobsChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete data logger job

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_OnDeleteLogJob()
{
   if (this->mpc_Ui->pc_JobsListView->IsEmpty() == false)
   {
      const uint32_t u32_SelectedLogJobIndex = this->mpc_Ui->pc_JobsListView->currentIndex().row();
      const int32_t s32_Retval = C_PuiSdHandler::h_GetInstance()->DeleteDataLogger(this->mu32_NodeIndex,
                                                                                   u32_SelectedLogJobIndex);
      if (s32_Retval == C_NO_ERR)
      {
         this->mpc_Ui->pc_JobsListView->DeleteLogJob(u32_SelectedLogJobIndex);

         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
         this->m_UpdateLogJobCountLabel(pc_Node->c_DataLoggerJobs.size());

         Q_EMIT this->SigNumLogJobsChanged();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setup context menu entries

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_SetupContextMenu()
{
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   this->setContextMenuPolicy(Qt::CustomContextMenu);

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                       "Add new Log Job"), this, &C_SdNdeDalLogJobsWidget::m_OnAddLogJob,
                                    static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_Plus));

   // separating line
   this->mpc_ContextMenu->addSeparator();

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                       "Delete"), this, &C_SdNdeDalLogJobsWidget::m_OnDeleteLogJob,
                                    static_cast<int32_t>(Qt::Key_Delete));

   connect(this, &C_SdNdeDalLogJobsWidget::customContextMenuRequested, this,
           &C_SdNdeDalLogJobsWidget::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   const QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);

   // check if focus is on tree view
   if (this->mpc_Ui->pc_JobsListView->hasFocus() && (this->mpc_Ui->pc_JobsListView->IsEmpty() == false))
   {
      this->mpc_ContextMenu->popup(c_PosGlobal);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updates the number of log jobs in the list on the list title

   \param[in]       ou32_LogJobCount     Number of log jobs

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_UpdateLogJobCountLabel(const uint32_t ou32_LogJobCount)
{
   const QString c_LogJobsCount = static_cast<QString>(C_GtGetText::h_GetText("Log Jobs (%1)")).arg(
      ou32_LogJobCount);

   this->mpc_Ui->pc_LabelLogJobs->setText(c_LogJobsCount);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  brief  Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOriginal = true;

   switch (opc_KeyEvent->key())
   {
   case Qt::Key_BracketRight: // Qt::Key_BracketRight matches the "Not-Num-Plus"-Key
   case Qt::Key_Plus:
      if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOriginal = false;
         this->m_OnAddLogJob();
      }
      break;
   case Qt::Key_Delete:
      q_CallOriginal = false;
      this->m_OnDeleteLogJob();
      break;
   default:
      break;
   }

   if (q_CallOriginal == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}
