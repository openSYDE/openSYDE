//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for status bar (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGELABSTATUSBAR_H
#define C_CAMOGELABSTATUSBAR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLabToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeLabStatusBar :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgeLabStatusBar(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
