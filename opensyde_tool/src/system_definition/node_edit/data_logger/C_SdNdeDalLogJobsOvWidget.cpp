//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       LogJobs Overview Widget

   LogJobs Overview Widget

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SdNdeDalLogJobsOvWidget.hpp"
#include "ui_C_SdNdeDalLogJobsOvWidget.h"
#include "C_GtGetText.hpp"
#include "C_SdNdeDalTabContentWidget.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobsOvWidget::C_SdNdeDalLogJobsOvWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDalLogJobsOvWidget)
{
   this->mpc_Ui->setupUi(this);
   this->m_InitStaticNames();
   //Initial setup
   this->mpc_Ui->pc_TableView->setVisible(false);
   this->mpc_Ui->pc_LabelNoLogJobs->setVisible(false);

   connect(this->mpc_Ui->pc_TableView, &C_SdNdeDalLogJobsOvTableView::SigLoadSelectedLogJob,
           this, &C_SdNdeDalLogJobsOvWidget::SigLoadSelectedLogJob);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobsOvWidget::~C_SdNdeDalLogJobsOvWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data

   \param[in]  orc_AllLoggerJobElementsLocationCount     vector holds all Logjobs Index, Local elements count, remote elements count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvWidget::UpdateData(const std::vector<std::tuple<uint32_t, uint32_t,
                                                                        uint32_t> > & orc_AllLoggerJobElementsLocationCount)
{
   this->mpc_Ui->pc_TableView->UpdateData(orc_AllLoggerJobElementsLocationCount);
   this->m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvWidget::LoadUserSettings() const
{
   this->mpc_Ui->pc_TableView->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvWidget::SaveUserSettings() const
{
   this->mpc_Ui->pc_TableView->SaveUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvWidget::SetNodeIndex(const uint32_t ou32_NodeIndex)
{
   this->mpc_Ui->pc_TableView->SetNodeIndex(ou32_NodeIndex);
   this->m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvWidget::m_InitStaticNames() const
{
   this->mpc_Ui->pc_LabelNoLogJobs->setText(C_GtGetText::h_GetText(
                                               "No LogJobs declared. \n"
                                               "Add any via the '+' button."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update UI
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvWidget::m_UpdateUi()
{
   if (this->mpc_Ui->pc_TableView->GetCountRows() > 0)
   {
      this->mpc_Ui->pc_LabelNoLogJobs->setVisible(false);
      this->mpc_Ui->pc_TableView->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_TableView->setVisible(false);
      this->mpc_Ui->pc_LabelNoLogJobs->setVisible(true);
   }

   this->updateGeometry();
}
