//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief      Visualization of the node boundary with its filled colors (implementation)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QLinearGradient>

#include "gitypes.hpp"
#include "constants.hpp"
#include "C_GiNodeBoundary.hpp"
#include "C_Uti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     orc_Text     Text of node
   \param[in]     of64_Width   Width of node
   \param[in]     of64_Height  Height of node
   \param[in,out] opc_Parent   Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiNodeBoundary::C_GiNodeBoundary(const QString & orc_Text, const float64_t of64_Width, const float64_t of64_Height,
                                   const uint32_t ou32_SubNodesCount, QGraphicsItem * const opc_Parent) :
   QGraphicsItem(opc_Parent),
   C_GiBiSizeableItem(of64_Width, of64_Height),
   mc_Text(orc_Text),
   mpc_Shadow(NULL),
   mq_DrawBoder(false),
   mq_DrawWhiteFilter(false),
   mu32_SubNodesCount(ou32_SubNodesCount)
{
   this->setFlag(ItemIsMovable);

   // Init z order
   this->setZValue(mf64_ZORDER_INIT_NODE);

   // set default font
   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   this->SetFont(mc_STYLE_GUIDE_FONT_REGULAR_18);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiNodeBoundary::~C_GiNodeBoundary()
{
   this->mpc_Shadow = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get bounding rectangle (estimate of the area painted by the item)

   \return
   bounding rectangle
*/
//----------------------------------------------------------------------------------------------------------------------
QRectF C_GiNodeBoundary::boundingRect() const
{
   return this->GetSizeRect();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event

   Here: Custom paint operation

   \param[in]     opc_Painter    Pointer to Painter
   \param[in]     opc_Option     Style option
   \param[in]     opc_Widget     Optional widget to paint on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNodeBoundary::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                             QWidget * const opc_Widget)
{
   Q_UNUSED(opc_Option)
   Q_UNUSED(opc_Widget)

   QPen c_Pen;

   QRect c_Rect = this->boundingRect().toAlignedRect();
   int32_t s32_Width;
   const float64_t f64_HalfRectHeight = static_cast<float64_t>(c_Rect.height()) / 1.2;
   //Text should not be painted center aligned
   c_Rect.setHeight(static_cast<int32_t>(f64_HalfRectHeight));

   m_DrawBackground(opc_Painter);

   if (this->mq_DrawWhiteFilter == false)
   {
      c_Pen.setColor(mc_STYLE_GUIDE_COLOR_4);
   }
   else
   {
      c_Pen.setColor(stw::opensyde_gui_logic::C_Uti::h_ScaleColor(mc_STYLE_GUIDE_COLOR_4, 54));
   }
   opc_Painter->setPen(c_Pen);
   opc_Painter->setFont(this->mc_Font);
   s32_Width = c_Rect.width() - 20;
   opc_Painter->drawText(c_Rect, static_cast<int32_t>(Qt::AlignCenter),
                         stw::opensyde_gui_logic::C_Uti::h_AdaptStringToSize(this->mc_Text, opc_Painter->fontMetrics(),
                                                                             static_cast<float64_t>(s32_Width)));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the drawing of the rounded bounding rect with its gradient

   It will be drawn into a pixmap which is used by the paint method
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in this implementation
void C_GiNodeBoundary::Redraw(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets new text for showing

   \param[in]     orc_Text      New text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNodeBoundary::SetText(const QString & orc_Text)
{
   this->mc_Text = orc_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets new font for text

   \param[in]     orc_Font      New font
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNodeBoundary::SetFont(const QFont & orc_Font)
{
   this->mc_Font = orc_Font;

   //Convert point to pixel
   this->mc_Font.setPixelSize(this->mc_Font.pointSize());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates the drawing of the border line

   \param[in]     oq_Active       Flag for activating or deactivating the border
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNodeBoundary::SetDrawBorder(const bool oq_Active)
{
   this->mq_DrawBoder = oq_Active;

   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates the drawing of the white filter

   \param[in]     oq_Active       Flag for activating or deactivating the white filter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNodeBoundary::SetDrawWhiteFilter(const bool oq_Active)
{
   this->mq_DrawWhiteFilter = oq_Active;
   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the drawing of the rounded bounding rect with its gradient

   It will be drawn into a pixmap which is used by the paint method

   \param[in,out] opc_Painter Current painter to draw on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNodeBoundary::m_DrawBackground(QPainter * const opc_Painter) const
{
   if (opc_Painter != NULL)
   {
      QLinearGradient c_Gradient;
      const QRectF c_Rect = boundingRect();
      QRectF c_ShadowRect;
      const QRectF c_SurroundingRect = c_Rect;
      QRectF c_InsideRect;
      QRectF c_MultiNodeIndicatorRect;

      if (this->mq_DrawBoder == true)
      {
         if (this->mq_DrawWhiteFilter == false)
         {
            //Draw Surrounding Rect
            opc_Painter->setPen(Qt::NoPen);
            opc_Painter->setBrush(static_cast<QBrush>(mc_STYLE_GUIDE_COLOR_3));
            opc_Painter->drawRoundedRect(c_SurroundingRect, 12.0, 12.0);

            // Draw shadow as bigger rect than base -> smaller when surrounding rect is drawn
            c_ShadowRect.setRect(static_cast<float64_t>(c_Rect.left()) + 2.0,
                                 static_cast<float64_t>(c_Rect.top()) + 2.0,
                                 static_cast<float64_t>(c_Rect.width()) - 4.0,
                                 static_cast<float64_t>(c_Rect.height()) - 4.0);
         }
         else
         {
            // Draw shadow as bigger rect than base -> bigger when no surrounding rect
            c_ShadowRect.setRect(static_cast<float64_t>(c_Rect.left()) + 2.0,
                                 static_cast<float64_t>(c_Rect.top()) + 2.0,
                                 static_cast<float64_t>(c_Rect.width()) - 4.0,
                                 static_cast<float64_t>(c_Rect.height()) - 3.0);
         }
      }
      else
      {
         // Draw shadow as bigger rect than base -> bigger when no surrounding rect
         c_ShadowRect.setRect(static_cast<float64_t>(c_Rect.left()) + 2.0,
                              static_cast<float64_t>(c_Rect.top()) + 2.0,
                              static_cast<float64_t>(c_Rect.width()) - 4.0,
                              static_cast<float64_t>(c_Rect.height()) - 3.0);
      }
      opc_Painter->setPen(Qt::NoPen);
      opc_Painter->setBrush(static_cast<QBrush>(QColor(206, 206, 217)));
      opc_Painter->drawRoundedRect(c_ShadowRect, 10.0, 10.0);

      // Scalings depending on assumed c_Rect with  w:210, h:137.5
      //rounded rect
      c_Gradient.setColorAt(0.0, QColor(251, 251, 252));
      c_Gradient.setColorAt(0.33, QColor(251, 251, 252));
      c_Gradient.setColorAt(0.86, QColor(237, 237, 237));
      c_Gradient.setColorAt(1.0, QColor(237, 237, 237));

      c_InsideRect.setRect(static_cast<float64_t>(c_Rect.left()) + 3.5,
                           static_cast<float64_t>(c_Rect.top()) + 2.5,
                           static_cast<float64_t>(c_Rect.width()) - 7.0,
                           static_cast<float64_t>(c_Rect.height()) - 6.0);

      c_Gradient.setStart(static_cast<float64_t>(c_Rect.left()) -
                          ((7.5 * static_cast<float64_t>(c_Rect.width())) / 210.0),
                          static_cast<float64_t>(c_Rect.top()) +
                          ((27.5 * static_cast<float64_t>(c_Rect.height())) / 137.5));
      c_Gradient.setFinalStop(static_cast<float64_t>(c_Rect.right()) +
                              ((6.25 * static_cast<float64_t>(c_Rect.width())) / 210.0),
                              static_cast<float64_t>(c_Rect.bottom()) -
                              ((27.5 * static_cast<float64_t>(c_Rect.height())) / 137.5));

      opc_Painter->setBrush(c_Gradient);
      QPen c_Pen;
      if (this->mq_DrawWhiteFilter == false)
      {
         c_Pen.setColor(QColor(227, 227, 230, 255));
      }
      else
      {
         c_Pen.setColor(mc_STYLE_GUIDE_COLOR_11);
      }
      opc_Painter->setPen(c_Pen);

      opc_Painter->drawRoundedRect(c_InsideRect, 10.0, 10.0);

      //draw "multi node indicator"
      if (this->mu32_SubNodesCount > 0)
      {
         const float64_t f64_LEFT_MARGIN = 15.0;
         const float64_t f64_RIGHT_MARGIN = 20.0;
         const float64_t f64_TopMargin = static_cast<float64_t>(c_InsideRect.top() + (c_InsideRect.height() * 0.85));
         const float64_t f64_ButtomMargin =
            static_cast<float64_t>(c_InsideRect.height() - (c_InsideRect.height() * 0.94));
         const float64_t f64_FREE_SPACE_BETWEEN = 5.0;
         const float64_t f64_SpaceEachNode =
            static_cast<float64_t>(((c_InsideRect.width() - (f64_RIGHT_MARGIN - c_InsideRect.left())) -
                                    (static_cast<float64_t>(this->mu32_SubNodesCount) * f64_FREE_SPACE_BETWEEN)) /
                                   static_cast<float64_t>(this->mu32_SubNodesCount));

         opc_Painter->setPen(Qt::NoPen);
         opc_Painter->setBrush(static_cast<QBrush>(QColor(178, 178, 189)));

         for (uint8_t u8_Counter = 0; u8_Counter < this->mu32_SubNodesCount; ++u8_Counter)
         {
            const float64_t f64_ExLeft =
               static_cast<float64_t>(f64_LEFT_MARGIN + (static_cast<float64_t>(u8_Counter) * f64_FREE_SPACE_BETWEEN) +
                                      (static_cast<float64_t>(u8_Counter) * f64_SpaceEachNode));

            const float64_t f64_ExRight = f64_SpaceEachNode;

            c_MultiNodeIndicatorRect.setRect(f64_ExLeft,
                                             f64_TopMargin, //const
                                             f64_ExRight,
                                             f64_ButtomMargin); //const

            opc_Painter->drawRoundedRect(c_MultiNodeIndicatorRect,
                                         static_cast<float64_t>(c_InsideRect.height() * 0.03),
                                         static_cast<float64_t>(c_InsideRect.height() * 0.03));
         }
      }

      if (this->mq_DrawWhiteFilter == true)
      {
         // draw the white filter
         QColor c_Color = mc_STYLE_GUIDE_COLOR_0;
         QBrush c_Brush;

         // half transparent
         c_Color.setAlpha(127);
         c_Brush.setColor(c_Color);
         c_Brush.setStyle(Qt::SolidPattern);

         opc_Painter->setBrush(c_Brush);
         opc_Painter->setPen(Qt::NoPen);
         opc_Painter->drawRoundedRect(c_InsideRect, 10.0, 10.0);
      }
   }
}
