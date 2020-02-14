//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label that emits a signal on double click (implementation)

   Label that emits a signal on double click.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgeLabDoubleClick.h"

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
C_OgeLabDoubleClick::C_OgeLabDoubleClick(QWidget * const opc_Parent) :
   C_OgeLabGenericNoPaddingNoMargins(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten double click event

   Open version edit on double click

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabDoubleClick::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   Q_EMIT this->SigDoubleClicked();
   C_OgeLabGenericNoPaddingNoMargins::mouseDoubleClickEvent(opc_Event);
}
