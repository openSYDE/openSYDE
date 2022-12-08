//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       C_CMONProtocols protocol adaption for openSYDE (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"

#include "C_OscComLoggerProtocols.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::scl;
using namespace stw::can;
using namespace stw::opensyde_core;
using namespace stw::cmon_protocol;

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
C_OscComLoggerProtocols::C_OscComLoggerProtocols(void) :
   C_CanMonProtocols()
{
   // Reassign the openSYDE protocol with the derived variant with system definition interpretation
   this->mapc_Protocols[CMONL7ProtocolOpenSYDE] = &mc_ProtocolOpenSydeExtended;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Converts a CAN message to a protocol specific string

   If the message does not match the configured protocol, an empty string will be returned.
   No 'Layer 2 protocol' string conversion will be made if an protocol is selected.

   \param[in]     orc_Msg        CAN Message for conversion

   \return
   Protocol specific string of CAN message
   Empty string in case of no matching message to the configured protocol
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscComLoggerProtocols::MessageToStringProtocolOnly(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SclString c_Text = "";

   if (static_cast<int32_t>(me_ActiveProtocol) < gs32_KFX_CMON_NUM_PROTOCOLS)
   {
      c_Text = mapc_Protocols[me_ActiveProtocol]->MessageToString(orc_Msg);
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds an openSYDE system definition configuration for analyzing

   \param[in]     opc_SysDefConfig               Pointer to openSYDE system definition configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComLoggerProtocols::AddOsySysDef(const C_OscComMessageLoggerOsySysDefConfig * const opc_SysDefConfig)
{
   this->mc_ProtocolOpenSydeExtended.AddOsySysDef(opc_SysDefConfig);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes an openSYDE system definition configuration

   \param[in]     opc_SysDefConfig               Pointer to openSYDE system definition configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComLoggerProtocols::RemoveOsySysDef(const C_OscComMessageLoggerOsySysDefConfig * const opc_SysDefConfig)
{
   this->mc_ProtocolOpenSydeExtended.RemoveOsySysDef(opc_SysDefConfig);
}
