//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit for message ID in hex or integer format, styled for pop up dialog. (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGELEIDPOPUP_H
#define C_CAMOGELEIDPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblEditLineEditBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeLeIDPopUp :
   public stw_opensyde_gui::C_TblEditLineEditBase
{
   Q_OBJECT

public:
   explicit C_CamOgeLeIDPopUp(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
