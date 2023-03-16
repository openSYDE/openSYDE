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

#include "C_SclString.hpp"
#include "C_OscHalcDefBase.hpp"
#include "C_OscHalcDefDomain.hpp"
#include "C_OscHalcConfigChannel.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcConfigDomain :
   public C_OscHalcDefDomain
{
public:
   C_OscHalcConfigDomain(void);
   C_OscHalcConfigDomain(const C_OscHalcDefDomain & orc_Base);

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   void CheckChannelNameUnique(const uint32_t ou32_ChannelIndex, bool * const opq_NameConflict) const;
   int32_t CheckChannelLinked(const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex, bool & orq_IsLinked,
                              std::vector<stw::scl::C_SclString> * const opc_LinkedChannelNames,
                              std::vector<uint32_t> * const opc_LinkedChannelIndices,
                              const uint32_t * const opu32_UseCaseIndex) const;
   int32_t ResetChannelToDefault(const uint32_t ou32_ChannelIndex);
   int32_t ResetChannelUseCase(const uint32_t ou32_ChannelIndex);
   void ResetDomainToDefault(void);
   int32_t GetRelevantIndicesForSelectedUseCase(const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                std::vector<uint32_t> * const opc_ParameterIndices,
                                                std::vector<uint32_t> * const opc_InputIndices,
                                                std::vector<uint32_t> * const opc_OutputIndices,
                                                std::vector<uint32_t> * const opc_StatusIndices)
   const;
   void HandleFileLoadPostProcessing(const C_OscHalcDefBase::E_SafetyMode oe_SafetyMode);

   C_OscHalcConfigChannel c_DomainConfig;                ///< Domain specific configuration
   std::vector<C_OscHalcConfigChannel> c_ChannelConfigs; ///< Channels of domain of IO description (synced with
                                                         // C_OscHalcDefBase)

private:
   static void mh_AddParameters(const std::vector<C_OscHalcDefStruct> & orc_Parameters,
                                std::vector<C_OscHalcConfigParameterStruct> & orc_ParamConfig);
   static C_OscHalcConfigChannel mh_InitConfigFromName(const stw::scl::C_SclString & orc_Name);
   C_OscHalcConfigChannel m_InitChannelConfig(const uint32_t ou32_ChannelIndex) const;
   uint32_t m_InitChannelUseCase(const uint32_t ou32_ChannelIndex) const;
   void m_InitDomainConfig(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
