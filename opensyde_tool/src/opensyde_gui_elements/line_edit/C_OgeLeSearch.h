//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field for search (header)

   Line edit field for search (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELESEARCH_H
#define C_OGELESEARCH_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeLeContextMenuBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLeSearch :
   public C_OgeLeContextMenuBase
{
   Q_OBJECT

public:
   C_OgeLeSearch(QWidget * const opc_Parent = NULL);
   void SetDarkTheme(void);
   void SetLightTheme(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
