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
#include "C_SclString.hpp"
#include "C_OscNodeDataPoolContent.hpp"
#include "C_OscHalcDefChannelAvailability.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcDefChannelUseCase
{
public:
   C_OscHalcDefChannelUseCase(void);
   virtual ~C_OscHalcDefChannelUseCase();

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   stw::scl::C_SclString c_Id;                                  ///< Unique ID for later reference of this use-case
   stw::scl::C_SclString c_Display;                             ///< Name to display to user
   stw::scl::C_SclString c_Comment;                             ///< Description to display to user
   C_OscNodeDataPoolContent c_Value;                            ///< Internal value to use
   std::vector<C_OscHalcDefChannelAvailability> c_Availability; ///< Channel availability settings
   std::vector<uint32_t> c_DefaultChannels;                     ///< Indices of channels which use this as default
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
