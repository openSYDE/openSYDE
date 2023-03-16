//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom message box cancel button with more tiny look than standard cancel button.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBMESSAGECANCEL_H
#define C_OGEPUBMESSAGECANCEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPushButton>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubMessageCancel :
   public QPushButton
{
   Q_OBJECT

public:
   explicit C_OgePubMessageCancel(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
