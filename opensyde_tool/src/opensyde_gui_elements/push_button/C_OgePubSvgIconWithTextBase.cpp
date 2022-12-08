//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for SVG push buttons with texts (set members as interface)

   Base class for SVG push buttons with texts (set members as interface)

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>

#include "C_OgePubSvgIconWithTextBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgePubSvgIconWithTextBase::C_OgePubSvgIconWithTextBase(QWidget * const opc_Parent) :
   C_OgePubSvgIconOnly(opc_Parent),
   ms32_RightBorderEnabledWidth(0),
   ms32_TopBorderEnabledWidth(0),
   ms32_LeftBorderEnabledWidth(0),
   ms32_BottomBorderEnabledWidth(0),
   ms32_RightBorderDisabledWidth(0),
   ms32_TopBorderDisabledWidth(0),
   ms32_LeftBorderDisabledWidth(0),
   ms32_BottomBorderDisabledWidth(0),
   mq_IconLeft(true),
   ms32_MarginLeft(0),
   ms32_MarginInBetween(0),
   mq_Dark(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set all supported margins

   \param[in] os32_MarginLeft      Margin left before icon
   \param[in] os32_MarginInBetween Margin between icon and text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubSvgIconWithTextBase::SetMargins(const int32_t os32_MarginLeft, const int32_t os32_MarginInBetween)
{
   this->ms32_MarginLeft = os32_MarginLeft;
   this->ms32_MarginInBetween = os32_MarginInBetween;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Override paint event

   Draws the element

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubSvgIconWithTextBase::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)
   if (this->mpc_SvgRendererEnabled != NULL)
   {
      QTextOption c_Option;
      QColor c_BackgroundColor;
      QColor c_TextColor;
      QPainter c_Painter(this);
      QRect c_IconRect;
      QRect c_TextRect;

      //Sub rectangles
      if (this->mq_IconLeft == true)
      {
         c_IconRect = QRect(this->rect().topLeft() +
                            QPoint(static_cast<int32_t>(this->ms32_MarginLeft),
                                   (this->rect().height() - this->iconSize().height()) / 2),
                            this->iconSize());
         c_TextRect = QRect(this->rect().topLeft() +
                            QPoint(static_cast<int32_t>(this->ms32_MarginLeft) + this->iconSize().width() +
                                   static_cast<int32_t>(this->ms32_MarginInBetween), 0),
                            QSize(this->rect().width(), this->rect().height()));
      }
      else
      {
         c_IconRect = QRect(this->rect().topRight() +
                            QPoint(-(static_cast<int32_t>(this->ms32_MarginLeft) + this->iconSize().width()),
                                   (this->rect().height() - this->iconSize().height()) / 2),
                            this->iconSize());
         c_TextRect = QRect(this->rect().topLeft(),
                            QSize(this->rect().width() - (static_cast<int32_t>(this->ms32_MarginLeft) +
                                                          this->iconSize().width() +
                                                          static_cast<int32_t>(this->ms32_MarginInBetween)),
                                  this->rect().height()));
      }
      //Background
      //==========
      if (this->isEnabled() == true)
      {
         if (this->isDown() == true)
         {
            c_BackgroundColor = this->mc_BackgroundColorEnabledPressed;
         }
         else if (this->mq_Hovered == true)
         {
            c_BackgroundColor = this->mc_BackgroundColorEnabledHover;
         }
         else
         {
            c_BackgroundColor = this->mc_BackgroundColorEnabledDefault;
         }
      }
      else
      {
         c_BackgroundColor = this->mc_BackgroundColorDisabled;
      }
      c_Painter.setPen(Qt::NoPen);
      c_Painter.setBrush(c_BackgroundColor);
      c_Painter.drawRect(this->rect());
      //Borders
      //=======
      if (this->isEnabled())
      {
         C_OgePubSvgIconWithTextBase::mh_DrawRectBorders(
            this->rect(), c_Painter, this->ms32_LeftBorderEnabledWidth, this->ms32_TopBorderEnabledWidth,
            this->ms32_RightBorderEnabledWidth, this->ms32_BottomBorderEnabledWidth, this->mc_BorderColorEnabledLeft,
            this->mc_BorderColorEnabledTop, this->mc_BorderColorEnabledRight,
            this->mc_BorderColorEnabledBottom);
      }
      else
      {
         C_OgePubSvgIconWithTextBase::mh_DrawRectBorders(
            this->rect(), c_Painter, this->ms32_LeftBorderDisabledWidth, this->ms32_TopBorderDisabledWidth,
            this->ms32_RightBorderDisabledWidth, this->ms32_BottomBorderDisabledWidth, this->mc_BorderColorDisabledLeft,
            this->mc_BorderColorDisabledTop, this->mc_BorderColorDisabledRight,
            this->mc_BorderColorDisabledBottom);
      }
      //Icon
      //====
      if (this->isChecked() == true)
      {
         if ((this->mpc_SvgRendererCheckedDisabeld != NULL) && (this->isEnabled() == false))
         {
            this->mpc_SvgRendererCheckedDisabeld->render(&c_Painter, c_IconRect);
         }
         else if ((this->mpc_SvgRendererCheckedHovered != NULL) && (this->mq_Hovered == true))
         {
            this->mpc_SvgRendererCheckedHovered->render(&c_Painter, c_IconRect);
         }
         else if (this->mpc_SvgRendererCheckedEnabled != NULL)
         {
            this->mpc_SvgRendererCheckedEnabled->render(&c_Painter, c_IconRect);
         }
         else
         {
            this->mpc_SvgRendererEnabled->render(&c_Painter, c_IconRect);
         }
      }
      else
      {
         if ((this->mpc_SvgRendererDisabeld != NULL) && (this->isEnabled() == false))
         {
            this->mpc_SvgRendererDisabeld->render(&c_Painter, c_IconRect);
         }
         else if ((this->mpc_SvgRendererHovered != NULL) && (this->mq_Hovered == true))
         {
            this->mpc_SvgRendererHovered->render(&c_Painter, c_IconRect);
         }
         else
         {
            this->mpc_SvgRendererEnabled->render(&c_Painter, c_IconRect);
         }
      }
      //Text
      //====
      if (this->isEnabled() == true)
      {
         if (this->mq_Dark == true)
         {
            c_TextColor = this->mc_TextColorDarkMode;
         }
         else
         {
            c_TextColor = this->mc_TextColorEnabled;
         }
      }
      else
      {
         c_TextColor = this->mc_TextColorDisabled;
      }
      c_Painter.setBrush(Qt::NoBrush);
      c_Painter.setPen(c_TextColor);
      c_Painter.setFont(this->mc_FontPixel);
      if (this->mq_IconLeft == true)
      {
         c_Option.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else
      {
         c_Option.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
      }
      c_Painter.drawText(c_TextRect, this->text(), c_Option);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dark flag value

   \param[in]  oq_Active   New dark flag value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubSvgIconWithTextBase::SetDarkMode(const bool oq_Active)
{
   this->mq_Dark = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Draw rectangle borders

   \param[in]     orc_RectOuter          Outer rectangle to draw borders at/in
   \param[in,out] orc_Painter            Painter to use for drawing
   \param[in]     os32_LeftBorderWidth   Left border width
   \param[in]     os32_TopBorderWidth    Top border width
   \param[in]     os32_RightBorderWidth  Right border width
   \param[in]     os32_BottomBorderWidth Bottom border width
   \param[in]     orc_BorderColorLeft    Left border color
   \param[in]     orc_BorderColorTop     Top border color
   \param[in]     orc_BorderColorRight   Right border color
   \param[in]     orc_BorderColorBottom  Bottom border color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubSvgIconWithTextBase::mh_DrawRectBorders(const QRect & orc_RectOuter, QPainter & orc_Painter,
                                                     const int32_t os32_LeftBorderWidth,
                                                     const int32_t os32_TopBorderWidth,
                                                     const int32_t os32_RightBorderWidth,
                                                     const int32_t os32_BottomBorderWidth,
                                                     const QColor & orc_BorderColorLeft,
                                                     const QColor & orc_BorderColorTop,
                                                     const QColor & orc_BorderColorRight,
                                                     const QColor & orc_BorderColorBottom)
{
   //Points for an outer rectangle (the rectangle all points are drawn in)
   //And points for an inner rectange (the rectangle the content is in)

   //for some reason drawing polygons won't fully fill in the bottom and right border
   // so the rect needs to be slightly expanded
   const QRect & rc_Outer = orc_RectOuter.adjusted(0, 0, 1, 1);
   const QRect c_Inner = rc_Outer.adjusted(os32_LeftBorderWidth, os32_TopBorderWidth, -os32_RightBorderWidth,
                                           -os32_BottomBorderWidth);

   //Left border
   if (os32_LeftBorderWidth > 0)
   {
      QPolygon c_Poly;
      c_Poly.append(rc_Outer.bottomLeft());
      c_Poly.append(rc_Outer.topLeft());
      c_Poly.append(c_Inner.topLeft());
      c_Poly.append(c_Inner.bottomLeft());
      orc_Painter.setPen(Qt::NoPen);
      orc_Painter.setBrush(orc_BorderColorLeft);
      orc_Painter.drawConvexPolygon(c_Poly);
   }
   //Top border
   if (os32_TopBorderWidth > 0)
   {
      QPolygon c_Poly;
      c_Poly.append(rc_Outer.topLeft());
      c_Poly.append(rc_Outer.topRight());
      c_Poly.append(c_Inner.topRight());
      c_Poly.append(c_Inner.topLeft());
      orc_Painter.setPen(Qt::NoPen);
      orc_Painter.setBrush(orc_BorderColorTop);
      orc_Painter.drawConvexPolygon(c_Poly);
   }
   //Right border
   if (os32_RightBorderWidth > 0)
   {
      QPolygon c_Poly;
      c_Poly.append(rc_Outer.topRight());
      c_Poly.append(rc_Outer.bottomRight());
      c_Poly.append(c_Inner.bottomRight());
      c_Poly.append(c_Inner.topRight());
      orc_Painter.setPen(Qt::NoPen);
      orc_Painter.setBrush(orc_BorderColorRight);
      orc_Painter.drawConvexPolygon(c_Poly);
   }
   //Bottom border
   if (os32_BottomBorderWidth > 0)
   {
      QPolygon c_Poly;
      c_Poly.append(rc_Outer.bottomRight());
      c_Poly.append(rc_Outer.bottomLeft());
      c_Poly.append(c_Inner.bottomLeft());
      c_Poly.append(c_Inner.bottomRight());
      orc_Painter.setPen(Qt::NoPen);
      orc_Painter.setBrush(orc_BorderColorBottom);
      orc_Painter.drawConvexPolygon(c_Poly);
   }
}
