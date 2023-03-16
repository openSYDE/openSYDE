//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Overview widget with current CANopen configuration of Manager and devices on the used interfaces

   Overview widget with current CANopen configuration of Manager and devices on the used interfaces

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_GtGetText.hpp"
#include "C_SdNdeCoOverviewWidget.hpp"
#include "ui_C_SdNdeCoOverviewWidget.h"
#include "C_SdNdeCoOverviewTableView.hpp"

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
C_SdNdeCoOverviewWidget::C_SdNdeCoOverviewWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeCoOverviewWidget)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   //Initial setup
   this->mpc_Ui->pc_TableView->setVisible(false);
   this->mpc_Ui->pc_LabelNoChannels->setVisible(false);

   //Connects
   connect(this->mpc_Ui->pc_TableView, &C_SdNdeCoOverviewTableView::SigInterfaceSelected, this,
           &C_SdNdeCoOverviewWidget::SigInterfaceSelected);
   connect(this->mpc_Ui->pc_TableView, &C_SdNdeCoOverviewTableView::SigDeviceSelected, this,
           &C_SdNdeCoOverviewWidget::SigDeviceSelected);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoOverviewWidget::~C_SdNdeCoOverviewWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewWidget::LoadUserSettings() const
{
   this->mpc_Ui->pc_TableView->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewWidget::SaveUserSettings() const
{
   this->mpc_Ui->pc_TableView->SaveUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewWidget::InitStaticNames() const
{
   this->mpc_Ui->pc_LabelNoChannels->setText(C_GtGetText::h_GetText("No CANopen data for node found."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewWidget::UpdateData(void)
{
   this->mpc_Ui->pc_TableView->UpdateData();
   this->m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewWidget::SetNodeIndex(const uint32_t ou32_NodeIndex)
{
   this->mpc_Ui->pc_TableView->SetNodeIndex(ou32_NodeIndex);
   this->UpdateData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update UI
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoOverviewWidget::m_UpdateUi()
{
   if (this->mpc_Ui->pc_TableView->GetCountRows() > 0)
   {
      this->mpc_Ui->pc_LabelNoChannels->setVisible(false);
      this->mpc_Ui->pc_VerticalSpacerBottom->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
      this->mpc_Ui->pc_TableView->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_TableView->setVisible(false);
      this->mpc_Ui->pc_VerticalSpacerBottom->changeSize(0, 40, QSizePolicy::Fixed, QSizePolicy::Expanding);
      this->mpc_Ui->pc_LabelNoChannels->setVisible(true);
   }

   this->updateGeometry();
}
