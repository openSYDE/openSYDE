//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus logic part of UI system definition

   Bus logic part of UI system definition

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_Uti.h"
#include "C_PuiSdUtil.h"
#include "C_PuiSdHandlerBusLogic.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add bus

   \param[in,out]  orc_OSCBus             New OSC bus (name might be modified by this function if not unique)
   \param[in]      orc_UIBus              UI bus
   \param[in]      opc_Name               New bus name
   \param[in]      oq_AllowBusIdAdaption  Flag if bus ID adaption is allowed

   \return
   Index of bus
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSdHandlerBusLogic::AddBusAndSort(C_OSCSystemBus & orc_OSCBus, const C_PuiSdBus & orc_UIBus,
                                             const QString * const opc_Name, const bool oq_AllowBusIdAdaption)
{
   const uint32 u32_Index = mc_CoreDefinition.c_Buses.size();

   if (opc_Name != NULL)
   {
      orc_OSCBus.c_Name = opc_Name->toStdString().c_str();
   }
   else
   {
      orc_OSCBus.c_Name = C_Uti::h_GetUniqueName(
         this->GetExistingBusNames(), orc_OSCBus.c_Name);
   }
   if (oq_AllowBusIdAdaption == true)
   {
      mc_CoreDefinition.GetNextFreeBusId(orc_OSCBus.u8_BusID);
   }

   mc_CoreDefinition.AddBus(orc_OSCBus);

   //insert UI part at same position as OSC part:
   this->mc_UIBuses.insert(mc_UIBuses.begin() + u32_Index, orc_UIBus);

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
std::map<C_SCLString, bool> C_PuiSdHandlerBusLogic::GetExistingBusNames(void) const
{
   std::map<C_SCLString, bool> c_Retval;
   for (uint32 u32_ItBus = 0; u32_ItBus < this->mc_CoreDefinition.c_Buses.size(); ++u32_ItBus)
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
void C_PuiSdHandlerBusLogic::SetUIBus(const uint32 ou32_Index, const C_PuiSdBus & orc_Item)
{
   if (ou32_Index < this->mc_UIBuses.size())
   {
      mc_UIBuses[ou32_Index] = orc_Item;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set OSC bus from array

   \param[in]  ou32_Index  Index
   \param[in]  orc_Item    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::SetOSCBus(const uint32 ou32_Index, const C_OSCSystemBus & orc_Item)
{
   if (ou32_Index < this->mc_CoreDefinition.c_Buses.size())
   {
      this->mc_CoreDefinition.c_Buses[ou32_Index] = orc_Item;
   }

   //signal "bus change"
   Q_EMIT this->SigBussesChanged();
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
const C_PuiSdBus * C_PuiSdHandlerBusLogic::GetUIBus(const uint32 & oru32_Index) const
{
   const C_PuiSdBus * pc_Retval;

   if (oru32_Index < this->mc_UIBuses.size())
   {
      pc_Retval = &(mc_UIBuses[oru32_Index]);
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
const C_OSCSystemBus * C_PuiSdHandlerBusLogic::GetOSCBus(const uint32 & oru32_Index) const
{
   const C_OSCSystemBus * pc_Retval;

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
bool C_PuiSdHandlerBusLogic::CheckBusNameAvailable(const C_SCLString & orc_Name,
                                                   const uint32 * const opu32_BusIndexToSkip,
                                                   std::vector<stw_scl::C_SCLString> * const opc_ExistingNames) const
{
   bool q_Retval = true;

   //Either end on error or continue if all node names are requested
   for (uint32 u32_ItBus = 0;
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
         const C_OSCSystemBus & rc_Bus = this->mc_CoreDefinition.c_Buses[u32_ItBus];
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
void C_PuiSdHandlerBusLogic::RemoveBus(const uint32 ou32_BusIndex)
{
   uint32 u32_Counter;

   this->mc_CoreDefinition.DeleteBus(ou32_BusIndex);
   this->mc_UIBuses.erase(this->mc_UIBuses.begin() + ou32_BusIndex);

   // Sync bus text elements indices
   for (u32_Counter = 0U; u32_Counter < this->c_BusTextElements.size(); ++u32_Counter)
   {
      if (this->c_BusTextElements[u32_Counter].u32_BusIndex > ou32_BusIndex)
      {
         this->c_BusTextElements[u32_Counter].u32_BusIndex = this->c_BusTextElements[u32_Counter].u32_BusIndex - 1U;
      }
   }

   //signal "bus change"
   Q_EMIT this->SigBussesChanged();
   Q_EMIT this->SigSyncBusDeleted(ou32_BusIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if there is a bus conflict

   \param[in]  ou32_BusIndex  Bus index

   \return
   true  Conflict
   false No conflict
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerBusLogic::CheckBusConflict(const uint32 ou32_BusIndex) const
{
   bool q_NameConflict;
   bool q_IdInvalid;
   static QMap<std::vector<uint32>, bool> hc_PreviousResults;

   QMap<std::vector<uint32>, bool>::const_iterator c_It;
   std::vector<uint32> c_Hashes;
   std::vector<uint32> c_NodeIndexes;
   std::vector<uint32> c_InterfaceIndexes;
   bool q_Retval;

   //get all required hashes
   this->mc_CoreDefinition.GetNodeIndexesOfBus(ou32_BusIndex, c_NodeIndexes, c_InterfaceIndexes);
   //Bus hash
   c_Hashes.push_back(this->m_GetHashBus(ou32_BusIndex));
   //Nodes hashes
   for (uint32 u32_ItNode = 0UL; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
   {
      c_Hashes.push_back(this->m_GetHashNode(c_NodeIndexes[u32_ItNode]));
   }

   //Lookup if there is a result for this hash
   c_It = hc_PreviousResults.find(c_Hashes);
   if (c_It == hc_PreviousResults.end())
   {
      bool q_NameEmpty;

      std::vector<QString> c_InvalidNodesForBitRate;
      std::vector<C_OSCCanProtocol::E_Type> c_InvalidProtocols;

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
sint32 C_PuiSdHandlerBusLogic::CheckBusConflictDetailed(const uint32 ou32_BusIndex, bool * const opq_NameConflict,
                                                        bool * const opq_NameEmpty, bool * const opq_IdInvalid,
                                                        std::vector<QString> * const opc_InvalidNodesForBitRate,
                                                        std::vector<C_OSCCanProtocol::E_Type> * const opc_InvalidProtocols)
const
{
   sint32 s32_Retval = C_NO_ERR;

   //Check bus errors
   if (this->mc_CoreDefinition.CheckErrorBus(ou32_BusIndex, opq_NameConflict, opq_NameEmpty, opq_IdInvalid,
                                             NULL) == C_NO_ERR)
   {
      // Check current bitrate
      if (opc_InvalidNodesForBitRate != NULL)
      {
         const C_OSCSystemBus * const pc_CheckedBus = this->GetOSCBus(ou32_BusIndex);

         if ((pc_CheckedBus != NULL) &&
             (pc_CheckedBus->e_Type == C_OSCSystemBus::eCAN)) // Only relevant for CAN
         {
            const uint32 u32_CurrentBitrate = static_cast<uint32>(pc_CheckedBus->u64_BitRate / 1000ULL);
            std::vector<uint32> c_ConnectedNodes;
            std::vector<uint32> c_ConnectedInterfaces;
            uint32 u32_NodeCounter;

            this->mc_CoreDefinition.GetNodeIndexesOfBus(ou32_BusIndex, c_ConnectedNodes, c_ConnectedInterfaces);

            // Every node must support the CAN bitrate
            for (u32_NodeCounter = 0U; u32_NodeCounter < c_ConnectedNodes.size(); ++u32_NodeCounter)
            {
               const C_OSCNode * const pc_Node = this->GetOSCNodeConst(c_ConnectedNodes[u32_NodeCounter]);

               if (pc_Node != NULL)
               {
                  uint32 u32_BitrateCounter;
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
                     opc_InvalidNodesForBitRate->push_back(pc_Node->c_Properties.c_Name.c_str());
                  }
               }
            }
         }
      }

      //Check message errors
      if (opc_InvalidProtocols != NULL)
      {
         for (uint8 u8_ItToggle = 0; u8_ItToggle < 3; ++u8_ItToggle)
         {
            bool q_MessageNameConflict;
            bool q_MessageIdInvalid;
            bool q_MessagesHaveNoTx;
            bool q_DelayTimeInvalid;
            bool q_MessageSignalInvalid;
            C_OSCCanProtocol::E_Type e_Type;
            uint32 u32_CANMessageValidSignalsDLCOffset = 0UL;
            if (u8_ItToggle == 0)
            {
               e_Type = C_OSCCanProtocol::eLAYER2;
            }
            else if (u8_ItToggle == 1)
            {
               e_Type = C_OSCCanProtocol::eECES;
               u32_CANMessageValidSignalsDLCOffset = 2UL;
            }
            else
            {
               e_Type = C_OSCCanProtocol::eCAN_OPEN_SAFETY;
            }
            {
               C_PuiSdNodeCanMessageSyncManager c_SyncMan;

               c_SyncMan.Init(ou32_BusIndex, e_Type);

               c_SyncMan.CheckErrorBus(&q_MessageNameConflict, &q_MessageIdInvalid, &q_MessagesHaveNoTx,
                                       &q_DelayTimeInvalid,
                                       &q_MessageSignalInvalid, u32_CANMessageValidSignalsDLCOffset);
               if (((((q_MessageNameConflict == false) && (q_MessageIdInvalid == false)) &&
                     (q_DelayTimeInvalid == false)) && (q_MessageSignalInvalid == false)) &&
                   (q_MessagesHaveNoTx == false))
               {
                  //No error
               }
               else
               {
                  opc_InvalidProtocols->push_back(e_Type);
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
stw_types::uint32 C_PuiSdHandlerBusLogic::GetOSCBusesSize(void) const
{
   return this->mc_CoreDefinition.c_Buses.size();
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
void C_PuiSdHandlerBusLogic::AddConnection(const uint32 ou32_NodeIndex, const uint8 ou8_InterfaceNumber,
                                           const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties,
                                           const uint32 ou32_BusIndex)
{
   const C_OSCSystemBus * const pc_Bus = this->GetOSCBus(ou32_BusIndex);
   const std::vector<uint32> c_Indices = this->GetAllNodeGroupIndicesUsingNodeIndex(ou32_NodeIndex);

   tgl_assert(c_Indices.size() == orc_Properties.size());
   if (c_Indices.size() == orc_Properties.size())
   {
      for (uint32 u32_ItIndex = 0UL; u32_ItIndex < c_Indices.size(); ++u32_ItIndex)
      {
         const uint32 u32_CurIndex = c_Indices[u32_ItIndex];
         if ((u32_CurIndex < this->mc_CoreDefinition.c_Nodes.size()) && (pc_Bus != NULL))
         {
            C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_CurIndex];
            const C_OSCNodeComInterfaceSettings * const pc_CurComInterface = rc_Node.c_Properties.GetComInterface(
               pc_Bus->e_Type, ou8_InterfaceNumber);
            if (pc_CurComInterface != NULL)
            {
               if (this->mc_CoreDefinition.AddConnection(u32_CurIndex, ou32_BusIndex,
                                                         pc_CurComInterface->u8_InterfaceNumber) == C_NO_ERR)
               {
                  //Adapt node ID
                  C_OSCNodeComInterfaceSettings c_CurComInterface = *pc_CurComInterface;
                  //Adapt IP address
                  c_CurComInterface.u8_NodeID = orc_Properties[u32_ItIndex].u8_NodeId;

                  tgl_assert(orc_Properties[u32_ItIndex].c_IP.size() == 4UL);
                  for (uint32 u32_It = 0UL; u32_It < orc_Properties[u32_ItIndex].c_IP.size(); ++u32_It)
                  {
                     c_CurComInterface.c_Ip.au8_IpAddress[u32_It] = orc_Properties[u32_ItIndex].c_IP[u32_It];
                  }

                  rc_Node.c_Properties.SetComInterface(c_CurComInterface);

                  C_PuiSdNodeConnection c_NewConnection;

                  //Add connection ID
                  c_NewConnection.c_ConnectionID.u8_InterfaceNumber = ou8_InterfaceNumber;
                  c_NewConnection.c_ConnectionID.e_InterfaceType = pc_Bus->e_Type;

                  if (u32_CurIndex < this->mc_UINodes.size())
                  {
                     //Also add corresponding UI element:
                     this->mc_UINodes[u32_CurIndex].c_UIBusConnections.push_back(c_NewConnection);
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove connection

   \param[in]  ou32_NodeIndex    Connector node index (0 -> first node)
   \param[in]  orc_ID            Connector ID encoding interface type and index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::RemoveConnection(const uint32 ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_ID)
{
   const std::vector<uint32> c_Indices = this->GetAllNodeGroupIndicesUsingNodeIndex(ou32_NodeIndex);

   //Check size & consistency
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());

   for (uint32 u32_ItIndex = 0UL; u32_ItIndex < c_Indices.size(); ++u32_ItIndex)
   {
      const uint32 u32_CurIndex = c_Indices[u32_ItIndex];
      if (u32_CurIndex < this->mc_CoreDefinition.c_Nodes.size())
      {
         C_PuiSdNode & rc_UINode = this->mc_UINodes[u32_CurIndex];
         C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[u32_CurIndex];

         rc_OSCNode.c_Properties.DisconnectComInterface(orc_ID.e_InterfaceType, orc_ID.u8_InterfaceNumber);
         rc_UINode.DeleteConnection(orc_ID);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change connection

   \param[in]  ou32_NodeIndex    Index of node this interface is part of
   \param[in]  orc_ID            Previous interface ID
   \param[in]  ou8_NewInterface  New interface number
   \param[in]  orc_Properties    Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::ChangeConnection(const uint32 ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_ID,
                                              const uint8 ou8_NewInterface,
                                              const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
{
   C_PuiSdNodeConnectionId c_Tmp;

   c_Tmp.e_InterfaceType = orc_ID.e_InterfaceType;
   c_Tmp.u8_InterfaceNumber = ou8_NewInterface;

   ChangeCompleteConnection(ou32_NodeIndex, orc_ID, c_Tmp, orc_Properties);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change complete connection

   \param[in]  ou32_NodeIndex    Index of node this interface is part of
   \param[in]  orc_PrevID        Previous interface ID
   \param[in]  orc_NewID         New interface ID
   \param[in]  orc_Properties    Properties
   \param[in]  oru32_BusIndex    Bus index to use instead of last used one
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerBusLogic::ChangeCompleteConnection(const uint32 ou32_NodeIndex,
                                                      const C_PuiSdNodeConnectionId & orc_PrevID,
                                                      const C_PuiSdNodeConnectionId & orc_NewID,
                                                      const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties,
                                                      const uint32 & oru32_BusIndex)
{
   //Copy is necessary as the value changes during function call
   const C_PuiSdNodeConnectionId c_PrevIDCopy = orc_PrevID;
   const std::vector<uint32> c_Indices = this->GetAllNodeGroupIndicesUsingNodeIndex(ou32_NodeIndex);

   //Check size & consistency
   tgl_assert(c_Indices.size() == orc_Properties.size());
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());

   for (uint32 u32_ItIndex = 0UL; u32_ItIndex < c_Indices.size(); ++u32_ItIndex)
   {
      const uint32 u32_CurIndex = c_Indices[u32_ItIndex];
      if (u32_CurIndex < this->mc_CoreDefinition.c_Nodes.size())
      {
         C_PuiSdNode & rc_UINode = this->mc_UINodes[u32_CurIndex];
         C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[u32_CurIndex];
         uint32 u32_LastBus = 0;
         //Unregister
         for (uint32 u32_ItComInterface = 0; u32_ItComInterface < rc_OSCNode.c_Properties.c_ComInterfaces.size();
              ++u32_ItComInterface)
         {
            C_OSCNodeComInterfaceSettings & rc_ComInterface =
               rc_OSCNode.c_Properties.c_ComInterfaces[u32_ItComInterface];
            if ((rc_ComInterface.e_InterfaceType == c_PrevIDCopy.e_InterfaceType) &&
                (rc_ComInterface.u8_InterfaceNumber == c_PrevIDCopy.u8_InterfaceNumber))
            {
               u32_LastBus = rc_ComInterface.u32_BusIndex;
               rc_ComInterface.RemoveConnection();
            }
         }
         //Register
         for (uint32 u32_ItComInterface = 0; u32_ItComInterface < rc_OSCNode.c_Properties.c_ComInterfaces.size();
              ++u32_ItComInterface)
         {
            C_OSCNodeComInterfaceSettings & rc_ComInterface =
               rc_OSCNode.c_Properties.c_ComInterfaces[u32_ItComInterface];
            if ((rc_ComInterface.e_InterfaceType == orc_NewID.e_InterfaceType) &&
                (rc_ComInterface.u8_InterfaceNumber == orc_NewID.u8_InterfaceNumber))
            {
               uint32 u32_BusIndex;
               if (oru32_BusIndex >= this->mc_CoreDefinition.c_Buses.size())
               {
                  u32_BusIndex = u32_LastBus;
               }
               else
               {
                  u32_BusIndex = oru32_BusIndex;
               }
               rc_ComInterface.AddConnection(u32_BusIndex);
               rc_ComInterface.u8_NodeID = orc_Properties[u32_ItIndex].u8_NodeId;

               tgl_assert(orc_Properties[u32_ItIndex].c_IP.size() == 4UL);
               for (uint32 u32_It = 0UL; u32_It < orc_Properties[u32_ItIndex].c_IP.size(); ++u32_It)
               {
                  rc_ComInterface.c_Ip.au8_IpAddress[u32_It] = orc_Properties[u32_ItIndex].c_IP[u32_It];
               }
            }
         }

         //UI
         for (uint32 u32_ItBusConn = 0; u32_ItBusConn < rc_UINode.c_UIBusConnections.size(); ++u32_ItBusConn)
         {
            C_PuiSdNodeConnection & rc_BusConn = rc_UINode.c_UIBusConnections[u32_ItBusConn];
            if (rc_BusConn.c_ConnectionID == c_PrevIDCopy)
            {
               rc_BusConn.c_ConnectionID = orc_NewID;
            }
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
void C_PuiSdHandlerBusLogic::SetUINodeConnections(const uint32 ou32_NodeIndex,
                                                  const std::vector<C_PuiSdNodeConnection> & orc_Connections)
{
   const std::vector<uint32> c_Indices = this->GetAllNodeGroupIndicesUsingNodeIndex(ou32_NodeIndex);

   for (uint32 u32_ItIndex = 0UL; u32_ItIndex < c_Indices.size(); ++u32_ItIndex)
   {
      const uint32 u32_CurIndex = c_Indices[u32_ItIndex];
      if (u32_CurIndex < this->mc_UINodes.size())
      {
         C_PuiSdNode & rc_UINode = this->mc_UINodes[u32_CurIndex];
         rc_UINode.c_UIBusConnections = orc_Connections;
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
void C_PuiSdHandlerBusLogic::SetUINodeConnectionId(const uint32 ou32_NodeIndex, const uint32 ou32_ConnectionIndex,
                                                   const C_PuiSdNodeConnectionId & orc_Id)
{
   const std::vector<uint32> c_Indices = this->GetAllNodeGroupIndicesUsingNodeIndex(ou32_NodeIndex);

   for (uint32 u32_ItIndex = 0UL; u32_ItIndex < c_Indices.size(); ++u32_ItIndex)
   {
      const uint32 u32_CurIndex = c_Indices[u32_ItIndex];
      if (u32_CurIndex < this->mc_UINodes.size())
      {
         C_PuiSdNode & rc_UINode = this->mc_UINodes[u32_CurIndex];
         if (ou32_ConnectionIndex < rc_UINode.c_UIBusConnections.size())
         {
            rc_UINode.c_UIBusConnections[ou32_ConnectionIndex].c_ConnectionID = orc_Id;
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
const C_OSCCanProtocol * C_PuiSdHandlerBusLogic::GetCanProtocol(const uint32 & oru32_NodeIndex,
                                                                const C_OSCCanProtocol::E_Type & ore_ComType,
                                                                const uint32 ou32_DatapoolIndex) const
{
   const C_OSCCanProtocol * pc_Retval = NULL;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      pc_Retval = rc_Node.GetCANProtocolConst(ore_ComType, ou32_DatapoolIndex);
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
const C_OSCNodeDataPool * C_PuiSdHandlerBusLogic::GetOSCCanDataPool(const uint32 & oru32_NodeIndex,
                                                                    const C_OSCCanProtocol::E_Type & ore_ComType,
                                                                    const uint32 & oru32_DatapoolIndex) const
{
   const C_OSCNodeDataPool * pc_Retval = NULL;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
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
std::vector< const C_OSCNodeDataPool *> C_PuiSdHandlerBusLogic::GetOSCCanDataPools(const uint32 & oru32_NodeIndex,
                                                                                   const C_OSCCanProtocol::E_Type & ore_ComType)
const
{
   std::vector<const C_OSCNodeDataPool *> c_Return;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
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
const C_PuiSdNodeDataPool * C_PuiSdHandlerBusLogic::GetUiCanDataPool(const uint32 & oru32_NodeIndex,
                                                                     const C_OSCCanProtocol::E_Type & ore_ComType,
                                                                     const uint32 & oru32_DatapoolIndex) const
{
   const C_PuiSdNodeDataPool * pc_Retval = NULL;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      const C_OSCCanProtocol * const pc_CanProtocol = rc_OSCNode.GetCANProtocolConst(ore_ComType, oru32_DatapoolIndex);
      if (pc_CanProtocol != NULL)
      {
         const C_PuiSdNode & rc_UINode = this->mc_UINodes[oru32_NodeIndex];
         if (pc_CanProtocol->u32_DataPoolIndex < rc_UINode.c_UIDataPools.size())
         {
            pc_Retval = &rc_UINode.c_UIDataPools[pc_CanProtocol->u32_DataPoolIndex];
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
const C_PuiSdNodeCanProtocol * C_PuiSdHandlerBusLogic::GetUiCanProtocolConst(const uint32 & oru32_NodeIndex,
                                                                             const C_OSCCanProtocol::E_Type & ore_ComType,
                                                                             const uint32 ou32_DatapoolIndex)
const
{
   const C_PuiSdNodeCanProtocol * pc_Retval = NULL;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      const C_PuiSdNode & rc_UINode = this->mc_UINodes[oru32_NodeIndex];
      tgl_assert(rc_OSCNode.c_ComProtocols.size() == rc_UINode.c_UICanProtocols.size());
      for (uint32 u32_ItCanProtocol = 0; u32_ItCanProtocol < rc_OSCNode.c_ComProtocols.size(); ++u32_ItCanProtocol)
      {
         const C_OSCCanProtocol & rc_CanProtocol = rc_OSCNode.c_ComProtocols[u32_ItCanProtocol];
         if ((rc_CanProtocol.u32_DataPoolIndex == ou32_DatapoolIndex) &&
             (rc_CanProtocol.e_Type == ore_ComType))
         {
            pc_Retval = &rc_UINode.c_UICanProtocols[u32_ItCanProtocol];
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
const C_OSCCanMessageContainer * C_PuiSdHandlerBusLogic::GetCanProtocolMessageContainer(const uint32 & oru32_NodeIndex,
                                                                                        const C_OSCCanProtocol::E_Type & ore_ComType, const uint32 & oru32_InterfaceIndex,
                                                                                        const uint32 ou32_DatapoolIndex)
const
{
   const C_OSCCanMessageContainer * pc_Retval = NULL;
   const C_OSCCanProtocol * const pc_Protocol = this->GetCanProtocol(oru32_NodeIndex, ore_ComType, ou32_DatapoolIndex);

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
std::vector<const C_OSCCanMessageContainer *> C_PuiSdHandlerBusLogic::GetCanProtocolMessageContainers(
   const uint32 & oru32_NodeIndex, const C_OSCCanProtocol::E_Type & ore_ComType,
   const uint32 & oru32_InterfaceIndex) const
{
   std::vector<const C_OSCCanMessageContainer *> c_Retval;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      std::vector<const C_OSCCanProtocol *> c_Protocols = rc_Node.GetCANProtocolsConst(ore_ComType);
      uint32 u32_Counter;

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
sint32 C_PuiSdHandlerBusLogic::GetCanProtocolType(const uint32 ou32_NodeIndex, const uint32 ou32_DatapoolIndex,
                                                  C_OSCCanProtocol::E_Type & ore_ComType) const
{
   sint32 s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const C_OSCCanProtocol * const pc_Prot = rc_Node.GetRelatedCANProtocolConst(ou32_DatapoolIndex);

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
const C_OSCCanMessage * C_PuiSdHandlerBusLogic::GetCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
const
{
   const C_OSCCanMessage * pc_Retval = NULL;
   const C_OSCCanMessageContainer * const pc_MessageContainer = this->GetCanProtocolMessageContainer(
      orc_MessageId.u32_NodeIndex,
      orc_MessageId.e_ComProtocol,
      orc_MessageId.u32_InterfaceIndex,
      orc_MessageId.u32_DatapoolIndex);

   if (pc_MessageContainer != NULL)
   {
      const std::vector<C_OSCCanMessage> & rc_Messages = pc_MessageContainer->GetMessagesConst(
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

   \param[in]   orc_MessageId          Message identification indices
   \param[out]  orc_Message            Message data
   \param[out]  orc_OSCSignalCommons   Signals data (osc common)
   \param[out]  orc_UISignalCommons    Signals data (ui common)
   \param[out]  orc_UISignals          Signals data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerBusLogic::GetCanMessageComplete(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                     C_OSCCanMessage & orc_Message,
                                                     std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons,
                                                     std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                                     std::vector<C_PuiSdNodeCanSignal> & orc_UISignals) const
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);
   const C_OSCNodeDataPoolList * const pc_OSCList = this->GetOSCCanDataPoolList(orc_MessageId.u32_NodeIndex,
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

   if ((((pc_Message != NULL) && (pc_OSCList != NULL)) && (pc_UiList != NULL)) && (pc_UiMessage != NULL))
   {
      orc_Message = *pc_Message;
      orc_UISignals = pc_UiMessage->c_Signals;
      //Copy RELEVANT signals
      orc_OSCSignalCommons.clear();
      orc_UISignalCommons.clear();
      orc_OSCSignalCommons.reserve(pc_Message->c_Signals.size());
      orc_UISignalCommons.reserve(pc_Message->c_Signals.size());
      for (uint32 u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
      {
         const C_OSCCanSignal & rc_Message = pc_Message->c_Signals[u32_ItSignal];
         if (rc_Message.u32_ComDataElementIndex < pc_OSCList->c_Elements.size())
         {
            orc_OSCSignalCommons.push_back(pc_OSCList->c_Elements[rc_Message.u32_ComDataElementIndex]);
         }
         if (rc_Message.u32_ComDataElementIndex < pc_UiList->c_DataPoolListElements.size())
         {
            orc_UISignalCommons.push_back(pc_UiList->c_DataPoolListElements[rc_Message.u32_ComDataElementIndex]);
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
const C_OSCNodeDataPoolList * C_PuiSdHandlerBusLogic::GetOSCCanDataPoolList(const uint32 & oru32_NodeIndex,
                                                                            const C_OSCCanProtocol::E_Type & ore_ComType, const uint32 & oru32_InterfaceIndex, const uint32 ou32_DatapoolIndex,
                                                                            const bool & orq_MessageIsTx) const
{
   const C_OSCNodeDataPoolList * pc_Retval = NULL;
   const C_OSCNodeDataPool * const pc_DataPool = this->GetOSCCanDataPool(oru32_NodeIndex, ore_ComType,
                                                                         ou32_DatapoolIndex);

   if (pc_DataPool != NULL)
   {
      pc_Retval = C_OSCCanProtocol::h_GetComListConst(*pc_DataPool, oru32_InterfaceIndex, orq_MessageIsTx);
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
const C_OSCCanSignal * C_PuiSdHandlerBusLogic::GetCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                            const uint32 & oru32_SignalIndex) const
{
   const C_OSCCanSignal * pc_Retval = NULL;
   const C_OSCCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);

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
   const C_OSCCanMessageIdentificationIndices & orc_MessageId)
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
   const C_OSCCanMessageIdentificationIndices & orc_MessageId, const uint32 & oru32_SignalIndex) const
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
const C_OSCNodeDataPoolListElement * C_PuiSdHandlerBusLogic::GetOSCCanDataPoolListElement(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId, const uint32 & oru32_SignalIndex)
const
{
   const C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   const C_OSCCanSignal * const pc_Signal = this->GetCanSignal(orc_MessageId, oru32_SignalIndex);

   if (pc_Signal != NULL)
   {
      const C_OSCNodeDataPoolList * const pc_List = this->GetOSCCanDataPoolList(orc_MessageId.u32_NodeIndex,
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
   const C_OSCCanMessageIdentificationIndices & orc_MessageId, const uint32 & oru32_SignalIndex)
const
{
   const C_PuiSdNodeDataPoolListElement * pc_Retval = NULL;
   const C_OSCCanSignal * const pc_Signal = this->GetCanSignal(orc_MessageId, oru32_SignalIndex);

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
const C_PuiSdNodeConnection * C_PuiSdHandlerBusLogic::GetUiConnection(const uint32 ou32_NodeIndex,
                                                                      const uint32 ou32_InterfaceIndex,
                                                                      const C_OSCSystemBus::E_Type oe_Type) const
{
   const C_PuiSdNodeConnection * pc_Retval = NULL;
   const C_PuiSdNode * const pc_Node = this->GetUINode(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      for (uint32 u32_ItConnection = 0; u32_ItConnection < pc_Node->c_UIBusConnections.size(); ++u32_ItConnection)
      {
         const C_PuiSdNodeConnection & rc_Connection = pc_Node->c_UIBusConnections[u32_ItConnection];
         if ((static_cast<uint32>(rc_Connection.c_ConnectionID.u8_InterfaceNumber) == ou32_InterfaceIndex) &&
             (rc_Connection.c_ConnectionID.e_InterfaceType == oe_Type))
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
   \param[out]  orc_OSCSignalCommon    Signal data (osc common)
   \param[out]  orc_UISignalCommon     Signal data (ui common)
   \param[out]  orc_UISignal           Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerBusLogic::GetCanSignalComplete(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                    const uint32 & oru32_SignalIndex, C_OSCCanSignal & orc_Signal,
                                                    C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                                    C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                                    C_PuiSdNodeCanSignal & orc_UISignal) const
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCCanSignal * const pc_Signal = this->GetCanSignal(orc_MessageId, oru32_SignalIndex);
   const C_OSCNodeDataPoolListElement * const pc_OSCSignalCommon = this->GetOSCCanDataPoolListElement(orc_MessageId,
                                                                                                      oru32_SignalIndex);
   const C_PuiSdNodeDataPoolListElement * const pc_UiSignalCommon = this->GetUiCanDataPoolListElement(orc_MessageId,
                                                                                                      oru32_SignalIndex);
   const C_PuiSdNodeCanSignal * const pc_UiSignal = GetUiCanSignal(orc_MessageId, oru32_SignalIndex);

   if ((((pc_Signal != NULL) && (pc_OSCSignalCommon != NULL)) && (pc_UiSignalCommon != NULL)) && (pc_UiSignal != NULL))
   {
      orc_Signal = *pc_Signal;
      orc_OSCSignalCommon = *pc_OSCSignalCommon;
      orc_UISignalCommon = *pc_UiSignalCommon;
      orc_UISignal = *pc_UiSignal;
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
void C_PuiSdHandlerBusLogic::SetCanProtocolMessageContainerConnected(const uint32 ou32_NodeIndex,
                                                                     const C_OSCCanProtocol::E_Type oe_ComType,
                                                                     const uint32 ou32_InterfaceIndex,
                                                                     const bool oq_Value)
{
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      // get necessary datapool
      std::vector<C_OSCCanProtocol *> c_Protocols = rc_Node.GetCANProtocols(oe_ComType);
      uint32 u32_Counter;

      for (u32_Counter = 0; u32_Counter < c_Protocols.size(); ++u32_Counter)
      {
         C_OSCCanProtocol * const pc_Protocol = c_Protocols[u32_Counter];

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
sint32 C_PuiSdHandlerBusLogic::SetCanMessageProperties(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                       const bool & orq_NewMessageIsTx,
                                                       const C_OSCCanMessage & orc_Message)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCCanMessageContainer * const pc_MessageContainer = this->GetCanProtocolMessageContainer(
         orc_MessageId.u32_NodeIndex,
         orc_MessageId.e_ComProtocol,
         orc_MessageId.u32_InterfaceIndex,
         orc_MessageId.u32_DatapoolIndex);
      const C_OSCCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);
      const C_OSCNodeDataPoolList * const pc_List = this->GetOSCCanDataPoolList(orc_MessageId.u32_NodeIndex,
                                                                                orc_MessageId.e_ComProtocol,
                                                                                orc_MessageId.u32_InterfaceIndex,
                                                                                orc_MessageId.u32_DatapoolIndex,
                                                                                orc_MessageId.q_MessageIsTx);
      if (((pc_List != NULL) && (pc_MessageContainer != NULL)) && (pc_Message != NULL))
      {
         uint32 u32_SignalStartIndex = pc_MessageContainer->GetMessageSignalDataStartIndex(
            orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex);
         if ((u32_SignalStartIndex + pc_Message->c_Signals.size()) <= pc_List->c_Elements.size())
         {
            std::vector<C_OSCNodeDataPoolListElement> c_Signals;
            C_OSCCanMessage c_TmpMsg = orc_Message;
            C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
            //Copy signals
            c_TmpMsg.c_Signals = pc_Message->c_Signals;
            c_Signals.reserve(pc_Message->c_Signals.size());
            for (uint32 u32_ItSignals = 0; u32_ItSignals < pc_Message->c_Signals.size(); ++u32_ItSignals)
            {
               const uint32 u32_Index = u32_SignalStartIndex + u32_ItSignals;
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
               const C_OSCCanMessageContainer * const pc_UpdatedMessageContainer = this->GetCanProtocolMessageContainer(
                  orc_MessageId.u32_NodeIndex,
                  orc_MessageId.e_ComProtocol,
                  orc_MessageId.u32_InterfaceIndex,
                  orc_MessageId.u32_DatapoolIndex);
               const C_PuiSdNodeDataPoolList * const pc_UiDataPoolList = this->m_GetUiCanDataPoolList(
                  orc_MessageId.u32_NodeIndex, orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                  orc_MessageId.u32_DatapoolIndex, orc_MessageId.q_MessageIsTx);
               if (((pc_UiMessage != NULL) && (pc_UpdatedMessageContainer != NULL)) && (pc_UiDataPoolList != NULL))
               {
                  const std::vector<C_OSCCanMessage> & rc_Messages = pc_UpdatedMessageContainer->GetMessagesConst(
                     orq_NewMessageIsTx);
                  const C_PuiSdNodeCanMessage c_UiMessageCopy = *pc_UiMessage;
                  std::vector<C_PuiSdNodeDataPoolListElement> c_UISignalCommons;
                  const C_OSCCanMessageIdentificationIndices c_NewId(orc_MessageId.u32_NodeIndex,
                                                                     orc_MessageId.e_ComProtocol,
                                                                     orc_MessageId.u32_InterfaceIndex,
                                                                     orc_MessageId.u32_DatapoolIndex,
                                                                     orq_NewMessageIsTx,
                                                                     rc_Messages.size() - 1UL);

                  u32_SignalStartIndex = pc_UpdatedMessageContainer->GetMessageSignalDataStartIndex(
                     orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex);

                  for (uint32 u32_ItSignal = 0; u32_ItSignal < c_UiMessageCopy.c_Signals.size(); ++u32_ItSignal)
                  {
                     const uint32 u32_TargetIndex = u32_SignalStartIndex + u32_ItSignal;
                     tgl_assert(u32_TargetIndex < pc_UiDataPoolList->c_DataPoolListElements.size());
                     if (u32_TargetIndex < pc_UiDataPoolList->c_DataPoolListElements.size())
                     {
                        c_UISignalCommons.push_back(pc_UiDataPoolList->c_DataPoolListElements[u32_TargetIndex]);
                     }
                  }
                  //Delete
                  s32_Retval = this->m_DeleteUiCanMessage(orc_MessageId);
                  if (s32_Retval == C_NO_ERR)
                  {
                     //Add
                     s32_Retval = this->m_InsertUiCanMessage(c_NewId, c_UISignalCommons, c_UiMessageCopy);
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
sint32 C_PuiSdHandlerBusLogic::SetUiCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                               const C_PuiSdNodeCanMessage & orc_Message)
{
   sint32 s32_Retval = C_NO_ERR;

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
   \param[in]  orc_OSCSignal        Signal data (osc)
   \param[in]  orc_OSCSignalCommon  Signal data (osc common)
   \param[in]  orc_UISignalCommon   Signal data (ui common)
   \param[in]  orc_UISignal         Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerBusLogic::SetCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                            const uint32 & oru32_SignalIndex, const C_OSCCanSignal & orc_OSCSignal,
                                            const C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                            const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                            const C_PuiSdNodeCanSignal & orc_UISignal)
{
   sint32 s32_Retval = C_NO_ERR;

   //Position
   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      const C_OSCCanProtocol * const pc_Protocol = rc_OSCNode.GetCANProtocolConst(orc_MessageId.e_ComProtocol,
                                                                                  orc_MessageId.u32_DatapoolIndex);
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol,
                                                                              orc_MessageId.u32_DatapoolIndex);
      C_OSCNodeDataPoolListElementId c_Id;
      s32_Retval = rc_OSCNode.SetSignal(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                                        orc_MessageId.u32_DatapoolIndex,
                                        orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex,
                                        oru32_SignalIndex, orc_OSCSignal, orc_OSCSignalCommon);
      //Signal changed signal to the sync engine
      if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, oru32_SignalIndex, c_Id) == C_NO_ERR)
      {
         Q_EMIT (this->SigSyncNodeDataPoolListElementChanged(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                             c_Id.u32_ListIndex, c_Id.u32_ElementIndex,
                                                             orc_OSCSignalCommon.GetType(),
                                                             orc_OSCSignalCommon.GetArray(),
                                                             orc_OSCSignalCommon.GetArraySize(), false));
      }
      //Ui data pool part
      if (pc_Protocol != NULL)
      {
         if (orc_MessageId.u32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
         {
            const C_OSCCanMessageContainer & rc_MessageContainer =
               pc_Protocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
            const std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessagesConst(
               orc_MessageId.q_MessageIsTx);
            if (orc_MessageId.u32_MessageIndex < rc_Messages.size())
            {
               const C_OSCCanMessage & rc_Message = rc_Messages[orc_MessageId.u32_MessageIndex];
               if (oru32_SignalIndex < rc_Message.c_Signals.size())
               {
                  const C_OSCNodeDataPool * const pc_OSCDataPool = rc_OSCNode.GetComDataPoolConst(
                     orc_MessageId.e_ComProtocol, orc_MessageId.u32_DatapoolIndex);

                  if (pc_OSCDataPool != NULL)
                  {
                     C_PuiSdNode & rc_UINode = this->mc_UINodes[orc_MessageId.u32_NodeIndex];
                     if (pc_Protocol->u32_DataPoolIndex < rc_UINode.c_UIDataPools.size())
                     {
                        uint32 u32_ListIndex;
                        if (C_OSCCanProtocol::h_GetComListIndex(*pc_OSCDataPool, orc_MessageId.u32_InterfaceIndex,
                                                                orc_MessageId.q_MessageIsTx,
                                                                u32_ListIndex) == C_NO_ERR)
                        {
                           C_PuiSdNodeDataPool & rc_UIDataPool =
                              rc_UINode.c_UIDataPools[pc_Protocol->u32_DataPoolIndex];
                           if (u32_ListIndex < rc_UIDataPool.c_DataPoolLists.size())
                           {
                              const C_OSCCanSignal & rc_Signal = rc_Message.c_Signals[oru32_SignalIndex];
                              C_PuiSdNodeDataPoolList & rc_UiList = rc_UIDataPool.c_DataPoolLists[u32_ListIndex];
                              if (rc_Signal.u32_ComDataElementIndex < rc_UiList.c_DataPoolListElements.size())
                              {
                                 rc_UiList.c_DataPoolListElements[rc_Signal.u32_ComDataElementIndex] =
                                    orc_UISignalCommon;
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
                  rc_Message.c_Signals[oru32_SignalIndex] = orc_UISignal;
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
sint32 C_PuiSdHandlerBusLogic::SetCanSignalMUXValue(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                    const uint32 & oru32_SignalIndex,
                                                    const stw_types::uint16 ou16_MultiplexValue)
{
   sint32 s32_Retval = C_NO_ERR;

   //Position
   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      s32_Retval = rc_OSCNode.SetSignalMUXValue(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
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
/*! \brief   Set can signal position part

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
   \param[in]  orc_OSCSignal        Signal data (osc)
   \param[in]  orc_UISignal         Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerBusLogic::SetCanSignalPosition(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                    const uint32 & oru32_SignalIndex,
                                                    const C_OSCCanSignal & orc_OSCSignal,
                                                    const C_PuiSdNodeCanSignal & orc_UISignal)
{
   sint32 s32_Retval = C_NO_ERR;

   //Position
   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol,
                                                                              orc_MessageId.u32_DatapoolIndex);
      s32_Retval = rc_OSCNode.SetSignalPosition(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                                                orc_MessageId.u32_DatapoolIndex,
                                                orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex,
                                                oru32_SignalIndex, orc_OSCSignal);
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
                  rc_Message.c_Signals[oru32_SignalIndex] = orc_UISignal;
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
/*! \brief   Add can message

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  ore_ComType                      Can protocol type
   \param[in]  oru32_InterfaceIndex             Node interface index
   \param[in]  ou32_DatapoolIndex               Datapool index
   \param[in]  orq_MessageIsTx                  Flag if message is tx type (else rx type assumed)
   \param[in]  orc_Message                      Message data
   \param[in]  orc_OSCSignalCommons             Signals data (osc common)
   \param[in]  orc_UISignalCommons              Signals data (ui common)
   \param[in]  orc_UIMessage                    Message data (ui)
   \param[in]  orq_AutomatedPropertiesAdaption  Optional parameter to allow or disable automated properties adaption

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerBusLogic::AddCanMessage(const uint32 & oru32_NodeIndex,
                                             const C_OSCCanProtocol::E_Type & ore_ComType,
                                             const uint32 & oru32_InterfaceIndex, const uint32 ou32_DatapoolIndex,
                                             const bool & orq_MessageIsTx, const C_OSCCanMessage & orc_Message,
                                             const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons,
                                             const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                             const C_PuiSdNodeCanMessage & orc_UIMessage,
                                             const bool & orq_AutomatedPropertiesAdaption)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCCanMessageContainer * const pc_MessageContainer = this->GetCanProtocolMessageContainer(oru32_NodeIndex,
                                                                                                     ore_ComType,
                                                                                                     oru32_InterfaceIndex,
                                                                                                     ou32_DatapoolIndex);

   if (pc_MessageContainer != NULL)
   {
      const std::vector<C_OSCCanMessage> & rc_Messages = pc_MessageContainer->GetMessagesConst(orq_MessageIsTx);
      const uint32 u32_MessageIndex = rc_Messages.size();
      const C_OSCCanMessageIdentificationIndices c_MessageId(oru32_NodeIndex, ore_ComType, oru32_InterfaceIndex,
                                                             ou32_DatapoolIndex,
                                                             orq_MessageIsTx,
                                                             u32_MessageIndex);
      s32_Retval = this->InsertCanMessage(c_MessageId, orc_Message, orc_OSCSignalCommons, orc_UISignalCommons,
                                          orc_UIMessage, orq_AutomatedPropertiesAdaption);
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
   \param[in]  orc_OSCSignalCommons             Signals data (osc common)
   \param[in]  orc_UISignalCommons              Signals data (ui common)
   \param[in]  orc_UIMessage                    Message data (ui)
   \param[in]  orq_AutomatedPropertiesAdaption  Optional parameter to allow or disable automated properties adaption

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerBusLogic::InsertCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                const C_OSCCanMessage & orc_Message,
                                                const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons,
                                                const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                                const C_PuiSdNodeCanMessage & orc_UIMessage,
                                                const bool & orq_AutomatedPropertiesAdaption)
{
   sint32 s32_Retval = C_NO_ERR;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      C_OSCCanMessage c_AdaptedMessage = orc_Message;
      if (orq_AutomatedPropertiesAdaption == true)
      {
         //Get unique name
         c_AdaptedMessage.c_Name = C_Uti::h_GetUniqueName(
            this->m_GetExistingMessageNames(orc_MessageId.u32_NodeIndex, orc_MessageId.u32_InterfaceIndex),
            orc_Message.c_Name);
      }
      s32_Retval = rc_OSCNode.InsertMessage(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                                            orc_MessageId.u32_DatapoolIndex,
                                            orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex,
                                            c_AdaptedMessage,
                                            orc_OSCSignalCommons);
      //Signal each new signal to the sync engine (ascending order is important)
      for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
      {
         C_OSCNodeDataPoolListElementId c_Id;
         if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, u32_ItSig, c_Id) == C_NO_ERR)
         {
            Q_EMIT (this->SigSyncNodeDataPoolListElementAdded(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                              c_Id.u32_ListIndex, c_Id.u32_ElementIndex));
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Ui part
         s32_Retval = m_InsertUiCanMessage(orc_MessageId, orc_UISignalCommons, orc_UIMessage);
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
sint32 C_PuiSdHandlerBusLogic::DeleteCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   sint32 s32_Retval = C_NO_ERR;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      if (pc_Message != NULL)
      {
         //Signal each deleted signal to the sync engine (descending order is important, do this before actually
         // deleting anything)
         for (uint32 u32_ItSig = pc_Message->c_Signals.size(); u32_ItSig > 0UL; --u32_ItSig)
         {
            C_OSCNodeDataPoolListElementId c_Id;
            if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, u32_ItSig - 1UL, c_Id) == C_NO_ERR)
            {
               Q_EMIT (this->SigSyncNodeDataPoolListElementAboutToBeDeleted(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                                            c_Id.u32_ListIndex, c_Id.u32_ElementIndex));
            }
         }
      }
      //Ui part
      s32_Retval = m_DeleteUiCanMessage(orc_MessageId);
      //After deleting signals delete other parts
      tgl_assert(rc_OSCNode.DeleteMessage(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
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
   \param[in]  orc_OSCSignalCommon  Signal data (osc common)
   \param[in]  orc_UISignalCommon   Signal data (ui common)
   \param[in]  orc_UISignal         Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerBusLogic::AddCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                            const C_OSCCanSignal & orc_Signal,
                                            const C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                            const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                            const C_PuiSdNodeCanSignal & orc_UISignal)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);

   if (pc_Message != NULL)
   {
      const uint32 u32_SignalIndex = pc_Message->c_Signals.size();
      s32_Retval = this->InsertCanSignal(orc_MessageId, u32_SignalIndex, orc_Signal, orc_OSCSignalCommon,
                                         orc_UISignalCommon, orc_UISignal);
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
   \param[in]  orc_OSCSignalCommon  Signal data (osc common)
   \param[in]  orc_UISignalCommon   Signal data (ui common)
   \param[in]  orc_UISignal         Signal data (ui)

   \return
   C_NO_ERR Operation success
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerBusLogic::InsertCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                               const uint32 & oru32_SignalIndex, const C_OSCCanSignal & orc_Signal,
                                               const C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                               const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                               const C_PuiSdNodeCanSignal & orc_UISignal)
{
   const sint32 s32_RETVAL = C_NO_ERR;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      const C_OSCCanProtocol * const pc_Protocol = rc_OSCNode.GetCANProtocolConst(orc_MessageId.e_ComProtocol,
                                                                                  orc_MessageId.u32_DatapoolIndex);
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol,
                                                                              orc_MessageId.u32_DatapoolIndex);
      C_OSCNodeDataPoolListElement c_OSCAdaptedSignalCommon = orc_OSCSignalCommon;
      C_OSCNodeDataPoolListElementId c_Id;

      //Get unique name
      c_OSCAdaptedSignalCommon.c_Name = C_Uti::h_GetUniqueName(
         this->m_GetExistingSignalNames(orc_MessageId), orc_OSCSignalCommon.c_Name);
      rc_OSCNode.InsertSignal(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                              orc_MessageId.u32_DatapoolIndex,
                              orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex, oru32_SignalIndex,
                              orc_Signal,
                              c_OSCAdaptedSignalCommon);

      //Signal new signal to the sync engine
      if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, oru32_SignalIndex, c_Id) == C_NO_ERR)
      {
         Q_EMIT (this->SigSyncNodeDataPoolListElementAdded(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                           c_Id.u32_ListIndex, c_Id.u32_ElementIndex));
      }
      //Ui part
      if (pc_Protocol != NULL)
      {
         if (orc_MessageId.u32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
         {
            const C_OSCCanMessageContainer & rc_MessageContainer =
               pc_Protocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
            const uint32 u32_SignalIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
               orc_MessageId.q_MessageIsTx,
               orc_MessageId.u32_MessageIndex);
            C_PuiSdNode & rc_UINode = this->mc_UINodes[orc_MessageId.u32_NodeIndex];
            if (pc_Protocol->u32_DataPoolIndex < rc_UINode.c_UIDataPools.size())
            {
               const C_OSCNodeDataPool * const pc_OSCDataPool = rc_OSCNode.GetComDataPoolConst(
                  orc_MessageId.e_ComProtocol,
                  orc_MessageId.u32_DatapoolIndex);

               if (pc_OSCDataPool != NULL)
               {
                  uint32 u32_ListIndex;
                  if (C_OSCCanProtocol::h_GetComListIndex(*pc_OSCDataPool, orc_MessageId.u32_InterfaceIndex,
                                                          orc_MessageId.q_MessageIsTx,
                                                          u32_ListIndex) == C_NO_ERR)
                  {
                     C_PuiSdNodeDataPool & rc_UIDataPool = rc_UINode.c_UIDataPools[pc_Protocol->u32_DataPoolIndex];
                     if (u32_ListIndex < rc_UIDataPool.c_DataPoolLists.size())
                     {
                        C_PuiSdNodeDataPoolList & rc_UiList = rc_UIDataPool.c_DataPoolLists[u32_ListIndex];
                        if ((u32_SignalIndex + oru32_SignalIndex) <= rc_UiList.c_DataPoolListElements.size())
                        {
                           rc_UiList.c_DataPoolListElements.insert(
                              rc_UiList.c_DataPoolListElements.begin() + u32_SignalIndex + oru32_SignalIndex,
                              orc_UISignalCommon);
                        }
                     }
                  }
               }
            }
         }
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
                  rc_Message.c_Signals.insert(rc_Message.c_Signals.begin() + oru32_SignalIndex, orc_UISignal);
               }
            }
         }
      }
   }
   return s32_RETVAL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete can signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index

   \return
   C_NO_ERR Operation success
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerBusLogic::DeleteCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                               const uint32 & oru32_SignalIndex)
{
   const sint32 s32_RETVAL = C_NO_ERR;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      const C_OSCCanProtocol * const pc_Protocol = rc_OSCNode.GetCANProtocolConst(orc_MessageId.e_ComProtocol,
                                                                                  orc_MessageId.u32_DatapoolIndex);
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol,
                                                                              orc_MessageId.u32_DatapoolIndex);
      C_OSCNodeDataPoolListElementId c_Id;

      //Signal deleted signal to the sync engine
      if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, oru32_SignalIndex, c_Id) == C_NO_ERR)
      {
         Q_EMIT this->SigSyncNodeDataPoolListElementAboutToBeDeleted(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                                     c_Id.u32_ListIndex, c_Id.u32_ElementIndex);
      }

      rc_OSCNode.DeleteSignal(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                              orc_MessageId.u32_DatapoolIndex,
                              orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex, oru32_SignalIndex);
      //Ui part
      if (pc_Protocol != NULL)
      {
         if (orc_MessageId.u32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
         {
            const C_OSCCanMessageContainer & rc_MessageContainer =
               pc_Protocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
            const uint32 u32_SignalIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
               orc_MessageId.q_MessageIsTx,
               orc_MessageId.u32_MessageIndex);
            C_PuiSdNode & rc_UINode = this->mc_UINodes[orc_MessageId.u32_NodeIndex];
            if (pc_Protocol->u32_DataPoolIndex < rc_UINode.c_UIDataPools.size())
            {
               const C_OSCNodeDataPool * const pc_OSCDataPool = rc_OSCNode.GetComDataPoolConst(
                  orc_MessageId.e_ComProtocol,
                  orc_MessageId.u32_DatapoolIndex);

               if (pc_OSCDataPool != NULL)
               {
                  uint32 u32_ListIndex;
                  if (C_OSCCanProtocol::h_GetComListIndex(*pc_OSCDataPool, orc_MessageId.u32_InterfaceIndex,
                                                          orc_MessageId.q_MessageIsTx,
                                                          u32_ListIndex) == C_NO_ERR)
                  {
                     C_PuiSdNodeDataPool & rc_UIDataPool = rc_UINode.c_UIDataPools[pc_Protocol->u32_DataPoolIndex];
                     if (u32_ListIndex < rc_UIDataPool.c_DataPoolLists.size())
                     {
                        C_PuiSdNodeDataPoolList & rc_UiList = rc_UIDataPool.c_DataPoolLists[u32_ListIndex];
                        if ((u32_SignalIndex + oru32_SignalIndex) < rc_UiList.c_DataPoolListElements.size())
                        {
                           rc_UiList.c_DataPoolListElements.erase(
                              rc_UiList.c_DataPoolListElements.begin() + u32_SignalIndex + oru32_SignalIndex);
                        }
                     }
                  }
               }
            }
         }
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
            }
         }
      }
   }
   return s32_RETVAL;
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
   const C_OSCNodeDataPoolListElementId & orc_NodeDatapoolListElementId,
   C_OSCCanMessageIdentificationIndices & orc_MessageId, uint32 & oru32_SignalIndex) const
{
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(orc_NodeDatapoolListElementId.u32_NodeIndex);
   const C_OSCNodeDataPool * const pc_Datapool = this->GetOSCDataPool(orc_NodeDatapoolListElementId.u32_NodeIndex,
                                                                      orc_NodeDatapoolListElementId.u32_DataPoolIndex);

   C_OSCCanProtocol::E_Type e_ComProtocol = C_OSCCanProtocol::eLAYER2;
   uint32 u32_InterfaceIndex;
   uint32 u32_MessageIndex = 0;

   if ((pc_Node != NULL) && (pc_Datapool != NULL))
   {
      bool q_MessageIsTx;
      const C_OSCNodeDataPoolList & rc_List = pc_Datapool->c_Lists[orc_NodeDatapoolListElementId.u32_ListIndex];

      // com protocol type
      for (uint32 u32_Pos = 0; u32_Pos < pc_Node->c_ComProtocols.size(); u32_Pos++)
      {
         if (pc_Node->c_ComProtocols[u32_Pos].u32_DataPoolIndex == orc_NodeDatapoolListElementId.u32_DataPoolIndex)
         {
            e_ComProtocol = pc_Node->c_ComProtocols[u32_Pos].e_Type;
         }
      }

      // message is tx
      q_MessageIsTx = C_OSCCanProtocol::h_ListIsComTx(rc_List);

      // interface index
      u32_InterfaceIndex = orc_NodeDatapoolListElementId.u32_ListIndex / 2; // 2:1 relation (see EA diagram)

      // message index
      const C_OSCCanMessageContainer * const pc_MessageContainer = this->GetCanProtocolMessageContainer(
         orc_NodeDatapoolListElementId.u32_NodeIndex, e_ComProtocol, u32_InterfaceIndex,
         orc_NodeDatapoolListElementId.u32_DataPoolIndex);
      std::vector<C_OSCCanMessage> c_Messages; // either tx messages or rx messages
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

      for (uint32 u32_PosMessage = 0; (u32_PosMessage < c_Messages.size()) && (q_IndexFound == false);
           u32_PosMessage++)
      {
         const C_OSCCanMessage & rc_Message = c_Messages[u32_PosMessage];
         for (uint32 u32_PosSignal = 0; (u32_PosSignal < rc_Message.c_Signals.size()) && (q_IndexFound == false);
              u32_PosSignal++)
         {
            const C_OSCCanSignal & rc_Signal = rc_Message.c_Signals[u32_PosSignal];
            if (rc_Signal.u32_ComDataElementIndex == orc_NodeDatapoolListElementId.u32_ElementIndex)
            {
               u32_MessageIndex = u32_PosMessage;
               oru32_SignalIndex = u32_PosSignal;
               q_IndexFound = true;
            }
         }
      }

      // output
      orc_MessageId = C_OSCCanMessageIdentificationIndices(orc_NodeDatapoolListElementId.u32_NodeIndex,
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
sint32 C_PuiSdHandlerBusLogic::CheckMessageMatch(const C_OSCCanMessageIdentificationIndices & orc_MessageId1,
                                                 const C_OSCCanMessageIdentificationIndices & orc_MessageId2,
                                                 bool & orq_IsMatch, const bool oq_IgnoreMessageDirection) const
{
   sint32 s32_Retval = this->mc_CoreDefinition.CheckMessageMatch(orc_MessageId1, orc_MessageId2, orq_IsMatch,
                                                                 oq_IgnoreMessageDirection);

   //Check Ui part
   if ((s32_Retval == C_NO_ERR) && (orq_IsMatch == true))
   {
      const C_OSCCanMessageContainer * const pc_MessageContainer1 =
         this->GetCanProtocolMessageContainer(orc_MessageId1.u32_NodeIndex, orc_MessageId1.e_ComProtocol,
                                              orc_MessageId1.u32_InterfaceIndex, orc_MessageId1.u32_DatapoolIndex);
      const C_OSCCanMessageContainer * const pc_MessageContainer2 =
         this->GetCanProtocolMessageContainer(orc_MessageId2.u32_NodeIndex, orc_MessageId2.e_ComProtocol,
                                              orc_MessageId2.u32_InterfaceIndex, orc_MessageId2.u32_DatapoolIndex);
      if ((pc_MessageContainer1 != NULL) && (pc_MessageContainer2 != NULL))
      {
         const std::vector<C_OSCCanMessage> & rc_Messages1 = pc_MessageContainer1->GetMessagesConst(
            orc_MessageId1.q_MessageIsTx);
         const std::vector<C_OSCCanMessage> & rc_Messages2 = pc_MessageContainer2->GetMessagesConst(
            orc_MessageId2.q_MessageIsTx);
         if ((orc_MessageId1.u32_MessageIndex < rc_Messages1.size()) &&
             (orc_MessageId2.u32_MessageIndex < rc_Messages2.size()))
         {
            const C_OSCCanMessage c_Message1 = rc_Messages1[orc_MessageId1.u32_MessageIndex];
            const C_OSCCanMessage c_Message2 = rc_Messages2[orc_MessageId2.u32_MessageIndex];
            if (c_Message1.c_Signals.size() == c_Message2.c_Signals.size())
            {
               //Step 1: Get position and size in list
               const uint32 u32_SignalSize = c_Message1.c_Signals.size();

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
                     const uint32 u32_List1Start = pc_MessageContainer1->GetMessageSignalDataStartIndex(
                        orc_MessageId1.q_MessageIsTx, orc_MessageId1.u32_MessageIndex);
                     const uint32 u32_List2Start = pc_MessageContainer2->GetMessageSignalDataStartIndex(
                        orc_MessageId2.q_MessageIsTx, orc_MessageId2.u32_MessageIndex);
                     if (((u32_List1Start + u32_SignalSize) <= pc_List1->c_DataPoolListElements.size()) &&
                         ((u32_List2Start + u32_SignalSize) <= pc_List2->c_DataPoolListElements.size()))
                     {
                        for (uint32 u32_ItSignal = 0; u32_ItSignal < u32_SignalSize; ++u32_ItSignal)
                        {
                           const uint32 u32_Index1 = u32_List1Start + u32_ItSignal;
                           const uint32 u32_Index2 = u32_List2Start + u32_ItSignal;
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
sint32 C_PuiSdHandlerBusLogic::MapBusNameToIndex(const QString & orc_BusName, uint32 & oru32_BusIndex) const
{
   sint32 s32_Retval = C_RANGE;

   for (uint32 u32_ItBus = 0UL; u32_ItBus < this->GetOSCBusesSize(); ++u32_ItBus)
   {
      const C_OSCSystemBus * const pc_Bus = this->GetOSCBus(u32_ItBus);
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
sint32 C_PuiSdHandlerBusLogic::MapBusIndexToName(const uint32 ou32_BusIndex, QString & orc_BusName) const
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCSystemBus * const pc_Bus = this->GetOSCBus(ou32_BusIndex);

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

   \return
   Signal display name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdHandlerBusLogic::GetCanSignalDisplayName(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                        const uint32 & oru32_SignalIndex) const
{
   QString c_Retval;
   const C_OSCCanSignal * const pc_Signal = this->GetCanSignal(orc_MessageId, oru32_SignalIndex);
   const C_OSCNodeDataPoolListElement * const pc_SignalData = this->GetOSCCanDataPoolListElement(orc_MessageId,
                                                                                                 oru32_SignalIndex);

   if ((pc_Signal != NULL) && (pc_SignalData != NULL))
   {
      if (pc_Signal->e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         c_Retval = static_cast<QString>("%1 (Multiplexer)").arg(pc_SignalData->c_Name.c_str());
      }
      else
      {
         c_Retval = pc_SignalData->c_Name.c_str();
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdHandlerBusLogic::C_PuiSdHandlerBusLogic(QObject * const opc_Parent) :
   C_PuiSdHandlerHALC(opc_Parent)
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
void C_PuiSdHandlerBusLogic::m_GetExistingMessageNamesProtocol(const uint32 & oru32_NodeIndex,
                                                               const C_OSCCanProtocol::E_Type & ore_ComType,
                                                               const uint32 & oru32_InterfaceIndex,
                                                               std::map<C_SCLString, bool> & orc_ExistingNames)
const
{
   const std::vector<const C_OSCCanMessageContainer *> c_Container = this->GetCanProtocolMessageContainers(
      oru32_NodeIndex, ore_ComType, oru32_InterfaceIndex);
   uint32 u32_Counter;

   // Check all COMM Datapools
   for (u32_Counter = 0U; u32_Counter < c_Container.size(); ++u32_Counter)
   {
      const C_OSCCanMessageContainer * const pc_Container = c_Container[u32_Counter];
      const std::vector<C_OSCCanMessage> & rc_TxMessages = pc_Container->GetMessagesConst(true);
      const std::vector<C_OSCCanMessage> & rc_RxMessages = pc_Container->GetMessagesConst(false);
      //Tx
      for (uint32 u32_ItMessage = 0; u32_ItMessage < rc_TxMessages.size(); ++u32_ItMessage)
      {
         const C_OSCCanMessage & rc_Message = rc_TxMessages[u32_ItMessage];
         orc_ExistingNames[rc_Message.c_Name] = true;
      }
      //Rx
      for (uint32 u32_ItMessage = 0; u32_ItMessage < rc_RxMessages.size(); ++u32_ItMessage)
      {
         const C_OSCCanMessage & rc_Message = rc_RxMessages[u32_ItMessage];
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
std::map<C_SCLString, bool> C_PuiSdHandlerBusLogic::m_GetExistingMessageNames(const uint32 & oru32_NodeIndex,
                                                                              const uint32 & oru32_InterfaceIndex) const
{
   std::map<C_SCLString, bool> c_ExistingNames;

   // get existing message names of all three protocols
   m_GetExistingMessageNamesProtocol(oru32_NodeIndex, C_OSCCanProtocol::eLAYER2, oru32_InterfaceIndex, c_ExistingNames);
   m_GetExistingMessageNamesProtocol(oru32_NodeIndex, C_OSCCanProtocol::eCAN_OPEN_SAFETY, oru32_InterfaceIndex,
                                     c_ExistingNames);
   m_GetExistingMessageNamesProtocol(oru32_NodeIndex, C_OSCCanProtocol::eECES, oru32_InterfaceIndex, c_ExistingNames);

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
std::map<C_SCLString, bool> C_PuiSdHandlerBusLogic::m_GetExistingSignalNames(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   std::map<C_SCLString, bool> c_Retval;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(orc_MessageId.u32_NodeIndex);
   if (pc_Node != NULL)
   {
      const C_OSCNodeDataPool * const pc_ComDataPool = pc_Node->GetComDataPoolConst(orc_MessageId.e_ComProtocol,
                                                                                    orc_MessageId.u32_DatapoolIndex);
      if (pc_ComDataPool != NULL)
      {
         const C_OSCNodeDataPoolList * const pc_List = C_OSCCanProtocol::h_GetComListConst(*pc_ComDataPool,
                                                                                           orc_MessageId.u32_InterfaceIndex,
                                                                                           orc_MessageId.q_MessageIsTx);
         const C_OSCCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);
         if ((pc_Message != NULL) && (pc_List != NULL))
         {
            for (uint32 u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
            {
               const C_OSCCanSignal & rc_Signal = pc_Message->c_Signals[u32_ItSignal];
               if (rc_Signal.u32_ComDataElementIndex < pc_List->c_Elements.size())
               {
                  const C_OSCNodeDataPoolListElement & rc_DataElement =
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
C_PuiSdNodeCanProtocol * C_PuiSdHandlerBusLogic::m_GetUiCanProtocol(const uint32 & oru32_NodeIndex,
                                                                    const C_OSCCanProtocol::E_Type & ore_ComType,
                                                                    const uint32 ou32_DatapoolIndex)
{
   C_PuiSdNodeCanProtocol * pc_Retval = NULL;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      C_PuiSdNode & rc_UINode = this->mc_UINodes[oru32_NodeIndex];
      tgl_assert(rc_OSCNode.c_ComProtocols.size() == rc_UINode.c_UICanProtocols.size());
      for (uint32 u32_ItCanProtocol = 0; u32_ItCanProtocol < rc_OSCNode.c_ComProtocols.size(); ++u32_ItCanProtocol)
      {
         const C_OSCCanProtocol & rc_CanProtocol = rc_OSCNode.c_ComProtocols[u32_ItCanProtocol];
         if ((rc_CanProtocol.e_Type == ore_ComType) &&
             (rc_CanProtocol.u32_DataPoolIndex == ou32_DatapoolIndex))
         {
            pc_Retval = &rc_UINode.c_UICanProtocols[u32_ItCanProtocol];
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
const C_PuiSdNodeDataPoolList * C_PuiSdHandlerBusLogic::m_GetUiCanDataPoolList(const uint32 & oru32_NodeIndex,
                                                                               const C_OSCCanProtocol::E_Type & ore_ComType, const uint32 & oru32_InterfaceIndex, const uint32 ou32_DatapoolIndex,
                                                                               const bool & orq_MessageIsTx) const
{
   const C_PuiSdNodeDataPoolList * pc_Retval = NULL;
   const C_OSCNodeDataPool * const pc_OSCDataPool = this->GetOSCCanDataPool(oru32_NodeIndex, ore_ComType,
                                                                            ou32_DatapoolIndex);

   if (pc_OSCDataPool != NULL)
   {
      uint32 u32_Index;

      if (C_OSCCanProtocol::h_GetComListIndex(*pc_OSCDataPool, oru32_InterfaceIndex, orq_MessageIsTx,
                                              u32_Index) == C_NO_ERR)
      {
         const C_PuiSdNodeDataPool * const pc_UIDataPool = this->GetUiCanDataPool(oru32_NodeIndex, ore_ComType,
                                                                                  ou32_DatapoolIndex);

         if (pc_UIDataPool != NULL)
         {
            if (u32_Index < pc_UIDataPool->c_DataPoolLists.size())
            {
               pc_Retval = &pc_UIDataPool->c_DataPoolLists[u32_Index];
            }
         }
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert ui part for can message

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  orc_UISignalCommons  Signals data (ui common)
   \param[in]  orc_UIMessage        Message data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerBusLogic::m_InsertUiCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                    const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                                    const C_PuiSdNodeCanMessage & orc_UIMessage)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(orc_MessageId.u32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OSCCanProtocol * const pc_Protocol = pc_Node->GetCANProtocolConst(orc_MessageId.e_ComProtocol,
                                                                                orc_MessageId.u32_DatapoolIndex);
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol,
                                                                              orc_MessageId.u32_DatapoolIndex);

      if (pc_Protocol != NULL)
      {
         if (orc_MessageId.u32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
         {
            const C_OSCCanMessageContainer & rc_MessageContainer =
               pc_Protocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
            const uint32 u32_SignalIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
               orc_MessageId.q_MessageIsTx,
               orc_MessageId.u32_MessageIndex);
            C_PuiSdNode & rc_UINode = this->mc_UINodes[orc_MessageId.u32_NodeIndex];
            if (pc_Protocol->u32_DataPoolIndex < rc_UINode.c_UIDataPools.size())
            {
               const C_OSCNodeDataPool * const pc_OSCDataPool = pc_Node->GetComDataPoolConst(
                  orc_MessageId.e_ComProtocol, orc_MessageId.u32_DatapoolIndex);

               if (pc_OSCDataPool != NULL)
               {
                  uint32 u32_ListIndex;
                  if (C_OSCCanProtocol::h_GetComListIndex(*pc_OSCDataPool, orc_MessageId.u32_InterfaceIndex,
                                                          orc_MessageId.q_MessageIsTx,
                                                          u32_ListIndex) == C_NO_ERR)
                  {
                     C_PuiSdNodeDataPool & rc_UIDataPool = rc_UINode.c_UIDataPools[pc_Protocol->u32_DataPoolIndex];
                     if (u32_ListIndex < rc_UIDataPool.c_DataPoolLists.size())
                     {
                        C_PuiSdNodeDataPoolList & rc_UiList = rc_UIDataPool.c_DataPoolLists[u32_ListIndex];
                        if (u32_SignalIndex <= rc_UiList.c_DataPoolListElements.size())
                        {
                           for (uint32 u32_ItSignal = 0; u32_ItSignal < orc_UISignalCommons.size(); ++u32_ItSignal)
                           {
                              rc_UiList.c_DataPoolListElements.insert(
                                 rc_UiList.c_DataPoolListElements.begin() + u32_SignalIndex + u32_ItSignal,
                                 orc_UISignalCommons[u32_ItSignal]);
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
               rc_Messages.insert(rc_Messages.begin() + orc_MessageId.u32_MessageIndex, orc_UIMessage);
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
sint32 C_PuiSdHandlerBusLogic::m_DeleteUiCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(orc_MessageId.u32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OSCCanProtocol * const pc_Protocol = pc_Node->GetCANProtocolConst(orc_MessageId.e_ComProtocol,
                                                                                orc_MessageId.u32_DatapoolIndex);
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol,
                                                                              orc_MessageId.u32_DatapoolIndex);

      if (pc_Protocol != NULL)
      {
         if (orc_MessageId.u32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
         {
            const C_OSCCanMessageContainer & rc_MessageContainer =
               pc_Protocol->c_ComMessages[orc_MessageId.u32_InterfaceIndex];
            const uint32 u32_SignalIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
               orc_MessageId.q_MessageIsTx,
               orc_MessageId.u32_MessageIndex);
            C_PuiSdNode & rc_UINode = this->mc_UINodes[orc_MessageId.u32_NodeIndex];
            if (pc_Protocol->u32_DataPoolIndex < rc_UINode.c_UIDataPools.size())
            {
               const C_OSCNodeDataPool * const pc_OSCDataPool = pc_Node->GetComDataPoolConst(
                  orc_MessageId.e_ComProtocol,
                  orc_MessageId.u32_DatapoolIndex);

               if (pc_OSCDataPool != NULL)
               {
                  uint32 u32_ListIndex;
                  if (C_OSCCanProtocol::h_GetComListIndex(*pc_OSCDataPool, orc_MessageId.u32_InterfaceIndex,
                                                          orc_MessageId.q_MessageIsTx,
                                                          u32_ListIndex) == C_NO_ERR)
                  {
                     C_PuiSdNodeDataPool & rc_UIDataPool = rc_UINode.c_UIDataPools[pc_Protocol->u32_DataPoolIndex];
                     if (u32_ListIndex < rc_UIDataPool.c_DataPoolLists.size())
                     {
                        C_PuiSdNodeDataPoolList & rc_UiList = rc_UIDataPool.c_DataPoolLists[u32_ListIndex];
                        if (u32_SignalIndex < rc_UiList.c_DataPoolListElements.size())
                        {
                           const C_PuiSdNodeCanMessage * const pc_Message = this->GetUiCanMessage(orc_MessageId);
                           if (pc_Message != NULL)
                           {
                              for (uint32 u32_ItSignal = pc_Message->c_Signals.size(); u32_ItSignal > 0; --u32_ItSignal)
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
