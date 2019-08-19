//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Adaption of data class for logging CAN messages for message trace tree view (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETTREELOGGERDATA_H
#define C_CAMMETTREELOGGERDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCComMessageLoggerData.h"
#include "C_CamMetTreeLoggerDataGreyOutInformation.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMetTreeLoggerData :
   public stw_opensyde_core::C_OSCComMessageLoggerData
{
public:
   C_CamMetTreeLoggerData(void);

   // For usage of concrete data model or widget. Will not be set by C_SyvComMessageMonitor
   C_CamMetTreeLoggerDataGreyOutInformation c_GreyOutInformation;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
