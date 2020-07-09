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
C_OSCExportHalc::C_OSCExportHalc(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create file name for exported .c and .h file.

   \return
   File name

   \param[in]      oq_IsSafe     true: generate code for safe HALC
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportHalc::h_GetFileName(const bool oq_IsSafe)
{
   return (oq_IsSafe == true) ? "hal_safe_configuration" : "hal_non_safe_configuration";
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

   //calculate hash value from a combination of HALC configuration and Datapool definition:
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
      s32_Retval = mh_CreateHeaderFile(orc_ExportToolInfo, orc_Path, orc_HalcConfig, c_ProjectId,
                                       orc_Datapool.q_IsSafety);
   }

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_CreateImplementationFile(orc_ExportToolInfo, orc_Path, orc_HalcConfig, c_ProjectId,
                                               orc_Datapool.q_IsSafety);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create header file

   \param[in]  orc_ExportToolInfo   Information about calling executable (name + version)
   \param[in]  orc_Path             Directory path for created file
   \param[in]  orc_HalcConfig       HALC configuration
   \param[in]  orc_ProjectId        Project id for consistency check
   \param[in]  oq_IsSafe            true: generate code for safe HALC

   \return
   C_NO_ERR    Success
   C_RD_WR     Operation failure: cannot store file
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportHalc::mh_CreateHeaderFile(const stw_scl::C_SCLString & orc_ExportToolInfo,
                                            const stw_scl::C_SCLString & orc_Path,
                                            const C_OSCHalcConfig & orc_HalcConfig,
                                            const stw_scl::C_SCLString & orc_ProjectId, const bool oq_IsSafe)
{
   sint32 s32_Retval;
   C_SCLStringList c_Data;

   // add header
   mh_AddHeader(orc_ExportToolInfo, c_Data, mhq_IS_HEADER_FILE, oq_IsSafe);

   // add includes
   mh_AddIncludes(c_Data, mhq_IS_HEADER_FILE, oq_IsSafe);

   // add defines
   mh_AddDefines(c_Data, orc_HalcConfig, orc_ProjectId, mhq_IS_HEADER_FILE, oq_IsSafe);

   // add types
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Types"));
   c_Data.Append("");

   // add global variables
   mh_AddGlobalVariables(c_Data, orc_HalcConfig, mhq_IS_HEADER_FILE, oq_IsSafe);

   // add function prototypes
   C_OSCExportUti::h_AddProjIdFunctionPrototype(c_Data, mh_GetMagicName(orc_ProjectId, oq_IsSafe));

   // add implementation
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Implementation"));
   C_OSCExportUti::h_AddExternCEnd(c_Data);
   c_Data.Append("#endif");

   // finally save to file
   s32_Retval = C_OSCExportUti::h_SaveToFile(c_Data, orc_Path, h_GetFileName(oq_IsSafe), true);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create implementation file

   \param[in]  orc_ExportToolInfo   Information about calling executable (name + version)
   \param[in]  orc_Path             Storage path for created file
   \param[in]  orc_HalcConfig       HALC configuration
   \param[in]  orc_ProjectId        Project id for consistency check
   \param[in]  oq_IsSafe            true: generate code for safe HALC

   \return
   C_NO_ERR    Success
   C_RD_WR     Operation failure: cannot store file
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportHalc::mh_CreateImplementationFile(const stw_scl::C_SCLString & orc_ExportToolInfo,
                                                    const stw_scl::C_SCLString & orc_Path,
                                                    const C_OSCHalcConfig & orc_HalcConfig,
                                                    const stw_scl::C_SCLString & orc_ProjectId, const bool oq_IsSafe)
{
   sint32 s32_Retval;

   C_SCLStringList c_Data;

   // add header
   mh_AddHeader(orc_ExportToolInfo, c_Data, mhq_IS_IMPLEMENTATION_FILE, oq_IsSafe);

   // add includes
   mh_AddIncludes(c_Data, mhq_IS_IMPLEMENTATION_FILE, oq_IsSafe);

   // add defines
   mh_AddDefines(c_Data, orc_HalcConfig, orc_ProjectId, mhq_IS_IMPLEMENTATION_FILE, oq_IsSafe);

   // add types
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Types"));
   c_Data.Append("");

   // add global variables
   mh_AddGlobalVariables(c_Data, orc_HalcConfig, mhq_IS_IMPLEMENTATION_FILE, oq_IsSafe);

   // add modul global variables
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Module Global Variables"));
   c_Data.Append("");

   // add function prototypes
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Module Global Function Prototypes"));
   c_Data.Append("");

   // add implementation
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Implementation"));

   // finally save to file
   s32_Retval = C_OSCExportUti::h_SaveToFile(c_Data, orc_Path, h_GetFileName(oq_IsSafe), false);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add file header

   \param[in]      orc_ExportToolInfo  Information about calling executable (name + version)
   \param[in,out]  orc_Data            Data converted to string list
   \param[in]      oq_FileType         Flag for .c vs. .h file
   \param[in]      oq_IsSafe           true: generate code for safe HALC
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportHalc::mh_AddHeader(const C_SCLString & orc_ExportToolInfo, C_SCLStringList & orc_Data,
                                   const bool oq_FileType, const bool oq_IsSafe)
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
      const C_SCLString c_HeaderGuard = h_GetFileName(oq_IsSafe).UpperCase() + "H";

      orc_Data.Append("#ifndef " + c_HeaderGuard);
      orc_Data.Append("#define " + c_HeaderGuard);
   }

   orc_Data.Append("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add includes

   \param[in,out]  orc_Data      Data converted to string list
   \param[in]      oq_FileType   Flag for .c vs. .h file
   \param[in]      oq_IsSafe     true: generate code for safe HALC
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportHalc::mh_AddIncludes(C_SCLStringList & orc_Data, const bool oq_FileType, const bool oq_IsSafe)
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
      orc_Data.Append("#include <stddef.h> //for NULL");
      orc_Data.Append("#include \"" + h_GetFileName(oq_IsSafe) + ".h\"");
      orc_Data.Append("#include \"" + C_OSCHALCMagicianUtil::h_GetDatapoolName(oq_IsSafe).LowerCase() +
                      "_data_pool.h\"");
      orc_Data.Append("");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add defines

   \param[in,out]  orc_Data         Data converted to string list
   \param[in]      orc_HalcConfig   HALC configuration
   \param[in]      orc_ProjectId    Project id for consistency check
   \param[in]      oq_FileType      Flag for .c vs. .h file
   \param[in]      oq_IsSafe        true: generate code for safe HALC
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportHalc::mh_AddDefines(C_SCLStringList & orc_Data, const C_OSCHalcConfig & orc_HalcConfig,
                                    const C_SCLString & orc_ProjectId, const bool oq_FileType, const bool oq_IsSafe)
{
   const C_SCLString c_MagicName = mh_GetMagicName(orc_ProjectId, oq_IsSafe);

   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Defines"));

   // unique ID for compilation fails if .c and .h file were not generated for same Datapool
   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      C_OSCExportUti::h_AddProjectIdDef(orc_Data, c_MagicName, true);
      if (orc_HalcConfig.GetDomainSize() > 0U)
      {
         orc_Data.Append("///constants to access specific channel in Datapool");
         for (uint32 u32_DomainIt = 0U; u32_DomainIt < orc_HalcConfig.GetDomainSize(); u32_DomainIt++)
         {
            const C_OSCHalcConfigDomain * const pc_Domain = orc_HalcConfig.GetDomainConfigDataConst(u32_DomainIt);

            if (pc_Domain != NULL)
            {
               const C_SCLString c_DataPoolName = C_OSCHALCMagicianUtil::h_GetDatapoolName(oq_IsSafe) + "_";
               uint32 u32_NumberOfAssignedChannels = 0U;
               // add define for each assigned channel

               for (uint32 u32_ChannelIt = 0U; u32_ChannelIt < pc_Domain->c_ChannelConfigs.size(); u32_ChannelIt++)
               {
                  const C_OSCHalcConfigChannel & rc_ChannelConfig = pc_Domain->c_ChannelConfigs[u32_ChannelIt];
                  if (rc_ChannelConfig.q_SafetyRelevant == oq_IsSafe)
                  {
                     orc_Data.Append(
                        "#define " + c_DataPoolName + pc_Domain->c_SingularName.UpperCase() + "_INDEX_" +
                        rc_ChannelConfig.c_Name.UpperCase() + " (" +
                        C_SCLString::IntToStr(u32_NumberOfAssignedChannels) + "U)");
                     u32_NumberOfAssignedChannels++;
                  }
               }

               // add number of assigned channels (also add if 0 as we want to use the constant in the .c file)
               // but only if there are in fact channels (not for channelless domains)
               if (pc_Domain->c_ChannelConfigs.size() > 0U)
               {
                  orc_Data.Append(
                     "#define " + c_DataPoolName + "NUMBER_OF_" + pc_Domain->c_Name.UpperCase() + " (" +
                     C_SCLString::IntToStr(u32_NumberOfAssignedChannels) + "U)");
                  orc_Data.Append("");
               }
            }
         }
      }
   }
   else
   {
      orc_Data.Append("///check for correct version of structure definitions");
      orc_Data.Append("#if OSY_HAL_DEFINITION_VERSION != 0x" +
                      C_SCLString::IntToHex(static_cast<sint64>(orc_HalcConfig.u32_ContentVersion), 4U) + "U");
      orc_Data.Append("///if compilation fails here the HALC driver version does not match the version of the PC-side "
                      "HALC definition");
      orc_Data.Append("static T_osy_non_existing_type_" + orc_ProjectId + " mt_Variable;");
      orc_Data.Append("#endif");
      orc_Data.Append("");

      C_OSCExportUti::h_AddProjectIdDef(orc_Data, c_MagicName, false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add global variables

   \param[in,out]  orc_Data         Data converted to string list
   \param[in]      orc_HalcConfig   HALC configuration
   \param[in]      oq_FileType      Flag for .c vs. .h file
   \param[in]      oq_IsSafe        true: generate code for safe HALC
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportHalc::mh_AddGlobalVariables(C_SCLStringList & orc_Data, const C_OSCHalcConfig & orc_HalcConfig,
                                            const bool oq_FileType, const bool oq_IsSafe)
{
   //compose name of HAL configuration structure based on name of Datapool
   const C_SCLString c_ConfigurationStructureName = "gt_" + C_OSCHALCMagicianUtil::h_GetDatapoolName(oq_IsSafe) +
                                                    "_Configuration";

   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Global Variables"));
   orc_Data.Append("///HAL configuration table referring to Datapool values holding the actual configuration");

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      orc_Data.Append("extern const T_osy_hal_trg_config " + c_ConfigurationStructureName + ";");
   }
   else
   {
      const uint32 u32_DomainNumber = orc_HalcConfig.GetDomainSize();

      orc_Data.Append("const T_osy_hal_trg_config " + c_ConfigurationStructureName + " =");
      orc_Data.Append("{");
      if (u32_DomainNumber > 0U)
      {
         for (uint32 u32_DomainIt = 0U; u32_DomainIt < u32_DomainNumber; u32_DomainIt++)
         {
            const C_OSCHalcConfigDomain * pc_Domain = orc_HalcConfig.GetDomainConfigDataConst(u32_DomainIt);
            C_SCLString c_Tmp;

            if (pc_Domain != NULL)
            {
               bool q_IsArray;
               const C_OSCHalcDefChannelValues * pc_ChannelValues;
               std::vector<C_OSCHalcConfigChannel> c_ChannelConfigs;
               uint32 u32_NumberOfAssignedChannels = 0U;

               orc_Data.Append("   //" + pc_Domain->c_Name);
               orc_Data.Append("   {");

               // add channel number resp. its define if there exists more than one channel (i.e. it is not just a
               // channelless domain)
               if (pc_Domain->c_ChannelConfigs.size() > 0U)
               {
                  orc_Data.Append("      " + C_OSCHALCMagicianUtil::h_GetDatapoolName(oq_IsSafe) + "_NUMBER_OF_" +
                                  pc_Domain->c_Name.UpperCase() + ",");
               }

               orc_Data.Append("      {");

               for (uint32 u32_Channel = 0U; u32_Channel < pc_Domain->c_ChannelConfigs.size(); u32_Channel++)
               {
                  if (pc_Domain->c_ChannelConfigs[u32_Channel].q_SafetyRelevant == oq_IsSafe)
                  {
                     u32_NumberOfAssignedChannels++;
                  }
               }

               // add channel number and use case and pointers if there exists at least one use case ...
               if (pc_Domain->c_ChannelUseCases.size() > 0U)
               {
                  //... and at least one channel is assigned to the desired safetyty:
                  if (u32_NumberOfAssignedChannels > 0U)
                  {
                     // result should be an array of length channel number resp. no array if less than 2 channels
                     const C_SCLString c_Head = "         &gt_" + C_OSCHALCMagicianUtil::h_GetDatapoolName(oq_IsSafe) +
                                                "_DataPoolValues.t_ConfigurationValues." +
                                                ((u32_NumberOfAssignedChannels > 1U) ? "a" : "");
                     const C_SCLString c_Tail =
                        ((u32_NumberOfAssignedChannels > 1U) ? "[0]" : static_cast<C_SCLString>("")) + ",";

                     //if we have use cases we also have channel numbers
                     c_Tmp = c_Head + "u16_" + pc_Domain->c_SingularName + "ChannelNumber" + c_Tail;
                     orc_Data.Append(c_Tmp);

                     c_Tmp = c_Head + "u8_" + pc_Domain->c_SingularName + "UseCase" + c_Tail;
                     orc_Data.Append(c_Tmp);
                  }
                  else
                  {
                     //no channels assigned to this safetyty: no data to reference: use NULL:
                     orc_Data.Append("         NULL,"); //channel number
                     orc_Data.Append("         NULL,"); //use case
                  }
               }

               // add references to corresponding elements of parameters/configuration
               if (pc_Domain->c_Channels.size() == 0)
               {
                  //we have a domain: use domain parameters
                  pc_ChannelValues = &pc_Domain->c_DomainValues;
                  c_ChannelConfigs.push_back(pc_Domain->c_DomainConfig); //just one config ...
                  q_IsArray = false;
               }
               else
               {
                  pc_ChannelValues = &pc_Domain->c_ChannelValues;
                  c_ChannelConfigs = pc_Domain->c_ChannelConfigs;
                  q_IsArray = (u32_NumberOfAssignedChannels > 1U);
               }

               mh_AddDpListElementReferences(orc_Data, pc_ChannelValues->c_Parameters, c_ChannelConfigs,
                                             C_OSCHalcDefDomain::eVA_PARAM,
                                             pc_Domain->c_SingularName, q_IsArray, oq_IsSafe);

               // add references to corresponding elements of list inputs
               if (pc_ChannelValues->c_InputValues.size() > 0U)
               {
                  orc_Data.Append("      },");
                  orc_Data.Append("      {");
                  mh_AddDpListElementReferences(orc_Data, pc_ChannelValues->c_InputValues, c_ChannelConfigs,
                                                C_OSCHalcDefDomain::eVA_INPUT,
                                                pc_Domain->c_SingularName, q_IsArray, oq_IsSafe);
               }

               // add references to corresponding elements of list outputs
               if (pc_ChannelValues->c_OutputValues.size() > 0U)
               {
                  orc_Data.Append("      },");
                  orc_Data.Append("      {");
                  mh_AddDpListElementReferences(orc_Data, pc_ChannelValues->c_OutputValues, c_ChannelConfigs,
                                                C_OSCHalcDefDomain::eVA_OUTPUT,
                                                pc_Domain->c_SingularName, q_IsArray, oq_IsSafe);
               }

               // add references to corresponding elements of list statuses
               if (pc_ChannelValues->c_StatusValues.size() > 0)
               {
                  orc_Data.Append("      },");
                  orc_Data.Append("      {");
                  mh_AddDpListElementReferences(orc_Data, pc_ChannelValues->c_StatusValues, c_ChannelConfigs,
                                                C_OSCHalcDefDomain::eVA_STATUS,
                                                pc_Domain->c_SingularName, q_IsArray, oq_IsSafe);
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
   \param[in]  oq_IsSafe      true: generate code for safe HALC

   \return
   Magic name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportHalc::mh_GetMagicName(const C_SCLString & orc_ProjectId, const bool oq_IsSafe)
{
   const C_SCLString c_Prefix = C_OSCHALCMagicianUtil::h_GetDatapoolName(oq_IsSafe);

   return c_Prefix + "_CONFIGURATION_PROJECT_ID_" + orc_ProjectId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add references to corresponding Datapool list elements for a specific domain.

   Distinguish if the definition array has subelements or not (e.g.: parameter without/with parameter elements)
   Made to have one call for configuration, inputs, outputs and statuses.

   \param[in,out]  orc_Data                  Data converted to string list
   \param[in]      orc_DefinitionArray       Definition array: parameters/configuration, inputs, outputs or statuses
   \param[in]      orc_ConfigArray           Configuration array: contains configured settings for each channel
   \param[in]      ore_Type                  Type: configuration/inputs/outputs/statuses
   \param[in]      orc_DomainSingularName    Domain singular name
   \param[in]      oq_IsArray                Flag for array type
   \param[in]      oq_IsSafe                 true: generate code for safe HALC
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportHalc::mh_AddDpListElementReferences(stw_scl::C_SCLStringList & orc_Data,
                                                    const std::vector<C_OSCHalcDefStruct> & orc_DefinitionArray,
                                                    const std::vector<C_OSCHalcConfigChannel> & orc_ConfigArray,
                                                    const C_OSCHalcDefDomain::E_VariableSelector & ore_Type,
                                                    const C_SCLString & orc_DomainSingularName, const bool oq_IsArray,
                                                    const bool oq_IsSafe)
{
   C_SCLString c_Tmp;
   bool q_AtLeastOneChannelIsAssigned = false;

   //We only can/need to add pointers to something useful if at least one of the channels is assigned to this
   // configuration. Check whether this is the case:
   for (uint32 u32_ItStruct = 0; u32_ItStruct < orc_ConfigArray.size(); u32_ItStruct++)
   {
      if (orc_ConfigArray[u32_ItStruct].q_SafetyRelevant == oq_IsSafe)
      {
         q_AtLeastOneChannelIsAssigned = true;
         break;
      }
   }

   for (uint32 u32_ItStruct = 0; u32_ItStruct < orc_DefinitionArray.size(); u32_ItStruct++)
   {
      const C_OSCHalcDefStruct & rc_DefStruct = orc_DefinitionArray[u32_ItStruct];
      // structure or POD element ?
      if (rc_DefStruct.c_StructElements.size() > 0)
      {
         // structure -> handle every element
         for (uint32 u32_ItElement = 0; u32_ItElement < rc_DefStruct.c_StructElements.size();
              u32_ItElement++)
         {
            const C_OSCHalcDefElement & rc_Element = rc_DefStruct.c_StructElements[u32_ItElement];

            c_Tmp = "         ";
            if (q_AtLeastOneChannelIsAssigned == true)
            {
               c_Tmp += mh_GetDpListElementReference(orc_DefinitionArray, ore_Type, rc_Element, u32_ItStruct,
                                                     u32_ItElement, orc_DomainSingularName, oq_IsArray, oq_IsSafe);
            }
            else
            {
               c_Tmp += "NULL";
            }
            c_Tmp = (((u32_ItElement + 1) < rc_DefStruct.c_StructElements.size()) ||
                     ((u32_ItStruct + 1) < orc_DefinitionArray.size())) ? (c_Tmp + ",") : c_Tmp;
            orc_Data.Append(c_Tmp);
         }
      }
      else
      {
         // POD -> handle itself
         c_Tmp = "         ";
         if (q_AtLeastOneChannelIsAssigned == true)
         {
            c_Tmp += mh_GetDpListElementReference(orc_DefinitionArray, ore_Type, rc_DefStruct, u32_ItStruct,
                                                  0 /*does not matter*/, orc_DomainSingularName, oq_IsArray, oq_IsSafe);
         }
         else
         {
            c_Tmp += "NULL";
         }
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
   \param[in]  oq_IsSafe               true: generate code for safe HALC

   \return
   Parameter variable string, e.g. &gt_HAL_DataPoolValues.t_ConfigurationValues.au8_InputScaleValue[0]
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportHalc::mh_GetDpListElementReference(const std::vector<C_OSCHalcDefStruct> & orc_DefinitionArray,
                                                          const C_OSCHalcDefDomain::E_VariableSelector & ore_Type,
                                                          const C_OSCHalcDefElement & orc_Element,
                                                          const uint32 ou32_StructIndex, const uint32 ou32_ElementIndex,
                                                          const C_SCLString & orc_DomainSingularName,
                                                          const bool oq_IsArray, const bool oq_IsSafe)
{
   C_SCLString c_Return;

   tgl_assert(C_OSCHALCMagicianUtil::h_GetVariableName(orc_DefinitionArray, ou32_StructIndex,
                                                       ou32_ElementIndex, orc_DomainSingularName,
                                                       c_Return) == C_NO_ERR);
   c_Return = C_OSCExportUti::h_GetElementCName(c_Return, oq_IsArray, orc_Element.GetType());
   c_Return = "&gt_" + C_OSCHALCMagicianUtil::h_GetDatapoolName(oq_IsSafe) + "_DataPoolValues.t_" +
              C_OSCHALCMagicianUtil::h_GetListName(ore_Type) + "Values." + c_Return;

   return c_Return;
}
