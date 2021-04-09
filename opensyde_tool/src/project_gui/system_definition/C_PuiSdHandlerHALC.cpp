//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC data interface

   HALC data interface

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_PuiSdHandlerHALC.h"
#include "C_OSCHALCMagicianUtil.h"
#include "C_OSCHALCMagicianGenerator.h"
#include "C_OSCHALCMagicianDatapoolListHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

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
C_PuiSdHandlerHALC::C_PuiSdHandlerHALC(QObject * const opc_Parent) :
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
const C_OSCHalcConfig * C_PuiSdHandlerHALC::GetHALCConfig(const uint32 ou32_NodeIndex) const
{
   const C_OSCHalcConfig * pc_Retval;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      pc_Retval = &rc_Node.c_HALCConfig;
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
const C_OSCHalcConfigDomain * C_PuiSdHandlerHALC::GetHALCDomainConfigDataConst(const uint32 ou32_NodeIndex,
                                                                               const uint32 ou32_DomainIndex) const
{
   const C_OSCHalcConfigDomain * pc_Retval;
   const C_OSCHalcConfig * const pc_Config = this->GetHALCConfig(ou32_NodeIndex);

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
const C_OSCHalcConfigChannel * C_PuiSdHandlerHALC::GetHALCDomainChannelConfigData(const uint32 ou32_NodeIndex,
                                                                                  const uint32 ou32_DomainIndex,
                                                                                  const uint32 ou32_ChannelIndex,
                                                                                  const bool oq_UseChannelIndex) const
{
   const C_OSCHalcConfigChannel * pc_Retval;
   const C_OSCHalcConfigDomain * const pc_Config = this->GetHALCDomainConfigDataConst(ou32_NodeIndex, ou32_DomainIndex);

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
const C_OSCHalcConfigParameterStruct * C_PuiSdHandlerHALC::GetHALCDomainChannelParameterConfigStructData(
   const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
   const uint32 ou32_ParameterIndex, const bool oq_UseChannelIndex) const
{
   const C_OSCHalcConfigParameterStruct * pc_Retval;
   const C_OSCHalcConfigChannel * const pc_Config = this->GetHALCDomainChannelConfigData(ou32_NodeIndex,
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
const C_OSCHalcDefDomain * C_PuiSdHandlerHALC::GetHALCDomainFileDataConst(const uint32 ou32_NodeIndex,
                                                                          const uint32 ou32_DomainIndex) const
{
   const C_OSCHalcDefDomain * pc_Retval;
   const C_OSCHalcConfig * const pc_Config = this->GetHALCConfig(ou32_NodeIndex);

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
const C_OSCHalcDefChannelDef * C_PuiSdHandlerHALC::GetHALCDomainFileChannelDataConst(const uint32 ou32_NodeIndex,
                                                                                     const uint32 ou32_DomainIndex,
                                                                                     const uint32 ou32_Channel) const
{
   const C_OSCHalcDefChannelDef * pc_Retval;
   const C_OSCHalcDefDomain * const pc_Config = this->GetHALCDomainFileDataConst(ou32_NodeIndex, ou32_DomainIndex);

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
const C_OSCHalcDefChannelUseCase * C_PuiSdHandlerHALC::GetHALCDomainFileUseCaseData(const uint32 ou32_NodeIndex,
                                                                                    const uint32 ou32_DomainIndex,
                                                                                    const uint32 ou32_UseCaseIndex)
const
{
   const C_OSCHalcDefChannelUseCase * pc_Retval;
   const C_OSCHalcDefDomain * const pc_Config = this->GetHALCDomainFileDataConst(ou32_NodeIndex, ou32_DomainIndex);

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
const C_OSCHalcDefChannelAvailability * C_PuiSdHandlerHALC::GetHALCDomainFileUseCaseAvailabilityData(
   const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex, const uint32 ou32_UseCaseIndex,
   const uint32 ou32_AvailabilityIndex) const
{
   const C_OSCHalcDefChannelAvailability * pc_Retval;
   const C_OSCHalcDefChannelUseCase * const pc_Config = this->GetHALCDomainFileUseCaseData(ou32_NodeIndex,
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
const C_OSCHalcDefStruct * C_PuiSdHandlerHALC::GetHALCDomainFileVariableData(const uint32 ou32_NodeIndex,
                                                                             const uint32 ou32_DomainIndex,
                                                                             const C_OSCHalcDefDomain::E_VariableSelector oe_Selector,
                                                                             const uint32 ou32_Index) const
{
   const C_OSCHalcDefStruct * pc_Retval;
   const C_OSCHalcDefDomain * const pc_DefDomain = this->GetHALCDomainFileDataConst(ou32_NodeIndex, ou32_DomainIndex);
   const C_OSCHalcConfigDomain * const pc_ConfigDomain = this->GetHALCDomainConfigDataConst(ou32_NodeIndex,
                                                                                            ou32_DomainIndex);

   if ((pc_DefDomain != NULL) && (pc_ConfigDomain != NULL))
   {
      //differentiate domain or channel values
      const C_OSCHalcDefChannelValues & rc_Values =
         (pc_ConfigDomain->c_ChannelConfigs.size() == 0UL) ?
         pc_DefDomain->c_DomainValues : pc_DefDomain->c_ChannelValues;

      switch (oe_Selector)
      {
      case C_OSCHalcDefDomain::eVA_PARAM:
         if (ou32_Index < rc_Values.c_Parameters.size())
         {
            pc_Retval = &rc_Values.c_Parameters[ou32_Index];
         }
         else
         {
            pc_Retval = NULL;
         }
         break;
      case C_OSCHalcDefDomain::eVA_INPUT:
         if (ou32_Index < rc_Values.c_InputValues.size())
         {
            pc_Retval = &rc_Values.c_InputValues[ou32_Index];
         }
         else
         {
            pc_Retval = NULL;
         }
         break;
      case C_OSCHalcDefDomain::eVA_OUTPUT:
         if (ou32_Index < rc_Values.c_OutputValues.size())
         {
            pc_Retval = &rc_Values.c_OutputValues[ou32_Index];
         }
         else
         {
            pc_Retval = NULL;
         }
         break;
      case C_OSCHalcDefDomain::eVA_STATUS:
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
/*! \brief  Get HALC datapool

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oq_SafeDatapool   Flag to differ HAL safe datapool and HAL nonsafe datapool

   \return
   Found: Pointer to OSC data pool of type HAL and safety type oq_SafeDatapool
   Else:  NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPool * C_PuiSdHandlerHALC::GetHALCDatapool(const uint32 ou32_NodeIndex,
                                                              const bool oq_SafeDatapool) const
{
   const C_OSCNodeDataPool * pc_Retval = NULL;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      pc_Retval = rc_OSCNode.GetHalDataPoolConst(oq_SafeDatapool);
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
C_OSCHalcDefDomain::E_Category C_PuiSdHandlerHALC::GetDomainCategoryFromDpId(
   const C_OSCNodeDataPoolListElementId & orc_Id, const uint32 ou32_ArrayIndex)
{
   uint32 u32_DomainIndex = 0;
   bool q_UseChannelIndex;
   uint32 u32_ChannelIndex;

   C_OSCHalcDefDomain::E_VariableSelector e_Selector;
   uint32 u32_ParameterIndex;
   bool q_UseElementIndex;
   uint32 u32_ParameterElementIndex;
   bool q_IsUseCaseIndex;
   bool q_IsChanNumIndex;

   C_OSCHalcDefDomain::E_Category e_Category = C_OSCHalcDefDomain::eCA_OTHER;

   if (this->TranslateToHALCIndex(orc_Id, ou32_ArrayIndex, u32_DomainIndex, q_UseChannelIndex, u32_ChannelIndex,
                                  e_Selector,
                                  u32_ParameterIndex, q_UseElementIndex, u32_ParameterElementIndex, q_IsUseCaseIndex,
                                  q_IsChanNumIndex) == C_NO_ERR)
   {
      const C_OSCHalcDefDomain * const pc_Domain = this->GetHALCDomainFileDataConst(orc_Id.u32_NodeIndex,
                                                                                    u32_DomainIndex);
      if (pc_Domain != NULL)
      {
         e_Category = pc_Domain->e_Category;

         // we don't need those here
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
sint32 C_PuiSdHandlerHALC::ClearHALCConfig(const uint32 ou32_NodeIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      rc_Node.c_HALCConfig.Clear();
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
sint32 C_PuiSdHandlerHALC::SetHALCConfig(const uint32 ou32_NodeIndex, const C_OSCHalcConfig & orc_Config)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      rc_Node.c_HALCConfig = orc_Config;
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
sint32 C_PuiSdHandlerHALC::SetHALCDomainConfig(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                               const C_OSCHalcConfigDomain & orc_Domain)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainConfig(ou32_DomainIndex, orc_Domain);
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
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfig(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                      const uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                      const C_OSCHalcConfigChannel & orc_Channel)
{
   return this->SetHALCDomainChannelConfig(ou32_NodeIndex, ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex,
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
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfig(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                      const uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                      const stw_scl::C_SCLString & orc_Name,
                                                      const stw_scl::C_SCLString & orc_Comment,
                                                      const bool oq_SafetyRelevant, const uint32 ou32_UseCaseIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelConfig(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex,
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
sint32 C_PuiSdHandlerHALC::ResetHALCDomainChannelConfig(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                        const uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval =
         rc_Node.c_HALCConfig.ResetDomainChannelConfig(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex);
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
sint32 C_PuiSdHandlerHALC::ResetHALCDomainChannelUseCase(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                         const uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval =
         rc_Node.c_HALCConfig.ResetDomainChannelUseCase(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex);
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
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfigName(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                          const uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                          const stw_scl::C_SCLString & orc_Name)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelConfigName(ou32_DomainIndex, ou32_ChannelIndex,
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
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfigComment(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                             const uint32 ou32_ChannelIndex,
                                                             const bool oq_UseChannelIndex,
                                                             const stw_scl::C_SCLString & orc_Comment)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelConfigComment(ou32_DomainIndex, ou32_ChannelIndex,
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
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfigSafety(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                            const uint32 ou32_ChannelIndex,
                                                            const bool oq_UseChannelIndex, const bool oq_SafetyRelevant)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelConfigSafety(ou32_DomainIndex, ou32_ChannelIndex,
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
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfigUseCase(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                             const uint32 ou32_ChannelIndex,
                                                             const bool oq_UseChannelIndex,
                                                             const uint32 ou32_UseCaseIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelConfigUseCase(ou32_DomainIndex, ou32_ChannelIndex,
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
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelParameterConfig(const uint32 ou32_NodeIndex,
                                                               const uint32 ou32_DomainIndex,
                                                               const uint32 ou32_ChannelIndex,
                                                               const uint32 ou32_ParameterIndex,
                                                               const bool oq_UseChannelIndex,
                                                               const C_OSCHalcConfigParameterStruct & orc_Parameter)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelParameterConfig(ou32_DomainIndex, ou32_ChannelIndex,
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
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelParameterConfigElement(const uint32 ou32_NodeIndex,
                                                                      const uint32 ou32_DomainIndex,
                                                                      const uint32 ou32_ChannelIndex,
                                                                      const uint32 ou32_ParameterIndex,
                                                                      const uint32 ou32_ElementIndex,
                                                                      const bool oq_UseChannelIndex,
                                                                      const C_OSCHalcConfigParameter & orc_Parameter)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelParameterConfigElement(ou32_DomainIndex, ou32_ChannelIndex,
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
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelParameterConfigElementPlain(const uint32 ou32_NodeIndex,
                                                                           const uint32 ou32_DomainIndex,
                                                                           const uint32 ou32_ChannelIndex,
                                                                           const uint32 ou32_ParameterIndex,
                                                                           const uint32 ou32_ElementIndex,
                                                                           const bool oq_UseChannelIndex,
                                                                           const C_OSCHalcDefContent & orc_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelParameterConfigElementPlain(ou32_DomainIndex, ou32_ChannelIndex,
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
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelParameterConfigElementEnum(const uint32 ou32_NodeIndex,
                                                                          const uint32 ou32_DomainIndex,
                                                                          const uint32 ou32_ChannelIndex,
                                                                          const uint32 ou32_ParameterIndex,
                                                                          const uint32 ou32_ElementIndex,
                                                                          const bool oq_UseChannelIndex,
                                                                          const stw_scl::C_SCLString & orc_DisplayName)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelParameterConfigElementEnum(ou32_DomainIndex, ou32_ChannelIndex,
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
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelParameterConfigElementBitmask(const uint32 ou32_NodeIndex,
                                                                             const uint32 ou32_DomainIndex,
                                                                             const uint32 ou32_ChannelIndex,
                                                                             const uint32 ou32_ParameterIndex,
                                                                             const uint32 ou32_ElementIndex,
                                                                             const bool oq_UseChannelIndex,
                                                                             const stw_scl::C_SCLString & orc_DisplayName,
                                                                             const bool oq_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelParameterConfigElementBitmask(ou32_DomainIndex,
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
sint32 C_PuiSdHandlerHALC::GetHalChannelOrDomainName(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                     const uint32 ou32_Channel, QString & orc_Name) const
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCHalcDefChannelDef * const pc_ChannelDef = this->GetHALCDomainFileChannelDataConst(ou32_NodeIndex,
                                                                                                ou32_DomainIndex,
                                                                                                ou32_Channel);

   if (pc_ChannelDef != NULL)
   {
      orc_Name = pc_ChannelDef->c_Name.c_str();
   }
   else
   {
      const C_OSCHalcConfigDomain * const pc_Domain = this->GetHALCDomainConfigDataConst(ou32_NodeIndex,
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

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerHALC::TranslateToHALCIndex(const C_OSCNodeDataPoolListElementId & orc_Id,
                                                const uint32 ou32_ArrayIndex, uint32 & oru32_DomainIndex,
                                                bool & orq_UseChannelIndex, uint32 & oru32_ChannelIndex,
                                                C_OSCHalcDefDomain::E_VariableSelector & ore_Selector,
                                                uint32 & oru32_ParameterIndex, bool & orq_UseElementIndex,
                                                uint32 & oru32_ParameterElementIndex, bool & orq_IsUseCaseIndex,
                                                bool & orq_IsChanNumIndex) const
{
   sint32 s32_Retval = C_NO_ERR;

   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(orc_Id.u32_NodeIndex);
   const C_OSCNodeDataPool * const pc_Dp = this->GetOSCDataPool(orc_Id.u32_NodeIndex, orc_Id.u32_DataPoolIndex);

   switch (orc_Id.u32_ListIndex)
   {
   case 0UL:
      ore_Selector = C_OSCHalcDefDomain::eVA_PARAM;
      break;
   case 1UL:
      ore_Selector = C_OSCHalcDefDomain::eVA_INPUT;
      break;
   case 2UL:
      ore_Selector = C_OSCHalcDefDomain::eVA_OUTPUT;
      break;
   case 3UL:
      ore_Selector = C_OSCHalcDefDomain::eVA_STATUS;
      break;
   default:
      ore_Selector = C_OSCHalcDefDomain::eVA_PARAM;
      break;
   }

   if ((pc_Node != NULL) && (pc_Dp != NULL))
   {
      bool q_Found = false;
      bool q_FoundChannelIndex = false;
      uint32 u32_ListIndex = 0UL;

      //Init
      oru32_ChannelIndex = 0UL;

      for (uint32 u32_ItDomain = 0UL; (u32_ItDomain < pc_Node->c_HALCConfig.GetDomainSize()) && (q_Found == false);
           ++u32_ItDomain)
      {
         const C_OSCHalcDefDomain * const pc_CurrentDomainDef =
            pc_Node->c_HALCConfig.GetDomainDefDataConst(u32_ItDomain);
         const C_OSCHalcConfigDomain * const pc_CurrentDomainConfig = pc_Node->c_HALCConfig.GetDomainConfigDataConst(
            u32_ItDomain);
         if ((pc_CurrentDomainDef != NULL) && (pc_CurrentDomainConfig != NULL))
         {
            const uint32 u32_CountRelevantItems = C_OSCHALCMagicianDatapoolListHandler::h_CountRelevantItems(
               pc_CurrentDomainConfig->c_ChannelConfigs, pc_CurrentDomainConfig->c_DomainConfig, pc_Dp->q_IsSafety);
            //other values
            if (u32_CountRelevantItems > 0UL)
            {
               //channels
               if (ore_Selector == C_OSCHalcDefDomain::eVA_PARAM)
               {
                  if (C_OSCHALCMagicianUtil::h_CheckChanNumVariableNecessary(*pc_CurrentDomainConfig))
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
                        //Stop
                        q_Found = true;
                     }
                     u32_ListIndex += 1UL;
                  }
               }

               if (q_Found == false)
               {
                  //usecase
                  if (ore_Selector == C_OSCHalcDefDomain::eVA_PARAM)
                  {
                     if (C_OSCHALCMagicianUtil::h_CheckUseCaseVariableNecessary(*pc_CurrentDomainConfig))
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
                           //Stop
                           q_Found = true;
                        }
                        u32_ListIndex += 1UL;
                     }
                  }
                  if (q_Found == false)
                  {
                     const uint32 u32_StartingIndex = u32_ListIndex;
                     //others
                     if (pc_CurrentDomainConfig->c_ChannelConfigs.size() == 0UL)
                     {
                        //domain values
                        switch (ore_Selector)
                        {
                        case C_OSCHalcDefDomain::eVA_PARAM:
                           u32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::h_CountElements(
                              pc_CurrentDomainDef->c_DomainValues.c_Parameters);
                           break;
                        case C_OSCHalcDefDomain::eVA_INPUT:
                           u32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::h_CountElements(
                              pc_CurrentDomainDef->c_DomainValues.c_InputValues);
                           break;
                        case C_OSCHalcDefDomain::eVA_OUTPUT:
                           u32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::h_CountElements(
                              pc_CurrentDomainDef->c_DomainValues.c_OutputValues);
                           break;
                        case C_OSCHalcDefDomain::eVA_STATUS:
                           u32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::h_CountElements(
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
                        case C_OSCHalcDefDomain::eVA_PARAM:
                           u32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::h_CountElements(
                              pc_CurrentDomainDef->c_ChannelValues.c_Parameters);
                           break;
                        case C_OSCHalcDefDomain::eVA_INPUT:
                           u32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::h_CountElements(
                              pc_CurrentDomainDef->c_ChannelValues.c_InputValues);
                           break;
                        case C_OSCHalcDefDomain::eVA_OUTPUT:
                           u32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::h_CountElements(
                              pc_CurrentDomainDef->c_ChannelValues.c_OutputValues);
                           break;
                        case C_OSCHalcDefDomain::eVA_STATUS:
                           u32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::h_CountElements(
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
                        //Parameter index
                        if (pc_CurrentDomainConfig->c_ChannelConfigs.size() == 0UL)
                        {
                           //domain values
                           switch (ore_Selector)
                           {
                           case C_OSCHalcDefDomain::eVA_PARAM:
                              s32_Retval = C_PuiSdHandlerHALC::mh_GetIndexInVector(
                                 pc_CurrentDomainDef->c_DomainValues.c_Parameters, u32_StartingIndex,
                                 orc_Id.u32_ElementIndex,
                                 oru32_ParameterIndex,
                                 orq_UseElementIndex,
                                 oru32_ParameterElementIndex);
                              break;
                           case C_OSCHalcDefDomain::eVA_INPUT:
                              s32_Retval = C_PuiSdHandlerHALC::mh_GetIndexInVector(
                                 pc_CurrentDomainDef->c_DomainValues.c_InputValues, u32_StartingIndex,
                                 orc_Id.u32_ElementIndex,
                                 oru32_ParameterIndex,
                                 orq_UseElementIndex,
                                 oru32_ParameterElementIndex);
                              break;
                           case C_OSCHalcDefDomain::eVA_OUTPUT:
                              s32_Retval = C_PuiSdHandlerHALC::mh_GetIndexInVector(
                                 pc_CurrentDomainDef->c_DomainValues.c_OutputValues, u32_StartingIndex,
                                 orc_Id.u32_ElementIndex,
                                 oru32_ParameterIndex,
                                 orq_UseElementIndex,
                                 oru32_ParameterElementIndex);
                              break;
                           case C_OSCHalcDefDomain::eVA_STATUS:
                              s32_Retval = C_PuiSdHandlerHALC::mh_GetIndexInVector(
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
                           case C_OSCHalcDefDomain::eVA_PARAM:
                              s32_Retval = C_PuiSdHandlerHALC::mh_GetIndexInVector(
                                 pc_CurrentDomainDef->c_ChannelValues.c_Parameters, u32_StartingIndex,
                                 orc_Id.u32_ElementIndex,
                                 oru32_ParameterIndex,
                                 orq_UseElementIndex,
                                 oru32_ParameterElementIndex);
                              break;
                           case C_OSCHalcDefDomain::eVA_INPUT:
                              s32_Retval = C_PuiSdHandlerHALC::mh_GetIndexInVector(
                                 pc_CurrentDomainDef->c_ChannelValues.c_InputValues, u32_StartingIndex,
                                 orc_Id.u32_ElementIndex,
                                 oru32_ParameterIndex,
                                 orq_UseElementIndex,
                                 oru32_ParameterElementIndex);
                              break;
                           case C_OSCHalcDefDomain::eVA_OUTPUT:
                              s32_Retval = C_PuiSdHandlerHALC::mh_GetIndexInVector(
                                 pc_CurrentDomainDef->c_ChannelValues.c_OutputValues, u32_StartingIndex,
                                 orc_Id.u32_ElementIndex,
                                 oru32_ParameterIndex,
                                 orq_UseElementIndex,
                                 oru32_ParameterElementIndex);
                              break;
                           case C_OSCHalcDefDomain::eVA_STATUS:
                              s32_Retval = C_PuiSdHandlerHALC::mh_GetIndexInVector(
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
               //Channel Index
               if (q_Found)
               {
                  uint32 u32_CurNum = 0UL;
                  if (pc_CurrentDomainConfig->c_ChannelConfigs.size() > 0UL)
                  {
                     for (uint32 u32_ItChannel = 0UL; u32_ItChannel < pc_CurrentDomainConfig->c_ChannelConfigs.size();
                          ++u32_ItChannel)
                     {
                        const C_OSCHalcConfigChannel & rc_Channel =
                           pc_CurrentDomainConfig->c_ChannelConfigs[u32_ItChannel];
                        if (rc_Channel.q_SafetyRelevant == pc_Dp->q_IsSafety)
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
sint32 C_PuiSdHandlerHALC::IsHalcClear(const uint32 ou32_NodeIndex, bool & orq_IsClear)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      orq_IsClear = rc_Node.c_HALCConfig.IsClear();
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
sint32 C_PuiSdHandlerHALC::CheckHALCDomainChannelLinked(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                        const uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                        bool & orq_IsLinked,
                                                        std::vector<QString> * const opc_LinkedChannelNames,
                                                        std::vector<uint32> * const opc_LinkedChannelIndices,
                                                        const uint32 * const opu32_UseCaseIndex) const
{
   sint32 s32_Retval = C_RANGE;
   const C_OSCHalcConfigDomain * const pc_Domain = this->GetHALCDomainConfigDataConst(ou32_NodeIndex, ou32_DomainIndex);

   if (pc_Domain != NULL)
   {
      std::vector<stw_scl::C_SCLString> c_LinkedChannelNames;
      s32_Retval = pc_Domain->CheckChannelLinked(ou32_ChannelIndex, oq_UseChannelIndex, orq_IsLinked,
                                                 &c_LinkedChannelNames, opc_LinkedChannelIndices, opu32_UseCaseIndex);
      if (opc_LinkedChannelNames != NULL)
      {
         for (std::vector<stw_scl::C_SCLString>::const_iterator c_ItNames = c_LinkedChannelNames.begin();
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
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfigOfLinkedChannels(const uint32 ou32_NodeIndex,
                                                                      const uint32 ou32_DomainIndex,
                                                                      const uint32 ou32_ChannelIndex,
                                                                      const bool oq_UseChannelIndex,
                                                                      const uint32 ou32_UseCaseIndexOld,
                                                                      const uint32 ou32_UseCaseIndexNew)
{
   sint32 s32_Retval = C_NO_ERR;

   // linking is a channel feature only
   if (oq_UseChannelIndex == true)
   {
      bool q_IsLinkedOld = false;
      bool q_IsLinkedNew = false;
      std::vector<uint32> c_LinkedChannelIndicesOld;
      std::vector<uint32> c_LinkedChannelIndicesNew;

      // get information about linked channels
      s32_Retval = this->CheckHALCDomainChannelLinked(ou32_NodeIndex, ou32_DomainIndex, ou32_ChannelIndex,
                                                      oq_UseChannelIndex, q_IsLinkedOld, NULL,
                                                      &c_LinkedChannelIndicesOld, &ou32_UseCaseIndexOld);

      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = this->CheckHALCDomainChannelLinked(ou32_NodeIndex, ou32_DomainIndex, ou32_ChannelIndex,
                                                         oq_UseChannelIndex, q_IsLinkedNew, NULL,
                                                         &c_LinkedChannelIndicesNew, &ou32_UseCaseIndexNew);
      }

      if ((s32_Retval == C_NO_ERR) && ((q_IsLinkedOld == true) || (q_IsLinkedNew == true)))
      {
         // first revert use case of all previously linked channels
         if (q_IsLinkedOld == true)
         {
            for (std::vector<uint32>::const_iterator c_ItChannels = c_LinkedChannelIndicesOld.begin();
                 (c_ItChannels != c_LinkedChannelIndicesOld.end()) && (s32_Retval == C_NO_ERR); ++c_ItChannels)
            {
               s32_Retval = this->ResetHALCDomainChannelUseCase(ou32_NodeIndex, ou32_DomainIndex,
                                                                *c_ItChannels, true);
            }
         }

         // then set use case of all new linked channels
         if ((s32_Retval == C_NO_ERR) && (q_IsLinkedNew == true))
         {
            const C_OSCHalcConfigChannel * const pc_Channel = this->GetHALCDomainChannelConfigData(ou32_NodeIndex,
                                                                                                   ou32_DomainIndex,
                                                                                                   ou32_ChannelIndex,
                                                                                                   oq_UseChannelIndex);
            if (pc_Channel != NULL)
            {
               for (std::vector<uint32>::const_iterator c_ItChannels = c_LinkedChannelIndicesNew.begin();
                    (c_ItChannels != c_LinkedChannelIndicesNew.end()) && (s32_Retval == C_NO_ERR); ++c_ItChannels)
               {
                  s32_Retval = this->SetHALCDomainChannelConfigUseCase(ou32_NodeIndex, ou32_DomainIndex,
                                                                       *c_ItChannels, true, ou32_UseCaseIndexNew);

                  // update all parameters in linked channel
                  for (uint32 u32_ItParams = 0;
                       (u32_ItParams < pc_Channel->c_Parameters.size()) && (s32_Retval == C_NO_ERR); u32_ItParams++)
                  {
                     const C_OSCHalcConfigParameterStruct & rc_Parameter = pc_Channel->c_Parameters[u32_ItParams];

                     s32_Retval = this->SetHALCDomainChannelParameterConfig(ou32_NodeIndex, ou32_DomainIndex,
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
sint32 C_PuiSdHandlerHALC::CheckHALCDomainChannelError(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                       const uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                       bool & orq_HasError) const
{
   sint32 s32_Retval = C_NO_ERR;

   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      bool q_DomainInvalid;
      bool q_Tmp;
      std::vector<uint32> c_InvalidChannelIndices;
      pc_Node->c_HALCConfig.CheckDomainConfigValid(ou32_DomainIndex, &q_DomainInvalid, &q_Tmp,
                                                   &c_InvalidChannelIndices);
      if (oq_UseChannelIndex == true)
      {
         orq_HasError = false;
         for (uint32 u32_Inv = 0UL; u32_Inv < c_InvalidChannelIndices.size(); ++u32_Inv)
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
sint32 C_PuiSdHandlerHALC::GetHALCRelevantIndicesForSelectedUseCase(const uint32 ou32_NodeIndex,
                                                                    const uint32 ou32_DomainIndex,
                                                                    const uint32 ou32_ChannelIndex,
                                                                    const bool oq_UseChannelIndex,
                                                                    std::vector<uint32> * const opc_ParameterIndices,
                                                                    std::vector<uint32> * const opc_InputIndices,
                                                                    std::vector<uint32> * const opc_OutputIndices,
                                                                    std::vector<uint32> * const opc_StatusIndices) const
{
   sint32 s32_Retval = C_NO_ERR;

   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      s32_Retval = pc_Node->c_HALCConfig.GetRelevantIndicesForSelectedUseCase(ou32_DomainIndex, ou32_ChannelIndex,
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
sint32 C_PuiSdHandlerHALC::HALCGenerateDatapools(const uint32 ou32_NodeIndex)
{
   sint32 s32_Retval;

   if ((ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size()) &&
       (ou32_NodeIndex < this->mc_UINodes.size()))
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      static std::map<uint32, uint32> hc_PreviousHashes; // key: node index, value: previous hash value
      const std::map<uint32, uint32>::const_iterator c_ItPrevHash = hc_PreviousHashes.find(ou32_NodeIndex);

      uint32 u32_CurrentHash = 0xFFFFFFFFUL;
      rc_OSCNode.c_HALCConfig.CalcHash(u32_CurrentHash);

      // check if HALC configuration changed since last HALC Datapool generation
      if ((c_ItPrevHash == hc_PreviousHashes.end()) || (c_ItPrevHash->second != u32_CurrentHash))
      {
         hc_PreviousHashes[ou32_NodeIndex] = u32_CurrentHash;

         uint32 u32_DatapoolCounter;
         stw_types::uint8 au8_SafeVersion[3] = {0};
         stw_scl::C_SCLString c_SafeComment;
         bool q_SafeScopeIsPrivate = true;
         stw_types::sint32 s32_SafeDatablockIndex = -1;
         bool q_SafeFound = false;

         stw_types::uint8 au8_NonsafeVersion[3] = {0};
         stw_scl::C_SCLString c_NonsafeComment;
         bool q_NonsafeScopeIsPrivate = true;
         stw_types::sint32 s32_NonsafeDatablockIndex = -1;
         bool q_NonsafeFound = false;

         // In case of existing HAL Datapools, safe Datapool specific properties
         for (u32_DatapoolCounter = 0U; u32_DatapoolCounter < rc_OSCNode.c_DataPools.size(); ++u32_DatapoolCounter)
         {
            const C_OSCNodeDataPool & rc_Datapool = rc_OSCNode.c_DataPools[u32_DatapoolCounter];
            if (rc_Datapool.e_Type == C_OSCNodeDataPool::eHALC)
            {
               if (rc_Datapool.q_IsSafety == true)
               {
                  memcpy(au8_SafeVersion, rc_Datapool.au8_Version, sizeof(au8_SafeVersion));
                  c_SafeComment = rc_Datapool.c_Comment;
                  q_SafeScopeIsPrivate = rc_Datapool.q_ScopeIsPrivate;
                  s32_SafeDatablockIndex = rc_Datapool.s32_RelatedDataBlockIndex;
                  q_SafeFound = true;
               }
               else
               {
                  memcpy(au8_NonsafeVersion, rc_Datapool.au8_Version, sizeof(au8_NonsafeVersion));
                  c_NonsafeComment = rc_Datapool.c_Comment;
                  q_NonsafeScopeIsPrivate = rc_Datapool.q_ScopeIsPrivate;
                  s32_NonsafeDatablockIndex = rc_Datapool.s32_RelatedDataBlockIndex;
                  q_NonsafeFound = true;
               }
            }

            if ((q_SafeFound == true) && (q_NonsafeFound == true))
            {
               break;
            }
         }

         //Clean up existing datapools
         s32_Retval = this->HALCRemoveDatapools(ou32_NodeIndex, true);

         if (s32_Retval == C_NO_ERR)
         {
            //Check if new datapools should be generated
            if (rc_OSCNode.c_HALCConfig.c_FileString.IsEmpty() == false)
            {
               std::vector<C_OSCNodeDataPool> c_Datapools;
               const C_OSCHALCMagicianGenerator c_Magician(&rc_OSCNode);
               s32_Retval = c_Magician.GenerateHALCDatapools(c_Datapools);
               if (s32_Retval == C_NO_ERR)
               {
                  C_PuiSdNode & rc_UiNode = this->mc_UINodes[ou32_NodeIndex];
                  tgl_assert(rc_OSCNode.c_DataPools.size() == rc_UiNode.c_UIDataPools.size());
                  if (rc_OSCNode.c_DataPools.size() == rc_UiNode.c_UIDataPools.size())
                  {
                     std::vector<C_PuiSdNodeDataPool> c_UiDatapools;

                     //Get ui datapools
                     for (uint32 u32_It = 0UL; u32_It < c_Datapools.size(); ++u32_It)
                     {
                        const C_PuiSdNodeDataPool c_Tmp =
                           C_PuiSdHandlerHALC::mh_GetUiDatapoolForOSCDataPool(c_Datapools[u32_It]);
                        c_UiDatapools.push_back(c_Tmp);
                     }
                     //Add new datapools
                     tgl_assert(c_Datapools.size() == c_UiDatapools.size());
                     for (uint32 u32_It = 0UL; u32_It < c_Datapools.size(); ++u32_It)
                     {
                        C_OSCNodeDataPool & rc_Datapool = c_Datapools[u32_It];
                        if (rc_Datapool.e_Type == C_OSCNodeDataPool::eHALC)
                        {
                           // Apply the saved Datapool properties
                           if ((q_NonsafeFound == true) &&
                               (rc_Datapool.q_IsSafety == false))
                           {
                              memcpy(rc_Datapool.au8_Version, au8_NonsafeVersion, sizeof(au8_NonsafeVersion));
                              rc_Datapool.c_Comment = c_NonsafeComment;
                              rc_Datapool.q_ScopeIsPrivate = q_NonsafeScopeIsPrivate;
                              rc_Datapool.s32_RelatedDataBlockIndex = s32_NonsafeDatablockIndex;
                           }
                           else if ((q_SafeFound == true) &&
                                    (rc_Datapool.q_IsSafety == true))
                           {
                              memcpy(rc_Datapool.au8_Version, au8_SafeVersion, sizeof(au8_SafeVersion));
                              rc_Datapool.c_Comment = c_SafeComment;
                              rc_Datapool.q_ScopeIsPrivate = q_SafeScopeIsPrivate;
                              rc_Datapool.s32_RelatedDataBlockIndex = s32_SafeDatablockIndex;
                           }
                           else
                           {
                              // Nothing to do
                           }
                        }

                        rc_OSCNode.c_DataPools.push_back(c_Datapools[u32_It]);
                        rc_UiNode.c_UIDataPools.push_back(c_UiDatapools[u32_It]);
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
         Q_EMIT (this->SigSyncNodeHALC(ou32_NodeIndex));
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
sint32 C_PuiSdHandlerHALC::HALCRemoveDatapools(const uint32 ou32_NodeIndex, const bool oq_SuppressSyncSignal)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];

      std::vector<C_OSCNodeDataPool>::const_iterator c_ItOSC = rc_OSCNode.c_DataPools.begin();
      uint32 u32_DatapoolIndex = 0;
      for (; c_ItOSC != rc_OSCNode.c_DataPools.end();)
      {
         if (c_ItOSC->e_Type == C_OSCNodeDataPool::eHALC)
         {
            // Remove Datapool
            s32_Retval = this->RemoveDataPool(ou32_NodeIndex, u32_DatapoolIndex, oq_SuppressSyncSignal);
         }
         else
         {
            //Iterate
            ++c_ItOSC;
            ++u32_DatapoolIndex;
         }
      }
      if (oq_SuppressSyncSignal == false)
      {
         Q_EMIT (this->SigSyncNodeHALC(ou32_NodeIndex));
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
bool C_PuiSdHandlerHALC::CheckHALCChannelNameAvailable(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                       const stw_scl::C_SCLString & orc_ChannelName,
                                                       const uint32 * const opu32_ChannelIndexToSkip) const
{
   bool q_Retval = true;

   const C_OSCHalcConfigDomain * const pc_Domain = this->GetHALCDomainConfigDataConst(ou32_NodeIndex, ou32_DomainIndex);

   if (pc_Domain != NULL)
   {
      // compare channel name with all existing channel names
      for (uint32 u32_ItChannel = 0; (u32_ItChannel < pc_Domain->c_ChannelConfigs.size()) && (q_Retval == true);
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
            const C_OSCHalcConfigChannel & rc_Channel = pc_Domain->c_ChannelConfigs[u32_ItChannel];
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

   \param[in]  orc_OSCDatapool   OSC datapool

   \return
   Ui datapool for OSC data pool
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdNodeDataPool C_PuiSdHandlerHALC::mh_GetUiDatapoolForOSCDataPool(const C_OSCNodeDataPool & orc_OSCDatapool)
{
   C_PuiSdNodeDataPool c_Retval;

   c_Retval.c_DataPoolLists.clear();

   for (uint32 u32_It = 0UL; u32_It < orc_OSCDatapool.c_Lists.size(); ++u32_It)
   {
      const C_OSCNodeDataPoolList & rc_List = orc_OSCDatapool.c_Lists[u32_It];
      C_PuiSdNodeDataPoolList c_List;
      c_List.c_DataPoolListElements.clear();
      for (uint32 u32_ItElem = 0UL; u32_ItElem < rc_List.c_Elements.size(); ++u32_ItElem)
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
sint32 C_PuiSdHandlerHALC::mh_GetIndexInVector(const std::vector<C_OSCHalcDefStruct> & orc_Structs,
                                               const uint32 ou32_StartingIndex, const uint32 ou32_TargetIndex,
                                               uint32 & oru32_ParameterIndex, bool & orq_UseElementIndex,
                                               uint32 & oru32_ParameterElementIndex)
{
   sint32 s32_Retval = C_NO_ERR;
   uint32 u32_Counter = ou32_StartingIndex;
   bool q_Found = false;

   for (uint32 u32_ItItem = 0UL; (u32_ItItem < orc_Structs.size()) && (q_Found == false); ++u32_ItItem)
   {
      const uint32 u32_StartingIndex = u32_Counter;
      u32_Counter += C_OSCHALCMagicianDatapoolListHandler::h_CountElements(orc_Structs[u32_ItItem]);
      if (u32_Counter > ou32_TargetIndex)
      {
         const C_OSCHalcDefStruct & rc_Struct = orc_Structs[u32_ItItem];
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
