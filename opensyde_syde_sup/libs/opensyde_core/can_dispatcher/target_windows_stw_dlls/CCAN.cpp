//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       C_CAN class implementation.

   CAN driver class providing an API interface for CAN access under
   Windows using the STW CAN DLL drivers.

   \copyright   Copyright 2002 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h" //pre-compiled headers

#ifdef __BORLANDC__ //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <limits.h>
#include "stwtypes.h"
#include "stwerrors.h"
#include "CCAN.h"
#include "CSCLString.h"
#include "SCLDynamicArray.h"
#include "TGLTime.h"
#include "TGLUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_can;
using namespace stw_scl;
using namespace stw_tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

C_CAN::C_CAN(const uint8 ou8_CommChannel) :
   C_CAN_Dispatcher(ou8_CommChannel),
   mc_DLLName(""),
   mpc_CAN(NULL),
   mq_DLLOpened(false),
   mu32_RXID(CAN_RX_ID_INVALID)
{
}

//----------------------------------------------------------------------------------------------------------------------

C_CAN::C_CAN(void) :
   C_CAN_Dispatcher(),
   mc_DLLName(""),
   mpc_CAN(NULL),
   mq_DLLOpened(false),
   mu32_RXID(CAN_RX_ID_INVALID)
{
}

//----------------------------------------------------------------------------------------------------------------------
//sequence of shutting down: 1. close can; 2. close DLL; 3. delete instance
C_CAN::~C_CAN(void)
{
   if (mu8_CANOpened != 0U)
   {
      (void)this->CAN_Exit();
   }

   if (mq_DLLOpened == true)
   {
      (void)this->DLL_Close();
   }

   if (mpc_CAN != NULL)
   {
      try
      {
         delete mpc_CAN;
      }
      catch (...)
      {
      }
      mpc_CAN = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::CAN_DLL_Info(STW_CAN_DLL_INFO & orc_Info) const
{
   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_CAN->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   if (mpc_CAN->mq_ExtFunctionsAvailable == false)
   {
      return mpc_CAN->CAN_DLL_Info(orc_Info);
   }
   return mpc_CAN->CANext_DLL_Info(mu8_CommChannel, orc_Info);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Close CAN bus

   Will invoke DLL's "CAN_Exit" function.

   Depending on availability the "Ext" or regular DLL functions are used ("Ext" has priority).

   \return
   C_NO_ERR                          DLL function returned OK
   -1                                DLL function returned error
   CAN_COMP_ERR_DLL_NOT_OPENED       DLL was not yet loaded
   C_CONFIG                          channel other than 0 configured but not supported by loaded DLL
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CAN::CAN_Exit(void)
{
   sint32 s32_Return;

   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_CAN->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   if (mpc_CAN->mq_ExtFunctionsAvailable == false)
   {
      s32_Return = mpc_CAN->CAN_Exit();
   }
   else
   {
      s32_Return = mpc_CAN->CANext_Exit(mu8_CommChannel);
   }
   mu8_CANOpened = 0U;

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reset CAN bus

   Will invoke DLL's "CAN_Reset" function.

   Depending on availability the "Ext" or regular DLL functions are used ("Ext" has priority).
   The "Ext" set does not provide a specific "Reset" function so a sequence of "Exit" and "Init" is called in this case.

   \return
   C_NO_ERR                          DLL function returned OK
   -1                                DLL function returned error
   CAN_COMP_ERR_DLL_NOT_OPENED       DLL was not yet loaded
   C_CONFIG                          channel other than 0 configured but not supported by loaded DLL
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CAN::CAN_Reset(void)
{
   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_CAN->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   if (mpc_CAN->mq_ExtFunctionsAvailable == false)
   {
      return mpc_CAN->CAN_Reset();
   }
   (void)mpc_CAN->CANext_Exit(mu8_CommChannel);
   return mpc_CAN->CANext_Init(mu8_CommChannel);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize CAN bus

   Depending on availability the "Ext" or regular DLL functions are used ("Ext" has priority).
   Will use the DLL's "Init" function for initialization.
   If the "limit RX ID" is configured to a valid value the DLL's init function "Init_One_ID" will be used
    (limiting reception to exactly that ID).

   \return
   C_NO_ERR                          DLL function returned OK; bus initialized
   -1                                initialization error returned by DLL function
   CAN_COMP_ERR_DLL_NOT_OPENED       DLL was not yet loaded
   C_CONFIG                          channel other than 0 configured but not supported by loaded DLL
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CAN::CAN_Init(void)
{
   sint32 s32_Return;

   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }

   if ((mu8_CommChannel > 0U) && (mpc_CAN->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   if (mpc_CAN->mq_ExtFunctionsAvailable == false)
   {
      if (mu32_RXID == CAN_RX_ID_INVALID)
      {
         s32_Return = mpc_CAN->CAN_Init();
      }
      else
      {
         s32_Return = mpc_CAN->CAN_Init_One_ID(0, mu32_RXID);
      }
   }
   else
   {
      if (mu32_RXID == CAN_RX_ID_INVALID)
      {
         s32_Return = mpc_CAN->CANext_Init(mu8_CommChannel);
      }
      else
      {
         s32_Return = mpc_CAN->CANext_Init_One_ID(mu8_CommChannel, 0, mu32_RXID);
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      mu8_CANOpened = 1U;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize CAN bus to specific bitrate

   Depending on availability the "Ext" or regular DLL functions are used ("Ext" has priority).
   Will use the DLL's "CAN_Bitrate" function for initialization.
   If the "limit RX ID" is configured to a valid value the DLL's init function "Init_One_ID" will be used
    (limiting reception to exactly that ID).

  \param[in] os32_BitrateKBitS   bitrate to pass for initialization

   \return
   C_NO_ERR                          DLL function returned OK; bus initialized
   -1                                initialization error returned by DLL function
   CAN_COMP_ERR_DLL_NOT_OPENED       DLL was not yet loaded
   C_CONFIG                          channel other than 0 configured but not supported by loaded DLL
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CAN::CAN_Init(const sint32 os32_BitrateKBitS)
{
   sint32 s32_Return;

   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_CAN->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   if (mpc_CAN->mq_ExtFunctionsAvailable == false)
   {
      if (mu32_RXID == CAN_RX_ID_INVALID)
      {
         s32_Return = mpc_CAN->CAN_Bitrate(os32_BitrateKBitS);
      }
      else
      {
         s32_Return = mpc_CAN->CAN_Init_One_ID(os32_BitrateKBitS, mu32_RXID);
      }
   }
   else
   {
      if (mu32_RXID == CAN_RX_ID_INVALID)
      {
         s32_Return = mpc_CAN->CANext_Bitrate(mu8_CommChannel, os32_BitrateKBitS);
      }
      else
      {
         s32_Return = mpc_CAN->CANext_Init_One_ID(mu8_CommChannel, os32_BitrateKBitS, mu32_RXID);
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      mu8_CANOpened = 1U;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show DLL's configuration dialog

   Depending on availability the "Ext" or regular DLL functions are used ("Ext" has priority).
   Execute's the DLL's blocking configuration dialog.

   Only the "Ext" function type returns the information whether the user clocked "OK" or "CANCEL".
   The this function can only return "1" if the "Ext" functions are available.

   \return
   C_NO_ERR                          DLL function returned OK   -> user clicked "CANCEL"
                                     or: non "Ext" function was executed
   1                                 DLL function returned 1    -> user clicked "OK"
   -1                                DLL function returns error
   CAN_COMP_ERR_DLL_NOT_OPENED       DLL was not yet loaded
   C_CONFIG                          channel other than 0 configured but not supported by loaded DLL
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CAN::CAN_InteractiveSetup(void) const
{
   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_CAN->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   if (mpc_CAN->mq_ExtFunctionsAvailable == true)
   {
      return mpc_CAN->CANext_InterfaceSetup(mu8_CommChannel);
   }
   return mpc_CAN->CAN_InterfaceSetup(NULL);
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::m_SendMsgToDLL(const T_STWCAN_Msg_TX & orc_Message) const
{
   //assume: pre-conditions already checked
   sint32 s32_Return;

   if (mpc_CAN == NULL)
   {
      return C_CONFIG; //will not happen in RL; defensive measure to pacify static code analysis
   }

   if (mpc_CAN->mq_ExtFunctionsAvailable == true)
   {
      s32_Return = mpc_CAN->CANext_Send_Msg(mu8_CommChannel, orc_Message);
   }
   else
   {
      if (mq_XTDAvailable == true)
      {
         STW_CAN_EXTMSG t_Msg;
         C_CAN_DLL::CANMSGTX_to_STWCANEXTMSG(t_Msg, orc_Message);
         if (orc_Message.u8_RTR == 0U)
         {
            s32_Return = mpc_CAN->CAN_Send_extMsg(t_Msg);
         }
         else
         {
            s32_Return = mpc_CAN->CAN_Send_extRTR(t_Msg);
         }
      }
      else
      {
         STW_CAN_MSG t_Msg;
         C_CAN_DLL::CANMSGTX_to_STWCANMSG(t_Msg, orc_Message);
         if (orc_Message.u8_RTR == 0U)
         {
            s32_Return = mpc_CAN->CAN_Send_Msg(t_Msg);
         }
         else
         {
            s32_Return = mpc_CAN->CAN_Send_RTR(t_Msg);
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::m_ReadMsgFromDLL(T_STWCAN_Msg_RX & orc_Message) const
{
   //assume: pre-conditions already checked
   sint32 s32_Return;

   if (mpc_CAN == NULL)
   {
      return C_CONFIG; //will not happen in RL; defensive measure to pacify static code analysis
   }

   if (mpc_CAN->mq_ExtFunctionsAvailable == true)
   {
      s32_Return = mpc_CAN->CANext_Read_Msg(mu8_CommChannel, orc_Message);
   }
   else
   {
      if (mq_XTDAvailable == true)
      {
         STW_CAN_EXTMSG t_Msg;
         s32_Return = mpc_CAN->CAN_Read_extMsg(t_Msg);
         if (s32_Return == C_NO_ERR)
         {
            C_CAN_DLL::STWCANEXTMSG_to_CANMSGRX(orc_Message, t_Msg);
         }
      }
      else
      {
         STW_CAN_MSG t_Msg;
         s32_Return = mpc_CAN->CAN_Read_Msg(t_Msg);
         if (s32_Return == 0)
         {
            C_CAN_DLL::STWCANMSG_to_CANMSGRX(orc_Message, t_Msg);
         }
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      if (orc_Message.u64_TimeStamp == 0U)
      {
         //give it the best timestamp we have:
         orc_Message.u64_TimeStamp = TGL_GetTickCountUS();
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send CAN message

   Send one CAN message using the DLL functions.
   Depending on availability the "Ext" or regular DLL functions are used ("Ext" has priority).

  \param[in] orc_Message   CAN message to send

   \return
   C_NO_ERR                          DLL function returned OK
   -1                                DLL function returns error
   CAN_COMP_ERR_DLL_NOT_OPENED       DLL was not yet loaded
   C_CONFIG                          channel other than 0 configured but not supported by loaded DLL
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CAN::CAN_Send_Msg(const T_STWCAN_Msg_TX & orc_Message)
{
   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_CAN->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }
   return m_SendMsgToDLL(orc_Message);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read CAN message

   Read one CAN message using the DLL functions.
   Depending on availability the "Ext" or regular DLL functions are used ("Ext" has priority).

  \param[out] orc_Message   read message

   \return
   C_NO_ERR                          DLL function returned OK; message placed in orc_Message
   -1                                DLL function returns error
   CAN_COMP_ERR_DLL_NOT_OPENED       DLL was not yet loaded
   C_CONFIG                          channel other than 0 configured but not supported by loaded DLL
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CAN::m_CAN_Read_Msg(T_STWCAN_Msg_RX & orc_Message)
{
   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_CAN->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   return m_ReadMsgFromDLL(orc_Message);
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::CAN_Read_Msg_Timeout(const uint32 ou32_MaxWaitTimeMS, T_STWCAN_Msg_RX & orc_Message) const
{
   sint32 s32_Return;
   uint32 u32_StartTime;

   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_CAN->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   // We have to poll actively ...
   u32_StartTime = TGL_GetTickCount();
   do
   {
      s32_Return = this->m_ReadMsgFromDLL(orc_Message);
   }
   while ((s32_Return != C_NO_ERR) && ((TGL_GetTickCount() - u32_StartTime) < ou32_MaxWaitTimeMS));

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get DLL's opinion about system time

   Will invoke DLL's "Get_System_Time" function.

   Depending on availability the "Ext" or regular DLL functions are used ("Ext" has priority).
   The "NonExt" set does not provide this functions; so in this case the time since system start is returned instead.

   \param[out]   oru64_SystemTimeUs    system time in micro-seconds as reported by the DLL

   \return
   C_NO_ERR                          DLL function returned OK
   -1                                DLL function returned error
   CAN_COMP_ERR_DLL_NOT_OPENED       DLL was not yet loaded
   C_CONFIG                          channel other than 0 configured but not supported by loaded DLL
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CAN::CAN_Get_System_Time(uint64 & oru64_SystemTimeUs) const
{
   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if (mpc_CAN->mq_ExtFunctionsAvailable == true)
   {
      return mpc_CAN->CANext_Get_System_Time(oru64_SystemTimeUs);
   }
   oru64_SystemTimeUs = TGL_GetTickCountUS(); //best we can do: we provide the timestamps ourself anyway
                                              // as long as there are no "ext" functions
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::CAN_Get_Supported_Bitrates(C_SCLString & orc_Unit, SCLDynamicArray<uint32> & orc_Bitrates,
                                         uint32 & oru32_MultiplicationFactor) const
{
   sint32 s32_Return;
   uint32 u32_Bitrate;
   sint32 s32_NumBitrates;
   sint32 s32_Loop;

   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if (mpc_CAN->mq_BitrateInformationFunctionsAvailable == false)
   {
      return C_CONFIG;
   }

   s32_Return = mpc_CAN->CANext_Get_Num_Supported_Bitrates(oru32_MultiplicationFactor);
   if (s32_Return < C_NO_ERR)
   {
      return s32_Return;
   }

   switch (oru32_MultiplicationFactor)
   {
   case 1U:
      orc_Unit = "bit/s";
      break;
   case 1000U:
      orc_Unit = "kBit/s";
      break;
   case 1000000U:
      orc_Unit = "mBit/s";
      break;
   default:
      orc_Unit = C_SCLString::IntToStr(oru32_MultiplicationFactor) + "Bit/s";
      break;
   }

   s32_NumBitrates = s32_Return;
   tgl_assert(s32_NumBitrates < 0xFFFF);

   orc_Bitrates.SetLength(s32_NumBitrates);

   for (s32_Loop = 0; s32_Loop < s32_NumBitrates; s32_Loop++)
   {
      s32_Return = mpc_CAN->CANext_Get_Supported_Bitrate(static_cast<uint16>(s32_Loop), u32_Bitrate);
      if (s32_Return != C_NO_ERR)
      {
         return s32_Return;
      }
      orc_Bitrates[s32_Loop] = u32_Bitrate;
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::CAN_Status(T_STWCAN_Status & orc_Status) const
{
   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_CAN->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   if (mpc_CAN->mq_ExtFunctionsAvailable == false)
   {
      return mpc_CAN->CAN_Status(orc_Status);
   }
   return mpc_CAN->CANext_Status(mu8_CommChannel, orc_Status);
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::DLL_Close(void)
{
   if (mq_DLLOpened == true)
   {
      if (mu8_CANOpened != 0U)
      {
         (void)this->CAN_Exit();
      }
      delete mpc_CAN;
      mpc_CAN = NULL;

      mu8_CANOpened = 0U;
      mq_DLLOpened = false;
      return 0;
   }
   return CAN_COMP_ERR_DLL_ALREADY_CLOSED;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::DLL_Open(void)
{
   sint32 s32_Return;
   T_STWCAN_DLLInfo t_DLLInfo;

   if (mq_DLLOpened == true)
   {
      return CAN_COMP_ERR_DLL_ALREADY_OPENED;
   }

   try
   {
      mpc_CAN = new C_CAN_DLL(mc_DLLName.c_str());
   }
   catch (...)
   {
      delete mpc_CAN;
      mpc_CAN = NULL;
      return CAN_COMP_ERR_DLL_INIT; //could not init DLL
   }
   if (mpc_CAN->mpv_DLL == NULL)
   {
      delete mpc_CAN;
      mpc_CAN = NULL;
      return CAN_COMP_ERR_DLL_FORMAT; //DLL not in STW CAN-DLL format
   }

   mq_DLLOpened = true;

   //check if XTD is possible
   mq_XTDAvailable = false;
   s32_Return = CAN_DLL_Info(t_DLLInfo);
   {
      if (s32_Return == C_NO_ERR)
      {
         mq_XTDAvailable = (t_DLLInfo.bXTDSupported == 1U) ? true : false;
      }
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::DLL_Open(const C_SCLString & orc_FileName)
{
   mc_DLLName = orc_FileName;
   return DLL_Open();
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::CANTAPI_Connect(const uint8 * const opu8_Number, const uint32 ou32_TimeOut)
{
   sint32 s32_Return;

   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   s32_Return = mpc_CAN->CANTAPI_Connect(opu8_Number, ou32_TimeOut);
   if (s32_Return != C_NO_ERR)
   {
      return s32_Return;
   }
   mu8_CANOpened = 1U;
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::CANTAPI_Disconnect(const uint32 ou32_TimeOut)
{
   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   mu8_CANOpened = 0U;
   return mpc_CAN->CANTAPI_Disconnect(ou32_TimeOut);
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::SER_Get_TX_Buf_Count(uint32 & oru32_NumBytes, uint32 & oru32_MaxBufSize) const
{
   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   return mpc_CAN->SER_Get_TX_Buf_Count(oru32_NumBytes, oru32_MaxBufSize);
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::SER_Send_Bytes(const uint8 * const opu8_Data, const uint32 ou32_NumBytes)
{
   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   return mpc_CAN->SER_Send_Bytes(opu8_Data, ou32_NumBytes);
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::SER_Get_RX_Buf_Count(uint32 & oru32_NumBytes, uint32 & oru32_MaxBufSize) const
{
   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   return mpc_CAN->SER_Get_RX_Buf_Count(oru32_NumBytes, oru32_MaxBufSize);
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::SER_Read_Bytes(uint8 * const opu8_Data, uint32 & oru32_NumBytes)
{
   if ((mq_DLLOpened == false) || (mpc_CAN == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   return mpc_CAN->SER_Read_Bytes(opu8_Data, oru32_NumBytes);
}

//----------------------------------------------------------------------------------------------------------------------

void C_CAN::SetDLLName(const C_SCLString & orc_DLLName)
{
   mc_DLLName = orc_DLLName;
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_CAN::GetDLLName(void) const
{
   return mc_DLLName;
}

//----------------------------------------------------------------------------------------------------------------------

void C_CAN::SetLimitRXID(const uint32 ou32_LimitRXID)
{
   mu32_RXID = ou32_LimitRXID;
}

//----------------------------------------------------------------------------------------------------------------------

uint32 C_CAN::GetLimitRXID(void) const
{
   return mu32_RXID;
}

//----------------------------------------------------------------------------------------------------------------------
