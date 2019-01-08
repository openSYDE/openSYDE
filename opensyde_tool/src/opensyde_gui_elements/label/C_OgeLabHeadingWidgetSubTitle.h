//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for widget heading sub title (header)

   Label for widget heading sub title (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABHEADINGWIDGETSUBTITLE_H
#define C_OGELABHEADINGWIDGETSUBTITLE_H

/* -- Includes ------------------------------------------------------------- */

#include <QLabel>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabHeadingWidgetSubTitle :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabHeadingWidgetSubTitle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
