//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for sequences required for device configuration. (implementation)

   Strategy for GUI interaction:
   All functions are executed in a thread.
   The function call just starts the process and returns. The caller is responsible to keep the
   specified "C_OSCComDriverFlash" stable till the thread was finished.

   Intermediate results are signaled to the GUI via Qt signals. A signal is also triggered when the
   functions finish. The results are passed along with the signals.

   Technical details are logged using C_OSCLoggingHandler.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "CSCLString.h"
#include "C_SyvDcSequences.h"
#include "TGLUtils.h"
#include "TGLTime.h"
#include "C_OSCProtocolDriverOsyTpCan.h"
#include "C_OSCLoggingHandler.h"
#include "C_PuiSdHandler.h"
#include "C_SyvComDriverUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDcDeviceInformation::C_SyvDcDeviceInformation(void) :
   c_DeviceName(""),
   q_DeviceNameValid(false),
   q_SerialNumberValid(false),
   u8_NodeId(0U),
   q_NodeIdValid(false),
   q_IpAddressValid(false)
{
   (void)std::memset(&au8_SerialNumber[0], 0U, sizeof(au8_SerialNumber));
   (void)std::memset(&au8_IpAddress[0], 0U, sizeof(au8_IpAddress));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a device name and its valid flag

   \param[in]  orc_DeviceName    New device name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcDeviceInformation::SetDeviceName(const C_SCLString & orc_DeviceName)
{
   this->c_DeviceName = orc_DeviceName;
   this->q_DeviceNameValid = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a serial number and its valid flag

   \param[in]  orau8_SerialNumber   New serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcDeviceInformation::SetSerialNumber(const uint8 (&orau8_SerialNumber)[6])
{
   (void)std::memcpy(this->au8_SerialNumber, orau8_SerialNumber, sizeof(this->au8_SerialNumber));
   this->q_SerialNumberValid = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a node id and its valid flag

   \param[in]  ou8_NodeId  New node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcDeviceInformation::SetNodeId(const uint8 ou8_NodeId)
{
   this->u8_NodeId = ou8_NodeId;
   this->q_NodeIdValid = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a ip address and its valid flag

   \param[in]  orau8_IpAddress   New node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcDeviceInformation::SetIpAddress(const uint8 (&orau8_IpAddress)[4])
{
   (void)memcpy(&this->au8_IpAddress[0], &orau8_IpAddress[0], sizeof(this->au8_IpAddress));
   this->q_IpAddressValid = true;
}

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDcDeviceInformation::h_SerialNumberFromStringToArray(const C_SCLString & orc_SerialNumber,
                                                               uint8 * const opu8_SerialNumer)
{
   bool q_Return = false;
   C_SCLString c_CompleteString;

   if (opu8_SerialNumer != NULL)
   {
      //format up to and including 2019. E.g: 05.123456.1001
      if (orc_SerialNumber.Length() == 14)
      {
         SCLDynamicArray<C_SCLString> c_Tokens;
         //Get all numbers
         orc_SerialNumber.Tokenize(".", c_Tokens);
         if (c_Tokens.GetLength() == 3)
         {
            //Combine all numbers
            for (uint32 u32_ItCounter = 0U; u32_ItCounter < static_cast<uint32>(c_Tokens.GetLength()); ++u32_ItCounter)
            {
               c_CompleteString += c_Tokens[u32_ItCounter];
            }
         }
      }
      //format from 2020. E.g: 200012345678
      else if (orc_SerialNumber.Length() == 12)
      {
         //no convert needed
         c_CompleteString = orc_SerialNumber;
      }
      else
      {
         //invalid format, should never happen
         C_SCLString c_Error;
         c_Error.PrintFormatted("Invalid serial number format. String: \"%s\".", c_CompleteString.c_str());
         osc_write_log_error("Convert serial number string to array", c_Error);
      }

      //get bytes
      if (c_CompleteString.Length() == 12)
      {
         uint32 u32_ItByte = 0U;
         q_Return = true;
         //For each 2 numbers assign one byte
         for (uint32 u32_ItChar = 1U; u32_ItChar < c_CompleteString.Length(); u32_ItChar += 2)
         {
            C_SCLString c_SubString = c_CompleteString[u32_ItChar];
            c_SubString += c_CompleteString[static_cast<sintn>(u32_ItChar) + 1];
            c_SubString = "0x" + c_SubString;
            opu8_SerialNumer[u32_ItByte] = static_cast<uint8>(c_SubString.ToInt());
            //Next byte
            ++u32_ItByte;
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDcDeviceConfiguation::C_SyvDcDeviceConfiguation(void) :
   au8_SerialNumber()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Assignment operator.

   \param[in]  orc_Source  instance to assign

   \return
   reference to new instance
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDcDeviceConfiguation & C_SyvDcDeviceConfiguation::operator =(const C_SyvDcDeviceConfiguation & orc_Source)
{
   if (this != &orc_Source)
   {
      (void)std::memcpy(this->au8_SerialNumber, orc_Source.au8_SerialNumber, sizeof(this->au8_SerialNumber));
      this->c_NodeIds = orc_Source.c_NodeIds;
      this->c_BusIds = orc_Source.c_BusIds;
      this->c_CanBitrates = orc_Source.c_CanBitrates;

      this->c_IpAddresses = orc_Source.c_IpAddresses;
   }

   return (*this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDcSequences::C_SyvDcSequences(void) :
   C_OSCComSequencesBase(false),
   mpc_CanDllDispatcher(NULL),
   mpc_EthernetDispatcher(NULL),
   // No routing for device configuration
   me_Sequence(eSCANCANENTERFLASHLOADER),
   mu32_CanBitrate(0U),
   mq_ConfigureAllInterfaces(false),
   mq_RunScanSendFlashloaderRequestEndless(false),
   ms32_Result(C_NOACT)
{
   mpc_Thread = new C_SyvComDriverThread(&C_SyvDcSequences::mh_ThreadFunc, this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDcSequences::~C_SyvDcSequences(void)
{
   if (this->mpc_Thread != NULL)
   {
      if (this->mpc_Thread->isRunning() == true)
      {
         this->mpc_Thread->requestInterruption();
         if (this->mpc_Thread->wait(2000U) == false)
         {
            // Not finished yet
            osc_write_log_warning("Closing device configuration sequences",
                                  "Waiting time for stopping thread was not enough");
         }
      }
      delete mpc_Thread;
      mpc_Thread = NULL;
   }

   if (this->mpc_ComDriver != NULL)
   {
      this->mpc_ComDriver->PrepareForDestructionFlash();
   }

   if (this->mpc_CanDllDispatcher != NULL)
   {
      this->mpc_CanDllDispatcher->CAN_Exit();
      this->mpc_CanDllDispatcher->DLL_Close();
      delete mpc_CanDllDispatcher;
      mpc_CanDllDispatcher = NULL;
   }

   delete mpc_EthernetDispatcher;
   mpc_EthernetDispatcher = NULL;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all members

   \param[in]  ou32_ViewIndex    Index of current used view

   \return
   C_NO_ERR      Operation success
   C_CONFIG      Invalid system definition for parameters
   C_RD_WR       Configured communication DLL does not exist
   C_OVERFLOW    Unknown transport protocol or unknown diagnostic server for at least one node
   C_NOACT       Parameter ou32_ActiveBusIndex invalid or no active nodes
   C_COM         CAN initialization failed or no active node
   C_CHECKSUM    Internal buffer overflow detected
   C_RANGE       Routing configuration failed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::InitDcSequences(const uint32 ou32_ViewIndex)
{
   sint32 s32_Return;
   uint32 u32_ActiveBusIndex;

   std::vector<uint8> c_ActiveNodes;

   // No CAN initialization due to initialization in the sequences itself
   s32_Return = C_SyvComDriverUtil::h_GetOSCComDriverParamFromView(ou32_ViewIndex, u32_ActiveBusIndex, c_ActiveNodes,
                                                                   &this->mpc_CanDllDispatcher,
                                                                   &this->mpc_EthernetDispatcher,
                                                                   false);

   if (s32_Return == C_NO_ERR)
   {
      s32_Return = C_OSCComSequencesBase::Init(C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinition(),
                                               u32_ActiveBusIndex, c_ActiveNodes, this->mpc_CanDllDispatcher,
                                               this->mpc_EthernetDispatcher);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets all configuration information for a node

   Preconditions as input part for orc_Config:
   - au8_SerialNumber must be set
   - size of c_NodeIds must be one
   - the value in c_NodeIds is the node id for the server to configure on the current used bus

   \param[in,out]  orc_Config       Device configuration which will be completed
   \param[out]     ore_Flashloader  Detected flashloader protocol for the node

   \return
   C_NO_ERR    filling device configuration finished
   C_RANGE     device configuration is invalid
   C_CONFIG    system definition does not match to the input parameters
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::FillDeviceConfig(C_SyvDcDeviceConfiguation & orc_Config,
                                          C_OSCNodeProperties::E_FlashLoaderProtocol & ore_Flashloader) const
{
   sint32 s32_Return = C_RANGE;

   if (orc_Config.c_NodeIds.size() == 1)
   {
      C_OSCProtocolDriverOsyNode c_ServerId = this->mpc_ComDriver->GetClientId();
      uint32 u32_NodeIndex;

      s32_Return = C_CONFIG;
      c_ServerId.u8_NodeIdentifier = orc_Config.c_NodeIds[0];

      if (this->mpc_ComDriver->GetNodeIndex(c_ServerId, u32_NodeIndex) == true)
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodeIndex);
         const C_OSCSystemBus * const pc_UsedBus =
            C_PuiSdHandler::h_GetInstance()->GetOSCBus(this->mu32_ActiveBusIndex);

         if ((pc_Node != NULL) && (pc_UsedBus != NULL))
         {
            s32_Return = C_NO_ERR;

            ore_Flashloader = pc_Node->c_Properties.e_FlashLoader;

            if (ore_Flashloader == C_OSCNodeProperties::eFL_OPEN_SYDE)
            {
               uint32 u32_InterfaceCounter;

               // Configuration of the used bus associated to the set node id
               orc_Config.c_BusIds.push_back(c_ServerId.u8_BusIdentifier);
               if (pc_UsedBus->e_Type == C_OSCSystemBus::eCAN)
               {
                  orc_Config.c_CanBitrates.push_back(static_cast<uint32>(pc_UsedBus->u64_BitRate));
               }
               else
               {
                  // Search the correct com interface to get the IP address configuration
                  // The interface must exist
                  s32_Return = C_CONFIG;

                  for (u32_InterfaceCounter = 0U; u32_InterfaceCounter < pc_Node->c_Properties.c_ComInterfaces.size();
                       ++u32_InterfaceCounter)
                  {
                     const C_OSCNodeComInterfaceSettings & rc_ComInterface =
                        pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceCounter];

                     if ((rc_ComInterface.e_InterfaceType == C_OSCSystemBus::eETHERNET) &&
                         (rc_ComInterface.q_IsBusConnected == true) &&
                         (rc_ComInterface.u8_NodeID == c_ServerId.u8_NodeIdentifier) &&
                         (rc_ComInterface.u32_BusIndex == this->mu32_ActiveBusIndex))
                     {
                        orc_Config.c_IpAddresses.push_back(rc_ComInterface.c_Ip);
                        s32_Return = C_NO_ERR;
                        break;
                     }
                  }
               }

               // Configuration of the rest of the buses
               for (u32_InterfaceCounter = 0U; u32_InterfaceCounter < pc_Node->c_Properties.c_ComInterfaces.size();
                    ++u32_InterfaceCounter)
               {
                  const C_OSCNodeComInterfaceSettings & rc_ComInterface =
                     pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceCounter];

                  if ((rc_ComInterface.q_IsBusConnected == true) &&
                      ((rc_ComInterface.u8_NodeID != c_ServerId.u8_NodeIdentifier) ||
                       (rc_ComInterface.u32_BusIndex != this->mu32_ActiveBusIndex)))
                  {
                     const C_OSCSystemBus * const pc_OtherBus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
                        rc_ComInterface.u32_BusIndex);

                     if (pc_OtherBus != NULL)
                     {
                        orc_Config.c_NodeIds.push_back(rc_ComInterface.u8_NodeID);
                        orc_Config.c_BusIds.push_back(pc_OtherBus->u8_BusID);

                        if (pc_OtherBus->e_Type == C_OSCSystemBus::eCAN)
                        {
                           orc_Config.c_CanBitrates.push_back(static_cast<uint32>(pc_OtherBus->u64_BitRate));
                        }
                        else
                        {
                           orc_Config.c_IpAddresses.push_back(rc_ComInterface.c_Ip);
                        }
                     }
                     else
                     {
                        // Error: Bus was not found
                        s32_Return = C_CONFIG;
                     }
                  }
               }
            }
            else
            {
               //Flashloader type "none" is supported by the device structure, but not the UI
               tgl_assert(ore_Flashloader == C_OSCNodeProperties::eFL_STW);

               if (pc_UsedBus->e_Type == C_OSCSystemBus::eCAN)
               {
                  // STW Flashloader can only be configured on the current used interface
                  orc_Config.c_BusIds.push_back(0);
                  orc_Config.c_CanBitrates.push_back(static_cast<uint32>(pc_UsedBus->u64_BitRate));
               }
               else
               {
                  // Error: STW flashloader does not support Ethernet
                  s32_Return = C_CONFIG;
               }
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to bring devices in flashloader state

   Supports openSYDE CAN-TP and STW Flashloader protocols.

   Concrete implementation is in function m_RunScanCanEnterFlashloader

   \param[in]  ou32_UsedBitrate  Bitrate in kbit/s of the previous configuration of the devices

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::ScanCanEnterFlashloader(const uint32 ou32_UsedBitrate)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eSCANCANENTERFLASHLOADER;
      this->mu32_CanBitrate = ou32_UsedBitrate;
      this->mpc_Thread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to send flashloader request messages

   Supports openSYDE CAN-TP and STW Flashloader protocols.

   Concrete implementation is in function m_RunScanCanSendFlashloaderReqeust

   \param[in]  ou32_ScanTime     Time till stop sending the requests in ms
   \param[in]  oq_ScanEndless    Flag if sending of the request shall not stop if the time is over
                                 Can be stopped by StopScanCanSendFlashloaderRequest

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::ScanCanSendFlashloaderRequest(const uint32 ou32_ScanTime, const bool oq_ScanEndless)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eSCANCANSENDFLASHLOADERREQUEST;
      this->mu32_ScanTime = ou32_ScanTime;
      this->mq_RunScanSendFlashloaderRequestEndless = oq_ScanEndless;
      this->mpc_Thread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the flag for stopping ScanCanSendFlashloaderRequest

   When ScanCanSendFlashloaderRequest is still running an the flag mq_RunScanSendFlashloaderRequestEndless
   is not set to false, the function sets it to true.
   The ScanCanSendFlashloaderRequest will then be stopped when the scan time is over too.

   This function is thread safe
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcSequences::StopScanCanSendFlashloaderRequest(void)
{
   this->mc_CriticalSection.Acquire();
   this->mq_RunScanSendFlashloaderRequestEndless = false;
   this->mc_CriticalSection.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to read information from STW Flashloader devices.

   Result of sequence can be get by calling GetDeviceInfosResult

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::ScanCanGetInfoFromStwFlashloaderDevices(void)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eSCANCANGETINFOFROMSTWFLASHLOADERDEVICES;
      this->mpc_Thread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to read information from openSYDE devices.

   Supports openSYDE CAN-TP protocol.
   Result of sequence can be get by calling GetDeviceInfosResult

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::ScanCanGetInfoFromOpenSydeDevices(void)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eSCANCANGETINFOFROMOPENSYDEDEVICES;
      this->mpc_Thread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to read information from openSYDE nodes.

   Result of sequence can be get by calling GetDeviceInfosResult

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::ScanEthGetInfoFromOpenSydeDevices(void)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eSCANETHGETINFOFROMOPENSYDEDEVICES;
      this->mpc_Thread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Precheck of the openSYDE devices configuration

   Function returns immediately with the result. It will not be run in the separate thread.

   This function shall be called before calling ConfCanStwFlashloaderDevices, ConfEthFlashloaderDevices,
      ConfCanOpenSydeDevices.

   For CAN:
   When the calling order is
   1. CheckOpenSydeDevicesConfig
   2. ConfCanStwFlashloaderDevices (Checks the configuration itself)
   3. ConfCanOpenSydeDevices
   no changes will be made on the server if at least one configuration is invalid.

   \param[in]  orc_DeviceConfig  Configuration for all openSYDE devices and their communication interfaces

   \return
   C_NO_ERR    all devices are configured
   C_NOACT     no device configurations exist
   C_RANGE     device configuration is invalid
   C_CONFIG    no com driver installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::CheckOpenSydeDevicesConfig(const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfig)
const
{
   return this->m_CheckConfOpenSydeDevices(orc_DeviceConfig);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to write new configuration to STW Flashloader devices.

   This function shall be called after calling CheckCanOpenSydeDevicesConfig and before ConfCanOpenSydeDevices.
   When the calling order is
   1. CheckCanOpenSydeDevicesConfig
   2. ConfCanStwFlashloaderDevices (Checks the configuration itself)
   3. ConfCanOpenSydeDevices
   no changes will be made on the server if minimum one configuration is invalid.

   \param[in]  orc_DeviceConfig  Configuration for all STW flashloader devices

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::ConfCanStwFlashloaderDevices(const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfig)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eCONFCANSTWFLASHLOADERDEVICES;
      this->mc_DeviceConfiguration.clear();
      this->mc_DeviceConfiguration = orc_DeviceConfig;
      this->mpc_Thread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to write new configuration to openSYDE devices via CAN-TP.

   This function shall be called after calling CheckCanOpenSydeDevicesConfig and ConfCanStwFlashloaderDevices.
   When the calling order is
   1. CheckCanOpenSydeDevicesConfig
   2. ConfCanStwFlashloaderDevices (Checks the configuration itself)
   3. ConfCanOpenSydeDevices
   no changes will be made on the server if minimum one configuration is invalid.

   \param[in]  orc_DeviceConfig           Configuration for all openSYDE devices and its
                                          communication interfaces
   \param[in]  oq_ConfigureAllInterfaces  Flag if the settings of all connected interfaces shall be set
                                          or only the settings of the used CAN bus

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::ConfCanOpenSydeDevices(const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfig,
                                                const bool oq_ConfigureAllInterfaces)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eCONFCANOPENSYDEDEVICES;
      this->mc_DeviceConfiguration.clear();
      this->mc_DeviceConfiguration = orc_DeviceConfig;
      this->mq_ConfigureAllInterfaces = oq_ConfigureAllInterfaces;
      this->mpc_Thread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to write new configuration to openSYDE devices via ETH-TP.

   \param[in]  orc_DeviceConfig           Configuration for all openSYDE devices and its
                                          communication interfaces
   \param[in]  oq_ConfigureAllInterfaces  Flag if the settings of all connected interfaces shall be set
                                          or only the settings of the used Ethernet bus

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::ConfEthOpenSydeDevices(const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfig,
                                                const bool oq_ConfigureAllInterfaces)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eCONFETHOPENSYDEDEVICES;
      this->mc_DeviceConfiguration.clear();
      this->mc_DeviceConfiguration = orc_DeviceConfig;
      this->mq_ConfigureAllInterfaces = oq_ConfigureAllInterfaces;
      this->mpc_Thread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends the service broadcast requestProgramming

   \param[out]  orq_NotAccepted  true: at least one "not accepted" response was received
                                 false: no "not accepted" response was received

   \return
   C_NO_ERR   Request programming flag sent and set
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_CONFIG   no transport protocol installed or broadcast protocol not initialized
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::SendOsyBroadcastRequestProgramming(bool & orq_NotAccepted) const
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      s32_Return = this->mpc_ComDriver->SendOsyBroadcastRequestProgramming(orq_NotAccepted);

      if (s32_Return == C_NO_ERR)
      {
         osc_write_log_info("Send openSYDE request programming flag",
                            "openSYDE broadcast request programming flag sent.");
      }
      else
      {
         osc_write_log_error("Send openSYDE request programming flag",
                             "Sending openSYDE broadcast request programming flag failed with error: " +
                             C_SCLString::IntToStr(s32_Return));
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resets all STW flashloader devices

   Function returns immediately with the result. It will not be run in the separate thread.

   The client bitrate of the bus will be reset to the purposed bitrate.
   The initialization of the CAN bus will be made by ReadBackCan and ScanCanEnterFlashloader.

   This function shall be called only, if only STW flashloader devices are connected to the bus.

   \return
   C_NO_ERR    Net reset request sent
   C_BUSY      Previously started sequence still going on
   C_COM       Error on sending reset request
   C_CONFIG    No com driver installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::ResetCanStwFlashloaderDevices(void)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      s32_Return = this->mpc_ComDriver->SendStwNetReset();

      if (s32_Return == C_NO_ERR)
      {
         osc_write_log_info("Configure CAN openSYDE devices", "openSYDE broadcast ECU reset sent.");
      }
      else
      {
         osc_write_log_error("Configure CAN openSYDE devices",
                             "openSYDE broadcast ECU reset failed with error: " + C_SCLString::IntToStr(s32_Return));
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resets all openSYDE devices

   Function returns immediately with the result. It will not be run in the separate thread.

   This function shall be called only, if only openSYDE devices are connected to the bus.

   In case of CAN:
   The client bitrate of the bus will be reset to the purposed bitrate.
   The initialization of the CAN bus will be made by ReadBackCan and ScanCanEnterFlashloader.

   \param[in]  oq_ToFlashloader  Flag for reset to flashloader

   \return
   C_NO_ERR   no problems
   C_BUSY      previously started sequence still going on
   C_COM       could not send request
   C_CONFIG    no dispatcher installed or broadcast protocol not initialized
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::ResetOpenSydeDevices(const bool oq_ToFlashloader) const
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      uint8 u8_ResetType;

      if (oq_ToFlashloader == true)
      {
         u8_ResetType = C_OSCProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_RESET_TO_FLASHLOADER;
      }
      else
      {
         u8_ResetType = C_OSCProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_KEY_OFF_ON;
      }

      // Disconnect from all nodes before reset if still connected. Because of broadcast no active connection necessary
      // In case of CAN it makes no difference
      this->mpc_ComDriver->DisconnectNodes();

      s32_Return = this->mpc_ComDriver->SendOsyBroadcastEcuReset(u8_ResetType);
      if (s32_Return == C_NO_ERR)
      {
         osc_write_log_info("Reset openSYDE devices", "openSYDE broadcast ECU reset sent.");
      }
      else
      {
         osc_write_log_error("Reset openSYDE devices",
                             "openSYDE broadcast ECU reset failed with error: " + C_SCLString::IntToStr(s32_Return));
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the new CAN bitrate

   \param[in]  ou32_Bitrate   Bitrate in kbit/s

   \return
   C_NO_ERR    Bitrate set
   C_BUSY      previously started sequence still going on
   C_COM       Error on reading bitrate
   C_CONFIG    No dispatcher installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::InitCanAndSetCanBitrate(const stw_types::uint32 ou32_Bitrate)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      s32_Return = this->mpc_ComDriver->InitCanAndSetCanBitrate(ou32_Bitrate);
      if (s32_Return == C_NO_ERR)
      {
         osc_write_log_info("Init CAN and set new bitrate", "CAN initialized successfully.");
      }
      else
      {
         osc_write_log_error("Init CAN and set new bitrate",
                             "CAN initialization failed: " + C_SCLString::IntToStr(s32_Return));
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to read back information from configured nodes via CAN.

   Result of sequence can be get by calling GetDeviceInfosResult

   \param[in]  orc_OpenSydeIds   Ids for openSYDE flashloader to read
   \param[in]  orc_StwIds        Ids for STW flashloader to read

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::ReadBackCan(const std::vector<C_OSCProtocolDriverOsyNode> & orc_OpenSydeIds,
                                     const std::vector<C_OSCProtocolDriverOsyNode> & orc_StwIds)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eREADBACKCAN;
      this->mc_OpenSydeIds.clear();
      this->mc_StwIds.clear();
      this->mc_OpenSydeIds = orc_OpenSydeIds;
      this->mc_StwIds = orc_StwIds;

      this->mpc_Thread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to read back information from configured nodes via Ethernet.

   Result of sequence can be get by calling GetDeviceInfosResult

   \param[in]  orc_OpenSydeIds   Ids for openSYDE flashloader to read

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::ReadBackEth(const std::vector<C_OSCProtocolDriverOsyNode> & orc_OpenSydeIds)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eREADBACKETH;
      this->mc_OpenSydeIds.clear();
      this->mc_StwIds.clear();
      this->mc_OpenSydeIds = orc_OpenSydeIds;
      this->mpc_Thread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get result of previously started service execution

   Can be used to extract the results of one service execution after it has finished.

   \param[out]  ors32_Result  result code of executed service function
                              for possible values see the DataDealer's function documentation

   \return
   C_NO_ERR       result code read
   C_BUSY         previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::GetResults(sint32 & ors32_Result) const
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      ors32_Result = this->ms32_Result;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the device information result

   \param[out]  orc_DeviceInfo   Vector for sequence results

   \return
   C_NO_ERR       result code read
   C_BUSY         previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::GetDeviceInfosResult(std::vector<C_SyvDcDeviceInformation> & orc_DeviceInfo) const
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      // Copy the result vector
      orc_DeviceInfo = this->mc_DeviceInfoResult;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle reports from STW Flashloader driver

   The functions we use for the device configuration is not necessary.
   "Sending FLASH request" would spam the log file.

   \param[in]  ou8_Progress   reported progress of operation (0..100)
   \param[in]  orc_Text       reported information

   \return
   C_NO_ERR       continue with procedure
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_XflReportProgress(const uint8 ou8_Progress, const C_SCLString & orc_Text)
{
   (void)ou8_Progress;
   (void)orc_Text;
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports the progress of the STW flashloader configuration sequence

   \param[in]  ou32_Progress  Progress of sequence
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcSequences::m_RunConfCanStwFlashloaderDevicesProgress(const uint32 ou32_Progress)
{
   Q_EMIT (this->SigRunConfCanStwFlashloaderDevicesProgress(ou32_Progress));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports the progress of the openSYDE CAN configuration sequence

   \param[in]  ou32_Progress  Progress of sequence
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcSequences::m_RunConfCanOpenSydeDevicesProgress(const uint32 ou32_Progress)
{
   Q_EMIT (this->SigRunConfCanOpenSydeDevicesProgress(ou32_Progress));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports the progress of the openSYDE Ethernet configuration sequence

   \param[in]  ou32_Progress  Progress of sequence
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcSequences::m_RunConfEthOpenSydeDevicesProgress(const uint32 ou32_Progress)
{
   Q_EMIT (this->SigRunConfEthOpenSydeDevicesProgress(ou32_Progress));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports the state of a concrete step of STW flashloader configuration sequence

   \param[in]  ou32_Step      Step of node configuration
                              - hu32_SETNODEID
                              - hu32_SETCANBITRATE
                              - hu32_SETIPADDRESS
   \param[in]  os32_Result    Result of service
   \param[in]  orc_Server     Configured server
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcSequences::m_RunConfCanStwFlashloaderDevicesState(const uint32 ou32_Step, const sint32 os32_Result,
                                                              const C_OSCProtocolDriverOsyNode & orc_Server) const
{
   Q_EMIT this->SigRunConfCanStwFlashloaderDevicesState(ou32_Step, os32_Result, orc_Server.u8_BusIdentifier,
                                                        orc_Server.u8_NodeIdentifier);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports the state of a concrete step of openSYDE configuration sequence

   \param[in]  ou32_Step            Step of node configuration
                                    - hu32_SETNODEID
                                    - hu32_SETCANBITRATE
                                    - hu32_SETIPADDRESS
   \param[in]  os32_Result          Result of service
   \param[in]  orc_Server           Configured server interface
                                    Node ID and Bus ID of the configured interface,
                                    must not be the connected interface
   \param[in]  oe_InterfaceType     Interface type
   \param[in]  ou8_InterfaceNumber  Interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcSequences::m_RunConfOpenSydeDevicesState(const uint32 ou32_Step, const sint32 os32_Result,
                                                     const C_OSCProtocolDriverOsyNode & orc_Server,
                                                     const C_OSCSystemBus::E_Type oe_InterfaceType,
                                                     const uint8 ou8_InterfaceNumber) const
{
   Q_EMIT this->SigRunConfOpenSydeDevicesState(ou32_Step, os32_Result, orc_Server.u8_BusIdentifier,
                                               orc_Server.u8_NodeIdentifier,
                                               static_cast<uint8>(oe_InterfaceType), ou8_InterfaceNumber);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcSequences::mh_ThreadFunc(void * const opv_Instance)
{
   //lint -e{9079}  This class is the only one which registers itself at the caller of this function. It must match.
   C_SyvDcSequences * const pc_Sequences = reinterpret_cast<C_SyvDcSequences *>(opv_Instance);

   tgl_assert(pc_Sequences != NULL);
   if (pc_Sequences != NULL)
   {
      pc_Sequences->m_ThreadFunc();
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcSequences::m_ThreadFunc(void)
{
   if (this->mpc_ComDriver != NULL)
   {
      switch (this->me_Sequence)
      {
      case eSCANCANENTERFLASHLOADER:
         this->ms32_Result = this->m_RunScanCanEnterFlashloader(this->mu32_CanBitrate);
         break;
      case eSCANCANSENDFLASHLOADERREQUEST:
         this->ms32_Result = this->m_RunScanCanSendFlashloaderRequest(this->mu32_ScanTime);
         break;
      case eSCANCANGETINFOFROMSTWFLASHLOADERDEVICES:
         this->ms32_Result = this->m_RunScanCanGetInfoFromStwFlashloaderDevices();
         break;
      case eSCANCANGETINFOFROMOPENSYDEDEVICES:
         this->ms32_Result = this->m_RunScanCanGetInfoFromOpenSydeDevices();
         break;
      case eSCANETHGETINFOFROMOPENSYDEDEVICES:
         this->ms32_Result = this->m_RunScanEthGetInfoFromOpenSydeDevices();
         break;
      case eCONFCANSTWFLASHLOADERDEVICES:
         this->ms32_Result = this->m_RunConfCanStwFlashloaderDevices();
         break;
      case eCONFCANOPENSYDEDEVICES:
         this->ms32_Result = this->m_RunConfCanOpenSydeDevices();
         break;
      case eCONFETHOPENSYDEDEVICES:
         this->ms32_Result = this->m_RunConfEthOpenSydeDevices();
         break;
      case eREADBACKCAN:
         this->ms32_Result = this->m_ReadBackCan();
         break;
      case eREADBACKETH:
         this->ms32_Result = this->m_ReadBackEth();
         break;
      default:
         tgl_assert(false);
         break;
      }
   }
   else
   {
      this->ms32_Result = C_CONFIG;
   }

   this->mpc_Thread->requestInterruption();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Bring devices in flashloader state

   Supports openSYDE CAN-TP and STW Flashloader protocols.
   Sending of services depends from if openSYDE or STW flashloader devices are active.

   Sequence:
   * send openSYDE CAN-TP broadcast "RequestProgramming"
   * send openSYDE broadcast "EcuReset"
   * send STW Flashloader reset messages
   * in parallel: send STW Flashloader "FLASH" and
     openSYDE "DiagnosticSessionControl(PreProgramming)" broadcast for 5 seconds every 5 milliseconds

   The com driver passed as parameter is expected to be initialized and have:
   * the correct number of nodes on the CAN bus we are connected to
   * instanced protocol drivers for openSYDE and STW Flashloader nodes on that CAN bus

   \param[in]  ou32_CanBitrate   CAN bitrate

   \return
   C_NO_ERR   Sequence finished
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed or no dispatcher installed
              no com driver installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_RANGE    Broadcast protocol not initialized
   C_COM      could not send request
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_RunScanCanEnterFlashloader(const uint32 ou32_CanBitrate)
{
   sint32 s32_Return = C_CONFIG;

   osc_write_log_info("Scan CAN enter Flashloader", "Sequence started");

   if (this->mpc_ComDriver != NULL)
   {
      // Init the CAN bus with the CAN bitrate
      s32_Return = this->mpc_ComDriver->InitCanAndSetCanBitrate(ou32_CanBitrate);

      if (s32_Return == C_NO_ERR)
      {
         bool q_RequestNotAccepted = false;

         if (this->mq_OpenSydeDevicesActive == true)
         {
            // send openSYDE CAN-TP broadcast "RequestProgramming"
            s32_Return = this->mpc_ComDriver->SendOsyBroadcastRequestProgramming(q_RequestNotAccepted);
         }

         if ((s32_Return == C_NO_ERR) && (q_RequestNotAccepted == false))
         {
            if (this->mq_OpenSydeDevicesActive == true)
            {
               // send openSYDE broadcast "EcuReset"
               s32_Return = this->mpc_ComDriver->SendOsyBroadcastEcuReset(
                  C_OSCProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_RESET_TO_FLASHLOADER);
            }

            if (s32_Return == C_NO_ERR)
            {
               if (this->mq_StwFlashloaderDevicesActiveOnLocalBus == true)
               {
                  // send STW Flashloader reset messages
                  s32_Return = this->mpc_ComDriver->SendStwRequestNodeReset();
               }
            }
            else
            {
               osc_write_log_error("Scan CAN enter Flashloader",
                                   "STW request node reset failed with error: " + C_SCLString::IntToStr(s32_Return));
            }
         }
         else
         {
            osc_write_log_error("Scan CAN enter Flashloader",
                                "openSYDE ECU reset broadcast failed with error: " + C_SCLString::IntToStr(s32_Return));
         }
      }
      else
      {
         osc_write_log_error("Scan CAN enter Flashloader",
                             "Setting the bitrate on CAN bus failed with error: " + C_SCLString::IntToStr(s32_Return));
      }

      if (s32_Return == C_NO_ERR)
      {
         uint32 u32_WaitTime = this->GetMinimumFlashloaderResetWaitTime(C_OSCComDriverFlash::eNO_CHANGES_CAN);

         if (u32_WaitTime < mhu32_DEFAULT_SCAN_TIME_MS)
         {
            // The scan time is necessary for the manual triggering of the nodes
            u32_WaitTime = mhu32_DEFAULT_SCAN_TIME_MS;
         }

         this->mq_RunScanSendFlashloaderRequestEndless = false;
         s32_Return = this->m_RunScanCanSendFlashloaderRequest(u32_WaitTime);
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info("Scan CAN enter Flashloader", "Sequence finished");
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sending of "Flash" and "PreProgrammingSession" request

   Sending the STW flashloader request "FLASH" and the openSYDE broadcast service PreProgrammingSession
   till at least the scan time is over and the flag mq_RunScanSendFlashloaderRequestEndless is set to false.

   \param[in]  ou32_ScanTime  Time till stop sending the requests in ms

   \return
   C_NO_ERR   Sequence finished
   C_CONFIG   no transport protocol installed or no dispatcher installed
              no com driver installed
   C_COM      could not send request
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_RunScanCanSendFlashloaderRequest(const uint32 ou32_ScanTime)
{
   sint32 s32_Return = C_NO_ERR;
   const uint32 u32_StartTime = stw_tgl::TGL_GetTickCount();
   uint32 u32_CurTime;
   bool q_RunEndless;

   do
   {
      // send STW Flashloader "FLASH" and
      if (this->mq_StwFlashloaderDevicesActiveOnLocalBus == true)
      {
         s32_Return = this->mpc_ComDriver->SendStwSendFlash(this->mc_StwFlashloaderDeviceOnLocalBus);
      }

      if (s32_Return == C_NO_ERR)
      {
         if (this->mq_OpenSydeDevicesActive == true)
         {
            // openSYDE "DiagnosticSessionControl(PreProgramming)" broadcast for 5 seconds every 5 milliseconds
            s32_Return = this->mpc_ComDriver->SendOsyCanBroadcastEnterPreProgrammingSession();
         }
      }
      else
      {
         osc_write_log_error("Scan CAN enter Flashloader",
                             "STW send flash failed with error: " + C_SCLString::IntToStr(s32_Return));
      }

      stw_tgl::TGL_Sleep(5);

      // Possible answers are not necessary and can disturb next services
      this->mpc_ComDriver->ClearDispatcherQueue();

      if (s32_Return == C_NO_ERR)
      {
         u32_CurTime = stw_tgl::TGL_GetTickCount();
      }
      else
      {
         osc_write_log_error("Scan CAN enter Flashloader",
                             "openSYDE diagnostic session control broadcast failed with error: " +
                             C_SCLString::IntToStr(s32_Return));
         break;
      }

      this->mc_CriticalSection.Acquire();
      // Check flag, which can be set by other thread at any time
      q_RunEndless = this->mq_RunScanSendFlashloaderRequestEndless;
      this->mc_CriticalSection.Release();
   }
   while ((q_RunEndless == true) ||
          (u32_CurTime < (ou32_ScanTime + u32_StartTime)));

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read information from STW Flashloader devices.

   Assumptions:
   * All devices are in flashloader mode.
   * The com driver passed as parameter is initialized and has set up protocol instances for all STW Flashloader nodes

   Sequence
   * send "node_wakeup(localId)" with local IDs 0x00 to 0xFE
   * check for responses
   * for all nodes with responses:
   ** perform "node_wakeup(localId)"
   ** perform "node_companyid(Y*)"
   ** perform "get_serial_number"; collect all responses
   ** for all nodes with responses do:
   *** perform "node_wakeup(serial_number)"
   *** perform "node_companyid(Y*)"
   *** perform "get_device_id"

   As a result we'll have the SNRs, Node-IDs, device-names

   \return
   C_NO_ERR   information successfully read
   C_COM      error on sending
   C_NOACT    error response from server
   C_CONFIG   no com driver installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_RunScanCanGetInfoFromStwFlashloaderDevices(void)
{
   sint32 s32_Return = C_CONFIG;

   osc_write_log_info("Scan CAN get info from STW Flashloader devices", "Sequence started");

   // Clear old results
   this->mc_DeviceInfoResult.clear();

   if (this->mpc_ComDriver != NULL)
   {
      uint8 au8_LocalIdsFound[stw_diag_lib::XFL_NUM_DIFFERENT_LOCAL_IDS];
      uint8 u8_FoundNodes = 0;

      // * send "node_wakeup(localId)" with local IDs 0x00 to 0xFE
      s32_Return = this->mpc_ComDriver->SendStwSearchId(au8_LocalIdsFound, u8_FoundNodes);

      if (s32_Return == C_NO_ERR)
      {
         osc_write_log_info("Scan CAN get info from STW Flashloader devices",
                            "Nodes found: " + C_SCLString::IntToStr(u8_FoundNodes));

         for (uint16 u16_LocalIdCounter = 0U;
              u16_LocalIdCounter < stw_diag_lib::XFL_NUM_DIFFERENT_LOCAL_IDS;
              ++u16_LocalIdCounter)
         {
            if (au8_LocalIdsFound[u16_LocalIdCounter] > 0)
            {
               s32_Return = this->m_RunScanCanGetInfoFromStwFlashloaderDevice(static_cast<uint8>(u16_LocalIdCounter));

               if (s32_Return != C_NO_ERR)
               {
                  break;
               }
            }
         }
      }
      else
      {
         osc_write_log_error("Scan CAN get info from STW Flashloader devices",
                             "STW Flashloader search id failed with error: " + C_SCLString::IntToStr(s32_Return));
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info("Scan CAN get info from STW Flashloader devices", "Sequence finished");
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_RunScanCanGetInfoFromStwFlashloaderDevice(const uint8 ou8_LocalId)
{
   sint32 s32_Return = C_CONFIG;

   if (this->mpc_ComDriver != NULL)
   {
      // We need the correct bus id for the SyvComDriver.
      C_OSCProtocolDriverOsyNode c_ServerId = this->mpc_ComDriver->GetClientId();

      c_ServerId.u8_NodeIdentifier = ou8_LocalId;
      // ** perform "node_wakeup(localId)"
      //** perform "node_companyid(Y*)"
      s32_Return = this->mpc_ComDriver->SendStwWakeupLocalId(c_ServerId, NULL);

      if (s32_Return == C_NO_ERR)
      {
         //maximum allowed: 100 ECU with same ID, really should be enough
         const uint8 u8_MAX_NUM_ECUS_PER_LOCAL_ID = 100U;
         uint8 aau8_SNRs[u8_MAX_NUM_ECUS_PER_LOCAL_ID][6];
         uint8 u8_NodesFound = 0;

         //** perform "get_serial_number"; collect all responses
         s32_Return = this->mpc_ComDriver->SendStwGetSerialNumbers(c_ServerId, &aau8_SNRs[0][0],
                                                                   u8_MAX_NUM_ECUS_PER_LOCAL_ID, u8_NodesFound);

         if (s32_Return == C_NO_ERR)
         {
            for (uint8 u8_NodeCounter = 0U; u8_NodeCounter < u8_NodesFound; ++u8_NodeCounter)
            {
               uint8 u8_LocalId;
               //*** perform "node_wakeup(serial_number)"
               //*** perform "node_companyid(Y*)"
               s32_Return = this->mpc_ComDriver->SendStwWakeupLocalSerialNumber(aau8_SNRs[u8_NodeCounter], u8_LocalId);

               if (s32_Return == C_NO_ERR)
               {
                  C_SCLString c_DeviceName;
                  c_ServerId.u8_NodeIdentifier = u8_LocalId;

                  //*** perform "get_device_id"
                  s32_Return = this->mpc_ComDriver->SendStwGetDeviceId(c_ServerId, c_DeviceName);

                  if (s32_Return == C_NO_ERR)
                  {
                     C_SyvDcDeviceInformation c_DeviceInfo;
                     c_DeviceInfo.SetNodeId(c_ServerId.u8_NodeIdentifier);
                     c_DeviceInfo.SetSerialNumber(aau8_SNRs[u8_NodeCounter]);
                     c_DeviceInfo.SetDeviceName(c_DeviceName);
                     this->mc_DeviceInfoResult.push_back(c_DeviceInfo);
                  }
                  else
                  {
                     osc_write_log_error("Scan CAN get info from STW Flashloader devices",
                                         "STW Flashloader get device id failed with error: " +
                                         C_SCLString::IntToStr(s32_Return));
                  }
               }
               else
               {
                  osc_write_log_error("Scan CAN get info from STW Flashloader devices",
                                      "STW Flashloader wakeup with serial number failed with error: " + C_SCLString::IntToStr(
                                         s32_Return));
               }

               if (s32_Return != C_NO_ERR)
               {
                  break;
               }
            }
         }
         else
         {
            osc_write_log_error("Scan CAN get info from STW Flashloader devices",
                                "STW Flashloader get serial number failed with error: " +
                                C_SCLString::IntToStr(s32_Return));
         }
      }
      else
      {
         osc_write_log_error("Scan CAN get info from STW Flashloader devices",
                             "STW Flashloader wakeup with local id failed with error: " +
                             C_SCLString::IntToStr(s32_Return));
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read information from openSYDE devices.

   Supports openSYDE CAN-TP protocol.

   Assumptions:
   * All devices are in flashloader mode.
   * The com driver passed as parameter is initialized and has set up protocol instances for all openSYDE nodes

   Sequence:
   Protocol service sequence:
   * broadcast: "ReadSerialNumber"
   * for all nodes that have a unique ID:
   ** directed "readdatabyID(DeviceName)"

   As a result we'll have the SNRs, Node-IDs and possibly device-names

   \return
   C_NO_ERR   information successfully read
   C_COM      could not send request
   C_CONFIG   no dispatcher installed
              no com driver installed
   C_RANGE    Broadcast protocol not initialized
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_RunScanCanGetInfoFromOpenSydeDevices(void)
{
   sint32 s32_Return = C_CONFIG;

   osc_write_log_info("Scan CAN get info from openSYDE devices", "Sequence started");

   // Clear old results
   this->mc_DeviceInfoResult.clear();

   if (this->mpc_ComDriver != NULL)
   {
      std::vector<C_OSCProtocolDriverOsyTpCan::C_BroadcastReadEcuSerialNumberResults> c_ReadSnResult;

      // * broadcast: "ReadSerialNumber"
      s32_Return = this->mpc_ComDriver->SendOsyCanBroadcastReadSerialNumber(c_ReadSnResult);

      if (s32_Return == C_NO_ERR)
      {
         uint32 u32_ResultCounter;

         std::vector<uint32> c_UniqueIdIndices;
         uint32 u32_UniqueIdIndicesCounter;

         osc_write_log_info("Scan CAN get info from openSYDE devices",
                            "Sequence finished. Nodes found: " + C_SCLString::IntToStr(c_ReadSnResult.size()));

         // Fill the result container
         for (u32_ResultCounter = 0U; u32_ResultCounter < c_ReadSnResult.size(); ++u32_ResultCounter)
         {
            C_SyvDcDeviceInformation c_DeviceInfo;
            c_DeviceInfo.SetSerialNumber(c_ReadSnResult[u32_ResultCounter].au8_SerialNumber);
            c_DeviceInfo.SetNodeId(c_ReadSnResult[u32_ResultCounter].c_SenderId.u8_NodeIdentifier);
            this->mc_DeviceInfoResult.push_back(c_DeviceInfo);
         }

         // Search all unique ID's
         for (u32_ResultCounter = 0U; u32_ResultCounter < mc_DeviceInfoResult.size(); ++u32_ResultCounter)
         {
            bool q_UniqueId = true;

            for (uint32 u32_UniqueIdCheckCounter = 0U;
                 u32_UniqueIdCheckCounter < mc_DeviceInfoResult.size();
                 ++u32_UniqueIdCheckCounter)
            {
               if ((u32_ResultCounter != u32_UniqueIdCheckCounter) &&
                   (this->mc_DeviceInfoResult[u32_ResultCounter].u8_NodeId ==
                    this->mc_DeviceInfoResult[u32_UniqueIdCheckCounter].u8_NodeId))
               {
                  q_UniqueId = false;
                  break;
               }
            }

            if (q_UniqueId == true)
            {
               c_UniqueIdIndices.push_back(u32_ResultCounter);
            }
         }

         tgl_assert(c_ReadSnResult.size() == mc_DeviceInfoResult.size());

         // Get the device names for all devices with unique ids
         for (u32_UniqueIdIndicesCounter = 0U;
              u32_UniqueIdIndicesCounter < c_UniqueIdIndices.size();
              ++u32_UniqueIdIndicesCounter)
         {
            // ** directed "readdatabyID(DeviceName)"
            C_SCLString c_Result;
            this->mpc_ComDriver->SendOsyReadDeviceName(
               c_ReadSnResult[c_UniqueIdIndices[u32_UniqueIdIndicesCounter]].c_SenderId,
               c_Result);

            this->mc_DeviceInfoResult[c_UniqueIdIndices[u32_UniqueIdIndicesCounter]].SetDeviceName(c_Result);
         }
      }
      else
      {
         osc_write_log_error("Scan CAN get info from openSYDE devices",
                             "openSYDE serial number broadcast failed with error: " +
                             C_SCLString::IntToStr(s32_Return));
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info("Scan CAN get info from openSYDE devices", "Sequence finished");
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read information from openSYDE devices via Ethernet

   Bring devices into flashloader and read device information.
   Supports openSYDE ETH-TP protocol.

   Assumptions:
   * The com driver passed as parameter is initialized and has set up protocol instances for all openSYDE nodes

   Sequence:
   * broadcast: "RequestProgramming"
   * broadcast: "net reset"
   * wait a little (all nodes should now be in the default session of the flashloader)
   * broadcast: "get device info" (returns serial number, device name, node-id, IP)

   As a result we'll have the SNRs, Node-IDs, IPs, device-names

   \return
   C_NO_ERR   information successfully read
   C_COM      could not send request
   C_CONFIG   no dispatcher installed
              no com driver installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_RunScanEthGetInfoFromOpenSydeDevices(void)
{
   sint32 s32_Return = C_CONFIG;

   osc_write_log_info("Scan ETH get info from openSYDE devices", "Sequence started");

   // Clear old results
   this->mc_DeviceInfoResult.clear();

   if (this->mpc_ComDriver != NULL)
   {
      //broadcast "RequestProgramming"
      bool q_NotAccepted;
      s32_Return = this->mpc_ComDriver->SendOsyBroadcastRequestProgramming(q_NotAccepted);
      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error("Scan ETH get info from openSYDE devices",
                             "openSYDE request programming broadcast failed with error: " +
                             C_SCLString::IntToStr(s32_Return));
      }
      else if (q_NotAccepted == true)
      {
         osc_write_log_error("Scan ETH get info from openSYDE devices",
                             "openSYDE request programming broadcast failed. Not accepted by at least one node.");
      }
      else
      {
         //broadcast "net reset"
         s32_Return = this->mpc_ComDriver->SendOsyBroadcastEcuReset(
            C_OSCProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_RESET_TO_FLASHLOADER);
         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error("Scan ETH get info from openSYDE devices",
                                "openSYDE broadcast ECU reset failed with error: " + C_SCLString::IntToStr(s32_Return));
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         std::vector<C_OSCProtocolDriverOsyTpIp::C_BroadcastGetDeviceInfoResults> c_ReadDeviceInfoResults;

         //wait the minimum wait time (all nodes should now be in the default session of the flashloader)
         stw_tgl::TGL_Sleep(this->GetMinimumFlashloaderResetWaitTime(C_OSCComDriverFlash::eNO_CHANGES_ETHERNET));

         //broadcast: "get device info" (returns serial number and device name)
         s32_Return = this->mpc_ComDriver->SendOsyEthBroadcastGetDeviceInformation(c_ReadDeviceInfoResults);
         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error("Scan ETH get info from openSYDE devices",
                                "openSYDE broadcast get device information failed with error: " +
                                C_SCLString::IntToStr(s32_Return));
         }
         else
         {
            osc_write_log_info("Scan ETH get info from openSYDE devices", "Number of responses detected: " +
                               C_SCLString::IntToStr(c_ReadDeviceInfoResults.size()));
            // Fill the result container
            for (uint32 u32_ResultCounter = 0U; u32_ResultCounter < c_ReadDeviceInfoResults.size(); ++u32_ResultCounter)
            {
               C_SyvDcDeviceInformation c_DeviceInfo;
               c_DeviceInfo.SetDeviceName(c_ReadDeviceInfoResults[u32_ResultCounter].c_DeviceName);
               c_DeviceInfo.SetIpAddress(c_ReadDeviceInfoResults[u32_ResultCounter].au8_IpAddress);
               c_DeviceInfo.SetNodeId(c_ReadDeviceInfoResults[u32_ResultCounter].c_NodeId.u8_NodeIdentifier);
               c_DeviceInfo.SetSerialNumber(c_ReadDeviceInfoResults[u32_ResultCounter].au8_SerialNumber);
               this->mc_DeviceInfoResult.push_back(c_DeviceInfo);
            }
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info("Scan ETH get info from openSYDE devices", "Sequence finished");
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Configure openSYDE devices via Ethernet

   Write new configuration to openSYDE devices via ETH-TP.
   For all interfaces that have a connected bus:
   * node-IDs and bus-IDs
   * for CAN-interfaces: bitrate
   * for Ethernet: IP-address

   Assumptions:
   * All devices are in flashloader mode.
   * The com driver is initialized and has set up protocol instances for all openSYDE nodes

   Sequence:
   * for all nodes
   ** broadcast "setIpAddress"
   * broadcast: "enter flashloader"
   * broadcast: "ecu reset"
   * wait a little (all nodes should now be in the default session of the flashloader)
   * try to connect to all nodes
   * for all nodes (directed services; uniquely identified by IP)
   ** "DiagnosticSessionControl(Preprogramming)"
   ** activate "SecurityAccess" for Level 1
   ** for all CAN interfaces with connected bus:
   *** "SetBitrate"
   * for all nodes (directed services; uniquely identified by IP)
   ** for all ETH interfaces with connected bus:
   *** "SetIPAddressForChannel"
   * for all nodes (directed services; uniquely identified by IP)
   ** for all interfaces (CAN + Ethernet)
   *** "setNodeIDForChannel" (including for the one we are connected to)

   \return
   C_NO_ERR    all devices are configured
   C_NOACT     no device configurations exist
   C_RANGE     device configuration is invalid
   C_WARN      negative response received
   C_COM       could not send request
   C_CONFIG    no dispatcher installed or broadcast protocol not initialized
               no com driver installed
   C_TIMEOUT   no response within timeout
   C_OVERFLOW  multiple responses received
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_RunConfEthOpenSydeDevices(void)
{
   sint32 s32_Return = C_CONFIG;

   this->m_RunConfEthOpenSydeDevicesProgress(0U);
   if (this->mpc_ComDriver != NULL)
   {
      uint32 u32_DeviceCounter;
      // We need the correct bus id for the SyvComDriver.
      C_OSCProtocolDriverOsyNode c_ServerIdOfCurBus = this->mpc_ComDriver->GetClientId();

      // Vector with server ids of all configured nodes and its connected and actual used bus
      std::vector<C_OSCProtocolDriverOsyNode> c_UsedServerIds;

      osc_write_log_info("Configure Ethernet openSYDE devices", "Sequence started");

      // Before we start check all device configurations
      s32_Return = this->m_CheckConfOpenSydeDevices(this->mc_DeviceConfiguration);

      if (s32_Return == C_NO_ERR)
      {
         // In the first step, set the IPs of all nodes on the current bus
         //* for all nodes
         //** broadcast "setIpAddress"
         for (u32_DeviceCounter = 0U; u32_DeviceCounter < this->mc_DeviceConfiguration.size(); ++u32_DeviceCounter)
         {
            const C_SyvDcDeviceConfiguation & rc_CurConfig = this->mc_DeviceConfiguration[u32_DeviceCounter];
            uint32 u32_InterfaceCounter;

            // Progress calculation for sequence 0% - 30%
            this->m_RunConfEthOpenSydeDevicesProgress(
               (u32_DeviceCounter * 30U) / this->mc_DeviceConfiguration.size());

            for (u32_InterfaceCounter = 0U; u32_InterfaceCounter < rc_CurConfig.c_BusIds.size(); ++u32_InterfaceCounter)
            {
               if (rc_CurConfig.c_BusIds[u32_InterfaceCounter] == c_ServerIdOfCurBus.u8_BusIdentifier)
               {
                  uint8 au8_ResponseIp[4];
                  uint8 u8_CommError;
                  uint32 u32_NodeIndex;
                  c_ServerIdOfCurBus.u8_NodeIdentifier = rc_CurConfig.c_NodeIds[u32_InterfaceCounter];
                  c_UsedServerIds.push_back(c_ServerIdOfCurBus);

                  s32_Return =
                     this->mpc_ComDriver->SendOsyEthBroadcastSetIpAddress(rc_CurConfig.au8_SerialNumber,
                                                                          rc_CurConfig.c_IpAddresses[
                                                                             u32_InterfaceCounter].au8_IpAddress,
                                                                          rc_CurConfig.c_IpAddresses[
                                                                             u32_InterfaceCounter].au8_NetMask,
                                                                          rc_CurConfig.c_IpAddresses[
                                                                             u32_InterfaceCounter].au8_DefaultGateway,
                                                                          c_ServerIdOfCurBus,
                                                                          au8_ResponseIp, &u8_CommError);
                  if (s32_Return != C_NO_ERR)
                  {
                     osc_write_log_error("Configure Ethernet openSYDE devices",
                                         "openSYDE broadcast set IP by serial number failed with error: " +
                                         C_SCLString::IntToStr(s32_Return));
                     break;
                  }
                  else
                  {
                     C_SCLString c_Text;
                     c_Text.PrintFormatted("openSYDE broadcast set IP by serial number changed the IP address on"
                                           " node with id %d on bus with id %d.",
                                           c_ServerIdOfCurBus.u8_NodeIdentifier,
                                           c_ServerIdOfCurBus.u8_BusIdentifier);

                     osc_write_log_info("Configure Ethernet openSYDE devices", c_Text);
                  }

                  if (this->mpc_ComDriver->GetNodeIndex(c_ServerIdOfCurBus, u32_NodeIndex) == true)
                  {
                     const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
                        u32_NodeIndex);

                     if (pc_Node != NULL)
                     {
                        uint32 u32_NodeIntfCounter;

                        for (u32_NodeIntfCounter = 0U;
                             u32_NodeIntfCounter < pc_Node->c_Properties.c_ComInterfaces.size();
                             ++u32_NodeIntfCounter)
                        {
                           const C_OSCNodeComInterfaceSettings & rc_InterfaceSettings =
                              pc_Node->c_Properties.c_ComInterfaces[u32_NodeIntfCounter];
                           const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
                              rc_InterfaceSettings.u32_BusIndex);

                           if ((pc_Bus != NULL) &&
                               (pc_Bus->u8_BusID == rc_CurConfig.c_BusIds[u32_InterfaceCounter]) &&
                               (rc_InterfaceSettings.q_IsBusConnected == true))
                           {
                              this->m_RunConfOpenSydeDevicesState(hu32_SETNODEID, s32_Return, c_ServerIdOfCurBus,
                                                                  rc_InterfaceSettings.e_InterfaceType,
                                                                  rc_InterfaceSettings.u8_InterfaceNumber);

                              this->m_RunConfOpenSydeDevicesState(hu32_SETIPADDRESS, s32_Return, c_ServerIdOfCurBus,
                                                                  rc_InterfaceSettings.e_InterfaceType,
                                                                  rc_InterfaceSettings.u8_InterfaceNumber);
                           }
                        }
                     }
                  }
               }
            }
         }
      }

      //from here on the sequence is the same as for CAN: use utility function
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = m_ConfigureNodes(false, c_UsedServerIds);
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Configure openSYDE devices

   Utility function used by the CAN and Ethernet sequences.
   Contains the final part of the sequence which is identical for CAN and Ethernet:

   Sequence:
   * broadcast: "enter flashloader"
   * broadcast: "ecu reset"
   * wait a little (all nodes should now be in the default session of the flashloader)
   * try to connect to all nodes
   * for all nodes (directed services)
   ** "DiagnosticSessionControl(Preprogramming)"
   ** activate "SecurityAccess" for Level 1
   ** for all CAN interfaces with connected bus:
   *** "SetBitrate"
   * for all nodes (directed services)
   ** for all ETH interfaces with connected bus:
   *** "SetIPAddressForChannel"
   * for all nodes (directed services)
   ** for all interfaces (CAN + Ethernet)
   *** "setNodeIDForChannel" (including for the one we are connected to)

   \param[in]  oq_ViaCan            true: running via CAN; false: running via Etherner (only used for user feedback)
   \param[in]  orc_UsedServerIds    Vector with server ids of all configured nodes and their connected and used bus

   \return
   C_NO_ERR    all devices are configured
   C_WARN      negative response received
   C_COM       could not send request
   C_TIMEOUT   no response within timeout (was SetNodeIdentifiersForBroadcasts() called ?)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_ConfigureNodes(const bool oq_ViaCan,
                                          std::vector<C_OSCProtocolDriverOsyNode> & orc_UsedServerIds)
{
   sint32 s32_Return;
   uint32 u32_DeviceCounter;

   // * broadcast: "RequestProgramming"
   bool q_RequestNotAccepted;

   // Progress calculation for sequence 30%
   if (oq_ViaCan == true)
   {
      this->m_RunConfCanOpenSydeDevicesProgress(30U);
   }
   else
   {
      this->m_RunConfEthOpenSydeDevicesProgress(30U);
   }

   tgl_assert(this->mc_DeviceConfiguration.size() == orc_UsedServerIds.size());

   s32_Return = this->mpc_ComDriver->SendOsyBroadcastRequestProgramming(q_RequestNotAccepted);
   if (s32_Return == C_NO_ERR)
   {
      // Check the result
      if (q_RequestNotAccepted == true)
      {
         // at least one node answered with a negative response
         s32_Return = C_WARN;
      }
   }

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Configure openSYDE devices",
                          "openSYDE broadcast request programming failed with error: " +
                          C_SCLString::IntToStr(s32_Return));
   }

   // * broadcast: "ecu reset"
   if (s32_Return == C_NO_ERR)
   {
      // Progress calculation for sequence 50%
      if (oq_ViaCan == true)
      {
         this->m_RunConfCanOpenSydeDevicesProgress(50U);
      }
      else
      {
         this->m_RunConfEthOpenSydeDevicesProgress(50U);
      }

      s32_Return = this->mpc_ComDriver->SendOsyBroadcastEcuReset(
         C_OSCProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_RESET_TO_FLASHLOADER);

      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error("Configure openSYDE devices",
                             "openSYDE broadcast ECU reset failed with error: " +
                             C_SCLString::IntToStr(s32_Return));
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      // * wait the minimum wait time (all nodes should now be in the default session of the flashloader)
      if (oq_ViaCan == true)
      {
         // The CAN broadcast does not change the CAN bitrate and so no fundamental changes were made
         // when using CAN
         stw_tgl::TGL_Sleep(this->GetMinimumFlashloaderResetWaitTime(C_OSCComDriverFlash::
                                                                     eNO_FUNDAMENTAL_COM_CHANGES_CAN));
      }
      else
      {
         // The Ethernet broadcast does change the IP address and so fundamental changes were made
         // when using Ethernet
         stw_tgl::TGL_Sleep(this->GetMinimumFlashloaderResetWaitTime(C_OSCComDriverFlash::
                                                                     eFUNDAMENTAL_COM_CHANGES_ETHERNET));
      }

      // Bring all nodes into the necessary state before changing the bitrate.
      // The bitrate is the most critical part. We have to try to avoid different configured bitrates on the
      // servers because of an error in the process between the servers.
      for (u32_DeviceCounter = 0U; u32_DeviceCounter < orc_UsedServerIds.size(); ++u32_DeviceCounter)
      {
         // Progress calculation for sequence 60% - 70%
         if (oq_ViaCan == true)
         {
            this->m_RunConfCanOpenSydeDevicesProgress(
               60U + ((u32_DeviceCounter * 10U) / this->mc_DeviceConfiguration.size()));
         }
         else
         {
            this->m_RunConfEthOpenSydeDevicesProgress(
               60U + ((u32_DeviceCounter * 10U) / this->mc_DeviceConfiguration.size()));
         }

         //first time we need to explicitly contact the node: reconnect (required for Ethernet)
         s32_Return = this->mpc_ComDriver->ReConnectNode(orc_UsedServerIds[u32_DeviceCounter]);
         if (s32_Return != C_NO_ERR)
         {
            C_SCLString c_Text;
            c_Text.PrintFormatted("Could not reconnect to node with ID %d on bus with id %d. Error code: %d",
                                  orc_UsedServerIds[u32_DeviceCounter].u8_NodeIdentifier,
                                  orc_UsedServerIds[u32_DeviceCounter].u8_BusIdentifier, s32_Return);
            osc_write_log_error("Configure openSYDE devices", c_Text);
         }

         // ** "DiagnosticSessionControl(Preprogramming)"
         // ** activate "SecurityAccess" for Level 1
         s32_Return = this->mpc_ComDriver->SendOsySetPreProgrammingMode(orc_UsedServerIds[u32_DeviceCounter], false);

         if (s32_Return != C_NO_ERR)
         {
            C_SCLString c_Text;
            c_Text.PrintFormatted("openSYDE setting preprogramming mode failed on node with ID %d on bus with ID"
                                  " %d with error: %d", orc_UsedServerIds[u32_DeviceCounter].u8_NodeIdentifier,
                                  orc_UsedServerIds[u32_DeviceCounter].u8_BusIdentifier, s32_Return);
            osc_write_log_error("Configure openSYDE devices", c_Text);
         }

         if (s32_Return != C_NO_ERR)
         {
            break;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      // ** for all CAN interfaces with a connected bus:
      for (u32_DeviceCounter = 0U; u32_DeviceCounter < orc_UsedServerIds.size(); ++u32_DeviceCounter)
      {
         // Progress calculation for sequence 70% - 80%
         if (oq_ViaCan == true)
         {
            this->m_RunConfCanOpenSydeDevicesProgress(
               70U + ((u32_DeviceCounter * 10U) / this->mc_DeviceConfiguration.size()));
         }
         else
         {
            this->m_RunConfEthOpenSydeDevicesProgress(
               70U + ((u32_DeviceCounter * 10U) / this->mc_DeviceConfiguration.size()));
         }
         // *** "SetBitrate"
         s32_Return = this->m_SetCanOpenSydeBitrate(orc_UsedServerIds[u32_DeviceCounter],
                                                    this->mc_DeviceConfiguration[u32_DeviceCounter]);
         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error("Configure openSYDE devices", "Setting CAN bitrate failed");
            break;
         }
      }
   }

   if ((s32_Return == C_NO_ERR) &&
       (this->mq_ConfigureAllInterfaces == true))
   {
      // ** for all ETH interfaces with a connected bus:
      for (u32_DeviceCounter = 0U; u32_DeviceCounter < orc_UsedServerIds.size(); ++u32_DeviceCounter)
      {
         // Progress calculation for sequence 80% - 90%
         if (oq_ViaCan == true)
         {
            this->m_RunConfCanOpenSydeDevicesProgress(
               80U + ((u32_DeviceCounter * 10U) / this->mc_DeviceConfiguration.size()));
         }
         else
         {
            this->m_RunConfEthOpenSydeDevicesProgress(80U +
                                                      ((u32_DeviceCounter * 10U) /
                                                       this->mc_DeviceConfiguration.size()));
         }
         // *** "SetIPAddressForChannel"
         s32_Return = this->m_SetEthOpenSydeIpAddress(orc_UsedServerIds[u32_DeviceCounter],
                                                      this->mc_DeviceConfiguration[u32_DeviceCounter]);
         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error("Configure openSYDE devices", "Setting IP address failed");
            break;
         }
      }
   }
   else
   {
      osc_write_log_info("Configure openSYDE devices", "Skipping setting IP addresses for other interfaces");
   }

   if ((s32_Return == C_NO_ERR) &&
       (this->mq_ConfigureAllInterfaces == true))
   {
      // ** for all interfaces (CAN + Ethernet)
      for (u32_DeviceCounter = 0U; u32_DeviceCounter < orc_UsedServerIds.size(); ++u32_DeviceCounter)
      {
         // Progress calculation for sequence 90% - 100%
         if (oq_ViaCan == true)
         {
            this->m_RunConfCanOpenSydeDevicesProgress(
               90U + ((u32_DeviceCounter * 10U) / this->mc_DeviceConfiguration.size()));
         }
         else
         {
            this->m_RunConfEthOpenSydeDevicesProgress(90U +
                                                      ((u32_DeviceCounter * 10U) /
                                                       this->mc_DeviceConfiguration.size()));
         }
         // *** "setNodeIDForChannel"
         s32_Return = this->m_SetOpenSydeNodeIds(orc_UsedServerIds[u32_DeviceCounter],
                                                 this->mc_DeviceConfiguration[u32_DeviceCounter]);
         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error("Configure openSYDE devices", "Setting node ID failed");
            break;
         }
      }
   }
   else
   {
      osc_write_log_info("Configure openSYDE devices", "Skipping setting node IDs for other interfaces");
   }

   if (s32_Return == C_NO_ERR)
   {
      if (oq_ViaCan == true)
      {
         osc_write_log_info("Configure CAN openSYDE devices", "Sequence finished");
         this->m_RunConfCanOpenSydeDevicesProgress(100U);
      }
      else
      {
         osc_write_log_info("Configure ETH openSYDE devices", "Sequence finished");
         this->m_RunConfEthOpenSydeDevicesProgress(100U);
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write new configuration to STW Flashloader devices.

   Assumptions:
   * All devices are in flashloader mode.
   * The com driver passed as parameter is initialized and has set up protocol instances for all STW Flashloader nodes

   Sequence:
   * for all devices
   ** perform "node_wakeup(serial_number)"
   ** perform "node_companyid(Y*)"
   ** perform "SetLocalId(newnodeid)"
   ** perform "SetBitrateCan(newbitrate)"

   \return
   C_NO_ERR    all devices are configured
   C_NOACT     no device configurations exist
   C_RANGE     device configuration is invalid
   C_CONFIG    no com driver installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_RunConfCanStwFlashloaderDevices(void)
{
   sint32 s32_Return = C_CONFIG;

   this->m_RunConfCanStwFlashloaderDevicesProgress(0U);

   if (this->mpc_ComDriver != NULL)
   {
      s32_Return = C_NOACT;

      uint32 u32_DeviceCounter;
      // We need the correct bus id for the SyvComDriver.
      C_OSCProtocolDriverOsyNode c_ServerId = this->mpc_ComDriver->GetClientId();

      osc_write_log_info("Configure CAN STW Flashloader devices", "Sequence started");

      // In the first step, adapt all local ids
      for (u32_DeviceCounter = 0U; u32_DeviceCounter < this->mc_DeviceConfiguration.size(); ++u32_DeviceCounter)
      {
         uint8 u8_LocalId;
         const C_SyvDcDeviceConfiguation & rc_CurConfig = this->mc_DeviceConfiguration[u32_DeviceCounter];

         // Progress calculation for sequence 0% - 50%
         this->m_RunConfCanStwFlashloaderDevicesProgress(
            (u32_DeviceCounter * 50U) / this->mc_DeviceConfiguration.size());

         tgl_assert(rc_CurConfig.c_NodeIds.size() == rc_CurConfig.c_BusIds.size());
         tgl_assert(rc_CurConfig.c_NodeIds.size() == rc_CurConfig.c_CanBitrates.size());
         // No bus-id in STW flashloader protocol.
         // We can change only the connected bus interface with the STW flashloader
         tgl_assert(rc_CurConfig.c_BusIds.size() == 1);

         if ((rc_CurConfig.c_NodeIds.size() == rc_CurConfig.c_BusIds.size()) &&
             (rc_CurConfig.c_NodeIds.size() == rc_CurConfig.c_CanBitrates.size()) &&
             (rc_CurConfig.c_BusIds.size() == 1))
         {
            // ** perform "node_wakeup(serial_number)"
            // ** perform "node_companyid(Y*)"
            s32_Return = this->mpc_ComDriver->SendStwWakeupLocalSerialNumber(rc_CurConfig.au8_SerialNumber, u8_LocalId);

            if (s32_Return == C_NO_ERR)
            {
               // The request needs the old identifier
               c_ServerId.u8_NodeIdentifier = u8_LocalId;

               // ** perform "SetLocalId(newnodeid)"
               s32_Return = this->mpc_ComDriver->SendStwSetLocalId(c_ServerId, rc_CurConfig.c_NodeIds[0]);

               // Send state with new node id
               this->m_RunConfCanStwFlashloaderDevicesState(hu32_SETNODEID, s32_Return,
                                                            C_OSCProtocolDriverOsyNode(c_ServerId.u8_BusIdentifier,
                                                                                       rc_CurConfig.c_NodeIds[0]));

               if (s32_Return != C_NO_ERR)
               {
                  osc_write_log_error("Configure CAN STW Flashloader devices",
                                      "STW Flashloader set new local id failed with error: " +
                                      C_SCLString::IntToStr(s32_Return));
               }
            }
            else
            {
               osc_write_log_error("Configure CAN STW Flashloader devices",
                                   "STW Flashloader wakeup with serial number failed with error: " +
                                   C_SCLString::IntToStr(s32_Return));
            }
         }
         else
         {
            s32_Return = C_RANGE;
            osc_write_log_error("Configure CAN STW Flashloader devices",
                                "device configuration is invalid.");
         }

         if (s32_Return != C_NO_ERR)
         {
            break;
         }
      }

      // In the second step, adapt all bitrates
      if (s32_Return == C_NO_ERR)
      {
         for (u32_DeviceCounter = 0U; u32_DeviceCounter < this->mc_DeviceConfiguration.size(); ++u32_DeviceCounter)
         {
            uint8 u8_LocalId;
            const C_SyvDcDeviceConfiguation & rc_CurConfig = this->mc_DeviceConfiguration[u32_DeviceCounter];

            // Progress calculation for sequence 50% - 100%
            this->m_RunConfCanStwFlashloaderDevicesProgress(
               50U + ((u32_DeviceCounter * 50U) / this->mc_DeviceConfiguration.size()));

            // ** perform "node_wakeup(serial_number)"
            // ** perform "node_companyid(Y*)"
            s32_Return = this->mpc_ComDriver->SendStwWakeupLocalSerialNumber(rc_CurConfig.au8_SerialNumber, u8_LocalId);

            if (s32_Return == C_NO_ERR)
            {
               // Use the new local id
               c_ServerId.u8_NodeIdentifier = u8_LocalId;

               // ** perform "SetBitrateCan(newbitrate)"
               s32_Return = this->mpc_ComDriver->SendStwSetBitrateCan(c_ServerId,
                                                                      rc_CurConfig.c_CanBitrates[0]);

               this->m_RunConfCanStwFlashloaderDevicesState(hu32_SETCANBITRATE, s32_Return, c_ServerId);

               if (s32_Return != C_NO_ERR)
               {
                  osc_write_log_error("Configure CAN STW Flashloader devices",
                                      "STW Flashloader set new bitrate failed with error: " +
                                      C_SCLString::IntToStr(s32_Return));
               }
            }
            else
            {
               osc_write_log_error("Configure CAN STW Flashloader devices",
                                   "STW Flashloader wakeup with serial number failed with error: " +
                                   C_SCLString::IntToStr(s32_Return));
            }

            if (s32_Return != C_NO_ERR)
            {
               break;
            }
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info("Configure CAN STW Flashloader devices", "Sequence finished");
      this->m_RunConfCanStwFlashloaderDevicesProgress(100U);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write new configuration to openSYDE devices via CAN-TP.

   For all interfaces that have a connected bus:
   * Node-IDs and Bus-IDs
   * For CAN-interfaces the bitrate
   * For Ethernet the IP-Address

   Note:
   * configureFlashloaderCommunicationChannel will not be performed; the application shall care about this
   ** the risk to lock oneself out from communication with the device is quite high

   Assumptions:
   * All devices are in flashloader mode.
   * The com driver passed as parameter is initialized and has set up protocol instances for all openSYDE nodes

   Sequence:
   * for all nodes
   ** broadcast "setNodeIdBySerialNumberPart1"
   ** broadcast "setNodeIdBySerialNumberPart2"
   ** broadcast "setNodeIdBySerialNumberPart3"
   * broadcast: "RequestProgramming"
   * broadcast: "ecu reset"
   * wait a little (all nodes should now be in the default session of the flashloader)
   * for all nodes (directed services; identified by Node-ID)
   ** "DiagnosticSessionControl(Preprogramming)"
   ** activate "SecurityAccess" for Level 1
   ** for all CAN interfaces with a connected bus:
   *** "SetBitrate"
   * for all nodes (directed services; identified by Node-ID)
   ** for all ETH interfaces with a connected bus:
   *** "SetIPAddressForChannel"
   * for all nodes (directed services; identified by Node-ID)
   ** for all interfaces (CAN + Ethernet)
   *** "setNodeIDForChannel" (except for the one we are connected to; we already configured it;
                              though "re-setting" does not hurt)

   \return
   C_NO_ERR    all devices are configured
   C_NOACT     no device configurations exist
   C_RANGE     device configuration is invalid
   C_WARN      negative response received
   C_COM       could not send requests
   C_CONFIG    no dispatcher installed or broadcast protocol not initialized
               no com driver installed
   C_TIMEOUT   no response within timeout (was SetNodeIdentifiersForBroadcasts() called ?)
   C_OVERFLOW  multiple responses received
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_RunConfCanOpenSydeDevices(void)
{
   sint32 s32_Return = C_CONFIG;

   this->m_RunConfCanOpenSydeDevicesProgress(0U);

   if (this->mpc_ComDriver != NULL)
   {
      uint32 u32_DeviceCounter;
      // We need the correct bus id for the SyvComDriver.
      C_OSCProtocolDriverOsyNode c_ServerIdOfCurBus = this->mpc_ComDriver->GetClientId();

      // Vector with server ids of all configured nodes and its connected and actual used bus
      std::vector<C_OSCProtocolDriverOsyNode> c_UsedServerIds;

      osc_write_log_info("Configure CAN openSYDE devices", "Sequence started");

      // Before we start check all device configurations
      s32_Return = this->m_CheckConfOpenSydeDevices(this->mc_DeviceConfiguration);

      // All openSYDE nodes must be in default session.
      // The broadcast SetNodeIdBySerialNumber works only in this session.
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = this->mpc_ComDriver->SendOsyCanBroadcastEnterDefaultSession();
      }

      if (s32_Return == C_NO_ERR)
      {
         // In the first step, adapt all node ids on the current used bus
         // * for all nodes
         // ** broadcast "setNodeIdBySerialNumberPartx"
         for (u32_DeviceCounter = 0U; u32_DeviceCounter < this->mc_DeviceConfiguration.size(); ++u32_DeviceCounter)
         {
            const C_SyvDcDeviceConfiguation & rc_CurConfig = this->mc_DeviceConfiguration[u32_DeviceCounter];
            uint32 u32_InterfaceCounter;

            // Progress calculation for sequence 0% - 30%
            this->m_RunConfCanOpenSydeDevicesProgress(
               (u32_DeviceCounter * 30U) / this->mc_DeviceConfiguration.size());

            for (u32_InterfaceCounter = 0U; u32_InterfaceCounter < rc_CurConfig.c_BusIds.size(); ++u32_InterfaceCounter)
            {
               if (rc_CurConfig.c_BusIds[u32_InterfaceCounter] == c_ServerIdOfCurBus.u8_BusIdentifier)
               {
                  uint32 u32_NodeIndex;
                  bool q_InterfaceFound = false;

                  c_ServerIdOfCurBus.u8_NodeIdentifier = rc_CurConfig.c_NodeIds[u32_InterfaceCounter];

                  if (this->GetNodeIndex(c_ServerIdOfCurBus, u32_NodeIndex) == true)
                  {
                     const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodeIndex);

                     c_UsedServerIds.push_back(c_ServerIdOfCurBus);
                     s32_Return = this->mpc_ComDriver->SendOsyCanBroadcastSetNodeIdBySerialNumber(
                        rc_CurConfig.au8_SerialNumber,
                        c_ServerIdOfCurBus);

                     // Get the real interface number of the bus
                     if (pc_Node != NULL)
                     {
                        uint32 u32_NodeInterfaceCounter;

                        for (u32_NodeInterfaceCounter = 0U;
                             u32_NodeInterfaceCounter < pc_Node->c_Properties.c_ComInterfaces.size();
                             ++u32_NodeInterfaceCounter)
                        {
                           const C_OSCNodeComInterfaceSettings & rc_ComInterface =
                              pc_Node->c_Properties.c_ComInterfaces[u32_NodeInterfaceCounter];
                           const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
                              rc_ComInterface.u32_BusIndex);

                           if ((pc_Bus != NULL) &&
                               (pc_Bus->u8_BusID == rc_CurConfig.c_BusIds[u32_InterfaceCounter]) &&
                               (rc_ComInterface.q_IsBusConnected == true))
                           {
                              // Configuration function for CAN, so the interface is type CAN for sure
                              this->m_RunConfOpenSydeDevicesState(hu32_SETNODEID, s32_Return, c_ServerIdOfCurBus,
                                                                  C_OSCSystemBus::eCAN,
                                                                  rc_ComInterface.u8_InterfaceNumber);
                           }
                        }
                     }
                     q_InterfaceFound = true;
                  }
                  else
                  {
                     s32_Return = C_RANGE;
                  }

                  if ((q_InterfaceFound == true) ||
                      (s32_Return != C_NO_ERR))
                  {
                     break;
                  }
               }
            }

            if (s32_Return != C_NO_ERR)
            {
               osc_write_log_error("Configure CAN openSYDE devices",
                                   "openSYDE broadcast set node ID by serial number failed with error: " +
                                   C_SCLString::IntToStr(s32_Return));
               break;
            }
         }
      }

      //from here on the sequence is the same as for Ethernet: use utility function
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = m_ConfigureNodes(true, c_UsedServerIds);
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the current set device configurations for correctness for openSYDE devices

   All connected interfaces of the node must be in the device configuration.
   Bus and node ids must be valid.

   \param[in]  orc_DeviceConfiguration    information about intended device configuration for all nodes

   \return
   C_NO_ERR    all devices are configured
   C_NOACT     no device configurations exist
   C_RANGE     device configuration is invalid
   C_CONFIG    no com driver installed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_CheckConfOpenSydeDevices(
   const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfiguration) const
{
   sint32 s32_Return = C_CONFIG;

   if (this->mpc_ComDriver != NULL)
   {
      uint32 u32_DeviceCounter;

      s32_Return = C_NOACT;

      // all in the system definition connected interfaces must be configured
      for (u32_DeviceCounter = 0U; u32_DeviceCounter < orc_DeviceConfiguration.size(); ++u32_DeviceCounter)
      {
         const C_SyvDcDeviceConfiguation & rc_CurConfig = orc_DeviceConfiguration[u32_DeviceCounter];

         tgl_assert(rc_CurConfig.c_NodeIds.size() == rc_CurConfig.c_BusIds.size());

         // Default error for this check
         s32_Return = C_RANGE;

         if (rc_CurConfig.c_NodeIds.size() == rc_CurConfig.c_BusIds.size())
         {
            uint32 u32_NodeIndex;
            bool q_Found = false;
            bool q_InvalidNodeId = false;
            uint32 u32_InterfaceCounter;
            C_OSCProtocolDriverOsyNode c_ServerIdOfCurBus = this->mpc_ComDriver->GetClientId();

            // Getting the real node index of the system definition
            for (u32_InterfaceCounter = 0U; u32_InterfaceCounter < rc_CurConfig.c_BusIds.size(); ++u32_InterfaceCounter)
            {
               bool q_StopLoop = false;

               // Check the ids
               if ((rc_CurConfig.c_BusIds[u32_InterfaceCounter] > C_OSCProtocolDriverOsyNode::mhu8_MAX_BUS) &&
                   (rc_CurConfig.c_NodeIds[u32_InterfaceCounter] >= C_OSCProtocolDriverOsyNode::mhu8_MAX_NODE))
               {
                  osc_write_log_error("Configure openSYDE devices",
                                      "device configuration is invalid: bus ID or node ID out of range.");
                  q_InvalidNodeId = true;
                  q_StopLoop = true;
               }
               else if (rc_CurConfig.c_BusIds[u32_InterfaceCounter] == c_ServerIdOfCurBus.u8_BusIdentifier)
               {
                  c_ServerIdOfCurBus.u8_NodeIdentifier = rc_CurConfig.c_NodeIds[u32_InterfaceCounter];
                  q_Found = true;
                  q_StopLoop = true;
               }
               else
               {
                  // Nothing to do
               }

               if (q_StopLoop == true)
               {
                  break;
               }
            }

            if ((q_Found == true) &&
                (q_InvalidNodeId == false) &&
                (this->mpc_ComDriver->GetNodeIndex(c_ServerIdOfCurBus, u32_NodeIndex) == true))
            {
               // All connected interfaces of the node must be in the device configuration
               const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodeIndex);

               if (pc_Node != NULL)
               {
                  uint32 u32_NodeInterfaceCounter;
                  uint32 u32_CanCount = 0U;
                  uint32 u32_EthCount = 0U;

                  for (u32_NodeInterfaceCounter = 0U;
                       u32_NodeInterfaceCounter < pc_Node->c_Properties.c_ComInterfaces.size();
                       ++u32_NodeInterfaceCounter)
                  {
                     const C_OSCNodeComInterfaceSettings & rc_NodeIntf =
                        pc_Node->c_Properties.c_ComInterfaces[u32_NodeInterfaceCounter];

                     if (rc_NodeIntf.q_IsBusConnected == true)
                     {
                        const C_OSCSystemBus * const pc_OtherBus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
                           rc_NodeIntf.u32_BusIndex);

                        q_Found = false;

                        if (pc_OtherBus != NULL)
                        {
                           for (u32_InterfaceCounter = 0U;
                                u32_InterfaceCounter < rc_CurConfig.c_BusIds.size();
                                ++u32_InterfaceCounter)
                           {
                              // Check the bus id
                              if (rc_CurConfig.c_BusIds[u32_InterfaceCounter] == pc_OtherBus->u8_BusID)
                              {
                                 q_Found = true;
                                 break;
                              }
                           }
                        }

                        if (q_Found == true)
                        {
                           // Count all interfaces
                           if (rc_NodeIntf.e_InterfaceType == C_OSCSystemBus::eCAN)
                           {
                              ++u32_CanCount;
                           }
                           else
                           {
                              ++u32_EthCount;
                           }
                        }
                        else
                        {
                           break;
                        }
                     }
                  }

                  // Check the count of found connected interfaces
                  if ((static_cast<uint32>(rc_CurConfig.c_BusIds.size()) == (u32_CanCount + u32_EthCount)) &&
                      (static_cast<uint32>(rc_CurConfig.c_CanBitrates.size()) == u32_CanCount) &&
                      (static_cast<uint32>(rc_CurConfig.c_IpAddresses.size()) == u32_EthCount))
                  {
                     // Configuration is valid
                     s32_Return = C_NO_ERR;
                  }
               }
            }
         }

         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error("Configure openSYDE devices", "Device configuration is invalid.");
            break;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the bitrate for all connected and configured CAN interfaces of the server

   \param[in]  orc_ServerId      Server id for communication
   \param[in]  orc_DeviceConfig  Configuration for the server

   \return
   C_NO_ERR    Bitrate set on server successfully
   C_RANGE     device configuration is invalid
               openSYDE protocol not found
   C_TIMEOUT   expected response not received within timeout
   C_NOACT     could not put request in Tx queue ...
   C_CONFIG    no transport protocol installed
               no com driver installed
   C_WARN      error response (negative response code placed in *opu8_NrCode)
   C_RD_WR     unexpected content in response (here: wrong data identifier ID)
   C_COM       communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_SetCanOpenSydeBitrate(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                 const C_SyvDcDeviceConfiguation & orc_DeviceConfig) const
{
   sint32 s32_Return = C_CONFIG;

   if (this->mpc_ComDriver != NULL)
   {
      uint32 u32_NodeIndex;

      s32_Return = C_RANGE;

      if (this->mpc_ComDriver->GetNodeIndex(orc_ServerId, u32_NodeIndex) == true)
      {
         // All connected interfaces of the node must be in the device configuration
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodeIndex);

         if (pc_Node != NULL)
         {
            uint32 u32_CanCounter = 0U;
            uint32 u32_BusCounter;
            std::set<uint32> c_FinishedBusIds;

            for (u32_BusCounter = 0U; u32_BusCounter < orc_DeviceConfig.c_BusIds.size(); ++u32_BusCounter)
            {
               std::pair<std::set<uint32>::iterator, bool> c_InsertResult;
               uint32 u32_InterfaceCounter;
               s32_Return = C_NO_ERR;

               // If a node is connected at least two times with the same bus,
               // the configuration will be handled by the interface loop
               c_InsertResult = c_FinishedBusIds.insert(orc_DeviceConfig.c_BusIds[u32_BusCounter]);

               //was the element inserted (or already present) ?
               if (c_InsertResult.second == true)
               {
                  // Newly inserted.
                  // We need the interface settings of the node which is connected to the bus with this id
                  for (u32_InterfaceCounter = 0U;
                       u32_InterfaceCounter < pc_Node->c_Properties.c_ComInterfaces.size();
                       ++u32_InterfaceCounter)
                  {
                     const C_OSCNodeComInterfaceSettings & rc_InterfaceSettings =
                        pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceCounter];
                     const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
                        rc_InterfaceSettings.u32_BusIndex);

                     if ((pc_Bus != NULL) &&
                         (pc_Bus->u8_BusID == orc_DeviceConfig.c_BusIds[u32_BusCounter]) &&
                         (rc_InterfaceSettings.q_IsBusConnected == true) &&
                         (rc_InterfaceSettings.e_InterfaceType == C_OSCSystemBus::eCAN))
                     {
                        bool q_SetBitrate = true;

                        // Configurable bitrate configuration
                        // If not wished, only for the connected and for the configuration used bus the bitrate shall be
                        // changed
                        if ((this->mq_ConfigureAllInterfaces == false) &&
                            ((pc_Bus->u8_BusID != orc_ServerId.u8_BusIdentifier) ||
                             (rc_InterfaceSettings.u8_NodeID != orc_ServerId.u8_NodeIdentifier)))
                        {
                           q_SetBitrate = false;
                        }

                        if (q_SetBitrate == true)
                        {
                           const C_OSCProtocolDriverOsyNode c_StateConfiguredInterface(pc_Bus->u8_BusID,
                                                                                       rc_InterfaceSettings.u8_NodeID);

                           s32_Return = this->mpc_ComDriver->SendOsySetBitrate(
                              orc_ServerId, rc_InterfaceSettings.u8_InterfaceNumber,
                              orc_DeviceConfig.c_CanBitrates[u32_CanCounter]);

                           this->m_RunConfOpenSydeDevicesState(hu32_SETCANBITRATE, s32_Return,
                                                               c_StateConfiguredInterface,
                                                               rc_InterfaceSettings.e_InterfaceType,
                                                               rc_InterfaceSettings.u8_InterfaceNumber);
                        }

                        // The size of c_CanBitrates depends on the number of CAN interfaces and it is possible
                        // that the interface configuration is not sorted for interface types
                        ++u32_CanCounter;

                        if (s32_Return != C_NO_ERR)
                        {
                           break;
                        }
                     }
                  }
               }

               if (s32_Return != C_NO_ERR)
               {
                  osc_write_log_error("Configure openSYDE devices",
                                      "openSYDE set CAN bitrate failed with error: " +
                                      C_SCLString::IntToStr(s32_Return));
                  break;
               }
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the ip address for all connected and configured ethernet interfacs of the server

   \param[in]  orc_ServerId      Server id for communication
   \param[in]  orc_DeviceConfig  Configuration for the server

   \return
   C_NO_ERR   Bitrate set on server successfully
   C_RANGE    device configuration is invalid
              openSYDE protocol not found
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
              no com driver installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong routine identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_SetEthOpenSydeIpAddress(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                                   const C_SyvDcDeviceConfiguation & orc_DeviceConfig) const
{
   sint32 s32_Return = C_CONFIG;

   if (this->mpc_ComDriver != NULL)
   {
      uint32 u32_NodeIndex;

      s32_Return = C_RANGE;

      if (this->mpc_ComDriver->GetNodeIndex(orc_ServerId, u32_NodeIndex) == true)
      {
         // All connected interfaces of the node must be in the device configuration
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodeIndex);

         if (pc_Node != NULL)
         {
            uint32 u32_EthCounter = 0U;
            uint32 u32_BusCounter;

            for (u32_BusCounter = 0U; u32_BusCounter < orc_DeviceConfig.c_BusIds.size(); ++u32_BusCounter)
            {
               uint32 u32_InterfaceCounter;
               s32_Return = C_NO_ERR;

               // We need the interface settings of the node which is connected to the bus with this id
               for (u32_InterfaceCounter = 0U;
                    u32_InterfaceCounter < pc_Node->c_Properties.c_ComInterfaces.size();
                    ++u32_InterfaceCounter)
               {
                  const C_OSCNodeComInterfaceSettings & rc_InterfaceSettings =
                     pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceCounter];
                  const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
                     rc_InterfaceSettings.u32_BusIndex);

                  if ((pc_Bus != NULL) &&
                      (pc_Bus->u8_BusID == orc_DeviceConfig.c_BusIds[u32_BusCounter]) &&
                      (rc_InterfaceSettings.q_IsBusConnected == true) &&
                      (rc_InterfaceSettings.e_InterfaceType == C_OSCSystemBus::eETHERNET))
                  {
                     const C_OSCProtocolDriverOsyNode c_StateConfiguredInterface(pc_Bus->u8_BusID,
                                                                                 rc_InterfaceSettings.u8_NodeID);

                     s32_Return = this->mpc_ComDriver->SendOsySetIpAddressForChannel(
                        orc_ServerId, rc_InterfaceSettings.u8_InterfaceNumber,
                        orc_DeviceConfig.c_IpAddresses[u32_EthCounter].au8_IpAddress,
                        orc_DeviceConfig.c_IpAddresses[u32_EthCounter].au8_NetMask,
                        orc_DeviceConfig.c_IpAddresses[u32_EthCounter].au8_DefaultGateway);

                     this->m_RunConfOpenSydeDevicesState(hu32_SETIPADDRESS, s32_Return, c_StateConfiguredInterface,
                                                         rc_InterfaceSettings.e_InterfaceType,
                                                         rc_InterfaceSettings.u8_InterfaceNumber);

                     // The size of c_IpAddresses depends on the number of Ethernet interfaces and it
                     // is possible that the interface configuration is not sorted for interface types
                     ++u32_EthCounter;

                     if (s32_Return != C_NO_ERR)
                     {
                        break;
                     }
                  }
               }

               if (s32_Return != C_NO_ERR)
               {
                  osc_write_log_error("Configure CAN openSYDE devices",
                                      "openSYDE set Ethernet IP address failed with error: " +
                                      C_SCLString::IntToStr(s32_Return));
                  break;
               }
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets node and bus ids for all configured communication interfaces except the interface of orc_ServerId

   \param[in]  orc_ServerId      Server id for communication
   \param[in]  orc_DeviceConfig  Configuration for the server

   \return
   C_RANGE    openSYDE protocol not found
              orc_ServerId is invalid
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
              no com driver installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_SetOpenSydeNodeIds(const C_OSCProtocolDriverOsyNode & orc_ServerId,
                                              const C_SyvDcDeviceConfiguation & orc_DeviceConfig) const
{
   sint32 s32_Return = C_CONFIG;

   if (this->mpc_ComDriver != NULL)
   {
      uint32 u32_NodeIndex;

      s32_Return = C_RANGE;

      if (this->mpc_ComDriver->GetNodeIndex(orc_ServerId, u32_NodeIndex) == true)
      {
         // All connected interfaces of the node must be in the device configuration
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodeIndex);

         if (pc_Node != NULL)
         {
            uint32 u32_BusCounter;

            for (u32_BusCounter = 0U; u32_BusCounter < orc_DeviceConfig.c_BusIds.size(); ++u32_BusCounter)
            {
               uint32 u32_InterfaceCounter;

               s32_Return = C_RANGE;

               // We need the interface settings of the node which is connected to the bus with this id
               for (u32_InterfaceCounter = 0U;
                    u32_InterfaceCounter < pc_Node->c_Properties.c_ComInterfaces.size();
                    ++u32_InterfaceCounter)
               {
                  const C_OSCNodeComInterfaceSettings & rc_InterfaceSettings =
                     pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceCounter];
                  const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
                     rc_InterfaceSettings.u32_BusIndex);

                  if ((pc_Bus != NULL) &&
                      (pc_Bus->u8_BusID == orc_DeviceConfig.c_BusIds[u32_BusCounter]) &&
                      (rc_InterfaceSettings.q_IsBusConnected == true))
                  {
                     //only if another interface than the one we are connected to ...
                     if ((orc_DeviceConfig.c_BusIds[u32_BusCounter] != orc_ServerId.u8_BusIdentifier) ||
                         (orc_DeviceConfig.c_NodeIds[u32_BusCounter] != orc_ServerId.u8_NodeIdentifier))
                     {
                        const C_OSCProtocolDriverOsyNode c_NewId(orc_DeviceConfig.c_BusIds[u32_BusCounter],
                                                                 orc_DeviceConfig.c_NodeIds[u32_BusCounter]);

                        s32_Return = this->mpc_ComDriver->SendOsySetNodeIdForChannel(
                           orc_ServerId, static_cast<uint8>(rc_InterfaceSettings.e_InterfaceType),
                           rc_InterfaceSettings.u8_InterfaceNumber, c_NewId);

                        this->m_RunConfOpenSydeDevicesState(hu32_SETNODEID, s32_Return, c_NewId,
                                                            rc_InterfaceSettings.e_InterfaceType,
                                                            rc_InterfaceSettings.u8_InterfaceNumber);
                        break;
                     }
                     else
                     {
                        // This channel was already configured and is used for the current connection to the client
                        s32_Return = C_NO_ERR;
                     }
                  }
               }

               if (s32_Return != C_NO_ERR)
               {
                  osc_write_log_error("Configure openSYDE devices",
                                      "openSYDE set node id by channel failed with error: " +
                                      C_SCLString::IntToStr(s32_Return));
                  break;
               }
            }
         }
         else
         {
            osc_write_log_error("Configure openSYDE devices", "Invalid node selected");
         }
      }
      else
      {
         osc_write_log_error("Configure openSYDE devices", "Could not determine node index");
      }
   }
   else
   {
      osc_write_log_error("Configure openSYDE devices", "Could not set node ID as no COMM driver was installed");
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read back information from configured nodes via CAN.

   Assumptions:
   * The com driver passed as parameter is initialized and has set up protocol
     instances for all STW Flashloader and openSYDE nodes
   * all nodes have unique IDs (i.e. configuration has been successfully performed)

   Sequence:
   * bring nodes into flashloader (same sequence as in "ScanCanEnterFlashloader")
   * Sequence of m_ReadBack

   As a result we'll have the serial numbers, Node-IDs, device-names of all devices

   \return
   C_NO_ERR   read back was successful
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed or no dispatcher installed
              no com driver installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_RANGE    Broadcast protocol not initialized
   C_COM      could not send request
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_ReadBackCan(void)
{
   sint32 s32_Return = C_CONFIG;

   osc_write_log_info("Read back CAN devices", "Sequence started");

   // Clear old results
   this->mc_DeviceInfoResult.clear();

   if (this->mpc_ComDriver != NULL)
   {
      s32_Return = this->m_ReadBack();
   }

   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info("Read back CAN devices", "Sequence finished");
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read back information from configured nodes via Ethernet.

   Assumptions:
   * The com driver passed as parameter is initialized and has set up protocol instances for all openSYDE nodes

   Sequence:
   * Reconnect to nodes
   * Sequence of m_ReadBack

   As a result we'll have the serial numbers, Node-IDs, device-names of all devices

   \return
   C_NO_ERR    read back was successful
   C_CONFIG    no com driver installed
   C_BUSY      could not re-connect to node
   C_RANGE     node not found or no openSYDE protocol installed
   C_CONFIG    Init function was not called or not successful or protocol was not initialized properly.
   C_NOACT     Could not put request in Tx queue
   C_WARN      Error response received
   C_TIMEOUT   Expected response not received within timeout
   C_COM       communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_ReadBackEth(void)
{
   sint32 s32_Return = C_CONFIG;

   osc_write_log_info("Read back ETH devices", "Sequence started");

   // Clear old results
   this->mc_DeviceInfoResult.clear();

   if (this->mpc_ComDriver != NULL)
   {
      uint32 u32_DeviceCounter;

      // * for all openSYDE nodes:
      // Connect to the devices again and prepare them for the read back function
      for (u32_DeviceCounter = 0U; u32_DeviceCounter < this->mc_OpenSydeIds.size(); ++u32_DeviceCounter)
      {
         // Get the information of the device

         stw_opensyde_core::C_OSCProtocolDriverOsyNode & rc_OsyServerId = this->mc_OpenSydeIds[u32_DeviceCounter];

         // we need to explicitly contact the node after the reset: reconnect (required for Ethernet)
         s32_Return = this->mpc_ComDriver->ReConnectNode(rc_OsyServerId);

         if (s32_Return != C_NO_ERR)
         {
            C_SCLString c_Text;
            c_Text.PrintFormatted("Could not reconnect to node with ID %d on bus with ID %d. Error code: %d",
                                  rc_OsyServerId.u8_NodeIdentifier,
                                  rc_OsyServerId.u8_BusIdentifier, s32_Return);
            osc_write_log_error("Configure openSYDE devices", c_Text);
         }
         else
         {
            s32_Return = this->mpc_ComDriver->SendOsySetPreProgrammingMode(rc_OsyServerId, false);

            if (s32_Return == C_NOACT)
            {
               // Node has no openSYDE protocol, protocol was not installed properly
               // Ethernet is only possible with openSYDE
               s32_Return = C_RANGE;
            }

            if (s32_Return != C_NO_ERR)
            {
               C_SCLString c_Text;
               c_Text.PrintFormatted("openSYDE setting preprogramming mode failed on node with ID %d on bus with ID"
                                     " %d with error: %d", rc_OsyServerId.u8_NodeIdentifier,
                                     rc_OsyServerId.u8_BusIdentifier,
                                     s32_Return);
               osc_write_log_error("Configure openSYDE devices", c_Text);
            }
         }

         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error("Read back ETH devices",
                                "Read back failed with error: " + C_SCLString::IntToStr(
                                   s32_Return));
         }

         if (s32_Return != C_NO_ERR)
         {
            break;
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = this->m_ReadBack();
      }

      if (s32_Return == C_NO_ERR)
      {
         osc_write_log_info("Read back ETH devices", "Sequence finished");
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read back information from configured nodes.

   Sequence:
   * uses "directed" services to read information from all nodes
   * for all openSYDE nodes:
   ** perform "readdatabyID(SerialNumber)"
   ** perform "readdatabyID(DeviceName)"
   * for all STW flashloader nodes:
   ** perform "node_wakeup(localId)"
   ** perform "node_companyid(Y*)"
   ** perform "get_serial_number"
   ** perform "get_device_id"

   As a result we'll have the serial numbers, Node-IDs, device-names of all devices

   \return
   C_NO_ERR   read back was successful
   C_CONFIG   no com driver installed or no transport protocol installed
   C_RANGE    openSYDE protocol not found
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_WARN     error response
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      error on creating temporary needed protocol
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcSequences::m_ReadBack(void)
{
   sint32 s32_Return = C_CONFIG;

   osc_write_log_info("Read back devices", "Sequence started");

   // Clear old results
   this->mc_DeviceInfoResult.clear();

   if (this->mpc_ComDriver != NULL)
   {
      uint32 u32_DeviceCounter;
      uint8 au8_SerialNumber[6];

      s32_Return = C_NO_ERR;

      // * for all openSYDE nodes:
      for (u32_DeviceCounter = 0U; u32_DeviceCounter < this->mc_OpenSydeIds.size(); ++u32_DeviceCounter)
      {
         C_SyvDcDeviceInformation c_OsyInfo;
         stw_opensyde_core::C_OSCProtocolDriverOsyNode & rc_OsyServerId = this->mc_OpenSydeIds[u32_DeviceCounter];

         c_OsyInfo.SetNodeId(rc_OsyServerId.u8_NodeIdentifier);

         // ** perform "readdatabyID(SerialNumber)"
         s32_Return = this->mpc_ComDriver->SendOsyReadSerialNumber(rc_OsyServerId, au8_SerialNumber);

         if (s32_Return == C_NO_ERR)
         {
            c_OsyInfo.SetSerialNumber(au8_SerialNumber);
         }
         else
         {
            osc_write_log_error("Read back devices",
                                "openSYDE read serial number failed with error: " +
                                C_SCLString::IntToStr(s32_Return));
         }

         if (s32_Return == C_NO_ERR)
         {
            C_SCLString c_OsyDeviceName;

            // ** perform "readdatabyID(DeviceName)"
            s32_Return = this->mpc_ComDriver->SendOsyReadDeviceName(rc_OsyServerId, c_OsyDeviceName);

            if (s32_Return == C_NO_ERR)
            {
               c_OsyInfo.SetDeviceName(c_OsyDeviceName);

               // All information for device ready
               this->mc_DeviceInfoResult.push_back(c_OsyInfo);
            }
            else
            {
               osc_write_log_error("Read back devices",
                                   "openSYDE read device name failed with error: " +
                                   C_SCLString::IntToStr(s32_Return));
            }
         }

         if (s32_Return != C_NO_ERR)
         {
            break;
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         // * for all STW flashloader nodes:
         for (u32_DeviceCounter = 0U; u32_DeviceCounter < this->mc_StwIds.size(); ++u32_DeviceCounter)
         {
            C_SyvDcDeviceInformation c_StwInfo;
            stw_opensyde_core::C_OSCProtocolDriverOsyNode & rc_StwServerId = this->mc_StwIds[u32_DeviceCounter];

            c_StwInfo.SetNodeId(rc_StwServerId.u8_NodeIdentifier);

            // ** perform "node_wakeup(localId)"
            // ** perform "node_companyid(Y*)"
            s32_Return = this->mpc_ComDriver->SendStwWakeupLocalId(rc_StwServerId, NULL);

            if (s32_Return == C_NO_ERR)
            {
               uint8 u8_NodesFound = 0U;

               // ** perform "get_serial_number"
               s32_Return = this->mpc_ComDriver->SendStwGetSerialNumbers(rc_StwServerId, &au8_SerialNumber[0],
                                                                         1, u8_NodesFound);

               if ((s32_Return == C_NO_ERR) &&
                   (u8_NodesFound == 1U))
               {
                  C_SCLString c_StwDeviceName;

                  c_StwInfo.SetSerialNumber(au8_SerialNumber);

                  // ** perform "get_device_id"
                  s32_Return = this->mpc_ComDriver->SendStwGetDeviceId(rc_StwServerId, c_StwDeviceName);

                  if (s32_Return == C_NO_ERR)
                  {
                     c_StwInfo.SetDeviceName(c_StwDeviceName);

                     // All information for device ready
                     this->mc_DeviceInfoResult.push_back(c_StwInfo);
                  }
                  else
                  {
                     osc_write_log_error("Read back devices",
                                         "STW Flashloader get device id failed with error: " +
                                         C_SCLString::IntToStr(s32_Return));
                  }
               }
               else
               {
                  C_SCLString c_Text;
                  c_Text.PrintFormatted("STW Flashloader get serial number failed with error: %d "
                                        "(Number of nodes found : %d", s32_Return, u8_NodesFound);
                  osc_write_log_error("Read back CAN devices", c_Text);
               }
            }
            else
            {
               osc_write_log_error("Read back devices",
                                   "STW Flashloader node wakeup failed with error: " +
                                   C_SCLString::IntToStr(s32_Return));
            }

            if (s32_Return != C_NO_ERR)
            {
               // Remapping of error codes to openSYDE service interpretation
               if (s32_Return == C_COM)
               {
                  // C_COM              no response from server
                  s32_Return = C_TIMEOUT;
               }
               if (s32_Return == C_NOACT)
               {
                  // C_NOACT            error response from server
                  s32_Return = C_WARN;
               }
               break;
            }
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info("Read back devices", "Sequence finished");
   }

   return s32_Return;
}
