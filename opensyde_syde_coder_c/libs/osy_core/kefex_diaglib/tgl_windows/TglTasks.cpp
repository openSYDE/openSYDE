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
#include "precomp_headers.hpp" //pre-compiled headers
#ifdef __BORLANDC__            //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <windows.h>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglTasks.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::tgl;
using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

C_TglCriticalSection::C_TglCriticalSection(void)
{
   InitializeCriticalSection(&mc_CriticalSection);
}

//----------------------------------------------------------------------------------------------------------------------

C_TglCriticalSection::~C_TglCriticalSection(void)
{
   DeleteCriticalSection(&mc_CriticalSection);
}

//----------------------------------------------------------------------------------------------------------------------
//block until the section is released, then claim it and return
void C_TglCriticalSection::Acquire(void)
{
   EnterCriticalSection(&mc_CriticalSection);
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
   bool q_Return;

   q_Return = (TryEnterCriticalSection(&mc_CriticalSection) == 0) ? false : true;
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
//release claimed section
void C_TglCriticalSection::Release(void)
{
   LeaveCriticalSection(&mc_CriticalSection);
}

//----------------------------------------------------------------------------------------------------------------------
