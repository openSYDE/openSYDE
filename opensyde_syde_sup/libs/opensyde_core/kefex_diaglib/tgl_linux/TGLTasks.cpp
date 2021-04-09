//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib Target Glue Layer: Task system related functionality.

   DiagLib Target Glue Layer module containing task system related functionality.
   Implementation for Linux.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <unistd.h>
#include <signal.h>
#include "stwtypes.h"
#include "TGLTasks.h"

using namespace stw_types;
using namespace stw_tgl;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */
C_TGLCriticalSection::C_TGLCriticalSection(void)
{
   pthread_mutex_init(&mt_mutex, NULL);
}

//----------------------------------------------------------------------------------------------------------------------

C_TGLCriticalSection::~C_TGLCriticalSection(void)
{
   pthread_mutex_destroy(&mt_mutex);
}

//----------------------------------------------------------------------------------------------------------------------

void C_TGLCriticalSection::Acquire(void)
{
   pthread_mutex_lock(&mt_mutex);
}

//----------------------------------------------------------------------------------------------------------------------

bool C_TGLCriticalSection::TryAcquire(void)
{
   int sn_Ret;

   sn_Ret = pthread_mutex_trylock(&mt_mutex);
   return (sn_Ret == 0) ? true : false;
}

//----------------------------------------------------------------------------------------------------------------------

void C_TGLCriticalSection::Release(void)
{
   pthread_mutex_unlock(&mt_mutex);
}

//----------------------------------------------------------------------------------------------------------------------


C_TGLTimer::C_TGLTimer(void)
{
   mpr_Callback         = NULL;
   mpv_CallbackInstance = NULL;
   mu32_Interval_ms     = 1000U;
   mq_ThreadRunning     = false;
   mq_Enabled = false; //make sure SetEnabled works !
   SetEnabled(true);   //start !
}

//----------------------------------------------------------------------------------------------------------------------

C_TGLTimer::C_TGLTimer(const PR_TGLTimer_Event opr_Callback, void * const opv_Instance, const uint32 ou32_Interval,
                       const bool oq_Enabled)
{
   mpr_Callback         = opr_Callback;
   mpv_CallbackInstance = opv_Instance;
   mu32_Interval_ms     = ou32_Interval;
   mq_ThreadRunning     = false;
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
   mu32_Interval_ms = ou32_Interval;
   m_UpdateTimer();
}

//----------------------------------------------------------------------------------------------------------------------

void * C_TGLTimer::m_Thread(void * opv_Arg)
{
   C_TGLTimer * pc_Instance = static_cast<C_TGLTimer*>(opv_Arg);

   while (pc_Instance->mq_ThreadRunning == true)
   {
      if (pc_Instance->mpr_Callback != NULL)
      {
         pc_Instance->mpr_Callback(pc_Instance->mpv_CallbackInstance);
      }

      // TODO: usleep might be interrupted by a signal -> re-sleep
      usleep((useconds_t)pc_Instance->mu32_Interval_ms * 1000U);
   }

   return NULL;
}

//----------------------------------------------------------------------------------------------------------------------

void C_TGLTimer::m_KillTimer(void)
{
   if (mq_ThreadRunning == true)
   {
      mq_ThreadRunning = false;
      pthread_kill(mt_ThreadHandle, SIGTERM);
      pthread_join(mt_ThreadHandle, NULL);
   }
}

//----------------------------------------------------------------------------------------------------------------------

void C_TGLTimer::m_UpdateTimer(void)
{
   int sn_Ret;

   m_KillTimer();
   if ((mu32_Interval_ms != 0U) && (mq_Enabled == true) && (mpr_Callback != NULL))
   {
      mq_ThreadRunning = true;
      sn_Ret = pthread_create(&mt_ThreadHandle, NULL, C_TGLTimer::m_Thread, (void*)this);
      if (sn_Ret != 0)
      {
         throw "C_TGLTimer: Starting timer failed. Out of resources ?";
         mq_ThreadRunning = false;
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
   return mu32_Interval_ms;
}

//----------------------------------------------------------------------------------------------------------------------
