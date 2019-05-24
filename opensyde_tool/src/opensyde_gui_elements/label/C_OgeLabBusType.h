//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for bus type (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABBUSTYPE_H
#define C_OGELABBUSTYPE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabBusType :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabBusType(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
