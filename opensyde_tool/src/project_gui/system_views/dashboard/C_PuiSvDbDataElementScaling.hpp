//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for data element scaling information (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBDATAELEMENTSCALING_HPP
#define C_PUISVDBDATAELEMENTSCALING_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbDataElementScaling
{
public:
   C_PuiSvDbDataElementScaling(void);
   virtual ~C_PuiSvDbDataElementScaling();

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   bool q_UseDefault;
   float64_t f64_Factor; ///< Adaptation factor for display
   float64_t f64_Offset; ///< Offset factor for display
   QString c_Unit;       ///< Variable unit
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
