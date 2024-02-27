//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box design in dark design (header)

   See cpp file for detailed description

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLAOGESPXDARK_HPP
#define C_FLAOGESPXDARK_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeSpxToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaOgeSpxDark :
   public C_OgeSpxToolTipBase
{
   Q_OBJECT

public:
   explicit C_FlaOgeSpxDark(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
