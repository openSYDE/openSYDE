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
#include "C_OSCHalcConfigDomain.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
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
   //initialize from base
   //Channels
   for (uint32 u32_ItChannel = 0UL; u32_ItChannel < orc_Base.c_Channels.size(); ++u32_ItChannel)
   {
      C_OSCHalcConfigChannel c_NewChannel =
         C_OSCHalcConfigDomain::mh_InitConfigFromName(orc_Base.c_Channels[u32_ItChannel].c_Name);

      //Defaults
      for (uint32 u32_ItUseCase = 0UL; u32_ItUseCase < orc_Base.c_ChannelUseCases.size(); ++u32_ItUseCase)
      {
         const C_OSCHalcDefChannelUseCase & rc_UseCase = orc_Base.c_ChannelUseCases[u32_ItUseCase];
         for (uint32 u32_ItDefault = 0UL; u32_ItDefault < rc_UseCase.c_DefaultChannels.size(); ++u32_ItDefault)
         {
            if (rc_UseCase.c_DefaultChannels[u32_ItDefault] == u32_ItChannel)
            {
               c_NewChannel.u32_UseCaseIndex = u32_ItUseCase;
               break;
            }
         }
      }

      //Add parameters
      C_OSCHalcConfigDomain::mh_AddParameters(orc_Base.c_ChannelValues.c_Parameters, c_NewChannel.c_Parameters);

      //Datablock config cannot be done here!

      this->c_ChannelConfigs.push_back(c_NewChannel);
   }

   //Domain
   this->c_DomainConfig = C_OSCHalcConfigDomain::mh_InitConfigFromName(orc_Base.c_Name);

   //Add parameters
   C_OSCHalcConfigDomain::mh_AddParameters(orc_Base.c_DomainValues.c_Parameters, this->c_DomainConfig.c_Parameters);
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
