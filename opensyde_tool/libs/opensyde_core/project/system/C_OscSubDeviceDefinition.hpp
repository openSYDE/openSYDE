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
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscSystemBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSubDeviceDefinition
{
public:
   C_OscSubDeviceDefinition();
   //lint -sem(stw::opensyde_core::C_OscSubDeviceDefinition::Clear,initializer)
   void Clear(void);

   bool IsUpdateAvailable(const C_OscSystemBus::E_Type oe_Type) const;
   bool IsRoutingAvailable(const C_OscSystemBus::E_Type oe_Type) const;
   bool IsDiagnosisAvailable(const C_OscSystemBus::E_Type oe_Type) const;
   bool IsConnected(const C_OscSystemBus::E_Type oe_Type, const uint8_t ou8_InterfaceNumber) const;
   static stw::scl::C_SclString h_GetInterfaceNameLower(const C_OscSystemBus::E_Type oe_Type,
                                                        const uint8_t ou8_InterfaceNumber);

   stw::scl::C_SclString c_SubDeviceName; ///< Name of sub device

   std::map<stw::scl::C_SclString, bool> c_ConnectedInterfaces;

   std::vector<stw::scl::C_SclString> c_OtherAcceptedNames; ///< Other compatible names for this device

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
   uint32_t u32_FlashloaderResetWaitTimeNoChangesCan;      ///<CAN is used. default time is 1000ms
   uint32_t u32_FlashloaderResetWaitTimeNoChangesEthernet; ///<Eth is used. default time is 5500ms

   ///Single scenario: From Flashloader to Flashloader
   ///no fundamental communication relevant configuration was changed (Bus-Id or Node Id).
   uint32_t u32_FlashloaderResetWaitTimeNoFundamentalChangesCan;      ///<CAN is used. default time is 1000ms
   uint32_t u32_FlashloaderResetWaitTimeNoFundamentalChangesEthernet; ///<Eth is used. default time is 5500ms

   /// Single scenario: From Flashloader to Flashloader
   ///fundamental communication relevant configuration was changed (CAN bitrate or IP address).
   uint32_t u32_FlashloaderResetWaitTimeFundamentalChangesCan;      ///<CAN is used. default time is 1000ms
   uint32_t u32_FlashloaderResetWaitTimeFundamentalChangesEthernet; ///<Eth is used. default time is 5500ms

   ///the maximum time in ms it can take to erase one continuous area in flash
   uint32_t u32_FlashloaderOpenSydeRequestDownloadTimeout;
   ///the maximum time in ms it can take to write up to 4kB of data to flash
   uint32_t u32_FlashloaderOpenSydeTransferDataTimeout;

   ///size of EEPROM memory available to the user application in bytes
   uint32_t u32_UserEepromSizeBytes;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
