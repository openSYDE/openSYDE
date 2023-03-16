//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Checkbox for setting widgets. (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGECHXSETTINGS_H
#define C_CAMOGECHXSETTINGS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeChxToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeChxSettings :
   public C_OgeChxToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgeChxSettings(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
