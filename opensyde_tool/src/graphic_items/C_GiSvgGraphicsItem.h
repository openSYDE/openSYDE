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

#include "C_GiBiSizeableItem.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvgGraphicsItem :
   public QGraphicsRectItem,
   public C_GiBiSizeableItem
{
public:
   C_GiSvgGraphicsItem(const QString & orc_ImagePath, const stw_types::float64 of64_Width,
                       const stw_types::float64 of64_Height, QGraphicsItem * const opc_Parent = NULL);
   ~C_GiSvgGraphicsItem(void);

   void SetSvg(const QString & orc_ImagePath);
   void SetNewSize(const stw_types::float64 of64_Width, const stw_types::float64 of64_Height);
   virtual void Redraw(void) override;

   virtual void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                      QWidget * const opc_Widget = NULL) override;

private:
   const QString mc_ImagePath;
   QSvgRenderer * mpc_SvgRenderer;

   //Avoid call
   C_GiSvgGraphicsItem(const C_GiSvgGraphicsItem &);
   C_GiSvgGraphicsItem & operator =(const C_GiSvgGraphicsItem &); //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif // C_GISVGGRAPHICSITEM_H
