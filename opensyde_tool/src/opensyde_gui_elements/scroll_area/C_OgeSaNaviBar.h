//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Scroll area for navigation bar (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESANAVIBAR_H
#define C_OGESANAVIBAR_H

/* -- Includes ------------------------------------------------------------- */
#include <QScrollArea>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSaNaviBar :
   public QScrollArea
{
   Q_OBJECT

public:
   explicit C_OgeSaNaviBar(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
