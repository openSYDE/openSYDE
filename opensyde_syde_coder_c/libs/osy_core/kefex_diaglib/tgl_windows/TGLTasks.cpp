//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib Target Glue Layer: Task system related functionality

   cf. header for details

   Here: Implementation for Windows.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h" //pre-compiled headers
#ifdef __BORLANDC__          //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <windows.h>
#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLTasks.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

C_TGLCriticalSection::C_TGLCriticalSection(void)
{
   InitializeCriticalSection(&mt_CriticalSection);
}

//----------------------------------------------------------------------------------------------------------------------

C_TGLCriticalSection::~C_TGLCriticalSection(void)
{
   DeleteCriticalSection(&mt_CriticalSection);
}

//----------------------------------------------------------------------------------------------------------------------
//block until the section is released, then claim it and return
void C_TGLCriticalSection::Acquire(void)
{
   EnterCriticalSection(&mt_CriticalSection);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   claim the section if it is free; do not if it is not

   \return
   true      section was claimed
   false     section was not claimed is it is already occupied
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TGLCriticalSection::TryAcquire(void)
{
   bool q_Return;
   q_Return = (TryEnterCriticalSection(&mt_CriticalSection) == 0) ? false : true;
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
//release claimed section
void C_TGLCriticalSection::Release(void)
{
   LeaveCriticalSection(&mt_CriticalSection);
}

//----------------------------------------------------------------------------------------------------------------------

SCLDynamicArray<UINT_PTR>     C_TGLTimer::mhc_Handles;
SCLDynamicArray<C_TGLTimer *> C_TGLTimer::mhc_Instances;

//----------------------------------------------------------------------------------------------------------------------

C_TGLTimer::C_TGLTimer(void)
{
   mpr_Callback         = NULL;
   mpv_CallbackInstance = NULL;
   u32_Interval         = 1000U;
   mun_TimerHandle        = 0;
   mq_Enabled = false; //make sure SetEnabled works !
   SetEnabled(true);   //start !
}

//----------------------------------------------------------------------------------------------------------------------

C_TGLTimer::C_TGLTimer(const PR_TGLTimer_Event opr_Callback, void * const opv_Instance, const uint32 ou32_Interval,
                       const bool oq_Enabled)
{
   mpr_Callback         = opr_Callback;
   mpv_CallbackInstance = opv_Instance;
   u32_Interval         = ou32_Interval;
   mun_TimerHandle        = 0;
   mq_Enabled = !oq_Enabled; //make sure SetEnabled works !
   SetEnabled(oq_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------

C_TGLTimer::~C_TGLTimer(void)
{
   m_KillTimer();
   mpv_CallbackInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------

void C_TGLTimer::SetEventCallback(const PR_TGLTimer_Event opr_Callback, void * const opv_Instance)
{
   mpr_Callback         = opr_Callback;
   mpv_CallbackInstance = opv_Instance;
   m_UpdateTimer();
}

//----------------------------------------------------------------------------------------------------------------------

void C_TGLTimer::SetInterval(const stw_types::uint32 ou32_Interval)
{
   u32_Interval = ou32_Interval;
   m_UpdateTimer();
}

//----------------------------------------------------------------------------------------------------------------------

void __stdcall C_TGLTimer::m_TimerCallback(HWND opv_Hwnd, const stw_types::uintn oun_Msg,
                                           const UINT_PTR oun_TimerHandle, const stw_types::uint32 ou32_Time)
{
   sint32 s32_Index;

   (void)opv_Hwnd;
   (void)oun_Msg;
   (void)ou32_Time;

   //search table for instances:
   for (s32_Index = 0; s32_Index < mhc_Handles.GetLength(); s32_Index++)
   {
      if (mhc_Handles[s32_Index] == oun_TimerHandle)
      {
         mhc_Instances[s32_Index]->mpr_Callback(mhc_Instances[s32_Index]->mpv_CallbackInstance);
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------

void C_TGLTimer::m_KillTimer(void)
{
   sint32 s32_Index;

   if (mun_TimerHandle != 0)
   {
      KillTimer(NULL, mun_TimerHandle);

      //remove from static callback lookup list:
      //search table for instances:
      for (s32_Index = 0; s32_Index < mhc_Handles.GetLength(); s32_Index++)
      {
         if (mhc_Handles[s32_Index] == mun_TimerHandle)
         {
            mhc_Handles.Delete(s32_Index);
            mhc_Instances.Delete(s32_Index);
            break;
         }
      }

      mun_TimerHandle = 0;
   }
}

//----------------------------------------------------------------------------------------------------------------------

void C_TGLTimer::m_UpdateTimer(void)
{
   m_KillTimer();
   if ((u32_Interval != 0U) && (mq_Enabled == true) && (mpr_Callback != NULL))
   {
      mun_TimerHandle = SetTimer(NULL, 0U, u32_Interval, &C_TGLTimer::m_TimerCallback);
      if (mun_TimerHandle == 0)
      {
         throw "C_TGLTimer: SetTimer failed. Out of resources ?";
      }
      else
      {
         //add to callback lookup list:
         mhc_Handles.IncLength();
         mhc_Instances.IncLength();

         mhc_Handles[mhc_Handles.GetHigh()]   = mun_TimerHandle;
         mhc_Instances[mhc_Handles.GetHigh()] = this;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------

void C_TGLTimer::SetEnabled(const bool oq_Enabled)
{
   if (mq_Enabled != oq_Enabled)
   {
      mq_Enabled = oq_Enabled;
      m_UpdateTimer();
   }
}

//----------------------------------------------------------------------------------------------------------------------

bool C_TGLTimer::GetEnabled(void) const
{
   return mq_Enabled;
}

//----------------------------------------------------------------------------------------------------------------------

uint32 C_TGLTimer::GetInterval(void) const
{
   return u32_Interval;
}

//----------------------------------------------------------------------------------------------------------------------
