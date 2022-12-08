//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol IP driver for Windows

   For details cf. documentation in .h file.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <winsock2.h>
#include <winsock.h> //Windows WinSock
#include <cstring>
#include <iphlpapi.h> //Windows IP helper utilities
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscIpDispatcherWinSock.hpp"
#include "TglFile.hpp"
#include "C_SclString.hpp"
#include "C_SclIniFile.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::scl;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

//export a few WinSock constants here
//Purpose: the definitions in the Windows headers violate a lot of coding rules.
//So we can relocate the required suppressions here at a central spot ...
//The violations do not cause problems in the application as long as we are on a Windows platform.
static SOCKET m_WsInvalidSocket(void)
{
   return INVALID_SOCKET; //lint !e9130
}

//----------------------------------------------------------------------------------------------------------------------
static int32_t m_WsFionBio(void)
{
   return FIONBIO; //lint !e569 !e970 !e1924 !e9105 !e9112 !e9128 !e9130 !e9136
}

//----------------------------------------------------------------------------------------------------------------------
static int32_t m_WsFionRead(void)
{
   return FIONREAD; //lint !e970 !e1924 !e9112 !e9128 !e9130 !e9136
}

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
std::map<C_OscIpDispatcherWinSock::C_BufferIdentifier,
         std::list<std::vector<uint8_t> > > C_OscIpDispatcherWinSock::mhc_TcpBuffer;
C_TglCriticalSection C_OscIpDispatcherWinSock::mhc_LockBuffer;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor

   Initialize elements
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscIpDispatcherWinSock::C_TcpConnection::C_TcpConnection(void)
{
   x_Socket = m_WsInvalidSocket();
   (void)std::memset(&au8_IpAddress[0], 0U, 4U);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current smaller than orc_Cmp

   \param[in] orc_Cmp Compared instance

   \return
   Current smaller than orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscIpDispatcherWinSock::C_BufferIdentifier::operator <(
   const C_OscIpDispatcherWinSock::C_BufferIdentifier & orc_Cmp) const
{
   bool q_Return;

   if (this->u8_ServerBusIdentifier < orc_Cmp.u8_ServerBusIdentifier)
   {
      q_Return = true;
   }
   else if (this->u8_ServerBusIdentifier == orc_Cmp.u8_ServerBusIdentifier)
   {
      if (this->u8_ServerNodeIdentifier < orc_Cmp.u8_ServerNodeIdentifier)
      {
         q_Return = true;
      }
      else if (this->u8_ServerNodeIdentifier == orc_Cmp.u8_ServerNodeIdentifier)
      {
         if (this->u8_ClientBusIdentifier < orc_Cmp.u8_ClientBusIdentifier)
         {
            q_Return = true;
         }
         else if (this->u8_ClientBusIdentifier == orc_Cmp.u8_ClientBusIdentifier)
         {
            if (this->u8_ClientNodeIdentifier < orc_Cmp.u8_ClientNodeIdentifier)
            {
               q_Return = true;
            }
            else
            {
               q_Return = false;
            }
         }
         else
         {
            q_Return = false;
         }
      }
      else
      {
         q_Return = false;
      }
   }
   else
   {
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set up class

   Initialize class elements
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscIpDispatcherWinSock::C_OscIpDispatcherWinSock(void) :
   C_OscIpDispatcher()
{
   WSADATA c_Data;
   const int x_Result =                               //lint !e8080 !e970 //using type to match library interface
                        WSAStartup(0x0201U, &c_Data); //Request version 2.1

   this->mc_PreferredInterfaceNames.Clear();

   if (x_Result != 0)
   {
      throw std::runtime_error("WSAStartup failed");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up class

   Release allocated resources
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscIpDispatcherWinSock::~C_OscIpDispatcherWinSock(void)
{
   //make sure to release resources in case the user forgot to
   for (uint16_t u16_Index = 0U; u16_Index < this->mc_SocketsTcp.size(); u16_Index++)
   {
      (void)this->C_OscIpDispatcherWinSock::CloseTcp(u16_Index);
   }
   (void)this->C_OscIpDispatcherWinSock::CloseUdp();

   (void)WSACleanup(); //spec: one call of cleanup per call of startup; final call performs actual cleanup
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize TCP communication

   Jobs to perform:
   - create one non-blocking TCP client socket

   Strategy for "book keeping":
   * new connections will always be added at the end
   * closing connection closes the handle bus does not reduce the list of handles
   * so we don't have any trouble with existing handles getting invalid

   \param[in]     orau8_Ip      IP address of server to connect to
   \param[out]    oru32_Handle  handle to new TCP connection (to be used in subsequent calls of TCP functions)

   \return
   C_NO_ERR   Connection created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscIpDispatcherWinSock::InitTcp(const uint8_t (&orau8_Ip)[4], uint32_t & oru32_Handle)
{
   C_TcpConnection c_NewConnection;

   (void)memcpy(&c_NewConnection.au8_IpAddress[0], &orau8_Ip[0], 4U);
   c_NewConnection.x_Socket = m_WsInvalidSocket();

   this->mc_SocketsTcp.push_back(c_NewConnection);
   oru32_Handle = static_cast<uint32_t>(mc_SocketsTcp.size() - 1U);

   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: get IP of all local interfaces

   Gets list of IP addresses of all local IP interfaces.
   Does not report "localhost".
   Results will be placed in mc_LocalInterfaceIps

   If mc_PreferredInterfaceName is not an empty string, only IPs of an adapter with this name will be used.

   \return
   C_NO_ERR     IPs listed
   C_NOACT      could not get IP information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscIpDispatcherWinSock::m_GetAllInstalledInterfaceIps(void)
{
   int32_t s32_Return = C_NOACT;
   PIP_ADAPTER_ADDRESSES pc_Addresses;
   uint32_t u32_RetVal;
   //lint -e{8080} //using type provided by the library for compatibility
   ULONG x_AddressesBufLen = 0U;

   mc_LocalInterfaceIps.resize(0);

   // Make an initial call to GetAdaptersAddresses to get
   // the necessary size into the u32_OutBufLen variable
   GetAdaptersAddresses(AF_INET, 0U, NULL, NULL, &x_AddressesBufLen);
   pc_Addresses = new IP_ADAPTER_ADDRESSES[x_AddressesBufLen];

   u32_RetVal = GetAdaptersAddresses(AF_INET, 0U, NULL, pc_Addresses, &x_AddressesBufLen);

   if (u32_RetVal == NO_ERROR) //lint !e620 !e9106 //constant defined by API; no problem
   {
      PIP_ADAPTER_ADDRESSES pc_Adapter = pc_Addresses;
      while (pc_Adapter != NULL)
      {
         // Only active adapters are relevant
         if (pc_Adapter->OperStatus == IfOperStatusUp)
         {
            IP_ADAPTER_UNICAST_ADDRESS * pc_Address = pc_Adapter->FirstUnicastAddress;

            while (pc_Address != NULL)
            {
               if ((this->mc_PreferredInterfaceNames.GetCount() == 0) ||
                   (this->mc_PreferredInterfaceNames.IndexOf(pc_Adapter->FriendlyName) != -1))
               {
                  // sockaddr is the generic descriptor and sockaddr_in is IPV4 specific
                  // https://stackoverflow.com/questions/21099041/why-do-we-cast-sockaddr-in-to-sockaddr-when-calling-bind/21099196
                  //lint -e{929,740,9176} // Both struct have the same size and is a kind of C style polymorphism
                  const in_addr c_IpAddr = reinterpret_cast<sockaddr_in *>(pc_Address->Address.lpSockaddr)->sin_addr;

                  //ignore localhost; otherwise "locally" running servers will get requests via the localhost and
                  // a physical local interface
                  if (ntohl(c_IpAddr.S_un.S_addr) != 0x7F000001U)
                  {
                     C_SclString c_Info;

                     mc_LocalInterfaceIps.push_back(ntohl(c_IpAddr.S_un.S_addr)); //add to list of known interfaces

                     c_Info =  "Local IP interface used with IP: " + static_cast<C_SclString>(inet_ntoa(c_IpAddr)) +
                              ", name of adapter: \"" + pc_Adapter->FriendlyName +
                              "\"";

                     osc_write_log_info("openSYDE IP-TP", c_Info);

                     s32_Return = C_NO_ERR;
                  }
               }

               pc_Address = pc_Address->Next;
            }
         }
         pc_Adapter = pc_Adapter->Next;
      }
   }
   else
   {
      osc_write_log_error("openSYDE IP-TP", "UDP init failed. Could not get IP addresses with error: " +
                          C_SclString::IntToStr(u32_RetVal));
   }

   delete[] pc_Addresses;

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: connect TCP socket to server

   Jobs to perform:
   - connect already created client socket to port 13400 of the specified server node

   Function shall return when connected or after timeout.
   The function will try the TCP connection for the configured timeout time.

   \param[in,out]  orc_Connection     TCP connection status and configuration

   \return
   C_NO_ERR   connected ...
   C_BUSY     connection failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscIpDispatcherWinSock::m_ConnectTcp(C_TcpConnection & orc_Connection) const
{
   bool q_Error = false;

   //create TCP socket:
   orc_Connection.x_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (orc_Connection.x_Socket == m_WsInvalidSocket())
   {
      osc_write_log_error("openSYDE IP-TP", "Error at TCP socket(): " + C_SclString::IntToStr(WSAGetLastError()) +
                          " IP-Address: " + mh_IpToText(orc_Connection.au8_IpAddress));
      q_Error = true;
   }
   else
   {
      osc_write_log_info("openSYDE IP-TP", "TCP socket OK IP-Address: " + mh_IpToText(orc_Connection.au8_IpAddress));
   }

   if (q_Error == false)
   {
      //make socket non-blocking:
      u_long x_Mode = 1U; //lint !e8080 //using type to match library interface
      if (ioctlsocket(orc_Connection.x_Socket, m_WsFionBio(), &x_Mode) == SOCKET_ERROR)
      {
         osc_write_log_error("openSYDE IP-TP",
                             "TCP socket ioctlsocket() failed. Error: " + C_SclString::IntToStr(WSAGetLastError()) +
                             "IP-Address: " + mh_IpToText(orc_Connection.au8_IpAddress));
         q_Error = true;
      }
   }

   if (q_Error == false)
   {
      int x_Return; //lint !e8080 !e970 //using type to match library interface
      sockaddr_in c_TcpService;

      c_TcpService.sin_family = AF_INET;
      c_TcpService.sin_addr.S_un.S_un_b.s_b1  = orc_Connection.au8_IpAddress[0];
      c_TcpService.sin_addr.S_un.S_un_b.s_b2  = orc_Connection.au8_IpAddress[1];
      c_TcpService.sin_addr.S_un.S_un_b.s_b3  = orc_Connection.au8_IpAddress[2];
      c_TcpService.sin_addr.S_un.S_un_b.s_b4  = orc_Connection.au8_IpAddress[3];
      c_TcpService.sin_port = htons(mhu16_UDP_TCP_PORT); //server port

      //lint -e{929,740,9176}  Side-effect of the POSIX-style API. Match is guaranteed by the API.
      x_Return =
         connect(orc_Connection.x_Socket, reinterpret_cast<const sockaddr *>(&c_TcpService), sizeof(c_TcpService));
      //for this non-blocking TCP socket the function should immediately return with WSAEWOULDBLOCK:
      if ((x_Return == SOCKET_ERROR) && (WSAGetLastError() != WSAEWOULDBLOCK))
      {
         osc_write_log_error("openSYDE IP-TP", "TCP connect() failed. Error: " + C_SclString::IntToStr(
                                WSAGetLastError()));
         q_Error = true;
      }
      else
      {
         fd_set c_SocketWriteSet;                                //monitor for connect completion
         fd_set c_SocketErrorSet;                                //monitor for connect error
         timeval c_TimeOut;                                      //connection timeout
         c_SocketWriteSet.fd_array[0] = orc_Connection.x_Socket; //watch this socket ...
         c_SocketWriteSet.fd_count = 1;                          //only one socket
         c_SocketErrorSet.fd_array[0] = orc_Connection.x_Socket; //watch this socket ...
         c_SocketErrorSet.fd_count = 1;                          //only one socket
         c_TimeOut.tv_sec = mu32_ConnectionTimeoutSeconds;
         c_TimeOut.tv_usec = 0;

         x_Return = select(0, NULL, &c_SocketWriteSet, &c_SocketErrorSet, &c_TimeOut);
         switch (x_Return)
         {
         case SOCKET_ERROR:
            osc_write_log_error("openSYDE IP-TP",
                                "TCP connect select() failed. IP-Address: " + mh_IpToText(
                                   orc_Connection.au8_IpAddress) +
                                " Error: " + C_SclString::IntToStr(WSAGetLastError()));
            q_Error = true;
            break;
         case 0:
            //no event -> timeout
            osc_write_log_warning("openSYDE IP-TP",
                                  "TCP connect select() failed. IP-Address: " + mh_IpToText(
                                     orc_Connection.au8_IpAddress) + " No connection within timeout");
            // No error. The connection to the concrete target can be established later
            q_Error = false;
            break;
         case 1:
            //lint -e{1924,9119,9177} //macro defined by API; no problem
            if (FD_ISSET(orc_Connection.x_Socket, &c_SocketWriteSet))
            {
               // Get port of client for logging (the byte order must be changed of the read port by ntohs)
               sockaddr_in c_SocketInfo;
               int x_Size = //lint !e8080 !e970 //using type to match library interface
                            sizeof(c_SocketInfo);

               //lint -e{929,740,9176}  Side-effect of the POSIX-style API. Match is guaranteed by the API.
               getsockname(orc_Connection.x_Socket, reinterpret_cast<sockaddr *>(&c_SocketInfo), &x_Size);

               //event caused by write (= connect finished)
               osc_write_log_info("openSYDE IP-TP",
                                  "TCP connect select() OK. IP-Address: " + mh_IpToText(orc_Connection.au8_IpAddress) +
                                  " on client port: " + C_SclString::IntToStr(ntohs(c_SocketInfo.sin_port)));
            }
            else
            {
               //event caused by error (= connect failed; e.g. rejected)
               osc_write_log_error("openSYDE IP-TP",
                                   "TCP connect select() failed. IP-Address: " +
                                   mh_IpToText(orc_Connection.au8_IpAddress));
               q_Error = true;
            }
            break;
         default:
            osc_write_log_error("openSYDE IP-TP",
                                "TCP connect select() failed. Unknown problem: " + C_SclString::IntToStr(
                                   x_Return) + " IP-Address: " + mh_IpToText(orc_Connection.au8_IpAddress));
            q_Error = true;
            break;
         }
      }
   }
   return (q_Error == true) ? C_BUSY : C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: configure UDP socket

   \param[in]   oq_ServerPort    true: set up server socket
                                 false: set up client socket
   \param[in]   ou32_IpToBindTo  IP to bind the socket to
   \param[out]  orx_Socket       Resulting socket handle

   \return
   C_NO_ERR   socket set up
   C_NOACT    could not set up socket
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{8080} //using type to match library interface
int32_t C_OscIpDispatcherWinSock::m_ConfigureUdpSocket(const bool oq_ServerPort, const uint32_t ou32_IpToBindTo,
                                                       SOCKET & orx_Socket) const
{
   bool q_Error = false;
   int x_Return; //lint !e8080 !e970 //using type to match library interface

   //create UDP socket:
   orx_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if (orx_Socket == m_WsInvalidSocket())
   {
      osc_write_log_error("openSYDE IP-TP", "Error at UDP socket(): " + C_SclString::IntToStr(WSAGetLastError()));
      q_Error = true;
   }
   else
   {
      osc_write_log_info("openSYDE IP-TP", "UDP socket OK");
   }

   if ((q_Error == false) && (oq_ServerPort == false))
   {
      //set broadcast permission for UDP client socket
      const char_t cn_BROADCAST = 'a';
      x_Return =
         setsockopt(orx_Socket, SOL_SOCKET, SO_BROADCAST, &cn_BROADCAST, sizeof(cn_BROADCAST));
      if (x_Return == SOCKET_ERROR)
      {
         osc_write_log_error("openSYDE IP-TP",
                             "UDP set broadcast permission failed. Error: " + C_SclString::IntToStr(
                                WSAGetLastError()));
         (void)closesocket(orx_Socket);
         q_Error = true;
      }
      else
      {
         osc_write_log_info("openSYDE IP-TP", "UDP set broadcast permission OK");
      }
   }

   if (q_Error == false)
   {
      //bind UDP:
      sockaddr_in c_UdpService;
      c_UdpService.sin_family = AF_INET;
      c_UdpService.sin_addr.s_addr = htonl(ou32_IpToBindTo);
      if (oq_ServerPort == true)
      {
         c_UdpService.sin_port = htons(mhu16_UDP_TCP_PORT); //provide port
      }
      else
      {
         //we want to bind to the IP address of the interface but NOT to the port
         // (this shall be assigned dynamically)
         c_UdpService.sin_port = 0U;
      }

      //lint -e{929,740,9176}  Side-effect of the POSIX-style API. Match is guaranteed by the API.
      x_Return =
         bind(orx_Socket, reinterpret_cast<const sockaddr *>(&c_UdpService), sizeof(c_UdpService));
      if (x_Return == SOCKET_ERROR)
      {
         osc_write_log_error("openSYDE IP-TP",
                             "UDP bind() failed. Error: " + C_SclString::IntToStr(WSAGetLastError()));
         q_Error = true;
      }
      else
      {
         osc_write_log_info("openSYDE IP-TP", "UDP bind() OK");
      }
   }

   if (q_Error == false)
   {
      //make socket non-blocking:
      //lint -e{8080} //using type provided by the library for compatibility
      u_long x_Mode = 1U;
      if (ioctlsocket(orx_Socket, m_WsFionBio(), &x_Mode) == SOCKET_ERROR)
      {
         osc_write_log_error("openSYDE IP-TP", "TCP socket ioctlsocket() failed. Error: " + C_SclString::IntToStr(
                                WSAGetLastError()));
         q_Error = true;
      }
      else
      {
         //nothing more for now ...
      }
   }
   return (q_Error == true) ? C_NOACT : C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: compose textual representation of IP

   Format: xxx.xxx.xxx.xxx

   \return
   test representation of IP
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscIpDispatcherWinSock::mh_IpToText(const uint8_t (&orau8_Ip)[4])
{
   C_SclString c_Text;

   c_Text.PrintFormatted("%d.%d.%d.%d", orau8_Ip[0], orau8_Ip[1], orau8_Ip[2], orau8_Ip[3]);
   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize UDP communication

   Jobs to perform:
   * create non-blocking UDP "server" sockets on each local interface
   ** bind them to the IP of the interface and port 13400
   * create non-blocking UDS "client" sockets on each local interface
   ** bind them to the IP of the interface but don't assign a fixed port
   ** set permissions to send broadcasts

   \return
   C_NO_ERR   connected ...
   C_NOACT    connection failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscIpDispatcherWinSock::InitUdp(void)
{
   bool q_Error = false;
   int32_t s32_Return;

   s32_Return = m_GetAllInstalledInterfaceIps();
   if (s32_Return != C_NO_ERR)
   {
      //we could not find out which interfaces we have: try "any"
      osc_write_log_warning("openSYDE IP-TP", "UDP init: could not find IP(s) of local interfaces. Trying \"any\" ...");
      mc_LocalInterfaceIps.resize(1);
      mc_LocalInterfaceIps[0] = INADDR_ANY;
   }
   mc_SocketsUdpClient.resize(mc_LocalInterfaceIps.size());
   mc_SocketsUdpServer.resize(mc_LocalInterfaceIps.size());

   for (uint32_t u32_Interface = 0U; u32_Interface < mc_LocalInterfaceIps.size(); u32_Interface++)
   {
      mc_SocketsUdpClient[u32_Interface] = m_WsInvalidSocket();
      mc_SocketsUdpServer[u32_Interface] = m_WsInvalidSocket();
   }

   for (uint32_t u32_Interface = 0U; u32_Interface < mc_LocalInterfaceIps.size(); u32_Interface++)
   {
      //create UDP socket:
      s32_Return = m_ConfigureUdpSocket(false, mc_LocalInterfaceIps[u32_Interface], mc_SocketsUdpClient[u32_Interface]);
      if (s32_Return == C_NO_ERR)
      {
         s32_Return =
            m_ConfigureUdpSocket(true, mc_LocalInterfaceIps[u32_Interface], mc_SocketsUdpServer[u32_Interface]);
      }
      if (s32_Return != C_NO_ERR)
      {
         q_Error = true;
         break;
      }
   }

   //close sockets in case of error:
   if (q_Error == true)
   {
      this->CloseUdp();
   }

   return (q_Error == true) ? C_NOACT : C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the connection of the TCP socket

   \param[in]   ou32_Handle   handle obtained by InitTcp()

   \return
   C_NO_ERR   is connected
   C_NOACT    is not connected
   C_RANGE    invalid handle
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscIpDispatcherWinSock::IsTcpConnected(const uint32_t ou32_Handle)
{
   int32_t s32_Return = C_NOACT;

   if (ou32_Handle >= this->mc_SocketsTcp.size())
   {
      osc_write_log_error("openSYDE IP-TP", "IsTcpConnected called with invalid handle.");
      s32_Return = C_RANGE;
   }
   else if (this->mc_SocketsTcp[ou32_Handle].x_Socket == m_WsInvalidSocket())
   {
      // Socket not opened yet
      s32_Return = C_NOACT;
   }
   else
   {
      char_t cn_Byte;
      // Dummy read to check the connection.
      // The parameter MSG_PEEK avoid that received data will removed from the queue when read
      const int x_BytesRead = //lint !e8080 !e970 //using type to match library interface
                              recv(this->mc_SocketsTcp[ou32_Handle].x_Socket,
                                   &cn_Byte, 1, MSG_PEEK);

      if (x_BytesRead > 0)
      {
         s32_Return = C_NO_ERR;
      }
      else if (x_BytesRead == 0)
      {
         // Connection closed by remote peer
         s32_Return = C_NOACT;
      }
      else
      {
         const int x_SocketError = //lint !e8080 !e970 //using type to match library interface
                                   WSAGetLastError();

         // Resource temporarily unavailable. It means that no bytes could be read, because of an empty queue.
         // But the connection is already established.
         if (x_SocketError == WSAEWOULDBLOCK)
         {
            s32_Return = C_NO_ERR;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reconnect TCP socket

   This function can be used by a client to reconnect after the connection was lost (e.g. dropped by server).
   If there is still an active connection the function shall close it before reconnecting.
   Function shall return when connected or after timeout.

   Jobs to perform:
   - re-connect TCP socket to port 13400 of specified server node

   \param[in]   ou32_Handle   handle obtained by InitTcp()

   \return
   C_NO_ERR   reconnected
   C_BUSY     connection failed
   C_RANGE    invalid handle
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscIpDispatcherWinSock::ReConnectTcp(const uint32_t ou32_Handle)
{
   int32_t s32_Return;

   if (ou32_Handle >= this->mc_SocketsTcp.size())
   {
      osc_write_log_error("openSYDE IP-TP", "ReConnectTcp called with invalid handle.");
      s32_Return = C_RANGE;
   }
   else
   {
      //still connected ?
      if (this->mc_SocketsTcp[ou32_Handle].x_Socket != m_WsInvalidSocket())
      {
         //disconnect first:
         (void)closesocket(this->mc_SocketsTcp[ou32_Handle].x_Socket);
         this->mc_SocketsTcp[ou32_Handle].x_Socket = m_WsInvalidSocket();
      }
      //connect:
      s32_Return = this->m_ConnectTcp(this->mc_SocketsTcp[ou32_Handle]);
      if (s32_Return != C_NO_ERR)
      {
         (void)closesocket(this->mc_SocketsTcp[ou32_Handle].x_Socket);
         this->mc_SocketsTcp[ou32_Handle].x_Socket = m_WsInvalidSocket();
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Close TCP communication

   Jobs to perform:
   - close opened TCP client socket

   \param[in]   ou32_Handle   handle obtained by InitTcp()

   \return
   C_NO_ERR   disconnected ...
   C_RANGE    handle invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscIpDispatcherWinSock::CloseTcp(const uint32_t ou32_Handle)
{
   int32_t s32_Return;

   if ((ou32_Handle >= this->mc_SocketsTcp.size()) ||
       (this->mc_SocketsTcp[ou32_Handle].x_Socket == m_WsInvalidSocket()))
   {
      osc_write_log_error("openSYDE IP-TP", "CloseTcp called with invalid handle.");
      s32_Return = C_RANGE;
   }
   else
   {
      sockaddr_in c_SocketInfo;
      int x_Size = //lint !e8080 !e970 //using type to match library interface
                   sizeof(c_SocketInfo);

      //lint -e{929,740,9176}  Side-effect of the POSIX-style API. Match is guaranteed by the API.
      getsockname(this->mc_SocketsTcp[ou32_Handle].x_Socket, reinterpret_cast<sockaddr *>(&c_SocketInfo), &x_Size);

      shutdown(this->mc_SocketsTcp[ou32_Handle].x_Socket, SD_BOTH);

      s32_Return = closesocket(this->mc_SocketsTcp[ou32_Handle].x_Socket);
      this->mc_SocketsTcp[ou32_Handle].x_Socket = m_WsInvalidSocket();

      osc_write_log_info("openSYDE IP-TP",
                         "TCP closesocket() OK. IP-Address: " +
                         mh_IpToText(this->mc_SocketsTcp[ou32_Handle].au8_IpAddress) +
                         " on client port: " + C_SclString::IntToStr(ntohs(c_SocketInfo.sin_port)));
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Close UDP communication

   Jobs to perform:
   - close opened UDP client sockets

   \return
   C_NO_ERR   disconnected ...
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscIpDispatcherWinSock::CloseUdp(void)
{
   for (uint32_t u32_Interface = 0U; u32_Interface < mc_SocketsUdpClient.size(); u32_Interface++)
   {
      if (mc_SocketsUdpClient[u32_Interface] != m_WsInvalidSocket())
      {
         (void)closesocket(mc_SocketsUdpClient[u32_Interface]);
      }
      if (mc_SocketsUdpServer[u32_Interface] != m_WsInvalidSocket())
      {
         (void)closesocket(mc_SocketsUdpServer[u32_Interface]);
      }
   }
   mc_SocketsUdpClient.resize(0);
   mc_SocketsUdpServer.resize(0);
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send package on TCP socket

   Jobs to perform:
   - send specified data to configured TCP socket

   \param[in]   ou32_Handle   handle obtained by InitTcp()
   \param[in]   orc_Data      data to sent

   \return
   C_NO_ERR   data sent successfully
   C_CONFIG   required socket not initialized
   C_RD_WR    error sending data
   C_RANGE    handle invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscIpDispatcherWinSock::SendTcp(const uint32_t ou32_Handle, const std::vector<uint8_t> & orc_Data)
{
   int32_t s32_Return;

   if (ou32_Handle >= this->mc_SocketsTcp.size())
   {
      osc_write_log_error("openSYDE IP-TP", "SendTcp called with invalid handle.");
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = C_NO_ERR;

      if (this->mc_SocketsTcp[ou32_Handle].x_Socket == m_WsInvalidSocket())
      {
         osc_write_log_error("openSYDE IP-TP", "SendTcp called with invalid socket.");
         s32_Return = C_CONFIG;
      }
      else
      {
         //No side effects as long as we are on the Windows platform.
         const int x_BytesToSend = static_cast<int>(orc_Data.size()); //lint !e8080 !e970 //using type to match library
                                                                      // interface
         //lint -e{9176}  //Side-effect of the "char"-based API.
         const int x_BytesSent = //lint !e8080 !e970 //using type to match library interface
                                 send(this->mc_SocketsTcp[ou32_Handle].x_Socket,
                                      reinterpret_cast<const char_t *>(&orc_Data[0]), x_BytesToSend, 0);
         if (x_BytesSent != x_BytesToSend)
         {
            if (x_BytesSent == SOCKET_ERROR)
            {
               const int x_Error = //lint !e8080 !e970 //using type to match library interface
                                   WSAGetLastError();

               osc_write_log_error("openSYDE IP-TP",
                                   "SendTcp: Could not send TCP service. Data lost. Error: " +
                                   C_SclString::IntToStr(x_Error) +
                                   " IP-Address: " + mh_IpToText(this->mc_SocketsTcp[ou32_Handle].au8_IpAddress));
               if ((x_Error == WSAECONNABORTED) || (x_Error == WSAECONNRESET))
               {
                  //we got kicked out; we'll remember that ...
                  osc_write_log_warning("openSYDE IP-TP", "SendTcp: Connection aborted or reset ... IP-Address: " +
                                        mh_IpToText(this->mc_SocketsTcp[ou32_Handle].au8_IpAddress));
                  (void)closesocket(this->mc_SocketsTcp[ou32_Handle].x_Socket);
                  this->mc_SocketsTcp[ou32_Handle].x_Socket = m_WsInvalidSocket();
                  m_OnTcpConnectionDropped(ou32_Handle);
               }
            }
            else
            {
               osc_write_log_error("openSYDE IP-TP",
                                   "SendTcp: Could not send all data: tried: " +
                                   C_SclString::IntToStr(orc_Data.size()) +
                                   "sent: " + C_SclString::IntToStr(x_BytesSent));
            }
            s32_Return = C_RD_WR;
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read data from TCP socket

   Jobs to perform:
   - check whether TCP receive buffer contains required number of data bytes
   - read those bytes

    The function shall not return any data unless it can provide as many data bytes as specified.

   \param[in]      ou32_Handle   handle obtained by InitTcp()
   \param[in,out]  orc_Data      in: the expected number of bytes is set by the caller (orc_Data.size());
                                 out: received data

   \return
   C_NO_ERR   data read successfully
   C_CONFIG   required socket not initialized
   C_NOACT    not enough bytes
   C_RD_WR    error reading data
   C_RANGE    handle invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscIpDispatcherWinSock::ReadTcp(const uint32_t ou32_Handle, std::vector<uint8_t> & orc_Data)
{
   int32_t s32_Return;

   if (ou32_Handle >= this->mc_SocketsTcp.size())
   {
      osc_write_log_error("openSYDE IP-TP", "ReadTcp called with invalid handle.");
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = C_NOACT;

      if (this->mc_SocketsTcp[ou32_Handle].x_Socket == m_WsInvalidSocket())
      {
         osc_write_log_error("openSYDE IP-TP", "ReadTcp called with invalid socket.");
         s32_Return = C_CONFIG;
      }
      else
      {
         int x_Return; //lint !e8080 !e970 //using type to match library interface
         //lint -e{8080} //using type provided by the library for compatibility
         u_long x_SizeInBuffer;
         //do we have enough bytes in Rx buffer ?
         x_Return = ioctlsocket(this->mc_SocketsTcp[ou32_Handle].x_Socket, m_WsFionRead(), &x_SizeInBuffer);
         if ((x_Return != SOCKET_ERROR) && (x_SizeInBuffer >= orc_Data.size()))
         {
            //enough bytes: read
            const int x_BytesToRead = static_cast<int>(orc_Data.size()); //lint !e8080 !e970 //using type to match
                                                                         // library interface
            //lint -e{9176}  Side-effect of the "char"-based API. No side effects as long as we are on the Windows
            // platform.
            const int x_BytesRead = //lint !e8080 !e970 //using type to match library interface
                                    recv(this->mc_SocketsTcp[ou32_Handle].x_Socket,
                                         reinterpret_cast<char_t *>(&orc_Data[0]), x_BytesToRead, 0);
            if (x_BytesRead == x_BytesToRead)
            {
               s32_Return = C_NO_ERR;
            }
            else
            {
               //comm error: no data read even though it was reported by ioctl
               osc_write_log_error("openSYDE IP-TP",
                                   "TCP unexpected error: data reported as available but reading failed. IP-Address: " +
                                   mh_IpToText(this->mc_SocketsTcp[ou32_Handle].au8_IpAddress));
               s32_Return = C_RD_WR;
            }
         }
         else if (x_Return == SOCKET_ERROR)
         {
            osc_write_log_error("openSYDE IP-TP", "Could not read TCP: buffer count could not be read. Error: " +
                                C_SclString::IntToStr(WSAGetLastError()) + " IP-Address: " +
                                mh_IpToText(this->mc_SocketsTcp[ou32_Handle].au8_IpAddress));
         }
         else
         {
            //done here
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read data from TCP socket

   Jobs to perform:
   - check whether TCP receive buffer contains required number of data bytes
   - Check if the identifier are matching (starting at byte 0). The header must be already read separately.
      If the identifier are not matching, the data will be stored in the buffer of the dispatcher.
   - read those bytes

    The function shall not return any data unless it can provide as many data bytes as specified.

   \param[in]      ou32_Handle               handle obtained by InitTcp()
   \param[in]      ou8_ClientBusIdentifier   client identifier of bus
   \param[in]      ou8_ClientNodeIdentifier  client identifier of node
   \param[in]      ou8_ServerBusIdentifier   server identifier of bus
   \param[in]      ou8_ServerNodeIdentifier  server identifier of node
   \param[in,out]  orc_Data                  in: the expected number of bytes is set by the caller (orc_Data.size());
                                             out: received data

   \return
   C_NO_ERR   data read successfully
   C_CONFIG   required socket not initialized
   C_NOACT    not enough bytes
   C_RD_WR    error reading data
   C_RANGE    handle invalid
   C_WARN     data is not for the server with the node identifier and bus identifier
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscIpDispatcherWinSock::ReadTcp(const uint32_t ou32_Handle, const uint8_t ou8_ClientBusIdentifier,
                                          const uint8_t ou8_ClientNodeIdentifier, const uint8_t ou8_ServerBusIdentifier,
                                          const uint8_t ou8_ServerNodeIdentifier, std::vector<uint8_t> & orc_Data)
{
   int32_t s32_Return;

   s32_Return = this->ReadTcp(ou32_Handle, orc_Data);

   if (s32_Return == C_NO_ERR)
   {
      if (orc_Data.size() > 4)
      {
         //are source and target address correct ?
         const uint16_t u16_SourceAddress =
            (static_cast<uint16_t>(static_cast<uint16_t>(orc_Data[0]) << 8U) + orc_Data[1]) - 1U;
         const uint16_t u16_TargetAddress =
            (static_cast<uint16_t>(static_cast<uint16_t>(orc_Data[2]) << 8U) + orc_Data[3]) - 1U;
         const uint8_t u8_SourceNodeId = static_cast<uint8_t>(u16_SourceAddress & 0x7FU);
         const uint8_t u8_SourceBusId = static_cast<uint8_t>((u16_SourceAddress >> 7U) & 0x0FU);
         const uint8_t u8_TargetNodeId = static_cast<uint8_t>(u16_TargetAddress & 0x7FU);
         const uint8_t u8_TargetbusId = static_cast<uint8_t>((u16_TargetAddress >> 7U) & 0x0FU);

         if ((u8_SourceNodeId != ou8_ServerNodeIdentifier) ||
             (u8_SourceBusId != ou8_ServerBusIdentifier) ||
             (u8_TargetNodeId != ou8_ClientNodeIdentifier) ||
             (u8_TargetbusId != ou8_ClientBusIdentifier))
         {
            // Save the message for other client server connections over the same IP address
            std::map<C_BufferIdentifier, std::list<std::vector<uint8_t> > >::iterator c_ItBuffer;
            const C_BufferIdentifier c_Id(u8_TargetbusId, u8_TargetNodeId, u8_SourceBusId, u8_SourceNodeId);

            mhc_LockBuffer.Acquire();

            // Search for already existing data of this identifier
            c_ItBuffer = mhc_TcpBuffer.find(c_Id);

            if (c_ItBuffer != mhc_TcpBuffer.end())
            {
               c_ItBuffer->second.push_back(orc_Data);
            }
            else
            {
               std::list<std::vector<uint8_t> > c_List;
               c_List.push_back(orc_Data);
               mhc_TcpBuffer.insert(
                  std::pair<C_BufferIdentifier, std::list<std::vector<uint8_t> > >(c_Id, c_List));
            }

            mhc_LockBuffer.Release();

            s32_Return = C_WARN;
         }
         else
         {
            s32_Return = C_NO_ERR;
         }
      }
      else
      {
         s32_Return = C_NOACT;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read data from TCP buffer of dispatcher

   Jobs to perform:
   - Check if the identifier are matching (starting at byte 0). The header must be already read separately.
   - read those bytes

    The function shall not return any data unless it can provide as many data bytes as specified.

   \param[in]      ou8_ClientBusIdentifier   client identifier of bus
   \param[in]      ou8_ClientNodeIdentifier  client identifier of node
   \param[in]      ou8_ServerBusIdentifier   server identifier of bus
   \param[in]      ou8_ServerNodeIdentifier  server identifier of node
   \param[in,out]  orc_Data                  in: the expected number of bytes is set by the caller (orc_Data.size());
                                             out: received data

   \return
   C_NO_ERR   data read successfully
   C_NOACT    no data for these identifier
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscIpDispatcherWinSock::ReadTcpBuffer(const uint8_t ou8_ClientBusIdentifier,
                                                const uint8_t ou8_ClientNodeIdentifier,
                                                const uint8_t ou8_ServerBusIdentifier,
                                                const uint8_t ou8_ServerNodeIdentifier, std::vector<uint8_t> & orc_Data)
{
   int32_t s32_Return = C_NOACT;

   std::map<C_BufferIdentifier, std::list<std::vector<uint8_t> > >::iterator c_ItBuffer;
   const C_BufferIdentifier c_Id(ou8_ClientBusIdentifier, ou8_ClientNodeIdentifier, ou8_ServerBusIdentifier,
                                 ou8_ServerNodeIdentifier);

   mhc_LockBuffer.Acquire();

   // Search for saved data in the buffer
   c_ItBuffer = mhc_TcpBuffer.find(c_Id);

   if (c_ItBuffer != mhc_TcpBuffer.end())
   {
      std::list<std::vector<uint8_t> > & rc_List = c_ItBuffer->second;

      if (rc_List.size() > 0)
      {
         // Copy the data
         orc_Data = *rc_List.begin();
         // Remove the read data package
         rc_List.erase(rc_List.begin());

         s32_Return = C_NO_ERR;
      }
   }
   mhc_LockBuffer.Release();

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send broadcast package on UDP request socket

   Jobs to perform:
   - send specified data as broadcast using the configured UDP request socket

   \param[in]  orc_Data   data to sent

   \return
   C_NO_ERR   data sent successfully
   C_CONFIG   required socket not initialized
   C_RD_WR    error sending data
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscIpDispatcherWinSock::SendUdp(const std::vector<uint8_t> & orc_Data)
{
   int32_t s32_Return = C_NO_ERR;

   if (mc_SocketsUdpClient.size() == 0)
   {
      osc_write_log_error("openSYDE IP-TP", "SendUdp called with no socket.");
      s32_Return = C_CONFIG;
   }
   else
   {
      for (uint32_t u32_Interface = 0U; u32_Interface < mc_SocketsUdpClient.size(); u32_Interface++)
      {
         if (mc_SocketsUdpClient[u32_Interface] != m_WsInvalidSocket())
         {
            sockaddr_in c_TargetAddress;
            c_TargetAddress.sin_family = AF_INET;
            c_TargetAddress.sin_port = htons(mhu16_UDP_TCP_PORT);      //target port [REQ DoIp-011]
            c_TargetAddress.sin_addr.s_addr = htonl(INADDR_BROADCAST); //lint !e9105 //constant defined by API;
            //no problem
            const int x_NumToSend = static_cast<int>(orc_Data.size()); //lint !e8080 !e970 //using type to match library
                                                                       // interface
            //lint -e{9176}  //Side-effect of the POSIX-style API. Match is guaranteed by the API.
            const int x_Return = //lint !e8080 !e970 //using type to match library interface
                                 sendto(mc_SocketsUdpClient[u32_Interface],
                                        reinterpret_cast<const char_t *>(&orc_Data[0]),
                                        x_NumToSend, 0, reinterpret_cast<const sockaddr *>(&c_TargetAddress),
                                        sizeof(c_TargetAddress));
            if (x_Return != x_NumToSend)
            {
               osc_write_log_error("openSYDE IP-TP",
                                   "SendUdp sendto error: " + C_SclString::IntToStr(WSAGetLastError()));
               s32_Return = C_RD_WR;
            }
         }
         else
         {
            // Write error to log, then ignore the error and continue
            osc_write_log_error("openSYDE IP-TP", "SendUdp called with invalid socket.");
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read package from UDP socket

   Jobs to perform:
   Go through all set up UDP "server" sockets and try to read incoming datagram from UDP socket.
   Report datagrams sent from:
   * any IP
   * port 13400 (to ignore reception of our own broadcasts and unexpected traffic)
   Return the read data.

   \param[out]  orc_Data   received data
   \param[out]  orau8_Ip   IP address the request was received from

   \return
   C_NO_ERR   datagram read successfully
   C_CONFIG   required socket not initialized
   C_NOACT    no data received
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscIpDispatcherWinSock::ReadUdp(std::vector<uint8_t> & orc_Data, uint8_t (&orau8_Ip)[4])
{
   int32_t s32_Return = C_NOACT;

   if (mc_SocketsUdpServer.size() == 0)
   {
      osc_write_log_error("openSYDE IP-TP", "ReadUdp called with no socket.");
      s32_Return = C_CONFIG;
   }
   else
   {
      for (uint32_t u32_Interface = 0U; u32_Interface < mc_SocketsUdpServer.size(); u32_Interface++)
      {
         if (mc_SocketsUdpClient[u32_Interface] != m_WsInvalidSocket())
         {
            //do we have a package in Rx buffer ?
            u_long x_SizeInBuffer; //lint !e8080 //using type to match library interface
            int x_Return =         //lint !e8080 !e970 //using type to match library interface
                           ioctlsocket(mc_SocketsUdpServer[u32_Interface], m_WsFionRead(), &x_SizeInBuffer);
            if ((x_Return != SOCKET_ERROR) && (x_SizeInBuffer >= 1U))
            {
               sockaddr_in c_Sender;
               const int x_NumToReceive = static_cast<int>(orc_Data.size()); //lint !e8080 !e970 //using type to match
                                                                             // library interface
               int x_AddressSize = sizeof(c_Sender);                         //lint !e8080 !e970 //using type to match
                                                                             // library interface

               //enough bytes: read
               orc_Data.resize(x_SizeInBuffer);

               //lint -e{926,929}  //Side-effect of the "char"-based API. No problems as long as we are on Windows.
               //lint -e{740,9176} //Side-effect of the POSIX-style API. Match is guaranteed by the API.
               x_Return = recvfrom(mc_SocketsUdpServer[u32_Interface], reinterpret_cast<char_t *>(&orc_Data[0]),
                                   x_NumToReceive, 0, reinterpret_cast<sockaddr *>(&c_Sender),
                                   &x_AddressSize);

               //there might be more than one package in the buffer; recvfrom only reads one
               //so we check whether we have more than zero bytes:
               if (x_Return > 0)
               {
                  //extract sender address
                  orau8_Ip[0] = c_Sender.sin_addr.S_un.S_un_b.s_b1;
                  orau8_Ip[1] = c_Sender.sin_addr.S_un.S_un_b.s_b2;
                  orau8_Ip[2] = c_Sender.sin_addr.S_un.S_un_b.s_b3;
                  orau8_Ip[3] = c_Sender.sin_addr.S_un.S_un_b.s_b4;

                  if (x_Return != static_cast<int>(orc_Data.size())) //lint !e970 //using type to match library
                                                                     // interface
                  {
                     orc_Data.resize(x_Return); //we only need the data we really received
                  }

                  //filter out local reception of broadcasts we sent ourselves
                  //strategy to identify those:
                  //Responses from real nodes are sent from Port 13400
                  //Our broadcasts are sent with dynamically assigned Port != 13400
                  //Also: we should not just accept anything that is thrown upon us ...
                  if (c_Sender.sin_port == htons(13400))
                  {
                     s32_Return = C_NO_ERR;
                  }
                  else
                  {
                     s32_Return = C_NOACT;
                  }
                  break; //we have a package ...
               }
               else
               {
                  //comm error: no data read even though it was reported by ioctl
                  osc_write_log_error("openSYDE IP-TP",
                                      "ReadUdp unexpected error: data reported as available but reading failed. Reported size: " +
                                      C_SclString::IntToStr(
                                         orc_Data.size()) + " Read size: " + C_SclString::IntToStr(x_Return));
                  s32_Return = C_RD_WR;
               }
            }
         }
         else
         {
            // Write error to log, then ignore the error and continue
            osc_write_log_error("openSYDE IP-TP", "ReadUdp called with invalid socket.");
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Loads optional configuration file

   If the file does not exist, no specific configuration will be used for Ethernet.

   \param[in] orc_FileLocation Log file location path and file name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscIpDispatcherWinSock::LoadConfigFile(const C_SclString & orc_FileLocation)
{
   if (TglFileExists(orc_FileLocation) == true)
   {
      C_SclIniFile c_Ini(orc_FileLocation);
      const C_SclString c_Help = c_Ini.ReadString("ETH_CONFIG", "ETH_INTERFACE_NAME", "");

      c_Help.Tokenize(",", this->mc_PreferredInterfaceNames.Strings);
   }
   else
   {
      this->mc_PreferredInterfaceNames.Clear();
   }
}
