//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group config data for HALC configuration

   Group config data for HALC configuration

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscHalcConfig.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;

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
C_OscHalcConfig::C_OscHalcConfig(void) :
   C_OscHalcDefBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain size

   \return
   Current domain count
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscHalcConfig::GetDomainSize() const
{
   return static_cast<uint32_t>(this->mc_Domains.size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain config data const

   \param[in]  ou32_Index  Index

   \return
   NULL Invalid index
   Else Valid domain
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscHalcConfigDomain * C_OscHalcConfig::GetDomainConfigDataConst(const uint32_t ou32_Index) const
{
   const C_OscHalcConfigDomain * pc_Retval = NULL;

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
const C_OscHalcDefDomain * C_OscHalcConfig::GetDomainDefDataConst(const uint32_t ou32_Index) const
{
   const C_OscHalcDefDomain * pc_Retval = NULL;

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
void C_OscHalcConfig::Clear()
{
   C_OscHalcDefBase::Clear();
   this->mc_Domains.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks if the HALC configuration is clear or if it has a loaded definition/configuration

   \retval   true    Definition is cleared. No definition is loaded.
   \retval   false   The definition is not cleared. A concrete definition is loaded.
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscHalcConfig::IsClear(void) const
{
   bool q_Return = C_OscHalcDefBase::IsClear();

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
void C_OscHalcConfig::AddDomain(const C_OscHalcDefDomain & orc_Domain)
{
   this->mc_Domains.push_back(orc_Domain);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle file load post processing
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcConfig::HandleFileLoadPostProcessing(void)
{
   for (uint32_t u32_It = 0UL; u32_It < this->mc_Domains.size(); ++u32_It)
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
int32_t C_OscHalcConfig::SetDomainConfig(const uint32_t ou32_Index, const C_OscHalcConfigDomain & orc_Domain)
{
   int32_t s32_Retval = C_NO_ERR;

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
int32_t C_OscHalcConfig::SetDomainChannelConfig(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                                const bool oq_UseChannelIndex, const stw::scl::C_SclString & orc_Name,
                                                const stw::scl::C_SclString & orc_Comment, const bool oq_SafetyRelevant,
                                                const uint32_t ou32_UseCaseIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OscHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
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
         C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
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
int32_t C_OscHalcConfig::ResetDomainChannelConfig(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                                  const bool oq_UseChannelIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OscHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
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
int32_t C_OscHalcConfig::ResetDomainChannelUseCase(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                                   const bool oq_UseChannelIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      if (oq_UseChannelIndex == true)
      {
         C_OscHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
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
int32_t C_OscHalcConfig::SetDomainChannelConfigName(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                                    const bool oq_UseChannelIndex,
                                                    const stw::scl::C_SclString & orc_Name)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OscHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            rc_Channel.c_Name = orc_Name;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
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
int32_t C_OscHalcConfig::SetDomainChannelConfigComment(const uint32_t ou32_DomainIndex,
                                                       const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                       const stw::scl::C_SclString & orc_Comment)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OscHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            rc_Channel.c_Comment = orc_Comment;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
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
int32_t C_OscHalcConfig::SetDomainChannelConfigSafety(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                                      const bool oq_UseChannelIndex, const bool oq_SafetyRelevant)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OscHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            rc_Channel.q_SafetyRelevant = oq_SafetyRelevant;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
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
int32_t C_OscHalcConfig::SetDomainChannelConfigUseCase(const uint32_t ou32_DomainIndex,
                                                       const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                       const uint32_t ou32_UseCaseIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OscHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            rc_Channel.u32_UseCaseIndex = ou32_UseCaseIndex;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
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
int32_t C_OscHalcConfig::SetDomainChannelParameterConfig(const uint32_t ou32_DomainIndex,
                                                         const uint32_t ou32_ChannelIndex,
                                                         const uint32_t ou32_ParameterIndex,
                                                         const bool oq_UseChannelIndex,
                                                         const C_OscHalcConfigParameterStruct & orc_Parameter)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OscHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
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
         C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
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
int32_t C_OscHalcConfig::SetDomainChannelParameterConfigElement(const uint32_t ou32_DomainIndex,
                                                                const uint32_t ou32_ChannelIndex,
                                                                const uint32_t ou32_ParameterIndex,
                                                                const uint32_t ou32_ElementIndex,
                                                                const bool oq_UseChannelIndex,
                                                                const C_OscHalcConfigParameter & orc_Parameter)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OscHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
            {
               C_OscHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
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
         C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
         {
            C_OscHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
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
int32_t C_OscHalcConfig::SetDomainChannelParameterConfigElementPlain(const uint32_t ou32_DomainIndex,
                                                                     const uint32_t ou32_ChannelIndex,
                                                                     const uint32_t ou32_ParameterIndex,
                                                                     const uint32_t ou32_ElementIndex,
                                                                     const bool oq_UseChannelIndex,
                                                                     const C_OscHalcDefContent & orc_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OscHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
            {
               C_OscHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
               if (rc_Struct.c_ParameterElements.size() > 0UL)
               {
                  if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
                  {
                     C_OscHalcConfigParameter & rc_Element = rc_Struct.c_ParameterElements[ou32_ElementIndex];
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
         C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
         {
            C_OscHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
            if (rc_Struct.c_ParameterElements.size() > 0UL)
            {
               if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
               {
                  C_OscHalcConfigParameter & rc_Element = rc_Struct.c_ParameterElements[ou32_ElementIndex];
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
int32_t C_OscHalcConfig::SetDomainChannelParameterConfigElementEnum(const uint32_t ou32_DomainIndex,
                                                                    const uint32_t ou32_ChannelIndex,
                                                                    const uint32_t ou32_ParameterIndex,
                                                                    const uint32_t ou32_ElementIndex,
                                                                    const bool oq_UseChannelIndex,
                                                                    const stw::scl::C_SclString & orc_DisplayName)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OscHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
            {
               C_OscHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
               if (rc_Struct.c_ParameterElements.size() > 0UL)
               {
                  if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
                  {
                     C_OscHalcConfigParameter & rc_Element = rc_Struct.c_ParameterElements[ou32_ElementIndex];
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
         C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
         {
            C_OscHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
            if (rc_Struct.c_ParameterElements.size() > 0UL)
            {
               if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
               {
                  C_OscHalcConfigParameter & rc_Element = rc_Struct.c_ParameterElements[ou32_ElementIndex];
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
int32_t C_OscHalcConfig::SetDomainChannelParameterConfigElementBitmask(const uint32_t ou32_DomainIndex,
                                                                       const uint32_t ou32_ChannelIndex,
                                                                       const uint32_t ou32_ParameterIndex,
                                                                       const uint32_t ou32_ElementIndex,
                                                                       const bool oq_UseChannelIndex,
                                                                       const stw::scl::C_SclString & orc_DisplayName,
                                                                       const bool oq_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OscHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
            {
               C_OscHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
               if (rc_Struct.c_ParameterElements.size() > 0UL)
               {
                  if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
                  {
                     C_OscHalcConfigParameter & rc_Element = rc_Struct.c_ParameterElements[ou32_ElementIndex];
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
         C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
         {
            C_OscHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
            if (rc_Struct.c_ParameterElements.size() > 0UL)
            {
               if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
               {
                  C_OscHalcConfigParameter & rc_Element = rc_Struct.c_ParameterElements[ou32_ElementIndex];
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
/*! \brief  Set domain channel parameter config element string

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  ou32_ParameterIndex  Parameter index
   \param[in]  ou32_ElementIndex    Element index
   \param[in]  oq_UseChannelIndex   Use channel index
   \param[in]  orc_Value            Value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
   C_CONFIG Type invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscHalcConfig::SetDomainChannelParameterConfigElementString(const uint32_t ou32_DomainIndex,
                                                                      const uint32_t ou32_ChannelIndex,
                                                                      const uint32_t ou32_ParameterIndex,
                                                                      const uint32_t ou32_ElementIndex,
                                                                      const bool oq_UseChannelIndex,
                                                                      const std::string & orc_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OscHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (oq_UseChannelIndex)
      {
         if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
         {
            C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
            if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
            {
               C_OscHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
               if (rc_Struct.c_ParameterElements.size() > 0UL)
               {
                  if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
                  {
                     C_OscHalcConfigParameter & rc_Element = rc_Struct.c_ParameterElements[ou32_ElementIndex];
                     s32_Retval = rc_Element.c_Value.SetStringValue(orc_Value);
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               else
               {
                  s32_Retval = rc_Struct.c_Value.SetStringValue(orc_Value);
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
         C_OscHalcConfigChannel & rc_Channel = rc_Domain.c_DomainConfig;
         if (ou32_ParameterIndex < rc_Channel.c_Parameters.size())
         {
            C_OscHalcConfigParameterStruct & rc_Struct = rc_Channel.c_Parameters[ou32_ParameterIndex];
            if (rc_Struct.c_ParameterElements.size() > 0UL)
            {
               if (ou32_ElementIndex < rc_Struct.c_ParameterElements.size())
               {
                  C_OscHalcConfigParameter & rc_Element = rc_Struct.c_ParameterElements[ou32_ElementIndex];
                  s32_Retval = rc_Element.c_Value.SetStringValue(orc_Value);
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            else
            {
               s32_Retval = rc_Struct.c_Value.SetStringValue(orc_Value);
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
int32_t C_OscHalcConfig::GetRelevantIndicesForSelectedUseCase(const uint32_t ou32_DomainIndex,
                                                              const uint32_t ou32_ChannelIndex,
                                                              const bool oq_UseChannelIndex,
                                                              std::vector<uint32_t> * const opc_ParameterIndices,
                                                              std::vector<uint32_t> * const opc_InputIndices,
                                                              std::vector<uint32_t> * const opc_OutputIndices,
                                                              std::vector<uint32_t> * const opc_StatusIndices) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      const C_OscHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
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
void C_OscHalcConfig::CheckConfigValid(bool * const opq_DomainsInvalid,
                                       std::vector<uint32_t> * const opc_InvalidDomainIndices) const
{
   if (opq_DomainsInvalid != NULL)
   {
      *opq_DomainsInvalid = false;

      for (uint32_t u32_ItDomain = 0; u32_ItDomain < this->mc_Domains.size(); u32_ItDomain++)
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
void C_OscHalcConfig::CheckDomainConfigValid(const uint32_t ou32_DomainIndex, bool * const opq_DomainInvalid,
                                             bool * const opq_ChannelsInvalid,
                                             std::vector<uint32_t> * const opc_InvalidChannelIndices) const
{
   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      const C_OscHalcConfigDomain & rc_CheckedDomain = this->mc_Domains[ou32_DomainIndex];

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

         for (uint32_t u32_ItChannel = 0; u32_ItChannel < rc_CheckedDomain.c_ChannelConfigs.size(); u32_ItChannel++)
         {
            bool q_NameConflict;
            bool q_NameInvalid;

            const C_OscHalcConfigChannel & rc_CheckedChannel = rc_CheckedDomain.c_ChannelConfigs[u32_ItChannel];

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
void C_OscHalcConfig::CalcHash(uint32_t & oru32_HashValue) const
{
   C_OscHalcDefBase::CalcHash(oru32_HashValue);

   for (uint32_t u32_It = 0UL; u32_It < this->mc_Domains.size(); ++u32_It)
   {
      this->mc_Domains[u32_It].CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle name max char limit

   \param[in]      ou32_NameMaxCharLimit  Name max char limit
   \param[in,out]  opc_ChangedItems       Changed items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcConfig::HandleNameMaxCharLimit(const uint32_t ou32_NameMaxCharLimit,
                                             std::list<C_OscSystemNameMaxCharLimitChangeReportItem> * const opc_ChangedItems)
{
   for (uint32_t u32_ItDomain = 0UL; u32_ItDomain < this->mc_Domains.size(); ++u32_ItDomain)
   {
      this->mc_Domains[u32_ItDomain].HandleNameMaxCharLimit(ou32_NameMaxCharLimit, opc_ChangedItems);
   }
}
