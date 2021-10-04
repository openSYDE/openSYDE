//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Device definition data container

   See .cpp file for full description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDEVICEDEFINITIONH
#define C_OSCDEVICEDEFINITIONH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.h"
#include "CSCLString.h"

#include "C_OSCSubDeviceDefinition.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///container for definition of one openSYDE device
class C_OSCDeviceDefinition
{
public:
   C_OSCDeviceDefinition(void);
   //lint -sem(stw_opensyde_core::C_OSCDeviceDefinition::Clear,initializer)
   void Clear(void);

   stw_scl::C_SCLString GetDisplayName(void) const;

   stw_scl::C_SCLString c_DeviceName;        ///< full device name used for checks
   stw_scl::C_SCLString c_DeviceNameAlias;   ///< full displayed name of device
   stw_scl::C_SCLString c_DeviceDescription; ///< text describing device
   stw_scl::C_SCLString c_ImagePath;         ///< path to file with image of device (e.g. JPG)
   stw_scl::C_SCLString c_FilePath;          ///< used for service update package (#24474)
   stw_types::uint8 u8_NumCanBusses;         ///< number of CAN busses present on device
   stw_types::uint8 u8_NumEthernetBusses;    ///< number of ethernet interfaces present on device

   std::vector<stw_types::uint16> c_SupportedBitrates; ///< supported CAN bitrates in kbit/s

   stw_types::uint8 u8_ManufacturerId;              ///< manufacturer id used by device configuration
   stw_scl::C_SCLString c_ManufacturerDisplayValue; ///< manufacturer string, displayed in device configuration

   std::vector<C_OSCSubDeviceDefinition> c_SubDevices;

   static const stw_scl::C_SCLString hc_DefaultCompanyName;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
