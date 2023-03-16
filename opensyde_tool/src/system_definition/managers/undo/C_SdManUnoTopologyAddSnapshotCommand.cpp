//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add new items based on snapshot information (implementation)

   Add new items based on snapshot information

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_SdTopologyScene.hpp"
#include "C_SdManUnoTopologyAddSnapshotCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Scene                 Pointer to currently active scene
   \param[in]      orc_InitialSnapshotData   Initial snapshot data
   \param[in]      orc_Ids                   Affected unique IDs
   \param[in]      orc_NewPos                New position
   \param[in]      of64_HighestUsedZetValue    Highest used Z value
   \param[in,out]  opc_Parent                Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyAddSnapshotCommand::C_SdManUnoTopologyAddSnapshotCommand(QGraphicsScene * const opc_Scene,
                                                                           const C_SdTopologyDataSnapshot & orc_InitialSnapshotData,
                                                                           const std::vector<uint64_t> & orc_Ids, const QPointF & orc_NewPos, const float64_t of64_HighestUsedZetValue,
                                                                           QUndoCommand * const opc_Parent) :
   C_SdManUnoTopologyAddBaseCommand(opc_Scene, orc_Ids, "Paste drawing element(s)",
                                    opc_Parent, orc_InitialSnapshotData)
{
   //Handle pos
   this->m_SetDataPositionOffset(orc_NewPos);
   //Handle Z
   this->m_SetDataZetOffset(of64_HighestUsedZetValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyAddSnapshotCommand::~C_SdManUnoTopologyAddSnapshotCommand(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add elements without previous knowledge
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddSnapshotCommand::m_AddNew(void)
{
   C_SdTopologyScene * const pc_Scene = dynamic_cast<C_SdTopologyScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      QMap<C_PuiBsTemporaryDataId, uint64_t> c_IdMap;
      const C_SdTopologyDataSnapshot c_InitialData = this->GetDataBackup();
      const std::vector<uint64_t> c_AllIds = this->m_GetIds();
      const uint32_t u32_ItemCount = c_InitialData.Count();
      if (u32_ItemCount <= c_AllIds.size())
      {
         uint32_t u32_ItId = 0;
         //Map IDs
         uint32_t u32_Counter = 0UL;

         //Nodes
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.c_UiNodes.size(); ++u32_ItItem)
         {
            const bool q_IsFirst = C_PuiSdUtil::h_CheckIsFirstInAnyGroupOrNotInAny(u32_ItItem,
                                                                                   c_InitialData.c_OscNodeGroups);
            if (q_IsFirst)
            {
               c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement::eNODE),
                                                     u32_Counter), c_AllIds[u32_ItId]);
               ++u32_ItId;
               ++u32_Counter;
            }
         }
         //Buses
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.c_OscBuses.size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement::eBUS),
                                                  u32_ItItem), c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Boundaries
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.c_Boundaries.size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement::eBOUNDARY),
                                                  u32_ItItem),
                           c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Images
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.c_Images.size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement::eIMAGE),
                                                  u32_ItItem), c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Line arrows
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.c_LineArrows.size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement::eLINE_ARROW),
                                                  u32_ItItem),
                           c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Text elements
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.c_TextElements.size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement::eTEXT_ELEMENT),
                                                  u32_ItItem),
                           c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Bus connector
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.c_BusConnections.size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement::eBUS_CONNECTOR),
                                                  u32_ItItem), c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         //Bus text elements
         for (uint32_t u32_ItItem = 0; u32_ItItem < c_InitialData.c_BusTextElements.size(); ++u32_ItItem)
         {
            c_IdMap.insert(C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement::eTEXT_ELEMENT_BUS),
                                                  u32_ItItem), c_AllIds[u32_ItId]);
            ++u32_ItId;
         }
         pc_Scene->CopyFromSnapshotToScene(c_InitialData, &c_IdMap);
      }
   }
}
