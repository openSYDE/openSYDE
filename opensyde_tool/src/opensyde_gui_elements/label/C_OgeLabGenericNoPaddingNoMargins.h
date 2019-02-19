//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Generic label styled via setters (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABGENERICNOPADDINGNOMARGINS_H
#define C_OGELABGENERICNOPADDINGNOMARGINS_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeLabToolTipBase.h"

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabGenericNoPaddingNoMargins :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabGenericNoPaddingNoMargins(QWidget * const opc_Parent = NULL);

   void SetForegroundColor(const stw_types::sintn osn_Value);
   void SetBackgroundColor(const stw_types::sintn osn_Value);
   void SetFontPixel(const stw_types::sintn osn_Value, const bool oq_SemiBold = false, const bool oq_Bold = false);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
