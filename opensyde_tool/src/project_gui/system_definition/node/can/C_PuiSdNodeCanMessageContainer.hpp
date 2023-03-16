//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage container for all CAN message types UI part (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDNODECANMESSAGECONTAINER_H
#define C_PUISDNODECANMESSAGECONTAINER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "C_PuiSdNodeCanMessage.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeCanMessageContainer
{
public:
   C_PuiSdNodeCanMessageContainer(void);

   void CalcHash(uint32_t & oru32_HashValue) const;
   const std::vector<C_PuiSdNodeCanMessage> & GetMessagesConst(const bool & orq_IsTx) const;
   std::vector<C_PuiSdNodeCanMessage> & GetMessages(const bool & orq_IsTx);

   std::vector<C_PuiSdNodeCanMessage> c_TxMessages;
   std::vector<C_PuiSdNodeCanMessage> c_RxMessages;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
