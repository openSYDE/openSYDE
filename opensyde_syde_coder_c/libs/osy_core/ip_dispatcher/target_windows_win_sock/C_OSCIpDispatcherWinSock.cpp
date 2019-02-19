//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       openSYDE protocol IP driver for Windows

   For details cf. documentation in .h file.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.04.2017  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <winsock.h> //Windows WinSock
#include <winsock2.h>
#include <cstring>
#include <iphlpapi.h> //Windows IP helper utilities
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCIpDispatcherWinSock.h"
#include "CSCLString.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_scl;
using namespace stw_tgl;

/* -- Module Global Constants ---------------------------------------------- */

//export a few WinSock constants here
//Purpose: the definitions in the Windows header violate a lot of coding rules.
//So we can relocate the required suppressions here at a central spot ...
//The violations do not cause problems in the application as long as we are on a Windows platform.
static uintn m_WsInvalidSocket(void)
{
   return INVALID_SOCKET; //lint !e1960
}

//-----------------------------------------------------------------------------
static sint32 m_WsFionBio(void)
{
   return FIONBIO; //lint !e1960 !e970 !e1924 !e569
}

//-----------------------------------------------------------------------------
static sint32 m_WsFionRead(void)
{
   return FIONREAD; //lint !e1960 !e970 !e1924
}

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */
std::map<C_OSCIpDispatcherWinSock::C_BufferIdentifier,
         std::list<std::vector<stw_types::uint8> > > C_OSCIpDispatcherWinSock::mhc_TcpBuffer;
C_TGLCriticalSection C_OSCIpDispatcherWinSock::mhc_LockBuffer;

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Constructor

   Initialize elements

   \created     07.01.2019  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_OSCIpDispatcherWinSock::C_TcpConnection::C_TcpConnection(void)
{
   un_Socket = m_WsInvalidSocket();
   (void)std::memset(&au8_IpAddress[0], 0U, 4U);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if current smaller than orc_Cmp

   \param[in] orc_Cmp Compared instance

   \return
   Current smaller than orc_Cmp
   Else false

   \created     16.04.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_OSCIpDispatcherWinSock::C_BufferIdentifier::operator <(
   const C_OSCIpDispatcherWinSock::C_BufferIdentifier & orc_Cmp) const
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set up class

   Initialize class elements

   \created     26.04.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_OSCIpDispatcherWinSock::C_OSCIpDispatcherWinSock(void) :
   C_OSCIpDispatcher()
{
   WSADATA t_Data;

   stw_types::sintn sn_Result = WSAStartup(0x0201U, &t_Data); //Request version 2.1
   if (sn_Result != 0)
   {
      throw "WSAp ?";
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clean up class

   Release allocated resources

   \created     26.04.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_OSCIpDispatcherWinSock::~C_OSCIpDispatcherWinSock(void)
{
   //make sure to release resources in case the user forgot to
   for (uint16 u16_Index = 0U; u16_Index < this->mc_SocketsTcp.size(); u16_Index++)
   {
      (void)this->CloseTcp(u16_Index);
   }
   (void)this->CloseUdp();

   (void)WSACleanup(); //spec: one call of cleanup per call of startup; final call performs actual cleanup
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize TCP communication

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

   \created     27.04.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::InitTcp(const uint8 (&orau8_Ip)[4], uint32 & oru32_Handle)
{
   C_TcpConnection c_NewConnection;

   (void)memcpy(&c_NewConnection.au8_IpAddress[0], &orau8_Ip[0], 4U);

   this->mc_SocketsTcp.push_back(c_NewConnection);
   oru32_Handle = static_cast<uint32>(mc_SocketsTcp.size() - 1U);

   return C_NO_ERR;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utility: get IP of all local interfaces

   Gets list of IP addresses of all local IP interfaces.
   Does not report "localhost".
   Results will be placed in mc_LocalInterfaceIps

   \return
   C_NO_ERR     IPs listed
   C_NOACT      could not get IP information

   \created     19.05.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::m_GetAllInstalledInterfaceIps(void)
{
   uint32 u32_Size = 0U;
   sint32 s32_Return = C_NOACT;
   uint32 u32_Result;

   mc_LocalInterfaceIps.resize(0);

   //Get required buffer size:
   u32_Result = GetIpAddrTable(NULL, &u32_Size, 0);

   //we really should get an error here:
   if (u32_Result == ERROR_INSUFFICIENT_BUFFER) //lint !e1960 //constant defined by API; no problem
   {
      //Now get the actual data:
      //lint -e{1960,927,433,826} //this is one beast of an API; but we have to use it ...
      MIB_IPADDRTABLE * const pt_Table = reinterpret_cast<MIB_IPADDRTABLE * const>(new uint8[u32_Size]);
      u32_Result = GetIpAddrTable(pt_Table, &u32_Size, 0);
      if (u32_Result != NO_ERROR) //lint !e1960 //constant defined by API; no problem
      {
         osc_write_log_error("openSYDE IP-TP", "UDP init failed. Could not get TP address table");
         delete[] pt_Table;
         s32_Return = C_NOACT;
      }
      else
      {
         s32_Return = C_NO_ERR;
         for (uint32 u32_Interface = 0U; u32_Interface < pt_Table->dwNumEntries; u32_Interface++)
         {
            //ignore localhost; otherwise "locally" running servers will get requests via the localhost and
            // a physical local interface
            if (ntohl(pt_Table->table[u32_Interface].dwAddr) != 0x7F000001U)
            {
               mc_LocalInterfaceIps.push_back(ntohl(pt_Table->table[u32_Interface].dwAddr));
            }
         }
         delete[] pt_Table;
      }
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utility: connect TCP socket to server

   Jobs to perform:
   - connect already created client socket to port 13400 of the specified server node

   Function shall return when connected or after timeout.
   The function will try the TCP connection for the configured timeout time.

   \param[in,out]  orc_Connection     TCP connection status and configuration

   \return
   C_NO_ERR   connected ...
   C_BUSY     connection failed

   \created     05.01.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::m_ConnectTcp(C_TcpConnection & orc_Connection) const
{
   bool q_Error = false;

   //create TCP socket:
   orc_Connection.un_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (orc_Connection.un_Socket == m_WsInvalidSocket())
   {
      osc_write_log_error("openSYDE IP-TP", "Error at TCP socket(): " + C_SCLString::IntToStr(WSAGetLastError()) +
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
      uint32 u32_Mode = 1U;
      if (ioctlsocket(orc_Connection.un_Socket, m_WsFionBio(), &u32_Mode) == SOCKET_ERROR)
      {
         osc_write_log_error("openSYDE IP-TP",
                             "TCP socket ioctlsocket() failed. Error: " + C_SCLString::IntToStr(WSAGetLastError()) +
                             "IP-Address: " + mh_IpToText(orc_Connection.au8_IpAddress));
         q_Error = true;
      }
   }

   if (q_Error == false)
   {
      sintn sn_Return;
      sockaddr_in c_TcpService;

      c_TcpService.sin_family = AF_INET;
      c_TcpService.sin_addr.S_un.S_un_b.s_b1  = orc_Connection.au8_IpAddress[0];
      c_TcpService.sin_addr.S_un.S_un_b.s_b2  = orc_Connection.au8_IpAddress[1];
      c_TcpService.sin_addr.S_un.S_un_b.s_b3  = orc_Connection.au8_IpAddress[2];
      c_TcpService.sin_addr.S_un.S_un_b.s_b4  = orc_Connection.au8_IpAddress[3];
      c_TcpService.sin_port = htons(mhu16_UDP_TCP_PORT); //server port

      //lint -e{929,740}  Side-effect of the POSIX-style API. Match is guaranteed by the API.
      sn_Return =
         connect(orc_Connection.un_Socket, reinterpret_cast<const sockaddr *>(&c_TcpService), sizeof(c_TcpService));
      //for this non-blocking TCP socket the function should immediately return with WSAEWOULDBLOCK:
      if ((sn_Return == SOCKET_ERROR) && (WSAGetLastError() != WSAEWOULDBLOCK))
      {
         osc_write_log_error("openSYDE IP-TP", "TCP connect() failed. Error: " + C_SCLString::IntToStr(
                                WSAGetLastError()));
         q_Error = true;
      }
      else
      {
         fd_set t_SocketWriteSet;                                 //monitor for connect completion
         fd_set t_SocketErrorSet;                                 //monitor for connect error
         timeval t_TimeOut;                                       //connection timeout
         t_SocketWriteSet.fd_array[0] = orc_Connection.un_Socket; //watch this socket ...
         t_SocketWriteSet.fd_count = 1;                           //only one socket
         t_SocketErrorSet.fd_array[0] = orc_Connection.un_Socket; //watch this socket ...
         t_SocketErrorSet.fd_count = 1;                           //only one socket
         t_TimeOut.tv_sec = mu32_ConnectionTimeoutSeconds;
         t_TimeOut.tv_usec = 0;

         sn_Return = select(0, NULL, &t_SocketWriteSet, &t_SocketErrorSet, &t_TimeOut);
         switch (sn_Return)
         {
         case SOCKET_ERROR:
            osc_write_log_error("openSYDE IP-TP",
                                "TCP connect select() failed. IP-Address: " + mh_IpToText(
                                   orc_Connection.au8_IpAddress) +
                                " Error: " + C_SCLString::IntToStr(WSAGetLastError()));
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
            //lint -e{1924,929,909} //macro defined by API; no problem
            if (FD_ISSET(orc_Connection.un_Socket, &t_SocketWriteSet))
            {
               // Get port of client for logging (the byte order must be changed of the read port by ntohs)
               sockaddr_in c_SocketInfo;
               sintn sn_Size = sizeof(c_SocketInfo);

               //lint -e{929,740}  Side-effect of the POSIX-style API. Match is guaranteed by the API.
               getsockname(orc_Connection.un_Socket, reinterpret_cast<sockaddr *>(&c_SocketInfo), &sn_Size);

               //event caused by write (= connect finished)
               osc_write_log_info("openSYDE IP-TP",
                                  "TCP connect select() OK. IP-Address: " + mh_IpToText(orc_Connection.au8_IpAddress) +
                                  " on client port: " + C_SCLString::IntToStr(ntohs(c_SocketInfo.sin_port)));
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
                                "TCP connect select() failed. Unknown problem: " + C_SCLString::IntToStr(
                                   sn_Return) + " IP-Address: " + mh_IpToText(orc_Connection.au8_IpAddress));
            q_Error = true;
            break;
         }
      }
   }
   return (q_Error == true) ? C_BUSY : C_NO_ERR;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utility: configure UDP socket

   \param[in]   oq_ServerPort    true: set up server socket
                                 false: set up client socket
   \param[in]   ou32_IpToBindTo  IP to bind the socket to
   \param[out]  orun_Socket      Resulting socket handle

   \return
   C_NO_ERR   socket set up
   C_NOACT    could not set up socket

   \created     22.02.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::m_ConfigureUdpSocket(const bool oq_ServerPort, const uint32 ou32_IpToBindTo,
                                                      SOCKET & orun_Socket) const
{
   bool q_Error = false;
   sintn sn_Return;

   //create UDP socket:
   orun_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if (orun_Socket == m_WsInvalidSocket())
   {
      osc_write_log_error("openSYDE IP-TP", "Error at UDP socket(): " + C_SCLString::IntToStr(WSAGetLastError()));
      q_Error = true;
   }
   else
   {
      osc_write_log_info("openSYDE IP-TP", "UDP socket OK");
   }

   if ((q_Error == false) && (oq_ServerPort == false))
   {
      //set broadcast permission for UDP client socket
      charn cn_Broadcast = 'a';
      sn_Return =
         setsockopt(orun_Socket, SOL_SOCKET, SO_BROADCAST, &cn_Broadcast, sizeof(cn_Broadcast));
      if (sn_Return == SOCKET_ERROR)
      {
         osc_write_log_error("openSYDE IP-TP",
                             "UDP set broadcast permission failed. Error: " + C_SCLString::IntToStr(
                                WSAGetLastError()));
         (void)closesocket(orun_Socket);
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

      //lint -e{929,740}  Side-effect of the POSIX-style API. Match is guaranteed by the API.
      sn_Return =
         bind(orun_Socket, reinterpret_cast<const sockaddr *>(&c_UdpService), sizeof(c_UdpService));
      if (sn_Return == SOCKET_ERROR)
      {
         osc_write_log_error("openSYDE IP-TP",
                             "UDP bind() failed. Error: " + C_SCLString::IntToStr(WSAGetLastError()));
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
      uint32 u32_Mode = 1U;
      if (ioctlsocket(orun_Socket, m_WsFionBio(), &u32_Mode) == SOCKET_ERROR)
      {
         osc_write_log_error("openSYDE IP-TP", "TCP socket ioctlsocket() failed. Error: " + C_SCLString::IntToStr(
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

//-----------------------------------------------------------------------------
/*!
   \brief   Utility: compose textual representation of IP

   Format: xxx.xxx.xxx.xxx

   \return
   test representation of IP

   \created     27.02.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCIpDispatcherWinSock::mh_IpToText(const uint8 (&orau8_Ip)[4])
{
   C_SCLString c_Text;

   c_Text.PrintFormatted("%d.%d.%d.%d", orau8_Ip[0], orau8_Ip[1], orau8_Ip[2], orau8_Ip[3]);
   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize UDP communication

   Jobs to perform:
   * create non-blocking UDP "server" sockets on each local interface
   ** bind them to the IP of the interface and port 13400
   * create non-blocking UDS "client" sockets on each local interface
   ** bind them to the IP of the interface but don't assign a fixed port
   ** set permissions to send broadcasts

   \return
   C_NO_ERR   connected ...
   C_NOACT    connection failed

   \created     27.04.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::InitUdp(void)
{
   bool q_Error = false;
   sint32 s32_Return;

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

   for (uint32 u32_Interface = 0U; u32_Interface < mc_LocalInterfaceIps.size(); u32_Interface++)
   {
      mc_SocketsUdpClient[u32_Interface] = m_WsInvalidSocket();
      mc_SocketsUdpServer[u32_Interface] = m_WsInvalidSocket();
   }

   for (uint32 u32_Interface = 0U; u32_Interface < mc_LocalInterfaceIps.size(); u32_Interface++)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Checks the connection of the TCP socket

   \param[in]   ou32_Handle   handle obtained by InitTcp()

   \return
   C_NO_ERR   is connected
   C_NOACT    is not connected
   C_RANGE    invalid handle
*/
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::IsTcpConnected(const uint32 ou32_Handle)
{
   sint32 s32_Return = C_NOACT;

   if (ou32_Handle >= this->mc_SocketsTcp.size())
   {
      osc_write_log_error("openSYDE IP-TP", "ReConnectTcp called with invalid handle.");
      s32_Return = C_RANGE;
   }
   else if (this->mc_SocketsTcp[ou32_Handle].un_Socket == m_WsInvalidSocket())
   {
      // Socket not opened yet
      s32_Return = C_NOACT;
   }
   else
   {
      charn cn_Byte;
      // Dummy read to check the connection.
      // The parameter MSG_PEEK avoid that received data will removed from the queue when read
      const sintn sn_BytesRead = recv(this->mc_SocketsTcp[ou32_Handle].un_Socket,
                                      &cn_Byte, 1, MSG_PEEK);

      if (sn_BytesRead > 0)
      {
         s32_Return = C_NO_ERR;
      }
      else if (sn_BytesRead == 0)
      {
         // Connection closed by remote peer
         s32_Return = C_NOACT;
      }
      else
      {
         const sintn sn_SocketError = WSAGetLastError();

         // Resource temporarily unavailable. It means that no bytes could be read, because of an empty queue.
         // But the connection is already established.
         if (sn_SocketError == WSAEWOULDBLOCK)
         {
            s32_Return = C_NO_ERR;
         }
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Reconnect TCP socket

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
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::ReConnectTcp(const uint32 ou32_Handle)
{
   sint32 s32_Return;

   if (ou32_Handle >= this->mc_SocketsTcp.size())
   {
      osc_write_log_error("openSYDE IP-TP", "ReConnectTcp called with invalid handle.");
      s32_Return = C_RANGE;
   }
   else
   {
      //still connected ?
      if (this->mc_SocketsTcp[ou32_Handle].un_Socket != m_WsInvalidSocket())
      {
         //disconnect first:
         (void)closesocket(this->mc_SocketsTcp[ou32_Handle].un_Socket);
         this->mc_SocketsTcp[ou32_Handle].un_Socket = m_WsInvalidSocket();
      }
      //connect:
      s32_Return = this->m_ConnectTcp(this->mc_SocketsTcp[ou32_Handle]);
      if (s32_Return != C_NO_ERR)
      {
         (void)closesocket(this->mc_SocketsTcp[ou32_Handle].un_Socket);
         this->mc_SocketsTcp[ou32_Handle].un_Socket = m_WsInvalidSocket();
      }
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Close TCP communication

   Jobs to perform:
   - close opened TCP client socket

   \param[in]   ou32_Handle   handle obtained by InitTcp()

   \return
   C_NO_ERR   disconnected ...
   C_RANGE    handle invalid

   \created     27.04.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::CloseTcp(const uint32 ou32_Handle)
{
   sint32 s32_Return;

   if ((ou32_Handle >= this->mc_SocketsTcp.size()) ||
       (this->mc_SocketsTcp[ou32_Handle].un_Socket == m_WsInvalidSocket()))
   {
      osc_write_log_error("openSYDE IP-TP", "CloseTcp called with invalid handle.");
      s32_Return = C_RANGE;
   }
   else
   {
      sockaddr_in c_SocketInfo;
      sintn sn_Size = sizeof(c_SocketInfo);

      //lint -e{929,740}  Side-effect of the POSIX-style API. Match is guaranteed by the API.
      getsockname(this->mc_SocketsTcp[ou32_Handle].un_Socket, reinterpret_cast<sockaddr *>(&c_SocketInfo), &sn_Size);

      shutdown(this->mc_SocketsTcp[ou32_Handle].un_Socket, SD_BOTH);

      s32_Return = closesocket(this->mc_SocketsTcp[ou32_Handle].un_Socket);
      this->mc_SocketsTcp[ou32_Handle].un_Socket = m_WsInvalidSocket();

      osc_write_log_info("openSYDE IP-TP",
                         "TCP closesocket() OK. IP-Address: " +
                         mh_IpToText(this->mc_SocketsTcp[ou32_Handle].au8_IpAddress) +
                         " on client port: " + C_SCLString::IntToStr(ntohs(c_SocketInfo.sin_port)));
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Close UDP communication

   Jobs to perform:
   - close opened UDP client sockets

   \return
   C_NO_ERR   disconnected ...

   \created     27.04.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::CloseUdp(void)
{
   for (uint32 u32_Interface = 0U; u32_Interface < mc_SocketsUdpClient.size(); u32_Interface++)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Send package on TCP socket

   Jobs to perform:
   - send specified data to configured TCP socket

   \param[in]   ou32_Handle   handle obtained by InitTcp()
   \param[in]   orc_Data      data to sent

   \return
   C_NO_ERR   data sent successfully
   C_CONFIG   required socket not initialized
   C_RD_WR    error sending data
   C_RANGE    handle invalid

   \created     27.04.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::SendTcp(const uint32 ou32_Handle, const std::vector<uint8> & orc_Data)
{
   sint32 s32_Return;

   if (ou32_Handle >= this->mc_SocketsTcp.size())
   {
      osc_write_log_error("openSYDE IP-TP", "SendTcp called with invalid handle.");
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = C_NO_ERR;

      if (this->mc_SocketsTcp[ou32_Handle].un_Socket == m_WsInvalidSocket())
      {
         osc_write_log_error("openSYDE IP-TP", "SendTcp called with invalid socket.");
         s32_Return = C_CONFIG;
      }
      else
      {
         //lint -e{926}  Side-effect of the "char"-based API. No side effects as long as we are on the Windows platform.
         const sintn sn_BytesSent = send(this->mc_SocketsTcp[ou32_Handle].un_Socket,
                                         reinterpret_cast<const charn *>(&orc_Data[0]), orc_Data.size(), 0);
         if (sn_BytesSent != static_cast<sintn>(orc_Data.size()))
         {
            if (sn_BytesSent == SOCKET_ERROR)
            {
               const sintn sn_Error = WSAGetLastError();

               osc_write_log_error("openSYDE IP-TP",
                                   "SendTcp: Could not send TCP service. Data lost. Error: " +
                                   C_SCLString::IntToStr(sn_Error) +
                                   " IP-Address: " + mh_IpToText(this->mc_SocketsTcp[ou32_Handle].au8_IpAddress));
               if ((sn_Error == WSAECONNABORTED) || (sn_Error == WSAECONNRESET))
               {
                  //we got kicked out; we'll remember that ...
                  osc_write_log_warning("openSYDE IP-TP", "SendTcp: Connection aborted or reset ... IP-Address: " +
                                        mh_IpToText(this->mc_SocketsTcp[ou32_Handle].au8_IpAddress));
                  (void)closesocket(this->mc_SocketsTcp[ou32_Handle].un_Socket);
                  this->mc_SocketsTcp[ou32_Handle].un_Socket = m_WsInvalidSocket();
                  m_OnTcpConnectionDropped(ou32_Handle);
               }
            }
            else
            {
               osc_write_log_error("openSYDE IP-TP",
                                   "SendTcp: Could not send all data: tried: " +
                                   C_SCLString::IntToStr(static_cast<sintn>(orc_Data.size())) +
                                   "sent: " + C_SCLString::IntToStr(sn_BytesSent));
            }
            s32_Return = C_RD_WR;
         }
      }
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Read data from TCP socket

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

   \created     27.04.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::ReadTcp(const uint32 ou32_Handle, std::vector<uint8> & orc_Data)
{
   sint32 s32_Return;

   if (ou32_Handle >= this->mc_SocketsTcp.size())
   {
      osc_write_log_error("openSYDE IP-TP", "ReadTcp called with invalid handle.");
      s32_Return = C_RANGE;
   }
   else
   {
      s32_Return = C_NOACT;

      if (this->mc_SocketsTcp[ou32_Handle].un_Socket == m_WsInvalidSocket())
      {
         osc_write_log_error("openSYDE IP-TP", "ReadTcp called with invalid socket.");
         s32_Return = C_CONFIG;
      }
      else
      {
         sintn sn_Return;
         uint32 u32_SizeInBuffer;
         //do we have enough bytes in RX buffer ?
         sn_Return = ioctlsocket(this->mc_SocketsTcp[ou32_Handle].un_Socket, m_WsFionRead(), &u32_SizeInBuffer);
         if ((sn_Return != SOCKET_ERROR) && (u32_SizeInBuffer >= orc_Data.size()))
         {
            //enough bytes: read
            //lint -e{926}  Side-effect of the "char"-based API. No side effects as long as we are on the Windows
            // platform.
            sintn sn_BytesRead = recv(this->mc_SocketsTcp[ou32_Handle].un_Socket,
                                      reinterpret_cast<charn *>(&orc_Data[0]), orc_Data.size(), 0);
            if (sn_BytesRead == static_cast<sintn>(orc_Data.size()))
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
         else if (sn_Return == SOCKET_ERROR)
         {
            osc_write_log_error("openSYDE IP-TP", "Could not read TCP: buffer count could not be read. Error: " +
                                C_SCLString::IntToStr(WSAGetLastError()) + " IP-Address: " +
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

//-----------------------------------------------------------------------------
/*!
   \brief   Read data from TCP socket

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

   \created     16.04.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::ReadTcp(const uint32 ou32_Handle, const uint8 ou8_ClientBusIdentifier,
                                         const uint8 ou8_ClientNodeIdentifier, const uint8 ou8_ServerBusIdentifier,
                                         const uint8 ou8_ServerNodeIdentifier, std::vector<uint8> & orc_Data)
{
   sint32 s32_Return;

   s32_Return = this->ReadTcp(ou32_Handle, orc_Data);

   if (s32_Return == C_NO_ERR)
   {
      if (orc_Data.size() > 4)
      {
         //are source and target address correct ?
         //lint -e{864} //false positive due to const/non-const misinterpretation
         const uint16 u16_SourceAddress =
            (static_cast<uint16>(static_cast<uint16>(orc_Data[0]) << 8) + orc_Data[1]) - 1U;
         //lint -e{864} //false positive due to const/non-const misinterpretation
         const uint16 u16_TargetAddress =
            (static_cast<uint16>(static_cast<uint16>(orc_Data[2]) << 8) + orc_Data[3]) - 1U;
         const uint8 u8_SourceNodeId = static_cast<uint8>(u16_SourceAddress & 0x7FU);
         const uint8 u8_SourceBusId = static_cast<uint8>((u16_SourceAddress >> 7) & 0x0FU);
         const uint8 u8_TargetNodeId = static_cast<uint8>(u16_TargetAddress & 0x7FU);
         const uint8 u8_TargetbusId = static_cast<uint8>((u16_TargetAddress >> 7) & 0x0FU);

         if ((u8_SourceNodeId != ou8_ServerNodeIdentifier) ||
             (u8_SourceBusId != ou8_ServerBusIdentifier) ||
             (u8_TargetNodeId != ou8_ClientNodeIdentifier) ||
             (u8_TargetbusId != ou8_ClientBusIdentifier))
         {
            // Save the message for other client server connections over the same IP address
            std::map<C_BufferIdentifier, std::list<std::vector<stw_types::uint8> > >::iterator c_ItBuffer;
            C_BufferIdentifier c_Id(u8_TargetbusId, u8_TargetNodeId, u8_SourceBusId, u8_SourceNodeId);

            mhc_LockBuffer.Acquire();

            // Search for already existing data of this identifier
            c_ItBuffer = mhc_TcpBuffer.find(c_Id);

            if (c_ItBuffer != mhc_TcpBuffer.end())
            {
               c_ItBuffer->second.push_back(orc_Data);
            }
            else
            {
               std::list<std::vector<stw_types::uint8> > c_List;
               c_List.push_back(orc_Data);
               mhc_TcpBuffer.insert(
                  std::pair<C_BufferIdentifier, std::list<std::vector<stw_types::uint8> > >(c_Id, c_List));
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

//-----------------------------------------------------------------------------
/*!
   \brief   Read data from TCP buffer of dispatcher

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

   \created     16.04.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::ReadTcpBuffer(const uint8 ou8_ClientBusIdentifier,
                                               const uint8 ou8_ClientNodeIdentifier,
                                               const uint8 ou8_ServerBusIdentifier,
                                               const uint8 ou8_ServerNodeIdentifier, std::vector<uint8> & orc_Data)
{
   sint32 s32_Return = C_NOACT;

   std::map<C_BufferIdentifier, std::list<std::vector<stw_types::uint8> > >::iterator c_ItBuffer;
   C_BufferIdentifier c_Id(ou8_ClientBusIdentifier, ou8_ClientNodeIdentifier, ou8_ServerBusIdentifier,
                           ou8_ServerNodeIdentifier);

   mhc_LockBuffer.Acquire();

   // Search for saved data in the buffer
   c_ItBuffer = mhc_TcpBuffer.find(c_Id);

   if (c_ItBuffer != mhc_TcpBuffer.end())
   {
      std::list<std::vector<stw_types::uint8> > & rc_List = c_ItBuffer->second;

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

//-----------------------------------------------------------------------------
/*!
   \brief   Send broadcast package on UDP request socket

   Jobs to perform:
   - send specified data as broadcast using the configured UDP request socket

   \param[in]  orc_Data   data to sent

   \return
   C_NO_ERR   data sent successfully
   C_CONFIG   required socket not initialized
   C_RD_WR    error sending data

   \created     04.05.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::SendUdp(const std::vector<uint8> & orc_Data)
{
   sint32 s32_Return = C_NO_ERR;

   if (mc_SocketsUdpClient.size() == 0)
   {
      osc_write_log_error("openSYDE IP-TP", "SendUdp called with no socket.");
      s32_Return = C_CONFIG;
   }
   else
   {
      for (uint32 u32_Interface = 0U; u32_Interface < mc_SocketsUdpClient.size(); u32_Interface++)
      {
         if (mc_SocketsUdpClient[u32_Interface] != m_WsInvalidSocket())
         {
            sockaddr_in t_TargetAddress;
            t_TargetAddress.sin_family = AF_INET;
            t_TargetAddress.sin_port = htons(mhu16_UDP_TCP_PORT);      //target port [REQ DoIp-011]
            t_TargetAddress.sin_addr.s_addr = htonl(INADDR_BROADCAST); //lint !e1960 //constant defined by API; no
                                                                       // problem
            //lint -e{740,926,929}  Side-effect of the POSIX-style API. Match is guaranteed by the API.
            const sintn sn_Return = sendto(mc_SocketsUdpClient[u32_Interface],
                                           reinterpret_cast<const charn *>(&orc_Data[0]),
                                           orc_Data.size(), 0, reinterpret_cast<const sockaddr *>(&t_TargetAddress),
                                           sizeof(t_TargetAddress));
            if (sn_Return != static_cast<sintn>(orc_Data.size()))
            {
               osc_write_log_error("openSYDE IP-TP",
                                   "SendUdp sendto error: " + C_SCLString::IntToStr(WSAGetLastError()));
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

//-----------------------------------------------------------------------------
/*!
   \brief   Read package from UDP socket

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

   \created     04.05.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCIpDispatcherWinSock::ReadUdp(std::vector<uint8> & orc_Data, uint8 (&orau8_Ip)[4])
{
   sint32 s32_Return = C_NOACT;

   if (mc_SocketsUdpServer.size() == 0)
   {
      osc_write_log_error("openSYDE IP-TP", "ReadUdp called with no socket.");
      s32_Return = C_CONFIG;
   }
   else
   {
      for (uint32 u32_Interface = 0U; u32_Interface < mc_SocketsUdpServer.size(); u32_Interface++)
      {
         if (mc_SocketsUdpClient[u32_Interface] != m_WsInvalidSocket())
         {
            //do we have a package in RX buffer ?
            uint32 u32_SizeInBuffer;
            sintn sn_Return = ioctlsocket(mc_SocketsUdpServer[u32_Interface], m_WsFionRead(), &u32_SizeInBuffer);
            if ((sn_Return != SOCKET_ERROR) && (u32_SizeInBuffer >= 1U))
            {
               sockaddr_in t_Sender;
               sintn sn_AddressSize = sizeof(t_Sender);
               //enough bytes: read
               orc_Data.resize(u32_SizeInBuffer);

               //lint -e{926,929} Side-effect of the "char"-based API. No problems as long as we are on Windows.
               //lint -e{740}     Side-effect of the POSIX-style API. Match is guaranteed by the API.
               sn_Return = recvfrom(mc_SocketsUdpServer[u32_Interface], reinterpret_cast<charn *>(&orc_Data[0]),
                                    orc_Data.size(), 0, reinterpret_cast<sockaddr *>(&t_Sender),
                                    &sn_AddressSize);

               //there might be more than one package in the buffer; recvfrom only reads one
               //so we check whether we have more than zero bytes:
               if (sn_Return > 0)
               {
                  //extract sender address
                  orau8_Ip[0] = t_Sender.sin_addr.S_un.S_un_b.s_b1;
                  orau8_Ip[1] = t_Sender.sin_addr.S_un.S_un_b.s_b2;
                  orau8_Ip[2] = t_Sender.sin_addr.S_un.S_un_b.s_b3;
                  orau8_Ip[3] = t_Sender.sin_addr.S_un.S_un_b.s_b4;

                  if (sn_Return != static_cast<sintn>(orc_Data.size()))
                  {
                     orc_Data.resize(sn_Return); //we only need the data we really received
                  }

                  //filter out local reception of broadcasts we sent ourselves
                  //strategy to identify those:
                  //Responses from real nodes are sent from Port 13400
                  //Our broadcasts are sent with dynamically assigned Port != 13400
                  //Also: we should not just accept anything that is thrown upon us ...
                  if (t_Sender.sin_port == htons(13400))
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
                                      C_SCLString::IntToStr(
                                         orc_Data.size()) + " Read size: " + C_SCLString::IntToStr(sn_Return));
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
