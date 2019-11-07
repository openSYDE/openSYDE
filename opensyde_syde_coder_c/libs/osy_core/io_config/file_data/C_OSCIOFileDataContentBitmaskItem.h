//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bitmask item
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOFILEDATACONTENTBITMASKITEM_H
#define C_OSCIOFILEDATACONTENTBITMASKITEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOFileDataContentBitmaskItem
{
public:
   C_OSCIOFileDataContentBitmaskItem(void);

   stw_types::sint32 SetValueByString(const stw_scl::C_SCLString & orc_Item);

   stw_scl::C_SCLString c_Display; ///< Name to display to user
   stw_scl::C_SCLString c_Comment; ///< Description to display to user
   bool q_ApplyValueSetting;       ///< Current setting to apply this bitmask value
   stw_types::uint64 u64_Value;    ///< Bitmask value

private:
   static stw_types::sint32 mh_ParseUintFromString(const stw_scl::C_SCLString & orc_Item,
                                                   stw_types::uint64 & oru64_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
