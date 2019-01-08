//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom group box transparent (header)

   Custom group box (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.07.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEGBXTRANSPARENT_H
#define C_OGEGBXTRANSPARENT_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeGbxToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeGbxTransparent :
   public C_OgeGbxToolTipBase
{
   Q_OBJECT

public:
   C_OgeGbxTransparent(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
