//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom label for pop up sub title (header)

   Custom label for pop up sub title (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABPOPUPSUBTITLE_H
#define C_OGELABPOPUPSUBTITLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeLabPopUpTitle.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabPopUpSubTitle :
   public C_OgeLabPopUpTitle
{
   Q_OBJECT

public:
   C_OgeLabPopUpSubTitle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
