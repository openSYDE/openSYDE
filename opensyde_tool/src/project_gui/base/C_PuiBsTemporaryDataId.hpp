//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for identifying data temporarily  (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUIBSTEMPORARYDATAID_H
#define C_PUIBSTEMPORARYDATAID_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <exception>
#include "stwtypes.hpp"
#include "C_PuiSdDataElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsTemporaryDataId
{
public:
   C_PuiBsTemporaryDataId(const int32_t & ors32_Type, const uint32_t & oru32_Index);

   bool operator <(const C_PuiBsTemporaryDataId & orc_Other) const;

   int32_t s32_Type;
   uint32_t u32_Index;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
