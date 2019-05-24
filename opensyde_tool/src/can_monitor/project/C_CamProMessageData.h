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
#include <QString>
#include "stw_can.h"
#include "stwtypes.h"
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamProMessageData :
   public stw_can::T_STWCAN_Msg_TX
{
public:
   C_CamProMessageData(void);

   stw_scl::C_SCLString c_DataBaseFilePath;
   stw_scl::C_SCLString c_Name;
   bool q_DoCyclicTrigger;
   stw_types::uint32 u32_CyclicTriggerTime;
   stw_scl::C_SCLString c_Key;
   stw_types::uint32 u32_KeyPressOffset;

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

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
   bool GetRTR(void) const;

   //Set
   void SetMessageUint32Value(const C_CamProMessageData::E_GenericUint32DataSelector oe_Selector,
                              const stw_types::uint32 ou32_Value);
   void SetMessageBoolValue(const C_CamProMessageData::E_GenericBoolDataSelector oe_Selector, const bool oq_Value);
   void SetMessageKey(const QString & orc_Key, const stw_types::uint32 ou32_Offset);
   stw_types::sint32 SetMessageDataBytes(const std::vector<stw_types::uint8> & orc_DataBytes);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
