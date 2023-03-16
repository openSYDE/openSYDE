//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle clipboard load and save (implementation)

   Handle clipboard load and save

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QApplication>
#include <QClipboard>
#include "C_UtiClipBoardHelper.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_UtiClipBoardHelper::C_UtiClipBoardHelper(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current clipboard content

   \return
   Current clip board content
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current clipboard content

   \param[in] orc_Text New clipboard text
*/
//----------------------------------------------------------------------------------------------------------------------
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
