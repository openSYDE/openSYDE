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
#include "stw_can.h"
#include "TGLTasks.h"

#include "C_OSCComDriverBase.h"
/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamComDriverBase :
   public stw_opensyde_core::C_OSCComDriverBase
{
public:
   C_CamComDriverBase(void);
   virtual ~C_CamComDriverBase(void);

   virtual stw_types::sint32 StartLogging(const stw_types::sint32 os32_Bitrate) override;
   virtual void StopLogging(void) override;

   virtual void ContinueLogging(void) override;
   virtual void PauseLogging(void) override;

   virtual void UpdateBitrate(const stw_types::sint32 os32_Bitrate) override;

   virtual void DistributeMessages(void) override;
   virtual void SendCanMessageQueued(const stw_can::T_STWCAN_Msg_TX & orc_Msg) override;

   virtual void SendCanMessage(const stw_opensyde_core::C_OSCComDriverBaseCanMessage & orc_MsgCfg) override;
   virtual void AddCyclicCanMessage(const stw_opensyde_core::C_OSCComDriverBaseCanMessage & orc_MsgCfg) override;
   virtual void RemoveCyclicCanMessage(const stw_opensyde_core::C_OSCComDriverBaseCanMessage & orc_MsgCfg) override;
   virtual void RemoveAllCyclicCanMessages(void) override;

private:
   // It is mutable because of the constness of the getter functions. Without the keyword mutable the getter functions
   // must be non const and that is not wanted.
   mutable stw_tgl::C_TGLCriticalSection mc_CriticalSectionMsg;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
