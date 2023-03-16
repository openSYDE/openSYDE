//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for com list header (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABCOMLISTHEADER_H
#define C_OGELABCOMLISTHEADER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeLabToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabComListHeader :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabComListHeader(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
