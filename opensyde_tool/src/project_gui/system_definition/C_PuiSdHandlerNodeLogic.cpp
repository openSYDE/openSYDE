//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node logic part of UI system definition

   Node logic part of UI system definition

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_Uti.hpp"
#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_PuiSdHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

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
bool C_PuiSdHandlerNodeLogic::CheckNodeNameAvailable(const C_SclString & orc_Name,
                                                     const uint32_t * const opu32_NodeIndexToSkip,
                                                     std::vector<stw::scl::C_SclString> * const opc_ExistingNames) const
{
   bool q_Retval = true;

   //Either end on error or continue if all node names are requested
   for (uint32_t u32_ItNode = 0;
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
         const stw::scl::C_SclString c_CurName = C_PuiSdUtil::h_GetNodeBaseNameOrName(u32_ItNode).toStdString().c_str();
         //Check conflict
         if (c_CurName.LowerCase() == orc_Name.LowerCase())
         {
            q_Retval = false;
         }
         //Store other (not checked) node name
         if (opc_ExistingNames != NULL)
         {
            bool q_Exists = false;
            for (uint32_t u32_It = 0UL; u32_It < opc_ExistingNames->size(); ++u32_It)
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
void C_PuiSdHandlerNodeLogic::SetOscNodeEthernetConfiguration(const uint32_t ou32_NodeIndex,
                                                              const uint32_t ou32_InterfaceIndex,
                                                              const std::vector<int32_t> & orc_Ip,
                                                              const std::vector<int32_t> & orc_NetMask,
                                                              const std::vector<int32_t> & orc_DefaultGateway)
{
   if (((orc_Ip.size() == 4UL) && (orc_NetMask.size() == 4UL)) && (orc_DefaultGateway.size() == 4UL))
   {
      if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
      {
         C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
         if (ou32_InterfaceIndex < rc_Node.c_Properties.c_ComInterfaces.size())
         {
            C_OscNodeComInterfaceSettings & rc_CurInterface = rc_Node.c_Properties.c_ComInterfaces[ou32_InterfaceIndex];
            rc_CurInterface.c_Ip.au8_IpAddress[0] = static_cast<uint8_t>(orc_Ip[0]);
            rc_CurInterface.c_Ip.au8_IpAddress[1] = static_cast<uint8_t>(orc_Ip[1]);
            rc_CurInterface.c_Ip.au8_IpAddress[2] = static_cast<uint8_t>(orc_Ip[2]);
            rc_CurInterface.c_Ip.au8_IpAddress[3] = static_cast<uint8_t>(orc_Ip[3]);
            rc_CurInterface.c_Ip.au8_NetMask[0] = static_cast<uint8_t>(orc_NetMask[0]);
            rc_CurInterface.c_Ip.au8_NetMask[1] = static_cast<uint8_t>(orc_NetMask[1]);
            rc_CurInterface.c_Ip.au8_NetMask[2] = static_cast<uint8_t>(orc_NetMask[2]);
            rc_CurInterface.c_Ip.au8_NetMask[3] = static_cast<uint8_t>(orc_NetMask[3]);
            rc_CurInterface.c_Ip.au8_DefaultGateway[0] = static_cast<uint8_t>(orc_DefaultGateway[0]);
            rc_CurInterface.c_Ip.au8_DefaultGateway[1] = static_cast<uint8_t>(orc_DefaultGateway[1]);
            rc_CurInterface.c_Ip.au8_DefaultGateway[2] = static_cast<uint8_t>(orc_DefaultGateway[2]);
            rc_CurInterface.c_Ip.au8_DefaultGateway[3] = static_cast<uint8_t>(orc_DefaultGateway[3]);

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
const C_PuiSdNode * C_PuiSdHandlerNodeLogic::GetUiNode(const uint32_t & oru32_Index) const
{
   const C_PuiSdNode * pc_Retval;

   if (oru32_Index < this->mc_UiNodes.size())
   {
      pc_Retval = &(this->mc_UiNodes[oru32_Index]);
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
const C_OscNodeSquad * C_PuiSdHandlerNodeLogic::GetOscNodeSquadConst(const uint32_t & oru32_Index) const
{
   const C_OscNodeSquad * pc_Retval;

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
const C_OscNode * C_PuiSdHandlerNodeLogic::GetOscNodeConst(const uint32_t & oru32_Index) const
{
   const C_OscNode * pc_Retval;

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
int32_t C_PuiSdHandlerNodeLogic::GetSortedOscNodeConst(const uint32_t & oru32_Index, C_OscNode & orc_Node) const
{
   int32_t s32_Retval = C_NO_ERR;
   C_PuiSdNode c_UiNode;

   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size())
   {
      if (oru32_Index < this->mc_CoreDefinition.c_Nodes.size())
      {
         orc_Node = this->mc_CoreDefinition.c_Nodes[oru32_Index];
         c_UiNode = this->mc_UiNodes[oru32_Index];
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
C_OscNode * C_PuiSdHandlerNodeLogic::GetOscNode(const uint32_t & oru32_Index)
{
   C_OscNode * pc_Retval;

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
C_OscNodeSquad * C_PuiSdHandlerNodeLogic::GetOscNodeSquad(const uint32_t & oru32_Index)
{
   C_OscNodeSquad * pc_Retval;

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
int32_t C_PuiSdHandlerNodeLogic::SetStwFlashloaderSettings(const uint32_t ou32_Index,
                                                           const C_OscNodeStwFlashloaderSettings & orc_Settings)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_Index];
      rc_Node.c_Properties.c_StwFlashloaderSettings = orc_Settings;
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
int32_t C_PuiSdHandlerNodeLogic::SetNodeOpenSydeServerSettings(const uint32_t ou32_Index,
                                                               const C_OscNodeOpenSydeServerSettings & orc_Settings)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_Index];
      rc_Node.c_Properties.c_OpenSydeServerSettings = orc_Settings;
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
int32_t C_PuiSdHandlerNodeLogic::SetNodeCodeExportSettings(const uint32_t ou32_Index,
                                                           const C_OscNodeCodeExportSettings & orc_Settings)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_Index];
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
void C_PuiSdHandlerNodeLogic::SetOscNodeProperties(const uint32_t ou32_NodeIndex,
                                                   const C_OscNodeProperties & orc_Properties)
{
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      rc_OscNode.c_Properties = orc_Properties;
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
void C_PuiSdHandlerNodeLogic::SetOscNodePropertiesDetailed(const uint32_t ou32_NodeIndex, const QString & orc_Name,
                                                           const QString & orc_Comment,
                                                           const C_OscNodeProperties::E_DiagnosticServerProtocol oe_DiagnosticServer, const C_OscNodeProperties::E_FlashLoaderProtocol oe_FlashLoader, const std::vector<uint8_t> & orc_NodeIds, const std::vector<bool> & orc_UpdateFlags, const std::vector<bool> & orc_RoutingFlags,
                                                           const std::vector<bool> & orc_DiagnosisFlags)
{
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];

      //set name (special handling)
      this->mc_CoreDefinition.SetNodeName(ou32_NodeIndex, orc_Name.toStdString().c_str());

      //set other properties
      rc_OscNode.c_Properties.c_Comment = orc_Comment.toStdString().c_str();
      rc_OscNode.c_Properties.e_DiagnosticServer = oe_DiagnosticServer;
      rc_OscNode.c_Properties.e_FlashLoader = oe_FlashLoader;
      if (((orc_NodeIds.size() == orc_UpdateFlags.size()) && (orc_NodeIds.size() == orc_RoutingFlags.size())) &&
          (orc_NodeIds.size() == orc_DiagnosisFlags.size()))
      {
         for (uint32_t u32_ItInt = 0UL;
              (u32_ItInt < rc_OscNode.c_Properties.c_ComInterfaces.size()) && (u32_ItInt < orc_NodeIds.size());
              ++u32_ItInt)
         {
            C_OscNodeComInterfaceSettings & rc_Interface = rc_OscNode.c_Properties.c_ComInterfaces[u32_ItInt];
            rc_Interface.q_IsUpdateEnabled = orc_UpdateFlags[u32_ItInt];
            rc_Interface.q_IsRoutingEnabled = orc_RoutingFlags[u32_ItInt];
            rc_Interface.q_IsDiagnosisEnabled = orc_DiagnosisFlags[u32_ItInt];
         }
      }
      m_SetOscNodeIds(ou32_NodeIndex, orc_NodeIds);
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
void C_PuiSdHandlerNodeLogic::SetUiNodeBox(const uint32_t ou32_NodeIndex, const C_PuiBsBox & orc_Box)
{
   if (ou32_NodeIndex < this->mc_UiNodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[ou32_NodeIndex];
      //Copy box manually
      rc_UiNode.c_UiPosition = orc_Box.c_UiPosition;
      rc_UiNode.f64_Height = orc_Box.f64_Height;
      rc_UiNode.f64_Width = orc_Box.f64_Width;
      rc_UiNode.f64_ZetOrder = orc_Box.f64_ZetOrder;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node

   \param[in,out]  orc_OscNode         New OSC node (name might be modified by this function if not unique)
   \param[in,out]  orc_UiNode          New UI node
   \param[in]      orc_SubDeviceName   Sub device name
   \param[in]      orc_MainDevice      Main device

   \return
   Index of node  (0 -> first node)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_PuiSdHandlerNodeLogic::AddNodeAndSort(C_OscNode & orc_OscNode, const C_PuiSdNode & orc_UiNode,
                                                 const QString & orc_SubDeviceName, const QString & orc_MainDevice)
{
   const uint32_t u32_Index = mc_CoreDefinition.c_Nodes.size();
   //Extract device name if the device was already set
   const C_SclString c_DeviceName = (orc_OscNode.pc_DeviceDefinition !=
                                     NULL) ? orc_OscNode.pc_DeviceDefinition->GetDisplayName() : "";
   const C_SclString c_DefaultDeviceName =
      C_PuiSdHandlerNodeLogic::h_AutomaticCeStringAdaptation(c_DeviceName.c_str()).toStdString().c_str();

   orc_OscNode.c_Properties.c_Name = C_Uti::h_GetUniqueName(
      this->m_GetExistingNodeNames(), orc_OscNode.c_Properties.c_Name, c_DefaultDeviceName);

   mc_CoreDefinition.AddNode(orc_OscNode, orc_SubDeviceName.toStdString().c_str(),
                             orc_MainDevice.toStdString().c_str()); //add node and set device definition
   // pointer

   //insert UI part at same position as OSC part:
   this->mc_UiNodes.insert(mc_UiNodes.begin() + u32_Index, orc_UiNode);

   //signal "node change"
   Q_EMIT (this->SigNodesChanged());
   this->m_HandleSyncNodeAdded(u32_Index);

   // No adaption of the shared Datapools necessary.
   // The new node index is always higher than the already existing nodes indexes

   return u32_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node squad

   \param[in,out]  orc_OscNodes        New OSC nodes (name might be modified by this function if not unique)
   \param[in,out]  orc_UiNodes         New UI nodes
   \param[in]      orc_NodeNames       Node names
   \param[in]      orc_MainDevice      Main device
   \param[in]      orc_NameProposal    Name proposal

   \return
   Index of first node of node squad (0 -> first node)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_PuiSdHandlerNodeLogic::AddNodeSquadAndSort(std::vector<C_OscNode> & orc_OscNodes,
                                                      const std::vector<C_PuiSdNode> & orc_UiNodes,
                                                      const std::vector<QString> & orc_NodeNames,
                                                      const QString & orc_MainDevice, const QString & orc_NameProposal)
{
   const uint32_t u32_NodeIndex = mc_CoreDefinition.c_Nodes.size();
   const uint32_t u32_NodeSquadIndex = mc_CoreDefinition.c_NodeSquads.size();

   std::vector<stw::scl::C_SclString> c_NodeNames;
   C_SclString c_Name;
   uint32_t u32_NodeCounter;

   c_NodeNames.reserve(orc_NodeNames.size());
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < orc_NodeNames.size(); ++u32_ItNode)
   {
      c_NodeNames.emplace_back(orc_NodeNames[u32_ItNode].toStdString().c_str());
   }

   tgl_assert(orc_OscNodes.size() > 0);
   tgl_assert(orc_OscNodes.size() == orc_UiNodes.size());
   tgl_assert(orc_OscNodes.size() == orc_NodeNames.size());
   if (orc_OscNodes.size() > 0)
   {
      // The device name of the first node is enough. Must be identical on all sub nodes
      C_OscNode & rc_OscNode = orc_OscNodes[0];
      //Extract device name if the device was already set
      const C_SclString c_DeviceName = (rc_OscNode.pc_DeviceDefinition !=
                                        NULL) ? rc_OscNode.pc_DeviceDefinition->GetDisplayName() : "";
      const C_SclString c_DefaultDeviceName =
         C_PuiSdHandlerNodeLogic::h_AutomaticCeStringAdaptation(c_DeviceName.c_str()).toStdString().c_str();
      c_Name = orc_NameProposal.isEmpty() ? c_DefaultDeviceName : orc_NameProposal.toStdString().c_str();

      // The proposed name would be identical for all sub nodes too. The sub node specific part of the name
      // will be added with SetBaseName
      c_Name = C_Uti::h_GetUniqueName(
         this->m_GetExistingNodeNames(), c_Name, c_DefaultDeviceName);
   }

   this->mc_CoreDefinition.AddNodeSquad(orc_OscNodes, c_NodeNames, orc_MainDevice.toStdString().c_str()); //add node and
                                                                                                          // set device
   // definition pointer

   tgl_assert(u32_NodeSquadIndex < mc_CoreDefinition.c_NodeSquads.size());
   this->mc_CoreDefinition.c_NodeSquads[u32_NodeSquadIndex].SetBaseName(this->mc_CoreDefinition.c_Nodes,
                                                                        c_Name);

   //insert UI part at same position as OSC part:
   this->mc_UiNodes.insert(mc_UiNodes.begin() + u32_NodeIndex, orc_UiNodes.begin(), orc_UiNodes.end());

   //signal "node change"
   Q_EMIT (this->SigNodesChanged());
   for (u32_NodeCounter = 0U; u32_NodeCounter < orc_OscNodes.size(); ++u32_NodeCounter)
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
void C_PuiSdHandlerNodeLogic::RemoveNode(const uint32_t ou32_NodeIndex)
{
   int32_t s32_NodeIndexCounter;

   const std::vector<uint32_t> c_AllNodeIndexToRemove = this->GetAllNodeGroupIndicesUsingNodeIndex(ou32_NodeIndex);

   // Start with the last index due to the highest index. Avoiding problems with deleting and changing orders for
   // the other node index
   for (s32_NodeIndexCounter = (static_cast<int32_t>(c_AllNodeIndexToRemove.size()) - 1); s32_NodeIndexCounter >= 0;
        --s32_NodeIndexCounter)
   {
      //Synchronization engine (First!)
      this->m_HandleSyncNodeAboutToBeDeleted(c_AllNodeIndexToRemove[static_cast<uint32_t>(s32_NodeIndexCounter)]);
   }

   tgl_assert(this->mc_CoreDefinition.DeleteNode(ou32_NodeIndex) == C_NO_ERR);

   for (s32_NodeIndexCounter = (static_cast<int32_t>(c_AllNodeIndexToRemove.size()) - 1); s32_NodeIndexCounter >= 0;
        --s32_NodeIndexCounter)
   {
      this->mc_UiNodes.erase(
         this->mc_UiNodes.begin() + c_AllNodeIndexToRemove[static_cast<uint32_t>(s32_NodeIndexCounter)]);

      // Update shared Datapool configuration
      this->mc_SharedDatapools.OnNodeRemoved(c_AllNodeIndexToRemove[static_cast<uint32_t>(s32_NodeIndexCounter)]);
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
bool C_PuiSdHandlerNodeLogic::CheckNodeConflict(const uint32_t & oru32_NodeIndex) const
{
   bool q_Retval;
   static QMap<uint32_t, bool> hc_PreviousResult;

   //Get reference hash
   const uint32_t u32_Hash = this->m_GetHashNode(oru32_NodeIndex);
   //Look up
   const QMap<uint32_t, bool>::const_iterator c_It = hc_PreviousResult.find(u32_Hash);

   if (c_It == hc_PreviousResult.end())
   {
      bool q_NameConflict;
      bool q_NameEmpty;
      bool q_NodeIdInvalid;
      bool q_IpInvalid;
      bool q_DataPoolsInvalid;
      bool q_ApplicationsInvalid;
      bool q_DomainsInvalid;
      bool q_CommMinSignalCountInvalid;
      bool q_CommMaxSignalCountInvalid;
      bool q_CoPdoCountInvalid;
      bool q_CoNodeIdInvalid;
      bool q_CoHeartbeatInvalid;

      if (this->mc_CoreDefinition.CheckErrorNode(oru32_NodeIndex, &q_NameConflict, &q_NameEmpty, &q_NodeIdInvalid,
                                                 &q_IpInvalid,
                                                 &q_DataPoolsInvalid, &q_ApplicationsInvalid, &q_DomainsInvalid,
                                                 &q_CommMinSignalCountInvalid,
                                                 &q_CommMaxSignalCountInvalid, &q_CoPdoCountInvalid,
                                                 &q_CoNodeIdInvalid, &q_CoHeartbeatInvalid,
                                                 true, NULL, NULL, NULL, NULL, NULL) == C_NO_ERR)
      {
         const bool q_NvmSizeConflict = this->CheckNodeNvmDataPoolsSizeConflict(oru32_NodeIndex);
         if ((q_NameConflict == true) || (q_NodeIdInvalid == true) || (q_IpInvalid == true) ||
             (q_DataPoolsInvalid == true) ||
             (q_ApplicationsInvalid == true) || (q_DomainsInvalid == true) || (q_NameEmpty == true) ||
             (q_NvmSizeConflict == true) ||
             (q_CommMinSignalCountInvalid == true) ||
             (q_CommMaxSignalCountInvalid == true) ||
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
             (q_CoNodeIdInvalid == true) || (q_CommMinSignalCountInvalid == true) ||
             (q_CommMaxSignalCountInvalid == true) || (q_CoPdoCountInvalid == true) ||
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
                                                 NULL, NULL, NULL, NULL, NULL, NULL, NULL,
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
void C_PuiSdHandlerNodeLogic::GetSupportedCanBitrates(const std::vector<uint32_t> & orc_Nodes,
                                                      std::vector<uint32_t> & orc_Bitrates) const
{
   uint32_t u32_NodeCounter;

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
      const C_OscNode * const pc_Node = this->GetOscNodeConst(orc_Nodes[u32_NodeCounter]);

      if (pc_Node != NULL)
      {
         uint32_t u32_SupportedBitrateCounter;
         std::vector<uint32_t> c_TempBitrates;

         tgl_assert(pc_Node->pc_DeviceDefinition != NULL);

         // Search common bitrates by comparing previously found bitrates with the bitrates of the current device
         for (u32_SupportedBitrateCounter = 0U; u32_SupportedBitrateCounter < orc_Bitrates.size();
              ++u32_SupportedBitrateCounter)
         {
            uint32_t u32_DeviceBitrateCounter;

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
uint32_t C_PuiSdHandlerNodeLogic::GetOscNodesSize(void) const
{
   return this->mc_CoreDefinition.c_Nodes.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC Node squads size

   \return
   total number of node squads
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_PuiSdHandlerNodeLogic::GetOscNodeSquadsSize(void) const
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
std::vector<uint32_t> C_PuiSdHandlerNodeLogic::GetAllNodeGroupIndicesUsingNodeIndex(const uint32_t ou32_NodeIndex) const
{
   std::vector<uint32_t> c_Retval;
   bool q_Found = false;
   for (uint32_t u32_ItGroup = 0UL; u32_ItGroup < this->mc_CoreDefinition.c_NodeSquads.size(); ++u32_ItGroup)
   {
      const C_OscNodeSquad & rc_Group = this->mc_CoreDefinition.c_NodeSquads[u32_ItGroup];
      for (uint32_t u32_ItSubDevice = 0UL; u32_ItSubDevice < rc_Group.c_SubNodeIndexes.size(); ++u32_ItSubDevice)
      {
         if (rc_Group.c_SubNodeIndexes[u32_ItSubDevice] == ou32_NodeIndex)
         {
            q_Found = true;
            c_Retval.reserve(rc_Group.c_SubNodeIndexes.size());
            for (uint32_t u32_ItNewSubDevice = 0UL; u32_ItNewSubDevice < rc_Group.c_SubNodeIndexes.size();
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
int32_t C_PuiSdHandlerNodeLogic::GetNodeSquadIndexWithNodeIndex(const uint32_t ou32_NodeIndex,
                                                                uint32_t & oru32_NodeSquadIndex) const
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
void C_PuiSdHandlerNodeLogic::GetNodeToNodeSquadMapping(std::vector<int32_t> & orc_Mapping)
{
   uint32_t u32_NodeCounter;

   orc_Mapping.clear();

   // Initialize with -1 for no squad node
   orc_Mapping.resize(this->mc_CoreDefinition.c_Nodes.size(), -1);

   for (u32_NodeCounter = 0U; u32_NodeCounter < this->mc_CoreDefinition.c_Nodes.size(); ++u32_NodeCounter)
   {
      uint32_t u32_NodeSquadIndex = 0U;
      if (this->mc_CoreDefinition.GetNodeSquadIndexWithNodeIndex(u32_NodeCounter, u32_NodeSquadIndex) == C_NO_ERR)
      {
         // Node is part of an node squad
         orc_Mapping[u32_NodeCounter] = static_cast<int32_t>(u32_NodeSquadIndex);
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
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      bool q_ErrorDetected = false;
      const C_OscNode & rc_CheckedNode = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      //Start at begin again to list each name conflict twice!
      for (uint32_t u32_ItNodeRef = 0UL; u32_ItNodeRef < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNodeRef)
      {
         const C_OscNode & rc_CurNode = this->mc_CoreDefinition.c_Nodes[u32_ItNodeRef];
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
            opc_CriticalNodeNames->emplace_back(rc_CheckedNode.c_Properties.c_Name.c_str());
         }
      }
      //Datapools
      for (uint32_t u32_ItDatapool = 0UL; u32_ItDatapool < rc_CheckedNode.c_DataPools.size(); ++u32_ItDatapool)
      {
         const C_OscNodeDataPool & rc_CheckedDatapool = rc_CheckedNode.c_DataPools[u32_ItDatapool];
         //reset error
         q_ErrorDetected = false;
         //Start at begin again to list each name conflict twice!
         for (uint32_t u32_ItDatapoolRef = 0UL; u32_ItDatapoolRef < rc_CheckedNode.c_DataPools.size();
              ++u32_ItDatapoolRef)
         {
            const C_OscNodeDataPool & rc_CurDatapool = rc_CheckedNode.c_DataPools[u32_ItDatapoolRef];
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
   for (uint32_t u32_ItBus = 0UL; u32_ItBus < this->mc_CoreDefinition.c_Buses.size(); ++u32_ItBus)
   {
      bool q_ErrorDetected = false;
      const C_OscSystemBus & rc_CheckedBus = this->mc_CoreDefinition.c_Buses[u32_ItBus];
      //Start at begin again to list each name conflict twice!
      for (uint32_t u32_ItBusRef = 0UL; u32_ItBusRef < this->mc_CoreDefinition.c_Buses.size(); ++u32_ItBusRef)
      {
         const C_OscSystemBus & rc_CurBus = this->mc_CoreDefinition.c_Buses[u32_ItBusRef];
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
            opc_CriticalBusNames->emplace_back(rc_CheckedBus.c_Name.c_str());
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
int32_t C_PuiSdHandlerNodeLogic::MapNodeNameToIndex(const QString & orc_NodeName, uint32_t & oru32_NodeIndex) const
{
   int32_t s32_Retval = C_RANGE;

   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->GetOscNodesSize(); ++u32_ItNode)
   {
      const C_OscNode * const pc_Node = this->GetOscNodeConst(u32_ItNode);
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
int32_t C_PuiSdHandlerNodeLogic::MapNodeIndexToName(const uint32_t ou32_NodeIndex, QString & orc_NodeName) const
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNode * const pc_Node = this->GetOscNodeConst(ou32_NodeIndex);

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
bool C_PuiSdHandlerNodeLogic::HasNodeAnAvailableFlashloader(const uint32_t ou32_NodeIndex) const
{
   bool q_Retval = false;

   const C_OscNode * const pc_Node = this->GetOscNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OscDeviceDefinition * const pc_Device = pc_Node->pc_DeviceDefinition;
      const uint32_t u32_SubDeviceIndex = pc_Node->u32_SubDeviceIndex;
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
bool C_PuiSdHandlerNodeLogic::CheckNodeIndexAssociatedWithAnotherNodeIndex(const uint32_t ou32_FirstNodeIndex,
                                                                           const uint32_t ou32_SecondNodeIndex) const
{
   bool q_Retval = false;

   const std::vector<uint32_t> c_NodeIndices = this->GetAllNodeGroupIndicesUsingNodeIndex(ou32_FirstNodeIndex);

   for (uint32_t u32_ItDevice = 0UL; u32_ItDevice < c_NodeIndices.size(); ++u32_ItDevice)
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
/*! \brief  Replace node

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_OscNode       Osc node
   \param[in]  orc_UiNode        Ui node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::ReplaceNode(const uint32_t ou32_NodeIndex,
                                          const stw::opensyde_core::C_OscNode & orc_OscNode,
                                          const C_PuiSdNode & orc_UiNode)
{
   //Save device definition
   const stw::opensyde_core::C_OscDeviceDefinition * const pc_DevDef =
      this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex].pc_DeviceDefinition;

   Q_EMIT (this->SigSyncNodeAboutToBeDeleted(ou32_NodeIndex, true));
   this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex] = orc_OscNode;
   //Restore device definition
   this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex].pc_DeviceDefinition = pc_DevDef;

   //insert UI part at same position as OSC part:
   this->mc_UiNodes[ou32_NodeIndex] = orc_UiNode;

   Q_EMIT (this->SigSyncNodeReplace(ou32_NodeIndex));

   //do not signal "node change" (unselects node in node bar and should not be necessary)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add data pool

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  orc_OscContent             OSC data pool content (name might be modified by this function if not unique)
   \param[in]  orc_UiContent              UI data pool content
   \param[in]  ore_ComProtocolType        Optional parameter for com protocol type
   \param[in]  orq_AllowNameAdaptation    Optional parameter to disable automatic name adaptation
   \param[in]  orq_AllowDataAdaptation    Optional parameter to disable automatic data adaptation

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::AddDataPool(const uint32_t & oru32_NodeIndex, const C_OscNodeDataPool & orc_OscContent,
                                             const C_PuiSdNodeDataPool & orc_UiContent,
                                             const C_OscCanProtocol::E_Type & ore_ComProtocolType,
                                             const bool & orq_AllowNameAdaptation, const bool & orq_AllowDataAdaptation)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_UiNodes.size())
   {
      const C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      uint32_t u32_ItTargetIndex = 0;
      //Handle correct array segment position
      if ((orc_OscContent.e_Type != C_OscNodeDataPool::eHALC) &&
          (orc_OscContent.e_Type != C_OscNodeDataPool::eHALC_NVM))
      {
         const C_OscNodeDataPool * pc_DataPool;

         //Always go to end of DIAG
         for (; u32_ItTargetIndex < rc_OscNode.c_DataPools.size(); ++u32_ItTargetIndex)
         {
            pc_DataPool = &rc_OscNode.c_DataPools[u32_ItTargetIndex];
            if (pc_DataPool->e_Type != C_OscNodeDataPool::eDIAG)
            {
               break;
            }
         }
         if ((orc_OscContent.e_Type == C_OscNodeDataPool::eNVM) ||
             (orc_OscContent.e_Type == C_OscNodeDataPool::eCOM))
         {
            //Go to end of NVM in case of NVM and COMM Datapools
            for (; u32_ItTargetIndex < rc_OscNode.c_DataPools.size(); ++u32_ItTargetIndex)
            {
               pc_DataPool = &rc_OscNode.c_DataPools[u32_ItTargetIndex];
               if (pc_DataPool->e_Type != C_OscNodeDataPool::eNVM)
               {
                  break;
               }
            }
         }
         if (orc_OscContent.e_Type == C_OscNodeDataPool::eCOM)
         {
            //Go to end of COMM
            for (; u32_ItTargetIndex < rc_OscNode.c_DataPools.size(); ++u32_ItTargetIndex)
            {
               pc_DataPool = &rc_OscNode.c_DataPools[u32_ItTargetIndex];
               if (pc_DataPool->e_Type != C_OscNodeDataPool::eCOM)
               {
                  break;
               }
            }
         }
      }
      else
      {
         //Go to "total" end
         u32_ItTargetIndex = rc_OscNode.c_DataPools.size();
      }

      //Insert
      tgl_assert(this->InsertDataPool(oru32_NodeIndex, u32_ItTargetIndex, orc_OscContent, orc_UiContent,
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
int32_t C_PuiSdHandlerNodeLogic::AddAutoGenCommDataPool(const uint32_t & oru32_NodeIndex,
                                                        const C_OscCanProtocol::E_Type & ore_ComProtocolType)
{
   C_OscNodeDataPool c_NewDatapool;
   const C_PuiSdNodeDataPool c_UiDataPool;
   int32_t s32_Return;
   QString c_Comment;
   QString c_ProtocolName = C_PuiSdUtil::h_ConvertProtocolTypeToString(ore_ComProtocolType);

   // add the new datapool
   c_NewDatapool.e_Type = C_OscNodeDataPool::eCOM;

   c_Comment = C_GtGetText::h_GetText("Automatically generated Datapool for ");
   c_Comment += c_ProtocolName;
   c_Comment += C_GtGetText::h_GetText(" CAN communication");

   // special case layer 2 -> no spaces and no underscore number at the end
   c_ProtocolName = C_PuiSdUtil::h_ConvertProtocolTypeToDatapoolNameString(ore_ComProtocolType);

   c_NewDatapool.c_Name = this->GetUniqueDataPoolName(oru32_NodeIndex, c_ProtocolName.toStdString().c_str());

   c_NewDatapool.c_Comment = c_Comment.toStdString().c_str();

   // set the default safety flag to true if protocol is a safety protocol
   c_NewDatapool.q_IsSafety = ((ore_ComProtocolType == C_OscCanProtocol::eCAN_OPEN_SAFETY) ||
                               (ore_ComProtocolType == C_OscCanProtocol::eECES));

   s32_Return = this->AddDataPool(oru32_NodeIndex, c_NewDatapool, c_UiDataPool, ore_ComProtocolType);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert data pool at specific position

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  orc_OscContent             OSC data pool content (name might be modified by this function if not unique)
   \param[in]  orc_UiContent              UI data pool content
   \param[in]  ore_ComProtocolType        Optional parameter for com protocol type
   \param[in]  orq_AllowNameAdaptation    Optional parameter to disable automatic name adaptation
   \param[in]  orq_AllowDataAdaptation    Optional parameter to disable automatic data adaptation

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::InsertDataPool(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                                const C_OscNodeDataPool & orc_OscContent,
                                                const C_PuiSdNodeDataPool & orc_UiContent,
                                                const C_OscCanProtocol::E_Type & ore_ComProtocolType,
                                                const bool & orq_AllowNameAdaptation,
                                                const bool & orq_AllowDataAdaptation)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_UiNodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      //Unique name
      C_OscNodeDataPool c_NodeDataPool = orc_OscContent;
      if (orq_AllowNameAdaptation == true)
      {
         c_NodeDataPool.c_Name = this->GetUniqueDataPoolName(oru32_NodeIndex, c_NodeDataPool.c_Name);
      }
      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex <= rc_UiNode.c_UiDataPools.size())
      {
         rc_UiNode.c_UiDataPools.insert(rc_UiNode.c_UiDataPools.begin() + oru32_DataPoolIndex, orc_UiContent);
         rc_OscNode.InsertDataPool(oru32_DataPoolIndex, c_NodeDataPool);
         //Handle NVM
         rc_OscNode.RecalculateAddress();
         //Handle COMM
         m_SetUpComDataPool(oru32_NodeIndex, oru32_DataPoolIndex, orq_AllowDataAdaptation, ore_ComProtocolType);

         // Update shared Datapool configuration
         this->mc_SharedDatapools.OnDatapoolInserted(C_OscNodeDataPoolId(oru32_NodeIndex, oru32_DataPoolIndex));

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
int32_t C_PuiSdHandlerNodeLogic::RemoveDataPool(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                                const bool oq_SuppressSyncSignal)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_UiNodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_UiNode.c_UiDataPools.size())
      {
         if (oq_SuppressSyncSignal == false)
         {
            //Synchronization engine (First!)
            Q_EMIT (this->SigSyncNodeDataPoolAboutToBeDeleted(oru32_NodeIndex, oru32_DataPoolIndex));
         }
         //Handle COMM
         m_CleanUpComDataPool(oru32_NodeIndex, oru32_DataPoolIndex);
         rc_UiNode.c_UiDataPools.erase(rc_UiNode.c_UiDataPools.begin() + oru32_DataPoolIndex);
         s32_Retval = rc_OscNode.DeleteDataPool(oru32_DataPoolIndex);
         //Handle NVM
         rc_OscNode.RecalculateAddress();

         // Update shared Datapool configuration
         this->mc_SharedDatapools.OnDatapoolRemoved(C_OscNodeDataPoolId(oru32_NodeIndex, oru32_DataPoolIndex));
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
   \param[in]  orc_OscContent          OSC data pool content
   \param[in]  orc_UiContent           UI data pool content
   \param[in]  oq_NewComProtocolType   Flag if the COM protocol type was changed
   \param[in]  ore_ComProtocolType     Optional parameter for com protocol type

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
   C_NOACT  Input invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::SetDataPool(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                             const C_OscNodeDataPool & orc_OscContent,
                                             const C_PuiSdNodeDataPool & orc_UiContent,
                                             const bool oq_NewComProtocolType,
                                             const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComProtocolType)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_UiNodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_UiNode.c_UiDataPools.size())
      {
         if (orc_OscContent.c_Lists.size() == orc_UiContent.c_DataPoolLists.size())
         {
            rc_UiNode.c_UiDataPools[oru32_DataPoolIndex] = orc_UiContent;
            rc_OscNode.c_DataPools[oru32_DataPoolIndex] = orc_OscContent;
            //Update addresses
            rc_OscNode.RecalculateAddress();
            //Handle COMM
            if ((oq_NewComProtocolType == true) &&
                (rc_OscNode.c_DataPools[oru32_DataPoolIndex].e_Type == C_OscNodeDataPool::eCOM))
            {
               // Update COM datapool protocol
               uint32_t u32_Counter;

               // Search the correct COM protocol
               for (u32_Counter = 0U; u32_Counter < rc_OscNode.c_ComProtocols.size(); ++u32_Counter)
               {
                  if (rc_OscNode.c_ComProtocols[u32_Counter].u32_DataPoolIndex == oru32_DataPoolIndex)
                  {
                     rc_OscNode.c_ComProtocols[u32_Counter].e_Type = ore_ComProtocolType;
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
int32_t C_PuiSdHandlerNodeLogic::UnassignDataPoolApplication(const uint32_t & oru32_NodeIndex,
                                                             const uint32_t & oru32_DataPoolIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      if (oru32_DataPoolIndex < rc_Node.c_DataPools.size())
      {
         C_OscNodeDataPool & rc_DataPool = rc_Node.c_DataPools[oru32_DataPoolIndex];
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
int32_t C_PuiSdHandlerNodeLogic::AssignDataPoolApplication(const uint32_t ou32_NodeIndex,
                                                           const uint32_t ou32_DataPoolIndex,
                                                           const int32_t os32_ApplicationIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_DataPoolIndex < rc_Node.c_DataPools.size())
      {
         C_OscNodeDataPool & rc_DataPool = rc_Node.c_DataPools[ou32_DataPoolIndex];
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
int32_t C_PuiSdHandlerNodeLogic::AssignAllHalcNvmDataPools(const uint32_t ou32_NodeIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      int32_t s32_DataBlockPsi = -1;

      // search for Data Block of type PSI generation
      if ((rc_Node.c_HalcConfig.q_NvmBasedConfig == true) && (rc_Node.c_HalcConfig.IsClear() == false))
      {
         for (uint32_t u32_ItDataBlock = 0UL; u32_ItDataBlock < rc_Node.c_Applications.size(); ++u32_ItDataBlock)
         {
            const C_OscNodeApplication & rc_CurDataBlock = rc_Node.c_Applications[u32_ItDataBlock];
            if (rc_CurDataBlock.e_Type == C_OscNodeApplication::ePARAMETER_SET_HALC)
            {
               s32_DataBlockPsi = static_cast<int32_t>(u32_ItDataBlock);
               break;
            }
         }
      }

      // assign all HALC NVM Datapools
      if (s32_DataBlockPsi >= 0)
      {
         for (uint32_t u32_ItDataPool = 0UL; (u32_ItDataPool < rc_Node.c_DataPools.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItDataPool)
         {
            const C_OscNodeDataPool & rc_DataPool = rc_Node.c_DataPools[u32_ItDataPool];
            if (rc_DataPool.e_Type == C_OscNodeDataPool::eHALC_NVM)
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
   \param[out]  orc_OscContent         OSC data pool content
   \param[out]  orc_UiContent          UI data pool content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::GetDataPool(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                             C_OscNodeDataPool & orc_OscContent,
                                             C_PuiSdNodeDataPool & orc_UiContent) const
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNodeDataPool * const pc_OscDataPool = GetOscDataPool(oru32_NodeIndex,
                                                                   oru32_DataPoolIndex);
   const C_PuiSdNodeDataPool * const pc_UiDataPool = GetUiDataPool(oru32_NodeIndex,
                                                                   oru32_DataPoolIndex);

   if ((pc_OscDataPool != NULL) && (pc_UiDataPool != NULL))
   {
      orc_UiContent = *pc_UiDataPool;
      orc_OscContent = *pc_OscDataPool;
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
C_SclString C_PuiSdHandlerNodeLogic::GetUniqueDataPoolName(const uint32_t & oru32_NodeIndex,
                                                           const C_SclString & orc_Proposal) const
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
const C_OscNodeDataPool * C_PuiSdHandlerNodeLogic::GetOscDataPool(const uint32_t & oru32_NodeIndex,
                                                                  const uint32_t & oru32_DataPoolIndex) const
{
   const C_OscNodeDataPool * pc_Retval = NULL;

   //Check size & consistency
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      //Check size & consistency
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         pc_Retval = &rc_OscNode.c_DataPools[oru32_DataPoolIndex];
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
const C_PuiSdNodeDataPool * C_PuiSdHandlerNodeLogic::GetUiDataPool(const uint32_t & oru32_NodeIndex,
                                                                   const uint32_t & oru32_DataPoolIndex) const
{
   const C_PuiSdNodeDataPool * pc_Retval = NULL;

   //Check size & consistency
   if (oru32_NodeIndex < this->mc_UiNodes.size())
   {
      const C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      //Check size & consistency
      if (oru32_DataPoolIndex < rc_UiNode.c_UiDataPools.size())
      {
         pc_Retval = &rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
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
int32_t C_PuiSdHandlerNodeLogic::GetDataPoolType(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                                 C_OscNodeDataPool::E_Type & ore_Type) const
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      //Check size
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         ore_Type = rc_OscNode.c_DataPools[oru32_DataPoolIndex].e_Type;
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
int32_t C_PuiSdHandlerNodeLogic::MoveDataPool(const uint32_t ou32_NodeIndex, const uint32_t ou32_SourceIndex,
                                              const uint32_t ou32_TargetIndex)
{
   int32_t s32_Retval = C_RANGE;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_UiNodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[ou32_NodeIndex];

      if ((ou32_SourceIndex < rc_UiNode.c_UiDataPools.size()) &&
          (ou32_TargetIndex < rc_UiNode.c_UiDataPools.size()))
      {
         // adapt core node
         s32_Retval = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex].MoveDataPool(ou32_SourceIndex, ou32_TargetIndex);

         // adapt ui node
         if (s32_Retval == C_NO_ERR)
         {
            //Copy
            const C_PuiSdNodeDataPool c_Data = rc_UiNode.c_UiDataPools[ou32_SourceIndex];
            //Erase
            rc_UiNode.c_UiDataPools.erase(rc_UiNode.c_UiDataPools.begin() + ou32_SourceIndex);
            //Insert
            rc_UiNode.c_UiDataPools.insert(rc_UiNode.c_UiDataPools.begin() + ou32_TargetIndex, c_Data);

            //Handle NVM
            this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex].RecalculateAddress();

            // Update shared Datapool configuration
            this->mc_SharedDatapools.OnDatapoolMoved(C_OscNodeDataPoolId(ou32_NodeIndex, ou32_SourceIndex),
                                                     C_OscNodeDataPoolId(ou32_NodeIndex, ou32_TargetIndex));

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
int32_t C_PuiSdHandlerNodeLogic::GetDataPoolIndex(const uint32_t ou32_NodeIndex,
                                                  const C_OscNodeDataPool::E_Type oe_DataPoolType,
                                                  const uint32_t ou32_DataPoolTypeIndex) const
{
   int32_t s32_Return = -1;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_UiNodes.size())
   {
      const C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Return = rc_OscNode.GetDataPoolIndex(oe_DataPoolType, ou32_DataPoolTypeIndex);
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
int32_t C_PuiSdHandlerNodeLogic::GetDataPoolTypeIndex(const uint32_t ou32_NodeIndex,
                                                      const uint32_t ou32_DataPoolIndex) const
{
   int32_t s32_Return = -1;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_UiNodes.size())
   {
      const C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Return = rc_OscNode.GetDataPoolTypeIndex(ou32_DataPoolIndex);
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
int32_t C_PuiSdHandlerNodeLogic::GetDataPoolCount(const uint32_t ou32_NodeIndex,
                                                  const C_OscNodeDataPool::E_Type oe_DataPoolType) const
{
   int32_t s32_Return = -1;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_UiNodes.size())
   {
      const C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Return = rc_OscNode.GetDataPoolCount(oe_DataPoolType);
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
bool C_PuiSdHandlerNodeLogic::CheckNodeDataPoolNameAvailable(const uint32_t & oru32_NodeIndex,
                                                             const C_SclString & orc_Name,
                                                             const uint32_t * const opu32_DataPoolIndexToSkip,
                                                             std::vector<C_SclString> * const opc_ExistingDatapoolNames)
const
{
   bool q_Retval = true;

   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      //Don't stop on error if the existing datapool names are relevant
      for (uint32_t u32_ItDataPool = 0;
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
            const C_OscNodeDataPool & rc_DataPool = rc_Node.c_DataPools[u32_ItDataPool];
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
   //lint -e{1536} it is intended to expose a private member to avoid copies of rather big data structures
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
operator =(const C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea & orc_Source) &
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
bool C_PuiSdHandlerNodeLogic::CheckNodeNvmDataPoolsSizeConflict(const uint32_t ou32_NodeIndex,
                                                                bool * const opq_SizeConflict,
                                                                bool * const opq_OverlapConflict) const
{
   bool q_Return = true;
   const C_OscNode * const pc_Node = this->GetOscNodeConst(ou32_NodeIndex);

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
         uint32_t u32_Counter;
         uint32_t u32_SizeUsedAreas = 0U;
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
int32_t C_PuiSdHandlerNodeLogic::GetNodeNvmDataPoolAreas(const uint32_t ou32_NodeIndex,
                                                         std::vector<C_PuiSdHandlerNodeLogicNvmArea> & orc_Areas)
const
{
   int32_t s32_Retval = C_NO_ERR;

   orc_Areas.clear();

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];

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
   \param[in,out]  orc_OscContent   Application content (Name might get changed)

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::AddApplication(const uint32_t ou32_NodeIndex, C_OscNodeApplication & orc_OscContent)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      tgl_assert(this->InsertApplication(ou32_NodeIndex, rc_Node.c_Applications.size(), orc_OscContent) == C_NO_ERR);
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
   \param[in,out]  orc_OscContent         Application content (Name might get changed)

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::InsertApplication(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex,
                                                   C_OscNodeApplication & orc_OscContent)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_ApplicationIndex <= rc_Node.c_Applications.size())
      {
         orc_OscContent.c_Name = this->GetUniqueApplicationName(ou32_NodeIndex, orc_OscContent.c_Name);
         orc_OscContent.u8_ProcessId = this->GetUniqueApplicationProcessId(ou32_NodeIndex, orc_OscContent.u8_ProcessId);
         rc_Node.c_Applications.insert(rc_Node.c_Applications.begin() + ou32_ApplicationIndex, orc_OscContent);
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
int32_t C_PuiSdHandlerNodeLogic::RemoveApplication(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
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
   \param[in]  orc_OscContent          Application content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::SetApplication(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex,
                                                const C_OscNodeApplication & orc_OscContent)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_ApplicationIndex < rc_Node.c_Applications.size())
      {
         const C_OscNodeApplication::E_Type e_PreviousType = rc_Node.c_Applications[ou32_ApplicationIndex].e_Type;

         // inform SC about incoming Data Block change from PSI to binary type (before change!)
         if ((e_PreviousType == C_OscNodeApplication::ePARAMETER_SET_HALC) &&
             (orc_OscContent.e_Type == C_OscNodeApplication::eBINARY))
         {
            Q_EMIT (C_PuiSdHandler::h_GetInstance()->
                    SigSyncNodeApplicationAboutToBeChangedFromParamSetHalc(ou32_NodeIndex, ou32_ApplicationIndex));
         }

         // set data
         rc_Node.c_Applications[ou32_ApplicationIndex] = orc_OscContent;

         // inform SC about Data Block change from binary to PSI type (after change!)
         if ((e_PreviousType == C_OscNodeApplication::eBINARY) &&
             (orc_OscContent.e_Type == C_OscNodeApplication::ePARAMETER_SET_HALC))
         {
            Q_EMIT (C_PuiSdHandler::h_GetInstance()->
                    SigSyncNodeApplicationChangedToParamSetHalc(ou32_NodeIndex, ou32_ApplicationIndex));
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
const C_OscNodeApplication * C_PuiSdHandlerNodeLogic::GetApplication(const uint32_t ou32_NodeIndex,
                                                                     const uint32_t ou32_ApplicationIndex) const
{
   const C_OscNodeApplication * pc_Retval = NULL;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
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
int32_t C_PuiSdHandlerNodeLogic::MoveApplication(const uint32_t ou32_NodeIndex, const uint32_t ou32_SourceIndex,
                                                 const uint32_t ou32_TargetIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if ((ou32_SourceIndex < rc_Node.c_Applications.size()) &&
          (ou32_TargetIndex < rc_Node.c_Applications.size()))
      {
         //Copy
         const C_OscNodeApplication c_Data = rc_Node.c_Applications[ou32_SourceIndex];
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
C_SclString C_PuiSdHandlerNodeLogic::GetUniqueApplicationName(const uint32_t & oru32_NodeIndex,
                                                              const C_SclString & orc_Proposal) const
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
uint8_t C_PuiSdHandlerNodeLogic::GetUniqueApplicationProcessId(const uint32_t ou32_NodeIndex,
                                                               const uint8_t ou8_Proposal) const
{
   //Use 16 bit to not run into overflow issues
   uint16_t u16_Retval = 0;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      bool q_Found = false;
      //1.Check if proposal is unique
      for (uint32_t u32_ItApp = 0; u32_ItApp < rc_Node.c_Applications.size(); ++u32_ItApp)
      {
         const C_OscNodeApplication & rc_Application = rc_Node.c_Applications[u32_ItApp];
         //Only check programmable applications
         if (rc_Application.e_Type == C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
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
            for (uint32_t u32_ItApp = 0; u32_ItApp < rc_Node.c_Applications.size(); ++u32_ItApp)
            {
               const C_OscNodeApplication & rc_Application = rc_Node.c_Applications[u32_ItApp];
               //Only check programmable applications
               if (rc_Application.e_Type == C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
               {
                  //Should never reach above 255 values
                  if (rc_Application.u8_ProcessId == static_cast<uint8_t>(u16_Retval))
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
         u16_Retval = static_cast<uint16_t>(ou8_Proposal);
      }
   }

   return static_cast<uint8_t>(u16_Retval);
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
int32_t C_PuiSdHandlerNodeLogic::GetNextProgrammableApplicationIndex(const uint32_t ou32_NodeIndex,
                                                                     int32_t & ors32_ApplicationIndex) const
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNode * const pc_Node = this->GetOscNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      //Check if current is valid
      if (ors32_ApplicationIndex >= 0)
      {
         if (static_cast<uint32_t>(ors32_ApplicationIndex) < pc_Node->c_Applications.size())
         {
            const C_OscNodeApplication & rc_Application =
               pc_Node->c_Applications[static_cast<uint32_t>(ors32_ApplicationIndex)];
            if (rc_Application.e_Type != C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
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
         for (uint32_t u32_ItApp = 0; u32_ItApp < pc_Node->c_Applications.size(); ++u32_ItApp)
         {
            const C_OscNodeApplication & rc_Application = pc_Node->c_Applications[u32_ItApp];
            if (rc_Application.e_Type == C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
            {
               ors32_ApplicationIndex = static_cast<int32_t>(u32_ItApp);
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
int32_t C_PuiSdHandlerNodeLogic::CheckApplicationName(const uint32_t ou32_NodeIndex,
                                                      const C_SclString & orc_ApplicationName, bool & orq_Valid,
                                                      const uint32_t * const opu32_SkipApplication) const
{
   int32_t s32_Retval = C_NO_ERR;

   orq_Valid = true;
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      for (uint32_t u32_ItApplication = 0; u32_ItApplication < rc_Node.c_Applications.size(); ++u32_ItApplication)
      {
         const C_OscNodeApplication & rc_Application = rc_Node.c_Applications[u32_ItApplication];
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
std::vector<const C_OscNodeApplication *> C_PuiSdHandlerNodeLogic::GetProgrammableApplications(
   const uint32_t ou32_NodeIndex) const
{
   std::vector<const C_OscNodeApplication *> c_Retval;
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      for (uint32_t u32_ItAppl = 0; u32_ItAppl < rc_Node.c_Applications.size(); ++u32_ItAppl)
      {
         const C_OscNodeApplication & rc_Application = rc_Node.c_Applications[u32_ItAppl];
         if (rc_Application.e_Type == C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
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
std::vector<uint32_t> C_PuiSdHandlerNodeLogic::GetFileGenAppIndices(const uint32_t ou32_NodeIndex) const
{
   std::vector<uint32_t> c_Return;
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      for (uint32_t u32_ItAppl = 0; u32_ItAppl < rc_Node.c_Applications.size(); ++u32_ItAppl)
      {
         const C_OscNodeApplication & rc_Application = rc_Node.c_Applications[u32_ItAppl];
         if (rc_Application.e_Type != C_OscNodeApplication::eBINARY)
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
   \param[in]  orc_OscContent             OSC data pool list content
                                          (name might be modified by this function if not unique)
   \param[in]  orc_UiContent              UI data pool list content
   \param[in]  oq_HandleSharedDatapools   Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::InsertDataPoolList(const uint32_t & oru32_NodeIndex,
                                                    const uint32_t & oru32_DataPoolIndex,
                                                    const uint32_t & oru32_DataPoolListIndex,
                                                    const C_OscNodeDataPoolList & orc_OscContent,
                                                    const C_PuiSdNodeDataPoolList & orc_UiContent,
                                                    const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size & consistency
         tgl_assert(rc_UiDataPool.c_DataPoolLists.size() == rc_OscDataPool.c_Lists.size());
         if (oru32_DataPoolListIndex <= rc_OscDataPool.c_Lists.size())
         {
            //Unique name
            C_OscNodeDataPoolList c_NodeDataPoolList = orc_OscContent;
            c_NodeDataPoolList.c_Name =
               C_Uti::h_GetUniqueName(this->m_GetExistingNodeDataPoolListNames(oru32_NodeIndex,
                                                                               oru32_DataPoolIndex),
                                      c_NodeDataPoolList.c_Name);
            //Adapt required fields
            if (rc_OscDataPool.q_IsSafety == true)
            {
               c_NodeDataPoolList.q_NvmCrcActive = true;
               for (uint32_t u32_ItElement = 0; u32_ItElement < c_NodeDataPoolList.c_Elements.size(); ++u32_ItElement)
               {
                  C_OscNodeDataPoolListElement & rc_CurElement = c_NodeDataPoolList.c_Elements[u32_ItElement];
                  rc_CurElement.e_Access = C_OscNodeDataPoolListElement::eACCESS_RO;
               }
            }
            //Insert
            rc_UiDataPool.c_DataPoolLists.insert(rc_UiDataPool.c_DataPoolLists.begin() + oru32_DataPoolListIndex,
                                                 orc_UiContent);
            rc_OscDataPool.c_Lists.insert(rc_OscDataPool.c_Lists.begin() + oru32_DataPoolListIndex, c_NodeDataPoolList);

            rc_OscNode.RecalculateAddress();

            //Synchronization engine
            Q_EMIT (this->SigSyncNodeDataPoolListAdded(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex));

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32_t u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size(); ++u32_SharedDpCounter)
                  {
                     const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                     // No error expected. The Datapools must have the same conditions
                     tgl_assert(this->InsertDataPoolList(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                         oru32_DataPoolListIndex,
                                                         orc_OscContent, orc_UiContent, false) == C_NO_ERR);
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
int32_t C_PuiSdHandlerNodeLogic::RemoveDataPoolList(const uint32_t & oru32_NodeIndex,
                                                    const uint32_t & oru32_DataPoolIndex,
                                                    const uint32_t & oru32_DataPoolListIndex,
                                                    const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size & consistency
         tgl_assert(rc_UiDataPool.c_DataPoolLists.size() == rc_OscDataPool.c_Lists.size());
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            //Synchronization engine (First!)
            Q_EMIT (this->SigSyncNodeDataPoolListAboutToBeDeleted(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                  oru32_DataPoolListIndex));
            rc_UiDataPool.c_DataPoolLists.erase(rc_UiDataPool.c_DataPoolLists.begin() + oru32_DataPoolListIndex);
            rc_OscDataPool.c_Lists.erase(rc_OscDataPool.c_Lists.begin() + oru32_DataPoolListIndex);

            rc_OscNode.RecalculateAddress();

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32_t u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size(); ++u32_SharedDpCounter)
                  {
                     const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
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
   \param[in]  orc_OscContent             OSC data pool list content
   \param[in]  orc_UiContent              UI data pool list content
   \param[in]  oq_HandleSharedDatapools   Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
   C_NOACT  Input invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::SetDataPoolList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                                 const uint32_t & oru32_DataPoolListIndex,
                                                 const C_OscNodeDataPoolList & orc_OscContent,
                                                 const C_PuiSdNodeDataPoolList & orc_UiContent,
                                                 const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size & consistency
         tgl_assert(rc_UiDataPool.c_DataPoolLists.size() == rc_OscDataPool.c_Lists.size());
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            if (orc_UiContent.c_DataPoolListElements.size() == orc_OscContent.c_Elements.size())
            {
               rc_UiDataPool.c_DataPoolLists[oru32_DataPoolListIndex] = orc_UiContent;
               rc_OscDataPool.c_Lists[oru32_DataPoolListIndex] = orc_OscContent;
               // Adapt required fields
               if (rc_OscDataPool.q_IsSafety)
               {
                  C_OscNodeDataPoolList & rc_List = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];
                  rc_List.q_NvmCrcActive = true;
                  for (uint32_t u32_ItEl = 0UL; u32_ItEl < rc_List.c_Elements.size(); ++u32_ItEl)
                  {
                     rc_List.c_Elements[u32_ItEl].e_Access = C_OscNodeDataPoolListElement::eACCESS_RO;
                  }
               }
               //Update addresses
               rc_OscDataPool.RecalculateAddress();

               // Handle shared Datapools
               if (oq_HandleSharedDatapools == true)
               {
                  this->m_SetDataPoolListSharedSync(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                    orc_OscContent, orc_UiContent);
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
int32_t C_PuiSdHandlerNodeLogic::SetDataPoolListName(const uint32_t & oru32_NodeIndex,
                                                     const uint32_t & oru32_DataPoolIndex,
                                                     const uint32_t & oru32_DataPoolListIndex,
                                                     const QString & orc_Value, const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size & consistency
         tgl_assert(rc_UiDataPool.c_DataPoolLists.size() == rc_OscDataPool.c_Lists.size());
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];
            rc_OscList.c_Name = orc_Value.toStdString().c_str();
            //Update addresses
            rc_OscDataPool.RecalculateAddress();

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32_t u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                       ++u32_SharedDpCounter)
                  {
                     const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
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
int32_t C_PuiSdHandlerNodeLogic::SetDataPoolListComment(const uint32_t & oru32_NodeIndex,
                                                        const uint32_t & oru32_DataPoolIndex,
                                                        const uint32_t & oru32_DataPoolListIndex,
                                                        const QString & orc_Value, const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size & consistency
         tgl_assert(rc_UiDataPool.c_DataPoolLists.size() == rc_OscDataPool.c_Lists.size());
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];
            rc_OscList.c_Comment = orc_Value.toStdString().c_str();
            //Update addresses
            rc_OscDataPool.RecalculateAddress();

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32_t u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                       ++u32_SharedDpCounter)
                  {
                     const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
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
int32_t C_PuiSdHandlerNodeLogic::SetDataPoolListNvmSize(const uint32_t & oru32_NodeIndex,
                                                        const uint32_t & oru32_DataPoolIndex,
                                                        const uint32_t & oru32_DataPoolListIndex,
                                                        const uint32_t ou32_Value, const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size & consistency
         tgl_assert(rc_UiDataPool.c_DataPoolLists.size() == rc_OscDataPool.c_Lists.size());
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];
            rc_OscList.u32_NvmSize = ou32_Value;
            //Update addresses
            rc_OscDataPool.RecalculateAddress();

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32_t u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                       ++u32_SharedDpCounter)
                  {
                     const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                     // No error expected. The Datapools must have the same conditions
                     tgl_assert(this->SetDataPoolListNvmSize(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
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
int32_t C_PuiSdHandlerNodeLogic::SetDataPoolListNvmCrc(const uint32_t & oru32_NodeIndex,
                                                       const uint32_t & oru32_DataPoolIndex,
                                                       const uint32_t & oru32_DataPoolListIndex, const bool oq_Value,
                                                       const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size & consistency
         tgl_assert(rc_UiDataPool.c_DataPoolLists.size() == rc_OscDataPool.c_Lists.size());
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];
            //Adapt required fields
            if (rc_OscDataPool.q_IsSafety)
            {
               rc_OscList.q_NvmCrcActive = true;
            }
            else
            {
               rc_OscList.q_NvmCrcActive = oq_Value;
            }
            //Update addresses
            rc_OscDataPool.RecalculateAddress();

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               this->m_SetDataPoolListNvmCrcSharedSync(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                       oq_Value);
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
   \param[out]  orc_OscContent            OSC data pool list content
   \param[out]  orc_UiContent             UI data pool list content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::GetDataPoolList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                                 const uint32_t & oru32_DataPoolListIndex,
                                                 C_OscNodeDataPoolList & orc_OscContent,
                                                 C_PuiSdNodeDataPoolList & orc_UiContent) const
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNodeDataPoolList * const pc_OscList = GetOscDataPoolList(oru32_NodeIndex,
                                                                       oru32_DataPoolIndex,
                                                                       oru32_DataPoolListIndex);
   const C_PuiSdNodeDataPoolList * const pc_UiList = GetUiDataPoolList(oru32_NodeIndex,
                                                                       oru32_DataPoolIndex,
                                                                       oru32_DataPoolListIndex);

   if ((pc_OscList != NULL) && (pc_UiList != NULL))
   {
      orc_UiContent = *pc_UiList;
      orc_OscContent = *pc_OscList;
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
   \param[out]  orc_OscName                     OSC data set name
   \param[out]  orc_OscValues                   OSC data set values

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::GetDataPoolListDataSet(const uint32_t & oru32_NodeIndex,
                                                        const uint32_t & oru32_DataPoolIndex,
                                                        const uint32_t & oru32_DataPoolListIndex,
                                                        const uint32_t & oru32_DataPoolListDataSetIndex,
                                                        C_OscNodeDataPoolDataSet & orc_OscName,
                                                        std::vector<C_OscNodeDataPoolContent> & orc_OscValues) const
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNodeDataPoolList * const pc_OscList = GetOscDataPoolList(oru32_NodeIndex,
                                                                       oru32_DataPoolIndex,
                                                                       oru32_DataPoolListIndex);

   if (pc_OscList != NULL)
   {
      if (oru32_DataPoolListDataSetIndex < pc_OscList->c_DataSets.size())
      {
         orc_OscName = pc_OscList->c_DataSets[oru32_DataPoolListDataSetIndex];
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      //Values
      orc_OscValues.resize(pc_OscList->c_Elements.size());
      for (uint32_t u32_ItDataElement = 0; u32_ItDataElement < pc_OscList->c_Elements.size(); ++u32_ItDataElement)
      {
         const C_OscNodeDataPoolListElement & rc_Element = pc_OscList->c_Elements[u32_ItDataElement];
         if (oru32_DataPoolListDataSetIndex < rc_Element.c_DataSetValues.size())
         {
            orc_OscValues[u32_ItDataElement] = rc_Element.c_DataSetValues[oru32_DataPoolListDataSetIndex];
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
const C_OscNodeDataPoolDataSet * C_PuiSdHandlerNodeLogic::GetOscDataPoolListDataSet(const uint32_t & oru32_NodeIndex,
                                                                                    const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_DataPoolListIndex,
                                                                                    const uint32_t & oru32_DataPoolListDataSetIndex)
const
{
   const C_OscNodeDataPoolDataSet * pc_Retval = NULL;
   const C_OscNodeDataPoolList * const pc_OscList = this->GetOscDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                             oru32_DataPoolListIndex);

   if (pc_OscList != NULL)
   {
      if (oru32_DataPoolListDataSetIndex < pc_OscList->c_DataSets.size())
      {
         pc_Retval = &pc_OscList->c_DataSets[oru32_DataPoolListDataSetIndex];
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
   \param[in]  orc_OscContent                   OSC data pool list element content
   \param[in]  oq_HandleSharedDatapools         Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::SetOscNodeDataPoolDataSet(const uint32_t & oru32_NodeIndex,
                                                           const uint32_t & oru32_DataPoolIndex,
                                                           const uint32_t & oru32_DataPoolListIndex,
                                                           const uint32_t & oru32_DataPoolListDataSetIndex,
                                                           const C_OscNodeDataPoolDataSet & orc_OscContent,
                                                           const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if (oru32_DataPoolListDataSetIndex < rc_OscList.c_DataSets.size())
            {
               rc_OscList.c_DataSets[oru32_DataPoolListDataSetIndex] = orc_OscContent;

               // Handle shared Datapools
               if (oq_HandleSharedDatapools == true)
               {
                  std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

                  if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                           c_SharedDatapools) == true)
                  {
                     uint32_t u32_SharedDpCounter;

                     // Synchronize with the shared Datapools
                     for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                          ++u32_SharedDpCounter)
                     {
                        const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                        // No error expected. The Datapools must have the same conditions
                        tgl_assert(this->SetOscNodeDataPoolDataSet(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                                   oru32_DataPoolListIndex,
                                                                   oru32_DataPoolListDataSetIndex,
                                                                   orc_OscContent, false) == C_NO_ERR);
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
   \param[in]  orc_OscName                      OSC data pool list data set name
                                                (name might be modified by this function if not unique)
   \param[in]  orc_OscValues                    OSC data pool list data set values
   \param[in]  oq_HandleSharedDatapools         Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::InsertDataPoolListDataSet(const uint32_t & oru32_NodeIndex,
                                                           const uint32_t & oru32_DataPoolIndex,
                                                           const uint32_t & oru32_DataPoolListIndex,
                                                           const uint32_t & oru32_DataPoolListDataSetIndex,
                                                           const C_OscNodeDataPoolDataSet & orc_OscName,
                                                           const std::vector<C_OscNodeDataPoolContent> & orc_OscValues,
                                                           const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if (oru32_DataPoolListDataSetIndex <= rc_OscList.c_DataSets.size())
            {
               //Unique name
               C_OscNodeDataPoolDataSet c_NodeDataPoolDataSet = orc_OscName;
               c_NodeDataPoolDataSet.c_Name =
                  C_Uti::h_GetUniqueName(this->m_GetExistingNodeDataPoolListDataSetNames(oru32_NodeIndex,
                                                                                         oru32_DataPoolIndex,
                                                                                         oru32_DataPoolListIndex),
                                         c_NodeDataPoolDataSet.c_Name);
               //Insert
               rc_OscList.c_DataSets.insert(
                  rc_OscList.c_DataSets.begin() + oru32_DataPoolListDataSetIndex, c_NodeDataPoolDataSet);
            }
            else
            {
               s32_Retval = C_RANGE;
            }
            //Values
            tgl_assert(rc_OscList.c_Elements.size() == orc_OscValues.size());
            for (uint32_t u32_ItDataElement = 0; u32_ItDataElement < rc_OscList.c_Elements.size();
                 ++u32_ItDataElement)
            {
               C_OscNodeDataPoolListElement & rc_DataElement = rc_OscList.c_Elements[u32_ItDataElement];
               //Check size
               if (oru32_DataPoolListDataSetIndex <= rc_DataElement.c_DataSetValues.size())
               {
                  rc_DataElement.c_DataSetValues.insert(
                     rc_DataElement.c_DataSetValues.begin() + oru32_DataPoolListDataSetIndex,
                     orc_OscValues[u32_ItDataElement]);
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
               std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32_t u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size(); ++u32_SharedDpCounter)
                  {
                     const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                     // No error expected. The Datapools must have the same conditions
                     tgl_assert(this->InsertDataPoolListDataSet(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                                oru32_DataPoolListIndex, oru32_DataPoolListDataSetIndex,
                                                                orc_OscName, orc_OscValues, false) == C_NO_ERR);
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
int32_t C_PuiSdHandlerNodeLogic::RemoveDataPoolListDataSet(const uint32_t & oru32_NodeIndex,
                                                           const uint32_t & oru32_DataPoolIndex,
                                                           const uint32_t & oru32_DataPoolListIndex,
                                                           const uint32_t & oru32_DataPoolListDataSetIndex,
                                                           const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];

            //Synchronization engine (First!)
            Q_EMIT (this->SigSyncNodeDataPoolListDataSetAboutToBeDeleted(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                         oru32_DataPoolListIndex,
                                                                         oru32_DataPoolListDataSetIndex));

            //Check size
            if (oru32_DataPoolListDataSetIndex <= rc_OscList.c_DataSets.size())
            {
               rc_OscList.c_DataSets.erase(rc_OscList.c_DataSets.begin() + oru32_DataPoolListDataSetIndex);
            }
            else
            {
               s32_Retval = C_RANGE;
            }
            //Values
            for (uint32_t u32_ItDataElement = 0; u32_ItDataElement < rc_OscList.c_Elements.size();
                 ++u32_ItDataElement)
            {
               C_OscNodeDataPoolListElement & rc_DataElement = rc_OscList.c_Elements[u32_ItDataElement];
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
               std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32_t u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size(); ++u32_SharedDpCounter)
                  {
                     const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
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
int32_t C_PuiSdHandlerNodeLogic::MoveDataPoolListDataSet(const uint32_t & oru32_NodeIndex,
                                                         const uint32_t & oru32_DataPoolIndex,
                                                         const uint32_t & oru32_DataPoolListIndex,
                                                         const uint32_t & oru32_SourceIndex,
                                                         const uint32_t & oru32_TargetIndex,
                                                         const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if ((oru32_SourceIndex < rc_OscList.c_DataSets.size()) &&
                (oru32_TargetIndex < rc_OscList.c_DataSets.size()))
            {
               //Copy
               const C_OscNodeDataPoolDataSet c_Data = rc_OscList.c_DataSets[oru32_SourceIndex];
               //Erase
               rc_OscList.c_DataSets.erase(rc_OscList.c_DataSets.begin() + oru32_SourceIndex);
               //Insert
               rc_OscList.c_DataSets.insert(rc_OscList.c_DataSets.begin() + oru32_TargetIndex, c_Data);
            }
            else
            {
               s32_Retval = C_RANGE;
            }
            //Values
            for (uint32_t u32_ItDataElement = 0; u32_ItDataElement < rc_OscList.c_Elements.size();
                 ++u32_ItDataElement)
            {
               C_OscNodeDataPoolListElement & rc_DataElement = rc_OscList.c_Elements[u32_ItDataElement];
               //Check size
               if ((oru32_SourceIndex < rc_DataElement.c_DataSetValues.size()) &&
                   (oru32_TargetIndex < rc_DataElement.c_DataSetValues.size()))
               {
                  //Copy
                  const C_OscNodeDataPoolContent c_Data = rc_DataElement.c_DataSetValues[oru32_SourceIndex];
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
               std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32_t u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size(); ++u32_SharedDpCounter)
                  {
                     const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
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
const C_OscNodeDataPoolList * C_PuiSdHandlerNodeLogic::GetOscDataPoolList(const uint32_t & oru32_NodeIndex,
                                                                          const uint32_t & oru32_DataPoolIndex,
                                                                          const uint32_t & oru32_DataPoolListIndex)
const
{
   const C_OscNodeDataPoolList * pc_Retval = NULL;
   const C_OscNodeDataPool * const pc_OscDataPool = this->GetOscDataPool(oru32_NodeIndex, oru32_DataPoolIndex);

   if (pc_OscDataPool != NULL)
   {
      if (oru32_DataPoolListIndex < pc_OscDataPool->c_Lists.size())
      {
         pc_Retval = &pc_OscDataPool->c_Lists[oru32_DataPoolListIndex];
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
const C_PuiSdNodeDataPoolList * C_PuiSdHandlerNodeLogic::GetUiDataPoolList(const uint32_t & oru32_NodeIndex,
                                                                           const uint32_t & oru32_DataPoolIndex,
                                                                           const uint32_t & oru32_DataPoolListIndex)
const
{
   const C_PuiSdNodeDataPoolList * pc_Retval = NULL;
   const C_PuiSdNodeDataPool * const pc_UiDataPool = this->GetUiDataPool(oru32_NodeIndex, oru32_DataPoolIndex);

   if (pc_UiDataPool != NULL)
   {
      if (oru32_DataPoolListIndex < pc_UiDataPool->c_DataPoolLists.size())
      {
         pc_Retval = &pc_UiDataPool->c_DataPoolLists[oru32_DataPoolListIndex];
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
int32_t C_PuiSdHandlerNodeLogic::MoveDataPoolList(const uint32_t & oru32_NodeIndex,
                                                  const uint32_t & oru32_DataPoolIndex,
                                                  const uint32_t & oru32_SourceIndex,
                                                  const uint32_t & oru32_TargetIndex,
                                                  const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];
         rc_OscDataPool.MoveList(oru32_SourceIndex, oru32_TargetIndex);
         if ((oru32_SourceIndex < rc_UiDataPool.c_DataPoolLists.size()) &&
             (oru32_TargetIndex < rc_UiDataPool.c_DataPoolLists.size()))
         {
            //Copy
            const C_PuiSdNodeDataPoolList c_Data = rc_UiDataPool.c_DataPoolLists[oru32_SourceIndex];
            //Erase
            rc_UiDataPool.c_DataPoolLists.erase(rc_UiDataPool.c_DataPoolLists.begin() + oru32_SourceIndex);
            //Insert
            rc_UiDataPool.c_DataPoolLists.insert(rc_UiDataPool.c_DataPoolLists.begin() + oru32_TargetIndex, c_Data);

            //Synchronization engine
            Q_EMIT (this->SigSyncNodeDataPoolListMoved(oru32_NodeIndex, oru32_DataPoolIndex, oru32_SourceIndex,
                                                       oru32_TargetIndex));

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32_t u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size(); ++u32_SharedDpCounter)
                  {
                     const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
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
bool C_PuiSdHandlerNodeLogic::CheckNodeDataPoolListNameAvailable(const uint32_t & oru32_NodeIndex,
                                                                 const uint32_t & oru32_DataPoolIndex,
                                                                 const C_SclString & orc_Name,
                                                                 const uint32_t * const opu32_DataPoolListIndexToSkip)
const
{
   bool q_Retval = true;
   const C_OscNodeDataPool * const pc_DataPool = this->GetOscDataPool(oru32_NodeIndex, oru32_DataPoolIndex);

   if (pc_DataPool != NULL)
   {
      for (uint32_t u32_ItList = 0; (u32_ItList < pc_DataPool->c_Lists.size()) && (q_Retval == true);
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
            const C_OscNodeDataPoolList & rc_List = pc_DataPool->c_Lists[u32_ItList];
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
bool C_PuiSdHandlerNodeLogic::CheckNodeDataPoolListDataSetNameAvailable(const uint32_t & oru32_NodeIndex,
                                                                        const uint32_t & oru32_DataPoolIndex,
                                                                        const uint32_t & oru32_ListIndex,
                                                                        const C_SclString & orc_Name,
                                                                        const uint32_t * const opu32_DataPoolListDataSetIndexToSkip)
const
{
   bool q_Retval = true;
   const C_OscNodeDataPoolList * const pc_DataPoolList = this->GetOscDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                                  oru32_ListIndex);

   if (pc_DataPoolList != NULL)
   {
      for (uint32_t u32_ItDataSet = 0; (u32_ItDataSet < pc_DataPoolList->c_DataSets.size()) && (q_Retval == true);
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
            const C_OscNodeDataPoolDataSet & rc_DataSet = pc_DataPoolList->c_DataSets[u32_ItDataSet];
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
int32_t C_PuiSdHandlerNodeLogic::ReserveDataPoolListElements(const uint32_t & oru32_NodeIndex,
                                                             const uint32_t & oru32_DataPoolIndex,
                                                             const uint32_t & oru32_DataPoolListIndex,
                                                             const uint32_t & oru32_AdditionalElements,
                                                             const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size & consistency
         tgl_assert(rc_UiDataPool.c_DataPoolLists.size() == rc_OscDataPool.c_Lists.size());
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_PuiSdNodeDataPoolList & rc_UiList = rc_UiDataPool.c_DataPoolLists[oru32_DataPoolListIndex];
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];
            //Reserve
            rc_UiList.c_DataPoolListElements.reserve(
               static_cast<std::vector<C_OscNodeDataPoolListElement>::size_type>(
                  rc_UiList.c_DataPoolListElements.size() +
                  oru32_AdditionalElements));
            rc_OscList.c_Elements.reserve(
               static_cast<std::vector<C_PuiSdNodeDataPoolListElement>::size_type>(rc_OscList
                                                                                   .
                                                                                   c_Elements
                                                                                   .size() +
                                                                                   oru32_AdditionalElements));

            // Handle shared Datapools
            if (oq_HandleSharedDatapools == true)
            {
               std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

               if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        c_SharedDatapools) == true)
               {
                  uint32_t u32_SharedDpCounter;

                  // Synchronize with the shared Datapools
                  for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                       ++u32_SharedDpCounter)
                  {
                     const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
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
   \param[in]  orc_OscContent                   OSC data pool list element content
                                                (name might be modified by this function if not unique)
   \param[in]  orc_UiContent                    UI data pool list element content
   \param[in]  oq_HandleSharedDatapools         Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::InsertDataPoolListElement(const uint32_t & oru32_NodeIndex,
                                                           const uint32_t & oru32_DataPoolIndex,
                                                           const uint32_t & oru32_DataPoolListIndex,
                                                           const uint32_t & oru32_DataPoolListElementIndex,
                                                           const C_OscNodeDataPoolListElement & orc_OscContent,
                                                           const C_PuiSdNodeDataPoolListElement & orc_UiContent,
                                                           const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size & consistency
         tgl_assert(rc_UiDataPool.c_DataPoolLists.size() == rc_OscDataPool.c_Lists.size());
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_PuiSdNodeDataPoolList & rc_UiList = rc_UiDataPool.c_DataPoolLists[oru32_DataPoolListIndex];
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size & consistency
            tgl_assert(rc_UiList.c_DataPoolListElements.size() == rc_OscList.c_Elements.size());
            if (oru32_DataPoolListElementIndex <= rc_UiList.c_DataPoolListElements.size())
            {
               //Unique name
               C_OscNodeDataPoolListElement c_NodeDataPoolListElement = orc_OscContent;
               c_NodeDataPoolListElement.c_Name =
                  C_Uti::h_GetUniqueName(this->m_GetExistingNodeDataPoolListVariableNames(oru32_NodeIndex,
                                                                                          oru32_DataPoolIndex,
                                                                                          oru32_DataPoolListIndex),
                                         c_NodeDataPoolListElement.c_Name);
               //Adapt required fields
               if (rc_OscDataPool.q_IsSafety == true)
               {
                  c_NodeDataPoolListElement.e_Access = C_OscNodeDataPoolListElement::eACCESS_RO;
               }
               //Insert
               rc_UiList.c_DataPoolListElements.insert(
                  rc_UiList.c_DataPoolListElements.begin() + oru32_DataPoolListElementIndex,
                  orc_UiContent);
               rc_OscList.c_Elements.insert(rc_OscList.c_Elements.begin() + oru32_DataPoolListElementIndex,
                                            c_NodeDataPoolListElement);

               rc_OscNode.RecalculateAddress();

               //Synchronization engine
               Q_EMIT (this->SigSyncNodeDataPoolListElementAdded(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                 oru32_DataPoolListIndex,
                                                                 oru32_DataPoolListElementIndex));

               // Handle shared Datapools
               if (oq_HandleSharedDatapools == true)
               {
                  std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

                  if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                           c_SharedDatapools) == true)
                  {
                     uint32_t u32_SharedDpCounter;

                     // Synchronize with the shared Datapools
                     for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                          ++u32_SharedDpCounter)
                     {
                        const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                        // No error expected. The Datapools must have the same conditions
                        tgl_assert(this->InsertDataPoolListElement(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                                   oru32_DataPoolListIndex,
                                                                   oru32_DataPoolListElementIndex,
                                                                   orc_OscContent, orc_UiContent, false) == C_NO_ERR);
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
int32_t C_PuiSdHandlerNodeLogic::RemoveDataPoolListElement(const uint32_t & oru32_NodeIndex,
                                                           const uint32_t & oru32_DataPoolIndex,
                                                           const uint32_t & oru32_DataPoolListIndex,
                                                           const uint32_t & oru32_DataPoolListElementIndex,
                                                           const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size & consistency
         tgl_assert(rc_UiDataPool.c_DataPoolLists.size() == rc_OscDataPool.c_Lists.size());
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_PuiSdNodeDataPoolList & rc_UiList = rc_UiDataPool.c_DataPoolLists[oru32_DataPoolListIndex];
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size & consistency
            tgl_assert(rc_UiList.c_DataPoolListElements.size() == rc_OscList.c_Elements.size());
            if (oru32_DataPoolListElementIndex < rc_UiList.c_DataPoolListElements.size())
            {
               //Synchronization engine (First!)
               Q_EMIT (this->SigSyncNodeDataPoolListElementAboutToBeDeleted(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                            oru32_DataPoolListIndex,
                                                                            oru32_DataPoolListElementIndex));
               rc_UiList.c_DataPoolListElements.erase(rc_UiList.c_DataPoolListElements.begin() +
                                                      oru32_DataPoolListElementIndex);
               rc_OscList.c_Elements.erase(rc_OscList.c_Elements.begin() + oru32_DataPoolListElementIndex);

               rc_OscNode.RecalculateAddress();

               // Handle shared Datapools
               if (oq_HandleSharedDatapools == true)
               {
                  std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

                  if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                           c_SharedDatapools) == true)
                  {
                     uint32_t u32_SharedDpCounter;

                     // Synchronize with the shared Datapools
                     for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                          ++u32_SharedDpCounter)
                     {
                        const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
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
   \param[in]  orc_OscContent                   OSC data pool list element content
   \param[in]  orc_UiContent                    UI data pool list element content
   \param[in]  oq_HandleSharedDatapools         Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::SetDataPoolListElement(const uint32_t & oru32_NodeIndex,
                                                        const uint32_t & oru32_DataPoolIndex,
                                                        const uint32_t & oru32_DataPoolListIndex,
                                                        const uint32_t & oru32_DataPoolListElementIndex,
                                                        const C_OscNodeDataPoolListElement & orc_OscContent,
                                                        const C_PuiSdNodeDataPoolListElement & orc_UiContent,
                                                        const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size & consistency
         tgl_assert(rc_UiDataPool.c_DataPoolLists.size() == rc_OscDataPool.c_Lists.size());
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_PuiSdNodeDataPoolList & rc_UiList = rc_UiDataPool.c_DataPoolLists[oru32_DataPoolListIndex];
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size & consistency
            tgl_assert(rc_UiList.c_DataPoolListElements.size() == rc_OscList.c_Elements.size());
            if (oru32_DataPoolListElementIndex < rc_UiList.c_DataPoolListElements.size())
            {
               C_OscNodeDataPoolListElement & rc_OscDataElement = rc_OscList.c_Elements[oru32_DataPoolListElementIndex];
               C_OscNodeDataPoolListElement c_NewValuesContent = orc_OscContent;
               //Adapt required fields
               if (rc_OscDataPool.q_IsSafety)
               {
                  c_NewValuesContent.e_Access = C_OscNodeDataPoolListElement::eACCESS_RO;
               }
               if ((rc_OscDataElement.GetType() != c_NewValuesContent.GetType()) ||
                   ((rc_OscDataElement.GetArray() != c_NewValuesContent.GetArray()) ||
                    (rc_OscDataElement.GetArraySize() !=
                     c_NewValuesContent.GetArraySize())))
               {
                  Q_EMIT this->SigSyncNodeDataPoolListElementTypeChanged(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                         oru32_DataPoolListIndex,
                                                                         oru32_DataPoolListElementIndex,
                                                                         c_NewValuesContent.GetType(),
                                                                         c_NewValuesContent.GetArray(),
                                                                         c_NewValuesContent.GetArraySize(),
                                                                         orc_UiContent.q_InterpretAsString);
               }
               if ((rc_OscDataElement.c_MinValue != orc_OscContent.c_MinValue) ||
                   (rc_OscDataElement.c_MaxValue != orc_OscContent.c_MaxValue))
               {
                  Q_EMIT (this->SigSyncNodeDataPoolListElementRangeChanged(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                           oru32_DataPoolListIndex,
                                                                           oru32_DataPoolListElementIndex,
                                                                           orc_OscContent.c_MinValue,
                                                                           orc_OscContent.c_MaxValue));
               }
               if (rc_OscDataElement.e_Access != c_NewValuesContent.e_Access)
               {
                  Q_EMIT this->SigSyncNodeDataPoolListElementAccessChanged(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                           oru32_DataPoolListIndex,
                                                                           oru32_DataPoolListElementIndex,
                                                                           c_NewValuesContent.e_Access);
               }
               rc_UiList.c_DataPoolListElements[oru32_DataPoolListElementIndex] = orc_UiContent;
               rc_OscDataElement = c_NewValuesContent;
               //Update addresses
               rc_OscDataPool.RecalculateAddress();

               // Handle shared Datapools
               if (oq_HandleSharedDatapools == true)
               {
                  this->m_SetDataPoolListElementSharedSync(oru32_NodeIndex, oru32_DataPoolIndex,
                                                           oru32_DataPoolListIndex, oru32_DataPoolListElementIndex,
                                                           c_NewValuesContent,
                                                           orc_UiContent);
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
   \param[out]  orc_OscContent                  OSC data pool list element content
   \param[out]  orc_UiContent                   UI data pool list element content

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::GetDataPoolListElement(const uint32_t & oru32_NodeIndex,
                                                        const uint32_t & oru32_DataPoolIndex,
                                                        const uint32_t & oru32_DataPoolListIndex,
                                                        const uint32_t & oru32_DataPoolListElementIndex,
                                                        C_OscNodeDataPoolListElement & orc_OscContent,
                                                        C_PuiSdNodeDataPoolListElement & orc_UiContent) const
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNodeDataPoolListElement * const pc_OscListElement = GetOscDataPoolListElement(oru32_NodeIndex,
                                                                                            oru32_DataPoolIndex,
                                                                                            oru32_DataPoolListIndex,
                                                                                            oru32_DataPoolListElementIndex);
   const C_PuiSdNodeDataPoolListElement * const pc_UiListElement = GetUiDataPoolListElement(oru32_NodeIndex,
                                                                                            oru32_DataPoolIndex,
                                                                                            oru32_DataPoolListIndex,
                                                                                            oru32_DataPoolListElementIndex);

   if ((pc_OscListElement != NULL) && (pc_UiListElement != NULL))
   {
      orc_UiContent = *pc_UiListElement;
      orc_OscContent = *pc_OscListElement;
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
   \param[in,out]  orc_OscElement   Data element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::h_InitDataElement(const C_OscNodeDataPool::E_Type & ore_Type, const bool oq_IsSafety,
                                                C_OscNodeDataPoolListElement & orc_OscElement)
{
   const QString c_Type =
      C_PuiSdHandler::h_AutomaticCeStringAdaptation(C_PuiSdHandlerNodeLogic::h_GetElementTypeName(ore_Type));

   //Translation: 1: Data element type
   orc_OscElement.c_Name = static_cast<QString>(static_cast<QString>("New%1").arg(c_Type)).toStdString().c_str();
   //Translation: 1: Data element type
   orc_OscElement.c_Comment =
      static_cast<QString>(static_cast<QString>("%1 description").arg(c_Type)).toStdString().c_str();
   //Default access value
   if (ore_Type == C_OscNodeDataPool::eDIAG)
   {
      orc_OscElement.e_Access = C_OscNodeDataPoolListElement::eACCESS_RO;
   }
   else
   {
      if (oq_IsSafety == false)
      {
         orc_OscElement.e_Access = C_OscNodeDataPoolListElement::eACCESS_RW;
      }
      else
      {
         orc_OscElement.e_Access = C_OscNodeDataPoolListElement::eACCESS_RO;
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
QString C_PuiSdHandlerNodeLogic::h_GetElementTypeName(const C_OscNodeDataPool::E_Type & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case C_OscNodeDataPool::eDIAG:
      c_Retval = "Variable";
      break;
   case C_OscNodeDataPool::eNVM:
      c_Retval = "Parameter";
      break;
   case C_OscNodeDataPool::eCOM:
      c_Retval = "Signal";
      break;
   case C_OscNodeDataPool::eHALC:
   case C_OscNodeDataPool::eHALC_NVM:
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
   \param[in]  orc_OscContent                   New value (Expected: No array & same type as in element)
   \param[in]  oq_HandleSharedDatapools         Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::SetDataPoolListElementMinArray(const uint32_t & oru32_NodeIndex,
                                                                const uint32_t & oru32_DataPoolIndex,
                                                                const uint32_t & oru32_DataPoolListIndex,
                                                                const uint32_t & oru32_DataPoolListElementIndex,
                                                                const uint32_t & oru32_ArrayIndex,
                                                                const C_OscNodeDataPoolContent & orc_OscContent,
                                                                const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if (oru32_DataPoolListElementIndex < rc_OscList.c_Elements.size())
            {
               C_OscNodeDataPoolListElement & rc_Element = rc_OscList.c_Elements[oru32_DataPoolListElementIndex];
               if (oru32_ArrayIndex < rc_Element.GetArraySize())
               {
                  //Check input correct
                  tgl_assert(orc_OscContent.GetType() == rc_Element.GetType());
                  tgl_assert(orc_OscContent.GetArray() == false);
                  switch (orc_OscContent.GetType())
                  {
                  case C_OscNodeDataPoolContent::eUINT8:
                     rc_Element.c_MinValue.SetValueArrU8Element(orc_OscContent.GetValueU8(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eUINT16:
                     rc_Element.c_MinValue.SetValueArrU16Element(orc_OscContent.GetValueU16(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eUINT32:
                     rc_Element.c_MinValue.SetValueArrU32Element(orc_OscContent.GetValueU32(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eUINT64:
                     rc_Element.c_MinValue.SetValueArrU64Element(orc_OscContent.GetValueU64(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eSINT8:
                     rc_Element.c_MinValue.SetValueArrS8Element(orc_OscContent.GetValueS8(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eSINT16:
                     rc_Element.c_MinValue.SetValueArrS16Element(orc_OscContent.GetValueS16(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eSINT32:
                     rc_Element.c_MinValue.SetValueArrS32Element(orc_OscContent.GetValueS32(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eSINT64:
                     rc_Element.c_MinValue.SetValueArrS64Element(orc_OscContent.GetValueS64(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eFLOAT32:
                     rc_Element.c_MinValue.SetValueArrF32Element(orc_OscContent.GetValueF32(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eFLOAT64:
                     rc_Element.c_MinValue.SetValueArrF64Element(orc_OscContent.GetValueF64(), oru32_ArrayIndex);
                     break;
                  default:
                     tgl_assert(false);
                     break;
                  }

                  // Handle shared Datapools
                  if (oq_HandleSharedDatapools == true)
                  {
                     std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

                     if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                              c_SharedDatapools) == true)
                     {
                        uint32_t u32_SharedDpCounter;

                        // Synchronize with the shared Datapools
                        for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                             ++u32_SharedDpCounter)
                        {
                           const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                           // No error expected. The Datapools must have the same conditions
                           tgl_assert(this->SetDataPoolListElementMinArray(rc_DpId.u32_NodeIndex,
                                                                           rc_DpId.u32_DataPoolIndex,
                                                                           oru32_DataPoolListIndex,
                                                                           oru32_DataPoolListElementIndex,
                                                                           oru32_ArrayIndex, orc_OscContent,
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
   \param[in]  orc_OscContent                   New value (Expected: No array & same type as in element)
   \param[in]  oq_HandleSharedDatapools         Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::SetDataPoolListElementMaxArray(const uint32_t & oru32_NodeIndex,
                                                                const uint32_t & oru32_DataPoolIndex,
                                                                const uint32_t & oru32_DataPoolListIndex,
                                                                const uint32_t & oru32_DataPoolListElementIndex,
                                                                const uint32_t & oru32_ArrayIndex,
                                                                const C_OscNodeDataPoolContent & orc_OscContent,
                                                                const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if (oru32_DataPoolListElementIndex < rc_OscList.c_Elements.size())
            {
               C_OscNodeDataPoolListElement & rc_Element = rc_OscList.c_Elements[oru32_DataPoolListElementIndex];
               if (oru32_ArrayIndex < rc_Element.GetArraySize())
               {
                  //Check input correct
                  tgl_assert(orc_OscContent.GetType() == rc_Element.GetType());
                  tgl_assert(orc_OscContent.GetArray() == false);
                  switch (orc_OscContent.GetType())
                  {
                  case C_OscNodeDataPoolContent::eUINT8:
                     rc_Element.c_MaxValue.SetValueArrU8Element(orc_OscContent.GetValueU8(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eUINT16:
                     rc_Element.c_MaxValue.SetValueArrU16Element(orc_OscContent.GetValueU16(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eUINT32:
                     rc_Element.c_MaxValue.SetValueArrU32Element(orc_OscContent.GetValueU32(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eUINT64:
                     rc_Element.c_MaxValue.SetValueArrU64Element(orc_OscContent.GetValueU64(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eSINT8:
                     rc_Element.c_MaxValue.SetValueArrS8Element(orc_OscContent.GetValueS8(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eSINT16:
                     rc_Element.c_MaxValue.SetValueArrS16Element(orc_OscContent.GetValueS16(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eSINT32:
                     rc_Element.c_MaxValue.SetValueArrS32Element(orc_OscContent.GetValueS32(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eSINT64:
                     rc_Element.c_MaxValue.SetValueArrS64Element(orc_OscContent.GetValueS64(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eFLOAT32:
                     rc_Element.c_MaxValue.SetValueArrF32Element(orc_OscContent.GetValueF32(), oru32_ArrayIndex);
                     break;
                  case C_OscNodeDataPoolContent::eFLOAT64:
                     rc_Element.c_MaxValue.SetValueArrF64Element(orc_OscContent.GetValueF64(), oru32_ArrayIndex);
                     break;
                  default:
                     tgl_assert(false);
                     break;
                  }

                  // Handle shared Datapools
                  if (oq_HandleSharedDatapools == true)
                  {
                     std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

                     if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                              c_SharedDatapools) == true)
                     {
                        uint32_t u32_SharedDpCounter;

                        // Synchronize with the shared Datapools
                        for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                             ++u32_SharedDpCounter)
                        {
                           const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                           // No error expected. The Datapools must have the same conditions
                           tgl_assert(this->SetDataPoolListElementMaxArray(rc_DpId.u32_NodeIndex,
                                                                           rc_DpId.u32_DataPoolIndex,
                                                                           oru32_DataPoolListIndex,
                                                                           oru32_DataPoolListElementIndex,
                                                                           oru32_ArrayIndex, orc_OscContent,
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
   \param[in]  orc_OscContent                         New value (Expected: No array & same type as in element)
   \param[in]  oq_HandleSharedDatapools               Optional flag if the shared Datapools shall be updated too

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::SetDataPoolListElementDataSetArray(const uint32_t & oru32_NodeIndex,
                                                                    const uint32_t & oru32_DataPoolIndex,
                                                                    const uint32_t & oru32_DataPoolListIndex,
                                                                    const uint32_t & oru32_DataPoolListElementIndex,
                                                                    const uint32_t & oru32_DataPoolListElementDataSetIndex, const uint32_t & oru32_ArrayIndex, const C_OscNodeDataPoolContent & orc_OscContent,
                                                                    const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if (oru32_DataPoolListElementIndex < rc_OscList.c_Elements.size())
            {
               C_OscNodeDataPoolListElement & rc_Element = rc_OscList.c_Elements[oru32_DataPoolListElementIndex];
               //Check size
               if (oru32_DataPoolListElementDataSetIndex < rc_Element.c_DataSetValues.size())
               {
                  C_OscNodeDataPoolContent & rc_DataSet =
                     rc_Element.c_DataSetValues[oru32_DataPoolListElementDataSetIndex];
                  if (oru32_ArrayIndex < rc_DataSet.GetArraySize())
                  {
                     //Check input correct
                     tgl_assert(orc_OscContent.GetType() == rc_Element.GetType());
                     tgl_assert(orc_OscContent.GetArray() == false);
                     switch (orc_OscContent.GetType())
                     {
                     case C_OscNodeDataPoolContent::eUINT8:
                        rc_DataSet.SetValueArrU8Element(orc_OscContent.GetValueU8(), oru32_ArrayIndex);
                        break;
                     case C_OscNodeDataPoolContent::eUINT16:
                        rc_DataSet.SetValueArrU16Element(orc_OscContent.GetValueU16(), oru32_ArrayIndex);
                        break;
                     case C_OscNodeDataPoolContent::eUINT32:
                        rc_DataSet.SetValueArrU32Element(orc_OscContent.GetValueU32(), oru32_ArrayIndex);
                        break;
                     case C_OscNodeDataPoolContent::eUINT64:
                        rc_DataSet.SetValueArrU64Element(orc_OscContent.GetValueU64(), oru32_ArrayIndex);
                        break;
                     case C_OscNodeDataPoolContent::eSINT8:
                        rc_DataSet.SetValueArrS8Element(orc_OscContent.GetValueS8(), oru32_ArrayIndex);
                        break;
                     case C_OscNodeDataPoolContent::eSINT16:
                        rc_DataSet.SetValueArrS16Element(orc_OscContent.GetValueS16(), oru32_ArrayIndex);
                        break;
                     case C_OscNodeDataPoolContent::eSINT32:
                        rc_DataSet.SetValueArrS32Element(orc_OscContent.GetValueS32(), oru32_ArrayIndex);
                        break;
                     case C_OscNodeDataPoolContent::eSINT64:
                        rc_DataSet.SetValueArrS64Element(orc_OscContent.GetValueS64(), oru32_ArrayIndex);
                        break;
                     case C_OscNodeDataPoolContent::eFLOAT32:
                        rc_DataSet.SetValueArrF32Element(orc_OscContent.GetValueF32(), oru32_ArrayIndex);
                        break;
                     case C_OscNodeDataPoolContent::eFLOAT64:
                        rc_DataSet.SetValueArrF64Element(orc_OscContent.GetValueF64(), oru32_ArrayIndex);
                        break;
                     default:
                        tgl_assert(false);
                        break;
                     }

                     // Handle shared Datapools
                     if (oq_HandleSharedDatapools == true)
                     {
                        std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

                        if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                                 c_SharedDatapools) == true)
                        {
                           uint32_t u32_SharedDpCounter;

                           // Synchronize with the shared Datapools
                           for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                                ++u32_SharedDpCounter)
                           {
                              const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
                              // No error expected. The Datapools must have the same conditions
                              tgl_assert(this->SetDataPoolListElementDataSetArray(rc_DpId.u32_NodeIndex,
                                                                                  rc_DpId.u32_DataPoolIndex,
                                                                                  oru32_DataPoolListIndex,
                                                                                  oru32_DataPoolListElementIndex,
                                                                                  oru32_DataPoolListElementDataSetIndex,
                                                                                  oru32_ArrayIndex, orc_OscContent,
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
   \param[in]  orc_OscContent                   New value

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerNodeLogic::SetDataPoolListElementNvmValue(const uint32_t & oru32_NodeIndex,
                                                                const uint32_t & oru32_DataPoolIndex,
                                                                const uint32_t & oru32_DataPoolListIndex,
                                                                const uint32_t & oru32_DataPoolListElementIndex,
                                                                const C_OscNodeDataPoolContent & orc_OscContent)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if (oru32_DataPoolListElementIndex < rc_OscList.c_Elements.size())
            {
               C_OscNodeDataPoolListElement & rc_Element = rc_OscList.c_Elements[oru32_DataPoolListElementIndex];
               rc_Element.c_NvmValue = orc_OscContent;
               rc_Element.q_NvmValueChanged = true;
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
int32_t C_PuiSdHandlerNodeLogic::SetDataPoolListElementNvmValueChanged(const uint32_t & oru32_NodeIndex,
                                                                       const uint32_t & oru32_DataPoolIndex,
                                                                       const uint32_t & oru32_DataPoolListIndex,
                                                                       const uint32_t & oru32_DataPoolListElementIndex,
                                                                       const bool oq_NvmValueChanged)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size
            if (oru32_DataPoolListElementIndex < rc_OscList.c_Elements.size())
            {
               C_OscNodeDataPoolListElement & rc_Element = rc_OscList.c_Elements[oru32_DataPoolListElementIndex];
               rc_Element.q_NvmValueChanged = oq_NvmValueChanged;
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
const C_OscNodeDataPoolListElement * C_PuiSdHandlerNodeLogic::GetOscDataPoolListElement(
   const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_DataPoolListIndex,
   const uint32_t & oru32_DataPoolListElementIndex)
const
{
   const C_OscNodeDataPoolListElement * pc_Retval = NULL;
   const C_OscNodeDataPoolList * const pc_OscList = this->GetOscDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                             oru32_DataPoolListIndex);

   if (pc_OscList != NULL)
   {
      if (oru32_DataPoolListElementIndex < pc_OscList->c_Elements.size())
      {
         pc_Retval = &pc_OscList->c_Elements[oru32_DataPoolListElementIndex];
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
const C_OscNodeDataPoolListElement * C_PuiSdHandlerNodeLogic::GetOscDataPoolListElement(
   const C_OscNodeDataPoolListElementId & orc_Id) const
{
   return this->GetOscDataPoolListElement(orc_Id.u32_NodeIndex, orc_Id.u32_DataPoolIndex, orc_Id.u32_ListIndex,
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
const C_PuiSdNodeDataPoolListElement * C_PuiSdHandlerNodeLogic::GetUiDataPoolListElement(
   const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_DataPoolListIndex,
   const uint32_t & oru32_DataPoolListElementIndex)
const
{
   const C_PuiSdNodeDataPoolListElement * pc_Retval = NULL;
   const C_PuiSdNodeDataPoolList * const pc_UiList = this->GetUiDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                             oru32_DataPoolListIndex);

   if (pc_UiList != NULL)
   {
      if (oru32_DataPoolListElementIndex < pc_UiList->c_DataPoolListElements.size())
      {
         pc_Retval = &pc_UiList->c_DataPoolListElements[oru32_DataPoolListElementIndex];
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
int32_t C_PuiSdHandlerNodeLogic::MoveDataPoolListElement(const uint32_t & oru32_NodeIndex,
                                                         const uint32_t & oru32_DataPoolIndex,
                                                         const uint32_t & oru32_DataPoolListIndex,
                                                         const uint32_t & oru32_SourceIndex,
                                                         const uint32_t & oru32_TargetIndex,
                                                         const bool oq_HandleSharedDatapools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];

      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];

         //Check size & consistency
         tgl_assert(rc_UiDataPool.c_DataPoolLists.size() == rc_OscDataPool.c_Lists.size());
         if (oru32_DataPoolListIndex < rc_OscDataPool.c_Lists.size())
         {
            C_PuiSdNodeDataPoolList & rc_UiList = rc_UiDataPool.c_DataPoolLists[oru32_DataPoolListIndex];
            C_OscNodeDataPoolList & rc_OscList = rc_OscDataPool.c_Lists[oru32_DataPoolListIndex];

            //Check size & consistency
            tgl_assert(rc_UiList.c_DataPoolListElements.size() == rc_OscList.c_Elements.size());
            if ((oru32_SourceIndex < rc_UiList.c_DataPoolListElements.size()) &&
                (oru32_TargetIndex < rc_UiList.c_DataPoolListElements.size()))
            {
               //Copy
               const C_PuiSdNodeDataPoolListElement c_Data = rc_UiList.c_DataPoolListElements[oru32_SourceIndex];
               rc_OscList.MoveElement(oru32_SourceIndex, oru32_TargetIndex);
               //Erase
               rc_UiList.c_DataPoolListElements.erase(
                  rc_UiList.c_DataPoolListElements.begin() + oru32_SourceIndex);
               //Insert
               rc_UiList.c_DataPoolListElements.insert(
                  rc_UiList.c_DataPoolListElements.begin() + oru32_TargetIndex, c_Data);

               //Synchronization engine
               Q_EMIT this->SigSyncNodeDataPoolListElementMoved(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                oru32_DataPoolListIndex,
                                                                oru32_SourceIndex, oru32_TargetIndex);

               // Handle shared Datapools
               if (oq_HandleSharedDatapools == true)
               {
                  std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

                  if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                           c_SharedDatapools) == true)
                  {
                     uint32_t u32_SharedDpCounter;

                     // Synchronize with the shared Datapools
                     for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
                          ++u32_SharedDpCounter)
                     {
                        const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
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
bool C_PuiSdHandlerNodeLogic::CheckNodeDataPoolListElementNameAvailable(const uint32_t & oru32_NodeIndex,
                                                                        const uint32_t & oru32_DataPoolIndex,
                                                                        const uint32_t & oru32_ListIndex,
                                                                        const C_SclString & orc_Name,
                                                                        const uint32_t * const opu32_DataPoolListElementIndexToSkip)
const
{
   bool q_Retval = true;
   const C_OscNodeDataPoolList * const pc_DataPoolList = this->GetOscDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                                  oru32_ListIndex);

   if (pc_DataPoolList != NULL)
   {
      for (uint32_t u32_ItElement = 0; (u32_ItElement < pc_DataPoolList->c_Elements.size()) && (q_Retval == true);
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
            const C_OscNodeDataPoolListElement & rc_Element = pc_DataPoolList->c_Elements[u32_ItElement];
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
std::map<C_SclString, bool> C_PuiSdHandlerNodeLogic::m_GetExistingNodeNames(void) const
{
   std::map<C_SclString, bool> c_Retval;
   for (uint32_t u32_ItNode = 0; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      uint32_t u32_GroupIndex;
      if (C_OscNodeSquad::h_CheckIsMultiDevice(u32_ItNode, this->mc_CoreDefinition.c_NodeSquads, &u32_GroupIndex))
      {
         // Squad sub node. Only use base name
         tgl_assert(u32_GroupIndex < this->mc_CoreDefinition.c_NodeSquads.size());
         if (u32_GroupIndex < this->mc_CoreDefinition.c_NodeSquads.size())
         {
            const C_SclString & rc_CurrentName = this->mc_CoreDefinition.c_NodeSquads[u32_GroupIndex].c_BaseName;
            c_Retval[rc_CurrentName] = true;
         }
      }
      else
      {
         // Normal node
         const C_SclString & rc_CurrentName = this->mc_CoreDefinition.c_Nodes[u32_ItNode].c_Properties.c_Name;
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
std::map<C_SclString,
         bool> C_PuiSdHandlerNodeLogic::m_GetExistingNodeApplicationNames(const uint32_t & oru32_NodeIndex) const
{
   std::map<C_SclString, bool> c_Retval;
   const C_OscNode * const pc_Node = this->GetOscNodeConst(oru32_NodeIndex);
   if (pc_Node != NULL)
   {
      for (uint32_t u32_ItApplication = 0; u32_ItApplication < pc_Node->c_Applications.size(); ++u32_ItApplication)
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
std::map<C_SclString,
         bool> C_PuiSdHandlerNodeLogic::m_GetExistingNodeDataPoolNames(const uint32_t & oru32_NodeIndex) const
{
   std::map<C_SclString, bool> c_Retval;
   const C_OscNode * const pc_Node = this->GetOscNodeConst(oru32_NodeIndex);
   if (pc_Node != NULL)
   {
      for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
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
std::map<C_SclString, bool> C_PuiSdHandlerNodeLogic::m_GetExistingNodeDataPoolListNames(
   const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex)
const
{
   std::map<C_SclString, bool> c_Retval;
   const C_OscNodeDataPool * const pc_NodeDataPool = this->GetOscDataPool(oru32_NodeIndex, oru32_DataPoolIndex);
   if (pc_NodeDataPool != NULL)
   {
      for (uint32_t u32_ItDataPoolList = 0; u32_ItDataPoolList < pc_NodeDataPool->c_Lists.size(); ++u32_ItDataPoolList)
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
std::map<C_SclString, bool> C_PuiSdHandlerNodeLogic::m_GetExistingNodeDataPoolListDataSetNames(
   const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_DataPoolListIndex)
const
{
   std::map<C_SclString, bool> c_Retval;
   const C_OscNodeDataPoolList * const pc_NodeDataPoolList = this->GetOscDataPoolList(oru32_NodeIndex,
                                                                                      oru32_DataPoolIndex,
                                                                                      oru32_DataPoolListIndex);
   if (pc_NodeDataPoolList != NULL)
   {
      for (uint32_t u32_ItDataPoolListElement = 0; u32_ItDataPoolListElement < pc_NodeDataPoolList->c_DataSets.size();
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
std::map<C_SclString, bool> C_PuiSdHandlerNodeLogic::m_GetExistingNodeDataPoolListVariableNames(
   const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_DataPoolListIndex)
const
{
   std::map<C_SclString, bool> c_Retval;
   const C_OscNodeDataPoolList * const pc_NodeDataPoolList = this->GetOscDataPoolList(oru32_NodeIndex,
                                                                                      oru32_DataPoolIndex,
                                                                                      oru32_DataPoolListIndex);
   if (pc_NodeDataPoolList != NULL)
   {
      for (uint32_t u32_ItDataPoolListElement = 0; u32_ItDataPoolListElement < pc_NodeDataPoolList->c_Elements.size();
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
void C_PuiSdHandlerNodeLogic::m_SyncNodeApplicationAdded(const uint32_t ou32_NodeIndex,
                                                         const uint32_t ou32_ApplicationIndex)
{
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      //Data pools
      for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < rc_Node.c_DataPools.size(); ++u32_ItDataPool)
      {
         C_OscNodeDataPool & rc_DataPool = rc_Node.c_DataPools[u32_ItDataPool];
         //Only sync valid indices
         if (rc_DataPool.s32_RelatedDataBlockIndex >= 0)
         {
            if (static_cast<uint32_t>(rc_DataPool.s32_RelatedDataBlockIndex) >= ou32_ApplicationIndex)
            {
               ++rc_DataPool.s32_RelatedDataBlockIndex;
            }
         }
      }
      //File generation settings
      //Only sync valid indices
      if (rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex >= 0)
      {
         //lint -e{571} Above zero is checked
         if (static_cast<uint32_t>(rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex) >=
             ou32_ApplicationIndex)
         {
            ++rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex;
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
void C_PuiSdHandlerNodeLogic::m_SyncNodeApplicationMoved(const uint32_t ou32_NodeIndex,
                                                         const uint32_t ou32_ApplicationSourceIndex,
                                                         const uint32_t ou32_ApplicationTargetIndex)
{
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      //Data pools
      for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < rc_Node.c_DataPools.size(); ++u32_ItDataPool)
      {
         C_OscNodeDataPool & rc_DataPool = rc_Node.c_DataPools[u32_ItDataPool];
         //Only sync valid indices
         if (rc_DataPool.s32_RelatedDataBlockIndex >= 0)
         {
            if (static_cast<uint32_t>(rc_DataPool.s32_RelatedDataBlockIndex) > ou32_ApplicationSourceIndex)
            {
               --rc_DataPool.s32_RelatedDataBlockIndex;
               if (static_cast<uint32_t>(rc_DataPool.s32_RelatedDataBlockIndex) >= ou32_ApplicationTargetIndex)
               {
                  ++rc_DataPool.s32_RelatedDataBlockIndex;
               }
               else
               {
                  //No adaptation necessary
               }
            }
            else if (static_cast<uint32_t>(rc_DataPool.s32_RelatedDataBlockIndex) == ou32_ApplicationSourceIndex)
            {
               rc_DataPool.s32_RelatedDataBlockIndex = ou32_ApplicationTargetIndex;
            }
            else
            {
               if (static_cast<uint32_t>(rc_DataPool.s32_RelatedDataBlockIndex) >= ou32_ApplicationTargetIndex)
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
      if (rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex >= 0)
      {
         //lint -e{571} Above zero is checked
         if (static_cast<uint32_t>(rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex) >
             ou32_ApplicationSourceIndex)
         {
            --rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex;
            //lint -e{571} Above zero is checked
            if (static_cast<uint32_t>(rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex) >=
                ou32_ApplicationTargetIndex)
            {
               ++rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex;
            }
            else
            {
               //No adaptation necessary
            }
         }
         //lint -e{571} Above zero is checked
         else if (static_cast<uint32_t>(rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex) ==
                  ou32_ApplicationSourceIndex)
         {
            rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex =
               static_cast<int16_t>(ou32_ApplicationTargetIndex);
         }
         else
         {
            //lint -e{571} Above zero is checked
            if (static_cast<uint32_t>(rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex) >=
                ou32_ApplicationTargetIndex)
            {
               ++rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex;
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
void C_PuiSdHandlerNodeLogic::m_SyncNodeApplicationAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                                    const uint32_t ou32_ApplicationIndex)
{
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      //Data pools
      for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < rc_Node.c_DataPools.size(); ++u32_ItDataPool)
      {
         C_OscNodeDataPool & rc_DataPool = rc_Node.c_DataPools[u32_ItDataPool];
         //Only sync valid indices
         if (rc_DataPool.s32_RelatedDataBlockIndex >= 0)
         {
            if (static_cast<uint32_t>(rc_DataPool.s32_RelatedDataBlockIndex) > ou32_ApplicationIndex)
            {
               --rc_DataPool.s32_RelatedDataBlockIndex;
            }
            else if (static_cast<uint32_t>(rc_DataPool.s32_RelatedDataBlockIndex) == ou32_ApplicationIndex)
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
      if (rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex >= 0)
      {
         //lint -e{571} Above zero is checked
         if (static_cast<uint32_t>(rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex) >
             ou32_ApplicationIndex)
         {
            --rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex;
         }
         //lint -e{571} Above zero is checked
         else if (static_cast<uint32_t>(rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex) ==
                  ou32_ApplicationIndex)
         {
            rc_Node.c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex = -1;
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
int32_t C_PuiSdHandlerNodeLogic::m_HandleNodeAutomatedProgrammableApplicationUpdate(const uint32_t ou32_NodeIndex)
{
   int32_t s32_Retval;
   const C_OscNode * const pc_Node = this->GetOscNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      int32_t s32_TmpIndex = static_cast<int32_t>(pc_Node->c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex);
      s32_Retval = GetNextProgrammableApplicationIndex(ou32_NodeIndex, s32_TmpIndex);
      if (s32_Retval == C_NO_ERR)
      {
         C_OscNodeOpenSydeServerSettings c_Copy = pc_Node->c_Properties.c_OpenSydeServerSettings;
         c_Copy.s16_DpdDataBlockIndex = static_cast<int16_t>(s32_TmpIndex);
         //See if there was any effective change
         if (c_Copy.s16_DpdDataBlockIndex != pc_Node->c_Properties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex)
         {
            s32_Retval = this->SetNodeOpenSydeServerSettings(ou32_NodeIndex, c_Copy);
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
void C_PuiSdHandlerNodeLogic::m_SetUpComDataPool(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                                 const bool & orq_AllowDataAdaptation,
                                                 const C_OscCanProtocol::E_Type & ore_ComProtocolType)
{
   //Check size & consistency
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_UiNodes.size())
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      //Check size & consistency
      tgl_assert(rc_UiNode.c_UiDataPools.size() == rc_OscNode.c_DataPools.size());
      if (oru32_DataPoolIndex < rc_OscNode.c_DataPools.size())
      {
         C_OscNodeDataPool & rc_OscDataPool = rc_OscNode.c_DataPools[oru32_DataPoolIndex];
         //Check if com data pool
         if (rc_OscDataPool.e_Type == C_OscNodeDataPool::eCOM)
         {
            C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UiDataPools[oru32_DataPoolIndex];
            C_OscNodeDataPoolDataSet c_DataSetInit;
            c_DataSetInit.c_Name = "Init";
            //Clean
            rc_OscDataPool.c_Lists.clear();
            rc_UiDataPool.c_DataPoolLists.clear();
            //Reserve
            rc_OscDataPool.c_Lists.reserve(rc_OscNode.c_Properties.c_ComInterfaces.size() * 2);
            rc_UiDataPool.c_DataPoolLists.reserve(rc_OscNode.c_Properties.c_ComInterfaces.size() * 2);
            //Check com interfaces
            for (uint32_t u32_ItComInterface = 0; u32_ItComInterface < rc_OscNode.c_Properties.c_ComInterfaces.size();
                 ++u32_ItComInterface)
            {
               const C_OscNodeComInterfaceSettings & rc_ComInterface =
                  rc_OscNode.c_Properties.c_ComInterfaces[u32_ItComInterface];
               //Do not create any list for Ethernet
               if (rc_ComInterface.e_InterfaceType == C_OscSystemBus::eCAN)
               {
                  const QString c_InterfaceName = C_PuiSdUtil::h_GetInterfaceName(rc_ComInterface.e_InterfaceType,
                                                                                  rc_ComInterface.u8_InterfaceNumber);
                  C_OscNodeDataPoolList c_OscList;
                  C_PuiSdNodeDataPoolList c_UiList;
                  //Clear
                  c_OscList.c_Elements.clear();
                  c_UiList.c_DataPoolListElements.clear();

                  //Add init data set
                  c_OscList.c_DataSets.clear();
                  c_OscList.c_DataSets.push_back(c_DataSetInit);

                  //Add tx
                  c_OscList.c_Name = (c_InterfaceName + "_TX").toStdString().c_str();

                  rc_OscDataPool.c_Lists.push_back(c_OscList);
                  rc_UiDataPool.c_DataPoolLists.push_back(c_UiList);

                  //Add rx
                  c_OscList.c_Name = (c_InterfaceName + "_RX").toStdString().c_str();

                  rc_OscDataPool.c_Lists.push_back(c_OscList);
                  rc_UiDataPool.c_DataPoolLists.push_back(c_UiList);
               }
            }
            //Set up com protocol
            {
               C_PuiSdNodeCanProtocol c_UiProtocol;
               const C_PuiSdNodeCanMessageContainer c_UiMessageContainer;
               C_OscCanProtocol c_Protocol;
               C_OscCanMessageContainer c_New;
               uint32_t u32_CanCounter = 0U;

               c_Protocol.u32_DataPoolIndex = oru32_DataPoolIndex;
               c_Protocol.e_Type = ore_ComProtocolType;
               c_Protocol.c_ComMessages.reserve(rc_OscNode.c_Properties.c_ComInterfaces.size());
               for (uint32_t u32_ItMessageContainer =
                       0; u32_ItMessageContainer < rc_OscNode.c_Properties.c_ComInterfaces.size();
                    ++u32_ItMessageContainer)
               {
                  const C_OscNodeComInterfaceSettings & rc_Interface =
                     rc_OscNode.c_Properties.c_ComInterfaces[u32_ItMessageContainer];

                  //Do not create any list for Ethernet
                  if (rc_Interface.e_InterfaceType == C_OscSystemBus::eCAN)
                  {
                     uint32_t u32_ComProtocolCounter;

                     c_New.q_IsComProtocolUsedByInterface = false;

                     // Check already existing protocols of same type if the are used by the interface
                     for (u32_ComProtocolCounter = 0U; u32_ComProtocolCounter < rc_OscNode.c_ComProtocols.size();
                          ++u32_ComProtocolCounter)
                     {
                        const C_OscCanProtocol & rc_Protocol = rc_OscNode.c_ComProtocols[u32_ComProtocolCounter];

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
               rc_OscNode.c_ComProtocols.push_back(c_Protocol);
               rc_UiNode.c_UiCanProtocols.push_back(c_UiProtocol);
            }
         }
         else
         {
            //Other data pools need small adjustment
            if ((orq_AllowDataAdaptation == true) && (rc_OscDataPool.c_Lists.size() > 0))
            {
               C_OscNodeDataPoolList & rc_List = rc_OscDataPool.c_Lists[0];
               if (rc_List.c_Elements.size() > 0)
               {
                  C_PuiSdHandlerNodeLogic::h_InitDataElement(rc_OscDataPool.e_Type, rc_OscDataPool.q_IsSafety,
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
void C_PuiSdHandlerNodeLogic::m_CleanUpComDataPool(const uint32_t & oru32_NodeIndex,
                                                   const uint32_t & oru32_DataPoolIndex)
{
   //Clean up com protocol
   tgl_assert(this->mc_UiNodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (oru32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[oru32_NodeIndex];
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[oru32_NodeIndex];
      tgl_assert(rc_Node.c_ComProtocols.size() == rc_UiNode.c_UiCanProtocols.size());
      for (uint32_t u32_ItProtocol = rc_Node.c_ComProtocols.size(); u32_ItProtocol > 0UL; --u32_ItProtocol)
      {
         const uint32_t u32_Index = u32_ItProtocol - 1UL;
         const C_OscCanProtocol & rc_Protcol = rc_Node.c_ComProtocols[u32_Index];
         if (rc_Protcol.u32_DataPoolIndex == oru32_DataPoolIndex)
         {
            rc_Node.c_ComProtocols.erase(rc_Node.c_ComProtocols.begin() + u32_Index);
            rc_UiNode.c_UiCanProtocols.erase(rc_UiNode.c_UiCanProtocols.begin() + u32_Index);
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
void C_PuiSdHandlerNodeLogic::mh_GetNodeNvmDataPoolAreas(const C_OscNode & orc_Node,
                                                         std::vector<C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea> & orc_Areas)
{
   const uint32_t u32_NvmSize =
      orc_Node.pc_DeviceDefinition->c_SubDevices[orc_Node.u32_SubDeviceIndex].u32_UserEepromSizeBytes;
   uint32_t u32_DatapoolCounter;
   uint32_t u32_AreaCounter;

   // Reserve the lowest potential number of areas
   orc_Areas.reserve(orc_Node.c_DataPools.size());

   // Getting all areas
   for (u32_DatapoolCounter = 0U; u32_DatapoolCounter < orc_Node.c_DataPools.size(); ++u32_DatapoolCounter)
   {
      const stw::opensyde_core::C_OscNodeDataPool * const pc_Datapool = &orc_Node.c_DataPools[u32_DatapoolCounter];

      // Checking each NVM based Datapool
      if ((pc_Datapool->e_Type == C_OscNodeDataPool::eNVM) ||
          (pc_Datapool->e_Type == C_OscNodeDataPool::eHALC_NVM))
      {
         // Prepare the original Datapool area
         C_PuiSdHandlerNodeLogicNvmArea c_CurrentDatapoolArea;
         c_CurrentDatapoolArea.u32_StartAddress = pc_Datapool->u32_NvmStartAddress;
         c_CurrentDatapoolArea.u32_Size = pc_Datapool->u32_NvmSize;
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
         orc_Areas[static_cast<uint32_t>(u32_AreaCounter - 1U)];
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
   uint32_t u32_AreaCounter;
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
   uint32_t u32_Counter;

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
void C_PuiSdHandlerNodeLogic::m_SetOscNodeIds(const uint32_t ou32_NodeIndex, const std::vector<uint8_t> & orc_NodeIds)
{
   tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      for (uint32_t u32_ItInt = 0UL;
           (u32_ItInt < rc_OscNode.c_Properties.c_ComInterfaces.size()) && (u32_ItInt < orc_NodeIds.size());
           ++u32_ItInt)
      {
         m_SetOscNodeId(ou32_NodeIndex, u32_ItInt, orc_NodeIds[u32_ItInt]);
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
void C_PuiSdHandlerNodeLogic::m_SetOscNodeId(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                             const uint8_t ou8_NodeId)
{
   tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      tgl_assert(ou32_InterfaceIndex < rc_OscNode.c_Properties.c_ComInterfaces.size());
      if (ou32_InterfaceIndex < rc_OscNode.c_Properties.c_ComInterfaces.size())
      {
         C_OscNodeComInterfaceSettings & rc_Interface = rc_OscNode.c_Properties.c_ComInterfaces[ou32_InterfaceIndex];
         rc_Interface.u8_NodeId = ou8_NodeId;
         m_SyncOsyNodeIdChange(ou32_NodeIndex, ou32_InterfaceIndex, ou8_NodeId);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data pool list, shared datapools sync operation only

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    Data pool list index
   \param[in]  orc_OscContent             OSC data pool list content
   \param[in]  orc_UiContent              UI data pool list content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::m_SetDataPoolListSharedSync(const uint32_t & oru32_NodeIndex,
                                                          const uint32_t & oru32_DataPoolIndex,
                                                          const uint32_t & oru32_DataPoolListIndex,
                                                          const C_OscNodeDataPoolList & orc_OscContent,
                                                          const C_PuiSdNodeDataPoolList & orc_UiContent)
{
   std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

   if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                            c_SharedDatapools) == true)
   {
      const C_OscNodeDataPool * const pc_DatapoolOrg = this->GetOscDataPool(oru32_NodeIndex, oru32_DataPoolIndex);
      if (pc_DatapoolOrg != NULL)
      {
         uint32_t u32_SharedDpCounter;

         // Synchronize with the shared Datapools
         for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
              ++u32_SharedDpCounter)
         {
            const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
            const C_OscNodeDataPool * const pc_Datapool = this->GetOscDataPool(rc_DpId.u32_NodeIndex,
                                                                               rc_DpId.u32_DataPoolIndex);
            const C_OscNodeDataPoolList * const pc_List = this->GetOscDataPoolList(rc_DpId.u32_NodeIndex,
                                                                                   rc_DpId.u32_DataPoolIndex,
                                                                                   oru32_DataPoolListIndex);
            tgl_assert((pc_Datapool != NULL) && (pc_List != NULL));
            if ((pc_Datapool != NULL) && (pc_List != NULL))
            {
               C_OscNodeDataPoolList c_NewValue = orc_OscContent;
               //Keep original values
               if ((!pc_Datapool->q_IsSafety) && (pc_DatapoolOrg->q_IsSafety))
               {
                  c_NewValue.q_NvmCrcActive = pc_List->q_NvmCrcActive;
                  tgl_assert(pc_List->c_Elements.size() == c_NewValue.c_Elements.size());
                  if (pc_List->c_Elements.size() == c_NewValue.c_Elements.size())
                  {
                     for (uint32_t u32_ItEl = 0UL; u32_ItEl < pc_List->c_Elements.size(); ++u32_ItEl)
                     {
                        c_NewValue.c_Elements[u32_ItEl].e_Access = pc_List->c_Elements[u32_ItEl].e_Access;
                     }
                  }
               }
               // No error expected. The Datapools must have the same conditions
               tgl_assert(this->SetDataPoolList(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                oru32_DataPoolListIndex, c_NewValue, orc_UiContent,
                                                false) == C_NO_ERR);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data pool list nvm crc flag, shared datapools sync operation only

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    Data pool list index
   \param[in]  oq_Value                   New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::m_SetDataPoolListNvmCrcSharedSync(const uint32_t & oru32_NodeIndex,
                                                                const uint32_t & oru32_DataPoolIndex,
                                                                const uint32_t & oru32_DataPoolListIndex,
                                                                const bool oq_Value)
{
   std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

   if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                            c_SharedDatapools) == true)
   {
      uint32_t u32_SharedDpCounter;

      // Synchronize with the shared Datapools
      for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
           ++u32_SharedDpCounter)
      {
         const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
         // No error expected. The Datapools must have the same conditions
         tgl_assert(this->SetDataPoolListNvmCrc(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                oru32_DataPoolListIndex, oq_Value,
                                                false) == C_NO_ERR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data pool list element, shared datapools sync operation only

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          Data pool list index
   \param[in]  oru32_DataPoolListElementIndex   Data pool list element index
   \param[in]  orc_OscContent                   OSC data pool list element content
   \param[in]  orc_UiContent                    UI data pool list element content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerNodeLogic::m_SetDataPoolListElementSharedSync(const uint32_t & oru32_NodeIndex,
                                                                 const uint32_t & oru32_DataPoolIndex,
                                                                 const uint32_t & oru32_DataPoolListIndex,
                                                                 const uint32_t & oru32_DataPoolListElementIndex,
                                                                 const C_OscNodeDataPoolListElement & orc_OscContent,
                                                                 const C_PuiSdNodeDataPoolListElement & orc_UiContent)
{
   std::vector<C_OscNodeDataPoolId> c_SharedDatapools;

   if (this->mc_SharedDatapools.IsSharedAndGetDatapoolGroup(oru32_NodeIndex, oru32_DataPoolIndex,
                                                            c_SharedDatapools) == true)
   {
      const C_OscNodeDataPool * const pc_DatapoolOrg = this->GetOscDataPool(oru32_NodeIndex, oru32_DataPoolIndex);
      if (pc_DatapoolOrg != NULL)
      {
         uint32_t u32_SharedDpCounter;

         // Synchronize with the shared Datapools
         for (u32_SharedDpCounter = 0U; u32_SharedDpCounter < c_SharedDatapools.size();
              ++u32_SharedDpCounter)
         {
            const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapools[u32_SharedDpCounter];
            const C_OscNodeDataPool * const pc_Datapool = this->GetOscDataPool(rc_DpId.u32_NodeIndex,
                                                                               rc_DpId.u32_DataPoolIndex);
            const C_OscNodeDataPoolListElement * const pc_Element = this->GetOscDataPoolListElement(
               rc_DpId.u32_NodeIndex,
               rc_DpId.u32_DataPoolIndex, oru32_DataPoolListIndex,
               oru32_DataPoolListElementIndex);
            tgl_assert((pc_Datapool != NULL) && (pc_Element != NULL));
            if ((pc_Datapool != NULL) && (pc_Element != NULL))
            {
               C_OscNodeDataPoolListElement c_NewElement = orc_OscContent;
               //Keep original values
               if ((!pc_Datapool->q_IsSafety) && (pc_DatapoolOrg->q_IsSafety))
               {
                  c_NewElement.e_Access = pc_Element->e_Access;
               }
               // No error expected. The Datapools must have the same conditions
               tgl_assert(this->SetDataPoolListElement(rc_DpId.u32_NodeIndex, rc_DpId.u32_DataPoolIndex,
                                                       oru32_DataPoolListIndex,
                                                       oru32_DataPoolListElementIndex, c_NewElement,
                                                       orc_UiContent,
                                                       false) == C_NO_ERR);
            }
         }
      }
   }
}
