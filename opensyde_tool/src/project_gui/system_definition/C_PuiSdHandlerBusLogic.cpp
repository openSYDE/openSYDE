//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus logic part of UI system definition

   Bus logic part of UI system definition

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QMap>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_OscCanUtil.hpp"
#include "C_OscUtils.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_PuiSdHandlerBusLogic.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
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
/*! \brief   Add bus

   \param[in,out]  orc_OscBus             New OSC bus (name might be modified by this function if not unique)
   \param[in]      orc_UiBus              UI bus
   \param[in]      opc_Name               New bus name
   \param[in]      oq_AllowBusIdAdaption  Flag if bus ID adaption is allowed

   \return
   Index of bus
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_PuiSdHandlerBusLogic::AddBusAndSort(C_OscSystemBus & orc_OscBus, const C_PuiSdBus & orc_UiBus,
                                               const QString * const opc_Name, const bool oq_AllowBusIdAdaption)
{
   const uint32_t u32_Index = mc_CoreDefinition.c_Buses.size();

   if (opc_Name != NULL)
   {
      orc_OscBus.c_Name = opc_Name->toStdString().c_str();
   }
   else
   {
      orc_OscBus.c_Name = C_OscUtils::h_GetUniqueName(
         this->GetExistingBusNames(), orc_OscBus.c_Name, this->GetNameMaxCharLimit());
   }
   if (oq_AllowBusIdAdaption == true)
   {
      mc_CoreDefinition.GetNextFreeBusId(orc_OscBus.u8_BusId);
   }

   mc_CoreDefinition.AddBus(orc_OscBus);

   //insert UI part at same position as OSC part:
   this->mc_UiBuses.insert(mc_UiBuses.begin() + u32_Index, orc_UiBus);

   //signal "bus change"
   Q_EMIT this->SigBussesChanged();
   Q_EMIT this->SigSyncBusAdded(u32_Index);

   return u32_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pointers to all currently registered bus names

   \return
   Vector of pointers to all currently registered bus names
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<C_SclString, bool> C_PuiSdHandlerBusLogic::GetExistingBusNames(void) const
{
   std::map<C_SclString, bool> c_Retval;
   for (uint32_t u32_ItBus = 0; u32_ItBus < this->mc_CoreDefinition.c_Buses.size(); ++u32_ItBus)
   {
      c_Retval[this->mc_CoreDefinition.c_Buses[u32_ItBus].c_Name] = true;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set UI bus from array

   \param[in]  ou32_Index  Index
   \param[in]  orc_Item    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::SetUiBus(const uint32_t ou32_Index, const C_PuiSdBus & orc_Item)
{
   if (ou32_Index < this->mc_UiBuses.size())
   {
      mc_UiBuses[ou32_Index] = orc_Item;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set OSC bus from array

   \param[in]  ou32_Index  Index
   \param[in]  orc_Item    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::SetOscBus(const uint32_t ou32_Index, const C_OscSystemBus & orc_Item)
{
   if (ou32_Index < this->mc_CoreDefinition.c_Buses.size())
   {
      this->mc_CoreDefinition.c_Buses[ou32_Index] = orc_Item;
   }

   //Signal new name!
   Q_EMIT (this->SigBusChanged(ou32_Index));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get UI bus from array

   Get UI bus from array

   \param[in]  oru32_Index    Index

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdBus * C_PuiSdHandlerBusLogic::GetUiBus(const uint32_t & oru32_Index) const
{
   const C_PuiSdBus * pc_Retval;

   if (oru32_Index < this->mc_UiBuses.size())
   {
      pc_Retval = &(mc_UiBuses[oru32_Index]);
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC bus from array

   Get OSC bus from array

   \param[in]  oru32_Index    Index

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscSystemBus * C_PuiSdHandlerBusLogic::GetOscBus(const uint32_t & oru32_Index) const
{
   const C_OscSystemBus * pc_Retval;

   if (oru32_Index < this->mc_CoreDefinition.c_Buses.size())
   {
      pc_Retval = &(this->mc_CoreDefinition.c_Buses[oru32_Index]);
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if any bus uses the provided name

   \param[in]      orc_Name               Bus name to check for
   \param[in]      opu32_BusIndexToSkip   Optional parameter to skip one index
                                          (Use-case: skip current bus to avoid conflict with itself)
   \param[in,out]  opc_ExistingNames      Optional parameter to list all OTHER existing bus names

   \return
   true  Available
   false Already in use
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerBusLogic::CheckBusNameAvailable(const C_SclString & orc_Name,
                                                   const uint32_t * const opu32_BusIndexToSkip,
                                                   std::vector<stw::scl::C_SclString> * const opc_ExistingNames) const
{
   bool q_Retval = true;

   //Either end on error or continue if all node names are requested
   for (uint32_t u32_ItBus = 0;
        (u32_ItBus < this->mc_CoreDefinition.c_Buses.size()) && ((q_Retval == true) || (opc_ExistingNames != NULL));
        ++u32_ItBus)
   {
      bool q_Skip = false;
      if (opu32_BusIndexToSkip != NULL)
      {
         if (*opu32_BusIndexToSkip == u32_ItBus)
         {
            q_Skip = true;
         }
      }
      if (q_Skip == false)
      {
         const C_OscSystemBus & rc_Bus = this->mc_CoreDefinition.c_Buses[u32_ItBus];
         //Check conflict
         if (rc_Bus.c_Name.LowerCase() == orc_Name.LowerCase())
         {
            q_Retval = false;
         }
         //Store other (not checked) bus name
         if (opc_ExistingNames != NULL)
         {
            opc_ExistingNames->push_back(rc_Bus.c_Name);
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove bus

   \param[in]  ou32_BusIndex  Connector bus index (0 -> first bus)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::RemoveBus(const uint32_t ou32_BusIndex)
{
   uint32_t u32_Counter;

   this->mc_CoreDefinition.DeleteBus(ou32_BusIndex);
   this->mc_UiBuses.erase(this->mc_UiBuses.begin() + ou32_BusIndex);

   // Sync bus text elements indices
   for (u32_Counter = 0U; u32_Counter < this->c_BusTextElements.size(); ++u32_Counter)
   {
      if (this->c_BusTextElements[u32_Counter].u32_BusIndex > ou32_BusIndex)
      {
         this->c_BusTextElements[u32_Counter].u32_BusIndex = this->c_BusTextElements[u32_Counter].u32_BusIndex - 1U;
      }
   }

   //signal "bus change"
   this->m_HandleSyncBusDeleted(ou32_BusIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if there is a bus conflict

   \param[in]  ou32_BusIndex  Bus index

   \return
   true  Conflict
   false No conflict
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerBusLogic::CheckBusConflict(const uint32_t ou32_BusIndex) const
{
   bool q_NameConflict;
   bool q_IdInvalid;
   static QMap<std::vector<uint32_t>, bool> hc_PreviousResults;

   QMap<std::vector<uint32_t>, bool>::const_iterator c_It;
   std::vector<uint32_t> c_Hashes;
   std::vector<uint32_t> c_NodeIndexes;
   std::vector<uint32_t> c_InterfaceIndexes;
   bool q_Retval;

   //get all required hashes
   this->mc_CoreDefinition.GetNodeIndexesOfBus(ou32_BusIndex, c_NodeIndexes, c_InterfaceIndexes);
   //Bus hash
   c_Hashes.push_back(this->m_GetHashBus(ou32_BusIndex));
   //Nodes hashes
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
   {
      c_Hashes.push_back(this->m_GetHashNode(c_NodeIndexes[u32_ItNode]));
   }

   //Lookup if there is a result for this hash
   c_It = hc_PreviousResults.find(c_Hashes);
   if (c_It == hc_PreviousResults.end())
   {
      bool q_NameEmpty;

      std::vector<QString> c_InvalidNodesForBitRate;
      std::vector<C_OscCanProtocol::E_Type> c_InvalidProtocols;

      //Do all checks
      if (this->CheckBusConflictDetailed(ou32_BusIndex, &q_NameConflict, &q_NameEmpty, &q_IdInvalid,
                                         &c_InvalidNodesForBitRate, &c_InvalidProtocols) == C_NO_ERR)
      {
         if (((((q_NameConflict == true) || (q_NameEmpty == true)) || (q_IdInvalid == true)) ||
              (c_InvalidNodesForBitRate.size() > 0UL)) || (c_InvalidProtocols.size() > 0UL))
         {
            q_Retval = true;
         }
         else
         {
            q_Retval = false;
         }
         //Store for future reference (without conflict checks)
         if (((q_NameEmpty == true) || (c_InvalidNodesForBitRate.size() > 0UL)) ||
             (c_InvalidProtocols.size() > 0UL))
         {
            hc_PreviousResults.insert(c_Hashes, true);
         }
         else
         {
            hc_PreviousResults.insert(c_Hashes, false);
         }
      }
      else
      {
         q_Retval = true;
      }
   }
   else
   {
      //rely on hash and do name & id conflict check
      if (this->CheckBusConflictDetailed(ou32_BusIndex, &q_NameConflict, NULL, &q_IdInvalid, NULL, NULL) == C_NO_ERR)
      {
         if ((q_NameConflict == true) || (q_IdInvalid == true))
         {
            q_Retval = true;
         }
         else
         {
            q_Retval = c_It.value();
         }
      }
      else
      {
         q_Retval = true;
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for bus conflict details

   \param[in]   ou32_BusIndex                Bus index
   \param[out]  opq_NameConflict             Name conflict
   \param[out]  opq_NameEmpty                Name empty
   \param[out]  opq_IdInvalid                Id out of range
   \param[out]  opc_InvalidNodesForBitRate   Additional info for bit rate error
   \param[out]  opc_InvalidProtocols         Additional info for protocol error

   \return
   true  Conflict
   false No conflict
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::CheckBusConflictDetailed(const uint32_t ou32_BusIndex, bool * const opq_NameConflict,
                                                         bool * const opq_NameEmpty, bool * const opq_IdInvalid,
                                                         std::vector<QString> * const opc_InvalidNodesForBitRate,
                                                         std::vector<C_OscCanProtocol::E_Type> * const opc_InvalidProtocols)
const
{
   int32_t s32_Retval = C_NO_ERR;

   //Check bus errors
   if (this->mc_CoreDefinition.CheckErrorBus(ou32_BusIndex, opq_NameConflict, opq_NameEmpty, opq_IdInvalid,
                                             NULL) == C_NO_ERR)
   {
      if ((opc_InvalidNodesForBitRate != NULL) ||
          (opc_InvalidProtocols != NULL))
      {
         std::vector<uint32_t> c_ConnectedNodes;
         std::vector<uint32_t> c_ConnectedInterfaces;
         uint32_t u32_NodeCounter;

         this->mc_CoreDefinition.GetNodeIndexesOfBus(ou32_BusIndex, c_ConnectedNodes, c_ConnectedInterfaces);

         // Check current bitrate
         if (opc_InvalidNodesForBitRate != NULL)
         {
            const C_OscSystemBus * const pc_CheckedBus = this->GetOscBus(ou32_BusIndex);

            if ((pc_CheckedBus != NULL) &&
                (pc_CheckedBus->e_Type == C_OscSystemBus::eCAN)) // Only relevant for CAN
            {
               const uint32_t u32_CurrentBitrate = static_cast<uint32_t>(pc_CheckedBus->u64_BitRate / 1000ULL);

               // Every node must support the CAN bitrate
               for (u32_NodeCounter = 0U; u32_NodeCounter < c_ConnectedNodes.size(); ++u32_NodeCounter)
               {
                  const C_OscNode * const pc_Node = this->GetOscNodeConst(c_ConnectedNodes[u32_NodeCounter]);

                  if (pc_Node != NULL)
                  {
                     uint32_t u32_BitrateCounter;
                     bool q_BitrateFound = false;
                     tgl_assert(pc_Node->pc_DeviceDefinition != NULL);

                     for (u32_BitrateCounter = 0U;
                          u32_BitrateCounter < pc_Node->pc_DeviceDefinition->c_SupportedBitrates.size();
                          ++u32_BitrateCounter)
                     {
                        if (u32_CurrentBitrate == pc_Node->pc_DeviceDefinition->c_SupportedBitrates[u32_BitrateCounter])
                        {
                           q_BitrateFound = true;
                           break;
                        }
                     }

                     if (q_BitrateFound == false)
                     {
                        // Bitrate not supported
                        opc_InvalidNodesForBitRate->emplace_back(pc_Node->c_Properties.c_Name.c_str());
                     }
                  }
               }
            }
         }

         if (opc_InvalidProtocols != NULL)
         {
            for (uint8_t u8_ItProt = 0; u8_ItProt < C_OscCanProtocol::hc_ALL_PROTOCOLS.size(); ++u8_ItProt)
            {
               //Check message errors
               bool q_MessageNameConflict;
               bool q_MessageIdInvalid;
               bool q_MessagesHaveNoTx;
               bool q_DelayTimeInvalid;
               bool q_MessageSignalInvalid;
               bool q_TxMethodInvalid;
               bool q_ErrorDetected = false;

               const C_OscCanProtocol::E_Type e_Type = C_OscCanProtocol::hc_ALL_PROTOCOLS[u8_ItProt];
               C_PuiSdNodeCanMessageSyncManager c_SyncMan;

               c_SyncMan.Init(ou32_BusIndex, e_Type);

               c_SyncMan.CheckErrorBus(&q_MessageNameConflict, &q_MessageIdInvalid, &q_MessagesHaveNoTx,
                                       &q_DelayTimeInvalid,
                                       &q_TxMethodInvalid,
                                       &q_MessageSignalInvalid,
                                       C_OscCanProtocol::h_GetCanMessageValidSignalsDlcOffset(e_Type),
                                       C_OscCanProtocol::h_GetCanMessageSignalGapsValid(e_Type),
                                       C_OscCanProtocol::h_GetCanMessageSignalByteAlignmentRequired(e_Type),
                                       C_OscCanProtocol::h_GetCanMessageSignalsRequired(e_Type));
               if ((q_MessageNameConflict == false) && (q_MessageIdInvalid == false) &&
                   (q_DelayTimeInvalid == false) && (q_MessageSignalInvalid == false) &&
                   (q_TxMethodInvalid == false) && (q_MessagesHaveNoTx == false))
               {
                  //No error
               }
               else
               {
                  opc_InvalidProtocols->push_back(e_Type);
                  q_ErrorDetected = true;
               }

               if (q_ErrorDetected == false)
               {
                  //Check protocol errors of specific nodes
                  for (u32_NodeCounter = 0U; u32_NodeCounter < c_ConnectedNodes.size(); ++u32_NodeCounter)
                  {
                     const C_OscNode * const pc_Node = this->GetOscNodeConst(c_ConnectedNodes[u32_NodeCounter]);

                     if (pc_Node != NULL)
                     {
                        bool q_TempCommMaxRxSignalCountInvalid;
                        bool q_TempCommMaxTxSignalCountInvalid;
                        bool q_TempCoRxPdoCountInvalid;
                        bool q_TempCoTxPdoCountInvalid;
                        bool q_TempCommMinSignalCountInvalid;

                        pc_Node->CheckErrorCanProtocol(c_ConnectedInterfaces[u32_NodeCounter],
                                                       e_Type,
                                                       true,
                                                       q_TempCommMaxRxSignalCountInvalid,
                                                       q_TempCommMaxTxSignalCountInvalid,
                                                       q_TempCoRxPdoCountInvalid,
                                                       q_TempCoTxPdoCountInvalid, q_TempCommMinSignalCountInvalid);

                        if ((q_TempCommMaxRxSignalCountInvalid == true) ||
                            (q_TempCommMaxTxSignalCountInvalid == true) ||
                            (q_TempCoRxPdoCountInvalid == true) ||
                            (q_TempCoTxPdoCountInvalid == true) ||
                            (q_TempCommMinSignalCountInvalid == true))
                        {
                           opc_InvalidProtocols->push_back(e_Type);
                        }
                     }
                  }
               }
            }
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC Buses size

   \return
   Number of OSC buses
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_PuiSdHandlerBusLogic::GetOscBusesSize(void) const
{
   return this->mc_CoreDefinition.c_Buses.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set automatic bus routing settings

   \param[in]  ou32_BusIndex  Bus index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::SetAutomaticBusRoutingSettings(const uint32_t ou32_BusIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   std::vector<uint32_t> c_NodeIndexes;
   std::vector<uint32_t> c_InterfaceIndexes;

   this->mc_CoreDefinition.GetNodeIndexesOfBus(ou32_BusIndex, c_NodeIndexes, c_InterfaceIndexes);
   tgl_assert(c_NodeIndexes.size() == c_InterfaceIndexes.size());

   if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
   {
      for (uint32_t u32_ItNode = 0UL; (u32_ItNode < c_NodeIndexes.size()) && (s32_Retval == C_NO_ERR); ++u32_ItNode)
      {
         s32_Retval =
            this->SetAutomaticNodeInterfaceRoutingSettings(c_NodeIndexes[u32_ItNode], c_InterfaceIndexes[u32_ItNode],
                                                           false);
      }
      this->m_HandleSyncNodeRoutingSettingsChanged();
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set automatic node interface routing settings

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  oe_ComType           Com type
   \param[in]  ou8_InterfaceNumber  Interface number

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::SetAutomaticNodeInterfaceRoutingSettings(const uint32_t ou32_NodeIndex,
                                                                         const C_OscSystemBus::E_Type oe_ComType,
                                                                         const uint8_t ou8_InterfaceNumber)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscNode * const pc_Node = this->GetOscNode(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      for (uint32_t u32_ItInterface = 0UL; u32_ItInterface < pc_Node->c_Properties.c_ComInterfaces.size();
           ++u32_ItInterface)
      {
         const C_OscNodeComInterfaceSettings & rc_Interface = pc_Node->c_Properties.c_ComInterfaces[u32_ItInterface];
         if ((rc_Interface.u8_InterfaceNumber == ou8_InterfaceNumber) && (rc_Interface.e_InterfaceType == oe_ComType))
         {
            s32_Retval = this->SetAutomaticNodeInterfaceRoutingSettings(ou32_NodeIndex, u32_ItInterface);
         }
      }
      this->m_HandleSyncNodeRoutingSettingsChanged();
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set automatic node interface routing settings

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  oq_SignalChange      Signal change

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::SetAutomaticNodeInterfaceRoutingSettings(const uint32_t ou32_NodeIndex,
                                                                         const uint32_t ou32_InterfaceIndex,
                                                                         const bool oq_SignalChange)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscNode * const pc_Node = this->GetOscNode(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      if (ou32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
      {
         C_OscNodeComInterfaceSettings & rc_Interface = pc_Node->c_Properties.c_ComInterfaces[ou32_InterfaceIndex];

         if (rc_Interface.GetBusConnected())
         {
            const C_OscSystemBus * const pc_Bus = this->GetOscBus(rc_Interface.u32_BusIndex);
            if (pc_Bus != NULL)
            {
               if (!pc_Bus->q_UseableForRouting)
               {
                  rc_Interface.q_IsRoutingEnabled = false;
                  if (oq_SignalChange)
                  {
                     this->m_HandleSyncNodeRoutingSettingsChanged();
                  }
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new connection

   Add connection from node to bus.
   If node and bus exist this function will
   - add a new connection to the node structure
   - add a corresponding new UI connection to the UI node structure

   \param[in]  ou32_NodeIndex       Connected node index
   \param[in]  ou8_InterfaceNumber  Interface number of node
   \param[in]  orc_Properties       Properties
   \param[in]  ou32_BusIndex        Connected bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::AddConnection(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                                           const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties,
                                           const uint32_t ou32_BusIndex)
{
   const C_OscSystemBus * const pc_Bus = this->GetOscBus(ou32_BusIndex);
   const std::vector<uint32_t> c_Indices = this->GetAllNodeGroupIndicesUsingNodeIndex(ou32_NodeIndex);

   tgl_assert(c_Indices.size() == orc_Properties.size());
   if (c_Indices.size() == orc_Properties.size())
   {
      for (uint32_t u32_ItIndex = 0UL; u32_ItIndex < c_Indices.size(); ++u32_ItIndex)
      {
         const uint32_t u32_CurIndex = c_Indices[u32_ItIndex];
         if ((u32_CurIndex < this->mc_CoreDefinition.c_Nodes.size()) && (pc_Bus != NULL))
         {
            C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_CurIndex];
            const C_OscNodeComInterfaceSettings * const pc_CurComInterface = rc_Node.c_Properties.GetComInterface(
               pc_Bus->e_Type, ou8_InterfaceNumber);
            if (pc_CurComInterface != NULL)
            {
               if (this->mc_CoreDefinition.AddConnection(u32_CurIndex, ou32_BusIndex,
                                                         pc_CurComInterface->u8_InterfaceNumber) == C_NO_ERR)
               {
                  //Adapt node ID
                  C_OscNodeComInterfaceSettings c_CurComInterface = *pc_CurComInterface;
                  //Adapt IP address
                  c_CurComInterface.u8_NodeId = orc_Properties[u32_ItIndex].u8_NodeId;

                  tgl_assert(orc_Properties[u32_ItIndex].c_Ip.size() == 4UL);
                  for (uint32_t u32_It = 0UL; u32_It < orc_Properties[u32_ItIndex].c_Ip.size(); ++u32_It)
                  {
                     c_CurComInterface.c_Ip.au8_IpAddress[u32_It] = orc_Properties[u32_ItIndex].c_Ip[u32_It];
                  }

                  rc_Node.c_Properties.SetComInterface(c_CurComInterface);

                  C_PuiSdNodeConnection c_NewConnection;

                  //Add connection ID
                  c_NewConnection.c_ConnectionId.u8_InterfaceNumber = ou8_InterfaceNumber;
                  c_NewConnection.c_ConnectionId.e_InterfaceType = pc_Bus->e_Type;

                  if (u32_CurIndex < this->mc_UiNodes.size())
                  {
                     //Also add corresponding UI element:
                     this->mc_UiNodes[u32_CurIndex].c_UiBusConnections.push_back(c_NewConnection);
                  }
                  tgl_assert(this->SetAutomaticNodeInterfaceRoutingSettings(ou32_NodeIndex, pc_Bus->e_Type,
                                                                            ou8_InterfaceNumber) == C_NO_ERR);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove connection

   \param[in]  ou32_NodeIndex    Connector node index (0 -> first node)
   \param[in]  orc_Id            Connector ID encoding interface type and index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::RemoveConnection(const uint32_t ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_Id)
{
   const std::vector<uint32_t> c_Indices = this->GetAllNodeGroupIndicesUsingNodeIndex(ou32_NodeIndex);

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());

   for (uint32_t u32_ItIndex = 0UL; u32_ItIndex < c_Indices.size(); ++u32_ItIndex)
   {
      const uint32_t u32_CurIndex = c_Indices[u32_ItIndex];
      if (u32_CurIndex < this->mc_CoreDefinition.c_Nodes.size())
      {
         C_PuiSdNode & rc_UiNode = this->mc_UiNodes[u32_CurIndex];
         C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[u32_CurIndex];

         rc_OscNode.c_Properties.DisconnectComInterface(orc_Id.e_InterfaceType, orc_Id.u8_InterfaceNumber);
         rc_UiNode.DeleteConnection(orc_Id);
         this->m_HandleSyncNodeInterfaceDeleted();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change connection

   \param[in]  ou32_NodeIndex    Index of node this interface is part of
   \param[in]  orc_Id            Previous interface ID
   \param[in]  ou8_NewInterface  New interface number
   \param[in]  orc_Properties    Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::ChangeConnection(const uint32_t ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_Id,
                                              const uint8_t ou8_NewInterface,
                                              const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
{
   //Copy is necessary as the value changes during function call
   const C_PuiSdNodeConnectionId c_OrgCopy = orc_Id;
   C_PuiSdNodeConnectionId c_Tmp;

   c_Tmp.e_InterfaceType = orc_Id.e_InterfaceType;
   c_Tmp.u8_InterfaceNumber = ou8_NewInterface;

   ChangeCompleteConnection(ou32_NodeIndex, c_OrgCopy, c_Tmp, orc_Properties, 0xFFFFFFFFUL, false);
   m_HandleChangeConnectionForCanOpen(ou32_NodeIndex, c_OrgCopy, ou8_NewInterface);
   tgl_assert(this->SetAutomaticNodeInterfaceRoutingSettings(ou32_NodeIndex, orc_Id.e_InterfaceType,
                                                             ou8_NewInterface) == C_NO_ERR);
   this->m_HandleSyncNodeInterfaceChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change complete connection

   \param[in]  ou32_NodeIndex          Index of node this interface is part of
   \param[in]  orc_PrevId              Previous interface ID
   \param[in]  orc_NewId               New interface ID
   \param[in]  orc_Properties          Properties
   \param[in]  oru32_BusIndex          Bus index to use instead of last used one
   \param[in]  oq_IncludeCanOpenSync   Flag to include CANopen data snyc handling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::ChangeCompleteConnection(const uint32_t ou32_NodeIndex,
                                                      const C_PuiSdNodeConnectionId & orc_PrevId,
                                                      const C_PuiSdNodeConnectionId & orc_NewId,
                                                      const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties, const uint32_t & oru32_BusIndex,
                                                      const bool oq_IncludeCanOpenSync)
{
   //Copy is necessary as the value changes during function call
   const C_PuiSdNodeConnectionId c_PrevIdCopy = orc_PrevId;
   const std::vector<uint32_t> c_Indices = this->GetAllNodeGroupIndicesUsingNodeIndex(ou32_NodeIndex);

   //Check size & consistency
   tgl_assert(c_Indices.size() == orc_Properties.size());
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());

   for (uint32_t u32_ItIndex = 0UL; u32_ItIndex < c_Indices.size(); ++u32_ItIndex)
   {
      const uint32_t u32_CurIndex = c_Indices[u32_ItIndex];
      if (u32_CurIndex < this->mc_CoreDefinition.c_Nodes.size())
      {
         C_PuiSdNode & rc_UiNode = this->mc_UiNodes[u32_CurIndex];
         C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[u32_CurIndex];
         uint32_t u32_LastBus = 0;
         //Unregister
         for (uint32_t u32_ItComInterface = 0; u32_ItComInterface < rc_OscNode.c_Properties.c_ComInterfaces.size();
              ++u32_ItComInterface)
         {
            C_OscNodeComInterfaceSettings & rc_ComInterface =
               rc_OscNode.c_Properties.c_ComInterfaces[u32_ItComInterface];
            if ((rc_ComInterface.e_InterfaceType == c_PrevIdCopy.e_InterfaceType) &&
                (rc_ComInterface.u8_InterfaceNumber == c_PrevIdCopy.u8_InterfaceNumber))
            {
               u32_LastBus = rc_ComInterface.u32_BusIndex;
               rc_ComInterface.RemoveConnection();
            }
         }
         //Register
         for (uint32_t u32_ItComInterface = 0; u32_ItComInterface < rc_OscNode.c_Properties.c_ComInterfaces.size();
              ++u32_ItComInterface)
         {
            C_OscNodeComInterfaceSettings & rc_ComInterface =
               rc_OscNode.c_Properties.c_ComInterfaces[u32_ItComInterface];
            if ((rc_ComInterface.e_InterfaceType == orc_NewId.e_InterfaceType) &&
                (rc_ComInterface.u8_InterfaceNumber == orc_NewId.u8_InterfaceNumber))
            {
               uint32_t u32_BusIndex;
               if (oru32_BusIndex >= this->mc_CoreDefinition.c_Buses.size())
               {
                  u32_BusIndex = u32_LastBus;
               }
               else
               {
                  u32_BusIndex = oru32_BusIndex;
               }
               rc_ComInterface.AddConnection(u32_BusIndex);
               rc_ComInterface.u8_NodeId = orc_Properties[u32_ItIndex].u8_NodeId;

               tgl_assert(orc_Properties[u32_ItIndex].c_Ip.size() == 4UL);
               for (uint32_t u32_It = 0UL; u32_It < orc_Properties[u32_ItIndex].c_Ip.size(); ++u32_It)
               {
                  rc_ComInterface.c_Ip.au8_IpAddress[u32_It] = orc_Properties[u32_ItIndex].c_Ip[u32_It];
               }
            }
         }
         tgl_assert(this->SetAutomaticNodeInterfaceRoutingSettings(u32_CurIndex, orc_NewId.e_InterfaceType,
                                                                   orc_NewId.u8_InterfaceNumber) == C_NO_ERR);

         //UI
         for (uint32_t u32_ItBusConn = 0; u32_ItBusConn < rc_UiNode.c_UiBusConnections.size(); ++u32_ItBusConn)
         {
            C_PuiSdNodeConnection & rc_BusConn = rc_UiNode.c_UiBusConnections[u32_ItBusConn];
            if (rc_BusConn.c_ConnectionId == c_PrevIdCopy)
            {
               rc_BusConn.c_ConnectionId = orc_NewId;
            }
         }
         if (oq_IncludeCanOpenSync)
         {
            this->m_HandleChangeCompleteConnectionForCanOpen(u32_CurIndex, c_PrevIdCopy, orc_NewId);
            this->m_HandleSyncNodeInterfaceChanged();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set UI node connections

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_Connections   Connection information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::SetUiNodeConnections(const uint32_t ou32_NodeIndex,
                                                  const std::vector<C_PuiSdNodeConnection> & orc_Connections)
{
   const std::vector<uint32_t> c_Indices = this->GetAllNodeGroupIndicesUsingNodeIndex(ou32_NodeIndex);

   for (uint32_t u32_ItIndex = 0UL; u32_ItIndex < c_Indices.size(); ++u32_ItIndex)
   {
      const uint32_t u32_CurIndex = c_Indices[u32_ItIndex];
      if (u32_CurIndex < this->mc_UiNodes.size())
      {
         C_PuiSdNode & rc_UiNode = this->mc_UiNodes[u32_CurIndex];
         rc_UiNode.c_UiBusConnections = orc_Connections;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set UI node connection ID

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ConnectionIndex    Connection index
   \param[in]  orc_Id                  New ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::SetUiNodeConnectionId(const uint32_t ou32_NodeIndex, const uint32_t ou32_ConnectionIndex,
                                                   const C_PuiSdNodeConnectionId & orc_Id)
{
   const std::vector<uint32_t> c_Indices = this->GetAllNodeGroupIndicesUsingNodeIndex(ou32_NodeIndex);

   for (uint32_t u32_ItIndex = 0UL; u32_ItIndex < c_Indices.size(); ++u32_ItIndex)
   {
      const uint32_t u32_CurIndex = c_Indices[u32_ItIndex];
      if (u32_CurIndex < this->mc_UiNodes.size())
      {
         C_PuiSdNode & rc_UiNode = this->mc_UiNodes[u32_CurIndex];
         if (ou32_ConnectionIndex < rc_UiNode.c_UiBusConnections.size())
         {
            rc_UiNode.c_UiBusConnections[ou32_ConnectionIndex].c_ConnectionId = orc_Id;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get com protocol data

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  ore_ComType          Can protocol type
   \param[in]  ou32_DatapoolIndex   Datapool index

   \return
   NULL No matching data found
   Else Pointer to data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanProtocol * C_PuiSdHandlerBusLogic::GetCanProtocol(const uint32_t & oru32_NodeIndex,
                                                                const C_OscCanProtocol::E_Type & ore_ComType,
                                                                const uint32_t ou32_DatapoolIndex) const
{
   const C_OscCanProtocol * pc_Retval = NULL;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      pc_Retval = rc_Node.GetCanProtocolConst(ore_ComType, ou32_DatapoolIndex);
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get com data pool (common)

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  ore_ComType          Can protocol type
   \param[in]  oru32_DatapoolIndex  Datapool index

   \return
   NULL No matching data found
   Else Pointer to data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNodeDataPool * C_PuiSdHandlerBusLogic::GetOscCanDataPool(const uint32_t & oru32_NodeIndex,
                                                                    const C_OscCanProtocol::E_Type & ore_ComType,
                                                                    const uint32_t & oru32_DatapoolIndex) const
{
   const C_OscNodeDataPool * pc_Retval = NULL;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      pc_Retval = rc_Node.GetComDataPoolConst(ore_ComType, oru32_DatapoolIndex);
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all com data pools of specific protocol type (common)

   \param[in]  oru32_NodeIndex   Node index
   \param[in]  ore_ComType       Can protocol type

   \return
   Empty vector            No matching data found for communication protocol
   Pointer to protocols    Pointers to communication datapools of node
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector< const C_OscNodeDataPool *> C_PuiSdHandlerBusLogic::GetOscCanDataPools(const uint32_t & oru32_NodeIndex,
                                                                                   const C_OscCanProtocol::E_Type & ore_ComType)
const
{
   std::vector<const C_OscNodeDataPool *> c_Return;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      c_Return = rc_Node.GetComDatapoolsConst(ore_ComType);
   }
   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get com data pool (ui)

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  ore_ComType          Can protocol type
   \param[in]  oru32_DatapoolIndex  Datapool index

   \return
   NULL No matching data found
   Else Pointer to data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNodeDataPool * C_PuiSdHandlerBusLogic::GetUiCanDataPool(const uint32_t & oru32_NodeIndex,
                                                                     const C_OscCanProtocol::E_Type & ore_ComType,
                                                                     const uint32_t & oru32_DatapoolIndex) const
{
   const C_PuiSdNodeDataPool * pc_Retval = NULL;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UiNodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      const C_OscCanProtocol * const pc_CanProtocol = rc_OscNode.GetCanProtocolConst(ore_ComType, oru32_DatapoolIndex);
      if (pc_CanProtocol != NULL)
      {
         const C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
         if (pc_CanProtocol->u32_DataPoolIndex < rc_UiNode.c_UiDataPools.size())
         {
            pc_Retval = &rc_UiNode.c_UiDataPools[pc_CanProtocol->u32_DataPoolIndex];
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get com data pool list (ui)

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  oe_ComType           Com type
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  ou32_DatapoolIndex   Datapool index
   \param[in]  oq_MessageIsTx       Message is tx

   \return
   Ui com data pool list
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNodeDataPoolList * C_PuiSdHandlerBusLogic::GetUiCanDataPoolList(const uint32_t ou32_NodeIndex,
                                                                             const C_OscCanProtocol::E_Type oe_ComType,
                                                                             const uint32_t ou32_InterfaceIndex,
                                                                             const uint32_t ou32_DatapoolIndex,
                                                                             const bool oq_MessageIsTx) const
{
   const C_PuiSdNodeDataPoolList * pc_Retval = NULL;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UiNodes.size());
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const C_PuiSdNode & rc_UiNode = this->mc_UiNodes[ou32_NodeIndex];
      const C_OscCanProtocol * const pc_CanProtocol = rc_OscNode.GetCanProtocolConst(oe_ComType, ou32_DatapoolIndex);
      if ((pc_CanProtocol != NULL) && (ou32_DatapoolIndex < rc_OscNode.c_DataPools.size()) &&
          (pc_CanProtocol->u32_DataPoolIndex < rc_UiNode.c_UiDataPools.size()))
      {
         const C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[ou32_DatapoolIndex];
         const C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[pc_CanProtocol->u32_DataPoolIndex];
         uint32_t u32_ListIndex = 0UL;
         if (C_OscCanProtocol::h_GetComListIndex(rc_OscDataPool, ou32_InterfaceIndex, oq_MessageIsTx,
                                                 u32_ListIndex) == C_NO_ERR)
         {
            if (u32_ListIndex < rc_UiDataPool.c_DataPoolLists.size())
            {
               pc_Retval = &rc_UiDataPool.c_DataPoolLists[u32_ListIndex];
            }
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get com protocol (ui)

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  ore_ComType          Can protocol type
   \param[in]  ou32_DatapoolIndex   Datapool index

   \return
   NULL No matching data found
   Else Pointer to data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNodeCanProtocol * C_PuiSdHandlerBusLogic::GetUiCanProtocolConst(const uint32_t & oru32_NodeIndex,
                                                                             const C_OscCanProtocol::E_Type & ore_ComType,
                                                                             const uint32_t ou32_DatapoolIndex)
const
{
   const C_PuiSdNodeCanProtocol * pc_Retval = NULL;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UiNodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      const C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      tgl_assert(rc_OscNode.c_ComProtocols.size() == rc_UiNode.c_UiCanProtocols.size());
      for (uint32_t u32_ItCanProtocol = 0; u32_ItCanProtocol < rc_OscNode.c_ComProtocols.size(); ++u32_ItCanProtocol)
      {
         const C_OscCanProtocol & rc_CanProtocol = rc_OscNode.c_ComProtocols[u32_ItCanProtocol];
         if ((rc_CanProtocol.u32_DataPoolIndex == ou32_DatapoolIndex) &&
             (rc_CanProtocol.e_Type == ore_ComType))
         {
            pc_Retval = &rc_UiNode.c_UiCanProtocols[u32_ItCanProtocol];
            break;
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get message container of one protocol type, interface and Datapool

   \param[in]  oru32_NodeIndex         Node index
   \param[in]  ore_ComType             Can protocol type
   \param[in]  oru32_InterfaceIndex    Node interface index
   \param[in]  ou32_DatapoolIndex      Datapool index

   \return
   NULL No matching data found
   Else Pointer to data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanMessageContainer * C_PuiSdHandlerBusLogic::GetCanProtocolMessageContainer(
   const uint32_t & oru32_NodeIndex, const C_OscCanProtocol::E_Type & ore_ComType,
   const uint32_t & oru32_InterfaceIndex, const uint32_t ou32_DatapoolIndex)
const
{
   const C_OscCanMessageContainer * pc_Retval = NULL;
   const C_OscCanProtocol * const pc_Protocol = this->GetCanProtocol(oru32_NodeIndex, ore_ComType, ou32_DatapoolIndex);

   if (pc_Protocol != NULL)
   {
      if (oru32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
      {
         pc_Retval = &pc_Protocol->c_ComMessages[oru32_InterfaceIndex];
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all message containers of one protocol type and interface

   \param[in]  oru32_NodeIndex         Node index
   \param[in]  ore_ComType             Can protocol type
   \param[in]  oru32_InterfaceIndex    Node interface index

   \return
   Empty vector            No matching data found for message container
   Pointer to protocols    Pointers to message containers
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<const C_OscCanMessageContainer *> C_PuiSdHandlerBusLogic::GetCanProtocolMessageContainers(
   const uint32_t & oru32_NodeIndex, const C_OscCanProtocol::E_Type & ore_ComType,
   const uint32_t & oru32_InterfaceIndex) const
{
   std::vector<const C_OscCanMessageContainer *> c_Retval;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      std::vector<const C_OscCanProtocol *> c_Protocols = rc_Node.GetCanProtocolsConst(ore_ComType);
      uint32_t u32_Counter;

      for (u32_Counter = 0U; u32_Counter < c_Protocols.size(); ++u32_Counter)
      {
         if (oru32_InterfaceIndex < c_Protocols[u32_Counter]->c_ComMessages.size())
         {
            c_Retval.push_back(&(c_Protocols[u32_Counter]->c_ComMessages[oru32_InterfaceIndex]));
         }
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get protocol type of specific Datapool

   \param[in]   ou32_NodeIndex      Node index
   \param[in]   ou32_DatapoolIndex  Datapool index
   \param[out]  ore_ComType         Detected CAN protocol type

   \retval C_NO_ERR     Com type read
   \retval C_RANGE      ou32_NodeIndex or ou32_DatapoolIndex invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::GetCanProtocolType(const uint32_t ou32_NodeIndex, const uint32_t ou32_DatapoolIndex,
                                                   C_OscCanProtocol::E_Type & ore_ComType) const
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const C_OscCanProtocol * const pc_Prot = rc_Node.GetRelatedCanProtocolConst(ou32_DatapoolIndex);

      if (pc_Prot != NULL)
      {
         ore_ComType = pc_Prot->e_Type;
         s32_Return = C_NO_ERR;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get message data

   \param[in]  orc_MessageId  Message identification indices

   \return
   NULL No matching data found
   Else Pointer to data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanMessage * C_PuiSdHandlerBusLogic::GetCanMessage(const C_OscCanMessageIdentificationIndices & orc_MessageId)
const
{
   const C_OscCanMessage * pc_Retval = NULL;
   const C_OscCanMessageContainer * const pc_MessageContainer = this->GetCanProtocolMessageContainer(
      orc_MessageId.u32_NodeIndex,
      orc_MessageId.e_ComProtocol,
      orc_MessageId.u32_InterfaceIndex,
      orc_MessageId.u32_DatapoolIndex);

   if (pc_MessageContainer != NULL)
   {
      const std::vector<C_OscCanMessage> & rc_Messages = pc_MessageContainer->GetMessagesConst(
         orc_MessageId.q_MessageIsTx);
      if (orc_MessageId.u32_MessageIndex < rc_Messages.size())
      {
         pc_Retval = &rc_Messages[orc_MessageId.u32_MessageIndex];
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy can message with all related data structures

   \param[in]   orc_MessageId                   Message identification indices
   \param[out]  orc_Message                     Message data
   \param[out]  orc_OscSignalCommons            Signals data (osc common)
   \param[out]  orc_UiSignalCommons             Signals data (ui common)
   \param[out]  orc_UiMessage                   Message ui data
   \param[in]   oq_ChangeSignalIndicesToOutput  Change signal indices to output

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::GetCanMessageComplete(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                      C_OscCanMessage & orc_Message,
                                                      std::vector<C_OscNodeDataPoolListElement> & orc_OscSignalCommons,
                                                      std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons,
                                                      C_PuiSdNodeCanMessage & orc_UiMessage,
                                                      const bool oq_ChangeSignalIndicesToOutput) const
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);
   const C_OscNodeDataPoolList * const pc_OscList = this->GetOscCanDataPoolList(orc_MessageId.u32_NodeIndex,
                                                                                orc_MessageId.e_ComProtocol,
                                                                                orc_MessageId.u32_InterfaceIndex,
                                                                                orc_MessageId.u32_DatapoolIndex,
                                                                                orc_MessageId.q_MessageIsTx);
   const C_PuiSdNodeDataPoolList * const pc_UiList = this->m_GetUiCanDataPoolList(orc_MessageId.u32_NodeIndex,
                                                                                  orc_MessageId.e_ComProtocol,
                                                                                  orc_MessageId.u32_InterfaceIndex,
                                                                                  orc_MessageId.u32_DatapoolIndex,
                                                                                  orc_MessageId.q_MessageIsTx);
   const C_PuiSdNodeCanMessage * const pc_UiMessage = GetUiCanMessage(orc_MessageId);

   if ((((pc_Message != NULL) && (pc_OscList != NULL)) && (pc_UiList != NULL)) && (pc_UiMessage != NULL))
   {
      orc_Message = *pc_Message;
      orc_UiMessage = *pc_UiMessage;
      //Copy RELEVANT signals
      orc_OscSignalCommons.clear();
      orc_UiSignalCommons.clear();
      orc_OscSignalCommons.reserve(pc_Message->c_Signals.size());
      orc_UiSignalCommons.reserve(pc_Message->c_Signals.size());
      for (uint32_t u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
      {
         C_OscCanSignal & rc_Signal = orc_Message.c_Signals[u32_ItSignal];
         if (rc_Signal.u32_ComDataElementIndex < pc_OscList->c_Elements.size())
         {
            orc_OscSignalCommons.push_back(pc_OscList->c_Elements[rc_Signal.u32_ComDataElementIndex]);
         }
         if (rc_Signal.u32_ComDataElementIndex < pc_UiList->c_DataPoolListElements.size())
         {
            orc_UiSignalCommons.push_back(pc_UiList->c_DataPoolListElements[rc_Signal.u32_ComDataElementIndex]);
         }
         if (oq_ChangeSignalIndicesToOutput)
         {
            rc_Signal.u32_ComDataElementIndex = u32_ItSignal;
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get signal list (common)

   \param[in]  oru32_NodeIndex         Node index
   \param[in]  ore_ComType             Can protocol type
   \param[in]  oru32_InterfaceIndex    Node interface index
   \param[in]  ou32_DatapoolIndex      Datapool index
   \param[in]  orq_MessageIsTx         Flag if message is tx type (else rx type assumed)

   \return
   NULL No matching data found
   Else Pointer to data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNodeDataPoolList * C_PuiSdHandlerBusLogic::GetOscCanDataPoolList(const uint32_t & oru32_NodeIndex,
                                                                            const C_OscCanProtocol::E_Type & ore_ComType, const uint32_t & oru32_InterfaceIndex, const uint32_t ou32_DatapoolIndex,
                                                                            const bool & orq_MessageIsTx) const
{
   const C_OscNodeDataPoolList * pc_Retval = NULL;
   const C_OscNodeDataPool * const pc_DataPool = this->GetOscCanDataPool(oru32_NodeIndex, ore_ComType,
                                                                         ou32_DatapoolIndex);

   if (pc_DataPool != NULL)
   {
      pc_Retval = C_OscCanProtocol::h_GetComListConst(*pc_DataPool, oru32_InterfaceIndex, orq_MessageIsTx);
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get can signal data (position)

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index

   \return
   NULL No matching data found
   Else Pointer to data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanSignal * C_PuiSdHandlerBusLogic::GetCanSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                            const uint32_t & oru32_SignalIndex) const
{
   const C_OscCanSignal * pc_Retval = NULL;
   const C_OscCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);

   if (pc_Message != NULL)
   {
      if (oru32_SignalIndex < pc_Message->c_Signals.size())
      {
         pc_Retval = &pc_Message->c_Signals[oru32_SignalIndex];
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get can message data (ui)

   \param[in]  orc_MessageId  Message identification indices

   \return
   NULL No matching data found
   Else Pointer to data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNodeCanMessage * C_PuiSdHandlerBusLogic::GetUiCanMessage(
   const C_OscCanMessageIdentificationIndices & orc_MessageId)
const
{
   const C_PuiSdNodeCanMessage * pc_Retval = NULL;
   const C_PuiSdNodeCanProtocol * const pc_CanProtocol = this->GetUiCanProtocolConst(orc_MessageId.u32_NodeIndex,
                                                                                     orc_MessageId.e_ComProtocol,
                                                                                     orc_MessageId.u32_DatapoolIndex);

   if (pc_CanProtocol != NULL)
   {
      if (orc_MessageId.u32_InterfaceIndex < pc_CanProtocol->c_ComMessages.size())
      {
         const C_PuiSdNodeCanMessageContainer & rc_MessageContainer =
            pc_CanProtocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
         const std::vector<C_PuiSdNodeCanMessage> & rc_Messages = rc_MessageContainer.GetMessagesConst(
            orc_MessageId.q_MessageIsTx);
         if (orc_MessageId.u32_MessageIndex < rc_Messages.size())
         {
            pc_Retval = &rc_Messages[orc_MessageId.u32_MessageIndex];
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get can signal data (ui)

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index

   \return
   NULL No matching data found
   Else Pointer to data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNodeCanSignal * C_PuiSdHandlerBusLogic::GetUiCanSignal(
   const C_OscCanMessageIdentificationIndices & orc_MessageId, const uint32_t & oru32_SignalIndex) const
{
   const C_PuiSdNodeCanSignal * pc_Retval = NULL;
   const C_PuiSdNodeCanMessage * const pc_Message = GetUiCanMessage(orc_MessageId);

   if (pc_Message != NULL)
   {
      if (oru32_SignalIndex < pc_Message->c_Signals.size())
      {
         pc_Retval = &pc_Message->c_Signals[oru32_SignalIndex];
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get can signal data (osc common)

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index

   \return
   NULL No matching data found
   Else Pointer to data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNodeDataPoolListElement * C_PuiSdHandlerBusLogic::GetOscCanDataPoolListElement(
   const C_OscCanMessageIdentificationIndices & orc_MessageId, const uint32_t & oru32_SignalIndex)
const
{
   const C_OscNodeDataPoolListElement * pc_Retval = NULL;
   const C_OscCanSignal * const pc_Signal = this->GetCanSignal(orc_MessageId, oru32_SignalIndex);

   if (pc_Signal != NULL)
   {
      const C_OscNodeDataPoolList * const pc_List = this->GetOscCanDataPoolList(orc_MessageId.u32_NodeIndex,
                                                                                orc_MessageId.e_ComProtocol,
                                                                                orc_MessageId.u32_InterfaceIndex,
                                                                                orc_MessageId.u32_DatapoolIndex,
                                                                                orc_MessageId.q_MessageIsTx);

      if (pc_List != NULL)
      {
         if (pc_Signal->u32_ComDataElementIndex < pc_List->c_Elements.size())
         {
            pc_Retval = &pc_List->c_Elements[pc_Signal->u32_ComDataElementIndex];
         }
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get can signal data (ui common)

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index

   \return
   NULL No matching data found
   Else Pointer to data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNodeDataPoolListElement * C_PuiSdHandlerBusLogic::GetUiCanDataPoolListElement(
   const C_OscCanMessageIdentificationIndices & orc_MessageId, const uint32_t & oru32_SignalIndex)
const
{
   const C_PuiSdNodeDataPoolListElement * pc_Retval = NULL;
   const C_OscCanSignal * const pc_Signal = this->GetCanSignal(orc_MessageId, oru32_SignalIndex);

   if (pc_Signal != NULL)
   {
      const C_PuiSdNodeDataPoolList * const pc_List = this->m_GetUiCanDataPoolList(orc_MessageId.u32_NodeIndex,
                                                                                   orc_MessageId.e_ComProtocol,
                                                                                   orc_MessageId.u32_InterfaceIndex,
                                                                                   orc_MessageId.u32_DatapoolIndex,
                                                                                   orc_MessageId.q_MessageIsTx);

      if (pc_List != NULL)
      {
         if (pc_Signal->u32_ComDataElementIndex < pc_List->c_DataPoolListElements.size())
         {
            pc_Retval = &pc_List->c_DataPoolListElements[pc_Signal->u32_ComDataElementIndex];
         }
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get connection data

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  oe_Type              Interface type

   \return
   NULL Connection not found
   Else Valid connection
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNodeConnection * C_PuiSdHandlerBusLogic::GetUiConnection(const uint32_t ou32_NodeIndex,
                                                                      const uint32_t ou32_InterfaceIndex,
                                                                      const C_OscSystemBus::E_Type oe_Type) const
{
   const C_PuiSdNodeConnection * pc_Retval = NULL;
   const C_PuiSdNode * const pc_Node = this->GetUiNode(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      for (uint32_t u32_ItConnection = 0; u32_ItConnection < pc_Node->c_UiBusConnections.size(); ++u32_ItConnection)
      {
         const C_PuiSdNodeConnection & rc_Connection = pc_Node->c_UiBusConnections[u32_ItConnection];
         if ((static_cast<uint32_t>(rc_Connection.c_ConnectionId.u8_InterfaceNumber) == ou32_InterfaceIndex) &&
             (rc_Connection.c_ConnectionId.e_InterfaceType == oe_Type))
         {
            pc_Retval = &rc_Connection;
            break;
         }
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy can signal with all related data structures

   \param[in]   orc_MessageId          Message identification indices
   \param[in]   oru32_SignalIndex      Signal index
   \param[out]  orc_Signal             Signal data (osc)
   \param[out]  orc_OscSignalCommon    Signal data (osc common)
   \param[out]  orc_UiSignalCommon     Signal data (ui common)
   \param[out]  orc_UiSignal           Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::GetCanSignalComplete(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                     const uint32_t & oru32_SignalIndex, C_OscCanSignal & orc_Signal,
                                                     C_OscNodeDataPoolListElement & orc_OscSignalCommon,
                                                     C_PuiSdNodeDataPoolListElement & orc_UiSignalCommon,
                                                     C_PuiSdNodeCanSignal & orc_UiSignal) const
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscCanSignal * const pc_Signal = this->GetCanSignal(orc_MessageId, oru32_SignalIndex);
   const C_OscNodeDataPoolListElement * const pc_OscSignalCommon = this->GetOscCanDataPoolListElement(orc_MessageId,
                                                                                                      oru32_SignalIndex);
   const C_PuiSdNodeDataPoolListElement * const pc_UiSignalCommon = this->GetUiCanDataPoolListElement(orc_MessageId,
                                                                                                      oru32_SignalIndex);
   const C_PuiSdNodeCanSignal * const pc_UiSignal = GetUiCanSignal(orc_MessageId, oru32_SignalIndex);

   if ((((pc_Signal != NULL) && (pc_OscSignalCommon != NULL)) && (pc_UiSignalCommon != NULL)) && (pc_UiSignal != NULL))
   {
      orc_Signal = *pc_Signal;
      orc_OscSignalCommon = *pc_OscSignalCommon;
      orc_UiSignalCommon = *pc_UiSignalCommon;
      orc_UiSignal = *pc_UiSignal;
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set CAN protocol message container connected flag

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  oe_ComType           Can protocol type
   \param[in]  ou32_InterfaceIndex  Node interface index
   \param[in]  oq_Value             New value to apply
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::SetCanProtocolMessageContainerConnected(const uint32_t ou32_NodeIndex,
                                                                     const C_OscCanProtocol::E_Type oe_ComType,
                                                                     const uint32_t ou32_InterfaceIndex,
                                                                     const bool oq_Value)
{
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      // get necessary datapool
      std::vector<C_OscCanProtocol *> c_Protocols = rc_Node.GetCanProtocols(oe_ComType);
      uint32_t u32_Counter;

      for (u32_Counter = 0; u32_Counter < c_Protocols.size(); ++u32_Counter)
      {
         C_OscCanProtocol * const pc_Protocol = c_Protocols[u32_Counter];

         if (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
         {
            // set the flag
            pc_Protocol->c_ComMessages[ou32_InterfaceIndex].q_IsComProtocolUsedByInterface = oq_Value;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set can message

   Do not change any signal using this function.

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  orq_NewMessageIsTx   Flag if new message is Tx
   \param[in]  orc_Message          Message data

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::SetCanMessageProperties(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                        const bool & orq_NewMessageIsTx,
                                                        const C_OscCanMessage & orc_Message)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscCanMessageContainer * const pc_MessageContainer = this->GetCanProtocolMessageContainer(
         orc_MessageId.u32_NodeIndex,
         orc_MessageId.e_ComProtocol,
         orc_MessageId.u32_InterfaceIndex,
         orc_MessageId.u32_DatapoolIndex);
      const C_OscCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);
      const C_OscNodeDataPoolList * const pc_List = this->GetOscCanDataPoolList(orc_MessageId.u32_NodeIndex,
                                                                                orc_MessageId.e_ComProtocol,
                                                                                orc_MessageId.u32_InterfaceIndex,
                                                                                orc_MessageId.u32_DatapoolIndex,
                                                                                orc_MessageId.q_MessageIsTx);
      if (((pc_List != NULL) && (pc_MessageContainer != NULL)) && (pc_Message != NULL))
      {
         uint32_t u32_SignalStartIndex = pc_MessageContainer->GetMessageSignalDataStartIndex(
            orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex);
         if ((u32_SignalStartIndex + pc_Message->c_Signals.size()) <= pc_List->c_Elements.size())
         {
            std::vector<C_OscNodeDataPoolListElement> c_Signals;
            C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
            //Copy signals
            c_Signals.reserve(pc_Message->c_Signals.size());
            for (uint32_t u32_ItSignals = 0; u32_ItSignals < pc_Message->c_Signals.size(); ++u32_ItSignals)
            {
               const uint32_t u32_Index = u32_SignalStartIndex + u32_ItSignals;
               c_Signals.push_back(pc_List->c_Elements[u32_Index]);
            }
            s32_Retval = rc_Node.SetMessage(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                                            orc_MessageId.u32_DatapoolIndex,
                                            orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex,
                                            orc_Message, orq_NewMessageIsTx, c_Signals);
            if ((orc_MessageId.q_MessageIsTx != orq_NewMessageIsTx) && (s32_Retval == C_NO_ERR))
            {
               //Sync ui data
               const C_PuiSdNodeCanMessage * const pc_UiMessage = this->GetUiCanMessage(orc_MessageId);
               const C_OscCanMessageContainer * const pc_UpdatedMessageContainer = this->GetCanProtocolMessageContainer(
                  orc_MessageId.u32_NodeIndex,
                  orc_MessageId.e_ComProtocol,
                  orc_MessageId.u32_InterfaceIndex,
                  orc_MessageId.u32_DatapoolIndex);
               const C_PuiSdNodeDataPoolList * const pc_UiDataPoolList = this->m_GetUiCanDataPoolList(
                  orc_MessageId.u32_NodeIndex, orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                  orc_MessageId.u32_DatapoolIndex, orc_MessageId.q_MessageIsTx);
               if (((pc_UiMessage != NULL) && (pc_UpdatedMessageContainer != NULL)) && (pc_UiDataPoolList != NULL))
               {
                  const std::vector<C_OscCanMessage> & rc_Messages = pc_UpdatedMessageContainer->GetMessagesConst(
                     orq_NewMessageIsTx);
                  const C_PuiSdNodeCanMessage c_UiMessageCopy = *pc_UiMessage;
                  std::vector<C_PuiSdNodeDataPoolListElement> c_UiSignalCommons;
                  const C_OscCanMessageIdentificationIndices c_NewId(orc_MessageId.u32_NodeIndex,
                                                                     orc_MessageId.e_ComProtocol,
                                                                     orc_MessageId.u32_InterfaceIndex,
                                                                     orc_MessageId.u32_DatapoolIndex,
                                                                     orq_NewMessageIsTx,
                                                                     rc_Messages.size() - 1UL);

                  u32_SignalStartIndex = pc_UpdatedMessageContainer->GetMessageSignalDataStartIndex(
                     orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex);

                  for (uint32_t u32_ItSignal = 0; u32_ItSignal < c_UiMessageCopy.c_Signals.size(); ++u32_ItSignal)
                  {
                     const uint32_t u32_TargetIndex = u32_SignalStartIndex + u32_ItSignal;
                     tgl_assert(u32_TargetIndex < pc_UiDataPoolList->c_DataPoolListElements.size());
                     if (u32_TargetIndex < pc_UiDataPoolList->c_DataPoolListElements.size())
                     {
                        c_UiSignalCommons.push_back(pc_UiDataPoolList->c_DataPoolListElements[u32_TargetIndex]);
                     }
                  }
                  //Delete
                  s32_Retval = this->m_DeleteUiCanMessage(orc_MessageId);
                  if (s32_Retval == C_NO_ERR)
                  {
                     //Add
                     s32_Retval = this->m_InsertUiCanMessage(c_NewId, c_UiSignalCommons, c_UiMessageCopy);
                  }
               }
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set UI part of can message

   Do not change any signals using this function.

   \param[in]  orc_MessageId  Message identification indices
   \param[in]  orc_Message    Message data

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::SetUiCanMessage(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                const C_PuiSdNodeCanMessage & orc_Message)
{
   int32_t s32_Retval = C_NO_ERR;

   C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                           orc_MessageId.e_ComProtocol,
                                                                           orc_MessageId.u32_DatapoolIndex);

   //Ui message part
   if (pc_UiProtocol != NULL)
   {
      if (orc_MessageId.u32_InterfaceIndex < pc_UiProtocol->c_ComMessages.size())
      {
         C_PuiSdNodeCanMessageContainer & rc_MessageContainer =
            pc_UiProtocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
         std::vector<C_PuiSdNodeCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(
            orc_MessageId.q_MessageIsTx);
         if (orc_MessageId.u32_MessageIndex < rc_Messages.size())
         {
            rc_Messages[orc_MessageId.u32_MessageIndex] = orc_Message;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set can signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
   \param[in]  orc_OscSignal        Signal data (osc)
   \param[in]  orc_OscSignalCommon  Signal data (osc common)
   \param[in]  orc_UiSignalCommon   Signal data (ui common)
   \param[in]  orc_UiSignal         Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::SetCanSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                             const uint32_t & oru32_SignalIndex, const C_OscCanSignal & orc_OscSignal,
                                             const C_OscNodeDataPoolListElement & orc_OscSignalCommon,
                                             const C_PuiSdNodeDataPoolListElement & orc_UiSignalCommon,
                                             const C_PuiSdNodeCanSignal & orc_UiSignal)
{
   int32_t s32_Retval = C_NO_ERR;

   //Position
   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UiNodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      const C_OscCanProtocol * const pc_Protocol = rc_OscNode.GetCanProtocolConst(orc_MessageId.e_ComProtocol,
                                                                                  orc_MessageId.u32_DatapoolIndex);
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol,
                                                                              orc_MessageId.u32_DatapoolIndex);
      C_OscNodeDataPoolListElementId c_Id;
      s32_Retval = rc_OscNode.SetSignal(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                                        orc_MessageId.u32_DatapoolIndex,
                                        orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex,
                                        oru32_SignalIndex, orc_OscSignal, orc_OscSignalCommon);
      //Signal changed signal to the sync engine
      if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, oru32_SignalIndex, c_Id) == C_NO_ERR)
      {
         this->m_HandleSyncNodeDataPoolListElementTypeOrArrayChanged(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                                     c_Id.u32_ListIndex, c_Id.u32_ElementIndex,
                                                                     orc_OscSignalCommon.GetType(),
                                                                     orc_OscSignalCommon.GetArray(),
                                                                     orc_OscSignalCommon.GetArraySize(), false);
         this->m_HandleSyncNodeDataPoolListElementRangeChanged(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                               c_Id.u32_ListIndex, c_Id.u32_ElementIndex,
                                                               orc_OscSignalCommon.c_MinValue,
                                                               orc_OscSignalCommon.c_MaxValue);
      }
      //Ui data pool part
      if (pc_Protocol != NULL)
      {
         if (orc_MessageId.u32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
         {
            const C_OscCanMessageContainer & rc_MessageContainer =
               pc_Protocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
            const std::vector<C_OscCanMessage> & rc_Messages = rc_MessageContainer.GetMessagesConst(
               orc_MessageId.q_MessageIsTx);
            if (orc_MessageId.u32_MessageIndex < rc_Messages.size())
            {
               const C_OscCanMessage & rc_Message = rc_Messages[orc_MessageId.u32_MessageIndex];
               if (oru32_SignalIndex < rc_Message.c_Signals.size())
               {
                  const C_OscNodeDataPool * const pc_OscDataPool = rc_OscNode.GetComDataPoolConst(
                     orc_MessageId.e_ComProtocol, orc_MessageId.u32_DatapoolIndex);

                  if (pc_OscDataPool != NULL)
                  {
                     C_PuiSdNode & rc_UiNode = this->mc_UiNodes[orc_MessageId.u32_NodeIndex];
                     if (pc_Protocol->u32_DataPoolIndex < rc_UiNode.c_UiDataPools.size())
                     {
                        uint32_t u32_ListIndex;
                        if (C_OscCanProtocol::h_GetComListIndex(*pc_OscDataPool, orc_MessageId.u32_InterfaceIndex,
                                                                orc_MessageId.q_MessageIsTx,
                                                                u32_ListIndex) == C_NO_ERR)
                        {
                           C_PuiSdNodeDataPool & rc_UiDataPool =
                              rc_UiNode.c_UiDataPools[pc_Protocol->u32_DataPoolIndex];
                           if (u32_ListIndex < rc_UiDataPool.c_DataPoolLists.size())
                           {
                              const C_OscCanSignal & rc_Signal = rc_Message.c_Signals[oru32_SignalIndex];
                              C_PuiSdNodeDataPoolList & rc_UiList = rc_UiDataPool.c_DataPoolLists[u32_ListIndex];
                              if (rc_Signal.u32_ComDataElementIndex < rc_UiList.c_DataPoolListElements.size())
                              {
                                 rc_UiList.c_DataPoolListElements[rc_Signal.u32_ComDataElementIndex] =
                                    orc_UiSignalCommon;
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      //Ui message part
      if (pc_UiProtocol != NULL)
      {
         if (orc_MessageId.u32_InterfaceIndex < pc_UiProtocol->c_ComMessages.size())
         {
            C_PuiSdNodeCanMessageContainer & rc_MessageContainer =
               pc_UiProtocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
            std::vector<C_PuiSdNodeCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(
               orc_MessageId.q_MessageIsTx);
            if (orc_MessageId.u32_MessageIndex < rc_Messages.size())
            {
               C_PuiSdNodeCanMessage & rc_Message = rc_Messages[orc_MessageId.u32_MessageIndex];
               if (oru32_SignalIndex < rc_Message.c_Signals.size())
               {
                  rc_Message.c_Signals[oru32_SignalIndex] = orc_UiSignal;
               }
            }
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set can signal common part

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
   \param[in]  ou16_MultiplexValue  New multiplex value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::SetCanSignalMuxValue(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                     const uint32_t & oru32_SignalIndex,
                                                     const uint16_t ou16_MultiplexValue)
{
   int32_t s32_Retval = C_NO_ERR;

   //Position
   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UiNodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      s32_Retval = rc_OscNode.SetSignalMuxValue(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                                                orc_MessageId.u32_DatapoolIndex,
                                                orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex,
                                                oru32_SignalIndex, ou16_MultiplexValue);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add can message

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  ore_ComType                      Can protocol type
   \param[in]  oru32_InterfaceIndex             Node interface index
   \param[in]  ou32_DatapoolIndex               Datapool index
   \param[in]  orq_MessageIsTx                  Flag if message is tx type (else rx type assumed)
   \param[in]  orc_Message                      Message data
   \param[in]  orc_OscSignalCommons             Signals data (osc common)
   \param[in]  orc_UiSignalCommons              Signals data (ui common)
   \param[in]  orc_UiMessage                    Message data (ui)
   \param[in]  orq_AutomatedPropertiesAdaption  Optional parameter to allow or disable automated properties adaption

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::AddCanMessage(const uint32_t & oru32_NodeIndex,
                                              const C_OscCanProtocol::E_Type & ore_ComType,
                                              const uint32_t & oru32_InterfaceIndex, const uint32_t ou32_DatapoolIndex,
                                              const bool & orq_MessageIsTx, const C_OscCanMessage & orc_Message,
                                              const std::vector<C_OscNodeDataPoolListElement> & orc_OscSignalCommons,
                                              const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons,
                                              const C_PuiSdNodeCanMessage & orc_UiMessage,
                                              const bool & orq_AutomatedPropertiesAdaption)
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscCanMessageContainer * const pc_MessageContainer = this->GetCanProtocolMessageContainer(oru32_NodeIndex,
                                                                                                     ore_ComType,
                                                                                                     oru32_InterfaceIndex,
                                                                                                     ou32_DatapoolIndex);

   if (pc_MessageContainer != NULL)
   {
      const std::vector<C_OscCanMessage> & rc_Messages = pc_MessageContainer->GetMessagesConst(orq_MessageIsTx);
      const uint32_t u32_MessageIndex = rc_Messages.size();
      const C_OscCanMessageIdentificationIndices c_MessageId(oru32_NodeIndex, ore_ComType, oru32_InterfaceIndex,
                                                             ou32_DatapoolIndex,
                                                             orq_MessageIsTx,
                                                             u32_MessageIndex);
      s32_Retval = this->InsertCanMessage(c_MessageId, orc_Message, orc_OscSignalCommons, orc_UiSignalCommons,
                                          orc_UiMessage, orq_AutomatedPropertiesAdaption);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert can message

   \param[in]  orc_MessageId                    Message identification indices
   \param[in]  orc_Message                      Message data
   \param[in]  orc_OscSignalCommons             Signals data (osc common)
   \param[in]  orc_UiSignalCommons              Signals data (ui common)
   \param[in]  orc_UiMessage                    Message data (ui)
   \param[in]  orq_AutomatedPropertiesAdaption  Optional parameter to allow or disable automated properties adaption

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::InsertCanMessage(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                 const C_OscCanMessage & orc_Message,
                                                 const std::vector<C_OscNodeDataPoolListElement> & orc_OscSignalCommons,
                                                 const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons, const C_PuiSdNodeCanMessage & orc_UiMessage,
                                                 const bool & orq_AutomatedPropertiesAdaption)
{
   int32_t s32_Retval = C_NO_ERR;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UiNodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      C_OscCanMessage c_AdaptedMessage = orc_Message;
      if (orq_AutomatedPropertiesAdaption == true)
      {
         //Get unique name
         c_AdaptedMessage.c_Name = C_OscUtils::h_GetUniqueName(
            this->m_GetExistingMessageNames(orc_MessageId.u32_NodeIndex, orc_MessageId.u32_InterfaceIndex),
            orc_Message.c_Name, this->GetNameMaxCharLimit());
      }
      s32_Retval = rc_OscNode.InsertMessage(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                                            orc_MessageId.u32_DatapoolIndex,
                                            orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex,
                                            c_AdaptedMessage,
                                            orc_OscSignalCommons);
      //Signal each new signal to the sync engine (ascending order is important)
      for (uint32_t u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
      {
         C_OscNodeDataPoolListElementId c_Id;
         if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, u32_ItSig, c_Id) == C_NO_ERR)
         {
            m_HandleSyncNodeDataPoolListElementAdded(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                     c_Id.u32_ListIndex, c_Id.u32_ElementIndex);
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Ui part
         s32_Retval = m_InsertUiCanMessage(orc_MessageId, orc_UiSignalCommons, orc_UiMessage);
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete can message

   \param[in]  orc_MessageId  Message identification indices

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::DeleteCanMessage(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   int32_t s32_Retval = C_NO_ERR;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UiNodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      if (pc_Message != NULL)
      {
         //Signal each deleted signal to the sync engine (descending order is important, do this before actually
         // deleting anything)
         for (uint32_t u32_ItSig = pc_Message->c_Signals.size(); u32_ItSig > 0UL; --u32_ItSig)
         {
            C_OscNodeDataPoolListElementId c_Id;
            if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, u32_ItSig - 1UL, c_Id) == C_NO_ERR)
            {
               m_HandleSyncNodeDataPoolListElementAboutToBeDeleted(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                                   c_Id.u32_ListIndex, c_Id.u32_ElementIndex);
            }
         }
      }
      //Ui part
      s32_Retval = m_DeleteUiCanMessage(orc_MessageId);
      //After deleting signals delete other parts
      tgl_assert(rc_OscNode.DeleteMessage(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                                          orc_MessageId.u32_DatapoolIndex,
                                          orc_MessageId.q_MessageIsTx,
                                          orc_MessageId.u32_MessageIndex) == C_NO_ERR);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add can signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  orc_Signal           Signal data (osc)
   \param[in]  orc_OscSignalCommon  Signal data (osc common)
   \param[in]  orc_UiSignalCommon   Signal data (ui common)
   \param[in]  orc_UiSignal         Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::AddCanSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                             const C_OscCanSignal & orc_Signal,
                                             const C_OscNodeDataPoolListElement & orc_OscSignalCommon,
                                             const C_PuiSdNodeDataPoolListElement & orc_UiSignalCommon,
                                             const C_PuiSdNodeCanSignal & orc_UiSignal)
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);

   if (pc_Message != NULL)
   {
      const uint32_t u32_SignalIndex = pc_Message->c_Signals.size();
      s32_Retval = this->InsertCanSignal(orc_MessageId, u32_SignalIndex, orc_Signal, orc_OscSignalCommon,
                                         orc_UiSignalCommon, orc_UiSignal);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert can signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
   \param[in]  orc_Signal           Signal data (osc)
   \param[in]  orc_OscSignalCommon  Signal data (osc common)
   \param[in]  orc_UiSignalCommon   Signal data (ui common)
   \param[in]  orc_UiSignal         Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::InsertCanSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                const uint32_t & oru32_SignalIndex, const C_OscCanSignal & orc_Signal,
                                                const C_OscNodeDataPoolListElement & orc_OscSignalCommon,
                                                const C_PuiSdNodeDataPoolListElement & orc_UiSignalCommon,
                                                const C_PuiSdNodeCanSignal & orc_UiSignal)
{
   int32_t s32_Retval = C_NO_ERR;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UiNodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      C_OscNodeDataPoolListElement c_OscAdaptedSignalCommon = orc_OscSignalCommon;
      C_OscNodeDataPoolListElementId c_Id;

      //Get unique name
      c_OscAdaptedSignalCommon.c_Name = C_OscUtils::h_GetUniqueName(
         this->m_GetExistingSignalNames(orc_MessageId), orc_OscSignalCommon.c_Name, this->GetNameMaxCharLimit());
      s32_Retval = rc_OscNode.InsertSignal(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                                           orc_MessageId.u32_DatapoolIndex,
                                           orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex,
                                           oru32_SignalIndex,
                                           orc_Signal,
                                           c_OscAdaptedSignalCommon);

      //Signal new signal to the sync engine
      if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, oru32_SignalIndex, c_Id) == C_NO_ERR)
      {
         m_HandleSyncNodeDataPoolListElementAdded(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                  c_Id.u32_ListIndex, c_Id.u32_ElementIndex);
      }
      if (s32_Retval == C_NO_ERR)
      {
         const C_OscCanProtocol * const pc_Protocol = rc_OscNode.GetCanProtocolConst(orc_MessageId.e_ComProtocol,
                                                                                     orc_MessageId.u32_DatapoolIndex);
         C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                                 orc_MessageId.e_ComProtocol,
                                                                                 orc_MessageId.u32_DatapoolIndex);
         //Ui part
         if (pc_Protocol != NULL)
         {
            if (orc_MessageId.u32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
            {
               const C_OscCanMessageContainer & rc_MessageContainer =
                  pc_Protocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
               const uint32_t u32_SignalIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
                  orc_MessageId.q_MessageIsTx,
                  orc_MessageId.u32_MessageIndex);
               C_PuiSdNode & rc_UiNode = this->mc_UiNodes[orc_MessageId.u32_NodeIndex];
               if (pc_Protocol->u32_DataPoolIndex < rc_UiNode.c_UiDataPools.size())
               {
                  const C_OscNodeDataPool * const pc_OscDataPool = rc_OscNode.GetComDataPoolConst(
                     orc_MessageId.e_ComProtocol,
                     orc_MessageId.u32_DatapoolIndex);

                  if (pc_OscDataPool != NULL)
                  {
                     uint32_t u32_ListIndex;
                     if (C_OscCanProtocol::h_GetComListIndex(*pc_OscDataPool, orc_MessageId.u32_InterfaceIndex,
                                                             orc_MessageId.q_MessageIsTx,
                                                             u32_ListIndex) == C_NO_ERR)
                     {
                        C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[pc_Protocol->u32_DataPoolIndex];
                        if (u32_ListIndex < rc_UiDataPool.c_DataPoolLists.size())
                        {
                           C_PuiSdNodeDataPoolList & rc_UiList = rc_UiDataPool.c_DataPoolLists[u32_ListIndex];
                           if ((u32_SignalIndex + oru32_SignalIndex) <= rc_UiList.c_DataPoolListElements.size())
                           {
                              rc_UiList.c_DataPoolListElements.insert(
                                 rc_UiList.c_DataPoolListElements.begin() + u32_SignalIndex + oru32_SignalIndex,
                                 orc_UiSignalCommon);
                           }
                           else
                           {
                              s32_Retval = C_RANGE;
                           }
                        }
                        else
                        {
                           s32_Retval = C_RANGE;
                        }
                     }
                     else
                     {
                        s32_Retval = C_RANGE;
                     }
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         //Ui part
         if (pc_UiProtocol != NULL)
         {
            if (orc_MessageId.u32_InterfaceIndex < pc_UiProtocol->c_ComMessages.size())
            {
               C_PuiSdNodeCanMessageContainer & rc_MessageContainer =
                  pc_UiProtocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
               std::vector<C_PuiSdNodeCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(
                  orc_MessageId.q_MessageIsTx);
               if (orc_MessageId.u32_MessageIndex < rc_Messages.size())
               {
                  C_PuiSdNodeCanMessage & rc_Message = rc_Messages[orc_MessageId.u32_MessageIndex];
                  if (oru32_SignalIndex <= rc_Message.c_Signals.size())
                  {
                     rc_Message.c_Signals.insert(rc_Message.c_Signals.begin() + oru32_SignalIndex, orc_UiSignal);
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete can signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::DeleteCanSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                const uint32_t & oru32_SignalIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UiNodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      C_OscNodeDataPoolListElementId c_Id;

      //Signal deleted signal to the sync engine
      if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, oru32_SignalIndex, c_Id) == C_NO_ERR)
      {
         m_HandleSyncNodeDataPoolListElementAboutToBeDeleted(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                             c_Id.u32_ListIndex, c_Id.u32_ElementIndex);
      }

      s32_Retval = rc_OscNode.DeleteSignal(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                                           orc_MessageId.u32_DatapoolIndex,
                                           orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex,
                                           oru32_SignalIndex);
      if (s32_Retval == C_NO_ERR)
      {
         const C_OscCanProtocol * const pc_Protocol = rc_OscNode.GetCanProtocolConst(orc_MessageId.e_ComProtocol,
                                                                                     orc_MessageId.u32_DatapoolIndex);
         C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                                 orc_MessageId.e_ComProtocol,
                                                                                 orc_MessageId.u32_DatapoolIndex);
         //Ui part
         if (pc_Protocol != NULL)
         {
            if (orc_MessageId.u32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
            {
               const C_OscCanMessageContainer & rc_MessageContainer =
                  pc_Protocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
               const uint32_t u32_SignalIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
                  orc_MessageId.q_MessageIsTx,
                  orc_MessageId.u32_MessageIndex);
               C_PuiSdNode & rc_UiNode = this->mc_UiNodes[orc_MessageId.u32_NodeIndex];
               if (pc_Protocol->u32_DataPoolIndex < rc_UiNode.c_UiDataPools.size())
               {
                  const C_OscNodeDataPool * const pc_OscDataPool = rc_OscNode.GetComDataPoolConst(
                     orc_MessageId.e_ComProtocol,
                     orc_MessageId.u32_DatapoolIndex);

                  if (pc_OscDataPool != NULL)
                  {
                     uint32_t u32_ListIndex;
                     if (C_OscCanProtocol::h_GetComListIndex(*pc_OscDataPool, orc_MessageId.u32_InterfaceIndex,
                                                             orc_MessageId.q_MessageIsTx,
                                                             u32_ListIndex) == C_NO_ERR)
                     {
                        C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[pc_Protocol->u32_DataPoolIndex];
                        if (u32_ListIndex < rc_UiDataPool.c_DataPoolLists.size())
                        {
                           C_PuiSdNodeDataPoolList & rc_UiList = rc_UiDataPool.c_DataPoolLists[u32_ListIndex];
                           if ((u32_SignalIndex + oru32_SignalIndex) < rc_UiList.c_DataPoolListElements.size())
                           {
                              rc_UiList.c_DataPoolListElements.erase(
                                 rc_UiList.c_DataPoolListElements.begin() + u32_SignalIndex + oru32_SignalIndex);
                           }
                           else
                           {
                              s32_Retval = C_RANGE;
                           }
                        }
                        else
                        {
                           s32_Retval = C_RANGE;
                        }
                     }
                     else
                     {
                        s32_Retval = C_RANGE;
                     }
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         //Ui part
         if (pc_UiProtocol != NULL)
         {
            if (orc_MessageId.u32_InterfaceIndex < pc_UiProtocol->c_ComMessages.size())
            {
               C_PuiSdNodeCanMessageContainer & rc_MessageContainer =
                  pc_UiProtocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
               std::vector<C_PuiSdNodeCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(
                  orc_MessageId.q_MessageIsTx);
               if (orc_MessageId.u32_MessageIndex < rc_Messages.size())
               {
                  C_PuiSdNodeCanMessage & rc_Message = rc_Messages[orc_MessageId.u32_MessageIndex];
                  if (oru32_SignalIndex < rc_Message.c_Signals.size())
                  {
                     rc_Message.c_Signals.erase(rc_Message.c_Signals.begin() + oru32_SignalIndex);
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert element index to signal index

   Take node, datapool, list, element index and return message id and signal index.

   \param[in]      orc_NodeDatapoolListElementId   node, datapool, list, element index packed together
   \param[in,out]  orc_MessageId                   message id
   \param[in,out]  oru32_SignalIndex               signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::ConvertElementIndexToSignalIndex(
   const C_OscNodeDataPoolListElementId & orc_NodeDatapoolListElementId,
   C_OscCanMessageIdentificationIndices & orc_MessageId, uint32_t & oru32_SignalIndex) const
{
   const C_OscNode * const pc_Node = this->GetOscNodeConst(orc_NodeDatapoolListElementId.u32_NodeIndex);
   const C_OscNodeDataPool * const pc_Datapool = this->GetOscDataPool(orc_NodeDatapoolListElementId.u32_NodeIndex,
                                                                      orc_NodeDatapoolListElementId.u32_DataPoolIndex);

   C_OscCanProtocol::E_Type e_ComProtocol = C_OscCanProtocol::eLAYER2;

   if ((pc_Node != NULL) && (pc_Datapool != NULL))
   {
      bool q_MessageIsTx;
      const C_OscNodeDataPoolList & rc_List = pc_Datapool->c_Lists[orc_NodeDatapoolListElementId.u32_ListIndex];
      uint32_t u32_InterfaceIndex;
      uint32_t u32_MessageIndex = 0;

      // com protocol type
      for (uint32_t u32_Pos = 0; u32_Pos < pc_Node->c_ComProtocols.size(); u32_Pos++)
      {
         if (pc_Node->c_ComProtocols[u32_Pos].u32_DataPoolIndex == orc_NodeDatapoolListElementId.u32_DataPoolIndex)
         {
            e_ComProtocol = pc_Node->c_ComProtocols[u32_Pos].e_Type;
         }
      }

      // message is tx
      q_MessageIsTx = C_OscCanProtocol::h_ListIsComTx(rc_List);

      // interface index
      u32_InterfaceIndex = orc_NodeDatapoolListElementId.u32_ListIndex / 2; // 2:1 relation (see EA diagram)

      // message index
      const C_OscCanMessageContainer * const pc_MessageContainer = this->GetCanProtocolMessageContainer(
         orc_NodeDatapoolListElementId.u32_NodeIndex, e_ComProtocol, u32_InterfaceIndex,
         orc_NodeDatapoolListElementId.u32_DataPoolIndex);
      std::vector<C_OscCanMessage> c_Messages; // either tx messages or rx messages
      bool q_IndexFound = false;
      tgl_assert(pc_MessageContainer != NULL);

      if (pc_MessageContainer != NULL)
      {
         if (q_MessageIsTx == true)
         {
            c_Messages = pc_MessageContainer->c_TxMessages;
         }
         else
         {
            c_Messages = pc_MessageContainer->c_RxMessages;
         }
      }

      for (uint32_t u32_PosMessage = 0; (u32_PosMessage < c_Messages.size()) && (q_IndexFound == false);
           u32_PosMessage++)
      {
         const C_OscCanMessage & rc_Message = c_Messages[u32_PosMessage];
         for (uint32_t u32_PosSignal = 0; (u32_PosSignal < rc_Message.c_Signals.size()) && (q_IndexFound == false);
              u32_PosSignal++)
         {
            const C_OscCanSignal & rc_Signal = rc_Message.c_Signals[u32_PosSignal];
            if (rc_Signal.u32_ComDataElementIndex == orc_NodeDatapoolListElementId.u32_ElementIndex)
            {
               u32_MessageIndex = u32_PosMessage;
               oru32_SignalIndex = u32_PosSignal;
               q_IndexFound = true;
            }
         }
      }

      // output
      orc_MessageId = C_OscCanMessageIdentificationIndices(orc_NodeDatapoolListElementId.u32_NodeIndex,
                                                           e_ComProtocol, u32_InterfaceIndex,
                                                           orc_NodeDatapoolListElementId.u32_DataPoolIndex,
                                                           q_MessageIsTx, u32_MessageIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if specified messages match

   \param[in]   orc_MessageId1               First message identification indices
   \param[in]   orc_MessageId2               Second message identification indices
   \param[out]  orq_IsMatch                  Flag if messages match
   \param[in]   oq_IgnoreMessageDirection    Flag to compare messages without message direction check
                                             True: compare the message content no matter what the message directions are
                                             False: if both messages are Tx this function will always say: no match

   \return
   C_NO_ERR Done
   C_RANGE  Nodes or Datapools or Lists or Messages do not exist
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::CheckMessageMatch(const C_OscCanMessageIdentificationIndices & orc_MessageId1,
                                                  const C_OscCanMessageIdentificationIndices & orc_MessageId2,
                                                  bool & orq_IsMatch, const bool oq_IgnoreMessageDirection) const
{
   int32_t s32_Retval = this->mc_CoreDefinition.CheckMessageMatch(orc_MessageId1, orc_MessageId2, orq_IsMatch,
                                                                  oq_IgnoreMessageDirection);

   //Check Ui part
   if ((s32_Retval == C_NO_ERR) && (orq_IsMatch == true))
   {
      const C_OscCanMessageContainer * const pc_MessageContainer1 =
         this->GetCanProtocolMessageContainer(orc_MessageId1.u32_NodeIndex, orc_MessageId1.e_ComProtocol,
                                              orc_MessageId1.u32_InterfaceIndex, orc_MessageId1.u32_DatapoolIndex);
      const C_OscCanMessageContainer * const pc_MessageContainer2 =
         this->GetCanProtocolMessageContainer(orc_MessageId2.u32_NodeIndex, orc_MessageId2.e_ComProtocol,
                                              orc_MessageId2.u32_InterfaceIndex, orc_MessageId2.u32_DatapoolIndex);
      if ((pc_MessageContainer1 != NULL) && (pc_MessageContainer2 != NULL))
      {
         const std::vector<C_OscCanMessage> & rc_Messages1 = pc_MessageContainer1->GetMessagesConst(
            orc_MessageId1.q_MessageIsTx);
         const std::vector<C_OscCanMessage> & rc_Messages2 = pc_MessageContainer2->GetMessagesConst(
            orc_MessageId2.q_MessageIsTx);
         if ((orc_MessageId1.u32_MessageIndex < rc_Messages1.size()) &&
             (orc_MessageId2.u32_MessageIndex < rc_Messages2.size()))
         {
            const C_OscCanMessage c_Message1 = rc_Messages1[orc_MessageId1.u32_MessageIndex];
            const C_OscCanMessage c_Message2 = rc_Messages2[orc_MessageId2.u32_MessageIndex];
            if (c_Message1.c_Signals.size() == c_Message2.c_Signals.size())
            {
               //Step 1: Get position and size in list
               const uint32_t u32_SignalSize = c_Message1.c_Signals.size();

               if (u32_SignalSize > 0)
               {
                  //Step 2: Check if list contains actual signal data
                  const C_PuiSdNodeDataPoolList * const pc_List1 = this->m_GetUiCanDataPoolList(
                     orc_MessageId1.u32_NodeIndex,
                     orc_MessageId1.e_ComProtocol,
                     orc_MessageId1.u32_InterfaceIndex,
                     orc_MessageId1.u32_DatapoolIndex,
                     orc_MessageId1.q_MessageIsTx);
                  const C_PuiSdNodeDataPoolList * const pc_List2 = this->m_GetUiCanDataPoolList(
                     orc_MessageId2.u32_NodeIndex,
                     orc_MessageId2.e_ComProtocol,
                     orc_MessageId2.u32_InterfaceIndex,
                     orc_MessageId2.u32_DatapoolIndex,
                     orc_MessageId2.q_MessageIsTx);

                  if ((pc_List1 != NULL) && (pc_List2 != NULL))
                  {
                     const uint32_t u32_List1Start = pc_MessageContainer1->GetMessageSignalDataStartIndex(
                        orc_MessageId1.q_MessageIsTx, orc_MessageId1.u32_MessageIndex);
                     const uint32_t u32_List2Start = pc_MessageContainer2->GetMessageSignalDataStartIndex(
                        orc_MessageId2.q_MessageIsTx, orc_MessageId2.u32_MessageIndex);
                     if (((u32_List1Start + u32_SignalSize) <= pc_List1->c_DataPoolListElements.size()) &&
                         ((u32_List2Start + u32_SignalSize) <= pc_List2->c_DataPoolListElements.size()))
                     {
                        for (uint32_t u32_ItSignal = 0; u32_ItSignal < u32_SignalSize; ++u32_ItSignal)
                        {
                           const uint32_t u32_Index1 = u32_List1Start + u32_ItSignal;
                           const uint32_t u32_Index2 = u32_List2Start + u32_ItSignal;
                           const C_PuiSdNodeDataPoolListElement & rc_UiElement1 =
                              pc_List1->c_DataPoolListElements[u32_Index1];
                           const C_PuiSdNodeDataPoolListElement & rc_UiElement2 =
                              pc_List2->c_DataPoolListElements[u32_Index2];
                           if (rc_UiElement1.q_AutoMinMaxActive != rc_UiElement2.q_AutoMinMaxActive)
                           {
                              orq_IsMatch = false;
                           }
                        }
                     }
                     else
                     {
                        s32_Retval = C_RANGE;
                     }
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
            }
            else
            {
               orq_IsMatch = false;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map bus name to index

   \param[in]   orc_BusName      Bus name
   \param[out]  oru32_BusIndex   Bus index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::MapBusNameToIndex(const QString & orc_BusName, uint32_t & oru32_BusIndex) const
{
   int32_t s32_Retval = C_RANGE;

   for (uint32_t u32_ItBus = 0UL; u32_ItBus < this->GetOscBusesSize(); ++u32_ItBus)
   {
      const C_OscSystemBus * const pc_Bus = this->GetOscBus(u32_ItBus);
      tgl_assert(pc_Bus != NULL);
      if (pc_Bus != NULL)
      {
         if (orc_BusName.compare(pc_Bus->c_Name.c_str()) == 0)
         {
            s32_Retval = C_NO_ERR;
            oru32_BusIndex = u32_ItBus;
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map bus index to name

   \param[in]      ou32_BusIndex    Bus index
   \param[in,out]  orc_BusName      Bus name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::MapBusIndexToName(const uint32_t ou32_BusIndex, QString & orc_BusName) const
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscSystemBus * const pc_Bus = this->GetOscBus(ou32_BusIndex);

   if (pc_Bus != NULL)
   {
      orc_BusName = pc_Bus->c_Name.c_str();
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get signal display name

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
   \param[in]  oq_ToolTip           True: display name for tool tip (short), false: general display name

   \return
   Signal display name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdHandlerBusLogic::GetCanSignalDisplayName(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                        const uint32_t & oru32_SignalIndex, const bool oq_ToolTip) const
{
   QString c_Retval;
   const C_OscCanSignal * const pc_Signal = this->GetCanSignal(orc_MessageId, oru32_SignalIndex);
   const C_OscNodeDataPoolListElement * const pc_SignalData = this->GetOscCanDataPoolListElement(orc_MessageId,
                                                                                                 oru32_SignalIndex);

   if ((pc_Signal != NULL) && (pc_SignalData != NULL))
   {
      c_Retval = pc_SignalData->c_Name.c_str();

      // append multiplexer information for multiplexer signals
      if (pc_Signal->e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         c_Retval = static_cast<QString>("%1 (Multiplexer)").arg(c_Retval);
      }

      // add SPN for J1939 signals (not for tooltip)
      if (orc_MessageId.e_ComProtocol == C_OscCanProtocol::eJ1939)
      {
         if (oq_ToolTip == false)
         {
            c_Retval =
               static_cast<QString>("SPN %1  %2").arg(pc_Signal->u32_J1939SuspectParameterNumber).arg(c_Retval);
         }
         else
         {
            c_Retval =
               static_cast<QString>("%1 (SPN %2)").arg(c_Retval).arg(pc_Signal->u32_J1939SuspectParameterNumber);
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get can message display name

   \param[in]  orc_MessageId  Message identification indices
   \param[in]  oq_ToolTip     True: display name for tool tip (short), false: general display name

   \return
   Can message display name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdHandlerBusLogic::GetCanMessageDisplayName(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                         const bool oq_ToolTip) const
{
   QString c_Retval;
   const C_OscCanMessage * const pc_MessageData = this->GetCanMessage(orc_MessageId);

   if (pc_MessageData != NULL)
   {
      c_Retval = static_cast<QString>("%1 (0x%2)").
                 arg(pc_MessageData->c_Name.c_str()).
                 arg(QString::number(pc_MessageData->u32_CanId, 16).toUpper());

      // Add PGN for J1939 signals (not for tooltip)
      if ((oq_ToolTip == false) && (orc_MessageId.e_ComProtocol  == C_OscCanProtocol::eJ1939))
      {
         C_OscCanUtilJ1939PgInfo c_PgInfo;
         C_OscCanUtil::h_GetJ1939PgInfoFromCanId(pc_MessageData->u32_CanId, c_PgInfo);

         const uint32_t u32_VisiblePgn = C_OscCanUtil::h_GetVisiblePgn(c_PgInfo.u32_Pgn);
         c_Retval = static_cast<QString>("PGN %1  %2").arg(u32_VisiblePgn).arg(c_Retval);
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Count local messages

   \param[in]   oru32_NodeIndex     Node index
   \param[out]  oru32_MessageCount  Message count

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::CountLocalMessages(const uint32_t & oru32_NodeIndex,
                                                   uint32_t & oru32_MessageCount) const
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNode * const pc_Node = this->GetOscNodeConst(oru32_NodeIndex);

   oru32_MessageCount = 0;
   if (pc_Node != NULL)
   {
      for (uint32_t u32_ItProt = 0UL; u32_ItProt < pc_Node->c_ComProtocols.size(); ++u32_ItProt)
      {
         const C_OscCanProtocol & rc_Prot = pc_Node->c_ComProtocols[u32_ItProt];
         for (uint32_t u32_ItCont = 0UL; u32_ItCont < rc_Prot.c_ComMessages.size(); ++u32_ItCont)
         {
            const C_OscCanMessageContainer & rc_Cont = rc_Prot.c_ComMessages[u32_ItCont];
            oru32_MessageCount += rc_Cont.c_RxMessages.size();
            oru32_MessageCount += rc_Cont.c_TxMessages.size();
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdHandlerBusLogic::C_PuiSdHandlerBusLogic(QObject * const opc_Parent) :
   C_PuiSdHandlerHalc(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pointers to all currently registered message names contained in the specified node interface & protocol

   \param[in]   oru32_NodeIndex        Node index
   \param[in]   ore_ComType            Protocol type
   \param[in]   oru32_InterfaceIndex   Index of interface of node
   \param[out]  orc_ExistingNames      Vector of pointers to all currently registered message names contained in the
                                       specified node interface & protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::m_GetExistingMessageNamesProtocol(const uint32_t & oru32_NodeIndex,
                                                               const C_OscCanProtocol::E_Type & ore_ComType,
                                                               const uint32_t & oru32_InterfaceIndex,
                                                               std::map<C_SclString, bool> & orc_ExistingNames)
const
{
   const std::vector<const C_OscCanMessageContainer *> c_Container = this->GetCanProtocolMessageContainers(
      oru32_NodeIndex, ore_ComType, oru32_InterfaceIndex);
   uint32_t u32_Counter;

   // Check all COMM Datapools
   for (u32_Counter = 0U; u32_Counter < c_Container.size(); ++u32_Counter)
   {
      const C_OscCanMessageContainer * const pc_Container = c_Container[u32_Counter];
      const std::vector<C_OscCanMessage> & rc_TxMessages = pc_Container->GetMessagesConst(true);
      const std::vector<C_OscCanMessage> & rc_RxMessages = pc_Container->GetMessagesConst(false);
      //Tx
      for (uint32_t u32_ItMessage = 0; u32_ItMessage < rc_TxMessages.size(); ++u32_ItMessage)
      {
         const C_OscCanMessage & rc_Message = rc_TxMessages[u32_ItMessage];
         orc_ExistingNames[rc_Message.c_Name] = true;
      }
      //Rx
      for (uint32_t u32_ItMessage = 0; u32_ItMessage < rc_RxMessages.size(); ++u32_ItMessage)
      {
         const C_OscCanMessage & rc_Message = rc_RxMessages[u32_ItMessage];
         orc_ExistingNames[rc_Message.c_Name] = true;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pointers to all currently registered message names contained in the specified node interface

   \param[in]  oru32_NodeIndex         Node index
   \param[in]  oru32_InterfaceIndex    Index of interface of node

   \return
   Vector of pointers to all currently registered message names contained in the specified node interface
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<C_SclString, bool> C_PuiSdHandlerBusLogic::m_GetExistingMessageNames(const uint32_t & oru32_NodeIndex,
                                                                              const uint32_t & oru32_InterfaceIndex)
const
{
   std::map<C_SclString, bool> c_ExistingNames;

   // get existing message names of all three protocols
   m_GetExistingMessageNamesProtocol(oru32_NodeIndex, C_OscCanProtocol::eLAYER2, oru32_InterfaceIndex, c_ExistingNames);
   m_GetExistingMessageNamesProtocol(oru32_NodeIndex, C_OscCanProtocol::eCAN_OPEN_SAFETY, oru32_InterfaceIndex,
                                     c_ExistingNames);
   m_GetExistingMessageNamesProtocol(oru32_NodeIndex, C_OscCanProtocol::eECES, oru32_InterfaceIndex, c_ExistingNames);
   m_GetExistingMessageNamesProtocol(oru32_NodeIndex, C_OscCanProtocol::eCAN_OPEN, oru32_InterfaceIndex,
                                     c_ExistingNames);
   m_GetExistingMessageNamesProtocol(oru32_NodeIndex, C_OscCanProtocol::eJ1939, oru32_InterfaceIndex,
                                     c_ExistingNames);

   return c_ExistingNames;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pointers to all currently registered signal names contained in the specified message

   Extract names of data pool.

   \param[in]  orc_MessageId  Message identification indices

   \return
   Vector of pointers to all currently registered signal names contained in the specified message
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<C_SclString, bool> C_PuiSdHandlerBusLogic::m_GetExistingSignalNames(
   const C_OscCanMessageIdentificationIndices & orc_MessageId) const
{
   std::map<C_SclString, bool> c_Retval;
   const C_OscNode * const pc_Node = this->GetOscNodeConst(orc_MessageId.u32_NodeIndex);
   if (pc_Node != NULL)
   {
      const C_OscNodeDataPool * const pc_ComDataPool = pc_Node->GetComDataPoolConst(orc_MessageId.e_ComProtocol,
                                                                                    orc_MessageId.u32_DatapoolIndex);
      if (pc_ComDataPool != NULL)
      {
         const C_OscNodeDataPoolList * const pc_List = C_OscCanProtocol::h_GetComListConst(*pc_ComDataPool,
                                                                                           orc_MessageId.u32_InterfaceIndex,
                                                                                           orc_MessageId.q_MessageIsTx);
         const C_OscCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);
         if ((pc_Message != NULL) && (pc_List != NULL))
         {
            for (uint32_t u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
            {
               const C_OscCanSignal & rc_Signal = pc_Message->c_Signals[u32_ItSignal];
               if (rc_Signal.u32_ComDataElementIndex < pc_List->c_Elements.size())
               {
                  const C_OscNodeDataPoolListElement & rc_DataElement =
                     pc_List->c_Elements[rc_Signal.u32_ComDataElementIndex];
                  c_Retval[rc_DataElement.c_Name] = true;
               }
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get com protocol (ui)

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  ore_ComType          Can protocol type
   \param[in]  ou32_DatapoolIndex   Datapool index

   \return
   NULL No matching data found
   Else Pointer to data
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdNodeCanProtocol * C_PuiSdHandlerBusLogic::m_GetUiCanProtocol(const uint32_t & oru32_NodeIndex,
                                                                    const C_OscCanProtocol::E_Type & ore_ComType,
                                                                    const uint32_t ou32_DatapoolIndex)
{
   C_PuiSdNodeCanProtocol * pc_Retval = NULL;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UiNodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      tgl_assert(rc_OscNode.c_ComProtocols.size() == rc_UiNode.c_UiCanProtocols.size());
      for (uint32_t u32_ItCanProtocol = 0; u32_ItCanProtocol < rc_OscNode.c_ComProtocols.size(); ++u32_ItCanProtocol)
      {
         const C_OscCanProtocol & rc_CanProtocol = rc_OscNode.c_ComProtocols[u32_ItCanProtocol];
         if ((rc_CanProtocol.e_Type == ore_ComType) &&
             (rc_CanProtocol.u32_DataPoolIndex == ou32_DatapoolIndex))
         {
            pc_Retval = &rc_UiNode.c_UiCanProtocols[u32_ItCanProtocol];
            break;
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get signal list (ui)

   \param[in]  oru32_NodeIndex         Node index
   \param[in]  ore_ComType             Can protocol type
   \param[in]  oru32_InterfaceIndex    Node interface index
   \param[in]  ou32_DatapoolIndex      Datapool index
   \param[in]  orq_MessageIsTx         Flag if message is tx type (else rx type assumed)

   \return
   NULL No matching data found
   Else Pointer to data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNodeDataPoolList * C_PuiSdHandlerBusLogic::m_GetUiCanDataPoolList(const uint32_t & oru32_NodeIndex,
                                                                               const C_OscCanProtocol::E_Type & ore_ComType, const uint32_t & oru32_InterfaceIndex, const uint32_t ou32_DatapoolIndex,
                                                                               const bool & orq_MessageIsTx) const
{
   const C_PuiSdNodeDataPoolList * pc_Retval = NULL;
   const C_OscNodeDataPool * const pc_OscDataPool = this->GetOscCanDataPool(oru32_NodeIndex, ore_ComType,
                                                                            ou32_DatapoolIndex);

   if (pc_OscDataPool != NULL)
   {
      uint32_t u32_Index;

      if (C_OscCanProtocol::h_GetComListIndex(*pc_OscDataPool, oru32_InterfaceIndex, orq_MessageIsTx,
                                              u32_Index) == C_NO_ERR)
      {
         const C_PuiSdNodeDataPool * const pc_UiDataPool = this->GetUiCanDataPool(oru32_NodeIndex, ore_ComType,
                                                                                  ou32_DatapoolIndex);

         if (pc_UiDataPool != NULL)
         {
            if (u32_Index < pc_UiDataPool->c_DataPoolLists.size())
            {
               pc_Retval = &pc_UiDataPool->c_DataPoolLists[u32_Index];
            }
         }
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert ui part for can message

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  orc_UiSignalCommons  Signals data (ui common)
   \param[in]  orc_UiMessage        Message data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::m_InsertUiCanMessage(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                     const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons,
                                                     const C_PuiSdNodeCanMessage & orc_UiMessage)
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNode * const pc_Node = this->GetOscNodeConst(orc_MessageId.u32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OscCanProtocol * const pc_Protocol = pc_Node->GetCanProtocolConst(orc_MessageId.e_ComProtocol,
                                                                                orc_MessageId.u32_DatapoolIndex);
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol,
                                                                              orc_MessageId.u32_DatapoolIndex);

      if (pc_Protocol != NULL)
      {
         if (orc_MessageId.u32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
         {
            const C_OscCanMessageContainer & rc_MessageContainer =
               pc_Protocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
            const uint32_t u32_SignalIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
               orc_MessageId.q_MessageIsTx,
               orc_MessageId.u32_MessageIndex);
            C_PuiSdNode & rc_UiNode = this->mc_UiNodes[orc_MessageId.u32_NodeIndex];
            if (pc_Protocol->u32_DataPoolIndex < rc_UiNode.c_UiDataPools.size())
            {
               const C_OscNodeDataPool * const pc_OscDataPool = pc_Node->GetComDataPoolConst(
                  orc_MessageId.e_ComProtocol, orc_MessageId.u32_DatapoolIndex);

               if (pc_OscDataPool != NULL)
               {
                  uint32_t u32_ListIndex;
                  if (C_OscCanProtocol::h_GetComListIndex(*pc_OscDataPool, orc_MessageId.u32_InterfaceIndex,
                                                          orc_MessageId.q_MessageIsTx,
                                                          u32_ListIndex) == C_NO_ERR)
                  {
                     C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[pc_Protocol->u32_DataPoolIndex];
                     if (u32_ListIndex < rc_UiDataPool.c_DataPoolLists.size())
                     {
                        C_PuiSdNodeDataPoolList & rc_UiList = rc_UiDataPool.c_DataPoolLists[u32_ListIndex];
                        if (u32_SignalIndex <= rc_UiList.c_DataPoolListElements.size())
                        {
                           for (uint32_t u32_ItSignal = 0; u32_ItSignal < orc_UiSignalCommons.size(); ++u32_ItSignal)
                           {
                              rc_UiList.c_DataPoolListElements.insert(
                                 rc_UiList.c_DataPoolListElements.begin() + u32_SignalIndex + u32_ItSignal,
                                 orc_UiSignalCommons[u32_ItSignal]);
                           }
                        }
                        else
                        {
                           s32_Retval = C_RANGE;
                        }
                     }
                     else
                     {
                        s32_Retval = C_RANGE;
                     }
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      //Ui part
      if (pc_UiProtocol != NULL)
      {
         if (orc_MessageId.u32_InterfaceIndex < pc_UiProtocol->c_ComMessages.size())
         {
            C_PuiSdNodeCanMessageContainer & rc_MessageContainer =
               pc_UiProtocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
            std::vector<C_PuiSdNodeCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(
               orc_MessageId.q_MessageIsTx);
            if (orc_MessageId.u32_MessageIndex <= rc_Messages.size())
            {
               rc_Messages.insert(rc_Messages.begin() + orc_MessageId.u32_MessageIndex, orc_UiMessage);
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete ui part of can message

   \param[in]  orc_MessageId  Message identification indices

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerBusLogic::m_DeleteUiCanMessage(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNode * const pc_Node = this->GetOscNodeConst(orc_MessageId.u32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OscCanProtocol * const pc_Protocol = pc_Node->GetCanProtocolConst(orc_MessageId.e_ComProtocol,
                                                                                orc_MessageId.u32_DatapoolIndex);
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol,
                                                                              orc_MessageId.u32_DatapoolIndex);

      if (pc_Protocol != NULL)
      {
         if (orc_MessageId.u32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
         {
            const C_OscCanMessageContainer & rc_MessageContainer =
               pc_Protocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
            const uint32_t u32_SignalIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
               orc_MessageId.q_MessageIsTx,
               orc_MessageId.u32_MessageIndex);
            C_PuiSdNode & rc_UiNode = this->mc_UiNodes[orc_MessageId.u32_NodeIndex];
            if (pc_Protocol->u32_DataPoolIndex < rc_UiNode.c_UiDataPools.size())
            {
               const C_OscNodeDataPool * const pc_OscDataPool = pc_Node->GetComDataPoolConst(
                  orc_MessageId.e_ComProtocol,
                  orc_MessageId.u32_DatapoolIndex);

               if (pc_OscDataPool != NULL)
               {
                  uint32_t u32_ListIndex;
                  if (C_OscCanProtocol::h_GetComListIndex(*pc_OscDataPool, orc_MessageId.u32_InterfaceIndex,
                                                          orc_MessageId.q_MessageIsTx,
                                                          u32_ListIndex) == C_NO_ERR)
                  {
                     C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[pc_Protocol->u32_DataPoolIndex];
                     if (u32_ListIndex < rc_UiDataPool.c_DataPoolLists.size())
                     {
                        C_PuiSdNodeDataPoolList & rc_UiList = rc_UiDataPool.c_DataPoolLists[u32_ListIndex];
                        if (u32_SignalIndex < rc_UiList.c_DataPoolListElements.size())
                        {
                           const C_PuiSdNodeCanMessage * const pc_Message = this->GetUiCanMessage(orc_MessageId);
                           if (pc_Message != NULL)
                           {
                              for (uint32_t u32_ItSignal = pc_Message->c_Signals.size(); u32_ItSignal > 0;
                                   --u32_ItSignal)
                              {
                                 rc_UiList.c_DataPoolListElements.erase(
                                    ((rc_UiList.c_DataPoolListElements.begin() + u32_SignalIndex) + u32_ItSignal) - 1);
                              }
                           }
                           else
                           {
                              s32_Retval = C_RANGE;
                           }
                        }
                     }
                     else
                     {
                        s32_Retval = C_RANGE;
                     }
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      //Ui part
      if (pc_UiProtocol != NULL)
      {
         if (orc_MessageId.u32_InterfaceIndex < pc_UiProtocol->c_ComMessages.size())
         {
            C_PuiSdNodeCanMessageContainer & rc_MessageContainer =
               pc_UiProtocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
            std::vector<C_PuiSdNodeCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(
               orc_MessageId.q_MessageIsTx);
            if (orc_MessageId.u32_MessageIndex < rc_Messages.size())
            {
               rc_Messages.erase(rc_Messages.begin() + orc_MessageId.u32_MessageIndex);
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}
