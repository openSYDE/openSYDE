//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for line edit with placeholder variables support (implementation)

   This class shows the resolved string in tooltip. Not suitable for paths, but for lineedits that contain more than a
   single path like e.g. a call to a executable with flags. Also it has a style without right border for
   seamless transition to a button for placeholder-variables-menu.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_PuiUtil.hpp"
#include "C_OgeLePlaceholderVar.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
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
C_OgeLePlaceholderVar::C_OgeLePlaceholderVar(QWidget * const opc_Parent) :
   C_OgeLeToolTipBase(opc_Parent),
   ms32_LastKnownCursorPos(-1),
   mc_DbProjectPath("")
{
   connect(this, &C_OgeLePlaceholderVar::editingFinished, this, &C_OgeLePlaceholderVar::m_UpdateTooltip);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Insert selected placeholder variable in front of string.

   \param[in]  orc_Variable   placeholder variable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLePlaceholderVar::InsertVariable(const QString & orc_Variable)
{
   QString c_Text = this->text();

   if (this->ms32_LastKnownCursorPos > 0)
   {
      c_Text.insert(this->ms32_LastKnownCursorPos, orc_Variable);
   }
   else
   {
      // insert at the front if last known position is invalid
      c_Text.insert(0, orc_Variable);
   }

   // update visualization
   this->setText(c_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set path for replacing Data Block project variable needed for tooltip

   \param[in]  orc_DbPath  path to replace special variable (Data Block project)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLePlaceholderVar::SetDbProjectPath(const QString & orc_DbPath)
{
   this->mc_DbProjectPath = orc_DbPath;
   this->m_UpdateTooltip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden focus out event slot

   Here: remember last known cursor position

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLePlaceholderVar::focusOutEvent(QFocusEvent * const opc_Event)
{
   // remember cursor position for insert functionality
   this->ms32_LastKnownCursorPos = this->cursorPosition();

   C_OgeLeToolTipBase::focusOutEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calls replace logic for placeholder variables and updates tooltip.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLePlaceholderVar::m_UpdateTooltip(void)
{
   this->SetToolTipInformation(C_GtGetText::h_GetText("Path"),
                               C_PuiUtil::h_ResolvePlaceholderVariables(this->text(), this->mc_DbProjectPath));
}
