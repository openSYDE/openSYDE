//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: X-Config Package base
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCXCOBASE_HPP
#define C_OSCXCOBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_SclStringList.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscXcoBase
{
public:
   static stw::scl::C_SclString h_GetPackageExtension();

   static const stw::scl::C_SclString hc_PACKAGE_EXT;
   static const stw::scl::C_SclString hc_PACKAGE_EXT_TMP; // intermediate directory before creating zip archive
   static const stw::scl::C_SclString hc_XCFG_SYSDEF;
   static const stw::scl::C_SclString hc_XCFG_SYSDEF_FOLDER;
   static const stw::scl::C_SclString hc_INI_DEV;
   static const stw::scl::C_SclString hc_INI_DEV_FOLDER;

protected:
   static stw::scl::C_SclStringList mhc_WarningMessages; // global warnings e.g. if update position of active node is
                                                         // not available
   static stw::scl::C_SclString mhc_ErrorMessage;        // description of error which caused the service update package
                                                         // to fail

   static void mh_Init(void);
   static void mh_GetWarningsAndErrors(stw::scl::C_SclStringList & orc_WarningMessages,
                                       stw::scl::C_SclString & orc_ErrorMessage);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
