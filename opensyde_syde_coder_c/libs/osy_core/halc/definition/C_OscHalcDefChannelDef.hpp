//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Store HALC channel information
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFCHANNEL_H
#define C_OSCHALCDEFCHANNEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcDefChannelDef
{
public:
   C_OscHalcDefChannelDef(void);

   stw::scl::C_SclString c_Name; ///< Name of this channel

   void CalcHash(uint32_t & oru32_HashValue) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
