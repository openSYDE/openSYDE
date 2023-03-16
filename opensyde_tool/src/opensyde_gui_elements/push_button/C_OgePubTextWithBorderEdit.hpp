//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Simple transparent, text button with border for edit mode (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBTEXTWITHBORDEREDIT_H
#define C_OGEPUBTEXTWITHBORDEREDIT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPushButton>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubTextWithBorderEdit :
   public QPushButton
{
   Q_OBJECT

public:
   C_OgePubTextWithBorderEdit(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
