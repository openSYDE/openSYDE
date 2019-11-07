//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       IO description domain channel use-case
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOFILEDATACHANNELUSECASE_H
#define C_OSCIOFILEDATACHANNELUSECASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLString.h"
#include "C_OSCNodeDataPoolContent.h"
#include "C_OSCIOFileDataChannelAvailability.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOFileDataChannelUseCase
{
public:
   C_OSCIOFileDataChannelUseCase(void);

   stw_scl::C_SCLString c_Id;                                      ///< Unique ID for later reference of this use-case
   stw_scl::C_SCLString c_Display;                                 ///< Name to display to user
   stw_scl::C_SCLString c_Comment;                                 ///< Description to display to user
   C_OSCNodeDataPoolContent c_Value;                               ///< Internal value to use
   std::vector<C_OSCIOFileDataChannelAvailability> c_Availability; ///< Channel availability settings
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
