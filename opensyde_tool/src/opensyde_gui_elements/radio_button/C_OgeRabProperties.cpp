//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Properties radio button style (implementation)

   Properties radio button style.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeWiUtil.hpp"
#include "C_OgeRabProperties.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
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
C_OgeRabProperties::C_OgeRabProperties(QWidget * const opc_Parent) :
   C_OgeRabToolTipBase(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Configures the stylesheet for the receivers variant of this type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeRabProperties::SetStyleVariantReceivers(void)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Margin", 3);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "IndicatorMargin", 4);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "CheckedColor", 4);
}
