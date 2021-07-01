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

#include "CSCLString.h"
#include "C_OSCHalcDefBase.h"
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
   stw_types::sint32 CheckChannelLinked(const stw_types::uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                        bool & orq_IsLinked,
                                        std::vector<stw_scl::C_SCLString> * const opc_LinkedChannelNames,
                                        std::vector<stw_types::uint32> * const opc_LinkedChannelIndices,
                                        const stw_types::uint32 * const opu32_UseCaseIndex) const;
   stw_types::sint32 ResetChannelToDefault(const stw_types::uint32 ou32_ChannelIndex);
   stw_types::sint32 ResetChannelUseCase(const stw_types::uint32 ou32_ChannelIndex);
   void ResetDomainToDefault(void);
   stw_types::sint32 GetRelevantIndicesForSelectedUseCase(const stw_types::uint32 ou32_ChannelIndex,
                                                          const bool oq_UseChannelIndex,
                                                          std::vector<stw_types::uint32> * const opc_ParameterIndices,
                                                          std::vector<stw_types::uint32> * const opc_InputIndices,
                                                          std::vector<stw_types::uint32> * const opc_OutputIndices,
                                                          std::vector<stw_types::uint32> * const opc_StatusIndices)
   const;
   void HandleFileLoadPostProcessing(const C_OSCHalcDefBase::E_SafetyMode oe_SafetyMode);

   C_OSCHalcConfigChannel c_DomainConfig;                ///< Domain specific configuration
   std::vector<C_OSCHalcConfigChannel> c_ChannelConfigs; ///< Channels of domain of IO description (synced with
                                                         // C_OSCHalcDefBase)

private:
   static void mh_AddParameters(const std::vector<C_OSCHalcDefStruct> & orc_Parameters,
                                std::vector<C_OSCHalcConfigParameterStruct> & orc_ParamConfig);
   static C_OSCHalcConfigChannel mh_InitConfigFromName(const stw_scl::C_SCLString & orc_Name);
   C_OSCHalcConfigChannel m_InitChannelConfig(const stw_types::uint32 ou32_ChannelIndex) const;
   stw_types::uint32 m_InitChannelUseCase(const stw_types::uint32 ou32_ChannelIndex) const;
   void m_InitDomainConfig(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
