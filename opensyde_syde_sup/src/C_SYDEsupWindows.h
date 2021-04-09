//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Windows class of SYDEsup (header)

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYDESUPWINDOWSH
#define C_SYDESUPWINDOWSH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"
#include "CCAN.h"
#include "C_OSCIpDispatcherWinSock.h"
#include "C_SYDEsup.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SYDEsupWindows :
   public C_SYDEsup
{
public:
   C_SYDEsupWindows(void);
   virtual ~C_SYDEsupWindows(void);

private:
   virtual void m_CloseCan(void);
   virtual E_Result m_OpenCan(const stw_scl::C_SCLString & orc_CanDriver, const stw_types::uint64 ou64_BitrateBps);
   virtual E_Result m_OpenEthernet(void);
   virtual stw_scl::C_SCLString m_GetApplicationVersion(const stw_scl::C_SCLString & orc_ApplicationFileName) const;
   virtual stw_scl::C_SCLString m_GetDefaultLogLocation(void) const;
   virtual stw_scl::C_SCLString m_GetCanInterfaceUsageExample(void) const;

   bool mq_CanDllLoaded;
   stw_can::C_CAN mc_CanDispatcher;
   stw_opensyde_core::C_OSCIpDispatcherWinSock mc_EthDispatcher;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#endif
