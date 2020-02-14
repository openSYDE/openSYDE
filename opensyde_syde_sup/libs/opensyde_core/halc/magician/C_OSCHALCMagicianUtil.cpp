//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility for HALC magician related functionality

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

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
C_OSCHALCMagicianUtil::C_OSCHALCMagicianUtil()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get datapool name

   \return
   Datapool name
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCHALCMagicianUtil::h_GetDatapoolName()
{
   const stw_scl::C_SCLString c_Retval = "HAL";

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
