//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom transparent tristate error check box (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECHXTRISTATETRANSPARENTERROR_H
#define C_OGECHXTRISTATETRANSPARENTERROR_H

/* -- Includes ------------------------------------------------------------- */

#include <QCheckBox>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeChxTristateTransparentError :
   public QCheckBox
{
   Q_OBJECT

public:
   C_OgeChxTristateTransparentError(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
