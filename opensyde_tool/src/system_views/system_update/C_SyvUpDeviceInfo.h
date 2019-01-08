//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to store pointers to any device info storage (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.02.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVUPDEVICEINFO_H
#define C_SYVUPDEVICEINFO_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OSCSuSequences.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvUpDeviceInfo
{
public:
   C_SyvUpDeviceInfo(void);
   const stw_opensyde_core::C_OSCSuSequences::C_XflDeviceInformation * pc_STWDevice;
   const stw_opensyde_core::C_OSCSuSequences::C_OsyDeviceInformation * pc_OSYDevice;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
