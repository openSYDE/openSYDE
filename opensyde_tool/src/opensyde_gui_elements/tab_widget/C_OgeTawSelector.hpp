//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tab widget for selecting modes (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETAWSELECTOR_H
#define C_OGETAWSELECTOR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeTawToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTawSelector :
   public C_OgeTawToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgeTawSelector(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
