//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief     Export HALC configuration of an openSYDE node.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCExportHalc.h"
#include "CSCLStringList.h"
#include "TGLUtils.h"
#include "C_OSCLoggingHandler.h"

#include "C_OSCExportUti.h"
#include "C_OSCHALCMagicianUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;
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
C_OSCExportHalc::C_OSCExportHalc()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create file name for exported .c and .h file.

   \return
   File name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportHalc::h_GetFileName(void)
{
   return "hal_configuration";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create source code

   \param[in]  orc_Path             Storage path for created file
   \param[in]  orc_HalcConfig       HALC configuration
   \param[in]  orc_Datapool         Datapool definition
   \param[in]  orc_ExportToolInfo   Information about calling executable (name + version)

   \return
   C_NO_ERR    Success
   C_RD_WR     Operation failure: cannot store file
   C_NOACT     Datapool is not of type HALC
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportHalc::h_CreateSourceCode(const stw_scl::C_SCLString & orc_Path,
                                           const C_OSCHalcConfig & orc_HalcConfig,
                                           const C_OSCNodeDataPool & orc_Datapool,
                                           const stw_scl::C_SCLString & orc_ExportToolInfo)
{
   sint32 s32_Retval = C_NO_ERR;

   uint32 u32_HashValue = 0U;

   orc_HalcConfig.CalcHash(u32_HashValue);
   orc_Datapool.CalcHash(u32_HashValue);
   const C_SCLString c_ProjectId = C_SCLString::IntToStr(u32_HashValue);

   // make sure Datapool is of type HALC
   if (orc_Datapool.e_Type != C_OSCNodeDataPool::eHALC)
   {
      osc_write_log_error("Creating source code",
                          "Did not generate code for HALC configuration of Datapool \"" + orc_Datapool.c_Name +
                          "\" because Datapool type is not HALC.");
      s32_Retval = C_NOACT;
   }

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_CreateHeaderFile(orc_ExportToolInfo, orc_Path, orc_HalcConfig, c_ProjectId);
   }

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_CreateImplementationFile(orc_ExportToolInfo, orc_Path, orc_HalcConfig, c_ProjectId);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create header file

   \param[in]  orc_ExportToolInfo   Information about calling executable (name + version)
   \param[in]  orc_Path             Directory path for created file
   \param[in]  orc_HalcConfig       HALC configuration
   \param[in]  orc_ProjectId        Project id for consistency check

   \return
   C_NO_ERR    Success
   C_RD_WR     Operation failure: cannot store file
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportHalc::mh_CreateHeaderFile(const stw_scl::C_SCLString & orc_ExportToolInfo,
                                            const stw_scl::C_SCLString & orc_Path,
                                            const C_OSCHalcConfig & orc_HalcConfig,
                                            const stw_scl::C_SCLString & orc_ProjectId)
{
   sint32 s32_Retval;
   C_SCLStringList c_Data;

   // add header
   mh_AddHeader(orc_ExportToolInfo, c_Data, mhq_IS_HEADER_FILE);

   // add includes
   mh_AddIncludes(c_Data, mhq_IS_HEADER_FILE);

   // add defines
   mh_AddDefines(c_Data, orc_HalcConfig, orc_ProjectId, mhq_IS_HEADER_FILE);

   // add types
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Types"));
   c_Data.Append("");

   // add global variables
   mh_AddGlobalVariables(c_Data, orc_HalcConfig, mhq_IS_HEADER_FILE);

   // add function prototypes
   C_OSCExportUti::h_AddProjIdFunctionPrototype(c_Data,  mh_GetMagicName(orc_ProjectId));

   // add implementation
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Implementation"));
   C_OSCExportUti::h_AddExternCEnd(c_Data);
   c_Data.Append("#endif");

   // finally save to file
   s32_Retval = C_OSCExportUti::h_SaveToFile(c_Data, orc_Path, h_GetFileName(), true);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create implementation file

   \param[in]  orc_ExportToolInfo   Information about calling executable (name + version)
   \param[in]  orc_Path             Storage path for created file
   \param[in]  orc_HalcConfig       HALC configuration
   \param[in]  orc_ProjectId        Project id for consistency check

   \return
   C_NO_ERR    Success
   C_RD_WR     Operation failure: cannot store file
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportHalc::mh_CreateImplementationFile(const stw_scl::C_SCLString & orc_ExportToolInfo,
                                                    const stw_scl::C_SCLString & orc_Path,
                                                    const C_OSCHalcConfig & orc_HalcConfig,
                                                    const stw_scl::C_SCLString & orc_ProjectId)
{
   sint32 s32_Retval;

   C_SCLStringList c_Data;

   // add header
   mh_AddHeader(orc_ExportToolInfo, c_Data, mhq_IS_IMPLEMENTATION_FILE);

   // add includes
   mh_AddIncludes(c_Data, mhq_IS_IMPLEMENTATION_FILE);

   // add defines
   mh_AddDefines(c_Data, orc_HalcConfig, orc_ProjectId, mhq_IS_IMPLEMENTATION_FILE);

   // add types
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Types"));
   c_Data.Append("");

   // add global variables
   mh_AddGlobalVariables(c_Data, orc_HalcConfig, mhq_IS_IMPLEMENTATION_FILE);

   // add modul global variables
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Module Global Variables"));
   c_Data.Append("");

   // add function prototypes
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Module Global Function Prototypes"));
   c_Data.Append("");

   // add implementation
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Implementation"));

   // finally save to file
   s32_Retval = C_OSCExportUti::h_SaveToFile(c_Data, orc_Path, h_GetFileName(), false);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add file header

   \param[in]   orc_ExportToolInfo  Information about calling executable (name + version)
   \param[out]  orc_Data            Data converted to string list
   \param[in]   oq_FileType         Flag for .c vs. .h file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportHalc::mh_AddHeader(const C_SCLString & orc_ExportToolInfo, C_SCLStringList & orc_Data,
                                   const bool oq_FileType)
{
   const C_SCLString c_Tmp = (oq_FileType == mhq_IS_IMPLEMENTATION_FILE) ? "Source file with constant definitions" :
                             "Header file with constant and global definitions";

   orc_Data.Append(C_OSCExportUti::h_GetHeaderSeparator());
   orc_Data.Append("/*!");
   orc_Data.Append("   \\file");
   orc_Data.Append("   \\brief       openSYDE HAL configuration definition (" + c_Tmp + ")");
   orc_Data.Append("");
   if (oq_FileType == mhq_IS_IMPLEMENTATION_FILE)
   {
      orc_Data.Append("   Defines the hardware abstraction layer configuration.");
      orc_Data.Append("");
   }

   orc_Data.Append(C_OSCExportUti::h_GetCreationToolInfo(orc_ExportToolInfo));
   orc_Data.Append("*/");
   orc_Data.Append(C_OSCExportUti::h_GetHeaderSeparator());

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      const C_SCLString c_HeaderGuard = h_GetFileName().UpperCase() + "H";

      orc_Data.Append("#ifndef " + c_HeaderGuard);
      orc_Data.Append("#define " + c_HeaderGuard);
   }

   orc_Data.Append("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add includes

   \param[out]  orc_Data      Data converted to string list
   \param[in]   oq_FileType   Flag for .c vs. .h file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportHalc::mh_AddIncludes(C_SCLStringList & orc_Data, const bool oq_FileType)
{
   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Includes"));
   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      orc_Data.Append("#include \"stwtypes.h\"");
      orc_Data.Append("#include \"osy_hal_trg_configuration.h\" ///configuration structures");
      orc_Data.Append("");
      C_OSCExportUti::h_AddExternCStart(orc_Data);
   }
   else
   {
      orc_Data.Append("#include \"" + h_GetFileName() + ".h\"");
      orc_Data.Append("#include \"" + C_OSCHALCMagicianUtil::h_GetDatapoolName().LowerCase() + "_data_pool.h\"");
      // TODO: use C_OSCExportDataPool::h_GetFileName with correct Datapool name
      orc_Data.Append("");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add defines

   \param[out]  orc_Data         Data converted to string list
   \param[in]   orc_HalcConfig   HALC configuration
   \param[in]   orc_ProjectId    Project id for consistency check
   \param[in]   oq_FileType      Flag for .c vs. .h file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportHalc::mh_AddDefines(C_SCLStringList & orc_Data, const C_OSCHalcConfig & orc_HalcConfig,
                                    const C_SCLString & orc_ProjectId, const bool oq_FileType)
{
   const C_SCLString c_MagicName = mh_GetMagicName(orc_ProjectId);

   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Defines"));
   C_OSCExportUti::h_AddProjectIdDef(orc_Data, c_MagicName, true);

   // unique ID for compilation fails if .c and .h file were not generated for same Datapool
   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      if (orc_HalcConfig.GetDomainSize() > 0)
      {
         orc_Data.Append("///constants to access specific channel in Datapool");
         for (uint32 u32_DomainIt = 0; u32_DomainIt < orc_HalcConfig.GetDomainSize(); u32_DomainIt++)
         {
            const C_OSCHalcConfigDomain * pc_Domain = orc_HalcConfig.GetDomainConfigDataConst(u32_DomainIt);

            if (pc_Domain != NULL)
            {
               const uint32 u32_NumberOfChannels = pc_Domain->c_ChannelConfigs.size();
               // add define for each channel
               for (uint32 u32_ChannelIt = 0; u32_ChannelIt < u32_NumberOfChannels; u32_ChannelIt++)
               {
                  const C_OSCHalcConfigChannel & rc_ChannelConfig = pc_Domain->c_ChannelConfigs[u32_ChannelIt];
                  orc_Data.Append("#define HAL_" + pc_Domain->c_SingularName.UpperCase() + "_INDEX_" +
                                  rc_ChannelConfig.c_Name.UpperCase() + " (" + C_SCLString::IntToStr(u32_ChannelIt) +
                                  "U)");
               }

               // add number of channels in this domain if > 0
               if (u32_NumberOfChannels > 0)
               {
                  orc_Data.Append("#define HAL_NUMBER_OF_" + pc_Domain->c_Name.UpperCase() + " (" +
                                  C_SCLString::IntToStr(u32_NumberOfChannels) + "U)");
                  orc_Data.Append("");
               }
            }
         }
      }
   }
   else
   {
      C_OSCExportUti::h_AddProjectIdDef(orc_Data, c_MagicName, false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add global variables

   \param[out]  orc_Data         Data converted to string list
   \param[in]   orc_HalcConfig   HALC configuration
   \param[in]   oq_FileType      Flag for .c vs. .h file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportHalc::mh_AddGlobalVariables(C_SCLStringList & orc_Data, const C_OSCHalcConfig & orc_HalcConfig,
                                            const bool oq_FileType)
{
   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Global Variables"));
   orc_Data.Append("///HAL configuration table referring to Datapool values holding the actual configuration");

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      orc_Data.Append("extern const T_osy_hal_trg_config gt_HAL_Configuration;");
   }
   else
   {
      const uint32 u32_DomainNumber = orc_HalcConfig.GetDomainSize();
      C_SCLString c_Tmp;

      orc_Data.Append("const T_osy_hal_trg_config gt_HAL_Configuration =");
      orc_Data.Append("{");
      if (u32_DomainNumber > 0)
      {
         for (uint32 u32_DomainIt = 0; u32_DomainIt < u32_DomainNumber; u32_DomainIt++)
         {
            const C_OSCHalcConfigDomain * pc_Domain = orc_HalcConfig.GetDomainConfigDataConst(u32_DomainIt);

            if (pc_Domain != NULL)
            {
               const uint32 u32_NumberOfChannels = pc_Domain->c_ChannelConfigs.size();
               const bool q_IsArray = u32_NumberOfChannels > 1;
               // if channel number is exactly 1, Datapool engine does not handle it as array so we have to do so too

               orc_Data.Append("   //" + pc_Domain->c_Name);
               orc_Data.Append("   {");

               // add channel number resp. its define if there exists more than one
               if (u32_NumberOfChannels > 0)
               {
                  orc_Data.Append("      HAL_NUMBER_OF_" + pc_Domain->c_Name.UpperCase() + ",");
               }

               orc_Data.Append("      {");

               // add use case if there exists at least one
               if (pc_Domain->c_ChannelUseCases.size() > 0)
               {
                  c_Tmp = "         &gt_" + C_OSCHALCMagicianUtil::h_GetDatapoolName() +
                          "_DataPoolValues.t_ConfigurationValues.";
                  // result should be an array of length channel number resp. no array if less than 2 channels
                  c_Tmp = (u32_NumberOfChannels > 1) ?  (c_Tmp + "a") : c_Tmp;
                  c_Tmp += "u8_" + pc_Domain->c_SingularName + "UseCase";
                  c_Tmp = (u32_NumberOfChannels > 1) ?  (c_Tmp + "[0]") : c_Tmp;
                  c_Tmp += ",";

                  orc_Data.Append(c_Tmp);
               }
               // add references to corresponding elements of list parameters/configuration
               mh_AddDpListElementReferences(orc_Data, pc_Domain->c_Parameters, C_OSCHalcDefDomain::eVA_PARAM,
                                             pc_Domain->c_SingularName, q_IsArray);

               // add references to corresponding elements of list inputs
               if (pc_Domain->c_InputValues.size() > 0)
               {
                  orc_Data.Append("      },");
                  orc_Data.Append("      {");
                  mh_AddDpListElementReferences(orc_Data, pc_Domain->c_InputValues, C_OSCHalcDefDomain::eVA_INPUT,
                                                pc_Domain->c_SingularName, q_IsArray);
               }

               // add references to corresponding elements of list outputs
               if (pc_Domain->c_OutputValues.size() > 0)
               {
                  orc_Data.Append("      },");
                  orc_Data.Append("      {");
                  mh_AddDpListElementReferences(orc_Data, pc_Domain->c_OutputValues, C_OSCHalcDefDomain::eVA_OUTPUT,
                                                pc_Domain->c_SingularName, q_IsArray);
               }

               // add references to corresponding elements of list statuses
               if (pc_Domain->c_StatusValues.size() > 0)
               {
                  orc_Data.Append("      },");
                  orc_Data.Append("      {");
                  mh_AddDpListElementReferences(orc_Data, pc_Domain->c_StatusValues, C_OSCHalcDefDomain::eVA_STATUS,
                                                pc_Domain->c_SingularName, q_IsArray);
               }

               orc_Data.Append("      }");

               c_Tmp = (u32_DomainIt != (u32_DomainNumber - 1)) ? "   }," : "   }";
               orc_Data.Append(c_Tmp);
            }
         }
      }
      orc_Data.Append("};");
   }

   orc_Data.Append("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get magic name for consistency checks.

   \param[in]  orc_ProjectId  Project id for consistency check

   \return
   Magic name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportHalc::mh_GetMagicName(const C_SCLString & orc_ProjectId)
{
   return "HAL_CONFIGURATION_PROJECT_ID_" + orc_ProjectId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add references to corresponding Datapool list elements for a specific domain.

   Distinguish if the definition array has subelements or not (e.g.: parameter without/with parameter elements)
   Made to have one call for configuration, inputs, outputs and statuses.

   \param[in,out]  orc_Data                  Data converted to string list
   \param[in]      orc_DefinitionArray       Definition array: parameters/configuration, inputs, outputs or statuses
   \param[in]      ore_Type                  Type: configuration/inputs/outputs/statuses
   \param[in]      orc_DomainSingularName    Domain singular name
   \param[in]      oq_IsArray                Flag for array type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportHalc::mh_AddDpListElementReferences(stw_scl::C_SCLStringList & orc_Data,
                                                    const std::vector<C_OSCHalcDefStruct> & orc_DefinitionArray,
                                                    const C_OSCHalcDefDomain::E_VariableSelector & ore_Type,
                                                    const C_SCLString & orc_DomainSingularName, const bool oq_IsArray)
{
   C_SCLString c_Tmp;

   for (uint32 u32_ItStruct = 0; u32_ItStruct < orc_DefinitionArray.size(); u32_ItStruct++)
   {
      const C_OSCHalcDefStruct & rc_DefStruct = orc_DefinitionArray[u32_ItStruct];
      if (rc_DefStruct.c_StructElements.size() > 0)
      {
         // handle every element
         for (uint32 u32_ItElement = 0; u32_ItElement < rc_DefStruct.c_StructElements.size();
              u32_ItElement++)
         {
            const C_OSCHalcDefElement & rc_Element = rc_DefStruct.c_StructElements[u32_ItElement];
            c_Tmp = mh_GetDpListElementReference(orc_DefinitionArray, ore_Type, rc_Element, u32_ItStruct,
                                                 u32_ItElement, orc_DomainSingularName, oq_IsArray);
            c_Tmp = (((u32_ItElement + 1) < rc_DefStruct.c_StructElements.size()) ||
                     ((u32_ItStruct + 1) < orc_DefinitionArray.size())) ? (c_Tmp + ",") : c_Tmp;
            orc_Data.Append(c_Tmp);
         }
      }
      else
      {
         // handle itself
         c_Tmp = mh_GetDpListElementReference(orc_DefinitionArray, ore_Type, rc_DefStruct, u32_ItStruct,
                                              0 /*does not matter*/, orc_DomainSingularName, oq_IsArray);
         c_Tmp = ((u32_ItStruct + 1) < orc_DefinitionArray.size()) ? (c_Tmp + ",") : c_Tmp;
         orc_Data.Append(c_Tmp);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Put together all information for a reference to corresponding Datapool element

   \param[in]  orc_DefinitionArray     Definition array
   \param[in]  ore_Type                Type: configuration/inputs/outputs/statuses
   \param[in]  orc_Element             Element e.g. parameter or parameter element
   \param[in]  ou32_StructIndex        Struct index
   \param[in]  ou32_ElementIndex       Element index (irrelevant if orc_Element is not a subelement)
   \param[in]  orc_DomainSingularName  Domain singular name
   \param[in]  oq_IsArray              Flag for array type (orc_Element.GetArray() does not hold desired information)

   \return
   Parameter variable string, e.g. &gt_HAL_DataPoolValues.t_ConfigurationValues.au8_InputScaleValue[0]
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportHalc::mh_GetDpListElementReference(const std::vector<C_OSCHalcDefStruct> & orc_DefinitionArray,
                                                          const C_OSCHalcDefDomain::E_VariableSelector & ore_Type,
                                                          const C_OSCHalcDefElement & orc_Element,
                                                          const uint32 ou32_StructIndex, const uint32 ou32_ElementIndex,
                                                          const C_SCLString & orc_DomainSingularName,
                                                          const bool oq_IsArray)
{
   C_SCLString c_Return = "";

   tgl_assert(C_OSCHALCMagicianUtil::h_GetVariableName(orc_DefinitionArray, ou32_StructIndex,
                                                       ou32_ElementIndex, orc_DomainSingularName,
                                                       c_Return) == C_NO_ERR);
   c_Return = C_OSCExportUti::h_GetElementCName(c_Return, oq_IsArray,
                                                orc_Element.GetType());
   c_Return = "         &gt_" + C_OSCHALCMagicianUtil::h_GetDatapoolName() + "_DataPoolValues.t_" +
              C_OSCHALCMagicianUtil::h_GetListName(ore_Type) + "Values." + c_Return;

   return c_Return;
}
