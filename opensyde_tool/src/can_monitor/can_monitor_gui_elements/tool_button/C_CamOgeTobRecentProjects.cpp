//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tool button for open project or showing recent projects. (implementation)

   Tool button for open project or showing recent projects.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QSvgRenderer>
#include <QPainter>

#include "C_CamOgeTobRecentProjects.hpp"

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
C_CamOgeTobRecentProjects::C_CamOgeTobRecentProjects(QWidget * const opc_Parent) :
   C_OgeTobToolTipBase(opc_Parent),
   mc_SvgIcon("")
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set svg icon.

   \param[in]     orc_Path        Path to svg Icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeTobRecentProjects::SetSvg(const QString & orc_Path)
{
   this->mc_SvgIcon = orc_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event.

   Here: padding between text and icon

   \param[in]     opc_PaintEvent     Event identification and information.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeTobRecentProjects::paintEvent(QPaintEvent * const opc_PaintEvent)
{
   // background & text
   C_OgeTobToolTipBase::paintEvent(opc_PaintEvent);

   if (this->mc_SvgIcon != "")
   {
      QPainter c_Painter(this);
      QRect c_Rect;
      QSvgRenderer c_Renderer(this->mc_SvgIcon);

      c_Rect = QRect(this->rect().topLeft() + QPoint(10, 3), this->iconSize());

      c_Renderer.render(&c_Painter, c_Rect);
   }
}
