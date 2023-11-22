//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Store system definition data

   System definition data container class.
   Also provides utility functions to use the data.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <map>
#include <algorithm>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclString.hpp"
#include "C_OscSystemDefinition.hpp"
#include "TglUtils.hpp"
#include "C_OscUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;

using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
C_OscDeviceManager C_OscSystemDefinition::hc_Devices;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSystemDefinition::C_OscSystemDefinition(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSystemDefinition::~C_OscSystemDefinition(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSystemDefinition::CalcHash(uint32_t & oru32_HashValue) const
{
   uint32_t u32_Counter;

   // check all subelements
   for (u32_Counter = 0U; u32_Counter < this->c_Nodes.size(); ++u32_Counter)
   {
      this->c_Nodes[u32_Counter].CalcHash(oru32_HashValue);
   }

   for (u32_Counter = 0U; u32_Counter < this->c_Buses.size(); ++u32_Counter)
   {
      this->c_Buses[u32_Counter].CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add bus

   \param[in]  orc_Bus  Bus value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSystemDefinition::AddBus(const C_OscSystemBus & orc_Bus)
{
   tgl_assert(this->InsertBus(static_cast<uint32_t>(this->c_Buses.size()), orc_Bus) == C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert bus

   \param[in]  ou32_BusIndex  Bus index
   \param[in]  orc_Bus        Bus value

   \return
   C_NO_ERR Done
   C_RANGE  Bus index invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSystemDefinition::InsertBus(const uint32_t ou32_BusIndex, const C_OscSystemBus & orc_Bus)
{
   int32_t s32_Retval = C_NO_ERR;

   //Smaller and equal because append is also valid
   if (ou32_BusIndex <= this->c_Buses.size())
   {
      this->c_Buses.insert(this->c_Buses.begin() + ou32_BusIndex, orc_Bus);
      //Sync interface indices
      for (uint32_t u32_ItNode = 0; u32_ItNode < this->c_Nodes.size(); ++u32_ItNode)
      {
         C_OscNode & rc_Node = this->c_Nodes[u32_ItNode];
         for (uint32_t u32_ItInterface = 0; u32_ItInterface < rc_Node.c_Properties.c_ComInterfaces.size();
              ++u32_ItInterface)
         {
            C_OscNodeComInterfaceSettings & rc_ComInterface = rc_Node.c_Properties.c_ComInterfaces[u32_ItInterface];
            if (rc_ComInterface.GetBusConnected() == true)
            {
               if (rc_ComInterface.u32_BusIndex >= ou32_BusIndex)
               {
                  //Adapt
                  ++rc_ComInterface.u32_BusIndex;
               }
               else
               {
                  //No change
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
/*! \brief   Delete bus

   \param[in]  ou32_BusIndex  Bus index

   \return
   C_NO_ERR Done
   C_RANGE  Bus index invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSystemDefinition::DeleteBus(const uint32_t ou32_BusIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_BusIndex < this->c_Buses.size())
   {
      this->c_Buses.erase(this->c_Buses.begin() + ou32_BusIndex);
      //Sync interface indices
      for (uint32_t u32_ItNode = 0; u32_ItNode < this->c_Nodes.size(); ++u32_ItNode)
      {
         C_OscNode & rc_Node = this->c_Nodes[u32_ItNode];
         for (uint32_t u32_ItInterface = 0; u32_ItInterface < rc_Node.c_Properties.c_ComInterfaces.size();
              ++u32_ItInterface)
         {
            C_OscNodeComInterfaceSettings & rc_ComInterface = rc_Node.c_Properties.c_ComInterfaces[u32_ItInterface];
            if (rc_ComInterface.GetBusConnectedRawValue() == true)
            {
               if (rc_ComInterface.u32_BusIndex == ou32_BusIndex)
               {
                  //Disconnect
                  rc_ComInterface.RemoveConnection();
               }
               else if (rc_ComInterface.u32_BusIndex > ou32_BusIndex)
               {
                  //Adapt
                  --rc_ComInterface.u32_BusIndex;
               }
               else
               {
                  //No change
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
/*! \brief   Add new connection from specified node to specified bus using specified interface number

   \param[in]  ou32_NodeIndex    Node to connect from
   \param[in]  ou32_BusIndex     Bus to connect to
   \param[in]  ou8_Interface     Interface number to use

   \return
   C_NO_ERR Done
   C_RANGE  Either node or bus or both do not exist
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSystemDefinition::AddConnection(const uint32_t ou32_NodeIndex, const uint32_t ou32_BusIndex,
                                             const uint8_t ou8_Interface)
{
   int32_t s32_Retval = C_NO_ERR;

   if ((ou32_NodeIndex < this->c_Nodes.size()) && (ou32_BusIndex < this->c_Buses.size()))
   {
      C_OscNode & rc_Node = this->c_Nodes[ou32_NodeIndex];
      C_OscSystemBus & rc_Bus = this->c_Buses[ou32_BusIndex];
      const C_OscNodeComInterfaceSettings * const pc_Interface = rc_Node.c_Properties.GetComInterface(rc_Bus.e_Type,
                                                                                                      ou8_Interface);
      if (pc_Interface != NULL)
      {
         C_OscNodeComInterfaceSettings c_Tmp = *pc_Interface;
         c_Tmp.AddConnection(ou32_BusIndex);
         rc_Node.c_Properties.SetComInterface(c_Tmp);
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if com interface of node allows specified connection

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_ComIndex     Communication interface
   \param[in]  ou8_ComNodeId     Node ID

   \return
   false Conflict
   true  Default
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscSystemDefinition::CheckInterfaceIsAvailable(const uint32_t ou32_NodeIndex, const uint32_t ou32_ComIndex,
                                                      const uint8_t ou8_ComNodeId) const
{
   bool q_Retval = true;

   //Check node exists
   if (ou32_NodeIndex < this->c_Nodes.size())
   {
      const C_OscNode & rc_CurNode = this->c_Nodes[ou32_NodeIndex];
      //Check com interface exists
      if (ou32_ComIndex < rc_CurNode.c_Properties.c_ComInterfaces.size())
      {
         const C_OscNodeComInterfaceSettings & rc_CurComInterface =
            rc_CurNode.c_Properties.c_ComInterfaces[ou32_ComIndex];
         //Check com interface has bus connected
         if (rc_CurComInterface.GetBusConnected() == true)
         {
            std::vector<uint32_t> c_NodeIndices;
            std::vector<uint32_t> c_InterfaceIndices;
            this->GetNodeIndexesOfBus(rc_CurComInterface.u32_BusIndex, c_NodeIndices, c_InterfaceIndices);
            if (c_NodeIndices.size() == c_InterfaceIndices.size())
            {
               for (uint32_t u32_ItFound = 0; u32_ItFound < c_NodeIndices.size(); ++u32_ItFound)
               {
                  //Skip selected com interface
                  if (((ou32_NodeIndex == c_NodeIndices[u32_ItFound]) &&
                       (c_InterfaceIndices[u32_ItFound] == ou32_ComIndex)) == false)
                  {
                     if (c_NodeIndices[u32_ItFound] < this->c_Nodes.size())
                     {
                        const C_OscNode & rc_Node = this->c_Nodes[c_NodeIndices[u32_ItFound]];
                        //Check all com interfaces
                        if (c_InterfaceIndices[u32_ItFound] < rc_Node.c_Properties.c_ComInterfaces.size())
                        {
                           //Check if connected bus matches
                           const C_OscNodeComInterfaceSettings & rc_ComInterface =
                              rc_Node.c_Properties.c_ComInterfaces[c_InterfaceIndices[u32_ItFound]];
                           if (rc_ComInterface.u8_NodeId == ou8_ComNodeId)
                           {
                              //Report collision
                              q_Retval = false;
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if com interface of node allows specified IP address

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_ComIndex     Communication interface
   \param[in]  orc_Ip            IP Address

   \return
   false Conflict
   true  Default
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscSystemDefinition::CheckIpAddressIsValid(const uint32_t ou32_NodeIndex, const uint32_t ou32_ComIndex,
                                                  const std::vector<int32_t> & orc_Ip) const
{
   bool q_Retval = true;

   //Check node exists
   if (ou32_NodeIndex < this->c_Nodes.size())
   {
      const C_OscNode & rc_CurNode = this->c_Nodes[ou32_NodeIndex];
      //Check com interface exists
      if (ou32_ComIndex < rc_CurNode.c_Properties.c_ComInterfaces.size())
      {
         const C_OscNodeComInterfaceSettings & rc_CurComInterface =
            rc_CurNode.c_Properties.c_ComInterfaces[ou32_ComIndex];
         //Check com interface has bus connected
         if (rc_CurComInterface.GetBusConnected() == true)
         {
            std::vector<uint32_t> c_NodeIndices;
            std::vector<uint32_t> c_InterfaceIndices;
            this->GetNodeIndexesOfBus(rc_CurComInterface.u32_BusIndex, c_NodeIndices, c_InterfaceIndices);
            if (c_NodeIndices.size() == c_InterfaceIndices.size())
            {
               for (uint32_t u32_ItFound = 0; u32_ItFound < c_NodeIndices.size(); ++u32_ItFound)
               {
                  //Skip selected com interface
                  if (((ou32_NodeIndex == c_NodeIndices[u32_ItFound]) &&
                       (c_InterfaceIndices[u32_ItFound] == ou32_ComIndex)) == false)
                  {
                     if (c_NodeIndices[u32_ItFound] < this->c_Nodes.size())
                     {
                        const C_OscNode & rc_Node = this->c_Nodes[c_NodeIndices[u32_ItFound]];
                        //Check all com interfaces
                        if (c_InterfaceIndices[u32_ItFound] < rc_Node.c_Properties.c_ComInterfaces.size())
                        {
                           //Check if connected bus matches
                           const C_OscNodeComInterfaceSettings & rc_ComInterface =
                              rc_Node.c_Properties.c_ComInterfaces[c_InterfaceIndices[u32_ItFound]];

                           if ((static_cast<uint8_t>(orc_Ip[0]) == rc_ComInterface.c_Ip.au8_IpAddress[0]) &&
                               (static_cast<uint8_t>(orc_Ip[1]) == rc_ComInterface.c_Ip.au8_IpAddress[1]) &&
                               (static_cast<uint8_t>(orc_Ip[2]) == rc_ComInterface.c_Ip.au8_IpAddress[2]) &&
                               (static_cast<uint8_t>(orc_Ip[3]) == rc_ComInterface.c_Ip.au8_IpAddress[3]))
                           {
                              q_Retval = false;
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if any bus uses the provided bus id

   \param[in]  ou8_BusId               Bus id to check for
   \param[in]  opu32_BusIndexToSkip    Optional parameter to skip one index
                                       (Use-case: skip current bus to avoid conflict with itself)

   \return
   true  Available
   false Already in use
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscSystemDefinition::CheckBusIdAvailable(const uint8_t ou8_BusId,
                                                const uint32_t * const opu32_BusIndexToSkip) const
{
   bool q_Retval = true;

   for (uint32_t u32_ItBus = 0; u32_ItBus < this->c_Buses.size(); ++u32_ItBus)
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
         const C_OscSystemBus & rc_Bus = this->c_Buses[u32_ItBus];
         if ((rc_Bus.q_UseableForRouting == true) && (rc_Bus.u8_BusId == ou8_BusId))
         {
            q_Retval = false;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get new unused bus id

   \param[out]  oru8_BusId    Bus id result value

   \return
   C_NO_ERR Valid bus id found
   C_NOACT  No valid bus id found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSystemDefinition::GetNextFreeBusId(uint8_t & oru8_BusId) const
{
   int32_t s32_Retval = C_NO_ERR;
   bool q_Continue = true;

   for (oru8_BusId = 0; (oru8_BusId <= 15) && (q_Continue == true); ++oru8_BusId)
   {
      if (this->CheckBusIdAvailable(oru8_BusId) == true)
      {
         q_Continue = false;
      }
   }
   //Revert last ++
   --oru8_BusId;
   //Check result
   if (q_Continue == true)
   {
      s32_Retval = C_NOACT;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check error for node

   \param[in]      ou32_NodeIndex                  Node index
   \param[out]     opq_NameConflict                Name conflict
   \param[out]     opq_NameInvalid                 Name not usable as variable
   \param[out]     opq_NodeIdInvalid               true: Node index is not usable
   \param[out]     opq_IpInvalid                   true: IP address is not usable
   \param[out]     opq_DataPoolsInvalid            true: error in data pool was detected
   \param[out]     opq_ApplicationsInvalid         true: error in application was detected
   \param[out]     opq_DomainsInvalid              true: error in HALC configuration was detected
   \param[out]     opq_CommMinSignalCountInvalid   true: error in COMM protocol with invalid min signal count was detected
   \param[out]     opq_CommMaxSignalCountInvalid   true: error in COMM protocol with invalid max signal count was detected
   \param[out]     opq_CoPdoCountInvalid           true: error in CANopen protocol with invalid PDO count was detected
   \param[out]     opq_CoNodeIdInvalid             true: error with CANopen Node ID detected
   \param[out]     opq_CoHearbeatTimeInvalid       true: error with hearbeat time of Manager detected
   \param[in]      orq_AllowComDataPoolException   true: allow exception to skip check for connected interface
   \param[in,out]  opc_InvalidInterfaceIndices     Optional storage for invalid interface indices
   \param[in,out]  opc_InvalidDataPoolIndices      Optional storage for invalid datapool indices
   \param[in,out]  opc_InvalidApplicationIndices   Optional storage for invalid application indices
   \param[in,out]  opc_InvalidDomainIndices        Optional storage for invalid application indices
   \param[in,out]  opc_InvalidProtocolTypes        Optional storage for invalid COMM protocol types

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSystemDefinition::CheckErrorNode(const uint32_t ou32_NodeIndex, bool * const opq_NameConflict,
                                              bool * const opq_NameInvalid, bool * const opq_NodeIdInvalid,
                                              bool * const opq_IpInvalid, bool * const opq_DataPoolsInvalid,
                                              bool * const opq_ApplicationsInvalid, bool * const opq_DomainsInvalid,
                                              bool * const opq_CommMinSignalCountInvalid,
                                              bool * const opq_CommMaxSignalCountInvalid,
                                              bool * const opq_CoPdoCountInvalid, bool * const opq_CoNodeIdInvalid,
                                              bool * const opq_CoHearbeatTimeInvalid,
                                              const bool & orq_AllowComDataPoolException,
                                              std::vector<uint32_t> * const opc_InvalidInterfaceIndices,
                                              std::vector<uint32_t> * const opc_InvalidDataPoolIndices,
                                              std::vector<uint32_t> * const opc_InvalidApplicationIndices,
                                              std::vector<uint32_t> * const opc_InvalidDomainIndices,
                                              std::vector<C_OscCanProtocol::E_Type> * const opc_InvalidProtocolTypes)
const
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->c_Nodes.size())
   {
      const C_OscNode & rc_CheckedNode = this->c_Nodes[ou32_NodeIndex];
      if (opq_NameConflict != NULL)
      {
         //check for node name used more than once (independent of character case)
         *opq_NameConflict = false;
         for (uint32_t u32_ItNode = 0; u32_ItNode < this->c_Nodes.size(); ++u32_ItNode)
         {
            if (u32_ItNode != ou32_NodeIndex)
            {
               uint32_t u32_GroupIndex;
               stw::scl::C_SclString c_CurName;
               if (C_OscNodeSquad::h_CheckIsMultiDevice(u32_ItNode, this->c_NodeSquads, &u32_GroupIndex))
               {
                  tgl_assert(u32_GroupIndex < this->c_NodeSquads.size());
                  if (u32_GroupIndex < this->c_NodeSquads.size())
                  {
                     const C_OscNodeSquad & rc_Group = this->c_NodeSquads[u32_GroupIndex];
                     c_CurName = rc_Group.c_BaseName;
                  }
               }
               else
               {
                  const C_OscNode & rc_CurrentNode = this->c_Nodes[u32_ItNode];
                  c_CurName = rc_CurrentNode.c_Properties.c_Name;
               }
               if (rc_CheckedNode.c_Properties.c_Name.LowerCase() == c_CurName.LowerCase())
               {
                  *opq_NameConflict = true;
                  break;
               }
            }
         }
      }
      if (opq_NameInvalid != NULL)
      {
         uint32_t u32_GroupIndex;

         if (C_OscNodeSquad::h_CheckIsMultiDevice(ou32_NodeIndex, this->c_NodeSquads, &u32_GroupIndex))
         {
            tgl_assert(u32_GroupIndex < this->c_NodeSquads.size());
            if (u32_GroupIndex < this->c_NodeSquads.size())
            {
               const C_OscNodeSquad & rc_Group = this->c_NodeSquads[u32_GroupIndex];
               *opq_NameInvalid = !C_OscUtils::h_CheckValidCeName(rc_Group.c_BaseName);
            }
         }
         else
         {
            //check for valid node name
            *opq_NameInvalid = !C_OscUtils::h_CheckValidCeName(rc_CheckedNode.c_Properties.c_Name);
         }
      }
      if (opq_NodeIdInvalid != NULL)
      {
         //check for valid node ID
         *opq_NodeIdInvalid = false;

         for (uint32_t u32_ItComInterface = 0; u32_ItComInterface < rc_CheckedNode.c_Properties.c_ComInterfaces.size();
              ++u32_ItComInterface)
         {
            //check for same node ID used twice on same bus
            const bool q_ComIdValid = this->CheckInterfaceIsAvailable(ou32_NodeIndex, u32_ItComInterface,
                                                                      rc_CheckedNode.c_Properties.c_ComInterfaces[
                                                                         u32_ItComInterface].u8_NodeId);

            //if conflict found abort check
            if (q_ComIdValid == false)
            {
               *opq_NodeIdInvalid = true;
               if (opc_InvalidInterfaceIndices == NULL)
               {
                  //not interested in details, we are finished here as we know there was at least one conflict
                  break;
               }
               else
               {
                  opc_InvalidInterfaceIndices->push_back(u32_ItComInterface);
               }
            }
         }
      }
      if (opq_IpInvalid != NULL)
      {
         *opq_IpInvalid = false;

         for (uint32_t u32_ItComInterface = 0; u32_ItComInterface < rc_CheckedNode.c_Properties.c_ComInterfaces.size();
              ++u32_ItComInterface)
         {
            //only consider ethernet interfaces for this check
            if (rc_CheckedNode.c_Properties.c_ComInterfaces[u32_ItComInterface].e_InterfaceType ==
                C_OscSystemBus::eETHERNET)
            {
               std::vector<int32_t> c_Ip;
               c_Ip.reserve(4U);
               c_Ip.push_back(static_cast<int32_t>(rc_CheckedNode.c_Properties.c_ComInterfaces[u32_ItComInterface].c_Ip.
                                                   au8_IpAddress[0]));
               c_Ip.push_back(static_cast<int32_t>(rc_CheckedNode.c_Properties.c_ComInterfaces[u32_ItComInterface].c_Ip.
                                                   au8_IpAddress[1]));
               c_Ip.push_back(static_cast<int32_t>(rc_CheckedNode.c_Properties.c_ComInterfaces[u32_ItComInterface].c_Ip.
                                                   au8_IpAddress[2]));
               c_Ip.push_back(static_cast<int32_t>(rc_CheckedNode.c_Properties.c_ComInterfaces[u32_ItComInterface].c_Ip.
                                                   au8_IpAddress[3]));
               //check for same IP used twice on same bus
               const bool q_ComIpValid = this->CheckIpAddressIsValid(ou32_NodeIndex, u32_ItComInterface, c_Ip);

               //if conflict found abort check
               if (q_ComIpValid == false)
               {
                  *opq_IpInvalid = true;
                  if (opc_InvalidInterfaceIndices == NULL)
                  {
                     //not interested in details, we are finished here as we know there was at least one conflics
                     break;
                  }
                  else
                  {
                     opc_InvalidInterfaceIndices->push_back(u32_ItComInterface);
                  }
               }
            }
         }
      }

      if (opq_DataPoolsInvalid != NULL)
      {
         // check all datapools for errors
         uint32_t u32_Counter;
         bool q_DataPoolNameConflict;
         bool q_DataPoolNameInvalid;
         bool q_DataPoolListError;
         bool q_DataPoolListOrElementLengthError;
         bool q_ResultError = false;
         static std::map<std::vector<uint32_t>, bool> hc_PreviousCommChecks;
         static std::map<uint32_t, bool> hc_PreviousCommonChecks;

         for (u32_Counter = 0U;
              (u32_Counter < rc_CheckedNode.c_DataPools.size()) &&
              ((q_ResultError == false) || (opc_InvalidDataPoolIndices != NULL));
              ++u32_Counter)
         {
            const uint32_t u32_Hash = this->m_GetDataPoolHash(ou32_NodeIndex, u32_Counter);
            bool q_AlreadyAdded = false;
            bool q_Skip = false;
            //Com data pool:
            //Exception if bus connected
            //Note: this error cannot be computed inside the comm datapool as it requires the protocol
            // which is outside the datapool reach
            if (orq_AllowComDataPoolException == true)
            {
               const C_OscNodeDataPool & rc_DataPool = rc_CheckedNode.c_DataPools[u32_Counter];
               if (rc_DataPool.e_Type == C_OscNodeDataPool::eCOM)
               {
                  //Get Hash for all relevant data
                  const uint32_t u32_ProtocolHash = this->m_GetRelatedProtocolHash(ou32_NodeIndex, u32_Counter);
                  std::map<std::vector<uint32_t>, bool>::const_iterator c_It;
                  std::vector<uint32_t> c_Hashes;
                  c_Hashes.push_back(u32_Hash);
                  c_Hashes.push_back(u32_ProtocolHash);

                  //check basic: list and element count:
                  rc_CheckedNode.CheckErrorDataPoolNumListsAndElements(u32_Counter, q_DataPoolListOrElementLengthError);
                  if (q_DataPoolListOrElementLengthError == true)
                  {
                     q_ResultError = true;
                     q_AlreadyAdded = true;
                     if (opc_InvalidDataPoolIndices != NULL)
                     {
                        opc_InvalidDataPoolIndices->push_back(u32_Counter);
                     }
                  }

                  //Check if check was already performed in the past
                  c_It = hc_PreviousCommChecks.find(c_Hashes);
                  if (c_It == hc_PreviousCommChecks.end())
                  {
                     bool q_CurRes = false;
                     const C_OscCanProtocol * const pc_Protocol =
                        rc_CheckedNode.GetRelatedCanProtocolConst(u32_Counter);
                     if (pc_Protocol != NULL)
                     {
                        //Matching data pool to protocol
                        for (uint32_t u32_ItInterface = 0;
                             (u32_ItInterface < pc_Protocol->c_ComMessages.size()) &&
                             ((q_ResultError == false) || (opc_InvalidDataPoolIndices != NULL));
                             ++u32_ItInterface)
                        {
                           const C_OscCanMessageContainer & rc_MessageContainer =
                              pc_Protocol->c_ComMessages[u32_ItInterface];
                           //only check if not connected to bus
                           if (rc_MessageContainer.q_IsComProtocolUsedByInterface == false)
                           {
                              //Check both lists here
                              const C_OscNodeDataPoolList * const pc_TxList = C_OscCanProtocol::h_GetComListConst(
                                 rc_DataPool, u32_ItInterface, true);
                              const C_OscNodeDataPoolList * const pc_RxList = C_OscCanProtocol::h_GetComListConst(
                                 rc_DataPool, u32_ItInterface, false);
                              if ((pc_TxList != NULL) && (pc_RxList != NULL))
                              {
                                 // Parameter oq_CanOpenPdoSyncValid is true due to node mode and no CANopen protocol
                                 // is possible here and must not be checked
                                 if (rc_MessageContainer.CheckLocalError(
                                        *pc_TxList, *pc_RxList,
                                        C_OscCanProtocol::h_GetCanMessageValidSignalsDlcOffset(
                                           pc_Protocol->e_Type),
                                        C_OscCanProtocol::h_GetCanMessageSignalGapsValid(
                                           pc_Protocol->e_Type),
                                        C_OscCanProtocol::h_GetCanMessageSignalByteAlignmentRequired(
                                           pc_Protocol->e_Type),
                                        C_OscCanProtocol::h_GetCanMessageSignalsRequired(pc_Protocol->e_Type),
                                        true) == true)
                                 {
                                    q_ResultError = true;
                                    q_CurRes = true;
                                    q_AlreadyAdded = true;
                                    if (opc_InvalidDataPoolIndices != NULL)
                                    {
                                       opc_InvalidDataPoolIndices->push_back(u32_Counter);
                                    }
                                 }
                              }
                           }
                        }
                     }
                     //Append for possible reusing this result
                     hc_PreviousCommChecks[c_Hashes] = q_CurRes;
                  }
                  else
                  {
                     //Do not reset error
                     q_ResultError = q_ResultError || c_It->second;
                     if ((opc_InvalidDataPoolIndices != NULL) && (c_It->second))
                     {
                        opc_InvalidDataPoolIndices->push_back(u32_Counter);
                     }
                  }

                  q_Skip = true;
               }
            }
            //Default check
            if ((q_Skip == false) && (q_AlreadyAdded == false))
            {
               //Check if check was already performed in the past
               const std::map<uint32_t, bool>::const_iterator c_It = hc_PreviousCommonChecks.find(u32_Hash);
               if (c_It == hc_PreviousCommonChecks.end())
               {
                  rc_CheckedNode.CheckErrorDataPool(u32_Counter, &q_DataPoolNameConflict, &q_DataPoolNameInvalid,
                                                    &q_DataPoolListError, &q_DataPoolListOrElementLengthError, NULL);

                  if (((q_DataPoolNameConflict == true) || (q_DataPoolNameInvalid == true)) ||
                      (q_DataPoolListError == true) || (q_DataPoolListOrElementLengthError == true))
                  {
                     q_ResultError = true;
                     if (opc_InvalidDataPoolIndices != NULL)
                     {
                        opc_InvalidDataPoolIndices->push_back(u32_Counter);
                     }
                  }
                  //Append for possible reusing this result (without conflict checks)
                  if ((q_DataPoolNameInvalid == true) || (q_DataPoolListError == true))
                  {
                     hc_PreviousCommonChecks[u32_Hash] = true;
                  }
                  else
                  {
                     hc_PreviousCommonChecks[u32_Hash] = false;
                  }
               }
               else
               {
                  //ALWAYS do datapool name conflict check
                  rc_CheckedNode.CheckErrorDataPool(u32_Counter, &q_DataPoolNameConflict, NULL, NULL, NULL, NULL);

                  if (q_DataPoolNameConflict == true)
                  {
                     //Signal datapool conflict error
                     q_ResultError = true;
                  }
                  else
                  {
                     //Do not reset error
                     q_ResultError = q_ResultError || c_It->second;
                     if ((opc_InvalidDataPoolIndices != NULL) && (c_It->second))
                     {
                        opc_InvalidDataPoolIndices->push_back(u32_Counter);
                     }
                  }
               }
            }
         }
         *opq_DataPoolsInvalid = q_ResultError;
      }
      if (opq_ApplicationsInvalid != NULL)
      {
         *opq_ApplicationsInvalid = false;
         for (uint32_t u32_ItApp = 0; u32_ItApp < rc_CheckedNode.c_Applications.size(); ++u32_ItApp)
         {
            bool q_Valid;
            if (rc_CheckedNode.CheckApplicationProcessIdValid(u32_ItApp, q_Valid) == C_NO_ERR)
            {
               if (q_Valid == false)
               {
                  *opq_ApplicationsInvalid = true;
                  if (opc_InvalidApplicationIndices != NULL)
                  {
                     opc_InvalidApplicationIndices->push_back(u32_ItApp);
                  }
               }
            }
         }
      }
      if (opq_DomainsInvalid != NULL)
      {
         rc_CheckedNode.CheckHalcConfigValid(opq_DomainsInvalid, opc_InvalidDomainIndices);
      }

      // COMM protocol check
      if ((opq_CommMinSignalCountInvalid != NULL) || (opq_CommMaxSignalCountInvalid != NULL) ||
          (opq_CoPdoCountInvalid != NULL))
      {
         bool q_TempCommRxSignalCountInvalid = false;
         bool q_TempCommTxSignalCountInvalid = false;
         bool q_TempCoRxPdoCountInvalid = false;
         bool q_TempCoTxPdoCountInvalid = false;
         bool q_TempMinSignalCountInvalid = false;
         uint32_t u32_ProtCounter;

         if (opq_CommMinSignalCountInvalid != NULL)
         {
            *opq_CommMinSignalCountInvalid = false;
         }
         if (opq_CommMaxSignalCountInvalid != NULL)
         {
            *opq_CommMaxSignalCountInvalid = false;
         }
         if (opq_CoPdoCountInvalid != NULL)
         {
            *opq_CoPdoCountInvalid = false;
         }

         // And all protocols which are not connected to a bus
         for (u32_ProtCounter = 0U; u32_ProtCounter < C_OscCanProtocol::hc_ALL_PROTOCOLS.size();
              ++u32_ProtCounter)
         {
            rc_CheckedNode.CheckErrorCanProtocol(C_OscCanProtocol::hc_ALL_PROTOCOLS[u32_ProtCounter],
                                                 false,
                                                 q_TempCommRxSignalCountInvalid,
                                                 q_TempCommTxSignalCountInvalid,
                                                 q_TempCoRxPdoCountInvalid,
                                                 q_TempCoTxPdoCountInvalid, q_TempMinSignalCountInvalid);

            if (((opq_CommMinSignalCountInvalid != NULL) || (opq_CommMaxSignalCountInvalid != NULL)) &&
                ((q_TempCommRxSignalCountInvalid == true) ||
                 (q_TempCommTxSignalCountInvalid == true) ||
                 (q_TempMinSignalCountInvalid == true)))
            {
               // Merge TX and RX error
               if ((opq_CommMinSignalCountInvalid != NULL) && (q_TempMinSignalCountInvalid == true))
               {
                  *opq_CommMinSignalCountInvalid = true;
               }

               if ((opq_CommMaxSignalCountInvalid != NULL) && ((q_TempCommRxSignalCountInvalid == true) ||
                                                               (q_TempCommTxSignalCountInvalid == true)))
               {
                  *opq_CommMaxSignalCountInvalid = true;
               }

               if (opc_InvalidProtocolTypes != NULL)
               {
                  opc_InvalidProtocolTypes->push_back(C_OscCanProtocol::hc_ALL_PROTOCOLS[u32_ProtCounter]);
               }
            }
            if ((opq_CoPdoCountInvalid != NULL) &&
                ((q_TempCoRxPdoCountInvalid == true) ||
                 (q_TempCoTxPdoCountInvalid == true)))
            {
               // Merge TPDO and RPDO error
               *opq_CoPdoCountInvalid = true;
            }
         }
      }

      // CANopen specific check
      if ((opq_CoNodeIdInvalid != NULL) || (opq_CoHearbeatTimeInvalid != NULL))
      {
         std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_ItManager;
         bool q_TempCoNodeIdConflict;
         bool * pq_TempCoNodeIdConflict = NULL;
         bool q_TempCoManagerNodeIdInvalid;
         bool * pq_TempCoManagerNodeIdInvalid = NULL;
         bool q_TempCoDevicesNodeIdInvalid;
         bool * pq_TempCoDevicesNodeIdInvalid = NULL;
         bool q_TempCoHeartbeatInvalid;
         bool * pq_TempCoHeartbeatInvalid = NULL;

         // Temporary pointers needed for avoid overwriting previous results in the loop
         // and avoid running error checks of not wanted checks
         if (opq_CoNodeIdInvalid != NULL)
         {
            *opq_CoNodeIdInvalid = false;
            pq_TempCoNodeIdConflict = &q_TempCoNodeIdConflict;

            // Summing all CANopen Node ID conflicts into one flag
            pq_TempCoManagerNodeIdInvalid = &q_TempCoManagerNodeIdInvalid;
            pq_TempCoDevicesNodeIdInvalid = &q_TempCoDevicesNodeIdInvalid;
         }

         if (opq_CoHearbeatTimeInvalid != NULL)
         {
            *opq_CoHearbeatTimeInvalid = false;
            pq_TempCoHeartbeatInvalid = &q_TempCoHeartbeatInvalid;
         }

         for (c_ItManager = rc_CheckedNode.c_CanOpenManagers.begin();
              c_ItManager != rc_CheckedNode.c_CanOpenManagers.end(); ++c_ItManager)
         {
            c_ItManager->second.CheckErrorManager(pq_TempCoNodeIdConflict, pq_TempCoManagerNodeIdInvalid,
                                                  pq_TempCoDevicesNodeIdInvalid,
                                                  pq_TempCoHeartbeatInvalid, true);

            if ((opq_CoNodeIdInvalid != NULL) &&
                ((q_TempCoNodeIdConflict == true) ||
                 (q_TempCoManagerNodeIdInvalid == true) ||
                 (q_TempCoDevicesNodeIdInvalid == true)))
            {
               // Summing all CANopen Node ID conflicts into one flag
               // Error for CANopen Node ID detected
               *opq_CoNodeIdInvalid = true;
            }

            if ((opq_CoHearbeatTimeInvalid != NULL) &&
                (q_TempCoHeartbeatInvalid == true))
            {
               // Error for CANopen Node ID detected
               *opq_CoHearbeatTimeInvalid = true;
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
/*! \brief   Check error for bus & connected node data pools

   \param[in]   ou32_BusIndex          Bus index
   \param[out]  opq_NameConflict       Name conflict
   \param[out]  opq_NameInvalid        Name not usable as variable
   \param[out]  opq_IdInvalid          Id out of range
   \param[out]  opq_DataPoolsInvalid   An error found for a data pool

   \return
   C_NO_ERR Done
   C_RANGE  Bus does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSystemDefinition::CheckErrorBus(const uint32_t ou32_BusIndex, bool * const opq_NameConflict,
                                             bool * const opq_NameInvalid, bool * const opq_IdInvalid,
                                             bool * const opq_DataPoolsInvalid) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (opq_NameConflict != NULL)
   {
      *opq_NameConflict = false;
      if (ou32_BusIndex < this->c_Buses.size())
      {
         const C_OscSystemBus & rc_CheckedBus = this->c_Buses[ou32_BusIndex];
         for (uint32_t u32_ItBus = 0; u32_ItBus < this->c_Buses.size(); ++u32_ItBus)
         {
            if (u32_ItBus != ou32_BusIndex)
            {
               const C_OscSystemBus & rc_CurrentBus = this->c_Buses[u32_ItBus];
               if (rc_CheckedBus.c_Name.LowerCase() == rc_CurrentBus.c_Name.LowerCase())
               {
                  *opq_NameConflict = true;
               }
            }
         }
      }
   }
   if (opq_NameInvalid != NULL)
   {
      *opq_NameInvalid = false;
      if (ou32_BusIndex < this->c_Buses.size())
      {
         const C_OscSystemBus & rc_CheckedBus = this->c_Buses[ou32_BusIndex];
         *opq_NameInvalid = !C_OscUtils::h_CheckValidCeName(rc_CheckedBus.c_Name);
      }
   }
   if (opq_IdInvalid != NULL)
   {
      if (ou32_BusIndex < this->c_Buses.size())
      {
         const C_OscSystemBus & rc_SystemBus = this->c_Buses[ou32_BusIndex];
         if (rc_SystemBus.q_UseableForRouting == true)
         {
            *opq_IdInvalid = rc_SystemBus.CheckErrorBusId();
            //Duplicate check
            if (*opq_IdInvalid == false)
            {
               for (uint32_t u32_ItBus = 0; u32_ItBus < this->c_Buses.size(); ++u32_ItBus)
               {
                  if (u32_ItBus != ou32_BusIndex)
                  {
                     const C_OscSystemBus & rc_Bus = this->c_Buses[u32_ItBus];
                     if ((rc_Bus.q_UseableForRouting == true) && (rc_SystemBus.u8_BusId == rc_Bus.u8_BusId))
                     {
                        *opq_IdInvalid = true;
                     }
                  }
               }
            }
         }
         else
         {
            *opq_IdInvalid = false;
         }
      }
      else
      {
         *opq_IdInvalid = false;
         s32_Retval = C_RANGE;
      }
   }
   if ((opq_DataPoolsInvalid != NULL) && (s32_Retval == C_NO_ERR))
   {
      std::vector<uint32_t> c_NodeIndexes;
      std::vector<uint32_t> c_InterfaceIndexes;
      *opq_DataPoolsInvalid = false;
      this->GetNodeIndexesOfBus(ou32_BusIndex, c_NodeIndexes, c_InterfaceIndexes);
      if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
      {
         for (uint32_t u32_ItNode = 0U;
              ((u32_ItNode < c_NodeIndexes.size()) && (*opq_DataPoolsInvalid == false)) && (s32_Retval == C_NO_ERR);
              ++u32_ItNode)
         {
            if (c_NodeIndexes[u32_ItNode] < this->c_Nodes.size())
            {
               const C_OscNode & rc_Node = this->c_Nodes[c_NodeIndexes[u32_ItNode]];
               // check all datapools for errors
               for (uint32_t u32_ItDataPool = 0U;
                    (u32_ItDataPool < rc_Node.c_DataPools.size()) && (*opq_DataPoolsInvalid == false);
                    ++u32_ItDataPool)
               {
                  const C_OscNodeDataPool & rc_DataPool = rc_Node.c_DataPools[u32_ItDataPool];
                  //Only com data pool:
                  if (rc_DataPool.e_Type == C_OscNodeDataPool::eCOM)
                  {
                     for (uint32_t u32_ItProtocol = 0;
                          ((u32_ItProtocol < rc_Node.c_ComProtocols.size()) && (*opq_DataPoolsInvalid == false)) &&
                          (s32_Retval == C_NO_ERR);
                          ++u32_ItProtocol)
                     {
                        const C_OscCanProtocol & rc_Protocol = rc_Node.c_ComProtocols[u32_ItProtocol];
                        if (rc_Protocol.u32_DataPoolIndex == u32_ItDataPool)
                        {
                           //Matching data pool to protocol
                           if (c_InterfaceIndexes[u32_ItNode] < rc_Protocol.c_ComMessages.size())
                           {
                              const C_OscCanMessageContainer & rc_MessageContainer =
                                 rc_Protocol.c_ComMessages[c_InterfaceIndexes[u32_ItNode]];
                              //only check if connected to bus
                              if (rc_MessageContainer.q_IsComProtocolUsedByInterface == true)
                              {
                                 //Check both lists here
                                 const C_OscNodeDataPoolList * const pc_TxList =
                                    C_OscCanProtocol::h_GetComListConst(
                                       rc_DataPool,
                                       c_InterfaceIndexes[u32_ItNode],
                                       true);
                                 const C_OscNodeDataPoolList * const pc_RxList =
                                    C_OscCanProtocol::h_GetComListConst(
                                       rc_DataPool,
                                       c_InterfaceIndexes[u32_ItNode],
                                       false);
                                 if ((pc_TxList != NULL) && (pc_RxList != NULL))
                                 {
                                    // Parameter oq_CanOpenPdoSyncValid is true in case of no CANopen protocol
                                    // to avoid a check for this scenario
                                    bool q_CanOpenPdoSyncValid = true;
                                    if (rc_Protocol.e_Type == C_OscCanProtocol::eCAN_OPEN)
                                    {
                                       // Special case CANopen
                                       // Get the CANopen Manager
                                       const std::map<uint8_t,
                                                      C_OscCanOpenManagerInfo>::const_iterator c_ItCoManager =
                                          rc_Node.c_CanOpenManagers.find(
                                             rc_Node.c_Properties.c_ComInterfaces[
                                                c_InterfaceIndexes[u32_ItNode]].u8_InterfaceNumber);

                                       // Check if it is the CANopen Manager of this bus
                                       if (c_ItCoManager != rc_Node.c_CanOpenManagers.end())
                                       {
                                          // When PDO SYNC message is produces, a configured TX method with PDO SYNC
                                          // is valid
                                          q_CanOpenPdoSyncValid = c_ItCoManager->second.q_ProduceSyncMessage;
                                       }
                                    }

                                    *opq_DataPoolsInvalid = rc_MessageContainer.CheckLocalError(
                                       *pc_TxList, *pc_RxList,
                                       C_OscCanProtocol::h_GetCanMessageValidSignalsDlcOffset(rc_Protocol.e_Type),
                                       C_OscCanProtocol::h_GetCanMessageSignalGapsValid(rc_Protocol.e_Type),
                                       C_OscCanProtocol::h_GetCanMessageSignalByteAlignmentRequired(rc_Protocol.e_Type),
                                       C_OscCanProtocol::h_GetCanMessageSignalsRequired(rc_Protocol.e_Type),
                                       q_CanOpenPdoSyncValid);
                                    //Check global error
                                    if (*opq_DataPoolsInvalid == false)
                                    {
                                       C_OscCanMessageIdentificationIndices c_MessageId;
                                       bool q_MessageValid = true;

                                       c_MessageId.u32_NodeIndex = c_NodeIndexes[u32_ItNode];
                                       c_MessageId.u32_InterfaceIndex = c_InterfaceIndexes[u32_ItNode];
                                       c_MessageId.e_ComProtocol = rc_Protocol.e_Type;
                                       //Tx
                                       c_MessageId.q_MessageIsTx = true;
                                       for (uint32_t u32_ItMessage = 0;
                                            ((u32_ItMessage < rc_MessageContainer.c_TxMessages.size()) &&
                                             (q_MessageValid == true)) &&
                                            (s32_Retval == C_NO_ERR);
                                            ++u32_ItMessage)
                                       {
                                          c_MessageId.u32_MessageIndex = u32_ItMessage;
                                          const C_OscCanMessage & rc_Message =
                                             rc_MessageContainer.c_TxMessages[u32_ItMessage];
                                          //Name
                                          s32_Retval = this->CheckMessageNameBus(ou32_BusIndex, rc_Message.c_Name,
                                                                                 q_MessageValid, &c_MessageId);
                                          //Id
                                          if ((s32_Retval == C_NO_ERR) && (q_MessageValid == true))
                                          {
                                             s32_Retval = this->CheckMessageIdBus(ou32_BusIndex,
                                                                                  C_OscCanMessageUniqueId(rc_Message.
                                                                                                          u32_CanId,
                                                                                                          rc_Message.
                                                                                                          q_IsExtended),
                                                                                  q_MessageValid, &c_MessageId);
                                          }
                                       }
                                       //Rx
                                       c_MessageId.q_MessageIsTx = false;
                                       for (uint32_t u32_ItMessage = 0;
                                            ((u32_ItMessage < rc_MessageContainer.c_RxMessages.size()) &&
                                             (q_MessageValid == true)) &&
                                            (s32_Retval == C_NO_ERR);
                                            ++u32_ItMessage)
                                       {
                                          const C_OscCanMessage & rc_Message =
                                             rc_MessageContainer.c_RxMessages[u32_ItMessage];
                                          //Name
                                          s32_Retval = this->CheckMessageNameBus(ou32_BusIndex, rc_Message.c_Name,
                                                                                 q_MessageValid, &c_MessageId);
                                          //Id
                                          if ((s32_Retval == C_NO_ERR) && (q_MessageValid == true))
                                          {
                                             s32_Retval = this->CheckMessageIdBus(ou32_BusIndex,
                                                                                  C_OscCanMessageUniqueId(rc_Message.
                                                                                                          u32_CanId,
                                                                                                          rc_Message.
                                                                                                          q_IsExtended),
                                                                                  q_MessageValid, &c_MessageId);
                                          }
                                       }
                                       if (q_MessageValid == false)
                                       {
                                          *opq_DataPoolsInvalid = true;
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
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if message id valid

   \param[in]   ou32_BusIndex    Bus index
   \param[in]   orc_MessageId    Message id
   \param[out]  orq_Valid        Flag if valid
   \param[in]   opc_SkipMessage  Optional parameter to skip one message
                                 (Use-case: skip current message to avoid conflict with itself)

   \return
   C_NO_ERR Done
   C_RANGE  Bus does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSystemDefinition::CheckMessageIdBus(const uint32_t ou32_BusIndex,
                                                 const C_OscCanMessageUniqueId & orc_MessageId, bool & orq_Valid,
                                                 const C_OscCanMessageIdentificationIndices * const opc_SkipMessage)
const
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_BusIndex < this->c_Buses.size())
   {
      std::vector<uint32_t> c_NodeIndices;
      std::vector<uint32_t> c_InterfaceIndices;
      //Get all connected nodes
      GetNodeIndexesOfBus(ou32_BusIndex, c_NodeIndices, c_InterfaceIndices);
      orq_Valid = true;
      if (c_NodeIndices.size() == c_InterfaceIndices.size())
      {
         //Parse connected node with connected interface
         for (uint32_t u32_ItNode = 0; u32_ItNode < c_NodeIndices.size(); ++u32_ItNode)
         {
            //Check node index
            if (c_NodeIndices[u32_ItNode] < this->c_Nodes.size())
            {
               const C_OscNode & rc_Node = this->c_Nodes[c_NodeIndices[u32_ItNode]];
               bool q_Valid;
               //Check if skip possible
               if ((opc_SkipMessage != NULL) && (opc_SkipMessage->u32_NodeIndex == c_NodeIndices[u32_ItNode]))
               {
                  rc_Node.CheckMessageId(c_InterfaceIndices[u32_ItNode], orc_MessageId, q_Valid,
                                         &opc_SkipMessage->e_ComProtocol,
                                         &opc_SkipMessage->u32_InterfaceIndex,
                                         &opc_SkipMessage->q_MessageIsTx,
                                         &opc_SkipMessage->u32_MessageIndex);
               }
               else
               {
                  //Check message id for one node
                  rc_Node.CheckMessageId(c_InterfaceIndices[u32_ItNode], orc_MessageId, q_Valid);
               }
               //Set invalid if necessary
               if (q_Valid == false)
               {
                  orq_Valid = false;
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if message id valid

   \param[in]   ou32_BusIndex    Bus index
   \param[in]   orc_MessageName  Message name
   \param[out]  orq_Valid        Flag if valid
   \param[in]   opc_SkipMessage  Optional parameter to skip one message
                                 (Use-case: skip current message to avoid conflict with itself)

   \return
   C_NO_ERR Done
   C_RANGE  Bus does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSystemDefinition::CheckMessageNameBus(const uint32_t ou32_BusIndex, const C_SclString & orc_MessageName,
                                                   bool & orq_Valid,
                                                   const C_OscCanMessageIdentificationIndices * const opc_SkipMessage)
const
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_BusIndex < this->c_Buses.size())
   {
      std::vector<uint32_t> c_NodeIndices;
      std::vector<uint32_t> c_InterfaceIndices;
      //Get all connected nodes
      GetNodeIndexesOfBus(ou32_BusIndex, c_NodeIndices, c_InterfaceIndices);
      orq_Valid = true;
      if (c_NodeIndices.size() == c_InterfaceIndices.size())
      {
         //Parse connected node with connected interface
         for (uint32_t u32_ItNode = 0; u32_ItNode < c_NodeIndices.size(); ++u32_ItNode)
         {
            //Check node index
            if (c_NodeIndices[u32_ItNode] < this->c_Nodes.size())
            {
               const C_OscNode & rc_Node = this->c_Nodes[c_NodeIndices[u32_ItNode]];
               bool q_Valid;
               //Check if skip possible
               if ((opc_SkipMessage != NULL) && (opc_SkipMessage->u32_NodeIndex == c_NodeIndices[u32_ItNode]))
               {
                  rc_Node.CheckMessageName(c_InterfaceIndices[u32_ItNode], orc_MessageName, q_Valid,
                                           &opc_SkipMessage->e_ComProtocol,
                                           &opc_SkipMessage->u32_InterfaceIndex,
                                           &opc_SkipMessage->q_MessageIsTx,
                                           &opc_SkipMessage->u32_MessageIndex);
               }
               else
               {
                  //Check message id for one node
                  rc_Node.CheckMessageName(c_InterfaceIndices[u32_ItNode], orc_MessageName, q_Valid);
               }
               //Set invalid if necessary
               if (q_Valid == false)
               {
                  orq_Valid = false;
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if specified messages match

   \param[in]   orc_MessageId1               First message identification indices
   \param[in]   orc_MessageId2               Second message identification indices
   \param[out]  orq_IsMatch                  Flag if messages match
   \param[in]   oq_IgnoreMessageDirection    Flag to compare messages without message direction check

   \return
   C_NO_ERR Done
   C_RANGE  Nodes or Datapools or Lists or Messages do not exist
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSystemDefinition::CheckMessageMatch(const C_OscCanMessageIdentificationIndices & orc_MessageId1,
                                                 const C_OscCanMessageIdentificationIndices & orc_MessageId2,
                                                 bool & orq_IsMatch, const bool oq_IgnoreMessageDirection) const
{
   int32_t s32_Retval = C_NO_ERR;

   orq_IsMatch = true;
   //Compare tx first
   if (!(orc_MessageId1 == orc_MessageId2))
   {
      if ((oq_IgnoreMessageDirection == false) &&
          ((orc_MessageId1.q_MessageIsTx == true) && (orc_MessageId1.q_MessageIsTx == orc_MessageId2.q_MessageIsTx)))
      {
         orq_IsMatch = false;
      }
      else
      {
         if ((orc_MessageId1.u32_NodeIndex < this->c_Nodes.size()) &&
             (orc_MessageId2.u32_NodeIndex < this->c_Nodes.size()))
         {
            const C_OscNode & rc_Node1 = this->c_Nodes[orc_MessageId1.u32_NodeIndex];
            const C_OscNode & rc_Node2 = this->c_Nodes[orc_MessageId2.u32_NodeIndex];
            const C_OscNodeDataPool * const pc_DataPool1 =
               rc_Node1.GetComDataPoolConst(orc_MessageId1.e_ComProtocol, orc_MessageId1.u32_DatapoolIndex);
            const C_OscNodeDataPool * const pc_DataPool2 =
               rc_Node2.GetComDataPoolConst(orc_MessageId2.e_ComProtocol, orc_MessageId2.u32_DatapoolIndex);
            const C_OscCanProtocol * const pc_Protocol1 =
               rc_Node1.GetCanProtocolConst(orc_MessageId1.e_ComProtocol, orc_MessageId1.u32_DatapoolIndex);
            const C_OscCanProtocol * const pc_Protocol2 =
               rc_Node2.GetCanProtocolConst(orc_MessageId2.e_ComProtocol, orc_MessageId2.u32_DatapoolIndex);

            if (((pc_DataPool1 != NULL) && (pc_DataPool2 != NULL)) &&
                ((pc_Protocol1 != NULL) && (pc_Protocol2 != NULL)))
            {
               const C_OscNodeDataPoolList * const pc_List1 =
                  C_OscCanProtocol::h_GetComListConst(*pc_DataPool1, orc_MessageId1.u32_InterfaceIndex,
                                                      orc_MessageId1.q_MessageIsTx);
               const C_OscNodeDataPoolList * const pc_List2 =
                  C_OscCanProtocol::h_GetComListConst(*pc_DataPool2, orc_MessageId2.u32_InterfaceIndex,
                                                      orc_MessageId2.q_MessageIsTx);

               if (((orc_MessageId1.u32_InterfaceIndex < pc_Protocol1->c_ComMessages.size()) &&
                    (orc_MessageId2.u32_InterfaceIndex < pc_Protocol2->c_ComMessages.size())) &&
                   ((pc_List1 != NULL) && (pc_List2 != NULL)))
               {
                  const C_OscCanMessageContainer & rc_MessageContainer1 =
                     pc_Protocol1->c_ComMessages[orc_MessageId1.u32_InterfaceIndex];
                  const C_OscCanMessageContainer & rc_MessageContainer2 =
                     pc_Protocol2->c_ComMessages[orc_MessageId2.u32_InterfaceIndex];
                  const std::vector<C_OscCanMessage> & rc_Messages1 = rc_MessageContainer1.GetMessagesConst(
                     orc_MessageId1.q_MessageIsTx);
                  const std::vector<C_OscCanMessage> & rc_Messages2 = rc_MessageContainer2.GetMessagesConst(
                     orc_MessageId2.q_MessageIsTx);
                  if ((orc_MessageId1.u32_MessageIndex < rc_Messages1.size()) &&
                      (orc_MessageId2.u32_MessageIndex < rc_Messages2.size()))
                  {
                     const C_OscCanMessage & rc_Message1 = rc_Messages1[orc_MessageId1.u32_MessageIndex];
                     const C_OscCanMessage & rc_Message2 = rc_Messages2[orc_MessageId2.u32_MessageIndex];
                     //Compare messages
                     if (rc_Message1.c_Name != rc_Message2.c_Name)
                     {
                        orq_IsMatch = false;
                     }
                     if (rc_Message1.c_Comment != rc_Message2.c_Comment)
                     {
                        orq_IsMatch = false;
                     }
                     if (rc_Message1.u32_CanId != rc_Message2.u32_CanId)
                     {
                        orq_IsMatch = false;
                     }
                     if (rc_Message1.q_IsExtended != rc_Message2.q_IsExtended)
                     {
                        orq_IsMatch = false;
                     }
                     if (rc_Message1.u16_Dlc != rc_Message2.u16_Dlc)
                     {
                        orq_IsMatch = false;
                     }
                     if (rc_Message1.e_TxMethod != rc_Message2.e_TxMethod)
                     {
                        orq_IsMatch = false;
                     }
                     else
                     {
                        //only check cycle time if both cyclic
                        if (rc_Message1.e_TxMethod == C_OscCanMessage::eTX_METHOD_CYCLIC)
                        {
                           if (rc_Message1.u32_CycleTimeMs != rc_Message2.u32_CycleTimeMs)
                           {
                              orq_IsMatch = false;
                           }
                        }
                     }
                     //Compare signals
                     if (rc_Message1.c_Signals.size() != rc_Message2.c_Signals.size())
                     {
                        orq_IsMatch = false;
                     }
                     else
                     {
                        for (uint32_t u32_ItSignal = 0;
                             (u32_ItSignal < rc_Message1.c_Signals.size()) && (orq_IsMatch == true);
                             ++u32_ItSignal)
                        {
                           const C_OscCanSignal & rc_Signal1 = rc_Message1.c_Signals[u32_ItSignal];
                           const C_OscCanSignal & rc_Signal2 = rc_Message2.c_Signals[u32_ItSignal];
                           const C_OscNodeDataPoolListElement * const pc_SignalData1 =
                              pc_Protocol1->GetComListElementConst(
                                 *pc_DataPool1,
                                 orc_MessageId1.u32_InterfaceIndex,
                                 orc_MessageId1.q_MessageIsTx, orc_MessageId1.u32_MessageIndex, u32_ItSignal);
                           const C_OscNodeDataPoolListElement * const pc_SignalData2 =
                              pc_Protocol2->GetComListElementConst(
                                 *pc_DataPool2,
                                 orc_MessageId2.u32_InterfaceIndex,
                                 orc_MessageId2.q_MessageIsTx, orc_MessageId2.u32_MessageIndex, u32_ItSignal);
                           if ((pc_SignalData1 != NULL) && (pc_SignalData2 != NULL))
                           {
                              if (pc_SignalData1->c_Name != pc_SignalData2->c_Name)
                              {
                                 orq_IsMatch = false;
                              }
                              if (pc_SignalData1->c_Comment != pc_SignalData2->c_Comment)
                              {
                                 orq_IsMatch = false;
                              }
                              if (pc_SignalData1->GetType() != pc_SignalData2->GetType())
                              {
                                 orq_IsMatch = false;
                              }
                              if (pc_SignalData1->c_MinValue != pc_SignalData2->c_MinValue)
                              {
                                 orq_IsMatch = false;
                              }
                              if (pc_SignalData1->c_MaxValue != pc_SignalData2->c_MaxValue)
                              {
                                 orq_IsMatch = false;
                              }
                              if (C_OscUtils::h_IsFloat64NearlyEqual(pc_SignalData1->f64_Factor,
                                                                     pc_SignalData2->f64_Factor) == false)
                              {
                                 orq_IsMatch = false;
                              }
                              if (C_OscUtils::h_IsFloat64NearlyEqual(pc_SignalData1->f64_Offset,
                                                                     pc_SignalData2->f64_Offset) == false)
                              {
                                 orq_IsMatch = false;
                              }
                              //Init
                              if (pc_SignalData1->c_DataSetValues.size() != pc_SignalData2->c_DataSetValues.size())
                              {
                                 orq_IsMatch = false;
                              }
                              else
                              {
                                 tgl_assert(pc_SignalData1->c_DataSetValues.size() == 1);
                                 if (pc_SignalData1->c_DataSetValues.size() >= 1)
                                 {
                                    //Check init value
                                    if (pc_SignalData1->c_DataSetValues[0] != pc_SignalData2->c_DataSetValues[0])
                                    {
                                       orq_IsMatch = false;
                                    }
                                 }
                              }
                              if (pc_SignalData1->c_Unit != pc_SignalData2->c_Unit)
                              {
                                 orq_IsMatch = false;
                              }
                           }
                           else
                           {
                              orq_IsMatch = false;
                           }
                           if (rc_Signal1.u16_ComBitLength != rc_Signal2.u16_ComBitLength)
                           {
                              orq_IsMatch = false;
                           }
                           if (rc_Signal1.e_ComByteOrder != rc_Signal2.e_ComByteOrder)
                           {
                              orq_IsMatch = false;
                           }
                           if (rc_Signal1.u16_ComBitStart != rc_Signal2.u16_ComBitStart)
                           {
                              orq_IsMatch = false;
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
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns all node indexes which are connected to the bus

   \param[in]   ou32_BusIndex          Bus index
   \param[out]  orc_NodeIndexes        Vector with all node ids which are connected to the bus
   \param[out]  orc_InterfaceIndexes   Vector with all node interface ids which are connected to the bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSystemDefinition::GetNodeIndexesOfBus(const uint32_t ou32_BusIndex, std::vector<uint32_t> & orc_NodeIndexes,
                                                std::vector<uint32_t> & orc_InterfaceIndexes) const
{
   m_GetNodeAndComDpIndexesOfBus(ou32_BusIndex, NULL, orc_NodeIndexes, orc_InterfaceIndexes, NULL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns all node and Datapool indexes which are connected to the bus

   \param[in]   ou32_BusIndex          Bus index
   \param[out]  orc_NodeIndexes        Vector with all node ids which are connected to the bus
   \param[out]  orc_InterfaceIndexes   Vector with all node interface ids which are connected to the bus
   \param[out]  orc_DatapoolIndexes    Vector with all Datapool ids which are connected to the bus and are associated
                                       to the protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSystemDefinition::GetNodeAndComDpIndexesOfBus(const uint32_t ou32_BusIndex,
                                                        std::vector<uint32_t> & orc_NodeIndexes,
                                                        std::vector<uint32_t> & orc_InterfaceIndexes,
                                                        std::vector<uint32_t> & orc_DatapoolIndexes) const
{
   m_GetNodeAndComDpIndexesOfBus(ou32_BusIndex, NULL, orc_NodeIndexes, orc_InterfaceIndexes,
                                 &orc_DatapoolIndexes);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns all node and Datapool indexes which are connected to the bus and have a specific protocol type

   \param[in]   ou32_BusIndex          Bus index
   \param[in]   ore_ComProtocol        Specific protocol
   \param[out]  orc_NodeIndexes        Vector with all node ids which are connected to the bus
   \param[out]  orc_InterfaceIndexes   Vector with all node interface ids which are connected to the bus
   \param[out]  orc_DatapoolIndexes    Vector with all Datapool ids which are connected to the bus and are associated
                                       to the protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSystemDefinition::GetNodeAndComDpIndexesOfBus(const uint32_t ou32_BusIndex,
                                                        const C_OscCanProtocol::E_Type & ore_ComProtocol,
                                                        std::vector<uint32_t> & orc_NodeIndexes,
                                                        std::vector<uint32_t> & orc_InterfaceIndexes,
                                                        std::vector<uint32_t> & orc_DatapoolIndexes) const
{
   m_GetNodeAndComDpIndexesOfBus(ou32_BusIndex, &ore_ComProtocol, orc_NodeIndexes, orc_InterfaceIndexes,
                                 &orc_DatapoolIndexes);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append a new node at the end of the nodes.

   * appends node at the end of the node list
   * updates pointer to device definition

   Caller is responsible to pass a node referring to a "c_DeviceType" that is part of "hc_Devices".
   Otherwise the behavior is undefined (function will throw an assertion)

   \param[in,out]  orc_Node            node value (out: with added pointer to device type)
   \param[in]      orc_SubDeviceName   Sub device name
   \param[in]      orc_MainDeviceName  Main device name (empty if none)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSystemDefinition::AddNode(C_OscNode & orc_Node, const stw::scl::C_SclString & orc_SubDeviceName,
                                    const stw::scl::C_SclString & orc_MainDeviceName)
{
   const stw::scl::C_SclString c_SubDeviceName =
      orc_SubDeviceName.IsEmpty() ? orc_Node.c_DeviceType : orc_SubDeviceName;

   orc_Node.pc_DeviceDefinition = C_OscSystemDefinition::hc_Devices.LookForDevice(c_SubDeviceName,
                                                                                  orc_MainDeviceName,
                                                                                  orc_Node.u32_SubDeviceIndex);
   tgl_assert(orc_Node.pc_DeviceDefinition != NULL);
   this->c_Nodes.push_back(orc_Node);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append a new node squad and its sub nodes at the end of the node squads and nodes.

   * appends squad node at the end of the squad node list
   * appends nodes at the end of the node list
   * updates pointer to device definition

   Caller is responsible to pass nodes referring to a "c_DeviceType" that is part of "hc_Devices".
   Otherwise the behavior is undefined (function will throw an assertion)

   \param[in,out]  orc_Nodes           nodes values (out: with added pointer to device type)
   \param[in]      orc_SubDeviceNames  Sub device names
   \param[in]      orc_MainDeviceName  Main device name (empty if none)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSystemDefinition::AddNodeSquad(std::vector<C_OscNode> & orc_Nodes,
                                         const std::vector<stw::scl::C_SclString> & orc_SubDeviceNames,
                                         const stw::scl::C_SclString & orc_MainDeviceName)
{
   tgl_assert(orc_SubDeviceNames.size() == orc_SubDeviceNames.size());
   if (orc_SubDeviceNames.size() == orc_SubDeviceNames.size())
   {
      uint32_t u32_Counter;

      C_OscNodeSquad c_NewNodeSquad;

      tgl_assert(orc_Nodes.size() > 0);

      for (u32_Counter = 0U; u32_Counter < orc_Nodes.size(); ++u32_Counter)
      {
         const uint32_t u32_NodeIndex = static_cast<uint32_t>(this->c_Nodes.size());

         this->AddNode(orc_Nodes[u32_Counter], orc_SubDeviceNames[u32_Counter], orc_MainDeviceName);
         c_NewNodeSquad.c_SubNodeIndexes.push_back(u32_NodeIndex);
      }

      this->c_NodeSquads.push_back(c_NewNodeSquad);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove node

   In case of a node which is a sub node of a squad node, the entire squad node will be removed

   \param[in]  ou32_NodeIndex    Node index (0 -> first node)

   \return
   C_NO_ERR Done
   C_RANGE  Node index invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSystemDefinition::DeleteNode(const uint32_t ou32_NodeIndex)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->c_Nodes.size())
   {
      uint32_t u32_SquadNodeIndexToDelete = 0U;

      int32_t s32_NodeIndexToDeleteCounter;

      std::vector<uint32_t> c_AllNodeIndexToRemove;
      const int32_t s32_ReturnSquadNode = this->GetNodeSquadIndexWithNodeIndex(
         ou32_NodeIndex,
         u32_SquadNodeIndexToDelete);

      if (s32_ReturnSquadNode == C_NO_ERR)
      {
         // Get all node indexes of all sub nodes of the node squad
         c_AllNodeIndexToRemove = this->c_NodeSquads[u32_SquadNodeIndexToDelete].c_SubNodeIndexes;
      }
      else
      {
         // No squad node
         c_AllNodeIndexToRemove.push_back(ou32_NodeIndex);
      }

      // Start with the last index due to the highest index. Avoiding problems with deleting and changing orders for
      // the other node index
      for (s32_NodeIndexToDeleteCounter = (static_cast<int32_t>(c_AllNodeIndexToRemove.size()) - 1);
           s32_NodeIndexToDeleteCounter >= 0;
           --s32_NodeIndexToDeleteCounter)
      {
         uint32_t u32_NodeSquadCounter;
         const uint32_t u32_CurrentNodeIndex =  c_AllNodeIndexToRemove[s32_NodeIndexToDeleteCounter];

         this->c_Nodes.erase(this->c_Nodes.begin() + u32_CurrentNodeIndex);

         // Synchronizing the other node squads
         for (u32_NodeSquadCounter = 0U; u32_NodeSquadCounter < this->c_NodeSquads.size(); ++u32_NodeSquadCounter)
         {
            // No adaption of the node squad which will be deleted necessary
            if ((s32_ReturnSquadNode != C_NO_ERR) ||
                (u32_NodeSquadCounter != u32_SquadNodeIndexToDelete))
            {
               uint32_t u32_NodexIndexToSyncCounter;
               C_OscNodeSquad & rc_NodeSquad = this->c_NodeSquads[u32_NodeSquadCounter];

               for (u32_NodexIndexToSyncCounter =
                       0U; u32_NodexIndexToSyncCounter < rc_NodeSquad.c_SubNodeIndexes.size();
                    ++u32_NodexIndexToSyncCounter)
               {
                  uint32_t & ru32_SubNodeIndex = rc_NodeSquad.c_SubNodeIndexes[u32_NodexIndexToSyncCounter];
                  if (ru32_SubNodeIndex > u32_CurrentNodeIndex)
                  {
                     --ru32_SubNodeIndex;
                  }
               }
            }
         }
      }

      if (s32_ReturnSquadNode == C_NO_ERR)
      {
         // Remove the node squad if the node to delete is part of a node squad
         this->c_NodeSquads.erase(this->c_NodeSquads.begin() + u32_SquadNodeIndexToDelete);
      }

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the name for a node or its squad if it is part of one

   In case of a squad node, the new name will be the new base name of the squad. All sub nodes will be get
   its new node name by the node squad

   \param[in]  ou32_NodeIndex    Index of node which gets a new name
   \param[in]  orc_NodeName      New name of node

   \retval   C_NO_ERR   Node squad with specific sub node with node index found
   \retval   C_RANGE    No node squad found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSystemDefinition::SetNodeName(const uint32_t ou32_NodeIndex, const C_SclString & orc_NodeName)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->c_Nodes.size())
   {
      uint32_t u32_SquadIndex = 0U;
      const int32_t s32_SquadReturn = this->GetNodeSquadIndexWithNodeIndex(ou32_NodeIndex, u32_SquadIndex);

      if (s32_SquadReturn == C_NO_ERR)
      {
         // Node is sub node of a squad. Name will be set for all sub nodes based on the new base name and the node
         // specific part
         s32_Return = this->c_NodeSquads[u32_SquadIndex].SetBaseName(this->c_Nodes, orc_NodeName);
      }
      else
      {
         // Normal node, simple assignment
         this->c_Nodes[ou32_NodeIndex].c_Properties.c_Name = orc_NodeName;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Search the node squad with a specific node index as potential sub node

   \param[in]   ou32_NodeIndex         Searched node index of a sub node of a squad node
   \param[out]  oru32_NodeSquadIndex   Found squad node index if return value is C_NO_ERR

   \retval   C_NO_ERR   Node squad with specific sub node with node index found
   \retval   C_RANGE    No node squad found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSystemDefinition::GetNodeSquadIndexWithNodeIndex(const uint32_t ou32_NodeIndex,
                                                              uint32_t & oru32_NodeSquadIndex) const
{
   int32_t s32_Return = C_RANGE;

   if (C_OscNodeSquad::h_CheckIsMultiDevice(ou32_NodeIndex, this->c_NodeSquads, &oru32_NodeSquadIndex))
   {
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get hash for datapool

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index

   \return
   Hash for datapool
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscSystemDefinition::m_GetDataPoolHash(const uint32_t ou32_NodeIndex,
                                                  const uint32_t ou32_DataPoolIndex) const
{
   uint32_t u32_Retval = 0xFFFFFFFFUL;

   if (ou32_NodeIndex < this->c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->c_Nodes[ou32_NodeIndex];
      if (ou32_DataPoolIndex < rc_Node.c_DataPools.size())
      {
         const C_OscNodeDataPool & rc_DataPool = rc_Node.c_DataPools[ou32_DataPoolIndex];
         rc_DataPool.CalcHash(u32_Retval);
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get hash for protocol

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index

   \return
   Hash for protocol
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscSystemDefinition::m_GetRelatedProtocolHash(const uint32_t ou32_NodeIndex,
                                                         const uint32_t ou32_DataPoolIndex) const
{
   uint32_t u32_Retval = 0xFFFFFFFFUL;

   if (ou32_NodeIndex < this->c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->c_Nodes[ou32_NodeIndex];
      const C_OscCanProtocol * const pc_ProToCol = rc_Node.GetRelatedCanProtocolConst(ou32_DataPoolIndex);
      if (pc_ProToCol != NULL)
      {
         pc_ProToCol->CalcHash(u32_Retval);
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns all node and Datapool indexes which are connected to the bus

   \param[in]   ou32_BusIndex          Bus index
   \param[in]   ope_ComProtocol        Optional: Specific protocol
   \param[out]  orc_NodeIndexes        Vector with all node ids which are connected to the bus
   \param[out]  orc_InterfaceIndexes   Vector with all node interface ids which are connected to the bus
   \param[out]  opc_DatapoolIndexes    Optional: Vector with all Datapool ids which are connected to the bus
                                       If ope_ComProtocol is not NULL the Datapool will be checked if it is
                                       associated to the protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSystemDefinition::m_GetNodeAndComDpIndexesOfBus(const uint32_t ou32_BusIndex,
                                                          const C_OscCanProtocol::E_Type * const ope_ComProtocol,
                                                          std::vector<uint32_t> & orc_NodeIndexes,
                                                          std::vector<uint32_t> & orc_InterfaceIndexes,
                                                          std::vector<uint32_t> * const opc_DatapoolIndexes) const
{
   uint32_t u32_NodeIndex;
   uint32_t u32_ComInterfaces;

   // check all nodes
   for (u32_NodeIndex = 0U; u32_NodeIndex < this->c_Nodes.size(); ++u32_NodeIndex)
   {
      const C_OscNode & rc_Node = this->c_Nodes[u32_NodeIndex];

      // check all com interfaces of the node
      for (u32_ComInterfaces = 0U;
           u32_ComInterfaces < rc_Node.c_Properties.c_ComInterfaces.size();
           ++u32_ComInterfaces)
      {
         const C_OscNodeComInterfaceSettings & rc_CurComInterface =
            rc_Node.c_Properties.c_ComInterfaces[u32_ComInterfaces];
         // is the bus connected
         if (rc_CurComInterface.GetBusConnected() == true)
         {
            // check the connections
            if (rc_CurComInterface.u32_BusIndex == ou32_BusIndex)
            {
               // node is connected to the bus
               if (opc_DatapoolIndexes == NULL)
               {
                  // Only nodes and interfaces are relevant
                  orc_NodeIndexes.push_back(u32_NodeIndex);
                  orc_InterfaceIndexes.push_back(u32_ComInterfaces);
               }
               else
               {
                  // Getting the Datapool indexes of all COM Datapools for the specific protocol
                  uint32_t u32_ProtocolCounter;

                  for (u32_ProtocolCounter = 0U; u32_ProtocolCounter < rc_Node.c_ComProtocols.size();
                       ++u32_ProtocolCounter)
                  {
                     const C_OscCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ProtocolCounter];

                     // If the protocol is relevant, check for it
                     if ((ope_ComProtocol == NULL) ||
                         (rc_Prot.e_Type == (*ope_ComProtocol)))
                     {
                        orc_NodeIndexes.push_back(u32_NodeIndex);
                        orc_InterfaceIndexes.push_back(u32_ComInterfaces);
                        opc_DatapoolIndexes->push_back(rc_Prot.u32_DataPoolIndex);
                     }
                  }
               }
            }
         }
      }
   }
}
