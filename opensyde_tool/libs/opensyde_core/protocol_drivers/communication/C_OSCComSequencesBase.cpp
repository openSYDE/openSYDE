//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Basic sequences to communicate with flashloader protocols. (implementation)

   Initialization for C_OSCComDriverFlash

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"

#include "C_OSCComSequencesBase.h"

#include "TGLUtils.h"
#include "C_OSCRoutingRoute.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  oq_RoutingActive     Flag for activating routing
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCComSequencesBase::C_OSCComSequencesBase(const bool oq_RoutingActive) :
   mpc_ComDriver(new C_OSCComDriverFlash(oq_RoutingActive, &mh_MyXflReportProgress, this)),
   mpc_SystemDefinition(NULL),
   mu32_ActiveBusIndex(0U),
   mq_OpenSydeDevicesActive(false),
   mq_StwFlashloaderDevicesActive(false),
   mq_StwFlashloaderDevicesActiveOnLocalBus(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   destructor

   Tear down class
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCComSequencesBase::~C_OSCComSequencesBase(void)
{
   delete this->mpc_ComDriver;
   mpc_SystemDefinition = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize class parameters to use for communication

   Set parameters required for processes:
   * whole system definition
   * Flash comm driver to use
   * flags: which of the nodes are active; this determines which nodes the client expected to be present
   * index of bus the client is connected to

   The caller is responsible to keep the referenced system definition stable while functions in this class are executed
   (this class does not create a copy)

   \param[in]  orc_SystemDefinition     Entire system definition
   \param[in]  ou32_ActiveBusIndex      Index of bus within system definition that the client tool is connected to
                                         (0 = first bus)
   \param[in]  orc_ActiveNodes          Vector of flags for each node in the system definition
                                           - set the entry to true if the device is expected to be present
                                           - set the entry to false if the device is not expected to be present
   \param[in]  opc_CanDispatcher        Pointer to concrete CAN dispatcher
   \param[in]  opc_IpDispatcher         Pointer to concrete IP dispatcher
   \param[in]  opc_SecurityPemDb        Pointer to PEM database (optional)
                                        Needed if nodes with enabled security are used in the system

   \return
   C_NO_ERR      Configuration set
   C_CONFIG      Invalid system definition for parameters
                 Active bus index refers to a bus that is not part of the system definition
                 Length of active nodes vector is not identical to number of nodes in system definition
                 No STW flashloader devices and no openSYDE devices are active
                 No STW flashloader devices on active bus and no openSYDE devices are active, but STW flashloader on
                     other busses are active
   C_OVERFLOW    Unknown transport protocol or unknown diagnostic server for at least one node
   C_NOACT       No active nodes
   C_COM         CAN initialization failed or no route found for at least one node
   C_CHECKSUM    Internal buffer overflow detected
   C_DEFAULT     Parameter ou32_ActiveBusIndex invalid
   C_RANGE       Routing configuration failed (can all nodes marked as active be reached from the defined bus ?)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComSequencesBase::Init(C_OSCSystemDefinition & orc_SystemDefinition, const uint32 ou32_ActiveBusIndex,
                                   const std::vector<uint8> & orc_ActiveNodes,
                                   stw_can::C_CAN_Dispatcher * const opc_CanDispatcher,
                                   C_OSCIpDispatcher * const opc_IpDispatcher,
                                   C_OSCSecurityPemDatabase * const opc_SecurityPemDb)
{
   sint32 s32_Return = C_CONFIG;

   if ((orc_SystemDefinition.c_Nodes.size() == orc_ActiveNodes.size()) &&
       (ou32_ActiveBusIndex < orc_SystemDefinition.c_Buses.size()))
   {
      this->mpc_SystemDefinition = &orc_SystemDefinition;
      this->mu32_ActiveBusIndex = ou32_ActiveBusIndex;
      this->mc_ActiveNodes = orc_ActiveNodes;
      this->mc_TimeoutNodes.resize(this->mc_ActiveNodes.size(), 0);

      s32_Return = this->mpc_ComDriver->Init(orc_SystemDefinition, ou32_ActiveBusIndex,
                                             orc_ActiveNodes, opc_CanDispatcher, opc_IpDispatcher, opc_SecurityPemDb);

      if (s32_Return == C_NO_ERR)
      {
         this->mq_OpenSydeDevicesActive = this->m_IsAtLeastOneOpenSydeNodeActive();
         this->mq_StwFlashloaderDevicesActive = this->m_IsAtLeastOneStwFlashloaderNodeActive();
         this->mq_StwFlashloaderDevicesActiveOnLocalBus = this->m_IsAtLeastOneStwFlashloaderNodeOnLocalBusActive(
            this->mc_StwFlashloaderDeviceOnLocalBus);

         // Either no devices are active or only STW flashloader devices which need routing to get reached are active
         if (((this->mq_OpenSydeDevicesActive == false) && (this->mq_StwFlashloaderDevicesActive == false)) ||
             ((this->mq_OpenSydeDevicesActive == false) && (this->mq_StwFlashloaderDevicesActiveOnLocalBus == false) &&
              (this->mq_StwFlashloaderDevicesActive == true)))
         {
            s32_Return = C_CONFIG;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get is initialized flag

   \return
   Is initialized flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCComSequencesBase::IsInitialized(void) const
{
   return this->mpc_ComDriver->IsInitialized();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the node index by the server id

   \param[in]     orc_ServerId         Server id
   \param[out]    oru32_NodeIndex      Found node index

   \return
   true     Node index found
   false    Node index not found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCComSequencesBase::GetNodeIndex(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                         uint32 & oru32_NodeIndex) const
{
   return this->mpc_ComDriver->GetNodeIndex(orc_ServerId, oru32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the information if at least one openSYDE device is active

   Init function must be called first.

   \return
   true     At least one device found
   false    No device found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCComSequencesBase::IsAtLeastOneOpenSydeNodeActive(void) const
{
   return this->mq_OpenSydeDevicesActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the information if at least one STW flashloader device is active

   Init function must be called first.

   \return
   true     At least one device found
   false    No device found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCComSequencesBase::IsAtLeastOneStwFlashloaderNodeActive(void) const
{
   return this->mq_StwFlashloaderDevicesActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the information if at least one STW flashloader device is on the local bus active

   Init function must be called first.

   \return
   true     At least one device found
   false    No device found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCComSequencesBase::IsAtLeastOneStwFlashloaderNodeActiveOnLocalBus(void) const
{
   return this->mq_StwFlashloaderDevicesActiveOnLocalBus;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if a specific node must be capable of Ethernet to Ethernet routing

   \param[in]       ou32_RouterNodeIndex     Node to check if node must be capable of Ethernet to Ethernet routing
                                             for at least one route

   \retval   true     Ethernet to Ethernet Routing must be supported by router node
   \retval   false    Ethernet to Ethernet Routing must not be supported by router node
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCComSequencesBase::IsEthToEthRoutingNecessary(const uint32 ou32_RouterNodeIndex) const
{
   return this->mpc_ComDriver->IsEthToEthRoutingNecessary(ou32_RouterNodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the minimum Flashloader reset wait time in ms

   Default minimum value is 500 ms independent of the type.

   \param[in]  oe_Type  Type of minimum flashloader reset wait time

   \return
   Time in ms all nodes needs at least to get from application to the Flashloader
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCComSequencesBase::GetMinimumFlashloaderResetWaitTime(
   const C_OSCComDriverFlash::E_MinimumFlashloaderResetWaitTimeType oe_Type) const
{
   return this->mpc_ComDriver->GetMinimumFlashloaderResetWaitTime(oe_Type);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the minimum Flashloader reset wait time in ms for a specific node

   Default minimum value is 500 ms independent of the type.

   \param[in]   oe_Type             Type of minimum flashloader reset wait time
   \param[in]   orc_ServerId        Server id to get the configured wait time
   \param[out]  oru32_TimeValue     Time in ms the node need at least to get from application to the Flashloader or
                                    from Flashloader to Flashloader

   \retval   C_NO_ERR   Time returned
   \retval   C_RANGE    Node with orc_ServerId does not exist or is not active
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComSequencesBase::GetMinimumFlashloaderResetWaitTime(
   const C_OSCComDriverFlash::E_MinimumFlashloaderResetWaitTimeType oe_Type,
   const C_OSCProtocolDriverOsyNode & orc_ServerId, stw_types::uint32 & oru32_TimeValue) const
{
   return this->mpc_ComDriver->GetMinimumFlashloaderResetWaitTime(oe_Type, orc_ServerId, oru32_TimeValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the node is reachable on the current route

   \param[in]     ou32_NodeIndex         Index of current node

   \return
   true     Node is reachable on the current route
   false    Node is not reachable on the current route
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCComSequencesBase::m_IsNodeReachable(const uint32 ou32_NodeIndex) const
{
   bool q_Return;

   if (this->mc_TimeoutNodes[ou32_NodeIndex] == 1U)
   {
      // Node itself had a timeout and is not reachable
      q_Return = false;
   }
   else
   {
      // Check the nodes on the route
      C_OSCRoutingRoute c_Route(ou32_NodeIndex);
      uint32 u32_PointCounter;
      q_Return = true;

      this->mpc_ComDriver->GetRouteOfNode(ou32_NodeIndex, c_Route);

      for (u32_PointCounter = 0U; u32_PointCounter < c_Route.c_VecRoutePoints.size(); ++u32_PointCounter)
      {
         if (this->mc_TimeoutNodes[c_Route.c_VecRoutePoints[u32_PointCounter].u32_NodeIndex] == 1U)
         {
            // A node on the route had a timeout
            q_Return = false;
            break;
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   report progress of STW Flashloader operation to application

   Report progress information from STW Flashloader driver via virtual function.

   \param[in]   opv_Instance           instance the callback was installed with
   \param[in]   ou8_Progress           progress of operation (from 0 to 100)
                                       progress invalid: 255
   \param[in]   orc_AdditionalText     textual information about progress

   \return
   C_NO_ERR    continue operation
   else        abort operation (not honored at each position)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComSequencesBase::mh_MyXflReportProgress(void * const opv_Instance, const uint8 ou8_Progress,
                                                     const C_SCLString & orc_Text)
{
   //lint -e{9079}  This class is the only one which registers itself at the caller of this function. It must match.
   return reinterpret_cast<C_OSCComSequencesBase *>(opv_Instance)->m_MyXflReportProgress(ou8_Progress, orc_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   report progress of STW Flashloader operation to application

   Report progress information from STW Flashloader driver via virtual function.

   \param[in]   ou8_Progress           progress of operation (from 0 to 100)
                                       progress invalid: 255
   \param[in]   orc_AdditionalText     textual information about progress

   \return
   C_NO_ERR    continue operation
   else        abort operation (not honored at each position)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComSequencesBase::m_MyXflReportProgress(const uint8 ou8_Progress, const C_SCLString & orc_Text)
{
   //invoke virtual function:
   return this->m_XflReportProgress(ou8_Progress, orc_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the information if at least one openSYDE device is active

   \return
   true     At least one device found
   false    No device found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCComSequencesBase::m_IsAtLeastOneOpenSydeNodeActive(void) const
{
   bool q_Return = false;

   if (this->mpc_SystemDefinition != NULL)
   {
      for (uint32 u32_Counter = 0U; u32_Counter < this->mc_ActiveNodes.size(); ++u32_Counter)
      {
         if ((u32_Counter < this->mpc_SystemDefinition->c_Nodes.size()) && (this->mc_ActiveNodes[u32_Counter] == 1U))
         {
            const C_OSCNode & rc_Node = this->mpc_SystemDefinition->c_Nodes[u32_Counter];

            if (rc_Node.c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_OPEN_SYDE)
            {
               q_Return = true;
               break;
            }
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the information if at least one STW flashloader device is active

   \return
   true     At least one device found
   false    No device found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCComSequencesBase::m_IsAtLeastOneStwFlashloaderNodeActive(void) const
{
   bool q_Return = false;

   if (this->mpc_SystemDefinition != NULL)
   {
      for (uint32 u32_Counter = 0U; u32_Counter < this->mc_ActiveNodes.size(); ++u32_Counter)
      {
         if ((u32_Counter < this->mpc_SystemDefinition->c_Nodes.size()) && (this->mc_ActiveNodes[u32_Counter] == 1U))
         {
            const C_OSCNode & rc_Node = this->mpc_SystemDefinition->c_Nodes[u32_Counter];

            if (rc_Node.c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_STW)
            {
               q_Return = true;
               break;
            }
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the information if at least one STW flashloader device is active on the local bus

   \param[out]    orc_StwFlashloaderDeviceOnLocalBus     The first found STW flashloader node on the local bus

   \return
   true     At least one device found
   false    No device found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCComSequencesBase::m_IsAtLeastOneStwFlashloaderNodeOnLocalBusActive(
   C_OSCProtocolDriverOsyNode & orc_StwFlashloaderDeviceOnLocalBus) const
{
   bool q_Return = false;

   if (this->mpc_SystemDefinition != NULL)
   {
      for (uint32 u32_Counter = 0U; u32_Counter < this->mc_ActiveNodes.size(); ++u32_Counter)
      {
         if ((u32_Counter < this->mpc_SystemDefinition->c_Nodes.size()) && (this->mc_ActiveNodes[u32_Counter] == 1U))
         {
            const C_OSCNode & rc_Node = this->mpc_SystemDefinition->c_Nodes[u32_Counter];

            if (rc_Node.c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_STW)
            {
               uint32 u32_IntfCounter;

               // Check all interfaces of node if connected to the bus which is connected to the client
               for (u32_IntfCounter = 0U; u32_IntfCounter < rc_Node.c_Properties.c_ComInterfaces.size();
                    ++u32_IntfCounter)
               {
                  const C_OSCNodeComInterfaceSettings & rc_ComIntf =
                     rc_Node.c_Properties.c_ComInterfaces[u32_IntfCounter];

                  if ((rc_ComIntf.u32_BusIndex == this->mu32_ActiveBusIndex) &&
                      (rc_ComIntf.GetBusConnected() == true))
                  {
                     tgl_assert(this->mu32_ActiveBusIndex < this->mpc_SystemDefinition->c_Buses.size());

                     // Save the node identifier for a local STW flashloader device to get easy access
                     // to the protocol for the local bus and not server specific services
                     orc_StwFlashloaderDeviceOnLocalBus.u8_NodeIdentifier = rc_ComIntf.u8_NodeID;
                     orc_StwFlashloaderDeviceOnLocalBus.u8_BusIdentifier =
                        this->mpc_SystemDefinition->c_Buses[this->mu32_ActiveBusIndex].u8_BusID;

                     q_Return = true;
                     break;
                  }
               }

               if (q_Return == true)
               {
                  break;
               }
            }
         }
      }
   }

   return q_Return;
}
