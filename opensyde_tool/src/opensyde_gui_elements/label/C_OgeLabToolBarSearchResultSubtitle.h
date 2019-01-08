//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for toolbar search result subtitle (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABTOOLBARSEARCHRESULTSUBTITLE_H
#define C_OGELABTOOLBARSEARCHRESULTSUBTITLE_H

/* -- Includes ------------------------------------------------------------- */

#include <QLabel>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabToolBarSearchResultSubtitle :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabToolBarSearchResultSubtitle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
