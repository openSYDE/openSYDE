//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for data element scaling information (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISVDBDATAELEMENTSCALING_H
#define C_PUISVDBDATAELEMENTSCALING_H

/* -- Includes ------------------------------------------------------------- */
#include <QString>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSvDbDataElementScaling
{
public:
   C_PuiSvDbDataElementScaling(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   bool q_UseDefault;
   stw_types::float64 f64_Factor; ///< Adaptation factor for display
   stw_types::float64 f64_Offset; ///< Offset factor for display
   QString c_Unit;                ///< Variable unit
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
