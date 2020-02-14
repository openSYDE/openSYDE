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
#include "precomp_headers.h"

#include <QPainter>
#include <QMouseEvent>

#include "stwtypes.h"
#include "C_GiSyColorBrightnessPicker.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up the color brightness picker with all its functions.

   \param[in,out]   opc_Parent Pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSyColorBrightnessPicker::C_GiSyColorBrightnessPicker(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   msn_Hue(0),
   msn_Sat(0),
   msn_Value(0),
   mp_Pixmap(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSyColorBrightnessPicker::~C_GiSyColorBrightnessPicker()
{
   delete this->mp_Pixmap;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set a hue, sat and value to the brightness picker and repaint it

   \param[in]   osn_Hue     New hue value
   \param[in]   osn_Sat     New sat value
   \param[in]   osn_Value   New value value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorBrightnessPicker::SetColorWithHSV(sintn const osn_Hue, sintn const osn_Sat, sintn const osn_Value)
{
   this->msn_Hue = osn_Hue;
   this->msn_Sat = osn_Sat;
   this->msn_Value = osn_Value;
   delete this->mp_Pixmap;
   this->mp_Pixmap = NULL;
   repaint();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Receives from a hue, sat chooser and relays.

   \param[in,out]   osn_Hue   New hue value
   \param[in,out]   osn_Sat   New sat value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorBrightnessPicker::SetColorWithHS(sintn const osn_Hue, sintn const osn_Sat)
{
   Q_EMIT NewHsv(osn_Hue, osn_Sat, this->msn_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paint the brightness picker with the white "Balken" which show the current brightness

   \param[in,out]   opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorBrightnessPicker::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   QRect c_Rect(0, 0, width(), height());
   if ((this->mp_Pixmap == NULL) ||
       (this->mp_Pixmap->height() != c_Rect.height()) ||
       (this->mp_Pixmap->width() != c_Rect.width()))
   {
      delete this->mp_Pixmap;
      QImage c_Image(c_Rect.width(), c_Rect.height(), QImage::Format_RGB32);
      //lint -e{826,927}  Cast is necessary due to Qt interface. See Qt documentation for scanLine
      sintn * psn_Pixel = reinterpret_cast<sintn *>(c_Image.scanLine(0));
      for (sintn sn_YValue = 0; sn_YValue < c_Rect.height(); sn_YValue++)
      {
         sintn * psn_End = psn_Pixel + c_Rect.width();
         std::fill(psn_Pixel, psn_End, QColor::fromHsv(this->msn_Hue,
                                                       this->msn_Sat,
                                                       (255 - ((sn_YValue * 255) / (height() - 1)))).rgb());
         psn_Pixel = psn_End;
      }
      this->mp_Pixmap = new QPixmap(QPixmap::fromImage(c_Image));
   }

   if (this->mp_Pixmap != NULL)
   {
      QPainter c_Painter(this);
      c_Painter.drawPixmap(1, 0, *this->mp_Pixmap);
      c_Painter.setPen(palette().foreground().color());
      c_Painter.setBrush(palette().foreground());
      QPolygon c_Polygon;
      sintn sn_YValue = ((255 - this->msn_Value) * (height() - 1)) / 255;
      c_Polygon.setPoints(4, width(), sn_YValue, width() - 36, sn_YValue, width(), sn_YValue + 1,
                          width() - 36, sn_YValue + 1);
      c_Painter.eraseRect(width(), 0, 5, height());
      c_Painter.setPen(Qt::white);
      c_Painter.drawPolygon(c_Polygon);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   \param[in,out]   opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorBrightnessPicker::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   if (this->msn_Value == (255 - ((opc_Event->y() * 255) / (height() - 1))))
   {
      return;
   }
   this->msn_Value = qMax(0, qMin((255 - ((opc_Event->y() * 255) / (height() - 1))), 255));
   delete this->mp_Pixmap;
   this->mp_Pixmap = NULL;
   repaint();
   Q_EMIT NewHsv(this->msn_Hue, this->msn_Sat, this->msn_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   \param[in,out]   opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorBrightnessPicker::mousePressEvent(QMouseEvent * const opc_Event)
{
   if (this->msn_Value == (255 - ((opc_Event->y() * 255) / (height() - 1))))
   {
      return;
   }
   this->msn_Value = qMax(0, qMin((255 - ((opc_Event->y() * 255) / (height() - 1))), 255));
   delete this->mp_Pixmap;
   this->mp_Pixmap = NULL;
   repaint();
   Q_EMIT NewHsv(this->msn_Hue, this->msn_Sat, this->msn_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse release event slot

   \param[in,out]   opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorBrightnessPicker::mouseReleaseEvent(QMouseEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)
   Q_EMIT ColorSelected();
}
