//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics item for signal items of message layout viewer (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>
#include <QGraphicsSceneHoverEvent>

#include "constants.hpp"

#include "C_SdBueMlvSignalItem.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const float64_t C_SdBueMlvSignalItem::mhf64_RESIZE_ITEM_HEIGHT = 20.0;
const float64_t C_SdBueMlvSignalItem::mhf64_RESIZE_ITEM_CLICK_OFFSET = 8.0;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]       orc_BackgroundColor      Background color
   \param[in]       orc_FontColor            Font color
   \param[in]       orc_ResizeItemColor      Color of the clickable resize item
   \param[in]       orc_Name                 Name of the signal
   \param[in]       of64_Space               Space configuration between signals
   \param[in,out]   opc_Parent               Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvSignalItem::C_SdBueMlvSignalItem(const QColor & orc_BackgroundColor, const QColor & orc_FontColor,
                                           const QColor & orc_ResizeItemColor, const QString & orc_Name,
                                           const float64_t of64_Space, QGraphicsItem * const opc_Parent) :
   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   C_SdBueMlvBaseItem(orc_BackgroundColor, orc_FontColor, mc_STYLE_GUIDE_FONT_REGULAR_16, orc_Name, true, opc_Parent),
   mu16_ByteRow(0U),
   mu16_StartBit(0U),
   mu16_LastBit(0U),
   mq_ShowMsb(false),
   mq_ShowLsb(false),
   mq_ShowLeftResizeItem(false),
   mq_ShowRightResizeItem(false),
   mq_IsResizeable(true),
   mq_Hovered(false),
   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   mc_ByteOrderFont(mc_STYLE_GUIDE_FONT_REGULAR_13),
   mc_LeftResizeItemRectDraw(QRect()),
   mc_RightResizeItemRectDraw(QRect()),
   mf64_Space(of64_Space),
   mc_DrawReizeItemColor(orc_ResizeItemColor)
{
   //Convert point to pixel
   this->mc_ByteOrderFont.setPixelSize(this->mc_ByteOrderFont.pointSize());

   this->setAcceptHoverEvents(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvSignalItem::~C_SdBueMlvSignalItem()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the size and position of the item

   \param[in]     of64_SingleItemWidth      New item width
   \param[in]     of64_SingleItemHeight     New item height
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalItem::Update(const float64_t of64_SingleItemWidth, const float64_t of64_SingleItemHeight)
{
   const uint16_t u16_Length = (this->mu16_LastBit - this->mu16_StartBit) + static_cast<uint16_t>(1U);
   const float64_t f64_Width = (static_cast<float64_t>(u16_Length) * of64_SingleItemWidth) +
                               ((static_cast<float64_t>(u16_Length) - 1.0) * this->mf64_Space);
   // start bit is right of the last bit in not inverted mode
   const uint16_t u16_LastBitIndex = 8U - this->mu16_LastBit;
   const float64_t f64_PosHorizontal = static_cast<float64_t>(u16_LastBitIndex) *
                                       (of64_SingleItemWidth + this->mf64_Space);
   // compensate the border by adding 1
   const float64_t f64_PosVertical = ((static_cast<float64_t>(this->mu16_ByteRow) + 1.0) *
                                      (of64_SingleItemHeight + this->mf64_Space));

   this->prepareGeometryChange();
   this->SetSize(QSizeF(f64_Width, of64_SingleItemHeight));
   this->setPos(f64_PosHorizontal, f64_PosVertical);
   this->update();

   // rectangle for the left resize item
   if (this->mq_ShowLeftResizeItem == true)
   {
      this->mc_LeftResizeItemRectDraw.setRect(0.0, (of64_SingleItemHeight / 2.0) -
                                              (C_SdBueMlvSignalItem::mhf64_RESIZE_ITEM_HEIGHT / 2.0),
                                              5.0, C_SdBueMlvSignalItem::mhf64_RESIZE_ITEM_HEIGHT);

      this->mc_LeftResizeItemRectClick = this->mc_LeftResizeItemRectDraw;

      // increase the size of the clickable area
      this->mc_LeftResizeItemRectClick.setWidth(mc_LeftResizeItemRectClick.width() + mhf64_RESIZE_ITEM_CLICK_OFFSET);
      this->mc_LeftResizeItemRectClick.setY(mc_LeftResizeItemRectClick.y() - mhf64_RESIZE_ITEM_CLICK_OFFSET);
      this->mc_LeftResizeItemRectClick.setHeight(mc_LeftResizeItemRectClick.height() + mhf64_RESIZE_ITEM_CLICK_OFFSET);
   }

   // rectangle for the right resize item
   if (this->mq_ShowRightResizeItem == true)
   {
      this->mc_RightResizeItemRectDraw.setRect((f64_Width - 5.0), (of64_SingleItemHeight / 2.0) -
                                               (C_SdBueMlvSignalItem::mhf64_RESIZE_ITEM_HEIGHT / 2.0),
                                               5.0, C_SdBueMlvSignalItem::mhf64_RESIZE_ITEM_HEIGHT);

      this->mc_RightResizeItemRectClick = this->mc_RightResizeItemRectDraw;

      // increase the size of the clickable area
      this->mc_RightResizeItemRectClick.setX(mc_RightResizeItemRectClick.x() - mhf64_RESIZE_ITEM_CLICK_OFFSET);
      this->mc_RightResizeItemRectClick.setY(mc_RightResizeItemRectClick.y() - mhf64_RESIZE_ITEM_CLICK_OFFSET);
      this->mc_RightResizeItemRectClick.setHeight(mc_RightResizeItemRectClick.height() +
                                                  mhf64_RESIZE_ITEM_CLICK_OFFSET);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the byte/bit position of the signal item in the layout

   \param[in]     ou16_ByteRow     Actual byte row
   \param[in]     ou16_StartBit    Actual start bit
   \param[in]     ou16_LastBit     Actual last bit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalItem::SetBitPosition(const uint16_t ou16_ByteRow, const uint16_t ou16_StartBit,
                                          const uint16_t ou16_LastBit)
{
   this->mu16_ByteRow = ou16_ByteRow;
   this->mu16_StartBit = ou16_StartBit;
   this->mu16_LastBit = ou16_LastBit;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the flags for showing the msb and / or lsb information

   \param[in]     oq_Msb     Flag for showing the msb
   \param[in]     oq_Lsb     Flag for showing the lsb
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalItem::SetShowSignificantBit(const bool oq_Msb, const bool oq_Lsb)
{
   this->mq_ShowMsb = oq_Msb;
   this->mq_ShowLsb = oq_Lsb;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the flags for showing the needed resize items

   \param[in]     oq_LeftResizeItem     Flag for showing the left resize item
   \param[in]     oq_RightResizeItem    Flag for showing the right resize item
   \param[in]     oq_IsResizeable       Flag if the item is really resize able
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalItem::SetResizeItem(const bool oq_LeftResizeItem, const bool oq_RightResizeItem,
                                         const bool oq_IsResizeable)
{
   this->mq_ShowLeftResizeItem = oq_LeftResizeItem;
   this->mq_ShowRightResizeItem = oq_RightResizeItem;
   this->mq_IsResizeable = oq_IsResizeable;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the flags if the signal item shall be handled as hovered

   \param[in]     oq_Hover     Flag if item is hovered
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalItem::SetHovered(const bool oq_Hover)
{
   this->mq_Hovered = oq_Hover;
   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the error state

   \param[in]     oq_Active     Flag if an error is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalItem::SetError(const bool oq_Active)
{
   this->mc_Font.setItalic(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the visualization to show the selection state

   \param[in]     oq_Selected     input parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalItem::SetSelected(const bool oq_Selected)
{
   if (oq_Selected == true)
   {
      this->SetFont(mc_STYLE_GUIDE_FONT_BOLD_16);
   }
   else
   {
      this->SetFont(mc_STYLE_GUIDE_FONT_REGULAR_16);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets new colors

   \param[in]       orc_BackgroundColor      Background color
   \param[in]       orc_FontColor            Font color
   \param[in]       orc_ResizeItemColor      Color of the clickable resize item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalItem::SetColors(const QColor & orc_BackgroundColor, const QColor & orc_FontColor,
                                     const QColor & orc_ResizeItemColor)
{
   this->mc_BackgroundColorDefault = orc_BackgroundColor;
   this->mc_DrawBackgroundColor = orc_BackgroundColor;
   this->mc_FontColorDefault = orc_FontColor;
   this->mc_DrawFontColor = orc_FontColor;
   this->mc_DrawReizeItemColor = orc_ResizeItemColor;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if a specific point is inside of the item

   \param[in]     orc_Pos        Searched point

   \return
   true     Point is inside
   false    Point is not inside
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMlvSignalItem::ContainsPoint(const QPointF & orc_Pos) const
{
   // position and size is relevant
   const QRectF c_Rect(this->pos(), this->boundingRect().size());

   return c_Rect.contains(orc_Pos);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if a specific point is inside of the left resize item

   \param[in]     orc_Pos        Searched point

   \return
   true     Point is inside
   false    Point is not inside
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMlvSignalItem::ContainsLeftResizeItemPoint(const QPointF & orc_Pos) const
{
   bool q_Return = false;
   const QPointF c_LocalPos = this->mapFromScene(orc_Pos);

   if ((this->mq_IsResizeable == true) &&
       (this->mq_ShowLeftResizeItem == true))
   {
      q_Return = this->mc_LeftResizeItemRectClick.contains(c_LocalPos);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if a specific point is inside of the right resize item

   \param[in]     orc_Pos        Searched point

   \return
   true     Point is inside
   false    Point is not inside
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMlvSignalItem::ContainsRightResizeItemPoint(const QPointF & orc_Pos) const
{
   bool q_Return = false;
   const QPointF c_LocalPos = this->mapFromScene(orc_Pos);

   if ((this->mq_IsResizeable == true) &&
       (this->mq_ShowRightResizeItem == true))
   {
      q_Return = this->mc_RightResizeItemRectClick.contains(c_LocalPos);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   \param[in,out] opc_Painter Painter
   \param[in,out] opc_Option  Option
   \param[in,out] opc_Widget  Widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalItem::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                                 QWidget * const opc_Widget)
{
   C_SdBueMlvBaseItem::paint(opc_Painter, opc_Option, opc_Widget);

   // drawing the resize "items"
   if ((this->mq_IsResizeable == true) &&
       (this->mq_Hovered == true) &&
       ((this->mq_ShowLeftResizeItem == true) || (this->mq_ShowRightResizeItem == true)))
   {
      QPen c_Pen = opc_Painter->pen();
      // Set the color
      c_Pen.setColor(this->mc_DrawReizeItemColor);
      opc_Painter->setPen(c_Pen);

      if (this->mq_ShowLeftResizeItem == true)
      {
         // draw the three lines for the left item
         QPointF c_Pos1(this->mc_LeftResizeItemRectDraw.topLeft());
         QPointF c_Pos2(this->mc_LeftResizeItemRectDraw.bottomLeft());
         opc_Painter->drawLine(c_Pos1, c_Pos2);

         c_Pos1.setX(c_Pos1.x() + 2.0);
         c_Pos2.setX(c_Pos2.x() + 2.0);
         opc_Painter->drawLine(c_Pos1, c_Pos2);

         c_Pos1.setX(c_Pos1.x() + 2.0);
         c_Pos2.setX(c_Pos2.x() + 2.0);
         opc_Painter->drawLine(c_Pos1, c_Pos2);
      }

      if (this->mq_ShowRightResizeItem == true)
      {
         // draw the three lines for the right item
         QPointF c_Pos1(this->mc_RightResizeItemRectDraw.topLeft());
         QPointF c_Pos2(this->mc_RightResizeItemRectDraw.bottomLeft());
         opc_Painter->drawLine(c_Pos1, c_Pos2);

         c_Pos1.setX(c_Pos1.x() + 2.0);
         c_Pos2.setX(c_Pos2.x() + 2.0);
         opc_Painter->drawLine(c_Pos1, c_Pos2);

         c_Pos1.setX(c_Pos1.x() + 2.0);
         c_Pos2.setX(c_Pos2.x() + 2.0);
         opc_Painter->drawLine(c_Pos1, c_Pos2);
      }
   }

   // drawing the information about msb/lsb
   if ((this->mq_ShowMsb == true) ||
       (this->mq_ShowLsb == true))
   {
      QPen c_Pen = opc_Painter->pen();
      QBrush c_Brush = opc_Painter->brush();
      QRectF c_Rect = this->boundingRect();
      uint32_t u32_Alignment = static_cast<uint32_t>(Qt::AlignBottom);

      // configure the font and color for the text
      c_Pen.setColor(this->mc_DrawFontColor);
      opc_Painter->setPen(c_Pen);
      c_Brush.setStyle(Qt::NoBrush);
      opc_Painter->setBrush(c_Brush);

      opc_Painter->setFont(this->mc_ByteOrderFont);

      // adapt the region
      c_Rect.setX(c_Rect.x() + 5.0);
      c_Rect.setWidth(c_Rect.width() - 5.0);

      if (this->mq_ShowMsb == true)
      {
         u32_Alignment |= static_cast<uint32_t>(Qt::AlignLeft);

         // draw the text
         opc_Painter->drawText(c_Rect, static_cast<int32_t>(u32_Alignment), C_GtGetText::h_GetText("MSB"));
      }
      if (this->mq_ShowLsb == true)
      {
         u32_Alignment |= static_cast<uint32_t>(Qt::AlignRight);

         // draw the text
         opc_Painter->drawText(c_Rect, static_cast<int32_t>(u32_Alignment), C_GtGetText::h_GetText("LSB"));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse hover enter event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalItem::hoverEnterEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   Q_EMIT this->SigItemHovered(true);

   C_SdBueMlvBaseItem::hoverEnterEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse hover enter event slot

   Here: Change the cursor if over resize icon

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalItem::hoverMoveEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   bool q_ResizeIconHover = false;

   if (this->mq_IsResizeable == true)
   {
      if (this->mq_ShowLeftResizeItem == true)
      {
         if (this->mc_LeftResizeItemRectClick.contains(this->mapFromScene(opc_Event->scenePos())) == true)
         {
            q_ResizeIconHover = true;
         }
      }

      if (this->mq_ShowRightResizeItem == true)
      {
         if (this->mc_RightResizeItemRectClick.contains(this->mapFromScene(opc_Event->scenePos())) == true)
         {
            q_ResizeIconHover = true;
         }
      }
   }

   if (q_ResizeIconHover == true)
   {
      Q_EMIT this->SigChangeCursor(Qt::SizeHorCursor);
   }
   else
   {
      Q_EMIT this->SigChangeCursor(Qt::ArrowCursor);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse hover leave event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   Q_EMIT this->SigItemHovered(false);
   Q_EMIT this->SigChangeCursor(Qt::ArrowCursor);

   C_SdBueMlvBaseItem::hoverLeaveEvent(opc_Event);
}
