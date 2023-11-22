//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to copy a message and its detected signals to the clipboard as text (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETCLIPBOARDHELPER_HPP
#define C_CAMMETCLIPBOARDHELPER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "C_UtiClipBoardHelper.hpp"

#include "C_OscComMessageLoggerData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
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

   const stw::opensyde_core::C_OscComMessageLoggerData * pc_MessageData;
   bool q_Extended;
   std::vector<int32_t> c_ExpandedIndices;
};

class C_CamMetClipBoardHelper :
   public C_UtiClipBoardHelper
{
public:
   static void h_StoreCanMessages(const bool oq_DisplayAsHex, const bool oq_DisplayTimestampRelative,
                                  const bool oq_DisplayTimestampAbsoluteTimeOfDay,
                                  const std::vector<C_CamMetClipBoardHelperCanMessageData> & orc_MessageData);

private:
   static void mh_AddHeader(QString & orc_Text, const int32_t os32_WidthInitial, const int32_t os32_WidthTime,
                            const int32_t os32_WidthId, const int32_t os32_WidthName, const int32_t os32_WidthDir,
                            const int32_t os32_WidthDlc, const int32_t os32_WidthData, const int32_t os32_WidthCounter);
   static void mh_AddMessage(QString & orc_Text, const stw::opensyde_core::C_OscComMessageLoggerData & orc_MessageData,
                             const bool oq_IsExtended, const bool oq_DisplayAsHex,
                             const bool oq_DisplayTimestampRelative, const bool oq_DisplayTimestampAbsoluteTimeOfDay,
                             const int32_t os32_WidthInitial, const int32_t os32_WidthTime, const int32_t os32_WidthId,
                             const int32_t os32_WidthName, const int32_t os32_WidthDir, const int32_t os32_WidthDlc,
                             const int32_t os32_WidthData, const int32_t os32_WidthCounter,
                             const std::vector<int32_t> & orc_ExpandedIndices);
   static void mh_AddCanSignals(const int32_t os32_LineWidthInitial, const bool oq_DisplayAsHex,
                                const std::vector<stw::opensyde_core::C_OscComMessageLoggerDataSignal> & orc_Signals,
                                const std::vector<int32_t> & orc_ExpandedSignalIndices, QString & orc_CompleteString);
   static void mh_AddCanSignal(const int32_t os32_LineWidthInitial, const bool oq_DisplayAsHex,
                               const stw::opensyde_core::C_OscComMessageLoggerDataSignal & orc_Signal,
                               QString & orc_CompleteString, const bool oq_IsMux);

protected:
   C_CamMetClipBoardHelper(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
