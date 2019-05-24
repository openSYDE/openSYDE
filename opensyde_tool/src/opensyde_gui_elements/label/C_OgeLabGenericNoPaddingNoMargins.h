//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic label styled via setters (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABGENERICNOPADDINGNOMARGINS_H
#define C_OGELABGENERICNOPADDINGNOMARGINS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLabToolTipBase.h"

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabGenericNoPaddingNoMargins :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabGenericNoPaddingNoMargins(QWidget * const opc_Parent = NULL);

   void SetForegroundColor(const stw_types::sintn osn_Value);
   void SetBackgroundColor(const stw_types::sintn osn_Value);
   void SetFontPixel(const stw_types::sintn osn_Value, const bool oq_SemiBold = false, const bool oq_Bold = false);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
