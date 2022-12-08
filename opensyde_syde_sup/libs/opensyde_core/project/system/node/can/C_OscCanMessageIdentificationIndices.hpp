//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to store all necessary indices to identify a message stored inside a node (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANMESSAGEIDENTIFICATIONINDICES_H
#define C_OSCCANMESSAGEIDENTIFICATIONINDICES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <ostream>
#include "stwtypes.hpp"
#include "C_OscCanProtocol.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanMessageIdentificationIndices
{
public:
   C_OscCanMessageIdentificationIndices(void);
   C_OscCanMessageIdentificationIndices(const uint32_t ou32_NodeIndex, const C_OscCanProtocol::E_Type oe_ComProtocol,
                                        const uint32_t ou32_InterfaceIndex, const uint32_t ou32_DatapoolIndex,
                                        const bool oq_MessageIsTx, const uint32_t ou32_MessageIndex = 0U);
   bool operator ==(const C_OscCanMessageIdentificationIndices & orc_Cmp) const;

   uint32_t u32_NodeIndex;                 ///< Node index
   C_OscCanProtocol::E_Type e_ComProtocol; ///< Com protocol
   uint32_t u32_InterfaceIndex;            ///< Interface index
   uint32_t u32_DatapoolIndex;             ///< Datapool index
   bool q_MessageIsTx;                     ///< Flag if message is tx, false: rx assumed
   uint32_t u32_MessageIndex;              ///< Message index
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
