//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Default label for dashboard, not scaling (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABDASHBOARDDEFAULTNONSCALING_H
#define C_OGELABDASHBOARDDEFAULTNONSCALING_H

/* -- Includes ------------------------------------------------------------- */
#include <QLabel>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabDashboardDefaultNonScaling :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabDashboardDefaultNonScaling(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
