//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       File handler for device definition file data

   see .cpp file for main information

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDEVICEDEFINITIONFILERH
#define C_OSCDEVICEDEFINITIONFILERH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCDeviceDefinition.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCDeviceDefinitionFiler
{
private:
   static const stw_types::uint16 mhu16_FILE_VERSION; ///< file version this class can read / write

   static void mh_ParseOpenSydeAvailability(const C_OSCXMLParser & orc_Parser, bool & orq_ProtocolSupportedCan,
                                            bool & orq_ProtocolSupportedEthernet);
   static void mh_ParseOpenSydeFlashloaderParameter(const C_OSCXMLParser & orc_Parser,
                                                    stw_types::uint32 & oru32_RequestDownloadTimeout,
                                                    stw_types::uint32 & oru32_TransferDataTimeout,
                                                    bool & orq_IsFileBased);
   static void mh_ParseSTWFlashloaderAvailability(const C_OSCXMLParser & orc_Parser, bool & orq_ProtocolSupportedCan);

   static stw_types::sint32 mh_Load(C_OSCDeviceDefinition & orc_DeviceDefinition, C_OSCXMLParser & orc_Parser,
                                    const stw_scl::C_SCLString & orc_Path);
   static stw_types::sint32 mh_LoadSubDevice(C_OSCSubDeviceDefinition & orc_SubDeviceDefinition,
                                             C_OSCXMLParser & orc_Parser,
                                             const C_OSCDeviceDefinition & orc_DeviceDefinition,
                                             const stw_scl::C_SCLString & orc_Path);
   static void mh_SaveSubDevice(const C_OSCSubDeviceDefinition & orc_SubDeviceDefinition, C_OSCXMLParser & orc_Parser);
   static stw_types::sint32 mh_HandleConnectedInterfaces(C_OSCDeviceDefinition & orc_DeviceDefinition);
   static stw_types::sint32 mh_CheckContentErrors(const C_OSCDeviceDefinition & orc_DeviceDefinition);
   static stw_types::sint32 mh_CheckNotConnectedInterface(const C_OSCDeviceDefinition & orc_DeviceDefinition);
   static stw_types::sint32 mh_CheckNotConnectedInterfaceByType(const C_OSCDeviceDefinition & orc_DeviceDefinition,
                                                                const C_OSCSystemBus::E_Type oe_Type,
                                                                const stw_types::uint8 ou8_NumAvailableInterfaces);
   static stw_types::sint32 mh_CheckNotConnectedDevice(const C_OSCDeviceDefinition & orc_DeviceDefinition);
   static bool mh_CheckNotConnectedDeviceByType(const C_OSCSubDeviceDefinition & orc_SubDeviceDefinition,
                                                const C_OSCSystemBus::E_Type oe_Type,
                                                const stw_types::uint8 ou8_NumAvailableInterfaces);
   static stw_types::sint32 mh_CheckDeviceName(const C_OSCDeviceDefinition & orc_DeviceDefinition);

public:
   static stw_types::sint32 h_Load(C_OSCDeviceDefinition & orc_DeviceDefinition, const stw_scl::C_SCLString & orc_Path);
   static stw_types::sint32 h_Save(const C_OSCDeviceDefinition & orc_DeviceDefinition,
                                   const stw_scl::C_SCLString & orc_Path);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
