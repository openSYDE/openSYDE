//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Core communication driver for flashloader protocols (implementation)

   Adds functionality to the base class:
   * flashloader protocol instances for openSYDE and STW Flashloader protocols

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cstring>
#include "stwerrors.h"

#include "C_OSCComDriverFlash.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_can;
using namespace stw_scl;
using namespace stw_diag_lib;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */
extern void XFLSetInternalKey(const uint16 ou16_Key);

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  oq_RoutingActive              Flag for activating routing
   \param[in]  opr_XflReportProgress         function to call if STW Flashloader driver has something to report
   \param[in]  opv_XflReportProgressInstance Instance pointer to pass when invoking opv_XflReportProgressInstance
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCComDriverFlash::C_OSCComDriverFlash(const bool oq_RoutingActive,
                                         const C_OSCFlashProtocolStwFlashloader::PR_ReportProgress opr_XflReportProgress,
                                         void * const opv_XflReportProgressInstance) :
   C_OSCComDriverProtocol(),
   pr_XflReportProgress(opr_XflReportProgress),
   pv_XflReportProgressInstance(opv_XflReportProgressInstance),
   mq_RoutingActive(oq_RoutingActive)
{
   mc_CompanyId.u8_NumBytes = 2U;
   mc_CompanyId.au8_Data[0] = static_cast<uint8>('Y');
   mc_CompanyId.au8_Data[1] = static_cast<uint8>('*');

   //configure STW Flashloader protocol engine to accept wildcard company ID:
   XFLSetInternalKey(0x3472U);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCComDriverFlash::~C_OSCComDriverFlash(void)
{
   uint32 u32_Counter;

   for (u32_Counter = 0U; u32_Counter < this->mc_StwFlashProtocols.size(); ++u32_Counter)
   {
      delete this->mc_StwFlashProtocols[u32_Counter];
      this->mc_StwFlashProtocols[u32_Counter] = NULL;
   }
   this->mc_StwFlashProtocols.clear();

   pr_XflReportProgress = NULL;
   pv_XflReportProgressInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all members

   \param[in]  orc_SystemDefinition    Entire system definition
   \param[in]  ou32_ActiveBusIndex     Bus index of bus in system definition where we are connected to
   \param[in]  orc_ActiveNodes         Flags for all available nodes in the system
   \param[in]  opc_CanDispatcher       Pointer to concrete CAN dispatcher
   \param[in]  opc_IpDispatcher        Pointer to concrete IP dispatcher

   \return
   C_NO_ERR      Operation success
   C_NOACT       No active nodes
   C_CONFIG      Invalid system definition for parameters
   C_RD_WR       Configured communication DLL does not exist
   C_OVERFLOW    Unknown transport protocol or unknown diagnostic server for at least one node
   C_COM         CAN initialization failed or no route found for at least one node
   C_CHECKSUM    Internal buffer overflow detected
   C_DEFAULT     Parameter ou32_ActiveBusIndex invalid
   C_RANGE       Routing configuration failed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::Init(const C_OSCSystemDefinition & orc_SystemDefinition, const uint32 ou32_ActiveBusIndex,
                                 const std::vector<uint8> & orc_ActiveNodes, C_CAN_Dispatcher * const opc_CanDispatcher,
                                 C_OSCIpDispatcher * const opc_IpDispatcher)
{
   sint32 s32_Return = C_OSCComDriverProtocol::Init(orc_SystemDefinition, ou32_ActiveBusIndex, orc_ActiveNodes,
                                                    opc_CanDispatcher, opc_IpDispatcher);

   if (s32_Return == C_NO_ERR)
   {
      s32_Return = this->m_InitFlashProtocol();
      if (s32_Return == C_NO_ERR)
      {
         this->mq_Initialized = true;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the new CAN bitrate

   An old connection will be closed.

   \param[in] ou32_Bitrate      Bitrate in kbit/s

   \return
   C_NO_ERR    Bitrate set
   C_COM       Error on reading bitrate
   C_CONFIG    No dispatcher installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::InitCanAndSetCanBitrate(const uint32 ou32_Bitrate)
{
   C_CAN_Dispatcher * const pc_CanDispatcher = this->m_GetCanDispatcher();
   sint32 s32_Return = C_CONFIG;

   if (pc_CanDispatcher != NULL)
   {
      pc_CanDispatcher->CAN_Exit();
      s32_Return = pc_CanDispatcher->CAN_Init(static_cast<sint32>(ou32_Bitrate));

      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_COM;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Configure openSYDE protocol polling timeout

   \param[in]    orc_ServerId      Server id for communication
   \param[in]    ou32_TimeoutMs    Timeout in ms

   \return
   C_NO_ERR   new timeout configured
   C_RANGE    openSYDE protocol not found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::OsySetPollingTimeout(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                 const uint32 ou32_TimeoutMs) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      pc_ExistingProtocol->SetTimeoutPolling(ou32_TimeoutMs);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reset the openSYDE protocol polling timeout

   \param[in]    orc_ServerId      Server id for communication

   \return
   C_NO_ERR   new timeout configured
   C_RANGE    openSYDE protocol not found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::OsyResetPollingTimeout(const C_OSCProtocolDriverOsyNode & orc_ServerId) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      pc_ExistingProtocol->ResetTimeoutPolling();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the minimum Flashloader reset wait time in ms

   Default minimum value is 500 ms independent of the type.

   \param[in]  oe_Type  Type of minimum flashloader reset wait time

   \return
   Time in ms all nodes need at least to get from application to the Flashloader or from Flashloader to Flashloader
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCComDriverFlash::GetMinimumFlashloaderResetWaitTime(const E_MinimumFlashloaderResetWaitTimeType oe_Type)
const
{
   uint32 u32_WaitTime = 500U;
   uint32 u32_Counter;

   for (u32_Counter = 0U; u32_Counter < this->mc_ActiveNodesIndexes.size(); ++u32_Counter)
   {
      uint32 u32_NodeWaitTime;
      tgl_assert(m_GetMinimumFlashloaderResetWaitTime(oe_Type, this->mc_ActiveNodesIndexes[u32_Counter],
                                                      u32_NodeWaitTime) == C_NO_ERR);
      if (u32_NodeWaitTime > u32_WaitTime)
      {
         // The node needs a longer wait time
         u32_WaitTime = u32_NodeWaitTime;
      }
   }

   return u32_WaitTime;
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
sint32 C_OSCComDriverFlash::GetMinimumFlashloaderResetWaitTime(const E_MinimumFlashloaderResetWaitTimeType oe_Type,
                                                               const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                               uint32 & oru32_TimeValue) const
{
   sint32 s32_Return = C_RANGE;
   bool q_Found = false;
   const uint32 u32_ActiveNodeIndex = this->m_GetActiveIndex(orc_ServerId, q_Found);

   if (q_Found == true)
   {
      s32_Return = this->m_GetMinimumFlashloaderResetWaitTime(oe_Type,
                                                              this->mc_ActiveNodesIndexes[u32_ActiveNodeIndex],
                                                              oru32_TimeValue);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send request programming

   Sending service as broadcast via CAN or ETH TP.
   Responses from servers will be collected.

   \param[out]    orq_NotAccepted   true: at least one "not accepted" response was received
                                    false: no "not accepted" response was received

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_CONFIG   no transport protocol installed or broadcast protocol not initialized
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyBroadcastRequestProgramming(bool & orq_NotAccepted) const
{
   sint32 s32_Return;

   orq_NotAccepted = false;

   if (this->mpc_CanTransportProtocolBroadcast != NULL)
   {
      std::vector<C_OSCProtocolDriverOsyTpCan::C_BroadcastRequestProgrammingResults> c_Results;
      s32_Return = this->mpc_CanTransportProtocolBroadcast->BroadcastRequestProgramming(c_Results);
      if (s32_Return == C_NO_ERR)
      {
         for (uint32 u32_ResponseIndex = 0U; u32_ResponseIndex < c_Results.size(); u32_ResponseIndex++)
         {
            if (c_Results[u32_ResponseIndex].q_RequestAccepted == false)
            {
               orq_NotAccepted = true;
               break;
            }
         }
      }
   }
   else
   {
      std::vector<C_OSCProtocolDriverOsyTpIp::C_BroadcastRequestProgrammingResults> c_Results;
      s32_Return = this->mpc_IpTransportProtocolBroadcast->BroadcastRequestProgramming(c_Results);
      if (s32_Return == C_NO_ERR)
      {
         for (uint32 u32_ResponseIndex = 0U; u32_ResponseIndex < c_Results.size(); u32_ResponseIndex++)
         {
            if (c_Results[u32_ResponseIndex].q_RequestAccepted == false)
            {
               orq_NotAccepted = true;
               break;
            }
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send network reset broadcast

   Use the constants provided by C_OSCProtocolDriverOsyTpBase for the parameter.

   \param[in]  ou8_ResetType        Reset type (0x02: keyOffOnReset, 0x60: resetToFlashloader)

   \return
   C_NO_ERR   no problems
   C_COM      could not send request
   C_CONFIG   no dispatcher installed or broadcast protocol not initialized
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyBroadcastEcuReset(const uint8 ou8_ResetType) const
{
   sint32 s32_Return;

   if ((this->mpc_CanTransportProtocolBroadcast == NULL) &&
       (this->mpc_IpTransportProtocolBroadcast == NULL))
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      if (this->mpc_CanTransportProtocolBroadcast != NULL)
      {
         s32_Return = this->mpc_CanTransportProtocolBroadcast->BroadcastEcuReset(ou8_ResetType);
      }
      else
      {
         s32_Return = this->mpc_IpTransportProtocolBroadcast->BroadcastNetReset(ou8_ResetType);
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send broadcast service EnterDiagnosticSession(PreProgramming)

   Sending service as broadcast. Only available for CAN broadcast.
   Only sends the request, does not wait for response.

   \return
   C_NO_ERR   no problems
   C_COM      could not send request
   C_CONFIG   no CAN dispatcher installed or broadcast protocol not initialized
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyCanBroadcastEnterPreProgrammingSession(void) const
{
   sint32 s32_Return;

   if (this->mpc_CanTransportProtocolBroadcast == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      s32_Return = this->mpc_CanTransportProtocolBroadcast->BroadcastSendEnterPreProgrammingSession();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send broadcast service EnterDiagnosticSession(Default)

   Sending service as broadcast. Only available for CAN broadcast.
   Only sends the request, does not wait for response.

   \return
   C_NO_ERR   no problems
   C_COM      could not send request
   C_CONFIG   no CAN dispatcher installed or broadcast protocol not initialized
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyCanBroadcastEnterDefaultSession(void) const
{
   sint32 s32_Return;

   if (this->mpc_CanTransportProtocolBroadcast == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      s32_Return = this->mpc_CanTransportProtocolBroadcast->BroadcastSendEnterDefaultSession();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read serial numbers of all devices on local bus

   \param[out]    orc_Responses   information about all nodes that sent a response

   \return
   C_NO_ERR   no problems; zero or more responses received; data placed in orc_Responses
   C_COM      could not send request
   C_CONFIG   no dispatcher installed or broadcast protocol not initialized
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyCanBroadcastReadSerialNumber(
   std::vector<C_OSCProtocolDriverOsyTpCan::C_BroadcastReadEcuSerialNumberResults> & orc_Responses) const
{
   sint32 s32_Return;

   if (this->mpc_CanTransportProtocolBroadcast == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      s32_Return = this->mpc_CanTransportProtocolBroadcast->BroadcastReadSerialNumber(orc_Responses);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reads the device name by service ReadDataByIdentifier

   \param[in]     orc_ServerId      Server id for communication
   \param[out]    orc_DeviceName    Result device name of service
   \param[out]    opu8_NrCode       if != NULL and error response: negative response code

   \return
   C_NO_ERR   request sent, positive response received
   C_RANGE    openSYDE protocol not found
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      error on creating temporary needed protocol
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyReadDeviceName(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                  C_SCLString & orc_DeviceName, uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   orc_DeviceName = "";

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->OsyReadDeviceName(orc_DeviceName, opu8_NrCode);
   }
   else
   {
      C_OSCProtocolDriverOsyTpCan c_TpCan;
      C_OSCProtocolDriverOsy c_OsyProtocol;

      // No device with this server id with openSYDE protocol exist. We need a temporary protocol.
      s32_Return = this->m_PrepareTemporaryOsyProtocol(orc_ServerId, c_OsyProtocol, c_TpCan);

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = c_OsyProtocol.OsyReadDeviceName(orc_DeviceName, opu8_NrCode);
      }
      else
      {
         s32_Return = C_COM;
      }
   }

   return s32_Return;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reads the serial number by service ReadDataByIdentifier

   \param[in]     orc_ServerId          Server id for communication
   \param[out]    orau8_SerialNumber    Read serial number
   \param[out]    opu8_NrCode           if != NULL and error response: negative response code

   \return
   C_NO_ERR   request sent, positive response received
   C_RANGE    openSYDE protocol not found
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      error on creating temporary needed protocol
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyReadSerialNumber(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                    uint8 (&orau8_SerialNumber)[6], uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->OsyReadEcuSerialNumber(orau8_SerialNumber, opu8_NrCode);
   }
   else
   {
      C_OSCProtocolDriverOsyTpCan c_TpCan;
      C_OSCProtocolDriverOsy c_OsyProtocol;

      // No device with this server id with openSYDE protocol exist. We need a temporary protocol.
      s32_Return = this->m_PrepareTemporaryOsyProtocol(orc_ServerId, c_OsyProtocol, c_TpCan);

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = c_OsyProtocol.OsyReadEcuSerialNumber(orau8_SerialNumber, opu8_NrCode);
      }
      else
      {
         s32_Return = C_COM;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node-id of node specified by serial number

   \param[in]    orau8_SerialNumber   serial number of node to address
   \param[in]    orc_NewNodeId        node ID to set

   \return
   C_NO_ERR    no problems; one positive response received
   C_RANGE     invalid node ID (bus id or node-ID out of range); "0x7F" is not permitted as node ID as it's reserved for
                broadcasts
   C_WARN      negative response received
   C_COM       could not send requests
   C_CONFIG    no dispatcher installed or broadcast protocol not initialized
   C_TIMEOUT   no response within timeout (was SetNodeIdentifiersForBroadcasts() called ?)
   C_OVERFLOW  multiple responses received
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyCanBroadcastSetNodeIdBySerialNumber(const uint8 (&orau8_SerialNumber)[6],
                                                                       const C_OSCProtocolDriverOsyNode & orc_NewNodeId)
const
{
   sint32 s32_Return;

   if (this->mpc_CanTransportProtocolBroadcast == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      s32_Return = this->mpc_CanTransportProtocolBroadcast->BroadcastSetNodeIdBySerialNumber(orau8_SerialNumber,
                                                                                             orc_NewNodeId);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send broadcast to get device information via ETH-TP

   \param[out]    orc_ReadDeviceInfoResults     received responses

   \return
   C_NO_ERR    no problems; zero or more responses received; data placed in c_ReadDeviceInfoResults
   C_COM       could not send request
   C_CONFIG    no dispatcher installed or ETH broadcast protocol not initialized
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyEthBroadcastGetDeviceInformation(
   std::vector<C_OSCProtocolDriverOsyTpIp::C_BroadcastGetDeviceInfoResults> & orc_ReadDeviceInfoResults) const
{
   sint32 s32_Return;

   if (this->mpc_IpTransportProtocolBroadcast == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      s32_Return = this->mpc_IpTransportProtocolBroadcast->BroadcastGetDeviceInfo(orc_ReadDeviceInfoResults);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send broadcast to set IP address via ETH-TP

   Send broadcast to change the IP address of one specific node.
   Only the node with a specified serial number is expected to send a response and change its IP address.
   The function will return as soon as it has received one response.

   Incoming UDP responses to other services will be dumped: we are strictly handshaking here ...

   \param[in]    orau8_SerialNumber    serial number of server to change IP on
   \param[in]    orau8_NewIpAddress    IP address to set
   \param[in]    orau8_NetMask         Net mask to set
   \param[in]    orau8_DefaultGateway  Default gateway to set
   \param[in]    orc_NewNodeId         New bus id and node id for the interface

   orau8_DefaultGateway

   \param[out]   orau8_ResponseIp     IP address the response was received from
   \param[out]   opu8_ErrorResult     if not NULL: code of error response (if C_WARN is returned)

   \return
   C_NO_ERR    no problems; one OK response received; response IP placed in orau8_ResponseIp
   C_COM       could not send request
   C_CONFIG    no dispatcher installed or ETH broadcast protocol not initialized
   C_TIMEOUT   no response within timeout
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyEthBroadcastSetIpAddress(const uint8 (&orau8_SerialNumber)[6],
                                                            const uint8 (&orau8_NewIpAddress)[4],
                                                            const uint8 (&orau8_NetMask)[4],
                                                            const uint8(&orau8_DefaultGateway)[4],
                                                            const C_OSCProtocolDriverOsyNode & orc_NewNodeId,
                                                            uint8 (&orau8_ResponseIp)[4],
                                                            uint8 * const opu8_ErrorResult) const
{
   sint32 s32_Return;

   if (this->mpc_IpTransportProtocolBroadcast == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      s32_Return = this->mpc_IpTransportProtocolBroadcast->BroadcastSetIpAddress(orau8_SerialNumber,
                                                                                 orau8_NewIpAddress,
                                                                                 orau8_NetMask,
                                                                                 orau8_DefaultGateway,
                                                                                 orc_NewNodeId,
                                                                                 orau8_ResponseIp,
                                                                                 opu8_ErrorResult);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send the request programming service to one node

   \param[in]     orc_ServerId      Server id for communication

   \return
   C_NO_ERR    Received positive response
   C_RANGE     openSYDE protocol not found
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Could not put request in Tx queue
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
   C_COM       communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyRequestProgramming(const C_OSCProtocolDriverOsyNode & orc_ServerId) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      // If the device is already in flashloader, the preprogramming session is needed.
      // If the device is in the application, this session request will return with an error. This error can be ignored.
      pc_ExistingProtocol->OsyDiagnosticSessionControl(C_OSCProtocolDriverOsy::hu8_DIAGNOSTIC_SESSION_PREPROGRAMMING,
                                                       NULL);
      s32_Return = pc_ExistingProtocol->OsyRequestProgramming();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send the ReadActiveDiagnosticSession service to one node and report response

   \param[in]     orc_ServerId      Server id for communication
   \param[out]    oru8_SessionId    reported session ID
   \param[out]    opu8_NrCode       if != NULL and error response: negative response code

   \return
   C_NO_ERR    Received positive response
   C_RANGE     openSYDE protocol not found
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Could not put request in Tx queue
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyReadActiveDiagnosticSession(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                               uint8 & oru8_SessionId, uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->OsyReadActiveDiagnosticSession(oru8_SessionId, opu8_NrCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send the ReadFlashBlockData service to read information about all flash blocks

   Executes the service in a loop starting from 0 (flashloader).
   Block 0 is expected to be present. Other blocks are optional.

   \param[in]     orc_ServerId      Server id for communication
   \param[out]    orc_BlockInfo     read server information
   \param[out]    opu8_NrCode       if != NULL and error response: negative response code

   \return
   C_NO_ERR    Read information for at least one block
   C_RANGE     openSYDE protocol not found
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Could not put request in Tx queue
   C_WARN      Error response received (except for requestOutOfRange for block > 0)
   C_TIMEOUT   Expected response not received within timeout
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyReadAllFlashBlockData(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                         std::vector<C_OSCProtocolDriverOsy::C_FlashBlockInfo> & orc_BlockInfo,
                                                         uint8 * const opu8_NrCode)
const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   orc_BlockInfo.resize(0);

   if (pc_ExistingProtocol != NULL)
   {
      for (uint16 u16_Block = 0U; u16_Block <= 0xFFU; u16_Block++)
      {
         uint8 u8_NrCode;
         C_OSCProtocolDriverOsy::C_FlashBlockInfo c_BlockInfo;
         s32_Return = pc_ExistingProtocol->OsyReadFlashBlockData(static_cast<uint8>(u16_Block), c_BlockInfo,
                                                                 &u8_NrCode);
         if (s32_Return == C_NO_ERR)
         {
            orc_BlockInfo.push_back(c_BlockInfo);
         }
         else
         {
            if (opu8_NrCode != NULL)
            {
               (*opu8_NrCode) = u8_NrCode;
            }

            if ((s32_Return == C_WARN) &&
                ((u16_Block > 0U) && (u8_NrCode == C_OSCProtocolDriverOsy::hu8_NR_CODE_REQUEST_OUT_OF_RANGE)))
            {
               //no more blocks ?
               //done here ... not a real problem
               s32_Return = C_NO_ERR;
            }
            break;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read various information from flashloader

   Read information from flashloader and fill in structure.

   \param[in]     orc_ServerId        Server id for communication
   \param[out]    orc_Information     read server information
   \param[out]    opu8_NrCode         if != NULL and error response: negative response code

   \return
   C_NO_ERR    Read information (placed in orc_Information)
   C_RANGE     openSYDE protocol not found
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Could not put request in Tx queue
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
   C_COM       communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyReadInformationFromFlashloader(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                                  C_FlashloaderInformation & orc_Information,
                                                                  uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return =
         pc_ExistingProtocol->OsyReadBootSoftwareIdentification(orc_Information.au8_FlashloaderSoftwareVersion,
                                                                opu8_NrCode);
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = pc_ExistingProtocol->OsyReadApplicationSoftwareFingerprint(
            orc_Information.au8_FlashFingerprintDate, orc_Information.au8_FlashFingerprintTime,
            orc_Information.c_FlashFingerprintUserName, opu8_NrCode);
      }

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = pc_ExistingProtocol->OsyReadEcuSerialNumber(orc_Information.au8_EcuSerialNumber, opu8_NrCode);
      }

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = pc_ExistingProtocol->OsyReadHardwareNumber(orc_Information.u32_EcuArticleNumber, opu8_NrCode);
      }

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = pc_ExistingProtocol->OsyReadHardwareVersionNumber(orc_Information.c_EcuHardwareVersionNumber,
                                                                        opu8_NrCode);
      }

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = pc_ExistingProtocol->OsyReadProtocolVersion(orc_Information.au8_ProtocolVersion, opu8_NrCode);
      }

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = pc_ExistingProtocol->OsyReadFlashloaderProtocolVersion(
            orc_Information.au8_FlashloaderProtocolVersion, opu8_NrCode);
      }

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = pc_ExistingProtocol->OsyReadFlashCount(orc_Information.u32_FlashCount, opu8_NrCode);
      }

      // Get available flashloader features
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = pc_ExistingProtocol->OsyReadListOfFeatures(orc_Information.c_AvailableFeatures, opu8_NrCode);
      }

      if ((s32_Return == C_NO_ERR) &&
          (orc_Information.c_AvailableFeatures.q_MaxNumberOfBlockLengthAvailable == true))
      {
         s32_Return = pc_ExistingProtocol->OsyReadMaxNumberOfBlockLength(orc_Information.u16_MaxNumberOfBlockLength,
                                                                         opu8_NrCode);
         if (s32_Return != C_NO_ERR)
         {
            orc_Information.u16_MaxNumberOfBlockLength = 0U;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Execute CheckFlashMemoryAvailable service

   \param[in]    orc_ServerId        Server id for communication
   \param[in]    ou32_StartAddress   Start address to check for
   \param[in]    ou32_Size           Data size to check for
   \param[out]    opu8_NrCode       if != NULL and error response: negative response code

   \return
   C_NO_ERR    service finished without problems
   C_RANGE     openSYDE protocol not found
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Could not put request in Tx queue
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
   C_COM       communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyCheckFlashMemoryAvailable(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                             const uint32 ou32_StartAddress, const uint32 ou32_Size,
                                                             uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->OsyCheckFlashMemoryAvailable(ou32_StartAddress, ou32_Size, opu8_NrCode);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Execute WriteApplicationSoftwareFingerprint service

   If a user name with more than 20 characters is passed only the first 20 will be used.

   \param[in] orc_ServerId          Server id for communication
   \param[in] orau8_Date            date of programming (yy.mm.dd)
   \param[in] orau8_Time            time of programming (hh:mm:ss)
   \param[in] orc_Username          name of operator
   \param[out]    opu8_NrCode       if != NULL and error response: negative response code

   \return
   C_NO_ERR    service finished without problems
   C_RANGE     openSYDE protocol not found
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Could not put request in Tx queue
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
   C_COM       communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyWriteApplicationSoftwareFingerprint(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                                       const uint8 (&orau8_Date)[3],
                                                                       const uint8 (&orau8_Time)[3],
                                                                       const C_SCLString & orc_Username,
                                                                       uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->OsyWriteApplicationSoftwareFingerprint(orau8_Date, orau8_Time, orc_Username,
                                                                               opu8_NrCode);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Execute RequestDownload service

   \param[in] orc_ServerId           Server id for communication
   \param[in] ou32_StartAddress      Start address to write to
   \param[in] ou32_Size              Number of bytes we want to write to the start address
   \param[out] oru32_MaxBlockLength  Maximum number of bytes we may send in one subsequent "TransferData" service
   \param[out] opu8_NrCode           if != NULL and error response: negative response code

   \return
   C_NO_ERR    service finished without problems
   C_RANGE     openSYDE protocol not found
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Could not put request in Tx queue
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
   C_COM       communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyRequestDownload(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                   const uint32 ou32_StartAddress, const uint32 ou32_Size,
                                                   uint32 & oru32_MaxBlockLength, uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->OsyRequestDownload(ou32_StartAddress, ou32_Size, oru32_MaxBlockLength,
                                                           opu8_NrCode);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Execute RequestFileTransfer service

   \param[in] orc_ServerId           Server id for communication
   \param[in] orc_FilePath           File to write
   \param[in] ou32_FileSize          Number of bytes we want to transfer
   \param[out] oru32_MaxBlockLength  Maximum number of bytes we may send in one subsequent "TransferData" service
   \param[out] opu8_NrCode           if != NULL and error response: negative response code

   \return
   C_NO_ERR    service finished without problems
   C_RANGE     openSYDE protocol not found; file path too long
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Could not put request in Tx queue
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
   C_COM       communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyRequestFileTransfer(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                       const C_SCLString & orc_FilePath, const uint32 ou32_FileSize,
                                                       uint32 & oru32_MaxBlockLength, uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->OsyRequestFileTransfer(orc_FilePath, ou32_FileSize, oru32_MaxBlockLength,
                                                               opu8_NrCode);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Execute TransferData service

   \param[in]  orc_ServerId               Server id for communication
   \param[in]  ou8_BlockSequenceCounter   sequence counter 0-255; first black starts with 1
   \param[in]  orc_Data                   data to be programmed to flash memory
   \param[out]    opu8_NrCode       if != NULL and error response: negative response code

   \return
   C_NO_ERR    service finished without problems
   C_RANGE     openSYDE protocol not found
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Could not put request in Tx queue
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
   C_COM       communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyTransferData(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                const uint8 ou8_BlockSequenceCounter, std::vector<uint8> & orc_Data,
                                                uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->OsyTransferData(ou8_BlockSequenceCounter, orc_Data, opu8_NrCode);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Execute TransferExitAddressBased service

   \param[in]  orc_ServerId               Server id for communication
   \param[in]  oq_SendSignatureBlockAddress  true: it's the last area of a block; send the signature address
                                             false: more areas will follow; do not send the signature address
   \param[in]  ou32_SignatureBlockAddress    address of signature block in flash covering the application/data
                                             previously programmed by service TransferData.
                                             (only used if oq_SendSignatureBlockAddress is true)
   \param[out]    opu8_NrCode       if != NULL and error response: negative response code

   \return
   C_NO_ERR    service finished without problems
   C_RANGE     openSYDE protocol not found
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Could not put request in Tx queue
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
   C_COM       communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyRequestTransferExitAddressBased(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                                   const bool oq_SendSignatureBlockAddress,
                                                                   const uint32 ou32_SignatureBlockAddress,
                                                                   uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->OsyRequestTransferExitAddressBased(oq_SendSignatureBlockAddress,
                                                                           ou32_SignatureBlockAddress, opu8_NrCode);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Execute TransferExitFileBased service

   \param[in]  orc_ServerId               Server id for communication
   \param[in]  ou32_CrcOverData           CRC32 over data sent with TransferData
   \param[out] opu8_NrCode                if != NULL and error response: negative response code

   \return
   C_NO_ERR    service finished without problems
   C_RANGE     openSYDE protocol not found
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Could not put request in Tx queue
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
   C_COM       communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyRequestTransferExitFileBased(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                                const uint32 ou32_CrcOverData,
                                                                uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      uint8 au8_Signature[8];
      //place the CRC into the first four bytes; rest is unused
      au8_Signature[0] = static_cast<uint8>(ou32_CrcOverData >> 24U);
      au8_Signature[1] = static_cast<uint8>(ou32_CrcOverData >> 16U);
      au8_Signature[2] = static_cast<uint8>(ou32_CrcOverData >> 8U);
      au8_Signature[3] = static_cast<uint8>(ou32_CrcOverData);
      au8_Signature[4] = 0U;
      au8_Signature[5] = 0U;
      au8_Signature[6] = 0U;
      au8_Signature[7] = 0U;
      s32_Return = pc_ExistingProtocol->OsyRequestTransferExitFileBased(au8_Signature, opu8_NrCode);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send the reset ECU service request to one node

   \param[in]     orc_ServerId      Server id for communication
   \param[in]     ou8_ResetType     Reset type (0x02: keyOffOnReset, 0x60: resetToFlashloader)

   \return
   C_NO_ERR    Request sent
   C_RANGE     openSYDE protocol not found
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Could not put request in Tx queue
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyEcuReset(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                            const uint8 ou8_ResetType) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->OsyEcuReset(ou8_ResetType);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the session to preprogramming and the necessary security access to level 1

   \param[in]     orc_ServerId      Server id for communication
   \param[in]     oq_SessionOnly    true: only set session; don't set security access
   \param[out]    opu8_NrCode       if != NULL: negative response code

   \return
   C_NO_ERR    Session and security access set successfully
   C_RANGE     openSYDE protocol not found
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Nodes has no openSYDE protocol
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsySetPreProgrammingMode(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                         const bool oq_SessionOnly, uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   bool q_Found = false;
   const uint32 u32_ActiveNodeIndex = this->m_GetActiveIndex(orc_ServerId, q_Found);

   if (q_Found == true)
   {
      s32_Return = this->m_SetNodeSessionId(u32_ActiveNodeIndex,
                                            C_OSCProtocolDriverOsy::hu8_DIAGNOSTIC_SESSION_PREPROGRAMMING,
                                            false, opu8_NrCode);

      if ((s32_Return == C_NO_ERR) && (oq_SessionOnly == false))
      {
         s32_Return = this->m_SetNodeSecurityAccess(u32_ActiveNodeIndex, 1, opu8_NrCode);
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the session to programming and the necessary security access to level 3

   \param[in]     orc_ServerId      Server id for communication
   \param[out]    opu8_NrCode       if != NULL: negative response code

   \return
   C_NO_ERR    Session and security access set successfully
   C_RANGE     openSYDE protocol not found
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Nodes has no openSYDE protocol
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsySetProgrammingMode(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                      uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   bool q_Found = false;
   const uint32 u32_ActiveNodeIndex = this->m_GetActiveIndex(orc_ServerId, q_Found);

   if (q_Found == true)
   {
      s32_Return = this->m_SetNodeSessionId(u32_ActiveNodeIndex,
                                            C_OSCProtocolDriverOsy::hu8_DIAGNOSTIC_SESSION_PROGRAMMING,
                                            false, opu8_NrCode);

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = this->m_SetNodeSecurityAccess(u32_ActiveNodeIndex, 3U, opu8_NrCode);
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set specified security level

   \param[in]     orc_ServerId      Server id for communication
   \param[in]     ou8_Level         security level to set

   \return
   C_NO_ERR    Session and security access set successfully
   C_RANGE     openSYDE protocol not found
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Nodes has no openSYDE protocol
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsySetSecurityLevel(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                    const uint8 ou8_Level) const
{
   sint32 s32_Return = C_RANGE;
   bool q_Found = false;
   const uint32 u32_ActiveNodeIndex = this->m_GetActiveIndex(orc_ServerId, q_Found);

   if (q_Found == true)
   {
      s32_Return = this->m_SetNodeSecurityAccess(u32_ActiveNodeIndex, ou8_Level, NULL);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new bitrate on an openSYDE server

   Only for CAN

   \param[in]  orc_ServerId          Server id for communication
   \param[in]  ou8_ChannelIndex      selected channel index
   \param[in]  ou32_Bitrate          new bitrate configuration for the server in bit/s
   \param[out]    opu8_NrCode       if != NULL and error response: negative response code

   \return
   C_NO_ERR   Bitrate was set successful
   C_RANGE    openSYDE protocol not found
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsySetBitrate(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                              const uint8 ou8_ChannelIndex, const uint32 ou32_Bitrate,
                                              uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->OsySetBitrate(0U, ou8_ChannelIndex, ou32_Bitrate, opu8_NrCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set the ip address for the referenced channel to the given value

   Only for Ethernet

   \param[in]  orc_ServerId          Server id for communication
   \param[in]  ou8_ChannelIndex      selected channel index
   \param[in]  orau8_IpAddress       IP V4 address
   \param[in]  orau8_NetMask         IP V4 net mask
   \param[in]  orau8_DefaultGateway  IP V4 default gateway
   \param[out]    opu8_NrCode       if != NULL and error response: negative response code

   \return
   C_NO_ERR   Bitrate was set successful
   C_RANGE    openSYDE protocol not found
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response
   C_RD_WR    unexpected content in response (here: wrong routine identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsySetIpAddressForChannel(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                          const uint8 ou8_ChannelIndex,
                                                          const uint8 (&orau8_IpAddress)[4],
                                                          const uint8 (&orau8_NetMask)[4],
                                                          const uint8 (&orau8_DefaultGateway)[4],
                                                          uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->OsySetIpAddressForChannel(1U, ou8_ChannelIndex, orau8_IpAddress, orau8_NetMask,
                                                                  orau8_DefaultGateway, opu8_NrCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets node id and bus id for specific communication channel

   \param[in]  orc_ServerId          Server id for communication
   \param[in]  ou8_ChannelType       selected channel type (0 equals CAN, 1 equals Ethernet)
   \param[in]  ou8_ChannelIndex      selected channel index
   \param[in]  orc_NewNodeId         new bus id (0..15) and node id (0..126) configuration for the server
   \param[out] opu8_NrCode           if != NULL and error response: negative response code

   \return
   C_NO_ERR   Bitrate was set successful
   C_RANGE    openSYDE protocol not found
              ou8_BusId or ou8_NodeId is out of range
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsySetNodeIdForChannel(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                       const uint8 ou8_ChannelType, const uint8 ou8_ChannelIndex,
                                                       const C_OSCProtocolDriverOsyNode & orc_NewNodeId,
                                                       uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->OsySetNodeIdForChannel(ou8_ChannelType, ou8_ChannelIndex,
                                                               orc_NewNodeId, opu8_NrCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets node id and bus id for specific communication channel

   \param[in]  orc_ServerId          Server id for communication
   \param[out] orc_ListOfFeatures    List of server features
   \param[out] opu8_NrCode           if != NULL and error response: negative response code

   \return
   C_NO_ERR   List of features was read successfully
   C_RANGE    openSYDE protocol not found
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendOsyReadListOfFeatures(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                      C_OSCProtocolDriverOsy::C_ListOfFeatures & orc_ListOfFeatures,
                                                      stw_types::uint8 * const opu8_NrCode) const
{
   sint32 s32_Return = C_RANGE;
   C_OSCProtocolDriverOsy * const pc_ExistingProtocol = this->m_GetOsyProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->OsyReadListOfFeatures(orc_ListOfFeatures, opu8_NrCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send reset request for all STW flashloader by specific reset message

   \return
   C_NO_ERR    Reset request sent
   C_RANGE     Reset message is invalid
   C_COM       Error on sending reset request
   C_CONFIG    Not initialized or not valid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendStwRequestNodeReset(void)
{
   sint32 s32_Return = C_CONFIG;

   if (this->mq_Initialized == true)
   {
      uint32 u32_Counter;
      std::vector<T_STWCAN_Msg_TX> c_ResetMsgs;

      s32_Return = C_NO_ERR;

      // Get all relevant reset messages
      for (u32_Counter = 0U; u32_Counter < this->m_GetActiveNodeCount(); ++u32_Counter)
      {
         if (this->mc_ActiveNodesIndexes[u32_Counter] < this->mpc_SysDef->c_Nodes.size())
         {
            T_STWCAN_Msg_TX t_ResetMsg;

            s32_Return = this->m_GetStwResetMessage(this->mc_ActiveNodesIndexes[u32_Counter], t_ResetMsg);

            if (s32_Return == C_NO_ERR)
            {
               c_ResetMsgs.push_back(t_ResetMsg);
            }
            else if (s32_Return == C_NOACT)
            {
               s32_Return = C_NO_ERR;
            }
            else
            {
               // Error
               break;
            }
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         // Send the service with the first STW flashloader protocol
         for (u32_Counter = 0U; u32_Counter < this->m_GetActiveNodeCount(); ++u32_Counter)
         {
            if (this->mc_StwFlashProtocols[u32_Counter] != NULL)
            {
               uint32 u32_MsgCounter;

               // Send all reset messages with the same protocol. No dependency to the concrete node id
               for (u32_MsgCounter = 0U; u32_MsgCounter < c_ResetMsgs.size(); ++u32_MsgCounter)
               {
                  s32_Return = this->mc_StwFlashProtocols[u32_Counter]->RequestNodeReset(&c_ResetMsgs[u32_MsgCounter]);

                  if (s32_Return != C_NO_ERR)
                  {
                     s32_Return = C_COM;
                     break;
                  }
               }
               break;
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send reset request for one STW flashloader by specific reset message

   \param[in]     orc_ServerId             Server id for communication

   \return
   C_NO_ERR    Reset request sent
   C_RANGE     Reset message is invalid
   C_COM       Error on sending reset request
   C_CONFIG    Not initialized or not valid
   C_NOACT     No reset message configured or node has no STW flashloader
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendStwRequestNodeReset(const C_OSCProtocolDriverOsyNode & orc_ServerId)
{
   sint32 s32_Return = C_CONFIG;
   uint32 u32_NodeIndex;

   if ((this->mq_Initialized == true) &&
       (this->GetNodeIndex(orc_ServerId, u32_NodeIndex) == true))
   {
      T_STWCAN_Msg_TX t_ResetMsg;
      C_OSCFlashProtocolStwFlashloader c_StwProtocol(pr_XflReportProgress, pv_XflReportProgressInstance);
      C_OSCFlashProtocolStwFlashloader * pc_ExistingProtocol = this->m_GetStwFlashloaderProtocol(orc_ServerId);

      if (pc_ExistingProtocol == NULL)
      {
         // No device with this server id with STW protocol exists. We need a temporary protocol.
         this->m_InitFlashProtocolStw(&c_StwProtocol, orc_ServerId.u8_NodeIdentifier);
         pc_ExistingProtocol = &c_StwProtocol;
      }

      s32_Return = this->m_GetStwResetMessage(u32_NodeIndex, t_ResetMsg);
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = pc_ExistingProtocol->RequestNodeReset(&t_ResetMsg);

         if (s32_Return != C_NO_ERR)
         {
            s32_Return = C_COM;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send "FLASH" requests

   Send the "FLASH" request for one time.

   \param[in]     orc_ServerId             Server id for communication

   \return
   C_NO_ERR      finished sending "FLASH"   \n
   C_DEFAULT     aborted by user
   C_CONFIG      no STW flashloader protocol initialized
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendStwSendFlash(const C_OSCProtocolDriverOsyNode & orc_ServerId)
{
   sint32 s32_Return;
   C_OSCFlashProtocolStwFlashloader c_StwProtocol(pr_XflReportProgress, pv_XflReportProgressInstance);
   C_OSCFlashProtocolStwFlashloader * pc_ExistingProtocol = this->m_GetStwFlashloaderProtocol(orc_ServerId);

   if (pc_ExistingProtocol == NULL)
   {
      // No device with this server id with STW protocol exists. We need a temporary protocol.
      this->m_InitFlashProtocolStw(&c_StwProtocol, orc_ServerId.u8_NodeIdentifier);
      pc_ExistingProtocol = &c_StwProtocol;
   }

   s32_Return = pc_ExistingProtocol->SendFLASH(0, 0);

   if (s32_Return != C_NO_ERR)
   {
      s32_Return = C_COM;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Scan for nodes in the network.

   \param[out]     orau8_LocalIDs     Number of responses for each ID.
                                       e.g. orau8_LocalIDs[1] contains the number of responding
                                       controllers on Local ID 1 after call.
   \param[out]     oru8_NodeFounds    Total number of found nodes

   \return
   C_NO_ERR    all ok
   C_COM       error on sending
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendStwSearchId(uint8 (&orau8_LocalIDs)[XFL_NUM_DIFFERENT_LOCAL_IDS],
                                            uint8 & oru8_NodeFounds)
{
   C_OSCFlashProtocolStwFlashloader c_StwProtocol(pr_XflReportProgress, pv_XflReportProgressInstance);

   // No concrete device. We need a temporary protocol.
   this->m_InitFlashProtocolStw(&c_StwProtocol, 0);

   // Send "FLASH" will be handled separately. Minimum parameter.
   return c_StwProtocol.SearchId(orau8_LocalIDs, &oru8_NodeFounds, 0, 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Perform a wakeup with local ID

   \param[in]     orc_ServerId      Server id for communication
   \param[out]    opu8_NodesFound   Number of found nodes (optional)

   \return
   C_NO_ERR           no errors
   C_COM              no response from server
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendStwWakeupLocalId(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                 uint8 * const opu8_NodesFound)
{
   sint32 s32_Return;
   C_OSCFlashProtocolStwFlashloader * const pc_ExistingProtocol = this->m_GetStwFlashloaderProtocol(orc_ServerId);

   C_XFLCompanyID c_ReceivedCompanyId;

   if (opu8_NodesFound != NULL)
   {
      *opu8_NodesFound = 0U;
   }

   if (pc_ExistingProtocol != NULL)
   {
      s32_Return = pc_ExistingProtocol->WakeupLocalId(this->mc_CompanyId, opu8_NodesFound,
                                                      &c_ReceivedCompanyId);
   }
   else
   {
      C_OSCFlashProtocolStwFlashloader c_StwProtocol(pr_XflReportProgress, pv_XflReportProgressInstance);

      // No device with this server id with STW protocol exist. We need a temporary protocol.
      this->m_InitFlashProtocolStw(&c_StwProtocol, orc_ServerId.u8_NodeIdentifier);
      s32_Return = c_StwProtocol.WakeupLocalId(this->mc_CompanyId, opu8_NodesFound, &c_ReceivedCompanyId);
   }

   if (s32_Return == C_WARN)
   {
      // An other company id
      stw_scl::C_SCLString c_CompanyId;

      C_OSCFlashProtocolStwFlashloader::CompIDStructToString(c_ReceivedCompanyId, c_CompanyId);

      osc_write_log_info("SendStwWakeupLocalId", C_SCLString("Other company id found: ") + c_CompanyId);

      // An other company id is no error
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Perform a wakeup with serial number

   \param[in]     orau8_SerialNumber   Serial number to send
   \param[out]    oru8_LocalId         Local id of server with the SN

   \return
   C_NO_ERR           no errors
   C_COM              no response from server
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendStwWakeupLocalSerialNumber(const uint8 (&orau8_SerialNumber)[6], uint8 & oru8_LocalId)
{
   sint32 s32_Return;
   C_XFLCompanyID c_ReceivedCompanyId;

   C_OSCFlashProtocolStwFlashloader c_StwProtocol(pr_XflReportProgress, pv_XflReportProgressInstance);

   // No concrete device. We need a temporary protocol.
   this->m_InitFlashProtocolStw(&c_StwProtocol, 0);

   s32_Return = c_StwProtocol.WakeupSerialNumber(orau8_SerialNumber, this->mc_CompanyId, oru8_LocalId,
                                                 &c_ReceivedCompanyId);

   if (s32_Return == C_WARN)
   {
      // An other company id
      stw_scl::C_SCLString c_CompanyId;

      C_OSCFlashProtocolStwFlashloader::CompIDStructToString(c_ReceivedCompanyId, c_CompanyId);

      osc_write_log_info("SendStwWakeupLocalId", C_SCLString("Other company id found: ") + c_CompanyId.c_str());

      // An other company id is no error
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read the SNR(s) from the controller(s) specified by LocalId

   \param[in]     orc_ServerId             Server id for communication
   \param[out]    opu8_SerialNumbers       Returns the received SNR(s), each 6 bytes
                                              the calling function has to provide a buffer large enough
                                              Set to NULL if only interested in the number of responses.
   \param[in]     ou8_NumMax               Maximum number of answers to record to prevent overflow in opu8_SerialNumbers
   \param[in,out] oru8_NumFound            Number of responses

   \return
   C_NO_ERR       no errors
   C_COM          no response
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendStwGetSerialNumbers(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                    uint8 * const opu8_SerialNumbers, const uint8 ou8_NumMax,
                                                    uint8 & oru8_NumFound)
{
   sint32 s32_Return;
   C_OSCFlashProtocolStwFlashloader c_StwProtocol(pr_XflReportProgress, pv_XflReportProgressInstance);
   C_OSCFlashProtocolStwFlashloader * pc_ExistingProtocol = this->m_GetStwFlashloaderProtocol(orc_ServerId);

   if (pc_ExistingProtocol == NULL)
   {
      // No device with this server id with STW protocol exists. We need a temporary protocol.
      this->m_InitFlashProtocolStw(&c_StwProtocol, orc_ServerId.u8_NodeIdentifier);
      pc_ExistingProtocol = &c_StwProtocol;
   }

   s32_Return = pc_ExistingProtocol->GetSNRExt(opu8_SerialNumbers, ou8_NumMax, oru8_NumFound);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read device-ID from server node

   \param[in]     orc_ServerId         Server id for communication
   \param[out]    orc_DeviceName       Device id/name (max.16 characters)

   \return
   C_NO_ERR           no errors
   C_COM              no response from server
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendStwGetDeviceId(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                               C_SCLString & orc_DeviceName)
{
   sint32 s32_Return;
   C_OSCFlashProtocolStwFlashloader c_StwProtocol(pr_XflReportProgress, pv_XflReportProgressInstance);
   C_OSCFlashProtocolStwFlashloader * pc_ExistingProtocol = this->m_GetStwFlashloaderProtocol(orc_ServerId);
   uint16 u16_ProtocolVersion;
   bool q_LongId;

   if (pc_ExistingProtocol == NULL)
   {
      // No device with this server id with STW protocol exists. We need a temporary protocol.
      this->m_InitFlashProtocolStw(&c_StwProtocol, orc_ServerId.u8_NodeIdentifier);
      pc_ExistingProtocol = &c_StwProtocol;
   }

   //check for supported version of service depending on the protocol version:
   s32_Return = pc_ExistingProtocol->GetImplementationInformationProtocolVersion(u16_ProtocolVersion);

   //no response or error response: we have to assume we have a version <= V3.00r0
   q_LongId = ((s32_Return == C_NO_ERR) && (u16_ProtocolVersion >= mhu16_STW_FLASHLOADER_PROTOCOL_VERSION_3_00));
   orc_DeviceName = "";
   s32_Return = pc_ExistingProtocol->GetDeviceID(q_LongId, orc_DeviceName);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   short description of function

   long description of function within several lines

   \param[in]     orc_ServerId             Server id for communication
   \param[in]     ou8_NewLocalId           New local id for server

   \return
   C_NO_ERR     no errors
   C_COM        no response
   C_NOACT      error response
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendStwSetLocalId(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                              const uint8 ou8_NewLocalId)
{
   sint32 s32_Return;

   C_OSCFlashProtocolStwFlashloader c_StwProtocol(pr_XflReportProgress, pv_XflReportProgressInstance);

   // We change the server Id and do not want to change the server Id of existing protocols.
   // We need a temporary protocol.
   this->m_InitFlashProtocolStw(&c_StwProtocol, orc_ServerId.u8_NodeIdentifier);

   s32_Return = c_StwProtocol.SetLocalID(ou8_NewLocalId);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change the flash bitrate of a node

   \param[in]     orc_ServerId             Server id for communication
   \param[in]     ou32_Bitrate             New bitrate for server in Bits/s

   \return
   C_NO_ERR   no errors
   C_COM      no response
   C_NOACT    error response
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendStwSetBitrateCan(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                 const uint32 ou32_Bitrate)
{
   sint32 s32_Return;
   C_OSCFlashProtocolStwFlashloader c_StwProtocol(pr_XflReportProgress, pv_XflReportProgressInstance);
   C_OSCFlashProtocolStwFlashloader * pc_ExistingProtocol = this->m_GetStwFlashloaderProtocol(orc_ServerId);

   if (pc_ExistingProtocol == NULL)
   {
      // No device with this server id with STW protocol exists. We need a temporary protocol.
      this->m_InitFlashProtocolStw(&c_StwProtocol, orc_ServerId.u8_NodeIdentifier);
      pc_ExistingProtocol = &c_StwProtocol;
   }

   // We support only 16 bit bitrate for STW flashloader -> bitrate resolution kbit/second
   s32_Return = pc_ExistingProtocol->SetBitrateCAN(ou32_Bitrate / 1000U, false);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends net reset request for STW flashloader devices for a specific device

   \param[in]     orc_ServerId             Server id for communication

   \return
   C_NO_ERR    Net reset request sent
   C_COM       Error on sending reset request
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendStwNetReset(const C_OSCProtocolDriverOsyNode & orc_ServerId)
{
   sint32 s32_Return;
   C_OSCFlashProtocolStwFlashloader c_StwProtocol(pr_XflReportProgress, pv_XflReportProgressInstance);
   C_OSCFlashProtocolStwFlashloader * pc_ExistingProtocol = this->m_GetStwFlashloaderProtocol(orc_ServerId);

   if (pc_ExistingProtocol == NULL)
   {
      // No device with this server id with STW protocol exists. We need a temporary protocol.
      this->m_InitFlashProtocolStw(&c_StwProtocol, orc_ServerId.u8_NodeIdentifier);
      pc_ExistingProtocol = &c_StwProtocol;
   }

   s32_Return = pc_ExistingProtocol->NetReset();

   if (s32_Return != C_NO_ERR)
   {
      s32_Return = C_COM;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends net reset request for STW flashloader devices

   \return
   C_NO_ERR    Net reset request sent
   C_COM       Error on sending reset request
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendStwNetReset(void)
{
   sint32 s32_Return;

   C_OSCFlashProtocolStwFlashloader c_StwProtocol(pr_XflReportProgress, pv_XflReportProgressInstance);

   // No concrete device. We need a temporary protocol.
   this->m_InitFlashProtocolStw(&c_StwProtocol, 0);

   // Send "FLASH" will be handled separately. Minimum parameter.
   s32_Return = c_StwProtocol.NetReset();

   if (s32_Return != C_NO_ERR)
   {
      s32_Return = C_COM;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read all kinds of information from STW flashloader

   Uses protocol services to read information about target device

   \param[in]     orc_ServerId             Server id for communication
   \param[out]    orc_Information          basic information
   \param[out]    orc_ChecksumInformation  information about block or sector checksums

   \return
   C_NO_ERR    information read
   C_COM       communication error
   C_CONFIG    node identified by orc_ServerId is unknown or does not speak STW Flashloader
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendStwReadDeviceInformation(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                         C_XFLInformationFromServer & orc_Information,
                                                         C_XFLChecksumAreas & orc_ChecksumInformation) const
{
   sint32 s32_Return = C_CONFIG;
   C_OSCFlashProtocolStwFlashloader * const pc_ExistingProtocol = this->m_GetStwFlashloaderProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      //always returns C_NO_ERR
      (void)pc_ExistingProtocol->ReadServerInformation(orc_Information);

      if ((orc_Information.c_AvailableFeatures.q_BlockBasedCRCsEEPROM == true) ||
          (orc_Information.c_AvailableFeatures.q_BlockBasedCRCsFlash == true))
      {
         s32_Return = pc_ExistingProtocol->ReadServerBlockChecksumInformation(orc_ChecksumInformation);
      }
      else
      {
         s32_Return = pc_ExistingProtocol->ReadServerSectorChecksumInformation(orc_Information.u16_SectorCount,
                                                                               orc_ChecksumInformation);
      }
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_COM;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write one hex file to an STW Flashloader node

   Prerequisites:
   * node is already in flashloader (no "FLASH" sequence will be performed)

   \param[in]     orc_ServerId             Server id to write the hex file to
   \param[in]     orc_HexFilePath          path to hex file to flash

   \return
   C_NO_ERR    hex file written
   C_CONFIG    node identified by orc_ServerId is unknown or does not speak STW Flashloader
   else        problem during flashing; details can be seen in the progress log
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::SendStwDoFlash(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                           const stw_scl::C_SCLString & orc_HexFilePath) const
{
   sint32 s32_Return = C_CONFIG;
   C_OSCFlashProtocolStwFlashloader * const pc_ExistingProtocol = this->m_GetStwFlashloaderProtocol(orc_ServerId);

   if (pc_ExistingProtocol != NULL)
   {
      C_XFLFlashWriteParameters c_Params;
      c_Params.u16_Version = CXFLFLASHWRITE_VERSION;
      c_Params.c_HexFile = orc_HexFilePath;
      c_Params.c_WakeupConfig.e_WakeupMode = eXFL_WAKEUP_MODE_LID;
      s32_Return = C_XFLActions::CompIDStructToString(this->mc_CompanyId, c_Params.c_WakeupConfig.c_CompanyID);
      tgl_assert(s32_Return == C_NO_ERR);                  //no plausible reason for this
      c_Params.c_WakeupConfig.q_SendFLASHRequired = false; //we are already in flashloader
      c_Params.c_WakeupConfig.q_SendResetRQ = false;       //we are already in flashloader
      c_Params.c_WakeupConfig.u8_LocalID = orc_ServerId.u8_NodeIdentifier;
      c_Params.e_EraseMode = eXFL_ERASE_MODE_AUTOMATIC; //only automatic mode supported
      c_Params.e_FlashFinishedAction =
         eXFL_FLASH_FINISHED_ACTION_NODE_SLEEP;   //go back to sleep after flashing
      c_Params.u16_InterFrameDelayUs = 0U;        //don't consider ancient hardware and targets
      c_Params.u8_HexRecordLength = 0U;           //keep hex record length as it is (unless we have the V3.x mechanisms)
      c_Params.q_XtdID       = mhq_XFL_ID_XTD;    //fixed IDs
      c_Params.u32_SendID    = mhu32_XFL_ID_TX;   //fixed IDs
      c_Params.u32_ReceiveID = mhu32_XFL_ID_RX;   //fixed IDs
      c_Params.q_DivertStream = false;            //no legacy support for ESX2 BBBs
      c_Params.q_WriteCRCsIfSupported = true;     //update CRCs
      c_Params.q_VerboseMode = true;              //give us some more progress information
      c_Params.q_XFLExchange = false;             //regular flashing
      c_Params.u8_IgnoreInvalidHexfileError = 0U; //always fail if the hex file is invalid

      c_Params.e_DevTypeCheck = eXFL_DEV_TYPE_CHECK_NONE; //no check performed

      s32_Return = pc_ExistingProtocol->ExecuteWrite(c_Params);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare for shutting down class

   To be called by child classes on shutdown, before they destroy all owned class instances
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCComDriverFlash::PrepareForDestructionFlash(void)
{
   this->PrepareForDestruction();
   for (uint32 u32_Counter = 0U; u32_Counter < this->mc_StwFlashProtocols.size(); ++u32_Counter)
   {
      C_OSCFlashProtocolStwFlashloader * const pc_FlashProtocol = this->mc_StwFlashProtocols[u32_Counter];
      if (pc_FlashProtocol != NULL)
      {
         pc_FlashProtocol->CfgSetCommDispatcher(NULL);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the information about the routing configuration

   \param[out]    ore_Mode       Needed routing mode

   \return
   true     Routing is necessary
   false    Routing is not necessary
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCComDriverFlash::m_GetRoutingMode(C_OSCRoutingCalculation::E_Mode & ore_Mode) const
{
   ore_Mode = C_OSCRoutingCalculation::eUPDATE;

   return this->mq_RoutingActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns needed session ID for the current routing mode

   \return
   Session ID for flashloader
*/
//----------------------------------------------------------------------------------------------------------------------
uint8 C_OSCComDriverFlash::m_GetRoutingSessionId(void) const
{
   return C_OSCProtocolDriverOsy::hu8_DIAGNOSTIC_SESSION_PREPROGRAMMING;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the routing for a not openSYDE server is necessary

   \param[in]     orc_Node                             Current node

   \return
   true    Specific server and legacy routing necessary
   false   No specific server and legacy routing necessary
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCComDriverFlash::m_IsRoutingSpecificNecessary(const C_OSCNode & orc_Node) const
{
   bool q_Return = false;

   if (orc_Node.c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_STW)
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepares the routing for STW flashloader protocol

   long description of function within several lines

   \param[in]     ou32_ActiveNode                      Active node index of vector mc_ActiveNodes
   \param[in]     opc_Node                             Pointer to current node
   \param[in]     orc_LastNodeOfRouting                The last node in the routing chain before the final target server
   \param[in]     opc_ProtocolOsyOfLastNodeOfRouting   The protocol of the last node
   \param[out]    oppc_RoutingDispatcher               The legacy routing dispatcher

   \return
   C_NO_ERR    Specific server necessary
   C_NOACT     No specific server necessary
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::m_StartRoutingSpecific(const uint32 ou32_ActiveNode, const C_OSCNode * const opc_Node,
                                                   const C_OSCRoutingRoutePoint & orc_LastNodeOfRouting,
                                                   C_OSCProtocolDriverOsy * const opc_ProtocolOsyOfLastNodeOfRouting,
                                                   C_OSCCanDispatcherOsyRouter ** const oppc_RoutingDispatcher)
{
   sint32 s32_Return = C_NOACT;

   if (opc_Node->c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_STW)
   {
      C_OSCFlashProtocolStwFlashloader * const pc_StwFlashloader =
         dynamic_cast<C_OSCFlashProtocolStwFlashloader * const>(this->mc_StwFlashProtocols[ou32_ActiveNode]);

      if ((pc_StwFlashloader != NULL) &&
          (opc_ProtocolOsyOfLastNodeOfRouting != NULL))
      {
         (*oppc_RoutingDispatcher) = new C_OSCCanDispatcherOsyRouter(*opc_ProtocolOsyOfLastNodeOfRouting);

         // Only 0x52 for Rx is relevant
         (*oppc_RoutingDispatcher)->SetFilterParameters(orc_LastNodeOfRouting.u8_OutInterfaceNumber, 0x00000052,
                                                        0x000007FF);

         this->mc_LegacyRouterDispatchers[ou32_ActiveNode] = (*oppc_RoutingDispatcher);
         // Set the new dispatcher
         pc_StwFlashloader->CfgSetCommDispatcher(*oppc_RoutingDispatcher);

         s32_Return = C_NO_ERR;
      }
      else
      {
         s32_Return = C_CONFIG;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stops the specific routing configuration for one specific node

   \param[in]     ou32_ActiveNode                      Active node index of vector mc_ActiveNodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCComDriverFlash::m_StopRoutingSpecific(const uint32 ou32_ActiveNode)
{
   const C_OSCNode * const pc_Node = &this->mpc_SysDef->c_Nodes[this->mc_ActiveNodesIndexes[ou32_ActiveNode]];

   if (pc_Node->c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_STW)
   {
      C_OSCFlashProtocolStwFlashloader * const pc_StwFlashloader =
         dynamic_cast<C_OSCFlashProtocolStwFlashloader * const>(this->mc_StwFlashProtocols[ou32_ActiveNode]);

      if (pc_StwFlashloader != NULL)
      {
         // Reset to 'normal' dispatcher
         pc_StwFlashloader->CfgSetCommDispatcher(this->m_GetCanDispatcher());
      }
   }

   C_OSCComDriverProtocol::m_StopRoutingSpecific(ou32_ActiveNode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the interface has relevant functions activated

   In this case routing and update functionality

   \param[in]     orc_ComItfSettings         Interface configuration

   \return
   true     Interface has relevant functions activated and is connected
   false    Interface has no relevant functions activated or is not connected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCComDriverFlash::m_CheckInterfaceForFunctions(const C_OSCNodeComInterfaceSettings & orc_ComItfSettings) const
{
   bool q_Return = false;

   if ((orc_ComItfSettings.q_IsBusConnected == true) &&
       ((orc_ComItfSettings.q_IsRoutingEnabled == true) ||
        (orc_ComItfSettings.q_IsUpdateEnabled == true)))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the pointer to the STW flashloader protocol of specific server id

   \param[in]     orc_ServerId             Server id for communication

   \return
   Valid pointer  Protocol for server found
   NULL           No protocol for server found
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCFlashProtocolStwFlashloader * C_OSCComDriverFlash::m_GetStwFlashloaderProtocol(
   const C_OSCProtocolDriverOsyNode & orc_ServerId) const
{
   C_OSCFlashProtocolStwFlashloader * pc_Return = NULL;

   for (uint32 u32_Counter = 0U; u32_Counter < this->mc_ServerIDs.size(); ++u32_Counter)
   {
      if (orc_ServerId == this->mc_ServerIDs[u32_Counter])
      {
         // Index found
         if ((u32_Counter < this->mc_StwFlashProtocols.size()) &&
             (this->mc_StwFlashProtocols[u32_Counter] != NULL))
         {
            pc_Return = this->mc_StwFlashProtocols[u32_Counter];
            break;
         }
      }
   }

   return pc_Return;
}

//----------------------------------------------------------------------------------------------------------------------
void C_OSCComDriverFlash::m_InitFlashProtocolStw(C_OSCFlashProtocolStwFlashloader * const opc_FlashProtocolStw,
                                                 const uint8 ou8_LocalId)
{
   opc_FlashProtocolStw->CfgSetCommDispatcher(this->m_GetCanDispatcher());
   // Send and received id have a fixed value here. It is not configurable.
   opc_FlashProtocolStw->CfgSetFlashId(mhu32_XFL_ID_TX, mhu32_XFL_ID_RX); //SendId, ReceiveId
   opc_FlashProtocolStw->CfgSetXtdId(mhq_XFL_ID_XTD);
   // local id equals node id of openSYDE protocol
   opc_FlashProtocolStw->CfgSetLocalId(ou8_LocalId);
   // The bitrate configuration is not necessary. It will be used for calculation of defined busload which is
   // insignificant
   opc_FlashProtocolStw->CfgSetBitrate(0U);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize flash protocols

   The functions fills the vector mc_OsyProtocols of the base class too.

   \return
   C_NO_ERR   Operation success
   C_CONFIG   Invalid initialization
   C_OVERFLOW Unknown diagnostic server for at least one node or invalid node identifier was set in diagnostic protocol
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::m_InitFlashProtocol(void)
{
   sint32 s32_Return = C_NO_ERR;
   const uint32 u32_ActiveNodeCount = this->m_GetActiveNodeCount();

   if ((this->mc_TransportProtocols.size() >= u32_ActiveNodeCount) &&
       (this->mc_ServerIDs.size() == u32_ActiveNodeCount))
   {
      uint32 u32_ActiveNodeCounter;

      //Init protocol driver
      // The last protocol is for broadcasts
      this->mc_OsyProtocols.resize(u32_ActiveNodeCount, NULL);
      this->mc_StwFlashProtocols.resize(u32_ActiveNodeCount, NULL);

      for (u32_ActiveNodeCounter = 0U; u32_ActiveNodeCounter < this->mc_ActiveNodesIndexes.size();
           ++u32_ActiveNodeCounter)
      {
         if (this->mc_ActiveNodesIndexes[u32_ActiveNodeCounter] < this->mpc_SysDef->c_Nodes.size())
         {
            const C_OSCNode * const pc_Node =
               &this->mpc_SysDef->c_Nodes[this->mc_ActiveNodesIndexes[u32_ActiveNodeCounter]];
            C_OSCProtocolDriverOsy * pc_ProtocolOsy;
            C_OSCFlashProtocolStwFlashloader * pc_ProtocolStw;

            switch (pc_Node->c_Properties.e_FlashLoader)
            {
            case C_OSCNodeProperties::eFL_OPEN_SYDE:
               pc_ProtocolOsy = new C_OSCProtocolDriverOsy();
               pc_ProtocolOsy->InitializeHandleWaitTime(&C_OSCComDriverFlash::mh_HandleWaitTime, this);
               s32_Return = pc_ProtocolOsy->SetTransportProtocol(this->mc_TransportProtocols[u32_ActiveNodeCounter]);
               if (s32_Return == C_NO_ERR)
               {
                  s32_Return = pc_ProtocolOsy->SetNodeIdentifiers(this->GetClientId(),
                                                                  this->mc_ServerIDs[u32_ActiveNodeCounter]);
                  if (s32_Return != C_NO_ERR)
                  {
                     //Invalid configuration = programming error
                     s32_Return = C_OVERFLOW;
                  }
               }
               else
               {
                  //Invalid configuration = programming error
                  s32_Return = C_OVERFLOW;
               }
               this->mc_OsyProtocols[u32_ActiveNodeCounter] = pc_ProtocolOsy;
               break;
            case C_OSCNodeProperties::eFL_STW:
               pc_ProtocolStw = new C_OSCFlashProtocolStwFlashloader(pr_XflReportProgress,
                                                                     pv_XflReportProgressInstance);
               this->m_InitFlashProtocolStw(pc_ProtocolStw,
                                            this->mc_ServerIDs[u32_ActiveNodeCounter].u8_NodeIdentifier);
               this->mc_StwFlashProtocols[u32_ActiveNodeCounter] = pc_ProtocolStw;
               break;
            case C_OSCNodeProperties::eFL_NONE:
            default:
               s32_Return = C_OVERFLOW;
               break;
            }
         }
         else
         {
            s32_Return = C_CONFIG;
         }

         if (s32_Return != C_NO_ERR)
         {
            break;
         }
      }
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::m_PrepareTemporaryOsyProtocol(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                          C_OSCProtocolDriverOsy & orc_OsyProtocol,
                                                          C_OSCProtocolDriverOsyTpCan & orc_CanTransportProtocol)
{
   sint32 s32_Return;

   s32_Return = orc_CanTransportProtocol.SetNodeIdentifiers(this->GetClientId(), orc_ServerId);
   if (s32_Return == C_NO_ERR)
   {
      s32_Return = orc_CanTransportProtocol.SetDispatcher(this->m_GetCanDispatcher());

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = orc_OsyProtocol.SetTransportProtocol(&orc_CanTransportProtocol);
         if (s32_Return == C_NO_ERR)
         {
            s32_Return = orc_OsyProtocol.SetNodeIdentifiers(this->GetClientId(), orc_ServerId);
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::m_GetStwResetMessage(const uint32 ou32_NodeIndex, T_STWCAN_Msg_TX & orc_Message) const
{
   const C_OSCNode * const pc_Node = &this->mpc_SysDef->c_Nodes[ou32_NodeIndex];
   sint32 s32_Return = C_NOACT;

   if ((pc_Node->c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_STW) &&
       (pc_Node->c_Properties.c_STWFlashloaderSettings.q_ResetMessageActive == true))
   {
      // Get all values for the reset messages
      const C_OSCNodeStwFlashloaderSettings & rc_StwFlSettings =
         pc_Node->c_Properties.c_STWFlashloaderSettings;

      orc_Message.u32_ID = rc_StwFlSettings.u32_ResetMessageId;
      if (rc_StwFlSettings.q_ResetMessageExtendedId == false)
      {
         orc_Message.u8_XTD = 0U;
      }
      else
      {
         orc_Message.u8_XTD = 1U;
      }

      orc_Message.u8_RTR = 0U;

      if (rc_StwFlSettings.u8_ResetMessageDlc <= 8)
      {
         orc_Message.u8_DLC = rc_StwFlSettings.u8_ResetMessageDlc;

         if (rc_StwFlSettings.c_Data.size() >= orc_Message.u8_DLC)
         {
            (void)std::memcpy(&orc_Message.au8_Data[0], &rc_StwFlSettings.c_Data[0], orc_Message.u8_DLC);

            s32_Return = C_NO_ERR;
         }
         else
         {
            s32_Return = C_RANGE;
         }
      }
      else
      {
         s32_Return = C_RANGE;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the minimum Flashloader reset wait time in ms for a specific node

   Default minimum value is 500 ms independent of the type.

   \param[in]   oe_Type             Type of minimum flashloader reset wait time
   \param[in]   ou32_NodeIndex      Node index to get the configured wait time
   \param[out]  oru32_TimeValue     Time in ms the node need at least to get from application to the Flashloader or
                                    from Flashloader to Flashloader

   \retval   C_NO_ERR   Time returned
   \retval   C_RANGE    Node with orc_ServerId does not exist or is not active
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCComDriverFlash::m_GetMinimumFlashloaderResetWaitTime(
   const C_OSCComDriverFlash::E_MinimumFlashloaderResetWaitTimeType oe_Type, const uint32 ou32_NodeIndex,
   uint32 & oru32_TimeValue) const
{
   sint32 s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mpc_SysDef->c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mpc_SysDef->c_Nodes[ou32_NodeIndex];

      tgl_assert(rc_Node.pc_DeviceDefinition != NULL);
      if (rc_Node.pc_DeviceDefinition != NULL)
      {
         s32_Return = C_NO_ERR;

         switch (oe_Type)
         {
         case C_OSCComDriverFlash::eNO_CHANGES_CAN:
            oru32_TimeValue = rc_Node.pc_DeviceDefinition->u32_FlashloaderResetWaitTimeNoChangesCan;
            break;
         case C_OSCComDriverFlash::eNO_CHANGES_ETHERNET:
            oru32_TimeValue = rc_Node.pc_DeviceDefinition->u32_FlashloaderResetWaitTimeNoChangesEthernet;
            break;
         case C_OSCComDriverFlash::eNO_FUNDAMENTAL_COM_CHANGES_CAN:
            oru32_TimeValue = rc_Node.pc_DeviceDefinition->u32_FlashloaderResetWaitTimeNoFundamentalChangesCan;
            break;
         case C_OSCComDriverFlash::eNO_FUNDAMENTAL_COM_CHANGES_ETHERNET:
            oru32_TimeValue = rc_Node.pc_DeviceDefinition->u32_FlashloaderResetWaitTimeNoFundamentalChangesEthernet;
            break;
         case C_OSCComDriverFlash::eFUNDAMENTAL_COM_CHANGES_CAN:
            oru32_TimeValue = rc_Node.pc_DeviceDefinition->u32_FlashloaderResetWaitTimeFundamentalChangesCan;
            break;
         case C_OSCComDriverFlash::eFUNDAMENTAL_COM_CHANGES_ETHERNET:
            oru32_TimeValue = rc_Node.pc_DeviceDefinition->u32_FlashloaderResetWaitTimeFundamentalChangesEthernet;
            break;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  In C_OSCProtocolDriverOsy registered function for handling long waiting times

   See m_HandleWaitTime for detailed description

   \param[in]     opv_Instance     Pointer to the instance of C_OSCComDriverFlash
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCComDriverFlash::mh_HandleWaitTime(void * const opv_Instance)
{
   //lint -e{9079}  This class is the only one which registers itself at the caller of this function. It must match.
   C_OSCComDriverFlash * const pc_ComDriver = reinterpret_cast<C_OSCComDriverFlash * const>(opv_Instance);

   tgl_assert(pc_ComDriver != NULL);
   if (pc_ComDriver != NULL)
   {
      pc_ComDriver->m_HandleWaitTime();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  In C_OSCProtocolDriverOsy registered function for handling long waiting times

   For example erasing of flash can have a long timeout time. It can be longer than five seconds.
   An active CAN routing would be stopped of the server by the session timeout. This function sends
   the tester present to the last CAN router node on the route to keep the route alive

   \param[in]     opv_Instance     Pointer to the instance of C_OSCComDriverFlash
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCComDriverFlash::m_HandleWaitTime(void)
{
   if (this->mq_Initialized == true)
   {
      uint32 u32_Counter;

      for (u32_Counter = 0U; u32_Counter < this->mc_ActiveNodesLastCanRouters.size(); ++u32_Counter)
      {
         const uint32 u32_ActiveNodeRouter = this->mc_ActiveNodesLastCanRouters[u32_Counter];

         tgl_assert(u32_ActiveNodeRouter < this->mc_OsyProtocols.size());
         if (u32_ActiveNodeRouter < this->mc_OsyProtocols.size())
         {
            C_OSCProtocolDriverOsy * const pc_ProtocolOsy = this->mc_OsyProtocols[u32_ActiveNodeRouter];
            if (pc_ProtocolOsy != NULL)
            {
               // Send tester present message without expecting a response
               const sint32 s32_Return = pc_ProtocolOsy->OsyTesterPresent(1U);

               if (s32_Return != C_NO_ERR)
               {
                  osc_write_log_error("Sending Tester Present", "Sending Tester Present failed with error code: " +
                                      C_OSCLoggingHandler::h_StwError(s32_Return));
               }
               else
               {
                  osc_write_log_info("Sending Tester Present", "Tester Present sent successfully.");
               }
            }
         }
      }
   }
}
