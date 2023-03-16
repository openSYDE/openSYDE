//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add new items based on snapshot information (implementation)

   Add new items based on snapshot information

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_SyvDaDashboardScene.hpp"
#include "C_SyvDaUnoAddSnapshotCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
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

   \param[in,out] opc_Scene               Pointer to currently active scene
   \param[in]     orc_InitialSnapshotData Initial snapshot data
   \param[in]     orc_Ids                 Affected unique IDs
   \param[in]     orc_RestoredRails       Rails to restore if possible
   \param[in]     orc_NewPos              New position
   \param[in]     of64_HighestUsedZetValue  Highest used Z value
   \param[in,out] opc_Parent              Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaUnoAddSnapshotCommand::C_SyvDaUnoAddSnapshotCommand(QGraphicsScene * const opc_Scene,
                                                           const C_SyvDaDashboardSnapshot & orc_InitialSnapshotData,
                                                           const std::vector<uint64_t> & orc_Ids,
                                                           const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                                                      C_PuiSvReadDataConfiguration> & orc_RestoredRails, const QPointF & orc_NewPos, const float64_t of64_HighestUsedZetValue,
                                                           QUndoCommand * const opc_Parent) :
   C_SyvDaUnoAddBaseCommand(opc_Scene, orc_Ids, "Paste drawing element(s)",
                            opc_Parent, orc_InitialSnapshotData)
{
   //Handle pos
   this->m_SetDataPositionOffset(orc_NewPos);
   //Handle Z
   this->m_SetDataZetOffset(of64_HighestUsedZetValue);
   //Handle rails
   for (QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
             C_PuiSvReadDataConfiguration>::const_iterator c_It = orc_RestoredRails.begin();
        c_It != orc_RestoredRails.end(); ++c_It)
   {
      //Should never return anything but C_NO_ERR
      this->m_AddReadRailToInternalBackup(c_It.key(), c_It.value());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaUnoAddSnapshotCommand::~C_SyvDaUnoAddSnapshotCommand(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add elements without previous knowledge
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoAddSnapshotCommand::m_AddNew(void)
{
   C_SyvDaDashboardScene * const pc_Scene = dynamic_cast<C_SyvDaDashboardScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      QMap<C_PuiBsTemporaryDataId, uint64_t> c_IdMap;
      const C_PuiSvDashboard c_InitialData = this->m_GetDataBackup();
      const std::vector<uint64_t> c_AllIds = this->m_GetIds();
      const uint32_t u32_ItemCount = c_InitialData.Count();
      if (u32_ItemCount <= c_AllIds.size())
      {
         uint32_t u32_ItId = 0;
         //Map IDs

         //Labels
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.GetLabels().size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::eLABEL),
                                                  u32_ItItem), c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Params
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.GetParams().size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::ePARAM),
                                                  u32_ItItem), c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Pie charts
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.GetPieCharts().size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::ePIE_CHART),
                                                  u32_ItItem), c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Spin boxes
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.GetSpinBoxes().size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::eSPIN_BOX),
                                                  u32_ItItem), c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Sliders
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.GetSliders().size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::eSLIDER),
                                                  u32_ItItem), c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Tables
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.GetTables().size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::eTABLE),
                                                  u32_ItItem), c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Progress bars
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.GetLabels().size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::ePROGRESS_BAR),
                                                  u32_ItItem), c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Toggles
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.GetToggles().size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::eTOGGLE),
                                                  u32_ItItem), c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Boundaries
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.c_Boundaries.size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::eBOUNDARY),
                                                  u32_ItItem),
                           c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Images
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.c_Images.size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::eIMAGE),
                                                  u32_ItItem), c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Line arrows
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.c_LineArrows.size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::eLINE_ARROW),
                                                  u32_ItItem),
                           c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Text elements
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.c_TextElements.size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSvDbDataElement::eTEXT_ELEMENT),
                                                  u32_ItItem),
                           c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Read rails
         //Check if any were set by constructor otherwise use default implementation
         if (this->m_GetStoredReadRailCount() == 0)
         {
            m_InitialReadRailHandling();
         }
         else
         {
            m_RestoreReadRailsOnly();
         }
         pc_Scene->CopyFromSnapshotToScene(c_InitialData, &c_IdMap);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initial read rail handling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoAddSnapshotCommand::m_InitialReadRailHandling(void)
{
   C_SyvDaDashboardScene * const pc_Scene = dynamic_cast<C_SyvDaDashboardScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(pc_Scene->GetViewIndex());
      if (pc_View != NULL)
      {
         std::vector<const C_PuiSvDbWidgetBase *> c_Widgets;
         const C_PuiSvDashboard c_InitialData = this->m_GetDataBackup();
         c_InitialData.GetAllWidgetItems(c_Widgets);
         for (uint32_t u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
         {
            const C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
            if ((pc_Widget != NULL) && (pc_Widget->IsReadElement() == true))
            {
               for (uint32_t u32_ItItem = 0; u32_ItItem < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItItem)
               {
                  const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Widget->c_DataPoolElementsConfig[u32_ItItem];
                  if ((rc_Config.c_ElementId.GetIsValid() == true) &&
                      (rc_Config.c_ElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT))
                  {
                     const stw::opensyde_core::C_OscNodeDataPoolListElement * const pc_Element =
                        C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(
                           rc_Config.c_ElementId.u32_NodeIndex,
                           rc_Config.c_ElementId.u32_DataPoolIndex, rc_Config.c_ElementId.u32_ListIndex,
                           rc_Config.c_ElementId.u32_ElementIndex);
                     tgl_assert(pc_Element != NULL);
                     if (pc_Element != NULL)
                     {
                        if (pc_View->CheckReadUsage(rc_Config.c_ElementId) == false)
                        {
                           C_PuiSvReadDataConfiguration c_Config;
                           tgl_assert(c_Config.InitDefaultThreshold(pc_Element->c_MinValue,
                                                                    pc_Element->c_MaxValue) == C_NO_ERR);
                           //Definitely add of read rail required
                           if (this->m_AddReadRailToInternalBackup(rc_Config.c_ElementId, c_Config) == C_NO_ERR)
                           {
                              //No explicit undo step after this so this has to be done manually
                              tgl_assert(C_PuiSvHandler::h_GetInstance()->AddViewReadRailItem(pc_Scene->GetViewIndex(),
                                                                                              rc_Config.c_ElementId,
                                                                                              c_Config) == C_NO_ERR);
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
