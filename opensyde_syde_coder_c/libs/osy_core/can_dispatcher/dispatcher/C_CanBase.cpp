//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN base class

   Abstract base class for interfacing with different CAN communication driver architectures.

   \copyright   Copyright 2010 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CanBase.hpp"
#include "C_SclDynamicArray.hpp"

//----------------------------------------------------------------------------------------------------------------------

using namespace stw::errors;
using namespace stw::can;
using namespace stw::scl;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

C_CanBase::C_CanBase(const uint8_t ou8_CommChannel)
{
   m_InitClassBase();
   mu8_CommChannel = ou8_CommChannel;
}

//----------------------------------------------------------------------------------------------------------------------

void C_CanBase::m_InitClassBase(void)
{
   mu8_CANOpened = 0U;
   mu8_CommChannel = 0U;
   mq_XTDAvailable = false;
}

//----------------------------------------------------------------------------------------------------------------------

void C_CanBase::SetCommChannel(const uint8_t ou8_CommChannel)
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

C_CanBase::C_CanBase(void)
{
   m_InitClassBase();
}

//----------------------------------------------------------------------------------------------------------------------

C_CanBase::~C_CanBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------

bool C_CanBase::GetXTDAvailable(void) const
{
   return mq_XTDAvailable;
}

//----------------------------------------------------------------------------------------------------------------------

uint8_t C_CanBase::GetCommChannel(void) const
{
   return mu8_CommChannel;
}

//----------------------------------------------------------------------------------------------------------------------
