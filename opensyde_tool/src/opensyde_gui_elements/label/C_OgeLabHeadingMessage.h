//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for heading of custom message box (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.04.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABHEADINGMESSAGE_H
#define C_OGELABHEADINGMESSAGE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeLabContextMenuBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabHeadingMessage :
   public C_OgeLabContextMenuBase
{
   Q_OBJECT

public:
   C_OgeLabHeadingMessage(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
