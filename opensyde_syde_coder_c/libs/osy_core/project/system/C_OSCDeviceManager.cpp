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
/*! \brief   Get all device groups

   \return
   copy of all device groups owned by this class
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OSCDeviceGroup> C_OSCDeviceManager::GetDeviceGroups(void) const
{
   return mc_DeviceGroups;
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

   this->mc_DeviceGroups.resize(sn_NumTypes);
   //Parse groups
   for (sintn sn_ItType = 0; sn_ItType < sn_NumTypes; ++sn_ItType)
   {
      //Get group name
      const C_SCLString c_GroupName = c_Ini.ReadString("DeviceTypes", "TypeName" + C_SCLString(sn_ItType + 1), "");
      this->mc_DeviceGroups[sn_ItType].SetGroupName(c_GroupName.c_str());
      s32_Return = this->mc_DeviceGroups[sn_ItType].LoadGroup(c_Ini, TGL_ExtractFilePath(orc_File));
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
