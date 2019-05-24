//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for status information (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABSTATEINFO_H
#define C_OGELABSTATEINFO_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLabToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabStateInfo :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabStateInfo(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
