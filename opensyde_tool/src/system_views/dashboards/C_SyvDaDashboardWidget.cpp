//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a concrete dashboard

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"

#include "C_SyvDaDashboardWidget.h"
#include "ui_C_SyvDaDashboardWidget.h"

#include "C_UsHandler.h"
#include "C_PuiSvHandler.h"
#include "C_OgeWiUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_ViewIndex Index of view
   \param[in]     ou32_DataIndex Index of dashboard
   \param[in]     orc_Name       Name of dashboard
   \param[in]     oq_Window      Flag if widget will be showed in a seperate window
   \param[in,out] opc_Parent     Optional pointer to parent

   \created     20.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvDaDashboardWidget::C_SyvDaDashboardWidget(const uint32 ou32_ViewIndex, const uint32 ou32_DataIndex,
                                               const QString & orc_Name, const bool oq_Window,
                                               QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaDashboardWidget),
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_DataIndex(ou32_DataIndex),
   mc_Name(orc_Name)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   QString c_ViewName = "";
   QString c_DashboardName = "";

   mpc_Ui->setupUi(this);

   this->setWindowTitle(orc_Name);

   // create scene for graphics view
   this->mpc_Scene = new C_SyvDaDashboardScene(this->mu32_ViewIndex, this->mu32_DataIndex, true);

   // configure scene
   this->mpc_Scene->setSceneRect(0.0, 0.0,
                                 static_cast<float64>(this->mpc_Ui->pc_GraphicsView->width()),
                                 static_cast<float64>(this->mpc_Ui->pc_GraphicsView->height()));
   this->mpc_Ui->pc_GraphicsView->SetSceneAndConnect(this->mpc_Scene);

   if (oq_Window == true)
   {
      this->layout()->setMargin(0);
   }

   // make all generic connects
   //Connect for tool tip
   connect(this->mpc_Scene, &C_SebScene::SigShowToolTip, this->mpc_Ui->pc_GraphicsView,
           &C_SebGraphicsView::ShowToolTip);
   connect(this->mpc_Scene, &C_SebScene::SigHideToolTip, this->mpc_Ui->pc_GraphicsView,
           &C_SebGraphicsView::HideToolTip);
   connect(this->mpc_Ui->pc_GraphicsView, &C_SebGraphicsView::SigShowToolTip, this->mpc_Scene,
           &C_SebScene::DisplayToolTip);
   //Interaction point scaling
   connect(this->mpc_Scene, &C_SebScene::SigTriggerUpdateTransform, this->mpc_Ui->pc_GraphicsView,
           &C_SebGraphicsView::UpdateTransform);
   //Error handling
   connect(this->mpc_Scene, &C_SyvDaDashboardScene::SigErrorChange, this, &C_SyvDaDashboardWidget::SigErrorChange);

   // Manual datapool element handling
   connect(this->mpc_Scene, &C_SyvDaDashboardScene::SigDataPoolWrite, this, &C_SyvDaDashboardWidget::SigDataPoolWrite);
   connect(this->mpc_Scene, &C_SyvDaDashboardScene::SigDataPoolRead, this, &C_SyvDaDashboardWidget::SigDataPoolRead);

   //Update all items with initial zoom & pos value
   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DataIndex);
      c_ViewName = pc_View->GetName();
      if (pc_Dashboard != NULL)
      {
         c_DashboardName = pc_Dashboard->GetName();
      }
   }
   this->mpc_Ui->pc_GraphicsView->SetZoomValue(C_UsHandler::h_GetInstance()->GetProjSvSetupView(
                                                  c_ViewName).GetDashboardSettings(
                                                  c_DashboardName).sn_SceneZoom, true);
   this->mpc_Ui->pc_GraphicsView->SetViewPos(C_UsHandler::h_GetInstance()->GetProjSvSetupView(
                                                c_ViewName).GetDashboardSettings(
                                                c_DashboardName).c_ScenePos);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     20.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvDaDashboardWidget::~C_SyvDaDashboardWidget(void)
{
   delete this->mpc_Ui;
   delete this->mpc_Scene;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the index of the view

   \return
   Index of the view

   \created     07.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
uint32 C_SyvDaDashboardWidget::GetViewIndex(void) const
{
   return this->mu32_ViewIndex;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the index of the dashboard

   \return
   Index of the dashboard

   \created     27.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
uint32 C_SyvDaDashboardWidget::GetDataIndex(void) const
{
   return this->mu32_DataIndex;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set data index

   \param[in] ou32_Value New data index

   \created     06.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardWidget::SetDataIndex(const uint32 ou32_Value)
{
   this->mu32_DataIndex = ou32_Value;
   this->mpc_Scene->SetDashboardIndex(this->mu32_DataIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the name of the dashboard

   \return
   Name of the dashboard

   \created     25.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_SyvDaDashboardWidget::GetName(void) const
{
   return this->mc_Name;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the edit mode

   \param[in]     oq_Active      Flag for edit mode

   \created     03.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardWidget::SetEditMode(const bool oq_Active)
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetDrawingBackground(oq_Active);
      this->mpc_Scene->SetEditMode(oq_Active);
   }
   this->mpc_Ui->pc_GraphicsView->SetDrawingBackground(oq_Active);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_DrawFrame, "NoBorder", true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the dark mode

   \param[in] oq_Active Dark mode active

   \created     02.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardWidget::SetDarkMode(const bool oq_Active)
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetDarkModeActive(oq_Active);
      this->mpc_Scene->SetDarkModeInitialized();
   }
   this->mpc_Ui->pc_GraphicsView->SetDarkMode(oq_Active);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save data

   \created     26.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardWidget::Save(void) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   // store configuration of the view
   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DataIndex);
      if (pc_Dashboard != NULL)
      {
         C_UsHandler::h_GetInstance()->SetProjSvDashboardScenePositionAndZoom(pc_View->GetName(),
                                                                              pc_Dashboard->GetName(),
                                                                              this->mpc_Ui->pc_GraphicsView->GetViewPos(),
                                                                              this->mpc_Ui->pc_GraphicsView->GetZoomValue());
      }
   }
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->Save();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Registers all relevant dashboard widgets at the associated data dealer

   \param[in]     orc_AllDataDealer    Reference to vector with all data dealer

   \created     29.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardWidget::RegisterWidgets(C_SyvComDriverDiag & orc_ComDriver) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if ((this->mpc_Scene != NULL) &&
       (pc_View != NULL))
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DataIndex);

      if ((pc_Dashboard != NULL) &&
          (pc_Dashboard->GetActive() == true))
      {
         this->mpc_Scene->RegisterWidgets(orc_ComDriver);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Information about the start or stop of a connection

   \param[in]  oq_Active      Flag if connection is active or not active now

   \created     01.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardWidget::ConnectionActiveChanged(const bool oq_Active) const
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->ConnectionActiveChanged(oq_Active);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates all values of all dashboard widgets

   \created     29.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardWidget::UpdateShowValues(void) const
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->UpdateShowValues();
      this->mpc_Ui->pc_GraphicsView->repaint();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle changes of transmission mode for any data element

   \created     11.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardWidget::UpdateTransmissionConfiguration(void) const
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->UpdateTransmissionConfiguration();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle manual user operation finished event

   \param[in] os32_Result Operation result
   \param[in] ou8_NRC     Negative response code, if any

   \created     09.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardWidget::HandleManualOperationFinished(const sint32 os32_Result, const uint8 ou8_NRC) const
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->HandleManualOperationFinished(os32_Result, ou8_NRC);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Signal all widgets which read rail element ID registrations failed

   \param[in]     orc_FailedIdRegisters    Failed IDs
   \param[in,out] orc_FailedIdErrorDetails Error details for element IDs which failed registration (if any)

   \created     13.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardWidget::SetErrorForFailedCyclicElementIdRegistrations(
   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_FailedIdRegisters,
   const std::vector<QString> & orc_FailedIdErrorDetails) const
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetErrorForFailedCyclicElementIdRegistrations(orc_FailedIdRegisters, orc_FailedIdErrorDetails);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set focus to scene

   \created     18.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardWidget::SetSceneFocus(void) const
{
   this->mpc_Ui->pc_GraphicsView->setFocus();
}
