//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic button with text and background color
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBGENERICTEXTWITHBACKGROUND_H
#define C_OGEPUBGENERICTEXTWITHBACKGROUND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.h"

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubGenericTextWithBackground :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgePubGenericTextWithBackground(QWidget * const opc_Parent = NULL);

   void SetForegroundColor(const stw_types::sintn osn_Value);
   void SetBackgroundColor(const stw_types::sintn osn_Value);
   void SetCheckedBackgroundColor(const stw_types::sintn osn_Value);
   void SetFontPixel(const stw_types::sintn osn_Value, const bool oq_SemiBold = false, const bool oq_Bold = false);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
