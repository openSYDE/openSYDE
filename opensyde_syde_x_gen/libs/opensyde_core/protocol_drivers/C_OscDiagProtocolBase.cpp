//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Diagnostic protocol driver abstract base class

   For details cf. documentation in .h file.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <iostream>
#include "C_OscDiagProtocolBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   constructor

   Set up class
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscDiagProtocolBase::C_OscDiagProtocolBase(void) :
   mpr_OnDataPoolReadEventReceived(NULL),
   mpr_OnDataPoolReadEventErrorReceived(NULL),
   mpv_OnAsyncInstance(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   constructor

   Tear down class
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscDiagProtocolBase::~C_OscDiagProtocolBase(void)
{
   mpr_OnDataPoolReadEventReceived = NULL;
   mpr_OnDataPoolReadEventErrorReceived = NULL;
   mpv_OnAsyncInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialization of the protocol

   Setting all callbacks for the async messages

   \param[in]  opr_DataPoolReadEventReceived       function to be called if an async DataPoolReadEvent is received
   \param[in]  opr_DataPoolReadEventErrorReceived  function to be called if an async DataPoolReadEvent error message
                                                   is received
   \param[in]  opv_Instance                        instance pointer to pass back when invoking read event callback
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDiagProtocolBase::Initialize(const PR_DataPoolReadEventReceived opr_DataPoolReadEventReceived,
                                       const PR_DataPoolReadEventErrorReceived opr_DataPoolReadEventErrorReceived,
                                       void * const opv_Instance)
{
   mpr_OnDataPoolReadEventErrorReceived = opr_DataPoolReadEventErrorReceived;
   mpr_OnDataPoolReadEventReceived = opr_DataPoolReadEventReceived;
   mpv_OnAsyncInstance = opv_Instance;
}
