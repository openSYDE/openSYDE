//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to group a unique node ID and CAN interace
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANINTERFACEID_H
#define C_OSCCANINTERFACEID_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCCanInterfaceId
{
public:
   C_OSCCanInterfaceId();
   C_OSCCanInterfaceId(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_InterfaceNumber);

   bool operator < (const C_OSCCanInterfaceId & orc_Cmp) const;
   bool operator == (const C_OSCCanInterfaceId & orc_Cmp) const;

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   stw_types::uint32 u32_NodeIndex;
   stw_types::uint8 u8_InterfaceNumber;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
