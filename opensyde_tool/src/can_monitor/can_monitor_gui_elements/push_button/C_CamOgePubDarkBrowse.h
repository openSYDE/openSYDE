//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Push button with left border styled for seamless transition to browse-line-edit. (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.01.2019  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMOGEPUBDARKBROWSE_H
#define C_CAMOGEPUBDARKBROWSE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgePubToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamOgePubDarkBrowse :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgePubDarkBrowse(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
