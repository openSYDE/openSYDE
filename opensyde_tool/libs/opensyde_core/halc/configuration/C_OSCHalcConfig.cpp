//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group config data for HALC configuration

   Group config data for HALC configuration

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLChecksums.h"
#include "C_OSCHalcConfig.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcConfig::C_OSCHalcConfig(void) :
   C_OSCHalcDefBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain size

   \return
   Current domain count
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCHalcConfig::GetDomainSize() const
{
   return this->mc_Domains.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain config data const

   \param[in]  ou32_Index  Index

   \return
   NULL Invalid index
   Else Valid domain
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCHalcConfigDomain * C_OSCHalcConfig::GetDomainConfigDataConst(const uint32 ou32_Index) const
{
   const C_OSCHalcConfigDomain * pc_Retval = NULL;

   if (ou32_Index < this->mc_Domains.size())
   {
      pc_Retval = &this->mc_Domains[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain file data const

   \param[in]  ou32_Index  Index

   \return
   NULL Invalid index
   Else Valid domain
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCHalcDefDomain * C_OSCHalcConfig::GetDomainDefDataConst(const uint32 ou32_Index) const
{
   const C_OSCHalcDefDomain * pc_Retval = NULL;

   if (ou32_Index < this->mc_Domains.size())
   {
      pc_Retval = &this->mc_Domains[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfig::Clear()
{
   C_OSCHalcDefBase::Clear();
   this->mc_Domains.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks if the HALC configuration is clear or if it has a loaded definition/configuration

   \retval   true    Definition is cleared. No definition is loaded.
   \retval   false   The definition is not cleared. A concrete definition is loaded.
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCHalcConfig::IsClear(void) const
{
   bool q_Return = C_OSCHalcDefBase::IsClear();

   if (q_Return == true)
   {
      q_Return = (this->mc_Domains.size() == 0);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add domain

   \param[in]  orc_Domain  Domain
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfig::AddDomain(const C_OSCHalcDefDomain & orc_Domain)
{
   this->mc_Domains.push_back(orc_Domain);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle file load post processing
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfig::HandleFileLoadPostProcessing(void)
{
   for (uint32 u32_It = 0UL; u32_It < this->mc_Domains.size(); ++u32_It)
   {
      this->mc_Domains[u32_It].HandleFileLoadPostProcessing(this->e_SafetyMode);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain config

   \param[in]  ou32_Index  Index
   \param[in]  orc_Domain  Domain

   \return
   C_NO_ERR Value set
   C_RANGE  Invalid index
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainConfig(const uint32 ou32_Index, const C_OSCHalcConfigDomain & orc_Domain)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Domains.size())
   {
      this->mc_Domains[ou32_Index] = orc_Domain;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel config

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
sint32 C_OSCHalcConfig::SetDomainChannelConfig(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                               const bool oq_UseChannelIndex, const stw_scl::C_SCLString & orc_Name,
                                               const stw_scl::C_SCLString & orc_Comment, const bool oq_SafetyRelevant,
                                               const uint32 ou32_UseCaseIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            rc_Channel.c_Name = orc_Name;
            rc_Channel.c_Comment = orc_Comment;
            rc_Channel.q_SafetyRelevant = oq_SafetyRelevant;
            rc_Channel.u32_UseCaseIndex = ou32_UseCaseIndex;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         rc_Channel.c_Name = orc_Name;
         rc_Channel.c_Comment = orc_Comment;
         rc_Channel.q_SafetyRelevant = oq_SafetyRelevant;
         rc_Channel.u32_UseCaseIndex = ou32_UseCaseIndex;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset domain channel config

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::ResetDomainChannelConfig(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                 const bool oq_UseChannelIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         s32_Retval = rc_Domain.ResetChannelToDefault(ou32_ChannelIndex);
      }
      else
      {
         rc_Domain.ResetDomainToDefault();
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset domain channel use case

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::ResetDomainChannelUseCase(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                  const bool oq_UseChannelIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      if (oq_UseChannelIndex == true)
      {
         C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
         s32_Retval = rc_Domain.ResetChannelUseCase(ou32_ChannelIndex);
      }
      // domains have no use case
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel config name

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  orc_Name             Name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainChannelConfigName(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                   const bool oq_UseChannelIndex, const stw_scl::C_SCLString & orc_Name)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            rc_Channel.c_Name = orc_Name;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         rc_Channel.c_Name = orc_Name;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel config comment

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  orc_Comment          Comment

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainChannelConfigComment(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                      const bool oq_UseChannelIndex,
                                                      const stw_scl::C_SCLString & orc_Comment)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            rc_Channel.c_Comment = orc_Comment;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         rc_Channel.c_Comment = orc_Comment;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel config safety

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  oq_SafetyRelevant    Safety relevant

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainChannelConfigSafety(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                     const bool oq_UseChannelIndex, const bool oq_SafetyRelevant)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            rc_Channel.q_SafetyRelevant = oq_SafetyRelevant;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         rc_Channel.q_SafetyRelevant = oq_SafetyRelevant;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel config use case

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  ou32_UseCaseIndex    Use case index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainChannelConfigUseCase(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                      const bool oq_UseChannelIndex, const uint32 ou32_UseCaseIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            rc_Channel.u32_UseCaseIndex = ou32_UseCaseIndex;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         rc_Channel.u32_UseCaseIndex = ou32_UseCaseIndex;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel parameter config

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
sint32 C_OSCHalcConfig::SetDomainChannelParameterConfig(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                        const uint32 ou32_ParameterIndex, const bool oq_UseChannelIndex,
                                                        const C_OSCHalcConfigParameterStruct & orc_Parameter)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
            {
               rc_Channel.c_Parameters[ou32_ParameterIndex] = orc_Parameter;
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
         {
            rc_Channel.c_Parameters[ou32_ParameterIndex] = orc_Parameter;
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
/*! \brief  Set domain channel parameter config element

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
sint32 C_OSCHalcConfig::SetDomainChannelParameterConfigElement(const uint32 ou32_DomainIndex,
                                                               const uint32 ou32_ChannelIndex,
                                                               const uint32 ou32_ParameterIndex,
                                                               const uint32 ou32_ElementIndex,
                                                               const bool oq_UseChannelIndex,
                                                               const C_OSCHalcConfigParameter & orc_Parameter)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
            {
               C_OSCHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
               if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
               {
                  rc_Struct.c_ParameterElements[ou32_ElementIndex] = orc_Parameter;
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
         {
            C_OSCHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
            if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
            {
               rc_Struct.c_ParameterElements[ou32_ElementIndex] = orc_Parameter;
            }
            else
            {
               s32_Retval = C_RANGE;
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
/*! \brief  Set domain channel parameter config element plain value

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
sint32 C_OSCHalcConfig::SetDomainChannelParameterConfigElementPlain(const uint32 ou32_DomainIndex,
                                                                    const uint32 ou32_ChannelIndex,
                                                                    const uint32 ou32_ParameterIndex,
                                                                    const uint32 ou32_ElementIndex,
                                                                    const bool oq_UseChannelIndex,
                                                                    const C_OSCHalcDefContent & orc_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
            {
               C_OSCHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
               if (rc_Struct.c_ParameterElements.size() > 0UL)
               {
                  if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
                  {
                     C_OSCHalcConfigParameter & rc_Element = rc_Struct.c_ParameterElements[ou32_ElementIndex];
                     rc_Element.c_Value = orc_Value;
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               else
               {
                  rc_Struct.c_Value = orc_Value;
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
         {
            C_OSCHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
            if (rc_Struct.c_ParameterElements.size() > 0UL)
            {
               if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
               {
                  C_OSCHalcConfigParameter & rc_Element = rc_Struct.c_ParameterElements[ou32_ElementIndex];
                  rc_Element.c_Value = orc_Value;
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            else
            {
               rc_Struct.c_Value = orc_Value;
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
/*! \brief  Set domain channel parameter config element

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  ou32_ParameterIndex  Parameter index
   \param[in]  ou32_ElementIndex    Element index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  orc_DisplayName      Display name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
   C_CONFIG Enum content invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainChannelParameterConfigElementEnum(const uint32 ou32_DomainIndex,
                                                                   const uint32 ou32_ChannelIndex,
                                                                   const uint32 ou32_ParameterIndex,
                                                                   const uint32 ou32_ElementIndex,
                                                                   const bool oq_UseChannelIndex,
                                                                   const stw_scl::C_SCLString & orc_DisplayName)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
            {
               C_OSCHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
               if (rc_Struct.c_ParameterElements.size() > 0UL)
               {
                  if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
                  {
                     C_OSCHalcConfigParameter & rc_Element = rc_Struct.c_ParameterElements[ou32_ElementIndex];
                     s32_Retval = rc_Element.c_Value.SetEnumValue(orc_DisplayName);
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               else
               {
                  s32_Retval = rc_Struct.c_Value.SetEnumValue(orc_DisplayName);
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
         {
            C_OSCHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
            if (rc_Struct.c_ParameterElements.size() > 0UL)
            {
               if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
               {
                  C_OSCHalcConfigParameter & rc_Element = rc_Struct.c_ParameterElements[ou32_ElementIndex];
                  s32_Retval = rc_Element.c_Value.SetEnumValue(orc_DisplayName);
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            else
            {
               s32_Retval = rc_Struct.c_Value.SetEnumValue(orc_DisplayName);
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
/*! \brief  Set domain channel parameter config element

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
   C_CONFIG Type invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainChannelParameterConfigElementBitmask(const uint32 ou32_DomainIndex,
                                                                      const uint32 ou32_ChannelIndex,
                                                                      const uint32 ou32_ParameterIndex,
                                                                      const uint32 ou32_ElementIndex,
                                                                      const bool oq_UseChannelIndex,
                                                                      const stw_scl::C_SCLString & orc_DisplayName,
                                                                      const bool oq_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
            {
               C_OSCHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
               if (rc_Struct.c_ParameterElements.size() > 0UL)
               {
                  if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
                  {
                     C_OSCHalcConfigParameter & rc_Element = rc_Struct.c_ParameterElements[ou32_ElementIndex];
                     s32_Retval = rc_Element.c_Value.SetBitmask(orc_DisplayName, oq_Value);
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               else
               {
                  s32_Retval = rc_Struct.c_Value.SetBitmask(orc_DisplayName, oq_Value);
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
         {
            C_OSCHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
            if (rc_Struct.c_ParameterElements.size() > 0UL)
            {
               if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
               {
                  C_OSCHalcConfigParameter & rc_Element = rc_Struct.c_ParameterElements[ou32_ElementIndex];
                  s32_Retval = rc_Element.c_Value.SetBitmask(orc_DisplayName, oq_Value);
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            else
            {
               s32_Retval = rc_Struct.c_Value.SetBitmask(orc_DisplayName, oq_Value);
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
/*! \brief  Get relevant indices for selected use case

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
sint32 C_OSCHalcConfig::GetRelevantIndicesForSelectedUseCase(const uint32 ou32_DomainIndex,
                                                             const uint32 ou32_ChannelIndex,
                                                             const bool oq_UseChannelIndex,
                                                             std::vector<uint32> * const opc_ParameterIndices,
                                                             std::vector<uint32> * const opc_InputIndices,
                                                             std::vector<uint32> * const opc_OutputIndices,
                                                             std::vector<uint32> * const opc_StatusIndices) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      const C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      rc_Domain.GetRelevantIndicesForSelectedUseCase(ou32_ChannelIndex, oq_UseChannelIndex, opc_ParameterIndices,
                                                     opc_InputIndices, opc_OutputIndices, opc_StatusIndices);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check config valid

   \param[out]     opq_DomainsInvalid        Domains invalid
   \param[in,out]  opc_InvalidDomainIndices  Invalid domain indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfig::CheckConfigValid(bool * const opq_DomainsInvalid,
                                       std::vector<uint32> * const opc_InvalidDomainIndices) const
{
   if (opq_DomainsInvalid != NULL)
   {
      *opq_DomainsInvalid = false;

      for (uint32 u32_ItDomain = 0; u32_ItDomain < this->mc_Domains.size(); u32_ItDomain++)
      {
         bool q_DomainInvalid;
         bool q_ChannelsInvalid;

         this->CheckDomainConfigValid(u32_ItDomain, &q_DomainInvalid, &q_ChannelsInvalid, NULL);

         if ((q_DomainInvalid == true) || (q_ChannelsInvalid == true))
         {
            *opq_DomainsInvalid = true;

            // remember index of invalid domain
            if (opc_InvalidDomainIndices != NULL)
            {
               opc_InvalidDomainIndices->push_back(u32_ItDomain);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check domain channel config valid

   \param[in]      ou32_DomainIndex             Domain index
   \param[out]     opq_DomainInvalid            Domain invalid flag
   \param[out]     opq_ChannelsInvalid          Channels invalid
   \param[in,out]  opc_InvalidChannelIndices    Invalid channel indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfig::CheckDomainConfigValid(const uint32 ou32_DomainIndex, bool * const opq_DomainInvalid,
                                             bool * const opq_ChannelsInvalid,
                                             std::vector<uint32> * const opc_InvalidChannelIndices) const
{
   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      const C_OSCHalcConfigDomain & rc_CheckedDomain = this->mc_Domains[ou32_DomainIndex];

      // check domain configuration itself
      if (opq_DomainInvalid != NULL)
      {
         *opq_DomainInvalid = false;

         bool q_DomainInvalid;
         rc_CheckedDomain.c_DomainConfig.CheckConfigValid(&q_DomainInvalid);
         if (q_DomainInvalid == true)
         {
            *opq_DomainInvalid = true;
         }
      }

      // check all channels
      if (opq_ChannelsInvalid != NULL)
      {
         *opq_ChannelsInvalid = false;

         for (uint32 u32_ItChannel = 0; u32_ItChannel < rc_CheckedDomain.c_ChannelConfigs.size(); u32_ItChannel++)
         {
            bool q_NameConflict;
            bool q_NameInvalid;

            const C_OSCHalcConfigChannel & rc_CheckedChannel = rc_CheckedDomain.c_ChannelConfigs[u32_ItChannel];

            // check channel
            rc_CheckedChannel.CheckConfigValid(&q_NameInvalid);

            // check name conflict
            rc_CheckedDomain.CheckChannelNameUnique(u32_ItChannel, &q_NameConflict);

            // result
            if ((q_NameConflict == true) || (q_NameInvalid == true))
            {
               *opq_ChannelsInvalid = true;
               if (opc_InvalidChannelIndices != NULL)
               {
                  opc_InvalidChannelIndices->push_back(u32_ItChannel);
               }
            }
         }
      }
   }
   else
   {
      if (opq_DomainInvalid != NULL)
      {
         *opq_DomainInvalid = false;
      }
      if (opq_ChannelsInvalid != NULL)
      {
         *opq_ChannelsInvalid = false;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfig::CalcHash(uint32 & oru32_HashValue) const
{
   C_OSCHalcDefBase::CalcHash(oru32_HashValue);

   for (uint32 u32_It = 0UL; u32_It < this->mc_Domains.size(); ++u32_It)
   {
      this->mc_Domains[u32_It].CalcHash(oru32_HashValue);
   }
}
