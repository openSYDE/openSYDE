//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility for HALC magician related functionality

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCHALCMagicianUtil.h"

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
C_OSCHALCMagicianUtil::C_OSCHALCMagicianUtil(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get datapool name

   \param[in]  oq_IsSafe         Safe Datapool flag
   \param[in]  ou32_CopyIndex    Copy index

   \return
   Datapool name
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCHALCMagicianUtil::h_GetDatapoolName(const bool oq_IsSafe, const uint32 ou32_CopyIndex)
{
   stw_scl::C_SCLString c_Retval = (oq_IsSafe == true) ? "HAL_SAFE" : "HAL_NON_SAFE";
   if (ou32_CopyIndex > 0UL)
   {
      c_Retval += "_COPY_" + stw_scl::C_SCLString::IntToStr(ou32_CopyIndex + 1UL);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get datapool comment

   \param[in]  oq_IsSafe         Is safe
   \param[in]  ou32_CopyIndex    Copy index

   \return
   Datapool comment
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCHALCMagicianUtil::h_GetDatapoolComment(const bool oq_IsSafe, const uint32 ou32_CopyIndex)
{
   const stw_scl::C_SCLString c_Info = (oq_IsSafe == true) ? "safe" : "non safe";

   stw_scl::C_SCLString c_Retval = "Automatically generated Datapool for HAL " + c_Info + " variable storage.";
   if (ou32_CopyIndex > 0UL)
   {
      c_Retval += " This instance is a redundant copy of the original HAL datapool for backup purposes.";
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get list name

   \param[in]  oe_Type  Type

   \return
   List name
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCHALCMagicianUtil::h_GetListName(const C_OSCHalcDefDomain::E_VariableSelector oe_Type)
{
   stw_scl::C_SCLString c_Retval;

   switch (oe_Type)
   {
   case C_OSCHalcDefDomain::eVA_PARAM:
      c_Retval = "Configuration";
      break;
   case C_OSCHalcDefDomain::eVA_INPUT:
      c_Retval = "Inputs";
      break;
   case C_OSCHalcDefDomain::eVA_OUTPUT:
      c_Retval = "Outputs";
      break;
   case C_OSCHalcDefDomain::eVA_STATUS:
      c_Retval = "Statuses";
      break;
   default:
      c_Retval = "Unknown";
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get variable name

   \param[in]      orc_DefinitionArray          Definition array
   \param[in]      ou32_ParameterIndexStruct    Parameter index struct
   \param[in]      ou32_ParameterIndexElement   Parameter index element
   \param[in,out]  orc_DomainSingularName       Domain singular name
   \param[in,out]  orc_Name                     Name

   \return
   C_NO_ERR Variable name
   C_RANGE  Invalid input
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianUtil::h_GetVariableName(const std::vector<C_OSCHalcDefStruct> & orc_DefinitionArray,
                                                const uint32 ou32_ParameterIndexStruct,
                                                const uint32 ou32_ParameterIndexElement,
                                                const stw_scl::C_SCLString & orc_DomainSingularName,
                                                stw_scl::C_SCLString & orc_Name)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ParameterIndexStruct < orc_DefinitionArray.size())
   {
      const C_OSCHalcDefStruct & rc_StructDef = orc_DefinitionArray[ou32_ParameterIndexStruct];
      if (rc_StructDef.c_StructElements.size() > 0UL)
      {
         if (ou32_ParameterIndexElement < rc_StructDef.c_StructElements.size())
         {
            const C_OSCHalcDefElement & rc_DefElem = rc_StructDef.c_StructElements[ou32_ParameterIndexElement];
            orc_Name = C_OSCHALCMagicianUtil::mh_GetElementName(rc_DefElem, orc_DomainSingularName);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         orc_Name = C_OSCHALCMagicianUtil::mh_GetElementName(rc_StructDef, orc_DomainSingularName);
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Combine variable name

   \param[in]  orc_Domain           Domain
   \param[in]  orc_ParameterName    Parameter name

   \return
   Complete element name
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCHALCMagicianUtil::h_CombineVariableName(const stw_scl::C_SCLString & orc_Domain,
                                                                  const stw_scl::C_SCLString & orc_ParameterName)
{
   stw_scl::C_SCLString c_Retval = "";

   //Domain
   c_Retval += orc_Domain;
   //Variable
   c_Retval += orc_ParameterName;
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get channel number variable

   \param[in]  orc_DomainSingularName  Domain singular name
   \param[in]  oq_IsSafe               Is safe
   \param[in]  ou32_NumChannels        Num channels
   \param[in]  oq_AddDataset           Add dataset

   \return
   Channel number variable
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolListElement C_OSCHALCMagicianUtil::h_GetChanNumVariable(
   const stw_scl::C_SCLString & orc_DomainSingularName, const bool oq_IsSafe, const uint32 ou32_NumChannels,
   const bool oq_AddDataset)
{
   C_OSCNodeDataPoolListElement c_Element;

   c_Element.c_Name = C_OSCHALCMagicianUtil::h_GetChanNumVariableName(orc_DomainSingularName);
   c_Element.c_Comment = "Zero based channel numbers";

   //Defined defaults
   h_SetCommonDpElementDefaults(c_Element);

   // Access
   c_Element.e_Access = oq_IsSafe ? C_OSCNodeDataPoolListElement::eACCESS_RO : C_OSCNodeDataPoolListElement::eACCESS_RW;

   //Type
   C_OSCHALCMagicianUtil::mh_HandleGenericType(c_Element, ou32_NumChannels, oq_AddDataset, true);

   return c_Element;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get channel number variable name

   \param[in]  orc_DomainSingularName  Domain singular name

   \return
   Channel number variable name
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCHALCMagicianUtil::h_GetChanNumVariableName(const stw_scl::C_SCLString & orc_DomainSingularName)
{
   return C_OSCHALCMagicianUtil::h_CombineVariableName(orc_DomainSingularName, "ChannelNumber");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get use case variable

   \param[in]  orc_DomainSingularName  Domain singular name
   \param[in]  oq_IsSafe               Is safe
   \param[in]  ou32_NumChannels        Num channels
   \param[in]  oq_AddDataset           Add dataset

   \return
   Use case variable
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolListElement C_OSCHALCMagicianUtil::h_GetUseCaseVariable(
   const stw_scl::C_SCLString & orc_DomainSingularName, const bool oq_IsSafe, const uint32 ou32_NumChannels,
   const bool oq_AddDataset)
{
   C_OSCNodeDataPoolListElement c_Element;

   c_Element.c_Name = C_OSCHALCMagicianUtil::h_GetUseCaseVariableName(orc_DomainSingularName);
   c_Element.c_Comment = "Selected use-case";

   //Defined defaults
   h_SetCommonDpElementDefaults(c_Element);

   // Access
   c_Element.e_Access = oq_IsSafe ? C_OSCNodeDataPoolListElement::eACCESS_RO : C_OSCNodeDataPoolListElement::eACCESS_RW;

   //Type
   C_OSCHALCMagicianUtil::mh_HandleGenericType(c_Element, ou32_NumChannels, oq_AddDataset, false);

   return c_Element;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get use case variable name

   \param[in]  orc_DomainSingularName  Domain singular name

   \return
   Use case variable name
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCHALCMagicianUtil::h_GetUseCaseVariableName(const stw_scl::C_SCLString & orc_DomainSingularName)
{
   return C_OSCHALCMagicianUtil::h_CombineVariableName(orc_DomainSingularName, "UseCase");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get safety flag variable

   \param[in]  orc_DomainSingularName  Domain singular name
   \param[in]  oq_IsSafe               Is safe
   \param[in]  ou32_NumChannels        Num channels
   \param[in]  oq_AddDataset           Add dataset

   \return
   Safety flag variable
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolListElement C_OSCHALCMagicianUtil::h_GetSafetyFlagVariable(
   const stw_scl::C_SCLString & orc_DomainSingularName, const bool oq_IsSafe, const uint32 ou32_NumChannels,
   const bool oq_AddDataset)
{
   const uint8 u8_MAX_VALUE = 1U;
   C_OSCNodeDataPoolListElement c_Element;

   c_Element.c_Name = C_OSCHALCMagicianUtil::h_GetSafetyFlagVariableName(orc_DomainSingularName);
   c_Element.c_Comment = "Information about which channel is safety relevant";

   //Defined defaults
   h_SetCommonDpElementDefaults(c_Element);

   // Access
   c_Element.e_Access = oq_IsSafe ? C_OSCNodeDataPoolListElement::eACCESS_RO : C_OSCNodeDataPoolListElement::eACCESS_RW;

   //Type
   C_OSCHALCMagicianUtil::mh_HandleGenericType(c_Element, ou32_NumChannels, oq_AddDataset, false);

   //Special value max
   if (ou32_NumChannels > 1UL)
   {
      for (uint32 u32_It = 0UL; u32_It < ou32_NumChannels; ++u32_It)
      {
         c_Element.c_MaxValue.SetValueAU8Element(u8_MAX_VALUE, u32_It);
      }
   }
   else
   {
      c_Element.c_MaxValue.SetValueU8(u8_MAX_VALUE);
   }

   return c_Element;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get safety flag variable name

   \param[in]  orc_DomainSingularName  Domain singular name

   \return
   Safety flag variable name
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCHALCMagicianUtil::h_GetSafetyFlagVariableName(
   const stw_scl::C_SCLString & orc_DomainSingularName)
{
   return C_OSCHALCMagicianUtil::h_CombineVariableName(orc_DomainSingularName, "SafetyRelevant");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set common datapool element defaults

   \param[in,out]  orc_Element   Element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHALCMagicianUtil::h_SetCommonDpElementDefaults(C_OSCNodeDataPoolListElement & orc_Element)
{
   orc_Element.c_Unit = "";
   orc_Element.f64_Factor = 1.0;
   orc_Element.f64_Offset = 0.0;
   orc_Element.q_DiagEventCall = false;
   orc_Element.q_NvmValueIsValid = false;
   orc_Element.q_NvMValueChanged = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get element name

   \param[in]  orc_Param   Param
   \param[in]  orc_Domain  Domain

   \return
   Complete element name
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCHALCMagicianUtil::mh_GetElementName(const C_OSCHalcDefElement & orc_Param,
                                                              const stw_scl::C_SCLString & orc_Domain)
{
   const stw_scl::C_SCLString c_Retval = C_OSCHALCMagicianUtil::h_CombineVariableName(orc_Domain, orc_Param.c_Display);

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle generic type

   \param[in,out]  orc_Element         Element
   \param[in]      ou32_NumChannels    Num channels
   \param[in]      oq_AddDataset       Add dataset
   \param[in]      oq_UseU16           Use u16
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHALCMagicianUtil::mh_HandleGenericType(C_OSCNodeDataPoolListElement & orc_Element,
                                                 const uint32 ou32_NumChannels, const bool oq_AddDataset,
                                                 const bool oq_UseU16)
{
   if (oq_UseU16)
   {
      orc_Element.c_MinValue.SetType(C_OSCNodeDataPoolContent::eUINT16);
   }
   else
   {
      orc_Element.c_MinValue.SetType(C_OSCNodeDataPoolContent::eUINT8);
   }
   if (ou32_NumChannels > 1UL)
   {
      orc_Element.c_MinValue.SetArray(true);
      orc_Element.c_MinValue.SetArraySize(ou32_NumChannels);
   }
   else
   {
      orc_Element.c_MinValue.SetArray(false);
   }

   //Copy type
   orc_Element.c_MaxValue = orc_Element.c_MinValue;

   //Value
   if (ou32_NumChannels > 1UL)
   {
      for (uint32 u32_It = 0UL; u32_It < ou32_NumChannels; ++u32_It)
      {
         if (oq_UseU16)
         {
            orc_Element.c_MinValue.SetValueAU16Element(std::numeric_limits<uint16>::min(), u32_It);
            orc_Element.c_MaxValue.SetValueAU16Element(std::numeric_limits<uint16>::max(), u32_It);
         }
         else
         {
            orc_Element.c_MinValue.SetValueAU8Element(std::numeric_limits<uint8>::min(), u32_It);
            orc_Element.c_MaxValue.SetValueAU8Element(std::numeric_limits<uint8>::max(), u32_It);
         }
      }
   }
   else
   {
      if (oq_UseU16)
      {
         orc_Element.c_MinValue.SetValueU16(std::numeric_limits<uint16>::min());
         orc_Element.c_MaxValue.SetValueU16(std::numeric_limits<uint16>::max());
      }
      else
      {
         orc_Element.c_MinValue.SetValueU8(std::numeric_limits<uint8>::min());
         orc_Element.c_MaxValue.SetValueU8(std::numeric_limits<uint8>::max());
      }
   }

   //Dataset
   if (oq_AddDataset)
   {
      orc_Element.c_DataSetValues.push_back(orc_Element.c_MinValue);
   }

   //Init including value
   orc_Element.c_Value = orc_Element.c_MinValue;
   orc_Element.c_NvmValue = orc_Element.c_MinValue;
}
