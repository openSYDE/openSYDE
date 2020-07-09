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
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcDefChannelDef
{
public:
   C_OSCHalcDefChannelDef(void);

   stw_scl::C_SCLString c_Name; ///< Name of this channel

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
