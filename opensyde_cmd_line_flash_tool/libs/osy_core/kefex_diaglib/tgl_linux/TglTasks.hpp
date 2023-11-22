//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib Target Glue Layer: Task system related functionality.

   DiagLib Target Glue Layer module containing task system related functionality.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef TGLTASKSHPP
#define TGLTASKSHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <pthread.h>
#include "stwtypes.hpp"
#include "C_SclDynamicArray.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace tgl
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
///Implements a handler for critical sections
class C_TglCriticalSection
{
protected:
   pthread_mutex_t mt_mutex; ///< synchronization object

private:
   //this class can not be copied:
   C_TglCriticalSection(const C_TglCriticalSection & orc_Source);
   C_TglCriticalSection & operator = (const C_TglCriticalSection & orc_Source);

public:
   C_TglCriticalSection(void);
   virtual ~C_TglCriticalSection(void);

   void Acquire(void);
   bool TryAcquire(void);
   void Release(void);
};


/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
