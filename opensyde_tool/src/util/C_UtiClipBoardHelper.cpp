//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Handle clipboard load and save (implementation)

   Handle clipboard load and save

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QApplication>
#include <QClipboard>
#include "C_UtiClipBoardHelper.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_UtiClipBoardHelper::C_UtiClipBoardHelper(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current clipboard content

   \return
   Current clip board content

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_UtiClipBoardHelper::mh_GetClipBoard(void)
{
   QString c_Retval;
   QClipboard * const pc_Clipboard = QApplication::clipboard();

   if (pc_Clipboard != NULL)
   {
      //Copy from clipboard
      c_Retval = pc_Clipboard->text();
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set current clipboard content

   \param[in] orc_Text New clipboard text

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_UtiClipBoardHelper::mh_SetClipBoard(const QString & orc_Text)
{
   QClipboard * const pc_Clipboard = QApplication::clipboard();

   if (pc_Clipboard != NULL)
   {
      //Copy to clipboard
      pc_Clipboard->clear();
      pc_Clipboard->setText(orc_Text);
   }
}
