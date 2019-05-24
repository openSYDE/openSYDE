//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom context menu (implementation)

   Custom context menu

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QFile>
#include <QDesktopWidget>
#include <QApplication>
#include "C_OgeContextMenu.h"
#include "constants.h"
#include "C_OgeWiUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_types;

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
C_OgeContextMenu::C_OgeContextMenu(QWidget * const opc_Parent) :
   QMenu(opc_Parent)
{
   this->setWindowFlags(this->windowFlags() | Qt::NoDropShadowWindowHint | Qt::FramelessWindowHint);
   this->setAttribute(Qt::WA_TranslucentBackground, true);

   //shadow
   {
      QColor c_Color;
      QGraphicsDropShadowEffect * pc_Shadow  = new QGraphicsDropShadowEffect(this);

      pc_Shadow->setBlurRadius(4.0);
      pc_Shadow->setOffset(4.0);
      // note: to avoid error messages like
      // UpdateLayeredWindowIndirect failed for ptDst=(2631, 561), size=(203x178), dirty=(223x198 -7, -7) (Falscher
      // Parameter.)
      // make sure blur-radius <= offset (the "-7" in above example is the difference of blur and offset and maybe
      // cannot be negative)
      // compare to https://bugreports.qt.io/browse/QTBUG-58602
      c_Color = mc_STYLE_GUIDE_COLOR_31;
      c_Color.setAlpha(50); // lower than for tool-tip because blur-radius is needed to be small
      pc_Shadow->setColor(c_Color);
      this->setGraphicsEffect(pc_Shadow);
      //lint -e{429}  no memory leak because of the parent of pc_Shadow and the Qt memory management
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeContextMenu::~C_OgeContextMenu()
{
}
