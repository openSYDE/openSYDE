//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       C_CAN_DLL class implementation.

   Class implementing an interface to communication driver DLLs in the STW-CAN-DLL
   format.
   Encapsulates dynamic loading and handling of the DLL.
   Concurrent access to the DLL from multiple threads is blocked by a critical section.

   The public functions are directly rerouted to the corresponding DLL functions.
   A return value of stw_errors::C_NOACT signals that the requested function is not available in the loaded DLL.

   For a detailed description of the underlying DLL-functions cf. the STW-comm-DLL specification.

   \copyright   Copyright 2008 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CanDll.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::can;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
static const uint32_t mu32_XTD_ID = 0x80000000UL; // MSB flag for 29Bit ID

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//lint -esym(1565,"stw::can::C_CAN_DLL::mt_Lock") //properly initialized by constructor
C_CanDll::C_CanDll(const char_t * const opcn_DllPath)
{
   mpv_DLL = NULL; // DLL-Handle
   m_ClearFunctionPointers();

   InitializeCriticalSection(&mt_Lock);

   (void)m_Init(opcn_DllPath);
}

//----------------------------------------------------------------------------------------------------------------------

C_CanDll::~C_CanDll()
{
   m_Exit();
   DeleteCriticalSection(&mt_Lock);
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_CanDll::m_Init(const char_t * const opcn_DllPath)
{
   mpv_DLL = LoadLibraryA(opcn_DllPath);
   if (mpv_DLL != NULL) //load DLL
   {
      //Using intermediate casts to "void (*)(void)" is a workaround for a GCC >= 8.1(?) compiler warning regarding
      // incompatible function types (while still being compatible with other compilers)
      //lint -save -e929  //casting the result of GetProcAddress is the only way to convert into a function pointer
      mpr_CAN_Init           = reinterpret_cast<PR_CAN_INIT>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CAN_Init")));
      mpr_CAN_Exit           = reinterpret_cast<PR_CAN_EXIT>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CAN_Exit")));
      mpr_CAN_Bitrate        = reinterpret_cast<PR_CAN_BITRATE>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CAN_Bitrate")));
      mpr_CAN_Read_Msg       = reinterpret_cast<PR_CAN_READ_MSG>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CAN_Read_Msg")));
      mpr_CAN_Send_Msg       = reinterpret_cast<PR_CAN_SEND_MSG>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CAN_Send_Msg")));
      mpr_CAN_Send_RTR       = reinterpret_cast<PR_CAN_SEND_RTR>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CAN_Send_RTR")));
      mpr_CAN_Reset          = reinterpret_cast<PR_CAN_RESET>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CAN_Reset")));
      mpr_CAN_InterfaceSetup = reinterpret_cast<PR_CAN_INTERFACESETUP>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CAN_InterfaceSetup")));

      //"optional" functions:
      mpr_CAN_Status         = reinterpret_cast<PR_CAN_STATUS>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CAN_Status")));
      mpr_CAN_DLL_Info       = reinterpret_cast<PR_CAN_DLL_INFO>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CAN_DLL_Info")));
      mpr_CAN_Read_extMsg    = reinterpret_cast<PR_CAN_READ_EXTMSG>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CAN_Read_extMsg")));
      mpr_CAN_Send_extMsg    = reinterpret_cast<PR_CAN_SEND_EXTMSG>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CAN_Send_extMsg")));
      mpr_CAN_Send_extRTR    = reinterpret_cast<PR_CAN_SEND_EXTRTR>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CAN_Send_extRTR")));
      mpr_CAN_Init_One_ID    = reinterpret_cast<PR_CAN_INIT_ONE_ID>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CAN_Init_One_ID")));

      //new style "ext" functions:
      mpr_CANext_Init             = reinterpret_cast<PR_CANext_Init>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CANext_Init")));
      mpr_CANext_Exit             = reinterpret_cast<PR_CANext_Exit>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CANext_Exit")));
      mpr_CANext_Bitrate          = reinterpret_cast<PR_CANext_Bitrate>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CANext_Bitrate")));
      mpr_CANext_Read_Msg         = reinterpret_cast<PR_CANext_Read_Msg>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CANext_Read_Msg")));
      mpr_CANext_Send_Msg         = reinterpret_cast<PR_CANext_Send_Msg>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CANext_Send_Msg")));
      mpr_CANext_InterfaceSetup   = reinterpret_cast<PR_CANext_InterfaceSetup>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CANext_InterfaceSetup")));
      mpr_CANext_Status           = reinterpret_cast<PR_CANext_Status>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CANext_Status")));
      mpr_CANext_Init_One_ID      = reinterpret_cast<PR_CANext_Init_One_ID>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CANext_Init_One_ID")));
      mpr_CANext_DLL_Info         = reinterpret_cast<PR_CANext_DLL_Info>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CANext_DLL_Info")));
      mpr_CANext_Get_Num_Channels = reinterpret_cast<PR_CANext_Get_Num_Channels>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CANext_Get_Num_Channels")));
      mpr_CANext_Get_System_Time  = reinterpret_cast<PR_CANext_Get_System_Time>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CANext_Get_System_Time")));
      mpr_CANext_Get_Num_Supported_Bitrates = reinterpret_cast<PR_CANext_Get_Num_Supported_Bitrates>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CANext_Get_Num_Supported_Bitrates")));
      mpr_CANext_Get_Supported_Bitrate = reinterpret_cast<PR_CANext_Get_Supported_Bitrate>(
         reinterpret_cast<void (*)(void)>(GetProcAddress(mpv_DLL, "CANext_Get_Supported_Bitrate")));
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
   }
   else
   {
      return (-1); // Dll not loaded
   }
   return (0); // Dll loaded
}

//----------------------------------------------------------------------------------------------------------------------

void C_CanDll::m_ClearFunctionPointers(void)
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

   mq_ExtFunctionsAvailable = false;
   mq_BitrateInformationFunctionsAvailable = false;
}

//----------------------------------------------------------------------------------------------------------------------

void C_CanDll::m_Exit(void)
{
   if (mpv_DLL != NULL)
   {
      (void)FreeLibrary(mpv_DLL); // release library
      mpv_DLL = NULL;
      m_ClearFunctionPointers();
   }
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_CanDll::CAN_Init(void)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CAN_Reset(void)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CAN_Exit(void)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CAN_Bitrate(const int32_t os32_BitrateKBitS)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CAN_Read_Msg(STW_CAN_MSG & orc_Message)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CAN_Send_Msg(const STW_CAN_MSG & orc_Message)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CAN_Read_extMsg(STW_CAN_EXTMSG & orc_Message)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CAN_Send_extMsg(const STW_CAN_EXTMSG & orc_Message)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CAN_Send_RTR(const STW_CAN_MSG & orc_Message)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CAN_Send_extRTR(const STW_CAN_EXTMSG & orc_Message)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CAN_InterfaceSetup(const HWND opv_Handle)
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

int32_t C_CanDll::CAN_Status(STW_CAN_STATUS & orc_Status)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CAN_DLL_Info(STW_CAN_DLL_INFO & orc_Info)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CAN_Init_One_ID(const int32_t os32_BitrateKBitS, const uint32_t ou32_ID)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CANext_Init(const uint8_t ou8_Channel)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CANext_Exit(const uint8_t ou8_Channel)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CANext_Bitrate(const uint8_t ou8_Channel, const int32_t os32_BitrateKBitS)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CANext_Read_Msg(const uint8_t ou8_Channel, T_STWCAN_Msg_RX & orc_Message)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CANext_Send_Msg(const uint8_t ou8_Channel, const T_STWCAN_Msg_TX & orc_Message)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CANext_InterfaceSetup(const uint8_t ou8_Channel)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CANext_Status(const uint8_t ou8_Channel, T_STWCAN_Status & orc_Status)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CANext_Init_One_ID(const uint8_t ou8_Channel, const int32_t os32_BitrateKBitS, const uint32_t ou32_ID)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CANext_DLL_Info(const uint8_t ou8_Channel, T_STWCAN_DLLInfo & orc_Info)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CANext_Get_Num_Channels(void)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CANext_Get_System_Time(uint64_t & oru64_SystemTimeUs)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CANext_Get_Num_Supported_Bitrates(uint32_t & oru32_MultiplicationFactor)
{
   int32_t s32_Return;

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

int32_t C_CanDll::CANext_Get_Supported_Bitrate(const uint16_t ou16_BitrateIndex, uint32_t & oru32_Bitrate)
{
   int32_t s32_Return;

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

void C_CanDll::STWCANMSG_to_CANMSGRX(T_STWCAN_Msg_RX & orc_Target, const STW_CAN_MSG & orc_Source)
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

void C_CanDll::STWCANEXTMSG_to_CANMSGRX(T_STWCAN_Msg_RX & orc_Target, const STW_CAN_EXTMSG & orc_Source)
{
   orc_Target.u32_ID = orc_Source.id & 0x7FFFFFFFU;
   orc_Target.u8_XTD = static_cast<uint8_t>((orc_Source.id & mu32_XTD_ID) >> 31);
   orc_Target.u8_DLC = orc_Source.length;
   orc_Target.u8_RTR = 0U;
   orc_Target.u8_Align = 0U;
   (void)memcpy(orc_Target.au8_Data, orc_Source.msg, 8U);
   orc_Target.u64_TimeStamp = 0U;
}

//----------------------------------------------------------------------------------------------------------------------

void C_CanDll::CANMSGTX_to_STWCANMSG(STW_CAN_MSG & orc_Target, const T_STWCAN_Msg_TX & orc_Source)
{
   orc_Target.id      = static_cast<uint16_t>(orc_Source.u32_ID);
   orc_Target.length  = orc_Source.u8_DLC;
   orc_Target.overrun = 0U;
   (void)memcpy(orc_Target.msg, orc_Source.au8_Data, 8U);
}

//----------------------------------------------------------------------------------------------------------------------

void C_CanDll::CANMSGTX_to_STWCANEXTMSG(STW_CAN_EXTMSG & orc_Target, const T_STWCAN_Msg_TX & orc_Source)
{
   orc_Target.id      = orc_Source.u32_ID;
   orc_Target.id     |= (orc_Source.u8_XTD == 1U) ? 0x80000000UL : 0U;
   orc_Target.length  = orc_Source.u8_DLC;
   orc_Target.overrun = 0U;
   (void)memcpy(orc_Target.msg, orc_Source.au8_Data, 8U);
}

//----------------------------------------------------------------------------------------------------------------------
