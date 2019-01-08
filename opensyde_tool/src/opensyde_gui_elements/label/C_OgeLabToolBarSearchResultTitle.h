//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for toolbar search result title (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABTOOLBARSEARCHRESULTTITLE_H
#define C_OGELABTOOLBARSEARCHRESULTTITLE_H

/* -- Includes ------------------------------------------------------------- */

#include <QLabel>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabToolBarSearchResultTitle :
   public QLabel
{
   Q_OBJECT

public:
   explicit C_OgeLabToolBarSearchResultTitle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
