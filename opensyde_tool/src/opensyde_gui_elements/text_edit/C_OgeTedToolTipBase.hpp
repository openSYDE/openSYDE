//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit field with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETEDTOOLTIPBASE_HPP
#define C_OGETEDTOOLTIPBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeToolTipBase.hpp"
#include "C_OgeTedContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
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
   bool event(QEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
