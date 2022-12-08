//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics item for showing an optional icon and a background (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIRECTPIXMAP_H
#define C_GIRECTPIXMAP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsRectItem>
#include <QSvgRenderer>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiRectPixmap :
   public QGraphicsRectItem
{
public:
   C_GiRectPixmap(const QRectF & orc_Rect, QGraphicsItem * const opc_Parent = NULL);
   ~C_GiRectPixmap() override;

   void SetBackgroundColor(const QColor & orc_Color);
   void SetDrawPixmap(const bool oq_Active);
   void SetNewSize(const QSizeF & orc_Size);
   void SetNewRect(const QRectF & orc_Rect);
   void SetSvg(const QString & orc_Value);

   void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
              QWidget * const opc_Widget) override;

protected:
   QPixmap mc_Pixmap;
   QPixmap mc_PixmapScaled;
   bool mq_DrawPixmap;
   bool mq_DrawSvg;

private:
   QSvgRenderer * mpc_SvgRenderer;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
