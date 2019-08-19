//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Styled line edit field for file paths (implementation)

   Line edit field for file paths which has a look very similar to C_OgeLeProperties
   but with right border styled for seamless transition to browse-button.
   Furthermore it inherits some path features like path minimizing.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_PuiUtil.h"
#include "C_OgeLeFilePath.h"

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
C_OgeLeFilePath::C_OgeLeFilePath(QWidget * const opc_Parent) :
   C_OgeLeFilePathBase(opc_Parent)
{
   this->mc_DbProjectPath = "";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set path for replacing Data Block project variable

   To update tool tip make sure to call SetPath after this!

   \param[in]   orc_DbProjectPath   path to replace special variable (Data Block project)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePath::SetDbProjectPath(const QString & orc_DbPath)
{
   this->mc_DbProjectPath = orc_DbPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calls replace logic for path variables.

   Needs to be wrapped because of different path variables in different projects.

   \param[in]       orc_Path           path that probably contains variables

   \return
   Resolved path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeLeFilePath::m_ResolveVariables(const QString & orc_Path)
{
   return C_PuiUtil::h_ResolvePlaceholderVariables(orc_Path, this->mc_DbProjectPath);
}
