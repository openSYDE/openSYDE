//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group box for miscellaneous navigation section (implementation)

   Group box for miscellaneous navigation section.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeWiUtil.hpp"
#include "C_OgeGbxNavigationSection.hpp"

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
C_OgeGbxNavigationSection::C_OgeGbxNavigationSection(QWidget * const opc_Parent) :
   QGroupBox(opc_Parent)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "SpecialBackground", false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set special background active

   \param[in] oq_Active Special background active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeGbxNavigationSection::SetSpecialBackground(const bool oq_Active)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "SpecialBackground", oq_Active);
}
