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
#include "stwtypes.hpp"
#include "C_OgePubSvgIconOnly.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubSvgIconWithTextBase :
   public C_OgePubSvgIconOnly
{
public:
   C_OgePubSvgIconWithTextBase(QWidget * const opc_Parent = NULL);
   void SetMargins(const int32_t os32_MarginLeft, const int32_t os32_MarginInBetween);
   void SetDarkMode(const bool oq_Active);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

   int32_t ms32_RightBorderEnabledWidth;
   int32_t ms32_TopBorderEnabledWidth;
   int32_t ms32_LeftBorderEnabledWidth;
   int32_t ms32_BottomBorderEnabledWidth;
   int32_t ms32_RightBorderDisabledWidth;
   int32_t ms32_TopBorderDisabledWidth;
   int32_t ms32_LeftBorderDisabledWidth;
   int32_t ms32_BottomBorderDisabledWidth;
   bool mq_IconLeft;
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
   QColor mc_TextColorDarkMode;
   QColor mc_BackgroundColorDisabled;
   QColor mc_BackgroundColorEnabledDefault;
   QColor mc_BackgroundColorEnabledHover;
   QColor mc_BackgroundColorEnabledPressed;
   QFont mc_FontPixel;

private:
   int32_t ms32_MarginLeft;
   int32_t ms32_MarginInBetween;
   bool mq_Dark;

   static void mh_DrawRectBorders(const QRect & orc_RectOuter, QPainter & orc_Painter,
                                  const int32_t os32_LeftBorderWidth, const int32_t os32_TopBorderWidth,
                                  const int32_t os32_RightBorderWidth, const int32_t os32_BottomBorderWidth,
                                  const QColor & orc_BorderColorLeft, const QColor & orc_BorderColorTop,
                                  const QColor & orc_BorderColorRight, const QColor & orc_BorderColorBottom);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
