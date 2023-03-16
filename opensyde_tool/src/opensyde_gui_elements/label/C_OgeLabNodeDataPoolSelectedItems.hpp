//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for display of selected items cound in node data pool (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABNODEDATAPOOLSELECTEDITEMS_H
#define C_OGELABNODEDATAPOOLSELECTEDITEMS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabNodeDataPoolSelectedItems :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabNodeDataPoolSelectedItems(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
