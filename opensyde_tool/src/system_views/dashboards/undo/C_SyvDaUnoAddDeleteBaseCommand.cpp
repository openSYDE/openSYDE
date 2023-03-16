//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add and delete base class for dashboard (implementation)

   Add and delete base class for dashboard

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_GiSvDaRectBaseGroup.hpp"
#include "C_GiSvDaLabelBase.hpp"
#include "C_GiSvDaParam.hpp"
#include "C_GiSvDaPieChartBase.hpp"
#include "C_GiSvDaSpinBoxBase.hpp"
#include "C_GiSvDaSliderBase.hpp"
#include "C_GiSvDaTableBase.hpp"
#include "C_GiSvDaProgressBarBase.hpp"
#include "C_GiSvDaToggleBase.hpp"
#include "C_PuiSvDbDataElement.hpp"
#include "C_SyvDaDashboardScene.hpp"
#include "C_SyvDaUnoAddDeleteBaseCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Scene               Pointer to currently active scene
   \param[in]     orc_Ids                 Affected unique IDs
   \param[in]     orc_Text                Command description
   \param[in,out] opc_Parent              Optional pointer to parent
   \param[in]     orc_InitialSnapshotData Initial snapshot data
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaUnoAddDeleteBaseCommand::C_SyvDaUnoAddDeleteBaseCommand(QGraphicsScene * const opc_Scene,
                                                               const std::vector<uint64_t> & orc_Ids,
                                                               const QString & orc_Text,
                                                               QUndoCommand * const opc_Parent,
                                                               const C_SyvDaDashboardSnapshot & orc_InitialSnapshotData)
   :
   C_SebUnoAddDeleteBaseCommand(opc_Scene, orc_Ids, orc_Text, opc_Parent),
   mc_DataBackup(orc_InitialSnapshotData)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete all relevant items and save them to internal backup
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoAddDeleteBaseCommand::m_DeleteSave(void)
{
   m_SaveToData();
   m_Delete();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore all items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoAddDeleteBaseCommand::m_Restore(void)
{
   C_SyvDaDashboardScene * const pc_Scene = dynamic_cast<C_SyvDaDashboardScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      //First: Restore data rail(s) if any (not replaced if already existing due to other not registered element
      // changes)
      m_RestoreReadRailsOnly();
      pc_Scene->CopyFromSnapshotToScene(this->mc_DataBackup, &(this->mc_MapTypeAndIndexToId));
      //Trigger error recheck
      pc_Scene->TriggerErrorCheck();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore all read rails (not replaced if already existing due to other not registered element changes)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoAddDeleteBaseCommand::m_RestoreReadRailsOnly(void)
{
   if (this->mc_SavedRailAssignments.size() > 0)
   {
      C_SyvDaDashboardScene * const pc_Scene = dynamic_cast<C_SyvDaDashboardScene * const>(mpc_Scene);

      if (pc_Scene != NULL)
      {
         for (QMap<C_OscNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::const_iterator c_It =
                 this->mc_SavedRailAssignments.begin();
              c_It != this->mc_SavedRailAssignments.end(); ++c_It)
         {
            if (C_PuiSvHandler::h_GetInstance()->AddViewReadRailItem(pc_Scene->GetViewIndex(), c_It.key(),
                                                                     c_It.value()) == C_NOACT)
            {
               //Special param widget handling
               const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(pc_Scene->GetViewIndex());
               if (pc_View != NULL)
               {
                  if (pc_View->CheckNonParamReadUsage(c_It.key()) == false)
                  {
                     //Re-add with new config
                     tgl_assert(C_PuiSvHandler::h_GetInstance()->RemoveViewReadRailItem(pc_Scene->GetViewIndex(),
                                                                                        c_It.key()) == C_NO_ERR);
                     tgl_assert(C_PuiSvHandler::h_GetInstance()->AddViewReadRailItem(pc_Scene->GetViewIndex(),
                                                                                     c_It.key(),
                                                                                     c_It.value()) == C_NO_ERR);
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get boundary element type

   \return
   Boundary element type
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaUnoAddDeleteBaseCommand::m_GetBoundaryType(void) const
{
   return static_cast<int32_t>(C_PuiSvDbDataElement::eBOUNDARY);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get image element type

   \return
   Image element type
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaUnoAddDeleteBaseCommand::m_GetImageType(void) const
{
   return static_cast<int32_t>(C_PuiSvDbDataElement::eIMAGE);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get line arrow element type

   \return
   Line arrow element type
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaUnoAddDeleteBaseCommand::m_GetLineArrowType(void) const
{
   return static_cast<int32_t>(C_PuiSvDbDataElement::eLINE_ARROW);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get text element type

   \return
   Text element type
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaUnoAddDeleteBaseCommand::m_GetTextElementType(void) const
{
   return static_cast<int32_t>(C_PuiSvDbDataElement::eTEXT_ELEMENT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply position offset

   \param[in] orc_NewPos Offset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoAddDeleteBaseCommand::m_SetDataPositionOffset(const QPointF & orc_NewPos)
{
   this->mc_DataBackup.SetDataPositionOffset(orc_NewPos);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply Z value offset

   \param[in] of64_HighestUsedZetValue Highest used Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoAddDeleteBaseCommand::m_SetDataZetOffset(const float64_t of64_HighestUsedZetValue)
{
   this->mc_DataBackup.SetDataZetOffset(of64_HighestUsedZetValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get backup data

   \return
   Current backup data
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDashboard C_SyvDaUnoAddDeleteBaseCommand::m_GetDataBackup(void) const
{
   return this->mc_DataBackup;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the number of stored read rails

   \return
   The number of stored read rails
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaUnoAddDeleteBaseCommand::m_GetStoredReadRailCount(void) const
{
   return this->mc_SavedRailAssignments.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add read rail config to internal storage

   \param[in] orc_Id    ID
   \param[in] orc_Value Value

   \return
   C_NO_ERR Operation success
   C_NOACT  Already exists
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaUnoAddDeleteBaseCommand::m_AddReadRailToInternalBackup(const C_OscNodeDataPoolListElementId & orc_Id,
                                                                      const C_PuiSvReadDataConfiguration & orc_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->mc_SavedRailAssignments.find(orc_Id) != this->mc_SavedRailAssignments.end())
   {
      s32_Retval = C_NOACT;
   }
   else
   {
      this->mc_SavedRailAssignments.insert(orc_Id, orc_Value);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save currently relevant system definition part to data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoAddDeleteBaseCommand::m_SaveToData(void)
{
   C_SyvDaDashboardScene * const pc_Scene = dynamic_cast<C_SyvDaDashboardScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(pc_Scene->GetViewIndex());
      if (pc_View != NULL)
      {
         const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(pc_Scene->GetDashboardIndex());
         if (pc_Dashboard != NULL)
         {
            const std::vector<QGraphicsItem *> c_RelatedItems = this->m_GetSceneItems();
            C_PuiSvDbDataElement * pc_Data;
            C_GiUnique * pc_Unique;
            C_GiSvDaRectBaseGroup * pc_RectBase;

            this->m_Clear();

            //Base elements
            m_StoreCommon(this->mc_DataBackup, this->mc_MapTypeAndIndexToId, *pc_Dashboard);
            //Other elements
            for (std::vector<QGraphicsItem *>::const_iterator c_ItRelatedItem = c_RelatedItems.begin();
                 c_ItRelatedItem != c_RelatedItems.end(); ++c_ItRelatedItem)
            {
               pc_Unique = dynamic_cast<C_GiUnique *>(*c_ItRelatedItem);
               if (pc_Unique != NULL)
               {
                  const uint64_t u64_CurUniqueId = pc_Unique->GetId();
                  pc_Data = dynamic_cast<C_PuiSvDbDataElement *>(*c_ItRelatedItem);
                  if (pc_Data != NULL)
                  {
                     const C_PuiSvDbWidgetBase * const pc_Widget = C_SyvDaUnoAddDeleteBaseCommand::mh_GetGenericWidget(
                        pc_Scene->GetViewIndex(), pc_Scene->GetDashboardIndex(), pc_Data);
                     const int32_t s32_Index = pc_Data->GetIndex();
                     if (s32_Index >= 0)
                     {
                        const uint32_t u32_Index = static_cast<uint32_t>(s32_Index);
                        //Save to scene data
                        pc_Data->UpdateData();

                        //Backup scene data internally
                        //Widgets

                        pc_RectBase = dynamic_cast<C_GiSvDaRectBaseGroup *>(*c_ItRelatedItem);
                        if (pc_RectBase != NULL)
                        {
                           const C_GiSvDaLabelBase * const pc_Label  =
                              dynamic_cast<const C_GiSvDaLabelBase * const>(*c_ItRelatedItem);

                           const C_GiSvDaParam * const pc_ParamWidget  =
                              dynamic_cast<const C_GiSvDaParam * const>(*c_ItRelatedItem);

                           const C_GiSvDaSpinBoxBase * const pc_SpinBox  =
                              dynamic_cast<const C_GiSvDaSpinBoxBase * const>(*c_ItRelatedItem);

                           const C_GiSvDaSliderBase * const pc_Slider  =
                              dynamic_cast<const C_GiSvDaSliderBase * const>(*c_ItRelatedItem);

                           const C_GiSvDaProgressBarBase * const pc_ProgressBar  =
                              dynamic_cast<const C_GiSvDaProgressBarBase * const>(*c_ItRelatedItem);

                           const C_GiSvDaToggleBase * const pc_Toggle  =
                              dynamic_cast<const C_GiSvDaToggleBase * const>(*c_ItRelatedItem);

                           const C_GiSvDaPieChartBase * const pc_PieChart  =
                              dynamic_cast<const C_GiSvDaPieChartBase * const>(*c_ItRelatedItem);

                           const C_GiSvDaTableBase * const pc_Table  =
                              dynamic_cast<const C_GiSvDaTableBase * const>(*c_ItRelatedItem);
                           if (pc_Label != NULL)
                           {
                              const C_PuiSvDbLabel * const pc_LabelData = pc_Dashboard->GetLabel(u32_Index);
                              if (pc_LabelData != NULL)
                              {
                                 tgl_assert(this->mc_DataBackup.AddWidget(pc_LabelData,
                                                                          C_PuiSvDbDataElement::eLABEL) == C_NO_ERR);
                                 this->mc_MapTypeAndIndexToId.insert(
                                    C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::eLABEL),
                                                           this->mc_DataBackup.GetLabels().size() - 1UL),
                                    u64_CurUniqueId);
                              }
                           }
                           if (pc_ParamWidget != NULL)
                           {
                              const C_PuiSvDbParam * const pc_ParamData = pc_Dashboard->GetParam(u32_Index);
                              if (pc_ParamData != NULL)
                              {
                                 tgl_assert(this->mc_DataBackup.AddWidget(pc_ParamData,
                                                                          C_PuiSvDbDataElement::ePARAM) == C_NO_ERR);
                                 this->mc_MapTypeAndIndexToId.insert(
                                    C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::ePARAM),
                                                           this->mc_DataBackup.GetParams().size() - 1UL),
                                    u64_CurUniqueId);
                              }
                           }
                           if (pc_SpinBox != NULL)
                           {
                              const C_PuiSvDbSpinBox * const pc_SpinBoxData = pc_Dashboard->GetSpinBox(u32_Index);
                              if (pc_SpinBoxData != NULL)
                              {
                                 tgl_assert(this->mc_DataBackup.AddWidget(pc_SpinBoxData,
                                                                          C_PuiSvDbDataElement::eSPIN_BOX) ==
                                            C_NO_ERR);
                                 this->mc_MapTypeAndIndexToId.insert(
                                    C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::eSPIN_BOX),
                                                           this->mc_DataBackup.GetSpinBoxes().size() - 1UL),
                                    u64_CurUniqueId);
                              }
                           }
                           if (pc_Slider != NULL)
                           {
                              const C_PuiSvDbSlider * const pc_SliderData = pc_Dashboard->GetSlider(u32_Index);
                              if (pc_SliderData != NULL)
                              {
                                 tgl_assert(this->mc_DataBackup.AddWidget(pc_SliderData,
                                                                          C_PuiSvDbDataElement::eSLIDER) ==
                                            C_NO_ERR);
                                 this->mc_MapTypeAndIndexToId.insert(
                                    C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::eSLIDER),
                                                           this->mc_DataBackup.GetSliders().size() - 1UL),
                                    u64_CurUniqueId);
                              }
                           }
                           if (pc_ProgressBar != NULL)
                           {
                              const C_PuiSvDbProgressBar * const pc_ProgressBarData =
                                 pc_Dashboard->GetProgressBar(
                                    u32_Index);
                              if (pc_ProgressBarData != NULL)
                              {
                                 tgl_assert(this->mc_DataBackup.AddWidget(pc_ProgressBarData,
                                                                          C_PuiSvDbDataElement::
                                                                          ePROGRESS_BAR) == C_NO_ERR);
                                 this->mc_MapTypeAndIndexToId.insert(
                                    C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::ePROGRESS_BAR),
                                                           this->mc_DataBackup.GetProgressBars().size() - 1UL),
                                    u64_CurUniqueId);
                              }
                           }
                           if (pc_Toggle != NULL)
                           {
                              const C_PuiSvDbToggle * const pc_ToggleData =
                                 pc_Dashboard->GetToggle(
                                    u32_Index);
                              if (pc_ToggleData != NULL)
                              {
                                 tgl_assert(this->mc_DataBackup.AddWidget(pc_ToggleData, C_PuiSvDbDataElement::
                                                                          eTOGGLE) == C_NO_ERR);
                                 this->mc_MapTypeAndIndexToId.insert(
                                    C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::eTOGGLE),
                                                           this->mc_DataBackup.GetToggles().size() -
                                                           1UL),
                                    u64_CurUniqueId);
                              }
                           }
                           if (pc_PieChart != NULL)
                           {
                              const C_PuiSvDbPieChart * const pc_PieChartData =
                                 pc_Dashboard->GetPieChart(
                                    u32_Index);
                              if (pc_PieChartData != NULL)
                              {
                                 tgl_assert(this->mc_DataBackup.AddWidget(pc_PieChartData, C_PuiSvDbDataElement::
                                                                          ePIE_CHART) == C_NO_ERR);
                                 this->mc_MapTypeAndIndexToId.insert(
                                    C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::ePIE_CHART),
                                                           this->mc_DataBackup.GetPieCharts().size() -
                                                           1UL),
                                    u64_CurUniqueId);
                              }
                           }
                           if (pc_Table != NULL)
                           {
                              const C_PuiSvDbTable * const pc_TableData =
                                 pc_Dashboard->GetTable(
                                    u32_Index);
                              if (pc_TableData != NULL)
                              {
                                 tgl_assert(this->mc_DataBackup.AddWidget(pc_TableData, C_PuiSvDbDataElement::
                                                                          eTABLE) == C_NO_ERR);
                                 this->mc_MapTypeAndIndexToId.insert(
                                    C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::eTABLE),
                                                           this->mc_DataBackup.GetTables().size() - 1UL),
                                    u64_CurUniqueId);
                              }
                           }
                        }
                     }
                     //Check if data element(s) in rail, if yes then store
                     if (pc_Widget != NULL)
                     {
                        for (uint32_t u32_ItRegisteredDataElement = 0;
                             u32_ItRegisteredDataElement < pc_Widget->c_DataPoolElementsConfig.size();
                             ++u32_ItRegisteredDataElement)
                        {
                           const C_PuiSvDbNodeDataElementConfig & rc_Config =
                              pc_Widget->c_DataPoolElementsConfig[u32_ItRegisteredDataElement];
                           if (rc_Config.c_ElementId.GetIsValid() == true)
                           {
                              const QMap<C_OscNodeDataPoolListElementId,
                                         C_PuiSvReadDataConfiguration>::const_iterator c_Result =
                                 pc_View->GetReadRailAssignments().find(rc_Config.c_ElementId);
                              if (c_Result != pc_View->GetReadRailAssignments().end())
                              {
                                 this->mc_SavedRailAssignments.insert(c_Result.key(), c_Result.value());
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all data storage
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoAddDeleteBaseCommand::m_Clear(void)
{
   this->mc_MapTypeAndIndexToId.clear();
   this->mc_DataBackup.Clear();
   this->mc_SavedRailAssignments.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete all relevant items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoAddDeleteBaseCommand::m_Delete(void)
{
   C_SyvDaDashboardScene * const pc_Scene = dynamic_cast<C_SyvDaDashboardScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(pc_Scene->GetViewIndex());
      const std::vector<QGraphicsItem *> c_Items = this->m_GetSceneItems();
      for (std::vector<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end();
           ++c_ItItem)
      {
         pc_Scene->DeleteItem(*c_ItItem);
      }
      //Delete data rail(s) if not in use (anymore) else clear stored element
      if (pc_View != NULL)
      {
         for (QMap<C_OscNodeDataPoolListElementId,
                   C_PuiSvReadDataConfiguration>::iterator c_ItReadRailAssignment =
                 this->mc_SavedRailAssignments.begin();
              c_ItReadRailAssignment != this->mc_SavedRailAssignments.end();)
         {
            if (pc_View->CheckReadUsage(c_ItReadRailAssignment.key()) == true)
            {
               //Special param widget handling
               if (pc_View->CheckNonParamReadUsage(c_ItReadRailAssignment.key()) == true)
               {
                  //Delete internal (Implicit iteration step)
                  c_ItReadRailAssignment = this->mc_SavedRailAssignments.erase(c_ItReadRailAssignment);
               }
               else
               {
                  C_PuiSvReadDataConfiguration c_NewConfig;

                  //Replace data
                  c_NewConfig.e_TransmissionMode = C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER;
                  tgl_assert(C_PuiSvHandler::h_GetInstance()->RemoveViewReadRailItem(pc_Scene->GetViewIndex(),
                                                                                     c_ItReadRailAssignment.key()) ==
                             C_NO_ERR);
                  tgl_assert(C_PuiSvHandler::h_GetInstance()->AddViewReadRailItem(pc_Scene->GetViewIndex(),
                                                                                  c_ItReadRailAssignment.key(),
                                                                                  c_NewConfig) ==
                             C_NO_ERR);
                  //Explicit iteration step
                  ++c_ItReadRailAssignment;
               }
            }
            else
            {
               //Delete data
               tgl_assert(C_PuiSvHandler::h_GetInstance()->RemoveViewReadRailItem(pc_Scene->GetViewIndex(),
                                                                                  c_ItReadRailAssignment.key()) ==
                          C_NO_ERR);
               //Explicit iteration step
               ++c_ItReadRailAssignment;
            }
         }
      }
      //Trigger error recheck
      pc_Scene->TriggerErrorCheck();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get generic data storage for widget item

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] opc_DataElement     Data element

   \return
   NULL Widget not found
   Else Valid widget
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbWidgetBase * C_SyvDaUnoAddDeleteBaseCommand::mh_GetGenericWidget(const uint32_t ou32_ViewIndex,
                                                                                const uint32_t ou32_DashboardIndex,
                                                                                const C_PuiSvDbDataElement * const opc_DataElement)
{
   const C_PuiSvDbWidgetBase * pc_Retval = NULL;

   if ((opc_DataElement != NULL) && (opc_DataElement->GetIndex() >= 0))
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

      if (pc_View != NULL)
      {
         const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(ou32_DashboardIndex);
         if (pc_Dashboard != NULL)
         {
            const uint32_t u32_Index = static_cast<uint32_t>(opc_DataElement->GetIndex());
            const C_GiSvDaLabelBase * const pc_Label  =
               dynamic_cast<const C_GiSvDaLabelBase * const>(opc_DataElement);
            const C_GiSvDaParam * const pc_ParamWidget  =
               dynamic_cast<const C_GiSvDaParam * const>(opc_DataElement);
            const C_GiSvDaSpinBoxBase * const pc_SpinBox  =
               dynamic_cast<const C_GiSvDaSpinBoxBase * const>(opc_DataElement);
            const C_GiSvDaSliderBase * const pc_Slider  =
               dynamic_cast<const C_GiSvDaSliderBase * const>(opc_DataElement);
            const C_GiSvDaProgressBarBase * const pc_ProgressBar  =
               dynamic_cast<const C_GiSvDaProgressBarBase * const>(opc_DataElement);
            const C_GiSvDaToggleBase * const pc_Toggle  =
               dynamic_cast<const C_GiSvDaToggleBase * const>(opc_DataElement);
            const C_GiSvDaPieChartBase * const pc_PieChart  =
               dynamic_cast<const C_GiSvDaPieChartBase * const>(opc_DataElement);
            const C_GiSvDaTableBase * const pc_Table  =
               dynamic_cast<const C_GiSvDaTableBase * const>(opc_DataElement);
            if (pc_Label != NULL)
            {
               pc_Retval = pc_Dashboard->GetLabel(u32_Index);
            }
            if (pc_ParamWidget != NULL)
            {
               pc_Retval = pc_Dashboard->GetParam(u32_Index);
            }
            if (pc_SpinBox != NULL)
            {
               pc_Retval = pc_Dashboard->GetSpinBox(u32_Index);
            }
            if (pc_Slider != NULL)
            {
               pc_Retval = pc_Dashboard->GetSlider(u32_Index);
            }
            if (pc_ProgressBar != NULL)
            {
               pc_Retval = pc_Dashboard->GetProgressBar(u32_Index);
            }
            if (pc_Toggle != NULL)
            {
               pc_Retval = pc_Dashboard->GetToggle(u32_Index);
            }
            if (pc_PieChart != NULL)
            {
               pc_Retval = pc_Dashboard->GetPieChart(u32_Index);
            }
            if (pc_Table != NULL)
            {
               pc_Retval = pc_Dashboard->GetTable(u32_Index);
            }
         }
      }
   }

   return pc_Retval;
}
