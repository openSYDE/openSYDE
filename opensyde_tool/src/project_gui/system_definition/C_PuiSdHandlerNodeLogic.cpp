//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node logic part of UI system definition

   Node logic part of UI system definition

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_Uti.h"
#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_GtGetText.h"
#include "C_PuiSdUtil.h"
#include "C_PuiSdHandler.h"

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
/*! \brief   Check if any node uses the provided name

   \param[in]      orc_Name               Node name to check for
   \param[in]      opu32_NodeIndexToSkip  Optional parameter to skip one index
                                          (Use-case: skip current node to avoid conflict with itself)
   \param[in,out]  opc_ExistingNames      Optional parameter to list all OTHER existing node names

   \return
   true  Available
   false Already in use
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerNodeLogic::CheckNodeNameAvailable(const C_SCLString & orc_Name,
                                                     const uint32 * const opu32_NodeIndexToSkip,
                                                     std::vector<stw_scl::C_SCLString> * const opc_ExistingNames) const
{
   bool q_Retval = true;

   //Either end on error or continue if all node names are requested
   for (uint32 u32_ItNode = 0;
        (u32_ItNode < this->mc_CoreDefinition.c_Nodes.size()) && ((q_Retval == true) || (opc_ExistingNames != NULL));
        ++u32_ItNode)
   {
      bool q_Skip = false;
      if (opu32_NodeIndexToSkip != NULL)
      {
         q_Skip = this->CheckNodeIndexAssociatedWithAnotherNodeIndex(*opu32_NodeIndexToSkip, u32_ItNode);
      }
      if (q_Skip == false)
      {
         const stw_scl::C_SCLString c_CurName = C_PuiSdUtil::h_GetNodeBaseNameOrName(u32_ItNode).toStdString().c_str();
         //Check conflict
         if (c_CurName.LowerCase() == orc_Name.LowerCase())
         {
            q_Retval = false;
         }
         //Store other (not checked) node name
         if (opc_ExistingNames != NULL)
         {
            bool q_Exists = false;
            for (uint32 u32_It = 0UL; u32_It < opc_ExistingNames->size(); ++u32_It)
            {
               if ((*opc_ExistingNames)[u32_It] == c_CurName)
               {
                  q_Exists = true;
               }
            }
            if (!q_Exists)
            {
               opc_ExistingNames->push_back(c_CurName);
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node ethernet configuration

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  orc_Ip               Ip address
   \param[in]  orc_NetMask          Net mask
   \param[in]  orc_DefaultGateway   Default gateway
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::SetOSCNodeEthernetConfiguration(const uint32 ou32_NodeIndex,
                                                              const uint32 ou32_InterfaceIndex,
                                                              const std::vector<sint32> & orc_Ip,
                                                              const std::vector<sint32> & orc_NetMask,
                                                              const std::vector<sint32> & orc_DefaultGateway)
{
   if (((orc_Ip.size() == 4UL) && (orc_NetMask.size() == 4UL)) && (orc_DefaultGateway.size() == 4UL))
   {
      if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
      {
         C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
         if (ou32_InterfaceIndex < rc_Node.c_Properties.c_ComInterfaces.size())
         {
            C_OSCNodeComInterfaceSettings & rc_CurInterface = rc_Node.c_Properties.c_ComInterfaces[ou32_InterfaceIndex];
            rc_CurInterface.c_Ip.au8_IpAddress[0] = static_cast<uint8>(orc_Ip[0]);
            rc_CurInterface.c_Ip.au8_IpAddress[1] = static_cast<uint8>(orc_Ip[1]);
            rc_CurInterface.c_Ip.au8_IpAddress[2] = static_cast<uint8>(orc_Ip[2]);
            rc_CurInterface.c_Ip.au8_IpAddress[3] = static_cast<uint8>(orc_Ip[3]);
            rc_CurInterface.c_Ip.au8_NetMask[0] = static_cast<uint8>(orc_NetMask[0]);
            rc_CurInterface.c_Ip.au8_NetMask[1] = static_cast<uint8>(orc_NetMask[1]);
            rc_CurInterface.c_Ip.au8_NetMask[2] = static_cast<uint8>(orc_NetMask[2]);
            rc_CurInterface.c_Ip.au8_NetMask[3] = static_cast<uint8>(orc_NetMask[3]);
            rc_CurInterface.c_Ip.au8_DefaultGateway[0] = static_cast<uint8>(orc_DefaultGateway[0]);
            rc_CurInterface.c_Ip.au8_DefaultGateway[1] = static_cast<uint8>(orc_DefaultGateway[1]);
            rc_CurInterface.c_Ip.au8_DefaultGateway[2] = static_cast<uint8>(orc_DefaultGateway[2]);
            rc_CurInterface.c_Ip.au8_DefaultGateway[3] = static_cast<uint8>(orc_DefaultGateway[3]);

            Q_EMIT (this->SigNodeChanged(ou32_NodeIndex));
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get UI node from array

   Get UI node from array

   \param[in]  oru32_Index    Index

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNode * C_PuiSdHandlerNodeLogic::GetUINode(const uint32 & oru32_Index) const
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC node squad from array

   Get OSC node squad from array

   \param[in]  oru32_Index    Index

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeSquad * C_PuiSdHandlerNodeLogic::GetOSCNodeSquadConst(const uint32 & oru32_Index) const
{
   const C_OSCNodeSquad * pc_Retval;

   if (oru32_Index < this->mc_CoreDefinition.c_NodeSquads.size())
   {
      pc_Retval = &(this->mc_CoreDefinition.c_NodeSquads[oru32_Index]);
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC node from array

   Get OSC node from array

   \param[in]  oru32_Index    Index

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNode * C_PuiSdHandlerNodeLogic::GetOSCNodeConst(const uint32 & oru32_Index) const
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get osc node with sorted internal structure

   Warning: this function might be time consuming

   \param[in]   oru32_Index   Node index (ID)
   \param[out]  orc_Node      Sorted node

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
   C_RANGE  Operation failure: parameter invalid
   C_COM    Sorting failed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::GetSortedOSCNodeConst(const uint32 & oru32_Index, C_OSCNode & orc_Node) const
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get non const OSC node from array

   Get non const OSC node from array

   \param[in]  oru32_Index    Index

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1762} no it cannot be const as it returns a non-const pointer; use const version of this function if needed
C_OSCNode * C_PuiSdHandlerNodeLogic::GetOSCNode(const uint32 & oru32_Index)
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get non const OSC node squad from array

   Get non const OSC node squad from array

   \param[in]  oru32_Index    Index

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1762} no it cannot be const as it returns a non-const pointer; use const version of this function if needed
C_OSCNodeSquad * C_PuiSdHandlerNodeLogic::GetOSCNodeSquad(const uint32 & oru32_Index)
{
   C_OSCNodeSquad * pc_Retval;

   if (oru32_Index < this->mc_CoreDefinition.c_NodeSquads.size())
   {
      pc_Retval = &(this->mc_CoreDefinition.c_NodeSquads[oru32_Index]);
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set STW flashloader settings

   \param[in]  ou32_Index     Node index
   \param[in]  orc_Settings   Settings

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetStwFlashloaderSettings(const uint32 ou32_Index,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node openSYDE server settings

   \param[in]  ou32_Index     Node index
   \param[in]  orc_Settings   Settings

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetNodeOpenSYDEServerSettings(const uint32 ou32_Index,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node code export settings

   \param[in]  ou32_Index     Node index
   \param[in]  orc_Settings   Settings

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetNodeCodeExportSettings(const uint32 ou32_Index,
                                                          const C_OSCNodeCodeExportSettings & orc_Settings)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_Index];
      rc_Node.c_Properties.c_CodeExportSettings = orc_Settings;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply node properties

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_Properties    New properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::SetOSCNodeProperties(const uint32 ou32_NodeIndex,
                                                   const C_OSCNodeProperties & orc_Properties)
{
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      rc_OSCNode.c_Properties = orc_Properties;
      //Signal new name!
      Q_EMIT (this->SigNodeChanged(ou32_NodeIndex));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply node properties

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  orc_Name             New name
   \param[in]  orc_Comment          New comment
   \param[in]  oe_DiagnosticServer  New diagnostic server
   \param[in]  oe_FlashLoader       New flash loader
   \param[in]  orc_NodeIds          New node IDs
   \param[in]  orc_UpdateFlags      New update active flags
   \param[in]  orc_RoutingFlags     New routing active flags
   \param[in]  orc_DiagnosisFlags   New diagnosis active flags
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::SetOSCNodePropertiesDetailed(const uint32 ou32_NodeIndex, const QString & orc_Name,
                                                           const QString & orc_Comment,
                                                           const C_OSCNodeProperties::E_DiagnosticServerProtocol oe_DiagnosticServer, const C_OSCNodeProperties::E_FlashLoaderProtocol oe_FlashLoader, const std::vector<uint8> & orc_NodeIds, const std::vector<bool> & orc_UpdateFlags, const std::vector<bool> & orc_RoutingFlags,
                                                           const std::vector<bool> & orc_DiagnosisFlags)
{
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];

      //set name (special handling)
      this->mc_CoreDefinition.SetNodeName(ou32_NodeIndex, orc_Name.toStdString().c_str());

      //set other properties
      rc_OSCNode.c_Properties.c_Comment = orc_Comment.toStdString().c_str();
      rc_OSCNode.c_Properties.e_DiagnosticServer = oe_DiagnosticServer;
      rc_OSCNode.c_Properties.e_FlashLoader = oe_FlashLoader;
      if (((orc_NodeIds.size() == orc_UpdateFlags.size()) && (orc_NodeIds.size() == orc_RoutingFlags.size())) &&
          (orc_NodeIds.size() == orc_DiagnosisFlags.size()))
      {
         for (uint32 u32_ItInt = 0UL;
              (u32_ItInt < rc_OSCNode.c_Properties.c_ComInterfaces.size()) && (u32_ItInt < orc_NodeIds.size());
              ++u32_ItInt)
         {
            C_OSCNodeComInterfaceSettings & rc_Interface = rc_OSCNode.c_Properties.c_ComInterfaces[u32_ItInt];
            rc_Interface.q_IsUpdateEnabled = orc_UpdateFlags[u32_ItInt];
            rc_Interface.q_IsRoutingEnabled = orc_RoutingFlags[u32_ItInt];
            rc_Interface.q_IsDiagnosisEnabled = orc_DiagnosisFlags[u32_ItInt];
         }
      }
      m_SetOSCNodeIds(ou32_NodeIndex, orc_NodeIds);
      //Signal new name!
      Q_EMIT (this->SigNodeChanged(ou32_NodeIndex));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set UI node box

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_Box           Box data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::SetUINodeBox(const uint32 ou32_NodeIndex, const C_PuiBsBox & orc_Box)
{
   if (ou32_NodeIndex < this->mc_UINodes.size())
   {
      C_PuiSdNode & rc_UINode = this->mc_UINodes[ou32_NodeIndex];
      //Copy box manually
      rc_UINode.c_UIPosition = orc_Box.c_UIPosition;
      rc_UINode.f64_Height = orc_Box.f64_Height;
      rc_UINode.f64_Width = orc_Box.f64_Width;
      rc_UINode.f64_ZOrder = orc_Box.f64_ZOrder;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node

   \param[in,out]  orc_OSCNode         New OSC node (name might be modified by this function if not unique)
   \param[in,out]  orc_UINode          New UI node
   \param[in]      orc_SubDeviceName   Sub device name
   \param[in]      orc_MainDevice      Main device

   \return
   Index of node  (0 -> first node)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSdHandlerNodeLogic::AddNodeAndSort(C_OSCNode & orc_OSCNode, const C_PuiSdNode & orc_UINode,
                                               const QString & orc_SubDeviceName, const QString & orc_MainDevice)
{
   const uint32 u32_Index = mc_CoreDefinition.c_Nodes.size();
   //Extract device name if the device was already set
   const C_SCLString c_DeviceName = (orc_OSCNode.pc_DeviceDefinition !=
                                     NULL) ? orc_OSCNode.pc_DeviceDefinition->GetDisplayName() : "";
   const C_SCLString c_DefaultDeviceName =
      C_PuiSdHandlerNodeLogic::h_AutomaticCStringAdaptation(c_DeviceName.c_str()).toStdString().c_str();

   orc_OSCNode.c_Properties.c_Name = C_Uti::h_GetUniqueName(
      this->m_GetExistingNodeNames(), orc_OSCNode.c_Properties.c_Name, c_DefaultDeviceName);

   mc_CoreDefinition.AddNode(orc_OSCNode, orc_SubDeviceName.toStdString().c_str(),
                             orc_MainDevice.toStdString().c_str()); //add node and set device definition
   // pointer

   //insert UI part at same position as OSC part:
   this->mc_UINodes.insert(mc_UINodes.begin() + u32_Index, orc_UINode);

   //signal "node change"
   Q_EMIT (this->SigNodesChanged());
   this->m_HandleSyncNodeAdded(u32_Index);

   // No adaption of the shared Datapools necessary.
   // The new node index is always higher than the already existing nodes indexes

   return u32_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node squad

   \param[in,out]  orc_OSCNodes        New OSC nodes (name might be modified by this function if not unique)
   \param[in,out]  orc_UINodes         New UI nodes
   \param[in]      orc_NodeNames       Node names
   \param[in]      orc_MainDevice      Main device
   \param[in]      orc_NameProposal    Name proposal

   \return
   Index of first node of node squad (0 -> first node)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSdHandlerNodeLogic::AddNodeSquadAndSort(std::vector<C_OSCNode> & orc_OSCNodes,
                                                    const std::vector<C_PuiSdNode> & orc_UINodes,
                                                    const std::vector<QString> & orc_NodeNames,
                                                    const QString & orc_MainDevice, const QString & orc_NameProposal)
{
   const uint32 u32_NodeIndex = mc_CoreDefinition.c_Nodes.size();
   const uint32 u32_NodeSquadIndex = mc_CoreDefinition.c_NodeSquads.size();

   std::vector<stw_scl::C_SCLString> c_NodeNames;
   C_SCLString c_Name;
   uint32 u32_NodeCounter;

   c_NodeNames.reserve(orc_NodeNames.size());
   for (uint32 u32_ItNode = 0UL; u32_ItNode < orc_NodeNames.size(); ++u32_ItNode)
   {
      c_NodeNames.push_back(orc_NodeNames[u32_ItNode].toStdString().c_str());
   }

   tgl_assert(orc_OSCNodes.size() > 0);
   tgl_assert(orc_OSCNodes.size() == orc_UINodes.size());
   tgl_assert(orc_OSCNodes.size() == orc_NodeNames.size());
   if (orc_OSCNodes.size() > 0)
   {
      // The device name of the first node is enough. Must be identical on all sub nodes
      C_OSCNode & rc_OSCNode = orc_OSCNodes[0];
      //Extract device name if the device was already set
      const C_SCLString c_DeviceName = (rc_OSCNode.pc_DeviceDefinition !=
                                        NULL) ? rc_OSCNode.pc_DeviceDefinition->GetDisplayName() : "";
      const C_SCLString c_DefaultDeviceName =
         C_PuiSdHandlerNodeLogic::h_AutomaticCStringAdaptation(c_DeviceName.c_str()).toStdString().c_str();
      c_Name = orc_NameProposal.isEmpty() ? c_DefaultDeviceName : orc_NameProposal.toStdString().c_str();

      // The proposed name would be identical for all sub nodes too. The sub node specific part of the name
      // will be added with SetBaseName
      c_Name = C_Uti::h_GetUniqueName(
         this->m_GetExistingNodeNames(), c_Name, c_DefaultDeviceName);
   }

   this->mc_CoreDefinition.AddNodeSquad(orc_OSCNodes, c_NodeNames, orc_MainDevice.toStdString().c_str()); //add node and
                                                                                                          // set device
   // definition pointer

   tgl_assert(u32_NodeSquadIndex < mc_CoreDefinition.c_NodeSquads.size());
   this->mc_CoreDefinition.c_NodeSquads[u32_NodeSquadIndex].SetBaseName(this->mc_CoreDefinition.c_Nodes,
                                                                        c_Name);

   //insert UI part at same position as OSC part:
   this->mc_UINodes.insert(mc_UINodes.begin() + u32_NodeIndex, orc_UINodes.begin(), orc_UINodes.end());

   //signal "node change"
   Q_EMIT (this->SigNodesChanged());
   for (u32_NodeCounter = 0U; u32_NodeCounter < orc_OSCNodes.size(); ++u32_NodeCounter)
   {
      // Signals for all node indexes necessary
      this->m_HandleSyncNodeAdded(u32_NodeIndex + u32_NodeCounter);
   }

   // No adaption of the shared Datapools or already existing node squads necessary.
   // The new node index is always higher than the already existing nodes indexes

   return u32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove node

   In case of a node which is a sub node of a squad node, the entire squad node will be removed

   \param[in]  ou32_NodeIndex    Connector node index (0 -> first node)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::RemoveNode(const uint32 ou32_NodeIndex)
{
   sint32 s32_NodeIndexCounter;

   const std::vector<uint32> c_AllNodeIndexToRemove = this->GetAllNodeGroupIndicesUsingNodeIndex(ou32_NodeIndex);

   // Start with the last index due to the highest index. Avoiding problems with deleting and changing orders for
   // the other node index
   for (s32_NodeIndexCounter = (static_cast<sint32>(c_AllNodeIndexToRemove.size()) - 1); s32_NodeIndexCounter >= 0;
        --s32_NodeIndexCounter)
   {
      //Synchronization engine (First!)
      this->m_HandleSyncNodeAboutToBeDeleted(c_AllNodeIndexToRemove[static_cast<uint32>(s32_NodeIndexCounter)]);
   }

   tgl_assert(this->mc_CoreDefinition.DeleteNode(ou32_NodeIndex) == C_NO_ERR);

   for (s32_NodeIndexCounter = (static_cast<sint32>(c_AllNodeIndexToRemove.size()) - 1); s32_NodeIndexCounter >= 0;
        --s32_NodeIndexCounter)
   {
      this->mc_UINodes.erase(
         this->mc_UINodes.begin() + c_AllNodeIndexToRemove[static_cast<uint32>(s32_NodeIndexCounter)]);

      // Update shared Datapool configuration
      this->mc_SharedDatapools.OnNodeRemoved(c_AllNodeIndexToRemove[static_cast<uint32>(s32_NodeIndexCounter)]);
   }

   //signal "node change"
   Q_EMIT (this->SigNodesChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if there is a node conflict

   \param[in]  oru32_NodeIndex   Node index

   \return
   true  Conflict
   false No conflict
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerNodeLogic::CheckNodeConflict(const uint32 & oru32_NodeIndex) const
{
   bool q_Retval;
   static QMap<uint32, bool> hc_PreviousResult;

   //Get reference hash
   const uint32 u32_Hash = this->m_GetHashNode(oru32_NodeIndex);
   //Look up
   const QMap<uint32, bool>::const_iterator c_It = hc_PreviousResult.find(u32_Hash);

   if (c_It == hc_PreviousResult.end())
   {
      bool q_NameConflict;
      bool q_NameEmpty;
      bool q_NodeIdInvalid;
      bool q_IpInvalid;
      bool q_DataPoolsInvalid;
      bool q_ApplicationsInvalid;
      bool q_DomainsInvalid;
      bool q_CommSignalCountInvalid;
      bool q_CoPdoCountInvalid;
      bool q_CoNodeIdInvalid;
      bool q_CoHeartbeatInvalid;

      if (this->mc_CoreDefinition.CheckErrorNode(oru32_NodeIndex, &q_NameConflict, &q_NameEmpty, &q_NodeIdInvalid,
                                                 &q_IpInvalid,
                                                 &q_DataPoolsInvalid, &q_ApplicationsInvalid, &q_DomainsInvalid,
                                                 &q_CommSignalCountInvalid, &q_CoPdoCountInvalid,
                                                 &q_CoNodeIdInvalid, &q_CoHeartbeatInvalid,
                                                 true, NULL, NULL, NULL, NULL, NULL) == C_NO_ERR)
      {
         const bool q_NvmSizeConflict = this->CheckNodeNvmDataPoolsSizeConflict(oru32_NodeIndex);
         if ((q_NameConflict == true) || (q_NodeIdInvalid == true) || (q_IpInvalid == true) ||
             (q_DataPoolsInvalid == true) ||
             (q_ApplicationsInvalid == true) || (q_DomainsInvalid == true) || (q_NameEmpty == true) ||
             (q_NvmSizeConflict == true) ||
             (q_CommSignalCountInvalid == true) ||
             (q_CoPdoCountInvalid == true) ||
             (q_CoNodeIdInvalid == true) ||
             (q_CoHeartbeatInvalid == true))
         {
            q_Retval = true;
         }
         else
         {
            q_Retval = false;
         }
         //Store for future reference
         if ((q_DataPoolsInvalid == true) || (q_ApplicationsInvalid == true) || (q_DomainsInvalid == true) ||
             (q_NameEmpty == true) || (q_NvmSizeConflict == true) ||
             (q_CoNodeIdInvalid == true) || (q_CommSignalCountInvalid == true) || (q_CoPdoCountInvalid == true) ||
             (q_CoHeartbeatInvalid == true))
         {
            hc_PreviousResult.insert(u32_Hash, true);
         }
         else
         {
            hc_PreviousResult.insert(u32_Hash, false);
         }
      }
      else
      {
         q_Retval = true;
      }
   }
   else
   {
      //Only do name and id conflict check, otherwise reuse stored value
      bool q_NameConflict;
      bool q_NodeIdInvalid;
      bool q_IpInvalid;

      if (this->mc_CoreDefinition.CheckErrorNode(oru32_NodeIndex, &q_NameConflict, NULL, &q_NodeIdInvalid, &q_IpInvalid,
                                                 NULL, NULL, NULL, NULL, NULL, NULL,
                                                 NULL, true, NULL, NULL, NULL, NULL, NULL) == C_NO_ERR)
      {
         if ((q_NameConflict == true) || (q_NodeIdInvalid == true) || (q_IpInvalid == true))
         {
            q_Retval = true;
         }
         else
         {
            // use stored value
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
/*! \brief   Returns all supported CAN bitrates of specific nodes

   If no nodes are available, all CAN bitrates will be supported.

   \param[in]   orc_Nodes     Vector with all relevant node indexes
   \param[out]  orc_Bitrates  Result vector with all supported CAN bitrates
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::GetSupportedCanBitrates(const std::vector<uint32> & orc_Nodes,
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
      const C_OSCNode * const pc_Node = this->GetOSCNodeConst(orc_Nodes[u32_NodeCounter]);

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC Nodes size

   \return
   total number of nodes
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::uint32 C_PuiSdHandlerNodeLogic::GetOSCNodesSize(void) const
{
   return this->mc_CoreDefinition.c_Nodes.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC Node squads size

   \return
   total number of node squads
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::uint32 C_PuiSdHandlerNodeLogic::GetOSCNodeSquadsSize(void) const
{
   return this->mc_CoreDefinition.c_NodeSquads.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all node group indices using node index

   \param[in]  ou32_NodeIndex    Node index

   \return
   All node group indices using node index
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_PuiSdHandlerNodeLogic::GetAllNodeGroupIndicesUsingNodeIndex(const uint32 ou32_NodeIndex) const
{
   std::vector<uint32> c_Retval;
   bool q_Found = false;
   for (uint32 u32_ItGroup = 0UL; u32_ItGroup < this->mc_CoreDefinition.c_NodeSquads.size(); ++u32_ItGroup)
   {
      const C_OSCNodeSquad & rc_Group = this->mc_CoreDefinition.c_NodeSquads[u32_ItGroup];
      for (uint32 u32_ItSubDevice = 0UL; u32_ItSubDevice < rc_Group.c_SubNodeIndexes.size(); ++u32_ItSubDevice)
      {
         if (rc_Group.c_SubNodeIndexes[u32_ItSubDevice] == ou32_NodeIndex)
         {
            q_Found = true;
            c_Retval.reserve(rc_Group.c_SubNodeIndexes.size());
            for (uint32 u32_ItNewSubDevice = 0UL; u32_ItNewSubDevice < rc_Group.c_SubNodeIndexes.size();
                 ++u32_ItNewSubDevice)
            {
               c_Retval.push_back(rc_Group.c_SubNodeIndexes[u32_ItNewSubDevice]);
            }
            break;
         }
      }
   }
   if (!q_Found)
   {
      c_Retval.push_back(ou32_NodeIndex);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Search the node squad with a specific node index as potential sub node

   \param[in]   ou32_NodeIndex         Searched node index of a sub node of a squad node
   \param[out]  oru32_NodeSquadIndex   Found squad node index if return value is C_NO_ERR

   \retval   C_NO_ERR   Node squad with specific sub node with node index found
   \retval   C_RANGE    No node squad found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::GetNodeSquadIndexWithNodeIndex(const uint32 ou32_NodeIndex,
                                                               stw_types::uint32 & oru32_NodeSquadIndex) const
{
   return this->mc_CoreDefinition.GetNodeSquadIndexWithNodeIndex(ou32_NodeIndex, oru32_NodeSquadIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns a vector with a mapping to the node squads

   The index of the vector equals the node index and the value has following definition:
   Value < 0 : Node with node index is not part of a squad
   Value >= 0: Node with node index is part of a node squad and the node squad has the index of the value

   The vector will be cleared first.

   \param[out]  orc_Mapping   Vector with mapping
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::GetNodeToNodeSquadMapping(std::vector<sint32> & orc_Mapping)
{
   uint32 u32_NodeCounter;

   orc_Mapping.clear();

   // Initialize with -1 for no squad node
   orc_Mapping.resize(this->mc_CoreDefinition.c_Nodes.size(), -1);

   for (u32_NodeCounter = 0U; u32_NodeCounter < this->mc_CoreDefinition.c_Nodes.size(); ++u32_NodeCounter)
   {
      uint32 u32_NodeSquadIndex = 0U;
      if (this->mc_CoreDefinition.GetNodeSquadIndexWithNodeIndex(u32_NodeCounter, u32_NodeSquadIndex) == C_NO_ERR)
      {
         // Node is part of an node squad
         orc_Mapping[u32_NodeCounter] = static_cast<sint32>(u32_NodeSquadIndex);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Detect critical name conflicts

   Warning: not performance optimized so the output is clear

   \param[in,out]  opc_CriticalNodeNames                 Conflicting node names
   \param[in,out]  opc_CriticalBusNames                  Conflicting bus names
   \param[in,out]  opc_CriticalDatapoolNamespaceNames    Conflicting datapool names

   \return
   True  Critical name conflict detected
   False No critical name conflict detected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerNodeLogic::CheckCriticalNameConflict(std::vector<QString> * const opc_CriticalNodeNames,
                                                        std::vector<QString> * const opc_CriticalBusNames,
                                                        std::vector<QString> * const opc_CriticalDatapoolNamespaceNames)
const
{
   bool q_Retval = false;

   //Nodes
   for (uint32 u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      bool q_ErrorDetected = false;
      const C_OSCNode & rc_CheckedNode = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      //Start at begin again to list each name conflict twice!
      for (uint32 u32_ItNodeRef = 0UL; u32_ItNodeRef < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNodeRef)
      {
         const C_OSCNode & rc_CurNode = this->mc_CoreDefinition.c_Nodes[u32_ItNodeRef];
         if (u32_ItNode != u32_ItNodeRef)
         {
            if (rc_CheckedNode.c_Properties.c_Name.LowerCase() == rc_CurNode.c_Properties.c_Name.LowerCase())
            {
               q_ErrorDetected = true;
               break;
            }
         }
      }
      if (q_ErrorDetected == true)
      {
         q_Retval = true;
         if (opc_CriticalNodeNames != NULL)
         {
            opc_CriticalNodeNames->push_back(rc_CheckedNode.c_Properties.c_Name.c_str());
         }
      }
      //Datapools
      for (uint32 u32_ItDatapool = 0UL; u32_ItDatapool < rc_CheckedNode.c_DataPools.size(); ++u32_ItDatapool)
      {
         const C_OSCNodeDataPool & rc_CheckedDatapool = rc_CheckedNode.c_DataPools[u32_ItDatapool];
         //reset error
         q_ErrorDetected = false;
         //Start at begin again to list each name conflict twice!
         for (uint32 u32_ItDatapoolRef = 0UL; u32_ItDatapoolRef < rc_CheckedNode.c_DataPools.size();
              ++u32_ItDatapoolRef)
         {
            const C_OSCNodeDataPool & rc_CurDatapool = rc_CheckedNode.c_DataPools[u32_ItDatapoolRef];
            if (u32_ItDatapool != u32_ItDatapoolRef)
            {
               if (rc_CheckedDatapool.c_Name.LowerCase() == rc_CurDatapool.c_Name.LowerCase())
               {
                  q_ErrorDetected = true;
                  break;
               }
            }
         }
         if (q_ErrorDetected == true)
         {
            q_Retval = true;
            if (opc_CriticalDatapoolNamespaceNames != NULL)
            {
               const QString c_Combined =
                  static_cast<QString>("%1::%2").arg(rc_CheckedNode.c_Properties.c_Name.c_str()).arg(
                     rc_CheckedDatapool.c_Name.c_str());
               opc_CriticalDatapoolNamespaceNames->push_back(c_Combined);
            }
         }
      }
   }
   //Buses
   for (uint32 u32_ItBus = 0UL; u32_ItBus < this->mc_CoreDefinition.c_Buses.size(); ++u32_ItBus)
   {
      bool q_ErrorDetected = false;
      const C_OSCSystemBus & rc_CheckedBus = this->mc_CoreDefinition.c_Buses[u32_ItBus];
      //Start at begin again to list each name conflict twice!
      for (uint32 u32_ItBusRef = 0UL; u32_ItBusRef < this->mc_CoreDefinition.c_Buses.size(); ++u32_ItBusRef)
      {
         const C_OSCSystemBus & rc_CurBus = this->mc_CoreDefinition.c_Buses[u32_ItBusRef];
         if (u32_ItBus != u32_ItBusRef)
         {
            if (rc_CheckedBus.c_Name.LowerCase() == rc_CurBus.c_Name.LowerCase())
            {
               q_ErrorDetected = true;
               break;
            }
         }
      }
      if (q_ErrorDetected == true)
      {
         q_Retval = true;
         if (opc_CriticalBusNames != NULL)
         {
            opc_CriticalBusNames->push_back(rc_CheckedBus.c_Name.c_str());
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map node name to index

   \param[in]   orc_NodeName     Node name
   \param[out]  oru32_NodeIndex  Node index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::MapNodeNameToIndex(const QString & orc_NodeName, uint32 & oru32_NodeIndex) const
{
   sint32 s32_Retval = C_RANGE;

   for (uint32 u32_ItNode = 0UL; u32_ItNode < this->GetOSCNodesSize(); ++u32_ItNode)
   {
      const C_OSCNode * const pc_Node = this->GetOSCNodeConst(u32_ItNode);
      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         if (orc_NodeName.compare(pc_Node->c_Properties.c_Name.c_str()) == 0)
         {
            s32_Retval = C_NO_ERR;
            oru32_NodeIndex = u32_ItNode;
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map node index to name

   \param[in]   ou32_NodeIndex   Node index
   \param[out]  orc_NodeName     Node name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::MapNodeIndexToName(const uint32 ou32_NodeIndex, QString & orc_NodeName) const
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      orc_NodeName = pc_Node->c_Properties.c_Name.c_str();
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if device has a flashloader available to communicate with

   \param[in]  ou32_NodeIndex    Node index

   \return
   True  STW device
   False Third party
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerNodeLogic::HasNodeAnAvailableFlashloader(const uint32 ou32_NodeIndex) const
{
   bool q_Retval = false;

   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OSCDeviceDefinition * const pc_Device = pc_Node->pc_DeviceDefinition;
      const uint32 u32_SubDeviceIndex = pc_Node->u32_SubDeviceIndex;
      tgl_assert(pc_Device != NULL);
      if (pc_Device != NULL)
      {
         tgl_assert(u32_SubDeviceIndex < pc_Device->c_SubDevices.size());
         if (u32_SubDeviceIndex < pc_Device->c_SubDevices.size())
         {
            if (((pc_Device->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderStwCan == false) &&
                 (pc_Device->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeCan == false)) &&
                (pc_Device->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeEthernet == false))
            {
               q_Retval = false;
            }
            else
            {
               q_Retval = true;
            }
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check node index associated with another node index

   \param[in]  ou32_FirstNodeIndex     First node index
   \param[in]  ou32_SecondNodeIndex    Second node index

   \return
   True  Associated
   False Not related
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerNodeLogic::CheckNodeIndexAssociatedWithAnotherNodeIndex(const uint32 ou32_FirstNodeIndex,
                                                                           const uint32 ou32_SecondNodeIndex) const
{
   bool q_Retval = false;

   const std::vector<uint32> c_NodeIndices = this->GetAllNodeGroupIndicesUsingNodeIndex(ou32_FirstNodeIndex);

   for (uint32 u32_ItDevice = 0UL; u32_ItDevice < c_NodeIndices.size(); ++u32_ItDevice)
   {
      if (c_NodeIndices[u32_ItDevice] == ou32_SecondNodeIndex)
      {
         q_Retval = true;
         break;
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add data pool

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  orc_OSCContent             OSC data pool content (name might be modified by this function if not unique)
   \param[in]  orc_UIContent              UI data pool content
   \param[in]  ore_ComProtocolType        Optional parameter for com protocol type
   \param[in]  orq_AllowNameAdaptation    Optional parameter to disable automatic name adaptation
   \param[in]  orq_AllowDataAdaptation    Optional parameter to disable automatic data adaptation

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::AddDataPool(const uint32 & oru32_NodeIndex, const C_OSCNodeDataPool & orc_OSCContent,
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
      if ((orc_OSCContent.e_Type != C_OSCNodeDataPool::eHALC) &&
          (orc_OSCContent.e_Type != C_OSCNodeDataPool::eHALC_NVM))
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
         if ((orc_OSCContent.e_Type == C_OSCNodeDataPool::eNVM) ||
             (orc_OSCContent.e_Type == C_OSCNodeDataPool::eCOM))
         {
            //Go to end of NVM in case of NVM and COMM Datapools
            for (; u32_ItTargetIndex < rc_OSCNode.c_DataPools.size(); ++u32_ItTargetIndex)
            {
               pc_DataPool = &rc_OSCNode.c_DataPools[u32_ItTargetIndex];
               if (pc_DataPool->e_Type != C_OSCNodeDataPool::eNVM)
               {
                  break;
               }
            }
         }
         if (orc_OSCContent.e_Type == C_OSCNodeDataPool::eCOM)
         {
            //Go to end of COMM
            for (; u32_ItTargetIndex < rc_OSCNode.c_DataPools.size(); ++u32_ItTargetIndex)
            {
               pc_DataPool = &rc_OSCNode.c_DataPools[u32_ItTargetIndex];
               if (pc_DataPool->e_Type != C_OSCNodeDataPool::eCOM)
               {
                  break;
               }
            }
         }
      }
      else
      {
         //Go to "total" end
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create and add automatically generated COMM Datapool

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  ore_ComProtocolType  Optional parameter for com protocol type

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::AddAutoGenCommDataPool(const uint32 & oru32_NodeIndex,
                                                       const C_OSCCanProtocol::E_Type & ore_ComProtocolType)
{
   C_OSCNodeDataPool c_NewDatapool;
   const C_PuiSdNodeDataPool c_UIDataPool;
   sint32 s32_Return;
   QString c_Comment;
   QString c_ProtocolName = C_PuiSdUtil::h_ConvertProtocolTypeToString(ore_ComProtocolType);

   // add the new datapool
   c_NewDatapool.e_Type = C_OSCNodeDataPool::eCOM;

   c_Comment = C_GtGetText::h_GetText("Automatically generated Datapool for ");
   c_Comment += c_ProtocolName;
   c_Comment += C_GtGetText::h_GetText(" CAN communication");

   // special case layer 2 -> no spaces and no underscore number at the end
   c_ProtocolName = C_PuiSdUtil::h_ConvertProtocolTypeToDatapoolNameString(ore_ComProtocolType);

   c_NewDatapool.c_Name = this->GetUniqueDataPoolName(oru32_NodeIndex, c_ProtocolName.toStdString().c_str());

   c_NewDatapool.c_Comment = c_Comment.toStdString().c_str();

   // set the default safety flag to true if protocol is a safety protocol
   c_NewDatapool.q_IsSafety = ((ore_ComProtocolType == C_OSCCanProtocol::eCAN_OPEN_SAFETY) ||
                               (ore_ComProtocolType == C_OSCCanProtocol::eECES));

   s32_Return = this->AddDataPool(oru32_NodeIndex, c_NewDatapool, c_UIDataPool, ore_ComProtocolType);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert data pool at specific position

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  orc_OSCContent             OSC data pool content (name might be modified by this function if not unique)
   \param[in]  orc_UIContent              UI data pool content
   \param[in]  ore_ComProtocolType        Optional parameter for com protocol type
   \param[in]  orq_AllowNameAdaptation    Optional parameter to disable automatic name adaptation
   \param[in]  orq_AllowDataAdaptation    Optional parameter to disable automatic data adaptation

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::InsertDataPool(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                               const C_OSCNodeDataPool & orc_OSCContent,
                                               const C_PuiSdNodeDataPool & orc_UIContent,
                                               const C_OSCCanProtocol::E_Type & ore_ComProtocolType,
                                               const bool & orq_AllowNameAdaptation,
                                               const bool & orq_AllowDataAdaptation)
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

         // Update shared Datapool configuration
         this->mc_SharedDatapools.OnDatapoolInserted(C_OSCNodeDataPoolId(oru32_NodeIndex, oru32_DataPoolIndex));

         //Synchronization engine
         Q_EMIT (this->SigSyncNodeDataPoolAdded(oru32_NodeIndex, oru32_DataPoolIndex));
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
/*! \brief   Remove data pool

   \param[in]  oru32_NodeIndex         Node index
   \param[in]  oru32_DataPoolIndex     Data pool index
   \param[in]  oq_SuppressSyncSignal   Suppress sync signal

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::RemoveDataPool(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                               const bool oq_SuppressSyncSignal)
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
         if (oq_SuppressSyncSignal == false)
         {
            //Synchronization engine (First!)
            Q_EMIT (this->SigSyncNodeDataPoolAboutToBeDeleted(oru32_NodeIndex, oru32_DataPoolIndex));
         }
         //Handle COMM
         m_CleanUpComDataPool(oru32_NodeIndex, oru32_DataPoolIndex);
         rc_UINode.c_UIDataPools.erase(rc_UINode.c_UIDataPools.begin() + oru32_DataPoolIndex);
         s32_Retval = rc_OSCNode.DeleteDataPool(oru32_DataPoolIndex);
         //Handle NVM
         rc_OSCNode.RecalculateAddress();

         // Update shared Datapool configuration
         this->mc_SharedDatapools.OnDatapoolRemoved(C_OSCNodeDataPoolId(oru32_NodeIndex, oru32_DataPoolIndex));
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
/*! \brief   Set data pool

   \param[in]  oru32_NodeIndex         Node index
   \param[in]  oru32_DataPoolIndex     Data pool index
   \param[in]  orc_OSCContent          OSC data pool content
   \param[in]  orc_UIContent           UI data pool content
   \param[in]  oq_NewComProtocolType   Flag if the COM protocol type was changed
   \param[in]  ore_ComProtocolType     Optional parameter for com protocol type

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
   C_NOACT  Input invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetDataPool(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                            const C_OSCNodeDataPool & orc_OSCContent,
                                            const C_PuiSdNodeDataPool & orc_UIContent, const bool oq_NewComProtocolType,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Unassign data pool application

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Data pool index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::UnassignDataPoolApplication(const uint32 & oru32_NodeIndex,
                                                            const uint32 & oru32_DataPoolIndex)
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Assign data pool application

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_DataPoolIndex      Data pool index
   \param[in]  os32_ApplicationIndex   Application index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::AssignDataPoolApplication(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Assign all HALC NVM Datapools to the PSI Data Block if possible.

   \param[in]  ou32_NodeIndex    Node index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::AssignAllHalcNvmDataPools(const uint32 ou32_NodeIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      sint32 s32_DataBlockPsi = -1;

      // search for Data Block of type PSI generation
      if ((rc_Node.c_HALCConfig.q_NvMBasedConfig == true) && (rc_Node.c_HALCConfig.IsClear() == false))
      {
         for (uint32 u32_ItDataBlock = 0UL; u32_ItDataBlock < rc_Node.c_Applications.size(); ++u32_ItDataBlock)
         {
            const C_OSCNodeApplication & rc_CurDataBlock = rc_Node.c_Applications[u32_ItDataBlock];
            if (rc_CurDataBlock.e_Type == C_OSCNodeApplication::ePARAMETER_SET_HALC)
            {
               s32_DataBlockPsi = static_cast<sint32>(u32_ItDataBlock);
               break;
            }
         }
      }

      // assign all HALC NVM Datapools
      if (s32_DataBlockPsi >= 0)
      {
         for (uint32 u32_ItDataPool = 0UL; (u32_ItDataPool < rc_Node.c_DataPools.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItDataPool)
         {
            const C_OSCNodeDataPool & rc_DataPool = rc_Node.c_DataPools[u32_ItDataPool];
            if (rc_DataPool.e_Type == C_OSCNodeDataPool::eHALC_NVM)
            {
               s32_Retval = AssignDataPoolApplication(ou32_NodeIndex, u32_ItDataPool, s32_DataBlockPsi);
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
/*! \brief   Get data pool

   \param[in]   oru32_NodeIndex        Node index
   \param[in]   oru32_DataPoolIndex    Data pool index
   \param[out]  orc_OSCContent         OSC data pool content
   \param[out]  orc_UIContent          UI data pool content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::GetDataPool(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                            C_OSCNodeDataPool & orc_OSCContent,
                                            C_PuiSdNodeDataPool & orc_UIContent) const
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get unique data pool name based on proposal

   \param[in]  oru32_NodeIndex   Node index
   \param[in]  orc_Proposal      Data pool name proposal

   \return
   Unique data pool name based on proposal
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_PuiSdHandlerNodeLogic::GetUniqueDataPoolName(const uint32 & oru32_NodeIndex,
                                                           const C_SCLString & orc_Proposal) const
{
   return C_Uti::h_GetUniqueName(this->m_GetExistingNodeDataPoolNames(oru32_NodeIndex), orc_Proposal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC data pool

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Data pool index

   \return
   Found: Pointer to OSC data pool
   Else:  NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPool * C_PuiSdHandlerNodeLogic::GetOSCDataPool(const uint32 & oru32_NodeIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get UI data pool

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Data pool index

   \return
   Found: Pointer to UI data pool
   Else:  NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNodeDataPool * C_PuiSdHandlerNodeLogic::GetUIDataPool(const uint32 & oru32_NodeIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data pool type

   \param[in]   oru32_NodeIndex        Node index
   \param[in]   oru32_DataPoolIndex    Data pool index
   \param[out]  ore_Type               OSC data pool type

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::GetDataPoolType(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move datapool in node

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_SourceIndex  Source list index
   \param[in]  ou32_TargetIndex  Target list index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::MoveDataPool(const uint32 ou32_NodeIndex, const uint32 ou32_SourceIndex,
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
            const C_PuiSdNodeDataPool c_Data = rc_UINode.c_UIDataPools[ou32_SourceIndex];
            //Erase
            rc_UINode.c_UIDataPools.erase(rc_UINode.c_UIDataPools.begin() + ou32_SourceIndex);
            //Insert
            rc_UINode.c_UIDataPools.insert(rc_UINode.c_UIDataPools.begin() + ou32_TargetIndex, c_Data);

            //Handle NVM
            this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex].RecalculateAddress();

            // Update shared Datapool configuration
            this->mc_SharedDatapools.OnDatapoolMoved(C_OSCNodeDataPoolId(ou32_NodeIndex, ou32_SourceIndex),
                                                     C_OSCNodeDataPoolId(ou32_NodeIndex, ou32_TargetIndex));

            //Synchronization engine
            Q_EMIT (this->SigSyncNodeDataPoolMoved(ou32_NodeIndex, ou32_SourceIndex, ou32_TargetIndex));
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get datapool index by type specific datapool index

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  oe_DataPoolType         Type of datapool
   \param[in]  ou32_DataPoolTypeIndex  Type specific index of datapool

   \return
   if datapool is found: zero based index
   if datapool is not found or parameter out of range: -1
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::GetDataPoolIndex(const uint32 ou32_NodeIndex,
                                                 const C_OSCNodeDataPool::E_Type oe_DataPoolType,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get Datapool type specific index from Datapool index

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool total index

   \return
   if datapool is found: zero based index
   if datapool is not found or parameter out of range: -1
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::GetDataPoolTypeIndex(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex) const
{
   sint32 s32_Return = -1;

   //Check size & consistency
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_UINodes.size())
   {
      const C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Return = rc_OSCNode.GetDataPoolTypeIndex(ou32_DataPoolIndex);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get number of Datapools of specified type

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oe_DataPoolType   Data pool type

   \return
   Number of Datapools (-1 if parameters are invalid)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::GetDataPoolCount(const uint32 ou32_NodeIndex,
                                                 const C_OSCNodeDataPool::E_Type oe_DataPoolType) const
{
   sint32 s32_Return = -1;

   //Check size & consistency
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_UINodes.size())
   {
      const C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Return = rc_OSCNode.GetDataPoolCount(oe_DataPoolType);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if any data pool uses the provided name

   \param[in]      oru32_NodeIndex              Node index
   \param[in]      orc_Name                     Data pool name to check for
   \param[in]      opu32_DataPoolIndexToSkip    Optional parameter to skip one index
                                                (Use-case: skip current data pool to avoid conflict with itself)
   \param[in,out]  opc_ExistingDatapoolNames    Optional parameter to list all OTHER existing datapool names

   \return
   true  Available
   false Already in use
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerNodeLogic::CheckNodeDataPoolNameAvailable(const uint32 & oru32_NodeIndex,
                                                             const C_SCLString & orc_Name,
                                                             const uint32 * const opu32_DataPoolIndexToSkip,
                                                             std::vector<C_SCLString> * const opc_ExistingDatapoolNames)
const
{
   bool q_Retval = true;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      //Don't stop on error if the existing datapool names are relevant
      for (uint32 u32_ItDataPool = 0;
           (u32_ItDataPool < rc_Node.c_DataPools.size()) && ((q_Retval == true) || (opc_ExistingDatapoolNames != NULL));
           ++u32_ItDataPool)
      {
         bool q_Skip = false;
         //Check for datapool to skip
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
            if (opc_ExistingDatapoolNames != NULL)
            {
               opc_ExistingDatapoolNames->push_back(rc_DataPool.c_Name);
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the shared datapools configuration

   \return
   Shared datapool configuration
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdSharedDatapools & C_PuiSdHandlerNodeLogic::GetSharedDatapools(void)
{
   return this->mc_SharedDatapools;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the const shared datapools configuration

   \return
   Shared datapool configuration
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdSharedDatapools & C_PuiSdHandlerNodeLogic::GetSharedDatapoolsConst(void) const
{
   return this->mc_SharedDatapools;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor for interface class
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea::C_PuiSdHandlerNodeLogicNvmArea(void) :
   u32_StartAddress(0U),
   u32_Size(0U),
   q_InRange(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current smaller than orc_Cmp

   Comparing by start address

   \param[in]  orc_Cmp  Compared instance

   \return
   true  Current smaller than orc_Cmp
   false orc_Cmp equal or bigger than current
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea::operator <(
   const C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea & orc_Cmp) const
{
   bool q_Return;

   if (this->u32_StartAddress < orc_Cmp.u32_StartAddress)
   {
      q_Return = true;
   }
   else if ((this->u32_StartAddress == orc_Cmp.u32_StartAddress) &&
            (this->u32_Size < orc_Cmp.u32_Size))
   {
      q_Return = true;
   }
   else
   {
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Assignment operator

   \param[in]  orc_Source  Reference to source

   \return
   reference to new instance
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea &  C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea::
operator =(const C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea & orc_Source)
{
   if (this != &orc_Source)
   {
      this->u32_StartAddress = orc_Source.u32_StartAddress;
      this->u32_Size = orc_Source.u32_Size;
      this->q_InRange = orc_Source.q_InRange;
      this->c_DataPoolIndexes = orc_Source.c_DataPoolIndexes;
   }

   return (*this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check all NVM Datapools of a node for size error

   \param[in]   ou32_NodeIndex         Node index
   \param[out]  opq_SizeConflict       Optional parameter for flag when a size conflict was detected
   \param[out]  opq_OverlapConflict    Optional parameter for flag when an overlap conflict was detected

   \return
   true  Conflict
   false No conflict
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerNodeLogic::CheckNodeNvmDataPoolsSizeConflict(const uint32 ou32_NodeIndex,
                                                                bool * const opq_SizeConflict,
                                                                bool * const opq_OverlapConflict) const
{
   bool q_Return = true;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(ou32_NodeIndex);

   if (opq_SizeConflict != NULL)
   {
      *opq_SizeConflict = false;
   }
   if (opq_OverlapConflict != NULL)
   {
      *opq_OverlapConflict = false;
   }

   if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL) &&
       (pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size()))
   {
      std::vector<C_PuiSdHandlerNodeLogicNvmArea> c_Areas;
      if (this->GetNodeNvmDataPoolAreas(ou32_NodeIndex, c_Areas) == C_NO_ERR)
      {
         uint32 u32_Counter;
         uint32 u32_SizeUsedAreas = 0U;
         bool q_OutOfRange = false;
         bool q_OverlapDetected = false;

         // get the size of all used NVM areas and check for overlaps
         for (u32_Counter = 0; u32_Counter < c_Areas.size(); ++u32_Counter)
         {
            if (c_Areas[u32_Counter].c_DataPoolIndexes.size() > 0)
            {
               u32_SizeUsedAreas += c_Areas[u32_Counter].u32_Size;
               //lint -e{514}  Using operator with a bool value was intended and is no accident
               q_OutOfRange |= (!c_Areas[u32_Counter].q_InRange);

               if (c_Areas[u32_Counter].c_DataPoolIndexes.size() > 1)
               {
                  q_OverlapDetected = true;
               }
            }
         }

         if ((u32_SizeUsedAreas <=
              pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].u32_UserEepromSizeBytes) &&
             (q_OverlapDetected == false) &&
             (q_OutOfRange == false))
         {
            // No errors detected
            q_Return = false;
         }
         else
         {
            // Return detailed information about the error when needed
            if (opq_SizeConflict != NULL)
            {
               *opq_SizeConflict =
                  (u32_SizeUsedAreas >
                   pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].u32_UserEepromSizeBytes) ||
                  q_OutOfRange;
            }
            if (opq_OverlapConflict != NULL)
            {
               *opq_OverlapConflict = q_OverlapDetected;
            }
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Analyzing the NVM memory areas of the node and its Datapools

   All NVM based Datapools are considered. Overlapping of areas or not enough space are not handled as error here.
   Three different scenarios can occur:
   c_DatapoolNames has
   * 0 elements:  Free area
   * 1 element:   One Datapool is in this area
   * >1 elements: At least two Datapools are in this area and has overlapping memory addresses

   If the flag q_InRange is set to false of at least one area, the NVM memory of the node is not big enough for the
   current configuration.

   \param[in]   ou32_NodeIndex   Node index
   \param[out]  orc_Areas        All detected areas

   \retval   C_NO_ERR   No error
   \retval   C_RANGE    Parameter out of range
                        Device definition of node not valid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::GetNodeNvmDataPoolAreas(const uint32 ou32_NodeIndex,
                                                        std::vector<C_PuiSdHandlerNodeLogicNvmArea> & orc_Areas)
const
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Areas.clear();

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];

      if (rc_Node.pc_DeviceDefinition != NULL)
      {
         C_PuiSdHandlerNodeLogic::mh_GetNodeNvmDataPoolAreas(rc_Node, orc_Areas);
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
/*! \brief   Add application

   \param[in]      ou32_NodeIndex   Node index
   \param[in,out]  orc_OSCContent   Application content (Name might get changed)

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::AddApplication(const uint32 ou32_NodeIndex, C_OSCNodeApplication & orc_OSCContent)
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert application at specific position

   \param[in]      ou32_NodeIndex         Node index
   \param[in]      ou32_ApplicationIndex  Application index
   \param[in,out]  orc_OSCContent         Application content (Name might get changed)

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::InsertApplication(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationIndex,
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
         Q_EMIT (this->SigSyncNodeApplicationAdded(ou32_NodeIndex, ou32_ApplicationIndex));
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove application

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::RemoveApplication(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_ApplicationIndex < rc_Node.c_Applications.size())
      {
         //Synchronization engine (First!)
         Q_EMIT (this->SigSyncNodeApplicationAboutToBeDeleted(ou32_NodeIndex, ou32_ApplicationIndex));
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set application

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
   \param[in]  orc_OSCContent          Application content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetApplication(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationIndex,
                                               const C_OSCNodeApplication & orc_OSCContent)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_ApplicationIndex < rc_Node.c_Applications.size())
      {
         const C_OSCNodeApplication::E_Type e_PreviousType = rc_Node.c_Applications[ou32_ApplicationIndex].e_Type;

         // inform SC about incoming Data Block change from PSI to binary type (before change!)
         if ((e_PreviousType == C_OSCNodeApplication::ePARAMETER_SET_HALC) &&
             (orc_OSCContent.e_Type == C_OSCNodeApplication::eBINARY))
         {
            Q_EMIT (C_PuiSdHandler::h_GetInstance()->
                    SigSyncNodeApplicationAboutToBeChangedFromParamSetHALC(ou32_NodeIndex, ou32_ApplicationIndex));
         }

         // set data
         rc_Node.c_Applications[ou32_ApplicationIndex] = orc_OSCContent;

         // inform SC about Data Block change from binary to PSI type (after change!)
         if ((e_PreviousType == C_OSCNodeApplication::eBINARY) &&
             (orc_OSCContent.e_Type == C_OSCNodeApplication::ePARAMETER_SET_HALC))
         {
            Q_EMIT (C_PuiSdHandler::h_GetInstance()->
                    SigSyncNodeApplicationChangedToParamSetHALC(ou32_NodeIndex, ou32_ApplicationIndex));
         }

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get application

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index

   \return
   NULL Application not found
   Else Valid application
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeApplication * C_PuiSdHandlerNodeLogic::GetApplication(const uint32 ou32_NodeIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move application in node

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_SourceIndex  Source application index
   \param[in]  ou32_TargetIndex  Target application index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::MoveApplication(const uint32 ou32_NodeIndex, const uint32 ou32_SourceIndex,
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
         const C_OSCNodeApplication c_Data = rc_Node.c_Applications[ou32_SourceIndex];
         //Erase
         rc_Node.c_Applications.erase(rc_Node.c_Applications.begin() + ou32_SourceIndex);
         //Insert
         rc_Node.c_Applications.insert(rc_Node.c_Applications.begin() + ou32_TargetIndex, c_Data);
         //Synchronization engine
         Q_EMIT (this->SigSyncNodeApplicationMoved(ou32_NodeIndex, ou32_SourceIndex, ou32_TargetIndex));
         m_SyncNodeApplicationMoved(ou32_NodeIndex, ou32_SourceIndex, ou32_TargetIndex);
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get unique application name based on proposal

   \param[in]  oru32_NodeIndex   Node index
   \param[in]  orc_Proposal      Application name proposal

   \return
   Unique application name based on proposal
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_PuiSdHandlerNodeLogic::GetUniqueApplicationName(const uint32 & oru32_NodeIndex,
                                                              const C_SCLString & orc_Proposal) const
{
   return C_Uti::h_GetUniqueName(this->m_GetExistingNodeApplicationNames(oru32_NodeIndex), orc_Proposal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get next unique application process ID

   \param[in]  ou32_NodeIndex    Node index (ID)
   \param[in]  ou8_Proposal      Proposal

   \return
   Next unique application process ID
*/
//----------------------------------------------------------------------------------------------------------------------
uint8 C_PuiSdHandlerNodeLogic::GetUniqueApplicationProcessId(const uint32 ou32_NodeIndex,
                                                             const uint8 ou8_Proposal) const
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle automated programmable application assignment

   \param[in]      ou32_NodeIndex            Node index (ID)
   \param[in,out]  ors32_ApplicationIndex    Application index, which is adapted if necessary

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::GetNextProgrammableApplicationIndex(const uint32 ou32_NodeIndex,
                                                                    sint32 & ors32_ApplicationIndex) const
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(ou32_NodeIndex);

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check application name valid

   \param[in]   ou32_NodeIndex         Node index
   \param[in]   orc_ApplicationName    Application name to check
   \param[out]  orq_Valid              Function result: flag if name is valid
   \param[in]   opu32_SkipApplication  Optional parameter to skip one application index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::CheckApplicationName(const uint32 ou32_NodeIndex,
                                                     const C_SCLString & orc_ApplicationName, bool & orq_Valid,
                                                     const uint32 * const opu32_SkipApplication) const
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all programmable applications for one node

   \param[in]  ou32_NodeIndex    Node index (ID)

   \return
   All known programmable applications for this node (empty if index invalid)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<const C_OSCNodeApplication *> C_PuiSdHandlerNodeLogic::GetProgrammableApplications(
   const uint32 ou32_NodeIndex) const
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all indices of file generation Data Blocks for one node

   \param[in]  ou32_NodeIndex    Node index (ID)

   \return
   All known file generation Data Blocks for this node (empty if index invalid)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_PuiSdHandlerNodeLogic::GetFileGenAppIndices(const uint32 ou32_NodeIndex) const
{
   std::vector<uint32> c_Return;
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      for (uint32 u32_ItAppl = 0; u32_ItAppl < rc_Node.c_Applications.size(); ++u32_ItAppl)
      {
         const C_OSCNodeApplication & rc_Application = rc_Node.c_Applications[u32_ItAppl];
         if (rc_Application.e_Type != C_OSCNodeApplication::eBINARY)
         {
            c_Return.push_back(u32_ItAppl);
         }
      }
   }
   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert data pool list at specific position

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    Data pool list index
   \param[in]  orc_OSCContent             OSC data pool list content
                                          (name might be modified by this function if not unique)
   \param[in]  orc_UIContent              UI data pool list content
   \param[in]  oq_HandleSharedDatapools   Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::InsertDataPoolList(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                                   const uint32 & oru32_DataPoolListIndex,
                                                   const C_OSCNodeDataPoolList & orc_OSCContent,
                                                   const C_PuiSdNodeDataPoolList & orc_UIContent,
                                                   const bool oq_HandleSharedDatapools)
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
         if (oru32_DataPoolListIndex <= rc_OSCDataPool.c_Lists.size())
         {
            //Unique name
            C_OSCNodeDataPoolList c_NodeDataPoolList = orc_OSCContent;
            c_NodeDataPoolList.c_Name =
               C_Uti::h_GetUniqueName(this->m_GetExistingNodeDataPoolListNames(oru32_NodeIndex,
                                                                               oru32_DataPoolIndex),
                                      c_NodeDataPoolList.c_Name);
            //Adapt required fields
            if (rc_OSCDataPool.q_IsSafety == true)
            {
               c_NodeDataPoolList.q_NvMCRCActive = true;
               for (uint32 u32_ItElement = 0; u32_ItElement < c_NodeDataPoolList.c_Elements.size(); ++u32_ItElement)
               {
                  C_OSCNodeDataPoolListElement & rc_CurElement = c_NodeDataPoolList.c_Elements[u32_ItElement];
                  rc_CurElement.e_Access = C_OSCNodeDataPoolListElement::eACCESS_RO;
               }
            }
            //Insert
            rc_UIDataPool.c_DataPoolLists.insert(rc_UIDataPool.c_DataPoolLists.begin() + oru32_DataPoolListIndex,
                                                 orc_UIContent);
            rc_OSCDataPool.c_Lists.insert(rc_OSCDataPool.c_Lists.begin() + oru32_DataPoolListIndex, c_NodeDataPoolList);

            rc_OSCNode.RecalculateAddress();

            //Synchronization engine
            Q_EMIT (this->SigSyncNodeDataPoolListAdded(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex));

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32 u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size(); ++u32_SharedDpCounter)
                  {
                     const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                     // No error expected. The Datapools must have the same conditions
                     tgl_assert(this->InsertDataPoolList(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                         oru32_DataPoolListIndex,
                                                         orc_OSCContent, orc_UIContent, false) == C_NO_ERR);
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
/*! \brief   Remove data pool list

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    Data pool list index
   \param[in]  oq_HandleSharedDatapools   Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::RemoveDataPoolList(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                                   const uint32 & oru32_DataPoolListIndex,
                                                   const bool oq_HandleSharedDatapools)
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
            //Synchronization engine (First!)
            Q_EMIT (this->SigSyncNodeDataPoolListAboutToBeDeleted(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                  oru32_DataPoolListIndex));
            rc_UIDataPool.c_DataPoolLists.erase(rc_UIDataPool.c_DataPoolLists.begin() + oru32_DataPoolListIndex);
            rc_OSCDataPool.c_Lists.erase(rc_OSCDataPool.c_Lists.begin() + oru32_DataPoolListIndex);

            rc_OSCNode.RecalculateAddress();

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32 u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size(); ++u32_SharedDpCounter)
                  {
                     const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                     // No error expected. The Datapools must have the same conditions
                     tgl_assert(this->RemoveDataPoolList(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                         oru32_DataPoolListIndex, false) == C_NO_ERR);
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
/*! \brief   Set data pool list

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    Data pool list index
   \param[in]  orc_OSCContent             OSC data pool list content
   \param[in]  orc_UIContent              UI data pool list content
   \param[in]  oq_HandleSharedDatapools   Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
   C_NOACT  Input invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetDataPoolList(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                                const uint32 & oru32_DataPoolListIndex,
                                                const C_OSCNodeDataPoolList & orc_OSCContent,
                                                const C_PuiSdNodeDataPoolList & orc_UIContent,
                                                const bool oq_HandleSharedDatapools)
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

               // Handle shared Datapools
               if (oq_HandleSharedDatapools == true)
               {
                  std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

                  if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                           c_SharedDatapools) == true)
                  {
                     uint32 u32_SharedDpCounter;

                     // Synchronize with the shared Datapools
                     for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                          ++u32_SharedDpCounter)
                     {
                        const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                        // No error expected. The Datapools must have the same conditions
                        tgl_assert(this->SetDataPoolList(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                         oru32_DataPoolListIndex, orc_OSCContent, orc_UIContent,
                                                         false) == C_NO_ERR);
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
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data pool list name

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    Data pool list index
   \param[in]  orc_Value                  New value
   \param[in]  oq_HandleSharedDatapools   Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetDataPoolListName(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                                    const uint32 & oru32_DataPoolListIndex, const QString & orc_Value,
                                                    const bool oq_HandleSharedDatapools)
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
            C_OSCNodeDataPoolList & rc_OSCList = rc_OSCDataPool.c_Lists[oru32_DataPoolListIndex];
            rc_OSCList.c_Name = orc_Value.toStdString().c_str();
            //Update addresses
            rc_OSCDataPool.RecalculateAddress();

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32 u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                       ++u32_SharedDpCounter)
                  {
                     const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                     // No error expected. The Datapools must have the same conditions
                     tgl_assert(this->SetDataPoolListName(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                          oru32_DataPoolListIndex, orc_Value,
                                                          false) == C_NO_ERR);
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
/*! \brief   Set data pool list comment

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    Data pool list index
   \param[in]  orc_Value                  New value
   \param[in]  oq_HandleSharedDatapools   Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetDataPoolListComment(const uint32 & oru32_NodeIndex,
                                                       const uint32 & oru32_DataPoolIndex,
                                                       const uint32 & oru32_DataPoolListIndex,
                                                       const QString & orc_Value, const bool oq_HandleSharedDatapools)
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
            C_OSCNodeDataPoolList & rc_OSCList = rc_OSCDataPool.c_Lists[oru32_DataPoolListIndex];
            rc_OSCList.c_Comment = orc_Value.toStdString().c_str();
            //Update addresses
            rc_OSCDataPool.RecalculateAddress();

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32 u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                       ++u32_SharedDpCounter)
                  {
                     const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                     // No error expected. The Datapools must have the same conditions
                     tgl_assert(this->SetDataPoolListComment(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                             oru32_DataPoolListIndex, orc_Value,
                                                             false) == C_NO_ERR);
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
/*! \brief   Set data pool list nvm size

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    Data pool list index
   \param[in]  ou32_Value                 New value
   \param[in]  oq_HandleSharedDatapools   Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetDataPoolListNVMSize(const uint32 & oru32_NodeIndex,
                                                       const uint32 & oru32_DataPoolIndex,
                                                       const uint32 & oru32_DataPoolListIndex, const uint32 ou32_Value,
                                                       const bool oq_HandleSharedDatapools)
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
            C_OSCNodeDataPoolList & rc_OSCList = rc_OSCDataPool.c_Lists[oru32_DataPoolListIndex];
            rc_OSCList.u32_NvMSize = ou32_Value;
            //Update addresses
            rc_OSCDataPool.RecalculateAddress();

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32 u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                       ++u32_SharedDpCounter)
                  {
                     const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                     // No error expected. The Datapools must have the same conditions
                     tgl_assert(this->SetDataPoolListNVMSize(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                             oru32_DataPoolListIndex, ou32_Value,
                                                             false) == C_NO_ERR);
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
/*! \brief   Set data pool list nvm crc flag

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    Data pool list index
   \param[in]  oq_Value                   New value
   \param[in]  oq_HandleSharedDatapools   Optional flag if the shared Datapools shall be updated too

   \retval C_NO_ERR  everything ok
   \retval C_RANGE   one of provided indices is out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetDataPoolListNVMCRC(const uint32 & oru32_NodeIndex,
                                                      const uint32 & oru32_DataPoolIndex,
                                                      const uint32 & oru32_DataPoolListIndex, const bool oq_Value,
                                                      const bool oq_HandleSharedDatapools)
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
            C_OSCNodeDataPoolList & rc_OSCList = rc_OSCDataPool.c_Lists[oru32_DataPoolListIndex];
            rc_OSCList.q_NvMCRCActive = oq_Value;
            //Update addresses
            rc_OSCDataPool.RecalculateAddress();

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32 u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                       ++u32_SharedDpCounter)
                  {
                     const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                     // No error expected. The Datapools must have the same conditions
                     tgl_assert(this->SetDataPoolListNVMCRC(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                            oru32_DataPoolListIndex, oq_Value,
                                                            false) == C_NO_ERR);
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
/*! \brief   Get data pool list

   \param[in]   oru32_NodeIndex           Node index
   \param[in]   oru32_DataPoolIndex       Data pool index
   \param[in]   oru32_DataPoolListIndex   Data pool list index
   \param[out]  orc_OSCContent            OSC data pool list content
   \param[out]  orc_UIContent             UI data pool list content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::GetDataPoolList(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                                const uint32 & oru32_DataPoolListIndex,
                                                C_OSCNodeDataPoolList & orc_OSCContent,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data pool list data set

   \param[in]   oru32_NodeIndex                 Node index
   \param[in]   oru32_DataPoolIndex             Data pool index
   \param[in]   oru32_DataPoolListIndex         Data pool list index
   \param[in]   oru32_DataPoolListDataSetIndex  Data pool list data set index
   \param[out]  orc_OSCName                     OSC data set name
   \param[out]  orc_OSCValues                   OSC data set values

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::GetDataPoolListDataSet(const uint32 & oru32_NodeIndex,
                                                       const uint32 & oru32_DataPoolIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC data pool list data set

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          Data pool list index
   \param[in]  oru32_DataPoolListDataSetIndex   Data pool list data set index

   \return
   Found: Pointer to OSC data pool list data set
   Else:  NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolDataSet * C_PuiSdHandlerNodeLogic::GetOSCDataPoolListDataSet(const uint32 & oru32_NodeIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data pool list data set

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          Data pool list index
   \param[in]  oru32_DataPoolListDataSetIndex   Data pool list data set index
   \param[in]  orc_OSCContent                   OSC data pool list element content
   \param[in]  oq_HandleSharedDatapools         Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetOSCNodeDataPoolDataSet(const uint32 & oru32_NodeIndex,
                                                          const uint32 & oru32_DataPoolIndex,
                                                          const uint32 & oru32_DataPoolListIndex,
                                                          const uint32 & oru32_DataPoolListDataSetIndex,
                                                          const C_OSCNodeDataPoolDataSet & orc_OSCContent,
                                                          const bool oq_HandleSharedDatapools)
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

               // Handle shared Datapools
               if (oq_HandleSharedDatapools == true)
               {
                  std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

                  if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                           c_SharedDatapools) == true)
                  {
                     uint32 u32_SharedDpCounter;

                     // Synchronize with the shared Datapools
                     for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                          ++u32_SharedDpCounter)
                     {
                        const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                        // No error expected. The Datapools must have the same conditions
                        tgl_assert(this->SetOSCNodeDataPoolDataSet(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                                   oru32_DataPoolListIndex,
                                                                   oru32_DataPoolListDataSetIndex,
                                                                   orc_OSCContent, false) == C_NO_ERR);
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert data pool list data set at specific position

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          Data pool list index
   \param[in]  oru32_DataPoolListDataSetIndex   Data pool list data set index
   \param[in]  orc_OSCName                      OSC data pool list data set name
                                                (name might be modified by this function if not unique)
   \param[in]  orc_OSCValues                    OSC data pool list data set values
   \param[in]  oq_HandleSharedDatapools         Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::InsertDataPoolListDataSet(const uint32 & oru32_NodeIndex,
                                                          const uint32 & oru32_DataPoolIndex,
                                                          const uint32 & oru32_DataPoolListIndex,
                                                          const uint32 & oru32_DataPoolListDataSetIndex,
                                                          const C_OSCNodeDataPoolDataSet & orc_OSCName,
                                                          const std::vector<C_OSCNodeDataPoolContent> & orc_OSCValues,
                                                          const bool oq_HandleSharedDatapools)
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
                  C_Uti::h_GetUniqueName(this->m_GetExistingNodeDataPoolListDataSetNames(oru32_NodeIndex,
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
            Q_EMIT (this->SigSyncNodeDataPoolListDataSetAdded(oru32_NodeIndex, oru32_DataPoolIndex,
                                                              oru32_DataPoolListIndex,
                                                              oru32_DataPoolListDataSetIndex));

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32 u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size(); ++u32_SharedDpCounter)
                  {
                     const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                     // No error expected. The Datapools must have the same conditions
                     tgl_assert(this->InsertDataPoolListDataSet(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                                oru32_DataPoolListIndex, oru32_DataPoolListDataSetIndex,
                                                                orc_OSCName, orc_OSCValues, false) == C_NO_ERR);
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
/*! \brief   Remove data pool list data set

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          Data pool list index
   \param[in]  oru32_DataPoolListDataSetIndex   Data pool list data set index
   \param[in]  oq_HandleSharedDatapools         Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::RemoveDataPoolListDataSet(const uint32 & oru32_NodeIndex,
                                                          const uint32 & oru32_DataPoolIndex,
                                                          const uint32 & oru32_DataPoolListIndex,
                                                          const uint32 & oru32_DataPoolListDataSetIndex,
                                                          const bool oq_HandleSharedDatapools)
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
            Q_EMIT (this->SigSyncNodeDataPoolListDataSetAboutToBeDeleted(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                         oru32_DataPoolListIndex,
                                                                         oru32_DataPoolListDataSetIndex));

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

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32 u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size(); ++u32_SharedDpCounter)
                  {
                     const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                     // No error expected. The Datapools must have the same conditions
                     tgl_assert(this->RemoveDataPoolListDataSet(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                                oru32_DataPoolListIndex,
                                                                oru32_DataPoolListDataSetIndex, false) == C_NO_ERR);
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
/*! \brief   Move data set in node data pool list

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    Data pool list index
   \param[in]  oru32_SourceIndex          Source data set index
   \param[in]  oru32_TargetIndex          Target data set index
   \param[in]  oq_HandleSharedDatapools   Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::MoveDataPoolListDataSet(const uint32 & oru32_NodeIndex,
                                                        const uint32 & oru32_DataPoolIndex,
                                                        const uint32 & oru32_DataPoolListIndex,
                                                        const uint32 & oru32_SourceIndex,
                                                        const uint32 & oru32_TargetIndex,
                                                        const bool oq_HandleSharedDatapools)
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
               const C_OSCNodeDataPoolDataSet c_Data = rc_OSCList.c_DataSets[oru32_SourceIndex];
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
                  const C_OSCNodeDataPoolContent c_Data = rc_DataElement.c_DataSetValues[oru32_SourceIndex];
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
            Q_EMIT (this->SigSyncNodeDataPoolListDataSetMoved(oru32_NodeIndex, oru32_DataPoolIndex,
                                                              oru32_DataPoolListIndex,
                                                              oru32_SourceIndex, oru32_TargetIndex));

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32 u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size(); ++u32_SharedDpCounter)
                  {
                     const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                     // No error expected. The Datapools must have the same conditions
                     tgl_assert(this->MoveDataPoolListDataSet(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                              oru32_DataPoolListIndex,
                                                              oru32_SourceIndex, oru32_TargetIndex, false) == C_NO_ERR);
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
/*! \brief   Get OSC data pool list

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    Data pool list index

   \return
   Found: Pointer to OSC data pool list
   Else:  NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolList * C_PuiSdHandlerNodeLogic::GetOSCDataPoolList(const uint32 & oru32_NodeIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get UI data pool list

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    Data pool list index

   \return
   Found: Pointer to UI data pool list
   Else:  NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNodeDataPoolList * C_PuiSdHandlerNodeLogic::GetUIDataPoolList(const uint32 & oru32_NodeIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move list in node data pool

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_SourceIndex          Source list index
   \param[in]  oru32_TargetIndex          Target list index
   \param[in]  oq_HandleSharedDatapools   Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::MoveDataPoolList(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                                 const uint32 & oru32_SourceIndex, const uint32 & oru32_TargetIndex,
                                                 const bool oq_HandleSharedDatapools)
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
         rc_OSCDataPool.MoveList(oru32_SourceIndex, oru32_TargetIndex);
         if ((oru32_SourceIndex < rc_UIDataPool.c_DataPoolLists.size()) &&
             (oru32_TargetIndex < rc_UIDataPool.c_DataPoolLists.size()))
         {
            //Copy
            const C_PuiSdNodeDataPoolList c_Data = rc_UIDataPool.c_DataPoolLists[oru32_SourceIndex];
            //Erase
            rc_UIDataPool.c_DataPoolLists.erase(rc_UIDataPool.c_DataPoolLists.begin() + oru32_SourceIndex);
            //Insert
            rc_UIDataPool.c_DataPoolLists.insert(rc_UIDataPool.c_DataPoolLists.begin() + oru32_TargetIndex, c_Data);

            //Synchronization engine
            Q_EMIT (this->SigSyncNodeDataPoolListMoved(oru32_NodeIndex, oru32_DataPoolIndex, oru32_SourceIndex,
                                                       oru32_TargetIndex));

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32 u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size(); ++u32_SharedDpCounter)
                  {
                     const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                     // No error expected. The Datapools must have the same conditions
                     tgl_assert(this->MoveDataPoolList(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                       oru32_SourceIndex, oru32_TargetIndex, false) == C_NO_ERR);
                  }
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
/*! \brief   Check if any list uses the provided name

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  orc_Name                         Data pool list name to check for
   \param[in]  opu32_DataPoolListIndexToSkip    Optional parameter to skip one index
                                                (Use-case: skip current data pool list to avoid conflict with itself)

   \return
   true  Available
   false Already in use
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerNodeLogic::CheckNodeDataPoolListNameAvailable(const uint32 & oru32_NodeIndex,
                                                                 const uint32 & oru32_DataPoolIndex,
                                                                 const C_SCLString & orc_Name,
                                                                 const uint32 * const opu32_DataPoolListIndexToSkip)
const
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if any list data set uses the provided name

   \param[in]  oru32_NodeIndex                        Node index
   \param[in]  oru32_DataPoolIndex                    Data pool index
   \param[in]  oru32_ListIndex                        List index
   \param[in]  orc_Name                               Data pool list data set name to check for
   \param[in]  opu32_DataPoolListDataSetIndexToSkip   Optional parameter to skip one index
                                                      (Use-case: skip current data pool list data set
                                                      to avoid conflict with itself)

   \return
   true  Available
   false Already in use
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerNodeLogic::CheckNodeDataPoolListDataSetNameAvailable(const uint32 & oru32_NodeIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reserve data pool list elements section

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    Data pool list index
   \param[in]  oru32_AdditionalElements   Number of additional elements to reserve for
   \param[in]  oq_HandleSharedDatapools   Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::ReserveDataPoolListElements(const uint32 & oru32_NodeIndex,
                                                            const uint32 & oru32_DataPoolIndex,
                                                            const uint32 & oru32_DataPoolListIndex,
                                                            const uint32 & oru32_AdditionalElements,
                                                            const bool oq_HandleSharedDatapools)
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
            //Reserve
            rc_UIList.c_DataPoolListElements.reserve(
               static_cast<std::vector<C_OSCNodeDataPoolListElement>::size_type>(
                  rc_UIList.c_DataPoolListElements.size() +
                  oru32_AdditionalElements));
            rc_OSCList.c_Elements.reserve(
               static_cast<std::vector<C_PuiSdNodeDataPoolListElement>::size_type>(rc_OSCList
                                                                                   .
                                                                                   c_Elements
                                                                                   .size() +
                                                                                   oru32_AdditionalElements));

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32 u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                       ++u32_SharedDpCounter)
                  {
                     const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                     // No error expected. The Datapools must have the same conditions
                     tgl_assert(this->ReserveDataPoolListElements(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                                  oru32_DataPoolListIndex,
                                                                  oru32_AdditionalElements, false) == C_NO_ERR);
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
/*! \brief   Insert data pool list elements at specific position

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          Data pool list index
   \param[in]  oru32_DataPoolListElementIndex   Data pool list element index
   \param[in]  orc_OSCContent                   OSC data pool list element content
                                                (name might be modified by this function if not unique)
   \param[in]  orc_UIContent                    UI data pool list element content
   \param[in]  oq_HandleSharedDatapools         Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::InsertDataPoolListElement(const uint32 & oru32_NodeIndex,
                                                          const uint32 & oru32_DataPoolIndex,
                                                          const uint32 & oru32_DataPoolListIndex,
                                                          const uint32 & oru32_DataPoolListElementIndex,
                                                          const C_OSCNodeDataPoolListElement & orc_OSCContent,
                                                          const C_PuiSdNodeDataPoolListElement & orc_UIContent,
                                                          const bool oq_HandleSharedDatapools)
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
            if (oru32_DataPoolListElementIndex <= rc_UIList.c_DataPoolListElements.size())
            {
               //Unique name
               C_OSCNodeDataPoolListElement c_NodeDataPoolListElement = orc_OSCContent;
               c_NodeDataPoolListElement.c_Name =
                  C_Uti::h_GetUniqueName(this->m_GetExistingNodeDataPoolListVariableNames(oru32_NodeIndex,
                                                                                          oru32_DataPoolIndex,
                                                                                          oru32_DataPoolListIndex),
                                         c_NodeDataPoolListElement.c_Name);
               //Adapt required fields
               if (rc_OSCDataPool.q_IsSafety == true)
               {
                  c_NodeDataPoolListElement.e_Access = C_OSCNodeDataPoolListElement::eACCESS_RO;
               }
               //Insert
               rc_UIList.c_DataPoolListElements.insert(
                  rc_UIList.c_DataPoolListElements.begin() + oru32_DataPoolListElementIndex,
                  orc_UIContent);
               rc_OSCList.c_Elements.insert(rc_OSCList.c_Elements.begin() + oru32_DataPoolListElementIndex,
                                            c_NodeDataPoolListElement);

               rc_OSCNode.RecalculateAddress();

               //Synchronization engine
               Q_EMIT (this->SigSyncNodeDataPoolListElementAdded(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                 oru32_DataPoolListIndex,
                                                                 oru32_DataPoolListElementIndex));

               // Handle shared Datapools
               if (oq_HandleSharedDatapools == true)
               {
                  std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

                  if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                           c_SharedDatapools) == true)
                  {
                     uint32 u32_SharedDpCounter;

                     // Synchronize with the shared Datapools
                     for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                          ++u32_SharedDpCounter)
                     {
                        const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                        // No error expected. The Datapools must have the same conditions
                        tgl_assert(this->InsertDataPoolListElement(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                                   oru32_DataPoolListIndex,
                                                                   oru32_DataPoolListElementIndex,
                                                                   orc_OSCContent, orc_UIContent, false) == C_NO_ERR);
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

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove data pool list element

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          Data pool list index
   \param[in]  oru32_DataPoolListElementIndex   Data pool list element index
   \param[in]  oq_HandleSharedDatapools         Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::RemoveDataPoolListElement(const uint32 & oru32_NodeIndex,
                                                          const uint32 & oru32_DataPoolIndex,
                                                          const uint32 & oru32_DataPoolListIndex,
                                                          const uint32 & oru32_DataPoolListElementIndex,
                                                          const bool oq_HandleSharedDatapools)
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
               //Synchronization engine (First!)
               Q_EMIT (this->SigSyncNodeDataPoolListElementAboutToBeDeleted(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                            oru32_DataPoolListIndex,
                                                                            oru32_DataPoolListElementIndex));
               rc_UIList.c_DataPoolListElements.erase(rc_UIList.c_DataPoolListElements.begin() +
                                                      oru32_DataPoolListElementIndex);
               rc_OSCList.c_Elements.erase(rc_OSCList.c_Elements.begin() + oru32_DataPoolListElementIndex);

               rc_OSCNode.RecalculateAddress();

               // Handle shared Datapools
               if (oq_HandleSharedDatapools == true)
               {
                  std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

                  if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                           c_SharedDatapools) == true)
                  {
                     uint32 u32_SharedDpCounter;

                     // Synchronize with the shared Datapools
                     for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                          ++u32_SharedDpCounter)
                     {
                        const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                        // No error expected. The Datapools must have the same conditions
                        tgl_assert(this->RemoveDataPoolListElement(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                                   oru32_DataPoolListIndex,
                                                                   oru32_DataPoolListElementIndex,
                                                                   false) == C_NO_ERR);
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

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data pool list element

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          Data pool list index
   \param[in]  oru32_DataPoolListElementIndex   Data pool list element index
   \param[in]  orc_OSCContent                   OSC data pool list element content
   \param[in]  orc_UIContent                    UI data pool list element content
   \param[in]  oq_HandleSharedDatapools         Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetDataPoolListElement(const uint32 & oru32_NodeIndex,
                                                       const uint32 & oru32_DataPoolIndex,
                                                       const uint32 & oru32_DataPoolListIndex,
                                                       const uint32 & oru32_DataPoolListElementIndex,
                                                       const C_OSCNodeDataPoolListElement & orc_OSCContent,
                                                       const C_PuiSdNodeDataPoolListElement & orc_UIContent,
                                                       const bool oq_HandleSharedDatapools)
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
                                                                     orc_OSCContent.GetArraySize(),
                                                                     orc_UIContent.q_InterpretAsString);
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

               // Handle shared Datapools
               if (oq_HandleSharedDatapools == true)
               {
                  std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

                  if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                           c_SharedDatapools) == true)
                  {
                     uint32 u32_SharedDpCounter;

                     // Synchronize with the shared Datapools
                     for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                          ++u32_SharedDpCounter)
                     {
                        const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                        // No error expected. The Datapools must have the same conditions
                        tgl_assert(this->SetDataPoolListElement(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                                oru32_DataPoolListIndex,
                                                                oru32_DataPoolListElementIndex, orc_OSCContent,
                                                                orc_UIContent,
                                                                false) == C_NO_ERR);
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

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data pool list element

   \param[in]   oru32_NodeIndex                 Node index
   \param[in]   oru32_DataPoolIndex             Data pool index
   \param[in]   oru32_DataPoolListIndex         Data pool list index
   \param[in]   oru32_DataPoolListElementIndex  Data pool list element index
   \param[out]  orc_OSCContent                  OSC data pool list element content
   \param[out]  orc_UIContent                   UI data pool list element content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::GetDataPoolListElement(const uint32 & oru32_NodeIndex,
                                                       const uint32 & oru32_DataPoolIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize data element name & comment

   \param[in]      ore_Type         Data pool type
   \param[in]      oq_IsSafety      Flag if datapool is safety datapool
   \param[in,out]  orc_OSCElement   Data element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::h_InitDataElement(const C_OSCNodeDataPool::E_Type & ore_Type, const bool oq_IsSafety,
                                                C_OSCNodeDataPoolListElement & orc_OSCElement)
{
   const QString c_Type =
      C_PuiSdHandler::h_AutomaticCStringAdaptation(C_PuiSdHandlerNodeLogic::h_GetElementTypeName(ore_Type));

   //Translation: 1: Data element type
   orc_OSCElement.c_Name = static_cast<QString>(static_cast<QString>("New%1").arg(c_Type)).toStdString().c_str();
   //Translation: 1: Data element type
   orc_OSCElement.c_Comment =
      static_cast<QString>(static_cast<QString>("%1 description").arg(c_Type)).toStdString().c_str();
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data pool type dependent element name

   \param[in]  ore_Type    Data pool type

   \return
   Datapool type element name as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdHandlerNodeLogic::h_GetElementTypeName(const C_OSCNodeDataPool::E_Type & ore_Type)
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
   case C_OSCNodeDataPool::eHALC:
   case C_OSCNodeDataPool::eHALC_NVM:
      c_Retval = "HAL Data Element";
      break;
   default:
      c_Retval = "Data Element";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data pool list element min array value

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          Data pool list index
   \param[in]  oru32_DataPoolListElementIndex   Data pool list element index
   \param[in]  oru32_ArrayIndex                 Array index
   \param[in]  orc_OSCContent                   New value (Expected: No array & same type as in element)
   \param[in]  oq_HandleSharedDatapools         Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetDataPoolListElementMinArray(const uint32 & oru32_NodeIndex,
                                                               const uint32 & oru32_DataPoolIndex,
                                                               const uint32 & oru32_DataPoolListIndex,
                                                               const uint32 & oru32_DataPoolListElementIndex,
                                                               const uint32 & oru32_ArrayIndex,
                                                               const C_OSCNodeDataPoolContent & orc_OSCContent,
                                                               const bool oq_HandleSharedDatapools)
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
                  default:
                     tgl_assert(false);
                     break;
                  }

                  // Handle shared Datapools
                  if (oq_HandleSharedDatapools == true)
                  {
                     std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

                     if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                              c_SharedDatapools) == true)
                     {
                        uint32 u32_SharedDpCounter;

                        // Synchronize with the shared Datapools
                        for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                             ++u32_SharedDpCounter)
                        {
                           const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                           // No error expected. The Datapools must have the same conditions
                           tgl_assert(this->SetDataPoolListElementMinArray(rc_DpId.u32_NodeIndex,
                                                                           rc_DpId.u32_DataPoolIndex,
                                                                           oru32_DataPoolListIndex,
                                                                           oru32_DataPoolListElementIndex,
                                                                           oru32_ArrayIndex, orc_OSCContent,
                                                                           false) == C_NO_ERR);
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
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data pool list element max array value

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          Data pool list index
   \param[in]  oru32_DataPoolListElementIndex   Data pool list element index
   \param[in]  oru32_ArrayIndex                 Array index
   \param[in]  orc_OSCContent                   New value (Expected: No array & same type as in element)
   \param[in]  oq_HandleSharedDatapools         Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetDataPoolListElementMaxArray(const uint32 & oru32_NodeIndex,
                                                               const uint32 & oru32_DataPoolIndex,
                                                               const uint32 & oru32_DataPoolListIndex,
                                                               const uint32 & oru32_DataPoolListElementIndex,
                                                               const uint32 & oru32_ArrayIndex,
                                                               const C_OSCNodeDataPoolContent & orc_OSCContent,
                                                               const bool oq_HandleSharedDatapools)
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
                  default:
                     tgl_assert(false);
                     break;
                  }

                  // Handle shared Datapools
                  if (oq_HandleSharedDatapools == true)
                  {
                     std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

                     if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                              c_SharedDatapools) == true)
                     {
                        uint32 u32_SharedDpCounter;

                        // Synchronize with the shared Datapools
                        for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                             ++u32_SharedDpCounter)
                        {
                           const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                           // No error expected. The Datapools must have the same conditions
                           tgl_assert(this->SetDataPoolListElementMaxArray(rc_DpId.u32_NodeIndex,
                                                                           rc_DpId.u32_DataPoolIndex,
                                                                           oru32_DataPoolListIndex,
                                                                           oru32_DataPoolListElementIndex,
                                                                           oru32_ArrayIndex, orc_OSCContent,
                                                                           false) == C_NO_ERR);
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
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data pool list element data set array value

   \param[in]  oru32_NodeIndex                        Node index
   \param[in]  oru32_DataPoolIndex                    Data pool index
   \param[in]  oru32_DataPoolListIndex                Data pool list index
   \param[in]  oru32_DataPoolListElementIndex         Data pool list element index
   \param[in]  oru32_DataPoolListElementDataSetIndex  Data pool list element index
   \param[in]  oru32_ArrayIndex                       Array index
   \param[in]  orc_OSCContent                         New value (Expected: No array & same type as in element)
   \param[in]  oq_HandleSharedDatapools               Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetDataPoolListElementDataSetArray(const uint32 & oru32_NodeIndex,
                                                                   const uint32 & oru32_DataPoolIndex,
                                                                   const uint32 & oru32_DataPoolListIndex,
                                                                   const uint32 & oru32_DataPoolListElementIndex,
                                                                   const uint32 & oru32_DataPoolListElementDataSetIndex,
                                                                   const uint32 & oru32_ArrayIndex,
                                                                   const C_OSCNodeDataPoolContent & orc_OSCContent,
                                                                   const bool oq_HandleSharedDatapools)
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
                     default:
                        tgl_assert(false);
                        break;
                     }

                     // Handle shared Datapools
                     if (oq_HandleSharedDatapools == true)
                     {
                        std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

                        if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                                 c_SharedDatapools) == true)
                        {
                           uint32 u32_SharedDpCounter;

                           // Synchronize with the shared Datapools
                           for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                                ++u32_SharedDpCounter)
                           {
                              const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                              // No error expected. The Datapools must have the same conditions
                              tgl_assert(this->SetDataPoolListElementDataSetArray(rc_DpId.u32_NodeIndex,
                                                                                  rc_DpId.u32_DataPoolIndex,
                                                                                  oru32_DataPoolListIndex,
                                                                                  oru32_DataPoolListElementIndex,
                                                                                  oru32_DataPoolListElementDataSetIndex,
                                                                                  oru32_ArrayIndex, orc_OSCContent,
                                                                                  false) == C_NO_ERR);
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
/*! \brief   Set data pool list element NVM value

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          Data pool list index
   \param[in]  oru32_DataPoolListElementIndex   Data pool list element index
   \param[in]  orc_OSCContent                   New value

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetDataPoolListElementNVMValue(const uint32 & oru32_NodeIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data pool list element NVM value changed flag

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          Data pool list index
   \param[in]  oru32_DataPoolListElementIndex   Data pool list element index
   \param[in]  oq_NvmValueChanged               New flag

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::SetDataPoolListElementNVMValueChanged(const uint32 & oru32_NodeIndex,
                                                                      const uint32 & oru32_DataPoolIndex,
                                                                      const uint32 & oru32_DataPoolListIndex,
                                                                      const uint32 & oru32_DataPoolListElementIndex,
                                                                      const bool oq_NvmValueChanged)
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
               rc_Element.q_NvMValueChanged = oq_NvmValueChanged;
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
/*! \brief   Get OSC data pool list element

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          Data pool list index
   \param[in]  oru32_DataPoolListElementIndex   Data pool list element index

   \return
   Found: Pointer to OSC data pool list element
   Else:  NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolListElement * C_PuiSdHandlerNodeLogic::GetOSCDataPoolListElement(const uint32 & oru32_NodeIndex,
                                                                                        const uint32 & oru32_DataPoolIndex, const uint32 & oru32_DataPoolListIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC data pool list element

   \param[in]  orc_Id   Aggregated identification indices

   \return
   Found: Pointer to OSC data pool list element
   Else:  NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolListElement * C_PuiSdHandlerNodeLogic::GetOSCDataPoolListElement(
   const C_OSCNodeDataPoolListElementId & orc_Id) const
{
   return this->GetOSCDataPoolListElement(orc_Id.u32_NodeIndex, orc_Id.u32_DataPoolIndex, orc_Id.u32_ListIndex,
                                          orc_Id.u32_ElementIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get UI data pool list element

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          Data pool list index
   \param[in]  oru32_DataPoolListElementIndex   Data pool list element index

   \return
   Found: Pointer to UI data pool list element
   Else:  NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNodeDataPoolListElement * C_PuiSdHandlerNodeLogic::GetUIDataPoolListElement(const uint32 & oru32_NodeIndex,
                                                                                         const uint32 & oru32_DataPoolIndex, const uint32 & oru32_DataPoolListIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move element in node data pool list

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    Data pool list index
   \param[in]  oru32_SourceIndex          Source list index
   \param[in]  oru32_TargetIndex          Target list index
   \param[in]  oq_HandleSharedDatapools   Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::MoveDataPoolListElement(const uint32 & oru32_NodeIndex,
                                                        const uint32 & oru32_DataPoolIndex,
                                                        const uint32 & oru32_DataPoolListIndex,
                                                        const uint32 & oru32_SourceIndex,
                                                        const uint32 & oru32_TargetIndex,
                                                        const bool oq_HandleSharedDatapools)
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
            if ((oru32_SourceIndex < rc_UIList.c_DataPoolListElements.size()) &&
                (oru32_TargetIndex < rc_UIList.c_DataPoolListElements.size()))
            {
               //Copy
               const C_PuiSdNodeDataPoolListElement c_Data = rc_UIList.c_DataPoolListElements[oru32_SourceIndex];
               rc_OSCList.MoveElement(oru32_SourceIndex, oru32_TargetIndex);
               //Erase
               rc_UIList.c_DataPoolListElements.erase(
                  rc_UIList.c_DataPoolListElements.begin() + oru32_SourceIndex);
               //Insert
               rc_UIList.c_DataPoolListElements.insert(
                  rc_UIList.c_DataPoolListElements.begin() + oru32_TargetIndex, c_Data);

               //Synchronization engine
               Q_EMIT this->SigSyncNodeDataPoolListElementMoved(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                oru32_DataPoolListIndex,
                                                                oru32_SourceIndex, oru32_TargetIndex);

               // Handle shared Datapools
               if (oq_HandleSharedDatapools == true)
               {
                  std::vector<C_OSCNodeDataPoolId> c_SharedDatapools;

                  if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                           c_SharedDatapools) == true)
                  {
                     uint32 u32_SharedDpCounter;

                     // Synchronize with the shared Datapools
                     for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                          ++u32_SharedDpCounter)
                     {
                        const C_OSCNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                        // No error expected. The Datapools must have the same conditions
                        tgl_assert(this->MoveDataPoolListElement(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                                 oru32_DataPoolListIndex,
                                                                 oru32_SourceIndex, oru32_TargetIndex,
                                                                 false) == C_NO_ERR);
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if any list element uses the provided name

   \param[in]  oru32_NodeIndex                        Node index
   \param[in]  oru32_DataPoolIndex                    Data pool index
   \param[in]  oru32_ListIndex                        List index
   \param[in]  orc_Name                               Data pool list element name to check for
   \param[in]  opu32_DataPoolListElementIndexToSkip   Optional parameter to skip one index
                                                      (Use-case: skip current data pool list element
                                                      to avoid conflict with itself)

   \return
   true  Available
   false Already in use
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerNodeLogic::CheckNodeDataPoolListElementNameAvailable(const uint32 & oru32_NodeIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogic(QObject * const opc_Parent) :
   C_PuiSdHandlerData(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pointers to all currently registered node names

   The sub node part in the names will be removed. In case of a squad node, only the base name will be used

   \return
   Vector of pointers to all currently registered node names
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<C_SCLString, bool> C_PuiSdHandlerNodeLogic::m_GetExistingNodeNames(void) const
{
   std::map<C_SCLString, bool> c_Retval;
   for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      uint32 u32_GroupIndex;
      if (C_OSCNodeSquad::h_CheckIsMultiDevice(u32_ItNode, this->mc_CoreDefinition.c_NodeSquads, &u32_GroupIndex))
      {
         // Squad sub node. Only use base name
         tgl_assert(u32_GroupIndex < this->mc_CoreDefinition.c_NodeSquads.size());
         if (u32_GroupIndex < this->mc_CoreDefinition.c_NodeSquads.size())
         {
            const C_SCLString & rc_CurrentName = this->mc_CoreDefinition.c_NodeSquads[u32_GroupIndex].c_BaseName;
            c_Retval[rc_CurrentName] = true;
         }
      }
      else
      {
         // Normal node
         const C_SCLString & rc_CurrentName = this->mc_CoreDefinition.c_Nodes[u32_ItNode].c_Properties.c_Name;
         c_Retval[rc_CurrentName] = true;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pointers to all currently registered node application names

   \param[in]  oru32_NodeIndex   Node index

   \return
   Vector of pointers to all currently registered node application names
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<C_SCLString,
         bool> C_PuiSdHandlerNodeLogic::m_GetExistingNodeApplicationNames(const uint32 & oru32_NodeIndex) const
{
   std::map<C_SCLString, bool> c_Retval;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(oru32_NodeIndex);
   if (pc_Node != NULL)
   {
      for (uint32 u32_ItApplication = 0; u32_ItApplication < pc_Node->c_Applications.size(); ++u32_ItApplication)
      {
         c_Retval[pc_Node->c_Applications[u32_ItApplication].c_Name] = true;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pointers to all currently registered data pool names contained in the specified node

   \param[in]  oru32_NodeIndex   Node index

   \return
   Vector of pointers to all currently registered data pool names contained in the specified node
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<C_SCLString,
         bool> C_PuiSdHandlerNodeLogic::m_GetExistingNodeDataPoolNames(const uint32 & oru32_NodeIndex) const
{
   std::map<C_SCLString, bool> c_Retval;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(oru32_NodeIndex);
   if (pc_Node != NULL)
   {
      for (uint32 u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
      {
         c_Retval[pc_Node->c_DataPools[u32_ItDataPool].c_Name] = true;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pointers to all currently registered list names contained in the specified node data pool

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Node data pool index

   \return
   Vector of pointers to all currently registered list names contained in the specified node data pool
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<C_SCLString, bool> C_PuiSdHandlerNodeLogic::m_GetExistingNodeDataPoolListNames(const uint32 & oru32_NodeIndex,
                                                                                        const uint32 & oru32_DataPoolIndex)
const
{
   std::map<C_SCLString, bool> c_Retval;
   const C_OSCNodeDataPool * const pc_NodeDataPool = this->GetOSCDataPool(oru32_NodeIndex, oru32_DataPoolIndex);
   if (pc_NodeDataPool != NULL)
   {
      for (uint32 u32_ItDataPoolList = 0; u32_ItDataPoolList < pc_NodeDataPool->c_Lists.size(); ++u32_ItDataPoolList)
      {
         c_Retval[pc_NodeDataPool->c_Lists[u32_ItDataPoolList].c_Name] = true;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pointers to all currently registered data set names contained in the specified node data pool list

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Node data pool index
   \param[in]  oru32_DataPoolListIndex    Node data pool list index

   \return
   Vector of pointers to all currently registered data set names contained in the specified node data pool list
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<C_SCLString, bool> C_PuiSdHandlerNodeLogic::m_GetExistingNodeDataPoolListDataSetNames(
   const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex, const uint32 & oru32_DataPoolListIndex)
const
{
   std::map<C_SCLString, bool> c_Retval;
   const C_OSCNodeDataPoolList * const pc_NodeDataPoolList = this->GetOSCDataPoolList(oru32_NodeIndex,
                                                                                      oru32_DataPoolIndex,
                                                                                      oru32_DataPoolListIndex);
   if (pc_NodeDataPoolList != NULL)
   {
      for (uint32 u32_ItDataPoolListElement = 0; u32_ItDataPoolListElement < pc_NodeDataPoolList->c_DataSets.size();
           ++u32_ItDataPoolListElement)
      {
         c_Retval[pc_NodeDataPoolList->c_DataSets[u32_ItDataPoolListElement].c_Name] = true;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pointers to all currently registered variable names contained in the specified node data pool list

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Node data pool index
   \param[in]  oru32_DataPoolListIndex    Node data pool list index

   \return
   Vector of pointers to all currently registered variable names contained in the specified node data pool list
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<C_SCLString, bool> C_PuiSdHandlerNodeLogic::m_GetExistingNodeDataPoolListVariableNames(
   const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex, const uint32 & oru32_DataPoolListIndex)
const
{
   std::map<C_SCLString, bool> c_Retval;
   const C_OSCNodeDataPoolList * const pc_NodeDataPoolList = this->GetOSCDataPoolList(oru32_NodeIndex,
                                                                                      oru32_DataPoolIndex,
                                                                                      oru32_DataPoolListIndex);
   if (pc_NodeDataPoolList != NULL)
   {
      for (uint32 u32_ItDataPoolListElement = 0; u32_ItDataPoolListElement < pc_NodeDataPoolList->c_Elements.size();
           ++u32_ItDataPoolListElement)
      {
         c_Retval[pc_NodeDataPoolList->c_Elements[u32_ItDataPoolListElement].c_Name] = true;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition application change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::m_SyncNodeApplicationAdded(const uint32 ou32_NodeIndex,
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
            if (static_cast<uint32>(rc_DataPool.s32_RelatedDataBlockIndex) >= ou32_ApplicationIndex)
            {
               ++rc_DataPool.s32_RelatedDataBlockIndex;
            }
         }
      }
      //File generation settings
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition application change

   \param[in]  ou32_NodeIndex                Node index
   \param[in]  ou32_ApplicationSourceIndex   Application source index
   \param[in]  ou32_ApplicationTargetIndex   Application target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::m_SyncNodeApplicationMoved(const uint32 ou32_NodeIndex,
                                                         const uint32 ou32_ApplicationSourceIndex,
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
      //File generation settings
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition application change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::m_SyncNodeApplicationAboutToBeDeleted(const uint32 ou32_NodeIndex,
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
      //File generation settings
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle automated programmable application assignment for file generation settings

   \param[in]  ou32_NodeIndex    Node index (ID)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerNodeLogic::m_HandleNodeAutomatedProgrammableApplicationUpdate(const uint32 ou32_NodeIndex)
{
   sint32 s32_Retval;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(ou32_NodeIndex);

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize com data pool

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  orq_AllowDataAdaptation    Optional parameter to disable automatic data adaptation
   \param[in]  ore_ComProtocolType        Optional parameter for com protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::m_SetUpComDataPool(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
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
         C_OSCNodeDataPool & rc_OSCDataPool = rc_OSCNode.c_DataPools[oru32_DataPoolIndex];
         //Check if com data pool
         if (rc_OSCDataPool.e_Type == C_OSCNodeDataPool::eCOM)
         {
            C_PuiSdNodeDataPool & rc_UIDataPool = rc_UINode.c_UIDataPools[oru32_DataPoolIndex];
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
               const C_PuiSdNodeCanMessageContainer c_UiMessageContainer;
               C_OSCCanProtocol c_Protocol;
               C_OSCCanMessageContainer c_New;
               uint32 u32_CanCounter = 0U;

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
                     uint32 u32_ComProtocolCounter;

                     c_New.q_IsComProtocolUsedByInterface = false;

                     // Check already existing protocols of same type if the are used by the interface
                     for (u32_ComProtocolCounter = 0U; u32_ComProtocolCounter < rc_OSCNode.c_ComProtocols.size();
                          ++u32_ComProtocolCounter)
                     {
                        const C_OSCCanProtocol & rc_Protocol = rc_OSCNode.c_ComProtocols[u32_ComProtocolCounter];

                        if ((ore_ComProtocolType == rc_Protocol.e_Type) &&
                            (u32_CanCounter < rc_Protocol.c_ComMessages.size()) &&
                            (rc_Protocol.c_ComMessages[u32_CanCounter].q_IsComProtocolUsedByInterface == true))
                        {
                           // The same protocol type is already used on the same CAN interface
                           c_New.q_IsComProtocolUsedByInterface = true;
                           break;
                        }
                     }

                     c_Protocol.c_ComMessages.push_back(c_New);
                     c_UiProtocol.c_ComMessages.push_back(c_UiMessageContainer);

                     ++u32_CanCounter;
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
                  C_PuiSdHandlerNodeLogic::h_InitDataElement(rc_OSCDataPool.e_Type, rc_OSCDataPool.q_IsSafety,
                                                             rc_List.c_Elements[0]);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up com data pool

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::m_CleanUpComDataPool(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex)
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Analyzing the NVM memory areas of the node and its Datapools

   All NVM based Datapools are considered. Overlapping of areas or not enough space are not handled as error here.
   Three different scenarios can occur:
   c_DatapoolNames has
   * 0 elements:  Free area
   * 1 element:   One Datapool is in this area
   * >1 elements: At least two Datapools are in this area and has overlapping memory addresses

   If the flag q_InRange is set to false of at least one area, the NVM memory of the node is not big enough for the
   current configuration.

   \param[in]   orc_Node   Node
   \param[out]  orc_Areas  All detected areas
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::mh_GetNodeNvmDataPoolAreas(const C_OSCNode & orc_Node,
                                                         std::vector<C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea> & orc_Areas)
{
   const uint32 u32_NvmSize =
      orc_Node.pc_DeviceDefinition->c_SubDevices[orc_Node.u32_SubDeviceIndex].u32_UserEepromSizeBytes;
   uint32 u32_DatapoolCounter;
   uint32 u32_AreaCounter;

   // Reserve the lowest potential number of areas
   orc_Areas.reserve(orc_Node.c_DataPools.size());

   // Getting all areas
   for (u32_DatapoolCounter = 0U; u32_DatapoolCounter < orc_Node.c_DataPools.size(); ++u32_DatapoolCounter)
   {
      const stw_opensyde_core::C_OSCNodeDataPool * const pc_Datapool = &orc_Node.c_DataPools[u32_DatapoolCounter];

      // Checking each NVM based Datapool
      if ((pc_Datapool->e_Type == C_OSCNodeDataPool::eNVM) ||
          (pc_Datapool->e_Type == C_OSCNodeDataPool::eHALC_NVM))
      {
         // Prepare the original Datapool area
         C_PuiSdHandlerNodeLogicNvmArea c_CurrentDatapoolArea;
         c_CurrentDatapoolArea.u32_StartAddress = pc_Datapool->u32_NvMStartAddress;
         c_CurrentDatapoolArea.u32_Size = pc_Datapool->u32_NvMSize;
         c_CurrentDatapoolArea.c_DataPoolIndexes.push_back(u32_DatapoolCounter);

         // Compare and adapt the area against the already registered areas
         C_PuiSdHandlerNodeLogic::mh_AddAndAdaptNvmDataPoolArea(c_CurrentDatapoolArea, orc_Areas);
      }
   }

   // Check if the areas fits into the NVM memory
   for (u32_AreaCounter = 0U; u32_AreaCounter < orc_Areas.size(); ++u32_AreaCounter)
   {
      C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea & rc_Area = orc_Areas[u32_AreaCounter];
      rc_Area.q_InRange = ((rc_Area.u32_StartAddress + rc_Area.u32_Size) <= u32_NvmSize);
   }

   // Check if a gap exists at the beginning
   if (orc_Areas.size() > 0)
   {
      if (orc_Areas[0].u32_StartAddress > 0)
      {
         // Gap found
         C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea c_GapArea;

         c_GapArea.u32_StartAddress = 0U;
         c_GapArea.u32_Size = orc_Areas[0].u32_StartAddress;
         orc_Areas.push_back(c_GapArea);

         std::sort(orc_Areas.begin(), orc_Areas.end());
      }
   }

   // Filling the gaps with "free" areas
   for (u32_AreaCounter = 1U; u32_AreaCounter < orc_Areas.size(); ++u32_AreaCounter)
   {
      const C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea & rc_FirstArea =
         orc_Areas[static_cast<uint32>(u32_AreaCounter - 1U)];
      const C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea & rc_SecondArea = orc_Areas[u32_AreaCounter];

      if ((rc_SecondArea.u32_StartAddress > 0U) &&
          ((rc_FirstArea.u32_StartAddress + rc_FirstArea.u32_Size) < (rc_SecondArea.u32_StartAddress - 1U)))
      {
         // Gap found
         C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea c_GapArea;

         c_GapArea.u32_StartAddress = rc_FirstArea.u32_StartAddress + rc_FirstArea.u32_Size;
         c_GapArea.u32_Size = rc_SecondArea.u32_StartAddress - c_GapArea.u32_StartAddress;
         orc_Areas.push_back(c_GapArea);

         std::sort(orc_Areas.begin(), orc_Areas.end());
      }
   }

   // Check the last potential gap till the end
   if (orc_Areas.size() > 0)
   {
      const C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea & rc_LastArea = orc_Areas[orc_Areas.size() - 1];

      if ((u32_NvmSize > 0U) &&
          ((rc_LastArea.u32_StartAddress + rc_LastArea.u32_Size) < (u32_NvmSize - 1U)))
      {
         C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea c_GapArea;

         c_GapArea.u32_StartAddress = rc_LastArea.u32_StartAddress + rc_LastArea.u32_Size;
         c_GapArea.u32_Size = u32_NvmSize - c_GapArea.u32_StartAddress;
         orc_Areas.push_back(c_GapArea);

         std::sort(orc_Areas.begin(), orc_Areas.end());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking and adapting a new NVM area to the area result

   orc_Areas must be sorted for start address.

   Different conditions:
      1: No Overlap
      2: Overlap
      2.1: The current block starts before an other block
      2.2: The current block starts in an other block
      2.3: The current block has the same start address
      2.4: The current block ends after it
      2.5: The current block ends in it
      2.6: The current block ends on the same address

   In case of condition 2.4 the new created block for the area after the detected block,
   a recursive check must be run.
   In all other conditions 2.x the loop can be aborted due to the sorting.

   \param[in]      orc_CurrentArea  NVM memory area for adding to orc_Areas
   \param[in,out]  orc_Areas        All detected areas
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::mh_AddAndAdaptNvmDataPoolArea(
   C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea & orc_CurrentArea,
   std::vector<C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea> & orc_Areas)
{
   uint32 u32_AreaCounter;
   bool q_OverlapDetected = false;

   for (u32_AreaCounter = 0U; u32_AreaCounter < orc_Areas.size(); ++u32_AreaCounter)
   {
      C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea & rc_ComparedArea = orc_Areas[u32_AreaCounter];
      C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea c_AreaBefore;
      bool q_AddAreaBefore = false;

      // Check for 2.1
      if ((orc_CurrentArea.u32_StartAddress < rc_ComparedArea.u32_StartAddress) &&
          ((orc_CurrentArea.u32_StartAddress + orc_CurrentArea.u32_Size) > rc_ComparedArea.u32_StartAddress))
      {
         // Create the area "before" based on the current area and add it to the result
         c_AreaBefore = orc_CurrentArea;
         c_AreaBefore.u32_Size = (rc_ComparedArea.u32_StartAddress - orc_CurrentArea.u32_StartAddress);
         q_AddAreaBefore = true;

         // Adapt the current area to the "rest". The further handling will be made in check for 2.3
         orc_CurrentArea.u32_Size = (orc_CurrentArea.u32_StartAddress + orc_CurrentArea.u32_Size) -
                                    rc_ComparedArea.u32_StartAddress;
         orc_CurrentArea.u32_StartAddress = rc_ComparedArea.u32_StartAddress;

         q_OverlapDetected = true;
      }

      // Check for 2.2
      if ((orc_CurrentArea.u32_StartAddress > rc_ComparedArea.u32_StartAddress) &&
          (orc_CurrentArea.u32_StartAddress < (rc_ComparedArea.u32_StartAddress  + rc_ComparedArea.u32_Size)))
      {
         // Create the area "before" based on the compared area and add it to the result
         c_AreaBefore = rc_ComparedArea;
         c_AreaBefore.u32_Size = (orc_CurrentArea.u32_StartAddress - rc_ComparedArea.u32_StartAddress);
         q_AddAreaBefore = true;

         // Adapt the compared area to the "rest". The further handling will be made in check for 2.3
         rc_ComparedArea.u32_Size = (rc_ComparedArea.u32_StartAddress + rc_ComparedArea.u32_Size) -
                                    orc_CurrentArea.u32_StartAddress;
         rc_ComparedArea.u32_StartAddress = orc_CurrentArea.u32_StartAddress;

         q_OverlapDetected = true;
      }

      // Check for 2.3 (or continuing 2.1 and 2.2)
      if ((orc_CurrentArea.u32_StartAddress == rc_ComparedArea.u32_StartAddress) &&
          (orc_CurrentArea.u32_Size > 0U) &&
          (rc_ComparedArea.u32_Size > 0U))
      {
         q_OverlapDetected = true;

         // Check for 2.4
         if (orc_CurrentArea.u32_Size > rc_ComparedArea.u32_Size)
         {
            // Merging of areas to mark the same usage on the area
            C_PuiSdHandlerNodeLogic::mh_MergeNvmDataPoolAreas(orc_CurrentArea, rc_ComparedArea);

            // Adapt the current area to the area "behind" the merged area
            orc_CurrentArea.u32_Size = orc_CurrentArea.u32_Size - rc_ComparedArea.u32_Size;
            orc_CurrentArea.u32_StartAddress = rc_ComparedArea.u32_StartAddress + rc_ComparedArea.u32_Size;

            // Recheck this adapted area with the entire function again. Maybe it overlaps an other area
            C_PuiSdHandlerNodeLogic::mh_AddAndAdaptNvmDataPoolArea(orc_CurrentArea, orc_Areas);
         }
         // Check for 2.5
         else if (orc_CurrentArea.u32_Size < rc_ComparedArea.u32_Size)
         {
            // Merging of areas to mark the same usage on the area
            C_PuiSdHandlerNodeLogic::mh_MergeNvmDataPoolAreas(rc_ComparedArea, orc_CurrentArea);

            // Adapt the compared area to the area "behind" the merged area
            rc_ComparedArea.u32_Size = rc_ComparedArea.u32_Size - orc_CurrentArea.u32_Size;
            rc_ComparedArea.u32_StartAddress = orc_CurrentArea.u32_StartAddress + orc_CurrentArea.u32_Size;

            // Add the current area
            orc_Areas.push_back(orc_CurrentArea);
         }
         // Check for 2.6
         else
         {
            // Merging of areas is enough. The current area must no be added to the result
            C_PuiSdHandlerNodeLogic::mh_MergeNvmDataPoolAreas(orc_CurrentArea, rc_ComparedArea);
         }
      }

      if (q_OverlapDetected == true)
      {
         if (q_AddAreaBefore == true)
         {
            // Adding at the end to avoid problems when the vector must resize and the elements could get a new address
            orc_Areas.push_back(c_AreaBefore);
         }

         // All overlaps separated due to the recursive handling of 2.4.
         break;
      }
   }

   if (q_OverlapDetected == false)
   {
      // Condition 1: Add the original unedited area
      orc_Areas.push_back(orc_CurrentArea);
   }

   // Sorting for start address each time to make sure the sequence works in any case
   std::sort(orc_Areas.begin(), orc_Areas.end());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Merges two NMV Datapool areas

   \param[in]      orc_AreaToAdd    Area for adding to orc_AreaToMerge area
   \param[in,out]  orc_AreaToMerge  Merged area
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::mh_MergeNvmDataPoolAreas(
   const C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea & orc_AreaToAdd,
   C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea & orc_AreaToMerge)
{
   uint32 u32_Counter;

   orc_AreaToMerge.c_DataPoolIndexes.reserve(
      orc_AreaToMerge.c_DataPoolIndexes.size() + orc_AreaToAdd.c_DataPoolIndexes.size());
   for (u32_Counter = 0; u32_Counter < orc_AreaToAdd.c_DataPoolIndexes.size(); ++u32_Counter)
   {
      orc_AreaToMerge.c_DataPoolIndexes.push_back(orc_AreaToAdd.c_DataPoolIndexes[u32_Counter]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node ids

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_NodeIds       Node ids
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::m_SetOSCNodeIds(const uint32 ou32_NodeIndex, const std::vector<uint8> & orc_NodeIds)
{
   tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      for (uint32 u32_ItInt = 0UL;
           (u32_ItInt < rc_OSCNode.c_Properties.c_ComInterfaces.size()) && (u32_ItInt < orc_NodeIds.size());
           ++u32_ItInt)
      {
         m_SetOSCNodeId(ou32_NodeIndex, u32_ItInt, orc_NodeIds[u32_ItInt]);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node id

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  ou8_NodeId           Node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::m_SetOSCNodeId(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                             const uint8 ou8_NodeId)
{
   tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      tgl_assert(ou32_InterfaceIndex < rc_OSCNode.c_Properties.c_ComInterfaces.size());
      if (ou32_InterfaceIndex < rc_OSCNode.c_Properties.c_ComInterfaces.size())
      {
         C_OSCNodeComInterfaceSettings & rc_Interface = rc_OSCNode.c_Properties.c_ComInterfaces[ou32_InterfaceIndex];
         rc_Interface.u8_NodeID = ou8_NodeId;
         m_SyncOsyNodeIdChange(ou32_NodeIndex, ou32_InterfaceIndex, ou8_NodeId);
      }
   }
}
