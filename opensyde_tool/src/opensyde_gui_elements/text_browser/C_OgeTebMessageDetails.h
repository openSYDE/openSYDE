//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Text browser field for message details (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.04.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETEBMESSAGEDETAILS_H
#define C_OGETEBMESSAGEDETAILS_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeTebContextMenuBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTebMessageDetails :
   public C_OgeTebContextMenuBase
{
   Q_OBJECT

public:
   C_OgeTebMessageDetails(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
