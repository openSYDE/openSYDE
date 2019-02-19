//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Line edit field for file paths with automated handling for too long paths (visibility only) (implementation)

   Line edit field for file paths with automated handling for too long paths (visibility only).
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.10.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_Uti.h"
#include "constants.h"
#include "C_OgeLeFilePathWrapper.h"
#include <QFileInfo>
#include <QDir>

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     04.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeLeFilePathWrapper::C_OgeLeFilePathWrapper(QWidget * const opc_Parent) :
   C_OgeLeFilePath(opc_Parent)
{
   connect(this, &C_OgeLeFilePathWrapper::SigFocus, this, &C_OgeLeFilePathWrapper::m_FocusOn);
   connect(this, &C_OgeLeFilePathWrapper::SigFocusOut, this, &C_OgeLeFilePathWrapper::m_FocusOff);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Setter for full path and tool tip resolving relative path.

   For appropriate QLineEdit the minimized path is shown.

   \param[in] orc_New         New value
   \param[in] orc_RelativeTo  Optional directory where path is relative to if it is relative

   \created     04.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLeFilePathWrapper::SetPath(const QString & orc_New, const QString & orc_RelativeTo)
{
   const QFont c_Font = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_13);
   const QString c_MinimizedPath = C_Uti::h_MinimizePath(orc_New, c_Font, this->width(),
                                                         12); // left and right padding of QLineEdit in LineEdit.qss
   const QDir c_Dir(orc_RelativeTo);
   const QFileInfo c_FileInfo(c_Dir, orc_New);

   // text
   this->mc_CompletePath = orc_New;
   this->setText(c_MinimizedPath);

   // tool tip
   this->mc_RelativeTo = c_Dir.absolutePath();
   // show tool tip only if there is a path given
   if (orc_New != "")
   {
      this->SetToolTipInformation(c_FileInfo.absoluteFilePath());
   }
   else
   {
      this->SetToolTipInformation("");
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current path

   \return
   Current path

   \created     04.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_OgeLeFilePathWrapper::GetCompletePath(void) const
{
   return this->mc_CompletePath;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot to show up full file path if user wants to edit path manually.

   \created     04.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLeFilePathWrapper::m_FocusOn(void)
{
   this->setText(QString(this->mc_CompletePath));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot to show up minimized file path if user exits from manual editing.

   \created     04.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLeFilePathWrapper::m_FocusOff(void)
{
   this->SetPath(this->text(), this->mc_RelativeTo);
}
