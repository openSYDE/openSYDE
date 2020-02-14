//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC Magician: HALC datapool generator

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>

#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCHALCMagicianUtil.h"
#include "C_OSCHALCMagicianGenerator.h"
#include "C_OSCHALCMagicianDatapoolListHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out]  opc_Node   Node
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHALCMagicianGenerator::C_OSCHALCMagicianGenerator(const C_OSCNode * const opc_Node) :
   mpc_Node(opc_Node)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Generate HALC datapools

   \param[in,out]  orc_Datapools    Datapools

   \return
   C_NO_ERR Datapool generated
   C_CONFIG Configuration invalid
   C_BUSY   No programmable application block found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::GenerateHALCDatapools(std::vector<C_OSCNodeDataPool> & orc_Datapools) const
{
   sint32 s32_Retval = m_GenerateHALCDatapoolsDefinition(orc_Datapools);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = m_FillHALCDatapools(orc_Datapools);
   }
   m_CleanUpHALCDatapools(orc_Datapools);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = m_AssignHALCDatapools(orc_Datapools);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Generate HALC datapools definition

   \param[in,out]  orc_Datapools    Datapools

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_GenerateHALCDatapoolsDefinition(std::vector<C_OSCNodeDataPool> & orc_Datapools)
const
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->mpc_Node != NULL)
   {
      orc_Datapools.clear();
      {
         //Generate datapool
         C_OSCNodeDataPool c_HALCDp;
         C_OSCNodeDataPoolList c_HALCListParam;
         C_OSCNodeDataPoolList c_HALCListInput;
         C_OSCNodeDataPoolList c_HALCListOutput;
         C_OSCNodeDataPoolList c_HALCListStatus;
         c_HALCDp.c_Lists.clear();
         c_HALCDp.e_Type = C_OSCNodeDataPool::eHALC;
         c_HALCDp.c_Name = C_OSCHALCMagicianUtil::h_GetDatapoolName();
         //Generate lists
         C_OSCHALCMagicianGenerator::m_InitList(c_HALCListParam,
                                                C_OSCHALCMagicianUtil::h_GetListName(
                                                   C_OSCHalcDefDomain::eVA_PARAM), true);
         C_OSCHALCMagicianGenerator::m_InitList(c_HALCListInput,
                                                C_OSCHALCMagicianUtil::h_GetListName(C_OSCHalcDefDomain::eVA_INPUT));
         C_OSCHALCMagicianGenerator::m_InitList(c_HALCListOutput,
                                                C_OSCHALCMagicianUtil::h_GetListName(C_OSCHalcDefDomain::eVA_OUTPUT));
         C_OSCHALCMagicianGenerator::m_InitList(c_HALCListStatus,
                                                C_OSCHALCMagicianUtil::h_GetListName(C_OSCHalcDefDomain::eVA_STATUS));
         //Generate variables
         for (uint32 u32_It = 0UL; u32_It < this->mpc_Node->c_HALCConfig.GetDomainSize(); ++u32_It)
         {
            const C_OSCHalcConfigDomain * const pc_Config =
               this->mpc_Node->c_HALCConfig.GetDomainConfigDataConst(u32_It);
            tgl_assert(pc_Config != NULL);
            if (pc_Config != NULL)
            {
               if (pc_Config->c_ChannelUseCases.size() > 0UL)
               {
                  m_AddUseCaseVariable(pc_Config->c_SingularName,
                                       pc_Config->c_ChannelConfigs.size(), true,
                                       c_HALCListParam);
               }
               s32_Retval = m_GenerateVariablesForVector(pc_Config->c_Parameters, pc_Config->c_SingularName,
                                                         pc_Config->c_ChannelConfigs.size(), true,
                                                         c_HALCListParam);

               if (s32_Retval == C_NO_ERR)
               {
                  s32_Retval = m_GenerateVariablesForVector(pc_Config->c_InputValues, pc_Config->c_SingularName,
                                                            pc_Config->c_ChannelConfigs.size(), false,
                                                            c_HALCListInput);
               }
               if (s32_Retval == C_NO_ERR)
               {
                  s32_Retval = m_GenerateVariablesForVector(pc_Config->c_OutputValues, pc_Config->c_SingularName,
                                                            pc_Config->c_ChannelConfigs.size(), false,
                                                            c_HALCListOutput);
               }
               if (s32_Retval == C_NO_ERR)
               {
                  s32_Retval = m_GenerateVariablesForVector(pc_Config->c_StatusValues, pc_Config->c_SingularName,
                                                            pc_Config->c_ChannelConfigs.size(), false,
                                                            c_HALCListStatus);
               }
            }
         }
         if (s32_Retval == C_NO_ERR)
         {
            c_HALCDp.c_Lists.push_back(c_HALCListParam);
            c_HALCDp.c_Lists.push_back(c_HALCListInput);
            c_HALCDp.c_Lists.push_back(c_HALCListOutput);
            c_HALCDp.c_Lists.push_back(c_HALCListStatus);
            orc_Datapools.push_back(c_HALCDp);
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
      osc_write_log_warning("HALC datapool generation", "node not assigned");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill HALC datapools

   \param[in,out]  orc_Datapools    Datapools

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_FillHALCDatapools(std::vector<C_OSCNodeDataPool> & orc_Datapools) const
{
   sint32 s32_Retval = C_NO_ERR;

   if ((this->mpc_Node != NULL) && (orc_Datapools.size() == 1UL))
   {
      C_OSCNodeDataPool & rc_Datapool = orc_Datapools[0UL];
      if (rc_Datapool.c_Lists.size() == 4UL)
      {
         C_OSCNodeDataPoolList & rc_List = rc_Datapool.c_Lists[0UL];
         const C_OSCHALCMagicianDatapoolListHandler c_Handler(this->mpc_Node->c_HALCConfig);
         for (uint32 u32_ItDomain = 0UL;
              (u32_ItDomain < this->mpc_Node->c_HALCConfig.GetDomainSize()) && (s32_Retval == C_NO_ERR); ++u32_ItDomain)
         {
            const C_OSCHalcConfigDomain * const pc_Config =
               this->mpc_Node->c_HALCConfig.GetDomainConfigDataConst(u32_ItDomain);
            tgl_assert(pc_Config != NULL);
            if (pc_Config != NULL)
            {
               for (uint32 u32_ItChannel = 0UL;
                    (u32_ItChannel < pc_Config->c_ChannelConfigs.size()) && (s32_Retval == C_NO_ERR); ++u32_ItChannel)
               {
                  const C_OSCHalcConfigChannel & rc_Channel = pc_Config->c_ChannelConfigs[u32_ItChannel];
                  if (pc_Config->c_ChannelConfigs.size() > 0UL)
                  {
                     //Fill Use-case
                     C_OSCNodeDataPoolListElement * const pc_UseCaseElement = c_Handler.GetUseCaseListElement(
                        u32_ItDomain,
                        rc_List);
                     if (pc_UseCaseElement != NULL)
                     {
                        if (pc_UseCaseElement->c_DataSetValues.size() > 0UL)
                        {
                           C_OSCNodeDataPoolContent & rc_Content = pc_UseCaseElement->c_DataSetValues[0UL];
                           uint32 u32_Value;
                           if (rc_Channel.q_UseCaseSet)
                           {
                              u32_Value = rc_Channel.u32_UseCaseIndex;
                           }
                           else
                           {
                              u32_Value = 0UL;
                           }
                           if (pc_UseCaseElement->GetArray())
                           {
                              rc_Content.SetValueAU8Element(static_cast<uint8>(u32_Value), u32_ItChannel);
                           }
                           else
                           {
                              rc_Content.SetValueU8(static_cast<uint8>(u32_Value));
                           }
                        }
                        else
                        {
                           s32_Retval = C_CONFIG;
                           osc_write_log_warning("HALC datapool generation", "unexpected number of datasets");
                        }
                     }
                     else
                     {
                        s32_Retval = C_CONFIG;
                        osc_write_log_warning("HALC datapool generation", "unexpected number of list elements");
                     }
                  }
                  for (uint32 u32_ItParam = 0UL;
                       (u32_ItParam < rc_Channel.c_Parameters.size()) && (s32_Retval == C_NO_ERR); ++u32_ItParam)
                  {
                     const C_OSCHalcConfigParameterStruct & rc_Param = rc_Channel.c_Parameters[u32_ItParam];
                     if (rc_Param.c_ParameterElements.size() > 0UL)
                     {
                        for (uint32 u32_ItElem = 0UL;
                             (u32_ItElem < rc_Param.c_ParameterElements.size()) && (s32_Retval == C_NO_ERR);
                             ++u32_ItElem)
                        {
                           C_OSCNodeDataPoolListElement * const pc_Element = c_Handler.GetListElement(u32_ItDomain,
                                                                                                      u32_ItParam,
                                                                                                      u32_ItElem,
                                                                                                      rc_List,
                                                                                                      C_OSCHalcDefDomain::eVA_PARAM);
                           s32_Retval =
                              m_FillHALCElement(pc_Element,
                                                rc_Param.c_ParameterElements[u32_ItElem],
                                                u32_ItChannel);
                        }
                     }
                     else
                     {
                        C_OSCNodeDataPoolListElement * const pc_Element = c_Handler.GetListElement(u32_ItDomain,
                                                                                                   u32_ItParam,
                                                                                                   0UL,
                                                                                                   rc_List,
                                                                                                   C_OSCHalcDefDomain::eVA_PARAM);
                        s32_Retval = m_FillHALCElement(pc_Element, rc_Param, u32_ItChannel);
                     }
                  }
               }
            }
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
      osc_write_log_warning("HALC datapool generation", "node not assigned or unexpected datapool size");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill HALC element

   \param[in,out]  opc_Element         Element
   \param[in]      orc_HALCParameter   HALC parameter
   \param[in]      ou32_ChannelIndex   Channel index

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_FillHALCElement(C_OSCNodeDataPoolListElement * const opc_Element,
                                                     const C_OSCHalcConfigParameter & orc_HALCParameter,
                                                     const uint32 ou32_ChannelIndex)
{
   sint32 s32_Retval;

   if (opc_Element != NULL)
   {
      if (opc_Element->c_DataSetValues.size() >= 1UL)
      {
         s32_Retval =
            m_ConvertToDatapoolAndAssign(orc_HALCParameter.c_Value, opc_Element->c_DataSetValues[0UL],
                                         ou32_ChannelIndex);
      }
      else
      {
         s32_Retval = C_CONFIG;
         osc_write_log_warning("HALC datapool generation", "unexpected number of datasets");
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
      osc_write_log_warning("HALC datapool generation", "unexpected number of list elements");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add use case variable

   \param[in]      orc_DomainSingularName    Domain singular name
   \param[in]      ou32_NumChannels          Num channels
   \param[in]      oq_AddDataset             Add dataset
   \param[in,out]  orc_List                  List
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHALCMagicianGenerator::m_AddUseCaseVariable(const stw_scl::C_SCLString & orc_DomainSingularName,
                                                      const uint32 ou32_NumChannels, const bool oq_AddDataset,
                                                      C_OSCNodeDataPoolList & orc_List)
{
   C_OSCNodeDataPoolListElement c_Element;

   c_Element.c_Name = C_OSCHALCMagicianUtil::h_CombineVariableName(orc_DomainSingularName, "UseCase");
   c_Element.c_Comment = "Selected use-case";
   //Defined defaults
   m_SetCommonDefaults(c_Element);

   //Type
   c_Element.c_MinValue.SetType(C_OSCNodeDataPoolContent::eUINT8);
   if (ou32_NumChannels > 1UL)
   {
      c_Element.c_MinValue.SetArray(true);
      c_Element.c_MinValue.SetArraySize(ou32_NumChannels);
   }
   else
   {
      c_Element.c_MinValue.SetArray(false);
   }

   //Copy type
   c_Element.c_MaxValue = c_Element.c_MinValue;

   //Value
   if (ou32_NumChannels > 1UL)
   {
      for (uint32 u32_It = 0UL; u32_It < ou32_NumChannels; ++u32_It)
      {
         c_Element.c_MinValue.SetValueAU8Element(std::numeric_limits<uint8>::min(), u32_It);
         c_Element.c_MaxValue.SetValueAU8Element(std::numeric_limits<uint8>::max(), u32_It);
      }
   }
   else
   {
      c_Element.c_MinValue.SetValueU8(std::numeric_limits<uint8>::min());
      c_Element.c_MaxValue.SetValueU8(std::numeric_limits<uint8>::max());
   }

   //Dataset
   if (oq_AddDataset)
   {
      c_Element.c_DataSetValues.push_back(c_Element.c_MinValue);
   }

   //Init including value
   c_Element.c_Value = c_Element.c_MinValue;
   c_Element.c_NvmValue = c_Element.c_MinValue;

   orc_List.c_Elements.push_back(c_Element);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Generate variables for vector

   \param[in]      orc_Definition            Definition
   \param[in]      orc_DomainSingularName    Domain singular name
   \param[in]      ou32_NumChannels          Num channels
   \param[in]      oq_AddDataset             Add dataset
   \param[in,out]  orc_List                  List

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_GenerateVariablesForVector(const std::vector<C_OSCHalcDefStruct> & orc_Definition,
                                                                const stw_scl::C_SCLString & orc_DomainSingularName,
                                                                const uint32 ou32_NumChannels, const bool oq_AddDataset,
                                                                C_OSCNodeDataPoolList & orc_List)
{
   sint32 s32_Retval = C_NO_ERR;

   for (uint32 u32_ItStruct = 0UL; (u32_ItStruct < orc_Definition.size()) && (s32_Retval == C_NO_ERR); ++u32_ItStruct)
   {
      const C_OSCHalcDefStruct & rc_Struct = orc_Definition[u32_ItStruct];
      if (rc_Struct.c_StructElements.size() > 0UL)
      {
         for (uint32 u32_ItElem = 0UL; (u32_ItElem < rc_Struct.c_StructElements.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItElem)
         {
            s32_Retval = m_AddVariableToList(rc_Struct.c_StructElements[u32_ItElem], orc_Definition, u32_ItStruct,
                                             u32_ItElem, orc_DomainSingularName, ou32_NumChannels, oq_AddDataset,
                                             orc_List);
         }
      }
      else
      {
         s32_Retval =
            m_AddVariableToList(rc_Struct, orc_Definition, u32_ItStruct, 0UL, orc_DomainSingularName, ou32_NumChannels,
                                oq_AddDataset,
                                orc_List);
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add variable to list

   \param[in]      orc_Definition               Definition
   \param[in]      orc_DefinitionArray          Definition array
   \param[in]      ou32_ParameterIndexStruct    Parameter index struct
   \param[in]      ou32_ParameterIndexElement   Parameter index element
   \param[in]      orc_DomainSingularName       Domain singular name
   \param[in]      ou32_NumChannels             Num channels
   \param[in]      oq_AddDataset                Add dataset
   \param[in,out]  orc_List                     List

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_AddVariableToList(const C_OSCHalcDefElement & orc_Definition,
                                                       const std::vector<C_OSCHalcDefStruct> & orc_DefinitionArray,
                                                       const uint32 ou32_ParameterIndexStruct,
                                                       const uint32 ou32_ParameterIndexElement,
                                                       const stw_scl::C_SCLString & orc_DomainSingularName,
                                                       const uint32 ou32_NumChannels, const bool oq_AddDataset,
                                                       C_OSCNodeDataPoolList & orc_List)
{
   sint32 s32_Retval;
   C_OSCNodeDataPoolListElement c_Element;

   c_Element.c_DataSetValues.clear();

   c_Element.c_Comment = orc_Definition.c_Comment;

   //Defined defaults
   m_SetCommonDefaults(c_Element);

   s32_Retval = m_ConvertToDatapoolAndResize(orc_Definition.c_MinValue, ou32_NumChannels, c_Element.c_MinValue);
   if (s32_Retval == C_NO_ERR)
   {
      m_ConvertToDatapoolAndResize(orc_Definition.c_MaxValue, ou32_NumChannels, c_Element.c_MaxValue);
   }
   if (s32_Retval == C_NO_ERR)
   {
      //Initialize these
      c_Element.c_Value = c_Element.c_MinValue;
      c_Element.c_NvmValue = c_Element.c_MinValue;
      if (oq_AddDataset)
      {
         c_Element.c_DataSetValues.push_back(c_Element.c_MinValue);
      }

      tgl_assert(C_OSCHALCMagicianUtil::h_GetVariableName(orc_DefinitionArray, ou32_ParameterIndexStruct,
                                                          ou32_ParameterIndexElement, orc_DomainSingularName,
                                                          c_Element.c_Name) == C_NO_ERR);
      orc_List.c_Elements.push_back(c_Element);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert to datapool without array

   \param[in]      orc_HALCContent  HALC content
   \param[in,out]  orc_DpContent    Dp content

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_ConvertToDatapoolWithoutArray(const C_OSCHalcDefContent & orc_HALCContent,
                                                                   C_OSCNodeDataPoolContent & orc_DpContent)
{
   sint32 s32_Retval = C_NO_ERR;
   uint64 u64_Val = 0ULL;
   sint64 s64_Val = 0LL;

   orc_DpContent = orc_HALCContent;

   if ((orc_HALCContent.GetArray()) ||
       ((orc_HALCContent.GetComplexType() == C_OSCHalcDefContent::eCT_BIT_MASK) &&
        ((orc_HALCContent.GetType() == C_OSCNodeDataPoolContent::eFLOAT32) ||
         (orc_HALCContent.GetType() == C_OSCNodeDataPoolContent::eFLOAT64))))
   {
      s32_Retval = C_CONFIG;
      osc_write_log_warning("HALC datapool generation",
                            "HALC config invalid: no arrays allowed and float not supported for bitmask");
   }
   else
   {
      switch (orc_HALCContent.GetComplexType())
      {
      case C_OSCHalcDefContent::eCT_ENUM:
      case C_OSCHalcDefContent::eCT_PLAIN:
         //Nothing to do
         break;
      case C_OSCHalcDefContent::eCT_BIT_MASK:
         //Get current value
         switch (orc_HALCContent.GetType())
         {
         case C_OSCNodeDataPoolContent::eUINT8:
            u64_Val = static_cast<uint64>(orc_HALCContent.GetValueU8());
            break;
         case C_OSCNodeDataPoolContent::eUINT16:
            u64_Val = static_cast<uint64>(orc_HALCContent.GetValueU16());
            break;
         case C_OSCNodeDataPoolContent::eUINT32:
            u64_Val = static_cast<uint64>(orc_HALCContent.GetValueU32());
            break;
         case C_OSCNodeDataPoolContent::eUINT64:
            u64_Val = orc_HALCContent.GetValueU64();
            break;
         case C_OSCNodeDataPoolContent::eSINT8:
            s64_Val = static_cast<sint64>(orc_HALCContent.GetValueS8());
            break;
         case C_OSCNodeDataPoolContent::eSINT16:
            s64_Val = static_cast<sint64>(orc_HALCContent.GetValueS16());
            break;
         case C_OSCNodeDataPoolContent::eSINT32:
            s64_Val = static_cast<sint64>(orc_HALCContent.GetValueS32());
            break;
         case C_OSCNodeDataPoolContent::eSINT64:
            s64_Val = orc_HALCContent.GetValueS64();
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
         case C_OSCNodeDataPoolContent::eFLOAT64:
            tgl_assert(false);
            break;
         }
         //Apply bits
         {
            const std::vector<C_OSCHalcDefContentBitmaskItem> & rc_BitmaskItems = orc_HALCContent.GetBitmaskItems();
            for (uint32 u32_It = 0UL; u32_It < rc_BitmaskItems.size(); ++u32_It)
            {
               const C_OSCHalcDefContentBitmaskItem & rc_BitmaskItem = rc_BitmaskItems[u32_It];
               u64_Val |= rc_BitmaskItem.u64_Value;
               s64_Val = static_cast<sint64>(static_cast<uint64>(s64_Val) | rc_BitmaskItem.u64_Value);
            }
         }
         //Set new value
         switch (orc_HALCContent.GetType())
         {
         case C_OSCNodeDataPoolContent::eUINT8:
            orc_DpContent.SetValueU8(static_cast<uint8>(u64_Val));
            break;
         case C_OSCNodeDataPoolContent::eUINT16:
            orc_DpContent.SetValueU16(static_cast<uint16>(u64_Val));
            break;
         case C_OSCNodeDataPoolContent::eUINT32:
            orc_DpContent.SetValueU32(static_cast<uint32>(u64_Val));
            break;
         case C_OSCNodeDataPoolContent::eUINT64:
            orc_DpContent.SetValueU64(u64_Val);
            break;
         case C_OSCNodeDataPoolContent::eSINT8:
            orc_DpContent.SetValueS8(static_cast<sint8>(s64_Val));
            break;
         case C_OSCNodeDataPoolContent::eSINT16:
            orc_DpContent.SetValueS16(static_cast<sint16>(s64_Val));
            break;
         case C_OSCNodeDataPoolContent::eSINT32:
            orc_DpContent.SetValueS32(static_cast<sint32>(s64_Val));
            break;
         case C_OSCNodeDataPoolContent::eSINT64:
            orc_DpContent.SetValueS64(s64_Val);
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
         case C_OSCNodeDataPoolContent::eFLOAT64:
            tgl_assert(false);
            break;
         }
         break;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert to datapool and assign

   \param[in]      orc_HALCContent  HALC content
   \param[in,out]  orc_DpContent    Dp content
   \param[in]      ou32_Index       Index

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_ConvertToDatapoolAndAssign(const C_OSCHalcDefContent & orc_HALCContent,
                                                                C_OSCNodeDataPoolContent & orc_DpContent,
                                                                const uint32 ou32_Index)
{
   C_OSCNodeDataPoolContent c_ApplyValue;
   sint32 s32_Retval = m_ConvertToDatapoolWithoutArray(orc_HALCContent, c_ApplyValue);

   if (s32_Retval == C_NO_ERR)
   {
      if (((orc_HALCContent.GetArray()) || (c_ApplyValue.GetArray())) ||
          ((orc_HALCContent.GetComplexType() == C_OSCHalcDefContent::eCT_BIT_MASK) &&
           ((orc_HALCContent.GetType() == C_OSCNodeDataPoolContent::eFLOAT32) ||
            (orc_HALCContent.GetType() == C_OSCNodeDataPoolContent::eFLOAT64))))
      {
         s32_Retval = C_CONFIG;
         osc_write_log_warning("HALC datapool generation",
                               "HALC config invalid: no arrays allowed and float not supported for bitmask");
      }
      else
      {
         if (orc_DpContent.GetArray())
         {
            if (ou32_Index < orc_DpContent.GetArraySize())
            {
               switch (orc_DpContent.GetType())
               {
               case C_OSCNodeDataPoolContent::eUINT8:
                  orc_DpContent.SetValueAU8Element(c_ApplyValue.GetValueU8(), ou32_Index);
                  break;
               case C_OSCNodeDataPoolContent::eUINT16:
                  orc_DpContent.SetValueAU16Element(c_ApplyValue.GetValueU16(), ou32_Index);
                  break;
               case C_OSCNodeDataPoolContent::eUINT32:
                  orc_DpContent.SetValueAU32Element(c_ApplyValue.GetValueU32(), ou32_Index);
                  break;
               case C_OSCNodeDataPoolContent::eUINT64:
                  orc_DpContent.SetValueAU64Element(c_ApplyValue.GetValueU64(), ou32_Index);
                  break;
               case C_OSCNodeDataPoolContent::eSINT8:
                  orc_DpContent.SetValueAS8Element(c_ApplyValue.GetValueS8(), ou32_Index);
                  break;
               case C_OSCNodeDataPoolContent::eSINT16:
                  orc_DpContent.SetValueAS16Element(c_ApplyValue.GetValueS16(), ou32_Index);
                  break;
               case C_OSCNodeDataPoolContent::eSINT32:
                  orc_DpContent.SetValueAS32Element(c_ApplyValue.GetValueS32(), ou32_Index);
                  break;
               case C_OSCNodeDataPoolContent::eSINT64:
                  orc_DpContent.SetValueAS64Element(c_ApplyValue.GetValueS64(), ou32_Index);
                  break;
               case C_OSCNodeDataPoolContent::eFLOAT32:
                  orc_DpContent.SetValueAF32Element(c_ApplyValue.GetValueF32(), ou32_Index);
                  break;
               case C_OSCNodeDataPoolContent::eFLOAT64:
                  orc_DpContent.SetValueAF64Element(c_ApplyValue.GetValueF64(), ou32_Index);
                  break;
               }
            }
            else
            {
               s32_Retval = C_CONFIG;
               osc_write_log_warning("HALC datapool generation",
                                     "Unexpected arraysize for datapool element");
            }
         }
         else
         {
            orc_DpContent = orc_HALCContent;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert to datapool and resize

   \param[in]      orc_HALCContent     HALC content
   \param[in]      ou32_NumChannels    Num channels
   \param[in,out]  orc_DpContent       Dp content

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_ConvertToDatapoolAndResize(const C_OSCHalcDefContent & orc_HALCContent,
                                                                const uint32 ou32_NumChannels,
                                                                C_OSCNodeDataPoolContent & orc_DpContent)
{
   sint32 s32_Retval = m_ConvertToDatapoolWithoutArray(orc_HALCContent, orc_DpContent);

   if (s32_Retval == C_NO_ERR)
   {
      if ((orc_HALCContent.GetArray()) ||
          ((orc_HALCContent.GetComplexType() == C_OSCHalcDefContent::eCT_BIT_MASK) &&
           ((orc_HALCContent.GetType() == C_OSCNodeDataPoolContent::eFLOAT32) ||
            (orc_HALCContent.GetType() == C_OSCNodeDataPoolContent::eFLOAT64))))
      {
         s32_Retval = C_CONFIG;
         osc_write_log_warning("HALC datapool generation",
                               "HALC config invalid: no arrays allowed and float not supported for bitmask");
      }
      else
      {
         switch (orc_HALCContent.GetComplexType())
         {
         case C_OSCHalcDefContent::eCT_ENUM:
         case C_OSCHalcDefContent::eCT_PLAIN:
            //Resize and adapt all values
            if (ou32_NumChannels > 1UL)
            {
               orc_DpContent.SetArray(true);
               orc_DpContent.SetArraySize(ou32_NumChannels);
               for (uint32 u32_It = 0UL; u32_It < ou32_NumChannels; ++u32_It)
               {
                  switch (orc_HALCContent.GetType())
                  {
                  case C_OSCNodeDataPoolContent::eUINT8:
                     orc_DpContent.SetValueAU8Element(orc_HALCContent.GetValueU8(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eUINT16:
                     orc_DpContent.SetValueAU16Element(orc_HALCContent.GetValueU16(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eUINT32:
                     orc_DpContent.SetValueAU32Element(orc_HALCContent.GetValueU32(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eUINT64:
                     orc_DpContent.SetValueAU64Element(orc_HALCContent.GetValueU64(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT8:
                     orc_DpContent.SetValueAS8Element(orc_HALCContent.GetValueS8(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT16:
                     orc_DpContent.SetValueAS16Element(orc_HALCContent.GetValueS16(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT32:
                     orc_DpContent.SetValueAS32Element(orc_HALCContent.GetValueS32(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT64:
                     orc_DpContent.SetValueAS64Element(orc_HALCContent.GetValueS64(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eFLOAT32:
                     orc_DpContent.SetValueAF32Element(orc_HALCContent.GetValueF32(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eFLOAT64:
                     orc_DpContent.SetValueAF64Element(orc_HALCContent.GetValueF64(), u32_It);
                     break;
                  }
               }
            }
            break;
         case C_OSCHalcDefContent::eCT_BIT_MASK:
            //Resize and adapt all values
            if (ou32_NumChannels > 1UL)
            {
               const C_OSCNodeDataPoolContent c_ApplyValue = orc_DpContent;
               orc_DpContent.SetArray(true);
               orc_DpContent.SetArraySize(ou32_NumChannels);
               for (uint32 u32_It = 0UL; u32_It < ou32_NumChannels; ++u32_It)
               {
                  switch (orc_HALCContent.GetType())
                  {
                  case C_OSCNodeDataPoolContent::eUINT8:
                     orc_DpContent.SetValueAU8Element(c_ApplyValue.GetValueU8(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eUINT16:
                     orc_DpContent.SetValueAU16Element(c_ApplyValue.GetValueU16(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eUINT32:
                     orc_DpContent.SetValueAU32Element(c_ApplyValue.GetValueU32(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eUINT64:
                     orc_DpContent.SetValueAU64Element(c_ApplyValue.GetValueU64(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT8:
                     orc_DpContent.SetValueAS8Element(c_ApplyValue.GetValueS8(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT16:
                     orc_DpContent.SetValueAS16Element(c_ApplyValue.GetValueS16(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT32:
                     orc_DpContent.SetValueAS32Element(c_ApplyValue.GetValueS32(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eSINT64:
                     orc_DpContent.SetValueAS64Element(c_ApplyValue.GetValueS64(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eFLOAT32:
                     orc_DpContent.SetValueAF32Element(c_ApplyValue.GetValueF32(), u32_It);
                     break;
                  case C_OSCNodeDataPoolContent::eFLOAT64:
                     orc_DpContent.SetValueAF64Element(c_ApplyValue.GetValueF64(), u32_It);
                     break;
                  }
               }
            }
            break;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init list

   \param[in,out]  orc_List         List
   \param[in]      orc_Name         Name
   \param[in]      oq_AddDataset    Add dataset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHALCMagicianGenerator::m_InitList(C_OSCNodeDataPoolList & orc_List, const stw_scl::C_SCLString & orc_Name,
                                            const bool oq_AddDataset)
{
   orc_List.c_DataSets.clear();
   orc_List.c_Elements.clear();
   orc_List.c_Name = orc_Name;

   //Datasets
   if (oq_AddDataset)
   {
      C_OSCNodeDataPoolDataSet c_Dataset;
      c_Dataset.c_Name = "Configuration_Values";
      orc_List.c_DataSets.push_back(c_Dataset);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clean up HALC datapools

   \param[in,out]  orc_Datapools    Datapools
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHALCMagicianGenerator::m_CleanUpHALCDatapools(std::vector<C_OSCNodeDataPool> & orc_Datapools)
{
   for (uint32 u32_ItDp = 0UL; u32_ItDp < orc_Datapools.size(); ++u32_ItDp)
   {
      C_OSCNodeDataPool & rc_Datapool = orc_Datapools[u32_ItDp];
      for (std::vector<C_OSCNodeDataPoolList>::iterator c_ItList = rc_Datapool.c_Lists.begin();
           c_ItList != rc_Datapool.c_Lists.end();)
      {
         if (c_ItList->c_Elements.size() > 0UL)
         {
            //Leave
            ++c_ItList;
         }
         else
         {
            //Discard
            c_ItList = rc_Datapool.c_Lists.erase(c_ItList);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Assign HALC datapools

   \param[in,out]  orc_Datapools    Datapools

   \return
   C_NO_ERR Datapool generated
   C_CONFIG Configuration invalid
   C_BUSY   No programmable application block found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_AssignHALCDatapools(std::vector<C_OSCNodeDataPool> & orc_Datapools) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->mpc_Node != NULL)
   {
      for (uint32 u32_ItDatablock = 0UL; u32_ItDatablock < this->mpc_Node->c_Applications.size(); ++u32_ItDatablock)
      {
         const C_OSCNodeApplication & rc_Application = this->mpc_Node->c_Applications[u32_ItDatablock];
         if (rc_Application.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
         {
            for (uint32 u32_ItDatapool = 0UL; u32_ItDatapool < orc_Datapools.size(); ++u32_ItDatapool)
            {
               C_OSCNodeDataPool & rc_Datapool = orc_Datapools[u32_ItDatapool];
               rc_Datapool.s32_RelatedDataBlockIndex = static_cast<sint32>(u32_ItDatablock);
            }
            //Stop after first found programmable application
            break;
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
      osc_write_log_warning("HALC datapool generation", "node not assigned");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set common defaults

   \param[in,out]  orc_Element   Element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHALCMagicianGenerator::m_SetCommonDefaults(C_OSCNodeDataPoolListElement & orc_Element)
{
   orc_Element.c_Unit = "";
   orc_Element.e_Access = C_OSCNodeDataPoolListElement::eACCESS_RO;
   orc_Element.f64_Factor = 1.0;
   orc_Element.f64_Offset = 0.0;
   orc_Element.q_DiagEventCall = false;
   orc_Element.q_NvmValueIsValid = false;
   orc_Element.q_NvMValueChanged = false;
}
