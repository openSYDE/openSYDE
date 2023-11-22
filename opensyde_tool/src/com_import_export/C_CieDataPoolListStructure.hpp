//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project internal openSYDE structure to fill data pool lists (header)

   Class was introduced for easier handling (and extension)
   of openSYDE data structures to fill up the data pool lists.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEDATAPOOLLISTSTRUCTURE_HPP
#define C_CIEDATAPOOLLISTSTRUCTURE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include "C_PuiSdNodeCanMessage.hpp"
#include "C_PuiSdNodeDataPoolListElement.hpp"
#include "C_OscCanMessage.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

// structure for core data pool list
class C_CieDataPoolListCoreStructure
{
public:
   // containers for openSYDE core data pool list structure
   std::vector<stw::opensyde_core::C_OscCanMessage> c_OscRxMessageData;
   std::vector<stw::opensyde_core::C_OscCanMessage> c_OscTxMessageData;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> c_OscRxSignalData;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> c_OscTxSignalData;
   // warning messages for each message element
   std::vector<QString> c_WarningMessagesPerRxMessage; // empty string means there is no warning message
   std::vector<QString> c_WarningMessagesPerTxMessage; // empty string means there is no warning message
};

class C_CieDataPoolListStructure
{
public:
   QString c_NodeName; // node name as optional parameter, empty string means there is no node name available
   C_CieDataPoolListCoreStructure c_Core;

   // subclass for additional user interface information/functionality
   class C_Ui
   {
   public:
      // containers for openSYDE Ui data pool list structure
      std::vector<stw::opensyde_gui_logic::C_PuiSdNodeCanMessage> c_UiRxMessageData;
      std::vector<stw::opensyde_gui_logic::C_PuiSdNodeCanMessage> c_UiTxMessageData;
      std::vector<stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> c_UiRxSignalData;
      std::vector<stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> c_UiTxSignalData;
   };

   C_Ui c_Ui;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
