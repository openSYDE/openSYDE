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

   //Domains
   virtual stw_types::uint32 GetDomainSize(void) const;
   const C_OSCHalcConfigDomain * GetDomainConfigDataConst(const stw_types::uint32 ou32_Index) const;
   virtual const C_OSCHalcDefDomain * GetDomainDefDataConst(const stw_types::uint32 ou32_Index) const;
   virtual void Clear(void);
   virtual bool IsClear(void) const;
   virtual void AddDomain(const C_OSCHalcDefDomain & orc_Domain);
   stw_types::sint32 SetDomainConfig(const stw_types::uint32 ou32_Index, const C_OSCHalcConfigDomain & orc_Domain);
   stw_types::sint32 SetDomainChannelConfig(const stw_types::uint32 ou32_DomainIndex,
                                            const stw_types::uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                            const stw_scl::C_SCLString & orc_Name,
                                            const stw_scl::C_SCLString & orc_Comment, const bool oq_SafetyRelevant,
                                            const stw_types::uint32 ou32_UseCaseIndex);
   stw_types::sint32 ResetDomainChannelConfig(const stw_types::uint32 ou32_DomainIndex,
                                              const stw_types::uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex);
   stw_types::sint32 ResetDomainChannelUseCase(const stw_types::uint32 ou32_DomainIndex,
                                               const stw_types::uint32 ou32_ChannelIndex,
                                               const bool oq_UseChannelIndex);
   stw_types::sint32 SetDomainChannelConfigName(const stw_types::uint32 ou32_DomainIndex,
                                                const stw_types::uint32 ou32_ChannelIndex,
                                                const bool oq_UseChannelIndex, const stw_scl::C_SCLString & orc_Name);
   stw_types::sint32 SetDomainChannelConfigComment(const stw_types::uint32 ou32_DomainIndex,
                                                   const stw_types::uint32 ou32_ChannelIndex,
                                                   const bool oq_UseChannelIndex,
                                                   const stw_scl::C_SCLString & orc_Comment);
   stw_types::sint32 SetDomainChannelConfigSafety(const stw_types::uint32 ou32_DomainIndex,
                                                  const stw_types::uint32 ou32_ChannelIndex,
                                                  const bool oq_UseChannelIndex, const bool oq_SafetyRelevant);
   stw_types::sint32 SetDomainChannelConfigUseCase(const stw_types::uint32 ou32_DomainIndex,
                                                   const stw_types::uint32 ou32_ChannelIndex,
                                                   const bool oq_UseChannelIndex,
                                                   const stw_types::uint32 ou32_UseCaseIndex);
   stw_types::sint32 SetDomainChannelParameterConfig(const stw_types::uint32 ou32_DomainIndex,
                                                     const stw_types::uint32 ou32_ChannelIndex,
                                                     const stw_types::uint32 ou32_ParameterIndex,
                                                     const bool oq_UseChannelIndex,
                                                     const stw_opensyde_core::C_OSCHalcConfigParameterStruct & orc_Parameter);
   stw_types::sint32 SetDomainChannelParameterConfigElement(const stw_types::uint32 ou32_DomainIndex,
                                                            const stw_types::uint32 ou32_ChannelIndex,
                                                            const stw_types::uint32 ou32_ParameterIndex,
                                                            const stw_types::uint32 ou32_ElementIndex,
                                                            const bool oq_UseChannelIndex, const stw_opensyde_core::C_OSCHalcConfigParameter &
                                                            orc_Parameter);
   stw_types::sint32 SetDomainChannelParameterConfigElementPlain(const stw_types::uint32 ou32_DomainIndex,
                                                                 const stw_types::uint32 ou32_ChannelIndex,
                                                                 const stw_types::uint32 ou32_ParameterIndex,
                                                                 const stw_types::uint32 ou32_ElementIndex,
                                                                 const bool oq_UseChannelIndex,
                                                                 const C_OSCHalcDefContent & orc_Value);
   stw_types::sint32 SetDomainChannelParameterConfigElementEnum(const stw_types::uint32 ou32_DomainIndex,
                                                                const stw_types::uint32 ou32_ChannelIndex,
                                                                const stw_types::uint32 ou32_ParameterIndex,
                                                                const stw_types::uint32 ou32_ElementIndex,
                                                                const bool oq_UseChannelIndex,
                                                                const stw_scl::C_SCLString & orc_DisplayName);
   stw_types::sint32 SetDomainChannelParameterConfigElementBitmask(const stw_types::uint32 ou32_DomainIndex,
                                                                   const stw_types::uint32 ou32_ChannelIndex,
                                                                   const stw_types::uint32 ou32_ParameterIndex,
                                                                   const stw_types::uint32 ou32_ElementIndex,
                                                                   const bool oq_UseChannelIndex,
                                                                   const stw_scl::C_SCLString & orc_DisplayName,
                                                                   const bool oq_Value);
   stw_types::sint32 GetRelevantIndicesForSelectedUseCase(const stw_types::uint32 ou32_DomainIndex,
                                                          const stw_types::uint32 ou32_ChannelIndex,
                                                          const bool oq_UseChannelIndex,
                                                          std::vector<stw_types::uint32> * const opc_ParameterIndices,
                                                          std::vector<stw_types::uint32> * const opc_InputIndices,
                                                          std::vector<stw_types::uint32> * const opc_OutputIndices,
                                                          std::vector<stw_types::uint32> * const opc_StatusIndices)
   const;
   void CheckConfigValid(bool * const opq_DomainsInvalid,
                         std::vector<stw_types::uint32> * const opc_InvalidDomainIndices) const;
   void CheckDomainConfigValid(const stw_types::uint32 ou32_DomainIndex, bool * const opq_DomainInvalid,
                               bool * const opq_ChannelsInvalid,
                               std::vector<stw_types::uint32> * const opc_InvalidChannelIndices) const;
   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

private:
   std::vector<C_OSCHalcConfigDomain> mc_Domains; ///< Domains of HALC configuration (synced with C_OSCHalcDefBase)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
