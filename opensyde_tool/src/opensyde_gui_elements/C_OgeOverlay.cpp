//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Overlay for background to concentrate focus on widget in foreground (implementation)

   Overlay for background to concentrate focus on widget in foreground

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>
#include "C_OgeOverlay.h"
#include "C_OgeWiUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

QWidget * C_OgeOverlay::mhpc_TopWidget = NULL;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent            Optional pointer to parent
   \param[in]     opc_EffectTargetChild Optional pointer to child widget of dialog to apply shadow effect to
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeOverlay::C_OgeOverlay(QWidget * const opc_Parent, QWidget * const opc_EffectTargetChild) :
   QDialog(opc_Parent)
{
   setAutoFillBackground(false);
   setWindowFlags(Qt::FramelessWindowHint);
   setAttribute(Qt::WA_TranslucentBackground);
   if
   //lint -e{1938} Global variable access necessary in this case
   ((mhpc_TopWidget != NULL) && (opc_EffectTargetChild == NULL)) // no effect target given
   {
      //lint -e{1938} Global variable access necessary in this case
      QPoint c_ParentPos = mhpc_TopWidget->mapToGlobal(mhpc_TopWidget->pos());
      //lint -e{1938} Global variable access necessary in this case
      this->setParent(mhpc_TopWidget);
      //lint -e{1938} Global variable access necessary in this case
      this->setGeometry(c_ParentPos.x(),
                        c_ParentPos.y(),
                        mhpc_TopWidget->width(), mhpc_TopWidget->height());
      this->move(0, 0);
   }
   else // child of effect target given (--> need to find parent of all parents)
   {
      //find root
      QWidget * const pc_ParentOfAllParents = C_OgeWiUtil::h_GetWidgetUnderNextPopUp(opc_EffectTargetChild);
      if (pc_ParentOfAllParents != NULL)
      {
         QPoint c_ParentPos = pc_ParentOfAllParents->mapToGlobal(pc_ParentOfAllParents->pos());
         this->setParent(pc_ParentOfAllParents);
         this->setGeometry(c_ParentPos.x(),
                           c_ParentPos.y(),
                           pc_ParentOfAllParents->width(), pc_ParentOfAllParents->height());
         this->move(0, 0);
      }
   }

   //Window icon
   C_OgeWiUtil::h_SetWindowIcon(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set master for overlay

   \param[in]  opc_TopWidget
*/
//----------------------------------------------------------------------------------------------------------------------

void C_OgeOverlay::h_SetTopWidget(QWidget * const opc_TopWidget)
{
   mhpc_TopWidget = opc_TopWidget;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: Paint transparent overlay

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------

void C_OgeOverlay::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);

   c_Painter.setPen(Qt::NoPen);
   c_Painter.setBrush(QColor(200, 200, 200, 200));
   c_Painter.drawRect(0, 0, this->width(), this->height());
   Q_UNUSED(opc_Event)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get top widget

   \return
   Top widget
*/
//----------------------------------------------------------------------------------------------------------------------
const QWidget * C_OgeOverlay::h_GetTopWidget(void)
{
   return mhpc_TopWidget;
}
