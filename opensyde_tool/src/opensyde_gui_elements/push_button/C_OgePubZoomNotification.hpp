//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Current zoom level notification item (header)

   Current zoom level notification item (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBZOOMNOTIFICATION_HPP
#define C_OGEPUBZOOMNOTIFICATION_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QPushButton>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubZoomNotification :
   public QPushButton
{
   Q_OBJECT

public:
   C_OgePubZoomNotification(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
