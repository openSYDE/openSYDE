//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text browser field for progress log (header)

   See cpp file for detailed description

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLAOGETEBPROGRESSLOG_HPP
#define C_FLAOGETEBPROGRESSLOG_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTebContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaOgeTebProgressLog :
   public C_OgeTebContextMenuBase
{
   Q_OBJECT

public:
   C_FlaOgeTebProgressLog(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
