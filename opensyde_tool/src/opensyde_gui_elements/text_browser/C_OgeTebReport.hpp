//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text browser field for report widgets (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETEBREPORT_HPP
#define C_OGETEBREPORT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTebContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTebReport :
   public C_OgeTebContextMenuBase
{
   Q_OBJECT

public:
   explicit C_OgeTebReport(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
