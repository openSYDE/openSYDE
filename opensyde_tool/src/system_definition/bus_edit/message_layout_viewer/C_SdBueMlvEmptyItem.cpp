//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics item for empty items of message layout viewer (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>
#include <QIcon>

#include "constants.h"

#include "C_SdBueMlvEmptyItem.h"

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
/*! \brief   Default constructor

   \param[in]      ou8_Index     Index number
   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvEmptyItem::C_SdBueMlvEmptyItem(const uint8 ou8_Index, QGraphicsItem * const opc_Parent) :
   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   C_SdBueMlvBaseItem(mc_STYLE_GUIDE_COLOR_11, mc_STYLE_GUIDE_COLOR_8, mc_STYLE_GUIDE_FONT_REGULAR_13,
                      QString::number(ou8_Index), false, opc_Parent),
   mc_BackgroundColorError(QColor(247, 196, 197)),
   mq_ErrorActive(false),
   mq_DrawRectangle(true),
   mc_BackgroundColorInactive(mc_STYLE_GUIDE_COLOR_12),
   mc_FontColorInactive(mc_STYLE_GUIDE_COLOR_10),
   mq_ItemActive(true)
{
   this->mc_ErrorIcon = QIcon("://images/Error_iconV2.svg").pixmap(mc_ICON_SIZE_24);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvEmptyItem::~C_SdBueMlvEmptyItem()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the index number for the text

   \param[in]  ou8_Index   Index number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvEmptyItem::SetIndex(const uint8 ou8_Index)
{
   this->SetText(QString::number(ou8_Index));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the error state

   \param[in]  oq_Error    Flag if error is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvEmptyItem::SetError(const bool oq_Error)
{
   this->mq_ErrorActive = oq_Error;

   if (this->mq_ItemActive == true)
   {
      if (this->mq_ErrorActive == false)
      {
         // error is not active, update the drawing of the background
         this->mq_DrawBackground = this->mq_DrawRectangle;

         // set the drawing standard background color
         this->mc_DrawBackgroundColor = this->mc_BackgroundColorDefault;
      }
      else
      {
         // error is active, draw always the background
         this->mq_DrawBackground = true;

         // set the drawing error background color
         this->mc_DrawBackgroundColor = this->mc_BackgroundColorError;

         // reset the font color
         this->mc_DrawFontColor = this->mc_FontColorDefault;
      }

      this->update();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the active state

   \param[in]  oq_Active   Flag if active state is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvEmptyItem::SetActive(const bool oq_Active)
{
   this->mq_ItemActive = oq_Active;

   if (this->mq_ItemActive == true)
   {
      // item is active, update the drawing of the background
      this->SetError(this->mq_ErrorActive);

      this->mc_DrawFontColor = this->mc_FontColorDefault;
   }
   else
   {
      // item is not active, draw always the background
      this->mq_DrawBackground = true;

      // set the drawing inactive colors
      this->mc_DrawBackgroundColor = this->mc_BackgroundColorInactive;
      this->mc_DrawFontColor = this->mc_FontColorInactive;
   }

   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the active font color to the parameter color

   \param[in]  orc_Color   New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvEmptyItem::SetFontColor(const QColor & orc_Color)
{
   if ((this->mq_ItemActive == true) && (this->mq_ErrorActive == false))
   {
      this->mc_DrawFontColor = orc_Color;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the active font color to the default color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvEmptyItem::RestoreFontColor(void)
{
   if (this->mq_ItemActive == true)
   {
      this->mc_DrawFontColor = this->mc_FontColorDefault;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the flag for drawing the rectangle

   \param[in]  oq_DrawRectangle  Flag for drawing the rectangle
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvEmptyItem::SetDrawRectangle(const bool oq_DrawRectangle)
{
   this->mq_DrawRectangle = oq_DrawRectangle;

   if ((this->mq_ErrorActive == false) &&
       (this->mq_ItemActive == true))
   {
      // deactivate the background drawing only if no error is active
      this->mq_DrawBackground = oq_DrawRectangle;
      this->update();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   \param[in,out]  opc_Painter   Painter
   \param[in,out]  opc_Option    Option
   \param[in,out]  opc_Widget    Widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvEmptyItem::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                                QWidget * const opc_Widget)
{
   QPen c_Pen;
   QBrush c_Brush;
   QRectF c_Rect;
   const uint32 u32_ALIGNMENT = static_cast<uint32>(Qt::AlignRight) | static_cast<uint32>(Qt::AlignTop);

   C_SdBueMlvBaseItem::paint(opc_Painter, opc_Option, opc_Widget);

   // configure the font and color
   c_Pen = opc_Painter->pen();
   c_Brush = opc_Painter->brush();

   c_Pen.setColor(this->mc_DrawFontColor);
   opc_Painter->setPen(c_Pen);
   c_Brush.setStyle(Qt::NoBrush);
   opc_Painter->setBrush(c_Brush);

   opc_Painter->setFont(this->mc_Font);

   // draw the text
   c_Rect = this->boundingRect();
   // adapt the region
   c_Rect.setWidth(c_Rect.width() - 5.0);
   opc_Painter->drawText(c_Rect, static_cast<sintn>(u32_ALIGNMENT), this->mc_Text);

   // draw the error icon if an error was set
   if (this->mq_ErrorActive == true)
   {
      const float64 f64_PosX = (this->mc_Size.width() / 2.0) - (static_cast<float64>(this->mc_ErrorIcon.width()) / 2.0);
      const float64 f64_PosY = (this->mc_Size.height() / 2.0) -
                               (static_cast<float64>(this->mc_ErrorIcon.height()) / 2.0);
      opc_Painter->drawPixmap(QPointF(f64_PosX, f64_PosY), this->mc_ErrorIcon);
   }
}
