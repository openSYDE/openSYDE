//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for the message layout viewer graphic items (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>

#include "stwtypes.hpp"

#include "constants.hpp"

#include "C_SdBueMlvBaseItem.hpp"
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

   \param[in]      orc_BackgroundColor    Background color
   \param[in]      orc_FontColor          Font color
   \param[in]      orc_Font               Font configuration
   \param[in]      orc_Text               Text
   \param[in]      oq_DrawText            Flag if text is visible
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvBaseItem::C_SdBueMlvBaseItem(const QColor & orc_BackgroundColor, const QColor & orc_FontColor,
                                       const QFont & orc_Font, const QString & orc_Text, const bool oq_DrawText,
                                       QGraphicsItem * const opc_Parent) :
   QGraphicsItem(opc_Parent),
   mc_BackgroundColorDefault(orc_BackgroundColor),
   mc_DrawBackgroundColor(orc_BackgroundColor),
   mc_FontColorDefault(orc_FontColor),
   mc_DrawFontColor(orc_FontColor),
   mc_Font(orc_Font),
   mc_Text(orc_Text),
   mc_Size(50.0, 50.0),
   mq_DrawBackground(true),
   mq_DrawText(oq_DrawText)
{
   //Convert point to pixel
   this->mc_Font.setPixelSize(this->mc_Font.pointSize());

   this->setFlag(ItemSendsGeometryChanges);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvBaseItem::~C_SdBueMlvBaseItem()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the shown text

   \param[in]  orc_Text    New text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvBaseItem::SetText(const QString & orc_Text)
{
   this->mc_Text = orc_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the size of the rectangle

   \param[in]  orc_Size    New size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvBaseItem::SetSize(const QSizeF & orc_Size)
{
   this->mc_Size = orc_Size;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets new font for the text

   \param[in]  orc_Font    New font
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvBaseItem::SetFont(const QFont & orc_Font)
{
   this->mc_Font = orc_Font;

   //Convert point to pixel
   this->mc_Font.setPixelSize(this->mc_Font.pointSize());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   \param[in,out]  opc_Painter   Painter
   \param[in,out]  opc_Option    Option
   \param[in,out]  opc_Widget    Widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvBaseItem::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                               QWidget * const opc_Widget)
{
   QPen c_Pen = opc_Painter->pen();
   QBrush c_Brush = opc_Painter->brush();

   Q_UNUSED(opc_Option)
   Q_UNUSED(opc_Widget)

   if (this->mq_DrawBackground == true)
   {
      // configure the font and color for the rectangle
      c_Pen.setColor(Qt::transparent);

      c_Brush.setColor(this->mc_DrawBackgroundColor);
      c_Brush.setStyle(Qt::SolidPattern);

      opc_Painter->setPen(c_Pen);
      opc_Painter->setBrush(c_Brush);
      opc_Painter->drawRect(QRectF(0.0, 0.0, this->mc_Size.width(), mc_Size.height()));
   }

   if (this->mq_DrawText == true)
   {
      // configure the font and color for the text
      c_Pen.setColor(this->mc_DrawFontColor);
      opc_Painter->setPen(c_Pen);
      c_Brush.setStyle(Qt::NoBrush);
      opc_Painter->setBrush(c_Brush);

      opc_Painter->setFont(this->mc_Font);

      // draw the text
      opc_Painter->drawText(this->boundingRect(), static_cast<int32_t>(Qt::AlignCenter),
                            stw::opensyde_gui_logic::C_Uti::h_AdaptStringToSize(this->mc_Text,
                                                                                opc_Painter->fontMetrics(),
                                                                                this->boundingRect().width()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns an estimate of the area painted by the item

   \return     Rectangle
*/
//----------------------------------------------------------------------------------------------------------------------
QRectF C_SdBueMlvBaseItem::boundingRect() const
{
   return QRectF(0.0, 0.0, this->mc_Size.width(), mc_Size.height());
}
