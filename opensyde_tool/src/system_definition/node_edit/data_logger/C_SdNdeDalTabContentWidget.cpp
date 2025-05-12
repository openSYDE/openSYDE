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
const uint32_t C_SdNdeDalTabContentWidget::mhu32_STATIC_LOG_JOB_INDEX = 0UL;

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
   mu32_NodeIndex(0)
{
   this->mpc_Ui->setupUi(this);

   // splitter
   this->mpc_Ui->pc_Splitter->SetMargins(0, 0);
   // if there is more space stretch right widget (i.e. index 1)
   this->mpc_Ui->pc_Splitter->setStretchFactor(0, 0);
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 10);

   this->InitStaticNames();

   this->mpc_Ui->pc_LogJobDataSelectionWidget->setVisible(false);
   this->mpc_Ui->pc_LogJobPropertiesWidget->setVisible(false);

   connect(this->mpc_Ui->pc_LogJobPropertiesWidget, &C_SdNdeDalLogJobPropertiesWidget::SigReloadDataLoggerDataElements,
           this, &C_SdNdeDalTabContentWidget::ReloadDataLogger);
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
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelNoData->setText(
      C_GtGetText::h_GetText("No log job declared.\n"
                             "Enable X-App support in node properties to configure data logger."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Trigger entire reload of data logger section

   Necessary if e.g. data elements could have changed or X-App support got disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::ReloadDataLogger()
{
   this->mpc_Ui->pc_LogJobDataSelectionWidget->ReloadDataElements();
   this->mpc_Ui->pc_LogJobsWidget->ReloadLogJobs();
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
   this->mpc_Ui->pc_LogJobDataSelectionWidget->SetNodeDataLoggerJob(ou32_NodeIndex,
                                                                    C_SdNdeDalTabContentWidget::mhu32_STATIC_LOG_JOB_INDEX);
   this->mpc_Ui->pc_LogJobsWidget->SetNode(ou32_NodeIndex);
   this->mpc_Ui->pc_LogJobPropertiesWidget->SetNodeDataLoggerJob(ou32_NodeIndex,
                                                                 C_SdNdeDalTabContentWidget::mhu32_STATIC_LOG_JOB_INDEX);

   this->m_UpdateUi();
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
         this->mpc_Ui->pc_LogJobDataSelectionWidget->setVisible(true);
         this->mpc_Ui->pc_LogJobPropertiesWidget->setVisible(true);
         this->mpc_Ui->pc_LabelNoData->setVisible(false);
      }
      else
      {
         this->mpc_Ui->pc_LogJobDataSelectionWidget->setVisible(false);
         this->mpc_Ui->pc_LogJobPropertiesWidget->setVisible(false);
         this->mpc_Ui->pc_LabelNoData->setVisible(true);
      }

      this->updateGeometry();
   }
}
