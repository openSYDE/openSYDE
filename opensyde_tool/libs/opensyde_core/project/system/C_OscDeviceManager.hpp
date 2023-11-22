//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Manager for all device descriptions (header)

   Manager for all device descriptions (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDEVICEMANAGER_HPP
#define C_OSCDEVICEMANAGER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SclString.hpp"
#include "C_OscDeviceGroup.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscDeviceManager
{
public:
   const C_OscDeviceDefinition * LookForDevice(const stw::scl::C_SclString & orc_Name,
                                               const stw::scl::C_SclString & orc_MainDeviceName,
                                               uint32_t & oru32_SubDeviceIndex) const;

   int32_t AddDevice(const stw::scl::C_SclString & orc_DeviceDefinitionFile,
                     const stw::scl::C_SclString & orc_DeviceGroup, const stw::scl::C_SclString & orc_IniFile);

   int32_t ChangeDevices(std::vector<C_OscDeviceDefinition> & orc_Devices,
                         const stw::scl::C_SclString & orc_DeviceGroup, const stw::scl::C_SclString & orc_IniFile);

   std::vector<C_OscDeviceGroup> GetDeviceGroups(void) const;

   bool WasLoaded(void) const;
   int32_t LoadFromFile(const stw::scl::C_SclString & orc_File, const bool oq_Optional,
                        int32_t * const ops32_DeviceCount);

   C_OscDeviceManager(void);

private:
   bool mq_WasLoaded;
   std::vector<C_OscDeviceGroup> mc_DeviceGroups; ///< Array of all known device groups
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
