//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to copy a message and its detected signals to the clipboard as text (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETCLIPBOARDHELPER_H
#define C_CAMMETCLIPBOARDHELPER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "C_UtiClipBoardHelper.h"

#include "C_OSCComMessageLoggerData.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMetClipBoardHelperCanMessageData
{
public:
   C_CamMetClipBoardHelperCanMessageData() :
      pc_MessageData(NULL),
      q_Extended(false)
   {
   }

   const stw_opensyde_core::C_OSCComMessageLoggerData * pc_MessageData;
   bool q_Extended;
   std::vector<stw_types::sintn> c_ExpandedIndices;
};

class C_CamMetClipBoardHelper :
   public C_UtiClipBoardHelper
{
public:
   static void h_StoreCanMessages(const bool oq_DisplayAsHex, const bool oq_DisplayTimestampRelative,
                                  const bool oq_DisplayTimestampAbsoluteTimeOfDay,
                                  const std::vector<C_CamMetClipBoardHelperCanMessageData> & orc_MessageData);

private:
   static void h_AddHeader(QString & orc_Text, const stw_types::sintn osn_WidthInitial,
                           const stw_types::sintn osn_WidthTime, const stw_types::sintn osn_WidthID,
                           const stw_types::sintn osn_WidthName, const stw_types::sintn osn_WidthDir,
                           const stw_types::sintn osn_WidthDLC, const stw_types::sintn osn_WidthData);
   static void h_AddMessage(QString & orc_Text, const stw_opensyde_core::C_OSCComMessageLoggerData & orc_MessageData,
                            const bool oq_IsExtended, const bool oq_DisplayAsHex,
                            const bool oq_DisplayTimestampRelative, const bool oq_DisplayTimestampAbsoluteTimeOfDay,
                            const stw_types::sintn osn_WidthInitial, const stw_types::sintn osn_WidthTime,
                            const stw_types::sintn osn_WidthID, const stw_types::sintn osn_WidthName,
                            const stw_types::sintn osn_WidthDir, const stw_types::sintn osn_WidthDLC,
                            const stw_types::sintn osn_WidthData,
                            const std::vector<stw_types::sintn> & orc_ExpandedIndices);
   static void h_AddCanSignals(const stw_types::sintn osn_LineWidthInitial, const bool oq_DisplayAsHex,
                               const std::vector<stw_opensyde_core::C_OSCComMessageLoggerDataSignal> & orc_Signals,
                               const std::vector<stw_types::sintn> & orc_ExpandedSignalIndices,
                               QString & orc_CompleteString);
   static void h_AddCanSignal(const stw_types::sintn osn_LineWidthInitial, const bool oq_DisplayAsHex,
                              const stw_opensyde_core::C_OSCComMessageLoggerDataSignal & orc_Signal,
                              QString & orc_CompleteString, const bool oq_IsMux);

protected:
   C_CamMetClipBoardHelper(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
