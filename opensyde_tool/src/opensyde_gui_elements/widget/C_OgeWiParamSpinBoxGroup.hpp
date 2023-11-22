//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for param widget spin boxes (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIPARAMSPINBOXGROUP_HPP
#define C_OGEWIPARAMSPINBOXGROUP_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiSpinBoxGroup.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiParamSpinBoxGroup :
   public C_OgeWiSpinBoxGroup
{
   Q_OBJECT

public:
   explicit C_OgeWiParamSpinBoxGroup(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
