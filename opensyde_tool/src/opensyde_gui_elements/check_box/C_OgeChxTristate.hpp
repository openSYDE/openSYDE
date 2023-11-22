//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom tristate check box (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECHXTRISTATE_HPP
#define C_OGECHXTRISTATE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeChxTristateBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeChxTristate :
   public C_OgeChxTristateBase
{
   Q_OBJECT

public:
   C_OgeChxTristate(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
