//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for title heading (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.08.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABHEADINGWIDGETTITLE_H
#define C_OGELABHEADINGWIDGETTITLE_H

/* -- Includes ------------------------------------------------------------- */
#include <QLabel>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabHeadingWidgetTitle :
   public QLabel
{
   Q_OBJECT

public:
   explicit C_OgeLabHeadingWidgetTitle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
