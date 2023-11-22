//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic button with text and background color
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBGENERICTEXTWITHBACKGROUND_HPP
#define C_OGEPUBGENERICTEXTWITHBACKGROUND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.hpp"

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubGenericTextWithBackground :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgePubGenericTextWithBackground(QWidget * const opc_Parent = NULL);

   void SetForegroundColor(const int32_t os32_Value);
   void SetBackgroundColor(const int32_t os32_Value);
   void SetCheckedBackgroundColor(const int32_t os32_Value);
   void SetFontPixel(const int32_t os32_Value, const bool oq_SemiBold = false, const bool oq_Bold = false);
   void SetBorderColor(const int32_t os32_Value);
   void SetClickedColor(const int32_t os32_ValueClicked, const int32_t os32_ValueHovered);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
