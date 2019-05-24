//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       C_CMONProtocols protocol adaption for openSYDE (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCOMLOGGERPROTOCOLS_H
#define C_OSCCOMLOGGERPROTOCOLS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLString.h"
#include "stw_can.h"

#include "CCMONProtocol.h"
#include "C_OSCComLoggerProtocolOpenSYDE.h"
#include "C_OSCComMessageLoggerOsySysDefConfig.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCComLoggerProtocols :
   public stw_cmon_protocol::C_CMONProtocols
{
public:
   C_OSCComLoggerProtocols(void);

   stw_scl::C_SCLString MessageToStringProtocolOnly(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;

   void AddOsySysDef(const C_OSCComMessageLoggerOsySysDefConfig * const opc_SysDefConfig);
   void RemoveOsySysDef(const C_OSCComMessageLoggerOsySysDefConfig * const opc_SysDefConfig);

private:
   C_OSCComLoggerProtocolOpenSYDE mc_ProtocolOpenSYDEExtended;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
