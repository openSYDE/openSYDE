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
#include "stwtypes.h"
#include "C_OSCCanSignal.h"
#include "C_CieConverter.h"
#include "C_CamProMessageData.h"
#include "C_OSCNodeDataPoolContent.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamGenSigUtil
{
public:
   static std::vector<stw_types::uint8> h_ConvertRawDataFormat(const C_CamProMessageData & orc_Message);
   static stw_opensyde_core::C_OSCNodeDataPoolContent h_DecodeRawToContentSignal(
      const std::vector<stw_types::uint8> & orc_Raw, const stw_opensyde_core::C_OSCCanSignal & orc_Signal,
      const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_SignalMin);
   static void h_DecodeSignalValueToRaw(std::vector<stw_types::uint8> & orc_Raw,
                                        const stw_opensyde_core::C_OSCCanSignal & orc_Signal,
                                        const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Value);
   static stw_opensyde_core::C_OSCCanSignal h_ConvertDBCToOSY(const C_CieConverter::C_CIECanSignal & orc_Input);
   static stw_opensyde_core::C_OSCNodeDataPoolListElement h_ConvertDBCToOSY(
      const C_CieConverter::C_CIEDataPoolElement & orc_Input);
   static stw_opensyde_core::C_OSCCanMessage h_ConvertDBCToOSY(const C_CieConverter::C_CIECanMessage & orc_Input);
   static bool h_GetBit(const stw_types::uint32 ou32_BitPos, const std::vector<stw_types::uint8> & orc_Bytes);
   static void h_SetBit(const stw_types::uint32 ou32_BitPos, std::vector<stw_types::uint8> & orc_Bytes,
                        const bool oq_Value);
   static stw_types::uint32 h_CalcMessageHash(const stw_opensyde_core::C_OSCCanMessage & orc_Message,
                                              const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_DatapoolPart);
   static stw_types::uint32 h_CalcMessageHash(const stw_opensyde_core::C_OSCCanMessage & orc_Message,
                                              const stw_opensyde_core::C_OSCNodeDataPoolList & orc_List);
   static stw_types::uint32 h_CalcMessageHash(const C_CieConverter::C_CIECanMessage & orc_Message);

private:
   C_CamGenSigUtil(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
