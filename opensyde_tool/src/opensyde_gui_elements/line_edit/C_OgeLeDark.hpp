//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit for dark background (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELEDARK_HPP
#define C_OGELEDARK_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLeContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeDark :
   public C_OgeLeContextMenuBase
{
   Q_OBJECT

public:
   explicit C_OgeLeDark(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
