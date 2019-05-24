//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Adaption of data class for logging CAN messages for message trace tree view (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_CamMetTreeLoggerData.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_logic;

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
C_CamMetTreeLoggerData::C_CamMetTreeLoggerData(void) :
   C_OSCComMessageLoggerData(),
   sn_GrayOutValueMsg(0),
   sn_GrayOutValueData(0),
   u32_DataChangedTimeStamp(0U)
{
   // Handling of maximum 8 data bytes
   c_GrayOutValueDataBytes.resize(8);
   c_DataBytesChangedTimeStamps.resize(8);
}
