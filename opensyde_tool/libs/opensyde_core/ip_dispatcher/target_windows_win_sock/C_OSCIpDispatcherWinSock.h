//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol IP driver for Windows

   \class       stw_opensyde_core::C_OSCIpDispatcherWinSock
   \brief       openSYDE protocol IP driver for Windows

   Implements target specific IP routines.
   Here: for MS Windows using WinSock

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIPDISPATCHERWINSOCKH
#define C_OSCIPDISPATCHERWINSOCKH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <map>
#include <list>
#include <winsock.h>
#include "stwtypes.h"
#include "C_OSCIpDispatcher.h"
#include "CSCLString.h"
#include "TGLTasks.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIpDispatcherWinSock :
   public C_OSCIpDispatcher
{
private:
   class C_TcpConnection
   {
   public:
      C_TcpConnection(void);

      SOCKET un_Socket;                  ///< client socket
      stw_types::uint8 au8_IpAddress[4]; ///< server's IP (remembered for reconnecting)
   };

   class C_BufferIdentifier
   {
   public:
      C_BufferIdentifier(const stw_types::uint8 ou8_ClientBusIdentifier,
                         const stw_types::uint8 ou8_ClientNodeIdentifier,
                         const stw_types::uint8 ou8_ServerBusIdentifier,
                         const stw_types::uint8 ou8_ServerNodeIdentifier) :
         u8_ClientBusIdentifier(ou8_ClientBusIdentifier),
         u8_ClientNodeIdentifier(ou8_ClientNodeIdentifier),
         u8_ServerBusIdentifier(ou8_ServerBusIdentifier),
         u8_ServerNodeIdentifier(ou8_ServerNodeIdentifier)
      {
      }

      bool operator < (const C_BufferIdentifier & orc_Cmp) const;

      const stw_types::uint8 u8_ClientBusIdentifier;
      const stw_types::uint8 u8_ClientNodeIdentifier;
      const stw_types::uint8 u8_ServerBusIdentifier;
      const stw_types::uint8 u8_ServerNodeIdentifier;
   };

   std::vector<C_TcpConnection> mc_SocketsTcp; ///< one per connection

   std::vector<SOCKET> mc_SocketsUdpClient; ///< one socket per local interface (for sending broadcasts)
   std::vector<SOCKET> mc_SocketsUdpServer; ///< one socket per local interface (for receiving responses)

   std::vector<stw_types::uint32> mc_LocalInterfaceIps; ///< IPs of local interfaces
   stw_scl::C_SCLString mc_PreferredInterfaceName;

   static std::map<C_BufferIdentifier, std::list<std::vector<stw_types::uint8> > > mhc_TcpBuffer; ///< dispatcher buffer
   static stw_tgl::C_TGLCriticalSection mhc_LockBuffer;

   stw_types::sint32 m_GetAllInstalledInterfaceIps(void);
   stw_types::sint32 m_ConnectTcp(C_TcpConnection & orc_Connection) const;
   stw_types::sint32 m_ConfigureUdpSocket(const bool oq_ServerPort, const stw_types::uint32 ou32_IpToBindTo,
                                          SOCKET & orun_Socket) const;

   static stw_scl::C_SCLString mh_IpToText(const stw_types::uint8 (&orau8_Ip)[4]);

public:
   C_OSCIpDispatcherWinSock(void);
   virtual ~C_OSCIpDispatcherWinSock(void);

   virtual stw_types::sint32 InitTcp(const stw_types::uint8 (&orau8_Ip)[4], stw_types::uint32 & oru32_Handle);
   virtual stw_types::sint32 InitUdp(void);
   virtual stw_types::sint32 IsTcpConnected(const stw_types::uint32 ou32_Handle);
   virtual stw_types::sint32 ReConnectTcp(const stw_types::uint32 ou32_Handle);

   virtual stw_types::sint32 CloseTcp(const stw_types::uint32 ou32_Handle);
   virtual stw_types::sint32 CloseUdp(void);
   virtual stw_types::sint32 SendTcp(const stw_types::uint32 ou32_Handle,
                                     const std::vector<stw_types::uint8> & orc_Data);
   virtual stw_types::sint32 ReadTcp(const stw_types::uint32 ou32_Handle, std::vector<stw_types::uint8> & orc_Data);
   virtual stw_types::sint32 ReadTcp(const stw_types::uint32 ou32_Handle,
                                     const stw_types::uint8 ou8_ClientBusIdentifier,
                                     const stw_types::uint8 ou8_ClientNodeIdentifier,
                                     const stw_types::uint8 ou8_ServerBusIdentifier,
                                     const stw_types::uint8 ou8_ServerNodeIdentifier,
                                     std::vector<stw_types::uint8> & orc_Data);
   virtual stw_types::sint32 ReadTcpBuffer(const stw_types::uint8 ou8_ClientBusIdentifier,
                                           const stw_types::uint8 ou8_ClientNodeIdentifier,
                                           const stw_types::uint8 ou8_ServerBusIdentifier,
                                           const stw_types::uint8 ou8_ServerNodeIdentifier,
                                           std::vector<stw_types::uint8> & orc_Data);
   virtual stw_types::sint32 SendUdp(const std::vector<stw_types::uint8> & orc_Data);
   virtual stw_types::sint32 ReadUdp(std::vector<stw_types::uint8> &orc_Data, stw_types::uint8(&orau8_Ip)[4]);

   void LoadConfigFile(const stw_scl::C_SCLString & orc_FileLocation);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
