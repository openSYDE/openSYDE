//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button with two states bold (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBTOGGLEBOLD_H
#define C_OGEPUBTOGGLEBOLD_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPushButton>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubToggleBold :
   public QPushButton
{
   Q_OBJECT

public:
   C_OgePubToggleBold(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
