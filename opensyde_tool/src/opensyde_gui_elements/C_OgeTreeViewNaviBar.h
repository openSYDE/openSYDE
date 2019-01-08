//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view for navigation bar (header)

   Tree view for navigation bar (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     31.01.2017  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETREEVIEWNAVIBAR_H
#define C_OGETREEVIEWNAVIBAR_H

/* -- Includes ------------------------------------------------------------- */

#include <QTreeWidget>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTreeViewNaviBar :
   public QTreeWidget
{
   Q_OBJECT

public:
   C_OgeTreeViewNaviBar(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
