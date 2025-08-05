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
#include "precomp_headers.hpp" //pre-compiled headers

#include <limits.h>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_Can.hpp"
#include "C_SclString.hpp"
#include "C_SclDynamicArray.hpp"
#include "TglTime.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::can;
using namespace stw::scl;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

C_Can::C_Can(const uint8_t ou8_CommChannel) :
   C_CanDispatcher(ou8_CommChannel),
   mc_DllName(""),
   mpc_Can(NULL),
   mq_DLLOpened(false),
   mu32_RxId(CAN_RX_ID_INVALID)
{
}

//----------------------------------------------------------------------------------------------------------------------

C_Can::C_Can(void) :
   C_CanDispatcher(),
   mc_DllName(""),
   mpc_Can(NULL),
   mq_DLLOpened(false),
   mu32_RxId(CAN_RX_ID_INVALID)
{
}

//----------------------------------------------------------------------------------------------------------------------
//sequence of shutting down: 1. close can; 2. close DLL; 3. delete instance
C_Can::~C_Can(void)
{
   if (mu8_CANOpened != 0U)
   {
      (void)this->CAN_Exit();
   }

   if (mq_DLLOpened == true)
   {
      (void)this->DLL_Close();
   }

   try
   {
      delete mpc_Can;
   }
   catch (...)
   {
   }
   mpc_Can = NULL;
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::CAN_DLL_Info(STW_CAN_DLL_INFO & orc_Info) const
{
   if ((mq_DLLOpened == false) || (mpc_Can == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_Can->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   if (mpc_Can->mq_ExtFunctionsAvailable == false)
   {
      return mpc_Can->CAN_DLL_Info(orc_Info);
   }
   return mpc_Can->CANext_DLL_Info(mu8_CommChannel, orc_Info);
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
int32_t C_Can::CAN_Exit(void)
{
   int32_t s32_Return;

   if ((mq_DLLOpened == false) || (mpc_Can == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_Can->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   mu8_CANOpened = 0U;
   if (mpc_Can->mq_ExtFunctionsAvailable == false)
   {
      s32_Return = mpc_Can->CAN_Exit();
   }
   else
   {
      s32_Return = mpc_Can->CANext_Exit(mu8_CommChannel);
   }

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
int32_t C_Can::CAN_Reset(void)
{
   if ((mq_DLLOpened == false) || (mpc_Can == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_Can->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   if (mpc_Can->mq_ExtFunctionsAvailable == false)
   {
      return mpc_Can->CAN_Reset();
   }
   (void)mpc_Can->CANext_Exit(mu8_CommChannel);
   return mpc_Can->CANext_Init(mu8_CommChannel);
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
int32_t C_Can::CAN_Init(void)
{
   int32_t s32_Return;

   if ((mq_DLLOpened == false) || (mpc_Can == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }

   if ((mu8_CommChannel > 0U) && (mpc_Can->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   if (mpc_Can->mq_ExtFunctionsAvailable == false)
   {
      if (mu32_RxId == CAN_RX_ID_INVALID)
      {
         s32_Return = mpc_Can->CAN_Init();
      }
      else
      {
         s32_Return = mpc_Can->CAN_Init_One_ID(0, mu32_RxId);
      }
   }
   else
   {
      if (mu32_RxId == CAN_RX_ID_INVALID)
      {
         s32_Return = mpc_Can->CANext_Init(mu8_CommChannel);
      }
      else
      {
         s32_Return = mpc_Can->CANext_Init_One_ID(mu8_CommChannel, 0, mu32_RxId);
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
int32_t C_Can::CAN_Init(const int32_t os32_BitrateKBitS)
{
   int32_t s32_Return;

   if ((mq_DLLOpened == false) || (mpc_Can == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_Can->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   if (mpc_Can->mq_ExtFunctionsAvailable == false)
   {
      if (mu32_RxId == CAN_RX_ID_INVALID)
      {
         s32_Return = mpc_Can->CAN_Bitrate(os32_BitrateKBitS);
      }
      else
      {
         s32_Return = mpc_Can->CAN_Init_One_ID(os32_BitrateKBitS, mu32_RxId);
      }
   }
   else
   {
      if (mu32_RxId == CAN_RX_ID_INVALID)
      {
         s32_Return = mpc_Can->CANext_Bitrate(mu8_CommChannel, os32_BitrateKBitS);
      }
      else
      {
         s32_Return = mpc_Can->CANext_Init_One_ID(mu8_CommChannel, os32_BitrateKBitS, mu32_RxId);
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
int32_t C_Can::CAN_InteractiveSetup(void) const
{
   if ((mq_DLLOpened == false) || (mpc_Can == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_Can->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   if (mpc_Can->mq_ExtFunctionsAvailable == true)
   {
      return mpc_Can->CANext_InterfaceSetup(mu8_CommChannel);
   }
   return mpc_Can->CAN_InterfaceSetup(NULL);
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::m_SendMsgToDLL(const T_STWCAN_Msg_TX & orc_Message) const
{
   //assume: pre-conditions already checked
   int32_t s32_Return;

   if (mpc_Can == NULL)
   {
      return C_CONFIG; //will not happen in RL; defensive measure to pacify static code analysis
   }

   if (mpc_Can->mq_ExtFunctionsAvailable == true)
   {
      s32_Return = mpc_Can->CANext_Send_Msg(mu8_CommChannel, orc_Message);
   }
   else
   {
      if (mq_XTDAvailable == true)
      {
         STW_CAN_EXTMSG t_Msg;
         C_CanDll::CANMSGTX_to_STWCANEXTMSG(t_Msg, orc_Message);
         if (orc_Message.u8_RTR == 0U)
         {
            s32_Return = mpc_Can->CAN_Send_extMsg(t_Msg);
         }
         else
         {
            s32_Return = mpc_Can->CAN_Send_extRTR(t_Msg);
         }
      }
      else
      {
         STW_CAN_MSG t_Msg;
         C_CanDll::CANMSGTX_to_STWCANMSG(t_Msg, orc_Message);
         if (orc_Message.u8_RTR == 0U)
         {
            s32_Return = mpc_Can->CAN_Send_Msg(t_Msg);
         }
         else
         {
            s32_Return = mpc_Can->CAN_Send_RTR(t_Msg);
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::m_ReadMsgFromDLL(T_STWCAN_Msg_RX & orc_Message) const
{
   //assume: pre-conditions already checked
   int32_t s32_Return;

   if (mpc_Can == NULL)
   {
      return C_CONFIG; //will not happen in RL; defensive measure to pacify static code analysis
   }

   if (mpc_Can->mq_ExtFunctionsAvailable == true)
   {
      s32_Return = mpc_Can->CANext_Read_Msg(mu8_CommChannel, orc_Message);
   }
   else
   {
      if (mq_XTDAvailable == true)
      {
         STW_CAN_EXTMSG t_Msg;
         s32_Return = mpc_Can->CAN_Read_extMsg(t_Msg);
         if (s32_Return == C_NO_ERR)
         {
            C_CanDll::STWCANEXTMSG_to_CANMSGRX(orc_Message, t_Msg);
         }
      }
      else
      {
         STW_CAN_MSG t_Msg;
         s32_Return = mpc_Can->CAN_Read_Msg(t_Msg);
         if (s32_Return == 0)
         {
            C_CanDll::STWCANMSG_to_CANMSGRX(orc_Message, t_Msg);
         }
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      if (orc_Message.u64_TimeStamp == 0U)
      {
         //give it the best timestamp we have:
         orc_Message.u64_TimeStamp = TglGetTickCountUs();
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
int32_t C_Can::CAN_Send_Msg(const T_STWCAN_Msg_TX & orc_Message)
{
   if ((mq_DLLOpened == false) || (mpc_Can == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_Can->mq_ExtFunctionsAvailable == false))
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
int32_t C_Can::m_CAN_Read_Msg(T_STWCAN_Msg_RX & orc_Message)
{
   if ((mq_DLLOpened == false) || (mpc_Can == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_Can->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   return m_ReadMsgFromDLL(orc_Message);
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::CAN_Read_Msg_Timeout(const uint32_t ou32_MaxWaitTimeMs, T_STWCAN_Msg_RX & orc_Message) const
{
   int32_t s32_Return;
   uint32_t u32_StartTime;

   if ((mq_DLLOpened == false) || (mpc_Can == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_Can->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   // We have to poll actively ...
   u32_StartTime = TglGetTickCount();
   do
   {
      s32_Return = this->m_ReadMsgFromDLL(orc_Message);
   }
   while ((s32_Return != C_NO_ERR) && ((TglGetTickCount() - u32_StartTime) < ou32_MaxWaitTimeMs));

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
int32_t C_Can::CAN_Get_System_Time(uint64_t & oru64_SystemTimeUs) const
{
   if ((mq_DLLOpened == false) || (mpc_Can == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if (mpc_Can->mq_ExtFunctionsAvailable == true)
   {
      return mpc_Can->CANext_Get_System_Time(oru64_SystemTimeUs);
   }
   oru64_SystemTimeUs = TglGetTickCountUs(); //best we can do: we provide the timestamps ourself anyway
                                             // as long as there are no "ext" functions
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::CAN_Get_Supported_Bitrates(C_SclString & orc_Unit, C_SclDynamicArray<uint32_t> & orc_Bitrates,
                                          uint32_t & oru32_MultiplicationFactor) const
{
   int32_t s32_Return;
   uint32_t u32_Bitrate;
   int32_t s32_NumBitrates;
   int32_t s32_Loop;

   if ((mq_DLLOpened == false) || (mpc_Can == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if (mpc_Can->mq_BitrateInformationFunctionsAvailable == false)
   {
      return C_CONFIG;
   }

   s32_Return = mpc_Can->CANext_Get_Num_Supported_Bitrates(oru32_MultiplicationFactor);
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
      orc_Unit = C_SclString::IntToStr(oru32_MultiplicationFactor) + "Bit/s";
      break;
   }

   s32_NumBitrates = s32_Return;
   tgl_assert(s32_NumBitrates < 0xFFFF);

   orc_Bitrates.SetLength(s32_NumBitrates);

   for (s32_Loop = 0; s32_Loop < s32_NumBitrates; s32_Loop++)
   {
      s32_Return = mpc_Can->CANext_Get_Supported_Bitrate(static_cast<uint16_t>(s32_Loop), u32_Bitrate);
      if (s32_Return != C_NO_ERR)
      {
         return s32_Return;
      }
      orc_Bitrates[s32_Loop] = u32_Bitrate;
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::CAN_Status(T_STWCAN_Status & orc_Status) const
{
   if ((mq_DLLOpened == false) || (mpc_Can == NULL))
   {
      return CAN_COMP_ERR_DLL_NOT_OPENED;
   }
   if ((mu8_CommChannel > 0U) && (mpc_Can->mq_ExtFunctionsAvailable == false))
   {
      return C_CONFIG;
   }

   if (mpc_Can->mq_ExtFunctionsAvailable == false)
   {
      return mpc_Can->CAN_Status(orc_Status);
   }
   return mpc_Can->CANext_Status(mu8_CommChannel, orc_Status);
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::DLL_Close(void)
{
   if (mq_DLLOpened == true)
   {
      if (mu8_CANOpened != 0U)
      {
         mu8_CANOpened = 0U;
         (void)this->CAN_Exit();
      }
      mq_DLLOpened = false;
      delete mpc_Can;
      mpc_Can = NULL;

      return 0;
   }
   return CAN_COMP_ERR_DLL_ALREADY_CLOSED;
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::DLL_Open(void)
{
   int32_t s32_Return;
   T_STWCAN_DLLInfo t_DLLInfo;

   if (mq_DLLOpened == true)
   {
      return CAN_COMP_ERR_DLL_ALREADY_OPENED;
   }

   try
   {
      mpc_Can = new C_CanDll(mc_DllName.c_str());
   }
   catch (...)
   {
      delete mpc_Can;
      mpc_Can = NULL;
      return CAN_COMP_ERR_DLL_INIT; //could not init DLL
   }
   if (mpc_Can->mpv_DLL == NULL)
   {
      delete mpc_Can;
      mpc_Can = NULL;
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

int32_t C_Can::DLL_Open(const C_SclString & orc_FileName)
{
   mc_DllName = orc_FileName;
   return DLL_Open();
}

//----------------------------------------------------------------------------------------------------------------------

void C_Can::SetDLLName(const C_SclString & orc_DLLName)
{
   mc_DllName = orc_DLLName;
}

//----------------------------------------------------------------------------------------------------------------------

C_SclString C_Can::GetDLLName(void) const
{
   return mc_DllName;
}

//----------------------------------------------------------------------------------------------------------------------

void C_Can::SetLimitRXID(const uint32_t ou32_LimitRXID)
{
   mu32_RxId = ou32_LimitRXID;
}

//----------------------------------------------------------------------------------------------------------------------

uint32_t C_Can::GetLimitRXID(void) const
{
   return mu32_RxId;
}

//----------------------------------------------------------------------------------------------------------------------
