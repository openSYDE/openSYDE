//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group config data for HALC definition
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIG_HPP
#define C_OSCHALCCONFIG_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscHalcDefBase.hpp"
#include "C_OscHalcConfigDomain.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcConfig :
   public C_OscHalcDefBase
{
public:
   C_OscHalcConfig(void);

   //Domains
   virtual uint32_t GetDomainSize(void) const;
   const C_OscHalcConfigDomain * GetDomainConfigDataConst(const uint32_t ou32_Index) const;
   virtual const C_OscHalcDefDomain * GetDomainDefDataConst(const uint32_t ou32_Index) const;
   virtual void Clear(void);
   virtual bool IsClear(void) const;
   virtual void AddDomain(const C_OscHalcDefDomain & orc_Domain);
   virtual void HandleFileLoadPostProcessing(void);
   int32_t SetDomainConfig(const uint32_t ou32_Index, const C_OscHalcConfigDomain & orc_Domain);
   int32_t SetDomainChannelConfig(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                  const bool oq_UseChannelIndex, const stw::scl::C_SclString & orc_Name,
                                  const stw::scl::C_SclString & orc_Comment, const bool oq_SafetyRelevant,
                                  const uint32_t ou32_UseCaseIndex);
   int32_t ResetDomainChannelConfig(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                    const bool oq_UseChannelIndex);
   int32_t ResetDomainChannelUseCase(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                     const bool oq_UseChannelIndex);
   int32_t SetDomainChannelConfigName(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                      const bool oq_UseChannelIndex, const stw::scl::C_SclString & orc_Name);
   int32_t SetDomainChannelConfigComment(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                         const bool oq_UseChannelIndex, const stw::scl::C_SclString & orc_Comment);
   int32_t SetDomainChannelConfigSafety(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                        const bool oq_UseChannelIndex, const bool oq_SafetyRelevant);
   int32_t SetDomainChannelConfigUseCase(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                         const bool oq_UseChannelIndex, const uint32_t ou32_UseCaseIndex);
   int32_t SetDomainChannelParameterConfig(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                           const uint32_t ou32_ParameterIndex, const bool oq_UseChannelIndex,
                                           const stw::opensyde_core::C_OscHalcConfigParameterStruct & orc_Parameter);
   int32_t SetDomainChannelParameterConfigElement(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                                  const uint32_t ou32_ParameterIndex, const uint32_t ou32_ElementIndex,
                                                  const bool oq_UseChannelIndex, const stw::opensyde_core::C_OscHalcConfigParameter &
                                                  orc_Parameter);
   int32_t SetDomainChannelParameterConfigElementPlain(const uint32_t ou32_DomainIndex,
                                                       const uint32_t ou32_ChannelIndex,
                                                       const uint32_t ou32_ParameterIndex,
                                                       const uint32_t ou32_ElementIndex, const bool oq_UseChannelIndex,
                                                       const C_OscHalcDefContent & orc_Value);
   int32_t SetDomainChannelParameterConfigElementEnum(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                                      const uint32_t ou32_ParameterIndex,
                                                      const uint32_t ou32_ElementIndex, const bool oq_UseChannelIndex,
                                                      const stw::scl::C_SclString & orc_DisplayName);
   int32_t SetDomainChannelParameterConfigElementBitmask(const uint32_t ou32_DomainIndex,
                                                         const uint32_t ou32_ChannelIndex,
                                                         const uint32_t ou32_ParameterIndex,
                                                         const uint32_t ou32_ElementIndex,
                                                         const bool oq_UseChannelIndex,
                                                         const stw::scl::C_SclString & orc_DisplayName,
                                                         const bool oq_Value);
   int32_t GetRelevantIndicesForSelectedUseCase(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                                const bool oq_UseChannelIndex,
                                                std::vector<uint32_t> * const opc_ParameterIndices,
                                                std::vector<uint32_t> * const opc_InputIndices,
                                                std::vector<uint32_t> * const opc_OutputIndices,
                                                std::vector<uint32_t> * const opc_StatusIndices)
   const;
   void CheckConfigValid(bool * const opq_DomainsInvalid, std::vector<uint32_t> * const opc_InvalidDomainIndices) const;
   void CheckDomainConfigValid(const uint32_t ou32_DomainIndex, bool * const opq_DomainInvalid,
                               bool * const opq_ChannelsInvalid,
                               std::vector<uint32_t> * const opc_InvalidChannelIndices) const;
   virtual void CalcHash(uint32_t & oru32_HashValue) const;
   void HandleNameMaxCharLimit(const uint32_t ou32_NameMaxCharLimit,
                               std::list<C_OscSystemNameMaxCharLimitChangeReportItem> * const opc_ChangedItems);

private:
   std::vector<C_OscHalcConfigDomain> mc_Domains; ///< Domains of HALC configuration (synced with C_OscHalcDefBase)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
