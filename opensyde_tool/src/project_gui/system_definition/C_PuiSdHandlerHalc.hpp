//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC data interface
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDHANDLERHALC_H
#define C_PUISDHANDLERHALC_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSdHandlerNodeLogic.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdHandlerHalc :
   public C_PuiSdHandlerNodeLogic
{
public:
   C_PuiSdHandlerHalc(QObject * const opc_Parent = NULL);

   //Get
   const stw::opensyde_core::C_OscHalcConfig * GetHalcConfig(const uint32_t ou32_NodeIndex) const;
   const stw::opensyde_core::C_OscHalcConfigDomain * GetHalcDomainConfigDataConst(const uint32_t ou32_NodeIndex,
                                                                                  const uint32_t ou32_DomainIndex)
   const;
   const stw::opensyde_core::C_OscHalcConfigChannel * GetHalcDomainChannelConfigData(const uint32_t ou32_NodeIndex,
                                                                                     const uint32_t ou32_DomainIndex,
                                                                                     const uint32_t ou32_ChannelIndex,
                                                                                     const bool oq_UseChannelIndex)
   const;
   const stw::opensyde_core::C_OscHalcConfigParameterStruct * GetHalcDomainChannelParameterConfigStructData(
      const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
      const uint32_t ou32_ParameterIndex, const bool oq_UseChannelIndex)
   const;
   const stw::opensyde_core::C_OscHalcDefDomain * GetHalcDomainFileDataConst(const uint32_t ou32_NodeIndex,
                                                                             const uint32_t ou32_DomainIndex)
   const;
   const stw::opensyde_core::C_OscHalcDefChannelDef * GetHalcDomainFileChannelDataConst(const uint32_t ou32_NodeIndex,
                                                                                        const uint32_t ou32_DomainIndex,
                                                                                        const uint32_t ou32_Channel)
   const;
   const stw::opensyde_core::C_OscHalcDefChannelUseCase * GetHalcDomainFileUseCaseData(const uint32_t ou32_NodeIndex,
                                                                                       const uint32_t ou32_DomainIndex,
                                                                                       const uint32_t ou32_UseCaseIndex)
   const;
   const stw::opensyde_core::C_OscHalcDefChannelAvailability * GetHalcDomainFileUseCaseAvailabilityData(
      const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex, const uint32_t ou32_UseCaseIndex,
      const uint32_t ou32_AvailabilityIndex)
   const;
   const stw::opensyde_core::C_OscHalcDefStruct * GetHalcDomainFileVariableData(const uint32_t ou32_NodeIndex,
                                                                                const uint32_t ou32_DomainIndex,
                                                                                const stw::opensyde_core::C_OscHalcDefDomain::E_VariableSelector oe_Selector,
                                                                                const uint32_t ou32_Index) const;
   const stw::opensyde_core::C_OscNodeDataPool * GetHalcDatapool(const uint32_t ou32_NodeIndex,
                                                                 const bool oq_SafeDatapool) const;
   stw::opensyde_core::C_OscHalcDefDomain::E_Category GetDomainCategoryFromDpId(
      const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id, const uint32_t ou32_ArrayIndex);

   //Set
   int32_t ClearHalcConfig(const uint32_t ou32_NodeIndex);
   int32_t SetHalcConfig(const uint32_t ou32_NodeIndex, const stw::opensyde_core::C_OscHalcConfig & orc_Config);
   int32_t SetHalcDomainConfig(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                               const stw::opensyde_core::C_OscHalcConfigDomain & orc_Domain);
   int32_t SetHalcDomainChannelConfig(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                      const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                      const stw::opensyde_core::C_OscHalcConfigChannel & orc_Channel);
   int32_t SetHalcDomainChannelConfig(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                      const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                      const stw::scl::C_SclString & orc_Name, const stw::scl::C_SclString & orc_Comment,
                                      const bool oq_SafetyRelevant, const uint32_t ou32_UseCaseIndex);
   int32_t ResetHalcDomainChannelConfig(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                        const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex);
   int32_t ResetHalcDomainChannelUseCase(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                         const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex);
   int32_t SetHalcDomainChannelConfigName(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                          const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                          const stw::scl::C_SclString & orc_Name);
   int32_t SetHalcDomainChannelConfigComment(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                             const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                             const stw::scl::C_SclString & orc_Comment);
   int32_t SetHalcDomainChannelConfigSafety(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                            const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                            const bool oq_SafetyRelevant);
   int32_t SetHalcDomainChannelConfigUseCase(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                             const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                             const uint32_t ou32_UseCaseIndex);
   int32_t SetHalcDomainChannelParameterConfig(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                               const uint32_t ou32_ChannelIndex, const uint32_t ou32_ParameterIndex,
                                               const bool oq_UseChannelIndex, const stw::opensyde_core::C_OscHalcConfigParameterStruct &
                                               orc_Parameter);
   int32_t SetHalcDomainChannelParameterConfigElement(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                      const uint32_t ou32_ChannelIndex,
                                                      const uint32_t ou32_ParameterIndex,
                                                      const uint32_t ou32_ElementIndex, const bool oq_UseChannelIndex, const stw::opensyde_core::C_OscHalcConfigParameter &
                                                      orc_Parameter);
   int32_t SetHalcDomainChannelParameterConfigElementPlain(const uint32_t ou32_NodeIndex,
                                                           const uint32_t ou32_DomainIndex,
                                                           const uint32_t ou32_ChannelIndex,
                                                           const uint32_t ou32_ParameterIndex,
                                                           const uint32_t ou32_ElementIndex,
                                                           const bool oq_UseChannelIndex,
                                                           const stw::opensyde_core::C_OscHalcDefContent & orc_Value);
   int32_t SetHalcDomainChannelParameterConfigElementEnum(const uint32_t ou32_NodeIndex,
                                                          const uint32_t ou32_DomainIndex,
                                                          const uint32_t ou32_ChannelIndex,
                                                          const uint32_t ou32_ParameterIndex,
                                                          const uint32_t ou32_ElementIndex,
                                                          const bool oq_UseChannelIndex,
                                                          const stw::scl::C_SclString & orc_DisplayName);
   int32_t SetHalcDomainChannelParameterConfigElementBitmask(const uint32_t ou32_NodeIndex,
                                                             const uint32_t ou32_DomainIndex,
                                                             const uint32_t ou32_ChannelIndex,
                                                             const uint32_t ou32_ParameterIndex,
                                                             const uint32_t ou32_ElementIndex,
                                                             const bool oq_UseChannelIndex,
                                                             const stw::scl::C_SclString & orc_DisplayName,
                                                             const bool oq_Value);

   //Misc
   int32_t GetHalChannelOrDomainName(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                     const uint32_t ou32_Channel, QString & orc_Name) const;
   int32_t TranslateToHalcIndex(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                                const uint32_t ou32_ArrayIndex, uint32_t & oru32_DomainIndex,
                                bool & orq_UseChannelIndex, uint32_t & oru32_ChannelIndex,
                                stw::opensyde_core::C_OscHalcDefDomain::E_VariableSelector & ore_Selector,
                                uint32_t & oru32_ParameterIndex, bool & orq_UseElementIndex,
                                uint32_t & oru32_ParameterElementIndex, bool & orq_IsUseCaseIndex,
                                bool & orq_IsChanNumIndex, bool & orq_IsSafetyFlagIndex) const;
   int32_t IsHalcClear(const uint32_t ou32_NodeIndex, bool & orq_IsClear);
   int32_t CheckHalcDomainChannelLinked(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                        const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                        bool & orq_IsLinked, std::vector<QString> * const
                                        opc_LinkedChannelNames = NULL, std::vector<uint32_t> * const
                                        opc_LinkedChannelIndices = NULL,
                                        const uint32_t * const opu32_UseCaseIndex = NULL) const;
   int32_t SetHalcDomainChannelConfigOfLinkedChannels(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                      const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                      const uint32_t ou32_UseCaseIndexOld,
                                                      const uint32_t ou32_UseCaseIndexNew);
   int32_t CheckHalcDomainChannelError(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                       const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                       bool & orq_HasError) const;
   int32_t GetHalcRelevantIndicesForSelectedUseCase(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                    const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                    std::vector<uint32_t> * const opc_ParameterIndices,
                                                    std::vector<uint32_t> * const opc_InputIndices,
                                                    std::vector<uint32_t> * const opc_OutputIndices,
                                                    std::vector<uint32_t> * const opc_StatusIndices)
   const;
   int32_t HalcGenerateDatapools(const uint32_t ou32_NodeIndex);
   int32_t HalcRemoveDatapools(const uint32_t ou32_NodeIndex, const bool oq_SuppressSyncSignal = false);
   int32_t HalcResetDataBlocks(const uint32_t ou32_NodeIndex);
   bool CheckHalcChannelNameAvailable(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                      const stw::scl::C_SclString & orc_ChannelName,
                                      const uint32_t * const opu32_ChannelIndexToSkip = NULL) const;

private:
   static C_PuiSdNodeDataPool mh_GetUiDatapoolForOscDataPool(
      const stw::opensyde_core::C_OscNodeDataPool & orc_OscDatapool);

   std::map<uint32_t, uint32_t> mc_PreviousHashes; // key: node index, value: previous hash
   // value

   static int32_t mh_GetIndexInVector(const std::vector<stw::opensyde_core::C_OscHalcDefStruct> & orc_Structs,
                                      const uint32_t ou32_StartingIndex, const uint32_t ou32_TargetIndex,
                                      uint32_t & oru32_ParameterIndex, bool & orq_UseElementIndex,
                                      uint32_t & oru32_ParameterElementIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
