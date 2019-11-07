//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain config data for IO config
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOCONFIGDATADOMAIN_H
#define C_OSCIOCONFIGDATADOMAIN_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "C_OSCIOFileDataDomain.h"
#include "C_OSCIOConfigDataChannel.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOConfigDataDomain :
   public C_OSCIOFileDataDomain
{
public:
   C_OSCIOConfigDataDomain(void);
   C_OSCIOConfigDataDomain(const C_OSCIOFileDataDomain & orc_Base);

   std::vector<C_OSCIOConfigDataChannel> c_ChannelConfigs; ///< Channels of domain of IO description (synced with
                                                           // C_OSCIOFileDataBase)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
