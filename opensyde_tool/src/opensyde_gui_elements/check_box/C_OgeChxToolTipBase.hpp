//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Check box with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECHXTOOLTIPBASE_H
#define C_OGECHXTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QCheckBox>
#include "C_OgeToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeChxToolTipBase :
   public QCheckBox,
   public C_OgeToolTipBase
{
public:
   C_OgeChxToolTipBase(QWidget * const opc_Parent = NULL);

protected:
   bool event(QEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
