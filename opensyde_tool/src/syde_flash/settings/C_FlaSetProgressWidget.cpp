//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Settings progress widget

   Settings progress widget

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_GtGetText.hpp"
#include "C_UsHandler.hpp"

#include "C_FlaSetProgressWidget.hpp"
#include "ui_C_FlaSetProgressWidget.h"
//#include "C_FlaOgeTebProgressLog.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

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
C_FlaSetProgressWidget::C_FlaSetProgressWidget(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_FlaSetProgressWidget)
{
   this->mpc_Ui->setupUi(this);

   this->SetBackgroundColor(5);

   //Progress initial text
   this->mpc_Ui->pc_TextBrowser->setReadOnly(true);
   // initialize title widget
   this->mpc_Ui->pc_WiHeader->SetTitle(C_GtGetText::h_GetText("Progress Log"));
   this->mpc_Ui->pc_WiHeader->SetIcon("://images/IconProgressLog.svg");
   this->mpc_Ui->pc_WiHeader->SetToggle(false);
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigExpandSection,
           this, &C_FlaSetProgressWidget::m_OnExpand);
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigHide,
           this, &C_FlaSetProgressWidget::SigHide);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaSetProgressWidget::~C_FlaSetProgressWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear progress
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetProgressWidget::ClearProgress(void)
{
   this->mpc_Ui->pc_TextBrowser->clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load all user settings.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetProgressWidget::LoadUserSettings() const
{
   this->mpc_Ui->pc_WiHeader->SetExpanded(C_UsHandler::h_GetInstance()->GetWiProgressExpanded());
   this->m_OnExpand(C_UsHandler::h_GetInstance()->GetWiProgressExpanded());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare widget for menu view in collapsed state resp. expanded view.

   \param[in]  oq_Expand   true: prepare for expanded, false: prepare for collapsed menu
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetProgressWidget::PrepareForExpanded(const bool oq_Expand) const
{
   if (oq_Expand == false)
   {
      this->mpc_Ui->pc_WiContent->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_WiContent->setVisible(C_UsHandler::h_GetInstance()->GetWiProgressExpanded());
   }

   // hide arrow button
   this->mpc_Ui->pc_WiHeader->ShowExpandButton(oq_Expand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show progress

   \param[in]  orc_Text    Text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetProgressWidget::ShowProgress(const QString & orc_Text)
{
   this->mpc_Ui->pc_TextBrowser->append(orc_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of expand signal.

   \param[in]  oq_Expand   true: expand; false: collapse
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetProgressWidget::m_OnExpand(const bool oq_Expand) const
{
   this->mpc_Ui->pc_WiContent->setVisible(oq_Expand);
   C_UsHandler::h_GetInstance()->SetWiProgressExpanded(oq_Expand);
}
