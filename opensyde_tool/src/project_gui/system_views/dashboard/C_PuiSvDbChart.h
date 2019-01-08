//-----------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard chart data (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISVDBCHART_H
#define C_PUISVDBCHART_H

/* -- Includes ------------------------------------------------------------- */
#include "C_PuiSvDbWidgetBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSvDbChart :
   public C_PuiSvDbWidgetBase
{
public:
   C_PuiSvDbChart();

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const override;
   virtual bool IsReadElement(void) const;

   std::vector<bool> c_DataPoolElementsActive;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
