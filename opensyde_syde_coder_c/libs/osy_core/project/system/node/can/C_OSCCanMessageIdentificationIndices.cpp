//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to store all necessary indices to identify a message stored inside a node (implementation)

   Utility class to store all necessary indices to identify a message stored inside a node

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_OSCCanMessageIdentificationIndices.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanMessageIdentificationIndices::C_OSCCanMessageIdentificationIndices(void) :
   u32_NodeIndex(0U),
   e_ComProtocol(C_OSCCanProtocol::eLAYER2),
   u32_InterfaceIndex(0U),
   u32_DatapoolIndex(0U),
   q_MessageIsTx(false),
   u32_MessageIndex(0U)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor to initialize all members at once

   \param[in] ou32_NodeIndex      Node index
   \param[in] oe_ComProtocol      Com protocol
   \param[in] ou32_InterfaceIndex Interface index
   \param[in] ou32_DatapoolIndex  Datapool index
   \param[in] oq_MessageIsTx      Flag if message is tx, false: rx assumed
   \param[in] ou32_MessageIndex   Message index
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanMessageIdentificationIndices::C_OSCCanMessageIdentificationIndices(const uint32 ou32_NodeIndex,
                                                                           const C_OSCCanProtocol::E_Type oe_ComProtocol, const uint32 ou32_InterfaceIndex, const uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                                                           const uint32 ou32_MessageIndex) :
   u32_NodeIndex(ou32_NodeIndex),
   e_ComProtocol(oe_ComProtocol),
   u32_InterfaceIndex(ou32_InterfaceIndex),
   u32_DatapoolIndex(ou32_DatapoolIndex),
   q_MessageIsTx(oq_MessageIsTx),
   u32_MessageIndex(ou32_MessageIndex)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compare two indices

   \param[in] orc_Cmp Message identification indices to compare with

   \return
   Current equal to orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCCanMessageIdentificationIndices::operator ==(const C_OSCCanMessageIdentificationIndices & orc_Cmp) const
{
   return (((((orc_Cmp.u32_NodeIndex == this->u32_NodeIndex) && (orc_Cmp.e_ComProtocol == this->e_ComProtocol)) &&
             (orc_Cmp.u32_InterfaceIndex == this->u32_InterfaceIndex) &&
             (orc_Cmp.u32_DatapoolIndex == this->u32_DatapoolIndex)) &&
            (orc_Cmp.q_MessageIsTx == this->q_MessageIsTx)) && (orc_Cmp.u32_MessageIndex == this->u32_MessageIndex));
}
