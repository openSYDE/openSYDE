//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to openSYDE interpretation (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCOMLOGGERPROTOCOLOPENSYDE_HPP
#define C_OSCCOMLOGGERPROTOCOLOPENSYDE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <list>

#include "C_SclString.hpp"

#include "C_CanMonProtocolOpenSyde.hpp"
#include "C_OscComMessageLoggerOsySysDefConfig.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscComLoggerProtocolOpenSyde :
   public stw::cmon_protocol::C_CanMonProtocolOpenSyde
{
public:
   C_OscComLoggerProtocolOpenSyde(void);

   void AddOsySysDef(const C_OscComMessageLoggerOsySysDefConfig * const opc_SysDefConfig);
   void RemoveOsySysDef(const C_OscComMessageLoggerOsySysDefConfig * const opc_SysDefConfig);

protected:
   virtual stw::scl::C_SclString m_AddressInformationToText(const T_CanAddressInformation & orc_CanAddressInformation)
   const;
   virtual stw::scl::C_SclString m_DataPoolIdentifierToText(const uint32_t ou32_DataPoolIdentifier,
                                                            const bool oq_IsResponse,
                                                            const T_CanAddressInformation & orc_CanAddressInformation)
   const;

   std::list<const C_OscComMessageLoggerOsySysDefConfig *> mc_SysDefConfigs;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
