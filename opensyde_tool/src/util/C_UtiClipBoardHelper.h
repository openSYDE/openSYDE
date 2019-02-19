//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Handle clipboard load and save (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_UTICLIPBOARDHELPER_H
#define C_UTICLIPBOARDHELPER_H

/* -- Includes ------------------------------------------------------------- */
#include <QString>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_UtiClipBoardHelper
{
protected:
   C_UtiClipBoardHelper(void);

   static QString mh_GetClipBoard(void);
   static void mh_SetClipBoard(const QString & orc_Text);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
