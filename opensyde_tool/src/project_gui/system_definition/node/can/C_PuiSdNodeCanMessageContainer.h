//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Storage container for all CAN message types UI part (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISDNODECANMESSAGECONTAINER_H
#define C_PUISDNODECANMESSAGECONTAINER_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include "C_PuiSdNodeCanMessage.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSdNodeCanMessageContainer
{
public:
   C_PuiSdNodeCanMessageContainer(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   const std::vector<C_PuiSdNodeCanMessage> & GetMessagesConst(const bool & orq_IsTx) const;
   std::vector<C_PuiSdNodeCanMessage> & GetMessages(const bool & orq_IsTx);

   std::vector<C_PuiSdNodeCanMessage> c_TxMessages;
   std::vector<C_PuiSdNodeCanMessage> c_RxMessages;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
