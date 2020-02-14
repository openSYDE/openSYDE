//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition domain channel use-case
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFCHANNELUSECASE_H
#define C_OSCHALCDEFCHANNELUSECASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLString.h"
#include "C_OSCNodeDataPoolContent.h"
#include "C_OSCHalcDefChannelAvailability.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcDefChannelUseCase
{
public:
   C_OSCHalcDefChannelUseCase(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   stw_scl::C_SCLString c_Id;                                   ///< Unique ID for later reference of this use-case
   stw_scl::C_SCLString c_Display;                              ///< Name to display to user
   stw_scl::C_SCLString c_Comment;                              ///< Description to display to user
   C_OSCNodeDataPoolContent c_Value;                            ///< Internal value to use
   std::vector<C_OSCHalcDefChannelAvailability> c_Availability; ///< Channel availability settings
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
