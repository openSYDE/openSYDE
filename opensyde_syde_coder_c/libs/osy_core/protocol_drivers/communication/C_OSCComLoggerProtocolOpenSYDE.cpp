//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to openSYDE interpretation (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"

#include "C_OSCComLoggerProtocolOpenSYDE.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_scl;
using namespace stw_opensyde_core;
using namespace stw_cmon_protocol;

/* -- Module Global Constants ---------------------------------------------- */
// ISO15765-2 addressing mode
#define OSY_CTP_ADDR_MODE_PHYSICAL   (0U) ///< CAN TP mode fixed physical addressing

// ISO15765-3 routing
#define OSY_CTP_ROUTING_INACTIVE (0U) ///< CAN TP routing is deactivated on ECU
#define OSY_CTP_ROUTING_ACTIVE   (1U) ///< CAN TP routing is activated on ECU

#define OSY_BROADCAST (0x7FU) ///< Broadcast ID
#define OSY_CLIENT    (0x7EU) ///< Standard client ID (can vary, it is the default ID of the openSYDE gui)

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//-----------------------------------------------------------------------------
C_OSCComLoggerProtocolOpenSYDE::C_OSCComLoggerProtocolOpenSYDE(void) :
   C_CMONProtocolOpenSYDE()
{
}

//-----------------------------------------------------------------------------
/*! \brief   Adds an openSYDE system definition configuration for analyzing

   \param[in]     opc_SysDefConfig               Pointer to openSYDE system definition configuration
*/
//-----------------------------------------------------------------------------
void C_OSCComLoggerProtocolOpenSYDE::AddOsySysDef(const C_OSCComMessageLoggerOsySysDefConfig * const opc_SysDefConfig)
{
   this->mc_SysDefConfigs.push_back(opc_SysDefConfig);
}

//-----------------------------------------------------------------------------
/*! \brief   Removes an openSYDE system definition configuration

   \param[in]     opc_SysDefConfig               Pointer to openSYDE system definition configuration
*/
//-----------------------------------------------------------------------------
void C_OSCComLoggerProtocolOpenSYDE::RemoveOsySysDef(const C_OSCComMessageLoggerOsySysDefConfig * const opc_SysDefConfig)
{
   this->mc_SysDefConfigs.remove(opc_SysDefConfig);
}

//-----------------------------------------------------------------------------
/*! \brief   Convert the openSYDE address information to text

   \param[in]     ort_CanAddressInformation   Address information

   \return
   Text interpretation of address information
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCComLoggerProtocolOpenSYDE::m_AddressInformationToText(
   const C_CMONProtocolOpenSYDE::T_CanAddressInformation & ort_CanAddressInformation) const
{
   C_SCLString c_Text = "";

   std::list<const C_OSCComMessageLoggerOsySysDefConfig *>::const_iterator c_ItConfig;
   bool q_ValidInterpretationFound = false;

   for (c_ItConfig = this->mc_SysDefConfigs.begin(); c_ItConfig != this->mc_SysDefConfigs.end(); ++c_ItConfig)
   {
      const C_OSCComMessageLoggerOsySysDefConfig * const pc_Config = *c_ItConfig;
      uint32 u32_SourceBusIndex = 0U;
      uint32 u32_TargetBusIndex = 0U;
      bool q_SourceBusFound = false;
      bool q_TargetBusFound = false;
      C_SCLString c_SourceBusName = "";
      C_SCLString c_TargetBusName = "";

      // Get the corrected bus indexes and names
      if (ort_CanAddressInformation.u8_RoutingMode == OSY_CTP_ROUTING_INACTIVE)
      {
         // No routing, it must be the to the PC connected bus
         if (pc_Config->u32_BusIndex < pc_Config->c_OsySysDef.c_Buses.size())
         {
            u32_SourceBusIndex = pc_Config->u32_BusIndex;
            u32_TargetBusIndex = pc_Config->u32_BusIndex;

            c_SourceBusName = pc_Config->c_OsySysDef.c_Buses[u32_SourceBusIndex].c_Name;
            c_TargetBusName = c_SourceBusName;

            q_SourceBusFound = true;
            q_TargetBusFound = true;
         }
      }
      else
      {
         uint32 u32_BusCounter;

         // Routing, search for both buses
         for (u32_BusCounter = 0U; u32_BusCounter < pc_Config->c_OsySysDef.c_Buses.size(); ++u32_BusCounter)
         {
            if (pc_Config->c_OsySysDef.c_Buses[u32_BusCounter].u8_BusID ==
                ort_CanAddressInformation.t_NodeIdSource.u8_Subnet)
            {
               c_SourceBusName = pc_Config->c_OsySysDef.c_Buses[u32_BusCounter].c_Name;
               u32_SourceBusIndex = u32_BusCounter;
               q_SourceBusFound = true;
            }
            if (pc_Config->c_OsySysDef.c_Buses[u32_BusCounter].u8_BusID ==
                ort_CanAddressInformation.t_NodeIdTarget.u8_Subnet)
            {
               c_TargetBusName = pc_Config->c_OsySysDef.c_Buses[u32_BusCounter].c_Name;
               u32_TargetBusIndex = u32_BusCounter;
               q_TargetBusFound = true;
            }

            if ((q_SourceBusFound == true) &&
                (q_TargetBusFound == true))
            {
               // Both buses found
               break;
            }
         }
      }

      if ((q_SourceBusFound == true) &&
          (q_TargetBusFound == true))
      {
         uint32 u32_NodeCounter;
         C_SCLString c_SourceNodeName;
         C_SCLString c_TargetNodeName;
         bool q_SourceNodeFound = false;
         bool q_TargetNodeFound = false;

         // Check for broadcast and default client
         if (ort_CanAddressInformation.t_NodeIdTarget.u8_NodeId == OSY_BROADCAST)
         {
            c_TargetNodeName = "ALL";
            q_TargetNodeFound = true;
         }
         else if (ort_CanAddressInformation.t_NodeIdTarget.u8_NodeId == OSY_CLIENT)
         {
            c_TargetNodeName = "CLIENT";
            q_TargetNodeFound = true;
         }
         else
         {
            // Nothing to do
         }

         if (ort_CanAddressInformation.t_NodeIdSource.u8_NodeId == OSY_CLIENT)
         {
            c_SourceNodeName = "CLIENT";
            q_SourceNodeFound = true;
         }

         // Search the correct node connected to the bus with the node id
         for (u32_NodeCounter = 0U; u32_NodeCounter < pc_Config->c_OsySysDef.c_Nodes.size(); ++u32_NodeCounter)
         {
            const C_OSCNode & rc_Node = pc_Config->c_OsySysDef.c_Nodes[u32_NodeCounter];
            uint32 u32_InterfaceCounter;

            for (u32_InterfaceCounter = 0U; u32_InterfaceCounter < rc_Node.c_Properties.c_ComInterfaces.size();
                 ++u32_InterfaceCounter)
            {
               if ((q_SourceNodeFound == false) &&
                   (rc_Node.c_Properties.c_ComInterfaces[u32_InterfaceCounter].u8_NodeID ==
                    ort_CanAddressInformation.t_NodeIdSource.u8_NodeId) &&
                   (rc_Node.c_Properties.c_ComInterfaces[u32_InterfaceCounter].u32_BusIndex == u32_SourceBusIndex))
               {
                  // Source Node
                  c_SourceNodeName = rc_Node.c_Properties.c_Name;
                  q_SourceNodeFound = true;
               }

               if ((q_TargetNodeFound == false) &&
                   (rc_Node.c_Properties.c_ComInterfaces[u32_InterfaceCounter].u8_NodeID ==
                    ort_CanAddressInformation.t_NodeIdTarget.u8_NodeId) &&
                   (rc_Node.c_Properties.c_ComInterfaces[u32_InterfaceCounter].u32_BusIndex == u32_TargetBusIndex))
               {
                  // Target Node
                  c_TargetNodeName = rc_Node.c_Properties.c_Name;
                  q_TargetNodeFound = true;
               }
            }

            if ((q_SourceNodeFound == true) &&
                (q_TargetNodeFound == true))
            {
               // Both nodes found
               break;
            }
         }

         // If the nodes was not found, use the IDs but show the names of the buses
         if (q_TargetNodeFound == false)
         {
            c_TargetNodeName = m_GetByteAsStringFormat(ort_CanAddressInformation.t_NodeIdTarget.u8_NodeId);
         }
         if (q_SourceNodeFound == false)
         {
            c_SourceNodeName = m_GetByteAsStringFormat(ort_CanAddressInformation.t_NodeIdSource.u8_NodeId);
         }

         // Create the string
         if (ort_CanAddressInformation.u8_RoutingMode == OSY_CTP_ROUTING_ACTIVE)
         {
            c_Text = "ROUTING (";
            c_Text += c_SourceBusName + "." + c_SourceNodeName + " -> " +
                      c_TargetBusName + "." + c_TargetNodeName + ") ";
         }
         else
         {
            c_Text = "LOCAL";
            if (ort_CanAddressInformation.u8_AddressMode == OSY_CTP_ADDR_MODE_PHYSICAL)
            {
               c_Text += "_PHYS ";
            }
            else
            {
               c_Text += "_FUNC ";
            }

            c_Text += "(" + c_SourceNodeName + " -> " + c_TargetNodeName + ") ";
         }

         q_ValidInterpretationFound = true;
         break;
      }
   }

   if (q_ValidInterpretationFound == false)
   {
      // Fallback if no matching information found in at least one system definition
      c_Text = C_CMONProtocolOpenSYDE::m_AddressInformationToText(ort_CanAddressInformation);
   }

   return c_Text;
}

//-----------------------------------------------------------------------------
/*! \brief   Convert 24bit openSYDE data pool identifier code to text

   \param[in]     ou32_DataPoolIdentifier      Data pool data identifier
   \param[in]     oq_IsResponse                Flag if service for accessing the datapool element was a response or
                                               a request
   \param[in]     ort_CanAddressInformation    Information about nodes involved in communication

   \return
   Text interpretation of data pool data identifier
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCComLoggerProtocolOpenSYDE::m_DataPoolIdentifierToText(const uint32 ou32_DataPoolIdentifier,
                                                                       const bool oq_IsResponse,
                                                                       const T_CanAddressInformation & ort_CanAddressInformation)
const
{
   C_SCLString c_Text = "";

   std::list<const C_OSCComMessageLoggerOsySysDefConfig *>::const_iterator c_ItConfig;
   bool q_ValidElementFound = false;
   uint8 u8_NodeId;

   if (oq_IsResponse == true)
   {
      // Datapool element of sender/source
      u8_NodeId = ort_CanAddressInformation.t_NodeIdSource.u8_NodeId;
   }
   else
   {
      // Datapool element of receiver/target
      u8_NodeId = ort_CanAddressInformation.t_NodeIdTarget.u8_NodeId;
   }

   for (c_ItConfig = this->mc_SysDefConfigs.begin(); c_ItConfig != this->mc_SysDefConfigs.end(); ++c_ItConfig)
   {
      const C_OSCComMessageLoggerOsySysDefConfig * const pc_Config = *c_ItConfig;

      // Get the corrected bus index
      uint32 u32_BusIndexWithNode = 0U;
      bool q_BusFound = false;

      if (ort_CanAddressInformation.u8_RoutingMode == OSY_CTP_ROUTING_INACTIVE)
      {
         // No routing, it must be the to the PC connected bus
         u32_BusIndexWithNode = pc_Config->u32_BusIndex;
         q_BusFound = true;
      }
      else
      {
         uint32 u32_BusCounter;

         for (u32_BusCounter = 0U; u32_BusCounter < pc_Config->c_OsySysDef.c_Buses.size(); ++u32_BusCounter)
         {
            if (oq_IsResponse == true)
            {
               // Datapool element of sender/source
               if (pc_Config->c_OsySysDef.c_Buses[u32_BusCounter].u8_BusID ==
                   ort_CanAddressInformation.t_NodeIdSource.u8_Subnet)
               {
                  u32_BusIndexWithNode = u32_BusCounter;
                  q_BusFound = true;
               }
            }
            else
            {
               // Datapool element of receiver/target
               if (pc_Config->c_OsySysDef.c_Buses[u32_BusCounter].u8_BusID ==
                   ort_CanAddressInformation.t_NodeIdTarget.u8_Subnet)
               {
                  u32_BusIndexWithNode = u32_BusCounter;
                  q_BusFound = true;
               }
            }

            if (q_BusFound == true)
            {
               break;
            }
         }
      }

      if (q_BusFound == true)
      {
         uint32 u32_NodeCounter;

         // Search the correct node connected to the bus with the node id
         for (u32_NodeCounter = 0U; u32_NodeCounter < pc_Config->c_OsySysDef.c_Nodes.size(); ++u32_NodeCounter)
         {
            const C_OSCNode & rc_Node = pc_Config->c_OsySysDef.c_Nodes[u32_NodeCounter];
            uint32 u32_InterfaceCounter;
            bool q_NodeFound = false;

            for (u32_InterfaceCounter = 0U; u32_InterfaceCounter < rc_Node.c_Properties.c_ComInterfaces.size();
                 ++u32_InterfaceCounter)
            {
               if ((rc_Node.c_Properties.c_ComInterfaces[u32_InterfaceCounter].u8_NodeID == u8_NodeId) &&
                   (rc_Node.c_Properties.c_ComInterfaces[u32_InterfaceCounter].u32_BusIndex == u32_BusIndexWithNode))
               {
                  // This is the node
                  q_NodeFound = true;
                  break;
               }
            }

            if (q_NodeFound == true)
            {
               const uint8 u8_DataPoolIndex = static_cast<uint8>(ou32_DataPoolIdentifier >> 18) & 0x1FU;
               const uint8 u8_ListIndex = static_cast<uint8>(ou32_DataPoolIdentifier >> 11) & 0x7FU;
               const uint16 u16_ElementIndex = static_cast<uint16>(ou32_DataPoolIdentifier) & 0x7FFU;

               // Get the datapool element if it exists
               if ((u8_DataPoolIndex < rc_Node.c_DataPools.size()) &&
                   (u8_ListIndex < rc_Node.c_DataPools[u8_DataPoolIndex].c_Lists.size()) &&
                   (u16_ElementIndex <
                    rc_Node.c_DataPools[u8_DataPoolIndex].c_Lists[u8_ListIndex].c_Elements.size()))
               {
                  const bool q_ReservedBitSet = ((ou32_DataPoolIdentifier & 0x00800000U) != 0U) ? true : false;

                  if (q_ReservedBitSet == true)
                  {
                     c_Text = "RESBIT=1 ";
                  }

                  c_Text += ("[" + rc_Node.c_DataPools[u8_DataPoolIndex].c_Name + "." +
                             rc_Node.c_DataPools[u8_DataPoolIndex].c_Lists[u8_ListIndex].c_Name + "." +
                             rc_Node.c_DataPools[u8_DataPoolIndex].c_Lists[u8_ListIndex].c_Elements[u16_ElementIndex]
                             .c_Name + "]");

                  q_ValidElementFound = true;
               }

               break;
            }
         }
      }

      if (q_ValidElementFound == true)
      {
         break;
      }
   }

   if (q_ValidElementFound == false)
   {
      // No element found in at least one openSYDE system definition found, use the standard interpretation
      c_Text = C_CMONProtocolOpenSYDE::m_DataPoolIdentifierToText(ou32_DataPoolIdentifier, oq_IsResponse,
                                                                  ort_CanAddressInformation);
   }

   return c_Text;
}
