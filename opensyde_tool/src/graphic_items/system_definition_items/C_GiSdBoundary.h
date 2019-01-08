//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element boundary in system definition (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GISDBOUNDARY_H
#define C_GISDBOUNDARY_H

/* -- Includes ------------------------------------------------------------- */
#include "C_GiBiBoundary.h"
#include "C_PuiSdDataElement.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiSdBoundary :
   public C_GiBiBoundary,
   public stw_opensyde_gui_logic::C_PuiSdDataElement
{
public:
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiSdBoundary(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                  const stw_types::float64 of64_Width = mhf64_MinWidthBoundary,
                  const stw_types::float64 of64_Height = mhf64_MinHeightBoundary,
                  QGraphicsItem * const opc_Parent = NULL);

   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
