//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage class to group element IDs with their CRC
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALCOPELEMENTIDCRCGROUP_H
#define C_SDNDEDALCOPELEMENTIDCRCGROUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdNdeDalCopElementIdCrcGroupBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalCopElementIdCrcGroup :
   public C_SdNdeDalCopElementIdCrcGroupBase
{
public:
   C_SdNdeDalCopElementIdCrcGroup();
   ~C_SdNdeDalCopElementIdCrcGroup() override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
