//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Transparent push button with hover state (implementation)

   Transparent push button with hover state

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgeWiUtil.h"
#include "C_OgePubNavigationHover.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_logic;
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
C_OgePubNavigationHover::C_OgePubNavigationHover(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Active", false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item active state

   \param[in] oq_Active Item active state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubNavigationHover::SetActive(const bool oq_Active)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Active", oq_Active);
}
