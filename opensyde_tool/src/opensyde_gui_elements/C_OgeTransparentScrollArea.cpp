//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Transparent (except scroll bars) scroll area (implementation)

   Transparent (except scroll bars) scroll area.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QScrollBar>

#include "C_OgeTransparentScrollArea.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     24.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeTransparentScrollArea::C_OgeTransparentScrollArea(QWidget * const opc_Parent) :
   QScrollArea(opc_Parent)
{
   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Deactivating the behaviour that the scrollarea will resize if the scroll bar will be showed or hided

   \created     23.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OgeTransparentScrollArea::DeactivateScrollbarResize(void)
{
   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();

   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_OgeTransparentScrollArea::m_ScrollBarRangeChanged);
}

//-----------------------------------------------------------------------------
void C_OgeTransparentScrollArea::m_ScrollBarRangeChanged(const sintn osn_Min, const sintn osn_Max) const
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
