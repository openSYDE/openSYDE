//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic line edit for table edit style for can monitor (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMLETABLEEDIT_H
#define C_CAMLETABLEEDIT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblEditLineEditBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeLeTableEdit :
   public stw_opensyde_gui::C_TblEditLineEditBase
{
   Q_OBJECT

public:
   C_CamOgeLeTableEdit(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
