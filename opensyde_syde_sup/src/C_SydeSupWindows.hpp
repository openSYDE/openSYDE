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
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_Can.hpp"
#include "C_OscIpDispatcherWinSock.hpp"
#include "C_SydeSup.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SydeSupWindows :
   public C_SydeSup
{
public:
   C_SydeSupWindows(void);
   virtual ~C_SydeSupWindows(void);

private:
   virtual void m_CloseCan(void);
   virtual E_Result m_OpenCan(const stw::scl::C_SclString & orc_CanDriver, const uint64_t ou64_BitrateBps);
   virtual E_Result m_OpenEthernet(void);
   virtual stw::scl::C_SclString m_GetApplicationVersion(const stw::scl::C_SclString & orc_ApplicationFileName) const;
   virtual stw::scl::C_SclString m_GetDefaultLogLocation(void) const;
   virtual stw::scl::C_SclString m_GetCanInterfaceUsageExample(void) const;

   bool mq_CanDllLoaded;
   stw::can::C_Can mc_CanDispatcher;
   stw::opensyde_core::C_OscIpDispatcherWinSock mc_EthDispatcher;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#endif
