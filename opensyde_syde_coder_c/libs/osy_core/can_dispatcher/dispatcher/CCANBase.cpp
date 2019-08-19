//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN base class

   Abstract base class for interfacing with different CAN communication driver architectures.

   \copyright   Copyright 2010 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#ifdef __BORLANDC__ //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "stwtypes.h"
#include "stwerrors.h"
#include "CCANBase.h"
#include "SCLDynamicArray.h"

//----------------------------------------------------------------------------------------------------------------------

using namespace stw_types;
using namespace stw_errors;
using namespace stw_can;
using namespace stw_scl;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

C_CAN_Base::C_CAN_Base(const uint8 ou8_CommChannel)
{
   m_InitClassBase();
   mu8_CommChannel = ou8_CommChannel;
}

//----------------------------------------------------------------------------------------------------------------------

void C_CAN_Base::m_InitClassBase(void)
{
   mu8_CANOpened = 0U;
   mu8_CommChannel = 0U;
   mq_XTDAvailable = false;
}

//----------------------------------------------------------------------------------------------------------------------

void C_CAN_Base::SetCommChannel(const uint8 ou8_CommChannel)
{
   if (mu8_CANOpened == 1U)
   {
      throw "Error in C_CAN_Base::SetDLLCommChannel: Channel can not be modified after being initialized !";
   }
   else
   {
      mu8_CommChannel = ou8_CommChannel;
   }
}

//----------------------------------------------------------------------------------------------------------------------

C_CAN_Base::C_CAN_Base(void)
{
   m_InitClassBase();
}

//----------------------------------------------------------------------------------------------------------------------

C_CAN_Base::~C_CAN_Base(void)
{
}

//----------------------------------------------------------------------------------------------------------------------

bool C_CAN_Base::GetXTDAvailable(void) const
{
   return mq_XTDAvailable;
}

//----------------------------------------------------------------------------------------------------------------------

uint8 C_CAN_Base::GetCommChannel(void) const
{
   return mu8_CommChannel;
}

//----------------------------------------------------------------------------------------------------------------------
