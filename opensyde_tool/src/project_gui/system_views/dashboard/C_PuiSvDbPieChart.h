//-----------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard pie chart data (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     28.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISVDBPIECHART_H
#define C_PUISVDBPIECHART_H

/* -- Includes ------------------------------------------------------------- */
#include "C_PuiSvDbWidgetBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSvDbPieChart :
   public C_PuiSvDbWidgetBase
{
public:
   C_PuiSvDbPieChart(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const override;
   virtual bool IsReadElement(void) const;

   bool q_ShowUnit;
   bool q_ShowValue;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
