//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Double spin box with dynamic decimal input (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXDOUBLEDYNAMICDECIMALSBASE_H
#define C_OGESPXDOUBLEDYNAMICDECIMALSBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QDoubleSpinBox>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxDoubleDynamicDecimalsBase :
   public QDoubleSpinBox
{
   Q_OBJECT

public:
   C_OgeSpxDoubleDynamicDecimalsBase(QWidget * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QString textFromValue(const stw_types::float64 of64_Value) const override;
   //lint -restore
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
