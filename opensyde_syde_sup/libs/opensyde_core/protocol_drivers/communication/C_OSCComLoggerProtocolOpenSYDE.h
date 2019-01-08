//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to openSYDE interpretation (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.10.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCCOMLOGGERPROTOCOLOPENSYDE_H
#define C_OSCCOMLOGGERPROTOCOLOPENSYDE_H

/* -- Includes ------------------------------------------------------------- */
#include <list>

#include "CSCLString.h"

#include "CCMONProtocolOpenSYDE.h"
#include "C_OSCComMessageLoggerOsySysDefConfig.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCComLoggerProtocolOpenSYDE :
   public stw_cmon_protocol::C_CMONProtocolOpenSYDE
{
public:
   C_OSCComLoggerProtocolOpenSYDE(void);

   void AddOsySysDef(const C_OSCComMessageLoggerOsySysDefConfig * const opc_SysDefConfig);
   void RemoveOsySysDef(const C_OSCComMessageLoggerOsySysDefConfig * const opc_SysDefConfig);

protected:
   virtual stw_scl::C_SCLString m_AddressInformationToText(const T_CanAddressInformation & ort_CanAddressInformation)
   const;
   virtual stw_scl::C_SCLString m_DataPoolIdentifierToText(const stw_types::uint32 ou32_DataPoolIdentifier,
                                                           const bool oq_IsResponse,
                                                           const T_CanAddressInformation & ort_CanAddressInformation)
   const;

   std::list<const C_OSCComMessageLoggerOsySysDefConfig *> mc_SysDefConfigs;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
