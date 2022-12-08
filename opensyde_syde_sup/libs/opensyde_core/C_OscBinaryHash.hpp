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

#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscBinaryHash
{
public:
   static stw::scl::C_SclString h_CreateBinaryHash(void);
   static void h_SetHashBool(const bool oq_State);
   static bool h_GetHashBool(void);

private:
   static stw::scl::C_SclString mhc_BinaryHash;
   static bool mhq_HashCompleted;
   C_OscBinaryHash();
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
