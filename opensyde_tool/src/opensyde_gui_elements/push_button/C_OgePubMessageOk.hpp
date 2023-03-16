//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom message box ok button with more tiny look than standard ok button.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBMESSAGEOK_H
#define C_OGEPUBMESSAGEOK_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPushButton>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubMessageOk :
   public QPushButton
{
   Q_OBJECT

public:
   explicit C_OgePubMessageOk(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
