//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class to find bus or node by string

   Class to find bus or node by string

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SclString.hpp"
#include "C_UtiFindNameHelper.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor

   Save string

   \param[in]     orc_Cmp   Search string
*/
//----------------------------------------------------------------------------------------------------------------------
C_UtiFindNameHelper::C_UtiFindNameHelper(const stw::scl::C_SclString & orc_Cmp) :
   mc_Cmp(orc_Cmp)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compare string with string

   \param[in] opc_Cur String to compare

   \return
   true: match
   false: no match
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UtiFindNameHelper::operator ()(const stw::scl::C_SclString * const opc_Cur) const
{
   bool q_Retval = false;

   if (opc_Cur != NULL)
   {
      if (*opc_Cur == mc_Cmp)
      {
         q_Retval = true;
      }
   }
   return q_Retval;
}
