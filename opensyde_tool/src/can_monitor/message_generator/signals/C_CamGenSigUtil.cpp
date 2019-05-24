//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Collection of utility function for message signals (implementation)

   Collection of utility function for message signals

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "TGLUtils.h"
#include "C_OSCCanUtil.h"
#include "C_CamGenSigUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert message data array to vector

   \param[in] orc_Message message data

   \return
   Converted message data of DLC length
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint8> C_CamGenSigUtil::h_ConvertRawDataFormat(const C_CamProMessageData & orc_Message)
{
   std::vector<uint8> c_Retval;
   c_Retval.reserve(orc_Message.u8_DLC);
   //Copy each byte
   for (uint8 u8_It = 0; u8_It < orc_Message.u8_DLC; ++u8_It)
   {
      c_Retval.push_back(orc_Message.au8_Data[u8_It]);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Decode raw value into C_OSCNodeDataPoolContent

   Based on: openSYDE structure

   \param[in] orc_Raw       Raw value
   \param[in] orc_Signal    Signal description
   \param[in] orc_SignalMin Signal min value structure (only used for init)

   \return
   Value as C_OSCNodeDataPoolContent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolContent C_CamGenSigUtil::h_DecodeRawToContentSignal(const std::vector<uint8> & orc_Raw,
                                                                     const C_OSCCanSignal & orc_Signal,
                                                                     const C_OSCNodeDataPoolContent & orc_SignalMin)
{
   C_OSCNodeDataPoolContent c_Retval = orc_SignalMin;
   //Convert to byte array
   uint8 au8_Bytes[8];

   for (uint8 u8_ItByte = 0U; u8_ItByte < 8U; ++u8_ItByte)
   {
      if (static_cast<uint32>(u8_ItByte) < orc_Raw.size())
      {
         au8_Bytes[u8_ItByte] = orc_Raw[static_cast<uint32>(u8_ItByte)];
      }
      else
      {
         au8_Bytes[u8_ItByte] = 0U;
      }
   }
   //Use core function
   C_OSCCanUtil::h_GetSignalValue(au8_Bytes, orc_Signal, c_Retval);

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Decode signal value to raw

   \param[in,out] orc_Raw    Raw CAN message values
   \param[in]     orc_Signal Signal description
   \param[in]     orc_Value  Signal value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigUtil::h_DecodeSignalValueToRaw(std::vector<uint8> & orc_Raw, const C_OSCCanSignal & orc_Signal,
                                               const C_OSCNodeDataPoolContent & orc_Value)
{
   //Convert to byte array
   uint8 au8_Bytes[8];

   //Step 1: zero all SIGNAL bits (required by core function)
   for (uint16 u16_ItBit = 0U; u16_ItBit < orc_Signal.u16_ComBitLength; ++u16_ItBit)
   {
      C_CamGenSigUtil::h_SetBit(orc_Signal.GetDataBytesBitPosOfSignalBit(u16_ItBit), orc_Raw, false);
   }

   //Step 2: covert all bytes to array
   for (uint8 u8_ItByte = 0U; u8_ItByte < 8U; ++u8_ItByte)
   {
      if (static_cast<uint32>(u8_ItByte) < orc_Raw.size())
      {
         au8_Bytes[u8_ItByte] = orc_Raw[static_cast<uint32>(u8_ItByte)];
      }
      else
      {
         au8_Bytes[u8_ItByte] = 0U;
      }
   }
   //Step 3: use core function
   C_OSCCanUtil::h_SetSignalValue(au8_Bytes, orc_Signal, orc_Value);
   //Step 4: write back to vector
   for (uint8 u8_ItByte = 0U; (static_cast<uint32>(u8_ItByte) < orc_Raw.size()) && (u8_ItByte < 8U); ++u8_ItByte)
   {
      orc_Raw[static_cast<uint32>(u8_ItByte)] = au8_Bytes[u8_ItByte];
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert DBC to opensyde signal

   Warning: u32_ComDataElementIndex is invalid

   \param[in] orc_Input DBC input

   \return
   opensyde signal
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanSignal C_CamGenSigUtil::h_ConvertDBCToOSY(const C_CieConverter::C_CIECanSignal & orc_Input)
{
   C_OSCCanSignal c_Retval;

   //Extract necessary openSYDE structure from DBC structure
   c_Retval.u16_ComBitLength = orc_Input.u16_ComBitLength;
   c_Retval.u16_ComBitStart = orc_Input.u16_ComBitStart;
   c_Retval.e_ComByteOrder = orc_Input.e_ComByteOrder;
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert DBC to opensyde signal

   Warning: e_Access & q_DiagEventCall & q_NvMValueChanged & q_IsValid & u32_NvMStartAddress are invalid

   \param[in] orc_Input DBC input

   \return
   opensyde signal
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolListElement C_CamGenSigUtil::h_ConvertDBCToOSY(const C_CieConverter::C_CIEDataPoolElement & orc_Input)
{
   C_OSCNodeDataPoolListElement c_Retval;

   c_Retval.c_Name = orc_Input.c_Name;
   c_Retval.c_Comment = orc_Input.c_Comment;
   c_Retval.c_MinValue = orc_Input.c_MinValue;
   c_Retval.c_MaxValue = orc_Input.c_MaxValue;
   c_Retval.f64_Factor = orc_Input.f64_Factor;
   c_Retval.f64_Offset = orc_Input.f64_Offset;
   c_Retval.c_Unit = orc_Input.c_Unit;
   c_Retval.c_DataSetValues = orc_Input.c_DataSetValues;
   c_Retval.c_Value = orc_Input.c_MinValue;
   c_Retval.c_NvmValue = orc_Input.c_MinValue;
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert DBC to opensyde message

   Warning: u32_TimeoutMs & u16_DelayTimeMs & signals are all not filled in

   \param[in] orc_Input DBC input

   \return
   opensyde message
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanMessage C_CamGenSigUtil::h_ConvertDBCToOSY(const C_CieConverter::C_CIECanMessage & orc_Input)
{
   C_OSCCanMessage c_Retval;

   c_Retval.c_Name = orc_Input.c_Name;
   c_Retval.c_Comment = orc_Input.c_Comment;
   c_Retval.u32_CanId = orc_Input.u32_CanId;
   c_Retval.q_IsExtended = orc_Input.q_IsExtended;
   c_Retval.u16_Dlc = orc_Input.u16_Dlc;
   c_Retval.e_TxMethod = orc_Input.e_TxMethod;
   c_Retval.u32_CycleTimeMs = orc_Input.u32_CycleTimeMs;
   //Both are not contained in DBC
   c_Retval.u32_TimeoutMs = 0UL;
   c_Retval.u16_DelayTimeMs = 0UL;
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get a single bit from the input byte vector

   \param[in] ou32_BitPos Bit position
   \param[in] orc_Bytes   Bytes to use as data

   \return
   Bit value
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamGenSigUtil::h_GetBit(const uint32 ou32_BitPos, const std::vector<uint8> & orc_Bytes)
{
   bool q_Retval = false;

   tgl_assert(ou32_BitPos < 64UL);
   if (ou32_BitPos < 64UL)
   {
      //Which byte (CAN)
      const uint16 u16_InBytePos = ou32_BitPos / 8U;
      if (u16_InBytePos < orc_Bytes.size())
      {
         const uint8 u8_Byte = orc_Bytes[u16_InBytePos];
         //Which bit (CAN)
         const uint8 u8_Mask = 0x1U << (ou32_BitPos % 8U);
         q_Retval = (u8_Byte & u8_Mask) > 0U;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set a single bit in the output byte vector

   \param[in]     ou32_BitPos Bit position
   \param[in,out] orc_Bytes   Bytes to change
   \param[in]     oq_Value    Bit value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigUtil::h_SetBit(const uint32 ou32_BitPos, std::vector<uint8> & orc_Bytes, const bool oq_Value)
{
   tgl_assert(ou32_BitPos < 64UL);
   if (ou32_BitPos < 64UL)
   {
      //Which byte (output)
      const uint16 u16_OutBytePos = ou32_BitPos / 8U;
      if (u16_OutBytePos < orc_Bytes.size())
      {
         //Which bit (output)
         const uint8 u8_Mask = 0x1U << (ou32_BitPos % 8U);
         if (oq_Value == true)
         {
            //Apply 1
            orc_Bytes[u16_OutBytePos] = orc_Bytes[u16_OutBytePos] | u8_Mask;
         }
         else
         {
            //Apply 0
            const uint8 u8_InvertedMask = ~u8_Mask;
            orc_Bytes[u16_OutBytePos] = orc_Bytes[u16_OutBytePos] & u8_InvertedMask;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamGenSigUtil::C_CamGenSigUtil(void)
{
}
