//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Push button styled for can monitor title bar (implementation)

   Push button styled for can monitor title bar (project operations).
   Emits a signal on hover for synchronizing buttons hover looks.
   This class does not contain much functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QEvent>
#include <QSvgRenderer>
#include <QPainter>

#include "C_CamOgePubProjOp.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamOgePubProjOp::C_CamOgePubProjOp(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent),
   mc_SvgIcon(""),
   mq_IconOnly(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set svg icon.

   \param[in]     orc_Path        Path to svg Icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgePubProjOp::SetSvg(const QString & orc_Path)
{
   this->mc_SvgIcon = orc_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set flag if button is icon-only.

   \param[in]     oq_IconOnly      true: only icon; false: icon and text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgePubProjOp::SetIconOnly(const bool oq_IconOnly)
{
   this->mq_IconOnly = oq_IconOnly;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event.

   Here: padding between text and icon

   \param[in]     opc_PaintEvent     Event identification and information.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgePubProjOp::paintEvent(QPaintEvent * const opc_PaintEvent)
{
   // background & text
   C_OgePubToolTipBase::paintEvent(opc_PaintEvent);

   if (this->mc_SvgIcon != "")
   {
      QPainter c_Painter(this);
      QRect c_Rect;
      QSvgRenderer c_Renderer(this->mc_SvgIcon);

      if (this->mq_IconOnly == true)
      {
         // center icon
         c_Rect = QRect(this->rect().topLeft() +
                        QPoint((this->rect().width() - this->iconSize().width()) / 2,
                               (this->rect().height() - this->iconSize().height()) / 2), this->iconSize());
      }
      else
      {
         // center vertically and put 10 pixel between icon and right border
         c_Rect = QRect(this->rect().topLeft() +
                        QPoint(10, (this->rect().height() - this->iconSize().height()) / 2), this->iconSize());
      }

      c_Renderer.render(&c_Painter, c_Rect);
   }
}
