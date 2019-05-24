//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Vertical splitter for navi bar (implementation)

   Vertical splitter for navi bar

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>
#include "constants.h"
#include "C_UsHandler.h"
#include "C_OgeSpiVerticalNavi.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSpiVerticalNavi::C_OgeSpiVerticalNavi(QWidget * const opc_Parent) :
   C_OgeSpiBase(opc_Parent),
   mq_Loaded(false),
   msn_LastNaviBarSize(0)
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
   this->SetFirstSegment(std::max(250, static_cast<sintn>(C_UsHandler::h_GetInstance()->GetNaviBarSize())));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store size in user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiVerticalNavi::StoreUserSettings(void) const
{
   if (this->mq_Loaded == true)
   {
      sintn sn_NewVal;

      if (this->sizes().at(0) > 0)
      {
         sn_NewVal = this->sizes().at(0);
      }
      else
      {
         sn_NewVal = this->msn_LastNaviBarSize;
      }
      C_UsHandler::h_GetInstance()->SetNaviBarSize(std::max(250, sn_NewVal));
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
      this->SetFirstSegment(std::max(250, this->msn_LastNaviBarSize));
   }
   else
   {
      //Not collapsed
      this->msn_LastNaviBarSize = std::max(250, this->sizes().at(0));
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

   \param[in,out] opc_Parent Optional pointer to parent
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

   \param[in,out] opc_Event Event identification and information
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

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiVerticalNaviHandle::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   if (orientation() == Qt::Horizontal)
   {
      const stw_types::sintn sn_HandleHeight = 20;
      const QPoint c_PTopCenter = this->rect().center() + QPoint(0, sn_HandleHeight / 2);
      const QPoint c_PBottomCenter = this->rect().center() - QPoint(0, sn_HandleHeight / 2);
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
      c_Painter.drawLine(c_PTopCenter, c_PBottomCenter);
      //Others
      c_Painter.drawLine(c_PTopCenter + QPoint(2, 0), c_PBottomCenter + QPoint(2, 0));
      c_Painter.drawLine(c_PTopCenter - QPoint(2, 0), c_PBottomCenter - QPoint(2, 0));
   }
   else
   {
      //not relevant
   }
}
