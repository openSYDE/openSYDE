//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol IP driver interface

   \class       stw_opensyde_core::C_OSCIpDispatcher
   \brief       openSYDE protocol IP driver interface

   Defines an interface for client applications for IP based communication with openSYDE protocol server nodes.

   Provides interfaces to:
   - generic TCP client/server communication for point-to-point services
   - UDP send/receive functions for sending and receiving broadcasts

   Typically each client protocol driver uses one IpDispatcher to communicate with one server node.
   Specific implementations can use e.g. the WinSock or POSIX APIs.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIPDISPATCHERH
#define C_OSCIPDISPATCHERH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIpDispatcher
{
private:
   //not implemented -> prevent copying
   C_OSCIpDispatcher(const C_OSCIpDispatcher & orc_Source);
   //not implemented -> prevent assignment
   C_OSCIpDispatcher & operator = (const C_OSCIpDispatcher & orc_Source);

protected:
   static const stw_types::uint16 mhu16_UDP_TCP_PORT = 13400U;

   stw_types::uint32 mu32_ConnectionTimeoutSeconds;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Notification about dropped TCP connection

      Shall be executed when the implementation detects a dropped TCP connection.
      So the application can react to that event (e.g. reconnect or cry)

      \param[in]  ou32_Handle   handle obtained by InitTcp()
   */
   //-----------------------------------------------------------------------------
   //lint -e{9175}  //intentionally no functionality in default implementation
   virtual void m_OnTcpConnectionDropped(const stw_types::uint32 ou32_Handle)
   {
      (void)ou32_Handle;
      //default: do nothing ...
   }

public:
   C_OSCIpDispatcher(const stw_types::uint16 ou16_ConnectionTimeoutSeconds)
   {
      mu32_ConnectionTimeoutSeconds = ou16_ConnectionTimeoutSeconds;
   }
   C_OSCIpDispatcher(void)
   {
      mu32_ConnectionTimeoutSeconds = 2U;
   }
   virtual ~C_OSCIpDispatcher(void)
   {
   }

   //-----------------------------------------------------------------------------
   /*!
      \brief   Initialize TCP communication

      Jobs to perform:
      - create non-blocking TCP client socket
      - connect TCP socket to port 13400 of specified server node
      Function shall return when connected or after timeout.

      \param[in]     orau8_Ip      IP address of server to connect to
      \param[out]    oru32_Handle  handle to new TCP connection (to be used in subsequent calls of TCP functions)

      \return
      C_NO_ERR   connected ...
      C_NOACT    connection failed
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 InitTcp(const stw_types::uint8 (&orau8_Ip)[4], stw_types::uint32 & oru32_Handle) = 0;

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
   virtual stw_types::sint32 IsTcpConnected(const stw_types::uint32 ou32_Handle) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Reconnect TCP socket

      This function can be used by a client to reconnect after the connection was lost (e.g. dropped by server).
      If there is still an active connection the function shall close it before reconnecting.
      Function shall return when connected or after timeout.

      Jobs to perform:
      - re-create non-blocking TCP client socket
      - re-connect TCP socket to port 13400 of specified server node

     \param[in]   ou32_Handle   handle obtained by InitTcp()

      \return
      C_NO_ERR   reconnected
      C_NOACT    connection failed
      C_RANGE    invalid handle
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 ReConnectTcp(const stw_types::uint32 ou32_Handle) = 0;

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
      C_NOACT    creation failed
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 InitUdp(void) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Disconnect from server node

      Jobs to perform:
      - close opened TCP socket

      \param[in]   ou32_Handle   handle obtained by InitTcp()

      \return
      C_NO_ERR   disconnected ...
      C_RANGE    invalid handle
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 CloseTcp(const stw_types::uint32 ou32_Handle) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Close UDP sockets

      Jobs to perform:
      - close opened UDP sockets

      \return
      C_NO_ERR   disconnected ...
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 CloseUdp(void) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Send package on TCP socket

      Jobs to perform:
      - send specified data to configured TCP socket

      \param[in]  ou32_Handle   handle obtained by InitTcp()
      \param[in]  orc_Data      data to sent

      \return
      C_NO_ERR   data sent successfully
      C_RD_WR    error sending data
      C_RANGE    invalid handle
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 SendTcp(const stw_types::uint32 ou32_Handle,
                                     const std::vector<stw_types::uint8> & orc_Data) = 0;

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
      C_NOACT    not enough bytes
      C_RD_WR    error reading data
      C_RANGE    invalid handle
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 ReadTcp(const stw_types::uint32 ou32_Handle, std::vector<stw_types::uint8> & orc_Data) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Read data from TCP socket

      Jobs to perform:
      - check whether TCP receive buffer contains required number of data bytes
      - Check if the identifier are matching (starting at byte zero). The header must be already read separately.
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
      C_NOACT    not enough bytes
      C_RD_WR    error reading data
      C_RANGE    invalid handle
      C_WARN     data is not for the server and/or client with the node identifier and bus identifier
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 ReadTcp(const stw_types::uint32 ou32_Handle,
                                     const stw_types::uint8 ou8_ClientBusIdentifier,
                                     const stw_types::uint8 ou8_ClientNodeIdentifier,
                                     const stw_types::uint8 ou8_ServerBusIdentifier,
                                     const stw_types::uint8 ou8_ServerNodeIdentifier,
                                     std::vector<stw_types::uint8> & orc_Data) = 0;

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
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 ReadTcpBuffer(const stw_types::uint8 ou8_ClientBusIdentifier,
                                           const stw_types::uint8 ou8_ClientNodeIdentifier,
                                           const stw_types::uint8 ou8_ServerBusIdentifier,
                                           const stw_types::uint8 ou8_ServerNodeIdentifier,
                                           std::vector<stw_types::uint8> & orc_Data) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Send package on UDP socket

      Jobs to perform:
      Go through all set up UDP "client" sockets and send specified data as broadcast.

      \param[in]  orc_Data   data to sent

      \return
      C_NO_ERR   data sent successfully
      C_RD_WR    error sending data
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 SendUdp(const std::vector<stw_types::uint8> & orc_Data) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Read package from UDP socket

      Go through all set up UDP "server" sockets and try to read incoming datagram from UDP socket.
      Report datagrams sent from:
      * any IP
      * port 13400 (to ignore reception of our own broadcasts)
      Return the read data.

      \param[out]  orc_Data   received data
      \param[out]  orau8_Ip   IP address of sender of data

      \return
      C_NO_ERR   data sent successfully
      C_NOACT    no data received
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 ReadUdp(std::vector<stw_types::uint8> &orc_Data, stw_types::uint8(&orau8_Ip)[4]) = 0;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
