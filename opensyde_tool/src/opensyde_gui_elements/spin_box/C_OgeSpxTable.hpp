//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom spin box for table (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXTABLE_H
#define C_OGESPXTABLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeSpxToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxTable :
   public C_OgeSpxToolTipBase
{
   Q_OBJECT

public:
   C_OgeSpxTable(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
