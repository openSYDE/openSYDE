//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group of device descriptions (header)

   Group of device descriptions (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDEVICEGROUP_HPP
#define C_OSCDEVICEGROUP_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OscDeviceDefinition.hpp"
#include "C_SclIniFile.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscDeviceGroup
{
public:
   C_OscDeviceGroup(void);

   const C_OscDeviceDefinition * LookForDevice(const stw::scl::C_SclString & orc_Name,
                                               const stw::scl::C_SclString & orc_MainDeviceName,
                                               uint32_t & oru32_SubDeviceIndex) const;
   bool PreCheckDevice(const stw::scl::C_SclString & orc_DeviceName, const stw::scl::C_SclString & orc_DeviceNameAlias,
                       const stw::scl::C_SclString & orc_DevicePath) const;
   int32_t LoadGroup(stw::scl::C_SclIniFile & orc_Ini, const stw::scl::C_SclString & orc_BasePath);

   void SetGroupName(const stw::scl::C_SclString & orc_GroupName);
   stw::scl::C_SclString GetGroupName(void) const;

   std::vector<C_OscDeviceDefinition> GetDevices(void) const;

private:
   stw::scl::C_SclString mc_GroupName;            ///< Group name
   std::vector<C_OscDeviceDefinition> mc_Devices; ///< All devices belonging to this group
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
