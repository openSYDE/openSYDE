//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for project operations (header)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_POPUTIL_H
#define C_POPUTIL_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PopUtil
{
public:
   static bool h_AskUserToContinue(QWidget * const opc_Parent, const bool oq_AllowContinueWithoutSaving = true);

private:
   C_PopUtil(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
