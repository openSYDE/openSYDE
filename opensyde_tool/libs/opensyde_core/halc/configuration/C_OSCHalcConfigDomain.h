//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain config data for HALC configuration
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGDOMAIN_H
#define C_OSCHALCCONFIGDOMAIN_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "C_OSCHalcDefDomain.h"
#include "C_OSCHalcConfigChannel.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcConfigDomain :
   public C_OSCHalcDefDomain
{
public:
   C_OSCHalcConfigDomain(void);
   C_OSCHalcConfigDomain(const C_OSCHalcDefDomain & orc_Base);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   void CheckChannelNameUnique(const stw_types::uint32 ou32_ChannelIndex, bool * const opq_NameConflict) const;

   C_OSCHalcConfigChannel c_DomainConfig;                ///< Domain specific configuration
   std::vector<C_OSCHalcConfigChannel> c_ChannelConfigs; ///< Channels of domain of IO description (synced with
                                                         // C_OSCHalcDefBase)

private:
   static void mh_AddParameters(const std::vector<C_OSCHalcDefStruct> & orc_Parameters,
                                std::vector<C_OSCHalcConfigParameterStruct> & orc_ParamConfig);
   static C_OSCHalcConfigChannel mh_InitConfigFromName(const stw_scl::C_SCLString & orc_Name);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
