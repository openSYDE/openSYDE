//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base widget for log jobs list

   Base widget for log jobs list

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SdNdeDalLogJobsWidget.hpp"
#include "ui_C_SdNdeDalLogJobsWidget.h"
#include "C_OscDataLoggerJob.hpp"
#include "C_OscDataLoggerJobProperties.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint32_t C_SdNdeDalLogJobsWidget::mhu32_STATIC_LOG_JOB_INDEX = 0UL;

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
   mu32_NodeIndex(0)
{
   this->mpc_Ui->setupUi(this);
   this->InitStaticNames();

   connect(this->mpc_Ui->pc_ChkBoxLogJob, &QCheckBox::stateChanged, this,
           &C_SdNdeDalLogJobsWidget::m_OnLogJobStateChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
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
   this->mpc_Ui->pc_BtnLogJobs->setText(C_GtGetText::h_GetText("Log Jobs"));
   this->mpc_Ui->pc_ChkBoxLogJob->setText("LogJob1");
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

   this->ReloadLogJobs();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reload log jobs on data change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::ReloadLogJobs(void) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
   const C_OscDataLoggerJob * const pc_Retval = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(mu32_NodeIndex,
                                                                                                  mhu32_STATIC_LOG_JOB_INDEX);

   if (pc_Retval != NULL)
   {
      this->mpc_Ui->pc_ChkBoxLogJob->setChecked(pc_Retval->q_IsEnabled);
   }

   if (pc_Node != NULL)
   {
      this->mpc_Ui->pc_ChkBoxLogJob->setVisible(pc_Node->c_Properties.q_XappSupport);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for Log job state change.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsWidget::m_OnLogJobStateChanged()
{
   const C_OscDataLoggerJob * const pc_Retval = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(
      this->mu32_NodeIndex,
      mhu32_STATIC_LOG_JOB_INDEX);

   if (pc_Retval != NULL)
   {
      if (this->mpc_Ui->pc_ChkBoxLogJob->isChecked() != pc_Retval->q_IsEnabled)
      {
         C_PuiSdHandler::h_GetInstance()->SetDataLoggerEnabled(mu32_NodeIndex, mhu32_STATIC_LOG_JOB_INDEX,
                                                               this->mpc_Ui->pc_ChkBoxLogJob->isChecked());
      }
   }
}
