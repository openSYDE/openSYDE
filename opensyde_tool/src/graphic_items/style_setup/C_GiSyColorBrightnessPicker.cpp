//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Show the brightness of the current color.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>
#include <QMouseEvent>

#include "stwtypes.hpp"
#include "C_GiSyColorBrightnessPicker.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up the color brightness picker with all its functions.

   \param[in,out]  opc_Parent    Pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSyColorBrightnessPicker::C_GiSyColorBrightnessPicker(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   ms32_Hue(0),
   ms32_Sat(0),
   ms32_Value(0),
   mpc_Pixmap(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSyColorBrightnessPicker::~C_GiSyColorBrightnessPicker()
{
   delete this->mpc_Pixmap;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set a hue, sat and value to the brightness picker and repaint it

   \param[in]  os32_Hue     New hue value
   \param[in]  os32_Sat     New sat value
   \param[in]  os32_Value   New value value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorBrightnessPicker::SetColorWithHsv(const int32_t os32_Hue, const int32_t os32_Sat,
                                                  const int32_t os32_Value)
{
   this->ms32_Hue = os32_Hue;
   this->ms32_Sat = os32_Sat;
   this->ms32_Value = os32_Value;
   delete this->mpc_Pixmap;
   this->mpc_Pixmap = NULL;
   repaint();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Receives from a hue, sat chooser and relays.

   \param[in,out]  os32_Hue    New hue value
   \param[in,out]  os32_Sat    New sat value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorBrightnessPicker::SetColorWithHs(const int32_t os32_Hue, const int32_t os32_Sat)
{
   Q_EMIT SigNewHsv(os32_Hue, os32_Sat, this->ms32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paint the brightness picker with the white "Balken" which show the current brightness

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorBrightnessPicker::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   const QRect c_Rect(0, 0, width(), height());
   if ((this->mpc_Pixmap == NULL) ||
       (this->mpc_Pixmap->height() != c_Rect.height()) ||
       (this->mpc_Pixmap->width() != c_Rect.width()))
   {
      delete this->mpc_Pixmap;
      QImage c_Image(c_Rect.width(), c_Rect.height(), QImage::Format_RGB32);
      //lint -e{826,927,9176}  Cast is necessary due to Qt interface. See Qt documentation for scanLine
      int32_t * ps32_Pixel = reinterpret_cast<int32_t *>(c_Image.scanLine(0));
      for (int32_t s32_VerticalValue = 0; s32_VerticalValue < c_Rect.height(); s32_VerticalValue++)
      {
         //lint -e{9016}  See Qt documentation for interface
         int32_t * const ps32_End = ps32_Pixel + c_Rect.width();
         std::fill(ps32_Pixel, ps32_End, QColor::fromHsv(this->ms32_Hue,
                                                         this->ms32_Sat,
                                                         (255 - ((s32_VerticalValue * 255) / (height() - 1)))).rgb());
         ps32_Pixel = ps32_End;
      }
      this->mpc_Pixmap = new QPixmap(QPixmap::fromImage(c_Image));
   }

   if (this->mpc_Pixmap != NULL)
   {
      QPainter c_Painter(this);
      c_Painter.drawPixmap(1, 0, *this->mpc_Pixmap);
      c_Painter.setPen(palette().windowText().color());
      c_Painter.setBrush(palette().windowText());
      QPolygon c_Polygon;
      const int32_t s32_VerticalValue = ((255 - this->ms32_Value) * (height() - 1)) / 255;
      c_Polygon.setPoints(4, width(), s32_VerticalValue, width() - 36, s32_VerticalValue,
                          width(), s32_VerticalValue + 1,
                          width() - 36, s32_VerticalValue + 1);
      c_Painter.eraseRect(width(), 0, 5, height());
      c_Painter.setPen(Qt::white);
      c_Painter.drawPolygon(c_Polygon);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorBrightnessPicker::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   if (this->ms32_Value != (255 - ((opc_Event->y() * 255) / (height() - 1))))
   {
      this->ms32_Value = qMax(0, qMin((255 - ((opc_Event->y() * 255) / (height() - 1))), 255));
      delete this->mpc_Pixmap;
      this->mpc_Pixmap = NULL;
      repaint();
      Q_EMIT SigNewHsv(this->ms32_Hue, this->ms32_Sat, this->ms32_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorBrightnessPicker::mousePressEvent(QMouseEvent * const opc_Event)
{
   if (this->ms32_Value != (255 - ((opc_Event->y() * 255) / (height() - 1))))
   {
      this->ms32_Value = qMax(0, qMin((255 - ((opc_Event->y() * 255) / (height() - 1))), 255));
      delete this->mpc_Pixmap;
      this->mpc_Pixmap = NULL;
      repaint();
      Q_EMIT SigNewHsv(this->ms32_Hue, this->ms32_Sat, this->ms32_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse release event slot

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorBrightnessPicker::mouseReleaseEvent(QMouseEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)
   Q_EMIT SigColorSelected();
}
