//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Push button for about dialog (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.11.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMOGEPUBABOUT_H
#define C_CAMOGEPUBABOUT_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgePubToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamOgePubAbout :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgePubAbout(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
