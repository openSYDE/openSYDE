//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Add new items based on snapshot information (implementation)

   Add new items based on snapshot information

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

#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_SyvDaDashboardScene.h"
#include "C_SyvDaUnoAddSnapshotCommand.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
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

   \param[in,out] opc_Scene               Pointer to currently active scene
   \param[in]     orc_InitialSnapshotData Initial snapshot data
   \param[in]     orc_IDs                 Affected unique IDs
   \param[in]     orc_RestoredRails       Rails to restore if possible
   \param[in]     orc_NewPos              New position
   \param[in,out] opc_Parent              Optional pointer to parent

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaUnoAddSnapshotCommand::C_SyvDaUnoAddSnapshotCommand(QGraphicsScene * const opc_Scene,
                                                           const C_PuiSvDashboard & orc_InitialSnapshotData,
                                                           const std::vector<uint64> & orc_IDs,
                                                           const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                                      C_PuiSvReadDataConfiguration> & orc_RestoredRails,
                                                           const QPointF & orc_NewPos,
                                                           QUndoCommand * const opc_Parent) :
   C_SyvDaUnoAddBaseCommand(opc_Scene, orc_IDs, "Paste drawing element(s)",
                            opc_Parent, orc_InitialSnapshotData),
   mc_NewPos(orc_NewPos)
{
   for (QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
             C_PuiSvReadDataConfiguration>::const_iterator c_It = orc_RestoredRails.begin();
        c_It != orc_RestoredRails.end(); ++c_It)
   {
      //Should never return anything but C_NO_ERR
      this->AddReadRailToInternalBackup(c_It.key(), c_It.value());
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     24.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaUnoAddSnapshotCommand::~C_SyvDaUnoAddSnapshotCommand(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add elements without previous knowledge

   \created     24.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaUnoAddSnapshotCommand::m_AddNew(void)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SyvDaDashboardScene * const pc_Scene = dynamic_cast<C_SyvDaDashboardScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      QMap<C_PuiBsTemporaryDataID, uint64> c_IDMap;
      uint32 u32_ItID = 0;
      const C_PuiSvDashboard c_InitialData = this->GetDataBackup();
      const std::vector<stw_types::uint64> c_AllIDs = this->GetIDs();
      const uint32 u32_ItemCount = c_InitialData.Count();
      if (u32_ItemCount <= c_AllIDs.size())
      {
         //Map IDs

         //Labels
         for (uint32 u32_ItItem = 0; u32_ItItem < c_InitialData.GetLabels().size(); ++u32_ItItem)
         {
            c_IDMap.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSvDbDataElement::eLABEL),
                                                  u32_ItItem), c_AllIDs[u32_ItID]);
            ++u32_ItID;
         }
         //Params
         for (uint32 u32_ItItem = 0; u32_ItItem < c_InitialData.GetParams().size(); ++u32_ItItem)
         {
            c_IDMap.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSvDbDataElement::ePARAM),
                                                  u32_ItItem), c_AllIDs[u32_ItID]);
            ++u32_ItID;
         }
         //Pie charts
         for (uint32 u32_ItItem = 0; u32_ItItem < c_InitialData.GetPieCharts().size(); ++u32_ItItem)
         {
            c_IDMap.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSvDbDataElement::ePIE_CHART),
                                                  u32_ItItem), c_AllIDs[u32_ItID]);
            ++u32_ItID;
         }
         //Spin boxes
         for (uint32 u32_ItItem = 0; u32_ItItem < c_InitialData.GetSpinBoxes().size(); ++u32_ItItem)
         {
            c_IDMap.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSvDbDataElement::eSPIN_BOX),
                                                  u32_ItItem), c_AllIDs[u32_ItID]);
            ++u32_ItID;
         }
         //Charts
         for (uint32 u32_ItItem = 0; u32_ItItem < c_InitialData.GetCharts().size(); ++u32_ItItem)
         {
            c_IDMap.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSvDbDataElement::eCHART),
                                                  u32_ItItem), c_AllIDs[u32_ItID]);
            ++u32_ItID;
         }
         //Sliders
         for (uint32 u32_ItItem = 0; u32_ItItem < c_InitialData.GetSliders().size(); ++u32_ItItem)
         {
            c_IDMap.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSvDbDataElement::eSLIDER),
                                                  u32_ItItem), c_AllIDs[u32_ItID]);
            ++u32_ItID;
         }
         //Tables
         for (uint32 u32_ItItem = 0; u32_ItItem < c_InitialData.GetTables().size(); ++u32_ItItem)
         {
            c_IDMap.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSvDbDataElement::eTABLE),
                                                  u32_ItItem), c_AllIDs[u32_ItID]);
            ++u32_ItID;
         }
         //Progress bars
         for (uint32 u32_ItItem = 0; u32_ItItem < c_InitialData.GetLabels().size(); ++u32_ItItem)
         {
            c_IDMap.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSvDbDataElement::ePROGRESS_BAR),
                                                  u32_ItItem), c_AllIDs[u32_ItID]);
            ++u32_ItID;
         }
         //Toggles
         for (uint32 u32_ItItem = 0; u32_ItItem < c_InitialData.GetToggles().size(); ++u32_ItItem)
         {
            c_IDMap.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSvDbDataElement::eTOGGLE),
                                                  u32_ItItem), c_AllIDs[u32_ItID]);
            ++u32_ItID;
         }
         //Boundaries
         for (uint32 u32_ItItem = 0; u32_ItItem < c_InitialData.c_Boundaries.size(); ++u32_ItItem)
         {
            c_IDMap.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSvDbDataElement::eBOUNDARY),
                                                  u32_ItItem),
                           c_AllIDs[u32_ItID]);
            ++u32_ItID;
         }
         //Images
         for (uint32 u32_ItItem = 0; u32_ItItem < c_InitialData.c_Images.size(); ++u32_ItItem)
         {
            c_IDMap.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSvDbDataElement::eIMAGE),
                                                  u32_ItItem), c_AllIDs[u32_ItID]);
            ++u32_ItID;
         }
         //Line arrows
         for (uint32 u32_ItItem = 0; u32_ItItem < c_InitialData.c_LineArrows.size(); ++u32_ItItem)
         {
            c_IDMap.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSvDbDataElement::eLINE_ARROW),
                                                  u32_ItItem),
                           c_AllIDs[u32_ItID]);
            ++u32_ItID;
         }
         //Text elements
         for (uint32 u32_ItItem = 0; u32_ItItem < c_InitialData.c_TextElements.size(); ++u32_ItItem)
         {
            c_IDMap.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSvDbDataElement::eTEXT_ELEMENT),
                                                  u32_ItItem),
                           c_AllIDs[u32_ItID]);
            ++u32_ItID;
         }
         //Read rails
         //Check if any were set by constructor otherwise use default implementation
         if (this->GetStoredReadRailCount() == 0)
         {
            m_InitialReadRailHandling();
         }
         else
         {
            m_RestoreReadRailsOnly();
         }
         pc_Scene->CopyFromSnapshotToScene(c_InitialData, &(this->mc_NewPos), &c_IDMap);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initial read rail handling

   \created     29.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaUnoAddSnapshotCommand::m_InitialReadRailHandling(void)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SyvDaDashboardScene * const pc_Scene = dynamic_cast<C_SyvDaDashboardScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(pc_Scene->GetViewIndex());
      if (pc_View != NULL)
      {
         std::vector<const C_PuiSvDbWidgetBase *> c_Widgets;
         const C_PuiSvDashboard c_InitialData = this->GetDataBackup();
         c_InitialData.GetAllWidgetItems(c_Widgets);
         for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
         {
            const C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
            if ((pc_Widget != NULL) && (pc_Widget->IsReadElement() == true))
            {
               for (uint32 u32_ItItem = 0; u32_ItItem < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItItem)
               {
                  const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Widget->c_DataPoolElementsConfig[u32_ItItem];
                  if ((rc_Config.c_ElementId.GetIsValid() == true) &&
                      (rc_Config.c_ElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT))
                  {
                     const stw_opensyde_core::C_OSCNodeDataPoolListElement * const pc_Element =
                        C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
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
                           if (this->AddReadRailToInternalBackup(rc_Config.c_ElementId, c_Config) == C_NO_ERR)
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
