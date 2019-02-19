//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Base combo box class with resizing view functionality (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.11.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECBXRESIZINGVIEW_H
#define C_OGECBXRESIZINGVIEW_H

/* -- Includes ------------------------------------------------------------- */
#include <QComboBox>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeCbxResizingView :
   public QComboBox
{
public:
   C_OgeCbxResizingView(QWidget * const opc_Parent = NULL);

   void ResizeViewToContents(void) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
