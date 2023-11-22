//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for progress log item (header)

   Label for progress log item (note: main module description should be in .cpp file)

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABPROGRESSLOGITEM_HPP
#define C_OGELABPROGRESSLOGITEM_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLabToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabProgressLogItem :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabProgressLogItem(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
