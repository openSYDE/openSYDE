//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standard open button (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBOPEN_H
#define C_OGEPUBOPEN_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.h"
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubOpen :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   C_OgePubOpen(QWidget * const opc_Parent = NULL);

   void SetBackgroundColor(const stw_types::sintn osn_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
