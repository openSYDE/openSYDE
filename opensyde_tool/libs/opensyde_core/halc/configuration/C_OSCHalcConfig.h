//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group config data for HALC definition
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIG_H
#define C_OSCHALCCONFIG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCHalcDefBase.h"
#include "C_OSCHalcConfigDomain.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcConfig :
   public C_OSCHalcDefBase
{
public:
   C_OSCHalcConfig(void);

   virtual stw_types::uint32 GetDomainSize(void) const;
   const C_OSCHalcConfigDomain * GetDomainConfigDataConst(const stw_types::uint32 ou32_Index) const;
   virtual const C_OSCHalcDefDomain * GetDomainDefDataConst(const stw_types::uint32 ou32_Index) const;
   virtual void Clear(void);
   virtual void AddDomain(const C_OSCHalcDefDomain & orc_Domain);
   stw_types::sint32 SetDomainConfig(const stw_types::uint32 ou32_Index, const C_OSCHalcConfigDomain & orc_Domain);
   stw_types::sint32 SetDomainChannelConfig(const stw_types::uint32 ou32_DomainIndex,
                                            const stw_types::uint32 ou32_ChannelIndex, const bool oq_Active,
                                            const stw_scl::C_SCLString & orc_Name,
                                            const stw_scl::C_SCLString & orc_Comment, const bool oq_SafetyRelevant,
                                            const bool oq_DatablockSet, const stw_types::uint32 ou32_DatablockIndex,
                                            const bool oq_UseCaseSet, const stw_types::uint32 ou32_UseCaseIndex,
                                            const std::vector<C_OSCHalcConfigChannel::E_DataBlockConfig> & orc_DatablockConfig);
   stw_types::sint32 SetDomainChannelConfigActive(const stw_types::uint32 ou32_DomainIndex,
                                                  const stw_types::uint32 ou32_ChannelIndex, const bool
                                                  oq_Active);
   stw_types::sint32 SetDomainChannelConfigName(const stw_types::uint32 ou32_DomainIndex,
                                                const stw_types::uint32 ou32_ChannelIndex,
                                                const stw_scl::C_SCLString & orc_Name);
   stw_types::sint32 SetDomainChannelConfigComment(const stw_types::uint32 ou32_DomainIndex,
                                                   const stw_types::uint32 ou32_ChannelIndex,
                                                   const stw_scl::C_SCLString & orc_Comment);
   stw_types::sint32 SetDomainChannelConfigSafety(const stw_types::uint32 ou32_DomainIndex,
                                                  const stw_types::uint32 ou32_ChannelIndex,
                                                  const bool oq_SafetyRelevant);
   stw_types::sint32 SetDomainChannelConfigDatablock(const stw_types::uint32 ou32_DomainIndex,
                                                     const stw_types::uint32 ou32_ChannelIndex,
                                                     const bool oq_DatablockSet,
                                                     const stw_types::uint32 ou32_DatablockIndex);
   stw_types::sint32 SetDomainChannelConfigUseCase(const stw_types::uint32 ou32_DomainIndex,
                                                   const stw_types::uint32 ou32_ChannelIndex, const bool oq_UseCaseSet,
                                                   const stw_types::uint32 ou32_UseCaseIndex);
   stw_types::sint32 SetDomainChannelConfigDatablockConfig(const stw_types::uint32 ou32_DomainIndex,
                                                           const stw_types::uint32 ou32_ChannelIndex, const
                                                           std::vector<stw_opensyde_core::C_OSCHalcConfigChannel::E_DataBlockConfig> & orc_DatablockConfig);
   stw_types::sint32 SetDomainChannelParameterConfig(const stw_types::uint32 ou32_DomainIndex,
                                                     const stw_types::uint32 ou32_ChannelIndex,
                                                     const stw_types::uint32 ou32_ParameterIndex,
                                                     const stw_opensyde_core::C_OSCHalcConfigParameterStruct & orc_Parameter);
   stw_types::sint32 SetDomainChannelParameterConfigElement(const stw_types::uint32 ou32_DomainIndex,
                                                            const stw_types::uint32 ou32_ChannelIndex,
                                                            const stw_types::uint32 ou32_ParameterIndex,
                                                            const stw_types::uint32 ou32_ElementIndex, const stw_opensyde_core::C_OSCHalcConfigParameter &
                                                            orc_Parameter);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

private:
   std::vector<C_OSCHalcConfigDomain> mc_Domains; ///< Domains of HALC configuration (synced with C_OSCHalcDefBase)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
