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
#include "C_OgeSpxDoubleAutoFix.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxDoubleAutoFixCustomTrigger :
   public C_OgeSpxDoubleAutoFix
{
   Q_OBJECT

public:
   C_OgeSpxDoubleAutoFixCustomTrigger(QWidget * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void stepBy(const stw_types::sintn osn_Steps) override;
   //lint -restore

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigValueChanged(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
