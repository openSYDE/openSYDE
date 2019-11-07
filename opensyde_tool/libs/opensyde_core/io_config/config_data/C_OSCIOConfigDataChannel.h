//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain channel config data for IO config
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOCONFIGDATACHANNEL_H
#define C_OSCIOCONFIGDATACHANNEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCIOConfigDataParameterStruct.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOConfigDataChannel
{
public:
   enum E_DataBlockConfig
   {
      eDC_READ_WRITE,
      eDC_READ_ONLY,
      eDC_NONE
   };

   C_OSCIOConfigDataChannel(void);

   bool q_Active;                                              ///< Flag if channel should be configured
   stw_scl::C_SCLString c_Name;                                ///< User configured name for this channel
   stw_scl::C_SCLString c_Comment;                             ///< User configured description for this channel
   bool q_SafetyRelevant;                                      ///< Flag if channel is safety relevant
   bool q_DatablockSet;                                        ///< Flag if datablock index is valid
   stw_types::uint32 u32_DatablockIndex;                       ///< Datablock index reference
   std::vector<E_DataBlockConfig> c_DatablockConfig;           ///< Datablock configuration
   std::vector<C_OSCIOConfigDataParameterStruct> c_Parameters; ///< Parameter configuration (synced with
                                                               // C_OSCIOFileDataBase)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
