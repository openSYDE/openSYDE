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

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <set>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanSignal
{
public:
   enum E_ByteOrderType ///< Enum for byte order types
   {
      eBYTE_ORDER_INTEL,   ///< Byte order type intel (little endian)
      eBYTE_ORDER_MOTOROLA ///< Byte order type motorola (big endian)
   };

   enum E_MultiplexerType ///< Enum for multiplexer signal types
   {
      eMUX_DEFAULT,            ///< Non multiplexed signal
      eMUX_MULTIPLEXER_SIGNAL, ///< Multiplexer signal which defines the multiplex group by its value. Maximum one
      ///< multiplexer per message!
      eMUX_MULTIPLEXED_SIGNAL ///< Signal presence depends on the multiplex value
   };

   C_OscCanSignal(void);

   bool operator !=(const C_OscCanSignal & orc_Cmp) const;
   bool operator ==(const C_OscCanSignal & orc_Cmp) const;
   bool operator <(const C_OscCanSignal & orc_Cmp) const;

   void CalcHash(uint32_t & oru32_HashValue, const bool oq_R20Compatible = false) const;
   // Helper functions for byte order
   uint16_t GetDataBytesBitPosOfSignalBit(const uint16_t ou16_SignalBitPosition) const;
   uint16_t GetDataBytesBitPosOfSignalBit(const uint16_t ou16_StartBit, const uint16_t ou16_SignalBitPosition) const;
   void GetDataBytesBitPositionsOfSignal(std::set<uint16_t> & orc_SetPositions) const;
   bool IsBitPosPartOfSignal(const uint16_t ou16_MessageBitPosition) const;

   E_ByteOrderType e_ComByteOrder; ///< Communication value byte order
   uint16_t u16_ComBitLength;      ///< Communication value bit length
   ///< Reserved for other protocols
   uint16_t u16_ComBitStart; ///< Communication value bit start position
   ///< Reserved for other protocols
   uint32_t u32_ComDataElementIndex; ///< Index of data element this signal refers to.
   ///< List index is implicit through convention.
   E_MultiplexerType e_MultiplexerType; ///< Signal multiplexer type
   uint16_t u16_MultiplexValue;         ///< Only used if eMULTIPLEXED_SIGNAL
   ///< Multiplexer value if this signal should be present and represents the associated multiplex group
   uint16_t u16_CanOpenManagerObjectDictionaryIndex; ///< CANopen manager specific information about object
   ///< dictionary index
   uint8_t u8_CanOpenManagerObjectDictionarySubIndex; ///< CANopen manager specific information about object
   ///< dictionary sub index
   uint32_t u32_J1939SuspectParameterNumber; ///< J1939 specific information about the SPN number
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
