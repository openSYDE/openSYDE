//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for tool tip group box (header)

   Label for tool tip group box (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.10.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEGBXToolTip_H
#define C_OGEGBXToolTip_H

/* -- Includes ------------------------------------------------------------- */

#include <QGroupBox>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeGbxToolTip :
   public QGroupBox
{
   Q_OBJECT

public:
   C_OgeGbxToolTip(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
