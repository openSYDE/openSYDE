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
      const stw_types::uint32 ou32_ChannelIndex)
   const;
   const stw_opensyde_core::C_OSCHalcConfigParameterStruct * GetHALCDomainChannelParameterConfigData(
      const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DomainIndex,
      const stw_types::uint32 ou32_ChannelIndex, const stw_types::uint32 ou32_ParameterIndex)
   const;
   const stw_opensyde_core::C_OSCHalcDefDomain * GetHALCDomainFileDataConst(const stw_types::uint32 ou32_NodeIndex,
                                                                            const stw_types::uint32 ou32_DomainIndex)
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

   //Set
   stw_types::sint32 SetHALCConfig(const stw_types::uint32 ou32_NodeIndex,
                                   const stw_opensyde_core::C_OSCHalcConfig & orc_Config);
   stw_types::sint32 SetHALCDomainConfig(const stw_types::uint32 ou32_NodeIndex,
                                         const stw_types::uint32 ou32_DomainIndex,
                                         const stw_opensyde_core::C_OSCHalcConfigDomain & orc_Domain);
   stw_types::sint32 SetHALCDomainChannelConfig(const stw_types::uint32 ou32_NodeIndex,
                                                const stw_types::uint32 ou32_DomainIndex,
                                                const stw_types::uint32 ou32_ChannelIndex, const bool oq_Active,
                                                const stw_scl::C_SCLString & orc_Name,
                                                const stw_scl::C_SCLString & orc_Comment, const bool oq_SafetyRelevant,
                                                const bool oq_DatablockSet, const stw_types::uint32 ou32_DatablockIndex,
                                                const bool oq_UseCaseSet, const stw_types::uint32 ou32_UseCaseIndex,
                                                const std::vector<stw_opensyde_core::C_OSCHalcConfigChannel::E_DataBlockConfig> & orc_DatablockConfig);
   stw_types::sint32 SetHALCDomainChannelConfigActive(const stw_types::uint32 ou32_NodeIndex,
                                                      const stw_types::uint32 ou32_DomainIndex,
                                                      const stw_types::uint32 ou32_ChannelIndex, const bool
                                                      oq_Active);
   stw_types::sint32 SetHALCDomainChannelConfigName(const stw_types::uint32 ou32_NodeIndex,
                                                    const stw_types::uint32 ou32_DomainIndex,
                                                    const stw_types::uint32 ou32_ChannelIndex,
                                                    const stw_scl::C_SCLString & orc_Name);
   stw_types::sint32 SetHALCDomainChannelConfigComment(const stw_types::uint32 ou32_NodeIndex,
                                                       const stw_types::uint32 ou32_DomainIndex,
                                                       const stw_types::uint32 ou32_ChannelIndex,
                                                       const stw_scl::C_SCLString & orc_Comment);
   stw_types::sint32 SetHALCDomainChannelConfigSafety(const stw_types::uint32 ou32_NodeIndex,
                                                      const stw_types::uint32 ou32_DomainIndex,
                                                      const stw_types::uint32 ou32_ChannelIndex,
                                                      const bool oq_SafetyRelevant);
   stw_types::sint32 SetHALCDomainChannelConfigDatablock(const stw_types::uint32 ou32_NodeIndex,
                                                         const stw_types::uint32 ou32_DomainIndex,
                                                         const stw_types::uint32 ou32_ChannelIndex,
                                                         const bool oq_DatablockSet,
                                                         const stw_types::uint32 ou32_DatablockIndex);
   stw_types::sint32 SetHALCDomainChannelConfigUseCase(const stw_types::uint32 ou32_NodeIndex,
                                                       const stw_types::uint32 ou32_DomainIndex,
                                                       const stw_types::uint32 ou32_ChannelIndex,
                                                       const bool oq_UseCaseSet,
                                                       const stw_types::uint32 ou32_UseCaseIndex);
   stw_types::sint32 SetHALCDomainChannelConfigDatablockConfig(const stw_types::uint32 ou32_NodeIndex,
                                                               const stw_types::uint32 ou32_DomainIndex,
                                                               const stw_types::uint32 ou32_ChannelIndex, const
                                                               std::vector<stw_opensyde_core::C_OSCHalcConfigChannel::E_DataBlockConfig> & orc_DatablockConfig);
   stw_types::sint32 SetHALCDomainChannelParameterConfig(const stw_types::uint32 ou32_NodeIndex,
                                                         const stw_types::uint32 ou32_DomainIndex,
                                                         const stw_types::uint32 ou32_ChannelIndex,
                                                         const stw_types::uint32 ou32_ParameterIndex, const stw_opensyde_core::C_OSCHalcConfigParameterStruct &
                                                         orc_Parameter);
   stw_types::sint32 SetHALCDomainChannelParameterConfigElement(const stw_types::uint32 ou32_NodeIndex,
                                                                const stw_types::uint32 ou32_DomainIndex,
                                                                const stw_types::uint32 ou32_ChannelIndex,
                                                                const stw_types::uint32 ou32_ParameterIndex,
                                                                const stw_types::uint32 ou32_ElementIndex, const stw_opensyde_core::C_OSCHalcConfigParameter &
                                                                orc_Parameter);

   //Misc
   stw_types::sint32 HALCGenerateDatapools(const stw_types::uint32 ou32_NodeIndex);

private:
   static C_PuiSdNodeDataPool mh_GetUiDatapoolForOSCDataPool(
      const stw_opensyde_core::C_OSCNodeDataPool & orc_OSCDatapool);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
