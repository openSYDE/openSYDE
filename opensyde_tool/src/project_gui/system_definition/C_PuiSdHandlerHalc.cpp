//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC data interface

   HALC data interface

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_PuiSdHandlerHalc.hpp"
#include "C_OscHalcMagicianUtil.hpp"
#include "C_OscHalcMagicianGenerator.hpp"
#include "C_OscHalcMagicianDatapoolListHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdHandlerHalc::C_PuiSdHandlerHalc(QObject * const opc_Parent) :
   C_PuiSdHandlerNodeLogic(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get HALC config

   \param[in]  ou32_NodeIndex    Node index

   \return
   NULL Data not found
   Else HALC config
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscHalcConfig * C_PuiSdHandlerHalc::GetHalcConfig(const uint32_t ou32_NodeIndex) const
{
   const C_OscHalcConfig * pc_Retval;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      pc_Retval = &rc_Node.c_HalcConfig;
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain config data const

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_DomainIndex  Domain index

   \return
   NULL Data not found
   Else HALC config domain
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscHalcConfigDomain * C_PuiSdHandlerHalc::GetHalcDomainConfigDataConst(const uint32_t ou32_NodeIndex,
                                                                               const uint32_t ou32_DomainIndex) const
{
   const C_OscHalcConfigDomain * pc_Retval;
   const C_OscHalcConfig * const pc_Config = this->GetHalcConfig(ou32_NodeIndex);

   if (pc_Config != NULL)
   {
      pc_Retval = pc_Config->GetDomainConfigDataConst(ou32_DomainIndex);
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain channel config data

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index

   \return
   NULL Data not found
   Else HALC config domain channel
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscHalcConfigChannel * C_PuiSdHandlerHalc::GetHalcDomainChannelConfigData(const uint32_t ou32_NodeIndex,
                                                                                  const uint32_t ou32_DomainIndex,
                                                                                  const uint32_t ou32_ChannelIndex,
                                                                                  const bool oq_UseChannelIndex) const
{
   const C_OscHalcConfigChannel * pc_Retval;
   const C_OscHalcConfigDomain * const pc_Config = this->GetHalcDomainConfigDataConst(ou32_NodeIndex, ou32_DomainIndex);

   if (pc_Config != NULL)
   {
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < pc_Config->c_ChannelConfigs.size())
         {
            pc_Retval = &pc_Config->c_ChannelConfigs[ou32_ChannelIndex];
         }
         else
         {
            pc_Retval = NULL;
         }
      }
      else
      {
         pc_Retval = &pc_Config->c_DomainConfig;
      }
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain channel parameter config data

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  ou32_ParameterIndex  Parameter index
   \param[in]  oq_UseChannelIndex   Use channel index

   \return
   NULL Data not found
   Else HALC config domain channel parameter
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscHalcConfigParameterStruct * C_PuiSdHandlerHalc::GetHalcDomainChannelParameterConfigStructData(
   const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
   const uint32_t ou32_ParameterIndex, const bool oq_UseChannelIndex) const
{
   const C_OscHalcConfigParameterStruct * pc_Retval;
   const C_OscHalcConfigChannel * const pc_Config = this->GetHalcDomainChannelConfigData(ou32_NodeIndex,
                                                                                         ou32_DomainIndex,
                                                                                         ou32_ChannelIndex,
                                                                                         oq_UseChannelIndex);

   if (pc_Config != NULL)
   {
      if (ou32_ParameterIndex < pc_Config->c_Parameters.size())
      {
         pc_Retval = &pc_Config->c_Parameters[ou32_ParameterIndex];
      }
      else
      {
         pc_Retval = NULL;
      }
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain file data const

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_DomainIndex  Domain index

   \return
   NULL Data not found
   Else HALC file domain
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscHalcDefDomain * C_PuiSdHandlerHalc::GetHalcDomainFileDataConst(const uint32_t ou32_NodeIndex,
                                                                          const uint32_t ou32_DomainIndex) const
{
   const C_OscHalcDefDomain * pc_Retval;
   const C_OscHalcConfig * const pc_Config = this->GetHalcConfig(ou32_NodeIndex);

   if (pc_Config != NULL)
   {
      pc_Retval = pc_Config->GetDomainDefDataConst(ou32_DomainIndex);
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get HALC domain file channel data const

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_DomainIndex  Domain index
   \param[in]  ou32_Channel      Channel

   \return
   NULL Data not found
   Else HALC file channel
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscHalcDefChannelDef * C_PuiSdHandlerHalc::GetHalcDomainFileChannelDataConst(const uint32_t ou32_NodeIndex,
                                                                                     const uint32_t ou32_DomainIndex,
                                                                                     const uint32_t ou32_Channel) const
{
   const C_OscHalcDefChannelDef * pc_Retval;
   const C_OscHalcDefDomain * const pc_Config = this->GetHalcDomainFileDataConst(ou32_NodeIndex, ou32_DomainIndex);

   if (pc_Config != NULL)
   {
      if (ou32_Channel < pc_Config->c_Channels.size())
      {
         pc_Retval = &pc_Config->c_Channels[ou32_Channel];
      }
      else
      {
         pc_Retval = NULL;
      }
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain file use case data

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_UseCaseIndex    Use case index

   \return
   NULL Data not found
   Else HALC file domain use case
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscHalcDefChannelUseCase * C_PuiSdHandlerHalc::GetHalcDomainFileUseCaseData(const uint32_t ou32_NodeIndex,
                                                                                    const uint32_t ou32_DomainIndex,
                                                                                    const uint32_t ou32_UseCaseIndex)
const
{
   const C_OscHalcDefChannelUseCase * pc_Retval;
   const C_OscHalcDefDomain * const pc_Config = this->GetHalcDomainFileDataConst(ou32_NodeIndex, ou32_DomainIndex);

   if (pc_Config != NULL)
   {
      if (ou32_UseCaseIndex < pc_Config->c_ChannelUseCases.size())
      {
         pc_Retval = &pc_Config->c_ChannelUseCases[ou32_UseCaseIndex];
      }
      else
      {
         pc_Retval = NULL;
      }
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain file use case availability data

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_DomainIndex        Domain index
   \param[in]  ou32_UseCaseIndex       Use case index
   \param[in]  ou32_AvailabilityIndex  Availability index

   \return
   NULL Data not found
   Else HALC file domain use case availability
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscHalcDefChannelAvailability * C_PuiSdHandlerHalc::GetHalcDomainFileUseCaseAvailabilityData(
   const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex, const uint32_t ou32_UseCaseIndex,
   const uint32_t ou32_AvailabilityIndex) const
{
   const C_OscHalcDefChannelAvailability * pc_Retval;
   const C_OscHalcDefChannelUseCase * const pc_Config = this->GetHalcDomainFileUseCaseData(ou32_NodeIndex,
                                                                                           ou32_DomainIndex,
                                                                                           ou32_UseCaseIndex);

   if (pc_Config != NULL)
   {
      if (ou32_AvailabilityIndex < pc_Config->c_Availability.size())
      {
         pc_Retval = &pc_Config->c_Availability[ou32_AvailabilityIndex];
      }
      else
      {
         pc_Retval = NULL;
      }
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain variable data

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_DomainIndex  Domain index
   \param[in]  oe_Selector       Selector
   \param[in]  ou32_Index        Index

   \return
   NULL Data not found
   Else HALC file domain variable
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscHalcDefStruct * C_PuiSdHandlerHalc::GetHalcDomainFileVariableData(const uint32_t ou32_NodeIndex,
                                                                             const uint32_t ou32_DomainIndex,
                                                                             const C_OscHalcDefDomain::E_VariableSelector oe_Selector,
                                                                             const uint32_t ou32_Index) const
{
   const C_OscHalcDefStruct * pc_Retval;
   const C_OscHalcDefDomain * const pc_DefDomain = this->GetHalcDomainFileDataConst(ou32_NodeIndex, ou32_DomainIndex);
   const C_OscHalcConfigDomain * const pc_ConfigDomain = this->GetHalcDomainConfigDataConst(ou32_NodeIndex,
                                                                                            ou32_DomainIndex);

   if ((pc_DefDomain != NULL) && (pc_ConfigDomain != NULL))
   {
      //differentiate domain or channel values
      const C_OscHalcDefChannelValues & rc_Values =
         (pc_ConfigDomain->c_ChannelConfigs.size() == 0UL) ?
         pc_DefDomain->c_DomainValues : pc_DefDomain->c_ChannelValues;

      switch (oe_Selector)
      {
      case C_OscHalcDefDomain::eVA_PARAM:
         if (ou32_Index < rc_Values.c_Parameters.size())
         {
            pc_Retval = &rc_Values.c_Parameters[ou32_Index];
         }
         else
         {
            pc_Retval = NULL;
         }
         break;
      case C_OscHalcDefDomain::eVA_INPUT:
         if (ou32_Index < rc_Values.c_InputValues.size())
         {
            pc_Retval = &rc_Values.c_InputValues[ou32_Index];
         }
         else
         {
            pc_Retval = NULL;
         }
         break;
      case C_OscHalcDefDomain::eVA_OUTPUT:
         if (ou32_Index < rc_Values.c_OutputValues.size())
         {
            pc_Retval = &rc_Values.c_OutputValues[ou32_Index];
         }
         else
         {
            pc_Retval = NULL;
         }
         break;
      case C_OscHalcDefDomain::eVA_STATUS:
         if (ou32_Index < rc_Values.c_StatusValues.size())
         {
            pc_Retval = &rc_Values.c_StatusValues[ou32_Index];
         }
         else
         {
            pc_Retval = NULL;
         }
         break;
      default:
         pc_Retval = NULL;
         break;
      }
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get first HALC datapool of specified type (HALC or HALC_NVM)

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oq_SafeDatapool   Flag to differ HAL safe datapool and HAL nonsafe datapool

   \return
   Found: Pointer to OSC data pool of type HAL and safety type oq_SafeDatapool
   Else:  NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNodeDataPool * C_PuiSdHandlerHalc::GetHalcDatapool(const uint32_t ou32_NodeIndex,
                                                              const bool oq_SafeDatapool) const
{
   const C_OscNodeDataPool * pc_Retval = NULL;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      pc_Retval = rc_OscNode.GetHalDataPoolConst(oq_SafeDatapool);
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the domain category from datapool list element id

   \param[in]  orc_Id            Id
   \param[in]  ou32_ArrayIndex   Array index

   \return
   eCA_INPUT   Use the input domain
   eCA_OUTPUT  Use the output domain
   eCA_OTHER   Use the other domain
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscHalcDefDomain::E_Category C_PuiSdHandlerHalc::GetDomainCategoryFromDpId(
   const C_OscNodeDataPoolListElementId & orc_Id, const uint32_t ou32_ArrayIndex)
{
   uint32_t u32_DomainIndex = 0;
   bool q_UseChannelIndex;
   uint32_t u32_ChannelIndex;

   C_OscHalcDefDomain::E_VariableSelector e_Selector;
   uint32_t u32_ParameterIndex;
   bool q_UseElementIndex;
   uint32_t u32_ParameterElementIndex;
   bool q_IsUseCaseIndex;
   bool q_IsChanNumIndex;
   bool q_IsSafetyFlagIndex;

   C_OscHalcDefDomain::E_Category e_Category = C_OscHalcDefDomain::eCA_OTHER;

   if (this->TranslateToHalcIndex(orc_Id, ou32_ArrayIndex, u32_DomainIndex, q_UseChannelIndex, u32_ChannelIndex,
                                  e_Selector,
                                  u32_ParameterIndex, q_UseElementIndex, u32_ParameterElementIndex, q_IsUseCaseIndex,
                                  q_IsChanNumIndex, q_IsSafetyFlagIndex) == C_NO_ERR)
   {
      const C_OscHalcDefDomain * const pc_Domain = this->GetHalcDomainFileDataConst(orc_Id.u32_NodeIndex,
                                                                                    u32_DomainIndex);
      if (pc_Domain != NULL)
      {
         e_Category = pc_Domain->e_Category;

         // we don't need those here
         Q_UNUSED(q_IsSafetyFlagIndex)
         Q_UNUSED(q_IsChanNumIndex)
         Q_UNUSED(q_IsUseCaseIndex)
         Q_UNUSED(u32_ChannelIndex)
         Q_UNUSED(q_UseChannelIndex)
      }
   }

   return e_Category;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear HALC configuration

   \param[in]  ou32_NodeIndex    Node index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::ClearHalcConfig(const uint32_t ou32_NodeIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      rc_Node.c_HalcConfig.Clear();

      // reset hash for this node's halc config if there already exists a hash value for this node's halc config
      // [] sets the value if key was found, otherwise creates a new key value pair
      mc_PreviousHashes[ou32_NodeIndex] = 0xFFFFFFFFUL;
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set config

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_Config        Config

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::SetHalcConfig(const uint32_t ou32_NodeIndex, const C_OscHalcConfig & orc_Config)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      rc_Node.c_HalcConfig = orc_Config;
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain config

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_DomainIndex  Domain index
   \param[in]  orc_Domain        Domain

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::SetHalcDomainConfig(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                const C_OscHalcConfigDomain & orc_Domain)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HalcConfig.SetDomainConfig(ou32_DomainIndex, orc_Domain);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel config

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  orc_Channel          New channel

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::SetHalcDomainChannelConfig(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                       const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                       const C_OscHalcConfigChannel & orc_Channel)
{
   return this->SetHalcDomainChannelConfig(ou32_NodeIndex, ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex,
                                           orc_Channel.c_Name, orc_Channel.c_Comment, orc_Channel.q_SafetyRelevant,
                                           orc_Channel.u32_UseCaseIndex);
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel config

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  orc_Name             Name
   \param[in]  orc_Comment          Comment
   \param[in]  oq_SafetyRelevant    Safety relevant
   \param[in]  ou32_UseCaseIndex    Use case index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::SetHalcDomainChannelConfig(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                       const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                       const stw::scl::C_SclString & orc_Name,
                                                       const stw::scl::C_SclString & orc_Comment,
                                                       const bool oq_SafetyRelevant, const uint32_t ou32_UseCaseIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HalcConfig.SetDomainChannelConfig(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex,
                                                               orc_Name, orc_Comment, oq_SafetyRelevant,
                                                               ou32_UseCaseIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset HALC domain channel configuration

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::ResetHalcDomainChannelConfig(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                         const uint32_t ou32_ChannelIndex,
                                                         const bool oq_UseChannelIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval =
         rc_Node.c_HalcConfig.ResetDomainChannelConfig(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset HALC domain channel use case to default

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::ResetHalcDomainChannelUseCase(const uint32_t ou32_NodeIndex,
                                                          const uint32_t ou32_DomainIndex,
                                                          const uint32_t ou32_ChannelIndex,
                                                          const bool oq_UseChannelIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval =
         rc_Node.c_HalcConfig.ResetDomainChannelUseCase(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC domain channel config name

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  orc_Name             Name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::SetHalcDomainChannelConfigName(const uint32_t ou32_NodeIndex,
                                                           const uint32_t ou32_DomainIndex,
                                                           const uint32_t ou32_ChannelIndex,
                                                           const bool oq_UseChannelIndex,
                                                           const stw::scl::C_SclString & orc_Name)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HalcConfig.SetDomainChannelConfigName(ou32_DomainIndex, ou32_ChannelIndex,
                                                                   oq_UseChannelIndex, orc_Name);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC domain channel config comment

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  orc_Comment          Comment

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::SetHalcDomainChannelConfigComment(const uint32_t ou32_NodeIndex,
                                                              const uint32_t ou32_DomainIndex,
                                                              const uint32_t ou32_ChannelIndex,
                                                              const bool oq_UseChannelIndex,
                                                              const stw::scl::C_SclString & orc_Comment)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HalcConfig.SetDomainChannelConfigComment(ou32_DomainIndex, ou32_ChannelIndex,
                                                                      oq_UseChannelIndex, orc_Comment);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC domain channel config safety

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  oq_SafetyRelevant    Safety relevant

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::SetHalcDomainChannelConfigSafety(const uint32_t ou32_NodeIndex,
                                                             const uint32_t ou32_DomainIndex,
                                                             const uint32_t ou32_ChannelIndex,
                                                             const bool oq_UseChannelIndex,
                                                             const bool oq_SafetyRelevant)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HalcConfig.SetDomainChannelConfigSafety(ou32_DomainIndex, ou32_ChannelIndex,
                                                                     oq_UseChannelIndex, oq_SafetyRelevant);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC domain channel config use case

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  ou32_UseCaseIndex    Use case index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::SetHalcDomainChannelConfigUseCase(const uint32_t ou32_NodeIndex,
                                                              const uint32_t ou32_DomainIndex,
                                                              const uint32_t ou32_ChannelIndex,
                                                              const bool oq_UseChannelIndex,
                                                              const uint32_t ou32_UseCaseIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HalcConfig.SetDomainChannelConfigUseCase(ou32_DomainIndex, ou32_ChannelIndex,
                                                                      oq_UseChannelIndex, ou32_UseCaseIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel parameter config

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  ou32_ParameterIndex  Parameter index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  orc_Parameter        Parameter

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::SetHalcDomainChannelParameterConfig(const uint32_t ou32_NodeIndex,
                                                                const uint32_t ou32_DomainIndex,
                                                                const uint32_t ou32_ChannelIndex,
                                                                const uint32_t ou32_ParameterIndex,
                                                                const bool oq_UseChannelIndex,
                                                                const C_OscHalcConfigParameterStruct & orc_Parameter)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HalcConfig.SetDomainChannelParameterConfig(ou32_DomainIndex, ou32_ChannelIndex,
                                                                        ou32_ParameterIndex, oq_UseChannelIndex,
                                                                        orc_Parameter);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC domain channel parameter config element

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  ou32_ParameterIndex  Parameter index
   \param[in]  ou32_ElementIndex    Element index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  orc_Parameter        Parameter

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::SetHalcDomainChannelParameterConfigElement(const uint32_t ou32_NodeIndex,
                                                                       const uint32_t ou32_DomainIndex,
                                                                       const uint32_t ou32_ChannelIndex,
                                                                       const uint32_t ou32_ParameterIndex,
                                                                       const uint32_t ou32_ElementIndex,
                                                                       const bool oq_UseChannelIndex,
                                                                       const C_OscHalcConfigParameter & orc_Parameter)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HalcConfig.SetDomainChannelParameterConfigElement(ou32_DomainIndex, ou32_ChannelIndex,
                                                                               ou32_ParameterIndex, ou32_ElementIndex,
                                                                               oq_UseChannelIndex, orc_Parameter);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC domain channel parameter config element plain value

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  ou32_ParameterIndex  Parameter index
   \param[in]  ou32_ElementIndex    Element index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  orc_Value            Value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::SetHalcDomainChannelParameterConfigElementPlain(const uint32_t ou32_NodeIndex,
                                                                            const uint32_t ou32_DomainIndex,
                                                                            const uint32_t ou32_ChannelIndex,
                                                                            const uint32_t ou32_ParameterIndex,
                                                                            const uint32_t ou32_ElementIndex,
                                                                            const bool oq_UseChannelIndex,
                                                                            const C_OscHalcDefContent & orc_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HalcConfig.SetDomainChannelParameterConfigElementPlain(ou32_DomainIndex, ou32_ChannelIndex,
                                                                                    ou32_ParameterIndex,
                                                                                    ou32_ElementIndex,
                                                                                    oq_UseChannelIndex, orc_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC domain channel parameter config element

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  ou32_ParameterIndex  Parameter index
   \param[in]  ou32_ElementIndex    Element index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  orc_DisplayName      Display name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::SetHalcDomainChannelParameterConfigElementEnum(const uint32_t ou32_NodeIndex,
                                                                           const uint32_t ou32_DomainIndex,
                                                                           const uint32_t ou32_ChannelIndex,
                                                                           const uint32_t ou32_ParameterIndex,
                                                                           const uint32_t ou32_ElementIndex,
                                                                           const bool oq_UseChannelIndex,
                                                                           const stw::scl::C_SclString & orc_DisplayName)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HalcConfig.SetDomainChannelParameterConfigElementEnum(ou32_DomainIndex, ou32_ChannelIndex,
                                                                                   ou32_ParameterIndex,
                                                                                   ou32_ElementIndex,
                                                                                   oq_UseChannelIndex, orc_DisplayName);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC domain channel parameter config element

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  ou32_ParameterIndex  Parameter index
   \param[in]  ou32_ElementIndex    Element index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  orc_DisplayName      Display name
   \param[in]  oq_Value             Value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::SetHalcDomainChannelParameterConfigElementBitmask(const uint32_t ou32_NodeIndex,
                                                                              const uint32_t ou32_DomainIndex,
                                                                              const uint32_t ou32_ChannelIndex,
                                                                              const uint32_t ou32_ParameterIndex,
                                                                              const uint32_t ou32_ElementIndex,
                                                                              const bool oq_UseChannelIndex,
                                                                              const stw::scl::C_SclString & orc_DisplayName,
                                                                              const bool oq_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HalcConfig.SetDomainChannelParameterConfigElementBitmask(ou32_DomainIndex,
                                                                                      ou32_ChannelIndex,
                                                                                      ou32_ParameterIndex,
                                                                                      ou32_ElementIndex,
                                                                                      oq_UseChannelIndex,
                                                                                      orc_DisplayName, oq_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get HALC channel or domain name

   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DomainIndex    Domain index
   \param[in]      ou32_Channel        Channel
   \param[in,out]  orc_Name            Name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::GetHalChannelOrDomainName(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                      const uint32_t ou32_Channel, QString & orc_Name) const
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscHalcDefChannelDef * const pc_ChannelDef = this->GetHalcDomainFileChannelDataConst(ou32_NodeIndex,
                                                                                                ou32_DomainIndex,
                                                                                                ou32_Channel);

   if (pc_ChannelDef != NULL)
   {
      orc_Name = pc_ChannelDef->c_Name.c_str();
   }
   else
   {
      const C_OscHalcConfigDomain * const pc_Domain = this->GetHalcDomainConfigDataConst(ou32_NodeIndex,
                                                                                         ou32_DomainIndex);
      if (pc_Domain != NULL)
      {
         orc_Name = pc_Domain->c_SingularName.c_str();
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Translate to HALC index

   \param[in]   orc_Id                       Id
   \param[in]   ou32_ArrayIndex              Array index
   \param[out]  oru32_DomainIndex            Domain index
   \param[out]  orq_UseChannelIndex          Use channel index
   \param[out]  oru32_ChannelIndex           Channel index
   \param[out]  ore_Selector                 Selector
   \param[out]  oru32_ParameterIndex         Parameter index
   \param[out]  orq_UseElementIndex          Use element index
   \param[out]  oru32_ParameterElementIndex  Parameter element index
   \param[out]  orq_IsUseCaseIndex           Is use case index
   \param[out]  orq_IsChanNumIndex           Is channel number index
   \param[out]  orq_IsSafetyFlagIndex        Is safety flag index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::TranslateToHalcIndex(const C_OscNodeDataPoolListElementId & orc_Id,
                                                 const uint32_t ou32_ArrayIndex, uint32_t & oru32_DomainIndex,
                                                 bool & orq_UseChannelIndex, uint32_t & oru32_ChannelIndex,
                                                 C_OscHalcDefDomain::E_VariableSelector & ore_Selector,
                                                 uint32_t & oru32_ParameterIndex, bool & orq_UseElementIndex,
                                                 uint32_t & oru32_ParameterElementIndex, bool & orq_IsUseCaseIndex,
                                                 bool & orq_IsChanNumIndex, bool & orq_IsSafetyFlagIndex) const
{
   int32_t s32_Retval = C_NO_ERR;

   const C_OscNode * const pc_Node = this->GetOscNodeConst(orc_Id.u32_NodeIndex);
   const C_OscNodeDataPool * const pc_Dp = this->GetOscDataPool(orc_Id.u32_NodeIndex, orc_Id.u32_DataPoolIndex);

   switch (orc_Id.u32_ListIndex)
   {
   case 0UL:
      ore_Selector = C_OscHalcDefDomain::eVA_PARAM;
      break;
   case 1UL:
      ore_Selector = C_OscHalcDefDomain::eVA_INPUT;
      break;
   case 2UL:
      ore_Selector = C_OscHalcDefDomain::eVA_OUTPUT;
      break;
   case 3UL:
      ore_Selector = C_OscHalcDefDomain::eVA_STATUS;
      break;
   default:
      ore_Selector = C_OscHalcDefDomain::eVA_PARAM;
      break;
   }

   if ((pc_Node != NULL) && (pc_Dp != NULL))
   {
      const C_OscHalcMagicianDatapoolListHandler c_DpHandler(pc_Node->c_HalcConfig, ore_Selector, pc_Dp->q_IsSafety);
      bool q_Found = false;
      bool q_FoundChannelIndex = false;
      uint32_t u32_ListIndex = 0UL;

      //Init
      oru32_ChannelIndex = 0UL;

      for (uint32_t u32_ItDomain = 0UL; (u32_ItDomain < pc_Node->c_HalcConfig.GetDomainSize()) && (q_Found == false);
           ++u32_ItDomain)
      {
         const C_OscHalcDefDomain * const pc_CurrentDomainDef =
            pc_Node->c_HalcConfig.GetDomainDefDataConst(u32_ItDomain);
         const C_OscHalcConfigDomain * const pc_CurrentDomainConfig = pc_Node->c_HalcConfig.GetDomainConfigDataConst(
            u32_ItDomain);
         if ((pc_CurrentDomainDef != NULL) && (pc_CurrentDomainConfig != NULL))
         {
            const uint32_t u32_CountRelevantItems = c_DpHandler.CountRelevantItems(
               pc_CurrentDomainConfig->c_ChannelConfigs, pc_CurrentDomainConfig->c_DomainConfig);
            //other values
            if (u32_CountRelevantItems > 0UL)
            {
               //channels
               if (ore_Selector == C_OscHalcDefDomain::eVA_PARAM)
               {
                  if (c_DpHandler.CheckChanNumVariableNecessary(*pc_CurrentDomainConfig))
                  {
                     if (u32_ListIndex == orc_Id.u32_ElementIndex)
                     {
                        oru32_DomainIndex = u32_ItDomain;
                        orq_UseChannelIndex = pc_CurrentDomainDef->c_Channels.size() > 0UL;
                        oru32_ParameterIndex = 0UL;
                        orq_UseElementIndex = false;
                        oru32_ParameterElementIndex = 0UL;
                        orq_IsUseCaseIndex = false;
                        orq_IsChanNumIndex = true;
                        orq_IsSafetyFlagIndex = false;
                        //Stop
                        q_Found = true;
                     }
                     u32_ListIndex += 1UL;
                  }
               }

               if (q_Found == false)
               {
                  //safety flag
                  if (ore_Selector == C_OscHalcDefDomain::eVA_PARAM)
                  {
                     if (c_DpHandler.CheckSafetyFlagVariableNecessary())
                     {
                        if (u32_ListIndex == orc_Id.u32_ElementIndex)
                        {
                           oru32_DomainIndex = u32_ItDomain;
                           orq_UseChannelIndex = pc_CurrentDomainDef->c_Channels.size() > 0UL;
                           oru32_ParameterIndex = 0UL;
                           orq_UseElementIndex = false;
                           oru32_ParameterElementIndex = 0UL;
                           orq_IsUseCaseIndex = false;
                           orq_IsChanNumIndex = false;
                           orq_IsSafetyFlagIndex = true;
                           //Stop
                           q_Found = true;
                        }
                        u32_ListIndex += 1UL;
                     }
                  }
                  if (q_Found == false)
                  {
                     //usecase
                     if (ore_Selector == C_OscHalcDefDomain::eVA_PARAM)
                     {
                        if (c_DpHandler.CheckUseCaseVariableNecessary(*pc_CurrentDomainConfig))
                        {
                           if (u32_ListIndex == orc_Id.u32_ElementIndex)
                           {
                              oru32_DomainIndex = u32_ItDomain;
                              orq_UseChannelIndex = pc_CurrentDomainDef->c_Channels.size() > 0UL;
                              oru32_ParameterIndex = 0UL;
                              orq_UseElementIndex = false;
                              oru32_ParameterElementIndex = 0UL;
                              orq_IsUseCaseIndex = true;
                              orq_IsChanNumIndex = false;
                              orq_IsSafetyFlagIndex = false;
                              //Stop
                              q_Found = true;
                           }
                           u32_ListIndex += 1UL;
                        }
                     }
                     if (q_Found == false)
                     {
                        const uint32_t u32_StartingIndex = u32_ListIndex;
                        //others
                        if (pc_CurrentDomainConfig->c_ChannelConfigs.size() == 0UL)
                        {
                           //domain values
                           switch (ore_Selector)
                           {
                           case C_OscHalcDefDomain::eVA_PARAM:
                              u32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                                 pc_CurrentDomainDef->c_DomainValues.c_Parameters);
                              break;
                           case C_OscHalcDefDomain::eVA_INPUT:
                              u32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                                 pc_CurrentDomainDef->c_DomainValues.c_InputValues);
                              break;
                           case C_OscHalcDefDomain::eVA_OUTPUT:
                              u32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                                 pc_CurrentDomainDef->c_DomainValues.c_OutputValues);
                              break;
                           case C_OscHalcDefDomain::eVA_STATUS:
                              u32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                                 pc_CurrentDomainDef->c_DomainValues.c_StatusValues);
                              break;
                           default:
                              break;
                           }
                        }
                        else
                        {
                           //channel values
                           switch (ore_Selector)
                           {
                           case C_OscHalcDefDomain::eVA_PARAM:
                              u32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                                 pc_CurrentDomainDef->c_ChannelValues.c_Parameters);
                              break;
                           case C_OscHalcDefDomain::eVA_INPUT:
                              u32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                                 pc_CurrentDomainDef->c_ChannelValues.c_InputValues);
                              break;
                           case C_OscHalcDefDomain::eVA_OUTPUT:
                              u32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                                 pc_CurrentDomainDef->c_ChannelValues.c_OutputValues);
                              break;
                           case C_OscHalcDefDomain::eVA_STATUS:
                              u32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                                 pc_CurrentDomainDef->c_ChannelValues.c_StatusValues);
                              break;
                           default:
                              break;
                           }
                        }
                        if (u32_ListIndex > orc_Id.u32_ElementIndex)
                        {
                           //General
                           oru32_DomainIndex = u32_ItDomain;
                           orq_UseChannelIndex = pc_CurrentDomainDef->c_Channels.size() > 0UL;
                           orq_IsUseCaseIndex = false;
                           orq_IsChanNumIndex = false;
                           orq_IsSafetyFlagIndex = false;
                           //Parameter index
                           if (pc_CurrentDomainConfig->c_ChannelConfigs.size() == 0UL)
                           {
                              //domain values
                              switch (ore_Selector)
                              {
                              case C_OscHalcDefDomain::eVA_PARAM:
                                 s32_Retval = C_PuiSdHandlerHalc::mh_GetIndexInVector(
                                    pc_CurrentDomainDef->c_DomainValues.c_Parameters, u32_StartingIndex,
                                    orc_Id.u32_ElementIndex,
                                    oru32_ParameterIndex,
                                    orq_UseElementIndex,
                                    oru32_ParameterElementIndex);
                                 break;
                              case C_OscHalcDefDomain::eVA_INPUT:
                                 s32_Retval = C_PuiSdHandlerHalc::mh_GetIndexInVector(
                                    pc_CurrentDomainDef->c_DomainValues.c_InputValues, u32_StartingIndex,
                                    orc_Id.u32_ElementIndex,
                                    oru32_ParameterIndex,
                                    orq_UseElementIndex,
                                    oru32_ParameterElementIndex);
                                 break;
                              case C_OscHalcDefDomain::eVA_OUTPUT:
                                 s32_Retval = C_PuiSdHandlerHalc::mh_GetIndexInVector(
                                    pc_CurrentDomainDef->c_DomainValues.c_OutputValues, u32_StartingIndex,
                                    orc_Id.u32_ElementIndex,
                                    oru32_ParameterIndex,
                                    orq_UseElementIndex,
                                    oru32_ParameterElementIndex);
                                 break;
                              case C_OscHalcDefDomain::eVA_STATUS:
                                 s32_Retval = C_PuiSdHandlerHalc::mh_GetIndexInVector(
                                    pc_CurrentDomainDef->c_DomainValues.c_StatusValues, u32_StartingIndex,
                                    orc_Id.u32_ElementIndex,
                                    oru32_ParameterIndex,
                                    orq_UseElementIndex,
                                    oru32_ParameterElementIndex);
                                 break;
                              default:
                                 break;
                              }
                           }
                           else
                           {
                              //channel values
                              switch (ore_Selector)
                              {
                              case C_OscHalcDefDomain::eVA_PARAM:
                                 s32_Retval = C_PuiSdHandlerHalc::mh_GetIndexInVector(
                                    pc_CurrentDomainDef->c_ChannelValues.c_Parameters, u32_StartingIndex,
                                    orc_Id.u32_ElementIndex,
                                    oru32_ParameterIndex,
                                    orq_UseElementIndex,
                                    oru32_ParameterElementIndex);
                                 break;
                              case C_OscHalcDefDomain::eVA_INPUT:
                                 s32_Retval = C_PuiSdHandlerHalc::mh_GetIndexInVector(
                                    pc_CurrentDomainDef->c_ChannelValues.c_InputValues, u32_StartingIndex,
                                    orc_Id.u32_ElementIndex,
                                    oru32_ParameterIndex,
                                    orq_UseElementIndex,
                                    oru32_ParameterElementIndex);
                                 break;
                              case C_OscHalcDefDomain::eVA_OUTPUT:
                                 s32_Retval = C_PuiSdHandlerHalc::mh_GetIndexInVector(
                                    pc_CurrentDomainDef->c_ChannelValues.c_OutputValues, u32_StartingIndex,
                                    orc_Id.u32_ElementIndex,
                                    oru32_ParameterIndex,
                                    orq_UseElementIndex,
                                    oru32_ParameterElementIndex);
                                 break;
                              case C_OscHalcDefDomain::eVA_STATUS:
                                 s32_Retval = C_PuiSdHandlerHalc::mh_GetIndexInVector(
                                    pc_CurrentDomainDef->c_ChannelValues.c_StatusValues, u32_StartingIndex,
                                    orc_Id.u32_ElementIndex,
                                    oru32_ParameterIndex,
                                    orq_UseElementIndex,
                                    oru32_ParameterElementIndex);
                                 break;
                              default:
                                 break;
                              }
                           }
                           //Stop
                           q_Found = true;
                        }
                     }
                  }
               }
               //Channel Index
               if (q_Found)
               {
                  if (pc_CurrentDomainConfig->c_ChannelConfigs.size() > 0UL)
                  {
                     uint32_t u32_CurNum = 0UL;
                     for (uint32_t u32_ItChannel = 0UL; u32_ItChannel < pc_CurrentDomainConfig->c_ChannelConfigs.size();
                          ++u32_ItChannel)
                     {
                        const C_OscHalcConfigChannel & rc_Channel =
                           pc_CurrentDomainConfig->c_ChannelConfigs[u32_ItChannel];
                        if (c_DpHandler.CheckChanPresent(rc_Channel))
                        {
                           if (u32_CurNum == ou32_ArrayIndex)
                           {
                              oru32_ChannelIndex = u32_ItChannel;
                              q_FoundChannelIndex = true;
                              break;
                           }
                           ++u32_CurNum;
                        }
                     }
                  }
                  else
                  {
                     q_FoundChannelIndex = true;
                  }
               }
            }
         }
      }
      //Simple
      if ((q_Found == false) || (q_FoundChannelIndex == false))
      {
         s32_Retval = C_RANGE;
      }
   }

   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if the HALC configuration is clear or if it has a loaded definition/configuration

   \param[in]   ou32_NodeIndex   Node index
   \param[out]  orq_IsClear      true    Definition is cleared. No definition is loaded.
                                 false   The definition is not cleared. A concrete definition is loaded.

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::IsHalcClear(const uint32_t ou32_NodeIndex, bool & orq_IsClear)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      orq_IsClear = rc_Node.c_HalcConfig.IsClear();
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get HALC domain channel linked information

   \param[in]      ou32_NodeIndex            Node index
   \param[in]      ou32_DomainIndex          Domain index
   \param[in]      ou32_ChannelIndex         Channel index
   \param[in]      oq_UseChannelIndex        Use channel index
   \param[out]     orq_IsLinked              Is linked
   \param[in,out]  opc_LinkedChannelNames    Linked channel names
   \param[in,out]  opc_LinkedChannelIndices  Linked channel indices
   \param[in]      opu32_UseCaseIndex        Use case index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::CheckHalcDomainChannelLinked(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                         const uint32_t ou32_ChannelIndex,
                                                         const bool oq_UseChannelIndex, bool & orq_IsLinked,
                                                         std::vector<QString> * const opc_LinkedChannelNames,
                                                         std::vector<uint32_t> * const opc_LinkedChannelIndices,
                                                         const uint32_t * const opu32_UseCaseIndex) const
{
   int32_t s32_Retval = C_RANGE;
   const C_OscHalcConfigDomain * const pc_Domain = this->GetHalcDomainConfigDataConst(ou32_NodeIndex, ou32_DomainIndex);

   if (pc_Domain != NULL)
   {
      std::vector<stw::scl::C_SclString> c_LinkedChannelNames;
      s32_Retval = pc_Domain->CheckChannelLinked(ou32_ChannelIndex, oq_UseChannelIndex, orq_IsLinked,
                                                 &c_LinkedChannelNames, opc_LinkedChannelIndices, opu32_UseCaseIndex);
      if (opc_LinkedChannelNames != NULL)
      {
         for (std::vector<stw::scl::C_SclString>::const_iterator c_ItNames = c_LinkedChannelNames.begin();
              c_ItNames != c_LinkedChannelNames.end(); ++c_ItNames)
         {
            opc_LinkedChannelNames->push_back((*c_ItNames).c_str());
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set resp. reset channel configuration of linked channels

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_DomainIndex        Domain index
   \param[in]  ou32_ChannelIndex       Channel index
   \param[in]  oq_UseChannelIndex      Use channel index
   \param[in]  ou32_UseCaseIndexOld    Previous use case index
   \param[in]  ou32_UseCaseIndexNew    New use case index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::SetHalcDomainChannelConfigOfLinkedChannels(const uint32_t ou32_NodeIndex,
                                                                       const uint32_t ou32_DomainIndex,
                                                                       const uint32_t ou32_ChannelIndex,
                                                                       const bool oq_UseChannelIndex,
                                                                       const uint32_t ou32_UseCaseIndexOld,
                                                                       const uint32_t ou32_UseCaseIndexNew)
{
   int32_t s32_Retval = C_NO_ERR;

   // linking is a channel feature only
   if (oq_UseChannelIndex == true)
   {
      bool q_IsLinkedOld = false;
      bool q_IsLinkedNew = false;
      std::vector<uint32_t> c_LinkedChannelIndicesOld;
      std::vector<uint32_t> c_LinkedChannelIndicesNew;

      // get information about linked channels
      s32_Retval = this->CheckHalcDomainChannelLinked(ou32_NodeIndex, ou32_DomainIndex, ou32_ChannelIndex,
                                                      oq_UseChannelIndex, q_IsLinkedOld, NULL,
                                                      &c_LinkedChannelIndicesOld, &ou32_UseCaseIndexOld);

      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = this->CheckHalcDomainChannelLinked(ou32_NodeIndex, ou32_DomainIndex, ou32_ChannelIndex,
                                                         oq_UseChannelIndex, q_IsLinkedNew, NULL,
                                                         &c_LinkedChannelIndicesNew, &ou32_UseCaseIndexNew);
      }

      if ((s32_Retval == C_NO_ERR) && ((q_IsLinkedOld == true) || (q_IsLinkedNew == true)))
      {
         // first revert use case of all previously linked channels
         if (q_IsLinkedOld == true)
         {
            for (std::vector<uint32_t>::const_iterator c_ItChannels = c_LinkedChannelIndicesOld.begin();
                 (c_ItChannels != c_LinkedChannelIndicesOld.end()) && (s32_Retval == C_NO_ERR); ++c_ItChannels)
            {
               s32_Retval = this->ResetHalcDomainChannelUseCase(ou32_NodeIndex, ou32_DomainIndex,
                                                                *c_ItChannels, true);
            }
         }

         // then set use case of all new linked channels
         if ((s32_Retval == C_NO_ERR) && (q_IsLinkedNew == true))
         {
            const C_OscHalcConfigChannel * const pc_Channel = this->GetHalcDomainChannelConfigData(ou32_NodeIndex,
                                                                                                   ou32_DomainIndex,
                                                                                                   ou32_ChannelIndex,
                                                                                                   oq_UseChannelIndex);
            if (pc_Channel != NULL)
            {
               for (std::vector<uint32_t>::const_iterator c_ItChannels = c_LinkedChannelIndicesNew.begin();
                    (c_ItChannels != c_LinkedChannelIndicesNew.end()) && (s32_Retval == C_NO_ERR); ++c_ItChannels)
               {
                  s32_Retval = this->SetHalcDomainChannelConfigUseCase(ou32_NodeIndex, ou32_DomainIndex,
                                                                       *c_ItChannels, true, ou32_UseCaseIndexNew);

                  // update all parameters in linked channel
                  for (uint32_t u32_ItParams = 0;
                       (u32_ItParams < pc_Channel->c_Parameters.size()) && (s32_Retval == C_NO_ERR); u32_ItParams++)
                  {
                     const C_OscHalcConfigParameterStruct & rc_Parameter = pc_Channel->c_Parameters[u32_ItParams];

                     s32_Retval = this->SetHalcDomainChannelParameterConfig(ou32_NodeIndex, ou32_DomainIndex,
                                                                            *c_ItChannels, u32_ItParams, true,
                                                                            rc_Parameter);
                  }
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get HALC domain channel error information

   \param[in]   ou32_NodeIndex      Node index
   \param[in]   ou32_DomainIndex    Domain index
   \param[in]   ou32_ChannelIndex   Channel index
   \param[in]   oq_UseChannelIndex  Use channel index
   \param[out]  orq_HasError        Has error

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::CheckHalcDomainChannelError(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                        const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                        bool & orq_HasError) const
{
   int32_t s32_Retval = C_NO_ERR;

   const C_OscNode * const pc_Node = this->GetOscNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      bool q_DomainInvalid;
      bool q_Tmp;
      std::vector<uint32_t> c_InvalidChannelIndices;
      pc_Node->c_HalcConfig.CheckDomainConfigValid(ou32_DomainIndex, &q_DomainInvalid, &q_Tmp,
                                                   &c_InvalidChannelIndices);
      if (oq_UseChannelIndex == true)
      {
         orq_HasError = false;
         for (uint32_t u32_Inv = 0UL; u32_Inv < c_InvalidChannelIndices.size(); ++u32_Inv)
         {
            if (c_InvalidChannelIndices[u32_Inv] == ou32_ChannelIndex)
            {
               orq_HasError = true;
            }
         }
      }
      else
      {
         orq_HasError = q_DomainInvalid;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get relevant indices for selected use case

   \param[in]      ou32_NodeIndex         Node index
   \param[in]      ou32_DomainIndex       Domain index
   \param[in]      ou32_ChannelIndex      Channel index
   \param[in]      oq_UseChannelIndex     Use channel index
   \param[in,out]  opc_ParameterIndices   Parameter indices
   \param[in,out]  opc_InputIndices       Input indices
   \param[in,out]  opc_OutputIndices      Output indices
   \param[in,out]  opc_StatusIndices      Status indices

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::GetHalcRelevantIndicesForSelectedUseCase(const uint32_t ou32_NodeIndex,
                                                                     const uint32_t ou32_DomainIndex,
                                                                     const uint32_t ou32_ChannelIndex,
                                                                     const bool oq_UseChannelIndex,
                                                                     std::vector<uint32_t> * const opc_ParameterIndices,
                                                                     std::vector<uint32_t> * const opc_InputIndices,
                                                                     std::vector<uint32_t> * const opc_OutputIndices,
                                                                     std::vector<uint32_t> * const opc_StatusIndices)
const
{
   int32_t s32_Retval = C_NO_ERR;

   const C_OscNode * const pc_Node = this->GetOscNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      s32_Retval = pc_Node->c_HalcConfig.GetRelevantIndicesForSelectedUseCase(ou32_DomainIndex, ou32_ChannelIndex,
                                                                              oq_UseChannelIndex, opc_ParameterIndices,
                                                                              opc_InputIndices, opc_OutputIndices,
                                                                              opc_StatusIndices);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  HALC generate datapools

   \param[in]  ou32_NodeIndex    Node index

   \return
   C_NO_ERR Datapool generated
   C_NOACT  No datapool generated because nothing changed
   C_CONFIG Configuration invalid
   C_RANGE  Parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::HalcGenerateDatapools(const uint32_t ou32_NodeIndex)
{
   int32_t s32_Retval;

   if ((ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size()) &&
       (ou32_NodeIndex < this->mc_UiNodes.size()))
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<uint32_t, uint32_t>::const_iterator c_ItPrevHash = mc_PreviousHashes.find(ou32_NodeIndex);

      uint32_t u32_CurrentHash = 0xFFFFFFFFUL;
      rc_OscNode.c_HalcConfig.CalcHash(u32_CurrentHash);

      // check if HALC configuration changed since last HALC Datapool generation
      if ((c_ItPrevHash == mc_PreviousHashes.end()) || (c_ItPrevHash->second != u32_CurrentHash))
      {
         std::vector<C_OscNodeDataPool> c_Tmp;
         mc_PreviousHashes[ou32_NodeIndex] = u32_CurrentHash;

         // In case of existing HAL Datapools, safe Datapool specific properties
         for (uint32_t u32_DatapoolCounter = 0U; u32_DatapoolCounter < rc_OscNode.c_DataPools.size();
              ++u32_DatapoolCounter)
         {
            const C_OscNodeDataPool & rc_Datapool = rc_OscNode.c_DataPools[u32_DatapoolCounter];
            if ((rc_Datapool.e_Type == C_OscNodeDataPool::eHALC) ||
                (rc_Datapool.e_Type == C_OscNodeDataPool::eHALC_NVM))
            {
               c_Tmp.push_back(rc_Datapool);
            }
         }

         //Clean up existing datapools
         s32_Retval = this->HalcRemoveDatapools(ou32_NodeIndex, true);

         if (s32_Retval == C_NO_ERR)
         {
            //Check if new datapools should be generated
            if (rc_OscNode.c_HalcConfig.c_FileString.IsEmpty() == false)
            {
               std::vector<C_OscNodeDataPool> c_Datapools;
               const C_OscHalcMagicianGenerator c_Magician(&rc_OscNode);
               s32_Retval = c_Magician.GenerateHalcDatapools(c_Datapools);
               if (s32_Retval == C_NO_ERR)
               {
                  C_PuiSdNode & rc_UiNode = this->mc_UiNodes[ou32_NodeIndex];
                  tgl_assert(rc_OscNode.c_DataPools.size() == rc_UiNode.c_UiDataPools.size());
                  if (rc_OscNode.c_DataPools.size() == rc_UiNode.c_UiDataPools.size())
                  {
                     std::vector<C_PuiSdNodeDataPool> c_UiDatapools;

                     //Get ui datapools
                     for (uint32_t u32_It = 0UL; u32_It < c_Datapools.size(); ++u32_It)
                     {
                        const C_PuiSdNodeDataPool c_Tmp2 =
                           C_PuiSdHandlerHalc::mh_GetUiDatapoolForOscDataPool(c_Datapools[u32_It]);
                        c_UiDatapools.push_back(c_Tmp2);
                     }
                     //Add new datapools
                     tgl_assert(c_Datapools.size() == c_UiDatapools.size());
                     for (uint32_t u32_It = 0UL; u32_It < c_Datapools.size(); ++u32_It)
                     {
                        if (c_Tmp.size() == c_Datapools.size())
                        {
                           C_OscNodeDataPool & rc_Datapool = c_Datapools[u32_It];
                           C_OscNodeDataPool & rc_Tmp = c_Tmp[u32_It];
                           tgl_assert(rc_Tmp.c_Name == rc_Datapool.c_Name);
                           if (rc_Tmp.c_Name == rc_Datapool.c_Name)
                           {
                              rc_Datapool.q_ScopeIsPrivate = rc_Tmp.q_ScopeIsPrivate;
                              rc_Datapool.s32_RelatedDataBlockIndex = rc_Tmp.s32_RelatedDataBlockIndex;
                           }
                        }

                        rc_OscNode.c_DataPools.push_back(c_Datapools[u32_It]);
                        rc_UiNode.c_UiDataPools.push_back(c_UiDatapools[u32_It]);
                     }

                     if ((rc_OscNode.c_HalcConfig.IsClear() == false) &&
                         (rc_OscNode.c_HalcConfig.q_NvmBasedConfig == true))
                     {
                        // Adapt auto address mode here due to NVM HALC Datapools
                        rc_OscNode.q_DatapoolAutoNvmStartAddress = false;
                     }
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
            }
         }

         //Trigger sync
         Q_EMIT (this->SigSyncNodeHalc(ou32_NodeIndex));
      }
      else
      {
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  HALC remove datapools

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  oq_SuppressSyncSignal   Optional flag to suppress sync signal

   \return
   C_NO_ERR Datapools removed
   C_RANGE  Parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::HalcRemoveDatapools(const uint32_t ou32_NodeIndex, const bool oq_SuppressSyncSignal)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];

      std::vector<C_OscNodeDataPool>::const_iterator c_ItOsc = rc_OscNode.c_DataPools.begin();
      uint32_t u32_DatapoolIndex = 0;
      for (; c_ItOsc != rc_OscNode.c_DataPools.end();)
      {
         if ((c_ItOsc->e_Type == C_OscNodeDataPool::eHALC) || (c_ItOsc->e_Type == C_OscNodeDataPool::eHALC_NVM))
         {
            // Remove Datapool
            s32_Retval = this->RemoveDataPool(ou32_NodeIndex, u32_DatapoolIndex, oq_SuppressSyncSignal);
         }
         else
         {
            //Iterate
            ++c_ItOsc;
            ++u32_DatapoolIndex;
         }
      }

      if (oq_SuppressSyncSignal == false)
      {
         Q_EMIT (this->SigSyncNodeHalc(ou32_NodeIndex));
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  HALC reset data blocks of type parameter set generation

   \param[in]  ou32_NodeIndex    Node index

   \return
   C_NO_ERR Data Blocks reset
   C_RANGE  Parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::HalcResetDataBlocks(const uint32_t ou32_NodeIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      uint32_t u32_Counter;

      // reset applications of type parameter set to binary
      for (u32_Counter = 0U; u32_Counter < rc_OscNode.c_Applications.size(); ++u32_Counter)
      {
         C_OscNodeApplication & rc_App = rc_OscNode.c_Applications[u32_Counter];

         if (rc_App.e_Type == C_OscNodeApplication::ePARAMETER_SET_HALC)
         {
            Q_EMIT (this->SigSyncNodeApplicationAboutToBeChangedFromParamSetHalc(ou32_NodeIndex, u32_Counter));

            rc_App.e_Type = C_OscNodeApplication::eBINARY;
            rc_App.c_CodeGeneratorPath = "";
            rc_App.u16_GenCodeVersion = 0;
            rc_App.u8_ProcessId = 0;
            rc_App.c_GeneratePath = "";
            rc_App.c_ResultPaths.resize(1);
            rc_App.c_ResultPaths[0] = "";
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check HALC channel name available

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DomainIndex           Domain index
   \param[in]  orc_ChannelName            Channel name to check for
   \param[in]  opu32_ChannelIndexToSkip   Channel index to skip
                                          (Use-case: skip current channel to avoid conflict with itself)

   \return
   true  Available
   false Already in use
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerHalc::CheckHalcChannelNameAvailable(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                       const stw::scl::C_SclString & orc_ChannelName,
                                                       const uint32_t * const opu32_ChannelIndexToSkip) const
{
   bool q_Retval = true;

   const C_OscHalcConfigDomain * const pc_Domain = this->GetHalcDomainConfigDataConst(ou32_NodeIndex, ou32_DomainIndex);

   if (pc_Domain != NULL)
   {
      // compare channel name with all existing channel names
      for (uint32_t u32_ItChannel = 0; (u32_ItChannel < pc_Domain->c_ChannelConfigs.size()) && (q_Retval == true);
           ++u32_ItChannel)
      {
         bool q_Skip = false;
         if (opu32_ChannelIndexToSkip != NULL)
         {
            if (*opu32_ChannelIndexToSkip == u32_ItChannel)
            {
               q_Skip = true;
            }
         }
         if (q_Skip == false)
         {
            const C_OscHalcConfigChannel & rc_Channel = pc_Domain->c_ChannelConfigs[u32_ItChannel];
            if (rc_Channel.c_Name.LowerCase() == orc_ChannelName.LowerCase())
            {
               q_Retval = false;
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get ui datapool for OSC data pool

   \param[in]  orc_OscDatapool   OSC datapool

   \return
   Ui datapool for OSC data pool
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdNodeDataPool C_PuiSdHandlerHalc::mh_GetUiDatapoolForOscDataPool(const C_OscNodeDataPool & orc_OscDatapool)
{
   C_PuiSdNodeDataPool c_Retval;

   c_Retval.c_DataPoolLists.clear();

   for (uint32_t u32_It = 0UL; u32_It < orc_OscDatapool.c_Lists.size(); ++u32_It)
   {
      const C_OscNodeDataPoolList & rc_List = orc_OscDatapool.c_Lists[u32_It];
      C_PuiSdNodeDataPoolList c_List;
      c_List.c_DataPoolListElements.clear();
      for (uint32_t u32_ItElem = 0UL; u32_ItElem < rc_List.c_Elements.size(); ++u32_ItElem)
      {
         C_PuiSdNodeDataPoolListElement c_Elem;
         c_Elem.q_AutoMinMaxActive = false;
         c_Elem.q_InterpretAsString = false;
         c_List.c_DataPoolListElements.push_back(c_Elem);
      }
      c_Retval.c_DataPoolLists.push_back(c_List);
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get index in vector

   \param[in]   orc_Structs                  Structs
   \param[in]   ou32_StartingIndex           Starting index
   \param[in]   ou32_TargetIndex             Target index
   \param[out]  oru32_ParameterIndex         Parameter index
   \param[out]  orq_UseElementIndex          Use element index
   \param[out]  oru32_ParameterElementIndex  Parameter element index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerHalc::mh_GetIndexInVector(const std::vector<C_OscHalcDefStruct> & orc_Structs,
                                                const uint32_t ou32_StartingIndex, const uint32_t ou32_TargetIndex,
                                                uint32_t & oru32_ParameterIndex, bool & orq_UseElementIndex,
                                                uint32_t & oru32_ParameterElementIndex)
{
   int32_t s32_Retval = C_NO_ERR;
   uint32_t u32_Counter = ou32_StartingIndex;
   bool q_Found = false;

   for (uint32_t u32_ItItem = 0UL; (u32_ItItem < orc_Structs.size()) && (q_Found == false); ++u32_ItItem)
   {
      const uint32_t u32_StartingIndex = u32_Counter;
      u32_Counter += C_OscHalcMagicianDatapoolListHandler::h_CountElements(orc_Structs[u32_ItItem]);
      if (u32_Counter > ou32_TargetIndex)
      {
         const C_OscHalcDefStruct & rc_Struct = orc_Structs[u32_ItItem];
         oru32_ParameterIndex = u32_ItItem;
         if (rc_Struct.c_StructElements.size() > 0UL)
         {
            orq_UseElementIndex = true;
            oru32_ParameterElementIndex = ou32_TargetIndex - u32_StartingIndex;
         }
         else
         {
            orq_UseElementIndex = false;
            oru32_ParameterElementIndex = 0UL;
         }
         q_Found = true;
      }
   }
   if (q_Found == false)
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}
