//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for one raw entry in a parameter set file (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETRAWENTRY_H
#define C_OSCPARAMSETRAWENTRY_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
///Describes one raw (black box) entry in a parameter set file
class C_OSCParamSetRawEntry
{
public:
   C_OSCParamSetRawEntry(void);

   stw_types::uint32 u32_StartAddress;    ///< Start address to write to
   std::vector<stw_types::uint8> c_Bytes; ///< Bytes to write at address
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
