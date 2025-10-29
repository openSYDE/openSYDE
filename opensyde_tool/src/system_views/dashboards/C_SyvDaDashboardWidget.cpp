//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a concrete dashboard

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "TglUtils.hpp"

#include "C_SyvDaDashboardWidget.hpp"
#include "ui_C_SyvDaDashboardWidget.h"

#include "C_PuiSvHandler.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_SyvDaDashboardSceneWidget.hpp"
#include "C_SyvDaChaWidget.hpp"

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

   \param[in]      ou32_ViewIndex        Index of view
   \param[in]      ou32_DashboardIndex   Index of dashboard
   \param[in]      orc_Name              Name of dashboard
   \param[in]      oq_Window             Flag if widget will be showed in a separate window
   \param[in,out]  opc_Parent            Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaDashboardWidget::C_SyvDaDashboardWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                               const QString & orc_Name, const bool oq_Window,
                                               QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaDashboardWidget),
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_DashboardIndex(ou32_DashboardIndex),
   mc_Name(orc_Name)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   bool q_InitSceneWidget = true;

   this->mpc_Ui->setupUi(this);

   tgl_assert(pc_View != NULL);
   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
      tgl_assert(pc_Dashboard != NULL);
      if (pc_Dashboard != NULL)
      {
         if (pc_Dashboard->GetType() == C_PuiSvDashboard::eCHART)
         {
            q_InitSceneWidget = false;
         }
      }
   }

   if (q_InitSceneWidget == true)
   {
      this->m_InitSceneWidget();
   }
   else
   {
      this->m_InitChartWidget();
   }

   this->setWindowTitle(orc_Name);

   if (oq_Window == true)
   {
      this->layout()->setContentsMargins(0, 0, 0, 0);
   }

   //Error handling
   connect(this->mpc_Content, &C_SyvDaDashboardContentBaseWidget::SigErrorChange, this,
           &C_SyvDaDashboardWidget::SigErrorChange);

   connect(this->mpc_Content, &C_SyvDaDashboardContentBaseWidget::SigTriggerUpdateTransmissionConfiguration, this,
           &C_SyvDaDashboardWidget::SigTriggerUpdateTransmissionConfiguration);

   // Manual datapool element handling
   connect(this->mpc_Content, &C_SyvDaDashboardContentBaseWidget::SigDataPoolWrite, this,
           &C_SyvDaDashboardWidget::SigDataPoolWrite);
   connect(this->mpc_Content, &C_SyvDaDashboardContentBaseWidget::SigDataPoolRead, this,
           &C_SyvDaDashboardWidget::SigDataPoolRead);
   connect(this->mpc_Content, &C_SyvDaDashboardContentBaseWidget::SigNvmReadList, this,
           &C_SyvDaDashboardWidget::SigNvmReadList);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_Content and the Qt memory management
C_SyvDaDashboardWidget::~C_SyvDaDashboardWidget(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the index of the view

   \return
   Index of the view
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvDaDashboardWidget::GetViewIndex(void) const
{
   return this->mu32_ViewIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the index of the dashboard

   \return
   Index of the dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvDaDashboardWidget::GetDashboardIndex(void) const
{
   return this->mu32_DashboardIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data index

   \param[in]  ou32_Value  New data index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardWidget::SetDashboardIndex(const uint32_t ou32_Value)
{
   this->mu32_DashboardIndex = ou32_Value;
   this->mpc_Content->SetDashboardIndex(this->mu32_DashboardIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the name of the dashboard

   \return
   Name of the dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaDashboardWidget::GetName(void) const
{
   return this->mc_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets current chart tab name

 *     \param[in]  orc_CurrentDashboardTabName   current chart tab name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardWidget::SetCurrentDashboardTabName(const QString & orc_CurrentDashboardTabName) const
{
   this->mpc_Content->SetCurrentDashboardName(orc_CurrentDashboardTabName);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the edit mode

   \param[in]  oq_Active   Flag for edit mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardWidget::SetEditMode(const bool oq_Active)
{
   this->mpc_Content->SetEditMode(oq_Active);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_DrawFrame, "NoBorder", true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the dark mode

   \param[in]  oq_Active   Dark mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardWidget::SetDarkMode(const bool oq_Active)
{
   this->mpc_Content->SetDarkMode(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to activate or deactivate drawing of performance heavy widgets

   \param[in]  oq_Active   Flag if widgets should currently be drawn
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardWidget::SetDrawingActive(const bool oq_Active) const
{
   this->mpc_Content->SetDrawingActive(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardWidget::Save(void) const
{
   this->mpc_Content->Save();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Registers all relevant dashboard widgets at the associated data dealer

   \param[in]  orc_ComDriver  Com driver containing information about all data dealer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardWidget::RegisterWidgets(C_SyvComDriverDiag & orc_ComDriver) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);

      if ((pc_Dashboard != NULL) && (pc_Dashboard->GetActive() == true))
      {
         this->mpc_Content->RegisterWidgets(orc_ComDriver);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active   Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardWidget::ConnectionActiveChanged(const bool oq_Active) const
{
   this->mpc_Content->ConnectionActiveChanged(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates all values of all dashboard widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardWidget::UpdateShowValues(void) const
{
   this->mpc_Content->UpdateShowValues();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle changes of transmission mode for any data element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardWidget::UpdateTransmissionConfiguration(void) const
{
   this->mpc_Content->UpdateTransmissionConfiguration();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle manual user operation finished event

   \param[in]  os32_Result    Operation result
   \param[in]  ou8_Nrc        Negative response code, if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardWidget::HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc) const
{
   this->mpc_Content->HandleManualOperationFinished(os32_Result, ou8_Nrc);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set focus to scene
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardWidget::SetFocus(void) const
{
   this->mpc_Content->setFocus();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Short function description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardWidget::m_InitSceneWidget(void)
{
   this->mpc_Content = new C_SyvDaDashboardSceneWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex, this);
   this->mpc_Ui->pc_DrawFrameLayout->addWidget(this->mpc_Content);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Short function description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardWidget::m_InitChartWidget(void)
{
   this->mpc_Content = new C_SyvDaChaWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex, this);
   this->mpc_Ui->pc_DrawFrameLayout->addWidget(this->mpc_Content);
   connect(this->mpc_Content, &C_SyvDaDashboardContentBaseWidget::SigGetCurrentDashboardTabName, this,
           &C_SyvDaDashboardWidget::SigGetCurrentDashboardTabName);
}
