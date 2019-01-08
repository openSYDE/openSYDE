//-----------------------------------------------------------------------------
/*!
   \file
   \brief       UI image data: stores UI information (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.10.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUIBSIMAGE_H
#define C_PUIBSIMAGE_H

/* -- Includes ------------------------------------------------------------- */
#include <QPixmap>

#include "C_PuiBsBox.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiBsImage :
   public C_PuiBsBox
{
public:
   C_PuiBsImage();

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   QPixmap c_UIImagePixmap;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
