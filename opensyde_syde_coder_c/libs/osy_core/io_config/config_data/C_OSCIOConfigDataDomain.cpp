//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain config data for IO config

   Group domain config data for IO config

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_OSCIOConfigDataDomain.h"

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
C_OSCIOConfigDataDomain::C_OSCIOConfigDataDomain(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in] orc_Base Base
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCIOConfigDataDomain::C_OSCIOConfigDataDomain(const C_OSCIOFileDataDomain & orc_Base)
{
   //Copy all
   this->c_Channels = orc_Base.c_Channels;
   this->c_ChannelUseCases = orc_Base.c_ChannelUseCases;
   this->c_Id = orc_Base.c_Id;
   this->c_InputValues = orc_Base.c_InputValues;
   this->c_Name = orc_Base.c_Name;
   this->c_OutputValues = orc_Base.c_OutputValues;
   this->c_Parameters = orc_Base.c_Parameters;
   this->c_SingularName = orc_Base.c_SingularName;
   this->c_StatusValues = orc_Base.c_StatusValues;
   //initialize from base
   for (uint32 u32_ItChannel = 0UL; u32_ItChannel < orc_Base.c_Channels.size(); ++u32_ItChannel)
   {
      C_OSCIOConfigDataChannel c_NewChannel;
      c_NewChannel.q_Active = false;
      c_NewChannel.q_SafetyRelevant = false;
      c_NewChannel.q_DatablockSet = false;
      c_NewChannel.u32_DatablockIndex = 0UL;
      c_NewChannel.c_Name = orc_Base.c_Channels[u32_ItChannel];
      c_NewChannel.c_Comment = "";
      //Datablock config cannot be done here!
      this->c_ChannelConfigs.push_back(c_NewChannel);
   }
   //Add parameters
   for (uint32 u32_ItParam = 0UL; u32_ItParam < orc_Base.c_Parameters.size(); ++u32_ItParam)
   {
      const C_OSCIOFileDataStruct & rc_Struct = orc_Base.c_Parameters[u32_ItParam];
      C_OSCIOConfigDataParameterStruct c_NewStruct;
      c_NewStruct.c_Comment = rc_Struct.c_Comment;
      c_NewStruct.c_Value = rc_Struct.c_InitialValue;
      for (uint32 u32_ItElem = 0UL; u32_ItElem < rc_Struct.c_StructElements.size(); ++u32_ItElem)
      {
         const C_OSCIOFileDataElement & rc_Elem = rc_Struct.c_StructElements[u32_ItElem];
         C_OSCIOConfigDataParameter c_NewElem;
         c_NewElem.c_Comment = rc_Elem.c_Comment;
         c_NewElem.c_Value = rc_Elem.c_InitialValue;
         c_NewStruct.c_ParameterElements.push_back(c_NewElem);
      }
      //Add for each channel
      for (uint32 u32_ItChannel = 0UL; u32_ItChannel < this->c_ChannelConfigs.size(); ++u32_ItChannel)
      {
         C_OSCIOConfigDataChannel & rc_Channel = this->c_ChannelConfigs[u32_ItChannel];
         rc_Channel.c_Parameters.push_back(c_NewStruct);
      }
   }
}
