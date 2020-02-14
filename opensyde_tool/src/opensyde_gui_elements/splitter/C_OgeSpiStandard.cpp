//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standard splitter (implementation)

   Standard splitter with three-lined-look, where the middle line is as wide as the widgets.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>
#include <QPaintEvent>
#include <QSplitter>
#include <QSplitterHandle>
#include <QLayout>

#include "C_OgeSpiStandard.h"
#include "constants.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const sint32 ms32_HANDLE_WIDTH = 5;   //px
const sint32 ms32_HANDLE_LENGTH = 30; //px

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSpiStandard::C_OgeSpiStandard(QWidget * const opc_Parent) :
   C_OgeSpiBase(opc_Parent)
{
   this->setHandleWidth(static_cast<sintn>(ms32_HANDLE_WIDTH));
   this->setChildrenCollapsible(false);

   this->setContentsMargins(0, 0, 0, 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the drawing color

   \param[in]  orc_Color   New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiStandard::SetColor(const QColor & orc_Color) const
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   // Index 0 is always is hidden, take number 1
   C_OgeSpiStandardHandle * const pc_Handle = dynamic_cast<C_OgeSpiStandardHandle *>(this->handle(1));

   if (pc_Handle != NULL)
   {
      pc_Handle->SetColor(orc_Color);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set margins

   \param[in]  os32_MarginTop       Margin top (resp. left if handle is horizontal)
   \param[in]  os32_MarginBottom    Margin bottom (resp. right if handle is horizontal)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiStandard::SetMargins(const sint32 os32_MarginTop, const sint32 os32_MarginBottom) const
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   // Index 0 is always is hidden, take number 1
   C_OgeSpiStandardHandle * const pc_Handle = dynamic_cast<C_OgeSpiStandardHandle *>(this->handle(1));

   if (pc_Handle != NULL)
   {
      pc_Handle->SetMargins(os32_MarginTop, os32_MarginBottom);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      oe_Orientation   Orientation (horizontal/vertical)
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSpiStandardHandle::C_OgeSpiStandardHandle(const Qt::Orientation oe_Orientation, QSplitter * const opc_Parent) :
   QSplitterHandle(oe_Orientation, opc_Parent),
   ms32_MarginTop(9),
   ms32_MarginBottom(9)
{
   this->SetColor(mc_STYLE_GUIDE_COLOR_10);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the drawing color

   \param[in]  orc_Color   New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiStandardHandle::SetColor(const QColor & orc_Color)
{
   this->mc_Color = orc_Color;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set margins

   \param[in]  os32_MarginTop       Margin top (resp. left if handle is horizontal)
   \param[in]  os32_MarginBottom    Margin bottom (resp. right if handle is horizontal)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiStandardHandle::SetMargins(const sint32 os32_MarginTop, const sint32 os32_MarginBottom)
{
   this->ms32_MarginTop = os32_MarginTop;
   this->ms32_MarginBottom = os32_MarginBottom;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event

   Draws the background element

   \param[in,out]  opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiStandardHandle::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   QPainter c_Painter(this);
   c_Painter.setPen(this->mc_Color);
   if (orientation() == Qt::Horizontal)
   {
      const sint32 s32_TopY = this->rect().top() + this->ms32_MarginTop;
      const sint32 s32_BotY = this->rect().bottom() - this->ms32_MarginBottom;
      const sint32 s32_DistanceToHandle = static_cast<sint32>((s32_BotY - s32_TopY) / 2) -
                                          static_cast<sint32>(ms32_HANDLE_LENGTH / 2);
      const sint32 s32_HandleTopY = s32_TopY + s32_DistanceToHandle;
      const sint32 s32_HandleBotY = s32_BotY - s32_DistanceToHandle;

      // draw middle line
      c_Painter.drawLine(rect().width() / 2, s32_TopY, rect().width() / 2, s32_BotY);

      // draw left handle line
      c_Painter.drawLine(this->rect().left(), s32_HandleTopY, this->rect().left(), s32_HandleBotY);

      //draw right handle line
      c_Painter.drawLine(this->rect().right(), s32_HandleTopY, this->rect().right(), s32_HandleBotY);
   }
   else
   {
      const sint32 s32_LeftX = this->rect().left() + this->ms32_MarginTop;
      const sint32 s32_RightX = this->rect().right() - this->ms32_MarginBottom;
      const sint32 s32_DistanceToHandle = static_cast<sint32>((s32_RightX - s32_LeftX) / 2) -
                                          static_cast<sint32>(ms32_HANDLE_LENGTH / 2);
      const sint32 s32_HandleLeftX = s32_LeftX + s32_DistanceToHandle;
      const sint32 s32_HandleRightX = s32_RightX - s32_DistanceToHandle;

      // draw middle line
      c_Painter.drawLine(s32_LeftX, this->rect().height() / 2, s32_RightX, this->rect().height() / 2);

      // draw top handle line
      c_Painter.drawLine(s32_HandleLeftX, this->rect().top(), s32_HandleRightX, this->rect().top());

      //draw bottom handle line
      c_Painter.drawLine(s32_HandleLeftX, this->rect().bottom(), s32_HandleRightX, this->rect().bottom());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create splitter handle for custom handle

   \return
   splitter handle
*/
//----------------------------------------------------------------------------------------------------------------------
QSplitterHandle * C_OgeSpiStandard::createHandle(void)
{
   return new C_OgeSpiStandardHandle(this->orientation(), this);
}
