//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Manager for all device descriptions (implementation)

   Manager for all device descriptions

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCDeviceManager.h"
#include "CSCLIniFile.h"
#include "TGLUtils.h"
#include "TGLFile.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_scl;
using namespace stw_tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Search for device with specified name

   The returned pointer points to one of the device instances owned by this class.
   So the caller has to consider the lifetime of the used instance of this class when using
   the returned pointer.

   \param[in]     orc_Name     Searched name

   \return
   != NULL:  pointer to device definition
   NULL:     device definition not found
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCDeviceDefinition * C_OSCDeviceManager::LookForDevice(const C_SCLString & orc_Name) const
{
   const C_OSCDeviceDefinition * pc_Device = NULL;

   for (uint32 u32_ItDevice = 0U; u32_ItDevice < this->mc_DeviceGroups.size(); ++u32_ItDevice)
   {
      pc_Device = this->mc_DeviceGroups[u32_ItDevice].LookForDevice(orc_Name);
      if (pc_Device != NULL)
      {
         break;
      }
   }
   return pc_Device;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add device to device group

   Add a new device with the user device definition file to
   a existing device group or to a new device group when no
   device group is available and save it to an .ini file.

   \param[in]        orc_DeviceDefinitionFile    Relative path of device definition file
   \param[in]        orc_DeviceGroup             Name of device group where device will be added
   \param[in,out]    orc_IniFile                 Path of .ini file where devices will be saved
                                                 (../devices/user_devices.ini)

   \return
   C_NO_ERR   device added without problems
   C_RD_WR    could not add device
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDeviceManager::AddDevice(const stw_scl::C_SCLString & orc_DeviceDefinitionFile,
                                     const stw_scl::C_SCLString & orc_DeviceGroup,
                                     const stw_scl::C_SCLString & orc_IniFile)
{
   sint32 s32_Return = C_NO_ERR;

   sintn sn_NumDevices;
   uint32 u32_DeviceGroupCounter;
   bool q_NewGroupNecessary = true;

   // Ini with toolbox structure definition
   C_SCLIniFile c_Ini(orc_IniFile);

   if (TGL_FileExists(orc_IniFile) == false)
   {
      c_Ini.WriteInteger("DeviceTypes", "NumTypes", 1);
      c_Ini.WriteString("DeviceTypes", "TypeName1", orc_DeviceGroup);
      c_Ini.WriteInteger(orc_DeviceGroup, "DeviceCount", 0);
      c_Ini.UpdateFile();
   }

   // Check number of devices in group
   sn_NumDevices = c_Ini.ReadInteger(orc_DeviceGroup, "DeviceCount", 0);

   for (sintn sn_Counter = 0; sn_Counter < sn_NumDevices; sn_Counter++)
   {
      C_SCLString c_DevicePath = c_Ini.ReadString(orc_DeviceGroup, "Device" + C_SCLString(sn_Counter + 1), "");
      // TODO: Compare existing device definition names with new one (LookForDevice method)
      if (c_DevicePath == orc_DeviceDefinitionFile)
      {
         s32_Return = C_BUSY;
         break;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      // Write device count in the list in order
      c_Ini.WriteInteger(orc_DeviceGroup, "DeviceCount", sn_NumDevices + 1);

      // Write device in the list in order
      c_Ini.WriteString(orc_DeviceGroup, "Device" + C_SCLString(sn_NumDevices + 1), orc_DeviceDefinitionFile);

      for (u32_DeviceGroupCounter = 0U; u32_DeviceGroupCounter < this->mc_DeviceGroups.size();
           ++u32_DeviceGroupCounter)
      {
         if (this->mc_DeviceGroups[u32_DeviceGroupCounter].GetGroupName() == orc_DeviceGroup)
         {
            s32_Return =
               this->mc_DeviceGroups[u32_DeviceGroupCounter].LoadGroup(c_Ini, TGL_ExtractFilePath(orc_IniFile));
            q_NewGroupNecessary = false;
            break;
         }
      }

      if (q_NewGroupNecessary == true)
      {
         // Set group name
         C_OSCDeviceGroup c_Group;
         c_Group.SetGroupName(c_Ini.ReadString("DeviceTypes", "TypeName1", "").c_str());
         s32_Return = c_Group.LoadGroup(c_Ini, TGL_ExtractFilePath(orc_IniFile));
         this->mc_DeviceGroups.push_back(c_Group);
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete device from device group

   Delete a device from .ini file and set
   the rest of device definitions new.

   \param[in]        orc_Devices                 List of device definitions
   \param[in]        orc_DeviceGroup             Name of device group where device will be added
   \param[in,out]    orc_IniFile                 Path of .ini file where devices will be saved
                                                 (../devices/user_devices.ini)

   \return
   C_NO_ERR   device deleted without problems
   C_WARN     no error, last device deleted
   C_CONFIG   device group not found
   C_RD_WR    could not delete device
              could not load information
   C_RANGE    no devices in group to delete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDeviceManager::ChangeDevices(std::vector<C_OSCDeviceDefinition> & orc_Devices,
                                         const stw_scl::C_SCLString & orc_DeviceGroup,
                                         const stw_scl::C_SCLString & orc_IniFile)
{
   sint32 s32_Return = C_CONFIG;

   uint32 u32_DeviceGroupCounter;

   for (u32_DeviceGroupCounter = 0U; u32_DeviceGroupCounter < this->mc_DeviceGroups.size();
        ++u32_DeviceGroupCounter)
   {
      if (this->mc_DeviceGroups[u32_DeviceGroupCounter].GetGroupName() == orc_DeviceGroup)
      {
         s32_Return = C_NO_ERR;
         break;
      }
   }

   // Ini with toolbox structure definition
   C_SCLIniFile c_Ini(orc_IniFile);

   if (TGL_FileExists(orc_IniFile) == false)
   {
      osc_write_log_error("Delete device definitions", "File \"" + orc_IniFile + "\" does not exist.");
      s32_Return = C_RD_WR;
   }

   // Check number of devices in group bevor deleting a device
   sintn sn_NumDevicesBevorChanges = c_Ini.ReadInteger(orc_DeviceGroup, "DeviceCount", 0);

   if (s32_Return == C_NO_ERR)
   {
      if (sn_NumDevicesBevorChanges > 0)
      {
         c_Ini.EraseSection(orc_DeviceGroup);

         // Write device count in the list in order
         c_Ini.WriteInteger(orc_DeviceGroup, "DeviceCount", orc_Devices.size());

         for (uint32 u32_ItDevice = 0; u32_ItDevice < orc_Devices.size(); ++u32_ItDevice)
         {
            // Write device in the list in order
            c_Ini.WriteString(orc_DeviceGroup, "Device" + C_SCLString(u32_ItDevice + 1),
                              orc_Devices[u32_ItDevice].c_FilePath.c_str());
         }

         s32_Return = this->mc_DeviceGroups[u32_DeviceGroupCounter].LoadGroup(c_Ini, TGL_ExtractFilePath(orc_IniFile));

         // Check number of devices in group after deleting a device
         if (c_Ini.ReadInteger(orc_DeviceGroup, "DeviceCount", 0) == 0)
         {
            s32_Return = C_WARN;
         }
      }
      else
      {
         // No devices in group to delete
         s32_Return = C_RANGE;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all device groups

   \return
   copy of all device groups owned by this class
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OSCDeviceGroup> C_OSCDeviceManager::GetDeviceGroups(void) const
{
   return this->mc_DeviceGroups;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get "WasLoaded" flag

   Flag will be set after loading device definitions.
   Can be used to prevent multiple loading.

   \return
   status of flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCDeviceManager::WasLoaded(void) const
{
   return mq_WasLoaded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load all known devices

   \param[in]   orc_File   Ini file path

   \return
   C_NO_ERR   all information loaded without problems
   C_RD_WR    could not load information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDeviceManager::LoadFromFile(const C_SCLString & orc_File)
{
   sint32 s32_Return = C_NO_ERR;

   if (TGL_FileExists(orc_File) == false)
   {
      osc_write_log_error("Loading device definitions", "File \"" + orc_File + "\" does not exist.");
      s32_Return = C_RD_WR;
   }

   //Ini with toolbox structure definition
   C_SCLIniFile c_Ini(orc_File);
   sintn sn_NumTypes = c_Ini.ReadInteger("DeviceTypes", "NumTypes", 0);

   //Parse groups
   for (sintn sn_ItType = 0; sn_ItType < sn_NumTypes; ++sn_ItType)
   {
      //Get group name
      C_OSCDeviceGroup c_Group;
      const C_SCLString c_GroupName = c_Ini.ReadString("DeviceTypes", "TypeName" + C_SCLString(sn_ItType + 1), "");
      c_Group.SetGroupName(c_GroupName.c_str());
      s32_Return = c_Group.LoadGroup(c_Ini, TGL_ExtractFilePath(orc_File));

      this->mc_DeviceGroups.push_back(c_Group);

      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_RD_WR;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      mq_WasLoaded = true;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCDeviceManager::C_OSCDeviceManager(void) :
   mq_WasLoaded(false)
{
}
