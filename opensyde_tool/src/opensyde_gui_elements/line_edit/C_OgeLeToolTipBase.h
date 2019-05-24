//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELETOOLTIPBASE_H
#define C_OGELETOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeToolTipBase.h"
#include "C_OgeLeContextMenuBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeToolTipBase :
   public C_OgeLeContextMenuBase,
   public C_OgeToolTipBase
{
public:
   C_OgeLeToolTipBase(QWidget * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
