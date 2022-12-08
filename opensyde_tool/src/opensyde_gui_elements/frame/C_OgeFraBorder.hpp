//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom border draw frame (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEFRABORDER_H
#define C_OGEFRABORDER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QFrame>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeFraBorder :
   public QFrame
{
   Q_OBJECT

public:
   C_OgeFraBorder(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
