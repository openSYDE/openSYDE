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
#include "stwtypes.hpp"
#include "TglTasks.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

C_TglCriticalSection::C_TglCriticalSection(void)
{
   pthread_mutex_init(&mt_mutex, NULL);
}

//----------------------------------------------------------------------------------------------------------------------

C_TglCriticalSection::~C_TglCriticalSection(void)
{
   pthread_mutex_destroy(&mt_mutex);
}

//----------------------------------------------------------------------------------------------------------------------
//block until the section is released, then claim it and return
void C_TglCriticalSection::Acquire(void)
{
   pthread_mutex_lock(&mt_mutex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   claim the section if it is free; do not if it is not

   \return
   true      section was claimed
   false     section was not claimed is it is already occupied
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TglCriticalSection::TryAcquire(void)
{
   int sn_Ret;

   sn_Ret = pthread_mutex_trylock(&mt_mutex);
   return (sn_Ret == 0) ? true : false;
}

//----------------------------------------------------------------------------------------------------------------------
//release claimed section
void C_TglCriticalSection::Release(void)
{
   pthread_mutex_unlock(&mt_mutex);
}

//----------------------------------------------------------------------------------------------------------------------
