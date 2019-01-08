//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit for displaying content of tooltips (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.07.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETEDTOOLTIPCONTENT_H
#define C_OGETEDTOOLTIPCONTENT_H

/* -- Includes ------------------------------------------------------------- */
#include <QTextEdit>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTedToolTipContent :
   public QTextEdit
{
   Q_OBJECT

public:
   C_OgeTedToolTipContent(QWidget * const opc_Parent = NULL);

private:
   void m_AdjustMinimumSize();
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
