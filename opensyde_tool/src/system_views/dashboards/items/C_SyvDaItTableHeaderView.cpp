//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom header view for table for tables used in proxy widgets

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QApplication>

#include "C_SyvDaItTableHeaderView.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     ore_Orientation Orientation
   \param[in,out] opc_Parent      Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItTableHeaderView::C_SyvDaItTableHeaderView(const Qt::Orientation & ore_Orientation,
                                                   QWidget * const opc_Parent) :
   C_SdNdeDpListTableHeaderView(ore_Orientation, opc_Parent),
   mq_CursorHandlingActive(true),
   mq_CursorChanged(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItTableHeaderView::~C_SyvDaItTableHeaderView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the cursor handling active

   If the cursor handling is active the SplitHCursor cursor will be set when necessary by setOverrideCursor
   and restored when not

   \param[in]       oq_Active     Flag if cursor handling is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTableHeaderView::SetCursorHandlingActive(const bool oq_Active)
{
   this->mq_CursorHandlingActive = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten event slot

   Here: handling of defect cursor change

   If a header view is placed in a proxy widget in a scene, the SplitHCursor cursor will not be adapted correctly.
   The correct events will be triggered, but the cursor will not be changed.
   This event implementation adapts the cursor when the necessary event is detected.

   One left problem: If the user hovers with the mouse from above or beyond the indicator into the widget, it can
   happen that the cursor is adapted by an other layer before the CursorChange is triggered in this class.
   This will cause that the SplitHCursor will not be reseted if the user does not leave the header widget.

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItTableHeaderView::event(QEvent * const opc_Event)
{
   if (this->mq_CursorHandlingActive == true)
   {
      if (opc_Event->type() == QEvent::CursorChange)
      {
         if (this->mq_CursorChanged == false)
         {
            this->mq_CursorChanged = true;
            QApplication::setOverrideCursor(Qt::SplitHCursor);
         }
         else
         {
            this->mq_CursorChanged = false;
            QApplication::restoreOverrideCursor();
         }
      }
      else if (opc_Event->type() == QEvent::HoverLeave)
      {
         this->mq_CursorChanged = false;
         QApplication::restoreOverrideCursor();
      }
      else
      {
         // Nothing to do
      }
   }

   return C_SdNdeDpListTableHeaderView::event(opc_Event);
}
