//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class to group multiple imported EDS/Dcf messages
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCEDSDCFIMPORTMESSAGEGROUP_HPP
#define C_OSCEDSDCFIMPORTMESSAGEGROUP_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OscCanMessage.hpp"
#include "C_OscNodeDataPoolListElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscEdsDcfImportMessageGroup
{
public:
   C_OscEdsDcfImportMessageGroup();

   std::vector<C_OscCanMessage> c_OscMessageData;
   std::vector<C_OscNodeDataPoolListElement> c_OscSignalData;
   std::vector<uint8_t> c_SignalDefaultMinMaxValuesUsed;

   void Clear(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
