//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle custom trigger for custom spin box
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXDOUBLEAUTOFIXCUSTOMTRIGGER_H
#define C_OGESPXDOUBLEAUTOFIXCUSTOMTRIGGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeSpxDoubleAutoFix.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxDoubleAutoFixCustomTrigger :
   public C_OgeSpxDoubleAutoFix
{
   Q_OBJECT

public:
   C_OgeSpxDoubleAutoFixCustomTrigger(QWidget * const opc_Parent = NULL);

   void SetSuffix(const QString & orc_Suffix);
   void stepBy(const int32_t os32_Steps) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigValueChanged(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
