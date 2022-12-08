//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Manager for all device descriptions (implementation)

   Manager for all device descriptions

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscDeviceManager.hpp"
#include "C_SclIniFile.hpp"
#include "TglUtils.hpp"
#include "TglFile.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscDeviceDefinitionFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::scl;
using namespace stw::tgl;

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

   \param[in]   orc_Name               Searched name
   \param[in]   orc_MainDeviceName     Main device name (empty if none exists)
   \param[out]  oru32_SubDeviceIndex   Sub device index

   \return
   != NULL:  pointer to device definition
   NULL:     device definition not found
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscDeviceDefinition * C_OscDeviceManager::LookForDevice(const C_SclString & orc_Name,
                                                                const C_SclString & orc_MainDeviceName,
                                                                uint32_t & oru32_SubDeviceIndex) const
{
   const C_OscDeviceDefinition * pc_Device = NULL;

   for (uint32_t u32_ItDevice = 0U; u32_ItDevice < this->mc_DeviceGroups.size(); ++u32_ItDevice)
   {
      pc_Device = this->mc_DeviceGroups[u32_ItDevice].LookForDevice(orc_Name, orc_MainDeviceName, oru32_SubDeviceIndex);
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

   \param[in]      orc_DeviceDefinitionFile  Relative path of device definition file
   \param[in]      orc_DeviceGroup           Name of device group where device will be added
   \param[in,out]  orc_IniFile               Path of .ini file where user devices will be saved

   \return
   C_NO_ERR     Device added without problems
   C_RD_WR      Could not add device
   C_OVERFLOW   Device already exists, won't be added
   C_NOACT      Specified file is invalid (invalid XML file)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDeviceManager::AddDevice(const stw::scl::C_SclString & orc_DeviceDefinitionFile,
                                      const stw::scl::C_SclString & orc_DeviceGroup,
                                      const stw::scl::C_SclString & orc_IniFile)
{
   int32_t s32_Return;

   C_OscDeviceDefinition c_DeviceDefinition;

   // Ini with toolbox structure definition
   C_SclIniFile c_Ini(orc_IniFile);

   if (TglFileExists(orc_IniFile) == false)
   {
      c_Ini.WriteInteger("DeviceTypes", "NumTypes", 1);
      c_Ini.WriteString("DeviceTypes", "TypeName1", orc_DeviceGroup);
      c_Ini.WriteInteger(orc_DeviceGroup, "DeviceCount", 0);
      c_Ini.UpdateFile();
   }

   // Load device definition for name checking
   s32_Return = C_OscDeviceDefinitionFiler::h_Load(c_DeviceDefinition, orc_DeviceDefinitionFile);
   if (s32_Return == C_NO_ERR)
   {
      // Compare new device definition file with existing
      // If the file exists, the device won't add
      // Otherwise it will be add to the selected device group
      for (uint32_t u32_GroupCounter = 0U; u32_GroupCounter < this->mc_DeviceGroups.size(); u32_GroupCounter++)
      {
         // Check if the device name and the device alias name already exist
         if (this->mc_DeviceGroups[u32_GroupCounter].PreCheckDevice(c_DeviceDefinition.c_DeviceName,
                                                                    c_DeviceDefinition.c_DeviceNameAlias,
                                                                    c_DeviceDefinition.c_FilePath) == true)
         {
            s32_Return = C_OVERFLOW;
            osc_write_log_error("Adding device definition",
                                "Device \"" + c_DeviceDefinition.c_FilePath + "\" already exists.");
            break;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      bool q_NewGroupNecessary = true;

      // Check number of devices in group
      const int32_t s32_NumDevices = c_Ini.ReadInteger(orc_DeviceGroup, "DeviceCount", 0);

      // Write device count in the list in order
      c_Ini.WriteInteger(orc_DeviceGroup, "DeviceCount", s32_NumDevices + 1);

      // Write device in the list in order
      c_Ini.WriteString(orc_DeviceGroup, "Device" + C_SclString::IntToStr(s32_NumDevices + 1),
                        orc_DeviceDefinitionFile);

      for (uint32_t u32_DeviceGroupCounter = 0U; u32_DeviceGroupCounter < this->mc_DeviceGroups.size();
           ++u32_DeviceGroupCounter)
      {
         if (this->mc_DeviceGroups[u32_DeviceGroupCounter].GetGroupName() == orc_DeviceGroup)
         {
            s32_Return =
               this->mc_DeviceGroups[u32_DeviceGroupCounter].LoadGroup(c_Ini, TglExtractFilePath(orc_IniFile));
            q_NewGroupNecessary = false;
            break;
         }
      }

      if (q_NewGroupNecessary == true)
      {
         // Set group name
         C_OscDeviceGroup c_Group;
         c_Group.SetGroupName(c_Ini.ReadString("DeviceTypes", "TypeName1", "").c_str());
         s32_Return = c_Group.LoadGroup(c_Ini, TglExtractFilePath(orc_IniFile));
         this->mc_DeviceGroups.push_back(c_Group);
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete device from device group

   Delete a device from .ini file and set
   the rest of device definitions new.

   \param[in]      orc_Devices      List of device definitions
   \param[in]      orc_DeviceGroup  Name of device group where device will be added
   \param[in,out]  orc_IniFile      Path of .ini file where devices will be saved

   \return
   C_NO_ERR   Device deleted without problems
   C_WARN     No error, last device deleted
   C_CONFIG   Device group not found
   C_RD_WR    Could not delete device
              Could not load information
   C_RANGE    No devices in group to delete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDeviceManager::ChangeDevices(std::vector<C_OscDeviceDefinition> & orc_Devices,
                                          const stw::scl::C_SclString & orc_DeviceGroup,
                                          const stw::scl::C_SclString & orc_IniFile)
{
   int32_t s32_Return = C_CONFIG;

   uint32_t u32_DeviceGroupCounter;

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
   C_SclIniFile c_Ini(orc_IniFile);

   if (TglFileExists(orc_IniFile) == false)
   {
      osc_write_log_error("Delete device definitions", "File \"" + orc_IniFile + "\" does not exist.");
      s32_Return = C_RD_WR;
   }

   if (s32_Return == C_NO_ERR)
   {
      // Check number of devices in group bevor deleting a device
      const int32_t s32_NumDevicesBeforeChanges = c_Ini.ReadInteger(orc_DeviceGroup, "DeviceCount", 0);

      if (s32_NumDevicesBeforeChanges > 0)
      {
         c_Ini.EraseSection(orc_DeviceGroup);

         // Write device count in the list in order
         c_Ini.WriteInteger(orc_DeviceGroup, "DeviceCount", orc_Devices.size());

         for (uint32_t u32_ItDevice = 0; u32_ItDevice < orc_Devices.size(); ++u32_ItDevice)
         {
            // Write device in the list in order
            c_Ini.WriteString(orc_DeviceGroup, "Device" + C_SclString::IntToStr(u32_ItDevice + 1),
                              orc_Devices[u32_ItDevice].c_FilePath.c_str());
         }

         s32_Return =
            this->mc_DeviceGroups[u32_DeviceGroupCounter].LoadGroup(c_Ini, TglExtractFilePath(orc_IniFile));

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
std::vector<C_OscDeviceGroup> C_OscDeviceManager::GetDeviceGroups(void) const
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
bool C_OscDeviceManager::WasLoaded(void) const
{
   return mq_WasLoaded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load all known devices

   \param[in]     orc_File           Ini file path
   \param[in]     oq_Optional        If user_devices.ini: Type of log entry when file is missing is set to "INFO".
                                      Otherwise: "ERROR".
   \param[in,out] ops32_DeviceCount  Optional parameter: can be used to keep track of how many devices are listed in
                                      an ini file

   \return
   C_NO_ERR   all information loaded without problems
   C_RD_WR    could not load information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDeviceManager::LoadFromFile(const C_SclString & orc_File, const bool oq_Optional,
                                         int32_t * const ops32_DeviceCount)
{
   int32_t s32_Return = C_NO_ERR;

   if (TglFileExists(orc_File) == false)
   {
      if (oq_Optional == true)
      {
         osc_write_log_info("Loading user device definitions", "File \"" + orc_File + "\" does not exist. (optional)");
      }
      else
      {
         osc_write_log_error("Loading device definitions", "File \"" + orc_File + "\" does not exist.");
      }

      s32_Return = C_RD_WR;
   }

   //Ini with toolbox structure definition
   C_SclIniFile c_Ini(orc_File);
   const int32_t s32_NumTypes = c_Ini.ReadInteger("DeviceTypes", "NumTypes", 0);

   //Parse groups
   for (int32_t s32_ItType = 0; s32_ItType < s32_NumTypes; ++s32_ItType)
   {
      //Get group name
      C_OscDeviceGroup c_Group;
      const C_SclString c_GroupName = c_Ini.ReadString("DeviceTypes", "TypeName" + C_SclString::IntToStr(
                                                          s32_ItType + 1), "");

      // special case user_devices.ini. We accept only one format. If an ini-file contains [User Nodes], the
      // number of types [NumTypes] has to be 1
      if (c_GroupName == "User Nodes")
      {
         if (s32_NumTypes > 1)
         {
            osc_write_log_error("Loading from ini file", "File \"" + orc_File + "\" should only contain User Nodes.");
            break;
         }
         // optional parameter (see above): sends number of files listed in ini to GUI layer for user feedback.
         if (ops32_DeviceCount != NULL)
         {
            *ops32_DeviceCount += c_Ini.ReadInteger("User Nodes", "DeviceCount", 0);
         }
      }

      c_Group.SetGroupName(c_GroupName.c_str());
      s32_Return = c_Group.LoadGroup(c_Ini, TglExtractFilePath(orc_File));
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
C_OscDeviceManager::C_OscDeviceManager(void) :
   mq_WasLoaded(false)
{
}
