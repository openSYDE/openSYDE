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
#include "C_OscNode.hpp"
#include "C_OscHalcMagicianDatapoolListHandler.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcMagicianGenerator
{
public:
   C_OscHalcMagicianGenerator(const C_OscNode * const opc_Node);

   int32_t GenerateHalcDatapools(std::vector<C_OscNodeDataPool> & orc_Datapools) const;

private:
   const C_OscNode * const mpc_Node;

   int32_t m_GenerateHalcDatapoolsDefinition(std::vector<C_OscNodeDataPool> & orc_Datapools) const;
   int32_t m_FillHalcDatapools(std::vector<C_OscNodeDataPool> & orc_Datapools) const;
   static int32_t mh_FillHalcDatapoolsDomain(C_OscNodeDataPoolList & orc_List, const C_OscHalcConfigDomain & orc_Domain,
                                             const C_OscHalcMagicianDatapoolListHandler & orc_Handler,
                                             const uint32_t ou32_ItDomain);
   static int32_t mh_FillHalcDatapoolsChanNum(C_OscNodeDataPoolList & orc_List,
                                              const C_OscHalcMagicianDatapoolListHandler & orc_Handler,
                                              const uint32_t ou32_ItDomain, const uint32_t ou32_RelevantIndex,
                                              const uint32_t ou32_ChanNum);
   static int32_t mh_FillHalcDatapoolsSafetyFlag(C_OscNodeDataPoolList & orc_List,
                                                 const C_OscHalcMagicianDatapoolListHandler & orc_Handler,
                                                 const uint32_t ou32_ItDomain, const uint32_t ou32_RelevantIndex,
                                                 const bool oq_IsSafetyRelevant);
   static int32_t mh_FillHalcDatapoolsUseCase(C_OscNodeDataPoolList & orc_List,
                                              const C_OscHalcMagicianDatapoolListHandler & orc_Handler,
                                              const uint32_t ou32_ItDomain, const uint32_t ou32_RelevantIndex,
                                              const uint8_t ou8_UseCaseValue);
   static int32_t mh_FillHalcDatapoolsChannel(C_OscNodeDataPoolList & orc_List,
                                              const C_OscHalcConfigChannel & orc_Channel,
                                              const C_OscHalcMagicianDatapoolListHandler & orc_Handler,
                                              const uint32_t ou32_ItDomain, const uint32_t ou32_ItRelevantChannel,
                                              const std::vector<uint32_t> & orc_RelevantParameters,
                                              const std::vector<C_OscHalcDefStruct> & orc_DefParam);
   static int32_t mh_FillHalcElement(C_OscNodeDataPoolListElement * const opc_Element,
                                     const C_OscHalcDefContent & orc_Value, const uint32_t ou32_ChannelIndex);
   static int32_t mh_GenerateVariablesForDomain(const C_OscHalcConfigDomain & orc_Domain,
                                                C_OscNodeDataPoolList & orc_HalcListParam,
                                                C_OscNodeDataPoolList & orc_HalcListInput,
                                                C_OscNodeDataPoolList & orc_HalcListOutput,
                                                C_OscNodeDataPoolList & orc_HalcListStatus, const bool oq_IsSafe,
                                                const C_OscHalcMagicianDatapoolListHandler & orc_DpHandler);
   static int32_t mh_GenerateVariablesForVector(const std::vector<C_OscHalcDefStruct> & orc_Definition,
                                                const stw::scl::C_SclString & orc_DomainSingularName,
                                                const bool oq_IsSafe, const uint32_t ou32_NumChannels,
                                                const bool oq_AddDataset, C_OscNodeDataPoolList & orc_List);
   static int32_t mh_GenerateVariablesForVectorElement(const std::vector<C_OscHalcDefStruct> & orc_Definition,
                                                       const uint32_t ou32_DefinitionElementIndex,
                                                       const stw::scl::C_SclString & orc_DomainSingularName,
                                                       const bool oq_IsSafe, const uint32_t ou32_NumChannels,
                                                       const bool oq_AddDataset, C_OscNodeDataPoolList & orc_List);
   static int32_t mh_AddVariableToList(const C_OscHalcDefElement & orc_Definition,
                                       const std::vector<C_OscHalcDefStruct> & orc_DefinitionArray,
                                       const uint32_t ou32_ParameterIndexStruct,
                                       const uint32_t ou32_ParameterIndexElement,
                                       const stw::scl::C_SclString & orc_DomainSingularName, const bool oq_IsSafe,
                                       const uint32_t ou32_NumChannels, const bool oq_AddDataset,
                                       C_OscNodeDataPoolList & orc_List);
   static int32_t mh_ConvertToDatapoolWithoutArray(const C_OscHalcDefContent & orc_HalcContent,
                                                   C_OscNodeDataPoolContent & orc_DpContent);
   static int32_t mh_ConvertToDatapoolAndAssign(const C_OscHalcDefContent & orc_HalcContent,
                                                C_OscNodeDataPoolContent & orc_DpContent, const uint32_t ou32_Index);
   static int32_t mh_ConvertToDatapoolAndResize(const C_OscHalcDefContent & orc_HalcContent,
                                                const uint32_t ou32_NumChannels,
                                                C_OscNodeDataPoolContent & orc_DpContent);
   static void mh_InitList(C_OscNodeDataPoolList & orc_List, const stw::scl::C_SclString & orc_Name,
                           const bool oq_AddDataset = false);
   static void mh_CleanUpHalcDatapools(std::vector<C_OscNodeDataPool> & orc_Datapools);
   void m_FillEmptySpaceHalcDatapools(std::vector<C_OscNodeDataPool> & orc_Datapools) const;
   bool m_CheckTwoDp(void) const;
   int32_t m_HandleCopies(std::vector<C_OscNodeDataPool> & orc_Datapools) const;
   int32_t m_HandleNvm(std::vector<C_OscNodeDataPool> & orc_Datapools) const;
   int32_t m_HandleNvmDpOffset(std::vector<C_OscNodeDataPool> & orc_Datapools,
                               const std::vector<uint32_t> & orc_Offsets, const bool oq_IsSafe) const;
   void m_HandleVersion(C_OscNodeDataPool & orc_Datapool) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
