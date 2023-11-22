//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Int64 spin box which should avoid a zero value (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXINT64FACTOR_HPP
#define C_OGESPXINT64FACTOR_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeSpxInt64ToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxInt64Factor :
   public C_OgeSpxInt64ToolTipBase
{
public:
   C_OgeSpxInt64Factor(QWidget * const opc_Parent = NULL, const bool & orq_IsUnsigned = false);

   void InterpretValue(void);

   void stepBy(const int32_t os32_Steps) override;
   QValidator::State validate(QString & orc_Text, int32_t & ors32_Pos) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
