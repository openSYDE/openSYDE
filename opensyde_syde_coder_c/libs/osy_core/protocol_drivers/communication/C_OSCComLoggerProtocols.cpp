//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       C_CMONProtocols protocol adaption for openSYDE (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"

#include "C_OSCComLoggerProtocols.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_scl;
using namespace stw_can;
using namespace stw_opensyde_core;
using namespace stw_cmon_protocol;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//-----------------------------------------------------------------------------
C_OSCComLoggerProtocols::C_OSCComLoggerProtocols(void) :
   C_CMONProtocols()
{
   // Reassign the openSYDE protocol with the derived variant with system definition interpretation
   this->mapc_Protocols[CMONL7ProtocolOpenSYDE] = &mc_ProtocolOpenSYDEExtended;
}

//-----------------------------------------------------------------------------
/*! \brief   Converts a CAN message to a protocol specific string

   If the message does not match the configured protocol, an empty string will be returned.
   No 'Layer 2 protocol' string conversion will be made if an protocol is selected.

   \param[in]     orc_Msg        CAN Message for conversion

   \return
   Protocol specific string of CAN message
   Empty string in case of no matching message to the configured protocol
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCComLoggerProtocols::MessageToStringProtocolOnly(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SCLString c_Text = "";

   if (static_cast<sint32>(me_ActiveProtocol) < gs32_KFX_CMON_NUM_PROTOCOLS)
   {
      c_Text = mapc_Protocols[me_ActiveProtocol]->MessageToString(orc_Msg);
   }

   return c_Text;
}

//-----------------------------------------------------------------------------
/*! \brief   Adds an openSYDE system definition configuration for analyzing

   \param[in]     opc_SysDefConfig               Pointer to openSYDE system definition configuration
*/
//-----------------------------------------------------------------------------
void C_OSCComLoggerProtocols::AddOsySysDef(const C_OSCComMessageLoggerOsySysDefConfig * const opc_SysDefConfig)
{
   this->mc_ProtocolOpenSYDEExtended.AddOsySysDef(opc_SysDefConfig);
}

//-----------------------------------------------------------------------------
/*! \brief   Removes an openSYDE system definition configuration

   \param[in]     opc_SysDefConfig               Pointer to openSYDE system definition configuration
*/
//-----------------------------------------------------------------------------
void C_OSCComLoggerProtocols::RemoveOsySysDef(const C_OSCComMessageLoggerOsySysDefConfig * const opc_SysDefConfig)
{
   this->mc_ProtocolOpenSYDEExtended.RemoveOsySysDef(opc_SysDefConfig);
}
