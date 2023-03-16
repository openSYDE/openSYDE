//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief     Export HALC configuration of an openSYDE node.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscExportHalc.hpp"
#include "C_SclStringList.hpp"
#include "TglUtils.hpp"
#include "C_OscLoggingHandler.hpp"

#include "C_OscExportUti.hpp"
#include "C_OscHalcMagicianUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create file name for exported .c and .h file.

   \param[in]  oq_IsSafe   true: generate code for safe HALC

   \return
   File name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscExportHalc::h_GetFileName(const bool oq_IsSafe)
{
   return (oq_IsSafe == true) ? "hal_safe_configuration" : "hal_non_safe_configuration";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert overall code structure version to HALC code subversion.

   The overall code generation version decodes the format versions of parametrization, COMM, HALC code.

   \param[in]  ou16_GenCodeVersion  Overall code format version

   \return version of HALC code
*/
//----------------------------------------------------------------------------------------------------------------------
uint16_t C_OscExportHalc::h_ConvertOverallCodeVersion(const uint16_t ou16_GenCodeVersion)
{
   uint16_t u16_Return;

   //Technically HALC code is "compatible" with overall versions up to 5. So don't impose any artificial limits here.
   if (ou16_GenCodeVersion <= 6U)
   {
      u16_Return = 1U;
   }
   else
   {
      // should never occur...
      tgl_assert(false);
      u16_Return = 0U;
   }

   return u16_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create source code

   \param[in]  orc_Path             Storage path for created file
   \param[in]  ou16_GenCodeVersion  version of structure (generate code as specified for this version)
   \param[in]  orc_HalcConfig       HALC configuration
   \param[in]  orc_Datapool         Datapool definition
   \param[in]  orc_ExportToolInfo   Information about calling executable (name + version)

   \return
   C_NO_ERR    Success
   C_RD_WR     Operation failure: cannot store file
   C_NOACT     Datapool is not of type HALC
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportHalc::h_CreateSourceCode(const stw::scl::C_SclString & orc_Path, const uint16_t ou16_GenCodeVersion,
                                            const C_OscHalcConfig & orc_HalcConfig,
                                            const C_OscNodeDataPool & orc_Datapool,
                                            const stw::scl::C_SclString & orc_ExportToolInfo)
{
   int32_t s32_Retval = C_NO_ERR;
   uint32_t u32_HashValue = 0U;

   //calculate hash value from a combination of HALC configuration and Datapool definition:
   orc_HalcConfig.CalcHash(u32_HashValue);
   orc_Datapool.CalcHash(u32_HashValue);
   const C_SclString c_ProjectId = C_SclString::IntToStr(u32_HashValue);

   // make sure version is known
   if (ou16_GenCodeVersion > C_OscNodeApplication::hu16_HIGHEST_KNOWN_CODE_VERSION)
   {
      s32_Retval = C_NOACT;
      osc_write_log_error("Creating source code",
                          "Did not generate code for HALC configuration because code format version is unknown.");
   }

   // make sure Datapool is of type HALC
   if ((orc_Datapool.e_Type != C_OscNodeDataPool::eHALC) &&
       (orc_Datapool.e_Type != C_OscNodeDataPool::eHALC_NVM))
   {
      osc_write_log_error("Creating source code",
                          "Did not generate code for HALC configuration of Datapool \"" + orc_Datapool.c_Name +
                          "\" because Datapool type is not HALC.");
      s32_Retval = C_NOACT;
   }

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_CreateHeaderFile(orc_ExportToolInfo, orc_Path, orc_HalcConfig, c_ProjectId,
                                       orc_Datapool.q_IsSafety, ou16_GenCodeVersion);
   }

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_CreateImplementationFile(orc_ExportToolInfo, orc_Path, orc_HalcConfig, c_ProjectId,
                                               orc_Datapool.q_IsSafety, ou16_GenCodeVersion);
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
   \param[in]  ou16_GenCodeVersion  version of structure (generate code as specified for this version)

   \return
   C_NO_ERR    Success
   C_RD_WR     Operation failure: cannot store file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportHalc::mh_CreateHeaderFile(const stw::scl::C_SclString & orc_ExportToolInfo,
                                             const stw::scl::C_SclString & orc_Path,
                                             const C_OscHalcConfig & orc_HalcConfig,
                                             const stw::scl::C_SclString & orc_ProjectId, const bool oq_IsSafe,
                                             const uint16_t ou16_GenCodeVersion)
{
   int32_t s32_Retval;
   C_SclStringList c_Data;

   // add header
   mh_AddHeader(orc_ExportToolInfo, c_Data, mhq_IS_HEADER_FILE, oq_IsSafe);

   // add includes
   mh_AddIncludes(c_Data, mhq_IS_HEADER_FILE, oq_IsSafe);

   // add defines
   mh_AddDefines(c_Data, orc_HalcConfig, orc_ProjectId, mhq_IS_HEADER_FILE, oq_IsSafe, ou16_GenCodeVersion);

   // add types
   c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Types"));
   c_Data.Append("");

   // add global variables
   mh_AddGlobalVariables(c_Data, orc_HalcConfig, mhq_IS_HEADER_FILE, oq_IsSafe);

   // add function prototypes
   C_OscExportUti::h_AddProjIdFunctionPrototype(c_Data, mh_GetMagicName(orc_ProjectId, oq_IsSafe));

   // add implementation
   c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Implementation"));
   C_OscExportUti::h_AddExternCeEnd(c_Data);
   c_Data.Append("#endif");

   // finally save to file
   s32_Retval = C_OscExportUti::h_SaveToFile(c_Data, orc_Path, h_GetFileName(oq_IsSafe), true);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create implementation file

   \param[in]  orc_ExportToolInfo   Information about calling executable (name + version)
   \param[in]  orc_Path             Storage path for created file
   \param[in]  orc_HalcConfig       HALC configuration
   \param[in]  orc_ProjectId        Project id for consistency check
   \param[in]  oq_IsSafe            true: generate code for safe HALC
   \param[in]  ou16_GenCodeVersion  version of structure (generate code as specified for this version)

   \return
   C_NO_ERR    Success
   C_RD_WR     Operation failure: cannot store file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportHalc::mh_CreateImplementationFile(const stw::scl::C_SclString & orc_ExportToolInfo,
                                                     const stw::scl::C_SclString & orc_Path,
                                                     const C_OscHalcConfig & orc_HalcConfig,
                                                     const stw::scl::C_SclString & orc_ProjectId, const bool oq_IsSafe,
                                                     const uint16_t ou16_GenCodeVersion)
{
   int32_t s32_Retval;

   C_SclStringList c_Data;

   // add header
   mh_AddHeader(orc_ExportToolInfo, c_Data, mhq_IS_IMPLEMENTATION_FILE, oq_IsSafe);

   // add includes
   mh_AddIncludes(c_Data, mhq_IS_IMPLEMENTATION_FILE, oq_IsSafe);

   // add defines
   mh_AddDefines(c_Data, orc_HalcConfig, orc_ProjectId, mhq_IS_IMPLEMENTATION_FILE, oq_IsSafe, ou16_GenCodeVersion);

   // add types
   c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Types"));
   c_Data.Append("");

   // add global variables
   mh_AddGlobalVariables(c_Data, orc_HalcConfig, mhq_IS_IMPLEMENTATION_FILE, oq_IsSafe);

   // add modul global variables
   c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Module Global Variables"));
   c_Data.Append("");

   // add function prototypes
   c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Module Global Function Prototypes"));
   c_Data.Append("");

   // add implementation
   c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Implementation"));

   // finally save to file
   s32_Retval = C_OscExportUti::h_SaveToFile(c_Data, orc_Path, h_GetFileName(oq_IsSafe), false);

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
void C_OscExportHalc::mh_AddHeader(const C_SclString & orc_ExportToolInfo, C_SclStringList & orc_Data,
                                   const bool oq_FileType, const bool oq_IsSafe)
{
   const C_SclString c_Tmp = (oq_FileType == mhq_IS_IMPLEMENTATION_FILE) ? "Source file with constant definitions" :
                             "Header file with constant and global definitions";

   orc_Data.Append(C_OscExportUti::h_GetHeaderSeparator());
   orc_Data.Append("/*!");
   orc_Data.Append("   \\file");
   orc_Data.Append("   \\brief       openSYDE HAL configuration definition (" + c_Tmp + ")");
   orc_Data.Append("");
   if (oq_FileType == mhq_IS_IMPLEMENTATION_FILE)
   {
      orc_Data.Append("   Defines the hardware abstraction layer configuration.");
      orc_Data.Append("");
   }

   orc_Data.Append(C_OscExportUti::h_GetCreationToolInfo(orc_ExportToolInfo));
   orc_Data.Append("*/");
   orc_Data.Append(C_OscExportUti::h_GetHeaderSeparator());

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      const C_SclString c_HeaderGuard = h_GetFileName(oq_IsSafe).UpperCase() + "H";

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
void C_OscExportHalc::mh_AddIncludes(C_SclStringList & orc_Data, const bool oq_FileType, const bool oq_IsSafe)
{
   orc_Data.Append(C_OscExportUti::h_GetSectionSeparator("Includes"));
   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      orc_Data.Append("#include \"stwtypes.h\"");
      orc_Data.Append("#include \"osy_hal_trg_configuration.h\" ///configuration structures");
      orc_Data.Append("");
      C_OscExportUti::h_AddExternCeStart(orc_Data);
   }
   else
   {
      orc_Data.Append("#include <stddef.h> //for NULL");
      orc_Data.Append("#include \"" + h_GetFileName(oq_IsSafe) + ".h\"");
      orc_Data.Append("#include \"" + C_OscHalcMagicianUtil::h_GetDatapoolName(oq_IsSafe).LowerCase() +
                      "_data_pool.h\"");
      orc_Data.Append("");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add defines

   \param[in,out]  orc_Data               Data converted to string list
   \param[in]      orc_HalcConfig         HALC configuration
   \param[in]      orc_ProjectId          Project id for consistency check
   \param[in]      oq_FileType            Flag for .c vs. .h file
   \param[in]      oq_IsSafe              true: generate code for safe HALC
   \param[in]      ou16_GenCodeVersion    version of structure (generate code as specified for this version)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportHalc::mh_AddDefines(C_SclStringList & orc_Data, const C_OscHalcConfig & orc_HalcConfig,
                                    const C_SclString & orc_ProjectId, const bool oq_FileType, const bool oq_IsSafe,
                                    const uint16_t ou16_GenCodeVersion)
{
   const C_SclString c_MagicName = mh_GetMagicName(orc_ProjectId, oq_IsSafe);

   orc_Data.Append(C_OscExportUti::h_GetSectionSeparator("Defines"));

   // unique ID for compilation fails if .c and .h file were not generated for same Datapool
   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      C_OscExportUti::h_AddProjectIdDef(orc_Data, c_MagicName, true);
      if (orc_HalcConfig.GetDomainSize() > 0U)
      {
         orc_Data.Append("///constants to access specific channel in Datapool");
         for (uint32_t u32_DomainIt = 0U; u32_DomainIt < orc_HalcConfig.GetDomainSize(); u32_DomainIt++)
         {
            const C_OscHalcConfigDomain * const pc_Domain = orc_HalcConfig.GetDomainConfigDataConst(u32_DomainIt);

            if (pc_Domain != NULL)
            {
               const C_SclString c_DataPoolName = C_OscHalcMagicianUtil::h_GetDatapoolName(oq_IsSafe) + "_";
               uint32_t u32_NumberOfAssignedChannels = 0U;
               // add define for each assigned channel

               for (uint32_t u32_ChannelIt = 0U; u32_ChannelIt < pc_Domain->c_ChannelConfigs.size(); u32_ChannelIt++)
               {
                  const C_OscHalcConfigChannel & rc_ChannelConfig = pc_Domain->c_ChannelConfigs[u32_ChannelIt];
                  if (mh_DropChannel(rc_ChannelConfig.q_SafetyRelevant, orc_HalcConfig.e_SafetyMode,
                                     oq_IsSafe) == false)
                  {
                     orc_Data.Append(
                        "#define " + c_DataPoolName + pc_Domain->c_SingularName.UpperCase() + "_INDEX_" +
                        rc_ChannelConfig.c_Name.UpperCase() + " (" +
                        C_SclString::IntToStr(u32_NumberOfAssignedChannels) + "U)");
                     u32_NumberOfAssignedChannels++;
                  }
               }

               // add number of assigned channels (also add if 0 as we want to use the constant in the .c file)
               // but only if there are in fact channels (not for channelless domains)
               if (pc_Domain->c_ChannelConfigs.size() > 0U)
               {
                  orc_Data.Append(
                     "#define " + c_DataPoolName + "NUMBER_OF_" + pc_Domain->c_Name.UpperCase() + " (" +
                     C_SclString::IntToStr(u32_NumberOfAssignedChannels) + "U)");
                  orc_Data.Append("");
               }
            }
         }
      }
   }
   else
   {
      orc_Data.Append("///check for correct version of structure definitions");
      orc_Data.Append("#if OSY_HAL_DEFINITION_FORMAT_VERSION != 0x" +
                      C_SclString::IntToHex(static_cast<int64_t>(C_OscExportHalc::h_ConvertOverallCodeVersion(
                                                                    ou16_GenCodeVersion)), 4U) + "U");

      orc_Data.Append("///if compilation fails here the HALC driver version does not match the format version of the "
                      "generated code");
      orc_Data.Append("static T_osy_non_existing_type_" + orc_ProjectId + " mt_Variable;");
      orc_Data.Append("#endif");
      orc_Data.Append("");
      orc_Data.Append("#if OSY_HAL_DEFINITION_VERSION != 0x" +
                      C_SclString::IntToHex(static_cast<int64_t>(orc_HalcConfig.u32_ContentVersion), 4U) + "U");
      orc_Data.Append("///if compilation fails here the HALC driver version does not match the version of the PC-side "
                      "HALC definition");
      orc_Data.Append("static T_osy_non_existing_type_" + orc_ProjectId + " mt_Variable;");
      orc_Data.Append("#endif");
      orc_Data.Append("");

      C_OscExportUti::h_AddProjectIdDef(orc_Data, c_MagicName, false);
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
void C_OscExportHalc::mh_AddGlobalVariables(C_SclStringList & orc_Data, const C_OscHalcConfig & orc_HalcConfig,
                                            const bool oq_FileType, const bool oq_IsSafe)
{
   //compose name of HAL configuration structure based on name of Datapool
   const C_SclString c_ConfigurationStructureName = "gt_" + C_OscHalcMagicianUtil::h_GetDatapoolName(oq_IsSafe) +
                                                    "_Configuration";

   orc_Data.Append(C_OscExportUti::h_GetSectionSeparator("Global Variables"));
   orc_Data.Append("///HAL configuration table referring to Datapool values holding the actual configuration");

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      orc_Data.Append("extern const T_osy_hal_trg_config " + c_ConfigurationStructureName + ";");
   }
   else
   {
      const uint32_t u32_DomainNumber = orc_HalcConfig.GetDomainSize();

      orc_Data.Append("const T_osy_hal_trg_config " + c_ConfigurationStructureName + " =");
      orc_Data.Append("{");

      orc_Data.Append("   " + C_OscHalcMagicianUtil::h_GetDatapoolName(oq_IsSafe) +
                      "_DATA_POOL_INDEX, //index of associated Datapool");

      if (u32_DomainNumber > 0U)
      {
         for (uint32_t u32_DomainIt = 0U; u32_DomainIt < u32_DomainNumber; u32_DomainIt++)
         {
            const C_OscHalcConfigDomain * const pc_Domain = orc_HalcConfig.GetDomainConfigDataConst(u32_DomainIt);
            C_SclString c_Tmp;

            if (pc_Domain != NULL)
            {
               bool q_IsArray;
               const C_OscHalcDefChannelValues * pc_ChannelValues;
               std::vector<C_OscHalcConfigChannel> c_ChannelConfigs;
               uint32_t u32_NumberOfAssignedChannels = 0U;
               bool q_BracketIsOpen = false;
               const bool q_DomainHasChannels = (pc_Domain->c_Channels.size() > 0U) ? true : false;

               orc_Data.Append("   //" + pc_Domain->c_Name);
               orc_Data.Append("   {");

               // get number of channels assigned to this safetyness in configured safety mode:
               for (uint32_t u32_Channel = 0U; u32_Channel < pc_Domain->c_ChannelConfigs.size(); u32_Channel++)
               {
                  if (mh_DropChannel(pc_Domain->c_ChannelConfigs[u32_Channel].q_SafetyRelevant,
                                     orc_HalcConfig.e_SafetyMode, oq_IsSafe) == false)
                  {
                     u32_NumberOfAssignedChannels++;
                  }
               }

               // make some decision based on whether we have channels or a "pure" domain
               if (q_DomainHasChannels == true)
               {
                  // add channel number resp. its define
                  orc_Data.Append("      " + C_OscHalcMagicianUtil::h_GetDatapoolName(oq_IsSafe) + "_NUMBER_OF_" +
                                  pc_Domain->c_Name.UpperCase() + ",");

                  //set actual data to use for values and parameters:
                  pc_ChannelValues = &pc_Domain->c_ChannelValues;
                  c_ChannelConfigs = pc_Domain->c_ChannelConfigs;

                  //domain with channel: depends on number of channels
                  q_IsArray = (u32_NumberOfAssignedChannels > 1U);
               }
               else
               {
                  //set actual data to use for values and parameters:
                  pc_ChannelValues = &pc_Domain->c_DomainValues;
                  c_ChannelConfigs.push_back(pc_Domain->c_DomainConfig); //just one config ...

                  //channelless domain: simple
                  q_IsArray = false;
               }

               // configuration sub-structure
               // To be generated if
               // a) the domain has channels or
               // b) the domain does not have channels but at least one parameter (in any safety)
               // c) the domain does not have channels but <safety_mode> is not set to "two-levels-without-dropping":
               if ((q_DomainHasChannels == true) || (pc_Domain->c_DomainValues.c_Parameters.size() > 0) ||
                   (orc_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eTWO_LEVELS_WITHOUT_DROPPING))
               {
                  orc_Data.Append("      {");
                  q_BracketIsOpen = true;

                  // result should be an array of length channel number resp. no array if less than 2 channels
                  const C_SclString c_Head = "         &gt_" +
                                             C_OscHalcMagicianUtil::h_GetDatapoolName(oq_IsSafe) +
                                             "_DataPoolValues.t_ConfigurationValues." +
                                             ((u32_NumberOfAssignedChannels > 1U) ? "a" : "");
                  const C_SclString c_TailNoComma =
                     ((u32_NumberOfAssignedChannels > 1U) ? "[0]" : static_cast<C_SclString>(""));
                  const C_SclString c_Tail = c_TailNoComma + ",";

                  // add channel number pointer if we have channels and if dropping is active ...
                  if ((q_DomainHasChannels == true) &&
                      (orc_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eTWO_LEVELS_WITH_DROPPING))
                  {
                     //... and at least one channel is assigned to the desired safety:
                     if ((u32_NumberOfAssignedChannels > 0U))
                     {
                        c_Tmp = c_Head + "u16_" + pc_Domain->c_SingularName + "ChannelNumber" + c_Tail;
                        orc_Data.Append(c_Tmp);
                     }
                     else
                     {
                        //no channels assigned to this safety -> no data to reference -> use NULL:
                        orc_Data.Append("         NULL,");
                     }
                  }

                  // add safety relevant pointer if dropping is not active
                  if (orc_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eTWO_LEVELS_WITHOUT_DROPPING)
                  {
                     // pointers always exist and are referencable in this case
                     c_Tmp = c_Head + "u8_" + pc_Domain->c_SingularName + "SafetyRelevant";

                     if ((q_DomainHasChannels == false) && (pc_ChannelValues->c_Parameters.empty() == true))
                     {
                        // no comma because this will remain the only entry
                        c_Tmp += c_TailNoComma;
                     }
                     else
                     {
                        c_Tmp += c_Tail;
                     }
                     orc_Data.Append(c_Tmp);
                  }

                  // add use case pointer if there exists at least one use case ...
                  if (pc_Domain->c_ChannelUseCases.size() > 0U)
                  {
                     //... and at least one channel is assigned to the desired safety:
                     if (u32_NumberOfAssignedChannels > 0U)
                     {
                        c_Tmp = c_Head + "u8_" + pc_Domain->c_SingularName + "UseCase" + c_Tail;
                        orc_Data.Append(c_Tmp);
                     }
                     else
                     {
                        //no channels assigned to this safety -> no data to reference -> use NULL:
                        orc_Data.Append("         NULL,");
                     }
                  }

                  // add references to corresponding elements of parameters/configuration
                  mh_AddDpListElementReferences(orc_Data, pc_ChannelValues->c_Parameters, c_ChannelConfigs,
                                                C_OscHalcDefDomain::eVA_PARAM, pc_Domain->c_SingularName, q_IsArray,
                                                orc_HalcConfig.e_SafetyMode, oq_IsSafe);
               }

               // add references to corresponding elements of list inputs
               if (pc_ChannelValues->c_InputValues.size() > 0U)
               {
                  if (q_BracketIsOpen == true)
                  {
                     orc_Data.Append("      },");
                  }
                  orc_Data.Append("      {");
                  q_BracketIsOpen = true;
                  mh_AddDpListElementReferences(orc_Data, pc_ChannelValues->c_InputValues, c_ChannelConfigs,
                                                C_OscHalcDefDomain::eVA_INPUT, pc_Domain->c_SingularName, q_IsArray,
                                                orc_HalcConfig.e_SafetyMode, oq_IsSafe);
               }

               // add references to corresponding elements of list outputs
               if (pc_ChannelValues->c_OutputValues.size() > 0U)
               {
                  if (q_BracketIsOpen == true)
                  {
                     orc_Data.Append("      },");
                  }
                  orc_Data.Append("      {");
                  q_BracketIsOpen = true;
                  mh_AddDpListElementReferences(orc_Data, pc_ChannelValues->c_OutputValues, c_ChannelConfigs,
                                                C_OscHalcDefDomain::eVA_OUTPUT, pc_Domain->c_SingularName, q_IsArray,
                                                orc_HalcConfig.e_SafetyMode, oq_IsSafe);
               }

               // add references to corresponding elements of list statuses
               if (pc_ChannelValues->c_StatusValues.size() > 0)
               {
                  if (q_BracketIsOpen == true)
                  {
                     orc_Data.Append("      },");
                  }
                  orc_Data.Append("      {");
                  q_BracketIsOpen = true;
                  mh_AddDpListElementReferences(orc_Data, pc_ChannelValues->c_StatusValues, c_ChannelConfigs,
                                                C_OscHalcDefDomain::eVA_STATUS, pc_Domain->c_SingularName, q_IsArray,
                                                orc_HalcConfig.e_SafetyMode, oq_IsSafe);
               }

               if (q_BracketIsOpen == true)
               {
                  orc_Data.Append("      }");
               }

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
C_SclString C_OscExportHalc::mh_GetMagicName(const C_SclString & orc_ProjectId, const bool oq_IsSafe)
{
   const C_SclString c_Prefix = C_OscHalcMagicianUtil::h_GetDatapoolName(oq_IsSafe);

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
   \param[in]      oe_SafetyMode             Safety mode of HALC
   \param[in]      oq_IsSafe                 true: generate code for safe HALC
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportHalc::mh_AddDpListElementReferences(stw::scl::C_SclStringList & orc_Data,
                                                    const std::vector<C_OscHalcDefStruct> & orc_DefinitionArray,
                                                    const std::vector<C_OscHalcConfigChannel> & orc_ConfigArray,
                                                    const C_OscHalcDefDomain::E_VariableSelector & ore_Type,
                                                    const C_SclString & orc_DomainSingularName, const bool oq_IsArray,
                                                    const C_OscHalcDefBase::E_SafetyMode oe_SafetyMode,
                                                    const bool oq_IsSafe)
{
   C_SclString c_Tmp;
   bool q_AtLeastOneChannelIsAssigned = false;

   //We only can/need to add pointers to something useful if at least one of the channels is assigned to this
   // configuration. Check whether this is the case:
   for (uint32_t u32_ItStruct = 0; u32_ItStruct < orc_ConfigArray.size(); u32_ItStruct++)
   {
      if (mh_DropChannel(orc_ConfigArray[u32_ItStruct].q_SafetyRelevant, oe_SafetyMode, oq_IsSafe) == false)
      {
         q_AtLeastOneChannelIsAssigned = true;
         break;
      }
   }

   for (uint32_t u32_ItStruct = 0; u32_ItStruct < orc_DefinitionArray.size(); u32_ItStruct++)
   {
      const C_OscHalcDefStruct & rc_DefStruct = orc_DefinitionArray[u32_ItStruct];
      // structure or POD element ?
      if (rc_DefStruct.c_StructElements.size() > 0)
      {
         // structure -> handle every element
         for (uint32_t u32_ItElement = 0; u32_ItElement < rc_DefStruct.c_StructElements.size();
              u32_ItElement++)
         {
            const C_OscHalcDefElement & rc_Element = rc_DefStruct.c_StructElements[u32_ItElement];

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
            c_Tmp = (((u32_ItElement + 1) < static_cast<uint32_t>(rc_DefStruct.c_StructElements.size())) ||
                     ((u32_ItStruct + 1) < static_cast<uint32_t>(orc_DefinitionArray.size()))) ? (c_Tmp + ",") : c_Tmp;
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
         c_Tmp = ((u32_ItStruct + 1) < static_cast<uint32_t>(orc_DefinitionArray.size())) ? (c_Tmp + ",") : c_Tmp;
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
C_SclString C_OscExportHalc::mh_GetDpListElementReference(const std::vector<C_OscHalcDefStruct> & orc_DefinitionArray,
                                                          const C_OscHalcDefDomain::E_VariableSelector & ore_Type,
                                                          const C_OscHalcDefElement & orc_Element,
                                                          const uint32_t ou32_StructIndex,
                                                          const uint32_t ou32_ElementIndex,
                                                          const C_SclString & orc_DomainSingularName,
                                                          const bool oq_IsArray, const bool oq_IsSafe)
{
   C_SclString c_Return;

   tgl_assert(C_OscHalcMagicianUtil::h_GetVariableName(orc_DefinitionArray, ou32_StructIndex,
                                                       ou32_ElementIndex, orc_DomainSingularName,
                                                       c_Return) == C_NO_ERR);
   c_Return = C_OscExportUti::h_GetElementCeName(c_Return, oq_IsArray, orc_Element.GetType());
   c_Return = "&gt_" + C_OscHalcMagicianUtil::h_GetDatapoolName(oq_IsSafe) + "_DataPoolValues.t_" +
              C_OscHalcMagicianUtil::h_GetListName(ore_Type) + "Values." + c_Return;

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if channel should be dropped depending on its safety state and the over all safety mode

   \param[in]  oq_IsChannelSafe  Safety setting of channel
   \param[in]  oe_SafetyMode     Safety mode of channel
   \param[in]  oq_IsSafeCase     Safe vs. non-safe case

   \retval   true   Drop channel, i.e. channel is not available in current safe case
   \retval   false  Do not drop channel
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscExportHalc::mh_DropChannel(const bool oq_IsChannelSafe, const C_OscHalcDefBase::E_SafetyMode oe_SafetyMode,
                                     const bool oq_IsSafeCase)
{
   bool q_Return = true;

   if ((oe_SafetyMode != (C_OscHalcDefBase::eTWO_LEVELS_WITH_DROPPING)) || (oq_IsChannelSafe == oq_IsSafeCase))
   {
      q_Return = false;
   }

   return q_Return;
}
