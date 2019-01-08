//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for node name as title in update package (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     12.02.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABUPDATENODETITLE_H
#define C_OGELABUPDATENODETITLE_H

/* -- Includes ------------------------------------------------------------- */
#include <QLabel>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabUpdateNodeTitle :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabUpdateNodeTitle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
