//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Handle most parts of copy paste implementation for scene (implementation)

   Handle most parts of copy paste implementation for scene

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "gitypes.h"
#include "TGLUtils.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_PuiSvDashboard.h"
#include "C_SyvClipBoardHelper.h"
#include "C_SyvDaCopyPasteManager.h"
#include "C_GiSvDaArrow.h"
#include "C_GiSvDaBoundary.h"
#include "C_GiSvDaChartBase.h"
#include "C_GiSvDaImageGroup.h"
#include "C_GiSvDaLabelBase.h"
#include "C_GiSvDaParam.h"
#include "C_GiSvDaPieChartBase.h"
#include "C_GiSvDaTableBase.h"
#include "C_GiSvDaSpinBoxBase.h"
#include "C_GiSvDaSliderBase.h"
#include "C_GiSvDaProgressBarBase.h"
#include "C_GiSvDaToggleBase.h"
#include "C_GiSvDaTextElement.h"
#include "C_SebUtil.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_OgeWiCustomMessage.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */
const QString C_SyvDaCopyPasteManager::hc_ClipBoardBaseTagName = "opensyde-system-definition";

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaCopyPasteManager::C_SyvDaCopyPasteManager(void) :
   C_SebBaseCopyPasteManager(),
   mu32_ViewIndex(0),
   mu32_DashboardIndex(0)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaCopyPasteManager::~C_SyvDaCopyPasteManager(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get data snapshot

   \return
   NULL No valid data snap shot found
   Else Valid data snapshot

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiBsElements * C_SyvDaCopyPasteManager::GetSnapshot(QWidget * const opc_Parent)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      bool q_InvalidElementsFound = false;
      std::vector<const C_PuiSvDbWidgetBase *> c_AllWidgets;
      //Check params validity
      for (uint32 u32_ItParam = 0; u32_ItParam < this->mc_LastKnownData.GetParams().size();)
      {
         bool q_Found = false;
         const C_PuiSvDbParam & rc_ParamWidget = this->mc_LastKnownData.GetParams()[u32_ItParam];
         for (uint32 u32_ItElement = 0; u32_ItElement < rc_ParamWidget.c_DataPoolElementsConfig.size();
              ++u32_ItElement)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config =
               rc_ParamWidget.c_DataPoolElementsConfig[u32_ItElement];
            if (rc_Config.c_ElementId.GetIsValid() == true)
            {
               if (pc_View->CheckNvmParamListUsage(stw_opensyde_core::C_OSCNodeDataPoolListId(rc_Config.c_ElementId.
                                                                                              u32_NodeIndex,
                                                                                              rc_Config.c_ElementId.
                                                                                              u32_DataPoolIndex,
                                                                                              rc_Config.c_ElementId.
                                                                                              u32_ListIndex)) ==
                   true)
               {
                  q_Found = true;
               }
            }
         }
         if (q_Found == true)
         {
            C_OgeWiCustomMessage c_Message(opc_Parent);
            //No iterator step because new element at current position
            this->mc_LastKnownData.DeleteWidget(u32_ItParam, C_PuiSvDbDataElement::ePARAM);
            c_Message.SetType(C_OgeWiCustomMessage::E_Type::eERROR);
            c_Message.SetHeading(C_GtGetText::h_GetText("Widget paste"));
            c_Message.SetDescription(C_GtGetText::h_GetText("Parametrization widget could not be pasted.\n"
                                                            "Another parametrization widget in this view is already "
                                                            "using at least one of the containing lists."));
            c_Message.Execute();
         }
         else
         {
            //Important iterator step
            ++u32_ItParam;
         }
      }
      //Check ID validity
      this->mc_LastKnownData.GetAllWidgetItems(c_AllWidgets);
      for (uint32 u32_ItWidget = 0; u32_ItWidget < c_AllWidgets.size(); ++u32_ItWidget)
      {
         const C_PuiSvDbWidgetBase * const pc_Widget = c_AllWidgets[u32_ItWidget];
         if (pc_Widget != NULL)
         {
            for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size();
                 ++u32_ItElement)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
               if (rc_Config.c_ElementId.GetIsValid() == true)
               {
                  if (C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                                 rc_Config.c_ElementId.
                                                                                 u32_DataPoolIndex,
                                                                                 rc_Config.c_ElementId.u32_ListIndex,
                                                                                 rc_Config.c_ElementId.
                                                                                 u32_ElementIndex) == NULL)
                  {
                     q_InvalidElementsFound = true;
                  }
               }
            }
         }
      }
      if (q_InvalidElementsFound == true)
      {
         C_OgeWiCustomMessage c_Message(opc_Parent);
         //Do not accept paste
         this->mc_LastKnownData.Clear();
         c_Message.SetType(C_OgeWiCustomMessage::E_Type::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Widget paste"));
         c_Message.SetDescription(C_GtGetText::h_GetText("Widgets could not be pasted.\n"
                                                         "An item index was found that does not mach any element in your SYSTEM DEFINITION."));
         c_Message.Execute();
      }
   }

   return &this->mc_LastKnownData;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Prepare call of CopyFromSceneToManager

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaCopyPasteManager::PrepareCopyFromSceneToManager(const uint32 ou32_ViewIndex,
                                                            const uint32 ou32_DashboardIndex)
{
   this->mu32_ViewIndex = ou32_ViewIndex;
   this->mu32_DashboardIndex = ou32_DashboardIndex;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Copy selected files to copy paste manager

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] orc_SelectedItems   Selected items to copy

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaCopyPasteManager::CopyFromSceneToManager(const QList<QGraphicsItem *> & orc_SelectedItems)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
      if (pc_Dashboard != NULL)
      {
         C_PuiSvDashboard c_Snapshot;
         uint32 u32_Index;
         const C_GiSvDaImageGroup * pc_Image;
         const C_GiSvDaBoundary * pc_Boundary;
         const C_GiSvDaArrow * pc_LineArrow;
         const C_GiSvDaTextElement * pc_TextElement;

         std::vector<const QGraphicsItem *> c_HandledItems;
         QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_Rails;

         //Clear data
         for (QList<QGraphicsItem *>::const_iterator c_ItItem = orc_SelectedItems.begin();
              c_ItItem != orc_SelectedItems.end();
              ++c_ItItem)
         {
            bool q_Handled = false;
            QGraphicsItem * const pc_CurItem = C_SebUtil::h_GetHighestParent(*c_ItItem);
            for (uint32 u32_ItHandledItems = 0; u32_ItHandledItems < c_HandledItems.size(); ++u32_ItHandledItems)
            {
               if (c_HandledItems[u32_ItHandledItems] == pc_CurItem)
               {
                  q_Handled = true;
               }
            }
            if (q_Handled == false)
            {
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               //lint -e{740}  no problem because of common base class
               C_PuiSvDbDataElement * const pc_Data = dynamic_cast<C_PuiSvDbDataElement *>(pc_CurItem);
               if (pc_Data != NULL)
               {
                  //Update core data
                  pc_Data->UpdateData();
                  {
                     //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                     const C_GiSvDaChartBase * const pc_Chart = dynamic_cast<const C_GiSvDaChartBase *>(pc_CurItem);
                     //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                     const C_GiSvDaLabelBase * const pc_Label = dynamic_cast<const C_GiSvDaLabelBase *>(pc_CurItem);
                     //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                     const C_GiSvDaParam * const pc_Param = dynamic_cast<const C_GiSvDaParam *>(pc_CurItem);
                     //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                     const C_GiSvDaPieChartBase * const pc_PieChart =
                        dynamic_cast<const C_GiSvDaPieChartBase *>(pc_CurItem);
                     //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                     const C_GiSvDaProgressBarBase * const pc_ProgressBar =
                        dynamic_cast<const C_GiSvDaProgressBarBase *>(pc_CurItem);
                     //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                     const C_GiSvDaSliderBase * const pc_Slider = dynamic_cast<const C_GiSvDaSliderBase *>(pc_CurItem);
                     //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                     const C_GiSvDaSpinBoxBase * const pc_SpinBox =
                        dynamic_cast<const C_GiSvDaSpinBoxBase *>(pc_CurItem);
                     //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                     const C_GiSvDaTableBase * const pc_Table = dynamic_cast<const C_GiSvDaTableBase *>(pc_CurItem);
                     //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                     const C_GiSvDaToggleBase * const pc_Toggle = dynamic_cast<const C_GiSvDaToggleBase *>(pc_CurItem);
                     if (pc_Chart != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_Chart->GetIndex());
                        const C_PuiSvDbChart * const pc_ChartData = pc_Dashboard->GetChart(u32_Index);
                        if (pc_ChartData != NULL)
                        {
                           tgl_assert(c_Snapshot.AddWidget(pc_ChartData, C_PuiSvDbDataElement::eCHART) == C_NO_ERR);
                        }
                     }
                     else if (pc_Label != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_Label->GetIndex());
                        const C_PuiSvDbLabel * const pc_LabelData = pc_Dashboard->GetLabel(u32_Index);
                        if (pc_LabelData != NULL)
                        {
                           tgl_assert(c_Snapshot.AddWidget(pc_LabelData, C_PuiSvDbDataElement::eLABEL) == C_NO_ERR);
                        }
                     }
                     else if (pc_Param != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_Param->GetIndex());
                        const C_PuiSvDbParam * const pc_ParamData = pc_Dashboard->GetParam(u32_Index);
                        if (pc_ParamData != NULL)
                        {
                           tgl_assert(c_Snapshot.AddWidget(pc_ParamData, C_PuiSvDbDataElement::ePARAM) == C_NO_ERR);
                        }
                     }
                     else if (pc_PieChart != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_PieChart->GetIndex());
                        const C_PuiSvDbPieChart * const pc_PieChartData = pc_Dashboard->GetPieChart(u32_Index);
                        if (pc_PieChartData != NULL)
                        {
                           tgl_assert(c_Snapshot.AddWidget(pc_PieChartData,
                                                           C_PuiSvDbDataElement::ePIE_CHART) == C_NO_ERR);
                        }
                     }
                     else if (pc_ProgressBar != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_ProgressBar->GetIndex());
                        const C_PuiSvDbProgressBar * const pc_ProgressBarData = pc_Dashboard->GetProgressBar(u32_Index);
                        if (pc_ProgressBarData != NULL)
                        {
                           tgl_assert(c_Snapshot.AddWidget(pc_ProgressBarData,
                                                           C_PuiSvDbDataElement::ePROGRESS_BAR) == C_NO_ERR);
                        }
                     }
                     else if (pc_Slider != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_Slider->GetIndex());
                        const C_PuiSvDbSlider * const pc_SliderData = pc_Dashboard->GetSlider(u32_Index);
                        if (pc_SliderData != NULL)
                        {
                           tgl_assert(c_Snapshot.AddWidget(pc_SliderData, C_PuiSvDbDataElement::eSLIDER) == C_NO_ERR);
                        }
                     }
                     else if (pc_SpinBox != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_SpinBox->GetIndex());
                        const C_PuiSvDbSpinBox * const pc_SpinBoxData = pc_Dashboard->GetSpinBox(u32_Index);
                        if (pc_SpinBoxData != NULL)
                        {
                           tgl_assert(c_Snapshot.AddWidget(pc_SpinBoxData,
                                                           C_PuiSvDbDataElement::eSPIN_BOX) == C_NO_ERR);
                        }
                     }
                     else if (pc_Table != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_Table->GetIndex());
                        const C_PuiSvDbTable * const pc_TableData = pc_Dashboard->GetTable(u32_Index);
                        if (pc_TableData != NULL)
                        {
                           tgl_assert(c_Snapshot.AddWidget(pc_TableData, C_PuiSvDbDataElement::eTABLE) == C_NO_ERR);
                        }
                     }
                     else if (pc_Toggle != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_Toggle->GetIndex());
                        const C_PuiSvDbToggle * const pc_ToggleData = pc_Dashboard->GetToggle(u32_Index);
                        if (pc_ToggleData != NULL)
                        {
                           tgl_assert(c_Snapshot.AddWidget(pc_ToggleData, C_PuiSvDbDataElement::eTOGGLE) == C_NO_ERR);
                        }
                     }
                     else
                     {
                        // copy the element
                        switch (pc_CurItem->type())
                        {
                        case msn_GRAPHICS_ITEM_TEXTELEMENT:
                           //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                           pc_TextElement = dynamic_cast<const C_GiSvDaTextElement *>(pc_CurItem);
                           if (pc_TextElement != NULL)
                           {
                              u32_Index = static_cast<uint32>(pc_TextElement->GetIndex());
                              if (u32_Index < pc_Dashboard->c_TextElements.size())
                              {
                                 c_Snapshot.c_TextElements.push_back(pc_Dashboard->c_TextElements[u32_Index]);
                              }
                           }
                           break;
                        case msn_GRAPHICS_ITEM_BOUNDARY:
                           //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                           pc_Boundary = dynamic_cast<const C_GiSvDaBoundary *>(pc_CurItem);
                           if (pc_Boundary != NULL)
                           {
                              u32_Index = static_cast<uint32>(pc_Boundary->GetIndex());
                              if (u32_Index < pc_Dashboard->c_Boundaries.size())
                              {
                                 c_Snapshot.c_Boundaries.push_back(pc_Dashboard->c_Boundaries[u32_Index]);
                              }
                           }
                           break;
                        case msn_GRAPHICS_ITEM_IMAGE:
                           //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                           pc_Image = dynamic_cast<const C_GiSvDaImageGroup *>(pc_CurItem);
                           if (pc_Image != NULL)
                           {
                              u32_Index = static_cast<uint32>(pc_Image->GetIndex());
                              if (u32_Index < pc_Dashboard->c_Images.size())
                              {
                                 c_Snapshot.c_Images.push_back(pc_Dashboard->c_Images[u32_Index]);
                              }
                           }
                           break;
                        case msn_GRAPHICS_ITEM_LINE_ARROW:
                           //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                           pc_LineArrow = dynamic_cast<const C_GiSvDaArrow *>(pc_CurItem);
                           if (pc_LineArrow != NULL)
                           {
                              u32_Index = static_cast<uint32>(pc_LineArrow->GetIndex());
                              if (u32_Index < pc_Dashboard->c_LineArrows.size())
                              {
                                 c_Snapshot.c_LineArrows.push_back(pc_Dashboard->c_LineArrows[u32_Index]);
                              }
                           }
                           break;
                        // TODO BAY: Add further elements to copy
                        default:
                           break;
                        }
                     }
                  }
                  c_HandledItems.push_back(pc_CurItem);
               }
            }
         }
         this->m_CalcOriginalPosition(&c_Snapshot);
         this->mu32_PasteCounter = 0;
         pc_View->GetRelevantReadRailAssigmentsForDashboard(c_Snapshot, c_Rails);

         //Copy to clip board
         C_SyvClipBoardHelper::h_StoreDashboardToClipboard(c_Snapshot, c_Rails,
                                                           C_SyvDaCopyPasteManager::hc_ClipBoardBaseTagName);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Indicator if copy paste manager has some content

   \return
   true: content
   false: no content

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvDaCopyPasteManager::CheckValidContentAndPrepareData(void)
{
   this->mc_LastKnownData.Clear();
   return (C_SyvClipBoardHelper::h_LoadDashboardFromClipboard(this->mc_LastKnownData,
                                                              C_SyvDaCopyPasteManager::hc_ClipBoardBaseTagName) ==
           C_NO_ERR);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Evaluate all items to get top left point

   \param[in] opc_Data Data

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaCopyPasteManager::m_CalcOriginalPosition(const C_PuiBsElements * const opc_Data)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const C_PuiSvDashboard * const pc_ExpectedData =
      dynamic_cast<const C_PuiSvDashboard * const>(opc_Data);

   C_SebBaseCopyPasteManager::m_CalcOriginalPosition(opc_Data);
   if (pc_ExpectedData != NULL)
   {
      uint32 u32_ItElem;
      const std::vector<C_PuiSvDbChart> & rc_Charts = pc_ExpectedData->GetCharts();
      const std::vector<C_PuiSvDbLabel> & rc_Labels = pc_ExpectedData->GetLabels();
      const std::vector<C_PuiSvDbPieChart> & rc_PieCharts = pc_ExpectedData->GetPieCharts();
      const std::vector<C_PuiSvDbProgressBar> & rc_ProgressBars = pc_ExpectedData->GetProgressBars();
      const std::vector<C_PuiSvDbSpinBox> & rc_SpinBoxs = pc_ExpectedData->GetSpinBoxes();
      const std::vector<C_PuiSvDbSlider> & rc_Sliders = pc_ExpectedData->GetSliders();
      const std::vector<C_PuiSvDbTable> & rc_Tabless = pc_ExpectedData->GetTables();
      const std::vector<C_PuiSvDbToggle> & rc_Toggles = pc_ExpectedData->GetToggles();

      for (u32_ItElem = 0; u32_ItElem < rc_Charts.size(); ++u32_ItElem)
      {
         const C_PuiSvDbChart & rc_Widget = rc_Charts[u32_ItElem];
         m_MinToOrgPos(rc_Widget.c_UIPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < rc_Labels.size(); ++u32_ItElem)
      {
         const C_PuiSvDbLabel & rc_Widget = rc_Labels[u32_ItElem];
         m_MinToOrgPos(rc_Widget.c_UIPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < rc_PieCharts.size(); ++u32_ItElem)
      {
         const C_PuiSvDbPieChart & rc_Widget = rc_PieCharts[u32_ItElem];
         m_MinToOrgPos(rc_Widget.c_UIPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < rc_ProgressBars.size(); ++u32_ItElem)
      {
         const C_PuiSvDbProgressBar & rc_Widget = rc_ProgressBars[u32_ItElem];
         m_MinToOrgPos(rc_Widget.c_UIPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < rc_SpinBoxs.size(); ++u32_ItElem)
      {
         const C_PuiSvDbSpinBox & rc_Widget = rc_SpinBoxs[u32_ItElem];
         m_MinToOrgPos(rc_Widget.c_UIPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < rc_Sliders.size(); ++u32_ItElem)
      {
         const C_PuiSvDbSlider & rc_Widget = rc_Sliders[u32_ItElem];
         m_MinToOrgPos(rc_Widget.c_UIPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < rc_Tabless.size(); ++u32_ItElem)
      {
         const C_PuiSvDbTable & rc_Widget = rc_Tabless[u32_ItElem];
         m_MinToOrgPos(rc_Widget.c_UIPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < rc_Toggles.size(); ++u32_ItElem)
      {
         const C_PuiSvDbToggle & rc_Widget = rc_Toggles[u32_ItElem];
         m_MinToOrgPos(rc_Widget.c_UIPosition);
      }
   }
}
