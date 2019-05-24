//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view base class with extra scrollbar feature (implementation)

   Table view base class that shows/hides scrollbars without resizing the widget.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QScrollBar>
#include "C_TblViewScroll.h"
#include "constants.h"

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

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblViewScroll::C_TblViewScroll(QWidget * const opc_Parent) :
   C_TblViewToolTipBase(opc_Parent)
{
   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->horizontalScrollBar()->hide();
   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();

   //Icon
   this->setIconSize(mc_ICON_SIZE_24);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   //Connects
   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_TblViewScroll::m_ScrollBarRangeChangedVer);
   connect(this->horizontalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_TblViewScroll::m_ScrollBarRangeChangedHor);
}

//----------------------------------------------------------------------------------------------------------------------
void C_TblViewScroll::m_ScrollBarRangeChangedVer(const sintn osn_Min, const sintn osn_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((osn_Min == 0) && (osn_Max == 0))
   {
      this->verticalScrollBar()->hide();
   }
   else
   {
      this->verticalScrollBar()->show();
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_TblViewScroll::m_ScrollBarRangeChangedHor(const sintn osn_Min, const sintn osn_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((osn_Min == 0) && (osn_Max == 0))
   {
      this->horizontalScrollBar()->hide();
   }
   else
   {
      this->horizontalScrollBar()->show();
   }
}
