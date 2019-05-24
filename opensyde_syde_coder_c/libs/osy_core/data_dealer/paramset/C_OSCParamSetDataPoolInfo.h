//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class, containing information about the data pools in a parameter set file (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETDATAPOOLINFO_H
#define C_OSCPARAMSETDATAPOOLINFO_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "CSCLString.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

///information about one data pool in a parameter set file
class C_OSCParamSetDataPoolInfo
{
public:
   C_OSCParamSetDataPoolInfo(void);

   stw_scl::C_SCLString c_Name;       ///< Data pool name
   stw_types::uint32 u32_DataPoolCrc; ///< Data pool CRC
   stw_types::uint8 au8_Version[3];   ///< User data pool version Major, minor, release
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
