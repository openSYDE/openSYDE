//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle most parts of copy paste implementation for scene (implementation)

   Handle most parts of copy paste implementation for scene

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "gitypes.hpp"
#include "TglUtils.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_PuiSvDashboard.hpp"
#include "C_SyvClipBoardHelper.hpp"
#include "C_SyvDaCopyPasteManager.hpp"
#include "C_GiSvDaArrow.hpp"
#include "C_GiSvDaBoundary.hpp"
#include "C_GiSvDaImageGroup.hpp"
#include "C_GiSvDaLabelBase.hpp"
#include "C_GiSvDaParam.hpp"
#include "C_GiSvDaPieChartBase.hpp"
#include "C_GiSvDaTableBase.hpp"
#include "C_GiSvDaSpinBoxBase.hpp"
#include "C_GiSvDaSliderBase.hpp"
#include "C_GiSvDaProgressBarBase.hpp"
#include "C_GiSvDaToggleBase.hpp"
#include "C_GiSvDaTextElement.hpp"
#include "C_SebUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvDaCopyPasteManager::hc_CLIP_BOARD_BASE_TAG_NAME = "opensyde-system-definition";

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
   C_SyvDaCopyPasteManager::h_AdaptCopyDataForPaste(this->mc_LastKnownData, this->mc_LastKnownRails,
                                                    this->mc_LastKnownElementIdGroups, this->mu32_ViewIndex,
                                                    opc_Parent);

   return &this->mc_LastKnownData;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get rails

   \return
   NULL No valid rails found
   Else Valid rails
*/
//----------------------------------------------------------------------------------------------------------------------
const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
           C_PuiSvReadDataConfiguration> * C_SyvDaCopyPasteManager::GetRails() const
{
   return &this->mc_LastKnownRails;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare call of CopyFromSceneToManager

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaCopyPasteManager::PrepareCopyFromSceneToManager(const uint32_t ou32_ViewIndex,
                                                            const uint32_t ou32_DashboardIndex)
{
   this->mu32_ViewIndex = ou32_ViewIndex;
   this->mu32_DashboardIndex = ou32_DashboardIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy selected files to copy paste manager

   \param[in]  orc_SelectedItems       Selected items to copy
   \param[in]  orc_NormalizedZetValues   Normalized Z values for all copied items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaCopyPasteManager::CopyFromSceneToManager(const QList<QGraphicsItem *> & orc_SelectedItems,
                                                     const QMap<const QGraphicsItem *,
                                                                float64_t> & orc_NormalizedZetValues)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
      if (pc_Dashboard != NULL)
      {
         C_PuiSvDashboard c_Snapshot;
         uint32_t u32_Index;
         const C_GiSvDaImageGroup * pc_Image;
         const C_GiSvDaBoundary * pc_Boundary;
         const C_GiSvDaArrow * pc_LineArrow;
         const C_GiSvDaTextElement * pc_TextElement;

         std::vector<const QGraphicsItem *> c_HandledItems;
         QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_Rails;

         //Clear data
         for (QList<QGraphicsItem *>::const_iterator c_ItItem = orc_SelectedItems.begin();
              c_ItItem != orc_SelectedItems.end();
              ++c_ItItem)
         {
            bool q_Handled = false;
            QGraphicsItem * const pc_CurItem = C_SebUtil::h_GetHighestParent(*c_ItItem);
            for (uint32_t u32_ItHandledItems = 0; u32_ItHandledItems < c_HandledItems.size(); ++u32_ItHandledItems)
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
                        u32_Index = static_cast<uint32_t>(pc_Label->GetIndex());
                        const C_PuiSvDbLabel * const pc_LabelData = pc_Dashboard->GetLabel(u32_Index);
                        if (pc_LabelData != NULL)
                        {
                           C_PuiSvDbLabel c_Tmp = *pc_LabelData;
                           C_SebBaseCopyPasteManager::mh_HandleZetValueBox(*c_ItItem, orc_NormalizedZetValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp, C_PuiSvDbDataElement::eLABEL) == C_NO_ERR);
                        }
                     }
                     else if (pc_Param != NULL)
                     {
                        u32_Index = static_cast<uint32_t>(pc_Param->GetIndex());
                        const C_PuiSvDbParam * const pc_ParamData = pc_Dashboard->GetParam(u32_Index);
                        if (pc_ParamData != NULL)
                        {
                           C_PuiSvDbParam c_Tmp = *pc_ParamData;
                           C_SebBaseCopyPasteManager::mh_HandleZetValueBox(*c_ItItem, orc_NormalizedZetValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp, C_PuiSvDbDataElement::ePARAM) == C_NO_ERR);
                        }
                     }
                     else if (pc_PieChart != NULL)
                     {
                        u32_Index = static_cast<uint32_t>(pc_PieChart->GetIndex());
                        const C_PuiSvDbPieChart * const pc_PieChartData = pc_Dashboard->GetPieChart(u32_Index);
                        if (pc_PieChartData != NULL)
                        {
                           C_PuiSvDbPieChart c_Tmp = *pc_PieChartData;
                           C_SebBaseCopyPasteManager::mh_HandleZetValueBox(*c_ItItem, orc_NormalizedZetValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp,
                                                           C_PuiSvDbDataElement::ePIE_CHART) == C_NO_ERR);
                        }
                     }
                     else if (pc_ProgressBar != NULL)
                     {
                        u32_Index = static_cast<uint32_t>(pc_ProgressBar->GetIndex());
                        const C_PuiSvDbProgressBar * const pc_ProgressBarData = pc_Dashboard->GetProgressBar(u32_Index);
                        if (pc_ProgressBarData != NULL)
                        {
                           C_PuiSvDbProgressBar c_Tmp = *pc_ProgressBarData;
                           C_SebBaseCopyPasteManager::mh_HandleZetValueBox(*c_ItItem, orc_NormalizedZetValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp,
                                                           C_PuiSvDbDataElement::ePROGRESS_BAR) == C_NO_ERR);
                        }
                     }
                     else if (pc_Slider != NULL)
                     {
                        u32_Index = static_cast<uint32_t>(pc_Slider->GetIndex());
                        const C_PuiSvDbSlider * const pc_SliderData = pc_Dashboard->GetSlider(u32_Index);
                        if (pc_SliderData != NULL)
                        {
                           C_PuiSvDbSlider c_Tmp = *pc_SliderData;
                           C_SebBaseCopyPasteManager::mh_HandleZetValueBox(*c_ItItem, orc_NormalizedZetValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp, C_PuiSvDbDataElement::eSLIDER) == C_NO_ERR);
                        }
                     }
                     else if (pc_SpinBox != NULL)
                     {
                        u32_Index = static_cast<uint32_t>(pc_SpinBox->GetIndex());
                        const C_PuiSvDbSpinBox * const pc_SpinBoxData = pc_Dashboard->GetSpinBox(u32_Index);
                        if (pc_SpinBoxData != NULL)
                        {
                           C_PuiSvDbSpinBox c_Tmp = *pc_SpinBoxData;
                           C_SebBaseCopyPasteManager::mh_HandleZetValueBox(*c_ItItem, orc_NormalizedZetValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp,
                                                           C_PuiSvDbDataElement::eSPIN_BOX) == C_NO_ERR);
                        }
                     }
                     else if (pc_Table != NULL)
                     {
                        u32_Index = static_cast<uint32_t>(pc_Table->GetIndex());
                        const C_PuiSvDbTable * const pc_TableData = pc_Dashboard->GetTable(u32_Index);
                        if (pc_TableData != NULL)
                        {
                           C_PuiSvDbTable c_Tmp = *pc_TableData;
                           C_SebBaseCopyPasteManager::mh_HandleZetValueBox(*c_ItItem, orc_NormalizedZetValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp, C_PuiSvDbDataElement::eTABLE) == C_NO_ERR);
                        }
                     }
                     else if (pc_Toggle != NULL)
                     {
                        u32_Index = static_cast<uint32_t>(pc_Toggle->GetIndex());
                        const C_PuiSvDbToggle * const pc_ToggleData = pc_Dashboard->GetToggle(u32_Index);
                        if (pc_ToggleData != NULL)
                        {
                           C_PuiSvDbToggle c_Tmp = *pc_ToggleData;
                           C_SebBaseCopyPasteManager::mh_HandleZetValueBox(*c_ItItem, orc_NormalizedZetValues, c_Tmp);
                           tgl_assert(c_Snapshot.AddWidget(&c_Tmp, C_PuiSvDbDataElement::eTOGGLE) == C_NO_ERR);
                        }
                     }
                     else
                     {
                        // copy the element
                        switch (pc_CurItem->type())
                        {
                        case ms32_GRAPHICS_ITEM_TEXTELEMENT:

                           pc_TextElement = dynamic_cast<const C_GiSvDaTextElement *>(pc_CurItem);
                           if (pc_TextElement != NULL)
                           {
                              u32_Index = static_cast<uint32_t>(pc_TextElement->GetIndex());
                              if (u32_Index < pc_Dashboard->c_TextElements.size())
                              {
                                 c_Snapshot.c_TextElements.push_back(pc_Dashboard->c_TextElements[u32_Index]);
                                 C_SyvDaCopyPasteManager::mh_HandleZetValueBox(
                                    *c_ItItem, orc_NormalizedZetValues,
                                    c_Snapshot.c_TextElements[c_Snapshot.c_TextElements.size() - 1]);
                              }
                           }
                           break;
                        case ms32_GRAPHICS_ITEM_BOUNDARY:

                           pc_Boundary = dynamic_cast<const C_GiSvDaBoundary *>(pc_CurItem);
                           if (pc_Boundary != NULL)
                           {
                              u32_Index = static_cast<uint32_t>(pc_Boundary->GetIndex());
                              if (u32_Index < pc_Dashboard->c_Boundaries.size())
                              {
                                 c_Snapshot.c_Boundaries.push_back(pc_Dashboard->c_Boundaries[u32_Index]);
                                 C_SyvDaCopyPasteManager::mh_HandleZetValueBox(
                                    *c_ItItem, orc_NormalizedZetValues,
                                    c_Snapshot.c_Boundaries[c_Snapshot.c_Boundaries.size() - 1]);
                              }
                           }
                           break;
                        case ms32_GRAPHICS_ITEM_IMAGE:

                           pc_Image = dynamic_cast<const C_GiSvDaImageGroup *>(pc_CurItem);
                           if (pc_Image != NULL)
                           {
                              u32_Index = static_cast<uint32_t>(pc_Image->GetIndex());
                              if (u32_Index < pc_Dashboard->c_Images.size())
                              {
                                 c_Snapshot.c_Images.push_back(pc_Dashboard->c_Images[u32_Index]);
                                 C_SyvDaCopyPasteManager::mh_HandleZetValueBox(
                                    *c_ItItem, orc_NormalizedZetValues,
                                    c_Snapshot.c_Images[c_Snapshot.c_Images.size() - 1]);
                              }
                           }
                           break;
                        case ms32_GRAPHICS_ITEM_LINE_ARROW:

                           pc_LineArrow = dynamic_cast<const C_GiSvDaArrow *>(pc_CurItem);
                           if (pc_LineArrow != NULL)
                           {
                              u32_Index = static_cast<uint32_t>(pc_LineArrow->GetIndex());
                              if (u32_Index < pc_Dashboard->c_LineArrows.size())
                              {
                                 c_Snapshot.c_LineArrows.push_back(pc_Dashboard->c_LineArrows[u32_Index]);
                                 C_SyvDaCopyPasteManager::mh_HandleZetValueLine(
                                    *c_ItItem, orc_NormalizedZetValues,
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
                                                           C_SyvDaCopyPasteManager::hc_CLIP_BOARD_BASE_TAG_NAME);
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
   this->mc_LastKnownRails.clear();
   this->mc_LastKnownData.Clear();
   this->mc_LastKnownElementIdGroups.clear();
   return (C_SyvClipBoardHelper::h_LoadDashboardFromClipboard(this->mc_LastKnownData, this->mc_LastKnownRails,
                                                              this->mc_LastKnownElementIdGroups,
                                                              C_SyvDaCopyPasteManager::hc_CLIP_BOARD_BASE_TAG_NAME) ==
           C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt copy content for paste

   \param[in,out]  orc_CopyData           Copy data
   \param[in,out]  orc_Rails              System view dashboard rails
   \param[in]      orc_ElementIdGroups    Element ID groups
   \param[in]      ou32_ViewIndex         View index
   \param[in,out]  opc_Parent             Parent for dialog
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaCopyPasteManager::h_AdaptCopyDataForPaste(C_PuiSvDashboard & orc_CopyData,
                                                      QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                                           C_PuiSvReadDataConfiguration> & orc_Rails,
                                                      const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                                 C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups,
                                                      const uint32_t ou32_ViewIndex,
                                                      QWidget * const opc_Parent)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

   if (pc_View != NULL)
   {
      //Check param widget validity
      for (uint32_t u32_ItParam = 0; u32_ItParam < orc_CopyData.GetParams().size();)
      {
         std::set<QString> c_AdaptedLists;
         bool q_Found = false;
         bool q_ElementMissing = false;
         const C_PuiSvDbParam & rc_ParamWidget = orc_CopyData.GetParams()[u32_ItParam];
         //Check each parameter element
         for (uint32_t u32_ItElement = 0; u32_ItElement < rc_ParamWidget.c_DataPoolElementsConfig.size();
              ++u32_ItElement)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = rc_ParamWidget.c_DataPoolElementsConfig[u32_ItElement];
            if (rc_Config.c_ElementId.GetIsValid() == true)
            {
               const stw::opensyde_core::C_OscNodeDataPoolListId c_ListId(rc_Config.c_ElementId.u32_NodeIndex,
                                                                          rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                          rc_Config.c_ElementId.u32_ListIndex);
               const stw::opensyde_core::C_OscNodeDataPoolList * const pc_List =
                  C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(c_ListId.u32_NodeIndex,
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
                  for (uint32_t u32_ItSdElement = 0UL; u32_ItSdElement < pc_List->c_Elements.size(); ++u32_ItSdElement)
                  {
                     bool q_FoundOne = false;
                     const stw::opensyde_core::C_OscNodeDataPoolListElementId c_ElementId(
                        rc_Config.c_ElementId.u32_NodeIndex,
                        rc_Config.c_ElementId.u32_DataPoolIndex,
                        rc_Config.c_ElementId.u32_ListIndex, u32_ItSdElement);
                     //Check if each element contained
                     for (uint32_t u32_ItCheckElement = 0;
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
                        const stw::opensyde_core::C_OscNode * const pc_Node =
                           C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_ListId.u32_NodeIndex);
                        const stw::opensyde_core::C_OscNodeDataPool * const pc_DataPool =
                           C_PuiSdHandler::h_GetInstance()->GetOscDataPool(c_ListId.u32_NodeIndex,
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
      {
         const bool q_Changed1 = orc_CopyData.DiscardInvalidIndices();
         const bool q_Changed2 = C_SyvDaCopyPasteManager::mh_ValidateCrcs(orc_CopyData, orc_Rails, orc_ElementIdGroups);
         if (q_Changed1 || q_Changed2)
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
      uint32_t u32_ItElem;
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
         m_MinToOrgPos(rc_Widget.c_UiPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < rc_PieCharts.size(); ++u32_ItElem)
      {
         const C_PuiSvDbPieChart & rc_Widget = rc_PieCharts[u32_ItElem];
         m_MinToOrgPos(rc_Widget.c_UiPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < rc_ProgressBars.size(); ++u32_ItElem)
      {
         const C_PuiSvDbProgressBar & rc_Widget = rc_ProgressBars[u32_ItElem];
         m_MinToOrgPos(rc_Widget.c_UiPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < rc_SpinBoxs.size(); ++u32_ItElem)
      {
         const C_PuiSvDbSpinBox & rc_Widget = rc_SpinBoxs[u32_ItElem];
         m_MinToOrgPos(rc_Widget.c_UiPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < rc_Sliders.size(); ++u32_ItElem)
      {
         const C_PuiSvDbSlider & rc_Widget = rc_Sliders[u32_ItElem];
         m_MinToOrgPos(rc_Widget.c_UiPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < rc_Tabless.size(); ++u32_ItElem)
      {
         const C_PuiSvDbTable & rc_Widget = rc_Tabless[u32_ItElem];
         m_MinToOrgPos(rc_Widget.c_UiPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < rc_Toggles.size(); ++u32_ItElem)
      {
         const C_PuiSvDbToggle & rc_Widget = rc_Toggles[u32_ItElem];
         m_MinToOrgPos(rc_Widget.c_UiPosition);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Validate CRCs

   \param[in,out]  orc_Data               System view dashboard
   \param[in,out]  orc_Rails              System view dashboard rails
   \param[in]      orc_ElementIdGroups    Element ID groups

   \return
   Flags

   \retval   True    Changes happened
   \retval   False   Nothing changed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaCopyPasteManager::mh_ValidateCrcs(C_PuiSvDashboard & orc_Data, QMap<C_OscNodeDataPoolListElementId,
                                                                                C_PuiSvReadDataConfiguration> & orc_Rails, const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                                                                                                      C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups)
{
   bool q_Retval = false;

   for (QMap<C_PuiSvDbNodeDataPoolListElementId,
             C_PuiSvDbElementIdCrcGroup>::ConstIterator c_It = orc_ElementIdGroups.cbegin();
        c_It != orc_ElementIdGroups.cend(); ++c_It)
   {
      if (c_It.value().CheckCrc() != C_NO_ERR)
      {
         orc_Rails.remove(c_It.key());
         orc_Data.RemoveAllReferencesToElementId(c_It.key());
         q_Retval = true;
      }
   }
   return q_Retval;
}
