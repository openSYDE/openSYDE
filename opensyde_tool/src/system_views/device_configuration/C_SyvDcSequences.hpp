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

#include "stwtypes.hpp"
#include "C_SclString.hpp"

#include "C_Can.hpp"
#include "TglTasks.hpp"
#include "C_OscComSequencesBase.hpp"
#include "C_OscProtocolSerialNumber.hpp"
#include "C_SyvComDriverThread.hpp"
#include "C_OscNodeComInterfaceSettings.hpp"
#include "C_OscIpDispatcherWinSock.hpp"
#include "C_OscSecurityPemDatabase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Basic information about one device
class C_SyvDcDeviceInformation
{
public:
   C_SyvDcDeviceInformation(void);

   void SetDeviceName(const stw::scl::C_SclString & orc_DeviceName);
   void SetNodeId(const uint8_t ou8_NodeId);
   void SetIpAddress(const uint8_t (&orau8_IpAddress)[4]);
   void SetSerialNumber(const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber);
   void SetExtendedInfo(const uint8_t ou8_SubNodeId, const bool oq_SecurityActivated);

   bool IsSerialNumberIdentical(const C_SyvDcDeviceInformation & orc_Cmp) const;

   stw::scl::C_SclString c_DeviceName;
   bool q_DeviceNameValid;

   uint8_t u8_NodeId;
   bool q_NodeIdValid;

   uint8_t au8_IpAddress[4];
   bool q_IpAddressValid;

   stw::opensyde_core::C_OscProtocolSerialNumber c_SerialNumber;

   // Extended specific information
   uint8_t u8_SubNodeId;
   bool q_SecuirtyActivated;
   bool q_ExtendedInfoValid;
};

///Old communication configuration of a node
class C_SyvDcDeviceOldComConfig
{
public:
   C_SyvDcDeviceOldComConfig(void);
   C_SyvDcDeviceOldComConfig & operator =(const C_SyvDcDeviceOldComConfig & orc_Source) &;

   void SetContent(const uint8_t ou8_OldNodeId, const bool oq_OldIpAddressValid,
                   const uint8_t * const opu8_OldIpAddress = NULL);

   // Old node id which was used and detected by getting serial number
   uint8_t u8_OldNodeId;
   uint8_t au8_OldIpAddress[4];
   bool q_OldIpAddressValid;
};

///Target configuration for one device
class C_SyvDcDeviceConfiguation
{
public:
   C_SyvDcDeviceConfiguation(void);
   C_SyvDcDeviceConfiguation & operator =(const C_SyvDcDeviceConfiguation & orc_Source) &;

   // Identifier of the device
   stw::opensyde_core::C_OscProtocolSerialNumber c_SerialNumber;
   uint8_t u8_SubNodeId;

   //Communication configuration of node which was detected by getting serial number
   C_SyvDcDeviceOldComConfig c_OldComConfig;

   // Configuration parameters for all interfaces
   std::vector<uint8_t> c_NodeIds;
   std::vector<uint8_t> c_BusIds;
   std::vector<uint32_t> c_CanBitrates; // In Bit/s
   std::vector<stw::opensyde_core::C_OscNodeComInterfaceSettings::C_IpAddress> c_IpAddresses;
};

class C_SyvDcSequences :
   public QObject,
   public stw::opensyde_core::C_OscComSequencesBase
{
   Q_OBJECT

public:
   C_SyvDcSequences(void);
   ~C_SyvDcSequences(void) override;

   int32_t InitDcSequences(const uint32_t ou32_ViewIndex);

   // Helper function
   int32_t FillDeviceConfig(C_SyvDcDeviceConfiguation & orc_Config,
                            stw::opensyde_core::C_OscNodeProperties::E_FlashLoaderProtocol & ore_Flashloader)
   const;

   int32_t ScanCanEnterFlashloader(const uint32_t ou32_UsedBitrate);
   int32_t ScanCanSendFlashloaderRequest(const uint32_t ou32_ScanTime, const bool oq_ScanEndless);
   void StopScanCanSendFlashloaderRequest(void);
   int32_t ScanCanGetInfoFromStwFlashloaderDevices(void);
   int32_t ScanCanGetInfoFromOpenSydeDevices(void);
   int32_t ScanEthGetInfoFromOpenSydeDevices(void);

   int32_t CheckOpenSydeDevicesConfig(const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfig) const;
   int32_t ConfCanStwFlashloaderDevices(const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfig);
   int32_t ConfCanOpenSydeDevices(const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfig,
                                  const bool oq_ConfigureAllInterfaces, const bool oq_SecurityFeatureUsed);
   int32_t ConfEthOpenSydeDevices(const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfig,
                                  const bool oq_ConfigureAllInterfaces, const bool oq_SecurityFeatureUsed);

   int32_t SendOsyBroadcastRequestProgramming(bool & orq_NotAccepted) const;
   int32_t ResetCanStwFlashloaderDevices(void);
   int32_t ResetOpenSydeDevices(const bool oq_ToFlashloader) const;

   int32_t InitCanAndSetCanBitrate(const uint32_t ou32_Bitrate);

   int32_t ReadBackCan(const std::vector<stw::opensyde_core::C_OscProtocolDriverOsyNode> & orc_OpenSydeIds,
                       const std::vector<bool> & orc_OpenSydeSnrExtFormat,
                       const std::vector<stw::opensyde_core::C_OscProtocolDriverOsyNode> & orc_StwIds);
   int32_t ReadBackEth(const std::vector<stw::opensyde_core::C_OscProtocolDriverOsyNode> & orc_OpenSydeIds,
                       const std::vector<bool> & orc_OpenSydeSnrExtFormat);

   int32_t GetResults(int32_t & ors32_Result) const;
   bool GetCanInitializationResult(void) const;
   int32_t GetDeviceInfosResult(std::vector<C_SyvDcDeviceInformation> & orc_DeviceInfo) const;
   int32_t GetSecurityFeatureUsageResult(bool & orq_SecurityFeatureUsed) const;

   static const uint32_t hu32_SETNODEID = 0U;
   static const uint32_t hu32_SETCANBITRATE = 1U;
   static const uint32_t hu32_SETIPADDRESS = 2U;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigRunConfCanStwFlashloaderDevicesProgress(const uint32_t ou32_Progress);
   void SigRunConfCanOpenSydeDevicesProgress(const uint32_t ou32_Progress);
   void SigRunConfEthOpenSydeDevicesProgress(const uint32_t ou32_Progress);

   void SigRunConfCanStwFlashloaderDevicesState(const uint32_t ou32_Step, const int32_t os32_Result,
                                                const uint8_t ou8_BusIdentifier,
                                                const uint8_t ou8_NodeIdentifier) const;
   void SigRunConfOpenSydeDevicesState(const uint32_t ou32_Step, const int32_t os32_Result,
                                       const uint8_t ou8_BusIdentifier, const uint8_t ou8_NodeIdentifier,
                                       const uint8_t ou8_InterfaceType, const uint8_t ou8_InterfaceNumber) const;

protected:
   int32_t m_XflReportProgress(const uint8_t ou8_Progress, const stw::scl::C_SclString & orc_Text) override;

   void m_RunConfCanStwFlashloaderDevicesProgress(const uint32_t ou32_Progress);
   void m_RunConfCanOpenSydeDevicesProgress(const uint32_t ou32_Progress);
   void m_RunConfEthOpenSydeDevicesProgress(const uint32_t ou32_Progress);

   void m_RunConfCanStwFlashloaderDevicesState(const uint32_t ou32_Step, const int32_t os32_Result,
                                               const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_Server)
   const;
   void m_RunConfOpenSydeDevicesState(const uint32_t ou32_Step, const int32_t os32_Result,
                                      const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_Server,
                                      const stw::opensyde_core::C_OscSystemBus::E_Type oe_InterfaceType,
                                      const uint8_t ou8_InterfaceNumber) const;

private:
   //Avoid call
   C_SyvDcSequences(const C_SyvDcSequences &);
   C_SyvDcSequences & operator =(const C_SyvDcSequences &) &; //lint !e1511 //we want to hide the base func.

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

   int32_t m_RunScanCanEnterFlashloader(const uint32_t ou32_CanBitrate);
   int32_t m_RunScanCanSendFlashloaderRequest(const uint32_t ou32_ScanTime);
   int32_t m_RunScanCanGetInfoFromStwFlashloaderDevices(void);
   int32_t m_RunScanCanGetInfoFromStwFlashloaderDevice(const uint8_t ou8_LocalId);
   int32_t m_RunScanCanGetInfoFromOpenSydeDevices(void);
   int32_t m_RunConfCanStwFlashloaderDevices(void);
   int32_t m_RunConfCanOpenSydeDevices(void);
   int32_t m_RunConfCanOpenSydeDevicesWithBroadcasts(
      std::vector<stw::opensyde_core::C_OscProtocolDriverOsyNode> & orc_UsedServerIds);
   int32_t m_RunConfCanOpenSydeDevicesWithoutBroadcasts(
      std::vector<stw::opensyde_core::C_OscProtocolDriverOsyNode> & orc_UsedServerIds);

   int32_t m_RunScanEthGetInfoFromOpenSydeDevices(void);
   int32_t m_RunConfEthOpenSydeDevices(void);
   int32_t m_RunConfEthOpenSydeDevicesWithBroadcasts(
      std::vector<stw::opensyde_core::C_OscProtocolDriverOsyNode> & orc_UsedServerIds);
   int32_t m_RunConfEthOpenSydeDevicesWithoutBroadcasts(
      std::vector<stw::opensyde_core::C_OscProtocolDriverOsyNode> & orc_UsedServerIds);

   int32_t m_ConfigureNodes(const bool oq_ViaCan,
                            std::vector<stw::opensyde_core::C_OscProtocolDriverOsyNode> & orc_UsedServerIds);

   int32_t m_CheckConfOpenSydeDevices(const std::vector<C_SyvDcDeviceConfiguation> & orc_DeviceConfiguration)
   const;
   int32_t m_SetCanOpenSydeBitrate(const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_ServerId,
                                   const C_SyvDcDeviceConfiguation & orc_DeviceConfig) const;
   int32_t m_SetEthOpenSydeIpAddress(const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_ServerId,
                                     const C_SyvDcDeviceConfiguation & orc_DeviceConfig) const;
   int32_t m_SetOpenSydeNodeIds(const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_ServerId,
                                const C_SyvDcDeviceConfiguation & orc_DeviceConfig) const;

   int32_t m_ReadBackCan(void);
   int32_t m_ReadBackEth(void);
   int32_t m_ReadBack(void);

   C_SyvComDriverThread * mpc_Thread;
   stw::tgl::C_TglCriticalSection mc_CriticalSectionRequestEndless;
   stw::can::C_Can * mpc_CanDllDispatcher;
   stw::opensyde_core::C_OscIpDispatcherWinSock * mpc_EthernetDispatcher;
   // Sequence execution parameter
   E_Sequence me_Sequence;
   // Input parameter for sequence
   std::vector<C_SyvDcDeviceConfiguation> mc_DeviceConfiguration; ///< desired device configuration
   std::vector<stw::opensyde_core::C_OscProtocolDriverOsyNode> mc_OpenSydeIds;
   std::vector<bool> mc_OpenSydeSnrExtFormat;
   std::vector<stw::opensyde_core::C_OscProtocolDriverOsyNode> mc_StwIds;
   uint32_t mu32_CanBitrate;
   bool mq_ConfigureAllInterfaces; ///< flag if the bitrate for all connected interfaces sould be configured or
   // only interfaces connected to the current bus

   // Scan flashloader request variables
   uint32_t mu32_ScanTime;
   bool mq_RunScanSendFlashloaderRequestEndless;

   // CAN Scan flashloader output
   bool mq_CanInitialized;

   // It is mutable because of the constness of the getter functions. Without the keyword mutable the getter functions
   // must be non const and that is not wanted.
   mutable stw::tgl::C_TglCriticalSection mc_CriticalSectionCanInitialization;

   // Result information of several sequences
   std::vector<C_SyvDcDeviceInformation> mc_DeviceInfoResult;
   // Result of ScanCanGetInfoFromOpenSydeDevices and ScanEthGetInfoFromOpenSydeDevices
   // Input of ConfCanOpenSydeDevices and ConfEthOpenSydeDevices
   bool mq_SecurityFeatureUsed;
   // Service execution result
   int32_t ms32_Result;

   // Security PEM database
   stw::opensyde_core::C_OscSecurityPemDatabase mc_PemDatabase;

   static const uint32_t mhu32_DEFAULT_SCAN_TIME_MS = 5000U;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
