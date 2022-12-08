//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description

   Detailed description (optional). The module shall be described detailed if it is not described completely
   by short description.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>
#include <QMouseEvent>

#include "stwtypes.hpp"
#include "C_GiSyColorPicker.hpp"

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

   Set up the color picker with all its functions.

   \param[in,out]   opc_Parent Pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSyColorPicker::C_GiSyColorPicker(QWidget * const opc_Parent) :
   QFrame(opc_Parent),
   mq_CircleVisible(true),
   ms32_Hue(0),
   ms32_Sat(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSyColorPicker::~C_GiSyColorPicker(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the circle for screen color picking visible

   \param[in]   oq_Visible   value if circle set or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorPicker::SetCircleVisible(const bool oq_Visible)
{
   if (this->mq_CircleVisible != oq_Visible)
   {
      this->mq_CircleVisible = oq_Visible;
      update();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set a new color to the color picker and repaint it

   \param[in]   os32_Hue         new hue value
   \param[in]   os32_Saturation   new sat value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorPicker::SetColor(const int32_t os32_Hue, const int32_t os32_Saturation)
{
   if ((qMin(qMax(0, os32_Hue), 359) != this->ms32_Hue) || (qMin(qMax(0, os32_Saturation), 255) != this->ms32_Sat))
   {
      QRect c_Rect(QPoint(((360 - this->ms32_Hue) * (contentsRect().width() - 1)) / 360,
                          ((255 - this->ms32_Sat) * (contentsRect().height() - 1)) / 255),
                   QSize(50, 20));
      this->ms32_Hue = qMin(qMax(0, os32_Hue), 359);
      this->ms32_Sat = qMin(qMax(0, os32_Saturation), 255);
      c_Rect = c_Rect.united(QRect(QPoint(((360 - this->ms32_Hue) * (contentsRect().width() - 1)) / 360,
                                          ((255 - this->ms32_Sat) * (contentsRect().height() - 1)) / 255),
                                   QSize(50, 20)));
      c_Rect.translate(contentsRect().x() - 9, contentsRect().y() - 9);
      repaint(c_Rect);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten size hint slot

   \return
   QSize   size of color picker
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_GiSyColorPicker::sizeHint(void) const
{
   return QSize(251 + (2 * frameWidth()), 178 + (2 * frameWidth()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   \param[in,out]   opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorPicker::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   QPainter c_Painter(this);
   c_Painter.setRenderHint(QPainter::Antialiasing);
   drawFrame(&c_Painter);
   c_Painter.drawPixmap(contentsRect().topLeft(), this->mc_Pixmap);

   if (this->mq_CircleVisible)
   {
      QPen c_Pen;
      c_Pen.setColor(Qt::white);
      c_Pen.setWidth(2);
      c_Painter.setPen(c_Pen);
      c_Painter.drawEllipse(QPoint(((360 - this->ms32_Hue) * (contentsRect().width() - 1)) / 360,
                                   ((255 - this->ms32_Sat) * (contentsRect().height() - 1)) / 255),
                            6, 6);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   \param[in,out]   opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorPicker::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   this->m_SetColorFromPosition(opc_Event->pos() - contentsRect().topLeft());
   this->repaint();
   Q_EMIT SigNewColor(this->ms32_Hue, this->ms32_Sat);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   \param[in,out]   opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorPicker::mousePressEvent(QMouseEvent * const opc_Event)
{
   this->m_SetColorFromPosition(opc_Event->pos() - contentsRect().topLeft());
   Q_EMIT SigNewColor(this->ms32_Hue, this->ms32_Sat);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse release event slot

   \param[in,out]   opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorPicker::mouseReleaseEvent(QMouseEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)
   Q_EMIT SigColorSelected();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   \param[in,out]   opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorPicker::resizeEvent(QResizeEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   QImage c_Image((width() - (frameWidth() * 2)), (height() - (frameWidth() * 2)), QImage::Format_RGB32);
   //lint -e{826,927,9176}  Cast is necessary due to Qt interface. See Qt documentation for scanLine
   int32_t * ps32_Pixel = reinterpret_cast<int32_t *>(c_Image.scanLine(0));
   for (int32_t s32_VerticalValue = 0; s32_VerticalValue < (height() - (frameWidth() * 2)); s32_VerticalValue++)
   {
      //lint -e{9016,9114}  See Qt documentation for interface
      const int32_t * const ps32_End = ps32_Pixel + (width() - (frameWidth() * 2));
      int32_t s32_HorizontalValue = 0;
      //lint -e{946}  operator is necessary for painting the brightness
      while (ps32_Pixel < ps32_End)
      {
         QColor c_Color;
         c_Color.setHsv(360 - ((QPoint(s32_HorizontalValue,
                                       s32_VerticalValue).x() * 360) / (contentsRect().width() - 1)),
                        255 -
                        ((QPoint(s32_HorizontalValue, s32_VerticalValue).y() * 255) / (contentsRect().height() - 1)),
                        200);
         *ps32_Pixel = c_Color.rgb();
         ++ps32_Pixel;
         ++s32_HorizontalValue;
      }
   }
   this->mc_Pixmap = QPixmap::fromImage(c_Image);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new color from position while screen color picking

   \param[in]   orc_Point   Point of color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorPicker::m_SetColorFromPosition(const QPoint & orc_Point)
{
   this->SetColor(360 - ((orc_Point.x() * 360) / (contentsRect().width() - 1)),
                  255 - ((orc_Point.y() * 255) / (contentsRect().height() - 1)));
}
