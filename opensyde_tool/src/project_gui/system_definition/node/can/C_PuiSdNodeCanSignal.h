//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for a signal positioning in a CAN message UI part (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISDNODECANSIGNAL_H
#define C_PUISDNODECANSIGNAL_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSdNodeCanSignal
{
public:
   C_PuiSdNodeCanSignal(void);

   stw_types::uint8 u8_ColorIndex;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
