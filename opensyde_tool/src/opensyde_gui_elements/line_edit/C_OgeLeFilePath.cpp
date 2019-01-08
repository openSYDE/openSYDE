//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Line edit field for file paths (implementation)

   Line edit field for file paths which inherits all of C_OgeLeProperties
   with its specific stylesheet. This class sends a signal in case the
   user wants to edit the appropriate file path. The purpose is to show a
   minimized path if the focus is out of the edit field and a full path
   if the user is editing the path.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     09.07.2018  STW/D.Pohl
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeLeFilePath.h"
#include <QFileInfo>

/* -- Used Namespaces ------------------------------------------------------ */
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

   \created     09.07.2018  STW/D.Pohl
*/
//-----------------------------------------------------------------------------
C_OgeLeFilePath::C_OgeLeFilePath(QWidget * const opc_Parent) :
   C_OgeLeProperties(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten focus in event slot to emit signal for maximizing path.

   \param[in,out] opc_Event Event identification and information

   \created     09.07.2018  STW/D.Pohl
*/
//-----------------------------------------------------------------------------
void C_OgeLeFilePath::focusInEvent(QFocusEvent * const opc_Event)
{
   QLineEdit::focusInEvent(opc_Event);
   Q_EMIT this->SigFocus();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten focus out event slot to emit signal for minimizing path

   \param[in,out] opc_Event Event identification and information

   \created     09.07.2018  STW/D.Pohl
*/
//-----------------------------------------------------------------------------
void C_OgeLeFilePath::focusOutEvent(QFocusEvent * const opc_Event)
{
   QLineEdit::focusOutEvent(opc_Event);
   Q_EMIT this->SigFocusOut();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Full file path must be shown if user hovers over field.

   Hides generic interface of C_OgeToolTipBase.

   \param[in]     orc_FullFilePath    full file path

   \created     10.07.2018  STW/D.Pohl
*/
//-----------------------------------------------------------------------------
void C_OgeLeFilePath::SetToolTipInformation(const QString & orc_FullFilePath)
{
   const QString c_NoHeading = "";
   QString c_Content = "";

   // absolute path is undefined if created from empty string
   if (orc_FullFilePath != "")
   {
      const QFileInfo c_FileInfo(orc_FullFilePath);
      c_Content = c_FileInfo.absoluteFilePath();
   }
   C_OgeLeProperties::SetToolTipInformation(c_NoHeading, c_Content,
                                            stw_opensyde_gui::C_NagToolTip::eDEFAULT);
}
