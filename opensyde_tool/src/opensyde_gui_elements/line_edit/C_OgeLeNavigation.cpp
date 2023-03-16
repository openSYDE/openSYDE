//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit for navigation (implementation)

   Line edit for navigation.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QKeyEvent>

#include "stwtypes.hpp"
#include "C_OgeLeNavigation.hpp"

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
C_OgeLeNavigation::C_OgeLeNavigation(QWidget * const opc_Parent) :
   C_OgeLeContextMenuBase(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Signal escape event

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeNavigation::keyPressEvent(QKeyEvent * const opc_Event)
{
   if (opc_Event->key() == static_cast<int32_t>(Qt::Key_Escape))
   {
      //Signal parent
      Q_EMIT this->SigEscape();
      //Don't call base and accept to properly signal a handled escape
      opc_Event->accept();
   }
   else
   {
      C_OgeLeContextMenuBase::keyPressEvent(opc_Event);
   }
}
