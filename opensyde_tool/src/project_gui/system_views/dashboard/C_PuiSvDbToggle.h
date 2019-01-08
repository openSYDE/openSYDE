//-----------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard toggle data (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISVDBTOGGLE_H
#define C_PUISVDBTOGGLE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_PuiSvDbWidgetBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSvDbToggle :
   public C_PuiSvDbWidgetBase
{
public:
   enum E_Type
   {
      eTYPE1,
      eTYPE2,
      eTYPE3
   };

   C_PuiSvDbToggle(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const override;
   virtual bool IsReadElement(void) const;

   E_Type e_Type;
   bool q_State;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
