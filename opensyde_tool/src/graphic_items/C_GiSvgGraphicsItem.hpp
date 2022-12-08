//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       For rendering SVG Images

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVGGRAPHICSITEM_H
#define C_GISVGGRAPHICSITEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QGraphicsRectItem>
#include <QString>
#include <QSvgRenderer>

#include "C_GiBiSizeableItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvgGraphicsItem :
   public QGraphicsRectItem,
   public C_GiBiSizeableItem
{
public:
   C_GiSvgGraphicsItem(const QString & orc_ImagePath, const float64_t of64_Width, const float64_t of64_Height,
                       QGraphicsItem * const opc_Parent = NULL);
   ~C_GiSvgGraphicsItem(void) override;

   void SetSvg(const QString & orc_ImagePath);
   void SetNewSize(const float64_t of64_Width, const float64_t of64_Height);
   void Redraw(void) override;

   void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
              QWidget * const opc_Widget = NULL) override;

private:
   const QString mc_ImagePath;
   QSvgRenderer * mpc_SvgRenderer;

   //Avoid call
   C_GiSvgGraphicsItem(const C_GiSvgGraphicsItem &);
   C_GiSvgGraphicsItem & operator =(const C_GiSvgGraphicsItem &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif // C_GISVGGRAPHICSITEM_H
