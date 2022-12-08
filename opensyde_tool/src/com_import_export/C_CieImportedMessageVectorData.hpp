//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Small utility data structure to group all imported message data for one specific direction (TX/RX)
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEIMPORTEDMESSAGEVECTORDATA_H
#define C_CIEIMPORTEDMESSAGEVECTORDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>

#include "C_OscCanMessage.hpp"
#include "C_OscNodeDataPoolListElement.hpp"
#include "C_PuiSdNodeCanMessage.hpp"
#include "C_PuiSdNodeDataPoolListElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieImportedMessageVectorData
{
public:
   C_CieImportedMessageVectorData();
   C_CieImportedMessageVectorData(const std::vector<stw::opensyde_core::C_OscCanMessage> & orc_OscMessageData,
                                  const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalData,
                                  const std::vector<C_PuiSdNodeCanMessage> & orc_UiMessageData,
                                  const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalData,
                                  const std::vector<QString> * const opc_WarningMessagesPerMessage = NULL);

   std::vector<stw::opensyde_core::C_OscCanMessage> c_OscMessageData;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> c_OscSignalData;
   std::vector<C_PuiSdNodeCanMessage> c_UiMessageData;
   std::vector<C_PuiSdNodeDataPoolListElement> c_UiSignalData;
   std::vector<QString> c_WarningMessagesPerMessage;

   void Clear(void);
   void WriteBack(std::vector<stw::opensyde_core::C_OscCanMessage> & orc_OscMessageData,
                  std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalData,
                  std::vector<C_PuiSdNodeCanMessage> & orc_UiMessageData,
                  std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalData,
                  std::vector<QString> & orc_WarningMessagesPerMessage) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
