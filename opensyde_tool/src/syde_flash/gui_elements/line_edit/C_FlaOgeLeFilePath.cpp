//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       SydeFlash variant of file path line edit widget

   SydeFlash variant of file path line edit widget

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_FlaUti.hpp"
#include "C_FlaOgeLeFilePath.hpp"

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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaOgeLeFilePath::C_FlaOgeLeFilePath(QWidget * const opc_Parent) :
   C_OgeLeFilePathBase(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calls replace logic for path variables.

   Needs to be wrapped because of different path variables in different projects.

   \param[in]  orc_Path    path that probably contains variables

   \return
   Resolved path
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{715} false positive: orc_Path is referenced in a call, but somehow PC Lint does not get this
QString C_FlaOgeLeFilePath::m_ResolveVariables(const QString & orc_Path)
{
   return C_FlaUti::h_ResolvePlaceholderVariables(orc_Path);
}
