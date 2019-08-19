//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       GUI communication driver for diagnostics (implementation)

   Add functionality for diagnostics to the base class:
   * drivers for accessing data pool elements ("DataDealers")
   * diagnostic protocols via openSYDE or KEFEX protocols

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"

#include "C_SyvComDriverDiag.h"
#include "C_OSCDiagProtocolOsy.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvData.h"
#include "TGLUtils.h"
#include "C_OSCLoggingHandler.h"
#include "C_Uti.h"
#include "C_GtGetText.h"
#include "C_SyvComDriverUtil.h"
#include "C_OSCCanUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_can;
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
/*! \brief   Default constructor

   Initialize all members based on view

   \param[in] ou32_ViewIndex View index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvComDriverDiag::C_SyvComDriverDiag(const uint32 ou32_ViewIndex) :
   C_OSCComDriverProtocol(),
   mu32_ViewIndex(ou32_ViewIndex),
   mpc_CanDllDispatcher(NULL),
   mpc_EthernetDispatcher(NULL)
{
   mpc_AsyncThread = new C_SyvComDriverThread(&C_SyvComDriverDiag::mh_ThreadFunc, this);

   connect(&this->mc_PollingThread, &C_SyvComPollingThreadDiag::finished, this,
           &C_SyvComDriverDiag::m_HandlePollingFinished);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvComDriverDiag::~C_SyvComDriverDiag(void)
{
   if (mpc_AsyncThread->isRunning() == true)
   {
      this->StopCycling();
   }

   delete mpc_AsyncThread;
   mpc_AsyncThread = NULL;

   if (this->mc_PollingThread.isRunning() == true)
   {
      this->mc_PollingThread.requestInterruption();

      if (this->mc_PollingThread.wait(2000U) == false)
      {
         // Not finished yet
         osc_write_log_warning("Closing diagnostic driver", "Waiting time for stopping polling thread was not enough");
      }
   }

   //let base class know we are about to die:
   this->PrepareForDestruction();

   for (uint32 u32_ItDiagProtocol = 0; u32_ItDiagProtocol < this->mc_DiagProtocols.size(); ++u32_ItDiagProtocol)
   {
      delete (this->mc_DiagProtocols[u32_ItDiagProtocol]);
      this->mc_DiagProtocols[u32_ItDiagProtocol] = NULL;
   }

   for (uint32 u32_DealerIndex = 0U; u32_DealerIndex < this->mc_DataDealers.size(); u32_DealerIndex++)
   {
      delete this->mc_DataDealers[u32_DealerIndex];
      this->mc_DataDealers[u32_DealerIndex] = NULL;
   }

   if (mpc_CanDllDispatcher != NULL)
   {
      this->mpc_CanDllDispatcher->CAN_Exit();
      this->mpc_CanDllDispatcher->DLL_Close();

      delete mpc_CanDllDispatcher;
      mpc_CanDllDispatcher = NULL;
   }

   delete this->mpc_EthernetDispatcher;
   //lint -e{1579}  no memory leak because mpc_AsyncThread is deleted here which is not detected by lint
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all members

   \return
   C_NO_ERR      Operation success
   C_NOACT       No active nodes
   C_CONFIG      Invalid system definition/view configuration
   C_RD_WR       Configured communication DLL does not exist
   C_OVERFLOW    Unknown transport protocol or unknown diagnostic server for at least one node
   C_BUSY        System view error detected
   C_COM         CAN initialization failed
   C_CHECKSUM    Internal buffer overflow detected
   C_RD_WR       Active bus index invalid
   C_RANGE       Routing configuration failed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::InitDiag(void)
{
   sint32 s32_Return;
   uint32 u32_ActiveBusIndex;

   std::vector<uint8> c_ActiveNodes;

   s32_Return = C_SyvComDriverUtil::h_GetOSCComDriverParamFromView(this->mu32_ViewIndex, u32_ActiveBusIndex,
                                                                   c_ActiveNodes, &this->mpc_CanDllDispatcher,
                                                                   &this->mpc_EthernetDispatcher);

   if (s32_Return == C_NO_ERR)
   {
      s32_Return = this->m_InitDiagNodes();
   }

   if (s32_Return == C_NO_ERR)
   {
      s32_Return = C_OSCComDriverProtocol::Init(C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst(),
                                                u32_ActiveBusIndex, c_ActiveNodes, this->mpc_CanDllDispatcher,
                                                this->mpc_EthernetDispatcher);
   }

   if (s32_Return == C_NO_ERR)
   {
      s32_Return = m_InitDiagProtocol();
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = m_InitDataDealer();

         if (s32_Return == C_NO_ERR)
         {
            this->mq_Initialized = true;
         }
      }
   }
   else if (s32_Return == C_NOACT)
   {
      this->mq_Initialized = true;
   }
   else
   {
      // Nothing to do
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets all node into diagnostic mode with necessary security access

   Steps:
   * set up required routing
   * bring server nodes into required sessions
   * activate security access level required for diagnostics

   \param[in] orc_ErrorDetails Details for current error

   \return
   C_NO_ERR    All nodes set to session successfully
   C_CONFIG    InitDiag function was not called or not successful or protocol was not initialized properly.
   C_COM       Error of service
   C_CHECKSUM  Checksum of a datapool does not match
   C_TIMEOUT   Expected response not received within timeout
   C_RD_WR     malformed protocol response
   C_WARN      error response
   C_BUSY      Connection to at least one server failed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::SetDiagnosticMode(QString & orc_ErrorDetails)
{
   sint32 s32_Return;

   this->mc_DefectNodeIndices.clear();

   s32_Return = this->m_StartRoutingDiag(orc_ErrorDetails, this->mc_DefectNodeIndices);

   // In case of a timeout, lets check all other nodes too, to get a complete list of not available nodes
   if ((s32_Return == C_NO_ERR) ||
       (s32_Return == C_TIMEOUT))
   {
      // Reset the previous error details in case of a timeout. It will be refilled with the next retries.
      orc_ErrorDetails = "";

      // Bring all nodes to the same session and security level
      // But check if the server is already in the correct session. The routing init has set some servers
      // to the session already
      s32_Return = this->m_SetNodesSessionId(C_OSCProtocolDriverOsy::hu8_DIAGNOSTIC_SESSION_EXTENDED_DIAGNOSIS, true,
                                             this->mc_DefectNodeIndices);
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = this->m_SetNodesSecurityAccess(1U, this->mc_DefectNodeIndices);
         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error("Initializing diagnostic protocol", "Could not get security access");
         }
      }
      else
      {
         osc_write_log_error("Initializing diagnostic protocol", "Could not activate extended diagnostic session");
      }
      if (s32_Return != C_NO_ERR)
      {
         std::set<uint32>::const_iterator c_ItDefectNode;
         for (c_ItDefectNode = this->mc_DefectNodeIndices.begin(); c_ItDefectNode != this->mc_DefectNodeIndices.end();
              ++c_ItDefectNode)
         {
            orc_ErrorDetails += "- " +
                                mh_GetNodeNameForActiveNodeIndex(this->mu32_ViewIndex,
                                                                 *c_ItDefectNode) + "\n";
         }
      }
   }

   // Start all diagnosis server
   if (s32_Return == C_NO_ERR)
   {
      s32_Return = this->m_StartDiagServers(orc_ErrorDetails);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Request all cyclic transmissions based on the view configuration

   Steps:
   * configure rails for all nodes
   * request all configured cyclic and change driven transmissions

   The function will abort on the first communication problem.

   \param[in,out] orc_ErrorDetails              Error details (if any)
   \param[in,out] orc_FailedIdRegisters         Element IDs which failed registration (if any)
   \param[in,out] orc_FailedIdErrorDetails      Error details for element IDs which failed registration (if any)
   \param[out]    orc_FailedNodesElementNumber  Map with all nodes as key with the number (not the index) of the
                                                element which caused the error OSY_UDS_NRC_UPLOAD_DOWNLOAD_NOT_ACCEPTED
                                                (0x70: To many transmissions already registered)
   \param[out]    orc_NodesElementNumber        Map with all nodes as key with the number (not the index) of the
                                                element which should be registered (With and without error)

   \return
   C_CONFIG   configured view does not exist
              InitDiag() was not performed
   C_COM      Communication error
   C_NO_ERR   transmissions initialized
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::SetUpCyclicTransmissions(QString & orc_ErrorDetails,
                                                    std::vector<C_OSCNodeDataPoolListElementId> & orc_FailedIdRegisters,
                                                    std::vector<QString> & orc_FailedIdErrorDetails, std::map<uint32,
                                                                                                              uint32> & orc_FailedNodesElementNumber, std::map<uint32,
                                                                                                                                                               uint32> & orc_NodesElementNumber)
{
   uint16 u16_Node;
   sint32 s32_Return = C_NO_ERR;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   orc_FailedNodesElementNumber.clear();
   orc_NodesElementNumber.clear();

   if ((pc_View == NULL) || (this->mq_Initialized == false))
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      uint16 u16_RateMs;
      //set up rails:
      for (u16_Node = 0U; u16_Node < this->mc_ActiveNodesIndexes.size(); u16_Node++)
      {
         u16_RateMs = pc_View->GetUpdateRateFast();
         s32_Return = this->mc_DiagProtocols[u16_Node]->DataPoolSetEventDataRate(0, u16_RateMs);
         if (s32_Return == C_NO_ERR)
         {
            u16_RateMs = pc_View->GetUpdateRateMedium();
            s32_Return = this->mc_DiagProtocols[u16_Node]->DataPoolSetEventDataRate(1, u16_RateMs);
         }

         if (s32_Return == C_NO_ERR)
         {
            u16_RateMs = pc_View->GetUpdateRateSlow();
            s32_Return = this->mc_DiagProtocols[u16_Node]->DataPoolSetEventDataRate(2, u16_RateMs);
         }

         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_warning("Asynchronous communication",
                                  QString("Node \"%1\" - DataPoolSetEventDataRate - error: %2\n"
                                          "C_RANGE    parameter out of range (checked by client-side function)\n"
                                          "C_TIMEOUT  expected response not received within timeout\n"
                                          "C_NOACT    could not send request (e.g. TX buffer full)\n"
                                          "C_CONFIG   pre-requisites not correct; e.g. driver not initialized\n"
                                          "C_WARN     error response\n"
                                          "C_RD_WR    malformed protocol response\n").arg(QString(m_GetActiveNodeName(
                                                                                                     u16_Node).c_str())).arg(
                                     C_Uti::h_StwError(s32_Return)).toStdString().c_str());
            s32_Return = C_COM;
            orc_ErrorDetails += m_GetActiveNodeName(u16_Node).c_str();
            break;
         }
      }
   }

   if ((pc_View != NULL) &&
       (s32_Return == C_NO_ERR))
   {
      //request all transmissions that are configured for the current view
      const QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> & rc_Transmissions =
         pc_View->GetReadRailAssignments();

      for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::const_iterator c_It =
              rc_Transmissions.begin(); c_It != rc_Transmissions.end(); ++c_It)
      {
         bool q_Found;
         //we need the node index within the list of active nodes:
         uint32 u32_ActiveNodeIndex = this->m_GetActiveIndex(c_It.key().u32_NodeIndex, &q_Found);
         //Skip inactive nodes
         if (q_Found == true)
         {
            uint8 u8_NegResponseCode = 0;
            //check for valid value ranges (node index is checked in "GetActiveIndex" function)
            tgl_assert(c_It.key().u32_DataPoolIndex <= 0xFFU);
            tgl_assert(c_It.key().u32_ListIndex <= 0xFFFFU);
            tgl_assert(c_It.key().u32_ElementIndex <= 0xFFFFU);

            if ((c_It.value().e_TransmissionMode == C_PuiSvReadDataConfiguration::eTM_CYCLIC) ||
                (c_It.value().e_TransmissionMode == C_PuiSvReadDataConfiguration::eTM_ON_CHANGE))
            {
               const std::map<uint32, uint32>::iterator c_ItNodesElementNumber = orc_NodesElementNumber.find(
                  c_It.key().u32_NodeIndex);
               if (c_ItNodesElementNumber == orc_NodesElementNumber.end())
               {
                  orc_NodesElementNumber[c_It.key().u32_NodeIndex] = 1U;
               }
               else
               {
                  ++c_ItNodesElementNumber->second;
               }
            }

            if (c_It.value().e_TransmissionMode == C_PuiSvReadDataConfiguration::eTM_CYCLIC)
            {
               s32_Return = this->mc_DiagProtocols[u32_ActiveNodeIndex]->DataPoolReadCyclic(
                  static_cast<uint8>(c_It.key().u32_DataPoolIndex), static_cast<uint16>(c_It.key().u32_ListIndex),
                  static_cast<uint16>(c_It.key().u32_ElementIndex), c_It.value().u8_RailIndex, &u8_NegResponseCode);
            }
            else if (c_It.value().e_TransmissionMode == C_PuiSvReadDataConfiguration::eTM_ON_CHANGE)
            {
               //convert the type dependent threshold to a uint32 representation
               std::vector<uint8> c_Threshold;
               uint32 u32_Threshold;
               c_It.value().c_ChangeThreshold.GetValueAsLittleEndianBlob(c_Threshold);
               //defensive measure: as element may only be up to 32bit the threshold may also not be > 32bit
               tgl_assert(c_Threshold.size() <= 4);
               //fill up to 4 bytes with zeroes
               c_Threshold.resize(4, 0U);
               //finally compose the uint32:
               //lint -e{864} //false positive due to const/non-const misinterpretation
               u32_Threshold = c_Threshold[0] +
                               (static_cast<uint32>(c_Threshold[1]) << 8) +
                               (static_cast<uint32>(c_Threshold[2]) << 16) +
                               (static_cast<uint32>(c_Threshold[3]) << 24);

               s32_Return = this->mc_DiagProtocols[u32_ActiveNodeIndex]->DataPoolReadChangeDriven(
                  static_cast<uint8>(c_It.key().u32_DataPoolIndex), static_cast<uint16>(c_It.key().u32_ListIndex),
                  static_cast<uint16>(c_It.key().u32_ElementIndex),
                  c_It.value().u8_RailIndex, u32_Threshold, &u8_NegResponseCode);
            }
            else
            {
               // No registration necessary
               s32_Return = C_NO_ERR;
            }
            //Both services map to the same error
            if (s32_Return != C_NO_ERR)
            {
               QString c_AdditionalInfo;
               QString c_Details;
               std::map<uint32, uint32>::iterator c_ItFailedNodesElementNumber;

               switch (s32_Return)
               {
               case C_RANGE:
                  c_Details = C_GtGetText::h_GetText("Parameter out of range (checked by client-side function)");
                  break;
               case C_NOACT:
                  c_Details = C_GtGetText::h_GetText("Could not send request (e.g. TX buffer full)");
                  break;
               case C_CONFIG:
                  c_Details = C_GtGetText::h_GetText("Pre-requisites not correct; e.g. driver not initialized");
                  break;
               case C_WARN:
                  switch (u8_NegResponseCode)
                  {
                  case 0x13:
                     c_AdditionalInfo = C_GtGetText::h_GetText("Incorrect length of request");
                     break;
                  case 0x22:
                     c_AdditionalInfo = C_GtGetText::h_GetText(
                        "Datapool element specified by data identifier cannot be transferred event driven (invalid data type)");
                     break;
                  case 0x70:
                     c_AdditionalInfo = C_GtGetText::h_GetText("Too many transmissions already registered");

                     c_ItFailedNodesElementNumber = orc_FailedNodesElementNumber.find(c_It.key().u32_NodeIndex);

                     if (c_ItFailedNodesElementNumber == orc_FailedNodesElementNumber.end())
                     {
                        // Save the information about the number of the first element which failed
                        orc_FailedNodesElementNumber[c_It.key().u32_NodeIndex] =
                           orc_NodesElementNumber[c_It.key().u32_NodeIndex];
                     }
                     break;
                  case 0x31:
                     c_AdditionalInfo = C_GtGetText::h_GetText("Invalid transmission mode.\n"
                                                               "\n"
                                                               "When initiating transmission:\n"
                                                               "- Datapool element specified by data identifier is not available\n"
                                                               "- changeDrivenThreshold is zero\n"
                                                               "\n"
                                                               "When stopping transmission:\n"
                                                               "- Datapool element specified by data identifier is currently not transferred event driven");
                     break;
                  case 0x33:
                     c_AdditionalInfo = C_GtGetText::h_GetText("Required security level was not unlocked");
                     break;
                  case 0x14:
                     c_AdditionalInfo = C_GtGetText::h_GetText(
                        "The total length of the event driven response messages would exceed the available buffer size");
                     break;
                  case 0x7F:
                     c_AdditionalInfo = C_GtGetText::h_GetText(
                        "The requested service is not available in the session currently active");
                     break;
                  default:
                     c_AdditionalInfo =
                        QString(C_GtGetText::h_GetText("Unknown NRC: 0x%1")).arg(QString::number(u8_NegResponseCode,
                                                                                                 16));
                     break;
                  }
                  c_Details = QString(C_GtGetText::h_GetText("Error response (%1)")).arg(c_AdditionalInfo);
                  break;
               case C_RD_WR:
                  c_Details = C_GtGetText::h_GetText("Malformed protocol response");
                  break;
               default:
                  c_Details = C_GtGetText::h_GetText("Unknown error");
                  break;
               }
               orc_FailedIdErrorDetails.push_back(c_Details);
               orc_FailedIdRegisters.push_back(c_It.key());
               //Error can be ignored, user feedback is different
               s32_Return = C_NO_ERR;
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stop cyclic transmissions for all nodes

    Even if one of the nodes reports an error this function will continue and try to stop
     communication for the rest.

   \return
   C_CONFIG   configured view does not exist
              InitDiag() was not performed
   C_COM      Communication error (at least one of the nodes did not confirm the stop)
   C_NO_ERR   requested to stop transmissions
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::StopCyclicTransmissions(void)
{
   uint16 u16_Node;
   sint32 s32_Return = C_NO_ERR;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if ((pc_View == NULL) || (this->mq_Initialized == false))
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      //stop all transmissions
      for (u16_Node = 0U; u16_Node < this->mc_ActiveNodesIndexes.size(); u16_Node++)
      {
         sint32 s32_Return2 = this->mc_DiagProtocols[u16_Node]->DataPoolStopEventDriven();
         if (s32_Return2 != C_NO_ERR)
         {
            osc_write_log_warning("Asynchronous communication",
                                  QString("Node \"%1\" - DataPoolStopEventDriven - warning: %2\n").
                                  arg(QString(m_GetActiveNodeName(u16_Node).c_str())).
                                  arg(C_Uti::h_StwError(s32_Return2)).toStdString().c_str());
            s32_Return = C_COM;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Close the com driver

   All KEFEX servers will be logged off and if used the routing to KEFEX servers will be closed and deactivated.
   The openSYDE protocol server will not be closed. The session timeout is used to close all connections.

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send protocol request
   C_WARN     error response
   C_CONFIG   CAN dispatcher not installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::StopDiagnosisServer(void)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mq_Initialized == true)
   {
      uint32 u32_Counter;

      for (u32_Counter = 0U; u32_Counter < this->mc_DiagProtocols.size(); ++u32_Counter)
      {
         //lint -e{740}  no problem because of common base class
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_OSCDiagProtocolKfx * pc_DiagProtocolKfx =
            dynamic_cast<C_OSCDiagProtocolKfx *>(this->mc_DiagProtocols[u32_Counter]);

         if (pc_DiagProtocolKfx != NULL)
         {
            sint32 s32_ReturnLogoff;
            s32_ReturnLogoff = pc_DiagProtocolKfx->Logoff(false);

            if (s32_ReturnLogoff != C_NO_ERR)
            {
               // We must logoff all nodes, but want to know an error. No break.
               s32_Return = s32_ReturnLogoff;
            }
         }
      }

      // Stop cyclic transmissions due to problems when closing TCP sockets.
      // If a socket was closed by client and a cyclic transmission was sent by server before the close was processed
      // a socket reset will occur
      this->StopCyclicTransmissions();

      this->m_StopRoutingOfActiveNodes();

      this->mq_Initialized = false;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts the thread for async communication

   \return
   C_NO_ERR    Thread started with cyclic communication
   C_CONFIG    InitDiag function was not called or not successful or protocol was not initialized properly.
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::StartCycling(void)
{
   sint32 s32_Return = C_CONFIG;

   if ((this->mpc_AsyncThread != NULL) &&
       (this->mq_Initialized == true))
   {
      this->mpc_AsyncThread->start();
      s32_Return = C_NO_ERR;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stops the thread for async communication
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiag::StopCycling(void)
{
   tgl_assert(this->mpc_AsyncThread != NULL);
   if (this->mpc_AsyncThread != NULL)
   {
      this->mpc_AsyncThread->requestInterruption();
      if (this->mpc_AsyncThread->wait(2000U) == false)
      {
         // Not finished yet
         osc_write_log_warning("Stopping diagnostic cycling", "Waiting time for stopping thread was not enough");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends the tester present message to all active and reached nodes

   \return
   C_NO_ERR    All nodes set to session successfully
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_COM       Error of service
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::SendTesterPresentWithoutDefectNodes(void)
{
   return this->SendTesterPresent(&this->mc_DefectNodeIndices);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start thread for polled reading from data pool

   \param[in]   ou32_NodeIndex       node index to read from
   \param[in]   ou8_DataPoolIndex    data pool to read from
   \param[in]   ou16_ListIndex       list index to read from
   \param[in]   ou16_ElementIndex    element index to read from

   \return
   C_RANGE   node index out of range
   C_BUSY    polling thread already busy (only one polled function possible in parallel)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::PollDataPoolRead(const uint32 ou32_NodeIndex, const uint8 ou8_DataPoolIndex,
                                            const uint16 ou16_ListIndex, const uint16 ou16_ElementIndex)
{
   sint32 s32_Return;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= mc_DataDealers.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = mc_PollingThread.StartDataPoolRead((*mc_DataDealers[u32_ActiveIndex]), ou8_DataPoolIndex,
                                                      ou16_ListIndex, ou16_ElementIndex);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start thread for polled writing to data pool

   \param[in]   ou32_NodeIndex       node index to write from
   \param[in]   ou8_DataPoolIndex    data pool to write from
   \param[in]   ou16_ListIndex       list index to write from
   \param[in]   ou16_ElementIndex    element index to write from

   \return
   C_RANGE   node index out of range
   C_BUSY    polling thread already busy (only one polled function possible in parallel)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::PollDataPoolWrite(const uint32 ou32_NodeIndex, const uint8 ou8_DataPoolIndex,
                                             const uint16 ou16_ListIndex, const uint16 ou16_ElementIndex)
{
   sint32 s32_Return;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= mc_DataDealers.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = mc_PollingThread.StartDataPoolWrite((*mc_DataDealers[u32_ActiveIndex]), ou8_DataPoolIndex,
                                                       ou16_ListIndex, ou16_ElementIndex);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start thread for polled reading from data pool

   \param[in]   ou32_NodeIndex       node index to read from
   \param[in]   ou8_DataPoolIndex    data pool to read from
   \param[in]   ou16_ListIndex       list index to read from
   \param[in]   ou16_ElementIndex    element index to read from

   \return
   C_RANGE   node index out of range
   C_BUSY    polling thread already busy (only one polled function possible in parallel)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::PollNvmRead(const uint32 ou32_NodeIndex, const uint8 ou8_DataPoolIndex,
                                       const uint16 ou16_ListIndex, const uint16 ou16_ElementIndex)
{
   sint32 s32_Return;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= mc_DataDealers.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = mc_PollingThread.StartNvmRead((*mc_DataDealers[u32_ActiveIndex]), ou8_DataPoolIndex,
                                                 ou16_ListIndex, ou16_ElementIndex);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start thread for polled writing to data pool

   \param[in]   ou32_NodeIndex       node index to write from
   \param[in]   ou8_DataPoolIndex    data pool to write from
   \param[in]   ou16_ListIndex       list index to write from
   \param[in]   ou16_ElementIndex    element index to write from

   \return
   C_RANGE   node index out of range
   C_BUSY    polling thread already busy (only one polled function possible in parallel)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::PollNvmWrite(const uint32 ou32_NodeIndex, const uint8 ou8_DataPoolIndex,
                                        const uint16 ou16_ListIndex, const uint16 ou16_ElementIndex)
{
   sint32 s32_Return;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= mc_DataDealers.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = mc_PollingThread.StartNvmWrite((*mc_DataDealers[u32_ActiveIndex]), ou8_DataPoolIndex,
                                                  ou16_ListIndex, ou16_ElementIndex);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start thread for polled reading a list from data pool

   \param[in]   ou32_NodeIndex       node index to read from
   \param[in]   ou8_DataPoolIndex    data pool to read from
   \param[in]   ou16_ListIndex       list index to read from

   \return
   C_RANGE   node index out of range
   C_BUSY    polling thread already busy (only one polled function possible in parallel)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::PollNvmReadList(const uint32 ou32_NodeIndex, const uint8 ou8_DataPoolIndex,
                                           const uint16 ou16_ListIndex)
{
   sint32 s32_Return;
   bool q_Found;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex, &q_Found);

   if ((u32_ActiveIndex >= mc_DataDealers.size()) || (q_Found == false))
   {
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = mc_PollingThread.StartNvmReadList((*mc_DataDealers[u32_ActiveIndex]), ou8_DataPoolIndex,
                                                     ou16_ListIndex);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start thread for polled writing changed NVM elements to data pool

   \param[in] ou32_NodeIndex Node index to write to
   \param[in] orc_ListIds    Lists to update CRC only

   \return
   C_RANGE   node index out of range
   C_BUSY    polling thread already busy (only one polled function possible in parallel)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::PollSafeNvmWriteChangedElements(const uint32 ou32_NodeIndex,
                                                           const std::vector<C_OSCNodeDataPoolListId> & orc_ListIds)
{
   sint32 s32_Return;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= mc_DataDealers.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = mc_PollingThread.StartNvmSafeWriteChangedValues((*mc_DataDealers[u32_ActiveIndex]), orc_ListIds);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the output of PollSafeNvmWriteChangedElements

   \param[out]    orc_ChangedElements       All changed elements

   \return
   C_BUSY    polling thread already busy (only one polled function possible in parallel)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::GetPollSafeNvmWriteChangedElementsOutput(
   std::vector<C_OSCNodeDataPoolListElementId> & orc_ChangedElements) const
{
   return this->mc_PollingThread.GetNvmSafeWriteChangedValuesOutput(orc_ChangedElements);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start thread for polled reading of NVM values

   PollSafeNvmWriteChangedElements must be called before calling PollSafeNvmReadValues

   \param[in]   ou32_NodeIndex       node index to read from

   \return
   C_RANGE   node index out of range
   C_BUSY    polling thread already busy (only one polled function possible in parallel)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::PollSafeNvmReadValues(const uint32 ou32_NodeIndex)
{
   sint32 s32_Return;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= mc_DataDealers.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = mc_PollingThread.StartNvmSafeReadValues(*mc_DataDealers[u32_ActiveIndex]);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the output of the function NvmSafeReadValues

   Must be called after the thread was finished after calling NvmSafeReadValues

   \param[out]    orpc_ParamNodeValues  Pointer to node with read values

   \return
   C_NO_ERR   result returned
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::GetPollNvmSafeReadValuesOutput(const C_OSCNode * & orpc_ParamNodeValues) const
{
   return this->mc_PollingThread.GetNvmSafeReadValuesOutput(orpc_ParamNodeValues);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start thread for polled reading of NVM values

   PollSafeNvmWriteChangedElements must be called before calling PollSafeNvmReadValues

   \param[in]   ou32_NodeIndex       node index to read from

   \return
   C_RANGE   node index out of range
   C_BUSY    polling thread already busy (only one polled function possible in parallel)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::PollSafeNvmSafeWriteCrcs(const uint32 ou32_NodeIndex)
{
   sint32 s32_Return;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= mc_DataDealers.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = mc_PollingThread.StartNvmSafeWriteCrcs(*mc_DataDealers[u32_ActiveIndex]);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start thread for polled reading of NVM values

   PollSafeNvmWriteChangedElements must be called before calling PollSafeNvmReadValues

   \param[in]   ou32_NodeIndex       node index to read from
   \param[in]   ou8_DataPoolIndex    data pool to read from
   \param[in]   ou16_ListIndex       list index to read from

   \return
   C_RANGE   node index out of range
   C_BUSY    polling thread already busy (only one polled function possible in parallel)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::PollNvmNotifyOfChanges(const uint32 ou32_NodeIndex, const uint8 ou8_DataPoolIndex,
                                                  const uint16 ou16_ListIndex)
{
   sint32 s32_Return;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= mc_DataDealers.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = mc_PollingThread.StartNvmNotifyOfChanges((*mc_DataDealers[u32_ActiveIndex]), ou8_DataPoolIndex,
                                                            ou16_ListIndex);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the output of PollSafeNvmWriteChangedElements

   \param[out] orq_ApplicationAcknowledge  true: positive acknowledge from server
                                           false: negative acknowledge from server

   \return
   C_BUSY    polling thread already busy (only one polled function possible in parallel)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::GetPollNvmNotifyOfChangesOutput(bool & orq_ApplicationAcknowledge) const
{
   return this->mc_PollingThread.GetNvmNotifyOfChangesOutput(orq_ApplicationAcknowledge);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Wrap polling results

   Can be used to extract the results of one service execution after it has finished.

   \param[out]  ors32_Result       result code of executed service function
                                   for possible values see the DataDealer's function documentation

   \return
   C_NO_ERR   result code read
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::GetPollResults(sint32 & ors32_Result) const
{
   return this->mc_PollingThread.GetResults(ors32_Result);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get result of previously started service execution

   Can be used to extract the results of one service execution after it has finished.

   \param[out]  oru8_NRC       negative response code of executed service function
                               for possible values see the DataDealer's function documentation

   \return
   C_NO_ERR       result code read
   C_BUSY         previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::GetPollResultNRC(uint8 & oru8_NRC) const
{
   return this->mc_PollingThread.GetNegativeResponseCode(oru8_NRC);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Wrapping call of h_NvmSafeClearInternalContent

   \param[in] ou32_NodeIndex     Index of node

   \return
   C_NO_ERR  started polling
   C_RANGE   node index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::NvmSafeClearInternalContent(const uint32 ou32_NodeIndex) const
{
   sint32 s32_Return;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= mc_DataDealers.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      C_SyvComDataDealer * const pc_DataDealer = mc_DataDealers[u32_ActiveIndex];
      if (pc_DataDealer != NULL)
      {
         s32_Return = C_NO_ERR;
         pc_DataDealer->NvmSafeClearInternalContent();
      }
      else
      {
         s32_Return = C_RANGE;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start thread for polled reading of NVM values for creating parameter set file

   \param[in] ou32_NodeIndex     Index of node
   \param[in] orc_ListIds        Container will relevant list IDs

   \return
   C_NO_ERR  started polling
   C_RANGE   node index out of range
   C_BUSY    polling thread already busy (only one polled function possible in parallel)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::PollNvmSafeReadParameterValues(const uint32 ou32_NodeIndex,
                                                          const std::vector<C_OSCNodeDataPoolListId> & orc_ListIds)
{
   sint32 s32_Return;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= mc_DataDealers.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = mc_PollingThread.StartNvmSafeReadParameterValues((*mc_DataDealers[u32_ActiveIndex]),
                                                                    orc_ListIds);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Wrapping call of h_NvmSafeCreateCleanFileWithoutCRC

   \param[in] ou32_NodeIndex Node index to work with
   \param[in] orc_FilePath   Parameter file path
   \param[in] orc_FileInfo   Optional general file information

   \return
   C_NO_ERR   data saved
   C_OVERFLOW Wrong sequence of function calls
   C_CONFIG   Internal data invalid
   C_BUSY     file already exists
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::NvmSafeCreateCleanFileWithoutCRC(const uint32 ou32_NodeIndex, const QString & orc_Path,
                                                            const C_OSCParamSetInterpretedFileInfoData & orc_FileInfo)
const
{
   sint32 s32_Return;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= mc_DataDealers.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      const stw_scl::C_SCLString c_Path = orc_Path.toStdString().c_str();
      C_SyvComDataDealer * const pc_DataDealer = mc_DataDealers[u32_ActiveIndex];
      if (pc_DataDealer != NULL)
      {
         s32_Return = C_NO_ERR;
         pc_DataDealer->NvmSafeCreateCleanFileWithoutCRC(c_Path, orc_FileInfo);
      }
      else
      {
         s32_Return = C_RANGE;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Wrapping call of h_NvmSafeReadFileWithoutCRC

   Warning: CRC is not checked

   \param[in] ou32_NodeIndex Node index to work with
   \param[in] orc_FilePath   Parameter file path

   \return
   C_NO_ERR   data read
   C_OVERFLOW Wrong sequence of function calls
   C_RANGE    Path does not match the path of the preceding function calls
   C_RD_WR    specified file does not exist
              specified file is present but structure is invalid (e.g. invalid XML file)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::NvmSafeReadFileWithoutCRC(const uint32 ou32_NodeIndex, const QString & orc_Path) const
{
   sint32 s32_Return;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= mc_DataDealers.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      const stw_scl::C_SCLString c_Path = orc_Path.toStdString().c_str();
      C_SyvComDataDealer * const pc_DataDealer = mc_DataDealers[u32_ActiveIndex];
      if (pc_DataDealer != NULL)
      {
         s32_Return = C_NO_ERR;
         pc_DataDealer->NvmSafeReadFileWithoutCRC(c_Path);
      }
      else
      {
         s32_Return = C_RANGE;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Wrapping call of NvmSafeCheckParameterFileContents

   \param[in]  ou32_NodeIndex    node index to read from
   \param[in]  orc_Path          File path
   \param[out] orc_DataPoolLists Loaded data pool lists (Always cleared at start)

   \return
   C_NO_ERR   Lists valid
   C_OVERFLOW Wrong sequence of function calls
   C_RANGE    Path does not match the path of the preceding function calls
              Node index out of range
   C_CONFIG   Mismatch of data with current node
               or no valid pointer to the original instance of "C_OSCNode" is set in "C_OSCDataDealer"
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::NvmSafeCheckParameterFileContents(const uint32 ou32_NodeIndex, const QString & orc_Path,
                                                             std::vector<C_OSCNodeDataPoolListId> & orc_DataPoolLists)
{
   sint32 s32_Return;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= mc_DataDealers.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      const stw_scl::C_SCLString c_Path = orc_Path.toStdString().c_str();
      s32_Return = this->mc_DataDealers[u32_ActiveIndex]->NvmSafeCheckParameterFileContents(
         c_Path, orc_DataPoolLists);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Wrapping call of h_NvmSafeUpdateCRCForFile

   \param[in] ou32_NodeIndex Node index to work with
   \param[in] orc_FilePath   Parameter file path

   \return
   C_NO_ERR   CRC updated
   C_OVERFLOW Wrong sequence of function calls
   C_RANGE    Path does not match the path of the preceding function calls
   C_RD_WR    specified file does not exist
              specified file is present but structure is invalid (e.g. invalid XML file)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::NvmSafeUpdateCRCForFile(const uint32 ou32_NodeIndex, const QString & orc_Path) const
{
   sint32 s32_Return;
   const uint32 u32_ActiveIndex = this->m_GetActiveIndex(ou32_NodeIndex);

   if (u32_ActiveIndex >= mc_DataDealers.size())
   {
      s32_Return = C_RANGE;
   }
   else
   {
      const stw_scl::C_SCLString c_Path = orc_Path.toStdString().c_str();
      C_SyvComDataDealer * const pc_DataDealer = mc_DataDealers[u32_ActiveIndex];
      if (pc_DataDealer != NULL)
      {
         s32_Return = C_NO_ERR;
         pc_DataDealer->NvmSafeUpdateCRCForFile(c_Path);
      }
      else
      {
         s32_Return = C_RANGE;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns a reference to all data dealer

   \return
   Reference to vector with all data dealer
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_SyvComDataDealer *> & C_SyvComDriverDiag::GetAllDataDealer(void) const
{
   return this->mc_DataDealers;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a widget to inform about new datapool com signal events

   \param[in]     opc_Widget     Pointer to dashboard widget base
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiag::RegisterWidget(C_PuiSvDbDataElementHandler * const opc_Widget)
{
   if (opc_Widget != NULL)
   {
      uint32 u32_Counter;

      for (u32_Counter = 0U; u32_Counter < opc_Widget->GetWidgetDataPoolElementCount(); ++u32_Counter)
      {
         C_PuiSvDbNodeDataPoolListElementId c_DpElementId;
         if (opc_Widget->GetDataPoolElementIndex(u32_Counter, c_DpElementId) == C_NO_ERR)
         {
            // Is it relevant for this data dealer (No handling of bus signals here)?
            if (c_DpElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eBUS_SIGNAL)
            {
               C_OSCCanMessageIdentificationIndices c_MsgId;
               uint32 u32_SignalIndex;
               const C_OSCCanMessage * pc_CanMsg;
               const C_OSCCanSignal * pc_Signal;
               const C_OSCNodeDataPoolListElement * const pc_Element =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(c_DpElementId);

               // Get the signal information out of the CAN message of this datapool element
               C_PuiSdHandler::h_GetInstance()->ConvertElementIndexToSignalIndex(c_DpElementId,
                                                                                 c_MsgId, u32_SignalIndex);
               pc_CanMsg = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MsgId);
               pc_Signal = C_PuiSdHandler::h_GetInstance()->GetCanSignal(c_MsgId, u32_SignalIndex);

               if ((pc_CanMsg != NULL) &&
                   (pc_Signal != NULL) &&
                   (pc_Element != NULL))
               {
                  const uint32 u32_MsgCanId = pc_CanMsg->u32_CanId;
                  C_SyvComDriverDiagWidgetRegistration c_WidgetRegistration;
                  QMap<stw_types::uint32, QList<C_SyvComDriverDiagWidgetRegistration> >::iterator c_ItElement;

                  c_WidgetRegistration.c_Signal = *pc_Signal;

                  if (c_WidgetRegistration.c_Signal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL)
                  {
                     uint32 u32_SignalCounter;
                     bool q_MultiplexerSignalFound = false;

                     // Special case: This signal is multiplexed. It is necessary to know the multiplexer signal
                     for (u32_SignalCounter = 0U; u32_SignalCounter < pc_CanMsg->c_Signals.size(); ++u32_SignalCounter)
                     {
                        if (pc_CanMsg->c_Signals[u32_SignalCounter].e_MultiplexerType ==
                            C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
                        {
                           // Save the multiplexer signal
                           c_WidgetRegistration.c_MultiplexerSignal = pc_CanMsg->c_Signals[u32_SignalCounter];
                           q_MultiplexerSignalFound = true;
                           break;
                        }
                     }

                     // A multiplexer signal must exist if at least one multiplexed signal is present
                     tgl_assert(q_MultiplexerSignalFound == true);
                  }

                  c_WidgetRegistration.pc_Handler = opc_Widget;
                  c_WidgetRegistration.q_IsExtended = pc_CanMsg->q_IsExtended;
                  c_WidgetRegistration.u16_Dlc = pc_CanMsg->u16_Dlc;
                  c_WidgetRegistration.c_ElementId = c_DpElementId;
                  // Save the value content to have the content instance with the correct type as template for
                  // the new values
                  c_WidgetRegistration.c_ElementContent = pc_Element->c_Value;

                  c_ItElement = this->mc_AllWidgets.find(u32_MsgCanId);

                  // Add the widget to the map
                  if (c_ItElement != this->mc_AllWidgets.end())
                  {
                     //Check if not already contained
                     if (c_ItElement.value().contains(c_WidgetRegistration) == false)
                     {
                        // There is a list for this datapool element already
                        c_ItElement.value().push_back(c_WidgetRegistration);
                     }
                  }
                  else
                  {
                     // The map has no entry for this datapool element. Add a new list with this widget
                     QList<C_SyvComDriverDiagWidgetRegistration> c_List;
                     c_List.push_back(c_WidgetRegistration);
                     this->mc_AllWidgets.insert(u32_MsgCanId, c_List);
                  }
               }
               else
               {
                  // TODO: Error handling
               }
            }
         }
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
bool C_SyvComDriverDiag::m_GetRoutingMode(C_OSCRoutingCalculation::E_Mode & ore_Mode) const
{
   ore_Mode = C_OSCRoutingCalculation::eDIAGNOSTIC;

   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns needed session ID for the current routing mode

   \return
   Session ID for flashloader
*/
//----------------------------------------------------------------------------------------------------------------------
uint8 C_SyvComDriverDiag::m_GetRoutingSessionId(void) const
{
   return C_OSCProtocolDriverOsy::hu8_DIAGNOSTIC_SESSION_EXTENDED_DIAGNOSIS;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the routing for a not openSYDE server is necessary

   \param[in]     orc_Node                             Current node

   \return
   true    Specific server and legacy routing necessary
   false   No specific server and legacy routing necessary
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvComDriverDiag::m_IsRoutingSpecificNecessary(const C_OSCNode & orc_Node) const
{
   bool q_Return = false;

   if (orc_Node.c_Properties.e_DiagnosticServer == C_OSCNodeProperties::eDS_KEFEX)
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepares the routing for a KEFEX server

   \param[in]     ou32_ActiveNode                      Active node index of vector mc_ActiveNodes
   \param[in]     opc_Node                             Pointer to current node
   \param[in]     orc_LastNodeOfRouting                The last node in the routing chain before the final target server
   \param[in]     opc_ProtocolOsyOfLastNodeOfRouting   The protocol of the last node
   \param[out]    oppc_RoutingDispatcher               The legacy routing dispatcher

   \return
   C_NO_ERR    Specific server necessary and legacy routing dispatcher created
   C_NOACT     No specific server necessary
   C_CONFIG    opc_ProtocolOsyOfLastNodeOfRouting is NULL
               Diagnose protocol is NULL
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::m_StartRoutingSpecific(const uint32 ou32_ActiveNode, const C_OSCNode * const opc_Node,
                                                  const C_OSCRoutingRoutePoint & orc_LastNodeOfRouting,
                                                  C_OSCProtocolDriverOsy * const opc_ProtocolOsyOfLastNodeOfRouting,
                                                  C_OSCCanDispatcherOsyRouter ** const oppc_RoutingDispatcher)
{
   sint32 s32_Return = C_NOACT;

   if (opc_Node->c_Properties.e_DiagnosticServer == C_OSCNodeProperties::eDS_KEFEX)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_OSCDiagProtocolKfx * const pc_DiagProtocolKfx =
         dynamic_cast<C_OSCDiagProtocolKfx * const>(this->mc_DiagProtocols[ou32_ActiveNode]);

      if ((pc_DiagProtocolKfx != NULL) &&
          (opc_ProtocolOsyOfLastNodeOfRouting != NULL))
      {
         (*oppc_RoutingDispatcher) = new C_OSCCanDispatcherOsyRouter(*opc_ProtocolOsyOfLastNodeOfRouting);
         // TODO Filter settings?
         (*oppc_RoutingDispatcher)->SetFilterParameters(orc_LastNodeOfRouting.u8_OutInterfaceNumber, 0x00000000,
                                                        0x00000000);

         this->mc_LegacyRouterDispatchers[ou32_ActiveNode] = (*oppc_RoutingDispatcher);
         // Set the new dispatcher
         pc_DiagProtocolKfx->SetDispatcher(*oppc_RoutingDispatcher);

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
void C_SyvComDriverDiag::m_StopRoutingSpecific(const uint32 ou32_ActiveNode)
{
   if ((this->mc_ActiveNodesIndexes[ou32_ActiveNode] < this->mpc_SysDef->c_Nodes.size()) &&
       (this->mpc_SysDef->c_Nodes[this->mc_ActiveNodesIndexes[ou32_ActiveNode]].c_Properties.e_DiagnosticServer ==
        C_OSCNodeProperties::eDS_KEFEX))
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_OSCDiagProtocolKfx * const pc_DiagProtocolKfx =
         dynamic_cast<C_OSCDiagProtocolKfx * const>(this->mc_DiagProtocols[ou32_ActiveNode]);

      if (pc_DiagProtocolKfx != NULL)
      {
         // Remove dispatcher
         pc_DiagProtocolKfx->SetDispatcher(NULL);
      }
   }

   C_OSCComDriverProtocol::m_StopRoutingSpecific(ou32_ActiveNode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the interface has relevant functions activated

   In this case diagnostic and update functionality

   \param[in]     orc_ComItfSettings         Interface configuration

   \return
   true     Interface has relevant functions activated and is connected
   false    Interface has no relevant functions activated or is not connected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvComDriverDiag::m_CheckInterfaceForFunctions(const C_OSCNodeComInterfaceSettings & orc_ComItfSettings) const
{
   bool q_Return = false;

   if ((orc_ComItfSettings.q_IsBusConnected == true) &&
       ((orc_ComItfSettings.q_IsRoutingEnabled == true) ||
        (orc_ComItfSettings.q_IsDiagnosisEnabled == true)))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Distributes the CAN message to all registered C_OSCDataDealer for all relevant Datapool comm signals

   \param[in]     orc_Msg        Current CAN message
   \param[in]     oq_IsTx        Message was sent by C_OSCComDriverBase itself
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiag::m_HandleCanMessage(const T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx)
{
   C_OSCComDriverBase::m_HandleCanMessage(orc_Msg, oq_IsTx);
   QMap<stw_types::uint32, QList<C_SyvComDriverDiagWidgetRegistration> >::const_iterator c_ItElement;

   // Check if this CAN message id is relevant
   c_ItElement = this->mc_AllWidgets.find(orc_Msg.u32_ID);

   if (c_ItElement != this->mc_AllWidgets.end())
   {
      const QList<C_SyvComDriverDiagWidgetRegistration> & rc_Registrations = c_ItElement.value();
      QList<C_SyvComDriverDiagWidgetRegistration>::const_iterator c_ItRegistration;

      // Iterate through all widget registrations
      for (c_ItRegistration = rc_Registrations.begin(); c_ItRegistration != rc_Registrations.end(); ++c_ItRegistration)
      {
         const C_SyvComDriverDiagWidgetRegistration & rc_WidgetRegistration = *c_ItRegistration;

         // Is the CAN message as expected
         if (((orc_Msg.u8_XTD == 1U) == rc_WidgetRegistration.q_IsExtended) &&
             (rc_WidgetRegistration.pc_Handler != NULL))
         {
            bool q_SignalFits;
            bool q_DlcErrorPossible = true;

            if (rc_WidgetRegistration.c_Signal.e_MultiplexerType != C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL)
            {
               // No multiplexed signal, no dependency of a multiplexer value
               q_SignalFits = C_OSCCanUtil::h_IsSignalInMessage(orc_Msg.u8_DLC, rc_WidgetRegistration.c_Signal);
            }
            else
            {
               // Multiplexed signal. Checking the multiplexer signal first
               q_SignalFits = C_OSCCanUtil::h_IsSignalInMessage(orc_Msg.u8_DLC,
                                                                rc_WidgetRegistration.c_MultiplexerSignal);

               if (q_SignalFits == true)
               {
                  // Multiplexer fits into the message. Get the multiplexer value
                  C_PuiSvDbDataElementContent c_MultiplexerContent;
                  C_OSCCanUtil::h_GetSignalValue(orc_Msg.au8_Data, rc_WidgetRegistration.c_MultiplexerSignal,
                                                 c_MultiplexerContent);
                  C_OSCNodeDataPoolContent::E_Type e_Type = c_MultiplexerContent.GetType();
                  uint16 u16_MultiplexerValue;

                  // Multiplexer can be maximum 16 bit
                  if (e_Type == C_OSCNodeDataPoolContent::eUINT8)
                  {
                     u16_MultiplexerValue = c_MultiplexerContent.GetValueU8();
                  }
                  else if (e_Type == C_OSCNodeDataPoolContent::eUINT16)
                  {
                     u16_MultiplexerValue = c_MultiplexerContent.GetValueU16();
                  }
                  else
                  {
                     // May not happen
                     tgl_assert(false);
                     u16_MultiplexerValue = 0;
                  }

                  if (rc_WidgetRegistration.c_Signal.u16_MultiplexValue == u16_MultiplexerValue)
                  {
                     // The multiplexer value is matching. The signal is in the message.
                     q_SignalFits = C_OSCCanUtil::h_IsSignalInMessage(orc_Msg.u8_DLC, rc_WidgetRegistration.c_Signal);
                  }
                  else
                  {
                     // The multiplexer value is not matching. The signal is not in the message, but it is no DLC error.
                     q_SignalFits = false;
                     q_DlcErrorPossible = false;
                  }
               }
            }

            if (q_SignalFits == true)
            {
               C_PuiSvDbDataElementContent c_Content;
               const uint64 u64_TimeStamp = orc_Msg.u64_TimeStamp / 1000U;

               // Get the content
               c_Content = rc_WidgetRegistration.c_ElementContent;
               C_OSCCanUtil::h_GetSignalValue(orc_Msg.au8_Data, rc_WidgetRegistration.c_Signal, c_Content);
               c_Content.SetTimeStamp(static_cast<uint32>(u64_TimeStamp));

               rc_WidgetRegistration.pc_Handler->InsertNewValueIntoQueue(rc_WidgetRegistration.c_ElementId,
                                                                         c_Content);
            }
            else if (q_DlcErrorPossible == true)
            {
               //Error message for widget
               rc_WidgetRegistration.pc_Handler->SetErrorForInvalidDlc(rc_WidgetRegistration.c_ElementId,
                                                                       orc_Msg.u8_DLC);
            }
            else
            {
               // Nothing to do
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Initialize all members based on view

   \param[in] ou32_ViewIndex View index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvComDriverDiag::C_SyvComDriverDiagWidgetRegistration::C_SyvComDriverDiagWidgetRegistration(void) :
   pc_Handler(NULL),
   u16_Dlc(0),
   q_IsExtended(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current equal to orc_Cmp

   \param[in] orc_Cmp Compared instance

   \return
   Current equal to orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvComDriverDiag::C_SyvComDriverDiagWidgetRegistration::operator ==(
   const C_SyvComDriverDiagWidgetRegistration & orc_Cmp) const
{
   bool q_Return = true;

   if ((this->q_IsExtended != orc_Cmp.q_IsExtended) ||
       (this->u16_Dlc != orc_Cmp.u16_Dlc) ||
       (this->pc_Handler != orc_Cmp.pc_Handler) ||
       (this->c_Signal != orc_Cmp.c_Signal) ||
       (this->c_MultiplexerSignal != orc_Cmp.c_MultiplexerSignal))
   {
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Detects all nodes which are used in current dashboard

   The nodes which which has used datapool elements will be saved in an addition vector

   \return
   C_NO_ERR      No error
   C_CONFIG      Invalid system definition/view configuration
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::m_InitDiagNodes(void)
{
   sint32 s32_Return = C_CONFIG;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      s32_Return = C_NO_ERR;

      //request all transmissions that are configured for the current view
      const QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> & rc_Transmissions =
         pc_View->GetReadRailAssignments();

      // Get all nodes which has used datapool elements on this dashboard
      for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::const_iterator c_ItElement =
              rc_Transmissions.begin(); c_ItElement != rc_Transmissions.end(); ++c_ItElement)
      {
         this->mc_DiagNodes.insert(c_ItElement.key().u32_NodeIndex);
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiag::m_InitDiagProtocolKfx(C_OSCDiagProtocolKfx * const opc_DiagProtocolKefex) const
{
   stw_diag_lib::C_KFXCommConfiguration c_CommConfig;

   // TODO Init KEFEX protocol dynamically
   c_CommConfig.SetBaseID(6);
   c_CommConfig.SetClientAddress(100);
   c_CommConfig.SetServerAddress(0);
   c_CommConfig.SetBSMax(20);
   c_CommConfig.SetSTMin(0);
   c_CommConfig.SetTimeout(200);

   opc_DiagProtocolKefex->SetNvmValidFlagUsed(false);
   opc_DiagProtocolKefex->SetCommunicationParameters(c_CommConfig);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize diagnostic protocols

   The functions fills the vector mc_OsyProtocols of the base class too.

   \return
   C_NO_ERR   Operation success
   C_CONFIG   Invalid initialization
   C_OVERFLOW Unknown diagnostic server for at least one node or invalid node identifier was set in diagnostic protocol
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::m_InitDiagProtocol(void)
{
   sint32 s32_Retval = C_NO_ERR;

   if ((this->mc_TransportProtocols.size() >= this->m_GetActiveNodeCount()) &&
       (this->mc_ServerIDs.size() == this->m_GetActiveNodeCount()))
   {
      //Initialize protocol driver
      this->mc_DiagProtocols.resize(this->m_GetActiveNodeCount(), NULL);
      this->mc_OsyProtocols.resize(this->m_GetActiveNodeCount(), NULL);

      for (uint32 u32_ItActiveNode = 0;
           (u32_ItActiveNode < this->mc_ActiveNodesIndexes.size()) && (s32_Retval == C_NO_ERR);
           ++u32_ItActiveNode)
      {
         const C_OSCNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mc_ActiveNodesIndexes[u32_ItActiveNode]);
         if (pc_Node != NULL)
         {
            //Diagnostic protocol initialization
            C_OSCDiagProtocolOsy * pc_DiagProtocolOsy = NULL;
            C_OSCDiagProtocolKfx * pc_DiagProtocolKefex = NULL;
            C_OSCDiagProtocolBase * pc_DiagProtocol = NULL;

            switch (pc_Node->c_Properties.e_DiagnosticServer)
            {
            case C_OSCNodeProperties::eDS_OPEN_SYDE:
               pc_DiagProtocolOsy = new C_OSCDiagProtocolOsy();
               s32_Retval = pc_DiagProtocolOsy->SetTransportProtocol(this->mc_TransportProtocols[u32_ItActiveNode]);
               if (s32_Retval == C_NO_ERR)
               {
                  s32_Retval =
                     pc_DiagProtocolOsy->SetNodeIdentifiers(this->GetClientId(),
                                                            this->mc_ServerIDs[u32_ItActiveNode]);
                  if (s32_Retval != C_NO_ERR)
                  {
                     //Invalid configuration = programming error
                     osc_write_log_error("Initializing diagnostic protocol", "Could not set node identifiers");
                     s32_Retval = C_OVERFLOW;
                  }
               }
               else
               {
                  //Invalid configuration = programming error
                  s32_Retval = C_OVERFLOW;
               }
               pc_DiagProtocol = pc_DiagProtocolOsy;
               this->mc_OsyProtocols[u32_ItActiveNode] = pc_DiagProtocolOsy;
               break;
            case C_OSCNodeProperties::eDS_KEFEX:
               pc_DiagProtocolKefex = new C_OSCDiagProtocolKfx();
               pc_DiagProtocolKefex->SetDispatcher(this->m_GetCanDispatcher());
               pc_DiagProtocol = pc_DiagProtocolKefex;
               this->m_InitDiagProtocolKfx(pc_DiagProtocolKefex);
               break;
            default:
               osc_write_log_error("Initializing diagnostic protocol", "Unknown diagnostic protocol");
               s32_Retval = C_OVERFLOW;
               break;
            }
            this->mc_DiagProtocols[u32_ItActiveNode] = pc_DiagProtocol;
         }
         else
         {
            osc_write_log_error("Initializing diagnostic protocol", "Node not found");
            s32_Retval = C_CONFIG;
         }

         if (s32_Retval != C_NO_ERR)
         {
            break;
         }
      }
   }
   else
   {
      osc_write_log_error("Initializing diagnostic protocol",
                          "Inconsistent number of diagnostic protocols or node IDs installed");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize data dealers

   \return
   C_NO_ERR Operation success
   C_CONFIG Invalid initialization
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::m_InitDataDealer(void)
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->mc_DiagProtocols.size() == this->m_GetActiveNodeCount())
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (pc_View != NULL)
      {
         this->mc_DataDealers.resize(this->m_GetActiveNodeCount());
         for (uint32 u32_ItActiveFlag = 0; u32_ItActiveFlag < this->mc_ActiveNodesIndexes.size(); ++u32_ItActiveFlag)
         {
            C_OSCNode * const pc_Node =
               C_PuiSdHandler::h_GetInstance()->GetOSCNode(this->mc_ActiveNodesIndexes[u32_ItActiveFlag]);
            if (pc_Node != NULL)
            {
               //Data dealer init
               this->mc_DataDealers[u32_ItActiveFlag] =
                  new C_SyvComDataDealer(pc_Node, this->mc_ActiveNodesIndexes[u32_ItActiveFlag],
                                         this->mc_DiagProtocols[u32_ItActiveFlag]);
            }
            else
            {
               osc_write_log_error("Initializing data dealer", "Node not found");
               s32_Retval = C_CONFIG;
            }
         }
      }
      else
      {
         osc_write_log_error("Initializing data dealer", "Configured view invalid");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      osc_write_log_error("Initializing data dealer", "Inconsistent number of diagnostic protocols installed");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize the necessary routing configuration to start the routing for diagnosis

   Prepares all active nodes with its routing configurations if necessary
   Three different types of routing:
   - openSYDE routing for a openSYDE server
   - legacy routing for a KEFEX server
   - legacy routing for a KEFEX server after openSYDE routing to a openSYDE server

   \param[in]  orc_ErrorDetails        Details for current error
   \param[out] orc_ErrorActiveNodes    All active node indexes of nodes which can not be reached

   \return
   C_NO_ERR Operation success
   C_CONFIG Invalid initialization
   C_WARN      Error response
   C_BUSY      Connection to at least one server failed
   C_COM       Communication problem
   C_TIMEOUT   Expected response not received within timeout
   C_RD_WR     Unexpected content in response
   C_NOACT     At least one node does not support Ethernet to Ethernet routing
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::m_StartRoutingDiag(QString & orc_ErrorDetails, std::set<uint32> & orc_ErrorActiveNodes)
{
   sint32 s32_Return = C_NO_ERR;
   uint32 u32_ActiveNodeCounter;
   uint32 u32_ErrorActiveNodeIndex;

   // Start IP to IP routing for all nodes which need it
   for (u32_ActiveNodeCounter = 0U; u32_ActiveNodeCounter < this->mc_ActiveNodesIndexes.size(); ++u32_ActiveNodeCounter)
   {
      s32_Return = this->m_StartRoutingIp2Ip(u32_ActiveNodeCounter, &u32_ErrorActiveNodeIndex);

      if (s32_Return != C_NO_ERR)
      {
         const C_OSCNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mc_ActiveNodesIndexes[u32_ActiveNodeCounter]);

         orc_ErrorActiveNodes.insert(u32_ActiveNodeCounter);
         orc_ErrorActiveNodes.insert(u32_ErrorActiveNodeIndex);

         tgl_assert(pc_Node != NULL);
         if (pc_Node != NULL)
         {
            orc_ErrorDetails += QString("\"") + pc_Node->c_Properties.c_Name.c_str() + "\"\n";
         }
         break;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      // Search nodes which needs routing
      for (u32_ActiveNodeCounter = 0U; u32_ActiveNodeCounter < this->mc_ActiveNodesIndexes.size();
           ++u32_ActiveNodeCounter)
      {
         const C_OSCNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mc_ActiveNodesIndexes[u32_ActiveNodeCounter]);

         tgl_assert(pc_Node != NULL);
         if (pc_Node != NULL)
         {
            s32_Return = this->m_StartRouting(u32_ActiveNodeCounter, &u32_ErrorActiveNodeIndex);

            tgl_assert(pc_Node->pc_DeviceDefinition != NULL);
            // Reconnect is only supported by openSYDE nodes
            if (((pc_Node->pc_DeviceDefinition->q_DiagnosticProtocolOpenSydeCan == true) ||
                 (pc_Node->pc_DeviceDefinition->q_DiagnosticProtocolOpenSydeEthernet == true)) &&
                (s32_Return == C_NO_ERR) &&
                (this->GetClientId().u8_BusIdentifier == this->mc_ServerIDs[u32_ActiveNodeCounter].u8_BusIdentifier))
            {
               s32_Return = this->ReConnectNode(this->mc_ServerIDs[u32_ActiveNodeCounter]);
            }

            if (s32_Return != C_NO_ERR)
            {
               orc_ErrorActiveNodes.insert(u32_ActiveNodeCounter);
               orc_ErrorActiveNodes.insert(u32_ErrorActiveNodeIndex);

               orc_ErrorDetails += QString("\"") + pc_Node->c_Properties.c_Name.c_str() + "\"\n";
               break;
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts the diagnose servers

   Calling the verify function to start all diag servers.

   \param[in,out] orc_ErrorDetails     Details for current error

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send request (e.g. TX buffer full)
   C_CONFIG   pre-requisites not correct; e.g. driver not initialized
   C_WARN     error response
   C_RD_WR    malformed protocol response
   C_CHECKSUM checksum of datapool does not match
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::m_StartDiagServers(QString & orc_ErrorDetails)
{
   sint32 s32_Retval = C_NO_ERR;

   if ((this->mq_Initialized == true) && (this->mc_DiagProtocols.size() > 0UL))
   {
      uint32 u32_Counter;

      for (u32_Counter = 0U; u32_Counter < this->mc_DiagProtocols.size(); ++u32_Counter)
      {
         // Check only if datapool of node is really used
         if (this->mc_DiagNodes.find(this->mc_ActiveNodesIndexes[u32_Counter]) != this->mc_DiagNodes.end())
         {
            const C_OSCNode * pc_Node =
               C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mc_ActiveNodesIndexes[u32_Counter]);

            if (pc_Node != NULL)
            {
               if (pc_Node->c_Properties.e_DiagnosticServer == C_OSCNodeProperties::eDS_KEFEX)
               {
                  // TODO BAY: Activate when supporting Kefex on dashboard

                  /*
                  // TODO Get all information from the core

                  uint8 u8_DataPoolIndex;
                  uint16 u16_NumberOfDataPoolElements;
                  uint16 u16_DataPoolVersion;

                  u8_DataPoolIndex = 0U;
                  u16_NumberOfDataPoolElements = 0x0049U;
                  u16_DataPoolVersion = 0x0000U;
                  u32_DataPoolChecksum = 0xC4CBU;
                  s32_Return = this->mc_DiagProtocols[u32_Counter]->DataPoolVerify(u8_DataPoolIndex,
                                                                                   u16_NumberOfDataPoolElements,
                                                                                   u16_DataPoolVersion,
                                                                                   u32_DataPoolChecksum,
                                                                                   q_Match);

                  if ((s32_Return != C_NO_ERR) || (q_Match == false))
                  {
                     if ((s32_Return == C_NO_ERR) && (q_Match == false))
                     {
                        stw_scl::C_SCLString c_Error;
                        c_Error.PrintFormatted("Datapool verify failed between client and server. Node: %s " \
                                               "Datapool: %s", pc_Node->c_Properties.c_Name.c_str(),
                                               pc_Node->c_DataPools[u8_DataPoolIndex].c_Name.c_str());
                        osc_write_log_error("Starting diagnostics", c_Error);
                        // Datapool checksum does not match
                        s32_Retval = C_CHECKSUM;
                     }
                     else
                     {
                        s32_Retval = s32_Return;
                     }
                     if (s32_Retval != C_NO_ERR)
                     {
                        orc_ErrorDetails += QString("- ") + pc_Node->c_Properties.c_Name.c_str() +
                                            ", Datapool: \"" +
                                            pc_Node->c_DataPools[u8_DataPoolIndex].c_Name.c_str() + "\"\n";
                     }
                  }
                  */
               }
               else
               {
                  const sint32 s32_Return = mh_CheckDatapools(pc_Node, this->mc_DiagProtocols[u32_Counter],
                                                              orc_ErrorDetails);

                  if (s32_Return != C_NO_ERR)
                  {
                     // Do not overwrite previous errors with C_NO_ERR
                     s32_Retval = s32_Return;
                  }
               }
            }
            else
            {
               s32_Retval = C_CONFIG;
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
/*! \brief  Check of Datapools

   Each Datapool will be checked for matching name, version and CRC

   \param[in,out] orc_ErrorDetails     Details for current error

   \return
   C_NO_ERR   Datapools are as expected
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send request (e.g. TX buffer full)
   C_CONFIG   pre-requisites not correct; e.g. driver not initialized
   C_WARN     error response
   C_RD_WR    malformed protocol response
   C_CHECKSUM Checksum, name or version of Datapool does not match
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::mh_CheckDatapools(const C_OSCNode * const opc_Node,
                                             stw_opensyde_core::C_OSCDiagProtocolBase * const opc_Protocol,
                                             QString & orc_ErrorDetails)
{
   sint32 s32_Retval = C_NO_ERR;
   QString c_DataPoolErrorString = "";

   for (uint32 u32_ItDataPool = 0; u32_ItDataPool < opc_Node->c_DataPools.size(); ++u32_ItDataPool)
   {
      const C_OSCNodeDataPool & rc_Datapool = opc_Node->c_DataPools[u32_ItDataPool];
      uint32 u32_DataPoolChecksum = 0U;
      sint32 s32_Return;
      uint8 au8_Version[3];
      C_SCLString c_DatapoolName;
      QString c_ErrorReason = "";
      uint8 u8_ErrorCode;

      // Check meta data
      s32_Return = opc_Protocol->DataPoolReadMetaData(static_cast<uint8>(u32_ItDataPool),
                                                      au8_Version, c_DatapoolName, &u8_ErrorCode);

      if (s32_Return == C_NO_ERR)
      {
         // Check name if string is not empty. Empty string in case of not supported data by protocol
         if ((c_DatapoolName != "") &&
             (c_DatapoolName != rc_Datapool.c_Name))
         {
            // Name does not match
            c_ErrorReason = "The name of Datapool does not match (Client: " + QString(rc_Datapool.c_Name.c_str()) +
                            ", Server: " + QString(c_DatapoolName.c_str()) + ").";

            s32_Return = C_CHECKSUM;
         }
         // Check version
         else if (memcmp(rc_Datapool.au8_Version, au8_Version, sizeof(au8_Version)) != 0)
         {
            const QString c_VersionServer = QString("v%1.%2r%3").
                                            arg(au8_Version[0], 2, 10, QChar('0')).
                                            arg(au8_Version[1], 2, 10, QChar('0')).
                                            arg(au8_Version[2], 2, 10, QChar('0'));
            const QString c_VersionClient = QString("v%1.%2r%3").
                                            arg(rc_Datapool.au8_Version[0], 2, 10, QChar('0')).
                                            arg(rc_Datapool.au8_Version[1], 2, 10, QChar('0')).
                                            arg(rc_Datapool.au8_Version[2], 2, 10, QChar('0'));

            // Version does not match
            c_ErrorReason = "The version of Datapool " + QString(rc_Datapool.c_Name.c_str()) +
                            " does not match (Client: " + c_VersionClient +
                            ", Server: " + c_VersionServer + ").";

            s32_Return = C_CHECKSUM;
         }
         else
         {
            bool q_Match = false;

            // Check checksum
            rc_Datapool.CalcDefinitionHash(u32_DataPoolChecksum);

            s32_Return = opc_Protocol->DataPoolVerify(
               static_cast<uint8>(u32_ItDataPool),
               0U, //N/A for openSYDE protocol
               0U, //N/A for openSYDE protocol
               u32_DataPoolChecksum,
               q_Match);

            if ((q_Match == false) &&
                (s32_Return == C_NO_ERR))
            {
               // Checksum does not match
               c_ErrorReason = "The checksum of Datapool " + QString(rc_Datapool.c_Name.c_str()) +
                               " does not match.";
               s32_Return = C_CHECKSUM;
            }
            else if (s32_Return != C_NO_ERR)
            {
               // Service error
               c_ErrorReason = "The verify of the Datapool " + QString(rc_Datapool.c_Name.c_str()) +
                               " failed with error " + QString(C_OSCLoggingHandler::h_StwError(s32_Return).c_str());
            }
            else
            {
               // Nothing to do
            }
         }
      }
      else if (s32_Return == C_WARN)
      {
         if (u8_ErrorCode == C_OSCProtocolDriverOsy::hu8_NR_CODE_REQUEST_OUT_OF_RANGE)
         {
            // Special case: Datapool does not exist
            c_ErrorReason = "The Datapool " + QString(rc_Datapool.c_Name.c_str()) +
                            " does not exist on the server.";
            s32_Return = C_CHECKSUM;
         }
         else
         {
            c_ErrorReason = "The read of the Datapool meta data of Datapool " + QString(rc_Datapool.c_Name.c_str()) +
                            "failed with error " + QString(C_OSCLoggingHandler::h_StwError(s32_Return).c_str()) +
                            " and negative response code: " + QString::number(u8_ErrorCode);
         }
      }
      else
      {
         // Service error
         c_ErrorReason = "The read of the Datapool meta data of Datapool " + QString(rc_Datapool.c_Name.c_str()) +
                         "failed with error " + QString(C_OSCLoggingHandler::h_StwError(s32_Return).c_str());
      }

      if (s32_Return != C_NO_ERR)
      {
         // Verify failed
         stw_scl::C_SCLString c_Error;
         c_Error.PrintFormatted("Datapool verify failed between client and node %s. " \
                                "Reason: %s",
                                opc_Node->c_Properties.c_Name.c_str(),
                                c_ErrorReason.toStdString().c_str());
         osc_write_log_error("Starting diagnostics", c_Error);

         c_DataPoolErrorString += "\n   ";
         //Translation: 1=Datapool name
         c_DataPoolErrorString += c_ErrorReason;
         s32_Retval = s32_Return;
      }
      else
      {
         stw_scl::C_SCLString c_Text;
         c_Text.PrintFormatted("Datapool verified. Node: %s " \
                               "Datapool: %s", opc_Node->c_Properties.c_Name.c_str(), rc_Datapool.c_Name.c_str());
         osc_write_log_info("Starting diagnostics", c_Text);
      }
   }

   if (s32_Retval != C_NO_ERR)
   {
      //Translation: 1=Node name, 2=List of Datapool names
      orc_ErrorDetails += QString(C_GtGetText::h_GetText("- %1: %2\n")).arg(
         opc_Node->c_Properties.c_Name.c_str()).arg(c_DataPoolErrorString);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calls the cycle functions of all protocols

   \return
   C_NO_ERR    no problems
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDriverDiag::m_Cycle(void)
{
   for (uint32 u32_Counter = 0U; u32_Counter < this->mc_DiagProtocols.size(); ++u32_Counter)
   {
      const sint32 s32_Return = this->mc_DiagProtocols[u32_Counter]->Cycle();
      if (s32_Return != C_NO_ERR)
      {
         // TODO Errorhandling
         break;
      }
   }

   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for continuous calling by thread.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiag::mh_ThreadFunc(void * const opv_Instance)
{
   //lint -e{925}  This class is the only one which registers itself at the caller of this function. It must match.
   C_SyvComDriverDiag * const pc_ComDriver = reinterpret_cast<C_SyvComDriverDiag * const>(opv_Instance);

   tgl_assert(pc_ComDriver != NULL);
   if (pc_ComDriver != NULL)
   {
      pc_ComDriver->m_ThreadFunc();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for continuous calling by thread.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiag::m_ThreadFunc(void)
{
   static uint32 hu32_LastSentTesterPresent = 0U;
   static uint32 hu32_LastSentDebugTest = 0U;
   uint32 u32_CurrentTime;

   if (hu32_LastSentTesterPresent == 0U)
   {
      // Initialize the time scheduling
      hu32_LastSentTesterPresent = stw_tgl::TGL_GetTickCount();
      hu32_LastSentDebugTest = hu32_LastSentTesterPresent;
   }

   u32_CurrentTime = stw_tgl::TGL_GetTickCount();

   if (u32_CurrentTime > (hu32_LastSentTesterPresent + 1000U))
   {
      hu32_LastSentTesterPresent = u32_CurrentTime;
      this->SendTesterPresent();
   }
   else if (u32_CurrentTime > (hu32_LastSentDebugTest + 200U))
   {
      // For testing
      hu32_LastSentDebugTest = u32_CurrentTime;
   }
   else
   {
      // nothing to do
   }

   // Handle datapool events
   this->m_Cycle();

   // Handle CAN message / com signal input
   this->DistributeMessages();

   //rescind CPU time to other threads ...
   stw_tgl::TGL_Sleep(1);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle polling finished event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiag::m_HandlePollingFinished(void)
{
   sint32 s32_Result;
   uint8 u8_NRC;

   tgl_assert(this->mc_PollingThread.GetResults(s32_Result) == C_NO_ERR);
   tgl_assert(this->mc_PollingThread.GetNegativeResponseCode(u8_NRC) == C_NO_ERR);
   //Start with next one
   this->mc_PollingThread.AcceptNextRequest();
   Q_EMIT this->SigPollingFinished(s32_Result, u8_NRC);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get node name for active node index

   \param[in] ou32_ViewIndex       View index (identifier)
   \param[in] ou32_ActiveNodeIndex Active node index (depending on number of active nodes)

   \return
   Node name if any found
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvComDriverDiag::mh_GetNodeNameForActiveNodeIndex(const uint32 ou32_ViewIndex,
                                                             const uint32 ou32_ActiveNodeIndex)
{
   QString c_Retval;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

   if (pc_View != NULL)
   {
      uint32 u32_ActiveCounter = 0;
      const std::vector<uint8> & rc_NodeActiveFlags = pc_View->GetNodeActiveFlags();
      for (uint32 u32_It = 0; u32_It < rc_NodeActiveFlags.size(); ++u32_It)
      {
         if (rc_NodeActiveFlags[u32_It] == true)
         {
            if (u32_ActiveCounter == ou32_ActiveNodeIndex)
            {
               const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_It);
               if (pc_Node != NULL)
               {
                  c_Retval = pc_Node->c_Properties.c_Name.c_str();
               }
            }
            //Important iterator step
            ++u32_ActiveCounter;
         }
      }
   }
   return c_Retval;
}
