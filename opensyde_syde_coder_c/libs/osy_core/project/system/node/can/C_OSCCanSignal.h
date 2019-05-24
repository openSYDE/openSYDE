//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for a signal positioning in a CAN message (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANSIGNAL_H
#define C_OSCCANSIGNAL_H

/* -- Includes ------------------------------------------------------------- */
#include <set>

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCCanSignal
{
public:
   enum E_ByteOrderType ///< Enum for byte order types
   {
      eBYTE_ORDER_INTEL,   ///< Byte order type intel (little endian)
      eBYTE_ORDER_MOTOROLA ///< Byte order type motorola (big endian)
   };

   C_OSCCanSignal(void);

   bool operator !=(const C_OSCCanSignal & orc_Cmp) const;

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   // Helper functions for byte order
   stw_types::uint16 GetDataBytesBitPosOfSignalBit(const stw_types::uint16 ou16_SignalBitPosition) const;
   stw_types::uint16 GetDataBytesBitPosOfSignalBit(const stw_types::uint16 ou16_StartBit,
                                                   const stw_types::uint16 ou16_SignalBitPosition) const;
   void GetDataBytesBitPositionsOfSignal(std::set<stw_types::uint16> & orc_SetPositions) const;

   E_ByteOrderType e_ComByteOrder;     ///< Communication value byte order
   stw_types::uint16 u16_ComBitLength; ///< Communication value bit length
   ///< Reserved for other protocols
   stw_types::uint16 u16_ComBitStart; ///< Communication value bit start position
   ///< Reserved for other protocols
   stw_types::uint32 u32_ComDataElementIndex; ///< Index of data element this signal refers to.
   ///< List index is implicit through convention.
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
