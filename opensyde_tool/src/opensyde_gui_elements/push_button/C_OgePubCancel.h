//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standard cancel button (header)

   Standard cancel button (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUSHBUTTONCANCEL_H
#define C_OGEPUSHBUTTONCANCEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QPushButton>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubCancel :
   public QPushButton
{
   Q_OBJECT

public:
   C_OgePubCancel(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
