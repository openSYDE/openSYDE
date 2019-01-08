//-----------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard label data (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISVDBLABEL_H
#define C_PUISVDBLABEL_H

/* -- Includes ------------------------------------------------------------- */
#include <QString>
#include "C_PuiSvDbWidgetBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSvDbLabel :
   public C_PuiSvDbWidgetBase
{
public:
   enum E_Type
   {
      eDEFAULT,
      eTRANSPARENT
   };

   C_PuiSvDbLabel(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   virtual bool IsReadElement(void) const;

   QString c_Caption;
   E_Type e_Type;
   bool q_ShowCaption;
   bool q_ShowUnit;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
