//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Combo box styled for smaller widget usage (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.01.2019  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMOGECBXTABLESMALL_H
#define C_CAMOGECBXTABLESMALL_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeCbxResizingView.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamOgeCbxTableSmall :
   public C_OgeCbxResizingView
{
   Q_OBJECT

public:
   explicit C_CamOgeCbxTableSmall(QWidget * const opc_Parent = NULL);
   ~C_CamOgeCbxTableSmall(void);

private:
   //Avoid call
   C_CamOgeCbxTableSmall(const C_CamOgeCbxTableSmall &);
   C_CamOgeCbxTableSmall & operator =(const C_CamOgeCbxTableSmall &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
