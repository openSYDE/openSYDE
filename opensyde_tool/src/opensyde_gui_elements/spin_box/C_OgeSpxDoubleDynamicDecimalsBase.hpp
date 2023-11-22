//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Double spin box with dynamic decimal input (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXDOUBLEDYNAMICDECIMALSBASE_HPP
#define C_OGESPXDOUBLEDYNAMICDECIMALSBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QDoubleSpinBox>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxDoubleDynamicDecimalsBase :
   public QDoubleSpinBox
{
   Q_OBJECT

public:
   C_OgeSpxDoubleDynamicDecimalsBase(QWidget * const opc_Parent = NULL);

   QString textFromValue(const float64_t of64_Value) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
