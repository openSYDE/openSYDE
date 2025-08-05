//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for CAN specific operations (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANUTIL_HPP
#define C_OSCCANUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscNodeDataPoolContent.hpp"
#include "C_OscCanSignal.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanUtilJ1939PgInfo
{
public:
   uint8_t u8_Priority;
   uint32_t u32_Pgn;
   uint8_t u8_SourceAddress;

   // Extracted information from PGN for easy use
   uint8_t u8_PduFormat;
   ///< In case of PDU Format 0x00 .. EF: PDU Specific is Destination Address
   ///< In case of PDU Format 0xF0 .. FF: PDU Specific is Group Extension
   uint8_t u8_PduSpecific;
   bool q_HasDestinationAddress;
   uint8_t u8_Edp;
   uint8_t u8_Dp;

   void SetPgn(const uint32_t ou32_Pgn);
};

class C_OscCanUtil
{
public:
   C_OscCanUtil(void);

   static bool h_IsSignalInMessage(const uint8_t ou8_Dlc, const stw::opensyde_core::C_OscCanSignal & orc_Signal);

   static void h_GetSignalValue(const uint8_t(&orau8_CanDb)[8], const stw::opensyde_core::C_OscCanSignal & orc_Signal,
                                std::vector<uint8_t> & orc_DataPoolData,
                                const C_OscNodeDataPoolContent::E_Type oe_ContentType);

   static void h_GetSignalValue(const uint8_t(&orau8_CanDb)[8], const stw::opensyde_core::C_OscCanSignal & orc_Signal,
                                C_OscNodeDataPoolContent & orc_Value);

   static void h_SetSignalValue(uint8_t(&orau8_CanDb)[8],
                                const stw::opensyde_core::C_OscCanSignal & orc_Signal,
                                const C_OscNodeDataPoolContent &orc_Value);

   static void h_GetJ1939PgInfoFromCanId(const uint32_t ou32_CanId, C_OscCanUtilJ1939PgInfo & orc_PgInfo);
   static void h_GetCanIdFromJ1939PgInfo(const C_OscCanUtilJ1939PgInfo & orc_PgInfo, uint32_t & oru32_CanId);
   static uint32_t h_GetVisiblePgn(const uint32_t ou32_Pgn);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
