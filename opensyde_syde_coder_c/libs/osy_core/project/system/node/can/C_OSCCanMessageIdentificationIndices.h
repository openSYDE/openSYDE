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
#include "stwtypes.h"
#include "C_OSCCanProtocol.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCCanMessageIdentificationIndices
{
public:
   C_OSCCanMessageIdentificationIndices(void);
   C_OSCCanMessageIdentificationIndices(const stw_types::uint32 ou32_NodeIndex,
                                        const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                        const stw_types::uint32 ou32_InterfaceIndex,
                                        const stw_types::uint32 ou32_DatapoolIndex,
                                        const bool oq_MessageIsTx,
                                        const stw_types::uint32 ou32_MessageIndex = 0U);
   bool operator ==(const C_OSCCanMessageIdentificationIndices & orc_Cmp) const;

   stw_types::uint32 u32_NodeIndex;        ///< Node index
   C_OSCCanProtocol::E_Type e_ComProtocol; ///< Com protocol
   stw_types::uint32 u32_InterfaceIndex;   ///< Interface index
   stw_types::uint32 u32_DatapoolIndex;    ///< Datapool index
   bool q_MessageIsTx;                     ///< Flag if message is tx, false: rx assumed
   stw_types::uint32 u32_MessageIndex;     ///< Message index
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
