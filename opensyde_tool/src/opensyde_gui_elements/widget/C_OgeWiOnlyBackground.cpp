//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with a simple background (implementation)

   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgeWiOnlyBackground.h"

#include "C_OgeWiUtil.h"

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
C_OgeWiOnlyBackground::C_OgeWiOnlyBackground(QWidget * const opc_Parent) :
   C_OgeWiWithToolTip(opc_Parent)
{
   this->SetBackgroundColor(0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set background color (stylesheet color index)

   \param[in]     osn_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiOnlyBackground::SetBackgroundColor(const stw_types::sintn osn_Value)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Background", osn_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiOnlyBackground::paintEvent(QPaintEvent * const opc_Event)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);

   QWidget::paintEvent(opc_Event);
}
