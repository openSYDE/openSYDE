//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Radio button with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGERABTOOLTIPBASE_HPP
#define C_OGERABTOOLTIPBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QRadioButton>
#include "C_OgeToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeRabToolTipBase :
   public QRadioButton,
   public C_OgeToolTipBase
{
public:
   C_OgeRabToolTipBase(QWidget * const opc_Parent = NULL);

protected:
   bool event(QEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
