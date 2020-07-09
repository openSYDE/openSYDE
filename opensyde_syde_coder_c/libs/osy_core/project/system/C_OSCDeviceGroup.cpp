//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group of device descriptions (implementation)

   Group of device descriptions

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCDeviceGroup.h"
#include "C_OSCDeviceDefinitionFiler.h"
#include "TGLFile.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_core;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;

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
C_OSCDeviceGroup::C_OSCDeviceGroup(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Search for device with specified name

   The returned pointer points to one of the device instances owned by this class.
   So the caller has to consider the lifetime of the used instance of this class when using
    the returned pointer.

   \param[in]     orc_Name     Searched device name

   \return
   != NULL:   pointer to found device
   NULL:      define not found
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCDeviceDefinition * C_OSCDeviceGroup::LookForDevice(const C_SCLString & orc_Name) const
{
   const C_OSCDeviceDefinition * pc_Device = NULL;

   for (uint32 u32_ItDevice = 0U; u32_ItDevice < this->mc_Devices.size(); ++u32_ItDevice)
   {
      if (this->mc_Devices[u32_ItDevice].c_DeviceName == orc_Name)
      {
         pc_Device = &(this->mc_Devices[u32_ItDevice]);
         break;
      }
   }
   return pc_Device;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load group listed in ini file

   Requires:
   Valid base path
   Valid group name

   \param[in,out]   orc_Ini          Ini to parse for device
   \param[in]       orc_BasePath     base path the relative device paths are relative to (with final "/" or "\")

   \return
   C_NO_ERR   group loaded
   C_RD_WR    could not load information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDeviceGroup::LoadGroup(C_SCLIniFile & orc_Ini, const C_SCLString & orc_BasePath)
{
   C_SCLString c_DevicePath;
   sintn sn_NumDevices;
   sint32 s32_Return = C_NO_ERR;

   this->mc_Devices.clear();
   //Check number of devices in group
   sn_NumDevices = orc_Ini.ReadInteger(this->mc_GroupName, "DeviceCount", 0);
   if (sn_NumDevices > 0)
   {
      this->mc_Devices.reserve(sn_NumDevices);
      //Read in all devices in the list in order
      for (sintn sn_ItDevice = 0; sn_ItDevice < sn_NumDevices; ++sn_ItDevice)
      {
         c_DevicePath = orc_Ini.ReadString(this->mc_GroupName, "Device" + C_SCLString(sn_ItDevice + 1), "");
         if (c_DevicePath == "")
         {
            osc_write_log_error("Load device descriptions", "Empty name of \"Device" + C_SCLString(
                                   sn_ItDevice + 1) + "\" in device group \"" + this->mc_GroupName + "\".");
            s32_Return = C_RD_WR;
         }
         else
         {
            C_OSCDeviceDefinition c_DeviceDefinition;
            const C_SCLString c_FullDevicePath = C_SCLString(orc_BasePath + c_DevicePath);
            if (C_OSCDeviceDefinitionFiler::h_Load(c_DeviceDefinition, c_FullDevicePath) == C_NO_ERR)
            {
               this->mc_Devices.push_back(c_DeviceDefinition);
            }
            else
            {
               osc_write_log_error("Load device descriptions", "Could not load \"" + c_FullDevicePath + "\"");
               s32_Return = C_RD_WR;
            }
         }
         if (s32_Return != C_NO_ERR)
         {
            break;
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set group name

   \param[in]   orc_GroupName   New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCDeviceGroup::SetGroupName(const C_SCLString & orc_GroupName)
{
   this->mc_GroupName = orc_GroupName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get group name

   \return
   group name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCDeviceGroup::GetGroupName(void) const
{
   return mc_GroupName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all devices

   \return
   copy of device definitions owned by this class
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<stw_opensyde_core::C_OSCDeviceDefinition> C_OSCDeviceGroup::GetDevices(void) const
{
   return mc_Devices;
}
