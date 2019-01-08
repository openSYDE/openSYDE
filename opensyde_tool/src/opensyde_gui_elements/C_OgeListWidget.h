//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom list widget (header)

   Custom list widget (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     28.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELISTWIDGET_H
#define C_OGELISTWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QListWidget>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeListWidget :
   public QListWidget
{
   Q_OBJECT

public:
   C_OgeListWidget(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
