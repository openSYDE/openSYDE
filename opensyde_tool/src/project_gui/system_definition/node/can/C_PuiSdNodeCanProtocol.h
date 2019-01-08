//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for a node CAN communication via a specific CAN communication protocol UI part (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISDNODECANPROTOCOL_H
#define C_PUISDNODECANPROTOCOL_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include "C_PuiSdNodeCanMessageContainer.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSdNodeCanProtocol
{
public:
   C_PuiSdNodeCanProtocol(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   std::vector<C_PuiSdNodeCanMessageContainer> c_ComMessages;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
