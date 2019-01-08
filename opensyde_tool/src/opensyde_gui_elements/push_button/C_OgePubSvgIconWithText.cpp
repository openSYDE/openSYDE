//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Button with  text and SVG item, drawn manually (implementation)

   Button with  text and SVG item, drawn manually

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.10.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPainter>

#include "C_Uti.h"
#include "stwtypes.h"
#include "constants.h"
#include "C_OgePubSvgIconWithText.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     17.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgePubSvgIconWithText::C_OgePubSvgIconWithText(QWidget * const opc_Parent) :
   C_OgePubSvgIconOnly(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Override paint event

   Draws the element

   \param[in,out] opc_Event  Pointer to paint event

   \created     10.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgePubSvgIconWithText::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)
   if (this->mpc_SvgRendererEnabled != NULL)
   {
      const sint32 s32_MarginLeft = 20;
      const sint32 s32_MarginInBetween = 10;
      QTextOption c_Option;
      QColor c_BackgroundColor;
      QColor c_TextColor;
      //Sub rectangles
      const QRect c_IconRect(this->rect().topLeft() + QPoint(s32_MarginLeft,
                                                             (this->rect().height() - this->iconSize().height()) / 2),
                             this->iconSize());
      const QRect c_TextRect(this->rect().topLeft() + QPoint(
                                static_cast<sintn>(s32_MarginLeft) + this->iconSize().width() + static_cast<sintn>(s32_MarginInBetween), 0), QSize(
                                this->rect().width(), this->rect().height()));
      QPainter c_Painter(this);
      //Background
      //==========
      if (this->isEnabled() == true)
      {
         if (this->isDown() == true)
         {
            c_BackgroundColor = mc_STYLE_GUIDE_COLOR_10;
         }
         else if (this->mq_Hovered == true)
         {
            c_BackgroundColor = mc_STYLE_GUIDE_COLOR_11;
         }
         else
         {
            c_BackgroundColor = Qt::transparent;
         }
      }
      else
      {
         c_BackgroundColor = Qt::transparent;
      }
      c_Painter.setPen(Qt::NoPen);
      c_Painter.setBrush(c_BackgroundColor);
      c_Painter.drawRect(this->rect());
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
         c_TextColor = mc_STYLE_GUIDE_COLOR_4;
      }
      else
      {
         c_TextColor = mc_STYLE_GUIDE_COLOR_9;
      }
      c_Painter.setBrush(Qt::NoBrush);
      c_Painter.setPen(c_TextColor);
      c_Painter.setFont(C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_SEMIBOLD_13));
      c_Option.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
      c_Painter.drawText(c_TextRect, this->text(), c_Option);
   }
}
