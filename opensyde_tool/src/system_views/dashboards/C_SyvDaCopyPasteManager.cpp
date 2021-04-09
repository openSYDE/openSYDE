//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle most parts of copy paste implementation for scene (implementation)

   Handle most parts of copy paste implementation for scene

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
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

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvDaCopyPasteManager::hc_ClipBoardBaseTagName = "opensyde-system-definition";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaCopyPasteManager::C_SyvDaCopyPasteManager(void) :
   C_SebBaseCopyPasteManager(),
   mu32_ViewIndex(0),
   mu32_DashboardIndex(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaCopyPasteManager::~C_SyvDaCopyPasteManager(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data snapshot

   \param[in,out]  opc_Parent    Optional pointer to parent

   \return
   NULL No valid data snap shot found
   Else Valid data snapshot
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiBsElements * C_SyvDaCopyPasteManager::GetSnapshot(QWidget * const opc_Parent)
{
   C_SyvDaCopyPasteManager::h_AdaptCopyDataForPaste(this->mc_LastKnownData, this->mu32_ViewIndex, opc_Parent);

   return &this->mc_LastKnownData;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare call of CopyFromSceneToManager

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaCopyPasteManager::PrepareCopyFromSceneToManager(const uint32 ou32_ViewIndex,
                                                            const uint32 ou32_DashboardIndex)
{
   this->mu32_ViewIndex = ou32_ViewIndex;
   this->mu32_DashboardIndex = ou32_DashboardIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy selected files to copy paste manager

   \param[in]  orc_SelectedItems       Selected items to copy
   \param[in]  orc_NormalizedZValues   Normalized Z values for all copied items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaCopyPasteManager::CopyFromSceneToManager(const QList<QGraphicsItem *> & orc_SelectedItems,
                                                     const QMap<const QGraphicsItem *, float64> & orc_NormalizedZValues)
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
               C_PuiSvDbDataElement * const pc_Data = dynamic_cast<C_PuiSvDbDataElement *>(pc_CurItem);
               if (pc_Data != NULL)
               {
                  //Update core data
                  pc_Data->UpdateData();
                  {
                     const C_GiSvDaLabelBase * const pc_Label = dynamic_cast<const C_GiSvDaLabelBase *>(pc_CurItem);

                     const C_GiSvDaParam * const pc_Param = dynamic_cast<const C_GiSvDaParam *>(pc_CurItem);

                     const C_GiSvDaPieChartBase * const pc_PieChart =
                        dynamic_cast<const C_GiSvDaPieChartBase *>(pc_CurItem);

                     const C_GiSvDaProgressBarBase * const pc_ProgressBar =
                        dynamic_cast<const C_GiSvDaProgressBarBase *>(pc_CurItem);

                     const C_GiSvDaSliderBase * const pc_Slider = dynamic_cast<const C_GiSvDaSliderBase *>(pc_CurItem);

                     const C_GiSvDaSpinBoxBase * const pc_SpinBox =
                        dynamic_cast<const C_GiSvDaSpinBoxBase *>(pc_CurItem);

                     const C_GiSvDaTableBase * const pc_Table = dynamic_cast<const C_GiSvDaTableBase *>(pc_CurItem);

                     const C_GiSvDaToggleBase * const pc_Toggle = dynamic_cast<const C_GiSvDaToggleBase *>(pc_CurItem);
                     if (pc_Label != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_Label->GetIndex());
                        const C_PuiSvDbLabel * const pc_LabelData = pc_Dashboard->GetLabel(u32_Index);
                        if (pc_LabelData != NULL)
                        {
                           C_PuiSvDbLabel c_Tmp = *pc_LabelData;
                           C_SebBaseCopyPasteManager::mh_HandleZValueBox(*c_ItItem, orc_NormalizedZValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp, C_PuiSvDbDataElement::eLABEL) == C_NO_ERR);
                        }
                     }
                     else if (pc_Param != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_Param->GetIndex());
                        const C_PuiSvDbParam * const pc_ParamData = pc_Dashboard->GetParam(u32_Index);
                        if (pc_ParamData != NULL)
                        {
                           C_PuiSvDbParam c_Tmp = *pc_ParamData;
                           C_SebBaseCopyPasteManager::mh_HandleZValueBox(*c_ItItem, orc_NormalizedZValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp, C_PuiSvDbDataElement::ePARAM) == C_NO_ERR);
                        }
                     }
                     else if (pc_PieChart != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_PieChart->GetIndex());
                        const C_PuiSvDbPieChart * const pc_PieChartData = pc_Dashboard->GetPieChart(u32_Index);
                        if (pc_PieChartData != NULL)
                        {
                           C_PuiSvDbPieChart c_Tmp = *pc_PieChartData;
                           C_SebBaseCopyPasteManager::mh_HandleZValueBox(*c_ItItem, orc_NormalizedZValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp,
                                                           C_PuiSvDbDataElement::ePIE_CHART) == C_NO_ERR);
                        }
                     }
                     else if (pc_ProgressBar != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_ProgressBar->GetIndex());
                        const C_PuiSvDbProgressBar * const pc_ProgressBarData = pc_Dashboard->GetProgressBar(u32_Index);
                        if (pc_ProgressBarData != NULL)
                        {
                           C_PuiSvDbProgressBar c_Tmp = *pc_ProgressBarData;
                           C_SebBaseCopyPasteManager::mh_HandleZValueBox(*c_ItItem, orc_NormalizedZValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp,
                                                           C_PuiSvDbDataElement::ePROGRESS_BAR) == C_NO_ERR);
                        }
                     }
                     else if (pc_Slider != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_Slider->GetIndex());
                        const C_PuiSvDbSlider * const pc_SliderData = pc_Dashboard->GetSlider(u32_Index);
                        if (pc_SliderData != NULL)
                        {
                           C_PuiSvDbSlider c_Tmp = *pc_SliderData;
                           C_SebBaseCopyPasteManager::mh_HandleZValueBox(*c_ItItem, orc_NormalizedZValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp, C_PuiSvDbDataElement::eSLIDER) == C_NO_ERR);
                        }
                     }
                     else if (pc_SpinBox != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_SpinBox->GetIndex());
                        const C_PuiSvDbSpinBox * const pc_SpinBoxData = pc_Dashboard->GetSpinBox(u32_Index);
                        if (pc_SpinBoxData != NULL)
                        {
                           C_PuiSvDbSpinBox c_Tmp = *pc_SpinBoxData;
                           C_SebBaseCopyPasteManager::mh_HandleZValueBox(*c_ItItem, orc_NormalizedZValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp,
                                                           C_PuiSvDbDataElement::eSPIN_BOX) == C_NO_ERR);
                        }
                     }
                     else if (pc_Table != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_Table->GetIndex());
                        const C_PuiSvDbTable * const pc_TableData = pc_Dashboard->GetTable(u32_Index);
                        if (pc_TableData != NULL)
                        {
                           C_PuiSvDbTable c_Tmp = *pc_TableData;
                           C_SebBaseCopyPasteManager::mh_HandleZValueBox(*c_ItItem, orc_NormalizedZValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp, C_PuiSvDbDataElement::eTABLE) == C_NO_ERR);
                        }
                     }
                     else if (pc_Toggle != NULL)
                     {
                        u32_Index = static_cast<uint32>(pc_Toggle->GetIndex());
                        const C_PuiSvDbToggle * const pc_ToggleData = pc_Dashboard->GetToggle(u32_Index);
                        if (pc_ToggleData != NULL)
                        {
                           C_PuiSvDbToggle c_Tmp = *pc_ToggleData;
                           C_SebBaseCopyPasteManager::mh_HandleZValueBox(*c_ItItem, orc_NormalizedZValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp, C_PuiSvDbDataElement::eTOGGLE) == C_NO_ERR);
                        }
                     }
                     else
                     {
                        // copy the element
                        switch (pc_CurItem->type())
                        {
                        case msn_GRAPHICS_ITEM_TEXTELEMENT:

                           pc_TextElement = dynamic_cast<const C_GiSvDaTextElement *>(pc_CurItem);
                           if (pc_TextElement != NULL)
                           {
                              u32_Index = static_cast<uint32>(pc_TextElement->GetIndex());
                              if (u32_Index < pc_Dashboard->c_TextElements.size())
                              {
                                 c_Snapshot.c_TextElements.push_back(pc_Dashboard->c_TextElements[u32_Index]);
                                 C_SyvDaCopyPasteManager::mh_HandleZValueBox(
                                    *c_ItItem, orc_NormalizedZValues,
                                    c_Snapshot.c_TextElements[c_Snapshot.c_TextElements.size() - 1]);
                              }
                           }
                           break;
                        case msn_GRAPHICS_ITEM_BOUNDARY:

                           pc_Boundary = dynamic_cast<const C_GiSvDaBoundary *>(pc_CurItem);
                           if (pc_Boundary != NULL)
                           {
                              u32_Index = static_cast<uint32>(pc_Boundary->GetIndex());
                              if (u32_Index < pc_Dashboard->c_Boundaries.size())
                              {
                                 c_Snapshot.c_Boundaries.push_back(pc_Dashboard->c_Boundaries[u32_Index]);
                                 C_SyvDaCopyPasteManager::mh_HandleZValueBox(
                                    *c_ItItem, orc_NormalizedZValues,
                                    c_Snapshot.c_Boundaries[c_Snapshot.c_Boundaries.size() - 1]);
                              }
                           }
                           break;
                        case msn_GRAPHICS_ITEM_IMAGE:

                           pc_Image = dynamic_cast<const C_GiSvDaImageGroup *>(pc_CurItem);
                           if (pc_Image != NULL)
                           {
                              u32_Index = static_cast<uint32>(pc_Image->GetIndex());
                              if (u32_Index < pc_Dashboard->c_Images.size())
                              {
                                 c_Snapshot.c_Images.push_back(pc_Dashboard->c_Images[u32_Index]);
                                 C_SyvDaCopyPasteManager::mh_HandleZValueBox(
                                    *c_ItItem, orc_NormalizedZValues,
                                    c_Snapshot.c_Images[c_Snapshot.c_Images.size() - 1]);
                              }
                           }
                           break;
                        case msn_GRAPHICS_ITEM_LINE_ARROW:

                           pc_LineArrow = dynamic_cast<const C_GiSvDaArrow *>(pc_CurItem);
                           if (pc_LineArrow != NULL)
                           {
                              u32_Index = static_cast<uint32>(pc_LineArrow->GetIndex());
                              if (u32_Index < pc_Dashboard->c_LineArrows.size())
                              {
                                 c_Snapshot.c_LineArrows.push_back(pc_Dashboard->c_LineArrows[u32_Index]);
                                 C_SyvDaCopyPasteManager::mh_HandleZValueLine(
                                    *c_ItItem, orc_NormalizedZValues,
                                    c_Snapshot.c_LineArrows[c_Snapshot.c_LineArrows.size() - 1]);
                              }
                           }
                           break;
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Indicator if copy paste manager has some content

   \return
   true: content
   false: no content
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaCopyPasteManager::CheckValidContentAndPrepareData(void)
{
   this->mc_LastKnownData.Clear();
   return (C_SyvClipBoardHelper::h_LoadDashboardFromClipboard(this->mc_LastKnownData,
                                                              C_SyvDaCopyPasteManager::hc_ClipBoardBaseTagName) ==
           C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt copy content for paste

   \param[in,out]  orc_CopyData     Copy data
   \param[in]      ou32_ViewIndex   View index
   \param[in,out]  opc_Parent       Parent for dialog
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaCopyPasteManager::h_AdaptCopyDataForPaste(C_PuiSvDashboard & orc_CopyData, const uint32 ou32_ViewIndex,
                                                      QWidget * const opc_Parent)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

   if (pc_View != NULL)
   {
      //Check param widget validity
      for (uint32 u32_ItParam = 0; u32_ItParam < orc_CopyData.GetParams().size();)
      {
         std::set<QString> c_AdaptedLists;
         bool q_Found = false;
         bool q_ElementMissing = false;
         const C_PuiSvDbParam & rc_ParamWidget = orc_CopyData.GetParams()[u32_ItParam];
         //Check each parameter element
         for (uint32 u32_ItElement = 0; u32_ItElement < rc_ParamWidget.c_DataPoolElementsConfig.size();
              ++u32_ItElement)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = rc_ParamWidget.c_DataPoolElementsConfig[u32_ItElement];
            if (rc_Config.c_ElementId.GetIsValid() == true)
            {
               const stw_opensyde_core::C_OSCNodeDataPoolListId c_ListId(rc_Config.c_ElementId.u32_NodeIndex,
                                                                         rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                         rc_Config.c_ElementId.u32_ListIndex);
               const stw_opensyde_core::C_OSCNodeDataPoolList * const pc_List =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(c_ListId.u32_NodeIndex,
                                                                      c_ListId.u32_DataPoolIndex,
                                                                      c_ListId.u32_ListIndex);
               if (pc_View->CheckNvmParamListUsage(c_ListId) == true)
               {
                  //Signal error message necessary
                  q_Found = true;
               }
               //For each element check if all elements of this list are contained
               if (pc_List != NULL)
               {
                  //Iterate over all list elements
                  for (uint32 u32_ItSDElement = 0UL; u32_ItSDElement < pc_List->c_Elements.size(); ++u32_ItSDElement)
                  {
                     bool q_FoundOne = false;
                     const stw_opensyde_core::C_OSCNodeDataPoolListElementId c_ElementId(
                        rc_Config.c_ElementId.u32_NodeIndex,
                        rc_Config.c_ElementId.u32_DataPoolIndex,
                        rc_Config.c_ElementId.u32_ListIndex, u32_ItSDElement);
                     //Check if each element contained
                     for (uint32 u32_ItCheckElement = 0;
                          u32_ItCheckElement < rc_ParamWidget.c_DataPoolElementsConfig.size();
                          ++u32_ItCheckElement)
                     {
                        const C_PuiSvDbNodeDataElementConfig & rc_CheckConfig =
                           rc_ParamWidget.c_DataPoolElementsConfig[u32_ItElement];
                        if (rc_CheckConfig.c_ElementId.GetIsValid() == true)
                        {
                           if (rc_CheckConfig.c_ElementId == c_ElementId)
                           {
                              //Signal match for this index
                              q_FoundOne = true;
                              break;
                           }
                        }
                     }
                     //Signal at least one element missing
                     if (q_FoundOne == false)
                     {
                        QString c_Tmp;
                        const stw_opensyde_core::C_OSCNode * const pc_Node =
                           C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_ListId.u32_NodeIndex);
                        const stw_opensyde_core::C_OSCNodeDataPool * const pc_DataPool =
                           C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(c_ListId.u32_NodeIndex,
                                                                           c_ListId.u32_DataPoolIndex);

                        //For each missing element add default
                        q_ElementMissing = true;
                        //Remember name
                        if ((pc_Node != NULL) && (pc_DataPool != NULL))
                        {
                           c_Tmp = static_cast<QString>("%1::%2::%3").arg(pc_Node->c_Properties.c_Name.c_str()).arg(
                              pc_DataPool->c_Name.c_str()).arg(
                              pc_List->c_Name.c_str());
                        }
                        c_AdaptedLists.insert(c_Tmp);
                        break;
                     }
                  }
               }
            }
         }
         //Check if error message necessary
         if ((q_Found) || (q_ElementMissing))
         {
            QString c_Details;
            C_OgeWiCustomMessage c_Message(opc_Parent);
            //No iterator step because new element at current position
            orc_CopyData.DeleteWidget(u32_ItParam, C_PuiSvDbDataElement::ePARAM);
            c_Message.SetType(C_OgeWiCustomMessage::eINFORMATION);
            c_Message.SetHeading(C_GtGetText::h_GetText("Widget paste"));
            c_Message.SetDescription(C_GtGetText::h_GetText("Parametrization widget could not be pasted."));
            if (q_Found)
            {
               c_Details = C_GtGetText::h_GetText("Another parametrization widget in this view is already "
                                                  "using at least one of the containing lists.");
               c_Message.SetCustomMinHeight(180, 250);
            }
            else if (q_ElementMissing)
            {
               c_Details = C_GtGetText::h_GetText(
                  "The following copied lists were changed before pasting the parametrization widget:\n");
               for (std::set<QString>::const_iterator c_It = c_AdaptedLists.begin(); c_It != c_AdaptedLists.end();
                    ++c_It)
               {
                  c_Details += *c_It;
                  c_Details += "\n";
               }
               c_Message.SetCustomMinHeight(180, 270);
            }
            else
            {
               //Unexpected
            }
            c_Message.SetDetails(c_Details);
            c_Message.Execute();
         }
         else
         {
            //Important iterator step
            ++u32_ItParam;
         }
      }
      //Check ID validity
      if (orc_CopyData.DiscardInvalidIndices())
      {
         C_OgeWiCustomMessage c_Message(opc_Parent);
         c_Message.SetType(C_OgeWiCustomMessage::eINFORMATION);
         c_Message.SetHeading(C_GtGetText::h_GetText("Widget paste"));
         c_Message.SetDescription(C_GtGetText::h_GetText(
                                     "Some data elements were not found in your SYSTEM DEFINITION.\n"
                                     "These were automatically removed from the pasted widgets."));
         c_Message.SetCustomMinHeight(180, 180);
         c_Message.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Evaluate all items to get top left point

   \param[in]  opc_Data    Data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaCopyPasteManager::m_CalcOriginalPosition(const C_PuiBsElements * const opc_Data)
{
   const C_PuiSvDashboard * const pc_ExpectedData =
      dynamic_cast<const C_PuiSvDashboard * const>(opc_Data);

   C_SebBaseCopyPasteManager::m_CalcOriginalPosition(opc_Data);
   if (pc_ExpectedData != NULL)
   {
      uint32 u32_ItElem;
      const std::vector<C_PuiSvDbLabel> & rc_Labels = pc_ExpectedData->GetLabels();
      const std::vector<C_PuiSvDbPieChart> & rc_PieCharts = pc_ExpectedData->GetPieCharts();
      const std::vector<C_PuiSvDbProgressBar> & rc_ProgressBars = pc_ExpectedData->GetProgressBars();
      const std::vector<C_PuiSvDbSpinBox> & rc_SpinBoxs = pc_ExpectedData->GetSpinBoxes();
      const std::vector<C_PuiSvDbSlider> & rc_Sliders = pc_ExpectedData->GetSliders();
      const std::vector<C_PuiSvDbTable> & rc_Tabless = pc_ExpectedData->GetTables();
      const std::vector<C_PuiSvDbToggle> & rc_Toggles = pc_ExpectedData->GetToggles();

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
