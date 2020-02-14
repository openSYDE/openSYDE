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
   static stw_types::sint32 m_FillHALCElement(C_OSCNodeDataPoolListElement * const opc_Element,
                                              const C_OSCHalcConfigParameter & orc_HALCParameter,
                                              const stw_types::uint32 ou32_ChannelIndex);
   static void m_AddUseCaseVariable(const stw_scl::C_SCLString & orc_DomainSingularName,
                                    const stw_types::uint32 ou32_NumChannels, const bool oq_AddDataset,
                                    C_OSCNodeDataPoolList & orc_List);
   static stw_types::sint32 m_GenerateVariablesForVector(const std::vector<C_OSCHalcDefStruct> & orc_Definition,
                                                         const stw_scl::C_SCLString & orc_DomainSingularName,
                                                         const stw_types::uint32 ou32_NumChannels,
                                                         const bool oq_AddDataset, C_OSCNodeDataPoolList & orc_List);
   static stw_types::sint32 m_AddVariableToList(const C_OSCHalcDefElement & orc_Definition,
                                                const std::vector<C_OSCHalcDefStruct> & orc_DefinitionArray,
                                                const stw_types::uint32 ou32_ParameterIndexStruct,
                                                const stw_types::uint32 ou32_ParameterIndexElement,
                                                const stw_scl::C_SCLString & orc_DomainSingularName,
                                                const stw_types::uint32 ou32_NumChannels, const bool oq_AddDataset,
                                                C_OSCNodeDataPoolList & orc_List);
   static stw_types::sint32 m_ConvertToDatapoolWithoutArray(const C_OSCHalcDefContent & orc_HALCContent,
                                                            C_OSCNodeDataPoolContent & orc_DpContent);
   static stw_types::sint32 m_ConvertToDatapoolAndAssign(const C_OSCHalcDefContent & orc_HALCContent,
                                                         C_OSCNodeDataPoolContent & orc_DpContent,
                                                         const stw_types::uint32 ou32_Index);
   static stw_types::sint32 m_ConvertToDatapoolAndResize(const C_OSCHalcDefContent & orc_HALCContent,
                                                         const stw_types::uint32 ou32_NumChannels,
                                                         C_OSCNodeDataPoolContent & orc_DpContent);
   static void m_InitList(C_OSCNodeDataPoolList & orc_List, const stw_scl::C_SCLString & orc_Name,
                          const bool oq_AddDataset = false);
   static void m_CleanUpHALCDatapools(std::vector<C_OSCNodeDataPool> & orc_Datapools);
   stw_types::sint32 m_AssignHALCDatapools(std::vector<C_OSCNodeDataPool> & orc_Datapools) const;
   static void m_SetCommonDefaults(C_OSCNodeDataPoolListElement & orc_Element);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
