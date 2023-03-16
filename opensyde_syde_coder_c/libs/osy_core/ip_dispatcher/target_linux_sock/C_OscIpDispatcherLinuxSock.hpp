//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol IP driver for Linux

   \class       stw::opensyde_core::C_OSCIpDispatcherLinuxSock
   \brief       openSYDE protocol IP driver for Linux

   Implements target specific IP routines.
   Here: for Linux using BSD Socket API
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIPDISPATCHERLINUXSOCKHPP
#define C_OSCIPDISPATCHERLINUXSOCKHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <map>
#include <list>
#include "stwtypes.hpp"
#include "C_OscIpDispatcher.hpp"
#include "C_SclString.hpp"
#include "TglTasks.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscIpDispatcherLinuxSock :
   public C_OscIpDispatcher
{
private:
   class C_TcpConnection
   {
   public:
      int32_t s32_Socket;       ///< client socket
      uint8_t au8_IpAddress[4]; ///< server's IP (remembered for reconnecting)
   };

   class C_BufferIdentifier
   {
   public:
      C_BufferIdentifier(const uint8_t ou8_ClientBusIdentifier, const uint8_t ou8_ClientNodeIdentifier,
                         const uint8_t ou8_ServerBusIdentifier, const uint8_t ou8_ServerNodeIdentifier) :
         u8_ClientBusIdentifier(ou8_ClientBusIdentifier),
         u8_ClientNodeIdentifier(ou8_ClientNodeIdentifier),
         u8_ServerBusIdentifier(ou8_ServerBusIdentifier),
         u8_ServerNodeIdentifier(ou8_ServerNodeIdentifier)
      {
      }

      bool operator < (const C_BufferIdentifier & orc_Cmp) const;

      const uint8_t u8_ClientBusIdentifier;
      const uint8_t u8_ClientNodeIdentifier;
      const uint8_t u8_ServerBusIdentifier;
      const uint8_t u8_ServerNodeIdentifier;
   };

   std::vector<C_TcpConnection> mc_SocketsTcp; ///< one per connection

   std::vector<int32_t> mc_SocketsUdpClient; ///< one socket per local interface (for sending broadcasts)
   int32_t ms32_SocketUdpServer;             ///< one socket for all interfaces (for receiving responses)

   std::vector<uint32_t> mc_LocalInterfaceIps; ///< IPs of local interfaces

   static std::map<C_BufferIdentifier, std::list<std::vector<uint8_t> > > mhc_TcpBuffer; ///< dispatcher buffer
   static stw::tgl::C_TglCriticalSection mhc_LockBuffer;

   int32_t m_GetAllInstalledInterfaceIps(void);
   int32_t m_ConnectTcp(C_TcpConnection & orc_Connection) const;
   int32_t m_ConfigureUdpSocket(const bool oq_ServerPort, const uint32_t ou32_IpToBindTo, int32_t & ors32_Socket) const;

   static stw::scl::C_SclString mh_IpToText(const uint8_t (&orau8_Ip)[4]);

public:
   C_OscIpDispatcherLinuxSock(void);
   virtual ~C_OscIpDispatcherLinuxSock(void);

   virtual int32_t InitTcp(const uint8_t (&orau8_Ip)[4], uint32_t & oru32_Handle);
   virtual int32_t InitUdp(void);
   virtual int32_t IsTcpConnected(const uint32_t ou32_Handle);
   virtual int32_t ReConnectTcp(const uint32_t ou32_Handle);

   virtual int32_t CloseTcp(const uint32_t ou32_Handle);
   virtual int32_t CloseUdp(void);
   virtual int32_t SendTcp(const uint32_t ou32_Handle, const std::vector<uint8_t> & orc_Data);
   virtual int32_t ReadTcp(const uint32_t ou32_Handle, std::vector<uint8_t> & orc_Data);
   virtual int32_t ReadTcp(const uint32_t ou32_Handle, const uint8_t ou8_ClientBusIdentifier,
                           const uint8_t ou8_ClientNodeIdentifier, const uint8_t ou8_ServerBusIdentifier,
                           const uint8_t ou8_ServerNodeIdentifier, std::vector<uint8_t> & orc_Data);
   virtual int32_t ReadTcpBuffer(const uint8_t ou8_ClientBusIdentifier, const uint8_t ou8_ClientNodeIdentifier,
                                 const uint8_t ou8_ServerBusIdentifier, const uint8_t ou8_ServerNodeIdentifier,
                                 std::vector<uint8_t> & orc_Data);
   virtual int32_t SendUdp(const std::vector<uint8_t> & orc_Data);
   virtual int32_t ReadUdp(std::vector<uint8_t> &orc_Data, uint8_t(&orau8_Ip)[4]);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
