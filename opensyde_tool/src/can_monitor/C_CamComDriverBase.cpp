//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Thread safe core communication driver base class (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_CamComDriverBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamComDriverBase::C_CamComDriverBase(void) :
   C_OscComDriverBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamComDriverBase::~C_CamComDriverBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts the logging

   This function is thread safe

   \param[in]  os32_Bitrate          CAN bitrate in kBit/s. Is used for the bus load calculation not the initialization

   \return
   C_NO_ERR                          CAN initialized and logging started
   C_CONFIG                          CAN dispatcher is not set
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamComDriverBase::StartLogging(const int32_t os32_Bitrate)
{
   int32_t s32_Return;

   this->mc_CriticalSectionMsg.Acquire();
   s32_Return = C_OscComDriverBase::StartLogging(os32_Bitrate);
   this->mc_CriticalSectionMsg.Release();

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stops the logging

   This function is thread safe
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamComDriverBase::StopLogging(void)
{
   this->mc_CriticalSectionMsg.Acquire();
   C_OscComDriverBase::StopLogging();
   this->mc_CriticalSectionMsg.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Continues the paused the logging

   This function is thread safe
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamComDriverBase::ContinueLogging(void)
{
   this->mc_CriticalSectionMsg.Acquire();
   C_OscComDriverBase::ContinueLogging();
   this->mc_CriticalSectionMsg.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Pauses the logging.

   This function is thread safe
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamComDriverBase::PauseLogging(void)
{
   this->mc_CriticalSectionMsg.Acquire();
   C_OscComDriverBase::PauseLogging();
   this->mc_CriticalSectionMsg.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update the bitrate to adapt the calculation for the bus load

   \param[in]  os32_Bitrate          CAN bitrate in kBit/s. Is used for the bus load calculation not the initialization
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamComDriverBase::UpdateBitrate(const int32_t os32_Bitrate)
{
   this->mc_CriticalSectionMsg.Acquire();
   C_OscComDriverBase::UpdateBitrate(os32_Bitrate);
   this->mc_CriticalSectionMsg.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Distributes the CAN message to all registered C_OscMessageLogger instances.

   This function is thread safe
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamComDriverBase::DistributeMessages(void)
{
   this->mc_CriticalSectionMsg.Acquire();
   C_OscComDriverBase::DistributeMessages();
   this->mc_CriticalSectionMsg.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sending a CAN message by putting it into the queue

   \param[in]  orc_Msg  Tx CAN message

   This function is thread safe
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamComDriverBase::SendCanMessageQueued(const stw::can::T_STWCAN_Msg_TX & orc_Msg)
{
   this->mc_CriticalSectionMsg.Acquire();
   C_OscComDriverBase::SendCanMessageQueued(orc_Msg);
   this->mc_CriticalSectionMsg.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends a CAN message with a specific configuration

   Interval is ignored in this function.
   Use AddCyclicCanMessage for registration of a cyclic CAN message.

   \param[in]     orc_MsgCfg         CAN message configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamComDriverBase::SendCanMessage(const C_OscComDriverBaseCanMessage & orc_MsgCfg)
{
   this->mc_CriticalSectionMsg.Acquire();
   C_OscComDriverBase::SendCanMessage(orc_MsgCfg);
   this->mc_CriticalSectionMsg.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Registers a cyclic CAN message with a specific configuration

   \param[in]     orc_MsgCfg         CAN message configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamComDriverBase::AddCyclicCanMessage(const C_OscComDriverBaseCanMessage & orc_MsgCfg)
{
   this->mc_CriticalSectionMsg.Acquire();
   C_OscComDriverBase::AddCyclicCanMessage(orc_MsgCfg);
   this->mc_CriticalSectionMsg.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes a cyclic CAN message with a specific configuration

   \param[in]     orc_MsgCfg         CAN message configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamComDriverBase::RemoveCyclicCanMessage(const C_OscComDriverBaseCanMessage & orc_MsgCfg)
{
   this->mc_CriticalSectionMsg.Acquire();
   C_OscComDriverBase::RemoveCyclicCanMessage(orc_MsgCfg);
   this->mc_CriticalSectionMsg.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes all cyclic CAN messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamComDriverBase::RemoveAllCyclicCanMessages(void)
{
   this->mc_CriticalSectionMsg.Acquire();
   C_OscComDriverBase::RemoveAllCyclicCanMessages();
   this->mc_CriticalSectionMsg.Release();
}
