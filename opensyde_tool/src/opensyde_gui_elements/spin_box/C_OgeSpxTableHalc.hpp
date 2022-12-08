//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Styled spinbox for HALC parameter configuration
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXTABLEHALC_H
#define C_OGESPXTABLEHALC_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiSpinBoxGroup.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxTableHalc :
   public C_OgeWiSpinBoxGroup
{
   Q_OBJECT

public:
   explicit C_OgeSpxTableHalc(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
