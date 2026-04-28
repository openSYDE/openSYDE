//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE .conf file handler
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
 */
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCFGFILEHANDLER_HPP
#define C_OSCCFGFILEHANDLER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "C_SclString.hpp"
#include "C_SclStringList.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Generic config class handler
class C_OscConfFileHandler
{
public:
   virtual ~C_OscConfFileHandler();

   int32_t LoadSettings(const stw::scl::C_SclString & orc_Path);

protected:
   // Function for loading concrete settings. Must return C_CONFIG in error case
   virtual int32_t m_LoadSettings(const stw::scl::C_SclStringList & orc_SettingsWithoutComments) = 0;
   static int32_t mh_ReplaceSettings(const stw::scl::C_SclString & orc_Path,
                                     const std::vector<std::pair<stw::scl::C_SclString,
                                                                 stw::scl::C_SclString> > & orc_Configs);

   stw::scl::C_SclString mc_ConfigFilePath; //path where the config file was loaded from
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
