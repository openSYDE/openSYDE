//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Simple transparent, text only button (header)

   Simple transparent, underlined text only button (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBTEXTONLYUNDERLINE_H
#define C_OGEPUBTEXTONLYUNDERLINE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QPushButton>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubTextOnlyUnderline :
   public QPushButton
{
   Q_OBJECT

public:
   C_OgePubTextOnlyUnderline(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
