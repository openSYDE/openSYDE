//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a dashboard with a specific chart

   Widget for showing a dashboard with a specific chart

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SyvDaChaWidget.hpp"
#include "ui_C_SyvDaChaWidget.h"

#include "stwerrors.hpp"

#include "TglUtils.hpp"
#include "C_Uti.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_SyvDaPeBase.hpp"
#include "C_OscNodeDataPoolListElement.hpp"
#include "C_SyvDaPeDataElementBrowse.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint32_t C_SyvDaChaWidget::mhu32_MAXIMUM_DATA_ELEMENTS = 0xFFFFU;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      ou32_ViewIndex         Index of view
   \param[in]      ou32_DashboardIndex    Index of dashboard
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaChaWidget::C_SyvDaChaWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                   QWidget * const opc_Parent) :
   C_SyvDaDashboardContentBaseWidget(opc_Parent),
   C_PuiSvDbDataElementHandler(ou32_ViewIndex, ou32_DashboardIndex, 0, C_PuiSvDbDataElement::eTAB_CHART,
                               mhu32_MAXIMUM_DATA_ELEMENTS, true),
   mpc_Ui(new Ui::C_SyvDaChaWidget),
   mq_InitialStyleCall(true),
   mq_DarkMode(false),
   mq_DrawingActive(true),
   mq_EditMode(false),
   mq_IsConnected(false),
   mq_ManualOperationAbortTriggered(false),
   mu32_ManualOperationActionIndex(0U)
{
   this->mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_ChartWidget->Init(mhu32_MAXIMUM_DATA_ELEMENTS);
   this->m_LoadChartData();

   connect(this->mpc_Ui->pc_ChartWidget, &C_SyvDaChaPlotHandlerWidget::SigChartDataChanged,
           this, &C_SyvDaChaWidget::m_SetChangedChartData);
   connect(this->mpc_Ui->pc_ChartWidget, &C_SyvDaChaPlotHandlerWidget::SigAddSignalClicked,
           this, &C_SyvDaChaWidget::m_AddNewDataElement);

   // Manual read signals
   connect(this->mpc_Ui->pc_ChartWidget, &C_SyvDaChaPlotHandlerWidget::SigManualReadTriggered,
           this, &C_SyvDaChaWidget::m_ManualReadTriggered);
   connect(this->mpc_Ui->pc_ChartWidget, &C_SyvDaChaPlotHandlerWidget::SigManualReadAbortTriggered,
           this, &C_SyvDaChaWidget::m_ManualReadAbortTriggered);
   connect(this->mpc_Ui->pc_ChartWidget, &C_SyvDaChaPlotHandlerWidget::SigGetCurrentDashboardTabName,
           this, &C_SyvDaChaWidget::SigGetCurrentDashboardTabName);

   //Custom context menu
   this->m_SetupContextMenu();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_Action* and the Qt memory management
C_SyvDaChaWidget::~C_SyvDaChaWidget(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data index

   \param[in]  ou32_Value  New data index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::SetDashboardIndex(const uint32_t ou32_Value)
{
   C_PuiSvDbDataElement::SetDashboardIndex(ou32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the edit mode

   \param[in]  oq_Active   Flag for edit mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::SetEditMode(const bool oq_Active)
{
   this->mq_EditMode = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the dark mode

   \param[in]  oq_Active   Dark mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::SetDarkMode(const bool oq_Active)
{
   if ((this->mq_InitialStyleCall == true) ||
       (this->mq_DarkMode != oq_Active))
   {
      this->mq_InitialStyleCall = false;
      this->mq_DarkMode = oq_Active;
      if (oq_Active == true)
      {
         C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "Style", "OPENSYDE_DARK");
      }
      else
      {
         C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "Style", "OPENSYDE_BRIGHT");
      }

      this->mpc_Ui->pc_ChartWidget->SetDarkMode(oq_Active);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to activate or deactivate drawing of performance heavy widgets

   \param[in]  oq_Active   Flag if widgets should currently be drawn
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::SetDrawingActive(const bool oq_Active)
{
   this->mq_DrawingActive = oq_Active;
   this->mpc_Ui->pc_ChartWidget->SetDrawingActive(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::Save(void)
{
   this->UpdateData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Registers all relevant dashboard widgets at the associated data dealer

   \param[in]  orc_ComDriver  Com driver containing information about all data dealer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::RegisterWidgets(C_SyvComDriverDiag & orc_ComDriver)
{
   const std::vector<C_SyvComDataDealer *> & rc_AllDataDealer = orc_ComDriver.GetAllDataDealer();
   uint32_t u32_Counter;

   // Registration of Datapool elements
   for (u32_Counter = 0U; u32_Counter < rc_AllDataDealer.size(); ++u32_Counter)
   {
      rc_AllDataDealer[u32_Counter]->RegisterWidget(this);
   }

   // Registration of Datapool com signals
   orc_ComDriver.RegisterWidget(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active   Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::ConnectionActiveChanged(const bool oq_Active)
{
   this->mq_IsConnected = oq_Active;
   this->mpc_Ui->pc_ChartWidget->ConnectionActiveChanged(oq_Active);

   if (oq_Active == true)
   {
      this->m_UpdateDataPoolElementTimeoutAndValidFlag();
   }
   else
   {
      //Clear error
      this->mc_CommmunicationErrors.clear();
      this->mc_InvalidDlcSignals.clear();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates all values of all dashboard widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::UpdateShowValues(void)
{
   uint32_t u32_Counter;

   for (u32_Counter = 0U; u32_Counter < this->GetWidgetDataPoolElementCount(); ++u32_Counter)
   {
      int32_t s32_Return;
      QString c_LastValue;
      QVector<float64_t> oc_ScaledValues;
      QVector<uint32_t> oc_Timestamps;

      // Get all values
      s32_Return = this->m_GetAllValues(u32_Counter, c_LastValue, oc_ScaledValues, oc_Timestamps);

      if (s32_Return == C_NO_ERR)
      {
         if (this->mpc_Ui->pc_ChartWidget->IsPaused() == false)
         {
            C_PuiSvDbNodeDataPoolListElementId c_ElementId;

            // Get the associated id
            s32_Return = this->GetDataPoolElementIndex(u32_Counter, c_ElementId);

            if (s32_Return == C_NO_ERR)
            {
               // Update the chart
               this->mpc_Ui->pc_ChartWidget->AddGraphContent(c_ElementId, c_LastValue, oc_ScaledValues, oc_Timestamps);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle changes of transmission mode for any data element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::UpdateTransmissionConfiguration(void)
{
   this->UpdateElementTransmissionConfiguration();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle manual user operation finished event

   \param[in]  os32_Result    Operation result
   \param[in]  ou8_Nrc        Negative response code, if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc)
{
   if (this->mu32_ManualOperationActionIndex > 0U)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
      C_PuiSvDbNodeDataPoolListElementId c_ElementId;
      const int32_t s32_Return =
         this->GetDataPoolElementIndex(this->mu32_ManualOperationActionIndex - 1UL, c_ElementId);
      //Response error handling
      if (os32_Result != C_NO_ERR)
      {
         QString c_Description;
         QString c_Details;
         int32_t s32_MaxHeight = 0;

         this->m_GetErrorDescriptionForManualOperation(os32_Result, ou8_Nrc, c_Description, c_Details, s32_MaxHeight);

         osc_write_log_info("Manual operation",
                            static_cast<QString>("The C_SyvComDataDealer function ended with error code \"%1\"").arg(
                               C_Uti::h_StwError(os32_Result)).toStdString().c_str());

         if (s32_Return == C_NO_ERR)
         {
            //Change icon
            this->mc_CommmunicationErrors.remove(c_ElementId);
            this->mc_CommmunicationErrors.insert(c_ElementId, c_Description);
            this->m_UpdateErrorIcon();
         }
         else
         {
            c_Message.SetHeading(C_GtGetText::h_GetText("Transmission failure"));
            c_Message.SetDescription(c_Description);
            c_Message.SetDetails(c_Details);
            c_Message.Execute();
         }
      }
      else
      {
         //Clear error if successful
         if (s32_Return == C_NO_ERR)
         {
            //Change icon
            this->mc_CommmunicationErrors.remove(c_ElementId);
            this->m_UpdateErrorIcon();
         }
      }
      //Continue after error
      //Don't continue if user abort
      if (this->mq_ManualOperationAbortTriggered == true)
      {
         this->mq_ManualOperationAbortTriggered = false;
         this->m_ManualReadFinished();
      }
      else
      {
         this->m_ManualRead();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register cyclic transmission error for specific data element

   \param[in]  orc_WidgetDataPoolElementId   Affected data element
   \param[in]  ou8_ErrorCode                 Registered error code
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::RegisterDataPoolElementCyclicError(
   const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId, const uint8_t ou8_ErrorCode)
{
   if (this->mq_ReadItem == true)
   {
      QString c_Info;
      QString c_AdditionalInfo;

      if (ou8_ErrorCode == 0x22U)
      {
         c_AdditionalInfo = C_GtGetText::h_GetText("Access to element value failed (e.g. blocked by application)");
      }
      else
      {
         c_AdditionalInfo =
            static_cast<QString>(C_GtGetText::h_GetText("Unknown NRC: 0x%1")).arg(QString::number(ou8_ErrorCode, 16));
      }
      c_Info =
         static_cast<QString>(C_GtGetText::h_GetText("Cyclic service failed with error: %1")).arg(c_AdditionalInfo);
      this->mc_CommmunicationErrors.remove(orc_WidgetDataPoolElementId);
      this->mc_CommmunicationErrors.insert(orc_WidgetDataPoolElementId, c_Info);
      this->m_UpdateErrorIcon();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set focus to scene
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::SetFocus(void) const
{
   this->mpc_Ui->pc_ChartWidget->setFocus();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data from system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::LoadData(void)
{
   this->m_LoadChartData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::UpdateData(void)
{
   if (this->ms32_Index >= 0)
   {
      C_PuiSvDbTabChart c_Box = this->mpc_Ui->pc_ChartWidget->GetData();
      c_Box.e_DisplayStyle = C_PuiSvDbWidgetBase::eOPENSYDE;

      tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                     this->mu32_DashboardIndex,
                                                                     static_cast<uint32_t>(this->ms32_Index),
                                                                     &c_Box, this->me_Type) == C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in implementation of pure virtual function of base class
// C_PuiBsDataElement
void C_SyvDaChaWidget::DeleteData(void)
{
   // Nothing to do here. Deletion is done on dashboard layer in this case.
   // When deleting the tab chart, the entire dashboard tab is deleted always
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set DLC error for specified element id

   \param[in]  orc_ElementId  Element ID
   \param[in]  ou8_Dlc        DLC
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::SetErrorForInvalidDlc(const C_OscNodeDataPoolListElementId & orc_ElementId,
                                             const uint8_t ou8_Dlc)
{
   this->mc_InvalidDlcSignals.insert(orc_ElementId, ou8_Dlc);
   m_UpdateErrorIcon();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets current dashboard tab name

 *     \param[in]  orc_CurrentDashboardTabName   current chart tab name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::SetCurrentDashboardName(const QString & orc_CurrentDashboardTabName)
{
   this->mpc_Ui->pc_ChartWidget->SetCurrentDashboardTabName(orc_CurrentDashboardTabName);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for handling a new registered data element

   Called at the end of RegisterDataPoolElement after registration of a specific Datapool element

   \param[in]       ou32_WidgetDataPoolElementIndex     Index of new registered element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_OnDataElementRegistered(const uint32_t ou32_WidgetDataPoolElementIndex)
{
   // Get the associated id
   C_PuiSvDbNodeDataPoolListElementId c_ElementId;
   int32_t s32_Return = this->GetDataPoolElementIndex(ou32_WidgetDataPoolElementIndex, c_ElementId);

   if (s32_Return == C_NO_ERR)
   {
      C_PuiSvDbDataElementDisplayFormatterConfig c_Config;
      s32_Return = this->GetDataPoolElementFormatterConfig(ou32_WidgetDataPoolElementIndex, c_Config);

      if (s32_Return == C_NO_ERR)
      {
         this->mpc_Ui->pc_ChartWidget->SetDisplayFormatterConfig(c_ElementId, c_Config);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Event function if the Datapool element configuration was changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_DataPoolElementsChanged(void)
{
   if (this->GetWidgetDataPoolElementCount() > 0U)
   {
      bool q_InvalidElement;

      // Check for invalid elements
      QString c_Name;
      const bool q_ManualReadElement = this->m_CheckManualReadRequired();

      q_InvalidElement = (this->m_CheckHasValidElements(c_Name) == false) ||
                         (this->m_CheckHasAnyRequiredNodesActive() == false) ||
                         (this->m_CheckHasAnyRequiredBusesConnected() == false) ||
                         (this->m_CheckHasAnyRequiredNodesValidDashboardRouting() == false);

      if ((q_ManualReadElement == true) &&
          (q_InvalidElement == false))
      {
         // Check if element is a writing element with manual update or a reading element with manual trigger
         this->mpc_Ui->pc_ChartWidget->SetManualOperationPossible(true);
      }
      else
      {
         this->mpc_Ui->pc_ChartWidget->SetManualOperationPossible(false);
      }
   }
   else
   {
      this->mpc_Ui->pc_ChartWidget->SetManualOperationPossible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden key press release event slot

   \param[in,out]  opc_KeyEvent  Key event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   // Actions only allowed if service mode is deactivated
   if (C_PuiSvHandler::h_GetInstance()->GetServiceModeActive() == false)
   {
      if (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_F5))
      {
         this->mpc_Ui->pc_ChartWidget->RefreshColors();
      }
      else if (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Delete))
      {
         this->m_RemoveDataElement();
      }
      else if ((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
               ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Plus)) ||
                (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_BracketRight))))
      {
         this->m_AddNewDataElement();
      }
      else
      {
         // Nothing to do
      }
   }

   C_SyvDaDashboardContentBaseWidget::keyPressEvent(opc_KeyEvent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::paintEvent(QPaintEvent * const opc_Event)
{
   if (this->mq_DrawingActive == true)
   {
      stw::opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);
      QWidget::paintEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden double click event slot

   Here: open data element properties on double click

   \param[in,out]  opc_Event  Event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   if (this->mq_IsConnected == false)
   {
      const bool q_AtLeastOneElementExist = (this->mpc_Ui->pc_ChartWidget->GetCountGraphs() > 0);

      if (q_AtLeastOneElementExist == true)
      {
         // Check position of cursor
         const bool q_ElementUnderCursor = this->mpc_Ui->pc_ChartWidget->IsAnyDataSerieOnPosition(opc_Event->pos());

         if (q_ElementUnderCursor == true)
         {
            this->m_CallProperties();
            opc_Event->accept();
         }
      }
   }

   C_SyvDaDashboardContentBaseWidget::mouseDoubleClickEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load chart data from system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_LoadChartData(void)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   tgl_assert(pc_View != NULL);
   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
      tgl_assert(pc_Dashboard != NULL);
      if (pc_Dashboard != NULL)
      {
         uint32_t u32_ElementConfigCounter;
         const C_PuiSvDbTabChart & rc_Box = pc_Dashboard->GetTabChart();

         this->ClearDataPoolElements();
         for (uint32_t u32_It = 0; u32_It < rc_Box.c_DataPoolElementsConfig.size(); ++u32_It)
         {
            if (rc_Box.c_DataPoolElementsConfig[u32_It].c_ElementId.GetIsValid() == true)
            {
               this->RegisterDataPoolElement(rc_Box.c_DataPoolElementsConfig[u32_It].c_ElementId,
                                             rc_Box.c_DataPoolElementsConfig[u32_It].c_ElementScaling,
                                             rc_Box.c_DataPoolElementsConfig[u32_It].c_DisplayFormatter);
            }
         }

         this->mpc_Ui->pc_ChartWidget->SetData(rc_Box, this->mu32_ViewIndex);

         // Special case: The display formatter could not be set by m_OnDataElementRegistered in RegisterDataPoolElement
         // due to the call of SetData after it. They must be registered initial manually
         for (u32_ElementConfigCounter = 0U; u32_ElementConfigCounter < this->GetWidgetDataPoolElementCount();
              ++u32_ElementConfigCounter)
         {
            this->m_OnDataElementRegistered(u32_ElementConfigCounter);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu = new C_OgeContextMenu(this);

   this->mpc_ActionAdd = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Add data element(s)"),
                                                          this, &C_SyvDaChaWidget::m_AddNewDataElement,
                                                          static_cast<int32_t>(Qt::CTRL) +
                                                          static_cast<int32_t>(Qt::Key_Plus));
   this->mpc_ContextMenu->addSeparator();

   this->mpc_ActionConfigDataElement =
      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Data element properties"),
                                       this, &C_SyvDaChaWidget::m_CallProperties);
   this->mpc_ContextMenu->addSeparator();

   this->mpc_ActionRemove = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Remove"),
                                                             this, &C_SyvDaChaWidget::m_RemoveDataElement,
                                                             static_cast<int32_t>(Qt::Key_Delete));
   this->mpc_ActionRemoveAll = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Remove all"),
                                                                this, &C_SyvDaChaWidget::m_RemoveAllDataElements);

   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_SyvDaChaWidget::customContextMenuRequested, this,
           &C_SyvDaChaWidget::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   if (this->mq_IsConnected == false)
   {
      const bool q_AtLeastOneElementExist = (this->mpc_Ui->pc_ChartWidget->GetCountGraphs() > 0);

      if (q_AtLeastOneElementExist == true)
      {
         // Check position of cursor. Selection is already done of C_SyvDaChaDataItemWidget itself
         const bool q_ElementUnderCursor = this->mpc_Ui->pc_ChartWidget->IsAnyDataSerieOnPosition(orc_Pos);

         if (q_ElementUnderCursor == true)
         {
            const bool q_ServiceModeActive = C_PuiSvHandler::h_GetInstance()->GetServiceModeActive();

            // Handle the service mode
            this->mpc_ActionAdd->setEnabled(!q_ServiceModeActive);
            this->mpc_ActionRemove->setEnabled(!q_ServiceModeActive);
            this->mpc_ActionRemoveAll->setEnabled(!q_ServiceModeActive);
            this->mpc_ActionConfigDataElement->setEnabled(!q_ServiceModeActive);

            // Configure context menu
            this->mpc_ActionRemove->setVisible(q_AtLeastOneElementExist);
            this->mpc_ActionRemoveAll->setVisible(q_AtLeastOneElementExist);
            this->mpc_ActionConfigDataElement->setVisible(q_AtLeastOneElementExist);

            this->mpc_ContextMenu->popup(this->mapToGlobal(orc_Pos));
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new data element to the chart
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_AddNewDataElement(void)
{
   // Get all registered elements for marking as used in the tree
   std::vector<C_PuiSvDbNodeDataPoolListElementId> c_RegisteredElements;
   this->m_GetAllRegisteredElements(c_RegisteredElements);

   {
      const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
      C_SyvDaPeDataElementBrowse * const pc_Dialog = new C_SyvDaPeDataElementBrowse(*c_New, this->mu32_ViewIndex, true,
                                                                                    false, false, true, true, false,
                                                                                    &c_RegisteredElements);

      //Resize
      c_New->SetSize(QSize(800, 800));

      if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
      {
         const std::vector<C_PuiSvDbNodeDataPoolListElementId> c_DataElements = pc_Dialog->GetSelectedDataElements();
         //Cursor
         QApplication::setOverrideCursor(Qt::WaitCursor);
         if (c_DataElements.size() > 0)
         {
            uint32_t u32_Counter;

            for (u32_Counter = 0U; u32_Counter < c_DataElements.size(); ++u32_Counter)
            {
               if (c_DataElements[u32_Counter].GetIsValid() == true)
               {
                  C_PuiSvDbDataElementScaling c_Scaling;

                  const C_OscNodeDataPoolListElement * const pc_Element =
                     C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(
                        c_DataElements[u32_Counter].u32_NodeIndex,
                        c_DataElements[u32_Counter].u32_DataPoolIndex,
                        c_DataElements[u32_Counter].u32_ListIndex,
                        c_DataElements[u32_Counter].u32_ElementIndex);
                  if (pc_Element != NULL)
                  {
                     // Get the original scaling configuration as initialization
                     c_Scaling.f64_Factor = pc_Element->f64_Factor;
                     c_Scaling.f64_Offset = pc_Element->f64_Offset;
                     c_Scaling.c_Unit = pc_Element->c_Unit.c_str();
                  }

                  tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckAndHandleNewElement(
                                c_DataElements[u32_Counter]) == C_NO_ERR);

                  if (this->mpc_Ui->pc_ChartWidget->AddNewGraph(c_DataElements[u32_Counter], c_Scaling) != C_NO_ERR)
                  {
                     break;
                  }

                  this->m_RegisterDataElementRail(c_DataElements[u32_Counter]);

                  // Apply to the project data
                  tgl_assert(this->m_SetChangedChartData() == C_NO_ERR);

                  this->RegisterDataPoolElement(c_DataElements[u32_Counter], c_Scaling,
                                                C_PuiSvDbDataElementDisplayFormatter());

                  //Handle new display region
                  this->mpc_Ui->pc_ChartWidget->FitDefault();
               }
            }
         }
         //Signal for error change
         Q_EMIT (this->SigDataElementsChanged());
         //Cursor
         QApplication::restoreOverrideCursor();
      }

      //Force update
      this->mq_InitialStyleCall = true;
      // Update the whole styling, because of new added gui elements
      this->SetDarkMode(this->mq_DarkMode);

      if (c_New != NULL)
      {
         pc_Dialog->SaveUserSettings();
         pc_Dialog->PrepareCleanUp();
         c_New->HideOverlay();
         c_New->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes a data element from the chart
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_RemoveDataElement(void)
{
   if (this->GetWidgetDataPoolElementCount() > 0U)
   {
      bool q_Return;
      C_PuiSvDbNodeDataPoolListElementId c_ElementId;
      q_Return = this->mpc_Ui->pc_ChartWidget->RemoveCurrentGraph(c_ElementId);

      if (q_Return == true)
      {
         const C_PuiSvData * pc_View;

         // Apply to the project data
         tgl_assert(this->m_SetChangedChartData() == C_NO_ERR);

         this->RemoveDataPoolElement(c_ElementId);

         //Remove read rail assignments as necessary
         pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

         if ((pc_View != NULL) &&
             (c_ElementId.GetIsValid() == true) &&
             (c_ElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT) &&
             (pc_View->CheckReadUsage(c_ElementId) == false))
         {
            tgl_assert(C_PuiSvHandler::h_GetInstance()->RemoveViewReadRailItem(this->mu32_ViewIndex,
                                                                               c_ElementId) == C_NO_ERR);
         }

         //Handle new display region
         this->mpc_Ui->pc_ChartWidget->FitDefault();

         //Signal for error change
         Q_EMIT (this->SigDataElementsChanged());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes all data elements from the chart
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_RemoveAllDataElements(void)
{
   uint32_t u32_NumberDataElementsLeft = this->GetWidgetDataPoolElementCount();

   if (u32_NumberDataElementsLeft > 0U)
   {
      //Remove read rail assignments as necessary
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      while (u32_NumberDataElementsLeft > 0U)
      {
         C_PuiSvDbNodeDataPoolListElementId c_ElementId;
         this->mpc_Ui->pc_ChartWidget->RemoveSpecificGraph(0U, c_ElementId);

         if ((pc_View != NULL) &&
             (c_ElementId.GetIsValid() == true) &&
             (pc_View->CheckReadUsage(c_ElementId) == false))
         {
            tgl_assert(C_PuiSvHandler::h_GetInstance()->RemoveViewReadRailItem(this->mu32_ViewIndex,
                                                                               c_ElementId) == C_NO_ERR);
         }

         --u32_NumberDataElementsLeft;
      }

      // Apply to the project data
      tgl_assert(this->m_SetChangedChartData() == C_NO_ERR);

      this->ClearDataPoolElements();

      //Handle new display region
      this->mpc_Ui->pc_ChartWidget->FitDefault();

      //Signal for error change
      Q_EMIT (this->SigDataElementsChanged());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register a specific data element for a rail

   \param[in]  orc_DataPoolElementId   Element Id for registration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_RegisterDataElementRail(
   const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId) const
{
   if (orc_DataPoolElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
   {
      const C_OscNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(orc_DataPoolElementId.u32_NodeIndex,
                                                                    orc_DataPoolElementId.u32_DataPoolIndex,
                                                                    orc_DataPoolElementId.u32_ListIndex,
                                                                    orc_DataPoolElementId.u32_ElementIndex);

      if (pc_Element != NULL)
      {
         C_PuiSvReadDataConfiguration c_Config;
         c_Config.u8_RailIndex = 1;
         if (pc_Element->GetArray() || (((pc_Element->GetType() == C_OscNodeDataPoolContent::eFLOAT64) ||
                                         (pc_Element->GetType() == C_OscNodeDataPoolContent::eSINT64)) ||
                                        (pc_Element->GetType() == C_OscNodeDataPoolContent::eUINT64)))
         {
            c_Config.e_TransmissionMode = C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER;
         }
         else
         {
            c_Config.e_TransmissionMode = C_PuiSvReadDataConfiguration::eTM_CYCLIC;
         }
         c_Config.InitDefaultThreshold(pc_Element->c_MinValue, pc_Element->c_MaxValue);
         C_PuiSvHandler::h_GetInstance()->AddViewReadRailItem(this->mu32_ViewIndex, orc_DataPoolElementId,
                                                              c_Config);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call properties for chart element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_CallProperties(void)
{
   uint32_t u32_ConfigIndex;

   if (this->mpc_Ui->pc_ChartWidget->GetCurrentGraph(u32_ConfigIndex) == true)
   {
      C_PuiSvDbTabChart & rc_Box = this->mpc_Ui->pc_ChartWidget->GetData();

      if (u32_ConfigIndex < rc_Box.c_DataPoolElementsConfig.size())
      {
         C_PuiSvDbNodeDataPoolListElementId c_ElementId;
         C_PuiSvDbDataElementScaling c_Scaling;
         C_PuiSvDbDataElementDisplayFormatter c_FormatterConfig;
         QString c_DisplayName;
         const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
         C_SyvDaPeBase * pc_Dialog;
         c_ElementId = rc_Box.c_DataPoolElementsConfig[u32_ConfigIndex].c_ElementId;
         c_Scaling = rc_Box.c_DataPoolElementsConfig[u32_ConfigIndex].c_ElementScaling;
         c_FormatterConfig = rc_Box.c_DataPoolElementsConfig[u32_ConfigIndex].c_DisplayFormatter;
         c_DisplayName = rc_Box.c_DataPoolElementsConfig[u32_ConfigIndex].c_DisplayName;

         pc_Dialog = new C_SyvDaPeBase(*c_New, this->mu32_ViewIndex, this->mu32_DashboardIndex, "Chart",
                                       c_ElementId, c_Scaling, true, c_FormatterConfig, true, this->mq_DarkMode, false,
                                       false, c_DisplayName);

         pc_Dialog->SetTheme(rc_Box.e_DisplayStyle);

         //Resize
         c_New->SetSize(C_SyvDaPeBase::h_GetPopupSizeWithoutDesignAndPreview());

         if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
         {
            QString c_WidgetName;
            C_PuiSvDbNodeDataElementConfig c_Tmp;

            c_Tmp.c_ElementId = c_ElementId;
            tgl_assert(c_ElementId == pc_Dialog->GetDataElementId());
            c_Tmp.c_ElementScaling = pc_Dialog->GetScalingInformation();
            c_Tmp.c_DisplayFormatter = pc_Dialog->GetFormatterInformation();
            c_Tmp.c_DisplayName = pc_Dialog->GetDisplayName();
            rc_Box.c_DataPoolElementsConfig[u32_ConfigIndex] = c_Tmp;

            //Apply
            this->RemoveDataPoolElement(c_ElementId);
            this->RegisterDataPoolElement(c_Tmp.c_ElementId, c_Tmp.c_ElementScaling, c_Tmp.c_DisplayFormatter);

            if (c_Tmp.c_DisplayName.compare("") == 0)
            {
               const C_OscNodeDataPoolListElement * const pc_OscElement =
                  C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(
                     c_ElementId.u32_NodeIndex, c_ElementId.u32_DataPoolIndex, c_ElementId.u32_ListIndex,
                     c_ElementId.u32_ElementIndex);

               const C_OscNodeDataPool * const pc_Datapool =
                  C_PuiSdHandler::h_GetInstance()->GetOscDataPool(c_ElementId.u32_NodeIndex,
                                                                  c_ElementId.u32_DataPoolIndex);

               if (pc_OscElement != NULL)
               {
                  if (pc_Datapool != NULL)
                  {
                     if ((pc_Datapool->e_Type == C_OscNodeDataPool::eHALC) ||
                         (pc_Datapool->e_Type == C_OscNodeDataPool::eHALC_NVM))
                     {
                        c_WidgetName = C_PuiSvHandler::h_GetShortNamespace(c_ElementId);
                     }
                     else
                     {
                        c_WidgetName = pc_OscElement->c_Name.c_str();
                     }
                  }
               }
            }
            else
            {
               c_WidgetName = c_Tmp.c_DisplayName;
            }

            this->mpc_Ui->pc_ChartWidget->SetScaling(u32_ConfigIndex, c_WidgetName, c_Tmp.c_ElementScaling);

            tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckAndHandleNewElement(c_Tmp.c_ElementId) == C_NO_ERR);
            tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                           this->mu32_DashboardIndex,
                                                                           static_cast<uint32_t>(this->ms32_Index),
                                                                           &rc_Box,
                                                                           this->me_Type) == C_NO_ERR);
         }
         Q_EMIT (this->SigTriggerUpdateTransmissionConfiguration());
         if (c_New != NULL)
         {
            c_New->HideOverlay();
            c_New->deleteLater();
         }
      } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply new chart content

   WARNING: Data element changes have to trigger RegisterDataElement

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaChaWidget::m_SetChangedChartData(void)
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->ms32_Index >= 0)
   {
      const C_PuiSvDbTabChart & rc_Data = this->mpc_Ui->pc_ChartWidget->GetData();

      tgl_assert(rc_Data.c_DataPoolElementsActive.size() ==
                 rc_Data.c_DataPoolElementsConfig.size());
      tgl_assert(rc_Data.c_DataPoolElementsActive.size() ==
                 rc_Data.c_DataPoolElementsColorIndex.size());
      for (uint32_t u32_ItEl = 0UL; u32_ItEl < rc_Data.c_DataPoolElementsConfig.size(); ++u32_ItEl)
      {
         const C_PuiSvDbNodeDataElementConfig & rc_Config = rc_Data.c_DataPoolElementsConfig[u32_ItEl];
         tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckAndHandleNewElement(rc_Config.c_ElementId) == C_NO_ERR);
      }
      s32_Retval = C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                       static_cast<uint32_t>(this->ms32_Index),
                                                                       &rc_Data, C_PuiSvDbDataElement::eTAB_CHART);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update the error icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_UpdateErrorIcon(void)
{
   for (uint32_t u32_ItItem = 0UL; u32_ItItem < this->GetWidgetDataPoolElementCount(); ++u32_ItItem)
   {
      C_PuiSvDbNodeDataPoolListElementId c_Id;
      if (this->GetDataPoolElementIndex(u32_ItItem, c_Id) == C_NO_ERR)
      {
         if (c_Id.GetIsValid())
         {
            bool q_FoundError = false;
            QMap<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, QString>::const_iterator c_ItError;
            QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId, uint8_t>::const_iterator c_ItSigError;

            //Errors
            for (c_ItError = this->mc_CommmunicationErrors.begin(); c_ItError != this->mc_CommmunicationErrors.end();
                 ++c_ItError)
            {
               if (c_ItError.key() == c_Id)
               {
                  //Set error
                  this->mpc_Ui->pc_ChartWidget->UpdateError(u32_ItItem, c_ItError.value(), true, true);

                  //Signal error
                  q_FoundError = true;
               }
            }
            for (c_ItSigError = this->mc_InvalidDlcSignals.begin(); c_ItSigError != this->mc_InvalidDlcSignals.end();
                 ++c_ItSigError)
            {
               if (c_ItSigError.key() == c_Id)
               {
                  const QString c_Text = static_cast<QString>(C_GtGetText::h_GetText("%1 had invalid DLC %2.")).
                                         arg(C_PuiSdUtil::h_GetSignalNamespace(c_ItSigError.key())).
                                         arg(QString::number(c_ItSigError.value()));
                  //Set error
                  this->mpc_Ui->pc_ChartWidget->UpdateError(u32_ItItem, c_Text, false, true);
                  //Signal error
                  q_FoundError = true;
               }
            }
            if (q_FoundError == false)
            {
               //Clear error
               this->mpc_Ui->pc_ChartWidget->UpdateError(u32_ItItem, "", false, false);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Triggering a manual read of data elements configured with "On Trigger"
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_ManualReadTriggered(void)
{
   this->mq_ManualOperationAbortTriggered = false;

   this->m_ManualRead();

   QApplication::setOverrideCursor(Qt::WaitCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger next manual read operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_ManualRead(void)
{
   if (this->mu32_ManualOperationActionIndex < this->GetWidgetDataPoolElementCount())
   {
      C_PuiSvDbNodeDataPoolListElementId c_ElementId;

      const int32_t s32_Return = this->GetDataPoolElementIndex(this->mu32_ManualOperationActionIndex, c_ElementId);

      tgl_assert(s32_Return == C_NO_ERR);
      if (s32_Return == C_NO_ERR)
      {
         //Always iterate to next element!
         ++this->mu32_ManualOperationActionIndex;
         if (this->m_CheckIsOnTrigger(c_ElementId))
         {
            if ((c_ElementId.GetIsValid() == true) &&
                (this->m_CheckNodeActive(c_ElementId.u32_NodeIndex) == true) &&
                (this->m_CheckNodeHasAnyRequiredValidDashboardRouting(c_ElementId.u32_NodeIndex) == true))
            {
               //-1 because we already prepared for the next element!
               if (this->m_CheckElementAlreadyRead(this->mu32_ManualOperationActionIndex - 1UL, c_ElementId) == false)
               {
                  Q_EMIT (this->SigDataPoolRead(c_ElementId, NULL));
               }
               else
               {
                  //Skip to next element
                  this->m_ManualRead();
               }
            }
            else
            {
               //Skip to next element
               this->m_ManualRead();
            }
         }
         else
         {
            //Skip to next element
            this->m_ManualRead();
         }
      }
      else
      {
         //Unexpected
         this->m_ManualReadFinished();
      }
   }
   else
   {
      //Last element reached or write element
      this->m_ManualReadFinished();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Triggering an abortion of the current manual read of data elements configured with "On Trigger"
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_ManualReadAbortTriggered(void)
{
   this->mq_ManualOperationAbortTriggered = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Final step of manual operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaWidget::m_ManualReadFinished(void)
{
   //Reset current state
   this->mu32_ManualOperationActionIndex = 0U;

   this->mpc_Ui->pc_ChartWidget->ManualReadFinished();

   QApplication::restoreOverrideCursor();
}
