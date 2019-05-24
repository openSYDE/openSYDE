//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom transparent group box for dashboard (implementation)

   Custom transparent group box for dashboard

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgeGbxTransparentDashboard.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
C_OgeGbxTransparentDashboard::C_OgeGbxTransparentDashboard(QWidget * const opc_Parent) :
   QGroupBox(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: signal resize

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeGbxTransparentDashboard::resizeEvent(QResizeEvent * const opc_Event)
{
   QGroupBox::resizeEvent(opc_Event);
   Q_EMIT this->SigResized();
}
