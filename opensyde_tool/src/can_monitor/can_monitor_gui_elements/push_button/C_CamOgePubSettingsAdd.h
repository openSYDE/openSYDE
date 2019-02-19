//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Push button for "Add Filter" and "Add Database". (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.11.2018  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMOGEPUBSETTINGSADD_H
#define C_CAMOGEPUBSETTINGSADD_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgePubToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamOgePubSettingsAdd :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgePubSettingsAdd(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
