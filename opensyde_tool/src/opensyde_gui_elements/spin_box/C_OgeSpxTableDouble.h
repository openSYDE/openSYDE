//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom double spin box for table (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXTABLEDOUBLE_H
#define C_OGESPXTABLEDOUBLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeSpxDoubleToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxTableDouble :
   public C_OgeSpxDoubleToolTipBase
{
   Q_OBJECT

public:
   C_OgeSpxTableDouble(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
