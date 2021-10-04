//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE device definition container class

   Serves as data class to hold device definition data.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OSCDeviceDefinition.h"
#include "CSCLString.h"
#include "TGLUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_scl::C_SCLString C_OSCDeviceDefinition::hc_DefaultCompanyName = "Sensor-Technik Wiedemann GmbH";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor

   Initialize all class elements with default values
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCDeviceDefinition::C_OSCDeviceDefinition(void)
{
   this->Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear elements

   Sets all numeric elements to zero and all strings to ""
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCDeviceDefinition::Clear(void)
{
   c_DeviceName = "";
   c_DeviceNameAlias = "";
   c_DeviceDescription = "";
   c_ImagePath = "";
   c_FilePath = "";

   u8_NumCanBusses = 0U;
   u8_NumEthernetBusses = 0U;

   c_SupportedBitrates.clear();

   u8_ManufacturerId = 0;
   c_ManufacturerDisplayValue = C_OSCDeviceDefinition::hc_DefaultCompanyName;

   c_SubDevices.clear();
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get string to display for this device

   \return
   String to display
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCDeviceDefinition::GetDisplayName(void) const
{
   stw_scl::C_SCLString c_Retval;
   if (this->c_DeviceNameAlias == "")
   {
      c_Retval = this->c_DeviceName;
   }
   else
   {
      c_Retval = this->c_DeviceNameAlias;
   }
   return c_Retval;
}
//----------------------------------------------------------------------------------------------------------------------
