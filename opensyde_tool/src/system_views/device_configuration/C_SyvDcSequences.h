//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for sequences required for device configuration. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDCSEQUENCES_H
#define C_SYVDCSEQUENCES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <QObject>
#include <QString>

#include "stwtypes.h"
#include "CSCLString.h"

#include "C_OSCComSequencesBase.h"
#include "C_SyvComDriverThread.h"
#include "C_OSCNodeComInterfaceSettings.h"
#include "CCAN.h"
#include "C_OSCIpDispatcherWinSock.h"
#include "TGLTasks.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Basic information about one device
class C_SyvDcDeviceInformation
{
public:
   C_SyvDcDeviceInformation(void);

   void SetDeviceName(const stw_scl::C_SCLString & orc_DeviceName);
   void SetSerialNumber(const stw_types::uint8 (&orau8_SerialNumber)[6]);
   void SetNodeId(const stw_types::uint8 ou8_NodeId);
   void SetIpAddress(const stw_types::uint8 (&orau8_IpAddress)[4]);

   static bool h_SerialNumberFromStringToArray(const stw_scl::C_SCLString & orc_SerialNumber,
                                               stw_types::uint8 * const opu8_SerialNumer);

   stw_scl::C_SCLString c_DeviceName;
   bool q_DeviceNameValid;

   stw_types::uint8 au8_SerialNumber[6];
   bool q_SerialNumberValid;

   stw_types::uint8 u8_NodeId;
   bool q_NodeIdValid;

   stw_types::uint8 au8_IpAddress[4];
   bool q_IpAddressValid;
};

///Target configuration for one device
class C_SyvDcDeviceConfiguation
{
public:
   C_SyvDcDeviceConfiguation(void);
   C_SyvDcDeviceConfiguation & operator =(const C_SyvDcDeviceConfiguation & orc_Source);

   // Identifier of the device
   stw_types::uint8 au8_SerialNumber[6];
   // Configuration parameters for all interfaces
   std::vector<stw_types::uint8> c_NodeIds;
   std::vector<stw_types::uint8> c_BusIds;
   std::vector<stw_types::uint32> c_CanBitrates; // In Bit/s
   std::vector<stw_opensyde_core::C_OSCNodeComInterfaceSettings::C_IpAddress> c_IpAddresses;
};

class C_SyvDcSequences :
   public QObject,
   public stw_opensyde_core::C_OSCComSequencesBase
{
   Q_OBJECT

public:
   C_SyvDcSequences(void);
   virtual ~C_SyvDcSequences(void);

   stw_types::sint32 InitDcSequences(const stw_types::uint32 ou32_ViewIndex);

   // Helper function
   stw_types::sint32 FillDeviceConfig(C_SyvDcDeviceConfiguation & orc_Config,
                                      stw_opensyde_core::C_OSCNodeProperties::E_FlashLoaderProtocol & ore_Flashloader)
   const;

   stw_types::sint32 ScanCanEnterFlashloader(const stw_types::uint32 ou32_UsedBitrate);
   stw_types::sint32 ScanCanSendFlashloaderRequest(const stw_types::uint32 ou32_ScanTime, const bool oq_ScanEndless);
   void StopScanCanSendFlashloaderRequest(void);
   stw_types::sint32 ScanCanGetInfoFromStwFlashloaderDevices(void);
   stw_types::sint32 ScanCanGetInfoFromOpenSydeDevices(void);
   stw_types::sint32 ScanEthGetInfoFromOpenSydeDevices(void);

   stw_types::sint32 CheckOpenSydeDevicesConfig(const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfig) const;
   stw_types::sint32 ConfCanStwFlashloaderDevices(const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfig);
   stw_types::sint32 ConfCanOpenSydeDevices(const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfig,
                                            const bool oq_ConfigureAllInterfaces);
   stw_types::sint32 ConfEthOpenSydeDevices(const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfig,
                                            const bool oq_ConfigureAllInterfaces);

   stw_types::sint32 SendOsyBroadcastRequestProgramming(bool & orq_NotAccepted) const;
   stw_types::sint32 ResetCanStwFlashloaderDevices(void);
   stw_types::sint32 ResetCanOpenSydeDevices(const bool oq_ToFlashloader) const;
   stw_types::sint32 ResetEthOpenSydeDevices(const bool oq_ToFlashloader) const;

   stw_types::sint32 InitCanAndSetCanBitrate(const stw_types::uint32 ou32_Bitrate);

   stw_types::sint32 ReadBackCan(const std::vector<stw_opensyde_core::C_OSCProtocolDriverOsyNode> & orc_OpenSydeIds,
                                 const std::vector<stw_opensyde_core::C_OSCProtocolDriverOsyNode> & orc_StwIds);
   stw_types::sint32 ReadBackEth(const std::vector<stw_opensyde_core::C_OSCProtocolDriverOsyNode> & orc_OpenSydeIds);

   stw_types::sint32 GetResults(stw_types::sint32 & ors32_Result) const;
   stw_types::sint32 GetDeviceInfosResult(std::vector<C_SyvDcDeviceInformation> & orc_DeviceInfo) const;

   static const stw_types::uint32 hu32_SETNODEID = 0U;
   static const stw_types::uint32 hu32_SETCANBITRATE = 1U;
   static const stw_types::uint32 hu32_SETIPADDRESS = 2U;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigRunConfCanStwFlashloaderDevicesProgress(const stw_types::uint32 ou32_Progress);
   void SigRunConfCanOpenSydeDevicesProgress(const stw_types::uint32 ou32_Progress);
   void SigRunConfEthOpenSydeDevicesProgress(const stw_types::uint32 ou32_Progress);

   void SigRunConfCanStwFlashloaderDevicesState(const stw_types::uint32 ou32_Step, const stw_types::sint32 os32_Result,
                                                const stw_types::uint8 ou8_BusIdentifier,
                                                const stw_types::uint8 ou8_NodeIdentifier) const;
   void SigRunConfOpenSydeDevicesState(const stw_types::uint32 ou32_Step, const stw_types::sint32 os32_Result,
                                       const stw_types::uint8 ou8_BusIdentifier,
                                       const stw_types::uint8 ou8_NodeIdentifier,
                                       const stw_types::uint8 ou8_InterfaceType,
                                       const stw_types::uint8 ou8_InterfaceNumber) const;

protected:
   virtual stw_types::sint32 m_XflReportProgress(const stw_types::uint8 ou8_Progress,
                                                 const stw_scl::C_SCLString & orc_Text);

   void m_RunConfCanStwFlashloaderDevicesProgress(const stw_types::uint32 ou32_Progress);
   void m_RunConfCanOpenSydeDevicesProgress(const stw_types::uint32 ou32_Progress);
   void m_RunConfEthOpenSydeDevicesProgress(const stw_types::uint32 ou32_Progress);

   void m_RunConfCanStwFlashloaderDevicesState(const stw_types::uint32 ou32_Step, const stw_types::sint32 os32_Result,
                                               const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_Server)
   const;
   void m_RunConfOpenSydeDevicesState(const stw_types::uint32 ou32_Step, const stw_types::sint32 os32_Result,
                                      const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_Server,
                                      const stw_opensyde_core::C_OSCSystemBus::E_Type oe_InterfaceType,
                                      const stw_types::uint8 ou8_InterfaceNumber) const;

private:
   //Avoid call
   C_SyvDcSequences(const C_SyvDcSequences &);
   C_SyvDcSequences & operator =(const C_SyvDcSequences &);

   static void mh_ThreadFunc(void * const opv_Instance);
   void m_ThreadFunc(void);

   enum E_Sequence
   {
      eSCANCANENTERFLASHLOADER,
      eSCANCANSENDFLASHLOADERREQUEST,
      eSCANCANGETINFOFROMSTWFLASHLOADERDEVICES,
      eSCANCANGETINFOFROMOPENSYDEDEVICES,
      eSCANETHGETINFOFROMOPENSYDEDEVICES,
      eCONFCANSTWFLASHLOADERDEVICES,
      eCONFCANOPENSYDEDEVICES,
      eCONFETHOPENSYDEDEVICES,
      eREADBACKCAN,
      eREADBACKETH
   };

   stw_types::sint32 m_RunScanCanEnterFlashloader(const stw_types::uint32 ou32_CanBitrate);
   stw_types::sint32 m_RunScanCanSendFlashloaderRequest(const stw_types::uint32 ou32_ScanTime);
   stw_types::sint32 m_RunScanCanGetInfoFromStwFlashloaderDevices(void);
   stw_types::sint32 m_RunScanCanGetInfoFromStwFlashloaderDevice(const stw_types::uint8 ou8_LocalId);
   stw_types::sint32 m_RunScanCanGetInfoFromOpenSydeDevices(void);
   stw_types::sint32 m_RunConfCanStwFlashloaderDevices(void);
   stw_types::sint32 m_RunConfCanOpenSydeDevices(void);

   stw_types::sint32 m_RunScanEthGetInfoFromOpenSydeDevices(void);
   stw_types::sint32 m_RunConfEthOpenSydeDevices(void);

   stw_types::sint32 m_ConfigureNodes(const bool oq_ViaCan,
                                      std::vector<stw_opensyde_core::C_OSCProtocolDriverOsyNode> & orc_UsedServerIds);

   stw_types::sint32 m_CheckConfOpenSydeDevices(const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfiguration)
   const;
   stw_types::sint32 m_SetCanOpenSydeBitrate(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                             const C_SyvDcDeviceConfiguation & orc_DeviceConfig) const;
   stw_types::sint32 m_SetEthOpenSydeIpAddress(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                               const C_SyvDcDeviceConfiguation & orc_DeviceConfig) const;
   stw_types::sint32 m_SetOpenSydeNodeIds(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                                          const C_SyvDcDeviceConfiguation & orc_DeviceConfig) const;

   stw_types::sint32 m_ReadBackCan(void);
   stw_types::sint32 m_ReadBackEth(void);
   stw_types::sint32 m_ReadBack(void);

   C_SyvComDriverThread * mpc_Thread;
   stw_tgl::C_TGLCriticalSection mc_CriticalSection;
   stw_can::C_CAN * mpc_CanDllDispatcher;
   stw_opensyde_core::C_OSCIpDispatcherWinSock * mpc_EthernetDispatcher;
   // Sequence execution parameter
   E_Sequence me_Sequence;
   // Input parameter for sequence
   std::vector<C_SyvDcDeviceConfiguation> mc_DeviceConfiguration; ///< desired device configuration
   std::vector<stw_opensyde_core::C_OSCProtocolDriverOsyNode> mc_OpenSydeIds;
   std::vector<stw_opensyde_core::C_OSCProtocolDriverOsyNode> mc_StwIds;
   stw_types::uint32 mu32_CanBitrate;
   bool mq_ConfigureAllInterfaces; ///< flag if the bitrate for all connected interfaces sould be configured or
   // only interfaces connected to the current bus

   // Scan flashloader request variables
   stw_types::uint32 mu32_ScanTime;
   bool mq_RunScanSendFlashloaderRequestEndless;

   // Result information of several sequences
   std::vector<C_SyvDcDeviceInformation> mc_DeviceInfoResult;
   // Service execution result
   stw_types::sint32 ms32_Result;

   static const stw_types::uint32 mhu32_DEFAULT_SCAN_TIME_MS = 5000U;
   static const stw_types::uint32 mhu32_RESET_WAIT_TIME_MS = 5000U;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
