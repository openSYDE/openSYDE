//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Default slider element (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESLIDEFAULT_H
#define C_OGESLIDEFAULT_H

/* -- Includes ------------------------------------------------------------- */
#include <QSlider>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSliDefault :
   public QSlider
{
   Q_OBJECT

public:
   C_OgeSliDefault(QWidget * const opc_Parent);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
