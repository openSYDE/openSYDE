//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Menu for recent projects selection (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     18.01.2019  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMOGEMURECENTPROJECTS_H
#define C_CAMOGEMURECENTPROJECTS_H

/* -- Includes ------------------------------------------------------------- */
#include <QMenu>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamOgeMuRecentProjects :
   public QMenu
{
   Q_OBJECT

public:
   explicit C_CamOgeMuRecentProjects(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
