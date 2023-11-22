//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCOMAUTOSUPPORT_HPP
#define C_OSCCOMAUTOSUPPORT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <map>

#include "C_OscCanProtocol.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscComAutoSupport
{
public:
   C_OscComAutoSupport();
   virtual ~C_OscComAutoSupport(void);

   uint8_t MessageCounter(const uint32_t ou32_MessageId, uint8_t ou8_MessageCounter);
   uint8_t SetCyclicRedundancyCheckCalculation(const uint32_t ou32_MessageId, const uint8_t ou8_Count,
                                               const uint8_t * const opu8_Data);
   void ClearAutoSupportInfo(void);
   bool SupportInvertedCanMessage(const int32_t os32_MessageId);
   static void h_InvertCanMessage(const uint8_t * const opu8_Data, uint8_t * const opu8_InvertedData);
   void AutoSupportModeInfo(const int32_t os32_MessageId, const bool oq_SetAutoSupportMode,
                            const C_OscCanProtocol::E_Type oe_ProtocolType);
   void ResetOnClearData(void);
   static uint8_t h_GetCyclicRedundancyCheckCalculation(const uint8_t ou8_Count, const uint8_t * const opu8_Data);
   void ResetMessageCounters(const int32_t os32_MessageId, const bool oq_SetAutoSupportMode,
                             const C_OscCanProtocol::E_Type oe_ProtocolType);

private:
   std::map<int32_t, uint8_t > mc_MessageCounterInfoMap;
   std::map<int32_t, std::pair<bool, C_OscCanProtocol::E_Type> > mc_MessageAutoSupportInfoMap;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
