//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE device definition container class

   Serves as data class to hold device definition data.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscDeviceDefinition.hpp"
#include "C_SclString.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw::scl::C_SclString C_OscDeviceDefinition::hc_DEFAULT_COMPANY_NAME = "Sensor-Technik Wiedemann GmbH";

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
C_OscDeviceDefinition::C_OscDeviceDefinition(void)
{
   this->Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear elements

   Sets all numeric elements to zero and all strings to ""
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDeviceDefinition::Clear(void)
{
   c_DeviceName = "";
   c_DeviceNameAlias = "";
   c_DeviceDescription = "";
   c_ImagePath = "";
   c_FilePath = "";

   u8_NumCanBusses = 0U;
   u8_NumEthernetBusses = 0U;

   c_SupportedBitrates.clear();
   c_SupportedCanFdDataBitrates.clear();
   c_SupportedCanFeatures.clear();

   u8_ManufacturerId = 0;
   c_ManufacturerDisplayValue = C_OscDeviceDefinition::hc_DEFAULT_COMPANY_NAME;
   c_CompanyLogoLink = "";
   c_ProductPageLink = "";
   c_ToolboxIcon = "";

   c_SubDevices.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get string to display for this device

   \return
   String to display
*/
//----------------------------------------------------------------------------------------------------------------------
stw::scl::C_SclString C_OscDeviceDefinition::GetDisplayName(void) const
{
   stw::scl::C_SclString c_Retval;
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
