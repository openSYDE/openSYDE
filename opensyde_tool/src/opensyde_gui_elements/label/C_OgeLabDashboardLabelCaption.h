//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for dashboard label item caption (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABDASHBOARDLABELCAPTION_H
#define C_OGELABDASHBOARDLABELCAPTION_H

/* -- Includes ------------------------------------------------------------- */
#include <QLabel>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabDashboardLabelCaption :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabDashboardLabelCaption(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
