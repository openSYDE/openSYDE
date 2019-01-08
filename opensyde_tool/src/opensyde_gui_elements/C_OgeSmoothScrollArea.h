//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Smooth scroll area (header)

   Smooth scroll area (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     28.09.2016  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESMOOTHSCROLLAREA_H
#define C_OGESMOOTHSCROLLAREA_H

/* -- Includes ------------------------------------------------------------- */

#include <QScrollArea>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSmoothScrollArea :
   public QScrollArea
{
   Q_OBJECT

public:
   C_OgeSmoothScrollArea(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
