//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Sub device part of device definition
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSUBDEVICEDEFINITION_H
#define C_OSCSUBDEVICEDEFINITION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <map>
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCSystemBus.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCSubDeviceDefinition
{
public:
   C_OSCSubDeviceDefinition();
   //lint -sem(stw_opensyde_core::C_OSCSubDeviceDefinition::Clear,initializer)
   void Clear(void);

   bool IsUpdateAvailable(const C_OSCSystemBus::E_Type oe_Type) const;
   bool IsRoutingAvailable(const C_OSCSystemBus::E_Type oe_Type) const;
   bool IsDiagnosisAvailable(const C_OSCSystemBus::E_Type oe_Type) const;
   bool IsConnected(const C_OSCSystemBus::E_Type oe_Type, const stw_types::uint8 ou8_InterfaceNumber) const;
   static stw_scl::C_SCLString h_GetInterfaceNameLower(const C_OSCSystemBus::E_Type oe_Type,
                                                       const stw_types::uint8 ou8_InterfaceNumber);

   stw_scl::C_SCLString c_SubDeviceName; ///< Name of sub device

   std::map<stw_scl::C_SCLString, bool> c_ConnectedInterfaces;

   std::vector<stw_scl::C_SCLString> c_OtherAcceptedNames; ///< Other compatible names for this device

   ///is programming supported enabled?
   bool q_ProgrammingSupport;

   ///is the KEFEX protocol supported (on CAN bus) ?
   bool q_DiagnosticProtocolKefex;
   ///is the openSYDE protocol supported on CAN bus ?
   bool q_DiagnosticProtocolOpenSydeCan;
   ///is the openSYDE protocol supported on ethernet ?
   bool q_DiagnosticProtocolOpenSydeEthernet;

   ///is the STW flashloader available (on CAN bus) ?
   bool q_FlashloaderStwCan;
   ///is the openSYDE flashloader available on CAN bus ?
   bool q_FlashloaderOpenSydeCan;
   ///is the openSYDE flashloader available on ethernet ?
   bool q_FlashloaderOpenSydeEthernet;
   ///is the device file based or address based?
   bool q_FlashloaderOpenSydeIsFileBased;

   ///minimal times in ms to reset from the application to the Flashloader or reset the Flashloader itself.
   ///possible scenario: Reset from Application to Flashloader or from Flashloader to Flashloader
   ///no communication relevant configuration was changed.
   stw_types::uint32 u32_FlashloaderResetWaitTimeNoChangesCan;      ///<CAN is used. default time is 1000ms
   stw_types::uint32 u32_FlashloaderResetWaitTimeNoChangesEthernet; ///<Eth is used. default time is 5500ms

   ///Single scenario: From Flashloader to Flashloader
   ///no fundamental communication relevant configuration was changed (Bus-Id or Node Id).
   stw_types::uint32 u32_FlashloaderResetWaitTimeNoFundamentalChangesCan;      ///<CAN is used. default time is 1000ms
   stw_types::uint32 u32_FlashloaderResetWaitTimeNoFundamentalChangesEthernet; ///<Eth is used. default time is 5500ms

   /// Single scenario: From Flashloader to Flashloader
   ///fundamental communication relevant configuration was changed (CAN bitrate or IP address).
   stw_types::uint32 u32_FlashloaderResetWaitTimeFundamentalChangesCan;      ///<CAN is used. default time is 1000ms
   stw_types::uint32 u32_FlashloaderResetWaitTimeFundamentalChangesEthernet; ///<Eth is used. default time is 5500ms

   ///the maximum time in ms it can take to erase one continuous area in flash
   stw_types::uint32 u32_FlashloaderOpenSydeRequestDownloadTimeout;
   ///the maximum time in ms it can take to write up to 4kB of data to flash
   stw_types::uint32 u32_FlashloaderOpenSydeTransferDataTimeout;

   ///size of EEPROM memory available to the user application in bytes
   stw_types::uint32 u32_UserEepromSizeBytes;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
