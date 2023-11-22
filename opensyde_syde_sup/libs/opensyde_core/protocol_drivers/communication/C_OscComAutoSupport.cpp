//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description

   Detailed description (optional). The module shall be described detailed if it is not described completely
   by short description.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_OscComAutoSupport.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComAutoSupport::C_OscComAutoSupport()
{
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default deconstructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComAutoSupport::~C_OscComAutoSupport()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check support for CRC Caluclation

   \param[in]       ou32_MessageId      Current message Id
   \param[in]       ou8_Count          number of bytes the checksum has to be calculated for
   \param[in]       opu8_Data          pointer to data bytes

   \return
   Type of int32_t  CheckSum
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_OscComAutoSupport::SetCyclicRedundancyCheckCalculation(const uint32_t ou32_MessageId, const uint8_t ou8_Count,
                                                                 const uint8_t * const opu8_Data)
{
   uint8_t u8_CrcValue = static_cast<uint8_t>(opu8_Data[7]);
   const std::pair<bool, C_OscCanProtocol::E_Type> c_PairValue = mc_MessageAutoSupportInfoMap[ou32_MessageId];

   if ((mc_MessageAutoSupportInfoMap.count(ou32_MessageId) == 1) &&
       ((c_PairValue.first == true) && (c_PairValue.second == C_OscCanProtocol::eECES)))
   {
      u8_CrcValue = h_GetCyclicRedundancyCheckCalculation(ou8_Count, opu8_Data);
   }

   return u8_CrcValue;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to manage/caluclate message counters based on receiving

   \param[in]       ou32_MessageId     Current message Id
   \param[in]      ou8_MessageCounter   6th bit value of data field in current can message

   \return
   int32_t  Message counter
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_OscComAutoSupport::MessageCounter(const uint32_t ou32_MessageId, uint8_t ou8_MessageCounter)
{
   const std::pair<bool, C_OscCanProtocol::E_Type> c_PairValue = mc_MessageAutoSupportInfoMap[ou32_MessageId];

   if ((mc_MessageAutoSupportInfoMap.count(ou32_MessageId) == 1) &&
       ((c_PairValue.first == true) && (c_PairValue.second == C_OscCanProtocol::eECES)))
   {
      if (mc_MessageCounterInfoMap.count(ou32_MessageId) == 0)
      {
         ou8_MessageCounter = 0;
         mc_MessageCounterInfoMap[ou32_MessageId] = ou8_MessageCounter;
      }
      else
      {
         ou8_MessageCounter = mc_MessageCounterInfoMap[ou32_MessageId];
         if (ou8_MessageCounter == 255)
         {
            ou8_MessageCounter = 0;
         }
         else
         {
            ++ou8_MessageCounter;
         }
         mc_MessageCounterInfoMap[ou32_MessageId] = ou8_MessageCounter;
      }
   }
   return ou8_MessageCounter;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clearing message counters
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComAutoSupport::ClearAutoSupportInfo()
{
   mc_MessageCounterInfoMap.clear();
   mc_MessageAutoSupportInfoMap.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset all counter values to 0
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComAutoSupport::ResetOnClearData()
{
   std::map<int32_t, uint8_t>::iterator c_It = mc_MessageCounterInfoMap.begin();
   while (c_It != mc_MessageCounterInfoMap.end())
   {
      c_It = mc_MessageCounterInfoMap.erase(c_It);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function calculates CRC

   \param[in]       ou8_Count          number of bytes the checksum has to be calculated for
   \param[in]       opu8_Data          pointer to data bytes

   \return
   Type of int32_t  CheckSum
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_OscComAutoSupport::h_GetCyclicRedundancyCheckCalculation(const uint8_t ou8_Count,
                                                                   const uint8_t * const opu8_Data)
{
   const uint8_t au8_CRCTABLE[256] =
   {
      //.0     .1     .2     .3     .4     .5     .6     .7     .8     .9     .A     .B     .C     .D     .E     .F
      0x00U, 0xE5U, 0x2FU, 0xCAU, 0x5EU, 0xBBU, 0x71U, 0x94U, 0xBCU, 0x59U, 0x93U, 0x76U, 0xE2U, 0x07U, 0xCDU, 0x28U,
      0x9DU, 0x78U, 0xB2U, 0x57U, 0xC3U, 0x26U, 0xECU, 0x09U, 0x21U, 0xC4U, 0x0EU, 0xEBU, 0x7FU, 0x9AU, 0x50U, 0xB5U,
      0xDFU, 0x3AU, 0xF0U, 0x15U, 0x81U, 0x64U, 0xAEU, 0x4BU, 0x63U, 0x86U, 0x4CU, 0xA9U, 0x3DU, 0xD8U, 0x12U, 0xF7U,
      0x42U, 0xA7U, 0x6DU, 0x88U, 0x1CU, 0xF9U, 0x33U, 0xD6U, 0xFEU, 0x1BU, 0xD1U, 0x34U, 0xA0U, 0x45U, 0x8FU, 0x6AU,
      0x5BU, 0xBEU, 0x74U, 0x91U, 0x05U, 0xE0U, 0x2AU, 0xCFU, 0xE7U, 0x02U, 0xC8U, 0x2DU, 0xB9U, 0x5CU, 0x96U, 0x73U,
      0xC6U, 0x23U, 0xE9U, 0x0CU, 0x98U, 0x7DU, 0xB7U, 0x52U, 0x7AU, 0x9FU, 0x55U, 0xB0U, 0x24U, 0xC1U, 0x0BU, 0xEEU,
      0x84U, 0x61U, 0xABU, 0x4EU, 0xDAU, 0x3FU, 0xF5U, 0x10U, 0x38U, 0xDDU, 0x17U, 0xF2U, 0x66U, 0x83U, 0x49U, 0xACU,
      0x19U, 0xFCU, 0x36U, 0xD3U, 0x47U, 0xA2U, 0x68U, 0x8DU, 0xA5U, 0x40U, 0x8AU, 0x6FU, 0xFBU, 0x1EU, 0xD4U, 0x31U,
      0xB6U, 0x53U, 0x99U, 0x7CU, 0xE8U, 0x0DU, 0xC7U, 0x22U, 0x0AU, 0xEFU, 0x25U, 0xC0U, 0x54U, 0xB1U, 0x7BU, 0x9EU,
      0x2BU, 0xCEU, 0x04U, 0xE1U, 0x75U, 0x90U, 0x5AU, 0xBFU, 0x97U, 0x72U, 0xB8U, 0x5DU, 0xC9U, 0x2CU, 0xE6U, 0x03U,
      0x69U, 0x8CU, 0x46U, 0xA3U, 0x37U, 0xD2U, 0x18U, 0xFDU, 0xD5U, 0x30U, 0xFAU, 0x1FU, 0x8BU, 0x6EU, 0xA4U, 0x41U,
      0xF4U, 0x11U, 0xDBU, 0x3EU, 0xAAU, 0x4FU, 0x85U, 0x60U, 0x48U, 0xADU, 0x67U, 0x82U, 0x16U, 0xF3U, 0x39U, 0xDCU,
      0xEDU, 0x08U, 0xC2U, 0x27U, 0xB3U, 0x56U, 0x9CU, 0x79U, 0x51U, 0xB4U, 0x7EU, 0x9BU, 0x0FU, 0xEAU, 0x20U, 0xC5U,
      0x70U, 0x95U, 0x5FU, 0xBAU, 0x2EU, 0xCBU, 0x01U, 0xE4U, 0xCCU, 0x29U, 0xE3U, 0x06U, 0x92U, 0x77U, 0xBDU, 0x58U,
      0x32U, 0xD7U, 0x1DU, 0xF8U, 0x6CU, 0x89U, 0x43U, 0xA6U, 0x8EU, 0x6BU, 0xA1U, 0x44U, 0xD0U, 0x35U, 0xFFU, 0x1AU,
      0xAFU, 0x4AU, 0x80U, 0x65U, 0xF1U, 0x14U, 0xDEU, 0x3BU, 0x13U, 0xF6U, 0x3CU, 0xD9U, 0x4DU, 0xA8U, 0x62U, 0x87U
   };

   uint8_t u8_CrcValue = 0xECU;

   for (int32_t s32_Index = 0; s32_Index < ou8_Count; s32_Index++)
   {
      const uint8_t u8_Index2 = u8_CrcValue ^ opu8_Data[s32_Index];
      u8_CrcValue = au8_CRCTABLE[u8_Index2];
   }
   return u8_CrcValue;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  reset message counters to 0
 *
   \param[in]       os32_MessageId         Current CAN Message Id
   \param[in]       oq_SetAutoSupportMode   Auto protocol support for current message
   \param[in]       oe_ProtocolType         Protocol type for current message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComAutoSupport::ResetMessageCounters(const int32_t os32_MessageId, const bool oq_SetAutoSupportMode,
                                               const C_OscCanProtocol::E_Type oe_ProtocolType)
{
   if (((oq_SetAutoSupportMode) == true) && (mc_MessageAutoSupportInfoMap.count(os32_MessageId) == 1) &&
       (oe_ProtocolType == C_OscCanProtocol::eECES))
   {
      mc_MessageCounterInfoMap.erase(os32_MessageId);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  setting/Updating AutoSupportProtocol Info in QMap for further processings
 *
   \param[in]       os32_MessageId            Current CAN Message Id
   \param[in]       oq_SetAutoSupportMode     Auto protocol support for current message
   \param[in]       oe_ProtocolType           Protocol type for current message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComAutoSupport::AutoSupportModeInfo(const int32_t os32_MessageId, const bool oq_SetAutoSupportMode,
                                              const C_OscCanProtocol::E_Type oe_ProtocolType)
{
   const std::pair<bool, C_OscCanProtocol::E_Type> c_Pair(oq_SetAutoSupportMode, oe_ProtocolType);

   mc_MessageAutoSupportInfoMap[os32_MessageId] = c_Pair;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Invert received Can message data field

   \param[in]            opu8_Data             Original message data
   \param[in, out]       opu8_InvertedData     Inverted message data

*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComAutoSupport::h_InvertCanMessage(const uint8_t * const opu8_Data, uint8_t * const opu8_InvertedData)
{
   for (int32_t s32_Itr = 0; s32_Itr < 8; s32_Itr++)
   {
      opu8_InvertedData[s32_Itr] = ~static_cast<uint8_t>(opu8_Data[s32_Itr]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check for Can Message supports to Invert message

   \param[in]       os32_MessageId         Current CAN Message Id

   \return
   bool

   \retval   true    On supports to Invert message
   \retval   fasle   On not supports to Invert message
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscComAutoSupport::SupportInvertedCanMessage(const int32_t os32_MessageId)
{
   bool q_IsSupportInvertedCanMessage = false;
   const std::pair<bool, C_OscCanProtocol::E_Type> c_PairValue = mc_MessageAutoSupportInfoMap[os32_MessageId];

   if ((mc_MessageAutoSupportInfoMap.count(os32_MessageId) == 1) &&
       ((c_PairValue.first == true) && (c_PairValue.second == C_OscCanProtocol::eCAN_OPEN_SAFETY)))
   {
      q_IsSupportInvertedCanMessage = true;
   }
   return q_IsSupportInvertedCanMessage;
}
