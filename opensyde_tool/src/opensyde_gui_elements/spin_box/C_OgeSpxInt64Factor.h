//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Int64 spin box which should avoid a zero value (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXINT64FACTOR_H
#define C_OGESPXINT64FACTOR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeSpxInt64ToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxInt64Factor :
   public C_OgeSpxInt64ToolTipBase
{
public:
   C_OgeSpxInt64Factor(QWidget * const opc_Parent = NULL, const bool & orq_IsUnsigned = false);

   void InterpretValue(void);
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void stepBy(const stw_types::sintn osn_Steps) override;
   virtual QValidator::State validate(QString & orc_Text, stw_types::sintn & orsn_Pos) const override;
   //lint -restore
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
