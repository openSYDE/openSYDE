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
#include "C_SdNdeDalCopClipBoardHelper.hpp"
#include "C_OscDataLoggerJobProperties.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_Uti.hpp"
#include "TglUtils.hpp"
#include "C_UsHandler.hpp"

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
   mu32_CurrentLogJobIndex(0),
   mpc_ContextMenu(NULL),
   mq_IsOverviewVisible(false)
{
   this->mpc_Ui->setupUi(this);
   this->InitStaticNames();
   this->m_SetupContextMenu();

   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "C_SdNdeDalLogJobsWidget", true);

   const QSize c_ICON_SIZE(16, 16);
   this->mpc_Ui->pc_PushButtonAdd->setIconSize(c_ICON_SIZE);
   this->mpc_Ui->pc_PushButtonAdd->SetSvg("://images/IconAddEnabled.svg", "://images/IconAddDisabled.svg",
                                          "://images/IconAddHovered.svg", "", "", "", "://images/IconAddClicked.svg",
                                          "");

   this->mpc_Ui->pc_JobsListView->setFrameStyle(static_cast<int32_t>(QFrame::NoFrame));

   // overview button
   this->mpc_Ui->pc_PbLogJobsOverview->setCheckable(true);
   this->mpc_Ui->pc_PbLogJobsOverview->setChecked(true); // start with overview
   this->mq_IsOverviewVisible = true;

   connect(this->mpc_Ui->pc_JobsListView, &C_SdNdeDalLogJobsListView::SigDataChanged, this,
           &C_SdNdeDalLogJobsWidget::m_OnLogJobStateChanged);
   connect(this->mpc_Ui->pc_JobsListView, &C_SdNdeDalLogJobsListView::SigSelectionChanged, this,
           &C_SdNdeDalLogJobsWidget::m_LogJobSelectionChanged);

   connect(this->mpc_Ui->pc_PushButtonAdd, &QPushButton::clicked, this, &C_SdNdeDalLogJobsWidget::m_OnAddLogJob);
   connect(this->mpc_Ui->pc_PbLogJobsOverview, &QPushButton::clicked, this,
           &C_SdNdeDalLogJobsWidget::m_OnOverviewClicked);
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
   // overview
   this->mpc_Ui->pc_PbLogJobsOverview->setText(C_GtGetText::h_GetText("Log Jobs"));
   this->mpc_Ui->pc_PbLogJobsOverview->
   SetToolTipInformation(C_GtGetText::h_GetText("Log Jobs Overview"),
                         C_GtGetText::h_GetText("Show overview of all Log Jobs."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::LoadUserSettings()
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_UsNode c_UsNode = C_UsHandler::h_GetInstance()->GetProjSdNode(pc_Node->c_Properties.c_Name.c_str());

      // If a valid log job index exists in user settings
      if (c_UsNode.GetSelectedDataLoggerLogJobIndex() >= 0)
      {
         this->mu32_CurrentLogJobIndex = c_UsNode.GetSelectedDataLoggerLogJobIndex();
      }

      // if log jobs overview was previously Not selected
      if (!c_UsNode.GetIsOverviewWidgetSelected())
      {
         this->mpc_Ui->pc_JobsListView->SetSelection(C_SdNdeDalLogJobsListView::eSELECTINDEX,
                                                     this->mu32_CurrentLogJobIndex);
         this->m_ShowOverview(false);
      }
      else
      {
         this->m_ShowOverview(true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::SaveUserSettings() const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      if (!this->mpc_Ui->pc_PbLogJobsOverview->isChecked())
      {
         C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedDataLoggerLogJobIndex(
            pc_Node->c_Properties.c_Name.c_str(), this->mu32_CurrentLogJobIndex);
      }
      else
      {
         C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedDataLoggerLogJobIndex(
            pc_Node->c_Properties.c_Name.c_str(), -1);
      }
      C_UsHandler::h_GetInstance()->SetProjSdNodeIsOverviewWidgetSelected(
         pc_Node->c_Properties.c_Name.c_str(), this->mpc_Ui->pc_PbLogJobsOverview->isChecked());
   }
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
         this->mpc_Ui->pc_PbLogJobsOverview->setText(C_GtGetText::h_GetText("Log Jobs"));
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
/*! \brief  Load selected LogJob from Overview widget

   \param[in]  ou32_LogJobIndex     LogJob index to load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::LoadSelectedLogJob(const uint32_t ou32_LogJobIndex)
{
   this->mpc_Ui->pc_JobsListView->SetSelection(C_SdNdeDalLogJobsListView::eSELECTINDEX, ou32_LogJobIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for Log job state change.
 *
 *  \param[in]  orc_Index     Index of the selected log job in the list view
 *  \param[in]  oq_IsEnabled  Indicates if log job is enabled
 *
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_OnLogJobStateChanged(const QModelIndex & orc_Index, const bool oq_IsEnabled)
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
/*! \brief  On cut log job
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_OnCutLogJob()
{
   m_OnCopyLogJob();
   m_OnDeleteLogJob();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On copy log job
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_OnCopyLogJob()
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      const QModelIndexList c_SelectedIndexes = this->mpc_Ui->pc_JobsListView->selectionModel()->selectedIndexes();
      if (c_SelectedIndexes.size() > 0L)
      {
         std::vector<C_OscDataLoggerJob> c_Data;
         for (QModelIndexList::ConstIterator c_It = c_SelectedIndexes.cbegin(); c_It != c_SelectedIndexes.cend();
              ++c_It)
         {
            const uint32_t u32_SelectedLogJobIndex = c_It->row();
            if (u32_SelectedLogJobIndex < pc_Node->c_DataLoggerJobs.size())
            {
               c_Data.push_back(pc_Node->c_DataLoggerJobs[u32_SelectedLogJobIndex]);
            }
         }
         if (c_Data.size() > 0UL)
         {
            C_SdNdeDalCopClipBoardHelper::h_StoreDataloggerToClipboard(c_Data);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On paste log job
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_OnPasteLogJob()
{
   std::vector<C_OscDataLoggerJob> c_Data;
   if (C_SdNdeDalCopClipBoardHelper::h_LoadDataloggerFromClipboardRemoveInvalidIdsAndReportChanges(
          mu32_NodeIndex,
          c_Data,
          this) ==
       C_NO_ERR)
   {
      if (c_Data.size() > 0UL)
      {
         for (uint32_t u32_ItNew = 0UL; u32_ItNew < c_Data.size(); ++u32_ItNew)
         {
            tgl_assert(C_PuiSdHandler::h_GetInstance()->AddDataLogger(mu32_NodeIndex, c_Data[u32_ItNew],
                                                                      NULL) == C_NO_ERR);
         }

         this->LoadLogJobs();

         this->mpc_Ui->pc_JobsListView->SetSelection(C_SdNdeDalLogJobsListView::eLOGJOB_ADDED);

         Q_EMIT this->SigNumLogJobsChanged();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete data logger job

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_OnDeleteLogJob()
{
   if (this->mpc_Ui->pc_JobsListView->IsEmpty() == false)
   {
      const QModelIndexList c_SelectedIndexes = this->mpc_Ui->pc_JobsListView->selectionModel()->selectedIndexes();

      if (c_SelectedIndexes.size() > 0L)
      {
         std::vector<uint32_t> c_SelectedItems;
         for (QModelIndexList::ConstIterator c_It = c_SelectedIndexes.cbegin(); c_It != c_SelectedIndexes.cend();
              ++c_It)
         {
            c_SelectedItems.push_back(c_It->row());
         }

         // Delete log jobs from backend i.e. data repository
         const int32_t s32_Retval = C_PuiSdHandler::h_GetInstance()->DeleteMultipleDataLoggers(this->mu32_NodeIndex,
                                                                                               c_SelectedItems);
         // Delete log jobs from front end i.e model / view
         if (s32_Retval == C_NO_ERR)
         {
            this->mpc_Ui->pc_JobsListView->DeleteLogJobs(c_SelectedItems);

            const C_OscNode * const pc_Node =
               C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
            this->m_UpdateLogJobCountLabel(pc_Node->c_DataLoggerJobs.size());

            // if all log jobs are deleted, show the overview
            if (pc_Node->c_DataLoggerJobs.size() == 0)
            {
               this->m_ShowOverview(true);
            }

            Q_EMIT this->SigNumLogJobsChanged();
         }
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
                                       "Cut"), this, &C_SdNdeDalLogJobsWidget::m_OnCutLogJob,
                                    static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_X));
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                       "Copy"), this, &C_SdNdeDalLogJobsWidget::m_OnCopyLogJob,
                                    static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_C));
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                       "Paste"), this, &C_SdNdeDalLogJobsWidget::m_OnPasteLogJob,
                                    static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_V));

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

   this->mpc_Ui->pc_PbLogJobsOverview->setText(c_LogJobsCount);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show or hide overview widget

   Show or hide log job edit widget inversely.

   \param[in]  oq_Show  true: show overview widget and hide log job edit widget; false inverse
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_ShowOverview(const bool oq_Show)
{
   if (oq_Show == true)
   {
      this->mpc_Ui->pc_WidgetBackground->SetBackgroundColor(10);
      this->mpc_Ui->pc_JobsListView->selectionModel()->clearSelection();
      this->mpc_Ui->pc_JobsListView->setCurrentIndex(QModelIndex()); // reset
   }
   else
   {
      this->mpc_Ui->pc_WidgetBackground->SetBackgroundColor(11);
   }
   this->mpc_Ui->pc_PbLogJobsOverview->setChecked(oq_Show);
   this->mq_IsOverviewVisible = oq_Show;

   Q_EMIT this->SigShowOverview(oq_Show);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for overview button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_OnOverviewClicked()
{
   this->m_ShowOverview(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load selected LogJob on Selection changed

   \param[in]  ou32_LogJobIndex     LogJob index to load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_LogJobSelectionChanged(const uint32_t ou32_LogJobIndex)
{
   this->mu32_CurrentLogJobIndex = ou32_LogJobIndex;

   // If Overview widget not already hidden, hide it
   if (this->mq_IsOverviewVisible)
   {
      this->m_ShowOverview(false);
   }
   Q_EMIT (this->SigSelectionChanged(ou32_LogJobIndex));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

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
   case Qt::Key_V:
      if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOriginal = false;
         this->m_OnPasteLogJob();
      }
      break;
   case Qt::Key_C:
      if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOriginal = false;
         this->m_OnCopyLogJob();
      }
      break;
   case Qt::Key_X:
      if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOriginal = false;
         this->m_OnCutLogJob();
      }
      break;
   case Qt::Key_Delete:
      q_CallOriginal = false;
      this->m_OnDeleteLogJob();
      break;
   default:
      break;
   }

   if (q_CallOriginal)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}
