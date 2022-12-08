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
#include "precomp_headers.hpp"

#include <QPainter>
#include <QPaintEvent>
#include <QSplitter>
#include <QSplitterHandle>
#include <QLayout>

#include "C_OgeSpiStandard.hpp"
#include "constants.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t ms32_HANDLE_WIDTH = 5;   //px
const int32_t ms32_HANDLE_LENGTH = 30; //px

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
   this->setHandleWidth(static_cast<int32_t>(ms32_HANDLE_WIDTH));
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
void C_OgeSpiStandard::SetMargins(const int32_t os32_MarginTop, const int32_t os32_MarginBottom) const
{
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
void C_OgeSpiStandardHandle::SetMargins(const int32_t os32_MarginTop, const int32_t os32_MarginBottom)
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
      const int32_t s32_TopVertical = this->rect().top() + this->ms32_MarginTop;
      const int32_t s32_BotVertical = this->rect().bottom() - this->ms32_MarginBottom;
      const int32_t s32_DistanceToHandle = static_cast<int32_t>((s32_BotVertical - s32_TopVertical) / 2) -
                                           static_cast<int32_t>(ms32_HANDLE_LENGTH / 2);
      const int32_t s32_HandleTopVertical = s32_TopVertical + s32_DistanceToHandle;
      const int32_t s32_HandleBotVertical = s32_BotVertical - s32_DistanceToHandle;

      // draw middle line
      c_Painter.drawLine(rect().width() / 2, s32_TopVertical, rect().width() / 2, s32_BotVertical);

      // draw left handle line
      c_Painter.drawLine(this->rect().left(), s32_HandleTopVertical, this->rect().left(), s32_HandleBotVertical);

      //draw right handle line
      c_Painter.drawLine(this->rect().right(), s32_HandleTopVertical, this->rect().right(), s32_HandleBotVertical);
   }
   else
   {
      const int32_t s32_LeftHorizontal = this->rect().left() + this->ms32_MarginTop;
      const int32_t s32_RightHorizontal = this->rect().right() - this->ms32_MarginBottom;
      const int32_t s32_DistanceToHandle = static_cast<int32_t>((s32_RightHorizontal - s32_LeftHorizontal) / 2) -
                                           static_cast<int32_t>(ms32_HANDLE_LENGTH / 2);
      const int32_t s32_HandleLeftHorizontal = s32_LeftHorizontal + s32_DistanceToHandle;
      const int32_t s32_HandleRightHorizontal = s32_RightHorizontal - s32_DistanceToHandle;

      // draw middle line
      c_Painter.drawLine(s32_LeftHorizontal, this->rect().height() / 2, s32_RightHorizontal, this->rect().height() / 2);

      // draw top handle line
      c_Painter.drawLine(s32_HandleLeftHorizontal, this->rect().top(), s32_HandleRightHorizontal, this->rect().top());

      //draw bottom handle line
      c_Painter.drawLine(s32_HandleLeftHorizontal, this->rect().bottom(), s32_HandleRightHorizontal,
                         this->rect().bottom());
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
