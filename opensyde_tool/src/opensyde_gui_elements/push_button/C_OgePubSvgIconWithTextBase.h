//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for SVG push buttons with texts (set members as interface)
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBSVGICONWITHTEXTBASE_H
#define C_OGEPUBSVGICONWITHTEXTBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OgePubSvgIconOnly.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubSvgIconWithTextBase :
   public C_OgePubSvgIconOnly
{
public:
   C_OgePubSvgIconWithTextBase(QWidget * const opc_Parent = NULL);
   void SetMargins(const stw_types::sint32 os32_MarginLeft, const stw_types::sint32 os32_MarginInBetween);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

   stw_types::sintn msn_RightBorderEnabledWidth;
   stw_types::sintn msn_TopBorderEnabledWidth;
   stw_types::sintn msn_LeftBorderEnabledWidth;
   stw_types::sintn msn_BottomBorderEnabledWidth;
   stw_types::sintn msn_RightBorderDisabledWidth;
   stw_types::sintn msn_TopBorderDisabledWidth;
   stw_types::sintn msn_LeftBorderDisabledWidth;
   stw_types::sintn msn_BottomBorderDisabledWidth;
   QColor mc_BorderColorEnabledLeft;
   QColor mc_BorderColorEnabledTop;
   QColor mc_BorderColorEnabledRight;
   QColor mc_BorderColorEnabledBottom;
   QColor mc_BorderColorDisabledLeft;
   QColor mc_BorderColorDisabledTop;
   QColor mc_BorderColorDisabledRight;
   QColor mc_BorderColorDisabledBottom;
   QColor mc_TextColorEnabled;
   QColor mc_TextColorDisabled;
   QColor mc_BackgroundColorDisabled;
   QColor mc_BackgroundColorEnabledDefault;
   QColor mc_BackgroundColorEnabledHover;
   QColor mc_BackgroundColorEnabledPressed;
   QFont mc_FontPixel;

private:
   stw_types::sint32 ms32_MarginLeft;
   stw_types::sint32 ms32_MarginInBetween;

   QRect GetInnerRect(void) const;
   static void h_DrawRectBorders(const QRect & orc_RectOuter, QPainter & orc_Painter,
                                 const stw_types::sintn osn_LeftBorderWidth, const stw_types::sintn osn_TopBorderWidth,
                                 const stw_types::sintn osn_RightBorderWidth,
                                 const stw_types::sintn osn_BottomBorderWidth, const QColor & orc_BorderColorLeft,
                                 const QColor & orc_BorderColorTop, const QColor & orc_BorderColorRight,
                                 const QColor & orc_BorderColorBottom);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
