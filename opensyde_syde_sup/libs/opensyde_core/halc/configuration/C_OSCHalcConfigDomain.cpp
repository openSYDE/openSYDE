//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain config data for HALC configuration

   Group domain config data for HALC configuration

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCHalcConfigDomain.h"

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
C_OSCHalcConfigDomain::C_OSCHalcConfigDomain(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in]  orc_Base    Base
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcConfigDomain::C_OSCHalcConfigDomain(const C_OSCHalcDefDomain & orc_Base)
{
   //Copy all
   this->c_Channels = orc_Base.c_Channels;
   this->c_ChannelUseCases = orc_Base.c_ChannelUseCases;
   this->c_Id = orc_Base.c_Id;
   this->c_DomainValues.c_InputValues = orc_Base.c_DomainValues.c_InputValues;
   this->c_ChannelValues.c_InputValues = orc_Base.c_ChannelValues.c_InputValues;
   this->c_Name = orc_Base.c_Name;
   this->c_Comment = orc_Base.c_Comment;
   this->c_DomainValues.c_OutputValues = orc_Base.c_DomainValues.c_OutputValues;
   this->c_ChannelValues.c_OutputValues = orc_Base.c_ChannelValues.c_OutputValues;
   this->c_DomainValues.c_Parameters = orc_Base.c_DomainValues.c_Parameters;
   this->c_ChannelValues.c_Parameters = orc_Base.c_ChannelValues.c_Parameters;
   this->c_SingularName = orc_Base.c_SingularName;
   this->c_DomainValues.c_StatusValues = orc_Base.c_DomainValues.c_StatusValues;
   this->c_ChannelValues.c_StatusValues = orc_Base.c_ChannelValues.c_StatusValues;
   this->e_Category = orc_Base.e_Category;

   //Initialize from base (after definition copy-over!):

   //Channels
   for (uint32 u32_ItChannel = 0UL; u32_ItChannel < orc_Base.c_Channels.size(); ++u32_ItChannel)
   {
      this->c_ChannelConfigs.push_back(this->m_InitChannelConfig(u32_ItChannel));
   }

   //Domain
   this->m_InitDomainConfig();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfigDomain::CalcHash(uint32 & oru32_HashValue) const
{
   C_OSCHalcDefDomain::CalcHash(oru32_HashValue);

   c_DomainConfig.CalcHash(oru32_HashValue);

   for (uint32 u32_It = 0UL; u32_It < this->c_ChannelConfigs.size(); ++u32_It)
   {
      this->c_ChannelConfigs[u32_It].CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel name unique

   \param[in]   ou32_ChannelIndex   Channel index
   \param[out]  opq_NameConflict    Name not unique
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfigDomain::CheckChannelNameUnique(const uint32 ou32_ChannelIndex, bool * const opq_NameConflict) const
{
   if ((opq_NameConflict != NULL) && (ou32_ChannelIndex < this->c_ChannelConfigs.size()))
   {
      const C_OSCHalcConfigChannel & rc_CheckedChannel = this->c_ChannelConfigs[ou32_ChannelIndex];

      *opq_NameConflict = false;

      // check if any other channel has the same name
      for (uint32 u32_ItCompChannels = 0; u32_ItCompChannels < this->c_ChannelConfigs.size();
           ++u32_ItCompChannels)
      {
         if (u32_ItCompChannels != ou32_ChannelIndex) // skip current channel to avoid comparison "with itself"
         {
            const C_OSCHalcConfigChannel & rc_ComparedChannel = this->c_ChannelConfigs[u32_ItCompChannels];
            if (rc_CheckedChannel.c_Name.LowerCase() == rc_ComparedChannel.c_Name.LowerCase())
            {
               *opq_NameConflict = true;
               break; // if we have one conflict we can stop searching
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel linked

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
sint32 C_OSCHalcConfigDomain::CheckChannelLinked(const uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                 bool & orq_IsLinked,
                                                 std::vector<stw_scl::C_SCLString> * const opc_LinkedChannelNames,
                                                 std::vector<uint32> * const opc_LinkedChannelIndices,
                                                 const uint32 * const opu32_UseCaseIndex) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (oq_UseChannelIndex == true)
   {
      if (ou32_ChannelIndex < this->c_ChannelConfigs.size())
      {
         const C_OSCHalcConfigChannel & rc_Config = this->c_ChannelConfigs[ou32_ChannelIndex];

         if (this->c_ChannelUseCases.size() > 0UL)
         {
            const uint32 u32_UseCaseIndex =
               (opu32_UseCaseIndex == NULL) ? rc_Config.u32_UseCaseIndex : *opu32_UseCaseIndex;
            if (u32_UseCaseIndex < this->c_ChannelUseCases.size())
            {
               bool q_Found = false;
               const C_OSCHalcDefChannelUseCase & rc_UseCase = this->c_ChannelUseCases[u32_UseCaseIndex];
               for (uint32 u32_ItAv = 0UL; u32_ItAv < rc_UseCase.c_Availability.size(); ++u32_ItAv)
               {
                  const C_OSCHalcDefChannelAvailability & rc_Avail = rc_UseCase.c_Availability[u32_ItAv];
                  if (rc_Avail.u32_ValueIndex == ou32_ChannelIndex)
                  {
                     q_Found = true;
                     if (rc_Avail.c_DependentValues.size() > 0UL)
                     {
                        orq_IsLinked = true;
                        if (opc_LinkedChannelNames != NULL)
                        {
                           opc_LinkedChannelNames->clear();
                           for (uint32 u32_ItDe = 0UL; u32_ItDe < rc_Avail.c_DependentValues.size(); ++u32_ItDe)
                           {
                              if (rc_Avail.c_DependentValues[u32_ItDe] < this->c_ChannelConfigs.size())
                              {
                                 const C_OSCHalcConfigChannel & rc_LinkedChannelConfig =
                                    this->c_ChannelConfigs[rc_Avail.c_DependentValues[u32_ItDe]];
                                 opc_LinkedChannelNames->push_back(rc_LinkedChannelConfig.c_Name.c_str());
                              }
                              else
                              {
                                 s32_Retval = C_RANGE;
                              }
                           }
                        }
                        if (opc_LinkedChannelIndices != NULL)
                        {
                           *opc_LinkedChannelIndices = rc_Avail.c_DependentValues;
                        }
                     }
                     else
                     {
                        orq_IsLinked = false;
                     }
                     //Stop
                     break;
                  }
               }
               if (q_Found == false)
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
            //Domain without use case -> irrelevant
            orq_IsLinked = false;
            if (opc_LinkedChannelNames != NULL)
            {
               opc_LinkedChannelNames->clear();
            }
            if (opc_LinkedChannelIndices != NULL)
            {
               opc_LinkedChannelIndices->clear();
            }
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset channel configuration to default

   \param[in]  ou32_ChannelIndex    Channel index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigDomain::ResetChannelToDefault(const uint32 ou32_ChannelIndex)
{
   sint32 s32_Return = C_NO_ERR;

   if ((ou32_ChannelIndex < this->c_ChannelConfigs.size()) && (ou32_ChannelIndex < this->c_Channels.size()))
   {
      this->c_ChannelConfigs[ou32_ChannelIndex] = this->m_InitChannelConfig(ou32_ChannelIndex);
   }
   else
   {
      s32_Return = C_RANGE;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset channel use case

   \param[in]  ou32_ChannelIndex    Channel index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigDomain::ResetChannelUseCase(const uint32 ou32_ChannelIndex)
{
   sint32 s32_Return = C_NO_ERR;

   if ((ou32_ChannelIndex < this->c_ChannelConfigs.size()) && (ou32_ChannelIndex < this->c_Channels.size()))
   {
      C_OSCHalcConfigChannel & rc_Config = this->c_ChannelConfigs[ou32_ChannelIndex];
      rc_Config.u32_UseCaseIndex = this->m_InitChannelUseCase(ou32_ChannelIndex);
   }
   else
   {
      s32_Return = C_RANGE;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset domain configuration to default
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfigDomain::ResetDomainToDefault(void)
{
   this->m_InitDomainConfig();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get relevant indices for selected use case

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
sint32 C_OSCHalcConfigDomain::GetRelevantIndicesForSelectedUseCase(const uint32 ou32_ChannelIndex,
                                                                   const bool oq_UseChannelIndex,
                                                                   std::vector<uint32> * const opc_ParameterIndices,
                                                                   std::vector<uint32> * const opc_InputIndices,
                                                                   std::vector<uint32> * const opc_OutputIndices,
                                                                   std::vector<uint32> * const opc_StatusIndices) const
{
   sint32 s32_Return = C_NO_ERR;

   if (oq_UseChannelIndex)
   {
      if (ou32_ChannelIndex < this->c_ChannelConfigs.size())
      {
         const C_OSCHalcConfigChannel & rc_Channel = this->c_ChannelConfigs[ou32_ChannelIndex];
         if (opc_ParameterIndices != NULL)
         {
            for (uint32 u32_It = 0UL; u32_It < this->c_ChannelValues.c_Parameters.size(); ++u32_It)
            {
               const C_OSCHalcDefStruct & rc_Struct = this->c_ChannelValues.c_Parameters[u32_It];
               for (uint32 u32_ItAvail = 0UL; u32_ItAvail < rc_Struct.c_UseCaseAvailabilities.size(); ++u32_ItAvail)
               {
                  if (rc_Struct.c_UseCaseAvailabilities[u32_ItAvail] == rc_Channel.u32_UseCaseIndex)
                  {
                     opc_ParameterIndices->push_back(u32_It);
                  }
               }
            }
         }
         if (opc_InputIndices != NULL)
         {
            for (uint32 u32_It = 0UL; u32_It < this->c_ChannelValues.c_InputValues.size(); ++u32_It)
            {
               const C_OSCHalcDefStruct & rc_Struct = this->c_ChannelValues.c_InputValues[u32_It];
               for (uint32 u32_ItAvail = 0UL; u32_ItAvail < rc_Struct.c_UseCaseAvailabilities.size(); ++u32_ItAvail)
               {
                  if (rc_Struct.c_UseCaseAvailabilities[u32_ItAvail] == rc_Channel.u32_UseCaseIndex)
                  {
                     opc_InputIndices->push_back(u32_It);
                  }
               }
            }
         }
         if (opc_OutputIndices != NULL)
         {
            for (uint32 u32_It = 0UL; u32_It < this->c_ChannelValues.c_OutputValues.size(); ++u32_It)
            {
               const C_OSCHalcDefStruct & rc_Struct = this->c_ChannelValues.c_OutputValues[u32_It];
               for (uint32 u32_ItAvail = 0UL; u32_ItAvail < rc_Struct.c_UseCaseAvailabilities.size(); ++u32_ItAvail)
               {
                  if (rc_Struct.c_UseCaseAvailabilities[u32_ItAvail] == rc_Channel.u32_UseCaseIndex)
                  {
                     opc_OutputIndices->push_back(u32_It);
                  }
               }
            }
         }
         if (opc_StatusIndices != NULL)
         {
            for (uint32 u32_It = 0UL; u32_It < this->c_ChannelValues.c_StatusValues.size(); ++u32_It)
            {
               const C_OSCHalcDefStruct & rc_Struct = this->c_ChannelValues.c_StatusValues[u32_It];
               for (uint32 u32_ItAvail = 0UL; u32_ItAvail < rc_Struct.c_UseCaseAvailabilities.size(); ++u32_ItAvail)
               {
                  if (rc_Struct.c_UseCaseAvailabilities[u32_ItAvail] == rc_Channel.u32_UseCaseIndex)
                  {
                     opc_StatusIndices->push_back(u32_It);
                  }
               }
            }
         }
      }
      else
      {
         s32_Return = C_RANGE;
      }
   }
   else
   {
      if (opc_ParameterIndices != NULL)
      {
         opc_ParameterIndices->reserve(this->c_DomainValues.c_Parameters.size());
         for (uint32 u32_It = 0UL; u32_It < this->c_DomainValues.c_Parameters.size(); ++u32_It)
         {
            opc_ParameterIndices->push_back(u32_It);
         }
      }
      if (opc_InputIndices != NULL)
      {
         opc_InputIndices->reserve(this->c_DomainValues.c_InputValues.size());
         for (uint32 u32_It = 0UL; u32_It < this->c_DomainValues.c_InputValues.size(); ++u32_It)
         {
            opc_InputIndices->push_back(u32_It);
         }
      }
      if (opc_OutputIndices != NULL)
      {
         opc_OutputIndices->reserve(this->c_DomainValues.c_OutputValues.size());
         for (uint32 u32_It = 0UL; u32_It < this->c_DomainValues.c_OutputValues.size(); ++u32_It)
         {
            opc_OutputIndices->push_back(u32_It);
         }
      }
      if (opc_StatusIndices != NULL)
      {
         opc_StatusIndices->reserve(this->c_DomainValues.c_StatusValues.size());
         for (uint32 u32_It = 0UL; u32_It < this->c_DomainValues.c_StatusValues.size(); ++u32_It)
         {
            opc_StatusIndices->push_back(u32_It);
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add parameters

   \param[in]      orc_Parameters   Parameters
   \param[in,out]  orc_ParamConfig  Param config
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfigDomain::mh_AddParameters(const std::vector<C_OSCHalcDefStruct> & orc_Parameters,
                                             std::vector<C_OSCHalcConfigParameterStruct> & orc_ParamConfig)
{
   for (uint32 u32_ItParam = 0UL; u32_ItParam < orc_Parameters.size(); ++u32_ItParam)
   {
      const C_OSCHalcDefStruct & rc_Struct = orc_Parameters[u32_ItParam];
      C_OSCHalcConfigParameterStruct c_NewStruct;
      c_NewStruct.c_Comment = rc_Struct.c_Comment;
      c_NewStruct.c_Value = rc_Struct.c_InitialValue;
      for (uint32 u32_ItElem = 0UL; u32_ItElem < rc_Struct.c_StructElements.size(); ++u32_ItElem)
      {
         const C_OSCHalcDefElement & rc_Elem = rc_Struct.c_StructElements[u32_ItElem];
         C_OSCHalcConfigParameter c_NewElem;
         c_NewElem.c_Comment = rc_Elem.c_Comment;
         c_NewElem.c_Value = rc_Elem.c_InitialValue;
         c_NewStruct.c_ParameterElements.push_back(c_NewElem);
      }
      //Append
      orc_ParamConfig.push_back(c_NewStruct);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init config from name

   \param[in]  orc_Name    Name

   \return
   Channel configuration
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcConfigChannel C_OSCHalcConfigDomain::mh_InitConfigFromName(const stw_scl::C_SCLString & orc_Name)
{
   C_OSCHalcConfigChannel c_NewChannel;

   c_NewChannel.q_SafetyRelevant = false;
   c_NewChannel.c_Name = orc_Name;
   c_NewChannel.c_Comment = "";

   return c_NewChannel;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize channel configuration

   \param[in]  ou32_ChannelIndex    Channel index (undefined behavior if out of range)

   \return
   Initialized channel configuration
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcConfigChannel C_OSCHalcConfigDomain::m_InitChannelConfig(const uint32 ou32_ChannelIndex) const
{
   C_OSCHalcConfigChannel c_NewChannel;

   if (ou32_ChannelIndex < this->c_Channels.size())
   {
      // Default Name
      c_NewChannel =
         C_OSCHalcConfigDomain::mh_InitConfigFromName(this->c_SingularName + "_" +
                                                      stw_scl::C_SCLString::IntToStr(ou32_ChannelIndex + 1));

      //Default use case
      c_NewChannel.u32_UseCaseIndex = this->m_InitChannelUseCase(ou32_ChannelIndex);

      //Add parameters
      C_OSCHalcConfigDomain::mh_AddParameters(this->c_ChannelValues.c_Parameters, c_NewChannel.c_Parameters);
   }

   return c_NewChannel;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize channel use case

   \param[in]  ou32_ChannelIndex    Channel index

   \return
   Use case index (0 if no default was found)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCHalcConfigDomain::m_InitChannelUseCase(const uint32 ou32_ChannelIndex) const
{
   uint32 u32_UseCaseIndex = 0;

   for (uint32 u32_ItUseCase = 0UL; u32_ItUseCase < this->c_ChannelUseCases.size(); ++u32_ItUseCase)
   {
      const C_OSCHalcDefChannelUseCase & rc_UseCase = this->c_ChannelUseCases[u32_ItUseCase];
      for (uint32 u32_ItDefault = 0UL; u32_ItDefault < rc_UseCase.c_DefaultChannels.size(); ++u32_ItDefault)
      {
         if (rc_UseCase.c_DefaultChannels[u32_ItDefault] == ou32_ChannelIndex)
         {
            u32_UseCaseIndex = u32_ItUseCase;
            break;
         }
      }
   }

   return u32_UseCaseIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize domain configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfigDomain::m_InitDomainConfig(void)
{
   //General
   this->c_DomainConfig = C_OSCHalcConfigDomain::mh_InitConfigFromName(this->c_Name);

   //Add parameters
   C_OSCHalcConfigDomain::mh_AddParameters(this->c_DomainValues.c_Parameters, this->c_DomainConfig.c_Parameters);
}
