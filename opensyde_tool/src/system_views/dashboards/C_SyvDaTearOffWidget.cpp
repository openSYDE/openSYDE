//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a concrete instance of C_SyvDaDashboardWidget in a seperated window

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QHBoxLayout>

#include "stwerrors.h"
#include "C_SyvUtil.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSvHandler.h"
#include "C_SyvDaTearOffWidget.h"
#include "C_SyvClipBoardHelper.h"
#include "ui_C_SyvDaTearOffWidget.h"

#include "C_GtGetText.h"
#include "C_OgeWiCustomMessage.h"
#include "C_OgePopUpDialog.h"
#include "C_SyvDaDashboardTabProperties.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_ViewIndex Index of view
   \param[in]     ou32_DataIndex Index of dashboard
   \param[in]     orc_Name       Name of dashboard
   \param[in,out] opc_Widget     Optional dashboard widget to integrate instead of creating a new one
   \param[in,out] opc_Parent     Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaTearOffWidget::C_SyvDaTearOffWidget(const uint32 ou32_ViewIndex, const stw_types::uint32 ou32_DataIndex,
                                           const QString & orc_Name, C_SyvDaDashboardWidget * const opc_Widget,
                                           QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaTearOffWidget)
{
   const C_PuiSvData * pc_View;
   QHBoxLayout * pc_Layout;

   mpc_Ui->setupUi(this);

   //Style error label
   this->mpc_Ui->pc_ErrorLabelTitle->SetForegroundColor(24);
   this->mpc_Ui->pc_ErrorLabelTitle->SetFontPixel(14, false, true);
   this->mpc_Ui->pc_ErrorLabelIcon->SetSvg("://images/Error_iconV2.svg");

   //Handle button icons
   this->mpc_Ui->pc_PbConfirm->SetSvg("://images/main_page_and_navi_bar/IconEdit.svg");

   //Remove debug label
   this->mpc_Ui->pc_GroupBoxButtons->setTitle("");

   pc_Layout = new QHBoxLayout(this->mpc_Ui->pc_DashboardContainerWidget);
   pc_Layout->setSpacing(0);
   pc_Layout->setMargin(0);

   //Replace internal widget
   if (opc_Widget == NULL)
   {
      this->mpc_Dashboard = new C_SyvDaDashboardWidget(ou32_ViewIndex, ou32_DataIndex, orc_Name, true, this);
   }
   else
   {
      this->mpc_Dashboard = opc_Widget;
      this->mpc_Dashboard->layout()->setMargin(0);
      this->mpc_Dashboard->setParent(this);
      //Explicit show necessary for this use-case
      this->mpc_Dashboard->show();
   }
   pc_Layout->addWidget(this->mpc_Dashboard);

   this->InitText();

   //Hide cancel button
   this->mpc_Ui->pc_PbCancel->setVisible(false);

   //Init tab
   this->mpc_Ui->pc_WidgetTab->DeactivateAdditionalActions();
   this->mpc_Ui->pc_WidgetTab->SetCloseButtonVisibility(false);
   this->mpc_Ui->pc_WidgetTab->SetCurrent(true);
   pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);
   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(ou32_DataIndex);
      if (pc_Dashboard != NULL)
      {
         this->mpc_Ui->pc_WidgetTab->SetText(pc_Dashboard->GetName());
         this->mpc_Ui->pc_WidgetTab->SetToolTip(pc_Dashboard->GetName(), pc_Dashboard->GetComment());
         this->mpc_Ui->pc_WidgetTab->SetActive(pc_Dashboard->GetActive());
      }
   }

   //Error check
   m_CheckError();
   connect(this->mpc_Dashboard, &C_SyvDaDashboardWidget::SigErrorChange, this, &C_SyvDaTearOffWidget::m_CheckError);
   connect(this->mpc_Dashboard, &C_SyvDaDashboardWidget::SigErrorChange, this, &C_SyvDaTearOffWidget::SigErrorChange);

   // Connect buttons
   connect(this->mpc_Ui->pc_PbConfirm, &stw_opensyde_gui_elements::C_OgePubSystemCommissioningEdit::clicked,
           this, &C_SyvDaTearOffWidget::SigConfirmClicked);
   connect(this->mpc_Ui->pc_PbCancel, &stw_opensyde_gui_elements::C_OgePubSystemCommissioningEdit::clicked,
           this, &C_SyvDaTearOffWidget::SigCancelClicked);
   //Connect tab
   connect(this->mpc_Ui->pc_WidgetTab, &C_OgeWiDashboardTab::SigActiveChanged, this,
           &C_SyvDaTearOffWidget::m_OnActiveChange);
   connect(this->mpc_Ui->pc_WidgetTab, &C_OgeWiDashboardTab::SigCopyAction, this,
           &C_SyvDaTearOffWidget::m_OnCopy);
   connect(this->mpc_Ui->pc_WidgetTab, &C_OgeWiDashboardTab::SigEditPropertiesAction, this,
           &C_SyvDaTearOffWidget::m_OnEditProperties);

   connect(this->mpc_Dashboard, &C_SyvDaDashboardWidget::SigTriggerUpdateTransmissionConfiguration, this,
           &C_SyvDaTearOffWidget::SigTriggerUpdateTransmissionConfiguration);

   // Manual datapool element handling
   connect(this->mpc_Dashboard, &C_SyvDaDashboardWidget::SigDataPoolWrite, this,
           &C_SyvDaTearOffWidget::SigDataPoolWrite);
   connect(this->mpc_Dashboard, &C_SyvDaDashboardWidget::SigDataPoolRead, this,
           &C_SyvDaTearOffWidget::SigDataPoolRead);
   connect(this->mpc_Dashboard, &C_SyvDaDashboardWidget::SigNvmReadList, this,
           &C_SyvDaTearOffWidget::SigNvmReadList);

   //lint -e{429}  no memory leak because of the parent of pc_Layout and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaTearOffWidget::~C_SyvDaTearOffWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_Dashboard and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::InitText(void) const
{
   this->mpc_Ui->pc_PbConfirm->setText(C_GtGetText::h_GetText("Edit"));
   this->mpc_Ui->pc_PbCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the index of the dashboard

   \return
   Index of the dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvDaTearOffWidget::GetIndex(void) const
{
   return this->mpc_Dashboard->GetDataIndex();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data index

   \param[in] ou32_Value New data index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::SetDataIndex(const uint32 ou32_Value)
{
   this->mpc_Dashboard->SetDataIndex(ou32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the name of the dashboard

   \return
   Name of the dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaTearOffWidget::GetName() const
{
   return this->mpc_Dashboard->GetName();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get integrated widget

   \return
   Current integrated widget
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaDashboardWidget * C_SyvDaTearOffWidget::GetWidget(void)
{
   return this->mpc_Dashboard;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the edit mode

   \param[in]     oq_Active      Flag for edit mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::SetEditMode(const bool oq_Active)
{
   this->mpc_Ui->pc_BackgroundWidget->SetEditBackground(oq_Active);

   this->mpc_Dashboard->SetEditMode(oq_Active);

   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_GroupBox, "Edit", oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the dark mode

   \param[in] oq_Active Dark mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::SetDarkMode(const bool oq_Active)
{
   if (this->mpc_Dashboard != NULL)
   {
      this->mpc_Dashboard->SetDarkMode(oq_Active);
   }
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_GroupBox, "DarkMode", oq_Active);
   this->mpc_Ui->pc_WidgetTab->SetDarkMode(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::Save(void) const
{
   if (this->mpc_Dashboard != NULL)
   {
      this->mpc_Dashboard->Save();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle manual user operation finished event

   \param[in] os32_Result Operation result
   \param[in] ou8_NRC     Negative response code, if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::HandleManualOperationFinished(const sint32 os32_Result, const uint8 ou8_NRC) const
{
   if (this->mpc_Dashboard != NULL)
   {
      this->mpc_Dashboard->HandleManualOperationFinished(os32_Result, ou8_NRC);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal all widgets which read rail element ID registrations failed

   \param[in]     orc_FailedIdRegisters    Failed IDs
   \param[in,out] orc_FailedIdErrorDetails Error details for element IDs which failed registration (if any)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::SetErrorForFailedCyclicElementIdRegistrations(
   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_FailedIdRegisters,
   const std::vector<QString> & orc_FailedIdErrorDetails) const
{
   if (this->mpc_Dashboard != NULL)
   {
      this->mpc_Dashboard->SetErrorForFailedCyclicElementIdRegistrations(orc_FailedIdRegisters,
                                                                         orc_FailedIdErrorDetails);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Registers all relevant dashboard widgets at the associated data dealer

   \param[in]     orc_AllDataDealer    Reference to vector with all data dealer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::RegisterWidgets(C_SyvComDriverDiag & orc_ComDriver) const
{
   if (this->mpc_Dashboard != NULL)
   {
      this->mpc_Dashboard->RegisterWidgets(orc_ComDriver);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in] oq_Active        Flag if connection is active or not active now
   \param[in] oq_WidgetTabOnly Optional flag if tab widget should be adapted to the connection change
                                (use if scene was already changed)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::ConnectionActiveChanged(const bool oq_Active, const bool oq_WidgetTabOnly) const
{
   if ((this->mpc_Dashboard != NULL) && (oq_WidgetTabOnly == false))
   {
      this->mpc_Dashboard->ConnectionActiveChanged(oq_Active);
   }
   this->mpc_Ui->pc_WidgetTab->SetInteractive(!oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates all values of all dashboard widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::UpdateShowValues(void) const
{
   if (this->mpc_Dashboard != NULL)
   {
      this->mpc_Dashboard->UpdateShowValues();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle changes of transmission mode for any data element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::UpdateTransmissionConfiguration(void) const
{
   if (this->mpc_Dashboard != NULL)
   {
      this->mpc_Dashboard->UpdateTransmissionConfiguration();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::paintEvent(QPaintEvent * const opc_Event)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);

   QWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten close event slot

   \param[in,out] opc_Event    Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::closeEvent(QCloseEvent * const opc_Event)
{
   // do not really close the widget. It has to "go back" to the tab widget
   opc_Event->ignore();

   this->setWindowFlags(Qt::Widget);

   QWidget::closeEvent(opc_Event);

   Q_EMIT this->SigWidgetComeBack(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle active flag changed

   \param[in] opc_Source Signal source widget
   \param[in] oq_Active  New active flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::m_OnActiveChange(C_OgeWiDashboardTab * const opc_Source, const bool oq_Active)
{
   if (this->mpc_Dashboard != NULL)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mpc_Dashboard->GetViewIndex());

      Q_UNUSED(opc_Source)
      if (pc_View != NULL)
      {
         const stw_types::uint32 u32_DataIndex = this->mpc_Dashboard->GetDataIndex();
         if (C_PuiSvHandler::h_GetInstance()->SetDashboardActive(this->mpc_Dashboard->GetViewIndex(), u32_DataIndex,
                                                                 oq_Active) == C_NO_ERR)
         {
            Q_EMIT this->SigChanged();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy dashboard content

   \param[in] opc_Source Signal source widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::m_OnCopy(C_OgeWiDashboardTab * const opc_Source) const
{
   Q_UNUSED(opc_Source)
   if (this->mpc_Dashboard != NULL)
   {
      const C_PuiSvData * pc_View;

      this->mpc_Dashboard->Save();
      pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mpc_Dashboard->GetViewIndex());

      if (pc_View != NULL)
      {
         const C_PuiSvDashboard * pc_Dashboard = pc_View->GetDashboard(this->mpc_Dashboard->GetDataIndex());
         if (pc_Dashboard != NULL)
         {
            QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_Rails;
            pc_View->GetRelevantReadRailAssigmentsForDashboard(*pc_Dashboard, c_Rails);
            C_SyvClipBoardHelper::h_StoreDashboardToClipboard(*pc_Dashboard, c_Rails, "Tab");
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Open popup to edit dashboard tab properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::m_OnEditProperties(C_OgeWiDashboardTab * const opc_Source)
{
   if ((this->mpc_Dashboard != NULL) && (opc_Source != NULL))
   {
      const uint32 u32_DashboardIndex = this->mpc_Dashboard->GetDataIndex();

      // show popup dialog
      QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
      C_SyvDaDashboardTabProperties * pc_Dialog = new C_SyvDaDashboardTabProperties(*c_New, "DASHBOARD TAB",
                                                                                    u32_DashboardIndex,
                                                                                    mpc_Dashboard->GetViewIndex());

      if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
      {
         const uint32 u32_ViewIndex = this->mpc_Dashboard->GetViewIndex();

         if ((C_PuiSvHandler::h_GetInstance()->SetDashboardName(
                 u32_ViewIndex, u32_DashboardIndex, pc_Dialog->GetDashboardTabName()) == C_NO_ERR) &&
             (C_PuiSvHandler::h_GetInstance()->SetDashboardComment(
                 u32_ViewIndex, u32_DashboardIndex, pc_Dialog->GetDashboardTabComment()) == C_NO_ERR))
         {
            Q_EMIT this->SigChanged();
            //Apply text
            opc_Source->SetText(pc_Dialog->GetDashboardTabName());
            opc_Source->SetToolTip(pc_Dialog->GetDashboardTabName(), pc_Dialog->GetDashboardTabComment());
         }
      }

      if (c_New != NULL)
      {
         c_New->HideOverlay();
      }
      //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Perform error check
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTearOffWidget::m_CheckError(void) const
{
   if (this->mpc_Dashboard != NULL)
   {
      QString c_ErrorTextHeading;
      QString c_ErrorText;
      QString c_ErrorTextTooltip;
      const bool q_ViewSetupError = C_SyvUtil::h_CheckViewSetupError(
         this->mpc_Dashboard->GetViewIndex(), c_ErrorTextHeading, c_ErrorText, c_ErrorTextTooltip);

      if (q_ViewSetupError == true)
      {
         this->mpc_Ui->pc_ErrorLabelIcon->SetToolTipInformation("", c_ErrorTextTooltip, C_NagToolTip::eERROR);
         this->mpc_Ui->pc_ErrorLabelTitle->setText(c_ErrorTextHeading);
         this->mpc_Ui->pc_ErrorLabelTitle->SetToolTipInformation("", c_ErrorTextTooltip, C_NagToolTip::eERROR);
         this->mpc_Ui->pc_ErrorLabel->SetCompleteText(c_ErrorText, c_ErrorTextTooltip);
         this->mpc_Ui->pc_GroupBoxErrorContent->setVisible(true);
      }
      else
      {
         this->mpc_Ui->pc_GroupBoxErrorContent->setVisible(false);
      }
   }
}
