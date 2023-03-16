//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box which should avoid a zero value (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXFACTOR_H
#define C_OGESPXFACTOR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OgeSpxDoubleToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxFactor :
   public C_OgeSpxDoubleToolTipBase
{
   Q_OBJECT

public:
   C_OgeSpxFactor(QWidget * const opc_Parent = NULL);
   void stepBy(const int32_t os32_Steps) override;
   QValidator::State validate(QString & orc_Text, int32_t & ors32_Pos) const override;

   static const float64_t mhf64_FACTOR_MIN;

protected:
   StepEnabled stepEnabled(void) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
