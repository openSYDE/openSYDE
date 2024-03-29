//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class, containing information about the data pools in a parameter set file (implementation)

   Data class, containing information about the data pools in a parameter set file

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscParamSetDataPoolInfo.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscParamSetDataPoolInfo::C_OscParamSetDataPoolInfo(void) :
   u32_DataPoolCrc(0),
   u32_NvmStartAddress(0),
   u32_NvmSize(0)
{
   au8_Version[0] = 0;
   au8_Version[1] = 0;
   au8_Version[2] = 0;
}
