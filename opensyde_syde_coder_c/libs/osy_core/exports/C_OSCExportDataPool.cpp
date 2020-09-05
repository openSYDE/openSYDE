//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export Datapool settings of an openSYDE node.

   Create a .c and .h file providing entire Datapool configuration.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>
#include "stwtypes.h"
#include "stwerrors.h"

#include "CSCLStringList.h"
#include "C_OSCExportDataPool.h"
#include "C_OSCNodeDataPoolContent.h"
#include "C_OSCUtils.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCExportUti.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor

   Initialize all elements with default values
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCExportDataPool::C_OSCExportDataPool(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCExportDataPool::~C_OSCExportDataPool(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Return filename (without extension)

   The caller must provide a valid Datapool.

   \param[in]  orc_DataPool            Datapool configuration

   \return assembled filename
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportDataPool::h_GetFileName(const C_OSCNodeDataPool & orc_DataPool)
{
   // assemble filename: Datapool name + 'data_pool'
   return orc_DataPool.c_Name.LowerCase() + "_data_pool";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert overall code structure version to parametrization code subversion.

   The overall code generation version decodes the format versions of parametrization (*_datapool.c/.h files)
   and process data communication code (comm_*.c/.h files).

   \param[in]       ou16_GenCodeVersion     Overall code format version

   \return version of parametrization code
*/
//----------------------------------------------------------------------------------------------------------------------
uint16 C_OSCExportDataPool::h_ConvertOverallCodeVersion(const uint16 ou16_GenCodeVersion)
{
   uint16 u16_Return = ou16_GenCodeVersion;

   switch (ou16_GenCodeVersion)
   {
   case 1:
      u16_Return = ou16_GenCodeVersion;
      break;
   case 2:
   case 3:
   case 4:
      u16_Return = ou16_GenCodeVersion - 1; // -1 because of no changes from version 1 to 2
      break;
   default:
      // should never occur...
      tgl_assert(false);
      break;
   }

   return u16_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create source files

   The caller must provide a valid path and Datapool.

   \param[in] orc_Path                 storage path for created files
   \param[in] orc_DataPool             Datapool configuration
   \param[in] ou16_GenCodeVersion      version of structure (generate code as specified for this version)
   \param[in] oe_ScalingSupport        Flag for data type and existence of factor, offset and scaling macros
   \param[in] ou8_DataPoolIndex        index of Datapool within local process
   \param[in] oe_Linkage               flag for linkage context
   \param[in] ou8_DataPoolIndexRemote  index of Datapool within remote process
   \param[in] ou8_ProcessId            ID of process owning this Datapool
   \param[in] orc_ExportToolInfo       information about calling executable (name + version)

   \return
   C_NO_ERR Operation success
   C_RD_WR  Operation failure: cannot store files
   C_NOACT  Application has unknown code structure version or invalid linkage configuration
   C_CONFIG Datapool list element factor negative or would be generated as zero
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportDataPool::h_CreateSourceCode(const C_SCLString & orc_Path, const uint16 ou16_GenCodeVersion,
                                               const C_OSCNodeCodeExportSettings::E_Scaling oe_ScalingSupport,
                                               const C_OSCNodeDataPool & orc_DataPool, const uint8 ou8_DataPoolIndex,
                                               const E_Linkage oe_Linkage, const uint8 ou8_DataPoolIndexRemote,
                                               const uint8 ou8_ProcessId, const C_SCLString & orc_ExportToolInfo)
{
   sint32 s32_Retval = C_NO_ERR;
   uint32 u32_HashValue = 0U;

   //calculate hash value over the current state of the Datapool definition
   orc_DataPool.CalcHash(u32_HashValue);
   const C_SCLString c_ProjectId = C_SCLString::IntToStr(u32_HashValue);

   // make sure version is known
   if (ou16_GenCodeVersion > C_OSCNodeApplication::hu16_HIGHEST_KNOWN_CODE_VERSION)
   {
      s32_Retval = C_NOACT;
      osc_write_log_error("Creating source code",
                          "Did not generate code for Datapool \"" + orc_DataPool.c_Name +
                          "\" because code format version is unknown.");
   }

   // make sure linkage is not eREMOTEPUBLIC if code format does not support this feature
   if ((s32_Retval == C_NO_ERR) && (ou16_GenCodeVersion < 4U) && (oe_Linkage == eREMOTEPUBLIC))
   {
      s32_Retval = C_NOACT;
      osc_write_log_error("Creating source code",
                          "Did not generate code for Datapool \"" + orc_DataPool.c_Name +
                          "\" because code format version does not support public Datapools.");
   }

   if (s32_Retval == C_NO_ERR)
   {
      // create header file
      s32_Retval = mh_CreateHeaderFile(orc_ExportToolInfo, orc_Path, orc_DataPool, ou8_DataPoolIndex, c_ProjectId,
                                       ou16_GenCodeVersion, oe_Linkage, oe_ScalingSupport);
   }

   // create implementation file
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_CreateImplementationFile(orc_ExportToolInfo, orc_Path, orc_DataPool, c_ProjectId,
                                               ou16_GenCodeVersion, ou8_DataPoolIndexRemote, ou8_ProcessId, oe_Linkage);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create header file

   \param[in] orc_ExportToolInfo       information about calling executable (name + version)
   \param[in] orc_Path                 storage path for created file
   \param[in] orc_DataPool             Datapool configuration
   \param[in] ou8_DataPoolIndex        index of Datapool
   \param[in] orc_ProjectId            project id for consistency check
   \param[in] ou16_GenCodeVersion      version of structure (generate code as specified for this version)
   \param[in] oe_Linkage               flag for linkage context
   \param[in] oe_ScalingSupport        Flag for data type and existence of factor, offset and scaling macros

   \return
   C_NO_ERR Operation success
   C_RD_WR  Cannot store file
   C_CONFIG Datapool list element factor negative or would be generated as zero
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportDataPool::mh_CreateHeaderFile(const C_SCLString & orc_ExportToolInfo, const C_SCLString & orc_Path,
                                                const C_OSCNodeDataPool & orc_DataPool, const uint8 ou8_DataPoolIndex,
                                                const C_SCLString & orc_ProjectId, const uint16 ou16_GenCodeVersion,
                                                const E_Linkage oe_Linkage,
                                                const C_OSCNodeCodeExportSettings::E_Scaling oe_ScalingSupport)
{
   sint32 s32_Retval;
   C_SCLStringList c_Data;

   // add header
   mh_AddHeader(orc_ExportToolInfo, c_Data, orc_DataPool, mhq_IS_HEADER_FILE);

   // add includes
   mh_AddIncludes(c_Data, orc_DataPool, mhq_IS_HEADER_FILE);

   // add defines
   s32_Retval = mh_AddDefinesHeader(c_Data, orc_DataPool, ou8_DataPoolIndex, orc_ProjectId, ou16_GenCodeVersion,
                                    oe_Linkage, oe_ScalingSupport);

   if (s32_Retval == C_NO_ERR)
   {
      // add types
      mh_AddTypes(c_Data, orc_DataPool, mhq_IS_HEADER_FILE, oe_Linkage);

      // add global variables
      mh_AddGlobalVariables(c_Data, orc_DataPool, ou16_GenCodeVersion, mhq_IS_HEADER_FILE, oe_Linkage);

      // add function prototypes
      C_OSCExportUti::h_AddProjIdFunctionPrototype(c_Data, mh_GetMagicName(orc_ProjectId, orc_DataPool));

      // add implementation
      mh_AddImplementation(c_Data, mhq_IS_HEADER_FILE);

      // finally save all stuff into the file
      s32_Retval = C_OSCExportUti::h_SaveToFile(c_Data, orc_Path, h_GetFileName(orc_DataPool), true);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create implementation file

   \param[in] orc_ExportToolInfo       information about calling executable (name + version)
   \param[in] orc_Path                 storage path for created file
   \param[in] orc_DataPool             Datapool configuration
   \param[in] orc_ProjectId            project id for consistency check
   \param[in] ou16_GenCodeVersion      version of structure (generate code as specified for this version)
   \param[in] ou8_DataPoolIndexRemote  index of Datapool within remote process
   \param[in] ou8_ProcessId            ID of process owning this Datapool
   \param[in] oe_Linkage               flag for linkage context


   \return
   C_NO_ERR Operation success
   C_RD_WR  Operation failure: cannot store file
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportDataPool::mh_CreateImplementationFile(const C_SCLString & orc_ExportToolInfo,
                                                        const C_SCLString & orc_Path,
                                                        const C_OSCNodeDataPool & orc_DataPool,
                                                        const C_SCLString & orc_ProjectId,
                                                        const uint16 ou16_GenCodeVersion,
                                                        const uint8 ou8_DataPoolIndexRemote, const uint8 ou8_ProcessId,
                                                        const E_Linkage oe_Linkage)
{
   sint32 s32_Retval;
   C_SCLStringList c_Data;

   // add header
   mh_AddHeader(orc_ExportToolInfo, c_Data, orc_DataPool, mhq_IS_IMPLEMENTATION_FILE);

   // add includes
   mh_AddIncludes(c_Data, orc_DataPool, mhq_IS_IMPLEMENTATION_FILE);

   // add defines
   mh_AddDefinesImpl(c_Data, orc_DataPool, orc_ProjectId, ou16_GenCodeVersion);

   // add types
   mh_AddTypes(c_Data, orc_DataPool, mhq_IS_IMPLEMENTATION_FILE, oe_Linkage);

   // add global variables
   mh_AddGlobalVariables(c_Data, orc_DataPool, ou16_GenCodeVersion, mhq_IS_IMPLEMENTATION_FILE, oe_Linkage);

   // add module global variables
   mh_AddModuleGlobal(c_Data, orc_DataPool, ou16_GenCodeVersion, ou8_ProcessId, ou8_DataPoolIndexRemote, oe_Linkage);

   // add function prototypes
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Module Global Function Prototypes"));
   c_Data.Append("");

   // add implementation
   mh_AddImplementation(c_Data, mhq_IS_IMPLEMENTATION_FILE);

   // finally save all stuff into the file
   s32_Retval = C_OSCExportUti::h_SaveToFile(c_Data, orc_Path, h_GetFileName(orc_DataPool), false);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add file header

   \param[in]  orc_ExportToolInfo       information about calling executable (name + version)
   \param[out] orc_Data                 converted data to string list
   \param[in]  orc_DataPool             Datapool configuration
   \param[in]  oq_FileType              .c or .h file selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportDataPool::mh_AddHeader(const C_SCLString & orc_ExportToolInfo, C_SCLStringList & orc_Data,
                                       const C_OSCNodeDataPool & orc_DataPool, const bool oq_FileType)
{
   orc_Data.Append(C_OSCExportUti::h_GetHeaderSeparator());
   orc_Data.Append("/*!");
   orc_Data.Append("   \\file");
   if (oq_FileType == mhq_IS_IMPLEMENTATION_FILE)
   {
      orc_Data.Append("   \\brief       openSYDE Datapool definition (Source file with constant definitions)");
      if (orc_DataPool.c_Comment.IsEmpty() == false)
      {
         orc_Data.Append("");
         orc_Data.Append("   " + C_OSCUtils::h_NiceifyStringForCComment(orc_DataPool.c_Comment));
      }
      orc_Data.Append("");
      orc_Data.Append(C_OSCExportUti::h_GetCreationToolInfo(orc_ExportToolInfo));
   }
   else
   {
      orc_Data.Append(
         "   \\brief       openSYDE Datapool definition (Header file with constant and global definitions)");
      orc_Data.Append("");
      orc_Data.Append(C_OSCExportUti::h_GetCreationToolInfo(orc_ExportToolInfo));
   }
   orc_Data.Append("*/");
   orc_Data.Append(C_OSCExportUti::h_GetHeaderSeparator());

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      const C_SCLString c_HeaderGuard = h_GetFileName(orc_DataPool).UpperCase() + "H";
      orc_Data.Append("#ifndef " + c_HeaderGuard);
      orc_Data.Append("#define " + c_HeaderGuard);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add includes

   \param[out] orc_Data                 converted data to string list
   \param[in]  orc_DataPool             Datapool configuration
   \param[in]  oq_FileType              .c or .h file selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportDataPool::mh_AddIncludes(C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                         const bool oq_FileType)
{
   orc_Data.Append("");
   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Includes"));

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      orc_Data.Append("#include \"stwtypes.h\"");
      orc_Data.Append("#include \"osy_dpa_data_pool.h\"");
      orc_Data.Append("");
      C_OSCExportUti::h_AddExternCStart(orc_Data);
   }
   else
   {
      orc_Data.Append("#include <stddef.h>");
      orc_Data.Append("#include \"" + h_GetFileName(orc_DataPool) + ".h\"");
      orc_Data.Append("");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add defines for header file

   \param[out] orc_Data                 converted data to string list
   \param[in]  orc_DataPool             Datapool configuration
   \param[in]  ou8_DataPoolIndex        index of Datapool
   \param[in]  orc_ProjectId            project id for consistency check
   \param[in]  ou16_GenCodeVersion      version of structure (generate code as specified for this version)
   \param[in]  oq_FileType              .c or .h file selected
   \param[in]  oe_Linkage               flag for linkage context
   \param[in]  oe_ScalingSupport        Flag for data type and existence of factor, offset and scaling macros

   \return
   C_NO_ERR Operation success
   C_CONFIG Datapool list element factor negative or would be generated as zero
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportDataPool::mh_AddDefinesHeader(C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                                const uint8 ou8_DataPoolIndex, const C_SCLString & orc_ProjectId,
                                                const uint16 ou16_GenCodeVersion, const E_Linkage oe_Linkage,
                                                const C_OSCNodeCodeExportSettings::E_Scaling oe_ScalingSupport)
{
   sint32 s32_Retval = C_NO_ERR;

   const C_SCLString c_DataPoolName = orc_DataPool.c_Name.UpperCase();
   const C_SCLString c_MagicName = mh_GetMagicName(orc_ProjectId, orc_DataPool);
   uint16 u16_ListIndex;

   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Defines"));

   C_OSCExportUti::h_AddProjectIdDef(orc_Data, c_MagicName, true);
   orc_Data.Append("///Index of this Datapool");
   orc_Data.Append("#define " + c_DataPoolName + "_DATA_POOL_INDEX (" + C_SCLString::IntToStr(ou8_DataPoolIndex) +
                   "U)");
   orc_Data.Append("");

   if ((oe_Linkage == eLOCAL) || ((ou16_GenCodeVersion >= 4U) && (oe_Linkage == eREMOTEPUBLIC)))
   {
      orc_Data.Append("///Index of lists");
      for (u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
      {
         const C_SCLString c_ListName = orc_DataPool.c_Lists[u16_ListIndex].c_Name.UpperCase();
         orc_Data.Append("#define " + c_DataPoolName + "_LIST_INDEX_" + c_ListName + " (" +
                         C_SCLString::IntToStr(u16_ListIndex) + "U)");
      }
      orc_Data.Append("#define " + c_DataPoolName + "_NUMBER_OF_LISTS " + "(" + C_SCLString::IntToStr(
                         u16_ListIndex) + "U)");
      orc_Data.Append("");

      orc_Data.Append("///Index of elements");
      for (u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
      {
         const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u16_ListIndex];
         const C_SCLString c_ListName = rc_List.c_Name.UpperCase();
         for (uint16 u16_ElementIndex = 0U; u16_ElementIndex < rc_List.c_Elements.size(); u16_ElementIndex++)
         {
            const C_SCLString c_ElementName = rc_List.c_Elements[u16_ElementIndex].c_Name.UpperCase();
            orc_Data.Append("#define " + c_DataPoolName + "_ELEM_INDEX_" + c_ListName + "_" + c_ElementName +
                            " (" + C_SCLString::IntToStr(u16_ElementIndex) + "U)");
         }
         orc_Data.Append("#define " + c_DataPoolName + "_" + c_ListName + "_NUMBER_OF_ELEMENTS " + "(" +
                         C_SCLString::IntToStr(rc_List.c_Elements.size()) + "U)");
         orc_Data.Append("");
      }

      orc_Data.Append("///Index of Datasets");
      for (u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
      {
         const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u16_ListIndex];
         const C_SCLString c_ListName = rc_List.c_Name.UpperCase();
         if (rc_List.c_Elements.size() > 0)
         {
            for (uint16 u16_DataSetIndex = 0U; u16_DataSetIndex < rc_List.c_DataSets.size(); u16_DataSetIndex++)
            {
               const C_SCLString c_DataSetName = rc_List.c_DataSets[u16_DataSetIndex].c_Name.UpperCase();
               orc_Data.Append("#define " + c_DataPoolName + "_DATA_SET_INDEX_" + c_ListName + "_" + c_DataSetName +
                               " (" + C_SCLString::IntToStr(u16_DataSetIndex) + "U)");
            }
            orc_Data.Append("#define " + c_DataPoolName + "_" + c_ListName + "_NUMBER_OF_DATA_SETS " + "(" +
                            C_SCLString::IntToStr(rc_List.c_DataSets.size()) + "U)");
         }
         else
         {
            orc_Data.Append("#define " + c_DataPoolName + "_" + c_ListName + "_NUMBER_OF_DATA_SETS " + "(0U)");
         }
         orc_Data.Append("");
      }

      if (oe_ScalingSupport != C_OSCNodeCodeExportSettings::eNONE)
      {
         orc_Data.Append("///Scaling values");
         for (u16_ListIndex = 0U; (u16_ListIndex < orc_DataPool.c_Lists.size()) && (s32_Retval == C_NO_ERR);
              u16_ListIndex++)
         {
            const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u16_ListIndex];
            const C_SCLString c_ListName = rc_List.c_Name.UpperCase();
            const C_SCLString c_FloatType = (oe_ScalingSupport == C_OSCNodeCodeExportSettings::eFLOAT64) ? "" : "F";
            for (uint16 u16_ElementIndex = 0U; u16_ElementIndex < rc_List.c_Elements.size(); u16_ElementIndex++)
            {
               const C_SCLString c_ElementName = rc_List.c_Elements[u16_ElementIndex].c_Name.UpperCase();
               const C_SCLString c_Factor =
                  C_OSCExportUti::h_FloatToStrCutZeroes(rc_List.c_Elements[u16_ElementIndex].f64_Factor, true);

               if ((rc_List.c_Elements[u16_ElementIndex].f64_Factor <= 0.0) || (c_Factor == "0.0"))
               {
                  osc_write_log_error("Creating source code",
                                      "Did not generate code because factor of element \"" + orc_DataPool.c_Name +
                                      "::" + rc_List.c_Name + "::" + rc_List.c_Elements[u16_ElementIndex].c_Name +
                                      "\" is negative or would be generated as 0.0.");
                  s32_Retval = C_CONFIG;
               }

               orc_Data.Append("#define " +
                               mh_GetElementScaleDefine(c_DataPoolName, c_ListName, c_ElementName, true) +
                               " (" + c_Factor + c_FloatType + ")");

               orc_Data.Append("#define " +
                               mh_GetElementScaleDefine(c_DataPoolName, c_ListName, c_ElementName, false) + " (" +
                               C_OSCExportUti::h_FloatToStrCutZeroes(rc_List.c_Elements[u16_ElementIndex].f64_Offset) +
                               c_FloatType + ")");
            }
            if (rc_List.c_Elements.size() > 0)
            {
               orc_Data.Append("");
            }
         }

         orc_Data.Append("///Scaling utilities");
         for (u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
         {
            const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u16_ListIndex];
            const C_SCLString c_ListName = rc_List.c_Name.UpperCase();
            for (uint16 u16_ElementIndex = 0U; u16_ElementIndex < rc_List.c_Elements.size(); u16_ElementIndex++)
            {
               const C_OSCNodeDataPoolListElement & rc_Element = rc_List.c_Elements[u16_ElementIndex];
               const C_SCLString c_ElementName = rc_Element.c_Name.UpperCase();
               C_SCLString c_ElementStruct;
               C_SCLString c_Makro;

               if ((ou16_GenCodeVersion >= 4U) && (oe_Linkage == eREMOTEPUBLIC))
               {
                  c_ElementStruct = "gpt_" + orc_DataPool.c_Name + "_DataPoolValues->";
               }
               else
               {
                  c_ElementStruct = "gt_" + orc_DataPool.c_Name + "_DataPoolValues.";
               }

               c_ElementStruct += "t_" + rc_List.c_Name + "Values." +
                                  C_OSCExportUti::h_GetTypePrefix(rc_Element.GetType(), rc_Element.GetArray()) +
                                  "_" + rc_Element.c_Name;

               if (rc_Element.GetArray() == true)
               {
                  c_ElementStruct += "[INDEX]";
               }

               // setter
               c_Makro = "#define " + c_DataPoolName + "_SET_VALUE_FROM_SCALED_" + c_ListName + "_" +
                         c_ElementName + "(";
               if (rc_Element.GetArray() == true)
               {
                  c_Makro += "INDEX, ";
               }
               c_Makro += "SCALED_VALUE) (" + c_ElementStruct + " = (SCALED_VALUE - " +
                          mh_GetElementScaleDefine(c_DataPoolName, c_ListName, c_ElementName, false) +
                          ") / " + mh_GetElementScaleDefine(c_DataPoolName, c_ListName, c_ElementName, true) +
                          ")";

               orc_Data.Append(c_Makro);

               // getter
               c_Makro = "#define " + c_DataPoolName + "_GET_SCALED_VALUE_" + c_ListName + "_" +
                         c_ElementName + "(";

               if (rc_Element.GetArray() == true)
               {
                  c_Makro += "INDEX";
               }
               c_Makro += ") ((" + c_ElementStruct +
                          " * " + mh_GetElementScaleDefine(c_DataPoolName, c_ListName, c_ElementName, true) +
                          ") + " + mh_GetElementScaleDefine(c_DataPoolName, c_ListName, c_ElementName, false) +
                          ")";
               orc_Data.Append(c_Makro);
            }

            if (rc_List.c_Elements.size() > 0)
            {
               orc_Data.Append("");
            }
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief Add defines for implementation file

   \param[out] orc_Data                 converted data to string list
   \param[in]  orc_DataPool             Datapool configuration
   \param[in]  orc_ProjectId            project id for consistency check
   \param[in]  ou16_GenCodeVersion      version of structure (generate code as specified for this version)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportDataPool::mh_AddDefinesImpl(C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                            const C_SCLString & orc_ProjectId, const uint16 ou16_GenCodeVersion)
{
   const C_SCLString c_MagicName = mh_GetMagicName(orc_ProjectId, orc_DataPool);

   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Defines"));

   if (ou16_GenCodeVersion >= 3U)
   {
      orc_Data.Append("///check for correct version of structure definitions");
      orc_Data.Append("#if OSY_DPA_DATA_POOL_DEFINITION_VERSION != 0x000" +
                      C_SCLString::IntToStr(static_cast<sint32>(C_OSCExportDataPool::h_ConvertOverallCodeVersion(
                                                                   ou16_GenCodeVersion))) + "U");
      // datapool export version is one less than over all code structure version (no changes from version 1 to 2)
      orc_Data.Append("///if compilation fails here the openSYDE library version does not match the version of the "
                      "generated code");
      orc_Data.Append("static T_osy_non_existing_type_" + orc_ProjectId + " mt_Variable;");
      orc_Data.Append("#endif");
      orc_Data.Append("");
   }

   C_OSCExportUti::h_AddProjectIdDef(orc_Data, c_MagicName, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add types

   \param[out] orc_Data                 converted data to string list
   \param[in]  orc_DataPool             Datapool configuration
   \param[in]  oq_FileType              .c or .h file selected
   \param[in]  oe_Linkage               flag for linkage context
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportDataPool::mh_AddTypes(C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                      const bool oq_FileType, const E_Linkage oe_Linkage)
{
   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Types"));

   if ((oq_FileType == mhq_IS_HEADER_FILE) &&
       ((oe_Linkage == eLOCAL) || (oe_Linkage == eREMOTEPUBLIC)))
   {
      bool q_AtLeastOneElement = false; // defensive (actually no file generation for empty datapools)

      for (uint16 u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
      {
         const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u16_ListIndex];
         if (rc_List.c_Elements.size() != 0)
         {
            q_AtLeastOneElement = true;
            orc_Data.Append("///Elements in list \"" + rc_List.c_Name + "\":");
            orc_Data.Append("typedef struct");
            orc_Data.Append("{");

            for (uint16 u16_ElementIndex = 0U; u16_ElementIndex < rc_List.c_Elements.size(); u16_ElementIndex++)
            {
               const C_OSCNodeDataPoolListElement & rc_Element = rc_List.c_Elements[u16_ElementIndex];
               const C_SCLString c_TypePrefix =
                  C_OSCExportUti::h_GetTypePrefix(rc_Element.GetType(), rc_Element.GetArray());
               const C_SCLString c_ElementType =  mh_GetType(rc_Element.GetType());
               if (rc_Element.GetArray() == true)
               {
                  const C_SCLString c_String = C_SCLString::IntToStr(rc_Element.GetArraySize());
                  orc_Data.Append("   " + c_ElementType + " " + c_TypePrefix + "_" + rc_Element.c_Name + "[" +
                                  c_String + "]; /* " + C_OSCUtils::h_NiceifyStringForCComment(rc_Element.c_Comment) +
                                  " */");
               }
               else
               {
                  orc_Data.Append("   " + c_ElementType + " " + c_TypePrefix + "_" + rc_Element.c_Name + "; /* " +
                                  C_OSCUtils::h_NiceifyStringForCComment(rc_Element.c_Comment) + " */");
               }
            }
            orc_Data.Append("} T_" + orc_DataPool.c_Name + "_" + rc_List.c_Name + "_Values;");
            orc_Data.Append("");
         }
      }

      if (q_AtLeastOneElement == true)
      {
         orc_Data.Append("///Elements of all lists:");
         orc_Data.Append("typedef struct");
         orc_Data.Append("{");

         for (uint16 u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
         {
            if (orc_DataPool.c_Lists[u16_ListIndex].c_Elements.size() != 0)
            {
               const C_SCLString c_ListName = orc_DataPool.c_Lists[u16_ListIndex].c_Name;
               orc_Data.Append("   T_" + orc_DataPool.c_Name + "_" + c_ListName + "_Values t_" + c_ListName +
                               "Values;");
            }
         }

         orc_Data.Append("} T_" + orc_DataPool.c_Name + "_DataPoolValues;");
      }
   }
   orc_Data.Append("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add global variables

   \param[out] orc_Data                 converted data to string list
   \param[in]  orc_DataPool             Datapool configuration
   \param[in]  ou16_GenCodeVersion      version of structure (generate code as specified for this version)
   \param[in]  oq_FileType              .c or .h file selected
   \param[in]  oe_Linkage               flag for linkage context
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportDataPool::mh_AddGlobalVariables(C_SCLStringList & orc_Data,  const C_OSCNodeDataPool & orc_DataPool,
                                                const uint16 ou16_GenCodeVersion, const bool oq_FileType,
                                                const E_Linkage oe_Linkage)
{
   bool q_AtLeastOneElement = false; // defensive (actually no file generation for empty datapools)

   for (uint16 u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
   {
      if (orc_DataPool.c_Lists[u16_ListIndex].c_Elements.size() != 0)
      {
         q_AtLeastOneElement = true;
         break;
      }
   }

   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Global Variables"));

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      if (q_AtLeastOneElement == true)
      {
         if (oe_Linkage == eLOCAL)
         {
            orc_Data.Append("///Global Datapool RAM variables:");
            orc_Data.Append("extern T_" + orc_DataPool.c_Name + "_DataPoolValues gt_" + orc_DataPool.c_Name +
                            "_DataPoolValues;");
            orc_Data.Append("");
         }

         if (oe_Linkage == eREMOTEPUBLIC)
         {
            orc_Data.Append("///Pointer to global Datapool RAM variables in other application:");
            orc_Data.Append("extern T_" + orc_DataPool.c_Name + "_DataPoolValues * gpt_" + orc_DataPool.c_Name +
                            "_DataPoolValues;");
            orc_Data.Append("");
         }
      }

      orc_Data.Append("///Datapool instance data for API functions:");
      orc_Data.Append("extern const T_osy_dpa_data_pool gt_" + orc_DataPool.c_Name + "_DataPool;");
   }
   else
   {
      if (q_AtLeastOneElement == true)
      {
         if (oe_Linkage == eLOCAL)
         {
            C_SCLString c_SafeRamData;
            if (orc_DataPool.q_IsSafety == true)
            {
               if (ou16_GenCodeVersion >= 3U)
               {
                  c_SafeRamData = "OSY_DPA_SAFE_RAM_DATA_READALL_ZERO ";
               }
               else
               {
                  c_SafeRamData = "OSY_DPA_SAFE_RAM_DATA ";
               }
            }
            else
            {
               if (ou16_GenCodeVersion >= 3U)
               {
                  c_SafeRamData = "OSY_DPA_NONSAFE_RAM_DATA_WRITEALL_ZERO ";
               }
            }
            orc_Data.Append(c_SafeRamData + "T_" + orc_DataPool.c_Name + "_DataPoolValues gt_" + orc_DataPool.c_Name +
                            "_DataPoolValues;");
         }

         if (oe_Linkage == eREMOTEPUBLIC)
         {
            orc_Data.Append("///Pointer to global Datapool RAM variables in other application:");
            orc_Data.Append("OSY_DPA_SAFE_RAM_DATA_PRIVATE_ZERO T_" + orc_DataPool.c_Name + "_DataPoolValues * gpt_" + orc_DataPool.c_Name +
                            "_DataPoolValues = NULL;");
         }
      }
   }
   orc_Data.Append("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add module global variables and function prototypes

   Implementation file only.

   \param[out] orc_Data                 converted data to string list
   \param[in]  orc_DataPool             Datapool configuration
   \param[in]  ou16_GenCodeVersion      version of structure (generate code as specified for this version)
   \param[in]  ou8_ProcessId            ID of process owning this Datapool
   \param[in]  ou8_DataPoolIndexRemote  index of Datapool within remote process (only used for remote Datapools)
   \param[in]  oe_Linkage               flag for linkage context
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportDataPool::mh_AddModuleGlobal(C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                             const uint16 ou16_GenCodeVersion, const uint8 ou8_ProcessId,
                                             const uint8 ou8_DataPoolIndexRemote, const E_Linkage oe_Linkage)
{
   C_SCLString c_DataPoolName = orc_DataPool.c_Name;
   C_SCLString c_String;
   uint32 u32_HashValue = 0U;

   orc_DataPool.CalcDefinitionHash(u32_HashValue);

   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Module Global Variables"));

   // list, element and dataset definitions
   if (oe_Linkage == eLOCAL)
   {
      uint16 u16_ListIndex;
      uint16 u16_ElementIndex;
      C_SCLString c_ListName;

      orc_Data.Append("///Create Datapool definition instance data:");
      orc_Data.Append("OSY_DPA_CREATE_STATIC_DP_DEFINITION_INSTANCE_DATA(mt_DpDefinitionInstanceData)");
      orc_Data.Append("");
      orc_Data.Append("///Min/max values:");

      for (u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
      {
         C_OSCNodeDataPoolList c_List = orc_DataPool.c_Lists[u16_ListIndex];
         if (c_List.c_Elements.size() != 0)
         {
            orc_Data.Append("///Minimum values");
            orc_Data.Append("static const T_" + orc_DataPool.c_Name + "_" + c_List.c_Name + "_Values mt_" +
                            c_List.c_Name + "MinValues =");
            orc_Data.Append("{");
            for (u16_ElementIndex = 0U; u16_ElementIndex < c_List.c_Elements.size(); u16_ElementIndex++)
            {
               const C_OSCNodeDataPoolListElement & rc_Element = c_List.c_Elements[u16_ElementIndex];
               c_String = "   " + mh_GetElementValueString(rc_Element.c_MinValue,
                                                           rc_Element.GetType(), rc_Element.GetArray());
               if (u16_ElementIndex == (c_List.c_Elements.size() - 1U))
               {
                  c_String += " ";
               }
               else
               {
                  c_String += ",";
               }
               c_String += ("   ///< " + rc_Element.c_Name + " (" +
                            C_OSCUtils::h_NiceifyStringForCComment(rc_Element.c_Comment) + ")");
               orc_Data.Append(c_String);
            }
            orc_Data.Append("};");
            orc_Data.Append("");
            orc_Data.Append("///Maximum values");
            orc_Data.Append("static const T_" + orc_DataPool.c_Name + "_" + c_List.c_Name + "_Values mt_" +
                            c_List.c_Name + "MaxValues =");
            orc_Data.Append("{");
            for (u16_ElementIndex = 0U; u16_ElementIndex < c_List.c_Elements.size(); u16_ElementIndex++)
            {
               const C_OSCNodeDataPoolListElement & rc_Element = c_List.c_Elements[u16_ElementIndex];
               c_String = "   " + mh_GetElementValueString(rc_Element.c_MaxValue,
                                                           rc_Element.GetType(), rc_Element.GetArray());
               if (u16_ElementIndex == (c_List.c_Elements.size() - 1U))
               {
                  c_String += " ";
               }
               else
               {
                  c_String += ",";
               }
               c_String += ("   ///< " + rc_Element.c_Name + " (" +
                            C_OSCUtils::h_NiceifyStringForCComment(rc_Element.c_Comment) + ")");
               orc_Data.Append(c_String);
            }
            orc_Data.Append("};");
            orc_Data.Append("");

            if (c_List.c_DataSets.size() > 0)
            {
               uint8 u8_DataSetIndex;
               orc_Data.Append("///Dataset values");
               c_ListName = c_List.c_Name;
               orc_Data.Append("static const T_" + c_DataPoolName + "_" + c_List.c_Name + "_Values mat_" + c_List.c_Name +
                               "DataSetValues [" + c_DataPoolName.UpperCase() + "_" + c_ListName.UpperCase() +
                               "_NUMBER_OF_DATA_SETS] =");
               orc_Data.Add("{");

               for (u8_DataSetIndex = 0U; u8_DataSetIndex < c_List.c_DataSets.size(); u8_DataSetIndex++)
               {
                  orc_Data.Append("   {");
                  for (u16_ElementIndex = 0U; u16_ElementIndex < c_List.c_Elements.size(); u16_ElementIndex++)
                  {
                     C_OSCNodeDataPoolListElement c_Element = c_List.c_Elements[u16_ElementIndex];
                     c_String = "      ";
                     c_String += mh_GetElementValueString(c_Element.c_DataSetValues[u8_DataSetIndex],
                                                          c_Element.GetType(),
                                                          c_Element.GetArray());
                     if (u16_ElementIndex == (c_List.c_Elements.size() - 1U))
                     {
                        c_String += " ";
                     }
                     else
                     {
                        c_String += ",";
                     }
                     c_String += ("   ///< " + c_Element.c_Name + " (" +
                                  C_OSCUtils::h_NiceifyStringForCComment(c_Element.c_Comment) + ")");
                     orc_Data.Append(c_String);
                  }
                  if (u8_DataSetIndex == (c_List.c_DataSets.size() - 1U))
                  {
                     orc_Data.Append("   }");
                  }
                  else
                  {
                     orc_Data.Append("   },");
                  }
               }
               orc_Data.Append("};");
               orc_Data.Append("");

               orc_Data.Append("///Dataset table:");
               c_ListName = c_List.c_Name;
               orc_Data.Append("static const T_osy_dpa_data_set mat_" + c_List.c_Name + "DataSetTable[" +
                               c_DataPoolName.UpperCase() + "_" +  c_ListName.UpperCase() +
                               "_NUMBER_OF_DATA_SETS] =");
               orc_Data.Add("{");

               for (u8_DataSetIndex = 0U; u8_DataSetIndex < c_List.c_DataSets.size(); u8_DataSetIndex++)
               {
                  c_String = "   { &mat_" + c_List.c_Name + "DataSetValues[" + C_SCLString::IntToStr(u8_DataSetIndex) +
                             "] }";

                  if (u8_DataSetIndex != (c_List.c_DataSets.size() - 1U))
                  {
                     c_String += ",";
                  }
                  orc_Data.Append(c_String);
               }
               orc_Data.Append("};");
               orc_Data.Append("");
            }
         }
      }

      orc_Data.Append("///List definitions:");
      for (u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
      {
         const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u16_ListIndex];
         if (rc_List.c_Elements.size() != 0)
         {
            c_ListName = rc_List.c_Name;
            orc_Data.Append("static const T_osy_dpa_element_definition mat_DataPool" + rc_List.c_Name + "Elements[" +
                            c_DataPoolName.UpperCase() + "_" + c_ListName.UpperCase() + "_NUMBER_OF_ELEMENTS] =");
            orc_Data.Append("{");

            for (u16_ElementIndex = 0U; u16_ElementIndex < rc_List.c_Elements.size(); u16_ElementIndex++)
            {
               const C_OSCNodeDataPoolListElement & rc_Element = rc_List.c_Elements[u16_ElementIndex];
               const C_SCLString c_ElementSize = mh_GetElementSize(rc_Element.GetType(), rc_Element.GetArraySize(),
                                                                   rc_Element.GetArray());
               const C_SCLString c_TriggerEvent = (rc_Element.q_DiagEventCall == true) ? "1U" : "0U";
               const C_SCLString c_ElementName = C_OSCExportUti::h_GetElementCName(rc_Element.c_Name,
                                                                                   rc_Element.GetArray(),
                                                                                   rc_Element.GetType());
               C_SCLString c_ElementType = mh_GetType(rc_Element.GetType());
               if (rc_Element.GetArray() == true)
               {
                  c_ElementType = "a" + c_ElementType;
               }
               c_String = "   { OSY_DPA_ELEMENT_TYPE_" + c_ElementType.UpperCase() + ", " + c_TriggerEvent + ", " +
                          c_ElementSize + "U, &gt_" + c_DataPoolName + "_DataPoolValues.t_" + c_ListName + "Values." +
                          c_ElementName +
                          ", &mt_" + c_ListName + "MinValues." + c_ElementName +
                          ", &mt_" + c_ListName + "MaxValues." + c_ElementName + " }";
               if (u16_ElementIndex != (rc_List.c_Elements.size() - 1U))
               {
                  c_String += ",";
               }

               orc_Data.Append(c_String);
            }
            orc_Data.Append("};");
            orc_Data.Append("");
         }
      }

      orc_Data.Append("///list of lists:");
      orc_Data.Append("static const T_osy_dpa_list_definition mat_DataPoolLists[" + c_DataPoolName.UpperCase() +
                      "_NUMBER_OF_LISTS] =");
      orc_Data.Append("{");

      for (u16_ListIndex = 0U; u16_ListIndex < orc_DataPool.c_Lists.size(); u16_ListIndex++)
      {
         const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u16_ListIndex];
         c_ListName = rc_List.c_Name;
         c_String = "   { ";
         c_String += c_DataPoolName.UpperCase() + "_" + c_ListName.UpperCase() + "_NUMBER_OF_ELEMENTS, ";
         c_String += c_DataPoolName.UpperCase() + "_" + c_ListName.UpperCase() + "_NUMBER_OF_DATA_SETS, ";

         if (rc_List.c_Elements.size() == 0)
         {
            //empty list with no elements
            //no CRC, no address, no size, pointers all NULL
            c_String += "0U, 0U, 0U, NULL, NULL, NULL";
         }
         else
         {
            if (orc_DataPool.e_Type == C_OSCNodeDataPool::eNVM)
            {
               c_String += C_SCLString::IntToStr(rc_List.q_NvMCRCActive) + "U, ";
               c_String += "0x" + C_SCLString::IntToHex(static_cast<sint64>(rc_List.u32_NvMStartAddress), 8U) + "U, ";
            }
            else
            {
               c_String += "0U, 0x00000000U, "; //no CRC and NVM start address
            }
            c_String += "sizeof(T_" + c_DataPoolName + "_" + c_ListName + "_Values), ";
            c_String += "&mat_DataPool" + c_ListName + "Elements[0], ";
            c_String += "&gt_" + c_DataPoolName + "_DataPoolValues.t_" + c_ListName + "Values, ";
            if (rc_List.c_DataSets.size() > 0U)
            {
               c_String += "&mat_" + c_ListName + "DataSetTable[0]";
            }
            else
            {
               c_String += "NULL";
            }
         }
         if (u16_ListIndex == (orc_DataPool.c_Lists.size() - 1U))
         {
            c_String += " }";
         }
         else
         {
            c_String += " },";
         }
         orc_Data.Append(c_String);
      }
      orc_Data.Append("};");
      orc_Data.Append("");

      orc_Data.Append("///Datapool definition:");
      orc_Data.Append("static const T_osy_dpa_data_pool_definition mt_DataPoolDefinition =");
      orc_Data.Append("{");
      orc_Data.Append("   OSY_DPA_DATA_POOL_DEFINITION_VERSION,");
      if (orc_DataPool.e_Type == C_OSCNodeDataPool::eNVM)
      {
         orc_Data.Append("   OSY_DPA_DATA_POOL_TYPE_NVM,");
      }
      else if (orc_DataPool.e_Type == C_OSCNodeDataPool::eCOM)
      {
         orc_Data.Append("   OSY_DPA_DATA_POOL_TYPE_COMMUNICATION,");
      }
      else
      {
         orc_Data.Append("   OSY_DPA_DATA_POOL_TYPE_DIAGNOSIS,");
      }
      orc_Data.Append("   { 0x" + C_SCLString::IntToHex(orc_DataPool.au8_Version[0], 2U) + "U, 0x" +
                      C_SCLString::IntToHex(orc_DataPool.au8_Version[1], 2U) + "U, 0x" +
                      C_SCLString::IntToHex(orc_DataPool.au8_Version[2], 2U) + "U }," +
                      " ///< Datapool definition version V" +
                      C_SCLString::IntToStr(orc_DataPool.au8_Version[0]) + "." +
                      C_SCLString::IntToStr(orc_DataPool.au8_Version[1]) + "r" +
                      C_SCLString::IntToStr(orc_DataPool.au8_Version[2]));
      orc_Data.Append("   \"" + orc_DataPool.c_Name + "\",  ///< name of Datapool");
      orc_Data.Append("   " + c_DataPoolName.UpperCase() + "_NUMBER_OF_LISTS,");
      orc_Data.Append("   0x" + C_SCLString::IntToHex(static_cast<sint64>(u32_HashValue), 4U) +
                      "U, ///< CRC of Datapool definition");
      if (orc_DataPool.e_Type == C_OSCNodeDataPool::eNVM)
      {
         orc_Data.Append("   0x" + C_SCLString::IntToHex(static_cast<sint64>(orc_DataPool.u32_NvMStartAddress), 8U) +
                         "U,  ///< NVM start address");
         orc_Data.Append("   " + C_SCLString::IntToStr(orc_DataPool.u32_NvMSize) +
                         "U,  ///< number of bytes occupied in NVM");
      }
      else
      {
         orc_Data.Append("   0x00000000U,  ///< NVM start address");
         orc_Data.Append("   0U,  ///< number of bytes occupied in NVM");
      }
      orc_Data.Append("   &mat_DataPoolLists[0],");
      orc_Data.Append("   &mt_DpDefinitionInstanceData");
      orc_Data.Append("};");
      orc_Data.Append("");
   }

   // Remote/public structure
   if (ou16_GenCodeVersion < 4U)
   {
      if (oe_Linkage == eLOCAL)
      {
         orc_Data.Append("///Information about process and Datapool:");
         orc_Data.Append("static const T_osy_dpa_remote_data_pool_info mt_DataPoolInfo =");
         orc_Data.Append("{");
         orc_Data.Append("   " + C_SCLString::IntToStr(ou8_ProcessId) +
                         "U,  ///< Identification ID of our own process");
         orc_Data.Append("   " + c_DataPoolName.UpperCase() + "_DATA_POOL_INDEX"
                         "  ///< Index of Datapool within process identified by ProcessId");
         orc_Data.Append("};");
         orc_Data.Append("");
      }
      else
      {
         c_String = "///Information about remote Datapool. ";
         c_String += "Can be used to request information about remote Datapool from another process:";
         orc_Data.Append(c_String);
         orc_Data.Append("static const T_osy_dpa_remote_data_pool_info mt_RemoteDataPoolInfo =");
         orc_Data.Append("{");
         orc_Data.Append("   " + C_SCLString::IntToStr(ou8_ProcessId) +
                         "U, ///< Identification ID of remote server providing information about the Datapool");
         orc_Data.Append("   " + C_SCLString::IntToStr(ou8_DataPoolIndexRemote) +
                         "U  ///< Index of Datapool within process identified by ProcessId");
         orc_Data.Append("};");
         orc_Data.Append("");
      }
   }
   else
   {
      if (oe_Linkage != eLOCAL) // structure was removed for local Datapools since version 4
      {
         orc_Data.Append("///Information about process and Datapool:");
         orc_Data.Append("static const T_osy_dpa_remote_data_pool_info mt_RemoteDataPoolInfo =");
         orc_Data.Append("{");
         orc_Data.Append("   " + C_SCLString::IntToStr(ou8_ProcessId) +
                         "U, ///< Identification ID of remote server providing information about the Datapool");
         switch (oe_Linkage)
         {
         case eLOCAL:
            // never landing here
            break;
         case eREMOTE:
            orc_Data.Append("   NULL,  ///< Pointer only relevant for Datapool with public scope");
            break;
         case eREMOTEPUBLIC:
            orc_Data.Append("   (void**)&gpt_" + orc_DataPool.c_Name +
                            "_DataPoolValues,  ///< Pointer to pointer to struct containing all Datapool values");
            break;
         }
         orc_Data.Append("   0x" + C_SCLString::IntToHex(static_cast<sint64>(u32_HashValue), 4U) +
                         "U, ///< CRC of Datapool definition");
         orc_Data.Append("   \"" + orc_DataPool.c_Name + "\"  ///< name of Datapool");
         orc_Data.Append("};");
         orc_Data.Append("");
      }
   }

   orc_Data.Append("///Create Datapool instance data:");
   c_String = "OSY_DPA_CREATE_STATIC_DP_INSTANCE_DATA(mt_DpInstanceData, ";
   switch (oe_Linkage)
   {
   case eLOCAL:
      c_String += "&mt_DataPoolDefinition)";
      break;
   case eREMOTE:
   case eREMOTEPUBLIC:
      c_String += "NULL)";
      break;
   }

   orc_Data.Append(c_String);
   orc_Data.Append("");

   orc_Data.Append("const T_osy_dpa_data_pool gt_" + c_DataPoolName + "_DataPool =");
   orc_Data.Append("{");
   orc_Data.Append("   OSY_DPA_DATA_POOL_MAGIC,  ///< identification of valid DP definition");
   orc_Data.Append("   " + c_DataPoolName.UpperCase() +
                   "_DATA_POOL_INDEX,  ///< Datapool index within this process");

   c_String = "   ";
   if (ou16_GenCodeVersion >= 4U)
   {
      c_String += "OSY_DPA_DATA_POOL_LINKAGE_" + C_OSCExportDataPool::mh_ConvertLinkageToString(oe_Linkage) +
                  ",  ///< Datapool linkage";
   }
   else
   {
      if (oe_Linkage == eLOCAL)
      {
         c_String += "0U,  ///< it's local ...";
      }
      else
      {
         c_String += "1U,  ///< it's remote ...";
         // eREMOTEPUBLIC is not supported in versions >=4U
      }
   }
   orc_Data.Append(c_String);

   switch (oe_Linkage)
   {
   case eLOCAL:
      if (ou16_GenCodeVersion >= 4U)
      {
         orc_Data.Append("   NULL,  ///<  info about remote Datapool");
      }
      else
      {
         orc_Data.Append("   &mt_DataPoolInfo,  ///<  info about local Datapool");
      }
      break;
   case eREMOTE:
   case eREMOTEPUBLIC:
      orc_Data.Append("   &mt_RemoteDataPoolInfo,  ///< info about remote Datapool");
      break;
   }

   orc_Data.Append("   &mt_DpInstanceData  ///< run-time data");
   orc_Data.Append("};");
   orc_Data.Append("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add implementation

   \param[out] orc_Data                 converted data to string list
   \param[in]  oq_FileType              .c or .h file selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportDataPool::mh_AddImplementation(C_SCLStringList & orc_Data, const bool oq_FileType)
{
   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Implementation"));

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      C_OSCExportUti::h_AddExternCEnd(orc_Data);
      orc_Data.Append("#endif");
   }
   else
   {
      orc_Data.Append("");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type string according to data type

   \param[in] oe_Type            data type (uint8, sint8, ...)

   \return
   data type as string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportDataPool::mh_GetType(const C_OSCNodeDataPoolContent::E_Type oe_Type)
{
   C_SCLString c_Type;

   switch (oe_Type)
   {
   case C_OSCNodeDataPoolContent::eUINT8: // Data type unsigned 8 bit integer
      c_Type += "uint8";
      break;
   case C_OSCNodeDataPoolContent::eUINT16: // Data type unsigned 16 bit integer
      c_Type += "uint16";
      break;
   case C_OSCNodeDataPoolContent::eUINT32: // Data type unsigned 32 bit integer
      c_Type += "uint32";
      break;
   case C_OSCNodeDataPoolContent::eUINT64: // Data type unsigned 64 bit integer
      c_Type += "uint64";
      break;
   case C_OSCNodeDataPoolContent::eSINT8: // Data type signed 8 bit integer
      c_Type += "sint8";
      break;
   case C_OSCNodeDataPoolContent::eSINT16: // Data type signed 16 bit integer
      c_Type += "sint16";
      break;
   case C_OSCNodeDataPoolContent::eSINT32: // Data type signed 32 bit integer
      c_Type += "sint32";
      break;
   case C_OSCNodeDataPoolContent::eSINT64: // Data type signed 64 bit integer
      c_Type += "sint64";
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32: // Data type 32 bit floating point
      c_Type += "float32";
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64: // Data type 64 bit floating point
      c_Type += "float64";
      break;
   default:
      break;
   }

   return c_Type;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get value string from list element

   \param[in] orc_Value                value to be converted
   \param[in] oe_Type                  type of element
   \param[in] oq_IsArray               indicator for array type

   \return
   element with unsigned indicator as string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportDataPool::mh_GetElementValueString(const C_OSCNodeDataPoolContent & orc_Value,
                                                          const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                          const bool oq_IsArray)
{
   C_SCLString c_String;
   sint64 s64_Value;
   sint32 s32_Value;

   if (oq_IsArray == false)
   {
      switch (oe_Type)
      {
      case C_OSCNodeDataPoolContent::eUINT8: ///< Data type unsigned 8 bit integer
         c_String += C_SCLString::IntToStr(orc_Value.GetValueU8()) + "U";
         break;
      case C_OSCNodeDataPoolContent::eUINT16: ///< Data type unsigned 16 bit integer
         c_String += C_SCLString::IntToStr(orc_Value.GetValueU16()) + "U";
         break;
      case C_OSCNodeDataPoolContent::eUINT32: ///< Data type unsigned 32 bit integer
         c_String += C_SCLString::IntToStr(orc_Value.GetValueU32()) + "UL";
         break;
      case C_OSCNodeDataPoolContent::eUINT64: ///< Data type unsigned 64 bit integer
         c_String += C_SCLString::IntToStr(orc_Value.GetValueU64()) + "ULL";
         break;
      case C_OSCNodeDataPoolContent::eSINT8: ///< Data type signed 8 bit integer
         c_String += C_SCLString::IntToStr(orc_Value.GetValueS8());
         break;
      case C_OSCNodeDataPoolContent::eSINT16: ///< Data type signed 16 bit integer
         c_String += C_SCLString::IntToStr(orc_Value.GetValueS16());
         break;
      case C_OSCNodeDataPoolContent::eSINT32: ///< Data type signed 32 bit integer
         s32_Value = orc_Value.GetValueS32();
         if (s32_Value == std::numeric_limits<sint32>::min())
         {
            //workaround for GCC warning when reaching lowest possible value
            c_String += C_SCLString::IntToStr(std::numeric_limits<sint32>::min() + 1) + "L - 1";
         }
         else
         {
            c_String += C_SCLString::IntToStr(orc_Value.GetValueS32()) + "L";
         }
         break;
      case C_OSCNodeDataPoolContent::eSINT64: ///< Data type signed 64 bit integer
         s64_Value = orc_Value.GetValueS64();
         if (s64_Value == std::numeric_limits<sint64>::min())
         {
            //workaround for GCC warning when reaching lowest possible value
            c_String += C_SCLString::IntToStr(std::numeric_limits<sint64>::min() + 1) + "LL - 1";
         }
         else
         {
            c_String += C_SCLString::IntToStr(orc_Value.GetValueS64()) + "LL";
         }
         break;
      case C_OSCNodeDataPoolContent::eFLOAT32: ///< Data type 32 bit floating point
         c_String += C_OSCExportUti::h_FloatToStrCutZeroes(orc_Value.GetValueF32()) + "F";
         break;
      case C_OSCNodeDataPoolContent::eFLOAT64: ///< Data type 64 bit floating point
         //depending on the value this could result in a very long string
         //but with other approaches (e.g. "printf formatter %g" we might lose precision)
         c_String += C_OSCExportUti::h_FloatToStrCutZeroes(orc_Value.GetValueF64());
         break;
      default:
         break;
      }
   }
   else
   {
      const uint32 u32_ArraySize = orc_Value.GetArraySize();
      c_String = "{ ";
      for (uint32 u32_ArrayIndex = 0U; u32_ArrayIndex < u32_ArraySize; u32_ArrayIndex++)
      {
         switch (oe_Type)
         {
         case C_OSCNodeDataPoolContent::eUINT8: ///< Data type unsigned 8 bit integer
            c_String += C_SCLString::IntToStr(orc_Value.GetValueAU8Element(u32_ArrayIndex)) + "U";
            break;
         case C_OSCNodeDataPoolContent::eUINT16: ///< Data type unsigned 16 bit integer
            c_String += C_SCLString::IntToStr(orc_Value.GetValueAU16Element(u32_ArrayIndex)) + "U";
            break;
         case C_OSCNodeDataPoolContent::eUINT32: ///< Data type unsigned 32 bit integer
            c_String += C_SCLString::IntToStr(orc_Value.GetValueAU32Element(u32_ArrayIndex)) + "UL";
            break;
         case C_OSCNodeDataPoolContent::eUINT64: ///< Data type unsigned 64 bit integer
            c_String += C_SCLString::IntToStr(orc_Value.GetValueAU64Element(u32_ArrayIndex)) + "ULL";
            break;
         case C_OSCNodeDataPoolContent::eSINT8: ///< Data type signed 8 bit integer
            c_String += C_SCLString::IntToStr(orc_Value.GetValueAS8Element(u32_ArrayIndex));
            break;
         case C_OSCNodeDataPoolContent::eSINT16: ///< Data type signed 16 bit integer
            c_String += C_SCLString::IntToStr(orc_Value.GetValueAS16Element(u32_ArrayIndex));
            break;
         case C_OSCNodeDataPoolContent::eSINT32: ///< Data type signed 32 bit integer
            s32_Value = orc_Value.GetValueAS32Element(u32_ArrayIndex);
            if (s32_Value == std::numeric_limits<sint32>::min())
            {
               //workaround for GCC warning when reaching lowest possible value
               c_String += C_SCLString::IntToStr(std::numeric_limits<sint32>::min() + 1) + "L - 1";
            }
            else
            {
               c_String += C_SCLString::IntToStr(orc_Value.GetValueAS32Element(u32_ArrayIndex)) + "L";
            }
            break;
         case C_OSCNodeDataPoolContent::eSINT64: ///< Data type signed 64 bit integer
            s64_Value = orc_Value.GetValueAS64Element(u32_ArrayIndex);
            if (s64_Value == std::numeric_limits<sint64>::min())
            {
               //workaround for GCC warning when reaching lowest possible value
               c_String += C_SCLString::IntToStr(std::numeric_limits<sint64>::min() + 1) + "LL - 1";
            }
            else
            {
               c_String += C_SCLString::IntToStr(orc_Value.GetValueAS64Element(u32_ArrayIndex)) + "LL";
            }
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32: ///< Data type 32 bit floating point
            c_String += C_OSCExportUti::h_FloatToStrCutZeroes(orc_Value.GetValueAF32Element(u32_ArrayIndex)) + "F";
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64: ///< Data type 64 bit floating point
            c_String += C_OSCExportUti::h_FloatToStrCutZeroes(orc_Value.GetValueAF64Element(u32_ArrayIndex));
            break;
         default:
            break;
         }

         if (u32_ArrayIndex == (u32_ArraySize - 1U))
         {
            c_String += " }";
         }
         else
         {
            c_String += ", ";
         }
      }
   }

   return c_String;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get element size according to data type

   \param[in] oe_Type                 data type (uint8, sint8, ...)
   \param[in] ou32_ArraySize          size of array (if ou8_DataType means an array)
   \param[in] oq_IsArray              data type is an array

   \return
   element size as string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportDataPool::mh_GetElementSize(const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                   const uint32 ou32_ArraySize, const bool oq_IsArray)
{
   C_SCLString c_Size;
   uint32 u32_Size = 0U;

   switch (oe_Type)
   {
   case C_OSCNodeDataPoolContent::eUINT8: // Data type unsigned 8 bit integer
      u32_Size = 1U;
      break;
   case C_OSCNodeDataPoolContent::eUINT16: // Data type unsigned 16 bit integer
      u32_Size = 2U;
      break;
   case C_OSCNodeDataPoolContent::eUINT32: // Data type unsigned 32 bit integer
      u32_Size = 4U;
      break;
   case C_OSCNodeDataPoolContent::eUINT64: // Data type unsigned 64 bit integer
      u32_Size = 8U;
      break;
   case C_OSCNodeDataPoolContent::eSINT8: // Data type signed 8 bit integer
      u32_Size = 1U;
      break;
   case C_OSCNodeDataPoolContent::eSINT16: // Data type signed 16 bit integer
      u32_Size = 2U;
      break;
   case C_OSCNodeDataPoolContent::eSINT32: // Data type signed 32 bit integer
      u32_Size = 4U;
      break;
   case C_OSCNodeDataPoolContent::eSINT64: // Data type signed 64 bit integer
      u32_Size = 8U;
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32: // Data type 32 bit floating point
      u32_Size = 4U;
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64: // Data type 64 bit floating point
      u32_Size = 8U;
      break;
   default:
      break;
   }

   if (oq_IsArray == true)
   {
      u32_Size *= ou32_ArraySize;
   }

   c_Size = C_SCLString::IntToStr(u32_Size);

   return c_Size;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert linkage enum to string.

   \param[in]       oe_Linkage  Linkage type

   \return
   Linkage type as upper case string.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportDataPool::mh_ConvertLinkageToString(const C_OSCExportDataPool::E_Linkage oe_Linkage)
{
   C_SCLString c_Return;

   switch (oe_Linkage)
   {
   case eLOCAL:
      c_Return = "LOCAL";
      break;
   case eREMOTE:
      c_Return = "REMOTE";
      break;
   case eREMOTEPUBLIC:
      c_Return = "REMOTE_PUBLIC";
      break;
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get magic name for consistency checks.

   \param[out] orc_Data                 converted data to string list
   \param[in]  orc_DataPool             Datapool configuration
   \param[in]  orc_ProjectId            project id for consistency check

   \return
   Magic name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportDataPool::mh_GetMagicName(const C_SCLString & orc_ProjectId,
                                                 const C_OSCNodeDataPool & orc_DataPool)
{
   const C_SCLString c_MagicName = orc_DataPool.c_Name.UpperCase() + "_PROJECT_ID_" + orc_ProjectId;

   return c_MagicName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility for getting define string of factor value constant.

   \param[in]       orc_Element     Datapool element

   \return
   scale value string, e.g. MY_NVM_SCALING_FACTOR_LIST1_U8VAR
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportDataPool::mh_GetElementScaleDefine(const C_SCLString & orc_DataPoolName,
                                                          const C_SCLString & orc_ListName,
                                                          const C_SCLString & orc_ElementName, const bool oq_Factor)
{
   const C_SCLString c_Type = (oq_Factor == true) ? "FACTOR" : "OFFSET";
   const C_SCLString c_Return = orc_DataPoolName + "_SCALING_" + c_Type + "_" + orc_ListName + "_" +
                                orc_ElementName;

   return c_Return;
}
