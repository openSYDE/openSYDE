//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box types group with properties styling (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWISPINBOXGROUPPROPERTIES_H
#define C_OGEWISPINBOXGROUPPROPERTIES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiSpinBoxGroup.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiSpinBoxGroupProperties :
   public C_OgeWiSpinBoxGroup
{
   Q_OBJECT

public:
   explicit C_OgeWiSpinBoxGroupProperties(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
