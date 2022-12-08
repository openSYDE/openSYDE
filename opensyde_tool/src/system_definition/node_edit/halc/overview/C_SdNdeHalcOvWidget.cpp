//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC overview widget

   HALC overview widget

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_GtGetText.hpp"
#include "C_SdNdeHalcOvWidget.hpp"
#include "ui_C_SdNdeHalcOvWidget.h"

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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcOvWidget::C_SdNdeHalcOvWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeHalcOvWidget)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   //Initial setup
   this->mpc_Ui->pc_TableView->setVisible(false);
   this->mpc_Ui->pc_LabelNoChannels->setVisible(false);

   //Connects
   connect(this->mpc_Ui->pc_TableView, &C_SdNdeHalcOvTableView::SigChannelSelected, this,
           &C_SdNdeHalcOvWidget::m_OnChannelSelected);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcOvWidget::~C_SdNdeHalcOvWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvWidget::LoadUserSettings() const
{
   this->mpc_Ui->pc_TableView->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvWidget::SaveUserSettings() const
{
   this->mpc_Ui->pc_TableView->SaveUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelNoChannels->setText(C_GtGetText::h_GetText(
                                                "No channels declared. \n"
                                                "Load the hardware description file via 'Select' button."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvWidget::UpdateData(void)
{
   this->mpc_Ui->pc_TableView->UpdateData();

   this->m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvWidget::SetNodeIndex(const uint32_t ou32_NodeIndex)
{
   this->mpc_Ui->pc_TableView->SetNodeIndex(ou32_NodeIndex);

   this->m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update UI
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvWidget::m_UpdateUi(void)
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On channel selected

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvWidget::m_OnChannelSelected(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                              const bool oq_UseChannelIndex)
{
   Q_EMIT (this->SigChannelSelected(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex));
}
