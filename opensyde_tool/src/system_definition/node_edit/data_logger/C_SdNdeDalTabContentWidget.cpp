//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main widget for data logger tab content

   Main widget for data logger tab content

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_UsHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeDalTabContentWidget.hpp"
#include "ui_C_SdNdeDalTabContentWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

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
C_SdNdeDalTabContentWidget::C_SdNdeDalTabContentWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDalTabContentWidget),
   mu32_NodeIndex(0),
   mu32_LogJobIndex(0)
{
   this->mpc_Ui->setupUi(this);

   // splitter
   this->mpc_Ui->pc_Splitter->SetMargins(0, 0);
   // if there is more space stretch right widget (i.e. index 1)
   this->mpc_Ui->pc_Splitter->setStretchFactor(0, 0);
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 10);

   this->InitStaticNames();

   connect(this->mpc_Ui->pc_GeneralSettingsWidget,
           &C_SdNdeDalLogJobGeneralSettingsWidget::SigReloadDataLoggerDataElements,
           this, &C_SdNdeDalTabContentWidget::ReloadDataLogger);

   connect(this->mpc_Ui->pc_LogJobsWidget, &C_SdNdeDalLogJobsWidget::SigSelectionChanged, this,
           &C_SdNdeDalTabContentWidget::SetNodeDataLoggerJob);
   connect(this->mpc_Ui->pc_LogJobsWidget, &C_SdNdeDalLogJobsWidget::SigNumLogJobsChanged,
           this, &C_SdNdeDalTabContentWidget::m_NumDataLoggersChanged);

   connect(this->mpc_Ui->pc_LogJobDataSelectionWidget, &C_SdNdeDalLogJobDataSelectionWidget::SigNumElementsChanged,
           this->mpc_Ui->pc_LogJobPropertiesWidget,
           &C_SdNdeDalLogJobPropertiesWidget::ReloadAdditionalTrigger);
   connect(this->mpc_Ui->pc_LogJobPropertiesWidget, &C_SdNdeDalLogJobPropertiesWidget::SigLogJobNameModified,
           this->mpc_Ui->pc_LogJobsWidget, &C_SdNdeDalLogJobsWidget::OnLogJobNameModified);
   connect(this->mpc_Ui->pc_LogJobsWidget, &C_SdNdeDalLogJobsWidget::SigShowOverview,
           this, &C_SdNdeDalTabContentWidget::m_ShowOverview);
   connect(this->mpc_Ui->pc_LogJobsOverviewWidget, &C_SdNdeDalLogJobsOvWidget::SigLoadSelectedLogJob,
           this, &C_SdNdeDalTabContentWidget::LoadSelectedLogJob);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalTabContentWidget::~C_SdNdeDalTabContentWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::LoadUserSettings() const
{
   // splitter
   this->mpc_Ui->pc_Splitter->SetFirstSegment(C_UsHandler::h_GetInstance()->GetSdNodeEditDataLoggerSplitterHorizontal());

   // sub widgets
   this->mpc_Ui->pc_LogJobsWidget->LoadUserSettings();
   this->mpc_Ui->pc_LogJobDataSelectionWidget->LoadUserSettings();
   this->mpc_Ui->pc_LogJobsOverviewWidget->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::SaveUserSettings() const
{
   // splitter
   const QList<int32_t> c_Sizes = this->mpc_Ui->pc_Splitter->sizes();

   if (c_Sizes.size() > 0)
   {
      C_UsHandler::h_GetInstance()->SetSdNodeEditDataLoggerSplitterHorizontal(c_Sizes.at(0));
   }

   // sub widgets
   this->mpc_Ui->pc_LogJobsWidget->SaveUserSettings();
   this->mpc_Ui->pc_LogJobDataSelectionWidget->SaveUserSettings();
   this->mpc_Ui->pc_GeneralSettingsWidget->SaveUserSettings();
   this->mpc_Ui->pc_LogJobPropertiesWidget->SaveUserSettings();
   this->mpc_Ui->pc_LogJobsOverviewWidget->SaveUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelNoData->setText(
      C_GtGetText::h_GetText("No Log Jobs declared.\nAdd any via the '+' button."));
   this->mpc_Ui->pc_LabelDisabled->setText(
      C_GtGetText::h_GetText("No Log Jobs declared.\n"
                             "Enable X.App support in node properties to configure data logger."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Trigger entire reload of data logger section

   Necessary if e.g. data elements could have changed or X.App support got disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::ReloadDataLogger()
{
   this->mpc_Ui->pc_LogJobDataSelectionWidget->ReloadDataElements();
   this->mpc_Ui->pc_LogJobsWidget->LoadLogJobs();
   this->mpc_Ui->pc_LogJobPropertiesWidget->ReloadAdditionalTrigger();
   this->m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new node index.

   \param[in]  ou32_NodeIndex    Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::SetNode(const uint32_t ou32_NodeIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   // Forward node index
   // Table first as reloading might get triggered while initializing other widgets
   this->mpc_Ui->pc_LogJobDataSelectionWidget->SetNodeDataLoggerJob(ou32_NodeIndex, this->mu32_LogJobIndex);

   this->mpc_Ui->pc_LogJobsOverviewWidget->SetNodeIndex(ou32_NodeIndex);
   this->mpc_Ui->pc_LogJobsWidget->SetNode(ou32_NodeIndex);
   this->mpc_Ui->pc_GeneralSettingsWidget->SetNode(ou32_NodeIndex);
   this->mpc_Ui->pc_LogJobPropertiesWidget->SetNodeDataLoggerJob(ou32_NodeIndex, this->mu32_LogJobIndex);

   this->m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::Save()
{
   this->mpc_Ui->pc_GeneralSettingsWidget->Save();
   this->mpc_Ui->pc_LogJobPropertiesWidget->Save();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Loads the log job properties when the selected log job changes
 *
 *  \param[in] ou32_LogJobIndex Selected Log Job index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::SetNodeDataLoggerJob(const uint32_t ou32_LogJobIndex)
{
   this->mu32_LogJobIndex = ou32_LogJobIndex;
   this->mpc_Ui->pc_LogJobPropertiesWidget->SetNodeDataLoggerJob(this->mu32_NodeIndex, this->mu32_LogJobIndex);
   this->mpc_Ui->pc_LogJobDataSelectionWidget->SetNodeDataLoggerJob(this->mu32_NodeIndex, this->mu32_LogJobIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load selected LogJob from Overview widget

   \param[in]  ou32_LogJobIndex     LogJob index to load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::LoadSelectedLogJob(const uint32_t ou32_LogJobIndex)
{
   this->mpc_Ui->pc_LogJobsWidget->LoadSelectedLogJob(ou32_LogJobIndex);
   this->m_ShowOverview(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update UI
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::m_UpdateUi()
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      if (pc_Node->c_Properties.q_XappSupport == true)
      {
         this->mpc_Ui->pc_GeneralSettingsWidget->setVisible(true);
         this->mpc_Ui->pc_Splitter->setVisible(true);
         this->mpc_Ui->pc_LabelDisabled->setVisible(false);
         m_HandleNoDataLoggersLabel();
      }
      else
      {
         this->mpc_Ui->pc_GeneralSettingsWidget->setVisible(false);
         this->mpc_Ui->pc_Splitter->setVisible(false);
         this->mpc_Ui->pc_LabelDisabled->setVisible(true);
      }

      this->updateGeometry();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Number of data loggers changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::m_NumDataLoggersChanged()
{
   this->mpc_Ui->pc_LogJobsOverviewWidget->update();
   m_HandleNoDataLoggersLabel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle no data loggers label
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::m_HandleNoDataLoggersLabel()
{
   bool q_DataValid = false;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      if (pc_Node->c_DataLoggerJobs.size() > 0UL)
      {
         q_DataValid = true;
      }
   }
   this->mpc_Ui->pc_LogJobDataSelectionWidget->setVisible(q_DataValid);
   this->mpc_Ui->pc_LogJobPropertiesWidget->setVisible(q_DataValid);
   this->mpc_Ui->pc_LabelNoData->setVisible(q_DataValid == false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show or hide overview widget

   Show or hide log job edit widget inversely.

   \param[in]  oq_Show  true: show overview widget and hide log job edit widget; false inverse
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::m_ShowOverview(const bool oq_Show)
{
   const C_OscNode * const pc_Node =
      C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      std::vector<std::tuple<uint32_t, uint32_t, uint32_t> > c_AllLoggerJobElementsLocationCount;
      for (uint32_t u32_It = 0UL; u32_It < pc_Node->c_DataLoggerJobs.size(); ++u32_It)
      {
         uint32_t u32_LogJobIndex = u32_It;
         uint32_t u32_LocalElements = 0;
         uint32_t u32_RemoteElements = 0;
         this->mpc_Ui->pc_LogJobDataSelectionWidget->SetNodeDataLoggerJob(this->mu32_NodeIndex, u32_LogJobIndex);
         this->mpc_Ui->pc_LogJobDataSelectionWidget->GetElementLocationCount(u32_LocalElements, u32_RemoteElements);
         c_AllLoggerJobElementsLocationCount.emplace_back(u32_LogJobIndex, u32_LocalElements, u32_RemoteElements);
      }
      this->mpc_Ui->pc_LogJobEditWidget->setVisible(!oq_Show);
      this->mpc_Ui->pc_LogJobsOverviewWidget->setVisible(oq_Show);

      if (oq_Show == true)
      {
         this->mpc_Ui->pc_LogJobsOverviewWidget->UpdateData(c_AllLoggerJobElementsLocationCount);
      }
      //After reading all logjob location data, set the logjob index back which was using before
      this->mpc_Ui->pc_LogJobDataSelectionWidget->SetNodeDataLoggerJob(this->mu32_NodeIndex, this->mu32_LogJobIndex);
   }
}
