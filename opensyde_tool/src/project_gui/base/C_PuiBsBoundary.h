//-----------------------------------------------------------------------------
/*!
   \file
   \brief       UI boundary data: stores UI information (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.10.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUIBSBOUNDARY_H
#define C_PUIBSBOUNDARY_H

/* -- Includes ------------------------------------------------------------- */
#include <QColor>

#include "stwtypes.h"
#include "C_PuiBsBox.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiBsBoundary :
   public C_PuiBsBox
{
public:
   C_PuiBsBoundary();

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   QColor c_UIBorderColorBright;        ///< color of border line in bright mode
   QColor c_UIBorderColorDark;          ///< color of border line in dark mode
   QColor c_UIBackgroundColorBright;    ///< color of background in bright mode
   QColor c_UIBackgroundColorDark;      ///< color of background in dark mode
   stw_types::sint32 s32_UIBorderWidth; ///< width of border line
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
