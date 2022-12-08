//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Grey out information for message

   Grey out information for message

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_CamMetTreeLoggerDataGreyOutInformation.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetTreeLoggerDataGreyOutInformation::C_CamMetTreeLoggerDataGreyOutInformation(void) :
   s32_GrayOutValueMsg(0),
   u32_DataChangedTimeStamp(0U)
{
   // Handling of maximum 8 data bytes
   c_GrayOutValueDataBytes.resize(8);
   c_DataBytesChangedTimeStamps.resize(8);
}
