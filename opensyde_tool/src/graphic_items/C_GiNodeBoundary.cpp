//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief      Visualization of the node boundary with its filled colors (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.08.2016  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QLinearGradient>

#include "gitypes.h"
#include "constants.h"
#include "C_GiNodeBoundary.h"
#include "C_Uti.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_types;
using namespace stw_opensyde_gui;

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

   \param[in]     orc_Text     Text of node
   \param[in]     of64_Width   Width of node
   \param[in]     of64_Height  Height of node
   \param[in,out] opc_Parent   Optional pointer to parent

   \created     22.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiNodeBoundary::C_GiNodeBoundary(const QString & orc_Text, const stw_types::float64 of64_Width,
                                   const stw_types::float64 of64_Height, QGraphicsItem * const opc_Parent) :
   QGraphicsItem(opc_Parent),
   C_GiBiSizeableItem(of64_Width, of64_Height),
   mc_Text(orc_Text),
   mpc_Shadow(NULL),
   mq_DrawBoder(false),
   mq_DrawWhiteFilter(false)
{
   this->setFlag(ItemIsMovable);

   /*
   //shadow
   this->mpc_Shadow = new QGraphicsDropShadowEffect();
   this->mpc_Shadow->setBlurRadius(1.0);
   //pc_Shadow->setColor(QColor(128, 128, 128, 180));
   this->mpc_Shadow->setColor(QColor(227, 227, 230, 255));
   this->mpc_Shadow->setOffset(-10.0, 20.0);
   this->setGraphicsEffect(this->mpc_Shadow);
   */

   this->Redraw();

   // Init z order
   this->setZValue(mf64_ZORDER_INIT_NODE);

   // set default font
   this->SetFont(mc_STYLE_GUIDE_FONT_REGULAR_18);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     19.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiNodeBoundary::~C_GiNodeBoundary()
{
   this->mpc_Shadow = NULL;
   //lint -e{1740}  no memory leak because of the parent of mpc_Shadow and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   boundingRect
            Returns an estimate of the area painted by the item

   \created     04.08.2016  STW/S.Singer
*/
//-----------------------------------------------------------------------------
QRectF C_GiNodeBoundary::boundingRect() const
{
   return this->GetSizeRect();
}

//-----------------------------------------------------------------------------
/*!
   \brief   paint


   \created     04.08.2016  STW/S.Singer
*/
//-----------------------------------------------------------------------------
void C_GiNodeBoundary::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                             QWidget * const opc_Widget)
{
   Q_UNUSED(opc_Option)
   Q_UNUSED(opc_Widget)

   QPen c_Pen;

   QRect c_Rect = this->boundingRect().toAlignedRect();
   const float64 f64_HalfRectHeight = static_cast<float64>(c_Rect.height()) / 1.2;
   //Text should not be painted center aligned
   c_Rect.setHeight(static_cast<sintn>(f64_HalfRectHeight));

   m_DrawBackground(opc_Painter);

   if (this->mq_DrawWhiteFilter == false)
   {
      c_Pen.setColor(mc_STYLE_GUIDE_COLOR_4);
   }
   else
   {
      c_Pen.setColor(stw_opensyde_gui_logic::C_Uti::h_ScaleColor(mc_STYLE_GUIDE_COLOR_4, 54));
   }
   opc_Painter->setPen(c_Pen);
   opc_Painter->setFont(this->mc_Font);
   opc_Painter->drawText(c_Rect, static_cast<sintn>(Qt::AlignCenter),
                         stw_opensyde_gui_logic::C_Uti::h_AdaptStringToSize(this->mc_Text, opc_Painter->fontMetrics(),
                                                                            static_cast<float64>(c_Rect.width())));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the drawing of the rounded bounding rect with its gradient

   It will be drawn into a pixmap which is used by the paint method

   \created     12.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiNodeBoundary::Redraw(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets new text for showing

   \param[in]     orc_Text      New text

   \created     21.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiNodeBoundary::SetText(const QString & orc_Text)
{
   this->mc_Text = orc_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets new font for text

   \param[in]     orc_Text      New font

   \created     27.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiNodeBoundary::SetFont(const QFont & orc_Font)
{
   this->mc_Font = orc_Font;

   //Convert point to pixel
   this->mc_Font.setPixelSize(this->mc_Font.pointSize());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Activates the drawing of the border line

   \param[in]     oq_Active       Flag for activating or deactivating the border

   \created     22.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiNodeBoundary::SetDrawBorder(const bool oq_Active)
{
   this->mq_DrawBoder = oq_Active;

   this->update();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Activates the drawing of the white filter

   \param[in]     oq_Active       Flag for activating or deactivating the white filter

   \created     22.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiNodeBoundary::SetDrawWhiteFilter(const bool oq_Active)
{
   this->mq_DrawWhiteFilter = oq_Active;
   this->update();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the drawing of the rounded bounding rect with its gradient

   It will be drawn into a pixmap which is used by the paint method

   \param[in,out] opc_Painter Current painter to draw on

   \created     12.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
/*void C_GiNodeBoundary::m_DrawBackground(QPainter * const opc_Painter) const
{
   if (opc_Painter != NULL)
   {
      QLinearGradient c_Gradient;
      QRectF c_Rect = boundingRect();

      //rounded rect
      c_Gradient.setColorAt(0.0, QColor(251, 251, 252));
      c_Gradient.setColorAt(0.5, QColor(251, 251, 252));
      c_Gradient.setColorAt(1.0, QColor(237, 237, 237));

      //c_Gradient.setStart(-50.0, 20.0);
      c_Gradient.setStart(-60.0, 30.0);
      c_Gradient.setFinalStop(c_Rect.width(), c_Rect.height());

      opc_Painter->setBrush(c_Gradient);
      if (this->mq_DrawBoder == false)
      {
         opc_Painter->setPen(Qt::NoPen);
      }
      else
      {
         QPen c_Pen;
         if (this->mq_DrawWhiteFilter == false)
         {
            c_Pen.setColor(mc_STYLE_GUIDE_COLOR_1);
         }
         else
         {
            c_Pen.setColor(mc_STYLE_GUIDE_COLOR_11);
         }
         opc_Painter->setPen(c_Pen);
      }

      opc_Painter->drawRoundedRect(c_Rect, 10.0, 10.0);

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
         opc_Painter->drawRoundedRect(c_Rect, 10.0, 10.0);
      }
   }
}*/
void C_GiNodeBoundary::m_DrawBackground(QPainter * const opc_Painter) const
{
   //   static sintn sn_Counter = 0;

   //   std::cout << sn_Counter << &std::endl;
   //   sn_Counter++;
   //   if (opc_Painter != NULL)
   //   {
   //      QLinearGradient c_Gradient;
   //      QRectF c_Rect = boundingRect();
   //      QRectF c_ShaddowRect;
   //      QRectF c_SurroundingRect;

   //      if (this->mq_DrawDefinitionRand == false)
   //      {
   //         if (this->mq_DrawWhiteFilter == false)
   //         {
   //            //Draw Surrounding Rect
   //            c_SurroundingRect.setRect(static_cast<float64>(c_Rect.left()) - 3.5,
   //                                      static_cast<float64>(c_Rect.top()) - 2.5,
   //                                      static_cast<float64>(c_Rect.width()) + 7.0,
   //                                      static_cast<float64>(c_Rect.height()) + 5.75);
   //            opc_Painter->setPen(Qt::NoPen);
   //            opc_Painter->setBrush(QBrush(mc_STYLE_GUIDE_COLOR_3));
   //            opc_Painter->drawRoundedRect(c_SurroundingRect, 10.0, 10.0);

   //            // Draw shaddow as bigger rect than base -> smaler when surrounding rect is drawn
   //            c_ShaddowRect.setRect(static_cast<float64>(c_Rect.left()) - 1.5,
   //                                  static_cast<float64>(c_Rect.top()) - 0.5, static_cast<float64>(c_Rect.width()) +
   // 3.0,
   //                                  static_cast<float64>(c_Rect.height()) + 1.75);
   //         }
   //         else
   //         {
   //            // Draw shaddow as bigger rect than base -> bigger when no surrounding rect
   //            c_ShaddowRect.setRect(static_cast<float64>(c_Rect.left()) - 1.5,
   //                                  static_cast<float64>(c_Rect.top()) - 0.5, static_cast<float64>(c_Rect.width()) +
   // 3.0,
   //                                  static_cast<float64>(c_Rect.height()) + 2.75);
   //         }
   //      }
   //      else
   //      {
   //         // Draw shaddow as bigger rect than base -> bigger when no surrounding rect
   //         c_ShaddowRect.setRect(static_cast<float64>(c_Rect.left()) - 1.5,
   //                               static_cast<float64>(c_Rect.top()) - 0.5, static_cast<float64>(c_Rect.width()) +
   // 3.0,
   //                               static_cast<float64>(c_Rect.height()) + 2.75);
   //      }
   //      opc_Painter->setPen(Qt::NoPen);
   //      opc_Painter->setBrush(QBrush(QColor(206, 206, 217)));
   //      opc_Painter->drawRoundedRect(c_ShaddowRect, 10.0, 10.0);

   //      // Scalings depending on assumed c_Rect with  w:210, h:137.5
   //      //rounded rect
   //      c_Gradient.setColorAt(0.0, QColor(251, 251, 252));
   //      c_Gradient.setColorAt(0.33, QColor(251, 251, 252));
   //      c_Gradient.setColorAt(0.86, QColor(237, 237, 237));
   //      c_Gradient.setColorAt(1.0, QColor(237, 237, 237));

   //      c_Gradient.setStart(static_cast<float64>(c_Rect.left()) - ((7.5 * static_cast<float64>(c_Rect.width())) /
   // 210.0),
   //                          static_cast<float64>(c_Rect.top()) +
   //                          ((27.5 * static_cast<float64>(c_Rect.height())) / 137.5));
   //      c_Gradient.setFinalStop(static_cast<float64>(c_Rect.right()) +
   //                              ((6.25 * static_cast<float64>(c_Rect.width())) / 210.0),
   //                              static_cast<float64>(c_Rect.bottom()) -
   //                              ((27.5 * static_cast<float64>(c_Rect.height())) / 137.5));

   //      opc_Painter->setBrush(c_Gradient);
   //      if (this->mq_DrawBoder == false)
   //      {
   //         opc_Painter->setPen(Qt::NoPen);
   //         std::cout << "Border off" << &std::endl;
   //      }
   //      else
   //      {
   //         QPen c_Pen;
   //         if (this->mq_DrawWhiteFilter == false)
   //         {
   //            c_Pen.setColor(QColor(227, 227, 230, 255));
   //         }
   //         else
   //         {
   //            c_Pen.setColor(mc_STYLE_GUIDE_COLOR_11);
   //         }
   //         opc_Painter->setPen(c_Pen);
   //      }

   //      opc_Painter->drawRoundedRect(c_Rect, 10.0, 10.0);

   //      if (this->mq_DrawWhiteFilter == true)
   //      {
   //         // draw the white filter
   //         QColor c_Color = mc_STYLE_GUIDE_COLOR_0;
   //         QBrush c_Brush;

   //         // half transparent
   //         c_Color.setAlpha(127);
   //         c_Brush.setColor(c_Color);
   //         c_Brush.setStyle(Qt::SolidPattern);

   //         opc_Painter->setBrush(c_Brush);
   //         opc_Painter->setPen(Qt::NoPen);
   //         opc_Painter->drawRoundedRect(c_Rect, 10.0, 10.0);
   //      }
   //   }
   if (opc_Painter != NULL)
   {
      QLinearGradient c_Gradient;
      QRectF c_Rect = boundingRect();
      QRectF c_ShaddowRect;
      QRectF c_SurroundingRect = c_Rect;
      QRectF c_InsideRect;

      if (this->mq_DrawBoder == true)
      {
         if (this->mq_DrawWhiteFilter == false)
         {
            //Draw Surrounding Rect
            opc_Painter->setPen(Qt::NoPen);
            opc_Painter->setBrush(QBrush(mc_STYLE_GUIDE_COLOR_3));
            opc_Painter->drawRoundedRect(c_SurroundingRect, 12.0, 12.0);

            // Draw shaddow as bigger rect than base -> smaler when surrounding rect is drawn
            c_ShaddowRect.setRect(static_cast<float64>(c_Rect.left()) + 2.0,
                                  static_cast<float64>(c_Rect.top()) + 2.0,
                                  static_cast<float64>(c_Rect.width()) - 4.0,
                                  static_cast<float64>(c_Rect.height()) - 4.0);
         }
         else
         {
            // Draw shaddow as bigger rect than base -> bigger when no surrounding rect
            c_ShaddowRect.setRect(static_cast<float64>(c_Rect.left()) + 2.0,
                                  static_cast<float64>(c_Rect.top()) + 2.0,
                                  static_cast<float64>(c_Rect.width()) - 4.0,
                                  static_cast<float64>(c_Rect.height()) - 3.0);
         }
      }
      else
      {
         // Draw shaddow as bigger rect than base -> bigger when no surrounding rect
         c_ShaddowRect.setRect(static_cast<float64>(c_Rect.left()) + 2.0,
                               static_cast<float64>(c_Rect.top()) + 2.0,
                               static_cast<float64>(c_Rect.width()) - 4.0,
                               static_cast<float64>(c_Rect.height()) - 3.0);
      }
      opc_Painter->setPen(Qt::NoPen);
      opc_Painter->setBrush(QBrush(QColor(206, 206, 217)));
      opc_Painter->drawRoundedRect(c_ShaddowRect, 10.0, 10.0);

      // Scalings depending on assumed c_Rect with  w:210, h:137.5
      //rounded rect
      c_Gradient.setColorAt(0.0, QColor(251, 251, 252));
      c_Gradient.setColorAt(0.33, QColor(251, 251, 252));
      c_Gradient.setColorAt(0.86, QColor(237, 237, 237));
      c_Gradient.setColorAt(1.0, QColor(237, 237, 237));

      c_InsideRect.setRect(static_cast<float64>(c_Rect.left()) + 3.5,
                           static_cast<float64>(c_Rect.top()) + 2.5,
                           static_cast<float64>(c_Rect.width()) - 7.0,
                           static_cast<float64>(c_Rect.height()) - 6.0);

      c_Gradient.setStart(static_cast<float64>(c_Rect.left()) - ((7.5 * static_cast<float64>(c_Rect.width())) / 210.0),
                          static_cast<float64>(c_Rect.top()) +
                          ((27.5 * static_cast<float64>(c_Rect.height())) / 137.5));
      c_Gradient.setFinalStop(static_cast<float64>(c_Rect.right()) +
                              ((6.25 * static_cast<float64>(c_Rect.width())) / 210.0),
                              static_cast<float64>(c_Rect.bottom()) -
                              ((27.5 * static_cast<float64>(c_Rect.height())) / 137.5));

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
