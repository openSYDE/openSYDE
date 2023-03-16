//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Collection of utility function for message signals (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMGENSIGUTIL_H
#define C_CAMGENSIGUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.hpp"
#include "C_OscCanSignal.hpp"
#include "C_CieConverter.hpp"
#include "C_CamProMessageData.hpp"
#include "C_OscNodeDataPoolContent.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamGenSigUtil
{
public:
   static std::vector<uint8_t> h_ConvertRawDataFormat(const C_CamProMessageData & orc_Message);
   static stw::opensyde_core::C_OscNodeDataPoolContent h_DecodeRawToContentSignal(const std::vector<uint8_t> & orc_Raw,
                                                                                  const stw::opensyde_core::C_OscCanSignal & orc_Signal, const stw::opensyde_core::C_OscNodeDataPoolContent & orc_SignalMin);
   static void h_DecodeSignalValueToRaw(std::vector<uint8_t> & orc_Raw,
                                        const stw::opensyde_core::C_OscCanSignal & orc_Signal,
                                        const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Value);
   static stw::opensyde_core::C_OscCanSignal h_ConvertDbcToOsy(const C_CieConverter::C_CieCanSignal & orc_Input);
   static stw::opensyde_core::C_OscNodeDataPoolListElement h_ConvertDbcToOsy(
      const C_CieConverter::C_CieDataPoolElement & orc_Input);
   static stw::opensyde_core::C_OscCanMessage h_ConvertDbcToOsy(const C_CieConverter::C_CieCanMessage & orc_Input);
   static bool h_GetBit(const uint32_t ou32_BitPos, const std::vector<uint8_t> & orc_Bytes);
   static void h_SetBit(const uint32_t ou32_BitPos, std::vector<uint8_t> & orc_Bytes, const bool oq_Value);
   static uint32_t h_CalcMessageHash(const stw::opensyde_core::C_OscCanMessage & orc_Message,
                                     const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_DatapoolPart);
   static uint32_t h_CalcMessageHash(const stw::opensyde_core::C_OscCanMessage & orc_Message,
                                     const stw::opensyde_core::C_OscNodeDataPoolList & orc_List);
   static uint32_t h_CalcMessageHash(const C_CieConverter::C_CieCanMessage & orc_Message);

private:
   C_CamGenSigUtil(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
