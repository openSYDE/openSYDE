//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Vertical splitter for navi bar (implementation)

   Vertical splitter for navi bar

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>
#include "constants.hpp"
#include "C_UsHandler.hpp"
#include "C_OgeSpiVerticalNavi.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

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
C_OgeSpiVerticalNavi::C_OgeSpiVerticalNavi(QWidget * const opc_Parent) :
   C_OgeSpiBase(opc_Parent),
   mq_Loaded(false),
   ms32_LastNaviBarSize(0)
{
   this->setHandleWidth(9);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore size from user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiVerticalNavi::LoadUserSettings(void)
{
   this->mq_Loaded = true;
   this->SetFirstSegment(std::max(250, static_cast<int32_t>(C_UsHandler::h_GetInstance()->GetNaviBarSize())));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store size in user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiVerticalNavi::StoreUserSettings(void) const
{
   if (this->mq_Loaded == true)
   {
      int32_t s32_NewVal;

      if (this->sizes().at(0) > 0)
      {
         s32_NewVal = this->sizes().at(0);
      }
      else
      {
         s32_NewVal = this->ms32_LastNaviBarSize;
      }
      C_UsHandler::h_GetInstance()->SetNaviBarSize(std::max(250, s32_NewVal));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle splitter handle double click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiVerticalNavi::m_OnDoubleClick(void)
{
   if (this->sizes().at(0) == 0)
   {
      //Collapsed
      this->SetFirstSegment(std::max(250, this->ms32_LastNaviBarSize));
   }
   else
   {
      //Not collapsed
      this->ms32_LastNaviBarSize = std::max(250, this->sizes().at(0));
      this->SetFirstSegment(0);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create splitter handle for custom handle

   \return
   splitter handle
*/
//----------------------------------------------------------------------------------------------------------------------
QSplitterHandle * C_OgeSpiVerticalNavi::createHandle(void)
{
   //technically it is a horizontal splitter
   //but it is better for understanding to call it vertical, because actually the handle is meant and it is has vertical
   // orientation
   C_OgeSpiVerticalNaviHandle * const pc_Retval = new C_OgeSpiVerticalNaviHandle(Qt::Horizontal, this);

   connect(pc_Retval, &C_OgeSpiVerticalNaviHandle::SigDoubleClick, this,
           &C_OgeSpiVerticalNavi::m_OnDoubleClick);

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      oe_Orientation   Orientation (vertical/horizontal)
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSpiVerticalNaviHandle::C_OgeSpiVerticalNaviHandle(const Qt::Orientation oe_Orientation,
                                                       QSplitter * const opc_Parent) :
   QSplitterHandle(oe_Orientation, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten double click event slot

   Here: forward double click

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiVerticalNaviHandle::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   QSplitterHandle::mouseDoubleClickEvent(opc_Event);
   Q_EMIT this->SigDoubleClick();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided paint event

   Draws the background element

   \param[in,out]  opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiVerticalNaviHandle::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   if (orientation() == Qt::Horizontal)
   {
      const int32_t s32_HANDLE_HEIGHT = 20;
      const QPoint c_PointTopCenter = this->rect().center() + QPoint(0, s32_HANDLE_HEIGHT / 2);
      const QPoint c_PointBottomCenter = this->rect().center() - QPoint(0, s32_HANDLE_HEIGHT / 2);
      QPainter c_Painter(this);
      //Background
      c_Painter.setPen(QPen(mc_STYLE_GUIDE_COLOR_1353, 1.0));
      c_Painter.setBrush(mc_STYLE_GUIDE_COLOR_5);
      //No idea why but these adjustments are necessary
      c_Painter.drawRect(this->rect().adjusted(0, 0, -1, -1));
      //Lines
      c_Painter.setPen(mc_STYLE_GUIDE_COLOR_10);
      c_Painter.setBrush(Qt::transparent);
      //Center
      c_Painter.drawLine(c_PointTopCenter, c_PointBottomCenter);
      //Others
      c_Painter.drawLine(c_PointTopCenter + QPoint(2, 0), c_PointBottomCenter + QPoint(2, 0));
      c_Painter.drawLine(c_PointTopCenter - QPoint(2, 0), c_PointBottomCenter - QPoint(2, 0));
   }
   else
   {
      //not relevant
   }
}
