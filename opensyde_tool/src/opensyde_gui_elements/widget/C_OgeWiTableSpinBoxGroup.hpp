//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for table spin boxes (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWITABLESPINBOXGROUP_H
#define C_OGEWITABLESPINBOXGROUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiSpinBoxGroup.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiTableSpinBoxGroup :
   public C_OgeWiSpinBoxGroup
{
   Q_OBJECT

public:
   C_OgeWiTableSpinBoxGroup(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
