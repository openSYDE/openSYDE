//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom label for pop up sub title (header)

   Custom label for pop up sub title (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     28.10.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABPOPUPSUBTITLE_H
#define C_OGELABPOPUPSUBTITLE_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeLabPopUpTitle.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabPopUpSubTitle :
   public C_OgeLabPopUpTitle
{
   Q_OBJECT

public:
   C_OgeLabPopUpSubTitle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
