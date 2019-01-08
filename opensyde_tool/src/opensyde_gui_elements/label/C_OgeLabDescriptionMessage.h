//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for description of custom message box.

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.06.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABDESCRIPTIONMESSAGE_H
#define C_OGELABDESCRIPTIONMESSAGE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeLabContextMenuBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabDescriptionMessage :
   public C_OgeLabContextMenuBase
{
   Q_OBJECT

public:
   explicit C_OgeLabDescriptionMessage(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
