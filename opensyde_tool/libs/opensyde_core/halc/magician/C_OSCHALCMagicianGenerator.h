//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC Magician: HALC datapool generator
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCMAGICIANGENERATOR_H
#define C_OSCHALCMAGICIANGENERATOR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCNode.h"
#include "C_OSCHALCMagicianDatapoolListHandler.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHALCMagicianGenerator
{
public:
   C_OSCHALCMagicianGenerator(const C_OSCNode * const opc_Node);

   stw_types::sint32 GenerateHALCDatapools(std::vector<C_OSCNodeDataPool> & orc_Datapools) const;

private:
   const C_OSCNode * const mpc_Node;

   stw_types::sint32 m_GenerateHALCDatapoolsDefinition(std::vector<C_OSCNodeDataPool> & orc_Datapools) const;
   stw_types::sint32 m_FillHALCDatapools(std::vector<C_OSCNodeDataPool> & orc_Datapools) const;
   stw_types::sint32 m_FillHALCDatapoolsDomain(C_OSCNodeDataPoolList & orc_List,
                                               const C_OSCHalcConfigDomain & orc_Domain,
                                               const C_OSCHALCMagicianDatapoolListHandler & orc_Handler,
                                               const stw_types::uint32 ou32_ItDomain) const;
   stw_types::sint32 m_FillHALCDatapoolsChanNum(C_OSCNodeDataPoolList & orc_List,
                                                const C_OSCHALCMagicianDatapoolListHandler & orc_Handler,
                                                const stw_types::uint32 ou32_ItDomain,
                                                const stw_types::uint32 ou32_RelevantIndex,
                                                const stw_types::uint32 ou32_ChanNum) const;
   stw_types::sint32 m_FillHALCDatapoolsSafetyFlag(C_OSCNodeDataPoolList & orc_List,
                                                   const C_OSCHALCMagicianDatapoolListHandler & orc_Handler,
                                                   const stw_types::uint32 ou32_ItDomain,
                                                   const stw_types::uint32 ou32_RelevantIndex,
                                                   const bool oq_IsSafetyRelevant) const;
   stw_types::sint32 m_FillHALCDatapoolsUseCase(C_OSCNodeDataPoolList & orc_List,
                                                const C_OSCHALCMagicianDatapoolListHandler & orc_Handler,
                                                const stw_types::uint32 ou32_ItDomain,
                                                const stw_types::uint32 ou32_RelevantIndex,
                                                const stw_types::uint8 ou8_UseCaseValue) const;
   stw_types::sint32 m_FillHALCDatapoolsChannel(C_OSCNodeDataPoolList & orc_List,
                                                const C_OSCHalcConfigChannel & orc_Channel,
                                                const C_OSCHALCMagicianDatapoolListHandler & orc_Handler,
                                                const stw_types::uint32 ou32_ItDomain,
                                                const stw_types::uint32 ou32_ItRelevantChannel,
                                                const std::vector<stw_types::uint32> & orc_RelevantParameters,
                                                const std::vector<C_OSCHalcDefStruct> & orc_DefParam) const;
   static stw_types::sint32 mh_FillHALCElement(C_OSCNodeDataPoolListElement * const opc_Element,
                                               const C_OSCHalcDefContent & orc_Value,
                                               const stw_types::uint32 ou32_ChannelIndex);
   static stw_types::sint32 mh_GenerateVariablesForDomain(const C_OSCHalcConfigDomain & orc_Domain,
                                                          C_OSCNodeDataPoolList & orc_HALCListParam,
                                                          C_OSCNodeDataPoolList & orc_HALCListInput,
                                                          C_OSCNodeDataPoolList & orc_HALCListOutput,
                                                          C_OSCNodeDataPoolList & orc_HALCListStatus,
                                                          const bool oq_IsSafe,
                                                          const C_OSCHALCMagicianDatapoolListHandler & orc_DpHandler);
   static stw_types::sint32 mh_GenerateVariablesForVector(const std::vector<C_OSCHalcDefStruct> & orc_Definition,
                                                          const stw_scl::C_SCLString & orc_DomainSingularName,
                                                          const bool oq_IsSafe,
                                                          const stw_types::uint32 ou32_NumChannels,
                                                          const bool oq_AddDataset, C_OSCNodeDataPoolList & orc_List);
   static stw_types::sint32 mh_GenerateVariablesForVectorElement(const std::vector<C_OSCHalcDefStruct> & orc_Definition,
                                                                 const stw_types::uint32 ou32_DefinitionElementIndex,
                                                                 const stw_scl::C_SCLString & orc_DomainSingularName,
                                                                 const bool oq_IsSafe,
                                                                 const stw_types::uint32 ou32_NumChannels,
                                                                 const bool oq_AddDataset,
                                                                 C_OSCNodeDataPoolList & orc_List);
   static stw_types::sint32 mh_AddVariableToList(const C_OSCHalcDefElement & orc_Definition,
                                                 const std::vector<C_OSCHalcDefStruct> & orc_DefinitionArray,
                                                 const stw_types::uint32 ou32_ParameterIndexStruct,
                                                 const stw_types::uint32 ou32_ParameterIndexElement,
                                                 const stw_scl::C_SCLString & orc_DomainSingularName,
                                                 const bool oq_IsSafe, const stw_types::uint32 ou32_NumChannels,
                                                 const bool oq_AddDataset, C_OSCNodeDataPoolList & orc_List);
   static stw_types::sint32 mh_ConvertToDatapoolWithoutArray(const C_OSCHalcDefContent & orc_HALCContent,
                                                             C_OSCNodeDataPoolContent & orc_DpContent);
   static stw_types::sint32 mh_ConvertToDatapoolAndAssign(const C_OSCHalcDefContent & orc_HALCContent,
                                                          C_OSCNodeDataPoolContent & orc_DpContent,
                                                          const stw_types::uint32 ou32_Index);
   static stw_types::sint32 mh_ConvertToDatapoolAndResize(const C_OSCHalcDefContent & orc_HALCContent,
                                                          const stw_types::uint32 ou32_NumChannels,
                                                          C_OSCNodeDataPoolContent & orc_DpContent);
   static void mh_InitList(C_OSCNodeDataPoolList & orc_List, const stw_scl::C_SCLString & orc_Name,
                           const bool oq_AddDataset = false);
   static void mh_CleanUpHALCDatapools(std::vector<C_OSCNodeDataPool> & orc_Datapools);
   void m_FillEmptySpaceHALCDatapools(std::vector<C_OSCNodeDataPool> & orc_Datapools) const;
   bool m_CheckTwoDp(void) const;
   stw_types::sint32 m_HandleCopies(std::vector<C_OSCNodeDataPool> & orc_Datapools) const;
   stw_types::sint32 m_HandleNVM(std::vector<C_OSCNodeDataPool> & orc_Datapools) const;
   stw_types::sint32 m_HandleNVMDpOffset(std::vector<C_OSCNodeDataPool> & orc_Datapools,
                                         const std::vector<stw_types::uint32> & orc_Offsets,
                                         const bool oq_IsSafe) const;
   void m_HandleVersion(C_OSCNodeDataPool & orc_Datapool) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
