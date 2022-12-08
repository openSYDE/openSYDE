//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Thread safe core communication driver base class (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMCOMDRIVERBASE_H
#define C_CAMCOMDRIVERBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stw_can.hpp"
#include "TglTasks.hpp"

#include "C_OscComDriverBase.hpp"
/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamComDriverBase :
   public stw::opensyde_core::C_OscComDriverBase
{
public:
   C_CamComDriverBase(void);
   ~C_CamComDriverBase(void) override;

   int32_t StartLogging(const int32_t os32_Bitrate) override;
   void StopLogging(void) override;

   void ContinueLogging(void) override;
   void PauseLogging(void) override;

   void UpdateBitrate(const int32_t os32_Bitrate) override;

   void DistributeMessages(void) override;
   void SendCanMessageQueued(const stw::can::T_STWCAN_Msg_TX & orc_Msg) override;

   void SendCanMessage(const stw::opensyde_core::C_OscComDriverBaseCanMessage & orc_MsgCfg) override;
   void AddCyclicCanMessage(const stw::opensyde_core::C_OscComDriverBaseCanMessage & orc_MsgCfg) override;
   void RemoveCyclicCanMessage(const stw::opensyde_core::C_OscComDriverBaseCanMessage & orc_MsgCfg) override;
   void RemoveAllCyclicCanMessages(void) override;

private:
   // It is mutable because of the constness of the getter functions. Without the keyword mutable the getter functions
   // must be non const and that is not wanted.
   mutable stw::tgl::C_TglCriticalSection mc_CriticalSectionMsg;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
