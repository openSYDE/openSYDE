//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       C_CAN_DLL class implementation.

   Class implementing an interface to communication driver DLLs in the STW-CAN-DLL
   format.
   Encapsulates dynamic loading and handling of the DLL.
   Concurrent access to the DLL from multiple threads is blocked by a critical section.

   The public functions are directly rerouted to the corresponding DLL functions.
   A return value of STWERRORS::C_NOACT signals that the requested function is not available in the loaded DLL.

   For a detailed description of the underlying DLL-functions cf. the STW-comm-DLL specification.

   \copyright   Copyright 2008 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h" //pre-compiled headers

#ifdef __BORLANDC__ //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "stwtypes.h"
#include "stwerrors.h"
#include "CCANDLL.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_can;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
static const uint32 mu32_XTD_ID = 0x80000000UL; // MSB flag for 29Bit ID

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//lint -esym(1565,"stw_can::C_CAN_DLL::mt_Lock") //properly initialized by constructor
C_CAN_DLL::C_CAN_DLL(const charn * const opcn_DLLPath)
{
   mpv_DLL = NULL; // DLL-Handle
   m_ClearFunctionPointers();

   InitializeCriticalSection(&mt_Lock);

   (void)m_Init(opcn_DLLPath);
}

//----------------------------------------------------------------------------------------------------------------------

C_CAN_DLL::~C_CAN_DLL()
{
   m_Exit();
   DeleteCriticalSection(&mt_Lock);
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::m_Init(const charn * const opcn_DLLPath)
{
   mpv_DLL = LoadLibraryA(opcn_DLLPath);
   if (mpv_DLL != NULL) //load DLL
   {
      //lint -save -e929  //casting the result of GetProcAddress is the only way to convert into a function pointer
      mpr_CAN_Init           = reinterpret_cast<PR_CAN_INIT>(GetProcAddress(mpv_DLL, "CAN_Init"));
      mpr_CAN_Exit           = reinterpret_cast<PR_CAN_EXIT>(GetProcAddress(mpv_DLL, "CAN_Exit"));
      mpr_CAN_Bitrate        = reinterpret_cast<PR_CAN_BITRATE>(GetProcAddress(mpv_DLL, "CAN_Bitrate"));
      mpr_CAN_Read_Msg       = reinterpret_cast<PR_CAN_READ_MSG>(GetProcAddress(mpv_DLL, "CAN_Read_Msg"));
      mpr_CAN_Send_Msg       = reinterpret_cast<PR_CAN_SEND_MSG>(GetProcAddress(mpv_DLL, "CAN_Send_Msg"));
      mpr_CAN_Send_RTR       = reinterpret_cast<PR_CAN_SEND_RTR>(GetProcAddress(mpv_DLL, "CAN_Send_RTR"));
      mpr_CAN_Reset          = reinterpret_cast<PR_CAN_RESET>(GetProcAddress(mpv_DLL, "CAN_Reset"));
      mpr_CAN_InterfaceSetup = reinterpret_cast<PR_CAN_INTERFACESETUP>(GetProcAddress(mpv_DLL, "CAN_InterfaceSetup"));

      //"optional" functions:
      mpr_CAN_Status         = reinterpret_cast<PR_CAN_STATUS>(GetProcAddress(mpv_DLL, "CAN_Status"));
      mpr_CAN_DLL_Info       = reinterpret_cast<PR_CAN_DLL_INFO>(GetProcAddress(mpv_DLL, "CAN_DLL_Info"));
      mpr_CAN_Read_extMsg    = reinterpret_cast<PR_CAN_READ_EXTMSG>(GetProcAddress(mpv_DLL, "CAN_Read_extMsg"));
      mpr_CAN_Send_extMsg    = reinterpret_cast<PR_CAN_SEND_EXTMSG>(GetProcAddress(mpv_DLL, "CAN_Send_extMsg"));
      mpr_CAN_Send_extRTR    = reinterpret_cast<PR_CAN_SEND_EXTRTR>(GetProcAddress(mpv_DLL, "CAN_Send_extRTR"));
      mpr_CAN_Init_One_ID    = reinterpret_cast<PR_CAN_INIT_ONE_ID>(GetProcAddress(mpv_DLL, "CAN_Init_One_ID"));

      //modem functions
      mpr_CANTAPI_CONNECT    = reinterpret_cast<PR_CANTAPI_CONNECT>(GetProcAddress(mpv_DLL, "CANTAPI_Connect"));
      mpr_CANTAPI_DISCONNECT = reinterpret_cast<PR_CANTAPI_DISCONNECT>(GetProcAddress(mpv_DLL, "CANTAPI_Disconnect"));

      //stream functions:
      mpr_SER_GET_TX_BUF_COUNT = reinterpret_cast<PR_SER_GET_TX_BUF_COUNT>(GetProcAddress(mpv_DLL,
                                                                                          "SER_Get_TX_Buf_Count"));
      mpr_SER_GET_RX_BUF_COUNT = reinterpret_cast<PR_SER_GET_RX_BUF_COUNT>(GetProcAddress(mpv_DLL,
                                                                                          "SER_Get_RX_Buf_Count"));
      mpr_SER_SEND_BYTES       = reinterpret_cast<PR_SER_SEND_BYTES>(GetProcAddress(mpv_DLL, "SER_Send_Bytes"));
      mpr_SER_READ_BYTES       = reinterpret_cast<PR_SER_READ_BYTES>(GetProcAddress(mpv_DLL, "SER_Read_Bytes"));

      //new style "ext" functions:
      mpr_CANext_Init             = reinterpret_cast<PR_CANext_Init>(GetProcAddress(mpv_DLL, "CANext_Init"));
      mpr_CANext_Exit             = reinterpret_cast<PR_CANext_Exit>(GetProcAddress(mpv_DLL, "CANext_Exit"));
      mpr_CANext_Bitrate          = reinterpret_cast<PR_CANext_Bitrate>(GetProcAddress(mpv_DLL, "CANext_Bitrate"));
      mpr_CANext_Read_Msg         = reinterpret_cast<PR_CANext_Read_Msg>(GetProcAddress(mpv_DLL, "CANext_Read_Msg"));
      mpr_CANext_Send_Msg         = reinterpret_cast<PR_CANext_Send_Msg>(GetProcAddress(mpv_DLL, "CANext_Send_Msg"));
      mpr_CANext_InterfaceSetup   = reinterpret_cast<PR_CANext_InterfaceSetup>(GetProcAddress(mpv_DLL,
                                                                                              "CANext_InterfaceSetup"));
      mpr_CANext_Status           = reinterpret_cast<PR_CANext_Status>(GetProcAddress(mpv_DLL, "CANext_Status"));
      mpr_CANext_Init_One_ID      = reinterpret_cast<PR_CANext_Init_One_ID>(GetProcAddress(mpv_DLL,
                                                                                           "CANext_Init_One_ID"));
      mpr_CANext_DLL_Info         = reinterpret_cast<PR_CANext_DLL_Info>(GetProcAddress(mpv_DLL, "CANext_DLL_Info"));
      mpr_CANext_Get_Num_Channels = reinterpret_cast<PR_CANext_Get_Num_Channels>(GetProcAddress(mpv_DLL,
                                                                                                "CANext_Get_Num_Channels"));
      mpr_CANext_Get_System_Time  = reinterpret_cast<PR_CANext_Get_System_Time>(GetProcAddress(mpv_DLL,
                                                                                               "CANext_Get_System_Time"));
      mpr_CANext_Get_Num_Supported_Bitrates = reinterpret_cast<PR_CANext_Get_Num_Supported_Bitrates>(
         GetProcAddress(mpv_DLL, "CANext_Get_Num_Supported_Bitrates"));
      mpr_CANext_Get_Supported_Bitrate = reinterpret_cast<PR_CANext_Get_Supported_Bitrate>(
         GetProcAddress(mpv_DLL, "CANext_Get_Supported_Bitrate"));

      mpr_CANtcp_Read_Device_List_From_Server = reinterpret_cast<PR_CANtcp_Read_Device_List_From_Server>
                                                (GetProcAddress(mpv_DLL, "CANtcp_ReadDeviceListFromServer"));
      mpr_CANtcp_Get_Device_Name = reinterpret_cast<PR_CANtcp_Get_Device_Name>(GetProcAddress(mpv_DLL,
                                                                                              "CANtcp_GetDeviceName"));
      //lint -restore

      if ((mpr_CAN_Init == NULL) ||
          (mpr_CAN_Exit == NULL) ||
          (mpr_CAN_Bitrate == NULL) ||
          (mpr_CAN_Read_Msg == NULL) ||
          (mpr_CAN_Send_Msg == NULL) ||
          (mpr_CAN_Send_RTR == NULL) ||
          (mpr_CAN_InterfaceSetup == NULL))
      {
         (void)FreeLibrary(mpv_DLL); // release library
         mpv_DLL = NULL;
         return (-2); // not all mandatory functions have been found
      }

      mq_ExtFunctionsAvailable =
         ((mpr_CANext_Init != NULL) &&
          (mpr_CANext_Exit != NULL) &&
          (mpr_CANext_Bitrate != NULL) &&
          (mpr_CANext_Read_Msg != NULL) &&
          (mpr_CANext_Send_Msg != NULL) &&
          (mpr_CANext_InterfaceSetup != NULL) &&
          (mpr_CANext_Status != NULL) &&
          (mpr_CANext_Init_One_ID != NULL) &&
          (mpr_CANext_DLL_Info != NULL) &&
          (mpr_CANext_Get_Num_Channels != NULL) &&
          (mpr_CANext_Get_System_Time != NULL)
         ) ? true : false;
      mq_BitrateInformationFunctionsAvailable =
         ((mpr_CANext_Get_Num_Supported_Bitrates != NULL) &&
          (mpr_CANext_Get_Supported_Bitrate != NULL)) ? true : false;

      mq_ModemFunctionsAvailable  = ((mpr_CANTAPI_CONNECT != NULL) && (mpr_CANTAPI_DISCONNECT != NULL));
      mq_StreamFunctionsAvailable = ((mpr_SER_GET_TX_BUF_COUNT != NULL) && (mpr_SER_GET_RX_BUF_COUNT != NULL) &&
                                     (mpr_SER_SEND_BYTES       != NULL) &&
                                     (mpr_SER_READ_BYTES       != NULL)) ? true : false;

      mq_TCPFunctionsAvailable =
         ((mpr_CANtcp_Read_Device_List_From_Server != NULL) && (mpr_CANtcp_Get_Device_Name != NULL));
   }
   else
   {
      return (-1); // Dll not loaded
   }
   return (0); // Dll loaded
} // End of Init_Dll

//----------------------------------------------------------------------------------------------------------------------

void C_CAN_DLL::m_ClearFunctionPointers(void)
{
   mpr_CAN_Init        = NULL;
   mpr_CAN_Exit        = NULL;
   mpr_CAN_Reset       = NULL;
   mpr_CAN_Bitrate     = NULL;
   mpr_CAN_Read_Msg    = NULL;
   mpr_CAN_Send_Msg    = NULL;
   mpr_CAN_Send_RTR    = NULL;
   mpr_CAN_InterfaceSetup = NULL;
   mpr_CAN_Status      = NULL;
   mpr_CAN_DLL_Info    = NULL;
   mpr_CAN_Read_extMsg = NULL;
   mpr_CAN_Send_extMsg = NULL;
   mpr_CAN_Send_extRTR = NULL;
   mpr_CAN_Init_One_ID = NULL;

   mpr_SER_GET_TX_BUF_COUNT = NULL;
   mpr_SER_GET_RX_BUF_COUNT = NULL;
   mpr_SER_SEND_BYTES       = NULL;
   mpr_SER_READ_BYTES       = NULL;

   mpr_CANTAPI_CONNECT     = NULL;
   mpr_CANTAPI_DISCONNECT  = NULL;

   mpr_CANext_Init         = NULL;
   mpr_CANext_Exit         = NULL;
   mpr_CANext_Bitrate      = NULL;
   mpr_CANext_Read_Msg     = NULL;
   mpr_CANext_Send_Msg     = NULL;
   mpr_CANext_InterfaceSetup = NULL;
   mpr_CANext_Status       = NULL;
   mpr_CANext_Init_One_ID  = NULL;
   mpr_CANext_DLL_Info     = NULL;
   mpr_CANext_Get_Num_Channels = NULL;
   mpr_CANext_Get_System_Time = NULL;
   mpr_CANext_Get_Num_Supported_Bitrates = NULL;
   mpr_CANext_Get_Supported_Bitrate = NULL;

   mpr_CANtcp_Read_Device_List_From_Server = NULL;
   mpr_CANtcp_Get_Device_Name = NULL;

   mq_ExtFunctionsAvailable = false;
   mq_ModemFunctionsAvailable = false;
   mq_StreamFunctionsAvailable = false;
   mq_BitrateInformationFunctionsAvailable = false;
   mq_TCPFunctionsAvailable = false;
}

//----------------------------------------------------------------------------------------------------------------------

void C_CAN_DLL::m_Exit(void)
{
   if (mpv_DLL != NULL)
   {
      (void)FreeLibrary(mpv_DLL); // release library
      mpv_DLL = NULL;
      m_ClearFunctionPointers();
   }
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CAN_Init(void)
{
   sint32 s32_Return;

   if (mpr_CAN_Init == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CAN_Init)();
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CAN_Reset(void)
{
   sint32 s32_Return;

   if (mpr_CAN_Reset == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CAN_Reset)();
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CAN_Exit(void)
{
   sint32 s32_Return;

   if (mpr_CAN_Exit == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CAN_Exit)();
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CAN_Bitrate(const sint32 os32_BitrateKBitS)
{
   sint32 s32_Return;

   if (mpr_CAN_Bitrate == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CAN_Bitrate)(os32_BitrateKBitS);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CAN_Read_Msg(STW_CAN_MSG & orc_Message)
{
   sint32 s32_Return;

   if (mpr_CAN_Read_Msg == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CAN_Read_Msg)(&orc_Message);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CAN_Send_Msg(const STW_CAN_MSG & orc_Message)
{
   sint32 s32_Return;

   if (mpr_CAN_Send_Msg == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CAN_Send_Msg)(&orc_Message);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CAN_Read_extMsg(STW_CAN_EXTMSG & orc_Message)
{
   sint32 s32_Return;

   if (mpr_CAN_Read_extMsg == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CAN_Read_extMsg)(&orc_Message);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CAN_Send_extMsg(const STW_CAN_EXTMSG & orc_Message)
{
   sint32 s32_Return;

   if (mpr_CAN_Send_extMsg == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CAN_Send_extMsg)(&orc_Message);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CAN_Send_RTR(const STW_CAN_MSG & orc_Message)
{
   sint32 s32_Return;

   if (mpr_CAN_Send_RTR == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CAN_Send_RTR)(&orc_Message);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CAN_Send_extRTR(const STW_CAN_EXTMSG & orc_Message)
{
   sint32 s32_Return;

   if (mpr_CAN_Send_extRTR == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CAN_Send_extRTR)(&orc_Message);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CAN_InterfaceSetup(const HWND opv_Handle)
{
   if (mpr_CAN_InterfaceSetup == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   (*mpr_CAN_InterfaceSetup)(opv_Handle);
   LeaveCriticalSection(&mt_Lock);
   return 0;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CAN_Status(STW_CAN_STATUS & orc_Status)
{
   sint32 s32_Return;

   if (mpr_CAN_Status == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CAN_Status)(&orc_Status);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CAN_DLL_Info(STW_CAN_DLL_INFO & orc_Info)
{
   sint32 s32_Return;

   if (mpr_CAN_DLL_Info == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CAN_DLL_Info)(&orc_Info);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CAN_Init_One_ID(const sint32 os32_BitrateKBitS, const uint32 ou32_ID)
{
   sint32 s32_Return;

   if (mpr_CAN_Init_One_ID == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CAN_Init_One_ID)(os32_BitrateKBitS, ou32_ID);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANext_Init(const uint8 ou8_Channel)
{
   sint32 s32_Return;

   if (mpr_CANext_Init == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANext_Init)(ou8_Channel);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANext_Exit(const uint8 ou8_Channel)
{
   sint32 s32_Return;

   if (mpr_CANext_Exit == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANext_Exit)(ou8_Channel);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANext_Bitrate(const uint8 ou8_Channel, const sint32 os32_BitrateKBitS)
{
   sint32 s32_Return;

   if (mpr_CANext_Bitrate == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANext_Bitrate)(ou8_Channel, os32_BitrateKBitS);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANext_Read_Msg(const uint8 ou8_Channel, T_STWCAN_Msg_RX & orc_Message)
{
   sint32 s32_Return;

   if (mpr_CANext_Read_Msg == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANext_Read_Msg)(ou8_Channel, &orc_Message);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANext_Send_Msg(const uint8 ou8_Channel, const T_STWCAN_Msg_TX & orc_Message)
{
   sint32 s32_Return;

   if (mpr_CANext_Send_Msg == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANext_Send_Msg)(ou8_Channel, &orc_Message);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANext_InterfaceSetup(const uint8 ou8_Channel)
{
   sint32 s32_Return;

   if (mpr_CANext_InterfaceSetup == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANext_InterfaceSetup)(ou8_Channel);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANext_Status(const uint8 ou8_Channel, T_STWCAN_Status & orc_Status)
{
   sint32 s32_Return;

   if (mpr_CANext_Status == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANext_Status)(ou8_Channel, &orc_Status);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANext_Init_One_ID(const uint8 ou8_Channel, const sint32 os32_BitrateKBitS, const uint32 ou32_ID)
{
   sint32 s32_Return;

   if (mpr_CANext_Init_One_ID == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANext_Init_One_ID)(ou8_Channel, os32_BitrateKBitS, ou32_ID);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANext_DLL_Info(const uint8 ou8_Channel, T_STWCAN_DLLInfo & orc_Info)
{
   sint32 s32_Return;

   if (mpr_CANext_DLL_Info == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANext_DLL_Info)(ou8_Channel, &orc_Info);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANext_Get_Num_Channels(void)
{
   sint32 s32_Return;

   if (mpr_CANext_Get_Num_Channels == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANext_Get_Num_Channels)();
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANext_Get_System_Time(uint64 & oru64_SystemTimeUs)
{
   sint32 s32_Return;

   if (mpr_CANext_Get_System_Time == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANext_Get_System_Time)(&oru64_SystemTimeUs);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANTAPI_Connect(const uint8 * const opu8_Number, const uint32 ou32_TimeOut)
{
   sint32 s32_Return;

   if (mpr_CANTAPI_CONNECT == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANTAPI_CONNECT)(opu8_Number, ou32_TimeOut);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANTAPI_Disconnect(const uint32 ou32_TimeOut)
{
   sint32 s32_Return;

   if (mpr_CANTAPI_DISCONNECT == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANTAPI_DISCONNECT)(ou32_TimeOut);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::SER_Get_TX_Buf_Count(uint32 & oru32_NumBytes, uint32 & oru32_MaxBufSize)
{
   sint32 s32_Return;

   if (mpr_SER_GET_TX_BUF_COUNT == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_SER_GET_TX_BUF_COUNT)(&oru32_NumBytes, &oru32_MaxBufSize);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::SER_Get_RX_Buf_Count(uint32 & oru32_NumBytes, uint32 & oru32_MaxBufSize)
{
   sint32 s32_Return;

   if (mpr_SER_GET_RX_BUF_COUNT == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_SER_GET_RX_BUF_COUNT)(&oru32_NumBytes, &oru32_MaxBufSize);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::SER_Send_Bytes(const uint8 * const opu8_Data, const uint32 ou32_NumBytes)
{
   sint32 s32_Return;

   if (mpr_SER_SEND_BYTES == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_SER_SEND_BYTES)(opu8_Data, ou32_NumBytes);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::SER_Read_Bytes(uint8 * const opu8_Data, uint32 & oru32_NumBytes)
{
   sint32 s32_Return;

   if (mpr_SER_READ_BYTES == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_SER_READ_BYTES)(opu8_Data, &oru32_NumBytes);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANext_Get_Num_Supported_Bitrates(uint32 & oru32_MultiplicationFactor)
{
   sint32 s32_Return;

   if (mpr_CANext_Get_Num_Supported_Bitrates == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANext_Get_Num_Supported_Bitrates)(&oru32_MultiplicationFactor);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANext_Get_Supported_Bitrate(const uint16 ou16_BitrateIndex, uint32 & oru32_Bitrate)
{
   sint32 s32_Return;

   if (mpr_CANext_Get_Supported_Bitrate == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANext_Get_Supported_Bitrate)(ou16_BitrateIndex, &oru32_Bitrate);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANtcp_Read_Device_List_From_Server(const uint8 ou8_Channel, uint32 & oru32_NoOfDevices,
                                                      uint32 & oru32_MaxLen)
{
   sint32 s32_Return;

   if (mpr_CANtcp_Read_Device_List_From_Server == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANtcp_Read_Device_List_From_Server)(ou8_Channel, &oru32_NoOfDevices, &oru32_MaxLen);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN_DLL::CANtcp_Get_Device_Name(const uint8 ou8_Channel, const uint32 ou32_Index,
                                         charn * const opcn_DeviceName)
{
   sint32 s32_Return;

   if (mpr_CANtcp_Get_Device_Name == NULL)
   {
      return C_NOACT;
   }
   EnterCriticalSection(&mt_Lock);
   s32_Return = (*mpr_CANtcp_Get_Device_Name)(ou8_Channel, ou32_Index, opcn_DeviceName);
   LeaveCriticalSection(&mt_Lock);
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

void C_CAN_DLL::STWCANMSG_to_CANMSGRX(T_STWCAN_Msg_RX & orc_Target, const STW_CAN_MSG & orc_Source)
{
   orc_Target.u32_ID = orc_Source.id;
   orc_Target.u8_XTD = 0U;
   orc_Target.u8_DLC = orc_Source.length;
   orc_Target.u8_RTR = 0U;
   orc_Target.u8_Align = 0U;
   (void)memcpy(orc_Target.au8_Data, orc_Source.msg, 8U);
   orc_Target.u64_TimeStamp = 0U;
}

//----------------------------------------------------------------------------------------------------------------------

void C_CAN_DLL::STWCANEXTMSG_to_CANMSGRX(T_STWCAN_Msg_RX & orc_Target, const STW_CAN_EXTMSG & orc_Source)
{
   orc_Target.u32_ID = orc_Source.id & 0x7FFFFFFFU;
   orc_Target.u8_XTD = static_cast<uint8>((orc_Source.id & mu32_XTD_ID) >> 31);
   orc_Target.u8_DLC = orc_Source.length;
   orc_Target.u8_RTR = 0U;
   orc_Target.u8_Align = 0U;
   (void)memcpy(orc_Target.au8_Data, orc_Source.msg, 8U);
   orc_Target.u64_TimeStamp = 0U;
}

//----------------------------------------------------------------------------------------------------------------------

void C_CAN_DLL::CANMSGTX_to_STWCANMSG(STW_CAN_MSG & orc_Target, const T_STWCAN_Msg_TX & orc_Source)
{
   orc_Target.id      = static_cast<uint16>(orc_Source.u32_ID);
   orc_Target.length  = orc_Source.u8_DLC;
   orc_Target.overrun = 0U;
   (void)memcpy(orc_Target.msg, orc_Source.au8_Data, 8U);
}

//----------------------------------------------------------------------------------------------------------------------

void C_CAN_DLL::CANMSGTX_to_STWCANEXTMSG(STW_CAN_EXTMSG & orc_Target, const T_STWCAN_Msg_TX & orc_Source)
{
   orc_Target.id      = orc_Source.u32_ID;
   orc_Target.id     |= (orc_Source.u8_XTD == 1U) ? 0x80000000UL : 0U;
   orc_Target.length  = orc_Source.u8_DLC;
   orc_Target.overrun = 0U;
   (void)memcpy(orc_Target.msg, orc_Source.au8_Data, 8U);
}

//----------------------------------------------------------------------------------------------------------------------
