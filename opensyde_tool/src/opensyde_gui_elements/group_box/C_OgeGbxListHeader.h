//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom group box with list header style (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     02.06.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEGBXLISTHEADER_H
#define C_OGEGBXLISTHEADER_H

/* -- Includes ------------------------------------------------------------- */
#include <QGroupBox>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeGbxListHeader :
   public QGroupBox
{
   Q_OBJECT

public:
   C_OgeGbxListHeader(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
