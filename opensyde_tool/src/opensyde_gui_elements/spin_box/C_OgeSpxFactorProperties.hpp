//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Factor spin box for properties dialog (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXFACTORPROPERTIES_HPP
#define C_OGESPXFACTORPROPERTIES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeSpxFactor.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxFactorProperties :
   public C_OgeSpxFactor
{
   Q_OBJECT

public:
   C_OgeSpxFactorProperties(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
