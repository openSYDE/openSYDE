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
const C_OSCHalcConfigParameterStruct * C_PuiSdHandlerHALC::GetHALCDomainChannelParameterConfigData(
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
/*! \brief  Set HALC config safe data block

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oq_IsSet          Is set
   \param[in]  ou32_NewValue     New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerHALC::SetHALCConfigSafeDataBlock(const uint32 ou32_NodeIndex, const bool oq_IsSet,
                                                      const uint32 ou32_NewValue)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      rc_Node.c_HALCConfig.SetSafeDatablockAssigned(oq_IsSet, ou32_NewValue);
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC config unsafe data block

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oq_IsSet          Is set
   \param[in]  ou32_NewValue     New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerHALC::SetHALCConfigUnsafeDataBlock(const uint32 ou32_NodeIndex, const bool oq_IsSet,
                                                        const uint32 ou32_NewValue)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      rc_Node.c_HALCConfig.SetUnsafeDatablockAssigned(oq_IsSet, ou32_NewValue);
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
/*! \brief  HALC generate datapools

   \param[in]  ou32_NodeIndex    Node index

   \return
   C_NO_ERR Datapool generated
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
      std::vector<C_OSCNodeDataPool> c_Datapools;
      C_OSCHALCMagicianGenerator c_Magician(&rc_OSCNode);
      s32_Retval = c_Magician.GenerateHALCDatapools(c_Datapools);
      if (s32_Retval == C_NO_ERR)
      {
         C_PuiSdNode & rc_UiNode = this->mc_UINodes[ou32_NodeIndex];
         tgl_assert(rc_OSCNode.c_DataPools.size() == rc_UiNode.c_UIDataPools.size());
         if (rc_OSCNode.c_DataPools.size() == rc_UiNode.c_UIDataPools.size())
         {
            std::vector<C_PuiSdNodeDataPool> c_UiDatapools;

            //Clean up existing datapools
            this->HALCRemoveDatapools(ou32_NodeIndex);

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
/*! \brief  HALC remove datapools

   \param[in]  ou32_NodeIndex    Node index

   \return
   C_NO_ERR Datapools removed
   C_RANGE  Parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerHALC::HALCRemoveDatapools(const uint32 ou32_NodeIndex)
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
            s32_Retval = this->RemoveDataPool(ou32_NodeIndex, u32_DatapoolIndex);
         }
         else
         {
            //Iterate
            ++c_ItOSC;
            ++u32_DatapoolIndex;
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
bool C_PuiSdHandlerHALC::CheckHALCChannelNameAvailable(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                       const stw_scl::C_SCLString & orc_ChannelName,
                                                       const uint32 * const opu32_ChannelIndexToSkip) const
{
   bool q_Retval = true;

   const C_OSCHalcConfigDomain * pc_Domain = this->GetHALCDomainConfigDataConst(ou32_NodeIndex, ou32_DomainIndex);

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
