//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Bold heading for group box (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.05.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABTOOLBOXHEADINGGROUPBOLD_H
#define C_OGELABTOOLBOXHEADINGGROUPBOLD_H

/* -- Includes ------------------------------------------------------------- */
#include <QLabel>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabToolboxHeadingGroupBold :
   public QLabel
{
   Q_OBJECT

public:
   explicit C_OgeLabToolboxHeadingGroupBold(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
