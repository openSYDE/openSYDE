//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom group box grayed (header)

   Custom group box grayed (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.07.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEGBXGRAYED_H
#define C_OGEGBXGRAYED_H

/* -- Includes ------------------------------------------------------------- */

#include <QGroupBox>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeGbxGrayed :
   public QGroupBox
{
   Q_OBJECT

public:
   C_OgeGbxGrayed(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
