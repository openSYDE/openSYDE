//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom default check box with a disabled checked state (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECHXDEFAULTCHECKEDDISABLED_H
#define C_OGECHXDEFAULTCHECKEDDISABLED_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeChxToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeChxDefaultCheckedDisabled :
   public C_OgeChxToolTipBase
{
   Q_OBJECT

public:
   C_OgeChxDefaultCheckedDisabled(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
