//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: X-Config Package base

   openSYDE: X-Config Package base

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_OscXcoBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const C_SclString C_OscXcoBase::hc_PACKAGE_EXT = ".syde_xcfg";
const C_SclString C_OscXcoBase::hc_PACKAGE_EXT_TMP = ".syde_xcfg_tmp"; // intermediate directory
                                                                       // before creating zip
                                                                       // archive
const C_SclString C_OscXcoBase::hc_XCFG_SYSDEF = "xcfg_system_definition.syde_sysdef";
const C_SclString C_OscXcoBase::hc_XCFG_SYSDEF_FOLDER = "system_definition";
const C_SclString C_OscXcoBase::hc_INI_DEV = "devices.ini";
const C_SclString C_OscXcoBase::hc_INI_DEV_FOLDER = "device_definitions";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
stw::scl::C_SclStringList C_OscXcoBase::mhc_WarningMessages; // global warnings e.g. if update
                                                             // position
                                                             // of
                                                             // active node is not available
stw::scl::C_SclString C_OscXcoBase::mhc_ErrorMessage;        // description of error which caused
                                                             // the
                                                             // service update package to fail

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns fix defined service update package extension.

   \return
   service update package extension
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscXcoBase::h_GetPackageExtension()
{
   return hc_PACKAGE_EXT;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXcoBase::mh_Init()
{
   mhc_WarningMessages.Clear(); // clear old warning messages
   mhc_ErrorMessage = "";       // clear old error message
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get warnings and errors

   \param[in,out]  orc_WarningMessages    Warning messages
   \param[in,out]  orc_ErrorMessage       Error message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXcoBase::mh_GetWarningsAndErrors(C_SclStringList & orc_WarningMessages, C_SclString & orc_ErrorMessage)
{
   orc_WarningMessages = mhc_WarningMessages; // set warning messages for caller
   orc_ErrorMessage = mhc_ErrorMessage;       // set error message for caller
}
