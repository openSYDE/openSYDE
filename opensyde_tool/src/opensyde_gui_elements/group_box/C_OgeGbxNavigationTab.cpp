//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group box for navigation tab style (implementation)

   Group box for navigation tab style.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeWiUtil.hpp"
#include "C_OgeGbxNavigationTab.hpp"

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
C_OgeGbxNavigationTab::C_OgeGbxNavigationTab(QWidget * const opc_Parent) :
   QGroupBox(opc_Parent)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "IncludeLeftBorder", false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set include left border flag

   \param[in] oq_Include Include left border flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeGbxNavigationTab::SetIncludeLeftBorder(const bool oq_Include)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "IncludeLeftBorder", oq_Include);
}
