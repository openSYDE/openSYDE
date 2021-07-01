//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain channel config data for HALC configuration
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGCHANNEL_H
#define C_OSCHALCCONFIGCHANNEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCHalcDefBase.h"
#include "C_OSCHalcConfigParameterStruct.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcConfigChannel
{
public:
   C_OSCHalcConfigChannel(void);
   virtual ~C_OSCHalcConfigChannel();

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   void CheckConfigValid(bool * const opq_NameInvalid) const;
   void HandleFileLoadPostProcessing(const C_OSCHalcDefBase::E_SafetyMode oe_SafetyMode);

   stw_scl::C_SCLString c_Name;                              ///< User configured name for this channel
   stw_scl::C_SCLString c_Comment;                           ///< User configured description for this channel
   bool q_SafetyRelevant;                                    ///< Flag if channel is safety relevant
   stw_types::uint32 u32_UseCaseIndex;                       ///< Current selected use-case index
   std::vector<C_OSCHalcConfigParameterStruct> c_Parameters; ///< Parameter configuration (synced with C_OSCHalcDefBase)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
