//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Small label for status informations (header)

   (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.02.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABSTATUSINFORMATIONSMALL_H
#define C_OGELABSTATUSINFORMATIONSMALL_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeLabToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabStatusInformationSmall :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabStatusInformationSmall(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
