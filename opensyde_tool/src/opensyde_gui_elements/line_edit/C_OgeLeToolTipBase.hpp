//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELETOOLTIPBASE_HPP
#define C_OGELETOOLTIPBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeToolTipBase.hpp"
#include "C_OgeLeContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeToolTipBase :
   public C_OgeLeContextMenuBase,
   public C_OgeToolTipBase
{
public:
   C_OgeLeToolTipBase(QWidget * const opc_Parent = NULL);

   bool event(QEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
