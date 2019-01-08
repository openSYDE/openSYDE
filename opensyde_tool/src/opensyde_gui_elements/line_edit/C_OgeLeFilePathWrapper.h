//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field for file paths with automated handling for too long paths (visibility only) (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.10.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELEFILEPATHWRAPPER_H
#define C_OGELEFILEPATHWRAPPER_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeLeFilePath.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLeFilePathWrapper :
   public C_OgeLeFilePath
{
   Q_OBJECT

public:
   explicit C_OgeLeFilePathWrapper(QWidget * const opc_Parent = NULL);

   void SetPath(const QString & orc_New, const QString & orc_RelativeTo = "");
   QString GetCompletePath(void) const;

private:
   QString mc_CompletePath;
   QString mc_RelativeTo;

   void m_FocusOn(void);
   void m_FocusOff(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
