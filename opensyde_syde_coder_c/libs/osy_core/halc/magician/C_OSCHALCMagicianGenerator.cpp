//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC Magician: HALC datapool generator

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCHALCMagicianUtil.h"
#include "C_OSCHALCMagicianGenerator.h"

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
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::GenerateHALCDatapools(std::vector<C_OSCNodeDataPool> & orc_Datapools) const
{
   const uint16 u16_TimerId = osc_write_log_performance_start();

   sint32 s32_Retval = m_GenerateHALCDatapoolsDefinition(orc_Datapools);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = m_FillHALCDatapools(orc_Datapools);
      if (s32_Retval == C_NO_ERR)
      {
         //last step as the resulting structure is then unknown
         mh_CleanUpHALCDatapools(orc_Datapools);

         //After all variables are present
         s32_Retval = this->m_HandleCopies(orc_Datapools);

         if (s32_Retval == C_NO_ERR)
         {
            //After everything is finished
            s32_Retval = this->m_HandleNVM(orc_Datapools);
            if (s32_Retval == C_NO_ERR)
            {
               m_FillEmptySpaceHALCDatapools(orc_Datapools);
            }
         }
      }
   }

   osc_write_log_performance_stop(u16_TimerId, "Generation of HAL Datapools");

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
         const C_OSCHALCMagicianDatapoolListHandler c_DpHandlerLP(this->mpc_Node->c_HALCConfig,
                                                                  C_OSCHalcDefDomain::eVA_PARAM, false);
         const C_OSCHALCMagicianDatapoolListHandler c_DpHandlerLPSafe(this->mpc_Node->c_HALCConfig,
                                                                      C_OSCHalcDefDomain::eVA_PARAM, true);
         const bool q_BothNecessary = this->m_CheckTwoDp();
         const bool q_SafeDpNecessary = q_BothNecessary ||
                                        (this->mpc_Node->c_HALCConfig.e_SafetyMode ==
                                         C_OSCHalcDefBase::eONE_LEVEL_ALL_SAFE);
         const bool q_NonSafeDpNecessary = q_BothNecessary ||
                                           (this->mpc_Node->c_HALCConfig.e_SafetyMode ==
                                            C_OSCHalcDefBase::eONE_LEVEL_ALL_NON_SAFE);
         //Generate datapool
         C_OSCNodeDataPool c_HALCDpSafe;
         C_OSCNodeDataPoolList c_HALCListParamSafe;
         C_OSCNodeDataPoolList c_HALCListInputSafe;
         C_OSCNodeDataPoolList c_HALCListOutputSafe;
         C_OSCNodeDataPoolList c_HALCListStatusSafe;
         C_OSCNodeDataPool c_HALCDp;
         C_OSCNodeDataPoolList c_HALCListParam;
         C_OSCNodeDataPoolList c_HALCListInput;
         C_OSCNodeDataPoolList c_HALCListOutput;
         C_OSCNodeDataPoolList c_HALCListStatus;
         //Datapool config
         if (q_NonSafeDpNecessary)
         {
            c_HALCDp.e_Type = C_OSCNodeDataPool::eHALC;
            c_HALCDp.c_Name = C_OSCHALCMagicianUtil::h_GetDatapoolName(false, 0UL);
            this->m_HandleVersion(c_HALCDp);
            c_HALCDp.c_Comment = C_OSCHALCMagicianUtil::h_GetDatapoolComment(false, 0UL);
            c_HALCDp.q_IsSafety = false;
         }
         if (q_SafeDpNecessary)
         {
            c_HALCDpSafe.e_Type = C_OSCNodeDataPool::eHALC;
            c_HALCDpSafe.c_Name = C_OSCHALCMagicianUtil::h_GetDatapoolName(true, 0UL);
            this->m_HandleVersion(c_HALCDpSafe);
            c_HALCDpSafe.c_Comment = C_OSCHALCMagicianUtil::h_GetDatapoolComment(true, 0UL);
            c_HALCDpSafe.q_IsSafety = true;
         }
         //Lists
         if (q_NonSafeDpNecessary)
         {
            c_HALCDp.c_Lists.clear();
         }
         if (q_SafeDpNecessary)
         {
            c_HALCDpSafe.c_Lists.clear();
         }
         //Generate lists
         if (q_NonSafeDpNecessary)
         {
            C_OSCHALCMagicianGenerator::mh_InitList(c_HALCListParam,
                                                    C_OSCHALCMagicianUtil::h_GetListName(
                                                       C_OSCHalcDefDomain::eVA_PARAM), true);
            C_OSCHALCMagicianGenerator::mh_InitList(c_HALCListInput,
                                                    C_OSCHALCMagicianUtil::h_GetListName(C_OSCHalcDefDomain::eVA_INPUT));
            C_OSCHALCMagicianGenerator::mh_InitList(c_HALCListOutput,
                                                    C_OSCHALCMagicianUtil::h_GetListName(C_OSCHalcDefDomain::eVA_OUTPUT));
            C_OSCHALCMagicianGenerator::mh_InitList(c_HALCListStatus,
                                                    C_OSCHALCMagicianUtil::h_GetListName(C_OSCHalcDefDomain::eVA_STATUS));
         }
         if (q_SafeDpNecessary)
         {
            C_OSCHALCMagicianGenerator::mh_InitList(c_HALCListParamSafe,
                                                    C_OSCHALCMagicianUtil::h_GetListName(
                                                       C_OSCHalcDefDomain::eVA_PARAM), true);
            C_OSCHALCMagicianGenerator::mh_InitList(c_HALCListInputSafe,
                                                    C_OSCHALCMagicianUtil::h_GetListName(C_OSCHalcDefDomain::eVA_INPUT));
            C_OSCHALCMagicianGenerator::mh_InitList(c_HALCListOutputSafe,
                                                    C_OSCHALCMagicianUtil::h_GetListName(C_OSCHalcDefDomain::eVA_OUTPUT));
            C_OSCHALCMagicianGenerator::mh_InitList(c_HALCListStatusSafe,
                                                    C_OSCHALCMagicianUtil::h_GetListName(C_OSCHalcDefDomain::eVA_STATUS));
         }
         //Generate variables
         for (uint32 u32_It = 0UL; u32_It < this->mpc_Node->c_HALCConfig.GetDomainSize(); ++u32_It)
         {
            const C_OSCHalcConfigDomain * const pc_Config =
               this->mpc_Node->c_HALCConfig.GetDomainConfigDataConst(u32_It);
            tgl_assert(pc_Config != NULL);
            if (pc_Config != NULL)
            {
               if (q_NonSafeDpNecessary)
               {
                  s32_Retval = C_OSCHALCMagicianGenerator::mh_GenerateVariablesForDomain(*pc_Config, c_HALCListParam,
                                                                                         c_HALCListInput,
                                                                                         c_HALCListOutput,
                                                                                         c_HALCListStatus, false,
                                                                                         c_DpHandlerLP);
               }
               if (s32_Retval == C_NO_ERR)
               {
                  if (q_SafeDpNecessary)
                  {
                     s32_Retval = C_OSCHALCMagicianGenerator::mh_GenerateVariablesForDomain(*pc_Config,
                                                                                            c_HALCListParamSafe,
                                                                                            c_HALCListInputSafe,
                                                                                            c_HALCListOutputSafe,
                                                                                            c_HALCListStatusSafe, true,
                                                                                            c_DpHandlerLPSafe);
                  }
               }
            }
         }
         if (s32_Retval == C_NO_ERR)
         {
            if (q_NonSafeDpNecessary)
            {
               c_HALCDp.c_Lists.push_back(c_HALCListParam);
               c_HALCDp.c_Lists.push_back(c_HALCListInput);
               c_HALCDp.c_Lists.push_back(c_HALCListOutput);
               c_HALCDp.c_Lists.push_back(c_HALCListStatus);
               orc_Datapools.push_back(c_HALCDp);
            }
            if (q_SafeDpNecessary)
            {
               c_HALCDpSafe.c_Lists.push_back(c_HALCListParamSafe);
               c_HALCDpSafe.c_Lists.push_back(c_HALCListInputSafe);
               c_HALCDpSafe.c_Lists.push_back(c_HALCListOutputSafe);
               c_HALCDpSafe.c_Lists.push_back(c_HALCListStatusSafe);
               orc_Datapools.push_back(c_HALCDpSafe);
            }
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

   if (this->mpc_Node != NULL)
   {
      if (this->m_CheckTwoDp())
      {
         if  (orc_Datapools.size() == 2UL)
         {
            C_OSCNodeDataPool & rc_DatapoolNonSafe = orc_Datapools[0UL];
            C_OSCNodeDataPool & rc_DatapoolSafe = orc_Datapools[1UL];
            if ((rc_DatapoolNonSafe.c_Lists.size() == 4UL) && (rc_DatapoolSafe.c_Lists.size() == 4UL))
            {
               C_OSCNodeDataPoolList & rc_ListNonSafe = rc_DatapoolNonSafe.c_Lists[0UL];
               C_OSCNodeDataPoolList & rc_ListSafe = rc_DatapoolSafe.c_Lists[0UL];
               const C_OSCHALCMagicianDatapoolListHandler c_Handler(this->mpc_Node->c_HALCConfig,
                                                                    C_OSCHalcDefDomain::eVA_PARAM, false);
               const C_OSCHALCMagicianDatapoolListHandler c_HandlerSafe(this->mpc_Node->c_HALCConfig,
                                                                        C_OSCHalcDefDomain::eVA_PARAM, true);
               for (uint32 u32_ItDomain = 0UL;
                    (u32_ItDomain < this->mpc_Node->c_HALCConfig.GetDomainSize()) && (s32_Retval == C_NO_ERR);
                    ++u32_ItDomain)
               {
                  const C_OSCHalcConfigDomain * const pc_Config =
                     this->mpc_Node->c_HALCConfig.GetDomainConfigDataConst(u32_ItDomain);
                  tgl_assert(pc_Config != NULL);
                  if (pc_Config != NULL)
                  {
                     this->m_FillHALCDatapoolsDomain(rc_ListNonSafe, *pc_Config, c_Handler, u32_ItDomain);
                     this->m_FillHALCDatapoolsDomain(rc_ListSafe, *pc_Config, c_HandlerSafe, u32_ItDomain);
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
      }
      else
      {
         if  (orc_Datapools.size() == 1UL)
         {
            C_OSCNodeDataPool & rc_SingleDatapool = orc_Datapools[0UL];
            if (rc_SingleDatapool.c_Lists.size() == 4UL)
            {
               C_OSCNodeDataPoolList & rc_List = rc_SingleDatapool.c_Lists[0UL];
               const C_OSCHALCMagicianDatapoolListHandler c_Handler(this->mpc_Node->c_HALCConfig,
                                                                    C_OSCHalcDefDomain::eVA_PARAM,
                                                                    this->mpc_Node->c_HALCConfig.e_SafetyMode ==
                                                                    C_OSCHalcDefBase::eONE_LEVEL_ALL_SAFE);
               for (uint32 u32_ItDomain = 0UL;
                    (u32_ItDomain < this->mpc_Node->c_HALCConfig.GetDomainSize()) && (s32_Retval == C_NO_ERR);
                    ++u32_ItDomain)
               {
                  const C_OSCHalcConfigDomain * const pc_Config =
                     this->mpc_Node->c_HALCConfig.GetDomainConfigDataConst(u32_ItDomain);
                  tgl_assert(pc_Config != NULL);
                  if (pc_Config != NULL)
                  {
                     this->m_FillHALCDatapoolsDomain(rc_List, *pc_Config, c_Handler, u32_ItDomain);
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
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill HALC datapools domain

   \param[in,out]  orc_List         List
   \param[in]      orc_Domain       Domain
   \param[in]      orc_Handler      Handler
   \param[in]      ou32_ItDomain    Iterator domain

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_FillHALCDatapoolsDomain(C_OSCNodeDataPoolList & orc_List,
                                                             const C_OSCHalcConfigDomain & orc_Domain,
                                                             const C_OSCHALCMagicianDatapoolListHandler & orc_Handler,
                                                             const uint32 ou32_ItDomain) const
{
   sint32 s32_Retval = C_NO_ERR;
   //Count
   const uint32 u32_CountRelevant = orc_Handler.CountRelevantItems(
      orc_Domain.c_ChannelConfigs, orc_Domain.c_DomainConfig);

   if (u32_CountRelevant > 0UL)
   {
      std::vector<uint32> c_RelevantParameters;
      uint32 u32_ItRelevantChannel = 0UL;

      if (orc_Domain.c_ChannelConfigs.size() == 0UL)
      {
         //Fill domain values
         if (orc_Handler.CheckChanPresent(orc_Domain.c_DomainConfig))
         {
            if (orc_Domain.GetRelevantIndicesForSelectedUseCase(0UL, false, &c_RelevantParameters, NULL,
                                                                NULL, NULL) == C_NO_ERR)
            {
               s32_Retval = this->m_FillHALCDatapoolsChannel(
                  orc_List, orc_Domain.c_DomainConfig, orc_Handler,
                  ou32_ItDomain,
                  0UL, c_RelevantParameters,
                  orc_Domain.c_DomainValues.c_Parameters);
            }
            else
            {
               s32_Retval = C_CONFIG;
               osc_write_log_warning("HALC datapool generation", "unexpected error: failed relevancy check");
            }

            if (s32_Retval == C_NO_ERR)
            {
               if (orc_Handler.CheckSafetyFlagVariableNecessary())
               {
                  //Fill Use-case
                  s32_Retval = C_OSCHALCMagicianGenerator::m_FillHALCDatapoolsSafetyFlag(
                     orc_List, orc_Handler, ou32_ItDomain, u32_ItRelevantChannel,
                     orc_Domain.c_DomainConfig.q_SafetyRelevant);
               }
            }
            //Check assumptions, assert if not filled but required/generated
            tgl_assert(orc_Handler.CheckChanNumVariableNecessary(orc_Domain) == false);
            tgl_assert(orc_Handler.CheckUseCaseVariableNecessary(orc_Domain) == false);
         }
      }

      //Fill other values
      for (uint32 u32_ItChannel = 0UL;
           (u32_ItChannel < orc_Domain.c_ChannelConfigs.size()) && (s32_Retval == C_NO_ERR); ++u32_ItChannel)
      {
         const C_OSCHalcConfigChannel & rc_Channel = orc_Domain.c_ChannelConfigs[u32_ItChannel];
         if (orc_Handler.CheckChanPresent(rc_Channel))
         {
            //Fill channel
            if (orc_Handler.CheckChanNumVariableNecessary(orc_Domain))
            {
               s32_Retval = C_OSCHALCMagicianGenerator::m_FillHALCDatapoolsChanNum(
                  orc_List, orc_Handler, ou32_ItDomain, u32_ItRelevantChannel, u32_ItChannel);
            }

            if (s32_Retval == C_NO_ERR)
            {
               if (orc_Handler.CheckSafetyFlagVariableNecessary())
               {
                  //Fill Use-case
                  s32_Retval = C_OSCHALCMagicianGenerator::m_FillHALCDatapoolsSafetyFlag(
                     orc_List, orc_Handler, ou32_ItDomain, u32_ItRelevantChannel, rc_Channel.q_SafetyRelevant);
               }
            }

            if (s32_Retval == C_NO_ERR)
            {
               if (orc_Handler.CheckUseCaseVariableNecessary(orc_Domain))
               {
                  //Get use-case value from index:
                  //use case index out of range of known use cases would mean we'd have a configuration error
                  tgl_assert(rc_Channel.u32_UseCaseIndex < orc_Domain.c_ChannelUseCases.size());

                  const C_OSCNodeDataPoolContent & rc_UseCaseContent =
                     orc_Domain.c_ChannelUseCases[rc_Channel.u32_UseCaseIndex].c_Value;

                  //definition: only uint8 supported for use case
                  tgl_assert(rc_UseCaseContent.GetType() == C_OSCNodeDataPoolContent::eUINT8);

                  //Fill Use-case
                  s32_Retval = C_OSCHALCMagicianGenerator::m_FillHALCDatapoolsUseCase(
                     orc_List, orc_Handler, ou32_ItDomain, u32_ItRelevantChannel,
                     rc_UseCaseContent.GetValueU8());
               }
            }

            if (s32_Retval == C_NO_ERR)
            {
               if (orc_Domain.GetRelevantIndicesForSelectedUseCase(u32_ItChannel, true, &c_RelevantParameters, NULL,
                                                                   NULL, NULL) == C_NO_ERR)
               {
                  s32_Retval = this->m_FillHALCDatapoolsChannel(
                     orc_List, rc_Channel, orc_Handler, ou32_ItDomain,
                     u32_ItRelevantChannel, c_RelevantParameters,
                     orc_Domain.c_ChannelValues.c_Parameters);
               }
               else
               {
                  s32_Retval = C_CONFIG;
                  osc_write_log_warning("HALC datapool generation", "unexpected error: failed relevancy check");
               }
            }
            //Iterate
            ++u32_ItRelevantChannel;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill HALC datapools channel number

   \param[in,out]  orc_List            List
   \param[in]      orc_Handler         Handler
   \param[in]      ou32_ItDomain       Iterator domain
   \param[in]      ou32_RelevantIndex  Relevant index
   \param[in]      ou32_ChanNum        Channel number

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_FillHALCDatapoolsChanNum(C_OSCNodeDataPoolList & orc_List,
                                                              const C_OSCHALCMagicianDatapoolListHandler & orc_Handler,
                                                              const uint32 ou32_ItDomain,
                                                              const uint32 ou32_RelevantIndex,
                                                              const uint32 ou32_ChanNum) const
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCNodeDataPoolListElement * const pc_ChanNumElement = orc_Handler.GetChanNumListElement(
      ou32_ItDomain,
      orc_List);

   if (pc_ChanNumElement != NULL)
   {
      if (pc_ChanNumElement->c_DataSetValues.size() > 0UL)
      {
         C_OSCNodeDataPoolContent & rc_Content = pc_ChanNumElement->c_DataSetValues[0UL];
         tgl_assert(rc_Content.GetType() == C_OSCNodeDataPoolContent::eUINT16);
         if (pc_ChanNumElement->GetArray())
         {
            rc_Content.SetValueAU16Element(static_cast<uint16>(ou32_ChanNum),
                                           ou32_RelevantIndex);
         }
         else
         {
            rc_Content.SetValueU16(static_cast<uint16>(ou32_ChanNum));
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill HALC datapools safety flag

   \param[in,out]  orc_List               List
   \param[in]      orc_Handler            Handler
   \param[in]      ou32_ItDomain          Iterator domain
   \param[in]      ou32_RelevantIndex     Relevant index
   \param[in]      oq_IsSafetyRelevant    Is safety relevant

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_FillHALCDatapoolsSafetyFlag(C_OSCNodeDataPoolList & orc_List,
                                                                 const C_OSCHALCMagicianDatapoolListHandler & orc_Handler, const uint32 ou32_ItDomain, const uint32 ou32_RelevantIndex,
                                                                 const bool oq_IsSafetyRelevant) const
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCNodeDataPoolListElement * const pc_ChanNumElement = orc_Handler.GetSafetyFlagListElement(
      ou32_ItDomain,
      orc_List);

   if (pc_ChanNumElement != NULL)
   {
      if (pc_ChanNumElement->c_DataSetValues.size() > 0UL)
      {
         const uint8 u8_SafetyVal = oq_IsSafetyRelevant ? 1U : 0U;
         C_OSCNodeDataPoolContent & rc_Content = pc_ChanNumElement->c_DataSetValues[0UL];
         tgl_assert(rc_Content.GetType() == C_OSCNodeDataPoolContent::eUINT8);
         if (pc_ChanNumElement->GetArray())
         {
            rc_Content.SetValueAU8Element(u8_SafetyVal,
                                          ou32_RelevantIndex);
         }
         else
         {
            rc_Content.SetValueU8(u8_SafetyVal);
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill HALC datapools use case

   \param[in,out]  orc_List            List
   \param[in]      orc_Handler         Handler
   \param[in]      ou32_ItDomain       Iterator domain
   \param[in]      ou32_RelevantIndex  Relevant index
   \param[in]      ou8_UseCaseValue    Use case value to fill into data set

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_FillHALCDatapoolsUseCase(C_OSCNodeDataPoolList & orc_List,
                                                              const C_OSCHALCMagicianDatapoolListHandler & orc_Handler,
                                                              const uint32 ou32_ItDomain,
                                                              const uint32 ou32_RelevantIndex,
                                                              const uint8 ou8_UseCaseValue) const
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCNodeDataPoolListElement * const pc_UseCaseElement = orc_Handler.GetUseCaseListElement(ou32_ItDomain, orc_List);

   if (pc_UseCaseElement != NULL)
   {
      if (pc_UseCaseElement->c_DataSetValues.size() > 0UL)
      {
         C_OSCNodeDataPoolContent & rc_Content = pc_UseCaseElement->c_DataSetValues[0UL];
         tgl_assert(rc_Content.GetType() == C_OSCNodeDataPoolContent::eUINT8);

         if (pc_UseCaseElement->GetArray())
         {
            rc_Content.SetValueAU8Element(ou8_UseCaseValue, ou32_RelevantIndex);
         }
         else
         {
            rc_Content.SetValueU8(ou8_UseCaseValue);
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill HALC datapools channel

   \param[in,out]  orc_List                  List
   \param[in]      orc_Channel               Channel
   \param[in]      orc_Handler               Handler
   \param[in]      ou32_ItDomain             Iterator domain
   \param[in]      ou32_ItRelevantChannel    Iterator relevant channel
   \param[in]      orc_RelevantParameters    Relevant parameters
   \param[in]      orc_DefParam              Parameter definition

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_FillHALCDatapoolsChannel(C_OSCNodeDataPoolList & orc_List,
                                                              const C_OSCHalcConfigChannel & orc_Channel,
                                                              const C_OSCHALCMagicianDatapoolListHandler & orc_Handler,
                                                              const uint32 ou32_ItDomain,
                                                              const uint32 ou32_ItRelevantChannel,
                                                              const std::vector<uint32> & orc_RelevantParameters,
                                                              const std::vector<C_OSCHalcDefStruct> & orc_DefParam)
const
{
   sint32 s32_Retval = C_NO_ERR;

   tgl_assert(orc_DefParam.size() == orc_Channel.c_Parameters.size());
   if (orc_DefParam.size() == orc_Channel.c_Parameters.size())
   {
      for (uint32 u32_ItParam = 0UL;
           (u32_ItParam < orc_Channel.c_Parameters.size()) && (s32_Retval == C_NO_ERR); ++u32_ItParam)
      {
         const C_OSCHalcConfigParameterStruct & rc_Param = orc_Channel.c_Parameters[u32_ItParam];
         const C_OSCHalcDefStruct & rc_ParamDef = orc_DefParam[u32_ItParam];
         bool q_IsRelevant = false;
         //Check relevancy for use case
         for (uint32 u32_ItRel = 0UL; u32_ItRel < orc_RelevantParameters.size(); ++u32_ItRel)
         {
            if (orc_RelevantParameters[u32_ItRel] == u32_ItParam)
            {
               q_IsRelevant = true;
               break;
            }
         }
         //Fill
         tgl_assert(rc_ParamDef.c_StructElements.size() == rc_Param.c_ParameterElements.size());
         if (rc_Param.c_ParameterElements.size() > 0UL)
         {
            for (uint32 u32_ItElem = 0UL;
                 (u32_ItElem < rc_Param.c_ParameterElements.size()) && (s32_Retval == C_NO_ERR);
                 ++u32_ItElem)
            {
               C_OSCNodeDataPoolListElement * const pc_Element = orc_Handler.GetListElement(
                  ou32_ItDomain,
                  u32_ItParam,
                  u32_ItElem,
                  orc_List);
               //Use user value
               if (q_IsRelevant)
               {
                  const C_OSCHalcConfigParameter & rc_Elem = rc_Param.c_ParameterElements[u32_ItElem];
                  s32_Retval =
                     mh_FillHALCElement(pc_Element,
                                        rc_Elem.c_Value,
                                        ou32_ItRelevantChannel);
               }
               else
               {
                  const C_OSCHalcDefElement & rc_DefElem = rc_ParamDef.c_StructElements[u32_ItElem];
                  //Use initial value
                  s32_Retval =
                     mh_FillHALCElement(pc_Element,
                                        rc_DefElem.c_InitialValue,
                                        ou32_ItRelevantChannel);
               }
            }
         }
         else
         {
            C_OSCNodeDataPoolListElement * const pc_Element = orc_Handler.GetListElement(
               ou32_ItDomain,
               u32_ItParam,
               0UL,
               orc_List);
            //Use user value
            if (q_IsRelevant)
            {
               s32_Retval = mh_FillHALCElement(pc_Element, rc_Param.c_Value, ou32_ItRelevantChannel);
            }
            else
            {
               //Use initial value
               s32_Retval = mh_FillHALCElement(pc_Element, rc_ParamDef.c_InitialValue, ou32_ItRelevantChannel);
            }
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
      osc_write_log_warning("HALC datapool generation", "unexpected error: invalid parameter sizes");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill HALC element

   \param[in,out]  opc_Element         Element
   \param[in]      orc_Value           Value
   \param[in]      ou32_ChannelIndex   Channel index

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::mh_FillHALCElement(C_OSCNodeDataPoolListElement * const opc_Element,
                                                      const C_OSCHalcDefContent & orc_Value,
                                                      const uint32 ou32_ChannelIndex)
{
   sint32 s32_Retval;

   if (opc_Element != NULL)
   {
      if (opc_Element->c_DataSetValues.size() >= 1UL)
      {
         s32_Retval =
            mh_ConvertToDatapoolAndAssign(orc_Value, opc_Element->c_DataSetValues[0UL],
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
/*! \brief  Generate variables for domain

   \param[in]      orc_Domain          Domain
   \param[in,out]  orc_HALCListParam   HALC list param
   \param[in,out]  orc_HALCListInput   HALC list input
   \param[in,out]  orc_HALCListOutput  HALC list output
   \param[in,out]  orc_HALCListStatus  HALC list status
   \param[in]      oq_IsSafe           Is safe
   \param[in]      orc_DpHandler       Datapool handler

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::mh_GenerateVariablesForDomain(const C_OSCHalcConfigDomain & orc_Domain,
                                                                 C_OSCNodeDataPoolList & orc_HALCListParam,
                                                                 C_OSCNodeDataPoolList & orc_HALCListInput,
                                                                 C_OSCNodeDataPoolList & orc_HALCListOutput,
                                                                 C_OSCNodeDataPoolList & orc_HALCListStatus,
                                                                 const bool oq_IsSafe,
                                                                 const C_OSCHALCMagicianDatapoolListHandler & orc_DpHandler)
{
   sint32 s32_Retval = C_NO_ERR;
   //Count
   const uint32 u32_CountRelevant = orc_DpHandler.CountRelevantItems(
      orc_Domain.c_ChannelConfigs, orc_Domain.c_DomainConfig);

   if (u32_CountRelevant > 0UL)
   {
      //Channels
      if (orc_DpHandler.CheckChanNumVariableNecessary(orc_Domain))
      {
         const C_OSCNodeDataPoolListElement c_Tmp =
            C_OSCHALCMagicianUtil::h_GetChanNumVariable(orc_Domain.c_SingularName, oq_IsSafe, u32_CountRelevant, true);
         orc_HALCListParam.c_Elements.push_back(c_Tmp);
      }
      //Safety flag
      if (orc_DpHandler.CheckSafetyFlagVariableNecessary())
      {
         const C_OSCNodeDataPoolListElement c_Tmp =
            C_OSCHALCMagicianUtil::h_GetSafetyFlagVariable(orc_Domain.c_SingularName, oq_IsSafe, u32_CountRelevant,
                                                           true);
         orc_HALCListParam.c_Elements.push_back(c_Tmp);
      }
      //Use cases
      if (orc_DpHandler.CheckUseCaseVariableNecessary(orc_Domain))
      {
         const C_OSCNodeDataPoolListElement c_Tmp =
            C_OSCHALCMagicianUtil::h_GetUseCaseVariable(orc_Domain.c_SingularName, oq_IsSafe, u32_CountRelevant, true);
         orc_HALCListParam.c_Elements.push_back(c_Tmp);
      }
      //Content
      if (orc_Domain.c_ChannelConfigs.size() == 0UL)
      {
         s32_Retval = mh_GenerateVariablesForVector(orc_Domain.c_DomainValues.c_Parameters,
                                                    orc_Domain.c_SingularName, oq_IsSafe,
                                                    u32_CountRelevant, true,
                                                    orc_HALCListParam);
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = mh_GenerateVariablesForVector(orc_Domain.c_DomainValues.c_InputValues,
                                                       orc_Domain.c_SingularName, oq_IsSafe,
                                                       u32_CountRelevant, false,
                                                       orc_HALCListInput);
         }
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = mh_GenerateVariablesForVector(orc_Domain.c_DomainValues.c_OutputValues,
                                                       orc_Domain.c_SingularName, oq_IsSafe,
                                                       u32_CountRelevant, false,
                                                       orc_HALCListOutput);
         }
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = mh_GenerateVariablesForVector(orc_Domain.c_DomainValues.c_StatusValues,
                                                       orc_Domain.c_SingularName, oq_IsSafe,
                                                       u32_CountRelevant, false,
                                                       orc_HALCListStatus);
         }
      }
      else
      {
         s32_Retval = mh_GenerateVariablesForVector(orc_Domain.c_ChannelValues.c_Parameters,
                                                    orc_Domain.c_SingularName, oq_IsSafe,
                                                    u32_CountRelevant, true,
                                                    orc_HALCListParam);
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = mh_GenerateVariablesForVector(orc_Domain.c_ChannelValues.c_InputValues,
                                                       orc_Domain.c_SingularName, oq_IsSafe,
                                                       u32_CountRelevant, false,
                                                       orc_HALCListInput);
         }
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = mh_GenerateVariablesForVector(orc_Domain.c_ChannelValues.c_OutputValues,
                                                       orc_Domain.c_SingularName, oq_IsSafe,
                                                       u32_CountRelevant, false,
                                                       orc_HALCListOutput);
         }
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = mh_GenerateVariablesForVector(orc_Domain.c_ChannelValues.c_StatusValues,
                                                       orc_Domain.c_SingularName, oq_IsSafe,
                                                       u32_CountRelevant, false,
                                                       orc_HALCListStatus);
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Generate variables for vector

   \param[in]      orc_Definition            Definition
   \param[in]      orc_DomainSingularName    Domain singular name
   \param[in]      oq_IsSafe                 Is safe
   \param[in]      ou32_NumChannels          Num channels
   \param[in]      oq_AddDataset             Add dataset
   \param[in,out]  orc_List                  List

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::mh_GenerateVariablesForVector(const std::vector<C_OSCHalcDefStruct> & orc_Definition,
                                                                 const stw_scl::C_SCLString & orc_DomainSingularName,
                                                                 const bool oq_IsSafe, const uint32 ou32_NumChannels,
                                                                 const bool oq_AddDataset,
                                                                 C_OSCNodeDataPoolList & orc_List)
{
   sint32 s32_Retval = C_NO_ERR;

   for (uint32 u32_ItStruct = 0UL; (u32_ItStruct < orc_Definition.size()) && (s32_Retval == C_NO_ERR); ++u32_ItStruct)
   {
      s32_Retval = C_OSCHALCMagicianGenerator::mh_GenerateVariablesForVectorElement(orc_Definition, u32_ItStruct,
                                                                                    orc_DomainSingularName, oq_IsSafe,
                                                                                    ou32_NumChannels, oq_AddDataset,
                                                                                    orc_List);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Generate variables for vector element

   \param[in]      orc_Definition               Definition
   \param[in]      ou32_DefinitionElementIndex  Definition element index
   \param[in]      orc_DomainSingularName       Domain singular name
   \param[in]      oq_IsSafe                    Is safe
   \param[in]      ou32_NumChannels             Num channels
   \param[in]      oq_AddDataset                Add dataset
   \param[in,out]  orc_List                     List

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::mh_GenerateVariablesForVectorElement(
   const std::vector<C_OSCHalcDefStruct> & orc_Definition, const uint32 ou32_DefinitionElementIndex,
   const stw_scl::C_SCLString & orc_DomainSingularName,   const bool oq_IsSafe, const uint32 ou32_NumChannels,
   const bool oq_AddDataset, C_OSCNodeDataPoolList & orc_List)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DefinitionElementIndex < orc_Definition.size())
   {
      const C_OSCHalcDefStruct & rc_Struct = orc_Definition[ou32_DefinitionElementIndex];
      if (rc_Struct.c_StructElements.size() > 0UL)
      {
         for (uint32 u32_ItElem = 0UL; (u32_ItElem < rc_Struct.c_StructElements.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItElem)
         {
            s32_Retval = mh_AddVariableToList(rc_Struct.c_StructElements[u32_ItElem], orc_Definition,
                                              ou32_DefinitionElementIndex, u32_ItElem, orc_DomainSingularName,
                                              oq_IsSafe, ou32_NumChannels, oq_AddDataset, orc_List);
         }
      }
      else
      {
         s32_Retval =
            mh_AddVariableToList(rc_Struct, orc_Definition, ou32_DefinitionElementIndex, 0UL, orc_DomainSingularName,
                                 oq_IsSafe, ou32_NumChannels, oq_AddDataset, orc_List);
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
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
   \param[in]      oq_IsSafe                    Is safe
   \param[in]      ou32_NumChannels             Num channels
   \param[in]      oq_AddDataset                Add dataset
   \param[in,out]  orc_List                     List

   \return
   C_NO_ERR Datapool definition generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::mh_AddVariableToList(const C_OSCHalcDefElement & orc_Definition,
                                                        const std::vector<C_OSCHalcDefStruct> & orc_DefinitionArray,
                                                        const uint32 ou32_ParameterIndexStruct,
                                                        const uint32 ou32_ParameterIndexElement,
                                                        const stw_scl::C_SCLString & orc_DomainSingularName,
                                                        const bool oq_IsSafe, const uint32 ou32_NumChannels,
                                                        const bool oq_AddDataset, C_OSCNodeDataPoolList & orc_List)
{
   sint32 s32_Retval;
   C_OSCNodeDataPoolListElement c_Element;

   c_Element.c_DataSetValues.clear();

   c_Element.c_Comment = orc_Definition.c_Comment;

   //Defined defaults
   C_OSCHALCMagicianUtil::h_SetCommonDpElementDefaults(c_Element);

   // Access
   c_Element.e_Access =
      oq_IsSafe ? C_OSCNodeDataPoolListElement::eACCESS_RO : C_OSCNodeDataPoolListElement::eACCESS_RW;

   s32_Retval = mh_ConvertToDatapoolAndResize(orc_Definition.c_MinValue, ou32_NumChannels, c_Element.c_MinValue);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_ConvertToDatapoolAndResize(orc_Definition.c_MaxValue, ou32_NumChannels, c_Element.c_MaxValue);
   }
   if (s32_Retval == C_NO_ERR)
   {
      //Initialize these
      c_Element.c_Value = c_Element.c_MinValue;
      c_Element.c_NvmValue = c_Element.c_MinValue;
      if (oq_AddDataset == true)
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
sint32 C_OSCHALCMagicianGenerator::mh_ConvertToDatapoolWithoutArray(const C_OSCHalcDefContent & orc_HALCContent,
                                                                    C_OSCNodeDataPoolContent & orc_DpContent)
{
   sint32 s32_Retval = C_NO_ERR;

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
sint32 C_OSCHALCMagicianGenerator::mh_ConvertToDatapoolAndAssign(const C_OSCHalcDefContent & orc_HALCContent,
                                                                 C_OSCNodeDataPoolContent & orc_DpContent,
                                                                 const uint32 ou32_Index)
{
   C_OSCNodeDataPoolContent c_ApplyValue;
   sint32 s32_Retval = mh_ConvertToDatapoolWithoutArray(orc_HALCContent, c_ApplyValue);

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
               tgl_assert(c_ApplyValue.GetArray() == false);
               tgl_assert(c_ApplyValue.GetType() == orc_DpContent.GetType());
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
               default:
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
sint32 C_OSCHALCMagicianGenerator::mh_ConvertToDatapoolAndResize(const C_OSCHalcDefContent & orc_HALCContent,
                                                                 const uint32 ou32_NumChannels,
                                                                 C_OSCNodeDataPoolContent & orc_DpContent)
{
   sint32 s32_Retval = mh_ConvertToDatapoolWithoutArray(orc_HALCContent, orc_DpContent);

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
                  tgl_assert(orc_HALCContent.GetArray() == false);
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
                  default:
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
                  tgl_assert(c_ApplyValue.GetArray() == false);
                  switch (c_ApplyValue.GetType())
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
                  default:
                     break;
                  }
               }
            }
            break;
         default:
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
void C_OSCHALCMagicianGenerator::mh_InitList(C_OSCNodeDataPoolList & orc_List, const stw_scl::C_SCLString & orc_Name,
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
void C_OSCHALCMagicianGenerator::mh_CleanUpHALCDatapools(std::vector<C_OSCNodeDataPool> & orc_Datapools)
{
   //Unused elements
   for (std::vector<C_OSCNodeDataPool>::iterator c_ItDp = orc_Datapools.begin(); c_ItDp != orc_Datapools.end();
        ++c_ItDp)
   {
      for (std::vector<C_OSCNodeDataPoolList>::iterator c_ItList = c_ItDp->c_Lists.begin();
           c_ItList != c_ItDp->c_Lists.end(); ++c_ItList)
      {
         if (c_ItList->c_Elements.size() == 0UL)
         {
            C_OSCNodeDataPoolListElement c_Empty;
            c_Empty.c_Name = "UnusedElement";
            c_Empty.c_Comment = "Empty data element to ensure valid list even if there are no exported data elements";
            c_Empty.SetArray(false);
            c_Empty.SetType(C_OSCNodeDataPoolContent::eUINT8);
            c_Empty.c_MinValue.SetValueU8(0U);
            c_Empty.c_MaxValue.SetValueU8(0U);
            c_Empty.c_NvmValue.SetValueU8(0U);
            c_Empty.c_Value.SetValueU8(0U);
            c_Empty.e_Access =
               c_ItDp->q_IsSafety ? C_OSCNodeDataPoolListElement::eACCESS_RO : C_OSCNodeDataPoolListElement::eACCESS_RW;
            for (uint32 u32_ItDataSet = 0UL; u32_ItDataSet < c_ItList->c_DataSets.size(); ++u32_ItDataSet)
            {
               c_Empty.c_DataSetValues.push_back(c_Empty.c_Value);
            }
            c_ItList->c_Elements.push_back(c_Empty);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill empty space HALC datapools

   \param[in,out]  orc_Datapools    Datapools
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHALCMagicianGenerator::m_FillEmptySpaceHALCDatapools(std::vector<C_OSCNodeDataPool> & orc_Datapools) const
{
   if (this->mpc_Node->c_HALCConfig.q_NvMBasedConfig)
   {
      for (std::vector<C_OSCNodeDataPool>::iterator c_ItDp = orc_Datapools.begin(); c_ItDp != orc_Datapools.end();
           ++c_ItDp)
      {
         for (std::vector<C_OSCNodeDataPoolList>::iterator c_ItList = c_ItDp->c_Lists.begin();
              c_ItList != c_ItDp->c_Lists.end(); ++c_ItList)
         {
            //CRC
            uint32 u32_ProjectedSize = 2UL;
            for (std::vector<C_OSCNodeDataPoolListElement>::const_iterator c_ItEl = c_ItList->c_Elements.begin();
                 c_ItEl != c_ItList->c_Elements.end(); ++c_ItEl)
            {
               u32_ProjectedSize += c_ItEl->GetSizeByte();
            }
            if (u32_ProjectedSize < c_ItList->u32_NvMSize)
            {
               const uint32 u32_Diff = c_ItList->u32_NvMSize - u32_ProjectedSize;
               C_OSCNodeDataPoolListElement c_Reserved;
               c_Reserved.c_Name = "Reserved";
               c_Reserved.c_Comment =
                  "Reserved data element to ensure empty space is set to zero";
               c_Reserved.e_Access =
                  c_ItDp->q_IsSafety ? C_OSCNodeDataPoolListElement::eACCESS_RO : C_OSCNodeDataPoolListElement::
                  eACCESS_RW;
               if (u32_Diff == 1UL)
               {
                  c_Reserved.SetArray(false);
                  c_Reserved.SetType(C_OSCNodeDataPoolContent::eUINT8);
                  c_Reserved.c_MinValue.SetValueU8(0U);
                  c_Reserved.c_MaxValue.SetValueU8(0U);
                  c_Reserved.c_NvmValue.SetValueU8(0U);
                  c_Reserved.c_Value.SetValueU8(0U);
                  for (uint32 u32_ItDataSet = 0UL; u32_ItDataSet < c_ItList->c_DataSets.size(); ++u32_ItDataSet)
                  {
                     c_Reserved.c_DataSetValues.push_back(c_Reserved.c_Value);
                  }
               }
               else
               {
                  c_Reserved.SetArray(true);
                  c_Reserved.SetType(C_OSCNodeDataPoolContent::eUINT8);
                  c_Reserved.SetArraySize(u32_Diff);
                  for (uint32 u32_ItAr = 0UL; u32_ItAr < u32_Diff; ++u32_ItAr)
                  {
                     c_Reserved.c_MinValue.SetValueAU8Element(0U, u32_ItAr);
                     c_Reserved.c_MaxValue.SetValueAU8Element(0U, u32_ItAr);
                     c_Reserved.c_NvmValue.SetValueAU8Element(0U, u32_ItAr);
                     c_Reserved.c_Value.SetValueAU8Element(0U, u32_ItAr);
                  }
                  for (uint32 u32_ItDataSet = 0UL; u32_ItDataSet < c_ItList->c_DataSets.size(); ++u32_ItDataSet)
                  {
                     c_Reserved.c_DataSetValues.push_back(c_Reserved.c_Value);
                  }
               }
               c_ItList->c_Elements.push_back(c_Reserved);
            }
         }
         c_ItDp->RecalculateAddress();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check two datapools scenario

   \return
   Is two datapools scenario
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCHALCMagicianGenerator::m_CheckTwoDp(void) const
{
   return (this->mpc_Node->c_HALCConfig.e_SafetyMode == C_OSCHalcDefBase::eTWO_LEVELS_WITH_DROPPING) ||
          (this->mpc_Node->c_HALCConfig.e_SafetyMode == C_OSCHalcDefBase::eTWO_LEVELS_WITHOUT_DROPPING);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle copies

   \param[in,out]  orc_Datapools    Datapools

   \return
   C_NO_ERR Datapool generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_HandleCopies(std::vector<C_OSCNodeDataPool> & orc_Datapools) const
{
   sint32 s32_Retval = C_NO_ERR;

   if ((this->mpc_Node != NULL) && (this->mpc_Node->c_HALCConfig.u8_NumConfigCopies >= 1U))
   {
      const std::vector<C_OSCNodeDataPool> c_Initial = orc_Datapools;
      for (uint8 u8_ItCopy = 0U; u8_ItCopy < (this->mpc_Node->c_HALCConfig.u8_NumConfigCopies - 1U); ++u8_ItCopy)
      {
         for (uint32 u32_ItInit = 0UL; u32_ItInit < c_Initial.size(); ++u32_ItInit)
         {
            C_OSCNodeDataPool c_Tmp = c_Initial[u32_ItInit];
            //lint -e{9114} kept for readability
            c_Tmp.c_Name = C_OSCHALCMagicianUtil::h_GetDatapoolName(c_Initial[u32_ItInit].q_IsSafety, u8_ItCopy + 1UL);
            //lint -e{9114} kept for readability
            c_Tmp.c_Comment = C_OSCHALCMagicianUtil::h_GetDatapoolComment(c_Initial[u32_ItInit].q_IsSafety,
                                                                          u8_ItCopy + 1UL);
            //Remove datasets
            for (std::vector<C_OSCNodeDataPoolList>::iterator c_ItList = c_Tmp.c_Lists.begin();
                 c_ItList != c_Tmp.c_Lists.end(); ++c_ItList)
            {
               c_ItList->c_DataSets.clear();
               for (std::vector<C_OSCNodeDataPoolListElement>::iterator c_ItEl = c_ItList->c_Elements.begin();
                    c_ItEl != c_ItList->c_Elements.end(); ++c_ItEl)
               {
                  c_ItEl->c_DataSetValues.clear();
               }
            }
            orc_Datapools.push_back(c_Tmp);
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
      osc_write_log_warning("HALC datapool generation", "node has unexpected number of copies");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle NVM

   \param[in,out]  orc_Datapools    Datapools

   \return
   C_NO_ERR Datapool generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_HandleNVM(std::vector<C_OSCNodeDataPool> & orc_Datapools) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->mpc_Node != NULL)
   {
      if (this->mpc_Node->c_HALCConfig.q_NvMBasedConfig)
      {
         const bool q_BothDps = this->m_CheckTwoDp();
         if (q_BothDps || (this->mpc_Node->c_HALCConfig.e_SafetyMode == C_OSCHalcDefBase::eONE_LEVEL_ALL_SAFE))
         {
            s32_Retval = m_HandleNVMDpOffset(orc_Datapools, this->mpc_Node->c_HALCConfig.c_NvMSafeAddressOffset, true);
         }
         if ((s32_Retval == C_NO_ERR) &&
             (q_BothDps || (this->mpc_Node->c_HALCConfig.e_SafetyMode == C_OSCHalcDefBase::eONE_LEVEL_ALL_NON_SAFE)))
         {
            s32_Retval = m_HandleNVMDpOffset(orc_Datapools, this->mpc_Node->c_HALCConfig.c_NvMNonSafeAddressOffset,
                                             false);
         }
         //ALL DPs
         if (s32_Retval == C_NO_ERR)
         {
            for (uint32 u32_ItDP = 0UL; (u32_ItDP < orc_Datapools.size()) && (s32_Retval == C_NO_ERR); ++u32_ItDP)
            {
               orc_Datapools[u32_ItDP].e_Type = C_OSCNodeDataPool::eHALC_NVM;
               //Lists
               if (orc_Datapools[u32_ItDP].c_Lists.size() == 4UL)
               {
                  C_OSCNodeDataPoolList & rc_ListParam = orc_Datapools[u32_ItDP].c_Lists[0UL];
                  C_OSCNodeDataPoolList & rc_ListInput = orc_Datapools[u32_ItDP].c_Lists[1UL];
                  C_OSCNodeDataPoolList & rc_ListOutput = orc_Datapools[u32_ItDP].c_Lists[2UL];
                  C_OSCNodeDataPoolList & rc_ListStatus = orc_Datapools[u32_ItDP].c_Lists[3UL];
                  //CRC (always)
                  rc_ListParam.q_NvMCRCActive = true;
                  rc_ListInput.q_NvMCRCActive = true;
                  rc_ListOutput.q_NvMCRCActive = true;
                  rc_ListStatus.q_NvMCRCActive = true;
                  //List size
                  rc_ListParam.u32_NvMSize = this->mpc_Node->c_HALCConfig.u32_NvMReservedListSizeParameters;

                  rc_ListInput.u32_NvMSize = this->mpc_Node->c_HALCConfig.u32_NvMReservedListSizeInputValues;
                  rc_ListOutput.u32_NvMSize = this->mpc_Node->c_HALCConfig.u32_NvMReservedListSizeOutputValues;
                  rc_ListStatus.u32_NvMSize = this->mpc_Node->c_HALCConfig.u32_NvMReservedListSizeStatusValues;
                  //DP size
                  {
                     uint32 u32_Sum = 0UL;
                     u32_Sum += this->mpc_Node->c_HALCConfig.u32_NvMReservedListSizeParameters;
                     u32_Sum += this->mpc_Node->c_HALCConfig.u32_NvMReservedListSizeInputValues;
                     u32_Sum += this->mpc_Node->c_HALCConfig.u32_NvMReservedListSizeOutputValues;
                     u32_Sum += this->mpc_Node->c_HALCConfig.u32_NvMReservedListSizeStatusValues;
                     orc_Datapools[u32_ItDP].u32_NvMSize = u32_Sum;
                  }
                  //Everything else
                  orc_Datapools[u32_ItDP].RecalculateAddress();
               }
               else
               {
                  s32_Retval = C_CONFIG;
                  osc_write_log_warning("HALC datapool generation",
                                        "unexpected number of lists found in datapool");
               }
            }
         }
      }
      else
      {
         for (uint32 u32_ItDP = 0UL; u32_ItDP < orc_Datapools.size(); ++u32_ItDP)
         {
            orc_Datapools[u32_ItDP].e_Type = C_OSCNodeDataPool::eHALC;
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle NVM datapool offsets

   \param[in,out]  orc_Datapools    Datapools
   \param[in]      orc_Offsets      Offsets
   \param[in]      oq_IsSafe        Is safe

   \return
   C_NO_ERR Datapool generated
   C_CONFIG Configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianGenerator::m_HandleNVMDpOffset(std::vector<C_OSCNodeDataPool> & orc_Datapools,
                                                       const std::vector<stw_types::uint32> & orc_Offsets,
                                                       const bool oq_IsSafe) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->mpc_Node != NULL)
   {
      uint32 u32_ItOffset = 0UL;
      for (uint32 u32_ItDP = 0UL; (u32_ItDP < orc_Datapools.size()) && (s32_Retval == C_NO_ERR); ++u32_ItDP)
      {
         if (orc_Datapools[u32_ItDP].q_IsSafety == oq_IsSafe)
         {
            if (u32_ItOffset < orc_Offsets.size())
            {
               orc_Datapools[u32_ItDP].u32_NvMStartAddress = orc_Offsets[u32_ItOffset];
               //Iterate
               ++u32_ItOffset;
            }
            else
            {
               s32_Retval = C_CONFIG;
               osc_write_log_warning("HALC datapool generation",
                                     "unexpected number of offset addresses for the datapools");
            }
         }
      }
      if (u32_ItOffset != orc_Offsets.size())
      {
         s32_Retval = C_CONFIG;
         osc_write_log_warning("HALC datapool generation",
                               "unexpected number of offset addresses for the datapools");
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle version

   \param[in,out]  orc_Datapool  Datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHALCMagicianGenerator::m_HandleVersion(C_OSCNodeDataPool & orc_Datapool) const
{
   if (this->mpc_Node != NULL)
   {
      orc_Datapool.au8_Version[0] =
         static_cast<uint8>((this->mpc_Node->c_HALCConfig.u32_ContentVersion & 0xF000UL) >> 12UL);
      orc_Datapool.au8_Version[1] =
         static_cast<uint8>((this->mpc_Node->c_HALCConfig.u32_ContentVersion & 0x0FF0UL) >> 4UL);
      orc_Datapool.au8_Version[2] =
         static_cast<uint8>(this->mpc_Node->c_HALCConfig.u32_ContentVersion & 0xF000FUL);
   }
}
