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

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///information about one data pool in a parameter set file
class C_OscParamSetDataPoolInfo
{
public:
   C_OscParamSetDataPoolInfo(void);

   stw::scl::C_SclString c_Name; ///< Data pool name
   uint32_t u32_DataPoolCrc;     ///< Data pool CRC
   uint32_t u32_NvmStartAddress; ///< NvM start address of data pool
   uint32_t u32_NvmSize;         ///< NvM size of data pool
   uint8_t au8_Version[3];       ///< User data pool version Major, minor, release
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
