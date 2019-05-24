//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Double spin box field for properties (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXDOUBLEPROPERTIES_H
#define C_OGESPXDOUBLEPROPERTIES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeSpxDoubleToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxDoubleProperties :
   public C_OgeSpxDoubleToolTipBase
{
   Q_OBJECT

public:
   C_OgeSpxDoubleProperties(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
