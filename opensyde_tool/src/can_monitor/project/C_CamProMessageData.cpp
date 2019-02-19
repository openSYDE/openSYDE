//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Message data structure with additional UI/ configuration info (implementation)

   Message data structure with additional UI/ configuration info

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.11.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "CSCLChecksums.h"
#include "C_CamProMessageData.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     26.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_CamProMessageData::C_CamProMessageData(void) :
   q_DoCyclicTrigger(false),
   u32_CyclicTriggerTime(100UL),
   u32_KeyPressOffset(0UL)
{
   //All necessary because struct cannot properly initialize its values!
   this->u8_XTD = 0U;
   this->u8_RTR = 0U;
   this->u32_ID = 0UL;
   this->u8_DLC = 0U;
   this->au8_Data[0UL] = 0U;
   this->au8_Data[1UL] = 0U;
   this->au8_Data[2UL] = 0U;
   this->au8_Data[3UL] = 0U;
   this->au8_Data[4UL] = 0U;
   this->au8_Data[5UL] = 0U;
   this->au8_Data[6UL] = 0U;
   this->au8_Data[7UL] = 0U;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     26.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamProMessageData::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_DataBaseFilePath.c_str(),
                                      this->c_DataBaseFilePath.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u8_XTD, sizeof(this->u8_XTD), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u8_RTR, sizeof(this->u8_RTR), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_ID, sizeof(this->u32_ID), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u8_DLC, sizeof(this->u8_DLC), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->au8_Data[0UL], 8, oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_DoCyclicTrigger, sizeof(this->q_DoCyclicTrigger), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_CyclicTriggerTime, sizeof(this->u32_CyclicTriggerTime),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Key.c_str(), this->c_Key.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_KeyPressOffset, sizeof(this->u32_KeyPressOffset),
                                      oru32_HashValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get extended flag

   \return
   True  Is extended format
   False Is standard format

   \created     29.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_CamProMessageData::GetExtended(void) const
{
   return this->u8_XTD == 1;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get remote transmission frame flag

   \return
   True  Is remote transmission frame
   False Is standard frame

   \created     15.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_CamProMessageData::GetRTR(void) const
{
   return this->u8_RTR == 1;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set message uint32 value

   \param[in] oe_Selector Data specifier
   \param[in] ou32_Value  New value


   \created     29.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamProMessageData::SetMessageUint32Value(const C_CamProMessageData::E_GenericUint32DataSelector oe_Selector,
                                                const uint32 ou32_Value)
{
   switch (oe_Selector)
   {
   case eGUIDS_ID:
      this->u32_ID = ou32_Value;
      break;
   case eGUIDS_DLC:
      this->u8_DLC = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB0:
      this->au8_Data[0UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB1:
      this->au8_Data[1UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB2:
      this->au8_Data[2UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB3:
      this->au8_Data[3UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB4:
      this->au8_Data[4UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB5:
      this->au8_Data[5UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB6:
      this->au8_Data[6UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_DB7:
      this->au8_Data[7UL] = static_cast<uint8>(ou32_Value);
      break;
   case eGUIDS_CYCLIC_TIME:
      this->u32_CyclicTriggerTime = ou32_Value;
      break;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set message bool value

   \param[in] oe_Selector Data specifier
   \param[in] oq_Value    New value

   \created     29.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamProMessageData::SetMessageBoolValue(const C_CamProMessageData::E_GenericBoolDataSelector oe_Selector,
                                              const bool oq_Value)
{
   switch (oe_Selector)
   {
   case eGBODS_RTR:
      if (oq_Value == true)
      {
         this->u8_RTR = 1;
      }
      else
      {
         this->u8_RTR = 0;
      }
      break;
   case eGBODS_EXTENDED:
      if (oq_Value == true)
      {
         this->u8_XTD = 1;
      }
      else
      {
         this->u8_XTD = 0;
      }
      break;
   case eGBODS_DO_CYCLIC:
      this->q_DoCyclicTrigger = oq_Value;
      break;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set message key related values

   \param[in] orc_Key     New key (single character only)
   \param[in] ou32_Offset Key trigger offset (ms)

   \created     15.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamProMessageData::SetMessageKey(const QString & orc_Key, const uint32 ou32_Offset)
{
   this->c_Key = orc_Key.toStdString().c_str();
   this->u32_KeyPressOffset = ou32_Offset;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set CAN message data bytes

   \param[in] orc_DataBytes New data byte content

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     16.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_CamProMessageData::SetMessageDataBytes(const std::vector<uint8> & orc_DataBytes)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_DataBytes.size() <= 8UL)
   {
      for (uint32 u32_It = 0UL; u32_It < orc_DataBytes.size(); ++u32_It)
      {
         this->au8_Data[u32_It] = orc_DataBytes[u32_It];
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}
