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
#include "C_OSCHALCMagicianGenerator.h"

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

   \return
   NULL Data not found
   Else HALC config domain channel
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCHalcConfigChannel * C_PuiSdHandlerHALC::GetHALCDomainChannelConfigData(const uint32 ou32_NodeIndex,
                                                                                  const uint32 ou32_DomainIndex,
                                                                                  const uint32 ou32_ChannelIndex) const
{
   const C_OSCHalcConfigChannel * pc_Retval;
   const C_OSCHalcConfigDomain * const pc_Config = this->GetHALCDomainConfigDataConst(ou32_NodeIndex, ou32_DomainIndex);

   if (pc_Config != NULL)
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

   \return
   NULL Data not found
   Else HALC config domain channel parameter
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCHalcConfigParameterStruct * C_PuiSdHandlerHALC::GetHALCDomainChannelParameterConfigData(
   const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
   const uint32 ou32_ParameterIndex) const
{
   const C_OSCHalcConfigParameterStruct * pc_Retval;
   const C_OSCHalcConfigChannel * const pc_Config = this->GetHALCDomainChannelConfigData(ou32_NodeIndex,
                                                                                         ou32_DomainIndex,
                                                                                         ou32_ChannelIndex);

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
   const C_OSCHalcDefDomain * const pc_Config = this->GetHALCDomainFileDataConst(ou32_NodeIndex, ou32_DomainIndex);

   if (pc_Config != NULL)
   {
      switch (oe_Selector)
      {
      case C_OSCHalcDefDomain::eVA_PARAM:
         if (ou32_Index < pc_Config->c_Parameters.size())
         {
            pc_Retval = &pc_Config->c_Parameters[ou32_Index];
         }
         else
         {
            pc_Retval = NULL;
         }
         break;
      case C_OSCHalcDefDomain::eVA_INPUT:
         if (ou32_Index < pc_Config->c_InputValues.size())
         {
            pc_Retval = &pc_Config->c_InputValues[ou32_Index];
         }
         else
         {
            pc_Retval = NULL;
         }
         break;
      case C_OSCHalcDefDomain::eVA_OUTPUT:
         if (ou32_Index < pc_Config->c_OutputValues.size())
         {
            pc_Retval = &pc_Config->c_OutputValues[ou32_Index];
         }
         else
         {
            pc_Retval = NULL;
         }
         break;
      case C_OSCHalcDefDomain::eVA_STATUS:
         if (ou32_Index < pc_Config->c_StatusValues.size())
         {
            pc_Retval = &pc_Config->c_StatusValues[ou32_Index];
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
   \param[in]  oq_Active            Active
   \param[in]  orc_Name             Name
   \param[in]  orc_Comment          Comment
   \param[in]  oq_SafetyRelevant    Safety relevant
   \param[in]  oq_DatablockSet      Datablock set
   \param[in]  ou32_DatablockIndex  Datablock index
   \param[in]  oq_UseCaseSet        Use case set
   \param[in]  ou32_UseCaseIndex    Use case index
   \param[in]  orc_DatablockConfig  Datablock config

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfig(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                      const uint32 ou32_ChannelIndex, const bool oq_Active,
                                                      const stw_scl::C_SCLString & orc_Name,
                                                      const stw_scl::C_SCLString & orc_Comment,
                                                      const bool oq_SafetyRelevant, const bool oq_DatablockSet,
                                                      const uint32 ou32_DatablockIndex, const bool oq_UseCaseSet,
                                                      const uint32 ou32_UseCaseIndex,
                                                      const std::vector<C_OSCHalcConfigChannel::E_DataBlockConfig> & orc_DatablockConfig)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelConfig(ou32_DomainIndex, ou32_ChannelIndex, oq_Active, orc_Name,
                                                               orc_Comment, oq_SafetyRelevant, oq_DatablockSet,
                                                               ou32_DatablockIndex,
                                                               oq_UseCaseSet, ou32_UseCaseIndex, orc_DatablockConfig);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC domain channel config active

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_Active            Active

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfigActive(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                            const uint32 ou32_ChannelIndex, const bool oq_Active)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelConfigActive(ou32_DomainIndex, ou32_ChannelIndex, oq_Active);
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
   \param[in]  orc_Name             Name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfigName(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                          const uint32 ou32_ChannelIndex,
                                                          const stw_scl::C_SCLString & orc_Name)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelConfigName(ou32_DomainIndex, ou32_ChannelIndex, orc_Name);
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
   \param[in]  orc_Comment          Comment

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfigComment(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                             const uint32 ou32_ChannelIndex,
                                                             const stw_scl::C_SCLString & orc_Comment)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelConfigComment(ou32_DomainIndex, ou32_ChannelIndex,
                                                                      orc_Comment);
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
   \param[in]  oq_SafetyRelevant    Safety relevant

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfigSafety(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                            const uint32 ou32_ChannelIndex,
                                                            const bool oq_SafetyRelevant)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelConfigSafety(ou32_DomainIndex, ou32_ChannelIndex,
                                                                     oq_SafetyRelevant);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC domain channel config datablock

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_DatablockSet      Datablock set
   \param[in]  ou32_DatablockIndex  Datablock index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfigDatablock(const uint32 ou32_NodeIndex,
                                                               const uint32 ou32_DomainIndex,
                                                               const uint32 ou32_ChannelIndex,
                                                               const bool oq_DatablockSet,
                                                               const uint32 ou32_DatablockIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelConfigDatablock(ou32_DomainIndex, ou32_ChannelIndex,
                                                                        oq_DatablockSet,
                                                                        ou32_DatablockIndex);
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
   \param[in]  oq_UseCaseSet        Use case set
   \param[in]  ou32_UseCaseIndex    Use case index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfigUseCase(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                             const uint32 ou32_ChannelIndex, const bool oq_UseCaseSet,
                                                             const uint32 ou32_UseCaseIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelConfigUseCase(ou32_DomainIndex, ou32_ChannelIndex,
                                                                      oq_UseCaseSet, ou32_UseCaseIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC domain channel config datablock config

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  orc_DatablockConfig  Datablock config

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerHALC::SetHALCDomainChannelConfigDatablockConfig(const uint32 ou32_NodeIndex,
                                                                     const uint32 ou32_DomainIndex,
                                                                     const uint32 ou32_ChannelIndex,
                                                                     const std::vector<C_OSCHalcConfigChannel::E_DataBlockConfig> & orc_DatablockConfig)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelConfigDatablockConfig(ou32_DomainIndex, ou32_ChannelIndex,
                                                                              orc_DatablockConfig);
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
                                                               const C_OSCHalcConfigParameterStruct & orc_Parameter)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelParameterConfig(ou32_DomainIndex, ou32_ChannelIndex,
                                                                        ou32_ParameterIndex, orc_Parameter);
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
                                                                      const C_OSCHalcConfigParameter & orc_Parameter)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      s32_Retval = rc_Node.c_HALCConfig.SetDomainChannelParameterConfigElement(ou32_DomainIndex, ou32_ChannelIndex,
                                                                               ou32_ParameterIndex, ou32_ElementIndex,
                                                                               orc_Parameter);
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
   C_CONFIG Configuration invalid
   C_RANGE  Parameter invalid
   C_BUSY   No programmable application block found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerHALC::HALCGenerateDatapools(const uint32 ou32_NodeIndex)
{
   sint32 s32_Retval;

   if ((ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size()) &&
       (ou32_NodeIndex < this->mc_UINodes.size()))
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      C_PuiSdNode & rc_UiNode = this->mc_UINodes[ou32_NodeIndex];
      std::vector<C_OSCNodeDataPool> c_Datapools;
      C_OSCHALCMagicianGenerator c_Magician(&rc_OSCNode);
      s32_Retval = c_Magician.GenerateHALCDatapools(c_Datapools);
      if (s32_Retval == C_NO_ERR)
      {
         tgl_assert(rc_OSCNode.c_DataPools.size() == rc_UiNode.c_UIDataPools.size());
         if (rc_OSCNode.c_DataPools.size() == rc_UiNode.c_UIDataPools.size())
         {
            std::vector<C_PuiSdNodeDataPool> c_UiDatapools;
            std::vector<C_OSCNodeDataPool>::iterator c_ItOSC = rc_OSCNode.c_DataPools.begin();
            std::vector<C_PuiSdNodeDataPool>::iterator c_ItUi = rc_UiNode.c_UIDataPools.begin();
            //Clean up existing datapools
            for (; c_ItOSC != rc_OSCNode.c_DataPools.end();)
            {
               if (c_ItOSC->e_Type == C_OSCNodeDataPool::eHALC)
               {
                  //Erase
                  c_ItOSC = rc_OSCNode.c_DataPools.erase(c_ItOSC);
                  c_ItUi = rc_UiNode.c_UIDataPools.erase(c_ItUi);
               }
               else
               {
                  //Iterate
                  ++c_ItOSC;
                  ++c_ItUi;
               }
            }
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
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
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
