//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Store system definition data

   System definition data container class.
   Also provides utility functions to use the data.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     02.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <map>
#include <algorithm>

#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLString.h"
#include "C_OSCSystemDefinition.h"
#include "TGLUtils.h"
#include "C_OSCUtils.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_core;
using namespace stw_types;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_errors;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */
C_OSCDeviceManager C_OSCSystemDefinition::hc_Devices;

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     01.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCSystemDefinition::C_OSCSystemDefinition(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     01.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCSystemDefinition::~C_OSCSystemDefinition(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out] oru32_HashValue    Hash value with initial [in] value and result [out] value

   \created     21.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCSystemDefinition::CalcHash(uint32 & oru32_HashValue) const
{
   uint32 u32_Counter;

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

//-----------------------------------------------------------------------------
/*!
   \brief   Add bus

   \param[in] orc_Bus Bus value

   \created     12.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCSystemDefinition::AddBus(const C_OSCSystemBus & orc_Bus)
{
   tgl_assert(this->InsertBus(this->c_Buses.size(), orc_Bus) == C_NO_ERR);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Insert bus

   \param[in] ou32_BusIndex  Bus index
   \param[in] orc_Bus        Bus value

   \return
   C_NO_ERR Done
   C_RANGE  Bus index invalid

   \created     12.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSystemDefinition::InsertBus(const uint32 ou32_BusIndex, const C_OSCSystemBus & orc_Bus)
{
   sint32 s32_Retval = C_NO_ERR;

   //Smaller and equal because append is also valid
   if (ou32_BusIndex <= this->c_Buses.size())
   {
      this->c_Buses.insert(this->c_Buses.begin() + ou32_BusIndex, orc_Bus);
      //Sync interface indices
      for (uint32 u32_ItNode = 0; u32_ItNode < this->c_Nodes.size(); ++u32_ItNode)
      {
         C_OSCNode & rc_Node = this->c_Nodes[u32_ItNode];
         for (uint32 u32_ItInterface = 0; u32_ItInterface < rc_Node.c_Properties.c_ComInterfaces.size();
              ++u32_ItInterface)
         {
            C_OSCNodeComInterfaceSettings & rc_ComInterface = rc_Node.c_Properties.c_ComInterfaces[u32_ItInterface];
            if (rc_ComInterface.q_IsBusConnected == true)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Delete bus

   \param[in] ou32_BusIndex  Bus index

   \return
   C_NO_ERR Done
   C_RANGE  Bus index invalid

   \created     12.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSystemDefinition::DeleteBus(const uint32 ou32_BusIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_BusIndex < this->c_Buses.size())
   {
      this->c_Buses.erase(this->c_Buses.begin() + ou32_BusIndex);
      //Sync interface indices
      for (uint32 u32_ItNode = 0; u32_ItNode < this->c_Nodes.size(); ++u32_ItNode)
      {
         C_OSCNode & rc_Node = this->c_Nodes[u32_ItNode];
         for (uint32 u32_ItInterface = 0; u32_ItInterface < rc_Node.c_Properties.c_ComInterfaces.size();
              ++u32_ItInterface)
         {
            C_OSCNodeComInterfaceSettings & rc_ComInterface = rc_Node.c_Properties.c_ComInterfaces[u32_ItInterface];
            if (rc_ComInterface.q_IsBusConnected == true)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Add new connection from specified node to specified bus using specified interface number

   \param[in] ou32_NodeIndex   Node to connect from
   \param[in] ou32_BusIndex    Bus to connect to
   \param[in] ou8_Interface    Interface number to use

   \return
   C_NO_ERR Done
   C_RANGE  Either node or bus or both do not exist

   \created     12.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSystemDefinition::AddConnection(const uint32 ou32_NodeIndex, const uint32 ou32_BusIndex,
                                            const uint8 ou8_Interface)
{
   sint32 s32_Retval = C_NO_ERR;

   if ((ou32_NodeIndex < this->c_Nodes.size()) && (ou32_BusIndex < this->c_Buses.size()))
   {
      C_OSCNode & rc_Node = this->c_Nodes[ou32_NodeIndex];
      C_OSCSystemBus & rc_Bus = this->c_Buses[ou32_BusIndex];
      const C_OSCNodeComInterfaceSettings * const pc_Interface = rc_Node.c_Properties.GetComInterface(rc_Bus.e_Type,
                                                                                                      ou8_Interface);
      if (pc_Interface != NULL)
      {
         C_OSCNodeComInterfaceSettings c_Tmp = *pc_Interface;
         c_Tmp.q_IsBusConnected = true;
         c_Tmp.u32_BusIndex = ou32_BusIndex;
         rc_Node.c_Properties.SetComInterface(c_Tmp);
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if com interface of node allows specified connection

   \param[in] ou32_NodeIndex Node index
   \param[in] ou32_ComIndex  Communication interface
   \param[in] ou8_ComNodeID  Node ID

   \return
   false Conflict
   true  Default

   \created     01.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OSCSystemDefinition::CheckInterfaceIsAvailable(const uint32 ou32_NodeIndex, const uint32 ou32_ComIndex,
                                                      const uint8 ou8_ComNodeID) const
{
   bool q_Retval = true;

   //Check node exists
   if (ou32_NodeIndex < this->c_Nodes.size())
   {
      const C_OSCNode & rc_CurNode = this->c_Nodes[ou32_NodeIndex];
      //Check com interface exists
      if (ou32_ComIndex < rc_CurNode.c_Properties.c_ComInterfaces.size())
      {
         const C_OSCNodeComInterfaceSettings & rc_CurComInterface =
            rc_CurNode.c_Properties.c_ComInterfaces[ou32_ComIndex];
         //Check com interface has bus connected
         if (rc_CurComInterface.q_IsBusConnected == true)
         {
            //Check all nodes for connected bus
            for (uint32 u32_ItNode = 0; u32_ItNode < this->c_Nodes.size(); ++u32_ItNode)
            {
               const C_OSCNode & rc_Node = this->c_Nodes[u32_ItNode];
               //Check all com interfaces
               for (uint32 u32_ItComInterface = 0; u32_ItComInterface < rc_Node.c_Properties.c_ComInterfaces.size();
                    ++u32_ItComInterface)
               {
                  //Skip selected com interface
                  if (((ou32_NodeIndex == u32_ItNode) && (u32_ItComInterface == ou32_ComIndex)) == false)
                  {
                     //Check if connected bus matches
                     const C_OSCNodeComInterfaceSettings & rc_ComInterface =
                        rc_Node.c_Properties.c_ComInterfaces[u32_ItComInterface];
                     if (rc_ComInterface.q_IsBusConnected == true)
                     {
                        if (rc_ComInterface.u32_BusIndex == rc_CurComInterface.u32_BusIndex)
                        {
                           if (rc_ComInterface.u8_NodeID == ou8_ComNodeID)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Check if any bus uses the provided bus id

   \param[in] ou8_BusId            Bus id to check for
   \param[in] opu32_BusIndexToSkip Optional parameter to skip one index
                                   (Use-case: skip current bus to avoid conflict with itself)

   \return
   true  Available
   false Already in use

   \created     03.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OSCSystemDefinition::CheckBusIdAvailable(const uint8 ou8_BusId, const uint32 * const opu32_BusIndexToSkip) const
{
   bool q_Retval = true;

   for (uint32 u32_ItBus = 0; u32_ItBus < this->c_Buses.size(); ++u32_ItBus)
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
         const C_OSCSystemBus & rc_Bus = this->c_Buses[u32_ItBus];
         if (rc_Bus.u8_BusID == ou8_BusId)
         {
            q_Retval = false;
         }
      }
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get new unused bus id

   \param[out] oru8_BusId Bus id result value

   \return
   C_NO_ERR Valid bus id found
   C_NOACT  No valid bus id found

   \created     04.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSystemDefinition::GetNextFreeBusId(uint8 & oru8_BusId) const
{
   sint32 s32_Retval = C_NO_ERR;
   bool q_Continue = true;

   for (oru8_BusId = 0; (oru8_BusId <= 15) && (q_Continue == true); ++oru8_BusId)
   {
      if (this->CheckBusIdAvailable(oru8_BusId) == true)
      {
         q_Continue = false;
      }
   }
   //Check result
   if (q_Continue == true)
   {
      s32_Retval = C_NOACT;
   }
   else
   {
      //Revert last ++
      --oru8_BusId;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check error for node

   \param[in]     ou32_NodeIndex                Node index
   \param[out]    opq_NameConflict              Name conflict
   \param[out]    opq_NameInvalid               Name not usable as variable
   \param[out]    opq_NodeIdInvalid             true: Node index is not usable
   \param[out]    opq_DataPoolsInvalid          true: error in data pool was detected
   \param[out]    opq_ApplicationsInvalid       true: error in application was detected
   \param[in]     orq_AllowComDataPoolException true: allow exception to skip check for connected interface
   \param[in,out] opc_InvalidInterfaceIndices   Optional storage for invalid interface indices
   \param[in,out] opc_InvalidDataPoolIndices    Optional storage for invalid datapool indices
   \param[in,out] opc_InvalidApplicationIndices Optional storage for invalid application indices

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     10.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSystemDefinition::CheckErrorNode(const uint32 ou32_NodeIndex, bool * const opq_NameConflict,
                                             bool * const opq_NameInvalid, bool * const opq_NodeIdInvalid,
                                             bool * const opq_DataPoolsInvalid, bool * const opq_ApplicationsInvalid,
                                             const bool & orq_AllowComDataPoolException,
                                             std::vector<uint32> * const opc_InvalidInterfaceIndices,
                                             std::vector<uint32> * const opc_InvalidDataPoolIndices,
                                             std::vector<uint32> * const opc_InvalidApplicationIndices) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->c_Nodes.size())
   {
      const C_OSCNode & rc_CheckedNode = this->c_Nodes[ou32_NodeIndex];
      if (opq_NameConflict != NULL)
      {
         *opq_NameConflict = false;
         for (uint32 u32_ItNode = 0; u32_ItNode < this->c_Nodes.size(); ++u32_ItNode)
         {
            if (u32_ItNode != ou32_NodeIndex)
            {
               const C_OSCNode & rc_CurrentNode = this->c_Nodes[u32_ItNode];
               if (rc_CheckedNode.c_Properties.c_Name.LowerCase() == rc_CurrentNode.c_Properties.c_Name.LowerCase())
               {
                  *opq_NameConflict = true;
               }
            }
         }
      }
      if (opq_NameInvalid != NULL)
      {
         *opq_NameInvalid = !C_OSCUtils::h_CheckValidCName(rc_CheckedNode.c_Properties.c_Name);
      }
      if (opq_NodeIdInvalid != NULL)
      {
         *opq_NodeIdInvalid = false;

         for (uint32 u32_ItComInterface = 0; u32_ItComInterface < rc_CheckedNode.c_Properties.c_ComInterfaces.size();
              ++u32_ItComInterface)
         {
            //check
            const bool q_ComIdValid = this->CheckInterfaceIsAvailable(ou32_NodeIndex, u32_ItComInterface,
                                                                      rc_CheckedNode.c_Properties.c_ComInterfaces[
                                                                         u32_ItComInterface].u8_NodeID);

            //if conflict found abort check
            if (q_ComIdValid == false)
            {
               *opq_NodeIdInvalid = true;
               if (opc_InvalidInterfaceIndices == NULL)
               {
                  break;
               }
               else
               {
                  opc_InvalidInterfaceIndices->push_back(u32_ItComInterface);
               }
            }
         }
      }
      if (opq_DataPoolsInvalid != NULL)
      {
         uint32 u32_Counter;
         bool q_DataPoolNameConflict;
         bool q_DataPoolNameInvalid;
         bool q_DataPoolListError;
         bool q_ResultError = false;
         static std::map<std::vector<uint32>, bool> hc_PreviousCommChecks;
         static std::map<uint32, bool> hc_PreviousCommonChecks;

         // check all datapools for errors
         for (u32_Counter = 0U;
              (u32_Counter < rc_CheckedNode.c_DataPools.size()) &&
              ((q_ResultError == false) || (opc_InvalidDataPoolIndices != NULL));
              ++u32_Counter)
         {
            const uint32 u32_Hash = this->m_GetDataPoolHash(ou32_NodeIndex, u32_Counter);
            bool q_AlreadyAdded = false;
            bool q_Skip = false;
            //Com data pool:
            //Exception if bus connected
            //Note: this error cannot be computed inside the comm datapool as it requires the protocol
            // which is outside the datapool reach
            if (orq_AllowComDataPoolException == true)
            {
               const C_OSCNodeDataPool & rc_DataPool = rc_CheckedNode.c_DataPools[u32_Counter];
               if (rc_DataPool.e_Type == C_OSCNodeDataPool::eCOM)
               {
                  //Get Hash for all relevant data
                  const uint32 u32_ProtocolHash = this->m_GetRelatedProtocolHash(ou32_NodeIndex, u32_Counter);
                  std::map<std::vector<uint32>, bool>::const_iterator c_It;
                  std::vector<uint32> c_Hashes;
                  c_Hashes.push_back(u32_Hash);
                  c_Hashes.push_back(u32_ProtocolHash);

                  //Check if check was already performed in the past
                  c_It = hc_PreviousCommChecks.find(c_Hashes);
                  if (c_It == hc_PreviousCommChecks.end())
                  {
                     bool q_CurRes = false;
                     const C_OSCCanProtocol * const pc_Protocol =
                        rc_CheckedNode.GetRelatedCANProtocolConst(u32_Counter);
                     if (pc_Protocol != NULL)
                     {
                        //Matching data pool to protocol
                        for (uint32 u32_ItInterface = 0;
                             (u32_ItInterface < pc_Protocol->c_ComMessages.size()) &&
                             ((q_ResultError == false) || (opc_InvalidDataPoolIndices != NULL));
                             ++u32_ItInterface)
                        {
                           const C_OSCCanMessageContainer & rc_MessageContainer =
                              pc_Protocol->c_ComMessages[u32_ItInterface];
                           //only check if not connected to bus
                           if (rc_MessageContainer.q_IsComProtocolUsedByInterface == false)
                           {
                              //Check both lists here
                              const C_OSCNodeDataPoolList * const pc_TxList = C_OSCCanProtocol::h_GetComListConst(
                                 rc_DataPool, u32_ItInterface, true);
                              const C_OSCNodeDataPoolList * const pc_RxList = C_OSCCanProtocol::h_GetComListConst(
                                 rc_DataPool, u32_ItInterface, false);
                              if ((pc_TxList != NULL) && (pc_RxList != NULL))
                              {
                                 if (rc_MessageContainer.CheckLocalError(*pc_TxList, *pc_RxList,
                                                                         C_OSCCanProtocol::
                                                                         h_GetCANMessageValidSignalsDLCOffset(
                                                                            pc_Protocol->e_Type)) == true)
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
               const std::map<uint32, bool>::const_iterator c_It = hc_PreviousCommonChecks.find(u32_Hash);
               if (c_It == hc_PreviousCommonChecks.end())
               {
                  rc_CheckedNode.CheckErrorDataPool(u32_Counter, &q_DataPoolNameConflict, &q_DataPoolNameInvalid,
                                                    &q_DataPoolListError, NULL);

                  if (((q_DataPoolNameConflict == true) || (q_DataPoolNameInvalid == true)) ||
                      (q_DataPoolListError == true))
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
                  //Do not reset error
                  q_ResultError = q_ResultError || c_It->second;
                  if ((opc_InvalidDataPoolIndices != NULL) && (c_It->second))
                  {
                     opc_InvalidDataPoolIndices->push_back(u32_Counter);
                  }
               }
            }
         }
         *opq_DataPoolsInvalid = q_ResultError;
      }
      if (opq_ApplicationsInvalid != NULL)
      {
         *opq_ApplicationsInvalid = false;
         for (uint32 u32_ItApp = 0; u32_ItApp < rc_CheckedNode.c_Applications.size(); ++u32_ItApp)
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
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check error for bus & connected node data pools

   \param[in]     ou32_BusIndex        Bus index
   \param[out]    opq_NameConflict     Name conflict
   \param[out]    opq_NameInvalid      Name not usable as variable
   \param[out]    opq_IdInvalid        Id out of range
   \param[out]    opq_DataPoolsInvalid An error found for a data pool

   \return
   C_NO_ERR Done
   C_RANGE  Bus does not exist

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSystemDefinition::CheckErrorBus(const uint32 ou32_BusIndex, bool * const opq_NameConflict,
                                            bool * const opq_NameInvalid, bool * const opq_IdInvalid,
                                            bool * const opq_DataPoolsInvalid) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (opq_NameConflict != NULL)
   {
      *opq_NameConflict = false;
      if (ou32_BusIndex < this->c_Buses.size())
      {
         const C_OSCSystemBus & rc_CheckedBus = this->c_Buses[ou32_BusIndex];
         for (uint32 u32_ItBus = 0; u32_ItBus < this->c_Buses.size(); ++u32_ItBus)
         {
            if (u32_ItBus != ou32_BusIndex)
            {
               const C_OSCSystemBus & rc_CurrentBus = this->c_Buses[u32_ItBus];
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
         const C_OSCSystemBus & rc_CheckedBus = this->c_Buses[ou32_BusIndex];
         *opq_NameInvalid = !C_OSCUtils::h_CheckValidCName(rc_CheckedBus.c_Name);
      }
   }
   if (opq_IdInvalid != NULL)
   {
      if (ou32_BusIndex < this->c_Buses.size())
      {
         const C_OSCSystemBus & rc_SystemBus = this->c_Buses[ou32_BusIndex];
         *opq_IdInvalid = rc_SystemBus.CheckErrorBusId();
         //Duplicate check
         if (*opq_IdInvalid == false)
         {
            for (uint32 u32_ItBus = 0; u32_ItBus < this->c_Buses.size(); ++u32_ItBus)
            {
               if (u32_ItBus != ou32_BusIndex)
               {
                  const C_OSCSystemBus & rc_Bus = this->c_Buses[u32_ItBus];
                  if (rc_SystemBus.u8_BusID == rc_Bus.u8_BusID)
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
         s32_Retval = C_RANGE;
      }
   }
   if ((opq_DataPoolsInvalid != NULL) && (s32_Retval == C_NO_ERR))
   {
      std::vector<uint32> c_NodeIndexes;
      std::vector<uint32> c_InterfaceIndexes;
      *opq_DataPoolsInvalid = false;
      this->GetNodeIndexesOfBus(ou32_BusIndex, c_NodeIndexes, c_InterfaceIndexes);
      if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
      {
         for (uint32 u32_ItNode = 0U;
              ((u32_ItNode < c_NodeIndexes.size()) && (*opq_DataPoolsInvalid == false)) && (s32_Retval == C_NO_ERR);
              ++u32_ItNode)
         {
            if (c_NodeIndexes[u32_ItNode] < this->c_Nodes.size())
            {
               const C_OSCNode & rc_Node = this->c_Nodes[c_NodeIndexes[u32_ItNode]];
               // check all datapools for errors
               for (uint32 u32_ItDataPool = 0U;
                    (u32_ItDataPool < rc_Node.c_DataPools.size()) && (*opq_DataPoolsInvalid == false);
                    ++u32_ItDataPool)
               {
                  const C_OSCNodeDataPool & rc_DataPool = rc_Node.c_DataPools[u32_ItDataPool];
                  //Only com data pool:
                  if (rc_DataPool.e_Type == C_OSCNodeDataPool::eCOM)
                  {
                     for (uint32 u32_ItProtocol = 0;
                          ((u32_ItProtocol < rc_Node.c_ComProtocols.size()) && (*opq_DataPoolsInvalid == false)) &&
                          (s32_Retval == C_NO_ERR);
                          ++u32_ItProtocol)
                     {
                        const C_OSCCanProtocol & rc_Protocol = rc_Node.c_ComProtocols[u32_ItProtocol];
                        if (rc_Protocol.u32_DataPoolIndex == u32_ItDataPool)
                        {
                           //Matching data pool to protocol
                           if (c_InterfaceIndexes[u32_ItNode] < rc_Protocol.c_ComMessages.size())
                           {
                              const C_OSCCanMessageContainer & rc_MessageContainer =
                                 rc_Protocol.c_ComMessages[c_InterfaceIndexes[u32_ItNode]];
                              //only check if connected to bus
                              if (rc_MessageContainer.q_IsComProtocolUsedByInterface == true)
                              {
                                 //Check both lists here
                                 const C_OSCNodeDataPoolList * const pc_TxList =
                                    C_OSCCanProtocol::h_GetComListConst(
                                       rc_DataPool,
                                       c_InterfaceIndexes[u32_ItNode],
                                       true);
                                 const C_OSCNodeDataPoolList * const pc_RxList =
                                    C_OSCCanProtocol::h_GetComListConst(
                                       rc_DataPool,
                                       c_InterfaceIndexes[u32_ItNode],
                                       false);
                                 if ((pc_TxList != NULL) && (pc_RxList != NULL))
                                 {
                                    *opq_DataPoolsInvalid = rc_MessageContainer.CheckLocalError(*pc_TxList,
                                                                                                *pc_RxList,
                                                                                                C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(
                                                                                                   rc_Protocol.e_Type));
                                    //Check global error
                                    if (*opq_DataPoolsInvalid == false)
                                    {
                                       C_OSCCanMessageIdentificationIndices c_MessageId;
                                       bool q_MessageValid = true;

                                       c_MessageId.u32_NodeIndex = c_NodeIndexes[u32_ItNode];
                                       c_MessageId.u32_InterfaceIndex = c_InterfaceIndexes[u32_ItNode];
                                       c_MessageId.e_ComProtocol = rc_Protocol.e_Type;
                                       //Tx
                                       c_MessageId.q_MessageIsTx = true;
                                       for (uint32 u32_ItMessage = 0;
                                            ((u32_ItMessage < rc_MessageContainer.c_TxMessages.size()) &&
                                             (q_MessageValid == true)) &&
                                            (s32_Retval == C_NO_ERR);
                                            ++u32_ItMessage)
                                       {
                                          c_MessageId.u32_MessageIndex = u32_ItMessage;
                                          const C_OSCCanMessage & rc_Message =
                                             rc_MessageContainer.c_TxMessages[u32_ItMessage];
                                          //Name
                                          s32_Retval = this->CheckMessageNameBus(ou32_BusIndex, rc_Message.c_Name,
                                                                                 q_MessageValid, &c_MessageId);
                                          //Id
                                          if ((s32_Retval == C_NO_ERR) && (q_MessageValid == true))
                                          {
                                             s32_Retval = this->CheckMessageIdBus(ou32_BusIndex,
                                                                                  rc_Message.u32_CanId,
                                                                                  q_MessageValid, &c_MessageId);
                                          }
                                       }
                                       //Rx
                                       c_MessageId.q_MessageIsTx = false;
                                       for (uint32 u32_ItMessage = 0;
                                            ((u32_ItMessage < rc_MessageContainer.c_RxMessages.size()) &&
                                             (q_MessageValid == true)) &&
                                            (s32_Retval == C_NO_ERR);
                                            ++u32_ItMessage)
                                       {
                                          const C_OSCCanMessage & rc_Message =
                                             rc_MessageContainer.c_RxMessages[u32_ItMessage];
                                          //Name
                                          s32_Retval = this->CheckMessageNameBus(ou32_BusIndex, rc_Message.c_Name,
                                                                                 q_MessageValid, &c_MessageId);
                                          //Id
                                          if ((s32_Retval == C_NO_ERR) && (q_MessageValid == true))
                                          {
                                             s32_Retval = this->CheckMessageIdBus(ou32_BusIndex,
                                                                                  rc_Message.u32_CanId,
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

//-----------------------------------------------------------------------------
/*!
   \brief   Check if message id valid

   \param[in]  ou32_BusIndex   Bus index
   \param[in]  ou32_MessageId  Message id
   \param[out] orq_Valid       Flag if valid
   \param[in]  opc_SkipMessage Optional parameter to skip one message
                               (Use-case: skip current message to avoid conflict with itself)

   \return
   C_NO_ERR Done
   C_RANGE  Bus does not exist

   \created     04.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSystemDefinition::CheckMessageIdBus(const uint32 ou32_BusIndex, const uint32 ou32_MessageId,
                                                bool & orq_Valid,
                                                const C_OSCCanMessageIdentificationIndices * const opc_SkipMessage)
const
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_BusIndex < this->c_Buses.size())
   {
      std::vector<uint32> c_NodeIndices;
      std::vector<uint32> c_InterfaceIndices;
      //Get all connected nodes
      GetNodeIndexesOfBus(ou32_BusIndex, c_NodeIndices, c_InterfaceIndices);
      orq_Valid = true;
      if (c_NodeIndices.size() == c_InterfaceIndices.size())
      {
         //Parse connected node with connected interface
         for (uint32 u32_ItNode = 0; u32_ItNode < c_NodeIndices.size(); ++u32_ItNode)
         {
            //Check node index
            if (c_NodeIndices[u32_ItNode] < this->c_Nodes.size())
            {
               const C_OSCNode & rc_Node = this->c_Nodes[c_NodeIndices[u32_ItNode]];
               bool q_Valid;
               //Check if skip possible
               if ((opc_SkipMessage != NULL) && (opc_SkipMessage->u32_NodeIndex == c_NodeIndices[u32_ItNode]))
               {
                  rc_Node.CheckMessageId(c_InterfaceIndices[u32_ItNode], ou32_MessageId, q_Valid,
                                         &opc_SkipMessage->e_ComProtocol,
                                         &opc_SkipMessage->u32_InterfaceIndex,
                                         &opc_SkipMessage->q_MessageIsTx,
                                         &opc_SkipMessage->u32_MessageIndex);
               }
               else
               {
                  //Check message id for one node
                  rc_Node.CheckMessageId(c_InterfaceIndices[u32_ItNode], ou32_MessageId, q_Valid);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Check if message id valid

   \param[in]  ou32_BusIndex   Bus index
   \param[in]  orc_MessageName Message name
   \param[out] orq_Valid       Flag if valid
   \param[in]  opc_SkipMessage Optional parameter to skip one message
                               (Use-case: skip current message to avoid conflict with itself)

   \return
   C_NO_ERR Done
   C_RANGE  Bus does not exist

   \created     04.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSystemDefinition::CheckMessageNameBus(const uint32 ou32_BusIndex, const C_SCLString & orc_MessageName,
                                                  bool & orq_Valid,
                                                  const C_OSCCanMessageIdentificationIndices * const opc_SkipMessage)
const
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_BusIndex < this->c_Buses.size())
   {
      std::vector<uint32> c_NodeIndices;
      std::vector<uint32> c_InterfaceIndices;
      //Get all connected nodes
      GetNodeIndexesOfBus(ou32_BusIndex, c_NodeIndices, c_InterfaceIndices);
      orq_Valid = true;
      if (c_NodeIndices.size() == c_InterfaceIndices.size())
      {
         //Parse connected node with connected interface
         for (uint32 u32_ItNode = 0; u32_ItNode < c_NodeIndices.size(); ++u32_ItNode)
         {
            //Check node index
            if (c_NodeIndices[u32_ItNode] < this->c_Nodes.size())
            {
               const C_OSCNode & rc_Node = this->c_Nodes[c_NodeIndices[u32_ItNode]];
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

//-----------------------------------------------------------------------------
/*!
   \brief   Check if specified messages match

   \param[in]  orc_MessageId1            First message identification indices
   \param[in]  orc_MessageId2            Second message identification indices
   \param[out] orq_IsMatch               Flag if messages match
   \param[in]  oq_IgnoreMessageDirection Flag to compare messages without message direction check

   \return
   C_NO_ERR Done
   C_RANGE  Nodes or Datapools or Lists or Messages do not exist

   \created     04.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSystemDefinition::CheckMessageMatch(const C_OSCCanMessageIdentificationIndices & orc_MessageId1,
                                                const C_OSCCanMessageIdentificationIndices & orc_MessageId2,
                                                bool & orq_IsMatch, const bool oq_IgnoreMessageDirection) const
{
   sint32 s32_Retval = C_NO_ERR;

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
            const C_OSCNode & rc_Node1 = this->c_Nodes[orc_MessageId1.u32_NodeIndex];
            const C_OSCNode & rc_Node2 = this->c_Nodes[orc_MessageId2.u32_NodeIndex];
            const C_OSCNodeDataPool * const pc_DataPool1 = rc_Node1.GetComDataPoolConst(orc_MessageId1.e_ComProtocol);
            const C_OSCNodeDataPool * const pc_DataPool2 = rc_Node2.GetComDataPoolConst(orc_MessageId2.e_ComProtocol);
            const C_OSCCanProtocol * const pc_Protocol1 = rc_Node1.GetCANProtocolConst(orc_MessageId1.e_ComProtocol);
            const C_OSCCanProtocol * const pc_Protocol2 = rc_Node2.GetCANProtocolConst(orc_MessageId2.e_ComProtocol);
            if (((pc_DataPool1 != NULL) && (pc_DataPool2 != NULL)) &&
                ((pc_Protocol1 != NULL) && (pc_Protocol2 != NULL)))
            {
               const C_OSCNodeDataPoolList * const pc_List1 = C_OSCCanProtocol::h_GetComListConst(*pc_DataPool1,
                                                                                                  orc_MessageId1.u32_InterfaceIndex,
                                                                                                  orc_MessageId1.q_MessageIsTx);
               const C_OSCNodeDataPoolList * const pc_List2 = C_OSCCanProtocol::h_GetComListConst(*pc_DataPool2,
                                                                                                  orc_MessageId2.u32_InterfaceIndex,
                                                                                                  orc_MessageId2.q_MessageIsTx);
               if (((orc_MessageId1.u32_InterfaceIndex < pc_Protocol1->c_ComMessages.size()) &&
                    (orc_MessageId2.u32_InterfaceIndex < pc_Protocol2->c_ComMessages.size())) &&
                   ((pc_List1 != NULL) && (pc_List2 != NULL)))
               {
                  const C_OSCCanMessageContainer & rc_MessageContainer1 =
                     pc_Protocol1->c_ComMessages[orc_MessageId1.u32_InterfaceIndex];
                  const C_OSCCanMessageContainer & rc_MessageContainer2 =
                     pc_Protocol2->c_ComMessages[orc_MessageId2.u32_InterfaceIndex];
                  const std::vector<C_OSCCanMessage> & rc_Messages1 = rc_MessageContainer1.GetMessagesConst(
                     orc_MessageId1.q_MessageIsTx);
                  const std::vector<C_OSCCanMessage> & rc_Messages2 = rc_MessageContainer2.GetMessagesConst(
                     orc_MessageId2.q_MessageIsTx);
                  if ((orc_MessageId1.u32_MessageIndex < rc_Messages1.size()) &&
                      (orc_MessageId2.u32_MessageIndex < rc_Messages2.size()))
                  {
                     const C_OSCCanMessage & rc_Message1 = rc_Messages1[orc_MessageId1.u32_MessageIndex];
                     const C_OSCCanMessage & rc_Message2 = rc_Messages2[orc_MessageId2.u32_MessageIndex];
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
                        if (rc_Message1.e_TxMethod == C_OSCCanMessage::eTX_METHOD_CYCLIC)
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
                        for (uint32 u32_ItSignal = 0;
                             (u32_ItSignal < rc_Message1.c_Signals.size()) && (orq_IsMatch == true);
                             ++u32_ItSignal)
                        {
                           const C_OSCCanSignal & rc_Signal1 = rc_Message1.c_Signals[u32_ItSignal];
                           const C_OSCCanSignal & rc_Signal2 = rc_Message2.c_Signals[u32_ItSignal];
                           const C_OSCNodeDataPoolListElement * const pc_SignalData1 =
                              pc_Protocol1->GetComListElementConst(
                                 *pc_DataPool1,
                                 orc_MessageId1.u32_InterfaceIndex,
                                 orc_MessageId1.q_MessageIsTx, orc_MessageId1.u32_MessageIndex, u32_ItSignal);
                           const C_OSCNodeDataPoolListElement * const pc_SignalData2 =
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
                              if (C_OSCUtils::h_IsFloat64NearlyEqual(pc_SignalData1->f64_Factor,
                                                                     pc_SignalData2->f64_Factor) == false)
                              {
                                 orq_IsMatch = false;
                              }
                              if (C_OSCUtils::h_IsFloat64NearlyEqual(pc_SignalData1->f64_Offset,
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

//-----------------------------------------------------------------------------
/*!
   \brief   Returns all node indexes which are connected to the bus

   \param[in]     ou32_BusIndex        Bus index
   \param[out]    orc_NodeIndexes      Vector with all node ids which are connected to the bus
   \param[out]    orc_InterfaceIndexes Vector with all node interface ids which are connected to the bus

   \created     03.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCSystemDefinition::GetNodeIndexesOfBus(const uint32 ou32_BusIndex, std::vector<uint32> & orc_NodeIndexes,
                                                std::vector<uint32> & orc_InterfaceIndexes) const
{
   uint32 u32_NodeIndex;
   uint32 u32_ComInterfaces;

   // check all nodes
   for (u32_NodeIndex = 0U; u32_NodeIndex < this->c_Nodes.size(); ++u32_NodeIndex)
   {
      const C_OSCNode & rc_Node = this->c_Nodes[u32_NodeIndex];

      // check all com interfaces of the node
      for (u32_ComInterfaces = 0U;
           u32_ComInterfaces < rc_Node.c_Properties.c_ComInterfaces.size();
           ++u32_ComInterfaces)
      {
         // is the bus connected
         if (rc_Node.c_Properties.c_ComInterfaces[u32_ComInterfaces].q_IsBusConnected == true)
         {
            // check the connections
            if (rc_Node.c_Properties.c_ComInterfaces[u32_ComInterfaces].u32_BusIndex == ou32_BusIndex)
            {
               // node is connected to the bus
               orc_NodeIndexes.push_back(u32_NodeIndex);
               orc_InterfaceIndexes.push_back(u32_ComInterfaces);
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get message receivers indices

   \param[in]  ou32_BusIndex        Bus index
   \param[in]  orc_MessageName      Message name
   \param[out] orc_NodeIndices      Node receivers indices
   \param[out] orc_InterfaceIndices Node receivers interfaces indices

   \created     05.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCSystemDefinition::GetMessageReceiversIndices(const uint32 ou32_BusIndex, const C_SCLString & orc_MessageName,
                                                       std::vector<uint32> & orc_NodeIndices,
                                                       std::vector<uint32> & orc_InterfaceIndices) const
{
   // check all nodes
   for (uint32 u32_NodeIndex = 0; u32_NodeIndex < this->c_Nodes.size(); ++u32_NodeIndex)
   {
      const C_OSCNode & rc_Node = this->c_Nodes[u32_NodeIndex];

      // check all com interfaces of the node
      for (uint32 u32_ItComInterface = 0; u32_ItComInterface < rc_Node.c_Properties.c_ComInterfaces.size();
           ++u32_ItComInterface)
      {
         // is the bus connected
         if (rc_Node.c_Properties.c_ComInterfaces[u32_ItComInterface].q_IsBusConnected == true)
         {
            // check the connections
            if (rc_Node.c_Properties.c_ComInterfaces[u32_ItComInterface].u32_BusIndex == ou32_BusIndex)
            {
               bool q_Found = false;
               //Match
               //For each com interface
               for (uint8 u8_ItComDataPool = 0; (u8_ItComDataPool < 3) && (q_Found == false); ++u8_ItComDataPool)
               {
                  const C_OSCCanProtocol * pc_Protocol;
                  switch (u8_ItComDataPool)
                  {
                  case 0:
                     pc_Protocol = rc_Node.GetCANProtocolConst(C_OSCCanProtocol::eLAYER2);
                     break;
                  case 1:
                     pc_Protocol = rc_Node.GetCANProtocolConst(C_OSCCanProtocol::eECES);
                     break;
                  case 2:
                     pc_Protocol = rc_Node.GetCANProtocolConst(C_OSCCanProtocol::eCAN_OPEN_SAFETY);
                     break;
                  default:
                     pc_Protocol = NULL;
                     break;
                  }
                  if (pc_Protocol != NULL)
                  {
                     if (u32_ItComInterface < pc_Protocol->c_ComMessages.size())
                     {
                        const C_OSCCanMessageContainer & rc_MessageContainer =
                           pc_Protocol->c_ComMessages[u32_ItComInterface];
                        const std::vector<C_OSCCanMessage> & rc_Messages =
                           rc_MessageContainer.GetMessagesConst(false);

                        //Search for message
                        for (uint32 u32_ItMessage = 0;
                             (u32_ItMessage < rc_Messages.size()) && (q_Found == false);
                             ++u32_ItMessage)
                        {
                           const C_OSCCanMessage & rc_Message = rc_Messages[u32_ItMessage];
                           if (rc_Message.c_Name == orc_MessageName)
                           {
                              orc_NodeIndices.push_back(u32_NodeIndex);
                              orc_InterfaceIndices.push_back(u32_ItComInterface);
                              q_Found = true;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get message transmitters indices

   \param[in]  ou32_BusIndex        Bus index
   \param[in]  orc_MessageName      Message name
   \param[out] orc_NodeIndices      Node transmitters indices
   \param[out] orc_InterfaceIndices Node transmitters interfaces indices

   \created     05.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCSystemDefinition::GetMessageTransmittersIndices(const uint32 ou32_BusIndex,
                                                          const C_SCLString & orc_MessageName,
                                                          std::vector<uint32> & orc_NodeIndices,
                                                          std::vector<uint32> & orc_InterfaceIndices) const
{
   // check all nodes
   for (uint32 u32_NodeIndex = 0; u32_NodeIndex < this->c_Nodes.size(); ++u32_NodeIndex)
   {
      const C_OSCNode & rc_Node = this->c_Nodes[u32_NodeIndex];

      // check all com interfaces of the node
      for (uint32 u32_ItComInterface = 0; u32_ItComInterface < rc_Node.c_Properties.c_ComInterfaces.size();
           ++u32_ItComInterface)
      {
         // is the bus connected
         if (rc_Node.c_Properties.c_ComInterfaces[u32_ItComInterface].q_IsBusConnected == true)
         {
            // check the connections
            if (rc_Node.c_Properties.c_ComInterfaces[u32_ItComInterface].u32_BusIndex == ou32_BusIndex)
            {
               bool q_Found = false;
               //Match
               //For each com interface
               for (uint8 u8_ItComDataPool = 0; (u8_ItComDataPool < 3) && (q_Found == false); ++u8_ItComDataPool)
               {
                  const C_OSCCanProtocol * pc_Protocol;
                  switch (u8_ItComDataPool)
                  {
                  case 0:
                     pc_Protocol = rc_Node.GetCANProtocolConst(C_OSCCanProtocol::eLAYER2);
                     break;
                  case 1:
                     pc_Protocol = rc_Node.GetCANProtocolConst(C_OSCCanProtocol::eECES);
                     break;
                  case 2:
                     pc_Protocol = rc_Node.GetCANProtocolConst(C_OSCCanProtocol::eCAN_OPEN_SAFETY);
                     break;
                  default:
                     pc_Protocol = NULL;
                     break;
                  }
                  if (pc_Protocol != NULL)
                  {
                     if (u32_ItComInterface < pc_Protocol->c_ComMessages.size())
                     {
                        const C_OSCCanMessageContainer & rc_MessageContainer =
                           pc_Protocol->c_ComMessages[u32_ItComInterface];
                        const std::vector<C_OSCCanMessage> & rc_Messages =
                           rc_MessageContainer.GetMessagesConst(true);

                        //Search for message
                        for (uint32 u32_ItMessage = 0;
                             (u32_ItMessage < rc_Messages.size()) && (q_Found == false);
                             ++u32_ItMessage)
                        {
                           const C_OSCCanMessage & rc_Message = rc_Messages[u32_ItMessage];
                           if (rc_Message.c_Name == orc_MessageName)
                           {
                              orc_NodeIndices.push_back(u32_NodeIndex);
                              orc_InterfaceIndices.push_back(u32_ItComInterface);
                              q_Found = true;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Append a new node at the end of the nodes.

   * appends node at the end of the node list
   * updates pointer to device definition

   Caller is responsible to pass a node referring to a "c_DeviceType" that is part of "hc_Devices".
   Otherwise the behavior is undefined (function will throw an assertion)

   \param[in,out] orc_Node  node value (out: with added pointer to device type)

   \created     26.02.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void C_OSCSystemDefinition::AddNode(C_OSCNode & orc_Node)
{
   orc_Node.pc_DeviceDefinition = C_OSCSystemDefinition::hc_Devices.LookForDevice(orc_Node.c_DeviceType);
   tgl_assert(orc_Node.pc_DeviceDefinition != NULL);
   this->c_Nodes.push_back(orc_Node);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get hash for datapool

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index

   \return
   Hash for datapool

   \created     20.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_OSCSystemDefinition::m_GetDataPoolHash(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex) const
{
   uint32 u32_Retval = 0xFFFFFFFFUL;

   if (ou32_NodeIndex < this->c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->c_Nodes[ou32_NodeIndex];
      if (ou32_DataPoolIndex < rc_Node.c_DataPools.size())
      {
         const C_OSCNodeDataPool & rc_DataPool = rc_Node.c_DataPools[ou32_DataPoolIndex];
         rc_DataPool.CalcHash(u32_Retval);
      }
   }
   return u32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get hash for protocol

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index

   \return
   Hash for protocol

   \created     20.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_OSCSystemDefinition::m_GetRelatedProtocolHash(const uint32 ou32_NodeIndex,
                                                       const uint32 ou32_DataPoolIndex) const
{
   uint32 u32_Retval = 0xFFFFFFFFUL;

   if (ou32_NodeIndex < this->c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->c_Nodes[ou32_NodeIndex];
      const C_OSCCanProtocol * const pc_ProToCol = rc_Node.GetRelatedCANProtocolConst(ou32_DataPoolIndex);
      if (pc_ProToCol != NULL)
      {
         pc_ProToCol->CalcHash(u32_Retval);
      }
   }
   return u32_Retval;
}
