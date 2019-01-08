//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom group box in read only look (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEGBXREAD_H
#define C_OGEGBXREAD_H

/* -- Includes ------------------------------------------------------------- */
#include <QGroupBox>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeGbxRead :
   public QGroupBox
{
   Q_OBJECT

public:
   C_OgeGbxRead(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
