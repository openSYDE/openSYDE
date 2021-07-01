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
#include "C_PuiSdHandlerNodeLogic.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdHandlerHALC :
   public C_PuiSdHandlerNodeLogic
{
public:
   C_PuiSdHandlerHALC(QObject * const opc_Parent = NULL);

   //Get
   const stw_opensyde_core::C_OSCHalcConfig * GetHALCConfig(const stw_types::uint32 ou32_NodeIndex) const;
   const stw_opensyde_core::C_OSCHalcConfigDomain * GetHALCDomainConfigDataConst(const stw_types::uint32 ou32_NodeIndex,
                                                                                 const stw_types::uint32 ou32_DomainIndex)
   const;
   const stw_opensyde_core::C_OSCHalcConfigChannel * GetHALCDomainChannelConfigData(
      const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DomainIndex,
      const stw_types::uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex)
   const;
   const stw_opensyde_core::C_OSCHalcConfigParameterStruct * GetHALCDomainChannelParameterConfigStructData(
      const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DomainIndex,
      const stw_types::uint32 ou32_ChannelIndex, const stw_types::uint32 ou32_ParameterIndex,
      const bool oq_UseChannelIndex)
   const;
   const stw_opensyde_core::C_OSCHalcDefDomain * GetHALCDomainFileDataConst(const stw_types::uint32 ou32_NodeIndex,
                                                                            const stw_types::uint32 ou32_DomainIndex)
   const;
   const stw_opensyde_core::C_OSCHalcDefChannelDef * GetHALCDomainFileChannelDataConst(
      const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DomainIndex,
      const stw_types::uint32 ou32_Channel)
   const;
   const stw_opensyde_core::C_OSCHalcDefChannelUseCase * GetHALCDomainFileUseCaseData(
      const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DomainIndex,
      const stw_types::uint32 ou32_UseCaseIndex)
   const;
   const stw_opensyde_core::C_OSCHalcDefChannelAvailability * GetHALCDomainFileUseCaseAvailabilityData(
      const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DomainIndex,
      const stw_types::uint32 ou32_UseCaseIndex, const stw_types::uint32 ou32_AvailabilityIndex)
   const;
   const stw_opensyde_core::C_OSCHalcDefStruct * GetHALCDomainFileVariableData(const stw_types::uint32 ou32_NodeIndex,
                                                                               const stw_types::uint32 ou32_DomainIndex,
                                                                               const stw_opensyde_core::C_OSCHalcDefDomain::E_VariableSelector oe_Selector,
                                                                               const stw_types::uint32 ou32_Index) const;
   const stw_opensyde_core::C_OSCNodeDataPool * GetHALCDatapool(const stw_types::uint32 ou32_NodeIndex,
                                                                const bool oq_SafeDatapool) const;
   stw_opensyde_core::C_OSCHalcDefDomain::E_Category GetDomainCategoryFromDpId(
      const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id, const stw_types::uint32 ou32_ArrayIndex);

   //Set
   stw_types::sint32 ClearHALCConfig(const stw_types::uint32 ou32_NodeIndex);
   stw_types::sint32 SetHALCConfig(const stw_types::uint32 ou32_NodeIndex,
                                   const stw_opensyde_core::C_OSCHalcConfig & orc_Config);
   stw_types::sint32 SetHALCDomainConfig(const stw_types::uint32 ou32_NodeIndex,
                                         const stw_types::uint32 ou32_DomainIndex,
                                         const stw_opensyde_core::C_OSCHalcConfigDomain & orc_Domain);
   stw_types::sint32 SetHALCDomainChannelConfig(const stw_types::uint32 ou32_NodeIndex,
                                                const stw_types::uint32 ou32_DomainIndex,
                                                const stw_types::uint32 ou32_ChannelIndex,
                                                const bool oq_UseChannelIndex,
                                                const stw_opensyde_core::C_OSCHalcConfigChannel & orc_Channel);
   stw_types::sint32 SetHALCDomainChannelConfig(const stw_types::uint32 ou32_NodeIndex,
                                                const stw_types::uint32 ou32_DomainIndex,
                                                const stw_types::uint32 ou32_ChannelIndex,
                                                const bool oq_UseChannelIndex, const stw_scl::C_SCLString & orc_Name,
                                                const stw_scl::C_SCLString & orc_Comment, const bool oq_SafetyRelevant,
                                                const stw_types::uint32 ou32_UseCaseIndex);
   stw_types::sint32 ResetHALCDomainChannelConfig(const stw_types::uint32 ou32_NodeIndex,
                                                  const stw_types::uint32 ou32_DomainIndex,
                                                  const stw_types::uint32 ou32_ChannelIndex,
                                                  const bool oq_UseChannelIndex);
   stw_types::sint32 ResetHALCDomainChannelUseCase(const stw_types::uint32 ou32_NodeIndex,
                                                   const stw_types::uint32 ou32_DomainIndex,
                                                   const stw_types::uint32 ou32_ChannelIndex,
                                                   const bool oq_UseChannelIndex);
   stw_types::sint32 SetHALCDomainChannelConfigName(const stw_types::uint32 ou32_NodeIndex,
                                                    const stw_types::uint32 ou32_DomainIndex,
                                                    const stw_types::uint32 ou32_ChannelIndex,
                                                    const bool oq_UseChannelIndex,
                                                    const stw_scl::C_SCLString & orc_Name);
   stw_types::sint32 SetHALCDomainChannelConfigComment(const stw_types::uint32 ou32_NodeIndex,
                                                       const stw_types::uint32 ou32_DomainIndex,
                                                       const stw_types::uint32 ou32_ChannelIndex,
                                                       const bool oq_UseChannelIndex,
                                                       const stw_scl::C_SCLString & orc_Comment);
   stw_types::sint32 SetHALCDomainChannelConfigSafety(const stw_types::uint32 ou32_NodeIndex,
                                                      const stw_types::uint32 ou32_DomainIndex,
                                                      const stw_types::uint32 ou32_ChannelIndex,
                                                      const bool oq_UseChannelIndex, const bool oq_SafetyRelevant);
   stw_types::sint32 SetHALCDomainChannelConfigUseCase(const stw_types::uint32 ou32_NodeIndex,
                                                       const stw_types::uint32 ou32_DomainIndex,
                                                       const stw_types::uint32 ou32_ChannelIndex,
                                                       const bool oq_UseChannelIndex,
                                                       const stw_types::uint32 ou32_UseCaseIndex);
   stw_types::sint32 SetHALCDomainChannelParameterConfig(const stw_types::uint32 ou32_NodeIndex,
                                                         const stw_types::uint32 ou32_DomainIndex,
                                                         const stw_types::uint32 ou32_ChannelIndex,
                                                         const stw_types::uint32 ou32_ParameterIndex,
                                                         const bool oq_UseChannelIndex, const stw_opensyde_core::C_OSCHalcConfigParameterStruct &
                                                         orc_Parameter);
   stw_types::sint32 SetHALCDomainChannelParameterConfigElement(const stw_types::uint32 ou32_NodeIndex,
                                                                const stw_types::uint32 ou32_DomainIndex,
                                                                const stw_types::uint32 ou32_ChannelIndex,
                                                                const stw_types::uint32 ou32_ParameterIndex,
                                                                const stw_types::uint32 ou32_ElementIndex,
                                                                const bool oq_UseChannelIndex, const stw_opensyde_core::C_OSCHalcConfigParameter &
                                                                orc_Parameter);
   stw_types::sint32 SetHALCDomainChannelParameterConfigElementPlain(const stw_types::uint32 ou32_NodeIndex,
                                                                     const stw_types::uint32 ou32_DomainIndex,
                                                                     const stw_types::uint32 ou32_ChannelIndex,
                                                                     const stw_types::uint32 ou32_ParameterIndex,
                                                                     const stw_types::uint32 ou32_ElementIndex,
                                                                     const bool oq_UseChannelIndex,
                                                                     const stw_opensyde_core::C_OSCHalcDefContent & orc_Value);
   stw_types::sint32 SetHALCDomainChannelParameterConfigElementEnum(const stw_types::uint32 ou32_NodeIndex,
                                                                    const stw_types::uint32 ou32_DomainIndex,
                                                                    const stw_types::uint32 ou32_ChannelIndex,
                                                                    const stw_types::uint32 ou32_ParameterIndex,
                                                                    const stw_types::uint32 ou32_ElementIndex,
                                                                    const bool oq_UseChannelIndex,
                                                                    const stw_scl::C_SCLString & orc_DisplayName);
   stw_types::sint32 SetHALCDomainChannelParameterConfigElementBitmask(const stw_types::uint32 ou32_NodeIndex,
                                                                       const stw_types::uint32 ou32_DomainIndex,
                                                                       const stw_types::uint32 ou32_ChannelIndex,
                                                                       const stw_types::uint32 ou32_ParameterIndex,
                                                                       const stw_types::uint32 ou32_ElementIndex,
                                                                       const bool oq_UseChannelIndex,
                                                                       const stw_scl::C_SCLString & orc_DisplayName,
                                                                       const bool oq_Value);

   //Misc
   stw_types::sint32 GetHalChannelOrDomainName(const stw_types::uint32 ou32_NodeIndex,
                                               const stw_types::uint32 ou32_DomainIndex,
                                               const stw_types::uint32 ou32_Channel, QString & orc_Name) const;
   stw_types::sint32 TranslateToHALCIndex(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                                          const stw_types::uint32 ou32_ArrayIndex,
                                          stw_types::uint32 & oru32_DomainIndex, bool & orq_UseChannelIndex,
                                          stw_types::uint32 & oru32_ChannelIndex,
                                          stw_opensyde_core::C_OSCHalcDefDomain::E_VariableSelector & ore_Selector,
                                          stw_types::uint32 & oru32_ParameterIndex, bool & orq_UseElementIndex,
                                          stw_types::uint32 & oru32_ParameterElementIndex, bool & orq_IsUseCaseIndex,
                                          bool & orq_IsChanNumIndex, bool & orq_IsSafetyFlagIndex) const;
   stw_types::sint32 IsHalcClear(const stw_types::uint32 ou32_NodeIndex, bool & orq_IsClear);
   stw_types::sint32 CheckHALCDomainChannelLinked(const stw_types::uint32 ou32_NodeIndex,
                                                  const stw_types::uint32 ou32_DomainIndex,
                                                  const stw_types::uint32 ou32_ChannelIndex,
                                                  const bool oq_UseChannelIndex, bool & orq_IsLinked, std::vector<QString> * const
                                                  opc_LinkedChannelNames = NULL, std::vector<stw_types::uint32> * const
                                                  opc_LinkedChannelIndices = NULL,
                                                  const stw_types::uint32 * const opu32_UseCaseIndex = NULL) const;
   stw_types::sint32 SetHALCDomainChannelConfigOfLinkedChannels(const stw_types::uint32 ou32_NodeIndex,
                                                                const stw_types::uint32 ou32_DomainIndex,
                                                                const stw_types::uint32 ou32_ChannelIndex,
                                                                const bool oq_UseChannelIndex,
                                                                const stw_types::uint32 ou32_UseCaseIndexOld,
                                                                const stw_types::uint32 ou32_UseCaseIndexNew);
   stw_types::sint32 CheckHALCDomainChannelError(const stw_types::uint32 ou32_NodeIndex,
                                                 const stw_types::uint32 ou32_DomainIndex,
                                                 const stw_types::uint32 ou32_ChannelIndex,
                                                 const bool oq_UseChannelIndex, bool & orq_HasError) const;
   stw_types::sint32 GetHALCRelevantIndicesForSelectedUseCase(const stw_types::uint32 ou32_NodeIndex,
                                                              const stw_types::uint32 ou32_DomainIndex,
                                                              const stw_types::uint32 ou32_ChannelIndex,
                                                              const bool oq_UseChannelIndex,
                                                              std::vector<stw_types::uint32> * const opc_ParameterIndices, std::vector<stw_types::uint32> * const opc_InputIndices, std::vector<stw_types::uint32> * const opc_OutputIndices, std::vector<stw_types::uint32> * const opc_StatusIndices)
   const;
   stw_types::sint32 HALCGenerateDatapools(const stw_types::uint32 ou32_NodeIndex);
   stw_types::sint32 HALCRemoveDatapools(const stw_types::uint32 ou32_NodeIndex,
                                         const bool oq_SuppressSyncSignal = false);
   stw_types::sint32 HALCResetDataBlocks(const stw_types::uint32 ou32_NodeIndex);
   bool CheckHALCChannelNameAvailable(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DomainIndex,
                                      const stw_scl::C_SCLString & orc_ChannelName,
                                      const stw_types::uint32 * const opu32_ChannelIndexToSkip = NULL) const;

private:
   static C_PuiSdNodeDataPool mh_GetUiDatapoolForOSCDataPool(
      const stw_opensyde_core::C_OSCNodeDataPool & orc_OSCDatapool);

   std::map<stw_types::uint32, stw_types::uint32> mc_PreviousHashes; // key: node index, value: previous hash
   // value

   static stw_types::sint32 mh_GetIndexInVector(const std::vector<stw_opensyde_core::C_OSCHalcDefStruct> & orc_Structs,
                                                const stw_types::uint32 ou32_StartingIndex,
                                                const stw_types::uint32 ou32_TargetIndex,
                                                stw_types::uint32 & oru32_ParameterIndex, bool & orq_UseElementIndex,
                                                stw_types::uint32 & oru32_ParameterElementIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
