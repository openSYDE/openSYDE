//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button with two states (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBTOGGLE_H
#define C_OGEPUBTOGGLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPushButton>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubToggle :
   public QPushButton
{
   Q_OBJECT

public:
   C_OgePubToggle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
