//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Styled line edit field for file paths (implementation)

   Line edit field for file paths which has its right border styled for seamless transition to browse-button.
   Furthermore it inherits some path features like path minimizing.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_CamUti.h"
#include "C_CamOgeLeFilePath.h"

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
C_CamOgeLeFilePath::C_CamOgeLeFilePath(QWidget * const opc_Parent) :
   C_OgeLeFilePathBase(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calls replace logic for path variables.

   Needs to be wrapped because of different path variables in different projects.

   \param[in]       orc_Path           path that probably contains variables

   \return
   Resolved path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamOgeLeFilePath::m_ResolveVariables(const QString & orc_Path)
{
   return C_CamUti::h_ResolvePlaceholderVariables(orc_Path);
}
