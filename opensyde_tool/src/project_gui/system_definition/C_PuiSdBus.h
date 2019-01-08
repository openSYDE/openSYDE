//-----------------------------------------------------------------------------
/*!
   \file
   \brief       UI bus: stores additional UI information (header)

   See .cpp file for documentation

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISDBUS_H
#define C_PUISDBUS_H

/* -- Includes ------------------------------------------------------------- */

#include <QPointF>
#include <QColor>
#include <vector>
#include "stwtypes.h"
#include "C_PuiBsLineBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSdBus :
   public C_PuiBsLineBase
{
public:
   C_PuiSdBus();

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   QColor c_UIColorMiddleLine; ///< color of middle line ("spine") of bus
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
