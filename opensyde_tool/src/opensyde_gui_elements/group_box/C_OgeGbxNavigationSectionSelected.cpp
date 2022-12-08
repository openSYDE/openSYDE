//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group box style for selectable navigation section

   Group box style for selectable navigation section.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeWiUtil.hpp"
#include "C_OgeGbxNavigationSectionSelected.hpp"

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
C_OgeGbxNavigationSectionSelected::C_OgeGbxNavigationSectionSelected(QWidget * const opc_Parent) :
   QGroupBox(opc_Parent)
{
   this->SetActive(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set active state for this group box

   \param[in] oq_Active Active state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeGbxNavigationSectionSelected::SetActive(const bool oq_Active)
{
   stw::opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Active", oq_Active);
}
