//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box for font (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECBXFONTPROPERTIES_H
#define C_OGECBXFONTPROPERTIES_H

/* -- Includes ------------------------------------------------------------- */
#include <QFontComboBox>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeCbxFontProperties :
   public QFontComboBox
{
   Q_OBJECT

public:
   C_OgeCbxFontProperties(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
