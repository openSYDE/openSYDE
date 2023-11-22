//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       C_CMONProtocols protocol adaption for openSYDE (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCOMLOGGERPROTOCOLS_HPP
#define C_OSCCOMLOGGERPROTOCOLS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"
#include "stw_can.hpp"

#include "C_CanMonProtocol.hpp"
#include "C_OscComLoggerProtocolOpenSyde.hpp"
#include "C_OscComMessageLoggerOsySysDefConfig.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscComLoggerProtocols :
   public stw::cmon_protocol::C_CanMonProtocols
{
public:
   C_OscComLoggerProtocols(void);

   stw::scl::C_SclString MessageToStringProtocolOnly(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;

   void AddOsySysDef(const C_OscComMessageLoggerOsySysDefConfig * const opc_SysDefConfig);
   void RemoveOsySysDef(const C_OscComMessageLoggerOsySysDefConfig * const opc_SysDefConfig);

private:
   C_OscComLoggerProtocolOpenSyde mc_ProtocolOpenSydeExtended;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
