//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       UI topology: stores additional UI information (implementation)

   UI topology: stores additional UI information

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

#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "CSCLString.h"
#include "C_OSCUtils.h"
#include "C_PuiSdHandler.h"
#include "C_SdBueSortHelper.h"
#include "C_OSCSystemDefinitionFiler.h"
#include "C_PuiSdUtil.h"
#include "C_PuiSdHandlerFiler.h"
#include "C_PuiBsElementsFiler.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"
#include "C_Uti.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_types;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_errors;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */
C_PuiSdHandler * C_PuiSdHandler::mhpc_Singleton = NULL;

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Set UI node from array

   \param[in] oru32_Index Index
   \param[in] orc_Item    New value

   \created     12.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::SetUINode(const uint32 & oru32_Index, const C_PuiSdNode & orc_Item)
{
   if (oru32_Index < this->mc_UINodes.size())
   {
      this->mc_UINodes[oru32_Index] = orc_Item;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set OSC node from array

   \param[in] oru32_Index Index
   \param[in] orc_Item    New value

   \created     12.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::SetOSCNode(const uint32 & oru32_Index, const C_OSCNode & orc_Item)
{
   if (oru32_Index < this->mc_CoreDefinition.c_Nodes.size())
   {
      this->mc_CoreDefinition.c_Nodes[oru32_Index] = orc_Item;
   }

   //signal "node change"
   Q_EMIT this->SigNodesChanged();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set UI bus from array

   \param[in] oru32_Index Index
   \param[in] orc_Item    New value

   \created     12.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::SetUIBus(const uint32 & oru32_Index, const C_PuiSdBus & orc_Item)
{
   if (oru32_Index < this->mc_UIBuses.size())
   {
      mc_UIBuses[oru32_Index] = orc_Item;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set OSC bus from array

   \param[in] oru32_Index Index
   \param[in] orc_Item    New value

   \created     12.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::SetOSCBus(const uint32 & oru32_Index, const C_OSCSystemBus & orc_Item)
{
   if (oru32_Index < this->mc_CoreDefinition.c_Buses.size())
   {
      this->mc_CoreDefinition.c_Buses[oru32_Index] = orc_Item;
   }

   //signal "bus change"
   Q_EMIT this->SigBussesChanged();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get UI node from array

   Get UI node from array

   \param[in] ors32_Index Index

   \return
   NULL Data not found
   Else Valid data

   \created     19.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiSdNode * C_PuiSdHandler::GetUINode(const uint32 & oru32_Index) const
{
   const C_PuiSdNode * pc_Retval;

   if (oru32_Index < this->mc_UINodes.size())
   {
      pc_Retval = &(this->mc_UINodes[oru32_Index]);
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get OSC node from array

   Get OSC node from array

   \param[in] ors32_Index Index

   \return
   NULL Data not found
   Else Valid data

   \created     19.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCNode * C_PuiSdHandler::GetOSCNodeConst(const uint32 & oru32_Index) const
{
   const C_OSCNode * pc_Retval;

   if (oru32_Index < this->mc_CoreDefinition.c_Nodes.size())
   {
      pc_Retval = &(this->mc_CoreDefinition.c_Nodes[oru32_Index]);
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get osc node with sorted internal structure

   Warning: this function might be time consuming

   \param[in]  oru32_Index Node index (ID)
   \param[out] orc_Node    Sorted node

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
   C_RANGE  Operation failure: parameter invalid
   C_COM    Sorting failed

   \created     09.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::GetSortedOSCNodeConst(const uint32 & oru32_Index, C_OSCNode & orc_Node) const
{
   sint32 s32_Retval = C_NO_ERR;
   C_PuiSdNode c_UiNode;

   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size())
   {
      if (oru32_Index < this->mc_CoreDefinition.c_Nodes.size())
      {
         orc_Node = this->mc_CoreDefinition.c_Nodes[oru32_Index];
         c_UiNode = this->mc_UINodes[oru32_Index];
         //Sort
         if (mh_SortMessagesByName(orc_Node, c_UiNode) != C_NO_ERR)
         {
            s32_Retval = C_COM;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get non const OSC node from array

   Get non const OSC node from array

   \param[in] ors32_Index Index

   \return
   NULL Data not found
   Else Valid data

   \created     21.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OSCNode * C_PuiSdHandler::GetOSCNode(const uint32 & oru32_Index)
{
   C_OSCNode * pc_Retval;

   if (oru32_Index < this->mc_CoreDefinition.c_Nodes.size())
   {
      pc_Retval = &(this->mc_CoreDefinition.c_Nodes[oru32_Index]);
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set STW flashloader settings

   \param[in] ou32_Index   Node index
   \param[in] orc_Settings Settings

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     07.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetStwFlashloaderSettings(const uint32 ou32_Index,
                                                 const C_OSCNodeStwFlashloaderSettings & orc_Settings)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_Index];
      rc_Node.c_Properties.c_STWFlashloaderSettings = orc_Settings;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set node openSYDE server settings

   \param[in] ou32_Index   Node index
   \param[in] orc_Settings Settings

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     29.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetNodeOpenSYDEServerSettings(const uint32 ou32_Index,
                                                     const C_OSCNodeOpenSYDEServerSettings & orc_Settings)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_Index];
      rc_Node.c_Properties.c_OpenSYDEServerSettings = orc_Settings;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get UI bus from array

   Get UI bus from array

   \param[in] ors32_Index Index

   \return
   NULL Data not found
   Else Valid data

   \created     19.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiSdBus * C_PuiSdHandler::GetUIBus(const uint32 & oru32_Index) const
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get OSC bus from array

   Get OSC bus from array

   \param[in] ors32_Index Index

   \return
   NULL Data not found
   Else Valid data

   \created     19.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCSystemBus * C_PuiSdHandler::GetOSCBus(const uint32 & oru32_Index) const
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

//-----------------------------------------------------------------------------
/*!
   \brief   Check if any node uses the provided name

   \param[in] orc_Name              Node name to check for
   \param[in] opu32_NodeIndexToSkip Optional parameter to skip one index
                                    (Use-case: skip current node to avoid conflict with itself)

   \return
   true  Available
   false Already in use

   \created     01.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_PuiSdHandler::CheckNodeNameAvailable(const C_SCLString & orc_Name,
                                            const uint32 * const opu32_NodeIndexToSkip) const
{
   bool q_Retval = true;

   for (uint32 u32_ItNode = 0; (u32_ItNode < this->mc_CoreDefinition.c_Nodes.size()) && (q_Retval == true);
        ++u32_ItNode)
   {
      bool q_Skip = false;
      if (opu32_NodeIndexToSkip != NULL)
      {
         if (*opu32_NodeIndexToSkip == u32_ItNode)
         {
            q_Skip = true;
         }
      }
      if (q_Skip == false)
      {
         const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
         if (rc_Node.c_Properties.c_Name.LowerCase() == orc_Name.LowerCase())
         {
            q_Retval = false;
         }
      }
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if any bus uses the provided name

   \param[in] orc_Name             Bus name to check for
   \param[in] opu32_BusIndexToSkip Optional parameter to skip one index
                                    (Use-case: skip current bus to avoid conflict with itself)

   \return
   true  Available
   false Already in use

   \created     29.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_PuiSdHandler::CheckBusNameAvailable(const C_SCLString & orc_Name,
                                           const uint32 * const opu32_BusIndexToSkip) const
{
   bool q_Retval = true;

   for (uint32 u32_ItBus = 0; (u32_ItBus < this->mc_CoreDefinition.c_Buses.size()) && (q_Retval == true);
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
         if (rc_Bus.c_Name.LowerCase() == orc_Name.LowerCase())
         {
            q_Retval = false;
         }
      }
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if any data pool uses the provided name

   \param[in] oru32_NodeIndex           Node index
   \param[in] orc_Name                  Data pool name to check for
   \param[in] opu32_DataPoolIndexToSkip Optional parameter to skip one index
                                        (Use-case: skip current data pool to avoid conflict with itself)

   \return
   true  Available
   false Already in use

   \created     01.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_PuiSdHandler::CheckNodeDataPoolNameAvailable(const uint32 & oru32_NodeIndex, const C_SCLString & orc_Name,
                                                    const uint32 * const opu32_DataPoolIndexToSkip) const
{
   bool q_Retval = true;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      for (uint32 u32_ItDataPool = 0; (u32_ItDataPool < rc_Node.c_DataPools.size()) && (q_Retval == true);
           ++u32_ItDataPool)
      {
         bool q_Skip = false;
         if (opu32_DataPoolIndexToSkip != NULL)
         {
            if (*opu32_DataPoolIndexToSkip == u32_ItDataPool)
            {
               q_Skip = true;
            }
         }
         if (q_Skip == false)
         {
            const C_OSCNodeDataPool & rc_DataPool = rc_Node.c_DataPools[u32_ItDataPool];
            if (rc_DataPool.c_Name.LowerCase() == orc_Name.LowerCase())
            {
               q_Retval = false;
            }
         }
      }
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if any list uses the provided name

   \param[in] oru32_NodeIndex               Node index
   \param[in] oru32_DataPoolIndex           Data pool index
   \param[in] orc_Name                      Data pool list name to check for
   \param[in] opu32_DataPoolListIndexToSkip Optional parameter to skip one index
                                            (Use-case: skip current data pool list to avoid conflict with itself)

   \return
   true  Available
   false Already in use

   \created     31.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_PuiSdHandler::CheckNodeDataPoolListNameAvailable(const uint32 & oru32_NodeIndex,
                                                        const uint32 & oru32_DataPoolIndex,
                                                        const C_SCLString & orc_Name,
                                                        const uint32 * const opu32_DataPoolListIndexToSkip) const
{
   bool q_Retval = true;
   const C_OSCNodeDataPool * const pc_DataPool = this->GetOSCDataPool(oru32_NodeIndex, oru32_DataPoolIndex);

   if (pc_DataPool != NULL)
   {
      for (uint32 u32_ItList = 0; (u32_ItList < pc_DataPool->c_Lists.size()) && (q_Retval == true);
           ++u32_ItList)
      {
         bool q_Skip = false;
         if (opu32_DataPoolListIndexToSkip != NULL)
         {
            if (*opu32_DataPoolListIndexToSkip == u32_ItList)
            {
               q_Skip = true;
            }
         }
         if (q_Skip == false)
         {
            const C_OSCNodeDataPoolList & rc_List = pc_DataPool->c_Lists[u32_ItList];
            if (rc_List.c_Name.LowerCase() == orc_Name.LowerCase())
            {
               q_Retval = false;
            }
         }
      }
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if any list element uses the provided name

   \param[in] oru32_NodeIndex                      Node index
   \param[in] oru32_DataPoolIndex                  Data pool index
   \param[in] oru32_ListIndex                      List index
   \param[in] orc_Name                             Data pool list element name to check for
   \param[in] opu32_DataPoolListElementIndexToSkip Optional parameter to skip one index
                                                   (Use-case: skip current data pool list element
                                                    to avoid conflict with itself)

   \return
   true  Available
   false Already in use

   \created     31.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_PuiSdHandler::CheckNodeDataPoolListElementNameAvailable(const uint32 & oru32_NodeIndex,
                                                               const uint32 & oru32_DataPoolIndex,
                                                               const uint32 & oru32_ListIndex,
                                                               const C_SCLString & orc_Name,
                                                               const uint32 * const opu32_DataPoolListElementIndexToSkip)
const
{
   bool q_Retval = true;
   const C_OSCNodeDataPoolList * const pc_DataPoolList = this->GetOSCDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                                  oru32_ListIndex);

   if (pc_DataPoolList != NULL)
   {
      for (uint32 u32_ItElement = 0; (u32_ItElement < pc_DataPoolList->c_Elements.size()) && (q_Retval == true);
           ++u32_ItElement)
      {
         bool q_Skip = false;
         if (opu32_DataPoolListElementIndexToSkip != NULL)
         {
            if (*opu32_DataPoolListElementIndexToSkip == u32_ItElement)
            {
               q_Skip = true;
            }
         }
         if (q_Skip == false)
         {
            const C_OSCNodeDataPoolListElement & rc_Element = pc_DataPoolList->c_Elements[u32_ItElement];
            if (rc_Element.c_Name.LowerCase() == orc_Name.LowerCase())
            {
               q_Retval = false;
            }
         }
      }
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if any list data set uses the provided name

   \param[in] oru32_NodeIndex                      Node index
   \param[in] oru32_DataPoolIndex                  Data pool index
   \param[in] oru32_ListIndex                      List index
   \param[in] orc_Name                             Data pool list data set name to check for
   \param[in] opu32_DataPoolListDataSetIndexToSkip Optional parameter to skip one index
                                                   (Use-case: skip current data pool list data set
                                                    to avoid conflict with itself)

   \return
   true  Available
   false Already in use

   \created     31.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_PuiSdHandler::CheckNodeDataPoolListDataSetNameAvailable(const uint32 & oru32_NodeIndex,
                                                               const uint32 & oru32_DataPoolIndex,
                                                               const uint32 & oru32_ListIndex,
                                                               const C_SCLString & orc_Name,
                                                               const uint32 * const opu32_DataPoolListDataSetIndexToSkip)
const
{
   bool q_Retval = true;
   const C_OSCNodeDataPoolList * const pc_DataPoolList = this->GetOSCDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                                  oru32_ListIndex);

   if (pc_DataPoolList != NULL)
   {
      for (uint32 u32_ItDataSet = 0; (u32_ItDataSet < pc_DataPoolList->c_DataSets.size()) && (q_Retval == true);
           ++u32_ItDataSet)
      {
         bool q_Skip = false;
         if (opu32_DataPoolListDataSetIndexToSkip != NULL)
         {
            if (*opu32_DataPoolListDataSetIndexToSkip == u32_ItDataSet)
            {
               q_Skip = true;
            }
         }
         if (q_Skip == false)
         {
            const C_OSCNodeDataPoolDataSet & rc_DataSet = pc_DataPoolList->c_DataSets[u32_ItDataSet];
            if (rc_DataSet.c_Name.LowerCase() == orc_Name.LowerCase())
            {
               q_Retval = false;
            }
         }
      }
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add new connection

   Add connection from node to bus.
   If node and bus exist this function will
   - add a new connection to the node structure
   - add a corresponding new UI connection to the UI node structure

   \param[in]   ou32_NodeIndex      Connected node index
   \param[in]   ou8_InterfaceNumber Interface number of node
   \param[in]   oru8_NodeId         New node id
   \param[in]   ou32_BusIndex       Connected bus index

   \created     14.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::AddConnection(const uint32 ou32_NodeIndex, const uint8 ou8_InterfaceNumber,
                                   const stw_types::uint8 & oru8_NodeId, const uint32 ou32_BusIndex)
{
   const C_OSCSystemBus * const pc_Bus = this->GetOSCBus(ou32_BusIndex);

   if ((ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size()) && (pc_Bus != NULL))
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const C_OSCNodeComInterfaceSettings * const pc_CurComInterface = rc_Node.c_Properties.GetComInterface(
         pc_Bus->e_Type, ou8_InterfaceNumber);
      if (pc_CurComInterface != NULL)
      {
         if (this->mc_CoreDefinition.AddConnection(ou32_NodeIndex, ou32_BusIndex,
                                                   pc_CurComInterface->u8_InterfaceNumber) == C_NO_ERR)
         {
            //Adapt node ID
            C_OSCNodeComInterfaceSettings c_CurComInterface = *pc_CurComInterface;
            c_CurComInterface.u8_NodeID = oru8_NodeId;
            rc_Node.c_Properties.SetComInterface(c_CurComInterface);

            C_PuiSdNodeConnection c_NewConnection;

            //Add connection ID
            c_NewConnection.c_ConnectionID.u8_InterfaceNumber = ou8_InterfaceNumber;
            c_NewConnection.c_ConnectionID.e_InterfaceType = pc_Bus->e_Type;

            if (ou32_NodeIndex < this->mc_UINodes.size())
            {
               //Also add corresponding UI element:
               this->mc_UINodes[ou32_NodeIndex].c_UIBusConnections.push_back(c_NewConnection);
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Remove connection

   \param[in]   ou32_NodeIndex          Connector node index (0 -> first node)
   \param[in]   ou32_ConnectionIndex    Connector connection index (0 -> first connection)

   \created     14.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::RemoveConnection(const uint32 ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_ID)
{
   const C_OSCNode * const pc_OSCNode = this->GetOSCNodeConst(ou32_NodeIndex);
   const C_PuiSdNode * const pc_UINode = this->GetUINode(ou32_NodeIndex);

   if (pc_OSCNode != NULL)
   {
      C_OSCNode c_OSCNode = *pc_OSCNode;
      c_OSCNode.c_Properties.DisconnectComInterface(orc_ID.e_InterfaceType, orc_ID.u8_InterfaceNumber);
      this->SetOSCNode(ou32_NodeIndex, c_OSCNode);
   }
   if (pc_UINode != NULL)
   {
      C_PuiSdNode c_UINode = *pc_UINode;
      c_UINode.DeleteConnection(orc_ID);
      this->SetUINode(ou32_NodeIndex, c_UINode);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Change connection

   \param[in] ou32_NodeIndex   Index of node this interface is part of
   \param[in] orc_ID           Previous interface ID
   \param[in] ou8_NewInterface New interface number
   \param[in] oru8_NodeId      New node id

   \created     12.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::ChangeConnection(const uint32 ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_ID,
                                      const uint8 ou8_NewInterface, const uint8 & oru8_NodeId)
{
   C_PuiSdNodeConnectionId c_Tmp;

   c_Tmp.e_InterfaceType = orc_ID.e_InterfaceType;
   c_Tmp.u8_InterfaceNumber = ou8_NewInterface;
   ChangeCompleteConnection(ou32_NodeIndex, orc_ID, c_Tmp, oru8_NodeId);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Change complete connection

   \param[in] ou32_NodeIndex Index of node this interface is part of
   \param[in] orc_PrevID     Previous interface ID
   \param[in] orc_NewID      New interface ID
   \param[in] oru8_NodeId    Node id
   \param[in] oru32_BusIndex Bus index to use instead of last used one

   \created     12.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::ChangeCompleteConnection(const uint32 ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_PrevID,
                                              const C_PuiSdNodeConnectionId & orc_NewID, const uint8 & oru8_NodeId,
                                              const uint32 & oru32_BusIndex)
{
   const C_OSCNode * const pc_OSCNode = this->GetOSCNodeConst(ou32_NodeIndex);
   const C_PuiSdNode * const pc_UINode = this->GetUINode(ou32_NodeIndex);

   //Core
   if (pc_OSCNode != NULL)
   {
      C_OSCNode c_OSCNode = *pc_OSCNode;
      uint32 u32_LastBus = 0;
      //Unregister
      for (uint32 u32_ItComInterface = 0; u32_ItComInterface < c_OSCNode.c_Properties.c_ComInterfaces.size();
           ++u32_ItComInterface)
      {
         C_OSCNodeComInterfaceSettings & rc_ComInterface = c_OSCNode.c_Properties.c_ComInterfaces[u32_ItComInterface];
         if ((rc_ComInterface.e_InterfaceType == orc_PrevID.e_InterfaceType) &&
             (rc_ComInterface.u8_InterfaceNumber == orc_PrevID.u8_InterfaceNumber))
         {
            rc_ComInterface.q_IsBusConnected = false;
            u32_LastBus = rc_ComInterface.u32_BusIndex;
            rc_ComInterface.u32_BusIndex = 0;
         }
      }
      //Register
      for (uint32 u32_ItComInterface = 0; u32_ItComInterface < c_OSCNode.c_Properties.c_ComInterfaces.size();
           ++u32_ItComInterface)
      {
         C_OSCNodeComInterfaceSettings & rc_ComInterface = c_OSCNode.c_Properties.c_ComInterfaces[u32_ItComInterface];
         if ((rc_ComInterface.e_InterfaceType == orc_NewID.e_InterfaceType) &&
             (rc_ComInterface.u8_InterfaceNumber == orc_NewID.u8_InterfaceNumber))
         {
            rc_ComInterface.q_IsBusConnected = true;
            if (oru32_BusIndex >= this->mc_CoreDefinition.c_Buses.size())
            {
               rc_ComInterface.u32_BusIndex = u32_LastBus;
            }
            else
            {
               rc_ComInterface.u32_BusIndex = oru32_BusIndex;
            }
            rc_ComInterface.u8_NodeID = oru8_NodeId;
         }
      }
      this->SetOSCNode(ou32_NodeIndex, c_OSCNode);
   }
   //UI
   if (pc_UINode != NULL)
   {
      C_PuiSdNode c_UINode = *pc_UINode;
      for (uint32 u32_ItBusConn = 0; u32_ItBusConn < c_UINode.c_UIBusConnections.size(); ++u32_ItBusConn)
      {
         C_PuiSdNodeConnection & rc_BusConn = c_UINode.c_UIBusConnections[u32_ItBusConn];
         if (rc_BusConn.c_ConnectionID == orc_PrevID)
         {
            rc_BusConn.c_ConnectionID = orc_NewID;
         }
      }
      this->SetUINode(ou32_NodeIndex, c_UINode);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add node

   \param[in,out] orc_OSCNode   New OSC node (name might be modified by this function if not unique)
   \param[in,out] orc_UINode    New UI node

   \return
   Index of node  (0 -> first node)

   \created     14.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_PuiSdHandler::AddNodeAndSort(C_OSCNode & orc_OSCNode, const C_PuiSdNode & orc_UINode)
{
   uint32 u32_Index = mc_CoreDefinition.c_Nodes.size();

   orc_OSCNode.c_Properties.c_Name = C_PuiSdUtil::h_GetUniqueName(
      this->m_GetExistingNodeNames(), orc_OSCNode.c_Properties.c_Name);

   mc_CoreDefinition.AddNode(orc_OSCNode); //add node and set device definition pointer

   //insert UI part at same position as OSC part:
   this->mc_UINodes.insert(mc_UINodes.begin() + u32_Index, orc_UINode);

   //signal "node change"
   Q_EMIT this->SigNodesChanged();
   Q_EMIT this->SigSyncNodeAdded(u32_Index);

   return u32_Index;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Remove node

   \param[in]   ou32_NodeIndex   Connector node index (0 -> first node)

   \created     14.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::RemoveNode(const uint32 ou32_NodeIndex)
{
   //Synchronization engine (First!)
   Q_EMIT this->SigSyncNodeAboutToBeDeleted(ou32_NodeIndex);

   this->mc_CoreDefinition.c_Nodes.erase(this->mc_CoreDefinition.c_Nodes.begin() + ou32_NodeIndex);
   this->mc_UINodes.erase(this->mc_UINodes.begin() + ou32_NodeIndex);

   //signal "node change"
   Q_EMIT this->SigNodesChanged();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add bus

   \param[in,out] opc_OSCBus   New OSC bus (name might be modified by this function if not unique)
   \param[in,out] opc_UIBus    New UI bus
   \param[in]     opc_Name     New bus name

   \return
   Index of bus

   \created     14.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_PuiSdHandler::AddBusAndSort(C_OSCSystemBus & orc_OSCBus, const C_PuiSdBus & orc_UIBus,
                                     const QString * const opc_Name, const bool oq_AllowBusIdAdaption)
{
   const uint32 u32_Index = mc_CoreDefinition.c_Buses.size();

   if (opc_Name != NULL)
   {
      orc_OSCBus.c_Name = opc_Name->toStdString().c_str();
   }
   else
   {
      orc_OSCBus.c_Name = C_PuiSdUtil::h_GetUniqueName(
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get pointers to all currently registered bus names

   \return
   Vector of pointers to all currently registered bus names

   \created     28.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<const C_SCLString *> C_PuiSdHandler::GetExistingBusNames(void) const
{
   std::vector<const C_SCLString *> c_Retval;
   c_Retval.reserve(this->mc_CoreDefinition.c_Buses.size());
   for (uint32 u32_ItBus = 0; u32_ItBus < this->mc_CoreDefinition.c_Buses.size(); ++u32_ItBus)
   {
      c_Retval.push_back(&this->mc_CoreDefinition.c_Buses[u32_ItBus].c_Name);
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get all programmable applications for one node

   \param[in] ou32_NodeIndex Node index (ID)

   \return
   All known programmable applications for this node (empty if index invalid)

   \created     03.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<const C_OSCNodeApplication *> C_PuiSdHandler::GetProgrammableApplications(const uint32 ou32_NodeIndex) const
{
   std::vector<const C_OSCNodeApplication *> c_Retval;
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      for (uint32 u32_ItAppl = 0; u32_ItAppl < rc_Node.c_Applications.size(); ++u32_ItAppl)
      {
         const C_OSCNodeApplication & rc_Application = rc_Node.c_Applications[u32_ItAppl];
         if (rc_Application.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
         {
            c_Retval.push_back(&rc_Application);
         }
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get all indices of programmable applications for one node

   \param[in] ou32_NodeIndex Node index (ID)

   \return
   All known programmable applications for this node (empty if index invalid)

   \created     12.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
std::vector<uint32> C_PuiSdHandler::GetProgrammableAppIndices(const uint32 ou32_NodeIndex) const
{
   std::vector<uint32> c_Return;
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      for (uint32 u32_ItAppl = 0; u32_ItAppl < rc_Node.c_Applications.size(); ++u32_ItAppl)
      {
         const C_OSCNodeApplication & rc_Application = rc_Node.c_Applications[u32_ItAppl];
         if (rc_Application.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
         {
            c_Return.push_back(u32_ItAppl);
         }
      }
   }
   return c_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Remove bus

   \param[in]   ou32_BusIndex   Connector bus index (0 -> first bus)

   \created     14.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::RemoveBus(const uint32 ou32_BusIndex)
{
   uint32 u32_Counter;

   this->mc_CoreDefinition.DeleteBus(ou32_BusIndex);
   this->mc_UIBuses.erase(this->mc_UIBuses.begin() + ou32_BusIndex);

   // Sync bus text elements indices
   for (u32_Counter = 0U; u32_Counter < this->c_BusTextElements.size(); ++u32_Counter)
   {
      if (this->c_BusTextElements[u32_Counter].u32_BusIndex > ou32_BusIndex)
      {
         --this->c_BusTextElements[u32_Counter].u32_BusIndex;
      }
   }

   //signal "bus change"
   Q_EMIT this->SigBussesChanged();
   Q_EMIT this->SigSyncBusDeleted(ou32_BusIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if there is a node conflict

   \param[in] oru32_NodeIndex Node index

   \return
   true  Conflict
   false No conflict

   \created     14.03.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------
bool C_PuiSdHandler::CheckNodeConflict(const uint32 & oru32_NodeIndex) const
{
   bool q_Retval;
   bool q_NameConflict;
   bool q_NameEmpty;
   bool q_NodeIdInvalid;
   bool q_DataPoolsInvalid;
   bool q_ApplicationsInvalid;

   if (this->mc_CoreDefinition.CheckErrorNode(oru32_NodeIndex, &q_NameConflict, &q_NameEmpty, &q_NodeIdInvalid,
                                              &q_DataPoolsInvalid, &q_ApplicationsInvalid, true, NULL, NULL,
                                              NULL) == C_NO_ERR)
   {
      if (((((q_NameConflict == true) || (q_NodeIdInvalid == true)) || (q_DataPoolsInvalid == true)) ||
           (q_ApplicationsInvalid == true)) || (q_NameEmpty == true))
      {
         q_Retval = true;
      }
      else
      {
         // one further check necessary
         q_Retval = this->CheckNodeNvmDataPoolsSizeConflict(oru32_NodeIndex);
      }
   }
   else
   {
      q_Retval = true;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check all NVM datapools of a node for size error

   \return
   true  Conflict
   false No conflict

   \created     30.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_PuiSdHandler::CheckNodeNvmDataPoolsSizeConflict(const uint32 ou32_NodeIndex) const
{
   bool q_Return = true;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);

   if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL))
   {
      uint32 u32_Counter;
      uint32 u32_SizeDataPools = 0U;

      // get the size of all NVM datapools
      for (u32_Counter = 0U; u32_Counter < pc_Node->c_DataPools.size(); ++u32_Counter)
      {
         if (pc_Node->c_DataPools[u32_Counter].e_Type == C_OSCNodeDataPool::eNVM)
         {
            u32_SizeDataPools += pc_Node->c_DataPools[u32_Counter].u32_NvMSize;
         }
      }

      if (u32_SizeDataPools <= pc_Node->pc_DeviceDefinition->u32_UserEepromSizeBytes)
      {
         // no error
         q_Return = false;
      }
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if there is a bus conflict

   \param[in]  ou32_BusIndex              Bus index
   \param[out] opq_NameConflict           Name conflict
   \param[out] opq_NameEmpty              Name empty
   \param[out] opq_IdInvalid              Id out of range
   \param[out] opc_InvalidNodesForBitRate Additional info for bit rate error
   \param[out] opc_InvalidProtocols       Additional info for protocol error

   \return
   true  Conflict
   false No conflict

   \created     18.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_PuiSdHandler::CheckBusConflict(const uint32 ou32_BusIndex, bool * const opq_NameConflict,
                                      bool * const opq_NameEmpty, bool * const opq_IdInvalid,
                                      std::vector<QString> * const opc_InvalidNodesForBitRate,
                                      std::vector<C_OSCCanProtocol::E_Type> * const opc_InvalidProtocols) const
{
   bool q_Retval = false;
   bool q_NameConflict;
   bool q_NameEmpty;
   bool q_IdInvalid;

   //Check bus errors
   if (this->mc_CoreDefinition.CheckErrorBus(ou32_BusIndex, &q_NameConflict, &q_NameEmpty, &q_IdInvalid,
                                             NULL) == C_NO_ERR)
   {
      // Check current bitrate
      const C_OSCSystemBus * const pc_CheckedBus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(ou32_BusIndex);
      if (((q_NameConflict == true) || (q_IdInvalid == true)) || (q_NameEmpty == true))
      {
         q_Retval = true;
      }

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
            const C_OSCNode * const pc_Node =
               C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_ConnectedNodes[u32_NodeCounter]);

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
                  q_Retval = true;
                  if (opc_InvalidNodesForBitRate != NULL)
                  {
                     opc_InvalidNodesForBitRate->push_back(pc_Node->c_Properties.c_Name.c_str());
                  }
               }
            }
         }
      }

      //Check message errors
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
               q_Retval = true;
               if (opc_InvalidProtocols != NULL)
               {
                  opc_InvalidProtocols->push_back(e_Type);
               }
            }
         }
      }
   }
   else
   {
      q_Retval = true;
   }
   if (opq_NameConflict != NULL)
   {
      *opq_NameConflict = q_NameConflict;
   }
   if (opq_NameEmpty != NULL)
   {
      *opq_NameEmpty = q_NameEmpty;
   }
   if (opq_IdInvalid != NULL)
   {
      *opq_IdInvalid = q_IdInvalid;
   }

   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns all supported CAN bitrates of specific nodes

   If no nodes are available, all CAN bitrates will be supported.

   \param[in]     orc_Nodes         Vector with all relevant node indexes
   \param[out]    orc_Bitrates      Result vector with all supported CAN bitrates

   \created     18.01.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::GetSupportedCanBitrates(const std::vector<uint32> & orc_Nodes,
                                             std::vector<uint32> & orc_Bitrates) const
{
   uint32 u32_NodeCounter;

   orc_Bitrates.clear();

   // If there are no nodes, we support all CAN bitrates.
   orc_Bitrates.push_back(100);
   orc_Bitrates.push_back(125);
   orc_Bitrates.push_back(250);
   orc_Bitrates.push_back(500);
   orc_Bitrates.push_back(800);
   orc_Bitrates.push_back(1000);

   for (u32_NodeCounter = 0U; u32_NodeCounter < orc_Nodes.size(); ++u32_NodeCounter)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(orc_Nodes[u32_NodeCounter]);

      if (pc_Node != NULL)
      {
         uint32 u32_SupportedBitrateCounter;
         std::vector<uint32> c_TempBitrates;

         tgl_assert(pc_Node->pc_DeviceDefinition != NULL);

         // Search common bitrates by comparing previously found bitrates with the bitrates of the current device
         for (u32_SupportedBitrateCounter = 0U; u32_SupportedBitrateCounter < orc_Bitrates.size();
              ++u32_SupportedBitrateCounter)
         {
            uint32 u32_DeviceBitrateCounter;

            for (u32_DeviceBitrateCounter = 0U;
                 u32_DeviceBitrateCounter < pc_Node->pc_DeviceDefinition->c_SupportedBitrates.size();
                 ++u32_DeviceBitrateCounter)
            {
               if (orc_Bitrates[u32_SupportedBitrateCounter] ==
                   pc_Node->pc_DeviceDefinition->c_SupportedBitrates[u32_DeviceBitrateCounter])
               {
                  c_TempBitrates.push_back(orc_Bitrates[u32_SupportedBitrateCounter]);
                  break;
               }
            }
         }

         // Save the found common bitrates with this device
         orc_Bitrates.clear();
         orc_Bitrates = c_TempBitrates;
      }
      else
      {
         // Error
         orc_Bitrates.clear();
      }

      // If no bitrates are available, it will not change by other devices
      if (orc_Bitrates.size() == 0)
      {
         break;
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add data pool

   \param[in] oru32_NodeIndex         Node index
   \param[in] orc_OSCContent          OSC data pool content (name might be modified by this function if not unique)
   \param[in] orc_UIContent           UI data pool content
   \param[in] ore_ComProtocolType     Optional parameter for com protocol type
   \param[in] orq_AllowNameAdaptation Optional parameter to disable automatic name adaptation
   \param[in] orq_AllowDataAdaptation Optional parameter to disable automatic data adaptation

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::AddDataPool(const uint32 & oru32_NodeIndex, const C_OSCNodeDataPool & orc_OSCContent,
                                   const C_PuiSdNodeDataPool & orc_UIContent,
                                   const C_OSCCanProtocol::E_Type & ore_ComProtocolType,
                                   const bool & orq_AllowNameAdaptation, const bool & orq_AllowDataAdaptation)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_UINodes.size())
   {
      const C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      uint32 u32_ItTargetIndex = 0;
      //Handle correct array segment position
      if (orc_OSCContent.e_Type != C_OSCNodeDataPool::eCOM)
      {
         const C_OSCNodeDataPool * pc_DataPool;

         //Always go to end of DIAG
         for (; u32_ItTargetIndex < rc_OSCNode.c_DataPools.size(); ++u32_ItTargetIndex)
         {
            pc_DataPool = &rc_OSCNode.c_DataPools[u32_ItTargetIndex];
            if (pc_DataPool->e_Type != C_OSCNodeDataPool::eDIAG)
            {
               break;
            }
         }
         if (orc_OSCContent.e_Type == C_OSCNodeDataPool::eNVM)
         {
            //Go to end of NVM
            for (; u32_ItTargetIndex < rc_OSCNode.c_DataPools.size(); ++u32_ItTargetIndex)
            {
               pc_DataPool = &rc_OSCNode.c_DataPools[u32_ItTargetIndex];
               if (pc_DataPool->e_Type != C_OSCNodeDataPool::eNVM)
               {
                  break;
               }
            }
         }
      }
      else
      {
         u32_ItTargetIndex = rc_OSCNode.c_DataPools.size();
      }

      //Insert
      tgl_assert(this->InsertDataPool(oru32_NodeIndex, u32_ItTargetIndex, orc_OSCContent, orc_UIContent,
                                      ore_ComProtocolType, orq_AllowNameAdaptation,
                                      orq_AllowDataAdaptation) == C_NO_ERR);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Insert data pool at specific position

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Data pool index
   \param[in] orc_OSCContent          OSC data pool content (name might be modified by this function if not unique)
   \param[in] orc_UIContent           UI data pool content
   \param[in] ore_ComProtocolType     Optional parameter for com protocol type
   \param[in] orq_AllowNameAdaptation Optional parameter to disable automatic name adaptation
   \param[in] orq_AllowDataAdaptation Optional parameter to disable automatic data adaptation

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::InsertDataPool(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                      const C_OSCNodeDataPool & orc_OSCContent,
                                      const C_PuiSdNodeDataPool & orc_UIContent,
                                      const C_OSCCanProtocol::E_Type & ore_ComProtocolType,
                                      const bool & orq_AllowNameAdaptation, const bool & orq_AllowDataAdaptation)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_UINodes.size())
   {
      C_PuiSdNode & rc_UINode = this->mc_UINodes[oru32_NodeIndex];
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      //Unique name
      C_OSCNodeDataPool c_NodeDataPool = orc_OSCContent;
      if (orq_AllowNameAdaptation == true)
      {
         c_NodeDataPool.c_Name = this->GetUniqueDataPoolName(oru32_NodeIndex, c_NodeDataPool.c_Name);
      }
      //Check size & consistency
      tgl_assert(rc_UINode.c_UIDataPools.size() == rc_OSCNode.c_DataPools.size());
      if (oru32_DataPoolIndex <= rc_UINode.c_UIDataPools.size())
      {
         rc_UINode.c_UIDataPools.insert(rc_UINode.c_UIDataPools.begin() + oru32_DataPoolIndex, orc_UIContent);
         rc_OSCNode.InsertDataPool(oru32_DataPoolIndex, c_NodeDataPool);
         //Handle NVM
         rc_OSCNode.RecalculateAddress();
         //Handle COMM
         m_SetUpComDataPool(oru32_NodeIndex, oru32_DataPoolIndex, orq_AllowDataAdaptation, ore_ComProtocolType);
         //Synchronization engine
         Q_EMIT this->SigSyncNodeDataPoolAdded(oru32_NodeIndex, oru32_DataPoolIndex);
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
   \brief   Remove data pool

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Data pool index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::RemoveDataPool(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_UINodes.size())
   {
      C_PuiSdNode & rc_UINode = this->mc_UINodes[oru32_NodeIndex];
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      //Check size & consistency
      tgl_assert(rc_UINode.c_UIDataPools.size() == rc_OSCNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_UINode.c_UIDataPools.size())
      {
         //Synchronization engine (First!)
         Q_EMIT this->SigSyncNodeDataPoolAboutToBeDeleted(oru32_NodeIndex, oru32_DataPoolIndex);
         //Handle COMM
         m_CleanUpComDataPool(oru32_NodeIndex, oru32_DataPoolIndex);
         rc_UINode.c_UIDataPools.erase(rc_UINode.c_UIDataPools.begin() + oru32_DataPoolIndex);
         rc_OSCNode.DeleteDataPool(oru32_DataPoolIndex);
         //Handle NVM
         rc_OSCNode.RecalculateAddress();
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
   \brief   Set data pool

   \param[in] oru32_NodeIndex       Node index
   \param[in] oru32_DataPoolIndex   Data pool index
   \param[in] orc_OSCContent        OSC data pool content
   \param[in] orc_UIContent         UI data pool content
   \param[in] oq_NewComProtocolType Flag if the COM protocol type was changed
   \param[in] ore_ComProtocolType   Optional parameter for com protocol type

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
   C_NOACT  Input invalid

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetDataPool(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                   const C_OSCNodeDataPool & orc_OSCContent, const C_PuiSdNodeDataPool & orc_UIContent,
                                   const bool oq_NewComProtocolType,
                                   const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComProtocolType)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_UINodes.size())
   {
      C_PuiSdNode & rc_UINode = this->mc_UINodes[oru32_NodeIndex];
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UINode.c_UIDataPools.size() == rc_OSCNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_UINode.c_UIDataPools.size())
      {
         if (orc_OSCContent.c_Lists.size() == orc_UIContent.c_DataPoolLists.size())
         {
            rc_UINode.c_UIDataPools[oru32_DataPoolIndex] = orc_UIContent;
            rc_OSCNode.c_DataPools[oru32_DataPoolIndex] = orc_OSCContent;
            //Update addresses
            rc_OSCNode.RecalculateAddress();
            //Handle COMM
            if ((oq_NewComProtocolType == true) &&
                (rc_OSCNode.c_DataPools[oru32_DataPoolIndex].e_Type == C_OSCNodeDataPool::eCOM))
            {
               // Update COM datapool protocol
               uint32 u32_Counter;

               // Search the correct COM protocol
               for (u32_Counter = 0U; u32_Counter < rc_OSCNode.c_ComProtocols.size(); ++u32_Counter)
               {
                  if (rc_OSCNode.c_ComProtocols[u32_Counter].u32_DataPoolIndex == oru32_DataPoolIndex)
                  {
                     rc_OSCNode.c_ComProtocols[u32_Counter].e_Type = ore_ComProtocolType;
                  }
               }
            }
         }
         else
         {
            s32_Retval = C_NOACT;
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
   \brief   Unassign data pool application

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Data pool index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     19.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::UnassignDataPoolApplication(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      if (oru32_DataPoolIndex < rc_Node.c_DataPools.size())
      {
         C_OSCNodeDataPool & rc_DataPool = rc_Node.c_DataPools[oru32_DataPoolIndex];
         rc_DataPool.s32_RelatedDataBlockIndex = -1;
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
   \brief   Assign data pool application

   \param[in] oru32_NodeIndex        Node index
   \param[in] oru32_DataPoolIndex    Data pool index
   \param[in] ors32_ApplicationIndex Application index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     02.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::AssignDataPoolApplication(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                 const sint32 os32_ApplicationIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_DataPoolIndex < rc_Node.c_DataPools.size())
      {
         C_OSCNodeDataPool & rc_DataPool = rc_Node.c_DataPools[ou32_DataPoolIndex];
         rc_DataPool.s32_RelatedDataBlockIndex = os32_ApplicationIndex;
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
   \brief   Get data pool

   \param[in]  oru32_NodeIndex     Node index
   \param[in]  oru32_DataPoolIndex Data pool index
   \param[out] orc_OSCContent      OSC data pool content
   \param[out] orc_UIContent       UI data pool content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::GetDataPool(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                   C_OSCNodeDataPool & orc_OSCContent, C_PuiSdNodeDataPool & orc_UIContent) const
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNodeDataPool * const pc_OSCDataPool = GetOSCDataPool(oru32_NodeIndex,
                                                                   oru32_DataPoolIndex);
   const C_PuiSdNodeDataPool * const pc_UIDataPool = GetUIDataPool(oru32_NodeIndex,
                                                                   oru32_DataPoolIndex);

   if ((pc_OSCDataPool != NULL) && (pc_UIDataPool != NULL))
   {
      orc_UIContent = *pc_UIDataPool;
      orc_OSCContent = *pc_OSCDataPool;
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add application

   \param[in]     oru32_NodeIndex Node index
   \param[in,out] orc_OSCContent  Application content (Name might get changed)

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     15.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::AddApplication(const uint32 ou32_NodeIndex, C_OSCNodeApplication & orc_OSCContent)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      tgl_assert(this->InsertApplication(ou32_NodeIndex, rc_Node.c_Applications.size(), orc_OSCContent) == C_NO_ERR);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Insert application at specific position

   \param[in]     oru32_NodeIndex       Node index
   \param[in]     ou32_ApplicationIndex Application index
   \param[in,out] orc_OSCContent        Application content (Name might get changed)

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     15.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::InsertApplication(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationIndex,
                                         C_OSCNodeApplication & orc_OSCContent)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_ApplicationIndex <= rc_Node.c_Applications.size())
      {
         orc_OSCContent.c_Name = this->GetUniqueApplicationName(ou32_NodeIndex, orc_OSCContent.c_Name);
         orc_OSCContent.u8_ProcessId = this->GetUniqueApplicationProcessId(ou32_NodeIndex, orc_OSCContent.u8_ProcessId);
         rc_Node.c_Applications.insert(rc_Node.c_Applications.begin() + ou32_ApplicationIndex, orc_OSCContent);
         //Synchronization engine
         Q_EMIT this->SigSyncNodeApplicationAdded(ou32_NodeIndex, ou32_ApplicationIndex);
         m_SyncNodeApplicationAdded(ou32_NodeIndex, ou32_ApplicationIndex);
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
   \brief   Remove application

   \param[in] oru32_NodeIndex       Node index
   \param[in] ou32_ApplicationIndex Application index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     15.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::RemoveApplication(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_ApplicationIndex < rc_Node.c_Applications.size())
      {
         //Synchronization engine (First!)
         Q_EMIT this->SigSyncNodeApplicationAboutToBeDeleted(ou32_NodeIndex, ou32_ApplicationIndex);
         m_SyncNodeApplicationAboutToBeDeleted(ou32_NodeIndex, ou32_ApplicationIndex);
         rc_Node.c_Applications.erase(rc_Node.c_Applications.begin() + ou32_ApplicationIndex);
         //Check if there is a replacement necessary
         m_HandleNodeAutomatedProgrammableApplicationUpdate(ou32_NodeIndex);
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
   \brief   Set application

   \param[in] oru32_NodeIndex       Node index
   \param[in] ou32_ApplicationIndex Application index
   \param[in] orc_OSCContent        Application content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     15.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetApplication(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationIndex,
                                      const C_OSCNodeApplication & orc_OSCContent)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_ApplicationIndex < rc_Node.c_Applications.size())
      {
         rc_Node.c_Applications[ou32_ApplicationIndex] = orc_OSCContent;
         //Check if there is a new programmable application
         m_HandleNodeAutomatedProgrammableApplicationUpdate(ou32_NodeIndex);
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
   \brief   Get application

   \param[in] oru32_NodeIndex       Node index
   \param[in] ou32_ApplicationIndex Application index

   \return
   NULL Application not found
   Else Valid application

   \created     15.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCNodeApplication * C_PuiSdHandler::GetApplication(const uint32 ou32_NodeIndex,
                                                            const uint32 ou32_ApplicationIndex) const
{
   const C_OSCNodeApplication * pc_Retval = NULL;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_ApplicationIndex < rc_Node.c_Applications.size())
      {
         pc_Retval = &rc_Node.c_Applications[ou32_ApplicationIndex];
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move application in node

   \param[in] ou32_NodeIndex   Node index
   \param[in] ou32_SourceIndex Source application index
   \param[in] ou32_TargetIndex Target application index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     15.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::MoveApplication(const uint32 ou32_NodeIndex, const uint32 ou32_SourceIndex,
                                       const uint32 ou32_TargetIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if ((ou32_SourceIndex < rc_Node.c_Applications.size()) &&
          (ou32_TargetIndex < rc_Node.c_Applications.size()))
      {
         //Copy
         C_OSCNodeApplication c_Data = rc_Node.c_Applications[ou32_SourceIndex];
         //Erase
         rc_Node.c_Applications.erase(rc_Node.c_Applications.begin() + ou32_SourceIndex);
         //Insert
         rc_Node.c_Applications.insert(rc_Node.c_Applications.begin() + ou32_TargetIndex, c_Data);
         //Synchronization engine
         Q_EMIT this->SigSyncNodeApplicationMoved(ou32_NodeIndex, ou32_SourceIndex, ou32_TargetIndex);
         m_SyncNodeApplicationMoved(ou32_NodeIndex, ou32_SourceIndex, ou32_TargetIndex);
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
   \brief   Get unique application name based on proposal

   \param[in] oru32_NodeIndex Node index
   \param[in] orc_Proposal    Application name proposal

   \return
   Unique application name based on proposal

   \created     12.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SCLString C_PuiSdHandler::GetUniqueApplicationName(const uint32 & oru32_NodeIndex,
                                                     const C_SCLString & orc_Proposal) const
{
   return C_PuiSdUtil::h_GetUniqueName(this->m_GetExistingNodeApplicationNames(oru32_NodeIndex), orc_Proposal);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get next unique application process ID

   \param[in]  ou32_NodeIndex  Node index (ID)

   \return
   Next unique application process ID

   \created     04.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint8 C_PuiSdHandler::GetUniqueApplicationProcessId(const uint32 ou32_NodeIndex, const uint8 ou8_Proposal) const
{
   //Use 16 bit to not run into overflow issues
   uint16 u16_Retval = 0;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      bool q_Found = false;
      //1.Check if proposal is unique
      for (uint32 u32_ItApp = 0; u32_ItApp < rc_Node.c_Applications.size(); ++u32_ItApp)
      {
         const C_OSCNodeApplication & rc_Application = rc_Node.c_Applications[u32_ItApp];
         //Only check programmable applications
         if (rc_Application.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
         {
            //Should never reach above 255 values
            if (rc_Application.u8_ProcessId == ou8_Proposal)
            {
               q_Found = true;
            }
         }
      }
      //2. Discard proposal if not unique
      if (q_Found == true)
      {
         for (; (u16_Retval < 256) && (q_Found == true); ++u16_Retval)
         {
            q_Found = false;
            for (uint32 u32_ItApp = 0; u32_ItApp < rc_Node.c_Applications.size(); ++u32_ItApp)
            {
               const C_OSCNodeApplication & rc_Application = rc_Node.c_Applications[u32_ItApp];
               //Only check programmable applications
               if (rc_Application.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
               {
                  //Should never reach above 255 values
                  if (rc_Application.u8_ProcessId == static_cast<uint8>(u16_Retval))
                  {
                     q_Found = true;
                  }
               }
            }
         }
         //Revert last iteration step
         --u16_Retval;
      }
      else
      {
         //Use proposal if unique
         u16_Retval = static_cast<uint16>(ou8_Proposal);
      }
   }

   return static_cast<uint8>(u16_Retval);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle automated programmable application assignment

   \param[in]     ou32_NodeIndex         Node index (ID)
   \param[in,out] ors32_ApplicationIndex Application index, which is adapted if necessary

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     16.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::GetNextProgrammableApplicationIndex(const uint32 ou32_NodeIndex,
                                                           sint32 & ors32_ApplicationIndex) const
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      //Check if current is valid
      if (ors32_ApplicationIndex >= 0)
      {
         if (static_cast<uint32>(ors32_ApplicationIndex) < pc_Node->c_Applications.size())
         {
            const C_OSCNodeApplication & rc_Application =
               pc_Node->c_Applications[static_cast<uint32>(ors32_ApplicationIndex)];
            if (rc_Application.e_Type != C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
            {
               //Set invalid
               ors32_ApplicationIndex = -1;
            }
         }
         else
         {
            //Set invalid
            ors32_ApplicationIndex = -1;
         }
      }
      //Replace invalid index if possible
      if (ors32_ApplicationIndex < 0)
      {
         for (uint32 u32_ItApp = 0; u32_ItApp < pc_Node->c_Applications.size(); ++u32_ItApp)
         {
            const C_OSCNodeApplication & rc_Application = pc_Node->c_Applications[u32_ItApp];
            if (rc_Application.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
            {
               ors32_ApplicationIndex = static_cast<sint32>(u32_ItApp);
               //Stop after first valid one
               break;
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
   \brief   Check application name valid

   \param[in]  ou32_NodeIndex        Node index
   \param[in]  orc_ApplicationName   Application name to check
   \param[out] orq_Valid             Function result: flag if name is valid
   \param[in]  opu32_SkipApplication Optional parameter to skip one application index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     12.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::CheckApplicationName(const uint32 ou32_NodeIndex, const C_SCLString & orc_ApplicationName,
                                            bool & orq_Valid, const uint32 * const opu32_SkipApplication) const
{
   sint32 s32_Retval = C_NO_ERR;

   orq_Valid = true;
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      for (uint32 u32_ItApplication = 0; u32_ItApplication < rc_Node.c_Applications.size(); ++u32_ItApplication)
      {
         const C_OSCNodeApplication & rc_Application = rc_Node.c_Applications[u32_ItApplication];
         bool q_Skip = false;
         if (opu32_SkipApplication != NULL)
         {
            if (*opu32_SkipApplication == u32_ItApplication)
            {
               q_Skip = true;
            }
         }
         if (q_Skip == false)
         {
            if (orc_ApplicationName == rc_Application.c_Name)
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

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get unique data pool name based on proposal

   \param[in] oru32_NodeIndex Node index
   \param[in] orc_Proposal    Data pool name proposal

   \return
   Unique data pool name based on proposal

   \created     01.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SCLString C_PuiSdHandler::GetUniqueDataPoolName(const uint32 & oru32_NodeIndex,
                                                  const C_SCLString & orc_Proposal) const
{
   return C_PuiSdUtil::h_GetUniqueName(this->m_GetExistingNodeDataPoolNames(oru32_NodeIndex), orc_Proposal);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get OSC data pool

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Data pool index

   \return
   Found: Pointer to OSC data pool
   Else:  NULL

   \created     23.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCNodeDataPool * C_PuiSdHandler::GetOSCDataPool(const uint32 & oru32_NodeIndex,
                                                         const uint32 & oru32_DataPoolIndex) const
{
   const C_OSCNodeDataPool * pc_Retval = NULL;

   //Check size & consistency
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      //Check size & consistency
      if (oru32_DataPoolIndex < rc_OSCNode.c_DataPools.size())
      {
         pc_Retval = &rc_OSCNode.c_DataPools[oru32_DataPoolIndex];
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get UI data pool

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Data pool index

   \return
   Found: Pointer to UI data pool
   Else:  NULL

   \created     23.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiSdNodeDataPool * C_PuiSdHandler::GetUIDataPool(const uint32 & oru32_NodeIndex,
                                                          const uint32 & oru32_DataPoolIndex) const
{
   const C_PuiSdNodeDataPool * pc_Retval = NULL;

   //Check size & consistency
   if (oru32_NodeIndex < this->mc_UINodes.size())
   {
      const C_PuiSdNode & rc_UINode = this->mc_UINodes[oru32_NodeIndex];
      //Check size & consistency
      if (oru32_DataPoolIndex < rc_UINode.c_UIDataPools.size())
      {
         pc_Retval = &rc_UINode.c_UIDataPools[oru32_DataPoolIndex];
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get data pool type

   \param[in]  oru32_NodeIndex     Node index
   \param[in]  oru32_DataPoolIndex Data pool index
   \param[out] ore_Type            OSC data pool type

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     22.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::GetDataPoolType(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                       C_OSCNodeDataPool::E_Type & ore_Type) const
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      //Check size
      if (oru32_DataPoolIndex < rc_OSCNode.c_DataPools.size())
      {
         ore_Type = rc_OSCNode.c_DataPools[oru32_DataPoolIndex].e_Type;
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
   \brief   Move datapool in node

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_SourceIndex   Source list index
   \param[in] ou32_TargetIndex   Target list index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     17.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::MoveDataPool(const uint32 ou32_NodeIndex, const uint32 ou32_SourceIndex,
                                    const uint32 ou32_TargetIndex)
{
   sint32 s32_Retval = C_RANGE;

   //Check size & consistency
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_UINodes.size())
   {
      C_PuiSdNode & rc_UINode = this->mc_UINodes[ou32_NodeIndex];

      if ((ou32_SourceIndex < rc_UINode.c_UIDataPools.size()) &&
          (ou32_TargetIndex < rc_UINode.c_UIDataPools.size()))
      {
         // adapt core node
         s32_Retval = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex].MoveDataPool(ou32_SourceIndex, ou32_TargetIndex);

         // adapt ui node
         if (s32_Retval == C_NO_ERR)
         {
            //Copy
            C_PuiSdNodeDataPool c_Data = rc_UINode.c_UIDataPools[ou32_SourceIndex];
            //Erase
            rc_UINode.c_UIDataPools.erase(rc_UINode.c_UIDataPools.begin() + ou32_SourceIndex);
            //Insert
            rc_UINode.c_UIDataPools.insert(rc_UINode.c_UIDataPools.begin() + ou32_TargetIndex, c_Data);
            //Synchronization engine
            Q_EMIT this->SigSyncNodeDataPoolMoved(ou32_NodeIndex, ou32_SourceIndex, ou32_TargetIndex);
         }
      }
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get datapool index by type specific datapool index

   \param[in] ou32_NodeIndex           Node index
   \param[in] oe_DataPoolType          Type of datapool
   \param[in] ou32_DataPoolTypeIndex   Type specific index of datapool

   \return
   if datapool is found: zero based index
   if datapool is not found or parameter out of range: -1

   \created     17.02.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::GetDataPoolIndex(const uint32 ou32_NodeIndex, const C_OSCNodeDataPool::E_Type oe_DataPoolType,
                                        const uint32 ou32_DataPoolTypeIndex) const
{
   sint32 s32_Return = -1;

   //Check size & consistency
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_UINodes.size())
   {
      const C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Return = rc_OSCNode.GetDataPoolIndex(oe_DataPoolType, ou32_DataPoolTypeIndex);
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Insert data pool list at specific position

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Data pool index
   \param[in] oru32_DataPoolListIndex Data pool list index
   \param[in] orc_OSCContent          OSC data pool list content
                                      (name might be modified by this function if not unique)
   \param[in] orc_UIContent           UI data pool list content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::InsertDataPoolList(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                          const uint32 & oru32_DataPoolListIndex,
                                          const C_OSCNodeDataPoolList & orc_OSCContent,
                                          const C_PuiSdNodeDataPoolList & orc_UIContent)
{
   sint32 s32_Retval;
   C_PuiSdNodeDataPool c_UIDataPool;
   C_OSCNodeDataPool c_OSCDataPool;

   s32_Retval = this->GetDataPool(oru32_NodeIndex, oru32_DataPoolIndex, c_OSCDataPool, c_UIDataPool);
   if (s32_Retval == C_NO_ERR)
   {
      //Unique name
      C_OSCNodeDataPoolList c_NodeDataPoolList = orc_OSCContent;
      c_NodeDataPoolList.c_Name =
         C_PuiSdUtil::h_GetUniqueName(this->m_GetExistingNodeDataPoolListNames(oru32_NodeIndex,
                                                                               oru32_DataPoolIndex),
                                      c_NodeDataPoolList.c_Name);
      //Adapt required fields
      if (c_OSCDataPool.q_IsSafety == true)
      {
         c_NodeDataPoolList.q_NvMCRCActive = true;
         for (uint32 u32_ItElement = 0; u32_ItElement < c_NodeDataPoolList.c_Elements.size(); ++u32_ItElement)
         {
            C_OSCNodeDataPoolListElement & rc_CurElement = c_NodeDataPoolList.c_Elements[u32_ItElement];
            rc_CurElement.e_Access = C_OSCNodeDataPoolListElement::eACCESS_RO;
         }
      }
      //Insert
      c_UIDataPool.c_DataPoolLists.insert(c_UIDataPool.c_DataPoolLists.begin() + oru32_DataPoolListIndex,
                                          orc_UIContent);
      c_OSCDataPool.c_Lists.insert(c_OSCDataPool.c_Lists.begin() + oru32_DataPoolListIndex, c_NodeDataPoolList);
      this->SetDataPool(oru32_NodeIndex, oru32_DataPoolIndex, c_OSCDataPool, c_UIDataPool);
      //Synchronization engine
      Q_EMIT this->SigSyncNodeDataPoolListAdded(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex);
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Remove data pool list

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Data pool index
   \param[in] oru32_DataPoolListIndex Data pool list index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::RemoveDataPoolList(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                          const uint32 & oru32_DataPoolListIndex)
{
   sint32 s32_Retval;
   C_PuiSdNodeDataPool c_UIDataPool;
   C_OSCNodeDataPool c_OSCDataPool;

   s32_Retval = this->GetDataPool(oru32_NodeIndex, oru32_DataPoolIndex, c_OSCDataPool, c_UIDataPool);
   if (s32_Retval == C_NO_ERR)
   {
      //Check size & consistency
      tgl_assert(c_UIDataPool.c_DataPoolLists.size() == c_OSCDataPool.c_Lists.size());
      if (oru32_DataPoolListIndex < c_UIDataPool.c_DataPoolLists.size())
      {
         //Synchronization engine (First!)
         Q_EMIT this->SigSyncNodeDataPoolListAboutToBeDeleted(oru32_NodeIndex, oru32_DataPoolIndex,
                                                              oru32_DataPoolListIndex);
         c_UIDataPool.c_DataPoolLists.erase(c_UIDataPool.c_DataPoolLists.begin() + oru32_DataPoolListIndex);
         c_OSCDataPool.c_Lists.erase(c_OSCDataPool.c_Lists.begin() + oru32_DataPoolListIndex);
         this->SetDataPool(oru32_NodeIndex, oru32_DataPoolIndex, c_OSCDataPool, c_UIDataPool);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set data pool list

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Data pool index
   \param[in] oru32_DataPoolListIndex Data pool list index
   \param[in] orc_OSCContent          OSC data pool list content
   \param[in] orc_UIContent           UI data pool list content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
   C_NOACT  Input invalid

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetDataPoolList(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                       const uint32 & oru32_DataPoolListIndex,
                                       const C_OSCNodeDataPoolList & orc_OSCContent,
                                       const C_PuiSdNodeDataPoolList & orc_UIContent)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UINode = this->mc_UINodes[oru32_NodeIndex];
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UINode.c_UIDataPools.size() == rc_OSCNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OSCNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UIDataPool = rc_UINode.c_UIDataPools[oru32_DataPoolIndex];
         C_OSCNodeDataPool & rc_OSCDataPool = rc_OSCNode.c_DataPools[oru32_DataPoolIndex];

         //Check size & consistency
         tgl_assert(rc_UIDataPool.c_DataPoolLists.size() == rc_OSCDataPool.c_Lists.size());
         if (oru32_DataPoolListIndex < rc_OSCDataPool.c_Lists.size())
         {
            if (orc_UIContent.c_DataPoolListElements.size() == orc_OSCContent.c_Elements.size())
            {
               rc_UIDataPool.c_DataPoolLists[oru32_DataPoolListIndex] = orc_UIContent;
               rc_OSCDataPool.c_Lists[oru32_DataPoolListIndex] = orc_OSCContent;
               //Update addresses
               rc_OSCDataPool.RecalculateAddress();
            }
            else
            {
               s32_Retval = C_NOACT;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get data pool list

   \param[in]  oru32_NodeIndex         Node index
   \param[in]  oru32_DataPoolIndex     Data pool index
   \param[in]  oru32_DataPoolListIndex Data pool list index
   \param[out] orc_OSCContent          OSC data pool list content
   \param[out] orc_UIContent           UI data pool list content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::GetDataPoolList(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                       const uint32 & oru32_DataPoolListIndex, C_OSCNodeDataPoolList & orc_OSCContent,
                                       C_PuiSdNodeDataPoolList & orc_UIContent) const
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNodeDataPoolList * const pc_OSCList = GetOSCDataPoolList(oru32_NodeIndex,
                                                                       oru32_DataPoolIndex,
                                                                       oru32_DataPoolListIndex);
   const C_PuiSdNodeDataPoolList * const pc_UIList = GetUIDataPoolList(oru32_NodeIndex,
                                                                       oru32_DataPoolIndex,
                                                                       oru32_DataPoolListIndex);

   if ((pc_OSCList != NULL) && (pc_UIList != NULL))
   {
      orc_UIContent = *pc_UIList;
      orc_OSCContent = *pc_OSCList;
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get data pool list data set

   \param[in]  oru32_NodeIndex                Node index
   \param[in]  oru32_DataPoolIndex            Data pool index
   \param[in]  oru32_DataPoolListIndex        Data pool list index
   \param[in]  oru32_DataPoolListDataSetIndex Data pool list data set index
   \param[out] orc_OSCName                    OSC data set name
   \param[out] orc_OSCValues                  OSC data set values

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     15.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::GetDataPoolListDataSet(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                              const uint32 & oru32_DataPoolListIndex,
                                              const uint32 & oru32_DataPoolListDataSetIndex,
                                              C_OSCNodeDataPoolDataSet & orc_OSCName,
                                              std::vector<C_OSCNodeDataPoolContent> & orc_OSCValues) const
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNodeDataPoolList * const pc_OSCList = GetOSCDataPoolList(oru32_NodeIndex,
                                                                       oru32_DataPoolIndex,
                                                                       oru32_DataPoolListIndex);

   if (pc_OSCList != NULL)
   {
      if (oru32_DataPoolListDataSetIndex < pc_OSCList->c_DataSets.size())
      {
         orc_OSCName = pc_OSCList->c_DataSets[oru32_DataPoolListDataSetIndex];
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      //Values
      orc_OSCValues.resize(pc_OSCList->c_Elements.size());
      for (uint32 u32_ItDataElement = 0; u32_ItDataElement < pc_OSCList->c_Elements.size(); ++u32_ItDataElement)
      {
         const C_OSCNodeDataPoolListElement & rc_Element = pc_OSCList->c_Elements[u32_ItDataElement];
         if (oru32_DataPoolListDataSetIndex < rc_Element.c_DataSetValues.size())
         {
            orc_OSCValues[u32_ItDataElement] = rc_Element.c_DataSetValues[oru32_DataPoolListDataSetIndex];
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get OSC data pool list data set

   \param[in] oru32_NodeIndex                Node index
   \param[in] oru32_DataPoolIndex            Data pool index
   \param[in] oru32_DataPoolListIndex        Data pool list index
   \param[in] oru32_DataPoolListDataSetIndex Data pool list data set index

   \return
   Found: Pointer to OSC data pool list data set
   Else:  NULL

   \created     23.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCNodeDataPoolDataSet * C_PuiSdHandler::GetOSCDataPoolListDataSet(const uint32 & oru32_NodeIndex,
                                                                           const uint32 & oru32_DataPoolIndex,
                                                                           const uint32 & oru32_DataPoolListIndex,
                                                                           const uint32 & oru32_DataPoolListDataSetIndex)
const
{
   const C_OSCNodeDataPoolDataSet * pc_Retval = NULL;
   const C_OSCNodeDataPoolList * const pc_OSCList = this->GetOSCDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                             oru32_DataPoolListIndex);

   if (pc_OSCList != NULL)
   {
      if (oru32_DataPoolListDataSetIndex < pc_OSCList->c_DataSets.size())
      {
         pc_Retval = &pc_OSCList->c_DataSets[oru32_DataPoolListDataSetIndex];
      }
   }

   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set data pool list data set

   \param[in] oru32_NodeIndex                Node index
   \param[in] oru32_DataPoolIndex            Data pool index
   \param[in] oru32_DataPoolListIndex        Data pool list index
   \param[in] oru32_DataPoolListDataSetIndex Data pool list data set index
   \param[in] orc_OSCContent                 OSC data pool list element content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     15.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetOSCNodeDataPoolDataSet(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                                 const uint32 & oru32_DataPoolListIndex,
                                                 const uint32 & oru32_DataPoolListDataSetIndex,
                                                 const C_OSCNodeDataPoolDataSet & orc_OSCContent)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OSCNode.c_DataPools.size())
      {
         C_OSCNodeDataPool & rc_OSCDataPool = rc_OSCNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OSCDataPool.c_Lists.size())
         {
            C_OSCNodeDataPoolList & rc_OSCList = rc_OSCDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if (oru32_DataPoolListDataSetIndex < rc_OSCList.c_DataSets.size())
            {
               rc_OSCList.c_DataSets[oru32_DataPoolListDataSetIndex] = orc_OSCContent;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Insert data pool list data set at specific position

   \param[in] oru32_NodeIndex                Node index
   \param[in] oru32_DataPoolIndex            Data pool index
   \param[in] oru32_DataPoolListIndex        Data pool list index
   \param[in] oru32_DataPoolListDataSetIndex Data pool list data set index
   \param[in] orc_OSCName                    OSC data pool list data set name
                                             (name might be modified by this function if not unique)
   \param[in] orc_OSCValues                  OSC data pool list data set values

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::InsertDataPoolListDataSet(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                                 const uint32 & oru32_DataPoolListIndex,
                                                 const uint32 & oru32_DataPoolListDataSetIndex,
                                                 const C_OSCNodeDataPoolDataSet & orc_OSCName,
                                                 const std::vector<C_OSCNodeDataPoolContent> & orc_OSCValues)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OSCNode.c_DataPools.size())
      {
         C_OSCNodeDataPool & rc_OSCDataPool = rc_OSCNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OSCDataPool.c_Lists.size())
         {
            C_OSCNodeDataPoolList & rc_OSCList = rc_OSCDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if (oru32_DataPoolListDataSetIndex <= rc_OSCList.c_DataSets.size())
            {
               //Unique name
               C_OSCNodeDataPoolDataSet c_NodeDataPoolDataSet = orc_OSCName;
               c_NodeDataPoolDataSet.c_Name =
                  C_PuiSdUtil::h_GetUniqueName(this->m_GetExistingNodeDataPoolListDataSetNames(oru32_NodeIndex,
                                                                                               oru32_DataPoolIndex,
                                                                                               oru32_DataPoolListIndex),
                                               c_NodeDataPoolDataSet.c_Name);
               //Insert
               rc_OSCList.c_DataSets.insert(
                  rc_OSCList.c_DataSets.begin() + oru32_DataPoolListDataSetIndex, c_NodeDataPoolDataSet);
            }
            else
            {
               s32_Retval = C_RANGE;
            }
            //Values
            tgl_assert(rc_OSCList.c_Elements.size() == orc_OSCValues.size());
            for (uint32 u32_ItDataElement = 0; u32_ItDataElement < rc_OSCList.c_Elements.size();
                 ++u32_ItDataElement)
            {
               C_OSCNodeDataPoolListElement & rc_DataElement = rc_OSCList.c_Elements[u32_ItDataElement];
               //Check size
               if (oru32_DataPoolListDataSetIndex <= rc_DataElement.c_DataSetValues.size())
               {
                  rc_DataElement.c_DataSetValues.insert(
                     rc_DataElement.c_DataSetValues.begin() + oru32_DataPoolListDataSetIndex,
                     orc_OSCValues[u32_ItDataElement]);
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            //Synchronization engine
            Q_EMIT this->SigSyncNodeDataPoolListDataSetAdded(oru32_NodeIndex, oru32_DataPoolIndex,
                                                             oru32_DataPoolListIndex,
                                                             oru32_DataPoolListDataSetIndex);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Remove data pool list data set

   \param[in] oru32_NodeIndex                Node index
   \param[in] oru32_DataPoolIndex            Data pool index
   \param[in] oru32_DataPoolListIndex        Data pool list index
   \param[in] oru32_DataPoolListDataSetIndex Data pool list data set index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::RemoveDataPoolListDataSet(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                                 const uint32 & oru32_DataPoolListIndex,
                                                 const uint32 & oru32_DataPoolListDataSetIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OSCNode.c_DataPools.size())
      {
         C_OSCNodeDataPool & rc_OSCDataPool = rc_OSCNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OSCDataPool.c_Lists.size())
         {
            C_OSCNodeDataPoolList & rc_OSCList = rc_OSCDataPool.c_Lists[oru32_DataPoolListIndex];

            //Synchronization engine (First!)
            Q_EMIT this->SigSyncNodeDataPoolListDataSetAboutToBeDeleted(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        oru32_DataPoolListIndex,
                                                                        oru32_DataPoolListDataSetIndex);

            //Check size
            if (oru32_DataPoolListDataSetIndex <= rc_OSCList.c_DataSets.size())
            {
               rc_OSCList.c_DataSets.erase(rc_OSCList.c_DataSets.begin() + oru32_DataPoolListDataSetIndex);
            }
            else
            {
               s32_Retval = C_RANGE;
            }
            //Values
            for (uint32 u32_ItDataElement = 0; u32_ItDataElement < rc_OSCList.c_Elements.size();
                 ++u32_ItDataElement)
            {
               C_OSCNodeDataPoolListElement & rc_DataElement = rc_OSCList.c_Elements[u32_ItDataElement];
               //Check size
               if (oru32_DataPoolListDataSetIndex <= rc_DataElement.c_DataSetValues.size())
               {
                  rc_DataElement.c_DataSetValues.erase(
                     rc_DataElement.c_DataSetValues.begin() + oru32_DataPoolListDataSetIndex);
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
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move data set in node data pool list

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Data pool index
   \param[in] oru32_DataPoolListIndex Data pool list index
   \param[in] oru32_SourceIndex       Source data set index
   \param[in] oru32_TargetIndex       Target data set index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::MoveDataPoolListDataSet(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                               const uint32 & oru32_DataPoolListIndex, const uint32 & oru32_SourceIndex,
                                               const uint32 & oru32_TargetIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OSCNode.c_DataPools.size())
      {
         C_OSCNodeDataPool & rc_OSCDataPool = rc_OSCNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OSCDataPool.c_Lists.size())
         {
            C_OSCNodeDataPoolList & rc_OSCList = rc_OSCDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if ((oru32_SourceIndex < rc_OSCList.c_DataSets.size()) &&
                (oru32_TargetIndex < rc_OSCList.c_DataSets.size()))
            {
               //Copy
               C_OSCNodeDataPoolDataSet c_Data = rc_OSCList.c_DataSets[oru32_SourceIndex];
               //Erase
               rc_OSCList.c_DataSets.erase(rc_OSCList.c_DataSets.begin() + oru32_SourceIndex);
               //Insert
               rc_OSCList.c_DataSets.insert(rc_OSCList.c_DataSets.begin() + oru32_TargetIndex, c_Data);
            }
            else
            {
               s32_Retval = C_RANGE;
            }
            //Values
            for (uint32 u32_ItDataElement = 0; u32_ItDataElement < rc_OSCList.c_Elements.size();
                 ++u32_ItDataElement)
            {
               C_OSCNodeDataPoolListElement & rc_DataElement = rc_OSCList.c_Elements[u32_ItDataElement];
               //Check size
               if ((oru32_SourceIndex < rc_DataElement.c_DataSetValues.size()) &&
                   (oru32_TargetIndex < rc_DataElement.c_DataSetValues.size()))
               {
                  //Copy
                  C_OSCNodeDataPoolContent c_Data = rc_DataElement.c_DataSetValues[oru32_SourceIndex];
                  //Erase
                  rc_DataElement.c_DataSetValues.erase(rc_DataElement.c_DataSetValues.begin() + oru32_SourceIndex);
                  //Insert
                  rc_DataElement.c_DataSetValues.insert(
                     rc_DataElement.c_DataSetValues.begin() + oru32_TargetIndex, c_Data);
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            //Synchronization engine
            Q_EMIT this->SigSyncNodeDataPoolListDataSetMoved(oru32_NodeIndex, oru32_DataPoolIndex,
                                                             oru32_DataPoolListIndex,
                                                             oru32_SourceIndex, oru32_TargetIndex);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get OSC data pool list

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Data pool index
   \param[in] oru32_DataPoolListIndex Data pool list index

   \return
   Found: Pointer to OSC data pool list
   Else:  NULL

   \created     23.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCNodeDataPoolList * C_PuiSdHandler::GetOSCDataPoolList(const uint32 & oru32_NodeIndex,
                                                                 const uint32 & oru32_DataPoolIndex,
                                                                 const uint32 & oru32_DataPoolListIndex) const
{
   const C_OSCNodeDataPoolList * pc_Retval = NULL;
   const C_OSCNodeDataPool * const pc_OSCDataPool = this->GetOSCDataPool(oru32_NodeIndex, oru32_DataPoolIndex);

   if (pc_OSCDataPool != NULL)
   {
      if (oru32_DataPoolListIndex < pc_OSCDataPool->c_Lists.size())
      {
         pc_Retval = &pc_OSCDataPool->c_Lists[oru32_DataPoolListIndex];
      }
   }

   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get UI data pool list

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Data pool index
   \param[in] oru32_DataPoolListIndex Data pool list index

   \return
   Found: Pointer to UI data pool list
   Else:  NULL

   \created     23.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiSdNodeDataPoolList * C_PuiSdHandler::GetUIDataPoolList(const uint32 & oru32_NodeIndex,
                                                                  const uint32 & oru32_DataPoolIndex,
                                                                  const uint32 & oru32_DataPoolListIndex) const
{
   const C_PuiSdNodeDataPoolList * pc_Retval = NULL;
   const C_PuiSdNodeDataPool * const pc_UIDataPool = this->GetUIDataPool(oru32_NodeIndex, oru32_DataPoolIndex);

   if (pc_UIDataPool != NULL)
   {
      if (oru32_DataPoolListIndex < pc_UIDataPool->c_DataPoolLists.size())
      {
         pc_Retval = &pc_UIDataPool->c_DataPoolLists[oru32_DataPoolListIndex];
      }
   }

   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move list in node data pool

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Data pool index
   \param[in] oru32_SourceIndex   Source list index
   \param[in] oru32_TargetIndex   Target list index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::MoveDataPoolList(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                        const uint32 & oru32_SourceIndex, const uint32 & oru32_TargetIndex)
{
   sint32 s32_Retval;
   C_OSCNodeDataPool c_OSCDataPool;
   C_PuiSdNodeDataPool c_UIDataPool;

   s32_Retval = this->GetDataPool(oru32_NodeIndex, oru32_DataPoolIndex, c_OSCDataPool, c_UIDataPool);
   if (s32_Retval == C_NO_ERR)
   {
      c_OSCDataPool.MoveList(oru32_SourceIndex, oru32_TargetIndex);
      if ((oru32_SourceIndex < c_UIDataPool.c_DataPoolLists.size()) &&
          (oru32_TargetIndex < c_UIDataPool.c_DataPoolLists.size()))
      {
         //Copy
         C_PuiSdNodeDataPoolList c_Data = c_UIDataPool.c_DataPoolLists[oru32_SourceIndex];
         //Erase
         c_UIDataPool.c_DataPoolLists.erase(c_UIDataPool.c_DataPoolLists.begin() + oru32_SourceIndex);
         //Insert
         c_UIDataPool.c_DataPoolLists.insert(c_UIDataPool.c_DataPoolLists.begin() + oru32_TargetIndex, c_Data);
      }
      this->SetDataPool(oru32_NodeIndex, oru32_DataPoolIndex, c_OSCDataPool, c_UIDataPool);
      //Synchronization engine
      Q_EMIT this->SigSyncNodeDataPoolListMoved(oru32_NodeIndex, oru32_DataPoolIndex, oru32_SourceIndex,
                                                oru32_TargetIndex);
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Insert data pool list element at specific position

   \param[in] oru32_NodeIndex                Node index
   \param[in] oru32_DataPoolIndex            Data pool index
   \param[in] oru32_DataPoolListIndex        Data pool list index
   \param[in] oru32_DataPoolListElementIndex Data pool list element index
   \param[in] orc_OSCContent                 OSC data pool list element content
                                             (name might be modified by this function if not unique)
   \param[in] orc_UIContent                  UI data pool list element content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::InsertDataPoolListElement(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                                 const uint32 & oru32_DataPoolListIndex,
                                                 const uint32 & oru32_DataPoolListElementIndex,
                                                 const C_OSCNodeDataPoolListElement & orc_OSCContent,
                                                 const C_PuiSdNodeDataPoolListElement & orc_UIContent)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNodeDataPool * const pc_OSCDataPool = this->GetOSCDataPool(oru32_NodeIndex, oru32_DataPoolIndex);

   if (pc_OSCDataPool != NULL)
   {
      C_PuiSdNodeDataPoolList c_UIList;
      C_OSCNodeDataPoolList c_OSCList;

      s32_Retval =
         this->GetDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex, c_OSCList, c_UIList);
      if (s32_Retval == C_NO_ERR)
      {
         //Unique name
         C_OSCNodeDataPoolListElement c_NodeDataPoolListElement = orc_OSCContent;
         c_NodeDataPoolListElement.c_Name =
            C_PuiSdUtil::h_GetUniqueName(this->m_GetExistingNodeDataPoolListVariableNames(oru32_NodeIndex,
                                                                                          oru32_DataPoolIndex,
                                                                                          oru32_DataPoolListIndex),
                                         c_NodeDataPoolListElement.c_Name);
         //Adapt required fields
         if (pc_OSCDataPool->q_IsSafety == true)
         {
            c_NodeDataPoolListElement.e_Access = C_OSCNodeDataPoolListElement::eACCESS_RO;
         }
         //Insert
         c_UIList.c_DataPoolListElements.insert(
            c_UIList.c_DataPoolListElements.begin() + oru32_DataPoolListElementIndex, orc_UIContent);
         c_OSCList.c_Elements.insert(c_OSCList.c_Elements.begin() + oru32_DataPoolListElementIndex,
                                     c_NodeDataPoolListElement);
         this->SetDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex, c_OSCList, c_UIList);
         //Synchronization engine
         Q_EMIT this->SigSyncNodeDataPoolListElementAdded(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                          oru32_DataPoolListElementIndex);
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
   \brief   Remove data pool list element

   \param[in] oru32_NodeIndex                Node index
   \param[in] oru32_DataPoolIndex            Data pool index
   \param[in] oru32_DataPoolListIndex        Data pool list index
   \param[in] oru32_DataPoolListElementIndex Data pool list element index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::RemoveDataPoolListElement(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                                 const uint32 & oru32_DataPoolListIndex,
                                                 const uint32 & oru32_DataPoolListElementIndex)
{
   sint32 s32_Retval;
   C_PuiSdNodeDataPoolList c_UIList;
   C_OSCNodeDataPoolList c_OSCList;

   s32_Retval =
      this->GetDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex, c_OSCList, c_UIList);
   if (s32_Retval == C_NO_ERR)
   {
      //Check size & consistency
      tgl_assert(c_UIList.c_DataPoolListElements.size() == c_OSCList.c_Elements.size());
      if (oru32_DataPoolListElementIndex < c_UIList.c_DataPoolListElements.size())
      {
         //Synchronization engine (First!)
         Q_EMIT this->SigSyncNodeDataPoolListElementAboutToBeDeleted(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                     oru32_DataPoolListIndex,
                                                                     oru32_DataPoolListElementIndex);
         c_UIList.c_DataPoolListElements.erase(c_UIList.c_DataPoolListElements.begin() +
                                               oru32_DataPoolListElementIndex);
         c_OSCList.c_Elements.erase(c_OSCList.c_Elements.begin() + oru32_DataPoolListElementIndex);
         this->SetDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex, c_OSCList, c_UIList);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set data pool list element

   \param[in] oru32_NodeIndex                Node index
   \param[in] oru32_DataPoolIndex            Data pool index
   \param[in] oru32_DataPoolListIndex        Data pool list index
   \param[in] oru32_DataPoolListElementIndex Data pool list element index
   \param[in] orc_OSCContent                 OSC data pool list element content
   \param[in] orc_UIContent                  UI data pool list element content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetDataPoolListElement(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                              const uint32 & oru32_DataPoolListIndex,
                                              const uint32 & oru32_DataPoolListElementIndex,
                                              const C_OSCNodeDataPoolListElement & orc_OSCContent,
                                              const C_PuiSdNodeDataPoolListElement & orc_UIContent)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UINode = this->mc_UINodes[oru32_NodeIndex];
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UINode.c_UIDataPools.size() == rc_OSCNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OSCNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UIDataPool = rc_UINode.c_UIDataPools[oru32_DataPoolIndex];
         C_OSCNodeDataPool & rc_OSCDataPool = rc_OSCNode.c_DataPools[oru32_DataPoolIndex];

         //Check size & consistency
         tgl_assert(rc_UIDataPool.c_DataPoolLists.size() == rc_OSCDataPool.c_Lists.size());
         if (oru32_DataPoolListIndex < rc_OSCDataPool.c_Lists.size())
         {
            C_PuiSdNodeDataPoolList & rc_UIList = rc_UIDataPool.c_DataPoolLists[oru32_DataPoolListIndex];
            C_OSCNodeDataPoolList & rc_OSCList = rc_OSCDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size & consistency
            tgl_assert(rc_UIList.c_DataPoolListElements.size() == rc_OSCList.c_Elements.size());
            if (oru32_DataPoolListElementIndex < rc_UIList.c_DataPoolListElements.size())
            {
               C_OSCNodeDataPoolListElement & rc_OSCDataElement = rc_OSCList.c_Elements[oru32_DataPoolListElementIndex];
               if ((rc_OSCDataElement.GetType() != orc_OSCContent.GetType()) ||
                   ((rc_OSCDataElement.GetArray() != orc_OSCContent.GetArray()) ||
                    (rc_OSCDataElement.GetArraySize() !=
                     orc_OSCContent.GetArraySize())))
               {
                  Q_EMIT this->SigSyncNodeDataPoolListElementChanged(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                     oru32_DataPoolListIndex,
                                                                     oru32_DataPoolListElementIndex,
                                                                     orc_OSCContent.GetType(),
                                                                     orc_OSCContent.GetArray(),
                                                                     orc_OSCContent.GetArraySize());
               }
               if (rc_OSCDataElement.e_Access != orc_OSCContent.e_Access)
               {
                  Q_EMIT this->SigSyncNodeDataPoolListElementAccessChanged(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                           oru32_DataPoolListIndex,
                                                                           oru32_DataPoolListElementIndex,
                                                                           orc_OSCContent.e_Access);
               }
               rc_UIList.c_DataPoolListElements[oru32_DataPoolListElementIndex] = orc_UIContent;
               rc_OSCDataElement = orc_OSCContent;
               //Update addresses
               rc_OSCDataPool.RecalculateAddress();
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get data pool list element

   \param[in]  oru32_NodeIndex                Node index
   \param[in]  oru32_DataPoolIndex            Data pool index
   \param[in]  oru32_DataPoolListIndex        Data pool list index
   \param[in]  oru32_DataPoolListElementIndex Data pool list element index
   \param[out] orc_OSCContent                 OSC data pool list element content
   \param[out] orc_UIContent                  UI data pool list element content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::GetDataPoolListElement(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                              const uint32 & oru32_DataPoolListIndex,
                                              const uint32 & oru32_DataPoolListElementIndex,
                                              C_OSCNodeDataPoolListElement & orc_OSCContent,
                                              C_PuiSdNodeDataPoolListElement & orc_UIContent) const
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNodeDataPoolListElement * const pc_OSCListElement = GetOSCDataPoolListElement(oru32_NodeIndex,
                                                                                            oru32_DataPoolIndex,
                                                                                            oru32_DataPoolListIndex,
                                                                                            oru32_DataPoolListElementIndex);
   const C_PuiSdNodeDataPoolListElement * const pc_UIListElement = GetUIDataPoolListElement(oru32_NodeIndex,
                                                                                            oru32_DataPoolIndex,
                                                                                            oru32_DataPoolListIndex,
                                                                                            oru32_DataPoolListElementIndex);

   if ((pc_OSCListElement != NULL) && (pc_UIListElement != NULL))
   {
      orc_UIContent = *pc_UIListElement;
      orc_OSCContent = *pc_OSCListElement;
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize data element name & comment

   \param[in]     ore_Type       Data pool type
   \param[in]     oq_IsSafety    Flag if datapool is safety datapool
   \param[in,out] orc_OSCElement Data element

   \created     08.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::h_InitDataElement(const C_OSCNodeDataPool::E_Type & ore_Type, const bool oq_IsSafety,
                                       C_OSCNodeDataPoolListElement & orc_OSCElement)
{
   const QString c_Type = C_PuiSdHandler::h_GetElementTypeName(ore_Type);

   //Translation: 1: Data element type
   orc_OSCElement.c_Name = QString(QString("New%1").arg(c_Type)).toStdString().c_str();
   //Translation: 1: Data element type
   orc_OSCElement.c_Comment = QString(QString("%1 description").arg(c_Type)).toStdString().c_str();
   //Default access value
   if (ore_Type == C_OSCNodeDataPool::eDIAG)
   {
      orc_OSCElement.e_Access = C_OSCNodeDataPoolListElement::eACCESS_RO;
   }
   else
   {
      if (oq_IsSafety == false)
      {
         orc_OSCElement.e_Access = C_OSCNodeDataPoolListElement::eACCESS_RW;
      }
      else
      {
         orc_OSCElement.e_Access = C_OSCNodeDataPoolListElement::eACCESS_RO;
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get data pool type dependent element name

   \param[in] ore_Type Data pool type

   \created     29.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_PuiSdHandler::h_GetElementTypeName(const C_OSCNodeDataPool::E_Type & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case C_OSCNodeDataPool::eDIAG:
      c_Retval = "Variable";
      break;
   case C_OSCNodeDataPool::eNVM:
      c_Retval = "Parameter";
      break;
   case C_OSCNodeDataPool::eCOM:
      c_Retval = "Signal";
      break;
   default:
      c_Retval = "Data Element";
      break;
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set data pool list element min array value

   \param[in] oru32_NodeIndex                Node index
   \param[in] oru32_DataPoolIndex            Data pool index
   \param[in] oru32_DataPoolListIndex        Data pool list index
   \param[in] oru32_DataPoolListElementIndex Data pool list element index
   \param[in] oru32_ArrayIndex               Array index
   \param[in] orc_OSCContent                 New value (Expected: No array & same type as in element)

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     21.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetDataPoolListElementMinArray(const uint32 & oru32_NodeIndex,
                                                      const uint32 & oru32_DataPoolIndex,
                                                      const uint32 & oru32_DataPoolListIndex,
                                                      const uint32 & oru32_DataPoolListElementIndex,
                                                      const uint32 & oru32_ArrayIndex,
                                                      const C_OSCNodeDataPoolContent & orc_OSCContent)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OSCNode.c_DataPools.size())
      {
         C_OSCNodeDataPool & rc_OSCDataPool = rc_OSCNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OSCDataPool.c_Lists.size())
         {
            C_OSCNodeDataPoolList & rc_OSCList = rc_OSCDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if (oru32_DataPoolListElementIndex < rc_OSCList.c_Elements.size())
            {
               C_OSCNodeDataPoolListElement & rc_Element = rc_OSCList.c_Elements[oru32_DataPoolListElementIndex];
               if (oru32_ArrayIndex < rc_Element.GetArraySize())
               {
                  //Check input correct
                  tgl_assert(orc_OSCContent.GetType() == rc_Element.GetType());
                  tgl_assert(orc_OSCContent.GetArray() == false);
                  switch (orc_OSCContent.GetType())
                  {
                  case C_OSCNodeDataPoolContent::eUINT8:
                     rc_Element.c_MinValue.SetValueAU8Element(orc_OSCContent.GetValueU8(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eUINT16:
                     rc_Element.c_MinValue.SetValueAU16Element(orc_OSCContent.GetValueU16(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eUINT32:
                     rc_Element.c_MinValue.SetValueAU32Element(orc_OSCContent.GetValueU32(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eUINT64:
                     rc_Element.c_MinValue.SetValueAU64Element(orc_OSCContent.GetValueU64(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT8:
                     rc_Element.c_MinValue.SetValueAS8Element(orc_OSCContent.GetValueS8(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT16:
                     rc_Element.c_MinValue.SetValueAS16Element(orc_OSCContent.GetValueS16(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT32:
                     rc_Element.c_MinValue.SetValueAS32Element(orc_OSCContent.GetValueS32(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT64:
                     rc_Element.c_MinValue.SetValueAS64Element(orc_OSCContent.GetValueS64(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eFLOAT32:
                     rc_Element.c_MinValue.SetValueAF32Element(orc_OSCContent.GetValueF32(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eFLOAT64:
                     rc_Element.c_MinValue.SetValueAF64Element(orc_OSCContent.GetValueF64(), oru32_ArrayIndex);
                     break;
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

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set data pool list element max array value

   \param[in] oru32_NodeIndex                Node index
   \param[in] oru32_DataPoolIndex            Data pool index
   \param[in] oru32_DataPoolListIndex        Data pool list index
   \param[in] oru32_DataPoolListElementIndex Data pool list element index
   \param[in] oru32_ArrayIndex               Array index
   \param[in] orc_OSCContent                 New value (Expected: No array & same type as in element)

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     21.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetDataPoolListElementMaxArray(const uint32 & oru32_NodeIndex,
                                                      const uint32 & oru32_DataPoolIndex,
                                                      const uint32 & oru32_DataPoolListIndex,
                                                      const uint32 & oru32_DataPoolListElementIndex,
                                                      const uint32 & oru32_ArrayIndex,
                                                      const C_OSCNodeDataPoolContent & orc_OSCContent)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OSCNode.c_DataPools.size())
      {
         C_OSCNodeDataPool & rc_OSCDataPool = rc_OSCNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OSCDataPool.c_Lists.size())
         {
            C_OSCNodeDataPoolList & rc_OSCList = rc_OSCDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if (oru32_DataPoolListElementIndex < rc_OSCList.c_Elements.size())
            {
               C_OSCNodeDataPoolListElement & rc_Element = rc_OSCList.c_Elements[oru32_DataPoolListElementIndex];
               if (oru32_ArrayIndex < rc_Element.GetArraySize())
               {
                  //Check input correct
                  tgl_assert(orc_OSCContent.GetType() == rc_Element.GetType());
                  tgl_assert(orc_OSCContent.GetArray() == false);
                  switch (orc_OSCContent.GetType())
                  {
                  case C_OSCNodeDataPoolContent::eUINT8:
                     rc_Element.c_MaxValue.SetValueAU8Element(orc_OSCContent.GetValueU8(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eUINT16:
                     rc_Element.c_MaxValue.SetValueAU16Element(orc_OSCContent.GetValueU16(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eUINT32:
                     rc_Element.c_MaxValue.SetValueAU32Element(orc_OSCContent.GetValueU32(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eUINT64:
                     rc_Element.c_MaxValue.SetValueAU64Element(orc_OSCContent.GetValueU64(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT8:
                     rc_Element.c_MaxValue.SetValueAS8Element(orc_OSCContent.GetValueS8(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT16:
                     rc_Element.c_MaxValue.SetValueAS16Element(orc_OSCContent.GetValueS16(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT32:
                     rc_Element.c_MaxValue.SetValueAS32Element(orc_OSCContent.GetValueS32(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT64:
                     rc_Element.c_MaxValue.SetValueAS64Element(orc_OSCContent.GetValueS64(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eFLOAT32:
                     rc_Element.c_MaxValue.SetValueAF32Element(orc_OSCContent.GetValueF32(), oru32_ArrayIndex);
                     break;
                  case C_OSCNodeDataPoolContent::eFLOAT64:
                     rc_Element.c_MaxValue.SetValueAF64Element(orc_OSCContent.GetValueF64(), oru32_ArrayIndex);
                     break;
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

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set data pool list element data set array value

   \param[in] oru32_NodeIndex                       Node index
   \param[in] oru32_DataPoolIndex                   Data pool index
   \param[in] oru32_DataPoolListIndex               Data pool list index
   \param[in] oru32_DataPoolListElementIndex        Data pool list element index
   \param[in] oru32_DataPoolListElementDataSetIndex Data pool list element index
   \param[in] oru32_ArrayIndex                      Array index
   \param[in] orc_OSCContent                        New value (Expected: No array & same type as in element)

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     21.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetDataPoolListElementDataSetArray(const uint32 & oru32_NodeIndex,
                                                          const uint32 & oru32_DataPoolIndex,
                                                          const uint32 & oru32_DataPoolListIndex,
                                                          const uint32 & oru32_DataPoolListElementIndex,
                                                          const uint32 & oru32_DataPoolListElementDataSetIndex,
                                                          const uint32 & oru32_ArrayIndex,
                                                          const C_OSCNodeDataPoolContent & orc_OSCContent)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OSCNode.c_DataPools.size())
      {
         C_OSCNodeDataPool & rc_OSCDataPool = rc_OSCNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OSCDataPool.c_Lists.size())
         {
            C_OSCNodeDataPoolList & rc_OSCList = rc_OSCDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if (oru32_DataPoolListElementIndex < rc_OSCList.c_Elements.size())
            {
               C_OSCNodeDataPoolListElement & rc_Element = rc_OSCList.c_Elements[oru32_DataPoolListElementIndex];
               //Check size
               if (oru32_DataPoolListElementDataSetIndex < rc_Element.c_DataSetValues.size())
               {
                  C_OSCNodeDataPoolContent & rc_DataSet =
                     rc_Element.c_DataSetValues[oru32_DataPoolListElementDataSetIndex];
                  if (oru32_ArrayIndex < rc_DataSet.GetArraySize())
                  {
                     //Check input correct
                     tgl_assert(orc_OSCContent.GetType() == rc_Element.GetType());
                     tgl_assert(orc_OSCContent.GetArray() == false);
                     switch (orc_OSCContent.GetType())
                     {
                     case C_OSCNodeDataPoolContent::eUINT8:
                        rc_DataSet.SetValueAU8Element(orc_OSCContent.GetValueU8(), oru32_ArrayIndex);
                        break;
                     case C_OSCNodeDataPoolContent::eUINT16:
                        rc_DataSet.SetValueAU16Element(orc_OSCContent.GetValueU16(), oru32_ArrayIndex);
                        break;
                     case C_OSCNodeDataPoolContent::eUINT32:
                        rc_DataSet.SetValueAU32Element(orc_OSCContent.GetValueU32(), oru32_ArrayIndex);
                        break;
                     case C_OSCNodeDataPoolContent::eUINT64:
                        rc_DataSet.SetValueAU64Element(orc_OSCContent.GetValueU64(), oru32_ArrayIndex);
                        break;
                     case C_OSCNodeDataPoolContent::eSINT8:
                        rc_DataSet.SetValueAS8Element(orc_OSCContent.GetValueS8(), oru32_ArrayIndex);
                        break;
                     case C_OSCNodeDataPoolContent::eSINT16:
                        rc_DataSet.SetValueAS16Element(orc_OSCContent.GetValueS16(), oru32_ArrayIndex);
                        break;
                     case C_OSCNodeDataPoolContent::eSINT32:
                        rc_DataSet.SetValueAS32Element(orc_OSCContent.GetValueS32(), oru32_ArrayIndex);
                        break;
                     case C_OSCNodeDataPoolContent::eSINT64:
                        rc_DataSet.SetValueAS64Element(orc_OSCContent.GetValueS64(), oru32_ArrayIndex);
                        break;
                     case C_OSCNodeDataPoolContent::eFLOAT32:
                        rc_DataSet.SetValueAF32Element(orc_OSCContent.GetValueF32(), oru32_ArrayIndex);
                        break;
                     case C_OSCNodeDataPoolContent::eFLOAT64:
                        rc_DataSet.SetValueAF64Element(orc_OSCContent.GetValueF64(), oru32_ArrayIndex);
                        break;
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

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set data pool list element NVM value

   \param[in] oru32_NodeIndex                Node index
   \param[in] oru32_DataPoolIndex            Data pool index
   \param[in] oru32_DataPoolListIndex        Data pool list index
   \param[in] oru32_DataPoolListElementIndex Data pool list element index
   \param[in] orc_OSCContent                 New value

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     26.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetDataPoolListElementNVMValue(const uint32 & oru32_NodeIndex,
                                                      const uint32 & oru32_DataPoolIndex,
                                                      const uint32 & oru32_DataPoolListIndex,
                                                      const uint32 & oru32_DataPoolListElementIndex,
                                                      const C_OSCNodeDataPoolContent & orc_OSCContent)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OSCNode.c_DataPools.size())
      {
         C_OSCNodeDataPool & rc_OSCDataPool = rc_OSCNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OSCDataPool.c_Lists.size())
         {
            C_OSCNodeDataPoolList & rc_OSCList = rc_OSCDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if (oru32_DataPoolListElementIndex < rc_OSCList.c_Elements.size())
            {
               C_OSCNodeDataPoolListElement & rc_Element = rc_OSCList.c_Elements[oru32_DataPoolListElementIndex];
               rc_Element.c_NvmValue = orc_OSCContent;
               rc_Element.q_NvMValueChanged = true;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get OSC data pool list element

   \param[in] oru32_NodeIndex                Node index
   \param[in] oru32_DataPoolIndex            Data pool index
   \param[in] oru32_DataPoolListIndex        Data pool list index
   \param[in] oru32_DataPoolListElementIndex Data pool list element index

   \return
   Found: Pointer to OSC data pool list element
   Else:  NULL

   \created     23.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCNodeDataPoolListElement * C_PuiSdHandler::GetOSCDataPoolListElement(const uint32 & oru32_NodeIndex,
                                                                               const uint32 & oru32_DataPoolIndex,
                                                                               const uint32 & oru32_DataPoolListIndex,
                                                                               const uint32 & oru32_DataPoolListElementIndex)
const
{
   const C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   const C_OSCNodeDataPoolList * const pc_OSCList = this->GetOSCDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                             oru32_DataPoolListIndex);

   if (pc_OSCList != NULL)
   {
      if (oru32_DataPoolListElementIndex < pc_OSCList->c_Elements.size())
      {
         pc_Retval = &pc_OSCList->c_Elements[oru32_DataPoolListElementIndex];
      }
   }

   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get OSC data pool list element

   \param[in] orc_Id Aggregated identification indices

   \return
   Found: Pointer to OSC data pool list element
   Else:  NULL

   \created     10.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCNodeDataPoolListElement * C_PuiSdHandler::GetOSCDataPoolListElement(
   const C_OSCNodeDataPoolListElementId & orc_Id) const
{
   return this->GetOSCDataPoolListElement(orc_Id.u32_NodeIndex, orc_Id.u32_DataPoolIndex, orc_Id.u32_ListIndex,
                                          orc_Id.u32_ElementIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get UI data pool list element

   \param[in] oru32_NodeIndex                Node index
   \param[in] oru32_DataPoolIndex            Data pool index
   \param[in] oru32_DataPoolListIndex        Data pool list index
   \param[in] oru32_DataPoolListElementIndex Data pool list element index

   \return
   Found: Pointer to UI data pool list element
   Else:  NULL

   \created     23.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiSdNodeDataPoolListElement * C_PuiSdHandler::GetUIDataPoolListElement(const uint32 & oru32_NodeIndex,
                                                                                const uint32 & oru32_DataPoolIndex,
                                                                                const uint32 & oru32_DataPoolListIndex,
                                                                                const uint32 & oru32_DataPoolListElementIndex)
const
{
   const C_PuiSdNodeDataPoolListElement * pc_Retval = NULL;
   const C_PuiSdNodeDataPoolList * const pc_UIList = this->GetUIDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                             oru32_DataPoolListIndex);

   if (pc_UIList != NULL)
   {
      if (oru32_DataPoolListElementIndex < pc_UIList->c_DataPoolListElements.size())
      {
         pc_Retval = &pc_UIList->c_DataPoolListElements[oru32_DataPoolListElementIndex];
      }
   }

   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move element in node data pool list

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Data pool index
   \param[in] oru32_DataPoolListIndex Data pool list index
   \param[in] oru32_SourceIndex       Source list index
   \param[in] oru32_TargetIndex       Target list index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     06.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::MoveDataPoolListElement(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                               const uint32 & oru32_DataPoolListIndex, const uint32 & oru32_SourceIndex,
                                               const uint32 & oru32_TargetIndex)
{
   sint32 s32_Retval;
   C_OSCNodeDataPoolList c_OSCDataPoolList;
   C_PuiSdNodeDataPoolList c_UIDataPoolList;

   s32_Retval = this->GetDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex, c_OSCDataPoolList,
                                      c_UIDataPoolList);
   if (s32_Retval == C_NO_ERR)
   {
      c_OSCDataPoolList.MoveElement(oru32_SourceIndex, oru32_TargetIndex);
      if ((oru32_SourceIndex < c_UIDataPoolList.c_DataPoolListElements.size()) &&
          (oru32_TargetIndex < c_UIDataPoolList.c_DataPoolListElements.size()))
      {
         //Copy
         C_PuiSdNodeDataPoolListElement c_Data = c_UIDataPoolList.c_DataPoolListElements[oru32_SourceIndex];
         //Erase
         c_UIDataPoolList.c_DataPoolListElements.erase(
            c_UIDataPoolList.c_DataPoolListElements.begin() + oru32_SourceIndex);
         //Insert
         c_UIDataPoolList.c_DataPoolListElements.insert(
            c_UIDataPoolList.c_DataPoolListElements.begin() + oru32_TargetIndex, c_Data);
      }
      this->SetDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex, c_OSCDataPoolList,
                            c_UIDataPoolList);
      //Synchronization engine
      Q_EMIT this->SigSyncNodeDataPoolListElementMoved(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                       oru32_SourceIndex, oru32_TargetIndex);
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get com protocol data

   \param[in] oru32_NodeIndex Node index
   \param[in] ore_ComType     Can protocol type

   \return
   NULL No matching data found
   Else Pointer to data

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCCanProtocol * C_PuiSdHandler::GetCanProtocol(const uint32 & oru32_NodeIndex,
                                                        const C_OSCCanProtocol::E_Type & ore_ComType) const
{
   const C_OSCCanProtocol * pc_Retval = NULL;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      pc_Retval = rc_Node.GetCANProtocolConst(ore_ComType);
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get com data pool (common)

   \param[in] oru32_NodeIndex Node index
   \param[in] ore_ComType     Can protocol type

   \return
   NULL No matching data found
   Else Pointer to data

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCNodeDataPool * C_PuiSdHandler::GetOSCCanDataPool(const uint32 & oru32_NodeIndex,
                                                            const C_OSCCanProtocol::E_Type & ore_ComType) const
{
   const C_OSCNodeDataPool * pc_Retval = NULL;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      pc_Retval = rc_Node.GetComDataPoolConst(ore_ComType);
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get com data pool (ui)

   \param[in] oru32_NodeIndex Node index
   \param[in] ore_ComType     Can protocol type

   \return
   NULL No matching data found
   Else Pointer to data

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiSdNodeDataPool * C_PuiSdHandler::GetUiCanDataPool(const uint32 & oru32_NodeIndex,
                                                             const C_OSCCanProtocol::E_Type & ore_ComType) const
{
   const C_PuiSdNodeDataPool * pc_Retval = NULL;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      const C_OSCCanProtocol * pc_CanProtocol = rc_OSCNode.GetCANProtocolConst(ore_ComType);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get com protocol (ui)

   \param[in] oru32_NodeIndex Node index
   \param[in] ore_ComType     Can protocol type

   \return
   NULL No matching data found
   Else Pointer to data

   \created     13.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiSdNodeCanProtocol * C_PuiSdHandler::GetUiCanProtocolConst(const uint32 & oru32_NodeIndex,
                                                                     const C_OSCCanProtocol::E_Type & ore_ComType) const
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
         if (rc_CanProtocol.e_Type == ore_ComType)
         {
            pc_Retval = &rc_UINode.c_UICanProtocols[u32_ItCanProtocol];
         }
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get message container

   \param[in] oru32_NodeIndex      Node index
   \param[in] ore_ComType          Can protocol type
   \param[in] oru32_InterfaceIndex Node interface index

   \return
   NULL No matching data found
   Else Pointer to data

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCCanMessageContainer * C_PuiSdHandler::GetCanProtocolMessageContainer(const uint32 & oru32_NodeIndex,
                                                                                const C_OSCCanProtocol::E_Type & ore_ComType,
                                                                                const uint32 & oru32_InterfaceIndex)
const
{
   const C_OSCCanMessageContainer * pc_Retval = NULL;
   const C_OSCCanProtocol * const pc_Protocol = this->GetCanProtocol(oru32_NodeIndex, ore_ComType);

   if (pc_Protocol != NULL)
   {
      if (oru32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
      {
         pc_Retval = &pc_Protocol->c_ComMessages[oru32_InterfaceIndex];
      }
   }

   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get message data

   \param[in] orc_MessageId Message identification indices

   \return
   NULL No matching data found
   Else Pointer to data

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCCanMessage * C_PuiSdHandler::GetCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   const C_OSCCanMessage * pc_Retval = NULL;
   const C_OSCCanMessageContainer * const pc_MessageContainer = this->GetCanProtocolMessageContainer(
      orc_MessageId.u32_NodeIndex,
      orc_MessageId.e_ComProtocol,
      orc_MessageId.u32_InterfaceIndex);

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

//-----------------------------------------------------------------------------
/*!
   \brief   Copy can message with all related data structures

   \param[in]  orc_MessageId        Message identification indices
   \param[out] orc_Message          Message data
   \param[out] orc_OSCSignalCommons Signals data (osc common)
   \param[out] orc_UISignalCommons  Signals data (ui common)
   \param[out] orc_UISignals        Signals data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::GetCanMessageComplete(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
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
                                                                                orc_MessageId.q_MessageIsTx);
   const C_PuiSdNodeDataPoolList * const pc_UiList = this->m_GetUiCanDataPoolList(orc_MessageId.u32_NodeIndex,
                                                                                  orc_MessageId.e_ComProtocol,
                                                                                  orc_MessageId.u32_InterfaceIndex,
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get signal list (common)

   \param[in] oru32_NodeIndex      Node index
   \param[in] ore_ComType          Can protocol type
   \param[in] oru32_InterfaceIndex Node interface index
   \param[in] orq_MessageIsTx      Flag if message is tx type (else rx type assumed)

   \return
   NULL No matching data found
   Else Pointer to data

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCNodeDataPoolList * C_PuiSdHandler::GetOSCCanDataPoolList(const uint32 & oru32_NodeIndex,
                                                                    const C_OSCCanProtocol::E_Type & ore_ComType,
                                                                    const uint32 & oru32_InterfaceIndex,
                                                                    const bool & orq_MessageIsTx) const
{
   const C_OSCNodeDataPoolList * pc_Retval = NULL;
   const C_OSCNodeDataPool * pc_DataPool = this->GetOSCCanDataPool(oru32_NodeIndex, ore_ComType);

   if (pc_DataPool != NULL)
   {
      pc_Retval = C_OSCCanProtocol::h_GetComListConst(*pc_DataPool, oru32_InterfaceIndex, orq_MessageIsTx);
   }

   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get can signal data (position)

   \param[in] orc_MessageId     Message identification indices
   \param[in] oru32_SignalIndex Signal index

   \return
   NULL No matching data found
   Else Pointer to data

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCCanSignal * C_PuiSdHandler::GetCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get can message data (ui)

   \param[in] orc_MessageId Message identification indices

   \return
   NULL No matching data found
   Else Pointer to data

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiSdNodeCanMessage * C_PuiSdHandler::GetUiCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
const
{
   const C_PuiSdNodeCanMessage * pc_Retval = NULL;
   const C_PuiSdNodeCanProtocol * const pc_CanProtocol = this->GetUiCanProtocolConst(orc_MessageId.u32_NodeIndex,
                                                                                     orc_MessageId.e_ComProtocol);

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

//-----------------------------------------------------------------------------
/*!
   \brief   Get can signal data (ui)

   \param[in] orc_MessageId     Message identification indices
   \param[in] oru32_SignalIndex Signal index

   \return
   NULL No matching data found
   Else Pointer to data

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiSdNodeCanSignal * C_PuiSdHandler::GetUiCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                            const uint32 & oru32_SignalIndex) const
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get can signal data (osc common)

   \param[in] orc_MessageId     Message identification indices
   \param[in] oru32_SignalIndex Signal index

   \return
   NULL No matching data found
   Else Pointer to data

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCNodeDataPoolListElement * C_PuiSdHandler::GetOSCCanDataPoolListElement(
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get can signal data (ui common)

   \param[in] orc_MessageId     Message identification indices
   \param[in] oru32_SignalIndex Signal index

   \return
   NULL No matching data found
   Else Pointer to data

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiSdNodeDataPoolListElement * C_PuiSdHandler::GetUiCanDataPoolListElement(
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get connection data

   \param[in] ou32_NodeIndex      Node index
   \param[in] ou32_InterfaceIndex Interface index
   \param[in] oe_Type             Interface type

   \return
   NULL Connection not found
   Else Valid connection

   \created     12.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiSdNodeConnection * C_PuiSdHandler::GetUiConnection(const uint32 ou32_NodeIndex,
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

//-----------------------------------------------------------------------------
/*!
   \brief   Copy can signal with all related data structures

   \param[in]  orc_MessageId       Message identification indices
   \param[in]  oru32_SignalIndex   Signal index
   \param[out] orc_Signal          Signal data (osc)
   \param[out] orc_OSCSignalCommon Signal data (osc common)
   \param[out] orc_UISignalCommon  Signal data (ui common)
   \param[out] orc_UISignal        Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     24.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::GetCanSignalComplete(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
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

   \created     19.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::CheckMessageMatch(const C_OSCCanMessageIdentificationIndices & orc_MessageId1,
                                         const C_OSCCanMessageIdentificationIndices & orc_MessageId2,
                                         bool & orq_IsMatch, const bool oq_IgnoreMessageDirection) const
{
   sint32 s32_Retval = this->mc_CoreDefinition.CheckMessageMatch(orc_MessageId1, orc_MessageId2, orq_IsMatch,
                                                                 oq_IgnoreMessageDirection);

   //Check Ui part
   if ((s32_Retval == C_NO_ERR) && (orq_IsMatch == true))
   {
      const C_OSCCanMessageContainer * const pc_MessageContainer1 =
         C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(orc_MessageId1.u32_NodeIndex,
                                                                         orc_MessageId1.e_ComProtocol,
                                                                         orc_MessageId1.u32_InterfaceIndex);
      const C_OSCCanMessageContainer * const pc_MessageContainer2 =
         C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(orc_MessageId2.u32_NodeIndex,
                                                                         orc_MessageId2.e_ComProtocol,
                                                                         orc_MessageId2.u32_InterfaceIndex);
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
                     orc_MessageId1.q_MessageIsTx);
                  const C_PuiSdNodeDataPoolList * const pc_List2 = this->m_GetUiCanDataPoolList(
                     orc_MessageId2.u32_NodeIndex,
                     orc_MessageId2.e_ComProtocol,
                     orc_MessageId2.u32_InterfaceIndex,
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

//-----------------------------------------------------------------------------
/*!
   \brief   Change protocol type

   \param[in] oru32_NodeIndex     Node index
   \param[in] ore_PreviousComType Previous can protocol type
   \param[in] ore_NewComType      New can protocol type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::ChangeCanProtocolType(const uint32 & oru32_NodeIndex,
                                             const C_OSCCanProtocol::E_Type & ore_PreviousComType,
                                             const C_OSCCanProtocol::E_Type & ore_NewComType)
{
   sint32 s32_Retval = C_NO_ERR;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      C_OSCCanProtocol * const pc_Protocol = rc_Node.GetCANProtocol(ore_PreviousComType);
      if (pc_Protocol != NULL)
      {
         pc_Protocol->e_Type = ore_NewComType;
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
   \brief   Set can message

   Do not change any signal using this function.

   \param[in] orc_MessageId     Message identification indices
   \param[in] oru32_SignalIndex Signal index
   \param[in] orc_Message       Message data

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetCanMessageProperties(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                               const bool & orq_NewMessageIsTx, const C_OSCCanMessage & orc_Message)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCCanMessageContainer * const pc_MessageContainer = this->GetCanProtocolMessageContainer(
         orc_MessageId.u32_NodeIndex,
         orc_MessageId.e_ComProtocol,
         orc_MessageId.u32_InterfaceIndex);
      const C_OSCCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);
      const C_OSCNodeDataPoolList * const pc_List = this->GetOSCCanDataPoolList(orc_MessageId.u32_NodeIndex,
                                                                                orc_MessageId.e_ComProtocol,
                                                                                orc_MessageId.u32_InterfaceIndex,
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
                                            orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex,
                                            orc_Message, orq_NewMessageIsTx, c_Signals);
            if ((orc_MessageId.q_MessageIsTx != orq_NewMessageIsTx) && (s32_Retval == C_NO_ERR))
            {
               //Sync ui data
               const C_PuiSdNodeCanMessage * const pc_UiMessage = this->GetUiCanMessage(orc_MessageId);
               const C_OSCCanMessageContainer * const pc_UpdatedMessageContainer = this->GetCanProtocolMessageContainer(
                  orc_MessageId.u32_NodeIndex,
                  orc_MessageId.e_ComProtocol,
                  orc_MessageId.u32_InterfaceIndex);
               const C_PuiSdNodeDataPoolList * const pc_UiDataPoolList = this->m_GetUiCanDataPoolList(
                  orc_MessageId.u32_NodeIndex, orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                  orc_MessageId.q_MessageIsTx);
               if (((pc_UiMessage != NULL) && (pc_UpdatedMessageContainer != NULL)) && (pc_UiDataPoolList != NULL))
               {
                  const std::vector<C_OSCCanMessage> & rc_Messages = pc_UpdatedMessageContainer->GetMessagesConst(
                     orq_NewMessageIsTx);
                  const std::vector<C_PuiSdNodeCanSignal> c_UiSignalsCopy = pc_UiMessage->c_Signals;
                  std::vector<C_PuiSdNodeDataPoolListElement> c_UISignalCommons;
                  const C_OSCCanMessageIdentificationIndices c_NewId(orc_MessageId.u32_NodeIndex,
                                                                     orc_MessageId.e_ComProtocol,
                                                                     orc_MessageId.u32_InterfaceIndex,
                                                                     orq_NewMessageIsTx,
                                                                     rc_Messages.size() - 1UL);

                  u32_SignalStartIndex = pc_UpdatedMessageContainer->GetMessageSignalDataStartIndex(
                     orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex);

                  for (uint32 u32_ItSignal = 0; u32_ItSignal < c_UiSignalsCopy.size(); ++u32_ItSignal)
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
                     s32_Retval = this->m_InsertUiCanMessage(c_NewId, c_UISignalCommons, c_UiSignalsCopy);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set UI part of can message

   Do not change any signals using this function.

   \param[in] orc_MessageId Message identification indices
   \param[in] orc_Message   Message data

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     22.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetUiCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                       const C_PuiSdNodeCanMessage & orc_Message)
{
   sint32 s32_Retval = C_NO_ERR;

   C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                           orc_MessageId.e_ComProtocol);

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

//-----------------------------------------------------------------------------
/*!
   \brief   Set can signal

   \param[in] orc_MessageId       Message identification indices
   \param[in] oru32_SignalIndex   Signal index
   \param[in] orc_OSCSignal       Signal data (osc)
   \param[in] orc_OSCSignalCommon Signal data (osc common)
   \param[in] orc_UISignalCommon  Signal data (ui common)
   \param[in] orc_UISignal        Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
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
      const C_OSCCanProtocol * const pc_Protocol = rc_OSCNode.GetCANProtocolConst(orc_MessageId.e_ComProtocol);
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol);
      C_OSCNodeDataPoolListElementId c_Id;
      s32_Retval = rc_OSCNode.SetSignal(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                                        orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex,
                                        oru32_SignalIndex, orc_OSCSignal, orc_OSCSignalCommon);
      //Signal changed signal to the sync engine
      if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, oru32_SignalIndex, c_Id) == C_NO_ERR)
      {
         Q_EMIT this->SigSyncNodeDataPoolListElementChanged(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                            c_Id.u32_ListIndex, c_Id.u32_ElementIndex,
                                                            orc_OSCSignalCommon.GetType(),
                                                            orc_OSCSignalCommon.GetArray(),
                                                            orc_OSCSignalCommon.GetArraySize());
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
                     orc_MessageId.e_ComProtocol);
                  if (pc_OSCDataPool != NULL)
                  {
                     C_PuiSdNode & rc_UINode = this->mc_UINodes[orc_MessageId.u32_NodeIndex];
                     if (pc_Protocol->u32_DataPoolIndex < rc_UINode.c_UIDataPools.size())
                     {
                        C_PuiSdNodeDataPool & rc_UIDataPool = rc_UINode.c_UIDataPools[pc_Protocol->u32_DataPoolIndex];
                        uint32 u32_ListIndex;
                        if (C_OSCCanProtocol::h_GetComListIndex(*pc_OSCDataPool, orc_MessageId.u32_InterfaceIndex,
                                                                orc_MessageId.q_MessageIsTx,
                                                                u32_ListIndex) == C_NO_ERR)
                        {
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set can signal common part

   \param[in] orc_MessageId       Message identification indices
   \param[in] oru32_SignalIndex   Signal index
   \param[in] orc_OSCSignalCommon Signal data (osc common)
   \param[in] orc_UISignalCommon  Signal data (ui common)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetCanSignalCommon(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                          const uint32 & oru32_SignalIndex,
                                          const C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                          const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon)
{
   sint32 s32_Retval = C_NO_ERR;

   //Position
   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      const C_OSCCanProtocol * const pc_Protocol = rc_OSCNode.GetCANProtocolConst(orc_MessageId.e_ComProtocol);
      C_OSCNodeDataPoolListElementId c_Id;
      s32_Retval = rc_OSCNode.SetSignalCommon(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                                              orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex,
                                              oru32_SignalIndex, orc_OSCSignalCommon);
      //Signal changed signal to the sync engine
      if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, oru32_SignalIndex, c_Id) == C_NO_ERR)
      {
         Q_EMIT this->SigSyncNodeDataPoolListElementChanged(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                            c_Id.u32_ListIndex, c_Id.u32_ElementIndex,
                                                            orc_OSCSignalCommon.GetType(),
                                                            orc_OSCSignalCommon.GetArray(),
                                                            orc_OSCSignalCommon.GetArraySize());
      }

      //Ui part
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
                     orc_MessageId.e_ComProtocol);
                  if (pc_OSCDataPool != NULL)
                  {
                     C_PuiSdNode & rc_UINode = this->mc_UINodes[orc_MessageId.u32_NodeIndex];
                     if (pc_Protocol->u32_DataPoolIndex < rc_UINode.c_UIDataPools.size())
                     {
                        C_PuiSdNodeDataPool & rc_UIDataPool = rc_UINode.c_UIDataPools[pc_Protocol->u32_DataPoolIndex];
                        uint32 u32_ListIndex;
                        if (C_OSCCanProtocol::h_GetComListIndex(*pc_OSCDataPool, orc_MessageId.u32_InterfaceIndex,
                                                                orc_MessageId.q_MessageIsTx,
                                                                u32_ListIndex) == C_NO_ERR)
                        {
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
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set can signal position part

   \param[in] orc_MessageId     Message identification indices
   \param[in] oru32_SignalIndex Signal index
   \param[in] orc_OSCSignal     Signal data (osc)
   \param[in] orc_UISignal      Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SetCanSignalPosition(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                            const uint32 & oru32_SignalIndex, const C_OSCCanSignal & orc_OSCSignal,
                                            const C_PuiSdNodeCanSignal & orc_UISignal)
{
   sint32 s32_Retval = C_NO_ERR;

   //Position
   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol);
      s32_Retval = rc_OSCNode.SetSignalPosition(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
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

//-----------------------------------------------------------------------------
/*!
   \brief   Add can message

   \param[in] oru32_NodeIndex                 Node index
   \param[in] ore_ComType                     Can protocol type
   \param[in] oru32_InterfaceIndex            Node interface index
   \param[in] orq_MessageIsTx                 Flag if message is tx type (else rx type assumed)
   \param[in] orc_Message                     Message data
   \param[in] orc_OSCSignalCommons            Signals data (osc common)
   \param[in] orc_UISignalCommons             Signals data (ui common)
   \param[in] orc_UISignals                   Signals data (ui)
   \param[in] orq_AutomatedPropertiesAdaption Optional parameter to allow or disable automated properties adaption

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::AddCanMessage(const uint32 & oru32_NodeIndex, const C_OSCCanProtocol::E_Type & ore_ComType,
                                     const uint32 & oru32_InterfaceIndex, const bool & orq_MessageIsTx,
                                     const C_OSCCanMessage & orc_Message,
                                     const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons,
                                     const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                     const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals,
                                     const bool & orq_AutomatedPropertiesAdaption)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCCanMessageContainer * const pc_MessageContainer = this->GetCanProtocolMessageContainer(oru32_NodeIndex,
                                                                                                     ore_ComType,
                                                                                                     oru32_InterfaceIndex);

   if (pc_MessageContainer != NULL)
   {
      const std::vector<C_OSCCanMessage> & rc_Messages = pc_MessageContainer->GetMessagesConst(orq_MessageIsTx);
      const uint32 u32_MessageIndex = rc_Messages.size();
      const C_OSCCanMessageIdentificationIndices c_MessageId(oru32_NodeIndex, ore_ComType, oru32_InterfaceIndex,
                                                             orq_MessageIsTx,
                                                             u32_MessageIndex);
      s32_Retval = this->InsertCanMessage(c_MessageId, orc_Message, orc_OSCSignalCommons, orc_UISignalCommons,
                                          orc_UISignals, orq_AutomatedPropertiesAdaption);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Insert can message

   \param[in] orc_MessageId                   Message identification indices
   \param[in] orc_Message                     Message data
   \param[in] orc_OSCSignalCommons            Signals data (osc common)
   \param[in] orc_UISignalCommons             Signals data (ui common)
   \param[in] orc_UISignals                   Signals data (ui)
   \param[in] orq_AutomatedPropertiesAdaption Optional parameter to allow or disable automated properties adaption

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::InsertCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                        const C_OSCCanMessage & orc_Message,
                                        const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons,
                                        const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                        const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals,
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
         c_AdaptedMessage.c_Name = C_PuiSdUtil::h_GetUniqueName(
            this->m_GetExistingMessageNames(orc_MessageId.u32_NodeIndex, orc_MessageId.e_ComProtocol,
                                            orc_MessageId.u32_InterfaceIndex), orc_Message.c_Name);
      }
      s32_Retval = rc_OSCNode.InsertMessage(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                                            orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex,
                                            c_AdaptedMessage,
                                            orc_OSCSignalCommons);
      //Signal each new signal to the sync engine (ascending order is important)
      for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
      {
         C_OSCNodeDataPoolListElementId c_Id;
         if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, u32_ItSig, c_Id) == C_NO_ERR)
         {
            Q_EMIT this->SigSyncNodeDataPoolListElementAdded(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                             c_Id.u32_ListIndex, c_Id.u32_ElementIndex);
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Ui part
         s32_Retval = m_InsertUiCanMessage(orc_MessageId, orc_UISignalCommons, orc_UISignals);
      }
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete can message

   \param[in] orc_MessageId Message identification indices

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::DeleteCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   sint32 s32_Retval = C_NO_ERR;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);
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
               Q_EMIT this->SigSyncNodeDataPoolListElementAboutToBeDeleted(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                                           c_Id.u32_ListIndex, c_Id.u32_ElementIndex);
            }
         }
      }
      //Ui part
      s32_Retval = m_DeleteUiCanMessage(orc_MessageId);
      //After deleting signals delete other parts
      tgl_assert(rc_OSCNode.DeleteMessage(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                                          orc_MessageId.q_MessageIsTx,
                                          orc_MessageId.u32_MessageIndex) == C_NO_ERR);
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add can signal

   \param[in] orc_MessageId       Message identification indices
   \param[in] orc_Signal          Signal data (osc)
   \param[in] orc_OSCSignalCommon Signal data (osc common)
   \param[in] orc_UISignalCommon  Signal data (ui common)
   \param[in] orc_UISignal        Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::AddCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
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

//-----------------------------------------------------------------------------
/*!
   \brief   Insert can signal

   \param[in] orc_MessageId       Message identification indices
   \param[in] oru32_SignalIndex   Signal index
   \param[in] orc_Signal          Signal data (osc)
   \param[in] orc_OSCSignalCommon Signal data (osc common)
   \param[in] orc_UISignalCommon  Signal data (ui common)
   \param[in] orc_UISignal        Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::InsertCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                       const uint32 & oru32_SignalIndex, const C_OSCCanSignal & orc_Signal,
                                       const C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                       const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                       const C_PuiSdNodeCanSignal & orc_UISignal)
{
   sint32 s32_Retval = C_NO_ERR;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      const C_OSCCanProtocol * const pc_Protocol = rc_OSCNode.GetCANProtocolConst(orc_MessageId.e_ComProtocol);
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol);
      C_OSCNodeDataPoolListElement c_OSCAdaptedSignalCommon = orc_OSCSignalCommon;
      C_OSCNodeDataPoolListElementId c_Id;

      //Get unique name
      c_OSCAdaptedSignalCommon.c_Name = C_PuiSdUtil::h_GetUniqueName(
         this->m_GetExistingSignalNames(orc_MessageId), orc_OSCSignalCommon.c_Name);
      rc_OSCNode.InsertSignal(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
                              orc_MessageId.q_MessageIsTx, orc_MessageId.u32_MessageIndex, oru32_SignalIndex,
                              orc_Signal,
                              c_OSCAdaptedSignalCommon);
      //Signal new signal to the sync engine
      if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, oru32_SignalIndex, c_Id) == C_NO_ERR)
      {
         Q_EMIT this->SigSyncNodeDataPoolListElementAdded(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                          c_Id.u32_ListIndex, c_Id.u32_ElementIndex);
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
                  orc_MessageId.e_ComProtocol);
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
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete can signal

   \param[in] orc_MessageId     Message identification indices
   \param[in] oru32_SignalIndex Signal index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::DeleteCanSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                       const uint32 & oru32_SignalIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   tgl_assert(this->mc_CoreDefinition.c_Nodes.size() == this->mc_UINodes.size());
   if (orc_MessageId.u32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[orc_MessageId.u32_NodeIndex];
      const C_OSCCanProtocol * const pc_Protocol = rc_OSCNode.GetCANProtocolConst(orc_MessageId.e_ComProtocol);
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol);
      C_OSCNodeDataPoolListElementId c_Id;

      //Signal deleted signal to the sync engine
      if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, oru32_SignalIndex, c_Id) == C_NO_ERR)
      {
         Q_EMIT this->SigSyncNodeDataPoolListElementAboutToBeDeleted(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                                     c_Id.u32_ListIndex, c_Id.u32_ElementIndex);
      }

      rc_OSCNode.DeleteSignal(orc_MessageId.e_ComProtocol, orc_MessageId.u32_InterfaceIndex,
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
                  orc_MessageId.e_ComProtocol);
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
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert element index to signal index

   Take node, datapool, list, element index and return message id and signal index.

   \param[in]        orc_NodeDatapoolListElementId    node, datapool, list, element index packed together
   \param[in,out]    orc_MessageId                    message id
   \param[in,out]    oru32_SignalIndex                signal index

   \created     20.06.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::ConvertElementIndexToSignalIndex(
   const C_OSCNodeDataPoolListElementId & orc_NodeDatapoolListElementId,
   C_OSCCanMessageIdentificationIndices & orc_MessageId, uint32 & oru32_SignalIndex) const
{
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(orc_NodeDatapoolListElementId.u32_NodeIndex);
   const C_OSCNodeDataPool * const pc_Datapool = this->GetOSCDataPool(orc_NodeDatapoolListElementId.u32_NodeIndex,
                                                                      orc_NodeDatapoolListElementId.u32_DataPoolIndex);

   C_OSCCanProtocol::E_Type e_ComProtocol = C_OSCCanProtocol::eLAYER2;
   uint32 u32_InterfaceIndex = 0;
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
         orc_NodeDatapoolListElementId.u32_NodeIndex, e_ComProtocol, u32_InterfaceIndex);
      std::vector<C_OSCCanMessage> c_Messages; // either tx messages or rx messages
      bool q_IndexFound = false;

      if (q_MessageIsTx == true)
      {
         c_Messages = pc_MessageContainer->c_TxMessages;
      }
      else
      {
         c_Messages = pc_MessageContainer->c_RxMessages;
      }
      for (uint32 u32_PosMessage = 0; (u32_PosMessage < c_Messages.size()) && (q_IndexFound == false); u32_PosMessage++)
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
                                                           q_MessageIsTx, u32_MessageIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get namespace for ID

   \param[in] orc_Id ID

   \return
   Namespace

   \created     17.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_PuiSdHandler::GetNamespace(const C_OSCNodeDataPoolListElementId & orc_Id) const
{
   QString c_Retval;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(orc_Id.u32_NodeIndex);
   const C_OSCNodeDataPool * const pc_DataPool = this->GetOSCDataPool(orc_Id.u32_NodeIndex, orc_Id.u32_DataPoolIndex);
   const C_OSCNodeDataPoolList * const pc_List = this->GetOSCDataPoolList(orc_Id.u32_NodeIndex,
                                                                          orc_Id.u32_DataPoolIndex,
                                                                          orc_Id.u32_ListIndex);
   const C_OSCNodeDataPoolListElement * const pc_Element = this->GetOSCDataPoolListElement(orc_Id.u32_NodeIndex,
                                                                                           orc_Id.u32_DataPoolIndex,
                                                                                           orc_Id.u32_ListIndex,
                                                                                           orc_Id.u32_ElementIndex);

   if ((((pc_Node != NULL) && (pc_DataPool != NULL)) && (pc_List != NULL)) && (pc_Element != NULL))
   {
      QString c_ElementName;
      C_OSCCanMessageIdentificationIndices c_MessageID;
      uint32 u32_SignalIndex;
      //Special handling for signals: append message name as the signal name might not be unique
      if ((pc_DataPool->e_Type == C_OSCNodeDataPool::eCOM) &&
          (C_PuiSdUtil::h_ConvertIndex(orc_Id, c_MessageID, u32_SignalIndex) == C_NO_ERR))
      {
         const C_OSCCanMessage * const pc_Message = this->GetCanMessage(c_MessageID);
         if (pc_Message != NULL)
         {
            c_ElementName = QString("%1_%2").arg(pc_Message->c_Name.c_str()).arg(pc_Element->c_Name.c_str());
         }
         else
         {
            c_ElementName = pc_Element->c_Name.c_str();
         }
      }
      else
      {
         c_ElementName = pc_Element->c_Name.c_str();
      }
      c_Retval =
         QString("%1::%2::%3::%4").arg(pc_Node->c_Properties.c_Name.c_str()).arg(pc_DataPool->c_Name.c_str()).arg(
            pc_List->c_Name.c_str()).arg(c_ElementName);
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get namespace for signal ID

   \param[in] orc_Id Signal ID

   \return
   Namespace

   \created     19.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_PuiSdHandler::GetSignalNamespace(const C_OSCNodeDataPoolListElementId & orc_Id) const
{
   QString c_Retval;
   C_OSCCanMessageIdentificationIndices c_MessageID;
   uint32 u32_SignalIndex;

   if (C_PuiSdUtil::h_ConvertIndex(orc_Id, c_MessageID, u32_SignalIndex) == C_NO_ERR)
   {
      const C_OSCNode * const pc_Node = this->GetOSCNodeConst(c_MessageID.u32_NodeIndex);
      if ((pc_Node != NULL) && (c_MessageID.u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size()))
      {
         const C_OSCNodeComInterfaceSettings & rc_Interface =
            pc_Node->c_Properties.c_ComInterfaces[c_MessageID.u32_InterfaceIndex];
         if (rc_Interface.q_IsBusConnected == true)
         {
            const C_OSCSystemBus * const pc_Bus = this->GetOSCBus(rc_Interface.u32_BusIndex);
            const C_OSCCanMessage * const pc_Message = this->GetCanMessage(c_MessageID);
            const C_OSCNodeDataPoolListElement * const pc_Element = this->GetOSCDataPoolListElement(
               orc_Id.u32_NodeIndex,
               orc_Id.u32_DataPoolIndex,
               orc_Id.u32_ListIndex,
               orc_Id.u32_ElementIndex);

            if (((pc_Bus != NULL) && (pc_Message != NULL)) && (pc_Element != NULL))
            {
               c_Retval = QString("%1::%2::%3").arg(pc_Bus->c_Name.c_str()).arg(pc_Message->c_Name.c_str()).arg(
                  pc_Element->c_Name.c_str());
            }
         }
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt given string to fulfill c variable name requirements

   \param[in] orc_Input               String to process
   \param[in] oq_ReplaceAllCharacters Flag to replace all not allowed characters by '_'

   \return
   String fulfilling c variable name requirements

   \created     05.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_PuiSdHandler::h_AutomaticCStringAdaptation(const QString & orc_Input, const bool oq_ReplaceAllCharacters)
{
   QString c_Retval;

   for (sintn sn_It = 0; sn_It < orc_Input.length(); ++sn_It)
   {
      if (((oq_ReplaceAllCharacters == false) && ((orc_Input.at(sn_It) == '-') || (orc_Input.at(sn_It) == ' '))) ||
          ((oq_ReplaceAllCharacters == true) &&
           (C_OSCUtils::h_CheckValidCName(orc_Input.at(sn_It).toLatin1()) == false)))
      {
         c_Retval += "_";
      }
      else
      {
         c_Retval += orc_Input.at(sn_It);
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get OSC Buses size

   \created     14.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
stw_types::uint32 C_PuiSdHandler::GetOSCBusesSize(void) const
{
   return this->mc_CoreDefinition.c_Buses.size();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get OSC Nodes size

   \created     14.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
stw_types::uint32 C_PuiSdHandler::GetOSCNodesSize(void) const
{
   return this->mc_CoreDefinition.c_Nodes.size();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clear system definition

   \created     15.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::Clear(void)
{
   this->mc_UIBuses.clear();
   this->mc_UINodes.clear();
   this->mc_CoreDefinition.c_Buses.clear();
   this->mc_CoreDefinition.c_Nodes.clear();
   this->c_Elements.Clear();
   this->c_BusTextElements.clear();

   //Reset hash
   this->mu32_CalculatedHashSystemDefinition = this->m_CalcHashSystemDefinition();
   //signal "node change"
   Q_EMIT this->SigNodesChanged();
   //signal "bus change"
   Q_EMIT this->SigBussesChanged();
   Q_EMIT this->SigSyncClear();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get snapshot of all current items

   \return
   Snapshot containing all items

   \created     23.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdTopologyDataSnapshot C_PuiSdHandler::GetSnapshot(void) const
{
   C_SdTopologyDataSnapshot c_Retval;

   //Linked objects
   c_Retval.c_OSCBuses = this->mc_CoreDefinition.c_Buses;
   c_Retval.c_UIBuses = this->mc_UIBuses;
   c_Retval.c_OSCNodes = this->mc_CoreDefinition.c_Nodes;
   c_Retval.c_UINodes = this->mc_UINodes;

   //Independent objects
   this->c_Elements.ReplaceSnapshotElements(c_Retval);
   c_Retval.c_BusTextElements = this->c_BusTextElements;

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load system definition

   Load system definition and store in information in our instance data.

   \param[in]     orc_Path   Path to system definition file

   \return
   C_NO_ERR    data read and placed into instance data
   C_RD_WR     problems accessing file system (e.g. no read access to file)
   C_RANGE     specified file does not exist (when loading)
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    content of file is invalid or incomplete
   C_OVERFLOW  node in system definition references a device not part of the device definitions

   \created     14.09.2016  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::LoadFromFile(const C_SCLString & orc_Path)
{
   sint32 s32_Return;

   s32_Return =
      C_OSCSystemDefinitionFiler::h_LoadSystemDefinitionFile(mc_CoreDefinition, orc_Path,
                                                             (C_Uti::h_GetExePath() +
                                                              "/../devices/devices.ini").toStdString().c_str());
   if (s32_Return == C_NO_ERR)
   {
      //Core data read successfully; now go for UI data.
      //As reading the core data was OK we can assume the file exists and its structure is OK
      C_OSCXMLParser c_XMLParser;
      (void)c_XMLParser.LoadFromFile(orc_Path);
      tgl_assert(c_XMLParser.SelectRoot() == "opensyde-system-definition");
      tgl_assert(c_XMLParser.SelectNodeChild("nodes") == "nodes");

      s32_Return = C_PuiSdHandlerFiler::h_LoadNodes(this->mc_UINodes, c_XMLParser);

      if (s32_Return == C_NO_ERR)
      {
         //Return
         tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-definition");
         //Bus
         tgl_assert(c_XMLParser.SelectNodeChild("buses") == "buses");
         s32_Return = C_PuiSdHandlerFiler::h_LoadBuses(this->mc_UIBuses, c_XMLParser);
      }
      //GUI items
      this->c_Elements.Clear();
      this->c_BusTextElements.clear();
      if (s32_Return == C_NO_ERR)
      {
         //Return
         tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-definition");
         if (c_XMLParser.SelectNodeChild("gui-only") == "gui-only")
         {
            //bus text elements
            if (c_XMLParser.SelectNodeChild("bus-text-elements") == "bus-text-elements")
            {
               s32_Return = C_PuiSdHandlerFiler::h_LoadBusTextElements(this->c_BusTextElements, c_XMLParser);
               tgl_assert(c_XMLParser.SelectNodeParent() == "gui-only");
            }
            else
            {
               s32_Return = C_CONFIG;
            }
            if (s32_Return == C_NO_ERR)
            {
               //Base elements
               s32_Return = C_PuiBsElementsFiler::h_LoadBaseElements(this->c_Elements, c_XMLParser);
            }
         }
         else
         {
            s32_Return = C_CONFIG;
         }
      }
   }
   else
   {
      osc_write_log_error("Loading System Definition", "Could not load System Definition. Error code: " +
                          C_SCLString::IntToStr(s32_Return));
   }

   if (s32_Return == C_NO_ERR)
   {
      //calculate the hash value and save it for comparing
      this->mu32_CalculatedHashSystemDefinition = this->m_CalcHashSystemDefinition();

      //Fix inconsistency problems with existing projects (notify user about changes, so do this after CRC update)
      m_FixCommInconsistencyErrors();
      m_FixAddressIssues();
      m_FixNameIssues();
   }

   //AFTER automated adaptions!
   //signal "node change"
   Q_EMIT this->SigNodesChanged();
   //signal "bus change"
   Q_EMIT this->SigBussesChanged();

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save system definition

   Save UI data part of system definition to XML file.

   \param[in]     orc_Path   Path to system definition file

   \return
   C_NO_ERR   data saved
   C_RD_WR    problems accessing file system (e.g. could not erase pre-existing file before saving)
   C_COM      Bus sorting failed

   \created     14.09.2016  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::SaveToFile(const C_SCLString & orc_Path)
{
   sint32 s32_Return = C_NO_ERR;

   //Sort first
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCSystemDefinition c_SortedOSCDefinition = this->mc_CoreDefinition;
      std::vector<C_PuiSdNode> c_SortedUiNodes = this->mc_UINodes;
      for (uint32 u32_ItNode = 0; (u32_ItNode < c_SortedUiNodes.size()) && (s32_Return == C_NO_ERR); ++u32_ItNode)
      {
         s32_Return = mh_SortMessagesByName(c_SortedOSCDefinition.c_Nodes[u32_ItNode], c_SortedUiNodes[u32_ItNode]);
      }
      //Save sorted structure
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = C_OSCSystemDefinitionFiler::h_SaveSystemDefinitionFile(c_SortedOSCDefinition, orc_Path);
         if (s32_Return == C_NO_ERR)
         {
            //Core data written successfully; now go for UI data.
            //As writing the core data was OK we can assume the file exists and its structure is OK
            C_OSCXMLParser c_XMLParser;
            (void)c_XMLParser.LoadFromFile(orc_Path);
            tgl_assert(c_XMLParser.SelectRoot() == "opensyde-system-definition");
            tgl_assert(c_XMLParser.SelectNodeChild("nodes") == "nodes");

            C_PuiSdHandlerFiler::h_SaveNodes(c_SortedUiNodes, c_XMLParser);
            tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-definition"); //back up

            //Bus
            tgl_assert(c_XMLParser.SelectNodeChild("buses") == "buses");
            C_PuiSdHandlerFiler::h_SaveBuses(this->mc_UIBuses, c_XMLParser);
            tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-definition"); //back up

            //GUI items
            c_XMLParser.CreateAndSelectNodeChild("gui-only");

            //Bus text elements
            c_XMLParser.CreateAndSelectNodeChild("bus-text-elements");
            C_PuiSdHandlerFiler::h_SaveBusTextElements(this->c_BusTextElements, c_XMLParser);
            tgl_assert(c_XMLParser.SelectNodeParent() == "gui-only"); //back up

            //Base elements
            C_PuiBsElementsFiler::h_SaveBaseElements(this->c_Elements, c_XMLParser);

            //calculate the hash value and save it for comparing
            this->mu32_CalculatedHashSystemDefinition = this->m_CalcHashSystemDefinition();

            (void)c_XMLParser.SaveToFile(orc_Path);
         }
      }
      else
      {
         s32_Return = C_COM;
      }
   }
   else
   {
      s32_Return = C_COM;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Compares the last saved hash value against the actual hash

   \return
   false    Hash has not changed
   true     Hash has changed

   \created     10.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_PuiSdHandler::HasHashChanged(void) const
{
   const uint32 u32_NewHash = this->m_CalcHashSystemDefinition();
   bool q_Changed = true;

   if (u32_NewHash == this->mu32_CalculatedHashSystemDefinition)
   {
      q_Changed = false;
   }

   return q_Changed;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get reference to core system definition

   \return
   Core system definition

   \created     12.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCSystemDefinition & C_PuiSdHandler::GetOSCSystemDefinitionConst(void) const
{
   return this->mc_CoreDefinition;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get singleton (Create if necessary)

   \return
   Pointer to singleton

   \created     06.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSdHandler * C_PuiSdHandler::h_GetInstance(void)
{
   if (C_PuiSdHandler::mhpc_Singleton == NULL)
   {
      C_PuiSdHandler::mhpc_Singleton = new C_PuiSdHandler();
   }
   return C_PuiSdHandler::mhpc_Singleton;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clean up singleton

   \created     06.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::h_Destroy(void)
{
   if (C_PuiSdHandler::mhpc_Singleton != NULL)
   {
      delete (C_PuiSdHandler::mhpc_Singleton);
      C_PuiSdHandler::mhpc_Singleton = NULL;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     01.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSdHandler::C_PuiSdHandler(QObject * const opc_Parent) :
   QObject(opc_Parent),
   mu32_CalculatedHashSystemDefinition(0)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     01.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSdHandler::~C_PuiSdHandler(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get pointers to all currently registered node names

   \return
   Vector of pointers to all currently registered node names

   \created     28.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<const C_SCLString *> C_PuiSdHandler::m_GetExistingNodeNames(void) const
{
   std::vector<const C_SCLString *> c_Retval;
   c_Retval.reserve(this->mc_CoreDefinition.c_Nodes.size());
   for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      c_Retval.push_back(&this->mc_CoreDefinition.c_Nodes[u32_ItNode].c_Properties.c_Name);
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get pointers to all currently registered node application names

   \return
   Vector of pointers to all currently registered node application names

   \created     12.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<const C_SCLString *> C_PuiSdHandler::m_GetExistingNodeApplicationNames(const uint32 & oru32_NodeIndex) const
{
   std::vector<const C_SCLString *> c_Retval;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(oru32_NodeIndex);
   if (pc_Node != NULL)
   {
      c_Retval.reserve(pc_Node->c_Applications.size());
      for (uint32 u32_ItApplication = 0; u32_ItApplication < pc_Node->c_Applications.size(); ++u32_ItApplication)
      {
         c_Retval.push_back(&pc_Node->c_Applications[u32_ItApplication].c_Name);
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get pointers to all currently registered data pool names contained in the specified node

   \param[in] oru32_NodeIndex Node index

   \return
   Vector of pointers to all currently registered data pool names contained in the specified node

   \created     28.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<const C_SCLString *> C_PuiSdHandler::m_GetExistingNodeDataPoolNames(const uint32 & oru32_NodeIndex) const
{
   std::vector<const C_SCLString *> c_Retval;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(oru32_NodeIndex);
   if (pc_Node != NULL)
   {
      c_Retval.reserve(pc_Node->c_DataPools.size());
      for (uint32 u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
      {
         c_Retval.push_back(&pc_Node->c_DataPools[u32_ItDataPool].c_Name);
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get pointers to all currently registered list names contained in the specified node data pool

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Node data pool index

   \return
   Vector of pointers to all currently registered list names contained in the specified node data pool

   \created     28.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<const C_SCLString *> C_PuiSdHandler::m_GetExistingNodeDataPoolListNames(const uint32 & oru32_NodeIndex,
                                                                                    const uint32 & oru32_DataPoolIndex)
const
{
   std::vector<const C_SCLString *> c_Retval;
   const C_OSCNodeDataPool * const pc_NodeDataPool = this->GetOSCDataPool(oru32_NodeIndex, oru32_DataPoolIndex);
   if (pc_NodeDataPool != NULL)
   {
      c_Retval.reserve(pc_NodeDataPool->c_Lists.size());
      for (uint32 u32_ItDataPoolList = 0; u32_ItDataPoolList < pc_NodeDataPool->c_Lists.size(); ++u32_ItDataPoolList)
      {
         c_Retval.push_back(&pc_NodeDataPool->c_Lists[u32_ItDataPoolList].c_Name);
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get pointers to all currently registered data set names contained in the specified node data pool list

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Node data pool index
   \param[in] oru32_DataPoolListIndex Node data pool list index

   \return
   Vector of pointers to all currently registered data set names contained in the specified node data pool list

   \created     16.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<const C_SCLString *> C_PuiSdHandler::m_GetExistingNodeDataPoolListDataSetNames(
   const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex, const uint32 & oru32_DataPoolListIndex) const
{
   std::vector<const C_SCLString *> c_Retval;
   const C_OSCNodeDataPoolList * const pc_NodeDataPoolList = this->GetOSCDataPoolList(oru32_NodeIndex,
                                                                                      oru32_DataPoolIndex,
                                                                                      oru32_DataPoolListIndex);
   if (pc_NodeDataPoolList != NULL)
   {
      c_Retval.reserve(pc_NodeDataPoolList->c_DataSets.size());
      for (uint32 u32_ItDataPoolListElement = 0; u32_ItDataPoolListElement < pc_NodeDataPoolList->c_DataSets.size();
           ++u32_ItDataPoolListElement)
      {
         c_Retval.push_back(&pc_NodeDataPoolList->c_DataSets[u32_ItDataPoolListElement].c_Name);
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get pointers to all currently registered variable names contained in the specified node data pool list

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Node data pool index
   \param[in] oru32_DataPoolListIndex Node data pool list index

   \return
   Vector of pointers to all currently registered variable names contained in the specified node data pool list

   \created     28.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<const C_SCLString *> C_PuiSdHandler::m_GetExistingNodeDataPoolListVariableNames(
   const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex, const uint32 & oru32_DataPoolListIndex) const
{
   std::vector<const C_SCLString *> c_Retval;
   const C_OSCNodeDataPoolList * const pc_NodeDataPoolList = this->GetOSCDataPoolList(oru32_NodeIndex,
                                                                                      oru32_DataPoolIndex,
                                                                                      oru32_DataPoolListIndex);
   if (pc_NodeDataPoolList != NULL)
   {
      c_Retval.reserve(pc_NodeDataPoolList->c_Elements.size());
      for (uint32 u32_ItDataPoolListElement = 0; u32_ItDataPoolListElement < pc_NodeDataPoolList->c_Elements.size();
           ++u32_ItDataPoolListElement)
      {
         c_Retval.push_back(&pc_NodeDataPoolList->c_Elements[u32_ItDataPoolListElement].c_Name);
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get pointers to all currently registered message names contained in the specified node interface  & protocol

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Node data pool index
   \param[in] oru32_DataPoolListIndex Node data pool list index

   \return
   Vector of pointers to all currently registered message names contained in the specified node interface  & protocol

   \created     28.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<const C_SCLString *> C_PuiSdHandler::m_GetExistingMessageNames(const uint32 & oru32_NodeIndex,
                                                                           const C_OSCCanProtocol::E_Type & ore_ComType,
                                                                           const uint32 & oru32_InterfaceIndex) const
{
   std::vector<const C_SCLString *> c_Retval;
   const C_OSCCanMessageContainer * const pc_Container = this->GetCanProtocolMessageContainer(oru32_NodeIndex,
                                                                                              ore_ComType,
                                                                                              oru32_InterfaceIndex);
   if (pc_Container != NULL)
   {
      const std::vector<C_OSCCanMessage> & rc_TxMessages = pc_Container->GetMessagesConst(true);
      const std::vector<C_OSCCanMessage> & rc_RxMessages = pc_Container->GetMessagesConst(false);
      c_Retval.reserve(rc_TxMessages.size() + rc_RxMessages.size());
      //Tx
      for (uint32 u32_ItMessage = 0; u32_ItMessage < rc_TxMessages.size(); ++u32_ItMessage)
      {
         const C_OSCCanMessage & rc_Message = rc_TxMessages[u32_ItMessage];
         c_Retval.push_back(&rc_Message.c_Name);
      }
      //Rx
      for (uint32 u32_ItMessage = 0; u32_ItMessage < rc_RxMessages.size(); ++u32_ItMessage)
      {
         const C_OSCCanMessage & rc_Message = rc_RxMessages[u32_ItMessage];
         c_Retval.push_back(&rc_Message.c_Name);
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get pointers to all currently registered signal names contained in the specified message

   Extract names of data pool.

   \param[in]  orc_MessageId   Message identification indices

   \return
   Vector of pointers to all currently registered signal names contained in the specified message

   \created     28.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<const C_SCLString *> C_PuiSdHandler::m_GetExistingSignalNames(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   std::vector<const C_SCLString *> c_Retval;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(orc_MessageId.u32_NodeIndex);
   if (pc_Node != NULL)
   {
      const C_OSCNodeDataPool * const pc_ComDataPool = pc_Node->GetComDataPoolConst(orc_MessageId.e_ComProtocol);
      if (pc_ComDataPool != NULL)
      {
         const C_OSCNodeDataPoolList * const pc_List = C_OSCCanProtocol::h_GetComListConst(*pc_ComDataPool,
                                                                                           orc_MessageId.u32_InterfaceIndex,
                                                                                           orc_MessageId.q_MessageIsTx);
         const C_OSCCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);
         if ((pc_Message != NULL) && (pc_List != NULL))
         {
            c_Retval.reserve(pc_Message->c_Signals.size());
            for (uint32 u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
            {
               const C_OSCCanSignal & rc_Signal = pc_Message->c_Signals[u32_ItSignal];
               if (rc_Signal.u32_ComDataElementIndex < pc_List->c_Elements.size())
               {
                  const C_OSCNodeDataPoolListElement & rc_DataElement =
                     pc_List->c_Elements[rc_Signal.u32_ComDataElementIndex];
                  c_Retval.push_back(&rc_DataElement.c_Name);
               }
            }
         }
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value of the system definition

   Start value is 0xFFFFFFFF

   \return
   Calculated hash value

   \created     21.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
uint32 C_PuiSdHandler::m_CalcHashSystemDefinition(void) const
{
   // init value of CRC
   uint32 u32_Hash = 0xFFFFFFFFU;
   uint32 u32_Counter;

   // calculate the hash for the core elements
   this->mc_CoreDefinition.CalcHash(u32_Hash);

   // calculate the hash for the ui elements
   this->c_Elements.CalcHash(u32_Hash);

   for (u32_Counter = 0U; u32_Counter < this->c_BusTextElements.size(); ++u32_Counter)
   {
      this->c_BusTextElements[u32_Counter].CalcHash(u32_Hash);
   }

   for (u32_Counter = 0U; u32_Counter < this->mc_UINodes.size(); ++u32_Counter)
   {
      this->mc_UINodes[u32_Counter].CalcHash(u32_Hash);
   }

   for (u32_Counter = 0U; u32_Counter < this->mc_UIBuses.size(); ++u32_Counter)
   {
      this->mc_UIBuses[u32_Counter].CalcHash(u32_Hash);
   }

   return u32_Hash;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize com data pool

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Data pool index
   \param[in] orq_AllowDataAdaptation Optional parameter to disable automatic data adaptation
   \param[in] ore_ComProtocolType     Optional parameter for com protocol type

   \created     23.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::m_SetUpComDataPool(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                        const bool & orq_AllowDataAdaptation,
                                        const C_OSCCanProtocol::E_Type & ore_ComProtocolType)
{
   //Check size & consistency
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_UINodes.size())
   {
      C_PuiSdNode & rc_UINode = this->mc_UINodes[oru32_NodeIndex];
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      //Check size & consistency
      tgl_assert(rc_UINode.c_UIDataPools.size() == rc_OSCNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OSCNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UIDataPool = rc_UINode.c_UIDataPools[oru32_DataPoolIndex];
         C_OSCNodeDataPool & rc_OSCDataPool = rc_OSCNode.c_DataPools[oru32_DataPoolIndex];
         //Check if com data pool
         if (rc_OSCDataPool.e_Type == C_OSCNodeDataPool::eCOM)
         {
            C_OSCNodeDataPoolDataSet c_DataSetInit;
            c_DataSetInit.c_Name = "Init";
            //Clean
            rc_OSCDataPool.c_Lists.clear();
            rc_UIDataPool.c_DataPoolLists.clear();
            //Reserve
            rc_OSCDataPool.c_Lists.reserve(rc_OSCNode.c_Properties.c_ComInterfaces.size() * 2);
            rc_UIDataPool.c_DataPoolLists.reserve(rc_OSCNode.c_Properties.c_ComInterfaces.size() * 2);
            //Check com interfaces
            for (uint32 u32_ItComInterface = 0; u32_ItComInterface < rc_OSCNode.c_Properties.c_ComInterfaces.size();
                 ++u32_ItComInterface)
            {
               const C_OSCNodeComInterfaceSettings & rc_ComInterface =
                  rc_OSCNode.c_Properties.c_ComInterfaces[u32_ItComInterface];
               //Do not create any list for Ethernet
               if (rc_ComInterface.e_InterfaceType == C_OSCSystemBus::eCAN)
               {
                  const QString c_InterfaceName = C_PuiSdUtil::h_GetInterfaceName(rc_ComInterface.e_InterfaceType,
                                                                                  rc_ComInterface.u8_InterfaceNumber);
                  C_OSCNodeDataPoolList c_OSCList;
                  C_PuiSdNodeDataPoolList c_UIList;
                  //Clear
                  c_OSCList.c_Elements.clear();
                  c_UIList.c_DataPoolListElements.clear();

                  //Add init data set
                  c_OSCList.c_DataSets.clear();
                  c_OSCList.c_DataSets.push_back(c_DataSetInit);

                  //Add tx
                  c_OSCList.c_Name = (c_InterfaceName + "_TX").toStdString().c_str();

                  rc_OSCDataPool.c_Lists.push_back(c_OSCList);
                  rc_UIDataPool.c_DataPoolLists.push_back(c_UIList);

                  //Add rx
                  c_OSCList.c_Name = (c_InterfaceName + "_RX").toStdString().c_str();

                  rc_OSCDataPool.c_Lists.push_back(c_OSCList);
                  rc_UIDataPool.c_DataPoolLists.push_back(c_UIList);
               }
            }
            //Set up com protocol
            {
               C_PuiSdNodeCanProtocol c_UiProtocol;
               C_PuiSdNodeCanMessageContainer c_UiMessageContainer;
               C_OSCCanProtocol c_Protocol;
               C_OSCCanMessageContainer c_New;
               c_New.q_IsComProtocolUsedByInterface = false;
               c_Protocol.u32_DataPoolIndex = oru32_DataPoolIndex;
               c_Protocol.e_Type = ore_ComProtocolType;
               c_Protocol.c_ComMessages.reserve(rc_OSCNode.c_Properties.c_ComInterfaces.size());
               for (uint32 u32_ItMessageContainer =
                       0; u32_ItMessageContainer < rc_OSCNode.c_Properties.c_ComInterfaces.size();
                    ++u32_ItMessageContainer)
               {
                  const C_OSCNodeComInterfaceSettings & rc_Interface =
                     rc_OSCNode.c_Properties.c_ComInterfaces[u32_ItMessageContainer];
                  //Do not create any list for Ethernet
                  if (rc_Interface.e_InterfaceType == C_OSCSystemBus::eCAN)
                  {
                     c_Protocol.c_ComMessages.push_back(c_New);
                     c_UiProtocol.c_ComMessages.push_back(c_UiMessageContainer);
                  }
               }
               rc_OSCNode.c_ComProtocols.push_back(c_Protocol);
               rc_UINode.c_UICanProtocols.push_back(c_UiProtocol);
            }
         }
         else
         {
            //Other data pools need small adjustment
            if ((orq_AllowDataAdaptation == true) && (rc_OSCDataPool.c_Lists.size() > 0))
            {
               C_OSCNodeDataPoolList & rc_List = rc_OSCDataPool.c_Lists[0];
               if (rc_List.c_Elements.size() > 0)
               {
                  C_PuiSdHandler::h_InitDataElement(rc_OSCDataPool.e_Type, rc_OSCDataPool.q_IsSafety,
                                                    rc_List.c_Elements[0]);
               }
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clean up com data pool

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Data pool index

   \created     23.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::m_CleanUpComDataPool(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex)
{
   //Clean up com protocol
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      C_PuiSdNode & rc_UiNode = this->mc_UINodes[oru32_NodeIndex];
      tgl_assert(rc_Node.c_ComProtocols.size() == rc_UiNode.c_UICanProtocols.size());
      for (uint32 u32_ItProtocol = rc_Node.c_ComProtocols.size(); u32_ItProtocol > 0UL; --u32_ItProtocol)
      {
         const uint32 u32_Index = u32_ItProtocol - 1UL;
         const C_OSCCanProtocol & rc_Protcol = rc_Node.c_ComProtocols[u32_Index];
         if (rc_Protcol.u32_DataPoolIndex == oru32_DataPoolIndex)
         {
            rc_Node.c_ComProtocols.erase(rc_Node.c_ComProtocols.begin() + u32_Index);
            rc_UiNode.c_UICanProtocols.erase(rc_UiNode.c_UICanProtocols.begin() + u32_Index);
            break;
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get com protocol (ui)

   \param[in] oru32_NodeIndex Node index
   \param[in] ore_ComType     Can protocol type

   \return
   NULL No matching data found
   Else Pointer to data

   \created     13.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSdNodeCanProtocol * C_PuiSdHandler::m_GetUiCanProtocol(const uint32 & oru32_NodeIndex,
                                                            const C_OSCCanProtocol::E_Type & ore_ComType)
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
         if (rc_CanProtocol.e_Type == ore_ComType)
         {
            pc_Retval = &rc_UINode.c_UICanProtocols[u32_ItCanProtocol];
         }
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get signal list (ui)

   \param[in] oru32_NodeIndex      Node index
   \param[in] ore_ComType          Can protocol type
   \param[in] oru32_InterfaceIndex Node interface index
   \param[in] orq_MessageIsTx      Flag if message is tx type (else rx type assumed)

   \return
   NULL No matching data found
   Else Pointer to data

   \created     11.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiSdNodeDataPoolList * C_PuiSdHandler::m_GetUiCanDataPoolList(const uint32 & oru32_NodeIndex,
                                                                       const C_OSCCanProtocol::E_Type & ore_ComType,
                                                                       const uint32 & oru32_InterfaceIndex,
                                                                       const bool & orq_MessageIsTx) const
{
   const C_PuiSdNodeDataPoolList * pc_Retval = NULL;
   const C_OSCNodeDataPool * pc_OSCDataPool = this->GetOSCCanDataPool(oru32_NodeIndex, ore_ComType);

   if (pc_OSCDataPool != NULL)
   {
      uint32 u32_Index;

      if (C_OSCCanProtocol::h_GetComListIndex(*pc_OSCDataPool, oru32_InterfaceIndex, orq_MessageIsTx,
                                              u32_Index) == C_NO_ERR)
      {
         const C_PuiSdNodeDataPool * pc_UIDataPool = this->GetUiCanDataPool(oru32_NodeIndex, ore_ComType);

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

//-----------------------------------------------------------------------------
/*!
   \brief   Insert ui part for can message

   \param[in] orc_MessageId                   Message identification indices
   \param[in] orc_UISignalCommons             Signals data (ui common)
   \param[in] orc_UISignals                   Signals data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     12.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::m_InsertUiCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                            const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                            const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(orc_MessageId.u32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OSCCanProtocol * const pc_Protocol = pc_Node->GetCANProtocolConst(orc_MessageId.e_ComProtocol);
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol);

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
                  orc_MessageId.e_ComProtocol);
               if (pc_OSCDataPool != NULL)
               {
                  C_PuiSdNodeDataPool & rc_UIDataPool = rc_UINode.c_UIDataPools[pc_Protocol->u32_DataPoolIndex];
                  uint32 u32_ListIndex;
                  if (C_OSCCanProtocol::h_GetComListIndex(*pc_OSCDataPool, orc_MessageId.u32_InterfaceIndex,
                                                          orc_MessageId.q_MessageIsTx,
                                                          u32_ListIndex) == C_NO_ERR)
                  {
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
               C_PuiSdNodeCanMessage c_Message;
               c_Message.c_Signals = orc_UISignals;
               rc_Messages.insert(rc_Messages.begin() + orc_MessageId.u32_MessageIndex, c_Message);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Delete ui part of can message

   \param[in] orc_MessageId Message identification indices

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     12.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::m_DeleteUiCanMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(orc_MessageId.u32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OSCCanProtocol * const pc_Protocol = pc_Node->GetCANProtocolConst(orc_MessageId.e_ComProtocol);
      C_PuiSdNodeCanProtocol * const pc_UiProtocol = this->m_GetUiCanProtocol(orc_MessageId.u32_NodeIndex,
                                                                              orc_MessageId.e_ComProtocol);

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
                  orc_MessageId.e_ComProtocol);
               if (pc_OSCDataPool != NULL)
               {
                  C_PuiSdNodeDataPool & rc_UIDataPool = rc_UINode.c_UIDataPools[pc_Protocol->u32_DataPoolIndex];
                  uint32 u32_ListIndex;
                  if (C_OSCCanProtocol::h_GetComListIndex(*pc_OSCDataPool, orc_MessageId.u32_InterfaceIndex,
                                                          orc_MessageId.q_MessageIsTx,
                                                          u32_ListIndex) == C_NO_ERR)
                  {
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

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition application change

   \param[in] ou32_NodeIndex        Node index
   \param[in] ou32_ApplicationIndex Application index

   \created     29.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::m_SyncNodeApplicationAdded(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationIndex)
{
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      //Data pools
      for (uint32 u32_ItDataPool = 0; u32_ItDataPool < rc_Node.c_DataPools.size(); ++u32_ItDataPool)
      {
         C_OSCNodeDataPool & rc_DataPool = rc_Node.c_DataPools[u32_ItDataPool];
         //Only sync valid indices
         if (rc_DataPool.s32_RelatedDataBlockIndex >= 0)
         {
            if (static_cast<uint32>(rc_DataPool.s32_RelatedDataBlockIndex) >= ou32_ApplicationIndex)
            {
               ++rc_DataPool.s32_RelatedDataBlockIndex;
            }
         }
      }
      //Code generation settings
      //Only sync valid indices
      if (rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex >= 0)
      {
         //lint -e{571} Above zero is checked
         if (static_cast<uint32>(rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex) >=
             ou32_ApplicationIndex)
         {
            ++rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex;
         }
      }
      else
      {
         //Check if there is a new programmable application
         m_HandleNodeAutomatedProgrammableApplicationUpdate(ou32_NodeIndex);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition application change

   \param[in] ou32_NodeIndex              Node index
   \param[in] ou32_ApplicationSourceIndex Application source index
   \param[in] ou32_ApplicationTargetIndex Application target index

   \created     29.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::m_SyncNodeApplicationMoved(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationSourceIndex,
                                                const uint32 ou32_ApplicationTargetIndex)
{
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      //Data pools
      for (uint32 u32_ItDataPool = 0; u32_ItDataPool < rc_Node.c_DataPools.size(); ++u32_ItDataPool)
      {
         C_OSCNodeDataPool & rc_DataPool = rc_Node.c_DataPools[u32_ItDataPool];
         //Only sync valid indices
         if (rc_DataPool.s32_RelatedDataBlockIndex >= 0)
         {
            if (static_cast<uint32>(rc_DataPool.s32_RelatedDataBlockIndex) > ou32_ApplicationSourceIndex)
            {
               --rc_DataPool.s32_RelatedDataBlockIndex;
               if (static_cast<uint32>(rc_DataPool.s32_RelatedDataBlockIndex) >= ou32_ApplicationTargetIndex)
               {
                  ++rc_DataPool.s32_RelatedDataBlockIndex;
               }
               else
               {
                  //No adaptation necessary
               }
            }
            else if (static_cast<uint32>(rc_DataPool.s32_RelatedDataBlockIndex) == ou32_ApplicationSourceIndex)
            {
               rc_DataPool.s32_RelatedDataBlockIndex = ou32_ApplicationTargetIndex;
            }
            else
            {
               if (static_cast<uint32>(rc_DataPool.s32_RelatedDataBlockIndex) >= ou32_ApplicationTargetIndex)
               {
                  ++rc_DataPool.s32_RelatedDataBlockIndex;
               }
               else
               {
                  //No adaptation necessary
               }
            }
         }
      }
      //Code generation settings
      //Only sync valid indices
      if (rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex >= 0)
      {
         //lint -e{571} Above zero is checked
         if (static_cast<uint32>(rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex) >
             ou32_ApplicationSourceIndex)
         {
            --rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex;
            //lint -e{571} Above zero is checked
            if (static_cast<uint32>(rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex) >=
                ou32_ApplicationTargetIndex)
            {
               ++rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex;
            }
            else
            {
               //No adaptation necessary
            }
         }
         //lint -e{571} Above zero is checked
         else if (static_cast<uint32>(rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex) ==
                  ou32_ApplicationSourceIndex)
         {
            rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex =
               static_cast<sint16>(ou32_ApplicationTargetIndex);
         }
         else
         {
            //lint -e{571} Above zero is checked
            if (static_cast<uint32>(rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex) >=
                ou32_ApplicationTargetIndex)
            {
               ++rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex;
            }
            else
            {
               //No adaptation necessary
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition application change

   \param[in] ou32_NodeIndex        Node index
   \param[in] ou32_ApplicationIndex Application index

   \created     29.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::m_SyncNodeApplicationAboutToBeDeleted(const uint32 ou32_NodeIndex,
                                                           const uint32 ou32_ApplicationIndex)
{
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      //Data pools
      for (uint32 u32_ItDataPool = 0; u32_ItDataPool < rc_Node.c_DataPools.size(); ++u32_ItDataPool)
      {
         C_OSCNodeDataPool & rc_DataPool = rc_Node.c_DataPools[u32_ItDataPool];
         //Only sync valid indices
         if (rc_DataPool.s32_RelatedDataBlockIndex >= 0)
         {
            if (static_cast<uint32>(rc_DataPool.s32_RelatedDataBlockIndex) > ou32_ApplicationIndex)
            {
               --rc_DataPool.s32_RelatedDataBlockIndex;
            }
            else if (static_cast<uint32>(rc_DataPool.s32_RelatedDataBlockIndex) == ou32_ApplicationIndex)
            {
               rc_DataPool.s32_RelatedDataBlockIndex = -1;
            }
            else
            {
               //No adaptation necessary
            }
         }
      }
      //Code generation settings
      //Only sync valid indices
      if (rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex >= 0)
      {
         //lint -e{571} Above zero is checked
         if (static_cast<uint32>(rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex) >
             ou32_ApplicationIndex)
         {
            --rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex;
         }
         //lint -e{571} Above zero is checked
         else if (static_cast<uint32>(rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex) ==
                  ou32_ApplicationIndex)
         {
            rc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex = -1;
         }
         else
         {
            //No adaptation necessary
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle automated programmable application assignment for code generation settings

   \param[in] ou32_NodeIndex Node index (ID)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     04.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::m_HandleNodeAutomatedProgrammableApplicationUpdate(const uint32 ou32_NodeIndex)
{
   sint32 s32_Retval;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      sint32 s32_TmpIndex = static_cast<sint32>(pc_Node->c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex);
      s32_Retval = GetNextProgrammableApplicationIndex(ou32_NodeIndex, s32_TmpIndex);
      if (s32_Retval == C_NO_ERR)
      {
         C_OSCNodeOpenSYDEServerSettings c_Copy = pc_Node->c_Properties.c_OpenSYDEServerSettings;
         c_Copy.s16_DPDDataBlockIndex = static_cast<sint16>(s32_TmpIndex);
         //See if there was any effective change
         if (c_Copy.s16_DPDDataBlockIndex != pc_Node->c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex)
         {
            s32_Retval = this->SetNodeOpenSYDEServerSettings(ou32_NodeIndex, c_Copy);
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
   \brief   Utility function to fix name errors for existing projects

   \created     05.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::m_FixNameIssues(void)
{
   for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      rc_OSCNode.c_Properties.c_Name = C_PuiSdHandler::h_AutomaticCStringAdaptation(
         rc_OSCNode.c_Properties.c_Name.c_str(), false).toStdString().c_str();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utility function to fix address errors for existing projects

   \created     25.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::m_FixAddressIssues(void)
{
   for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      rc_OSCNode.RecalculateAddress();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utility function to fix inconsistency errors for internally created inconsistencies in existing projects

   \created     09.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdHandler::m_FixCommInconsistencyErrors(void)
{
   //Nodes
   //-----
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size())
   {
      for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size();
           ++u32_ItNode)
      {
         C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
         C_PuiSdNode & rc_UiNode = this->mc_UINodes[u32_ItNode];

         //Protocols
         //---------
         tgl_assert(rc_UiNode.c_UICanProtocols.size() == rc_OSCNode.c_ComProtocols.size());
         if (rc_UiNode.c_UICanProtocols.size() == rc_OSCNode.c_ComProtocols.size())
         {
            for (uint32 u32_ItProtocol = 0;
                 u32_ItProtocol < rc_OSCNode.c_ComProtocols.size(); ++u32_ItProtocol)
            {
               uint32 u32_CANInterfaceCounter = 0;
               C_OSCCanProtocol & rc_OSCProtocol = rc_OSCNode.c_ComProtocols[u32_ItProtocol];
               C_PuiSdNodeCanProtocol & rc_UiProtocol = rc_UiNode.c_UICanProtocols[u32_ItProtocol];

               //Fix interface inconsistency (message container)
               for (uint32 u32_ItInterface = 0; u32_ItInterface < rc_OSCNode.c_Properties.c_ComInterfaces.size();
                    ++u32_ItInterface)
               {
                  const C_OSCNodeComInterfaceSettings & rc_CurInterface =
                     rc_OSCNode.c_Properties.c_ComInterfaces[u32_ItInterface];
                  if (rc_CurInterface.e_InterfaceType == C_OSCSystemBus::eCAN)
                  {
                     ++u32_CANInterfaceCounter;
                  }
               }
               if (u32_CANInterfaceCounter != rc_OSCProtocol.c_ComMessages.size())
               {
                  rc_OSCProtocol.c_ComMessages.resize(u32_CANInterfaceCounter);
                  rc_UiProtocol.c_ComMessages.resize(u32_CANInterfaceCounter);
                  osc_write_log_warning("Loading project",
                                        "The number of COMM message containers is inconsistent with the number"
                                        " of CAN interfaces in your node,"
                                        " so the number of COMM message containers was"
                                        " changed to the expected level.");
               }

               //Fix interface inconsistency (lists)
               //Data pools
               //----------
               tgl_assert((rc_OSCProtocol.u32_DataPoolIndex < rc_OSCNode.c_DataPools.size()) &&
                          (rc_OSCProtocol.u32_DataPoolIndex < rc_UiNode.c_UIDataPools.size()));
               if ((rc_OSCProtocol.u32_DataPoolIndex < rc_OSCNode.c_DataPools.size()) &&
                   (rc_OSCProtocol.u32_DataPoolIndex < rc_UiNode.c_UIDataPools.size()))
               {
                  C_OSCNodeDataPool & rc_OSCDataPool = rc_OSCNode.c_DataPools[rc_OSCProtocol.u32_DataPoolIndex];
                  C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UIDataPools[rc_OSCProtocol.u32_DataPoolIndex];

                  //Lists
                  //-----
                  tgl_assert(rc_OSCDataPool.c_Lists.size() == rc_UiDataPool.c_DataPoolLists.size());
                  if (rc_OSCDataPool.c_Lists.size() == rc_UiDataPool.c_DataPoolLists.size())
                  {
                     //Message containers
                     //------------------
                     tgl_assert(rc_UiProtocol.c_ComMessages.size() == rc_OSCProtocol.c_ComMessages.size());
                     if (rc_UiProtocol.c_ComMessages.size() == rc_OSCProtocol.c_ComMessages.size())
                     {
                        //Inconsistency error check
                        if ((rc_OSCDataPool.c_Lists.size() % 2) == 0)
                        {
                           if (rc_OSCDataPool.c_Lists.size() < (rc_OSCProtocol.c_ComMessages.size() * 2))
                           {
                              rc_OSCProtocol.c_ComMessages.resize(rc_OSCDataPool.c_Lists.size() / 2);
                              rc_UiProtocol.c_ComMessages.resize(rc_OSCDataPool.c_Lists.size() / 2);
                              osc_write_log_warning("Loading project",
                                                    "The number of COMM Datapools are inconsistent to the number"
                                                    " of COMM Message container in your COMM protocols"
                                                    " so the number of COMM Message container were"
                                                    " reduced to the expected level.");
                           }
                        }
                        else
                        {
                           osc_write_log_warning("Loading project",
                                                 "At least one COMM Datapool has an invalid number of lists"
                                                 " (should be double the number of CAN interfaces)");
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
   \brief   Do sort of messages by name for specified node

   \param[in,out] orc_OSCNode Node core part
   \param[in,out] orc_UiNode  Node UI part

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid

   \created     09.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSdHandler::mh_SortMessagesByName(C_OSCNode & orc_OSCNode, C_PuiSdNode & orc_UiNode)
{
   sint32 s32_Retval = C_NO_ERR;

   //No sync manager necessary because sorting the nodes by name internally
   // does not change how the messages are merged together

   //Protocols
   //---------
   tgl_assert(orc_UiNode.c_UICanProtocols.size() == orc_OSCNode.c_ComProtocols.size());
   if (orc_UiNode.c_UICanProtocols.size() == orc_OSCNode.c_ComProtocols.size())
   {
      for (uint32 u32_ItProtocol = 0;
           (u32_ItProtocol < orc_OSCNode.c_ComProtocols.size()) && (s32_Retval == C_NO_ERR); ++u32_ItProtocol)
      {
         C_OSCCanProtocol & rc_OSCProtocol = orc_OSCNode.c_ComProtocols[u32_ItProtocol];
         C_PuiSdNodeCanProtocol & rc_UiProtocol = orc_UiNode.c_UICanProtocols[u32_ItProtocol];

         //Data pools
         //----------
         tgl_assert((rc_OSCProtocol.u32_DataPoolIndex < orc_OSCNode.c_DataPools.size()) &&
                    (rc_OSCProtocol.u32_DataPoolIndex < orc_UiNode.c_UIDataPools.size()));
         if ((rc_OSCProtocol.u32_DataPoolIndex < orc_OSCNode.c_DataPools.size()) &&
             (rc_OSCProtocol.u32_DataPoolIndex < orc_UiNode.c_UIDataPools.size()))
         {
            C_OSCNodeDataPool & rc_OSCDataPool = orc_OSCNode.c_DataPools[rc_OSCProtocol.u32_DataPoolIndex];
            C_PuiSdNodeDataPool & rc_UiDataPool = orc_UiNode.c_UIDataPools[rc_OSCProtocol.u32_DataPoolIndex];

            //Message containers
            //------------------
            tgl_assert(rc_UiProtocol.c_ComMessages.size() == rc_OSCProtocol.c_ComMessages.size());
            if (rc_UiProtocol.c_ComMessages.size() == rc_OSCProtocol.c_ComMessages.size())
            {
               for (uint32 u32_ItContainer = 0;
                    (u32_ItContainer < rc_OSCProtocol.c_ComMessages.size()) && (s32_Retval == C_NO_ERR);
                    ++u32_ItContainer)
               {
                  C_OSCCanMessageContainer & rc_OSCContainer = rc_OSCProtocol.c_ComMessages[u32_ItContainer];
                  C_PuiSdNodeCanMessageContainer & rc_UiContainer =
                     rc_UiProtocol.c_ComMessages[u32_ItContainer];

                  //Messages
                  //--------
                  tgl_assert((rc_OSCContainer.c_RxMessages.size() == rc_UiContainer.c_RxMessages.size()) &&
                             (rc_OSCContainer.c_TxMessages.size() == rc_UiContainer.c_TxMessages.size()));
                  if ((rc_OSCContainer.c_RxMessages.size() == rc_UiContainer.c_RxMessages.size()) &&
                      (rc_OSCContainer.c_TxMessages.size() == rc_UiContainer.c_TxMessages.size()))
                  {
                     //Lists
                     //-----
                     //Container index -> interface index
                     const sint32 s32_TxListIndex = C_OSCCanProtocol::h_GetListIndex(rc_OSCDataPool,
                                                                                     u32_ItContainer,
                                                                                     true);
                     const sint32 s32_RxListIndex = C_OSCCanProtocol::h_GetListIndex(rc_OSCDataPool,
                                                                                     u32_ItContainer,
                                                                                     false);
                     tgl_assert(((((s32_TxListIndex >= 0) && (s32_RxListIndex >= 0)) &&
                                  (rc_OSCDataPool.c_Lists.size() == rc_UiDataPool.c_DataPoolLists.size())) &&
                                 (static_cast<uint32>(s32_TxListIndex) < rc_OSCDataPool.c_Lists.size())) &&
                                (static_cast<uint32>(s32_RxListIndex) < rc_OSCDataPool.c_Lists.size()));
                     if (((((s32_TxListIndex >= 0) && (s32_RxListIndex >= 0)) &&
                           (rc_OSCDataPool.c_Lists.size() == rc_UiDataPool.c_DataPoolLists.size())) &&
                          (static_cast<uint32>(s32_TxListIndex) < rc_OSCDataPool.c_Lists.size())) &&
                         (static_cast<uint32>(s32_RxListIndex) < rc_OSCDataPool.c_Lists.size()))
                     {
                        C_OSCNodeDataPoolList & rc_OSCTxList =
                           rc_OSCDataPool.c_Lists[static_cast<uint32>(s32_TxListIndex)];
                        C_OSCNodeDataPoolList & rc_OSCRxList =
                           rc_OSCDataPool.c_Lists[static_cast<uint32>(s32_RxListIndex)];
                        C_PuiSdNodeDataPoolList & rc_UiTxList =
                           rc_UiDataPool.c_DataPoolLists[static_cast<uint32>(s32_TxListIndex)];
                        C_PuiSdNodeDataPoolList & rc_UiRxList =
                           rc_UiDataPool.c_DataPoolLists[static_cast<uint32>(s32_RxListIndex)];

                        //Start sorting
                        //-------------
                        s32_Retval = C_SdBueSortHelper::h_SortOneMessageVector(rc_OSCContainer.c_TxMessages,
                                                                               rc_UiContainer.c_TxMessages,
                                                                               rc_OSCTxList,
                                                                               rc_UiTxList);
                        if (s32_Retval == C_NO_ERR)
                        {
                           s32_Retval = C_SdBueSortHelper::h_SortOneMessageVector(rc_OSCContainer.c_RxMessages,
                                                                                  rc_UiContainer.c_RxMessages,
                                                                                  rc_OSCRxList,
                                                                                  rc_UiRxList);
                        }
                     }
                     else
                     {
                        s32_Retval = C_CONFIG;
                     }
                  }
                  else
                  {
                     s32_Retval = C_CONFIG;
                  }
                  //Trigger data element index recalculation
                  rc_OSCContainer.ReCalcDataElementIndices();
               }
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}
