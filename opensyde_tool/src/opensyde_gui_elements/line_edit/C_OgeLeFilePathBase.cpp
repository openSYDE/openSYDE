//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for line edit field with file paths (implementation)

   This class shows a minimized path if the focus is out of the edit field and a full
   path if the user is editing the path. Furthermore the complete and absolute path is
   shown in tool tip (path variables are resolved for tooltips).

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFileInfo>
#include <QDir>

#include "C_Uti.h"
#include "constants.h"
#include "C_OgeLeFilePathBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
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
C_OgeLeFilePathBase::C_OgeLeFilePathBase(QWidget * const opc_Parent) :
   C_OgeLeToolTipBase(opc_Parent)
{
   this->ms32_LastKnownCursorPos = -1;
   this->mc_Path = "";
   this->mc_RelativeTo = "";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setter for full path and tool tip resolving relative path.

   For appropriate QLineEdit the minimized path is shown.

   \param[in] orc_New            New value
   \param[in] orc_RelativeTo     Optional directory where path is relative to if it is relative (must be absolute)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::SetPath(const QString & orc_New, const QString & orc_RelativeTo)
{
   const QDir c_Dir(this->m_ResolveVariables(orc_RelativeTo));

   // text
   this->mc_Path = orc_New;
   this->mc_Path.replace("\\", "/"); // replace backslashes
   this->m_UpdateMinimizing();

   // tool tip
   const QFileInfo c_FileInfo(c_Dir, this->m_ResolveVariables(this->mc_Path));
   this->mc_RelativeTo = c_Dir.absolutePath();
   if (this->mc_Path != "")
   {
      C_OgeLeToolTipBase::SetToolTipInformation("", c_FileInfo.absoluteFilePath(),
                                                stw_opensyde_gui::C_NagToolTip::eDEFAULT);
   }
   else
   {
      C_OgeLeToolTipBase::SetToolTipInformation("", "", stw_opensyde_gui::C_NagToolTip::eDEFAULT);
   }

   // make sure last known cursor position is not out of range
   this->ms32_LastKnownCursorPos = std::min(static_cast<stw_types::sintn>(this->ms32_LastKnownCursorPos),
                                            this->mc_Path.size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current path

   \return
   Current path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeLeFilePathBase::GetPath(void) const
{
   return this->mc_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Insert selected path variable in front of string.

   \param[in]       orc_Variable     path variable
   \param[in]       sn_Position      index of position where to insert variable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::InsertVariable(const QString & orc_Variable)
{
   if (this->ms32_LastKnownCursorPos > 0)
   {
      mc_Path.insert(this->ms32_LastKnownCursorPos, orc_Variable);
   }
   else
   {
      // insert at the front if last known position is invalid
      mc_Path.insert(0, orc_Variable);
   }

   // update visualization
   this->SetPath(this->mc_Path, this->mc_RelativeTo);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown text based on the current set path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::UpdateText(void)
{
   this->setText(QString(this->mc_Path));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden focus in event slot

   Here: show full path

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::focusInEvent(QFocusEvent * const opc_Event)
{
   this->setText(QString(this->mc_Path));
   C_OgeLeToolTipBase::focusInEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden focus out event slot

   Here: show minimized path if necessary

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::focusOutEvent(QFocusEvent * const opc_Event)
{
   // remember cursor position for insert functionality
   this->ms32_LastKnownCursorPos = this->cursorPosition();

   // remember complete path  (before focus out to avoid confusion of editingFinished)
   this->SetPath(this->text(), this->mc_RelativeTo);

   C_OgeLeToolTipBase::focusOutEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden key press event slot

   Here: Set new path on enter or return key press.
   This is analogue to normal line edit but often not needed because of following focus loose.

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   if ((opc_KeyEvent->key() == static_cast<stw_types::sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<stw_types::sintn>(Qt::Key_Return)))
   {
      this->SetPath(this->text(), this->mc_RelativeTo);
   }

   // after update of complete path
   C_OgeLeToolTipBase::keyPressEvent(opc_KeyEvent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden show event slot

   Here: Reload minimized path

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::showEvent(QShowEvent * const opc_Event)
{
   C_OgeLeToolTipBase::showEvent(opc_Event);
   this->m_UpdateMinimizing();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden resize event.

   Here: Recalculate minimized path on resize.

   \param[in]     opc_Event      event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::resizeEvent(QResizeEvent * const opc_Event)
{
   C_OgeLeToolTipBase::resizeEvent(opc_Event);
   this->m_UpdateMinimizing();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calls replace logic for path variables.

   Needs to be wrapped because of different path variables in different projects.
   But line edit must know resolve-stuff to adapt tool tip on editing finished and focus out.
   Default implementation does nothing.

   \param[in]       orc_Path           path that probably contains variables
   \param[in]       orc_DbProjectPath  optional path to replace special variable (Data Block project)

   \return
   Resolved path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeLeFilePathBase::m_ResolveVariables(const QString & orc_Path)
{
   return orc_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update minimized path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::m_UpdateMinimizing()
{
   if (this->hasFocus() == false)
   {
      const QFont c_Font = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_13);
      this->setText(C_Uti::h_MinimizePath(this->mc_Path, c_Font, this->width(), 12));
   }
}
