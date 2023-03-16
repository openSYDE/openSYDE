//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group box with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBXTOOLTIPBASE_H
#define C_OGEGBXTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGroupBox>
#include "C_OgeToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeGbxToolTipBase :
   public QGroupBox,
   public C_OgeToolTipBase
{
public:
   C_OgeGbxToolTipBase(QWidget * const opc_Parent = NULL);

protected:
   bool event(QEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
