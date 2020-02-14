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
#include "C_OSCHalcConfigParameterStruct.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcConfigChannel
{
public:
   enum E_DataBlockConfig
   {
      eDC_READ_WRITE,
      eDC_READ_ONLY,
      eDC_NONE
   };

   C_OSCHalcConfigChannel(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   bool q_Active;                                            ///< Flag if channel should be configured
   stw_scl::C_SCLString c_Name;                              ///< User configured name for this channel
   stw_scl::C_SCLString c_Comment;                           ///< User configured description for this channel
   bool q_SafetyRelevant;                                    ///< Flag if channel is safety relevant
   bool q_DatablockSet;                                      ///< Flag if datablock index is valid
   stw_types::uint32 u32_DatablockIndex;                     ///< Datablock index reference
   bool q_UseCaseSet;                                        ///< Flag if use-case index is valid
   stw_types::uint32 u32_UseCaseIndex;                       ///< Current selected use-case index
   std::vector<E_DataBlockConfig> c_DatablockConfig;         ///< Datablock configuration
   std::vector<C_OSCHalcConfigParameterStruct> c_Parameters; ///< Parameter configuration (synced with C_OSCHalcDefBase)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
