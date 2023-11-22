//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom spin box for numbers (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXNUMBER_HPP
#define C_OGESPXNUMBER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeSpxToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxNumber :
   public C_OgeSpxToolTipBase
{
   Q_OBJECT

public:
   C_OgeSpxNumber(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
