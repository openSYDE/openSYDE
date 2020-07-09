//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       This class uses a platform-specific path of the executable, which is running in the current process and
                creates a MD5-Checksum from this file. It also sets an boolean global state, if the Checksum has already
                been calculated.
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCBINARYHASH_H
#define C_OSCBINARYHASH_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCBinaryHash
{
public:
   static stw_scl::C_SCLString h_CreateBinaryHash(void);
   static void h_SetHashBool(bool const q_State);
   static bool h_GetHashBool(void);

private:
   static stw_scl::C_SCLString hc_BinaryHash;
   static bool hq_HashCompleted;
   C_OSCBinaryHash();
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
