//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for dashboard progress bar item value, scaled to maximum size (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABDASHBOARDPROGRESSBARMAXIMUM_H
#define C_OGELABDASHBOARDPROGRESSBARMAXIMUM_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeLabAdaptiveSize.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabDashboardProgressBarMaximum :
   public C_OgeLabAdaptiveSize
{
   Q_OBJECT

public:
   C_OgeLabDashboardProgressBarMaximum(QWidget * const opc_Parent = NULL);

   virtual void AdjustFontToSize(void) override;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
