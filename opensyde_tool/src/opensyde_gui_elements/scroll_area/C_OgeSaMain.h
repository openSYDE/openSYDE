//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Scroll area for main widget (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     28.03.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESAMAIN_H
#define C_OGESAMAIN_H

/* -- Includes ------------------------------------------------------------- */
#include <QScrollArea>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSaMain :
   public QScrollArea
{
   Q_OBJECT

public:
   C_OgeSaMain(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
