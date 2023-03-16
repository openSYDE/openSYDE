//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for project operations (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_POPUTIL_H
#define C_POPUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PopUtil
{
public:
   static bool h_AskUserToContinue(QWidget * const opc_Parent, const bool oq_AllowContinueWithoutSaving = true);
   static bool h_CheckCriticalNamingConflict(QWidget * const opc_Parent, const bool oq_DisableActiveWaitCursor);

private:
   C_PopUtil(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
