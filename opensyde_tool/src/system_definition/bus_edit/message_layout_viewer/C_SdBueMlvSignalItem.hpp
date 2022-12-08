//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics item for signal items of message layout viewer (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMLVSIGNALITEM_H
#define C_SDBUEMLVSIGNALITEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include <QColor>

#include "stwtypes.hpp"

#include "C_SdBueMlvBaseItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMlvSignalItem :
   public QObject,
   public C_SdBueMlvBaseItem
{
   Q_OBJECT

public:
   C_SdBueMlvSignalItem(const QColor & orc_BackgroundColor, const QColor & orc_FontColor,
                        const QColor & orc_ResizeItemColor, const QString & orc_Name, const float64_t of64_Space,
                        QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_SdBueMlvSignalItem();

   void Update(const float64_t of64_SingleItemWidth, const float64_t of64_SingleItemHeight);
   void SetBitPosition(const uint16_t ou16_ByteRow, const uint16_t ou16_StartBit, const uint16_t ou16_LastBit);
   void SetShowSignificantBit(const bool oq_Msb, const bool oq_Lsb);
   void SetResizeItem(const bool oq_LeftResizeItem, const bool oq_RightResizeItem, const bool oq_IsResizeable);
   void SetHovered(const bool oq_Hover);
   void SetError(const bool oq_Active);
   void SetSelected(const bool oq_Selected);
   void SetColors(const QColor & orc_BackgroundColor, const QColor & orc_FontColor, const QColor & orc_ResizeItemColor);

   bool ContainsPoint(const QPointF & orc_Pos) const;
   bool ContainsLeftResizeItemPoint(const QPointF & orc_Pos) const;
   bool ContainsRightResizeItemPoint(const QPointF & orc_Pos) const;

   void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
              QWidget * const opc_Widget) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigItemHovered(const bool oq_Hover);
   void SigChangeCursor(const Qt::CursorShape oe_Cursor);

protected:
   void hoverEnterEvent(QGraphicsSceneHoverEvent * const opc_Event) override;
   void hoverMoveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;
   void hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdBueMlvSignalItem(const C_SdBueMlvSignalItem &);
   C_SdBueMlvSignalItem & operator =(const C_SdBueMlvSignalItem &) &; //lint !e1511 //we want to hide the base function

   uint16_t mu16_ByteRow;
   uint16_t mu16_StartBit;
   uint16_t mu16_LastBit;
   bool mq_ShowMsb;
   bool mq_ShowLsb;
   bool mq_ShowLeftResizeItem;
   bool mq_ShowRightResizeItem;
   bool mq_IsResizeable;
   bool mq_Hovered;

   QFont mc_ByteOrderFont;
   QRectF mc_LeftResizeItemRectDraw;
   QRectF mc_RightResizeItemRectDraw;
   QRectF mc_LeftResizeItemRectClick;
   QRectF mc_RightResizeItemRectClick;

   const float64_t mf64_Space;
   static const float64_t mhf64_RESIZE_ITEM_HEIGHT;
   static const float64_t mhf64_RESIZE_ITEM_CLICK_OFFSET;
   QColor mc_DrawReizeItemColor;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
