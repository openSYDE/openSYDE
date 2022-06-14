//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI topology: stores additional UI information (implementation)

   UI topology: stores additional UI information

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_PuiSdHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
C_PuiSdHandler * C_PuiSdHandler::mhpc_Singleton = NULL;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get singleton (Create if necessary)

   \return
   Pointer to singleton
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdHandler * C_PuiSdHandler::h_GetInstance(void)
{
   if (C_PuiSdHandler::mhpc_Singleton == NULL)
   {
      C_PuiSdHandler::mhpc_Singleton = new C_PuiSdHandler();
   }
   return C_PuiSdHandler::mhpc_Singleton;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up singleton
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandler::h_Destroy(void)
{
   delete C_PuiSdHandler::mhpc_Singleton;
   C_PuiSdHandler::mhpc_Singleton = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdHandler::C_PuiSdHandler(QObject * const opc_Parent) :
   C_PuiSdHandlerCanOpenLogic(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdHandler::~C_PuiSdHandler(void)
{
}
