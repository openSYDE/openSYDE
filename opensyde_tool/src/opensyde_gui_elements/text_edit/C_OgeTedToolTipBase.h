//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit field with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETEDTOOLTIPBASE_H
#define C_OGETEDTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeToolTipBase.h"
#include "C_OgeTedContextMenuBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTedToolTipBase :
   public C_OgeTedContextMenuBase,
   public C_OgeToolTipBase
{
public:
   C_OgeTedToolTipBase(QWidget * const opc_Parent = NULL);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
