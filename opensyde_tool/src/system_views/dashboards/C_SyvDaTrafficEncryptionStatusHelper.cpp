//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Util class for traffic encryption status functions

   Util class for traffic encryption status functions

   \copyright   Copyright 2026 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_SyvRoRouteCalculation.hpp"
#include "C_OscProtocolSecuritySubLayer.hpp"
#include "C_SyvDaTrafficEncryptionStatusHelper.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get mapping node to traffic encryption status

   \param[in]  ou32_ViewIndex             View index
   \param[in]  oq_ConnectionActiveStatus  Connection active status

   \return
   Mapping node to traffic encryption status
*/
//----------------------------------------------------------------------------------------------------------------------
QMap<uint32_t, bool> C_SyvDaTrafficEncryptionStatusHelper::h_GetMappingNodeToTrafficEncryptionStatus(
   const uint32_t ou32_ViewIndex, const bool oq_ConnectionActiveStatus)
{
   QMap<uint32_t, bool> c_Retval;
   if (oq_ConnectionActiveStatus)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);
      tgl_assert(pc_View != NULL);
      if (pc_View != NULL)
      {
         const std::vector<uint8_t> & rc_ActiveNodes = pc_View->GetNodeActiveFlags();
         tgl_assert(rc_ActiveNodes.size() == C_PuiSdHandler::h_GetInstance()->GetOscNodesSize());
         if (rc_ActiveNodes.size() == C_PuiSdHandler::h_GetInstance()->GetOscNodesSize())
         {
            for (uint32_t u32_ItNode = 0UL; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOscNodesSize();
                 ++u32_ItNode)
            {
               if (rc_ActiveNodes[u32_ItNode] == 1U)
               {
                  const C_SyvRoRouteCalculation c_RouteCalculation(ou32_ViewIndex, u32_ItNode,
                                                                   C_OscRoutingCalculation::eDIAGNOSTIC);

                  const int32_t s32_Retval = c_RouteCalculation.GetState();
                  c_Retval[u32_ItNode] = false;

                  // Can happen with disabled communication flags: no issue as there is no dashboard communication
                  if (s32_Retval == C_NO_ERR)
                  {
                     const C_OscRoutingRoute * const pc_Route = c_RouteCalculation.GetBestRoute();
                     tgl_assert(pc_Route != NULL);
                     if (pc_Route != NULL)
                     {
                        if (pc_Route->c_VecRoutePoints.size() > 0UL)
                        {
                           const C_OscRoutingRoutePoint & rc_LastPoint =
                              pc_Route->c_VecRoutePoints[pc_Route->c_VecRoutePoints.size() - 1UL];
                           mh_GetTrafficEncryptionStatusForNodeUsingBus(c_Retval, u32_ItNode,
                                                                        rc_LastPoint.u32_OutBusIndex);
                        }
                        else
                        {
                           mh_GetTrafficEncryptionStatusForNodeUsingBus(c_Retval, u32_ItNode,
                                                                        pc_View->GetOscPcData().GetBusIndex());
                        }
                     }
                  }
               }
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get view node encrypted

   \param[in]  orc_DataPoolElementId                     Datapool element ID
   \param[in]  oq_ConnectionActiveStatus                 Connection active status
   \param[in]  ou32_ViewIndex                            View index
   \param[in]  orc_MappingNodeToTrafficEncryptionStatus  Mapping node to traffic encryption status

   \return
   Flags

   \retval   True    View node encrypted
   \retval   False   View node not encrypted
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaTrafficEncryptionStatusHelper::h_GetViewNodeEncrypted(
   const opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
   const bool oq_ConnectionActiveStatus, const uint32_t ou32_ViewIndex, const QMap<uint32_t,
                                                                                   bool> & orc_MappingNodeToTrafficEncryptionStatus)
{
   bool q_Retval = false;

   if (oq_ConnectionActiveStatus == true)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

      if (pc_View != NULL)
      {
         C_PuiSvReadDataConfiguration c_RailConfig;

         if (pc_View->GetReadRailAssignment(orc_DataPoolElementId, c_RailConfig) == C_NO_ERR)
         {
            if ((c_RailConfig.e_TransmissionMode == C_PuiSvReadDataConfiguration::eTM_CYCLIC) ||
                (c_RailConfig.e_TransmissionMode == C_PuiSvReadDataConfiguration::eTM_ON_CHANGE))
            {
               const QMap<uint32_t,
                          bool>::const_iterator c_It =
                  orc_MappingNodeToTrafficEncryptionStatus.constFind(orc_DataPoolElementId.u32_NodeIndex);
               if (c_It != orc_MappingNodeToTrafficEncryptionStatus.cend())
               {
                  q_Retval = c_It.value();
               }
            }
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaTrafficEncryptionStatusHelper::C_SyvDaTrafficEncryptionStatusHelper()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get traffic encryption status for node using just the bus

   \param[in,out]  orc_MappingNodeToTrafficEncryptionStatus    Mapping node to traffic encryption status
   \param[in]      ou32_NodeIndex                              Node index
   \param[in]      ou32_BusIndex                               Bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTrafficEncryptionStatusHelper::mh_GetTrafficEncryptionStatusForNodeUsingBus(QMap<uint32_t,
                                                                                             bool> & orc_MappingNodeToTrafficEncryptionStatus, const uint32_t ou32_NodeIndex,
                                                                                        const uint32_t ou32_BusIndex)
{
   const C_OscNode * const pc_Node =
      C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      for (uint32_t u32_ItInterface = 0UL;
           u32_ItInterface < pc_Node->c_Properties.c_ComInterfaces.size(); ++u32_ItInterface)
      {
         const C_OscNodeComInterfaceSettings & rc_Interface =
            pc_Node->c_Properties.c_ComInterfaces[u32_ItInterface];
         if (rc_Interface.GetBusConnected() &&
             (rc_Interface.u32_BusIndex == ou32_BusIndex))
         {
            mh_GetTrafficEncryptionStatusForNodeUsingBusAndNodeId(orc_MappingNodeToTrafficEncryptionStatus,
                                                                  ou32_NodeIndex,
                                                                  ou32_BusIndex,
                                                                  rc_Interface.u8_NodeId);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get traffic encryption status for node using bus and node ID

   \param[in,out]  orc_MappingNodeToTrafficEncryptionStatus    Mapping node to traffic encryption status
   \param[in]      ou32_NodeIndex                              Node index
   \param[in]      ou32_BusIndex                               Bus index
   \param[in]      ou8_NodeId                                  Node ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaTrafficEncryptionStatusHelper::mh_GetTrafficEncryptionStatusForNodeUsingBusAndNodeId(QMap<uint32_t,
                                                                                                      bool> & orc_MappingNodeToTrafficEncryptionStatus, const uint32_t ou32_NodeIndex, const uint32_t ou32_BusIndex,
                                                                                                 const uint8_t ou8_NodeId)
{
   const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(ou32_BusIndex);

   tgl_assert(pc_Bus != NULL);
   if ((pc_Bus != NULL) && (pc_Bus->e_Type != C_OscSystemBus::eETHERNET))
   {
      const C_OscProtocolDriverOsyNode c_NodeId(pc_Bus->u8_BusId, ou8_NodeId);
      const C_OscProtocolSecuritySubLayer * const pc_NodeStatus =
         C_OscProtocolSecuritySubLayer::h_GetConfigByNodeId(c_NodeId);

      if (pc_NodeStatus != NULL)
      {
         if (pc_NodeStatus->GetEncryptionIsActive())
         {
            orc_MappingNodeToTrafficEncryptionStatus[ou32_NodeIndex] = true;
         }
      }
   }
}
