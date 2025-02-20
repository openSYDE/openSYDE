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
#include "C_SdNdeDalTabContentWidget.hpp"
#include "ui_C_SdNdeDalTabContentWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
   mpc_Ui(new Ui::C_SdNdeDalTabContentWidget)
{
   this->mpc_Ui->setupUi(this);

   connect(this->mpc_Ui->pc_LogJobPropertiesWidget, &C_SdNdeDalLogJobPropertiesWidget::SigReloadDataLoggerDataElements,
           this, &C_SdNdeDalTabContentWidget::ReloadDataLoggerDataElements);
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
/*! \brief  Trigger entire reload of data logger data elements section

   Necessary if data elements could have changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::ReloadDataLoggerDataElements()
{
   this->mpc_Ui->pc_LogJobDataSelectionWidget->ReloadDataElements();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new node index.

   \param[in]  ou32_NodeIndex    Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalTabContentWidget::SetNode(const uint32_t ou32_NodeIndex)
{
   // Forward node index
   // Table first as reloading might get triggered while initialising other widgets
   this->mpc_Ui->pc_LogJobDataSelectionWidget->SetNodeDataLoggerJob(ou32_NodeIndex,
                                                                    C_SdNdeDalTabContentWidget::mhu32_STATIC_LOG_JOB_INDEX);
   this->mpc_Ui->pc_LogJobsWidget->SetNode(ou32_NodeIndex);
   this->mpc_Ui->pc_LogJobPropertiesWidget->SetNodeDataLoggerJob(ou32_NodeIndex,
                                                                 C_SdNdeDalTabContentWidget::mhu32_STATIC_LOG_JOB_INDEX);
}
