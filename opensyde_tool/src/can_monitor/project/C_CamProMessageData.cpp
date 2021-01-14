//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message data structure with additional UI/ configuration info (implementation)

   Message data structure with additional UI/ configuration info

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "CSCLChecksums.h"
#include "C_CamProMessageData.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamProMessageData::C_CamProMessageData(void) :
   q_ContainsValidHash(false),
   u32_Hash(0UL),
   q_DoCyclicTrigger(false),
   u32_CyclicTriggerTime(100UL),
   u32_KeyPressOffset(0UL),
   q_IsExtended(false),
   q_IsRTR(false),
   u16_Dlc(0U),
   u32_Id(0UL)
{
   //Start with 8 Bytes, initialize with zero
   c_Bytes.resize(8U, 0U);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProMessageData::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_DataBaseFilePath.c_str(),
                                      this->c_DataBaseFilePath.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_ContainsValidHash, sizeof(this->q_ContainsValidHash), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_Hash, sizeof(this->u32_Hash), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_IsExtended, sizeof(this->q_IsExtended), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_IsRTR, sizeof(this->q_IsRTR), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_Id, sizeof(this->u32_Id), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u16_Dlc, sizeof(this->u16_Dlc), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->c_Bytes[0UL], c_Bytes.size(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_DoCyclicTrigger, sizeof(this->q_DoCyclicTrigger), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_CyclicTriggerTime, sizeof(this->u32_CyclicTriggerTime),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Key.c_str(), this->c_Key.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_KeyPressOffset, sizeof(this->u32_KeyPressOffset),
                                      oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill CAN message based on current content

   \return
   CAN message based on current content
*/
//----------------------------------------------------------------------------------------------------------------------
stw_can::T_STWCAN_Msg_TX C_CamProMessageData::ToCANMessage(void) const
{
   stw_can::T_STWCAN_Msg_TX c_Retval;
   c_Retval.u32_ID = this->u32_Id;
   c_Retval.u8_Align = 0U;
   c_Retval.u8_XTD = C_CamProMessageData::h_GetBoolValue(this->q_IsExtended);
   c_Retval.u8_RTR = C_CamProMessageData::h_GetBoolValue(this->q_IsRTR);
   c_Retval.u8_DLC = static_cast<uint8>(this->u16_Dlc);
   for (uint8 u8_ItByte = 0U; u8_ItByte < 8U; ++u8_ItByte)
   {
      if (u8_ItByte < this->u16_Dlc)
      {
         c_Retval.au8_Data[u8_ItByte] = this->c_Bytes[u8_ItByte];
      }
      else
      {
         c_Retval.au8_Data[u8_ItByte] = 0U;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get extended flag

   \return
   True  Is extended format
   False Is standard format
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamProMessageData::GetExtended(void) const
{
   return this->q_IsExtended;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get remote transmission frame flag

   \return
   True  Is remote transmission frame
   False Is standard frame
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamProMessageData::GetRTR(void) const
{
   return this->q_IsRTR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message uint32 value

   \param[in]  oe_Selector    Data specifier
   \param[in]  ou32_Value     New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProMessageData::SetMessageUint32Value(const C_CamProMessageData::E_GenericUint32DataSelector oe_Selector,
                                                const uint32 ou32_Value)
{
   switch (oe_Selector)
   {
   case eGUIDS_ID:
      this->u32_Id = ou32_Value;
      break;
   case eGUIDS_DLC:
      this->u16_Dlc = static_cast<uint16>(ou32_Value);
      break;
   case eGUIDS_DB0:
      this->c_Bytes[0UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB1:
      this->c_Bytes[1UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB2:
      this->c_Bytes[2UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB3:
      this->c_Bytes[3UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB4:
      this->c_Bytes[4UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB5:
      this->c_Bytes[5UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB6:
      this->c_Bytes[6UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB7:
      this->c_Bytes[7UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_CYCLIC_TIME:
      this->u32_CyclicTriggerTime = ou32_Value;
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message bool value

   \param[in]  oe_Selector    Data specifier
   \param[in]  oq_Value       New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProMessageData::SetMessageBoolValue(const C_CamProMessageData::E_GenericBoolDataSelector oe_Selector,
                                              const bool oq_Value)
{
   switch (oe_Selector)
   {
   case eGBODS_RTR:
      this->q_IsRTR = oq_Value;
      break;
   case eGBODS_EXTENDED:
      this->q_IsExtended = oq_Value;
      break;
   case eGBODS_DO_CYCLIC:
      this->q_DoCyclicTrigger = oq_Value;
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message key related values

   \param[in]  orc_Key        New key (single character only)
   \param[in]  ou32_Offset    Key trigger offset (ms)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProMessageData::SetMessageKey(const QString & orc_Key, const uint32 ou32_Offset)
{
   this->c_Key = orc_Key.toStdString().c_str();
   this->u32_KeyPressOffset = ou32_Offset;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set CAN message data bytes

   \param[in]  orc_DataBytes  New data byte content

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProMessageData::SetMessageDataBytes(const std::vector<uint8> & orc_DataBytes)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_DataBytes.size() <= 8UL)
   {
      for (uint32 u32_It = 0UL; u32_It < orc_DataBytes.size(); ++u32_It)
      {
         this->c_Bytes[u32_It] = orc_DataBytes[u32_It];
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert bool to U8

   \param[in]  oq_Value    Bool value

   \return
   U8 value
*/
//----------------------------------------------------------------------------------------------------------------------
uint8 C_CamProMessageData::h_GetBoolValue(const bool oq_Value)
{
   uint8 u8_Retval;

   if (oq_Value == true)
   {
      u8_Retval = 1;
   }
   else
   {
      u8_Retval = 0;
   }
   return u8_Retval;
}
