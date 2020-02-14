//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol driver

   For details cf. documentation in .h file.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <iostream>
#include <cstring>
#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLString.h"
#include "CSCLChecksums.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCProtocolDriverOsy.h"
#include "TGLTime.h"
#include "TGLUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set up class

   Initializes class elements
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCProtocolDriverOsy::C_OSCProtocolDriverOsy(void) :
   mpr_OnOsyTunnelCanMessageReceived(NULL),
   mpv_OnAsyncTunnelCanMessageInstance(NULL),
   mpr_OnOsyWaitTime(NULL),
   mpv_OnOsyWaitTimeInstance(NULL),
   mpc_TransportProtocol(NULL),
   //lint -e{1938}  //constant is initialized as it's in the same translation unit
   mu32_TimeoutPollingMs(hu32_DEFAULT_TIMEOUT),
   mu16_MaxServiceSize(C_OSCProtocolDriverOsyTpBase::hu16_OSY_MAXIMUM_SERVICE_SIZE)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Tear down class
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCProtocolDriverOsy::~C_OSCProtocolDriverOsy(void)
{
   mpc_TransportProtocol = NULL;
   mpv_OnAsyncTunnelCanMessageInstance = NULL;
   mpr_OnOsyWaitTime = NULL;
   mpv_OnOsyWaitTimeInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set timeout for "polling" functions

   Set timeout for "polling functions.

   \param[in]  ou32_TimeoutPollingMs   timeout in milliseconds
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsy::SetTimeoutPolling(const uint32 ou32_TimeoutPollingMs)
{
   mu32_TimeoutPollingMs = ou32_TimeoutPollingMs;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resets the timeout to the default value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsy::ResetTimeoutPolling(void)
{
   this->mu32_TimeoutPollingMs = hu32_DEFAULT_TIMEOUT;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set maximum size of service

   Set maximum size of service.
   Currently only used for "WriteMemoryByAddress".
   If this function is not called the default value of C_OSCProtocolDriverOsyTpBase::hu16_OSY_MAXIMUM_SERVICE_SIZE
    will be used.

   \param[in]  ou16_MaxServiceSize   Maximum service size to use (including header)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsy::SetMaxServiceSize(const uint16 ou16_MaxServiceSize)
{
   mu16_MaxServiceSize = ou16_MaxServiceSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialization of the async handling of tunneled CAN messages

   \param[in]  opr_OsyTunnelCanMessageReceived     function to be called if an async Tunnel CAN message is received
   \param[in]  opv_Instance                        instance pointer to pass back when invoking read event callback
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsy::InitializeTunnelCanMessage(
   const C_OSCProtocolDriverOsy::PR_OsyTunnelCanMessageReceived opr_OsyTunnelCanMessageReceived,
   void * const opv_Instance)
{
   this->mpr_OnOsyTunnelCanMessageReceived = opr_OsyTunnelCanMessageReceived;
   this->mpv_OnAsyncTunnelCanMessageInstance = opv_Instance;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the handling of long waiting times for services with high timeout times

   \param[in]  opr_OsyHandleWaitTime               function to be called if a service has a long waiting time for the
                                                   response
   \param[in]  opv_Instance                        instance pointer to pass back when invoking read event callback
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsy::InitializeHandleWaitTime(
   const C_OSCProtocolDriverOsy::PR_OsyHandleWaitTime opr_OsyHandleWaitTime, void * const opv_Instance)
{
   this->mpr_OnOsyWaitTime = opr_OsyHandleWaitTime;
   this->mpv_OnOsyWaitTimeInstance = opv_Instance;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the connection of the TCP socket

   \return
   C_NO_ERR   is connected
   C_NOACT    is not connected
   C_CONFIG   no transport protocol installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::IsConnected(void)
{
   sint32 s32_Return;

   if (this->mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      s32_Return = this->mpc_TransportProtocol->IsConnected();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReConnect to node

   Re-connect to openSYDE node.
   Useful after losing a connection (e.g. after ECU reset)

   \return
   C_NO_ERR   re-connected
   C_CONFIG   no transport protocol installed
   C_BUSY     re-connect failed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::ReConnect(void)
{
   sint32 s32_Return;

   if (this->mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      s32_Return = this->mpc_TransportProtocol->ReConnect();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect to node

   Disconnect to openSYDE node.

   \return
   C_NO_ERR   disconnected
   C_CONFIG   no transport protocol installed
   C_NOACT    disconnect failed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::Disconnect(void)
{
   sint32 s32_Return;

   if (this->mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      s32_Return = this->mpc_TransportProtocol->Disconnect();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add log entry for common protocol problems

   Add log entries for standard protocol error codes.

   \param[in]   orc_Service       Name of protocol service
   \param[in]   os32_ReturnCode   Protocol driver function return code (C_NO_ERR -> no log entry)
   \param[in]   ou8_NrCode        Negative response code (if os32_ReturnCode is C_WARN)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsy::m_LogServiceError(const C_SCLString & orc_Service, const sint32 os32_ReturnCode,
                                               const uint8 ou8_NrCode) const
{
   if (os32_ReturnCode != C_NO_ERR)
   {
      bool q_IsHardError; //we want to log error responses just as "warnings"

      const C_SCLString c_ErrorText =
         C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(os32_ReturnCode, ou8_NrCode, &q_IsHardError);
      m_LogErrorWithHeader("openSYDE protocol driver", "Service " + orc_Service + " failed. Error: " + c_ErrorText,
                           TGL_UTIL_FUNC_ID, q_IsHardError);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Perform cyclic communication tasks

   Invoke transport protocol's "Cycle" function.
   Invoke virtual functions to report all incoming event driven responses.

   Option:
   If a non event-driven response with service ID "ou8_ExpectedServiceId" is
    received the function will return, otherwise it will continue to read all incoming responses from the Rx queue.

   \param[in]   oq_CheckForSpecificServiceId    false: try to treat all responses as event-driven
                                                true: return if a response to service ou8_ExpectedServiceId is received
   \param[in]   ou8_ExpectedServiceId           if oq_CheckForSpecificServiceId is true: expected service ID
   \param[out]  opc_ReceivedService             if oq_CheckForSpecificServiceId is true and if != NULL:
                                                 received non event-driven response

   \return
   C_NO_ERR   if oq_CheckForSpecificServiceId is true: expected service response (positive or negative) received and
                                                       placed in orc_ReceivedService
              if oq_CheckForSpecificServiceId is false: finished with handling Rx queue
   C_WARN     if oq_CheckForSpecificServiceId is true: expected service not received
   C_CONFIG   no transport protocol or transport protocol returns error
   C_NOACT    nothing received
   C_COM      communication error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::m_Cycle(const bool oq_CheckForSpecificServiceId, const uint8 ou8_ExpectedServiceId,
                                       C_OSCProtocolDriverOsyService * const opc_ReceivedService)
{
   sint32 s32_Return = C_NO_ERR;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      s32_Return = mpc_TransportProtocol->Cycle();
      if (s32_Return == C_NO_ERR)
      {
         bool q_ExpectedServiceReceived = false;
         C_OSCProtocolDriverOsyService c_Service;
         //handle all full services in Rx queue:
         s32_Return = C_NO_ERR;
         while (s32_Return == C_NO_ERR)
         {
            s32_Return = mpc_TransportProtocol->ReadResponse(c_Service);

            if (s32_Return == C_NO_ERR)
            {
               //Check error response first
               // The SID must match and the length must be minimum 3
               // It might be a negative response with more information than UDS specified
               if ((c_Service.c_Data.size() >= 3U) && (c_Service.c_Data[0] == mhu8_OSY_NR_SI))
               {
                  //... to our request ?
                  if ((oq_CheckForSpecificServiceId == true) && (c_Service.c_Data[1] == ou8_ExpectedServiceId))
                  {
                     //yes. report to caller
                     q_ExpectedServiceReceived = true;
                     if (opc_ReceivedService != NULL)
                     {
                        (*opc_ReceivedService) = c_Service;
                     }
                  }
                  else
                  {
                     //trigger async virtual functions for all received async responses
                     this->m_HandleAsyncResponse(c_Service);
                  }
               }
               //check for specific response ?
               else
               {
                  if ((oq_CheckForSpecificServiceId == true) && (c_Service.c_Data.size() >= 1) &&
                      (c_Service.c_Data[0] == (ou8_ExpectedServiceId | 0x40U)))
                  {
                     q_ExpectedServiceReceived = true;
                     if (opc_ReceivedService != NULL)
                     {
                        (*opc_ReceivedService) = c_Service;
                     }
                  }
                  else
                  {
                     //trigger async virtual functions for all received async responses
                     this->m_HandleAsyncResponse(c_Service);
                  }
               }
               if (q_ExpectedServiceReceived == true)
               {
                  break; //finished here
               }
            }
         }
         if ((oq_CheckForSpecificServiceId == true) && (q_ExpectedServiceReceived == false))
         {
            s32_Return = C_WARN;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Poll for one specific response service

   Wait for a specific incoming response to a non event-driver service.
   Timeout can be configured with SetTimeoutPolling()

   Event-driven responses will be reported asynchronously (see ::Cycle).
   Non event-driven responses not matching the expected service will be dumped.

   The function will not actively wait and poll but use a platform specific mechanism the put the calling
    thread to sleep between individual polling calls.

   \param[in]  ou8_ExpectedServiceId   ID of service the response is expected for
   \param[in]  ou16_ExpectedSize       expected size of response service
   \param[out] orc_Service             response service
   \param[in]  oru8_NrCode             if negative response: negative response error code
   \param[in]  oq_ExactSizeExpected    true: response size must exactly match ou16_ExpectedSize
                                       false: response size must be >= ou16_ExpectedSize
   \param[in]  opc_ExpectedErrData     optional pointer to a vector with data for more specific
                                       identifying the possible matching negative response after the
                                       first three byte

   \return
   C_NO_ERR   non event-driven service matching the definition received and placed in orc_Service
   C_WARN     error response
   C_TIMEOUT  expected response not received within timeout
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::m_PollForSpecificServiceResponse(const uint8 ou8_ExpectedServiceId,
                                                                const uint16 ou16_ExpectedSize,
                                                                C_OSCProtocolDriverOsyService & orc_Service,
                                                                uint8 & oru8_NrCode, const bool oq_ExactSizeExpected,
                                                                const std::vector<uint8> * const opc_ExpectedErrData)
{
   sint32 s32_Return = C_NO_ERR;
   uint32 u32_StartTime = stw_tgl::TGL_GetTickCount();
   uint32 u32_LastWaitTimeHandled = u32_StartTime;
   uint16 u16_RxSize;
   bool q_Finished = false;

   //services cannot be > 16bit
   tgl_assert(orc_Service.c_Data.size() <= 0xFFFFU);

   //lock access to "polling"
   //If another thread checking for async responses kicks in (calling ::Cycle) is could otherwise
   // snatch away the response we want to get.
   mc_LockReception.Acquire();

   while (((stw_tgl::TGL_GetTickCount() - mu32_TimeoutPollingMs) < u32_StartTime) && (q_Finished == false))
   {
      //trigger handling of Rx and Tx communication
      s32_Return = this->m_Cycle(true, ou8_ExpectedServiceId, &orc_Service);
      if (s32_Return == C_NO_ERR)
      {
         u16_RxSize = static_cast<uint16>(orc_Service.c_Data.size());

         //Check error response first
         // It might be a negative response with more information than UDS specified
         if ((u16_RxSize >= 3U) && (orc_Service.c_Data[0] == mhu8_OSY_NR_SI))
         {
            //is this a negative response to our request ?
            if (orc_Service.c_Data[1] == ou8_ExpectedServiceId)
            {
               bool q_Match = true;

               // Check only if it is a negative response with additional bytes
               if ((opc_ExpectedErrData != NULL) && (u16_RxSize > 3U))
               {
                  // Extended not UDS conform error response expected
                  uint32 u32_Counter;
                  const uint32 u32_NumberOfBytes = opc_ExpectedErrData->size();

                  // Enough bytes for comparing?
                  if (orc_Service.c_Data.size() >= (u32_NumberOfBytes + 3U))
                  {
                     // Check all bytes for
                     for (u32_Counter = 0U; u32_Counter < u32_NumberOfBytes; ++u32_Counter)
                     {
                        if ((*opc_ExpectedErrData)[u32_Counter] !=
                            orc_Service.c_Data[static_cast<uintn>(u32_Counter) + 3U])
                        {
                           q_Match = false;
                           //when starting cyclic calls we might interpret the first data transmission of the last call
                           // as the response of the next cyclic service registration,
                           // so we should not discard this message but instead handle the error accordingly
                           if (orc_Service.c_Data[1] == mhu8_OSY_SI_READ_DATA_POOL_DATA_EVENT_DRIVEN)
                           {
                              //handle data error
                              s32_Return = m_HandleAsyncResponse(orc_Service);
                           }
                           else
                           {
                              m_LogErrorWithHeader("Synchronous communication", "Sync negative response to expected service"
                                                   " but unexpected data bytes received. Ignoring.",
                                                   TGL_UTIL_FUNC_ID);
                           }
                           break;
                        }
                     }
                  }
                  else
                  {
                     // Received message is too short
                     q_Match = false;
                     m_LogErrorWithHeader("Synchronous communication", "Sync negative response to expected service"
                                          " but unexpected size received. Ignoring.", TGL_UTIL_FUNC_ID);
                  }
               }

               if (q_Match == true)
               {
                  // Matching error response found!
                  // special handling for "responsePending": rewind Rx timeout expectation
                  if (orc_Service.c_Data[2] == hu8_NR_CODE_RESPONSE_PENDING)
                  {
                     u32_StartTime = stw_tgl::TGL_GetTickCount();
                     // The response of the server resets the session timeouts
                     u32_LastWaitTimeHandled = u32_StartTime;
                  }
                  else
                  {
                     //other code: report to application
                     oru8_NrCode = orc_Service.c_Data[2];
                     s32_Return = C_WARN;
                     q_Finished = true;
                  }
               }
            }
            else
            {
               m_LogErrorWithHeader(TGL_UTIL_FUNC_ID, "Synchronous communication",
                                    "Sync negative response to unexpected service received. Ignoring.");
            }
         }

         if (q_Finished == false)
         {
            //check for response: is it as expected ?
            if (((oq_ExactSizeExpected == true) && (u16_RxSize == ou16_ExpectedSize)) ||
                ((oq_ExactSizeExpected == false) && (u16_RxSize >= ou16_ExpectedSize)))
            {
               //looks like a response to our request
               if (orc_Service.c_Data[0] == (ou8_ExpectedServiceId | 0x40U))
               {
                  //OK response !
                  q_Finished = true;
               }
            }
            else
            {
               //when initiating event driven transfers we might get a response to the expected service but it's
               // an actual data response to an already ongoing transfer
               if (orc_Service.c_Data[0] == (mhu8_OSY_SI_READ_DATA_POOL_DATA_EVENT_DRIVEN | 0x40U))
               {
                  //last chance ...
                  s32_Return = m_HandleAsyncResponse(orc_Service);
               }
               if (s32_Return != C_NO_ERR)
               {
                  //unexpected response
                  m_LogErrorWithHeader("Synchronous communication",
                                       "Sync response with unexpected length received. Ignoring.", TGL_UTIL_FUNC_ID);
               }
            }
         }
      }
      else if (s32_Return == C_COM)
      {
         // Communication error
         break;
      }
      else
      {
         // Handle long waiting time by registered function
         if (this->mpr_OnOsyWaitTime != NULL)
         {
            const uint32 u32_CurrentTime = stw_tgl::TGL_GetTickCount();
            if ((u32_CurrentTime - hu32_DEFAULT_HANDLE_WAIT_TIME) > u32_LastWaitTimeHandled)
            {
               this->mpr_OnOsyWaitTime(this->mpv_OnOsyWaitTimeInstance);
               u32_LastWaitTimeHandled = u32_CurrentTime;
            }
         }
      }

      if (q_Finished == false)
      {
         stw_tgl::TGL_Sleep(0U); //rescind CPU time to other threads ...
      }
   }
   mc_LockReception.Release();

   if (s32_Return == C_COM)
   {
      m_LogErrorWithHeader("Synchronous communication", "No response received (communication error).",
                           TGL_UTIL_FUNC_ID);
   }
   else if (q_Finished == false)
   {
      m_LogErrorWithHeader("Synchronous communication", "No response received (timed out).", TGL_UTIL_FUNC_ID);
      s32_Return = C_TIMEOUT;
   }
   else
   {
      // Nothing to do
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   DiagnosticSessionControl service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[in]  ou8_SessionId         session ID to switch to
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong session ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyDiagnosticSessionControl(const uint8 ou8_SessionId, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Service;
   uint8 u8_NrErrorCode = 0U;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      c_Service.c_Data.resize(2);
      c_Service.c_Data[0] = mhu8_OSY_SI_DIAGNOSTIC_SESSION_CONTROL;
      c_Service.c_Data[1] = ou8_SessionId;

      s32_Return = mpc_TransportProtocol->SendRequest(c_Service);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
      else
      {
         s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_DIAGNOSTIC_SESSION_CONTROL, 6U, c_Service,
                                                       u8_NrErrorCode);
         if ((s32_Return == C_NO_ERR) && (c_Service.c_Data[1] != ou8_SessionId))
         {
            s32_Return = C_RD_WR;
         }
         else if ((s32_Return == C_WARN) && (opu8_NrCode != NULL))
         {
            (*opu8_NrCode) = u8_NrErrorCode;
         }
         else
         {
            //nothing more to do ...
         }
      }
   }

   m_LogServiceError("DiagnosticSessionControl", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadDataByIdentifier utility wrapper

   Send ReadDataByIdentifier request and wait for response.
   See class description for general handling of "polled" services.

   \param[in]  ou16_Identifier            identifier to read
   \param[in]  ou16_ExpectedPayloadSize   number of payload bytes to expect in response
   \param[in]  oq_ExactSizeExpected       true: expect exactly ou16_ExpectedPayloadSize bytes
                                          false: expect at least ou16_ExpectedPayloadSize bytes
   \param[out] orc_ReadData               payload data read
   \param[out] oru8_NrCode                negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::m_ReadDataByIdentifier(const uint16 ou16_Identifier,
                                                      const uint16 ou16_ExpectedPayloadSize,
                                                      const bool oq_ExactSizeExpected,
                                                      std::vector<uint8> & orc_ReadData, uint8 & oru8_NrCode)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Request;
   C_OSCProtocolDriverOsyService c_Response;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      c_Request.c_Data.resize(3);
      c_Request.c_Data[0] = mhu8_OSY_SI_READ_DATA_BY_IDENTIFIER;
      c_Request.c_Data[1] = static_cast<uint8>(ou16_Identifier >> 8U);
      c_Request.c_Data[2] = static_cast<uint8>(ou16_Identifier & 0xFFU);

      s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
      else
      {
         uint8 u8_NrErrorCode;
         s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_READ_DATA_BY_IDENTIFIER,
                                                       ou16_ExpectedPayloadSize + 3U, c_Response,
                                                       u8_NrErrorCode, oq_ExactSizeExpected);
         switch (s32_Return)
         {
         case C_NO_ERR:
            //check DI:
            if ((c_Request.c_Data[1] != c_Response.c_Data[1]) ||
                (c_Request.c_Data[2] != c_Response.c_Data[2]))
            {
               s32_Return = C_RD_WR;
            }
            else
            {
               orc_ReadData.resize(c_Response.c_Data.size() - 3);
               (void)std::memcpy(&orc_ReadData[0], &c_Response.c_Data[3], c_Response.c_Data.size() - 3);
            }
            break;
         case C_WARN:
            oru8_NrCode = u8_NrErrorCode;
            break;
         default:
            break;
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   WriteDataByIdentifier utility wrapper

   Send WriteDataByIdentifier request and wait for response.
   See class description for general handling of "polled" services.

   \param[in]  ou16_Identifier            identifier to read
   \param[in]  ou16_ExpectedPayloadSize   number of payload bytes to expect in response
   \param[in]  oq_ExactSizeExpected       true: expect exactly ou16_ExpectedPayloadSize bytes
                                          false: expect at least ou16_ExpectedPayloadSize bytes
   \param[out] orc_ReadData               payload data read
   \param[out] oru8_NrCode                negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::m_WriteDataByIdentifier(const uint16 ou16_Identifier, std::vector<uint8> & orc_WriteData,
                                                       uint8 & oru8_NrCode)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Request;
   C_OSCProtocolDriverOsyService c_Response;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      c_Request.c_Data.resize(3 + orc_WriteData.size());
      c_Request.c_Data[0] = mhu8_OSY_SI_WRITE_DATA_BY_IDENTIFIER;
      c_Request.c_Data[1] = static_cast<uint8>(ou16_Identifier >> 8U);
      c_Request.c_Data[2] = static_cast<uint8>(ou16_Identifier & 0xFFU);
      (void)std::memcpy(&c_Request.c_Data[3], &orc_WriteData[0], orc_WriteData.size());

      s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
      else
      {
         uint8 u8_NrErrorCode;
         s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_WRITE_DATA_BY_IDENTIFIER,
                                                       3U, c_Response, u8_NrErrorCode, true);
         switch (s32_Return)
         {
         case C_NO_ERR:
            //check response:
            if ((c_Request.c_Data[1] != c_Response.c_Data[1]) ||
                (c_Request.c_Data[2] != c_Response.c_Data[2]))
            {
               s32_Return = C_RD_WR;
            }
            break;
         case C_WARN:
            oru8_NrCode = u8_NrErrorCode;
            break;
         default:
            break;
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadEcuSerialNumber service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[out] orau8_SerialNumber    read serial number
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadEcuSerialNumber(uint8 (&orau8_SerialNumber)[6], uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Snr;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_ECU_SERIAL_NUMBER, 6U, true, c_Snr, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      //lint -e{420} //std::vector reference returned by [] is guaranteed to have linear data in memory
      (void)std::memcpy(&orau8_SerialNumber[0], &c_Snr[0], 6);
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }

   m_LogServiceError("ReadDataByIdentifier::EcuSerialNumber", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadHardwareNumber service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[out] oru32_HardwareNumber   read article number
   \param[out] opu8_NrCode            if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadHardwareNumber(uint32 & oru32_HardwareNumber, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_SYS_SUPPLIER_ECU_HW_NUMBER, 4U, true, c_Data, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      //lint -e{864} //false positive due to const/non-const misinterpretation
      oru32_HardwareNumber = ((static_cast<uint32>(c_Data[0])) << 24U) +
                             ((static_cast<uint32>(c_Data[1])) << 16U) +
                             ((static_cast<uint32>(c_Data[2])) << 8U) +
                             (static_cast<uint32>(c_Data[3]));
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }

   m_LogServiceError("ReadDataByIdentifier::HardwareNumber", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadHardwareVersionNumber service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[out] orc_HardwareVersionNumber    read hardware version number
   \param[out] opu8_NrCode                  if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadHardwareVersionNumber(C_SCLString & orc_HardwareVersionNumber,
                                                            uint8 * const opu8_NrCode)

{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_SYS_SUPPLIER_ECU_HW_VERSION, 0U, false, c_Data, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      //extract text:
      std::vector<charn> c_Text;
      c_Text.resize(c_Data.size() + 1); //plus 1 for termination
      c_Text[c_Text.size() - 1] = '\0'; //add termination
      (void)std::memcpy(&c_Text[0], &c_Data[0], c_Data.size());
      orc_HardwareVersionNumber = &c_Text[0];
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }

   m_LogServiceError("ReadDataByIdentifier::HardwareVersionNumber", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadListOfFeatures service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[out] orc_ListOfFeatures           read list of features
   \param[out] opu8_NrCode                  if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadListOfFeatures(C_ListOfFeatures & orc_ListOfFeatures, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_LIST_OF_FEATURES, 8U, true, c_Data, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      //extract bits:
      orc_ListOfFeatures.q_FlashloaderCanWriteToNvm        = ((c_Data[7] & 0x01U) == 0x01U) ? true : false;
      orc_ListOfFeatures.q_MaxNumberOfBlockLengthAvailable = ((c_Data[7] & 0x02U) == 0x02U) ? true : false;
      orc_ListOfFeatures.q_EthernetToEthernetRoutingSupported = ((c_Data[7] & 0x04U) == 0x04U) ? true : false;
      //we don't know anything about the meaning of the rest of the bits as we have no crystal ball
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }

   m_LogServiceError("ReadDataByIdentifier::ListOfFeatures", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   MaxNumberOfBlockLength service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Whether this service is available on the target can be checked with OsyReadListOfFeatures().

   \param[out] oru16_MaxNumberOfBlockLength     read MaxNumberOfBlockLength
   \param[out] opu8_NrCode                      if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadMaxNumberOfBlockLength(uint16 & oru16_MaxNumberOfBlockLength,
                                                             uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_MAX_NUMBER_OF_BLOCK_LENGTH, 2U, true, c_Data, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      //extract information:
      //lint -e{864} //false positive due to const/non-const misinterpretation
      oru16_MaxNumberOfBlockLength = (static_cast<uint16>((static_cast<uint16>(c_Data[0])) << 8U)) + (c_Data[1]);
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }

   m_LogServiceError("ReadDataByIdentifier::ReadMaxNumberOfBlockLength", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadDeviceName service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.
   Per specification the maximum length is 28 characters. This is not checked by this function.

   \param[out] orc_DeviceName               read device name
   \param[out] opu8_NrCode                  if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadDeviceName(C_SCLString & orc_DeviceName, stw_types::uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_DEVICE_NAME, 0U, false, c_Data, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      //extract text:
      std::vector<charn> c_Text;
      c_Text.resize(c_Data.size() + 1); //plus 1 for termination
      c_Text[c_Text.size() - 1] = '\0'; //add termination
      (void)std::memcpy(&c_Text[0], &c_Data[0], c_Data.size());
      orc_DeviceName = &c_Text[0];
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }

   m_LogServiceError("ReadDataByIdentifier::DeviceName", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadApplicationName service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.
   Per specification the maximum length is 32 characters. This is not checked by this function.

   \param[out] orc_ApplicationName          read application name
   \param[out] opu8_NrCode                  if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadApplicationName(C_SCLString & orc_ApplicationName, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_APPLICATION_NAME, 0U, false, c_Data, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      //extract text:
      std::vector<charn> c_Text;
      c_Text.resize(c_Data.size() + 1); //plus 1 for termination
      c_Text[c_Text.size() - 1] = '\0'; //add termination
      (void)std::memcpy(&c_Text[0], &c_Data[0], c_Data.size());
      orc_ApplicationName = &c_Text[0];
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }

   m_LogServiceError("ReadDataByIdentifier::ApplicationName", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadApplicationName service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.
   Per specification the maximum length is 16 characters. This is not checked by this function.

   \param[out] orc_ApplicationVersion       read application version
   \param[out] opu8_NrCode                  if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadApplicationVersion(C_SCLString & orc_ApplicationVersion,
                                                         uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_APPLICATION_VERSION, 0U, false, c_Data, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      //extract text:
      std::vector<charn> c_Text;
      c_Text.resize(c_Data.size() + 1); //plus 1 for termination
      c_Text[c_Text.size() - 1] = '\0'; //add termination
      (void)std::memcpy(&c_Text[0], &c_Data[0], c_Data.size());
      orc_ApplicationVersion = &c_Text[0];
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }

   m_LogServiceError("ReadDataByIdentifier::ApplicationVersion", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadBootSoftwareIdentification service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Technically the protocol would support multiple version numbers in the response.
   This is for UDS compatibility.
   In openSYDE we only send one version.
   This function also only supports one version.

   Exemplary format of returned version:
   0x01; 0x17; 0x04  -> V1.23r4

   \param[out] orau8_Version        read boot software version
   \param[out] opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID, or: number of modules is not 1)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadBootSoftwareIdentification(uint8 (&orau8_Version)[3], uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_BOOT_SOFTWARE_IDENTIFICATION, 4U, true, c_Data, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      if (c_Data[0] != 1U)
      {
         s32_Return = C_RD_WR;
      }
      else
      {
         //lint -e{420} //sub-function reported we have enough bytes; no problem
         (void)std::memcpy(&orau8_Version[0], &c_Data[1], 3U);
      }
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }

   m_LogServiceError("ReadDataByIdentifier::BootSoftwareIdentification", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadActiveDiagnosticSession service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[out] oru8_SessionId       active session ID
   \param[out] opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_COM      communication driver reported error
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadActiveDiagnosticSession(uint8 & oru8_SessionId, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_ACTIVE_DIAGNOSTIC_SESSION, 1U, true, c_Data, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      oru8_SessionId = c_Data[0];
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   m_LogServiceError("ReadDataByIdentifier::ActiveDiagnosticSession", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadApplicationSoftwareFingerprint service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.
   Per specification the length is minimum 7 bytes, maximum 21 bytes. This is not checked by this function.

   \param[out] orau8_Date           date of programming yy.mm.dd
   \param[out] orau8_Time           time of programming hh.mm.ss
   \param[out] orc_Username         name of operator
   \param[out] opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadApplicationSoftwareFingerprint(uint8 (&orau8_Date)[3], uint8 (&orau8_Time)[3],
                                                                     C_SCLString & orc_Username,
                                                                     uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return =
      m_ReadDataByIdentifier(mhu16_OSY_DI_APPLICATION_SOFTWARE_FINGERPRINT, 7U, false, c_Data, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      std::vector<charn> c_Text;
      //lint -e{420} //sub-function reported we have enough bytes; no problem
      (void)std::memcpy(&orau8_Date[0], &c_Data[0], 3U);
      //lint -e{420} //sub-function reported we have enough bytes; no problem
      (void)std::memcpy(&orau8_Time[0], &c_Data[3], 3U);
      //extract text:
      c_Text.resize(c_Data.size() - 6); //minus 7 for date, time and length of username, plus 1 for termination
      c_Text[c_Text.size() - 1] = '\0'; //add termination
      (void)std::memcpy(&c_Text[0], &c_Data[7], c_Data.size() - 7U);
      orc_Username = &c_Text[0];
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   m_LogServiceError("ReadDataByIdentifier::ApplicationSoftwareFingerprint", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   WriteApplicationSoftwareFingerprint service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.
   If a user name with more than 20 characters is passed only the first 20 will be used.

   \param[in] orau8_Date            date of programming (yy.mm.dd)
   \param[in] orau8_Time            time of programming (hh:mm:ss)
   \param[in] orc_UserName          name of operator
   \param[out] opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyWriteApplicationSoftwareFingerprint(const uint8 (&orau8_Date)[3],
                                                                      const uint8 (&orau8_Time)[3],
                                                                      const C_SCLString & orc_UserName,
                                                                      uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   C_SCLString c_UserName = orc_UserName;
   uint8 u8_NrErrorCode = 0U;

   if (c_UserName.Length() > 20)
   {
      c_UserName.SetLength(20);
   }

   c_Data.resize(7U + static_cast<uintn>(c_UserName.Length()));
   //lint -e{419} //std::vector reference returned by [] is linear; we allocated enough memory
   (void)std::memcpy(&c_Data[0], &orau8_Date[0], 3U);
   //lint -e{419} //std::vector reference returned by [] is linear; we allocated enough memory
   (void)std::memcpy(&c_Data[3], &orau8_Time[0], 3U);
   c_Data[6] = static_cast<uint8>(orc_UserName.Length());
   (void)std::memcpy(&c_Data[7], orc_UserName.c_str(), orc_UserName.Length());

   s32_Return = m_WriteDataByIdentifier(mhu16_OSY_DI_APPLICATION_SOFTWARE_FINGERPRINT, c_Data, u8_NrErrorCode);
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   m_LogServiceError("WriteDataByIdentifier::ApplicationSoftwareFingerprint", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadMaxNumOfEventDrivenTransmissions service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[out] oru16_MaxNum         maximum number of transmissions supported
   \param[out] opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadMaxNumOfEventDrivenTransmissions(uint16 & oru16_MaxNum, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_MAX_NUM_ASYNC, 2U, true, c_Data, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      //lint -e{864} //false positive due to const/non-const misinterpretation
      oru16_MaxNum = (static_cast<uint16>((static_cast<uint16>(c_Data[0])) << 8U)) + (c_Data[1]);
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   m_LogServiceError("ReadDataByIdentifier::MaxNumOfEventDrivenTransmissions", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadProtocolVersion service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Returned format: 3bytes major.minor.release:
   Example: (0x01, 0x17, 0x04) -> V1.23r4

   \param[out] orau8_Version         read driver implementation version
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadProtocolVersion(uint8 (&orau8_Version)[3], uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_PROTOCOL_VERSION, 3U, true, c_Data, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      //lint -e{420} //sub-function reported we have enough bytes; no problem
      (void)std::memcpy(&orau8_Version[0], &c_Data[0], 3U);
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   m_LogServiceError("ReadDataByIdentifier::ProtocolVersion", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadFlashloaderProtocolVersion service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Returned format: 3bytes major.minor.release:
   Example: (0x01, 0x17, 0x04) -> V1.23r4

   \param[out] orau8_Version         read driver implementation version
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadFlashloaderProtocolVersion(uint8 (&orau8_Version)[3], uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_FLASHLOADER_PROTOCOL_VERSION, 3U, true, c_Data, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      //lint -e{420} //sub-function reported we have enough bytes; no problem
      (void)std::memcpy(&orau8_Version[0], &c_Data[0], 3U);
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   m_LogServiceError("ReadDataByIdentifier::FlashloaderProtocolVersion", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadFlashCount service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[out] oru32_FlashCount       read number of flash programming cycles performed on server
   \param[out] opu8_NrCode            if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadFlashCount(uint32 & oru32_FlashCount, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_FLASH_COUNT, 4U, true, c_Data, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      //lint -e{864} //false positive due to const/non-const misinterpretation
      oru32_FlashCount = ((static_cast<uint32>(c_Data[0])) << 24U) +
                         ((static_cast<uint32>(c_Data[1])) << 16U) +
                         ((static_cast<uint32>(c_Data[2])) << 8U) +
                         (static_cast<uint32>(c_Data[3]));
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   m_LogServiceError("ReadDataByIdentifier::FlashCount", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadProtocolDriverImplementationVersion service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Returned format: 3bytes major.minor.release:
   Example: (0x01, 0x17, 0x04) -> V1.23r4

   \param[out] orau8_Version         read driver implementation version
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadProtocolDriverImplementationVersion(uint8 (&orau8_Version)[3],
                                                                          uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_ReadDataByIdentifier(mhu16_OSY_DI_PROTOCOL_DRIVER_IMPLEMENTATION_VERSION, 3U, true, c_Data,
                                       u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      //lint -e{420} //called function only returns C_NO_ERR if it was able to read the expected number of bytes
      (void)std::memcpy(&orau8_Version[0], &c_Data[0], 3U);
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   m_LogServiceError("ReadDataByIdentifier::ProtocolDriverImplementationVersion", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compose 24bit data pool ID

   Pack data pool, list, element index into 24bit data pool ID.

   \param[in]  ou8_DataPoolIndex  index of data pool
   \param[in]  ou16_ListIndex     list index
   \param[in]  ou16_ElementIndex  element index
   \param[out] orau8_PackedId     packed ID to be placed into protocol service

   \return
   C_NO_ERR   packed
   C_RANGE    one of the parameters is out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::m_PackDataPoolIdentifier(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                        const uint16 ou16_ElementIndex, uint8(&orau8_PackedId)[3]) const
{
   sint32 s32_Return = C_RANGE;

   if ((ou8_DataPoolIndex < mhu8_OSY_MAX_NUM_DATA_POOLS) && (ou16_ListIndex < mhu8_OSY_MAX_NUM_DATA_POOL_LISTS) &&
       (ou16_ElementIndex < mhu16_OSY_MAX_NUM_DATA_POOL_LIST_ELEMENTS))
   {
      const uint32 u32_PackedId = (static_cast<uint32>(ou8_DataPoolIndex) << 18) +
                                  (static_cast<uint32>(ou16_ListIndex) << 11) +
                                  ou16_ElementIndex;
      orau8_PackedId[0] = static_cast<uint8>(u32_PackedId >> 16);
      orau8_PackedId[1] = static_cast<uint8>(u32_PackedId >> 8);
      orau8_PackedId[2] = static_cast<uint8>(u32_PackedId);
      s32_Return = C_NO_ERR;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Split 24bit data pool ID

   Unpack data pool, list, element index from 24bit data pool ID.

   \param[in]  orau8_PackedId      packed ID to be placed into protocol service
   \param[out] oru8_DataPoolIndex  index of data pool
   \param[out] oru16_ListIndex     list index
   \param[out] oru16_ElementIndex  element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsy::m_UnpackDataPoolIdentifier(const uint8 (&orau8_PackedId)[3], uint8 & oru8_DataPoolIndex,
                                                        uint16 & oru16_ListIndex, uint16 & oru16_ElementIndex) const
{
   const uint32 u32_PackedId = (static_cast<uint32>(orau8_PackedId[0]) << 16) +
                               (static_cast<uint32>(orau8_PackedId[1]) << 8) +
                               static_cast<uint32>(orau8_PackedId[2]);

   oru8_DataPoolIndex = static_cast<uint8>(u32_PackedId >> 18) & 0x1FU;
   oru16_ListIndex = static_cast<uint16>(u32_PackedId >> 11) & 0x7FU;
   oru16_ElementIndex = static_cast<uint16>(u32_PackedId) & 0x7FFU;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadDataPoolDataByIdentifier service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[in]  ou8_DataPoolIndex  index of data pool to read from (0..31)
   \param[in]  ou16_ListIndex     list index to read from (0..127)
   \param[in]  ou16_ElementIndex  element index to read (0..2047)
   \param[out] orc_ReadData       read data (size as sent by server)
   \param[out] opu8_NrCode        if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_RANGE    data pool, list or element index out of range
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadDataPoolData(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                   const uint16 ou16_ElementIndex, std::vector<uint8> & orc_ReadData,
                                                   uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Request;
   C_OSCProtocolDriverOsyService c_Response;
   uint8 u8_NrErrorCode = 0U;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      uint8 au8_Identifier[3];
      s32_Return = m_PackDataPoolIdentifier(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, au8_Identifier);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_RANGE;
      }
      else
      {
         c_Request.c_Data.resize(4);
         c_Request.c_Data[0] = mhu8_OSY_SI_READ_DATA_POOL_DATA_BY_ID;
         c_Request.c_Data[1] = au8_Identifier[0];
         c_Request.c_Data[2] = au8_Identifier[1];
         c_Request.c_Data[3] = au8_Identifier[2];
         s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
         if (s32_Return != C_NO_ERR)
         {
            s32_Return = C_NOACT;
         }
         else
         {
            s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_READ_DATA_POOL_DATA_BY_ID, 4U, c_Response,
                                                          u8_NrErrorCode, false);
            switch (s32_Return)
            {
            case C_NO_ERR:

               //check DI:
               if ((c_Request.c_Data[1] != c_Response.c_Data[1]) ||
                   (c_Request.c_Data[2] != c_Response.c_Data[2]) ||
                   (c_Request.c_Data[3] != c_Response.c_Data[3]))
               {
                  s32_Return = C_RD_WR;
               }
               else
               {
                  orc_ReadData.resize(c_Response.c_Data.size() - 4);
                  (void)std::memcpy(&orc_ReadData[0], &c_Response.c_Data[4], c_Response.c_Data.size() - 4);
               }
               break;
            case C_WARN:
               if (opu8_NrCode != NULL)
               {
                  (*opu8_NrCode) = u8_NrErrorCode;
               }
               break;
            default:
               break;
            }
         }
      }
   }

   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("ReadDataPoolData(Datapool:%d List:%d Element:%d)",
                                 ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   WriteDataPoolDataByIdentifier service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[in]  ou8_DataPoolIndex  index of data pool to read from (0..31)
   \param[in]  ou16_ListIndex     list index to read from (0..127)
   \param[in]  ou16_ElementIndex  element index to read (0..2047)
   \param[in]  orc_DataToWrite    data to write
   \param[out] opu8_NrCode        if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_RANGE    data pool, list or element index out of range; length of data zero
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyWriteDataPoolData(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                    const uint16 ou16_ElementIndex,
                                                    const std::vector<uint8> & orc_DataToWrite,
                                                    uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Request;
   C_OSCProtocolDriverOsyService c_Response;
   uint8 u8_NrErrorCode = 0U;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else if (orc_DataToWrite.size() == 0)
   {
      s32_Return = C_RANGE;
   }
   else
   {
      uint8 au8_Identifier[3];
      s32_Return = m_PackDataPoolIdentifier(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, au8_Identifier);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_RANGE;
      }
      else
      {
         c_Request.c_Data.resize(4 + orc_DataToWrite.size());
         c_Request.c_Data[0] = mhu8_OSY_SI_WRITE_DATA_POOL_DATA_BY_ID;
         c_Request.c_Data[1] = au8_Identifier[0];
         c_Request.c_Data[2] = au8_Identifier[1];
         c_Request.c_Data[3] = au8_Identifier[2];
         (void)std::memcpy(&c_Request.c_Data[4], &orc_DataToWrite[0], orc_DataToWrite.size());

         s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
         if (s32_Return != C_NO_ERR)
         {
            s32_Return = C_NOACT;
         }
         else
         {
            s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_WRITE_DATA_POOL_DATA_BY_ID, 4U, c_Response,
                                                          u8_NrErrorCode, true);
            switch (s32_Return)
            {
            case C_NO_ERR:

               //check DI:
               if ((c_Request.c_Data[1] != c_Response.c_Data[1]) ||
                   (c_Request.c_Data[2] != c_Response.c_Data[2]) ||
                   (c_Request.c_Data[3] != c_Response.c_Data[3]))
               {
                  s32_Return = C_RD_WR;
               }
               break;
            case C_WARN:
               if (opu8_NrCode != NULL)
               {
                  (*opu8_NrCode) = u8_NrErrorCode;
               }
               break;
            default:
               break;
            }
         }
      }
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("WriteDataPoolData(Datapool:%d List:%d Element:%d Size:%d)",
                                 ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, orc_DataToWrite.size());
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   WriteDataPoolEventDataRate service request implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[in]  ou8_TransmissionRail  index of interval "rail" to send on (0..2)
   \param[in]  ou16_DataRate         data rate in ms
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_RANGE    Transmission rail invalid
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyWriteDataPoolEventDataRate(const uint8 ou8_TransmissionRail,
                                                             const uint16 ou16_DataRate, uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;

   std::vector<uint8> c_Data;
   uint8 u8_NrErrorCode = 0U;

   if (ou8_TransmissionRail <= 2)
   {
      uint16 u16_DataIdentifier;

      // Get the correct data identifier
      switch (ou8_TransmissionRail)
      {
      case 0:
         u16_DataIdentifier = mhu16_OSY_DI_DATARATE_1;
         break;
      case 1:
         u16_DataIdentifier = mhu16_OSY_DI_DATARATE_2;
         break;
      case 2:
      default: // Can not happen because of range checks
         u16_DataIdentifier = mhu16_OSY_DI_DATARATE_3;
         break;
      }

      c_Data.resize(2);
      c_Data[0] = static_cast<uint8>(ou16_DataRate >> 8);
      c_Data[1] = static_cast<uint8>(ou16_DataRate & 0xFFU);

      s32_Return = m_WriteDataByIdentifier(u16_DataIdentifier, c_Data, u8_NrErrorCode);

      if (opu8_NrCode != NULL)
      {
         (*opu8_NrCode) = u8_NrErrorCode;
      }
   }

   m_LogServiceError("WriteDataByIdentifier::DataPoolEventDataRate", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadDataPoolDataCyclic service request implementation

   Send request and wait for initial response.
   Further Responses are handled asynchronously in Cycle()

   \param[in]  ou8_DataPoolIndex     index of data pool to read from
   \param[in]  ou16_ListIndex        list index to read from
   \param[in]  ou16_ElementIndex     element index to read
   \param[in]  ou8_TransmissionRail  index of interval "rail" to send on (0..2)
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent
   C_RANGE    data pool, list, element index, rail index out of range; length of data zero
   C_NOACT    could not put request in Tx queue
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data pool index)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadDataPoolDataCyclic(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                         const uint16 ou16_ElementIndex,
                                                         const uint8 ou8_TransmissionRail, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Request;
   C_OSCProtocolDriverOsyService c_Response;
   uint8 u8_NrErrorCode = 0U;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else if (ou8_TransmissionRail > 2U)
   {
      s32_Return = C_RANGE;
   }
   else
   {
      uint8 au8_Identifier[3];
      s32_Return = m_PackDataPoolIdentifier(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, au8_Identifier);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_RANGE;
      }
      else
      {
         c_Request.c_Data.resize(5);
         c_Request.c_Data[0] = mhu8_OSY_SI_READ_DATA_POOL_DATA_EVENT_DRIVEN;
         c_Request.c_Data[1] = ou8_TransmissionRail + 1U; //cyclic rails: 1..3
         c_Request.c_Data[2] = au8_Identifier[0];
         c_Request.c_Data[3] = au8_Identifier[1];
         c_Request.c_Data[4] = au8_Identifier[2];
         s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
         if (s32_Return != C_NO_ERR)
         {
            s32_Return = C_NOACT;
         }
         else
         {
            std::vector<uint8> c_IdentifierForErrorResponse;

            c_IdentifierForErrorResponse.resize(3);
            c_IdentifierForErrorResponse[0] = au8_Identifier[0];
            c_IdentifierForErrorResponse[1] = au8_Identifier[1];
            c_IdentifierForErrorResponse[2] = au8_Identifier[2];

            s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_READ_DATA_POOL_DATA_EVENT_DRIVEN,
                                                          4U, c_Response,
                                                          u8_NrErrorCode, true, &c_IdentifierForErrorResponse);
            if (s32_Return == C_NO_ERR)
            {
               if ((c_Response.c_Data[1] != c_Request.c_Data[2]) ||
                   (c_Response.c_Data[2] != c_Request.c_Data[3]) ||
                   (c_Response.c_Data[3] != c_Request.c_Data[4]))
               {
                  s32_Return = C_RD_WR;
               }
            }
            else if (s32_Return == C_WARN)
            {
               if (opu8_NrCode != NULL)
               {
                  (*opu8_NrCode) = u8_NrErrorCode;
               }
            }
            else
            {
               // Nothing to do
            }
         }
      }
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("ReadDataPoolDataCyclic(Datapool:%d List:%d Element:%d Rail:%d)",
                                 ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, ou8_TransmissionRail);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadDataPoolDataChangeDriver service request implementation

   Send request and wait for initial response.
   Further responses are handled asynchronously in Cycle()

   \param[in]  ou8_DataPoolIndex     index of data pool to read from
   \param[in]  ou16_ListIndex        list index to read from
   \param[in]  ou16_ElementIndex     element index to read
   \param[in]  ou8_TransmissionRail  index of interval "rail" to send on (0..2)
   \param[in]  ou32_Hysteresis       threshold the value must change before it is re-sent
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent
   C_RANGE    data pool, list, element index, rail index out of range; length of data zero
   C_NOACT    could not put request in Tx queue
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data pool index)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadDataPoolDataChangeDriven(const uint8 ou8_DataPoolIndex,
                                                               const uint16 ou16_ListIndex,
                                                               const uint16 ou16_ElementIndex,
                                                               const uint8 ou8_TransmissionRail,
                                                               const uint32 ou32_Hysteresis, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Request;
   C_OSCProtocolDriverOsyService c_Response;
   uint8 u8_NrErrorCode = 0U;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else if (ou8_TransmissionRail > 2U)
   {
      s32_Return = C_RANGE;
   }
   else
   {
      uint8 au8_Identifier[3];
      s32_Return = m_PackDataPoolIdentifier(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, au8_Identifier);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_RANGE;
      }
      else
      {
         c_Request.c_Data.resize(9);
         c_Request.c_Data[0] = mhu8_OSY_SI_READ_DATA_POOL_DATA_EVENT_DRIVEN;
         c_Request.c_Data[1] = ou8_TransmissionRail + 4U; //cyclic rails: 4..6
         c_Request.c_Data[2] = au8_Identifier[0];
         c_Request.c_Data[3] = au8_Identifier[1];
         c_Request.c_Data[4] = au8_Identifier[2];
         c_Request.c_Data[5] = static_cast<uint8>(ou32_Hysteresis >> 24U);
         c_Request.c_Data[6] = static_cast<uint8>(ou32_Hysteresis >> 16U);
         c_Request.c_Data[7] = static_cast<uint8>(ou32_Hysteresis >> 8U);
         c_Request.c_Data[8] = static_cast<uint8>(ou32_Hysteresis & 0xFFU);

         s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
         if (s32_Return != C_NO_ERR)
         {
            s32_Return = C_NOACT;
         }
         else
         {
            std::vector<uint8> c_IdentifierForErrorResponse;

            c_IdentifierForErrorResponse.resize(3);
            c_IdentifierForErrorResponse[0] = au8_Identifier[0];
            c_IdentifierForErrorResponse[1] = au8_Identifier[1];
            c_IdentifierForErrorResponse[2] = au8_Identifier[2];

            s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_READ_DATA_POOL_DATA_EVENT_DRIVEN,
                                                          4U, c_Response,
                                                          u8_NrErrorCode, true, &c_IdentifierForErrorResponse);

            if (s32_Return == C_NO_ERR)
            {
               if ((c_Response.c_Data[1] != c_Request.c_Data[2]) ||
                   (c_Response.c_Data[2] != c_Request.c_Data[3]) ||
                   (c_Response.c_Data[3] != c_Request.c_Data[4]))
               {
                  s32_Return = C_RD_WR;
               }
            }
            else if (s32_Return == C_WARN)
            {
               if (opu8_NrCode != NULL)
               {
                  (*opu8_NrCode) = u8_NrErrorCode;
               }
            }
            else
            {
               // Nothing to do
            }
         }
      }
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("ReadDataPoolDataChangeDriven(Datapool:%d List:%d Element:%d Rail:%d Hysteresis:%d)",
                                 ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, ou8_TransmissionRail,
                                 ou32_Hysteresis);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stop datapool event driven service request implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent
   C_RANGE    data pool, list, element index, rail index out of range; length of data zero
   C_NOACT    could not put request in Tx queue
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data pool index)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyStopDataPoolEvents(uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   uint8 u8_NrErrorCode = 0U;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      C_OSCProtocolDriverOsyService c_Request;
      C_OSCProtocolDriverOsyService c_Response;

      c_Request.c_Data.resize(2);
      c_Request.c_Data[0] = mhu8_OSY_SI_READ_DATA_POOL_DATA_EVENT_DRIVEN;
      c_Request.c_Data[1] = 0U; // Deactivate transmission

      s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
      else
      {
         s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_READ_DATA_POOL_DATA_EVENT_DRIVEN,
                                                       1U, c_Response, u8_NrErrorCode, true);

         if (s32_Return == C_WARN)
         {
            if (opu8_NrCode != NULL)
            {
               (*opu8_NrCode) = u8_NrErrorCode;
            }
         }
      }
   }
   m_LogServiceError("StopDataPoolEvents", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read data pool meta data

   \param[in]  ou8_DataPoolIndex  Data pool index
   \param[out] orc_MetaData       Meta data returned by service
   \param[out] opu8_NrCode        if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data pool index)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadDataPoolMetaData(const uint8 ou8_DataPoolIndex, C_DataPoolMetaData & orc_MetaData,
                                                       uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   uint8 u8_NrErrorCode = 0U;

   std::vector<uint8> c_SendData;
   std::vector<uint8> c_ReceiveData;

   c_SendData.resize(1, ou8_DataPoolIndex);
   s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_READ_DATAPOOL_META_DATA, mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE,
                                 c_SendData, 0, false, c_ReceiveData, u8_NrErrorCode);
   if ((s32_Return == C_NO_ERR) && (c_ReceiveData.size() >= 1))
   {
      if (c_ReceiveData[0] == ou8_DataPoolIndex)
      {
         bool q_Stop = false;
         orc_MetaData.c_Name = "";
         orc_MetaData.au8_Version[0] = 0x00U;
         orc_MetaData.au8_Version[1] = 0x00U;
         orc_MetaData.au8_Version[2] = 0x00U;

         //Parse received data
         for (uint32 u32_ItReceivedData = 1; (u32_ItReceivedData < c_ReceiveData.size()) && (q_Stop == false);)
         {
            for (uint32 u32_ItSegment = 0; u32_ItSegment < 2; ++u32_ItSegment)
            {
               //Segment start
               switch (c_ReceiveData[u32_ItReceivedData])
               {
               case 1UL:
                  //Iterate to version segment
                  ++u32_ItReceivedData;
                  for (uint32 u32_ItVersion = 0; (u32_ItReceivedData < c_ReceiveData.size()) && (u32_ItVersion < 3);
                       ++u32_ItVersion)
                  {
                     orc_MetaData.au8_Version[u32_ItVersion] = c_ReceiveData[u32_ItReceivedData];

                     //Iterate segment
                     ++u32_ItReceivedData;
                  }
                  break;
               case 2UL:
                  //Iterate to byte count
                  ++u32_ItReceivedData;
                  if (u32_ItReceivedData < c_ReceiveData.size())
                  {
                     const uint32 u32_Count = c_ReceiveData[u32_ItReceivedData];
                     orc_MetaData.c_Name = "";
                     //Iterate to name segment
                     ++u32_ItReceivedData;
                     for (uint32 u32_ItName =
                             0; (u32_ItReceivedData < c_ReceiveData.size()) && (u32_ItName < u32_Count);
                          ++u32_ItName)
                     {
                        orc_MetaData.c_Name += static_cast<charn>(c_ReceiveData[u32_ItReceivedData]);
                        //Iterate segment
                        ++u32_ItReceivedData;
                     }
                  }
                  break;
               default:
                  m_LogErrorWithHeader("Synchronous communication",
                                       "Unknown data pool meta data ID received. Parsing stopped.", TGL_UTIL_FUNC_ID);
                  q_Stop = true;
                  break;
               }
            }
         }
      }
      else
      {
         s32_Return = C_RD_WR;
      }
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RoutineControl::ReadDataPoolMetaData(Datapool:%d)", ou8_DataPoolIndex);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Verify data pool consistency

   \param[in]  ou8_DataPoolIndex     Data pool index
   \param[in]  ou32_DataPoolChecksum Data pool checksum
   \param[out] orq_Match             Data pool checksum match response
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data pool index)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyVerifyDataPool(const uint8 ou8_DataPoolIndex, const uint32 ou32_DataPoolChecksum,
                                                 bool & orq_Match, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   uint8 u8_NrErrorCode = 0U;

   std::vector<uint8> c_SendData;
   std::vector<uint8> c_ReceiveData;
   c_SendData.resize(5);
   c_SendData[0] = ou8_DataPoolIndex;
   c_SendData[1] = static_cast<uint8>(ou32_DataPoolChecksum >> 24U);
   c_SendData[2] = static_cast<uint8>(ou32_DataPoolChecksum >> 16U);
   c_SendData[3] = static_cast<uint8>(ou32_DataPoolChecksum >> 8U);
   c_SendData[4] = static_cast<uint8>(ou32_DataPoolChecksum & 0xFFU);
   s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_VERIFY_DATAPOOL, mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE,
                                 c_SendData, 2, true, c_ReceiveData, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      if (c_ReceiveData[0] == ou8_DataPoolIndex)
      {
         orq_Match = ((c_ReceiveData[1] & 0x01U) == 0x01U) ? false : true;
      }
      else
      {
         s32_Return = C_RD_WR;
      }
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RoutineControl::VerifyDataPool(Datapool:%d, Checksum:%d)", ou8_DataPoolIndex,
                                 ou32_DataPoolChecksum);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Route diagnosis communication activation service request implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[in]     ou8_InputChannelType    Channel type of input channel (0 equals CAN, 1 equals Ethernet)
   \param[in]     ou8_InputChannelIndex   Channel index of input channel
   \param[in]     ou8_OutputChannelType   Channel type of output channel (0 equals CAN, 1 equals Ethernet)
   \param[in]     ou8_OutputChannelIndex  Channel index of output channel
   \param[in]     ou8_SourceBusId         The bus ID on client side on the input channel direction (0..15)
   \param[in]     ou8_TargetBusId         The bus ID on target server side on the output channel direction (0..15)
   \param[out]    opu8_NrCode             if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong routine identifier ID)
   C_RANGE    - input channel type is CAN and output channel type is Ethernet. This combination is not supported.
              - a bus ID is out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsySetRouteDiagnosisCommunication(const uint8 ou8_InputChannelType,
                                                                 const uint8 ou8_InputChannelIndex,
                                                                 const uint8 ou8_OutputChannelType,
                                                                 const uint8 ou8_OutputChannelIndex,
                                                                 const uint8 ou8_SourceBusId,
                                                                 const uint8 ou8_TargetBusId, uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;
   uint8 u8_NrErrorCode = 0U;

   if ((ou8_SourceBusId <= C_OSCProtocolDriverOsyNode::mhu8_MAX_BUS) &&
       (ou8_TargetBusId <= C_OSCProtocolDriverOsyNode::mhu8_MAX_BUS) &&
       ((ou8_InputChannelType != 0) || (ou8_OutputChannelType != 1)))
   {
      std::vector<uint8> c_SendData;
      std::vector<uint8> c_ReceiveData;

      c_SendData.resize(6);
      c_SendData[0] = ou8_InputChannelType;
      c_SendData[1] = ou8_InputChannelIndex;
      c_SendData[2] = ou8_OutputChannelType;
      c_SendData[3] = ou8_OutputChannelIndex;
      c_SendData[4] = ou8_SourceBusId;
      c_SendData[5] = ou8_TargetBusId;

      s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_ROUTE_DIAGNOSIS_COMMUNICATION,
                                    mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE,
                                    c_SendData, 0, true, c_ReceiveData, u8_NrErrorCode);
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RoutineControl::SetRouteDiagnosisCommunication(InputType:%d, InputIndex:%d, "
                                 "OutputType:%d, OutputIndex:%d, SourceId:%d, TargetId:%d)",
                                 ou8_InputChannelType,
                                 ou8_InputChannelIndex,
                                 ou8_OutputChannelType,
                                 ou8_OutputChannelIndex,
                                 ou8_SourceBusId,
                                 ou8_TargetBusId);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Route diagnosis communication deactivation service request implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong routine identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyStopRouteDiagnosisCommunication(uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_SendData;
   std::vector<uint8> c_ReceiveData;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_ROUTE_DIAGNOSIS_COMMUNICATION,
                                 mhu8_OSY_RC_SUB_FUNCTION_STOP_ROUTINE,
                                 c_SendData, 0, true, c_ReceiveData, u8_NrErrorCode);
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   m_LogServiceError("RoutineControl::StopRouteDiagnosisCommunication", s32_Return, u8_NrErrorCode);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates the IP to IP routing for a specific node

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Can be used with Ethernet only.

   Caution: The router node will drop out of its session after the check. All previous activated
            CAN routes will be dropped on the router node!

   \param[in]     ou8_OutputChannelType   Channel type of output channel (0 equals CAN, 1 equals Ethernet)
   \param[in]     ou8_OutputChannelIndex  Channel index of output channel
   \param[in]     ou8_SourceBusId         The bus ID on client side on the input channel direction (0..15)
   \param[in]     ou8_TargetBusId         The bus ID on target server side on the output channel direction (0..15)
   \param[in]     orau8_IpAddress         IP V4 address of target server
   \param[out]    opu8_NrCode             if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_RANGE    - output channel type is not Ethernet. Only Ethernet is supported.
              - a bus ID is out of range
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsySetRouteIp2IpCommunication(const uint8 ou8_OutputChannelType,
                                                             const uint8 ou8_OutputChannelIndex,
                                                             const uint8 ou8_SourceBusId, const uint8 ou8_TargetBusId,
                                                             const uint8 (&orau8_IpAddress)[4],
                                                             uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;
   uint8 u8_NrErrorCode = 0U;

   if ((ou8_SourceBusId <= C_OSCProtocolDriverOsyNode::mhu8_MAX_BUS) &&
       (ou8_TargetBusId <= C_OSCProtocolDriverOsyNode::mhu8_MAX_BUS) &&
       (ou8_OutputChannelType == 1))
   {
      std::vector<uint8> c_SendData;
      std::vector<uint8> c_ReceiveData;

      c_SendData.resize(8);
      c_SendData[0] = ou8_OutputChannelType;
      c_SendData[1] = ou8_OutputChannelIndex;
      c_SendData[2] = ou8_SourceBusId;
      c_SendData[3] = ou8_TargetBusId;
      c_SendData[4] = orau8_IpAddress[0];
      c_SendData[5] = orau8_IpAddress[1];
      c_SendData[6] = orau8_IpAddress[2];
      c_SendData[7] = orau8_IpAddress[3];

      s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_ROUTE_IP_2_IP_COMMUNICATION,
                                    mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE,
                                    c_SendData, 0, true, c_ReceiveData, u8_NrErrorCode);
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RoutineControl::OsySetRouteIp2IpCommunication("
                                 "OutputType:%d, OutputIndex:%d, SourceId:%d, TargetId:%d, "
                                 "IP:[%d.%d.%d.%d])",
                                 ou8_OutputChannelType,
                                 ou8_OutputChannelIndex,
                                 ou8_SourceBusId,
                                 ou8_TargetBusId,
                                 orau8_IpAddress[0], orau8_IpAddress[1], orau8_IpAddress[2], orau8_IpAddress[3]);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check status of IP to IP routing

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Can be used with Ethernet only.

   \param[out] oru8_Status        Status information of IP to IP routing
                                    0: idle (no routing activation in progress)
                                    1: in progress (requested activation in progress)
                                    2: error (requests routing activation failed)
                                    3: connected (requested activation finished)
                                    4..255: reserved
   \param[out] opu8_NrCode        if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyCheckRouteIp2IpCommunication(uint8 & oru8_Status, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   uint8 u8_NrErrorCode = 0U;

   std::vector<uint8> c_SendData;
   std::vector<uint8> c_ReceiveData;

   c_SendData.resize(0);

   s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_ROUTE_IP_2_IP_COMMUNICATION,
                                 mhu8_OSY_RC_SUB_FUNCTION_REQUEST_ROUTINE_RESULTS,
                                 c_SendData, 1, true, c_ReceiveData, u8_NrErrorCode);

   if (s32_Return == C_NO_ERR)
   {
      oru8_Status = c_ReceiveData[0];
   }

   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RoutineControl::OsyCheckRouteIp2IpCommunication()");
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   StartRoutine: SendCanMessage service implementation

   Send request to send CAN message on target channel.
   Wait for response (i.e. confirmation the message was sent) from server node.
   See class description for general handling of "polled" services.
   Payload data of the CAN message which are not used (DLC configuration) are set to 0.

   \param[in]  ou8_ChannelIndex   target CAN channel to send on
   \param[in]  orc_CanMessage     CAN message to send
   \param[out] opu8_NrCode        if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_RANGE    CAN message invalid (RTR bit set; ID out of range; DLC out of range)
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsySendCanMessage(const uint8 ou8_ChannelIndex,
                                                 const stw_can::T_STWCAN_Msg_TX & orc_CanMessage,
                                                 uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   uint8 u8_NrErrorCode = 0U;

   std::vector<uint8> c_ReceiveData;
   std::vector<uint8> c_SendData;

   if ((orc_CanMessage.u8_RTR != 0U) || (orc_CanMessage.u8_XTD > 1U) ||
       ((orc_CanMessage.u8_XTD == 0U) && (orc_CanMessage.u32_ID > 0x7FFU)) ||
       ((orc_CanMessage.u8_XTD == 1U) && (orc_CanMessage.u32_ID > 0x3FFFFFFFU)) ||
       (orc_CanMessage.u8_DLC > 8U))
   {
      s32_Return = C_RANGE;
   }
   else
   {
      uint32 u32_Id;
      c_SendData.resize(14);
      u32_Id = orc_CanMessage.u32_ID;
      if (orc_CanMessage.u8_XTD == 1U)
      {
         u32_Id |= 0x80000000U; //set 29bit flag
      }

      c_SendData[0] = static_cast<uint8>(u32_Id >> 24U);
      c_SendData[1] = static_cast<uint8>(u32_Id >> 16U);
      c_SendData[2] = static_cast<uint8>(u32_Id >> 8U);
      c_SendData[3] = static_cast<uint8>(u32_Id);
      c_SendData[4] = orc_CanMessage.u8_DLC;
      (void)std::memcpy(&c_SendData[5], &orc_CanMessage.au8_Data[0], orc_CanMessage.u8_DLC);
      //keep a clean (reproducible) message; set empty bytes to zero
      (void)std::memset(&c_SendData[5U + static_cast<uintn>(orc_CanMessage.u8_DLC)], 0x00U,
                        8U - static_cast<uintn>(orc_CanMessage.u8_DLC));
      c_SendData[13] = ou8_ChannelIndex;

      // Using openSYDE specific multi frames without flow control
      s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_SEND_CAN_MESSAGE, mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE,
                                    c_SendData, 0U, true, c_ReceiveData, u8_NrErrorCode, true);
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RoutineControl::SendCanMessage(Channel:%d Id:%d DLC:%d)",
                                 ou8_ChannelIndex,
                                 orc_CanMessage.u32_ID,
                                 orc_CanMessage.u8_DLC);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Tunnel can messages activation service request implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[in]     ou8_CanChannelIndex     Channel index of observed channel
   \param[in]     ou32_FilterId           Filter id for all relevant IDs
                                          The MSB is the flag for 11 Bit (= 0) or 29 Bit (= 1)
                                          Bits 29 and 30 are reserved (shall be set to zero)
   \param[in]     ou32_FilterMask         Mask that is applied to both the received CAN frame identifiers an the
                                          FilterId before comparison.
                                          Bits 29, 30 and 31 are reserved (shall be set to zero)
   \param[out]    opu8_NrCode             if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong routine identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsySetTunnelCanMessages(const uint8 ou8_CanChannelIndex, const uint32 ou32_FilterId,
                                                       const uint32 ou32_FilterMask, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_SendData;
   std::vector<uint8> c_ReceiveData;
   uint8 u8_NrErrorCode = 0U;

   c_SendData.resize(9);
   c_SendData[0] = ou8_CanChannelIndex;
   c_SendData[1] = static_cast<uint8>(ou32_FilterId >> 24U);
   c_SendData[2] = static_cast<uint8>(ou32_FilterId >> 16U);
   c_SendData[3] = static_cast<uint8>(ou32_FilterId >> 8U);
   c_SendData[4] = static_cast<uint8>(ou32_FilterId);
   c_SendData[5] = static_cast<uint8>(ou32_FilterMask >> 24U);
   c_SendData[6] = static_cast<uint8>(ou32_FilterMask >> 16U);
   c_SendData[7] = static_cast<uint8>(ou32_FilterMask >> 8U);
   c_SendData[8] = static_cast<uint8>(ou32_FilterMask);

   s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_TUNNEL_CAN_MESSAGE,
                                 mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE,
                                 c_SendData, 0, true, c_ReceiveData, u8_NrErrorCode);
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RoutineControl::SetTunnelCanMessages(Channel:%d Filter-Id:%d Mask:%d)",
                                 ou8_CanChannelIndex,
                                 ou32_FilterId,
                                 ou32_FilterMask);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Tunnel can messages deactivation service request implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong routine identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyStopTunnelCanMessages(uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_SendData;
   std::vector<uint8> c_ReceiveData;
   uint8 u8_NrErrorCode = 0U;

   s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_TUNNEL_CAN_MESSAGE,
                                 mhu8_OSY_RC_SUB_FUNCTION_STOP_ROUTINE,
                                 c_SendData, 0, true, c_ReceiveData, u8_NrErrorCode);
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      m_LogServiceError("RoutineControl::StopTunnelCanMessages", s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Print logging text

   Add information about this class and the current server-id configuration and call logger
    to perform the actual logging.

   \param[in]     orc_Activity        Current activity
   \param[in]     orc_Information     text to log
   \param[in]     opcn_Function       Function name
   \param[in]     oq_AsError          true: log as "ERROR"
                                      false: log as "WARNING"
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsy::m_LogErrorWithHeader(const C_SCLString & orc_Activity,
                                                  const stw_scl::C_SCLString & orc_Information,
                                                  const charn * const opcn_Function, const bool oq_AsError) const
{
   const C_SCLString c_LogText = "openSYDE protocol driver node " +
                                 C_SCLString::IntToStr(mc_ServerId.u8_BusIdentifier) + "." +
                                 C_SCLString::IntToStr(mc_ServerId.u8_NodeIdentifier) + ": " + orc_Information;

   if (oq_AsError == true)
   {
      C_OSCLoggingHandler::h_WriteLogError(orc_Activity, c_LogText, __FILE__, opcn_Function);
   }
   else
   {
      C_OSCLoggingHandler::h_WriteLogWarning(orc_Activity, c_LogText, __FILE__, opcn_Function);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Make transport protocol to use known

   Install (or uninstall) transport protocol to use.
   The passed protocol will be remembered for further use and the client and server addresses will be
    assigned to the newly installed protocol.

   \param[in] opc_TransportProtocol   transport protocol to use; NULL: do not use protocol

   \return
   C_NO_ERR   no problems
   C_CONFIG   transport protocol was set, but could not set node identifiers in installed protocol
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::SetTransportProtocol(C_OSCProtocolDriverOsyTpBase * const opc_TransportProtocol)
{
   sint32 s32_Return = C_NO_ERR;

   mpc_TransportProtocol = opc_TransportProtocol;
   if (mpc_TransportProtocol != NULL)
   {
      s32_Return = mpc_TransportProtocol->SetNodeIdentifiers(mc_ClientId, mc_ServerId);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_CONFIG;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Make client and server identifiers known

   Set client and server node identifiers.
   If a transport protocol ist installed the new identifiers will be set there, too.

   \param[in] orc_ClientId    new client ID (= our own ID)
   \param[in] orc_ServerId    new server ID (= ID of server we communicate with)

   \return
   C_NO_ERR   no problems
   C_CONFIG   IDs were set, but could not be propagated to the installed transport protocol
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::SetNodeIdentifiers(const C_OSCProtocolDriverOsyNode & orc_ClientId,
                                                  const C_OSCProtocolDriverOsyNode & orc_ServerId)
{
   sint32 s32_Return = C_NO_ERR;

   mc_ClientId = orc_ClientId;
   mc_ServerId = orc_ServerId;

   //propagate to installed transport protocol:
   if (mpc_TransportProtocol != NULL)
   {
      s32_Return = mpc_TransportProtocol->SetNodeIdentifiers(mc_ClientId, mc_ServerId);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_CONFIG;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get used transport protocol

   \return
   Installed transport protocol (NULL -> none installed)
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCProtocolDriverOsyTpBase * C_OSCProtocolDriverOsy::GetTransportProtocol(void)
{
   return mpc_TransportProtocol;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get configured client and server identifiers

   \param[out] orc_ClientId    client ID (= our own ID)
   \param[out] orc_ServerId    server ID (= ID of server we communicate with)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsy::GetNodeIdentifiers(C_OSCProtocolDriverOsyNode & orc_ClientId,
                                                C_OSCProtocolDriverOsyNode & orc_ServerId) const
{
   orc_ClientId = mc_ClientId;
   orc_ServerId = mc_ServerId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Perform cyclic communication tasks

   * Get exclusive access to reception handling
   * Invoke transport protocol's "Cycle" function
   * Invoke virtual functions to report all incoming event driven responses.
     If a non event-driven response is received the function will return,
     otherwise it will continue to read all incoming responses from the Rx queue.
   * Relinquish exclusive access to reception handling

   If reception is already in progress the function will return with no action.

   \return
   C_NO_ERR   finished cycle
   C_CONFIG   no transport protocol installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::Cycle(void)
{
   sint32 s32_Return = C_NO_ERR;

   //while we are checking for async responses, prevent other threads starting to poll
   const bool q_LockClaimed = mc_LockReception.TryAcquire();

   if (q_LockClaimed == true)
   {
      s32_Return = this->m_Cycle();
      if ((s32_Return != C_CONFIG) && (s32_Return != C_COM))
      {
         // Only C_CONFIG is relevant for an extern call
         s32_Return = C_NO_ERR;
      }
      mc_LockReception.Release();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle async reception of ReadDataPoolEventDriven response

   Default implementation: just log ...

   \param[in] ou8_DataPoolIndex    reported data pool index
   \param[in] ou16_ListIndex       reported list index
   \param[in] ou16_ElementIndex    reported element index
   \param[in] orc_Value            reported value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsy::m_OsyReadDataPoolDataEventReceived(const uint8 ou8_DataPoolIndex,
                                                                const uint16 ou16_ListIndex,
                                                                const uint16 ou16_ElementIndex,
                                                                const std::vector<uint8> & orc_Value)
{
   (void)orc_Value;
   m_LogErrorWithHeader("Asynchronous communication", "Unhandled reception of async ReadDataPoolEventDriven response "
                        "(Datapool index: " + C_SCLString::IntToStr(
                           ou8_DataPoolIndex) +
                        " List index: " + C_SCLString::IntToStr(ou16_ListIndex) +
                        " Element index: " + C_SCLString::IntToStr(ou16_ElementIndex) +
                        "). Ignoring.", TGL_UTIL_FUNC_ID);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle async reception of ReadDataPoolEventDriven negative response

   Default implementation: just log ...

   \param[in] ou8_DataPoolIndex    reported data pool index
   \param[in] ou16_ListIndex       reported list index
   \param[in] ou16_ElementIndex    reported element index
   \param[in] ou8_NrCode           reported negative response code
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsy::m_OsyReadDataPoolDataEventErrorReceived(const uint8 ou8_DataPoolIndex,
                                                                     const uint16 ou16_ListIndex,
                                                                     const uint16 ou16_ElementIndex,
                                                                     const uint8 ou8_NrCode)
{
   m_LogErrorWithHeader("Asynchronous communication", "Unhandled reception of async ReadDataPoolEventDriven negative response"
                        " (Datapool index: " + C_SCLString::IntToStr(
                           ou8_DataPoolIndex) +
                        " List index: " + C_SCLString::IntToStr(ou16_ListIndex) +
                        " Element index: " + C_SCLString::IntToStr(ou16_ElementIndex) + " NRC: " +
                        C_SCLString::IntToHex(ou8_NrCode, 2) + "). Ignoring.", TGL_UTIL_FUNC_ID);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   CheckFlashMemoryAvailable service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Check on enough flash memory available on the server.

   Send format: 4 bytes flash start address, 4 bytes number of bytes
   Example: (0xA0, 0x08, 0x00, 0x00, 0x00, 0x10, 0x00) -> 0xA0080000, 4096 bytes

   \param[in]  ou32_StartAddress     start address in flash memory
   \param[in]  ou32_Size             size of flash memory to check
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyCheckFlashMemoryAvailable(const uint32 ou32_StartAddress, const uint32 ou32_Size,
                                                            uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_ReceiveData;
   std::vector<uint8> c_SendData;
   uint8 u8_NrErrorCode = 0U;

   c_SendData.resize(8);
   c_SendData[0] = static_cast<uint8>(ou32_StartAddress >> 24U);
   c_SendData[1] = static_cast<uint8>(ou32_StartAddress >> 16U);
   c_SendData[2] = static_cast<uint8>(ou32_StartAddress >> 8U);
   c_SendData[3] = static_cast<uint8>(ou32_StartAddress & 0xFFU);
   c_SendData[4] = static_cast<uint8>(ou32_Size >> 24U);
   c_SendData[5] = static_cast<uint8>(ou32_Size >> 16U);
   c_SendData[6] = static_cast<uint8>(ou32_Size >> 8U);
   c_SendData[7] = static_cast<uint8>(ou32_Size & 0xFFU);
   s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_CHECK_FLASH_MEMORY_AVAILABILITY,
                                 mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE, c_SendData,  0U, true, c_ReceiveData,
                                 u8_NrErrorCode);
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RoutineControl::CheckFlashMemoryAvailable(Address:%d Size:%d)",
                                 ou32_StartAddress, ou32_Size);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   RoutineControl utility wrapper

   Send RoutineControl request and wait for response.
   See class description for general handling of "polled" services.

   \param[in]  ou16_RoutineIdentifier     identifier to select type of routine
   \param[in]  ou8_SubFunction            1: start routine, 2: stop routine, 3: request routine results
   \param[in]  orc_SendData               payload data to send
   \param[in]  ou16_ExpectedPayloadSize   number of payload bytes to expect in response
   \param[in]  oq_ExactSizeExpected       true: expect exactly ou16_ExpectedPayloadSize bytes
                                          false: expect at least ou16_ExpectedPayloadSize bytes
   \param[out] orc_ReadData               payload data read
   \param[out] oru8_NrCode                negative response code in case of an error response
   \param[in]  oq_CanTransferWithoutFlowControl   flag if openSYDE specific frame shall be used for the service

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::m_RoutineControl(const uint16 ou16_RoutineIdentifier, const uint8 ou8_SubFunction,
                                                const std::vector<uint8> & orc_SendData,
                                                const uint16 ou16_ExpectedPayloadSize, const bool oq_ExactSizeExpected,
                                                std::vector<uint8> & orc_ReadData, uint8 & oru8_NrCode,
                                                const bool oq_CanTransferWithoutFlowControl)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Request;
   C_OSCProtocolDriverOsyService c_Response;
   const uint32 u32_SendPayloadSize = orc_SendData.size();

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      c_Request.c_Data.resize(4U + static_cast<uintn>(u32_SendPayloadSize));
      c_Request.c_Data[0] = mhu8_OSY_SI_ROUTINE_CONTROL;
      c_Request.c_Data[1] = ou8_SubFunction;
      c_Request.c_Data[2] = static_cast<uint8>(ou16_RoutineIdentifier >> 8U);
      c_Request.c_Data[3] = static_cast<uint8>(ou16_RoutineIdentifier & 0xFFU);
      c_Request.q_CanTransferWithoutFlowControl = oq_CanTransferWithoutFlowControl;

      if (u32_SendPayloadSize > 0U)
      {
         (void)std::memcpy(&c_Request.c_Data[4], &orc_SendData[0], u32_SendPayloadSize);
      }

      s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
      else
      {
         uint8 u8_NrErrorCode;
         s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_ROUTINE_CONTROL,
                                                       ou16_ExpectedPayloadSize + 4U, c_Response,
                                                       u8_NrErrorCode, oq_ExactSizeExpected);
         switch (s32_Return)
         {
         case C_NO_ERR:
            //check response:
            if ((c_Request.c_Data[1] != c_Response.c_Data[1]) ||
                (c_Request.c_Data[2] != c_Response.c_Data[2]) ||
                (c_Request.c_Data[3] != c_Response.c_Data[3]))
            {
               s32_Return = C_RD_WR;
            }
            else
            {
               orc_ReadData.resize(c_Response.c_Data.size() - 4);
               (void)std::memcpy(&orc_ReadData[0], &c_Response.c_Data[4], c_Response.c_Data.size() - 4);
            }
            break;
         case C_WARN:
            oru8_NrCode = u8_NrErrorCode;
            break;
         default:
            break;
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Security access - Request Seed service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Request the seed for key generation from server.
   The sub-function means the level of security.
   The seed consist of 4bytes.

   \param[in]  ou8_SecurityLevel     level of requested security (= sub-function)
   \param[out] oru32_Seed            seed for key generation
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsySecurityAccessRequestSeed(const uint8 ou8_SecurityLevel, uint32 & oru32_Seed,
                                                            uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_SendData;
   std::vector<uint8> c_ReceiveData;
   uint8 u8_NrErrorCode = 0U;

   c_SendData.resize(0);
   c_ReceiveData.resize(4);

   s32_Return = m_SecurityAccess(ou8_SecurityLevel, c_SendData, 0U, 4U, c_ReceiveData, u8_NrErrorCode);

   //lint -e{864} //false positive due to const/non-const misinterpretation
   oru32_Seed = ((static_cast<uint32>(c_ReceiveData[0])) << 24U) +
                ((static_cast<uint32>(c_ReceiveData[1])) << 16U) +
                ((static_cast<uint32>(c_ReceiveData[2])) << 8U) +
                (static_cast<uint32>(c_ReceiveData[3]));
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("SecurityAccessRequestSeed(Level:%d)", ou8_SecurityLevel);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Security access - Send Key service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Send the computed key to the server.
   The sub-function + 1 means the level of security.
   The key consist of 4 bytes.

   \param[in]  ou8_SecurityLevel     level of requested security (= sub-function)
   \param[out] ou32_Key              key for security access
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsySecurityAccessSendKey(const uint8 ou8_SecurityLevel, const uint32 ou32_Key,
                                                        uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_SendData;
   std::vector<uint8> c_ReceiveData;
   uint8 u8_NrErrorCode = 0U;

   c_SendData.resize(4);
   c_SendData[0] = static_cast<uint8>(ou32_Key >> 24U);
   c_SendData[1] = static_cast<uint8>(ou32_Key >> 16U);
   c_SendData[2] = static_cast<uint8>(ou32_Key >> 8U);
   c_SendData[3] = static_cast<uint8>(ou32_Key & 0xFFU);
   c_ReceiveData.resize(0);

   s32_Return = m_SecurityAccess(ou8_SecurityLevel + 1U, c_SendData, 4U, 0U, c_ReceiveData, u8_NrErrorCode);
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("SecurityAccessSendKey(Level:%d, Key:%d)", ou8_SecurityLevel, ou32_Key);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Security access utility wrapper

   Send RoutineControl request and wait for response.
   See class description for general handling of "polled" services.

   The function either request the seed from server or send the calculated key to the server.

   \param[in]  ou8_SubFunction            0x01, 0x03, 0x05 - 0x41: request seed
                                          0x02, 0x04, 0x06 - 0x42: send key
   \param[in]  orc_SendData               payload data to send
   \param[in]  ou16_SendPayloadSize       size of payload data to send
   \param[in]  ou16_ExpectedPayloadSize   number of payload bytes to expect in response
   \param[out] orc_ReadData               payload data read
   \param[out] oru8_NrCode                negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::m_SecurityAccess(const uint8 ou8_SubFunction, const std::vector<uint8> & orc_SendData,
                                                const uint16 ou16_SendPayloadSize,
                                                const uint16 ou16_ExpectedPayloadSize,
                                                std::vector<uint8> & orc_ReadData, uint8 & oru8_NrCode)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Request;
   C_OSCProtocolDriverOsyService c_Response;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      // request seed
      if ((ou8_SubFunction % 2U) == 1U)
      {
         c_Request.c_Data.resize(2);
         c_Request.c_Data[0] = mhu8_OSY_SI_SECURITY_ACCESS;
         c_Request.c_Data[1] = ou8_SubFunction;
      }
      // send key
      else
      {
         c_Request.c_Data.resize(2U + static_cast<uintn>(ou16_SendPayloadSize));
         c_Request.c_Data[0] = mhu8_OSY_SI_SECURITY_ACCESS;
         c_Request.c_Data[1] = ou8_SubFunction;
         //lint -e{419} //std::vector reference returned by [] is linear; we allocated enough memory
         //lint -e{420} //std::vector reference returned by [] is linear; we allocated enough memory
         (void)std::memcpy(&c_Request.c_Data[2], &orc_SendData[0], ou16_SendPayloadSize);
      }

      s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
      else
      {
         uint8 u8_NrErrorCode;
         s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_SECURITY_ACCESS,
                                                       ou16_ExpectedPayloadSize + 2U, c_Response,
                                                       u8_NrErrorCode, true);
         switch (s32_Return)
         {
         case C_NO_ERR:
            //check response:
            if ((c_Request.c_Data[1] != c_Response.c_Data[1]))
            {
               s32_Return = C_RD_WR;
            }
            else
            {
               orc_ReadData.resize(c_Response.c_Data.size() - 2);
               (void)std::memcpy(&orc_ReadData[0], &c_Response.c_Data[2], c_Response.c_Data.size() - 2);
            }
            break;
         case C_WARN:
            oru8_NrCode = u8_NrErrorCode;
            break;
         default:
            break;
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle incoming async response

   Called on reception of a supposedly async positive or negative response.

   * try to interpret specified service as async response
   * decode service
   * invoke responsible virtual handler function

   Handles the following known async response types:
   * ReadDataPoolDataEventDriven
   * TunnelCanMessage
   * ReadSerialNumber (flags a server restart)

   \param[in]  orc_ReceivedService        received service suspected to be an async response

   \return
   C_NO_ERR  something we could handle ...
   C_NOACT   this function was not interested
   C_RANGE   Received message is not complete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::m_HandleAsyncResponse(const C_OSCProtocolDriverOsyService & orc_ReceivedService)
{
   sint32 s32_Return = C_NOACT;

   //is it a response at all ?
   if ((orc_ReceivedService.c_Data[0] & 0x40U) == 0U)
   {
      m_LogErrorWithHeader("Asynchronous communication",
                           "Unexpectedly received service request (not response). Ignoring.", TGL_UTIL_FUNC_ID);
   }
   else
   {
      // Check for error message
      if (orc_ReceivedService.c_Data[0] != 0x7FU)
      {
         const uint8 u8_ServiceId = (orc_ReceivedService.c_Data[0] & 0xBFU); //filter out "response" bit
         switch (u8_ServiceId)
         {
         case mhu8_OSY_SI_READ_DATA_POOL_DATA_EVENT_DRIVEN:
            s32_Return = this->m_HandleAsyncOsyReadDataPoolDataEvent(orc_ReceivedService);
            break;
         case mhu8_OSY_SI_TUNNEL_CAN_MESSAGE:
            s32_Return = this->m_HandleAsyncOsyTunnelCanMessagesEvent(orc_ReceivedService);
            break;
         case mhu8_OSY_SI_READ_SERIAL_NUMBER:
            //not used in this implementation; at least report in log
            m_LogErrorWithHeader("Asynchronous communication",
                                 "Received read serial number response (possibly startup message). Ignoring.",
                                 TGL_UTIL_FUNC_ID);
            break;
         default:
            //this is nothing we can handle; report in log
            m_LogErrorWithHeader("Asynchronous communication", "Unexpectedly received service response to service ID 0x" +
                                 C_SCLString::IntToHex(u8_ServiceId,
                                                       2) + ". Ignoring.", TGL_UTIL_FUNC_ID);

            break;
         }
      }
      else
      {
         switch (orc_ReceivedService.c_Data[1])
         {
         case mhu8_OSY_SI_READ_DATA_POOL_DATA_EVENT_DRIVEN:
            s32_Return = this->m_HandleAsyncOsyReadDataPoolDataErrorEvent(orc_ReceivedService);
            break;
         case mhu8_OSY_SI_READ_SERIAL_NUMBER:
            m_LogErrorWithHeader("Asynchronous communication",
                                 "Received read serial number response (possibly startup message). Ignoring.",
                                 TGL_UTIL_FUNC_ID);
            break;
         default:
            //this is nothing we can handle; report in log
            m_LogErrorWithHeader("Asynchronous communication", "Unexpectedly received negative response to service ID 0x" +
                                 C_SCLString::IntToHex(orc_ReceivedService.c_Data[1],
                                                       2) + ". Ignoring.", TGL_UTIL_FUNC_ID);

            break;
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle incoming async response of service OsyReadDataPoolDataEvent

   \param[in]  orc_ReceivedService        received service suspected to be an async response

   \return
   C_NO_ERR     Async message received successfully
   C_RANGE      Received message is not complete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::m_HandleAsyncOsyReadDataPoolDataEvent(
   const C_OSCProtocolDriverOsyService & orc_ReceivedService)
{
   sint32 s32_Return = C_RANGE;

   if (orc_ReceivedService.c_Data.size() > 3)
   {
      uint8 u8_DataPoolIndex;
      uint16 u16_ListIndex;
      uint16 u16_ElementIndex;
      const uint16 u16_NumberOfBytes = static_cast<uint16>(orc_ReceivedService.c_Data.size()) - 4U;
      uint8 au8_Identifier[3];

      // Copy the datapool element indexes
      au8_Identifier[0] = orc_ReceivedService.c_Data[1];
      au8_Identifier[1] = orc_ReceivedService.c_Data[2];
      au8_Identifier[2] = orc_ReceivedService.c_Data[3];

      // Copy the received value of the datapool element
      std::vector<uint8> c_Value;
      c_Value.resize(u16_NumberOfBytes);
      (void)std::memcpy(&c_Value[0], &orc_ReceivedService.c_Data[4], u16_NumberOfBytes);

      // Get the indexes of all parameter
      this->m_UnpackDataPoolIdentifier(au8_Identifier, u8_DataPoolIndex, u16_ListIndex, u16_ElementIndex);

      m_OsyReadDataPoolDataEventReceived(u8_DataPoolIndex, u16_ListIndex, u16_ElementIndex, c_Value);

      s32_Return = C_NO_ERR;
   }
   else
   {
      m_LogErrorWithHeader("Asynchronous communication",
                           "Received async ReadDataPoolDataEvent with invalid length. Ignoring.", TGL_UTIL_FUNC_ID);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle incoming async error response of service OsyReadDataPoolDataEvent

   Check the negative response only if the additional information with the datapool index is
   available.
   If a negative response was received without the indexes and this service, it was caused by the
   protocol mechanisms.

   \param[in]  orc_ReceivedService        received service suspected to be an async response

   \return
   C_NO_ERR     Async message received successfully
   C_RANGE      Received message is not complete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::m_HandleAsyncOsyReadDataPoolDataErrorEvent(
   const C_OSCProtocolDriverOsyService & orc_ReceivedService)
{
   sint32 s32_Return = C_RANGE;

   if (orc_ReceivedService.c_Data.size() == 6)
   {
      uint8 u8_DataPoolIndex;
      uint16 u16_ListIndex;
      uint16 u16_ElementIndex;
      uint8 au8_Identifier[3];

      // Copy the datapool element indexes
      au8_Identifier[0] = orc_ReceivedService.c_Data[3];
      au8_Identifier[1] = orc_ReceivedService.c_Data[4];
      au8_Identifier[2] = orc_ReceivedService.c_Data[5];

      // Get the indexes of all parameter
      this->m_UnpackDataPoolIdentifier(au8_Identifier, u8_DataPoolIndex, u16_ListIndex, u16_ElementIndex);

      m_OsyReadDataPoolDataEventErrorReceived(u8_DataPoolIndex, u16_ListIndex, u16_ElementIndex,
                                              orc_ReceivedService.c_Data[2]);

      s32_Return = C_NO_ERR;
   }
   else
   {
      m_LogErrorWithHeader("Asynchronous communication",
                           "Received async ReadDataPoolDataErrorEvent with invalid length. Ignoring.",
                           TGL_UTIL_FUNC_ID);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle incoming async response of service OsyTunnelCanMessagesEvent

   \param[in]  orc_ReceivedService        received service suspected to be an async response

   \return
   C_NO_ERR     Async message received successfully
   C_RANGE      - Received message is not complete
                - 11 Bit identifier is not valid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::m_HandleAsyncOsyTunnelCanMessagesEvent(
   const C_OSCProtocolDriverOsyService & orc_ReceivedService)
{
   sint32 s32_Return = C_RANGE;

   if (orc_ReceivedService.c_Data.size() == 15U)
   {
      stw_can::T_STWCAN_Msg_RX c_CanMessage;

      // Bit 31 of CAN id is the flag for 29 bit or 11 bit decision
      c_CanMessage.u8_XTD = ((orc_ReceivedService.c_Data[1] & 0x80U) == 0x80U) ? 1U : 0U;
      // Bits 30 and 29 are reserved
      c_CanMessage.u32_ID = ((static_cast<uint32>(orc_ReceivedService.c_Data[1]) & 0x1FU) << 24) +
                            (static_cast<uint32>(orc_ReceivedService.c_Data[2]) << 16) +
                            (static_cast<uint32>(orc_ReceivedService.c_Data[3]) << 8) +
                            static_cast<uint32>(orc_ReceivedService.c_Data[4]);

      // 11 bit identifier range check
      if ((c_CanMessage.u8_XTD == 1U) || (c_CanMessage.u32_ID <= 0x7FFU))
      {
         // RTR is not supported
         c_CanMessage.u8_RTR = 0U;

         // Fill the struct
         c_CanMessage.u8_DLC = orc_ReceivedService.c_Data[5];
         //lint -e{420} //std::vector reference returned by [] is linear; we checked for enough memory
         (void)std::memcpy(&c_CanMessage.au8_Data[0], &orc_ReceivedService.c_Data[6], 8U);
         c_CanMessage.u64_TimeStamp = stw_tgl::TGL_GetTickCountUS();

         if (this->mpr_OnOsyTunnelCanMessageReceived != NULL)
         {
            this->mpr_OnOsyTunnelCanMessageReceived(this->mpv_OnAsyncTunnelCanMessageInstance,
                                                    orc_ReceivedService.c_Data[14], c_CanMessage);
         }
         else
         {
            m_LogErrorWithHeader("Asynchronous communication",
                                 "Received async ReadDataPoolDataErrorEvent with no handler. Ignoring.",
                                 TGL_UTIL_FUNC_ID);
         }

         s32_Return = C_NO_ERR;
      }
   }
   else
   {
      m_LogErrorWithHeader("Asynchronous communication",
                           "Received async TunnelCanMessagesEvent with invalid length. Ignoring.", TGL_UTIL_FUNC_ID);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert variable to necessary bytes

   \param[in]  ou32_Variable Input variable
   \param[out] orc_Bytes     Necessary bytes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsy::mh_ConvertVariableToNecessaryBytes(const uint32 ou32_Variable,
                                                                std::vector<uint8> & orc_Bytes)
{
   if (ou32_Variable <= 0xFFUL)
   {
      orc_Bytes.resize(1);
      orc_Bytes[0] = static_cast<uint8>(ou32_Variable);
   }
   else if (ou32_Variable <= 0xFFFFUL)
   {
      orc_Bytes.resize(2);
      orc_Bytes[0] = static_cast<uint8>(ou32_Variable >> 8U);
      orc_Bytes[1] = static_cast<uint8>(ou32_Variable & 0xFFU);
   }
   else if (ou32_Variable <= 0xFFFFFFUL)
   {
      orc_Bytes.resize(3);
      orc_Bytes[0] = static_cast<uint8>(ou32_Variable >> 16U);
      orc_Bytes[1] = static_cast<uint8>(ou32_Variable >> 8U);
      orc_Bytes[2] = static_cast<uint8>(ou32_Variable & 0xFFU);
   }
   else
   {
      orc_Bytes.resize(4);
      orc_Bytes[0] = static_cast<uint8>(ou32_Variable >> 24U);
      orc_Bytes[1] = static_cast<uint8>(ou32_Variable >> 16U);
      orc_Bytes[2] = static_cast<uint8>(ou32_Variable >> 8U);
      orc_Bytes[3] = static_cast<uint8>(ou32_Variable & 0xFFU);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Request Download service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Initiate a programming sequence. Must be called before service TransferData.
   Address and memory length is fixed to 4bytes each.
   Data format is set to 0x00 (no encryption and no compression on transferred data).

   \param[in]  ou32_StartAddress     first address in flash to write to
   \param[in]  ou32_Size             number of bytes to be written to flash memory
   \param[out] oru32_MaxBlockLength  maximum number of data bytes for a single transfer data request message
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: incorrect response length)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyRequestDownload(const uint32 ou32_StartAddress, const uint32 ou32_Size,
                                                  uint32 & oru32_MaxBlockLength, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Request;
   C_OSCProtocolDriverOsyService c_Response;
   uint8 u8_NrErrorCode = 0U;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      c_Request.c_Data.resize(11);
      c_Request.c_Data[0] = mhu8_OSY_SI_REQUEST_DOWNLOAD;
      c_Request.c_Data[1] = 0x00U;
      c_Request.c_Data[2] = 0x44U; // length of 4bytes for address and size
      c_Request.c_Data[3] = static_cast<uint8>(ou32_StartAddress >> 24U);
      c_Request.c_Data[4] = static_cast<uint8>(ou32_StartAddress >> 16U);
      c_Request.c_Data[5] = static_cast<uint8>(ou32_StartAddress >> 8U);
      c_Request.c_Data[6] = static_cast<uint8>(ou32_StartAddress & 0xFFU);
      c_Request.c_Data[7] = static_cast<uint8>(ou32_Size >> 24U);
      c_Request.c_Data[8] = static_cast<uint8>(ou32_Size >> 16U);
      c_Request.c_Data[9] = static_cast<uint8>(ou32_Size >> 8U);
      c_Request.c_Data[10] = static_cast<uint8>(ou32_Size & 0xFFU);

      s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
      else
      {
         uint8 u8_LengthFormat;
         s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_REQUEST_DOWNLOAD, 3U, c_Response,
                                                       u8_NrErrorCode, false);

         switch (s32_Return)
         {
         case C_NO_ERR:
            //check response:
            u8_LengthFormat = c_Response.c_Data[1] >> 4U;
            if (((u8_LengthFormat == 0U) || (u8_LengthFormat > 4U)) ||
                (c_Response.c_Data.size() != (2U + static_cast<uintn>(u8_LengthFormat))))
            {
               s32_Return = C_RD_WR;
            }
            else
            {
               uint8 u8_Index;
               oru32_MaxBlockLength = 0U;
               for (u8_Index = 0U; u8_Index < u8_LengthFormat; u8_Index++)
               {
                  oru32_MaxBlockLength += (static_cast<uint32>(c_Response.c_Data[2U + static_cast<uintn>(u8_Index)]) <<
                                           (((u8_LengthFormat - 1U) - u8_Index) * 8U));
               }
            }
            break;
         case C_WARN:
            if (opu8_NrCode != NULL)
            {
               (*opu8_NrCode) = u8_NrErrorCode;
            }
            break;
         default:
            break;
         }
      }
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RequestDownload(Address:%d, Size:%d, MaxBlockLength:%d)", ou32_StartAddress,
                                 ou32_Size, oru32_MaxBlockLength);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Request File Transfer service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Initiate a file transfer sequence. Must be called before service TransferData.
   Size is fixed to 4bytes.
   Mode of operation is set to 0x03 ("replace file")
   Data format is set to 0x00 (no encryption and no compression of transferred data).

   \param[in]  orc_FilePath          file path to send
   \param[in]  ou32_FileSize         number of bytes to be written
   \param[out] oru32_MaxBlockLength  maximum number of data bytes for a single transfer data request message
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_RANGE    file path too long (theoretical maximum: 0xFFFF characters)
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response
               (here: incorrect response length, mode of operation, data format)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyRequestFileTransfer(const C_SCLString & orc_FilePath, const uint32 ou32_FileSize,
                                                      uint32 & oru32_MaxBlockLength, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Request;
   C_OSCProtocolDriverOsyService c_Response;
   uint8 u8_NrErrorCode = 0U;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else if (orc_FilePath.Length() > 0xFFFF)
   {
      s32_Return = C_RANGE;
   }
   else
   {
      const uint16 u16_PathLength = static_cast<uint16>(orc_FilePath.Length());

      c_Request.c_Data.resize(10 + static_cast<uintn>(u16_PathLength));
      c_Request.c_Data[0U] = mhu8_OSY_SI_REQUEST_FILE_TRANSFER;
      c_Request.c_Data[1U] = 0x03U; // "replace file"
      c_Request.c_Data[2U] = static_cast<uint8>(u16_PathLength >> 8U);
      c_Request.c_Data[3U] = static_cast<uint8>(u16_PathLength);
      (void)std::memcpy(&c_Request.c_Data[4], orc_FilePath.c_str(), u16_PathLength);
      c_Request.c_Data[4 + static_cast<uintn>(u16_PathLength)] = 0x00U; //no compression; no encryption
      c_Request.c_Data[5 + static_cast<uintn>(u16_PathLength)] = 0x04U; //4 bytes with address information
      c_Request.c_Data[6 + static_cast<uintn>(u16_PathLength)] = static_cast<uint8>(ou32_FileSize >> 24U);
      c_Request.c_Data[7 + static_cast<uintn>(u16_PathLength)] = static_cast<uint8>(ou32_FileSize >> 16U);
      c_Request.c_Data[8 + static_cast<uintn>(u16_PathLength)] = static_cast<uint8>(ou32_FileSize >> 8U);
      c_Request.c_Data[9 + static_cast<uintn>(u16_PathLength)] = static_cast<uint8>(ou32_FileSize);

      s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
      else
      {
         uint8 u8_LengthFormat;
         s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_REQUEST_FILE_TRANSFER, 4U, c_Response,
                                                       u8_NrErrorCode, false);
         switch (s32_Return)
         {
         case C_NO_ERR:
            //check response:
            u8_LengthFormat = c_Response.c_Data[2];
            if ((c_Response.c_Data[1] != 0x03U) ||                                          //mode of operation OK ?
                ((u8_LengthFormat == 0U) || (u8_LengthFormat > 4U)) ||                      //valid length format ?
                (c_Response.c_Data.size() != (4U + static_cast<uintn>(u8_LengthFormat))) || //enough data bytes ?
                (c_Response.c_Data[static_cast<uintn>(u8_LengthFormat) + 3U] != 0x00U))     //data format OK ?
            {
               s32_Return = C_RD_WR;
            }
            else
            {
               uint8 u8_Index;
               oru32_MaxBlockLength = 0U;
               for (u8_Index = 0U; u8_Index < u8_LengthFormat; u8_Index++)
               {
                  oru32_MaxBlockLength += (static_cast<uint32>(c_Response.c_Data[3U + static_cast<uintn>(u8_Index)]) <<
                                           (((u8_LengthFormat - 1U) - u8_Index) * 8U));
               }
            }
            break;
         case C_WARN:
            if (opu8_NrCode != NULL)
            {
               (*opu8_NrCode) = u8_NrErrorCode;
            }
            break;
         default:
            break;
         }
      }
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RequestFileTransfer(Path:%s, Size:%d, MaxBlockLength:%d)", orc_FilePath.c_str(),
                                 ou32_FileSize, oru32_MaxBlockLength);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transfer Data service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Send data in ascending order to be programmed to flash memory starting at address set
   by service RequestDownload.
   The maximum number of bytes transferred is limited by the buffer size provided by the
   response message of service RequestDownload.

   \param[in]  ou8_BlockSequenceCounter   sequence counter 0-255; first black starts with 1
   \param[in]  orc_Data                   data to be programmed to flash memory
   \param[out] opu8_NrCode                if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyTransferData(const uint8 ou8_BlockSequenceCounter, std::vector<uint8> & orc_Data,
                                               uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   uint8 u8_NrErrorCode = 0U;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      const uint16 u16_NumberOfBytes = static_cast<uint16>(orc_Data.size());
      C_OSCProtocolDriverOsyService c_Request;
      c_Request.c_Data.resize(static_cast<uintn>(u16_NumberOfBytes) + 2U);
      c_Request.c_Data[0] = mhu8_OSY_SI_TRANSFER_DATA;
      c_Request.c_Data[1] = ou8_BlockSequenceCounter;
      (void)std::memcpy(&c_Request.c_Data[2], &orc_Data[0], u16_NumberOfBytes);

      s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
      else
      {
         C_OSCProtocolDriverOsyService c_Response;
         s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_TRANSFER_DATA, 2U, c_Response, u8_NrErrorCode, true);
         switch (s32_Return)
         {
         case C_NO_ERR:
            //check response:
            if (c_Response.c_Data[1] != ou8_BlockSequenceCounter)
            {
               s32_Return = C_RD_WR;
            }
            break;
         case C_WARN:
            if (opu8_NrCode != NULL)
            {
               (*opu8_NrCode) = u8_NrErrorCode;
            }
            break;
         default:
            break;
         }
      }
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("TransferData(Sequence:%d, Size:%d)", ou8_BlockSequenceCounter, orc_Data.size());
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Request Transfer Exit service implementation based on address

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Finalize a programming sequence. Must be called after last service TransferData.
   The signature block must be located in the previously programmed range of flash memory.

   \param[in]  oq_SendSignatureBlockAddress  true: it's the last area of a block; send the signature address
                                             false: more areas will follow; do not send the signature address
   \param[in]  ou32_SignatureBlockAddress    address of signature block in flash covering the application/data
                                             previously programmed by service TransferData.
                                             (only used if oq_SendSignatureBlockAddress is true)
   \param[out] opu8_NrCode                   if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyRequestTransferExitAddressBased(const bool oq_SendSignatureBlockAddress,
                                                                  const uint32 ou32_SignatureBlockAddress,
                                                                  uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Request;
   C_OSCProtocolDriverOsyService c_Response;
   uint8 u8_NrErrorCode = 0U;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      if (oq_SendSignatureBlockAddress == true)
      {
         c_Request.c_Data.resize(5);
         c_Request.c_Data[0] = mhu8_OSY_SI_REQUEST_TRANSFER_EXIT;
         c_Request.c_Data[1] = static_cast<uint8>(ou32_SignatureBlockAddress >> 24U);
         c_Request.c_Data[2] = static_cast<uint8>(ou32_SignatureBlockAddress >> 16U);
         c_Request.c_Data[3] = static_cast<uint8>(ou32_SignatureBlockAddress >> 8U);
         c_Request.c_Data[4] = static_cast<uint8>(ou32_SignatureBlockAddress & 0xFFU);
      }
      else
      {
         c_Request.c_Data.resize(1);
         c_Request.c_Data[0] = mhu8_OSY_SI_REQUEST_TRANSFER_EXIT;
      }

      s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
      else
      {
         s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_REQUEST_TRANSFER_EXIT, 1U, c_Response,
                                                       u8_NrErrorCode, true);
         switch (s32_Return)
         {
         case C_NO_ERR:
            // check response:
            // nothing to do here
            break;
         case C_WARN:
            if (opu8_NrCode != NULL)
            {
               (*opu8_NrCode) = u8_NrErrorCode;
            }
            break;
         default:
            break;
         }
      }
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RequestTransferExitAddressBased(WithSignatureAddress:%d, SignatureAddress:%d)",
                                 (oq_SendSignatureBlockAddress == true) ? 1 : 0, ou32_SignatureBlockAddress);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Request Transfer Exit service implementation based on file system

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Finalize a programming sequence. Must be called after last service TransferData.

   \param[in]  orau8_Signature               calculated signature
                                             previously programmed by service TransferData.
   \param[out] opu8_NrCode                   if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyRequestTransferExitFileBased(const uint8 (&orau8_Signature)[8],
                                                               uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Request;
   C_OSCProtocolDriverOsyService c_Response;
   uint8 u8_NrErrorCode = 0U;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      uint32 u32_Counter;
      c_Request.c_Data.resize(9);
      c_Request.c_Data[0] = mhu8_OSY_SI_REQUEST_TRANSFER_EXIT;

      for (u32_Counter = 0U; u32_Counter < 8U; ++u32_Counter)
      {
         c_Request.c_Data[static_cast<uintn>(u32_Counter) + 1U] = orau8_Signature[u32_Counter];
      }

      s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
      else
      {
         s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_REQUEST_TRANSFER_EXIT, 1U, c_Response,
                                                       u8_NrErrorCode, true);
         switch (s32_Return)
         {
         case C_NO_ERR:
            // check response:
            // nothing to do here
            break;
         case C_WARN:
            if (opu8_NrCode != NULL)
            {
               (*opu8_NrCode) = u8_NrErrorCode;
            }
            break;
         default:
            break;
         }
      }
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RequestTransferExitFileBased(Signature:[%d,%d,%d,%d,%d,%d,%d,%d]",
                                 orau8_Signature[0], orau8_Signature[1], orau8_Signature[2], orau8_Signature[3],
                                 orau8_Signature[4], orau8_Signature[5], orau8_Signature[6], orau8_Signature[7]);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read NVM memory by address

   As the openSYDE protocol can only handle transfers up to 4kB bigger packages are split up into multiple
    services (each transferring (C_OSCProtocolDriverOsyTpBase::hu16_OSY_MAXIMUM_SERVICE_SIZE - 10) bytes).

   \param[in]     ou32_MemoryAddress NVM memory address to read (first read byte)
   \param[in,out] orc_DataRecord     in: size defines number of bytes to read
                                     out: Read data bytes
   \param[out]    opu8_NrCode        if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request(s) sent, positive response(s) received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RANGE    size of orc_DataRecord is zero
   C_COM      expected server response not received because of communication error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadMemoryByAddress(const uint32 ou32_MemoryAddress,
                                                      std::vector<uint8> & orc_DataRecord, uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;
   uint8 u8_NrErrorCode = 0U;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      //worst case protocol overhead:
      //1 byte service ID
      //1 byte FormatIdentifier
      //4 bytes address
      //4 bytes size
      const uint32 u32_BlockSize = (C_OSCProtocolDriverOsyTpBase::hu16_OSY_MAXIMUM_SERVICE_SIZE - 10U);

      //split up into smaller blocks:
      for (uint32 u32_ReadIndex = 0U; u32_ReadIndex < orc_DataRecord.size(); u32_ReadIndex += u32_BlockSize)
      {
         C_OSCProtocolDriverOsyService c_Request;
         std::vector<uint8> c_MemoryAddress;
         std::vector<uint8> c_MemorySize;
         uint8 u8_MemoryAddressByteCount;
         uint8 u8_MemorySizeByteCount;
         const uint32 u32_MemoryAddress = ou32_MemoryAddress + u32_ReadIndex;
         uint32 u32_Size = (orc_DataRecord.size() - u32_ReadIndex);
         if (u32_Size > u32_BlockSize)
         {
            u32_Size = u32_BlockSize;
         }

         //Convert to bytes
         C_OSCProtocolDriverOsy::mh_ConvertVariableToNecessaryBytes(u32_MemoryAddress, c_MemoryAddress);
         C_OSCProtocolDriverOsy::mh_ConvertVariableToNecessaryBytes(u32_Size, c_MemorySize);

         //Assign helper variables
         u8_MemoryAddressByteCount = static_cast<uint8>(c_MemoryAddress.size());
         u8_MemorySizeByteCount = static_cast<uint8>(c_MemorySize.size());

         //Request
         c_Request.c_Data.resize(2U + static_cast<uintn>(static_cast<uintn>(u8_MemoryAddressByteCount) +
                                                         static_cast<uintn>(u8_MemorySizeByteCount)), 0U);
         c_Request.c_Data[0] = mhu8_OSY_SI_READ_MEMORY_BY_ADDRESS;
         c_Request.c_Data[1] = (static_cast<uint8>(u8_MemorySizeByteCount << 4U)) + u8_MemoryAddressByteCount;
         //Address part
         (void)std::memcpy(&c_Request.c_Data[2], &c_MemoryAddress[0], c_MemoryAddress.size());
         //Size part
         (void)std::memcpy(&c_Request.c_Data[static_cast<size_t>(2) + u8_MemoryAddressByteCount], &c_MemorySize[0],
                           c_MemorySize.size());

         s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
         if (s32_Return != C_NO_ERR)
         {
            s32_Return = C_NOACT;
         }
         else
         {
            C_OSCProtocolDriverOsyService c_Response;
            s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_READ_MEMORY_BY_ADDRESS,
                                                          static_cast<uint16>(1UL + u32_Size), c_Response,
                                                          u8_NrErrorCode, true);
            switch (s32_Return)
            {
            case C_NO_ERR:
               // size is OK: copy data
               (void)std::memcpy(&orc_DataRecord[u32_ReadIndex], &c_Response.c_Data[1], c_Response.c_Data.size() - 1U);
               break;
            case C_WARN:
               if (opu8_NrCode != NULL)
               {
                  (*opu8_NrCode) = u8_NrErrorCode;
               }
               break;
            default:
               break;
            }
         }
         if (s32_Return != C_NO_ERR)
         {
            break;
         }
      }
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("ReadMemoryByAddress(Address:%d Size:%d)", ou32_MemoryAddress, orc_DataRecord.size());
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write NVM memory by address

   The maximum size of transfers the openSYDE protocol can handle in one package is 4kB.
   This can be further limited by the server's capabilities.
   A client can check for this limit using OsyReadMaxNumberOfBlockLength().

   This function uses "mu16_MaxServiceSize" to split the individual write services into suitable chunks.

   \param[in]  ou32_MemoryAddress   NVM memory address to write to (first written byte)
   \param[in]  orc_DataRecord       Data bytes to write
   \param[out] opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: unexpected length or address)
   C_RANGE    orc_DataRecord empty
   C_COM      expected server response not received because of communication error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyWriteMemoryByAddress(const uint32 ou32_MemoryAddress,
                                                       const std::vector<uint8> & orc_DataRecord,
                                                       uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;
   uint8 u8_NrErrorCode = 0U;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      //worst case protocol overhead:
      //1 byte service ID
      //1 byte FormatIdentifier
      //4 bytes address
      //4 bytes size
      const uint32 u32_BlockSize = static_cast<uint32>(mu16_MaxServiceSize) - 10U;

      //split up into smaller blocks:
      for (uint32 u32_WriteIndex = 0U; u32_WriteIndex < orc_DataRecord.size(); u32_WriteIndex += u32_BlockSize)
      {
         C_OSCProtocolDriverOsyService c_Request;
         std::vector<uint8> c_MemoryAddress;
         std::vector<uint8> c_MemorySize;
         uint8 u8_MemoryAddressByteCount;
         uint8 u8_MemorySizeByteCount;
         const uint32 u32_MemoryAddress = ou32_MemoryAddress + u32_WriteIndex;
         uint32 u32_Size = (orc_DataRecord.size() - u32_WriteIndex);
         if (u32_Size > u32_BlockSize)
         {
            u32_Size = u32_BlockSize;
         }

         //Convert to bytes
         C_OSCProtocolDriverOsy::mh_ConvertVariableToNecessaryBytes(u32_MemoryAddress, c_MemoryAddress);
         C_OSCProtocolDriverOsy::mh_ConvertVariableToNecessaryBytes(u32_Size, c_MemorySize);

         //Assign helper variables
         u8_MemoryAddressByteCount = static_cast<uint8>(c_MemoryAddress.size());
         u8_MemorySizeByteCount = static_cast<uint8>(c_MemorySize.size());

         //Request
         c_Request.c_Data.resize(2U + (((static_cast<uintn>(u8_MemoryAddressByteCount) +
                                         static_cast<uintn>(u8_MemorySizeByteCount)) +
                                        static_cast<uintn>(u32_Size))), 0U);
         c_Request.c_Data[0] = mhu8_OSY_SI_WRITE_MEMORY_BY_ADDRESS;
         c_Request.c_Data[1] = static_cast<uint8>(u8_MemorySizeByteCount << 4U) + u8_MemoryAddressByteCount;
         //Address part
         (void)std::memcpy(&c_Request.c_Data[2], &c_MemoryAddress[0], c_MemoryAddress.size());
         //Size part
         (void)std::memcpy(&c_Request.c_Data[static_cast<size_t>(2) + u8_MemoryAddressByteCount], &c_MemorySize[0],
                           c_MemorySize.size());
         //Data part
         //lint -e{669,670} //std::vector reference returned by [] is guaranteed to have linear data in memory
         (void)std::memcpy(
            &c_Request.c_Data[static_cast<size_t>(2) + u8_MemoryAddressByteCount + u8_MemorySizeByteCount],
            &orc_DataRecord[u32_WriteIndex], u32_Size);

         s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
         if (s32_Return != C_NO_ERR)
         {
            s32_Return = C_NOACT;
         }
         else
         {
            C_OSCProtocolDriverOsyService c_Response;
            sintn sn_Diff;
            const uint8 u8_ExpectedSize = 2U + u8_MemoryAddressByteCount + u8_MemorySizeByteCount;

            s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_WRITE_MEMORY_BY_ADDRESS, u8_ExpectedSize,
                                                          c_Response, u8_NrErrorCode, true);
            switch (s32_Return)
            {
            case C_NO_ERR:
               // format, size, address OK ?
               sn_Diff = std::memcmp(&c_Request.c_Data[1], &c_Response.c_Data[1],
                                     static_cast<size_t>(u8_ExpectedSize) - 1);
               if (sn_Diff != 0)
               {
                  //unexpected response
                  s32_Return = C_RD_WR;
               }
               break;
            case C_WARN:
               if (opu8_NrCode != NULL)
               {
                  (*opu8_NrCode) = u8_NrErrorCode;
               }
               break;
            default:
               break;
            }
         }
         if (s32_Return != C_NO_ERR)
         {
            break;
         }
      }
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("WriteMemoryByAddress(Address:%d Size:%d)", ou32_MemoryAddress, orc_DataRecord.size());
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Notify server of changes in NVM

   \param[in]  ou8_DataPoolIndex           Data pool index
   \param[in]  ou8_ListIndex               List index
   \param[out] orq_ApplicationAcknowledge  true: positive acknowledge from server
                                           false: negative acknowledge from server
   \param[out] opu8_NrCode                 if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data pool or list index)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyNotifyNvmDataChanges(const uint8 ou8_DataPoolIndex, const uint8 ou8_ListIndex,
                                                       bool & orq_ApplicationAcknowledge, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   uint8 u8_NrErrorCode = 0U;

   std::vector<uint8> c_SendData;
   std::vector<uint8> c_ReceiveData;
   c_SendData.resize(2);
   c_SendData[0] = ou8_DataPoolIndex;
   c_SendData[1] = ou8_ListIndex;
   s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_NOTIFY_NVM_DATA_CHANGED, mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE,
                                 c_SendData, 3, true, c_ReceiveData, u8_NrErrorCode);
   if (s32_Return == C_NO_ERR)
   {
      if ((c_ReceiveData[0] == ou8_DataPoolIndex) && (c_ReceiveData[1] == ou8_ListIndex))
      {
         orq_ApplicationAcknowledge = ((c_ReceiveData[2] & 0x01U) == 0x01U) ? false : true;
      }
      else
      {
         s32_Return = C_RD_WR;
      }
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RoutineControl::NotifyNvmDataChanges(Datapool:%d List:%d)", ou8_DataPoolIndex,
                                 ou8_ListIndex);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Tester Present service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Send the tester present service to indicate to the server that the client is still active.
   Sub-function byte is always zero.
   Usually the suppressPosRspMsgIndicationBit may be set to avoid flooding the bus with response messages.

   \param[in]  ou8_SuppressResponseMsg    0: server shall send a response message
                                          1: server shall send no response message
   \param[out] opu8_NrCode                if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyTesterPresent(const uint8 ou8_SuppressResponseMsg, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Request;
   C_OSCProtocolDriverOsyService c_Response;
   uint8 u8_NrErrorCode = 0U;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      c_Request.c_Data.resize(2);
      c_Request.c_Data[0] = mhu8_OSY_SI_TESTER_PRESENT;
      c_Request.c_Data[1] = static_cast<uint8>(ou8_SuppressResponseMsg << 7U);

      s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
      else
      {
         // response message expected
         if (ou8_SuppressResponseMsg == 0U)
         {
            s32_Return = m_PollForSpecificServiceResponse(mhu8_OSY_SI_TESTER_PRESENT, 2U, c_Response,
                                                          u8_NrErrorCode, true);

            switch (s32_Return)
            {
            case C_NO_ERR:
               //check response:
               if (((c_Request.c_Data[1] & 0x7FU) != c_Response.c_Data[1]))
               {
                  s32_Return = C_RD_WR;
               }
               break;
            case C_WARN:
               if (opu8_NrCode != NULL)
               {
                  (*opu8_NrCode) = u8_NrErrorCode;
               }
               break;
            default:
               break;
            }
         }
         // no response message expected, only send single frame message
         else
         {
            s32_Return = mpc_TransportProtocol->Cycle();
            if (opu8_NrCode != NULL)
            {
               (*opu8_NrCode) = 0U;
            }
         }
      }
   }
   if (s32_Return != C_NO_ERR)
   {
      m_LogServiceError("TesterPresent", s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ECU Reset service implementation

   Send request.
   See class description for general handling of "polled" services.

   Initiate a reset on the server. Sub-function means reset type.
   Supported reset types:
   - 0x02: keyOffOnReset
   - 0x60: resetToFlashloader
   No response from server is expected.

   The function sends only request. No waiting for a possible error response.

   \param[in]  ou8_ResetType        Reset type (Default by openSYDE protocol: 0x02: keyOffOnReset)

   \return
   C_NO_ERR   request sent
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyEcuReset(const uint8 ou8_ResetType)
{
   sint32 s32_Return;
   C_OSCProtocolDriverOsyService c_Request;

   if (mpc_TransportProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      c_Request.c_Data.resize(2);
      c_Request.c_Data[0] = mhu8_OSY_SI_ECU_RESET;
      c_Request.c_Data[1] = ou8_ResetType;

      s32_Return = mpc_TransportProtocol->SendRequest(c_Request);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_NOACT;
      }
      else
      {
         s32_Return = mpc_TransportProtocol->Cycle();
      }
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("EcuReset(Type:%d List:%d)", ou8_ResetType);
      m_LogServiceError(c_ErrorText, s32_Return, 0);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   SetNodeIdForChannel service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Request the server to reconfigure the settings for node id and bus id for the provided channel.

   Send format: 2 bytes channel identifier, 2 bytes new node identifier
   Example: (0x00, 0x03, 0x01, 0x09) -> , type:CAN, channel:3, bus id:1, node id:9

   \param[in]  ou8_ChannelType       selected channel type (0 equals CAN, 1 equals Ethernet)
   \param[in]  ou8_ChannelIndex      selected channel index
   \param[in]  orc_NewNodeId         new bus id (0..15) and node id (0..126) configuration for the server
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_RANGE    BusId or NodeId out of range
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsySetNodeIdForChannel(const uint8 ou8_ChannelType, const uint8 ou8_ChannelIndex,
                                                      const C_OSCProtocolDriverOsyNode & orc_NewNodeId,
                                                      uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;
   uint8 u8_NrErrorCode = 0U;

   if ((orc_NewNodeId.u8_BusIdentifier <= C_OSCProtocolDriverOsyNode::mhu8_MAX_BUS) &&
       (orc_NewNodeId.u8_NodeIdentifier < C_OSCProtocolDriverOsyNode::mhu8_MAX_NODE))
   {
      std::vector<uint8> c_ReceiveData;
      std::vector<uint8> c_SendData;

      c_SendData.resize(4);
      c_SendData[0] = ou8_ChannelType;
      c_SendData[1] = ou8_ChannelIndex;
      c_SendData[2] = orc_NewNodeId.u8_BusIdentifier;
      c_SendData[3] = orc_NewNodeId.u8_NodeIdentifier;

      s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_SET_NODE_ID_FOR_CHANNEL, mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE,
                                    c_SendData, 0U, true, c_ReceiveData, u8_NrErrorCode);
   }

   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RoutineControl::SetNodeIdForChannel(Type:%d Index:%d BusId:%d NodeId:%d)",
                                 ou8_ChannelType, ou8_ChannelIndex, orc_NewNodeId.u8_BusIdentifier,
                                 orc_NewNodeId.u8_NodeIdentifier);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   SetBitrate service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Request the server to reconfigure the setting for bitrate for the provided channel.

   Send format:  2 bytes channel identifier, 4 bytes new bitrate
   Example: (0x00, 0x03, 0x00, 0x0F, 0x42, 0x40) -> , type:CAN, channel:3, bitrate:1000000

   \param[in]  ou8_ChannelType       selected channel type (0 equals CAN, 1 equals Ethernet)
   \param[in]  ou8_ChannelIndex      selected channel index
   \param[in]  ou32_Bitrate          new bitrate configuration for the server in bit/s
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsySetBitrate(const uint8 ou8_ChannelType, const uint8 ou8_ChannelIndex,
                                             const uint32 ou32_Bitrate, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_ReceiveData;
   std::vector<uint8> c_SendData;
   uint8 u8_NrErrorCode = 0U;

   c_SendData.resize(6);
   c_SendData[0] = ou8_ChannelType;
   c_SendData[1] = ou8_ChannelIndex;
   c_SendData[2] = static_cast<uint8>(ou32_Bitrate >> 24U);
   c_SendData[3] = static_cast<uint8>(ou32_Bitrate >> 16U);
   c_SendData[4] = static_cast<uint8>(ou32_Bitrate >> 8U);
   c_SendData[5] = static_cast<uint8>(ou32_Bitrate & 0xFFU);

   s32_Return = m_RoutineControl(mhu16_OSY_RC_RC_SID_SET_BITRATE, mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE,
                                 c_SendData, 0U, true, c_ReceiveData, u8_NrErrorCode);
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RoutineControl::SetBitrate(Type:%d Index:%d Bitrate:%d)",
                                 ou8_ChannelType, ou8_ChannelIndex, ou32_Bitrate);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   ReadFlashBlockData service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Request from server the application information for the selected block in flash memory on an address based server.
   Flash block index 0 requests information about the flashloader.

   To read out information about all flash blocks present on the server call this function in a loop starting from
    block index 0.
   Stop if the server responds with "requestOutOfRange".

   Send format: 1 byte flash block index
   Receive format:
   - 4 bytes block start address
   - 4 bytes block end address
   - 1 byte result signature check for flash block
   - 1 byte length of application version
   - max. 16 bytes application version
   - 11 bytes timestamp - date
   - 8 bytes timestamp - time (no NULL termination)
   - 1 byte length of application name
   - max. 32 byte application name
   - 1 byte length of additional information
   - max. 255 bytes additional information

   \param[in]  ou8_FlashBlock        index of selected flash block
   \param[in]  orc_BlockInfo         structure to store flash block info retrieved from server
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyReadFlashBlockData(const uint8 ou8_FlashBlock, C_FlashBlockInfo & orc_BlockInfo,
                                                     uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   uint8 u8_NrErrorCode = 0U;

   std::vector<uint8> c_ReceiveData;
   std::vector<uint8> c_SendData;
   std::vector<charn> c_Text;
   uintn un_Counter = 0U;

   c_SendData.resize(1);
   c_SendData[0] = ou8_FlashBlock;

   s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_READ_FLASH_BLOCK_DATA, mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE,
                                 c_SendData, 0U, false, c_ReceiveData, u8_NrErrorCode);

   if (s32_Return == C_NO_ERR)
   {
      //clear all information:
      orc_BlockInfo.ClearContent();

      // parse response message for information
      if (c_ReceiveData[un_Counter] == C_FlashBlockInfo::hu8_IdBlockAddresses)
      {
         //lint -e{864} //false positive due to const/non-const misinterpretation
         orc_BlockInfo.u32_BlockStartAddress = ((static_cast<uint32>(c_ReceiveData[un_Counter + 1U])) << 24U) +
                                               ((static_cast<uint32>(c_ReceiveData[un_Counter + 2U])) << 16U) +
                                               ((static_cast<uint32>(c_ReceiveData[un_Counter + 3U])) << 8U) +
                                               (static_cast<uint32>(c_ReceiveData[un_Counter + 4U]));
         //lint -e{864} //false positive due to const/non-const misinterpretation
         orc_BlockInfo.u32_BlockEndAddress = ((static_cast<uint32>(c_ReceiveData[un_Counter + 5U])) << 24U) +
                                             ((static_cast<uint32>(c_ReceiveData[un_Counter + 6U])) << 16U) +
                                             ((static_cast<uint32>(c_ReceiveData[un_Counter + 7U])) << 8U) +
                                             (static_cast<uint32>(c_ReceiveData[un_Counter + 8U]));
         un_Counter += 9U;
      }
      if (c_ReceiveData[un_Counter] == C_FlashBlockInfo::hu8_IdResultSignature)
      {
         orc_BlockInfo.u8_SignatureValid = c_ReceiveData[un_Counter + 1U];
         un_Counter += 2U;
      }
      if (c_ReceiveData[un_Counter] == C_FlashBlockInfo::hu8_IdApplicationVersion)
      {
         const uintn un_Length = static_cast<uintn>(c_ReceiveData[un_Counter + 1U]);
         c_Text.resize(un_Length + 1U);     //plus 1 for termination
         c_Text[c_Text.size() - 1U] = '\0'; //add termination
         (void)std::memcpy(&c_Text[0], &c_ReceiveData[un_Counter + 2U], un_Length);
         orc_BlockInfo.c_ApplicationVersion = &c_Text[0];
         un_Counter += un_Length + 2U;
      }
      if (c_ReceiveData[un_Counter] == C_FlashBlockInfo::hu8_IdBuildTimestamp)
      {
         c_Text.resize(11U + 1U);           //plus 1 for termination
         c_Text[c_Text.size() - 1U] = '\0'; //add termination
         //lint -e{419,420} //std::vector reference returned by [] is linear; we allocated enough memory
         (void)std::memcpy(&c_Text[0], &c_ReceiveData[un_Counter + 1U], 11U);
         orc_BlockInfo.c_BuildDate = &c_Text[0];
         un_Counter += (11U + 1U);
         c_Text.resize(8U + 1U);            //plus 1 for termination
         c_Text[c_Text.size() - 1U] = '\0'; //add termination
         //lint -e{419,420} //std::vector reference returned by [] is linear; we allocated enough memory
         (void)std::memcpy(&c_Text[0], &c_ReceiveData[un_Counter], 8U);
         orc_BlockInfo.c_BuildTime = &c_Text[0];
         un_Counter += 8U;
      }
      if (c_ReceiveData[un_Counter] == C_FlashBlockInfo::hu8_IdApplicationName)
      {
         const uintn un_Length = static_cast<uintn>(c_ReceiveData[un_Counter + 1U]);
         c_Text.resize(un_Length + 1U);     //plus 1 for termination
         c_Text[c_Text.size() - 1U] = '\0'; //add termination
         (void)std::memcpy(&c_Text[0], &c_ReceiveData[un_Counter + 2U], un_Length);
         orc_BlockInfo.c_ApplicationName = &c_Text[0];
         un_Counter += un_Length + 2U;
      }
      if (c_ReceiveData[un_Counter] == C_FlashBlockInfo::hu8_IdAdditionalInformation)
      {
         const uintn un_Length = static_cast<uintn>(c_ReceiveData[un_Counter + 1U]);
         c_Text.resize(un_Length + 1U);     //plus 1 for termination
         c_Text[c_Text.size() - 1U] = '\0'; //add termination
         (void)std::memcpy(&c_Text[0], &c_ReceiveData[un_Counter + 2U], un_Length);
         orc_BlockInfo.c_AdditionalInformation = &c_Text[0];
      }
   }
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RoutineControl::ReadFlashBlockData(Block:%d)", ou8_FlashBlock);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Request programming service implementation

   Send request and wait for response.
   See class description for general handling of "polled" services.

   Request the server to stay in flashloader after next reset.

   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyRequestProgramming(uint8 * const opu8_NrCode)
{
   sint32 s32_Return;
   uint8 u8_NrErrorCode = 0U;

   std::vector<uint8> c_ReceiveData;
   std::vector<uint8> c_SendData;

   s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_REQUEST_PROGRAMMING, mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE,
                                 c_SendData, 0U, true, c_ReceiveData, u8_NrErrorCode);
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      m_LogServiceError("RoutineControl::RequestProgramming", s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   This service is for activating/deactivating a CAN/Ethernet channel for flashloader communication use

   \param[in]  ou8_ChannelType  selected channel type (0 equals CAN, 1 equals Ethernet)
   \param[in]  ou8_ChannelIndex selected channel index
   \param[in]  oq_Activated     input parameter description
   \param[out] opu8_NrCode      if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong routine identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsyConfigureFlashloaderCommunicationChannel(const uint8 ou8_ChannelType,
                                                                           const uint8 ou8_ChannelIndex,
                                                                           const bool oq_Activated,
                                                                           uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_SendData;
   std::vector<uint8> c_ReceiveData;
   uint8 u8_NrErrorCode = 0U;

   c_SendData.resize(3);
   c_SendData[0] = ou8_ChannelType;
   c_SendData[1] = ou8_ChannelIndex;
   if (oq_Activated == true)
   {
      c_SendData[2] = 1;
   }
   else
   {
      c_SendData[2] = 0;
   }
   s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_CONFIGURE_FLASHLOADER_COMMUNICATION_CHANNEL,
                                 mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE, c_SendData, 0, true, c_ReceiveData,
                                 u8_NrErrorCode);
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RoutineControl::ConfigureFlashloaderCommunicationChannel(Type:%d Index:%d Active:%d)",
                                 ou8_ChannelType, ou8_ChannelIndex, (oq_Activated == true) ? 1 : 0);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set the ip address for the referenced channel to the given value

   \param[in]  ou8_ChannelType       selected channel type (0 equals CAN, 1 equals Ethernet)
   \param[in]  ou8_ChannelIndex      selected channel index
   \param[in]  orau8_IpAddress       IP V4 address
   \param[in]  orau8_NetMask         IP V4 net mask
   \param[in]  orau8_DefaultGateway  IP V4 default gateway
   \param[out] opu8_NrCode           if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong routine identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsy::OsySetIpAddressForChannel(const uint8 ou8_ChannelType, const uint8 ou8_ChannelIndex,
                                                         const uint8 (&orau8_IpAddress)[4],
                                                         const uint8 (&orau8_NetMask)[4],
                                                         const uint8 (&orau8_DefaultGateway)[4],
                                                         uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   std::vector<uint8> c_SendData;
   std::vector<uint8> c_ReceiveData;
   uint8 u8_NrErrorCode = 0U;

   c_SendData.resize(14);
   c_SendData[0] = ou8_ChannelType;
   c_SendData[1] = ou8_ChannelIndex;
   c_SendData[2] = orau8_IpAddress[0];
   c_SendData[3] = orau8_IpAddress[1];
   c_SendData[4] = orau8_IpAddress[2];
   c_SendData[5] = orau8_IpAddress[3];
   c_SendData[6] = orau8_NetMask[0];
   c_SendData[7] = orau8_NetMask[1];
   c_SendData[8] = orau8_NetMask[2];
   c_SendData[9] = orau8_NetMask[3];
   c_SendData[10] = orau8_DefaultGateway[0];
   c_SendData[11] = orau8_DefaultGateway[1];
   c_SendData[12] = orau8_DefaultGateway[2];
   c_SendData[13] = orau8_DefaultGateway[3];
   s32_Return = m_RoutineControl(mhu16_OSY_RC_SID_SET_IP_ADDRESS_FOR_CHANNEL, mhu8_OSY_RC_SUB_FUNCTION_START_ROUTINE,
                                 c_SendData, 0, true, c_ReceiveData, u8_NrErrorCode);
   if (opu8_NrCode != NULL)
   {
      (*opu8_NrCode) = u8_NrErrorCode;
   }
   if (s32_Return != C_NO_ERR)
   {
      C_SCLString c_ErrorText;
      c_ErrorText.PrintFormatted("RoutineControl::SetIpAddressForChannel(Type:%d Index:%d IP:[%d.%d.%d.%d] "
                                 " NetMask:[%d.%d.%d.%d] Gateway: [%d.%d.%d.%d]",
                                 ou8_ChannelType, ou8_ChannelIndex,
                                 orau8_IpAddress[0], orau8_IpAddress[1], orau8_IpAddress[2], orau8_IpAddress[3],
                                 orau8_NetMask[0], orau8_NetMask[1], orau8_NetMask[2], orau8_NetMask[3],
                                 orau8_DefaultGateway[0], orau8_DefaultGateway[1], orau8_DefaultGateway[2],
                                 orau8_DefaultGateway[3]);
      m_LogServiceError(c_ErrorText, s32_Return, u8_NrErrorCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set up class

   Initializes class elements
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCProtocolDriverOsy::C_FlashBlockInfo::C_FlashBlockInfo(void)
{
   this->ClearContent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Tear down class
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCProtocolDriverOsy::C_FlashBlockInfo::~C_FlashBlockInfo(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize members

   Set defined default values for class members
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCProtocolDriverOsy::C_FlashBlockInfo::ClearContent(void)
{
   u32_BlockStartAddress = 0U;
   u32_BlockEndAddress = 0U;
   u8_SignatureValid = 1U; //1 = invalid; to have a pessimistic default state
   c_ApplicationVersion = "";
   c_BuildDate = "";
   c_BuildTime = "";
   c_ApplicationName = "";
   c_AdditionalInformation = "";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set up class

   Initializes class elements
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCProtocolDriverOsy::C_DataPoolMetaData::C_DataPoolMetaData(void) :
   c_Name("")
{
   this->au8_Version[0] = 0;
   this->au8_Version[1] = 0;
   this->au8_Version[2] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: create textual representation of openSYDE protocol service result

   \param[in]  os32_FunctionResult   error result as returned by openSYDE protocol driver service function
   \param[in]  ou8_NrCode            negative response code received
   \param[out] opq_IsHardError       set by function if not NULL:
                                     false: service was performed but an error response was received
                                     true: all other errors

   \return
   string representation
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(const sint32 os32_FunctionResult,
                                                                     const uint8 ou8_NrCode,
                                                                     bool * const opq_IsHardError)
{
   C_SCLString c_Text;

   if (opq_IsHardError != NULL)
   {
      (*opq_IsHardError) = true;
   }

   switch (os32_FunctionResult)
   {
   case C_NO_ERR:
      c_Text = "No problem";
      break;
   case C_TIMEOUT:
      c_Text = "No response received within timeout";
      break;
   case C_COM:
      c_Text = "No response received because of communication error";
      break;
   case C_NOACT:
      c_Text = "Could not send request";
      break;
   case C_CONFIG:
      c_Text = "Misconfigured protocol stack.";
      break;
   case C_WARN:
      if (opq_IsHardError != NULL)
      {
         (*opq_IsHardError) = false;
      }
      c_Text = "Error response received (";
      switch (ou8_NrCode)
      {
      case (0x00U):
         c_Text += "positive response";
         break;
      case (C_OSCProtocolDriverOsy::hu8_NR_CODE_SERVICE_NOT_SUPPORTED):
         c_Text += "serviceNotSupported";
         break;
      case (C_OSCProtocolDriverOsy::hu8_NR_CODE_SUB_FUNCTION_NOT_SUPPORTED):
         c_Text += "sub-functionNotSupported";
         break;
      case (C_OSCProtocolDriverOsy::hu8_NR_CODE_INCORRECT_MESSAGE_LENGTH_OR_FORMAT):
         c_Text += "incorrectMessageLengthOrInvalidFormat";
         break;
      case (C_OSCProtocolDriverOsy::hu8_NR_CODE_RESPONSE_TOO_LONG):
         c_Text += "responseTooLong";
         break;
      case (C_OSCProtocolDriverOsy::hu8_NR_CODE_CONDITIONS_NOT_CORRECT):
         c_Text += "conditionsNotCorrect";
         break;
      case (C_OSCProtocolDriverOsy::hu8_NR_CODE_REQUEST_SEQUENCE_ERROR):
         c_Text += "requestSequenceError";
         break;
      case (C_OSCProtocolDriverOsy::hu8_NR_CODE_REQUEST_OUT_OF_RANGE):
         c_Text += "requestOutOfRange";
         break;
      case (C_OSCProtocolDriverOsy::hu8_NR_CODE_SECURITY_ACCESS_DENIED):
         c_Text += "securityAccessDenied";
         break;
      case (C_OSCProtocolDriverOsy::hu8_NR_CODE_INVALID_KEY):
         c_Text += "invalidKey";
         break;
      case (C_OSCProtocolDriverOsy::hu8_NR_CODE_UPLOAD_DOWNLOAD_NOT_ACCEPTED):
         c_Text += "uploadDownloadNotAccepted";
         break;
      case (C_OSCProtocolDriverOsy::hu8_NR_CODE_GENERAL_PROGRAMMING_FAILURE):
         c_Text += "generalProgrammingFailure";
         break;
      case (C_OSCProtocolDriverOsy::hu8_NR_CODE_SERVICE_NOT_SUPPORTED_IN_ACTIVE_SESSION):
         c_Text += "serviceNotSupportedInActiveSession";
         break;
      default:
         c_Text += "unknownnegativeresponsecode";
         break;
      }
      c_Text += ")";
      break;
   case C_RD_WR:
      c_Text = "Unexpected protocol response";
      break;
   default:
      c_Text = "Undefined error code";
      break;
   }
   return c_Text;
}
