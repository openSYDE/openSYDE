//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom properties check box (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECHXPROPERTIES_H
#define C_OGECHXPROPERTIES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeChxToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeChxProperties :
   public C_OgeChxToolTipBase
{
   Q_OBJECT

public:
   C_OgeChxProperties(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
