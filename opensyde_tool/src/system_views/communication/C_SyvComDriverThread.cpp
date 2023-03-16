//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Thread for openSYDE communication driver (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SyvComDriverThread.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]      opr_ThreadFunc   Function pointer to function which the thread shall call cyclic
   \param[in]      opv_Instance     Instance pointer to pass back when invoking read event callback
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvComDriverThread::C_SyvComDriverThread(const PR_ThreadFunction opr_ThreadFunc, void * const opv_Instance,
                                           QObject * const opc_Parent) :
   QThread(opc_Parent),
   mpr_ThreadFunc(opr_ThreadFunc),
   mpv_FuncInstance(opv_Instance)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Thread function which is executed by the thread
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverThread::run(void)
{
   if ((this->mpr_ThreadFunc != NULL) &&
       (this->mpv_FuncInstance != NULL))
   {
      while (this->isInterruptionRequested() == false)
      {
         this->mpr_ThreadFunc(this->mpv_FuncInstance);
      }
   }
}
