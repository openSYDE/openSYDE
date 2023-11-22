//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle clipboard load and save (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_UTICLIPBOARDHELPER_HPP
#define C_UTICLIPBOARDHELPER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UtiClipBoardHelper
{
protected:
   C_UtiClipBoardHelper(void);

   static QString mh_GetClipBoard(void);
   static void mh_SetClipBoard(const QString & orc_Text);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
