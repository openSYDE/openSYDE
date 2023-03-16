//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to store pointers to any device info storage (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPDEVICEINFO_HPP
#define C_SYVUPDEVICEINFO_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscSuSequences.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpDeviceInfo
{
public:
   C_SyvUpDeviceInfo(void);
   const stw::opensyde_core::C_OscSuSequences::C_XflDeviceInformation * pc_StwDevice;
   const stw::opensyde_core::C_OscSuSequences::C_OsyDeviceInformation * pc_OpenSydeDevice;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
