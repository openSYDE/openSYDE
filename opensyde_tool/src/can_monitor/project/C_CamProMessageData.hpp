//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message data structure with additional UI/ configuration info (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMPROMESSAGEDATA_H
#define C_CAMPROMESSAGEDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <array>
#include <QString>
#include "stw_can.hpp"
#include "stwtypes.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamProMessageData
{
public:
   C_CamProMessageData(void);

   stw::scl::C_SclString c_DataBaseFilePath;
   stw::scl::C_SclString c_Name;
   bool q_ContainsValidHash;
   uint32_t u32_Hash;
   bool q_DoCyclicTrigger;
   uint32_t u32_CyclicTriggerTime;
   stw::scl::C_SclString c_Key;
   uint32_t u32_KeyPressOffset;
   bool q_IsExtended;
   bool q_IsRtr;
   uint16_t u16_Dlc;
   uint32_t u32_Id;
   std::vector<uint8_t> c_Bytes;

   void CalcHash(uint32_t & oru32_HashValue) const;
   stw::can::T_STWCAN_Msg_TX ToCanMessage(void) const;

   enum E_GenericUint32DataSelector
   {
      eGUIDS_ID = 0,
      eGUIDS_DLC,
      eGUIDS_DB0,
      eGUIDS_DB1,
      eGUIDS_DB2,
      eGUIDS_DB3,
      eGUIDS_DB4,
      eGUIDS_DB5,
      eGUIDS_DB6,
      eGUIDS_DB7,
      eGUIDS_CYCLIC_TIME
   };

   enum E_GenericBoolDataSelector
   {
      eGBODS_EXTENDED = 0,
      eGBODS_RTR,
      eGBODS_DO_CYCLIC
   };

   //Get
   bool GetExtended(void) const;
   bool GetRtr(void) const;

   //Set
   void SetMessageUint32Value(const C_CamProMessageData::E_GenericUint32DataSelector oe_Selector,
                              const uint32_t ou32_Value);
   void SetMessageBoolValue(const C_CamProMessageData::E_GenericBoolDataSelector oe_Selector, const bool oq_Value);
   void SetMessageKey(const QString & orc_Key, const uint32_t ou32_Offset);
   int32_t SetMessageDataBytes(const std::vector<uint8_t> & orc_DataBytes);
   static uint8_t h_GetBoolValue(const bool oq_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
