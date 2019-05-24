//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field for file paths with minimizing file path handling (implementation)

   Line edit field for file paths which inherits all of C_OgeLeProperties with its
   specific stylesheet. This class shows a minimized path if the focus is out of
   the edit field and a full path if the user is editing the path. Furthermore the
   complete and absolute path is shown in tool tip.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_Uti.h"
#include "constants.h"
#include "C_OgeLeFilePath.h"
#include <QFileInfo>
#include <QDir>

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
C_OgeLeFilePath::C_OgeLeFilePath(QWidget * const opc_Parent) :
   C_OgeLeProperties(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setter for full path and tool tip resolving relative path.

   For appropriate QLineEdit the minimized path is shown.

   \param[in] orc_New         New value
   \param[in] orc_RelativeTo  Optional directory where path is relative to if it is relative (must be absolute)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePath::SetPath(const QString & orc_New, const QString & orc_RelativeTo)
{
   const QFont c_Font = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_13);
   const QDir c_Dir(orc_RelativeTo);

   // text
   this->mc_Path = QDir::cleanPath(orc_New);
   if (this->hasFocus() == false)
   {
      this->setText(C_Uti::h_MinimizePath(this->mc_Path, c_Font, this->width(), 12));
   }

   // tool tip
   const QFileInfo c_FileInfo(c_Dir, this->mc_Path);
   this->mc_RelativeTo = c_Dir.absolutePath();
   C_OgeLeProperties::SetToolTipInformation("", c_FileInfo.absoluteFilePath(),
                                            stw_opensyde_gui::C_NagToolTip::eDEFAULT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current path

   \return
   Current path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeLeFilePath::GetPath(void) const
{
   return this->mc_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden focus in event slot

   Here: show full path

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePath::focusInEvent(QFocusEvent * const opc_Event)
{
   this->setText(QString(this->mc_Path));
   C_OgeLeProperties::focusInEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden focus out event slot

   Here: show minimized path if necessary

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePath::focusOutEvent(QFocusEvent * const opc_Event)
{
   this->SetPath(this->text(), this->mc_RelativeTo); // before focus out to avoid confusion of editingFinished
   C_OgeLeProperties::focusOutEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden key press event slot

   Here: Set new path on enter or return key press.
   This is analogue to normal line edit but often not needed because of following focus loose.

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePath::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   if ((opc_KeyEvent->key() == static_cast<stw_types::sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<stw_types::sintn>(Qt::Key_Return)))
   {
      this->SetPath(this->text(), this->mc_RelativeTo);
   }

   // after update of complete path
   C_OgeLeProperties::keyPressEvent(opc_KeyEvent);
}
