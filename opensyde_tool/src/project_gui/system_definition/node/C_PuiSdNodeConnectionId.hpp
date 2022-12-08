//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDNODECONNECTIONID_H
#define C_PUISDNODECONNECTIONID_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QString>
#include "stwtypes.hpp"
#include "C_OscSystemBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeConnectionId
{
public:
   C_PuiSdNodeConnectionId();
   bool operator ==(const C_PuiSdNodeConnectionId & orc_Ref) const;

   void CalcHash(uint32_t & oru32_HashValue) const;

   stw::opensyde_core::C_OscSystemBus::E_Type e_InterfaceType;
   uint8_t u8_InterfaceNumber;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
