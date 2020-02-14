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
C_OSCHalcConfig::C_OSCHalcConfig(void)
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
/*! \brief  Add domain

   \param[in]  orc_Domain  Domain
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfig::AddDomain(const C_OSCHalcDefDomain & orc_Domain)
{
   this->mc_Domains.push_back(orc_Domain);
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
sint32 C_OSCHalcConfig::SetDomainChannelConfig(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                               const bool oq_Active, const stw_scl::C_SCLString & orc_Name,
                                               const stw_scl::C_SCLString & orc_Comment, const bool oq_SafetyRelevant,
                                               const bool oq_DatablockSet, const uint32 ou32_DatablockIndex,
                                               const bool oq_UseCaseSet, const uint32 ou32_UseCaseIndex,
                                               const std::vector<C_OSCHalcConfigChannel::E_DataBlockConfig> & orc_DatablockConfig)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
         rc_Channel.q_Active = oq_Active;
         rc_Channel.c_Name = orc_Name;
         rc_Channel.c_Comment = orc_Comment;
         rc_Channel.q_SafetyRelevant = oq_SafetyRelevant;
         rc_Channel.q_DatablockSet = oq_DatablockSet;
         rc_Channel.u32_DatablockIndex = ou32_DatablockIndex;
         rc_Channel.q_UseCaseSet = oq_UseCaseSet;
         rc_Channel.u32_UseCaseIndex = ou32_UseCaseIndex;
         rc_Channel.c_DatablockConfig = orc_DatablockConfig;
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel config active

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_Active            Active

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainChannelConfigActive(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                     const bool oq_Active)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
         rc_Channel.q_Active = oq_Active;
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel config name

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  orc_Name             Name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainChannelConfigName(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                   const stw_scl::C_SCLString & orc_Name)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
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
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel config comment

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  orc_Comment          Comment

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainChannelConfigComment(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                      const stw_scl::C_SCLString & orc_Comment)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
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
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel config safety

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_SafetyRelevant    Safety relevant

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainChannelConfigSafety(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                     const bool oq_SafetyRelevant)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
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
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel config datablock

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_DatablockSet      Datablock set
   \param[in]  ou32_DatablockIndex  Datablock index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainChannelConfigDatablock(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                        const bool oq_DatablockSet, const uint32 ou32_DatablockIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
         rc_Channel.q_DatablockSet = oq_DatablockSet;
         rc_Channel.u32_DatablockIndex = ou32_DatablockIndex;
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel config use case

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseCaseSet        Use case set
   \param[in]  ou32_UseCaseIndex    Use case index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainChannelConfigUseCase(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                      const bool oq_UseCaseSet, const uint32 ou32_UseCaseIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
         rc_Channel.q_UseCaseSet = oq_UseCaseSet;
         rc_Channel.u32_UseCaseIndex = ou32_UseCaseIndex;
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel config datablock config

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  orc_DatablockConfig  Datablock config

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainChannelConfigDatablockConfig(const uint32 ou32_DomainIndex,
                                                              const uint32 ou32_ChannelIndex,
                                                              const std::vector<C_OSCHalcConfigChannel::E_DataBlockConfig> & orc_DatablockConfig)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
      if (ou32_ChannelIndex < rc_Domain.c_ChannelConfigs.size())
      {
         C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[ou32_ChannelIndex];
         rc_Channel.c_DatablockConfig = orc_DatablockConfig;
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain channel parameter config

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  ou32_ParameterIndex  Parameter index
   \param[in]  orc_Parameter        Parameter

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfig::SetDomainChannelParameterConfig(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                        const uint32 ou32_ParameterIndex,
                                                        const C_OSCHalcConfigParameterStruct & orc_Parameter)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
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
                                                               const C_OSCHalcConfigParameter & orc_Parameter)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DomainIndex < this->mc_Domains.size())
   {
      C_OSCHalcConfigDomain & rc_Domain = this->mc_Domains[ou32_DomainIndex];
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
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
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
