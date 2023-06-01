//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to handle imports of KEFEX RAMView .def style projects

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cctype>
#include "TglFile.hpp"
#include "stwerrors.hpp"
#include "C_OscImportRamView.hpp"
#include "CKFXDEFProject.hpp"
#include "CKFXDATFile.hpp"
#include "CKFXProjectOptions.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::diag_lib;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load RAMView .def project and import lists to openSYDE Data Pool

   Load information from RAMView .def project and associated files:
   * Data Pool list and variable definitions
   * Basic project options

   The caller can select which types of RAMView lists to import using orc_ProjectPath::e_Type:
   * eDIAG will result in all RAMView "RAM" lists to be imported
   * eNVM  will result in all RAMView "EEPROM" lists to be imported

   On importing the function will fill the following elements of orc_DataPool:
   * c_Lists: will contain the actual list and element definition grabbed from the RAMView project
   * c_Name: will contain the RAMView project name
   * c_Comment: will contain the RAMView project comment
   * au8_Version: will contain the RAMView project "data version"
   * u32_NvmSize: will be set to the number of bytes occupied by the sum of all imported NVM elements (only for "eNVM")

   Other elements will not be touched.

   Error reporting:
   When loading the RAMView project we assume we have correct project files that load ok in RAMView.
   Details about errors when loading the project will be logged using the OSC logger.

   Reporting interface:
   One String list with zero to many information string which we can use to inform the user on compromises made that
   were needed as RAMView and openSYDE data structures are not 100% compatible.

   \param[in]     orc_ProjectPath        Full path to .def file
   \param[in,out] orc_DataPool           Loaded Data Pool definition
   \param[out]    orc_ImportInformation  Textual information about issues when importing

   \return
   C_NO_ERR     information read
   C_RANGE      orc_ProjectPath::e_Type is not eNMV or eDIAG
   C_RD_WR      critical problem loading project information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscImportRamView::h_ImportDataPoolFromRamViewDefProject(const C_SclString & orc_ProjectPath,
                                                                  C_OscNodeDataPool & orc_DataPool,
                                                                  stw::scl::C_SclStringList & orc_ImportInformation)
{
   stw::diag_lib::C_KFXProjectOptions c_ProjectOptions;
   stw::diag_lib::C_KFXVariableLists c_VariableLists;
   int32_t s32_Result = C_NO_ERR;

   orc_ImportInformation.Clear();

   if ((orc_DataPool.e_Type != C_OscNodeDataPool::eDIAG) && (orc_DataPool.e_Type != C_OscNodeDataPool::eNVM))
   {
      s32_Result = C_RANGE;
   }

   if (s32_Result == C_NO_ERR)
   {
      s32_Result = mh_LoadRamViewDefProject(orc_ProjectPath, c_ProjectOptions, c_VariableLists);
   }

   if (s32_Result == C_NO_ERR)
   {
      //Add information to log regarding NVM addresses
      if (orc_DataPool.e_Type == C_OscNodeDataPool::eNVM)
      {
         const C_SclString c_Info = "When importing RAMView EEPROM lists the absolute addresses after import will not "
                                    "match the addresses of the RAMView project. "
                                    "If gaps are intended to be kept between individual lists those should be "
                                    "added manually after the import.";
         orc_ImportInformation.Add(c_Info);
      }

      //set basic Datapool information:
      orc_DataPool.c_Name = c_ProjectOptions.c_DeviceName;
      for (uint32_t u32_Line = 0U; u32_Line < c_ProjectOptions.c_MetaInfo.c_Text.GetCount(); u32_Line++)
      {
         orc_DataPool.c_Comment += c_ProjectOptions.c_MetaInfo.c_Text.Strings[u32_Line];
         if (u32_Line != (c_ProjectOptions.c_MetaInfo.c_Text.GetCount() - 1))
         {
            orc_DataPool.c_Comment += "\n";
         }
      }
      //make sure the name is openSYDE compliant:
      C_OscImportRamView::mh_AdaptName(orc_DataPool.c_Name, orc_DataPool.c_Comment, "Datapool", orc_ImportInformation);

      orc_DataPool.au8_Version[0] = static_cast<uint8_t>((c_ProjectOptions.u16_DataVersion & 0xF000U) >> 12U);
      orc_DataPool.au8_Version[1] = static_cast<uint8_t>((c_ProjectOptions.u16_DataVersion & 0x0FF0U) >> 4U);
      orc_DataPool.au8_Version[2] = static_cast<uint8_t>(c_ProjectOptions.u16_DataVersion & 0x000FU);

      //copy over lists contents:
      orc_DataPool.c_Lists.clear();

      for (uint16_t u16_ListRamView = 0U; u16_ListRamView < c_VariableLists.GetLength(); u16_ListRamView++)
      {
         //check for correct list type:
         if (((orc_DataPool.e_Type == C_OscNodeDataPool::eNVM) &&
              (c_VariableLists[u16_ListRamView].u8_ListType == KFX_VARIABLE_TYPE_EEPROM)) ||
             ((orc_DataPool.e_Type == C_OscNodeDataPool::eDIAG) &&
              (c_VariableLists[u16_ListRamView].u8_ListType == KFX_VARIABLE_TYPE_RAM)))
         {
            //this is a list we are interested in:
            C_OscNodeDataPoolList c_List;

            c_List.c_Name = c_VariableLists[u16_ListRamView].c_ListName;
            c_List.c_Comment = ""; //comments not available in RAMView project
            //make sure the name is openSYDE compliant:
            C_OscImportRamView::mh_AdaptName(c_List.c_Name, c_List.c_Comment, "list", orc_ImportInformation);

            //Dataset information
            c_List.c_DataSets.resize(c_VariableLists[u16_ListRamView].GetNumDefaults());
            //In RAMView we do not have names of sets per list but as a basic project setting.
            //Adopt names of named sets:
            for (uint16_t u16_DataSet = 0U; u16_DataSet < c_List.c_DataSets.size(); u16_DataSet++)
            {
               C_OscNodeDataPoolDataSet & rc_DataSet = c_List.c_DataSets[u16_DataSet];

               if (u16_DataSet < c_VariableLists.ac_DefaultNames.GetLength())
               {
                  rc_DataSet.c_Name = c_VariableLists.ac_DefaultNames[u16_DataSet];
               }
               else
               {
                  //No name known for this set; use some default.
                  rc_DataSet.c_Name = "Dataset_" + C_SclString::IntToStr(static_cast<uint32_t>(u16_DataSet) + 1U);
               }
               rc_DataSet.c_Comment = ""; //no comments supported in RAMView

               //make sure the name is openSYDE compliant:
               C_OscImportRamView::mh_AdaptName(rc_DataSet.c_Name, rc_DataSet.c_Comment, "Dataset",
                                                orc_ImportInformation);
            }

            //setting specific to the type of list resp. Datapool:
            if (orc_DataPool.e_Type == C_OscNodeDataPool::eNVM)
            {
               c_List.q_NvmCrcActive = c_VariableLists[u16_ListRamView].q_CheckSummed;
            }
            else
            {
               c_List.q_NvmCrcActive = false;
            }
            c_List.u32_NvmSize = 0U; //will be increased when adding NVM elements
            c_List.u32_NvmStartAddress = 0U;

            //Elements
            //Note that the type "KFX_DATA_TYPE_CRC" is only used in code generated by RAMView.
            //Such variables are not part of the RAMView project.
            //So there is no need to handle those when loading the project.

            //If a list is covered by a CRC it will occupy 2 bytes of NVM
            if (c_VariableLists[u16_ListRamView].q_CheckSummed == true)
            {
               c_List.u32_NvmSize = 2U; //set offset
            }

            c_List.c_Elements.resize(c_VariableLists[u16_ListRamView].VariableList.GetLength());

            for (uint32_t u32_ElementIndex = 0U; u32_ElementIndex <
                 static_cast<uint32_t>(c_VariableLists[u16_ListRamView].VariableList.GetLength());
                 u32_ElementIndex++)
            {
               const C_KFXVariableBase & rc_ElementRamView =
                  c_VariableLists[u16_ListRamView].VariableList[u32_ElementIndex];
               C_OscNodeDataPoolListElement & rc_Element = c_List.c_Elements[u32_ElementIndex];
               int32_t s32_TypeSetResult;
               uint8_t u8_NumCommentSets = 0U;

               rc_Element.c_Name = rc_ElementRamView.c_Name;
               //Only one comment is supported in openSYDE.
               //If there is only one comment set defined then this comment will be imported
               //If there are more comment sets defined: add one line of comment for each RAMView comment set
               // with pattern: <symbolic_name>: <comment>\n
               for (uint8_t u8_CommentIndex = 0U; u8_CommentIndex < KFX_DATA_MAX_NUM_LANGUAGES; u8_CommentIndex++)
               {
                  if (c_ProjectOptions.ac_CommentDescriptions[u8_CommentIndex] == "")
                  {
                     break;
                  }
                  u8_NumCommentSets++;
               }

               //zero or one named sets: use first comment only:
               if (u8_NumCommentSets <= 1U)
               {
                  rc_Element.c_Comment = rc_ElementRamView.ac_Comments[0];
               }
               else
               {
                  //more than one named set: compose:
                  C_SclString c_Comment;
                  for (uint8_t u8_CommentIndex = 0U; u8_CommentIndex < u8_NumCommentSets; u8_CommentIndex++)
                  {
                     c_Comment += (c_ProjectOptions.ac_CommentDescriptions[u8_CommentIndex] + ": " +
                                   rc_ElementRamView.ac_Comments[u8_CommentIndex]);
                     if (u8_CommentIndex < (u8_NumCommentSets - 1U))
                     {
                        c_Comment += "\n";
                     }
                  }
                  rc_Element.c_Comment = c_Comment;
               }

               //make sure the name is openSYDE compliant:
               C_OscImportRamView::mh_AdaptName(rc_Element.c_Name, rc_Element.c_Comment, "element",
                                                orc_ImportInformation);

               //apply number of datasets from list:
               rc_Element.c_DataSetValues.resize(c_List.c_DataSets.size());

               //set type:
               s32_TypeSetResult = mh_ImportElementType(rc_ElementRamView, rc_Element);
               if (s32_TypeSetResult != C_NO_ERR)
               {
                  const C_SclString c_Info = "Variable \"" + c_List.c_Name + "." + rc_Element.c_Name +
                                             "\" has an unsupported type. It was imported as uint8.";
                  orc_ImportInformation.Add(c_Info);
               }

               //set min and max values
               mh_ImportElementMinMax(rc_ElementRamView, rc_Element);

               //Dataset values:
               mh_ImportElementDataSetValues(rc_ElementRamView, rc_Element);

               //report and handle collapsing arrays:
               if ((rc_ElementRamView.IsArrayType() == true) && (rc_ElementRamView.GetNumberOfArrayElements() == 1))
               {
                  const C_SclString c_Info = "Variable \"" + c_List.c_Name + "." + rc_Element.c_Name +
                                             "\" is an array with only one entry. It was imported as non-array element.";
                  orc_ImportInformation.Add(c_Info);

                  //collapse to non-array (do so after importing min/max and Dataset so we get the proper data imported.
                  rc_Element.SetArray(false);
               }

               //RAMView factor is decimal to reduce need for FPU operation (10000 = 1)
               rc_Element.f64_Factor = static_cast<float64_t>(rc_ElementRamView.s32_ScalingFactor) / 10000.0;
               rc_Element.f64_Offset = 0.0; //offset not supported in RAMView
               rc_Element.c_Unit = rc_ElementRamView.c_Unit;

               //Decisions depending on the Datapool type:
               if (orc_DataPool.e_Type == C_OscNodeDataPool::eNVM)
               {
                  //Apply the openSYDE tool default permissions depending on the DP type.
                  rc_Element.e_Access = C_OscNodeDataPoolListElement::eACCESS_RW;

                  //count up number of occupied bytes in NVM:
                  c_List.u32_NvmSize += rc_Element.GetSizeByte();
               }
               else
               {
                  //Apply the openSYDE tool default permissions depending on the DP type.
                  rc_Element.e_Access = C_OscNodeDataPoolListElement::eACCESS_RO;
               }

               //Location "FUNCTION" in RAMView translates to "Event" flag in openSYDE
               rc_Element.q_DiagEventCall = (rc_ElementRamView.q_LocationRAM == false);

               //Special handling if element is of type "array of sint8" and the Dataset value is not zero
               // "terminated". In RAMView this type is used to describe a visible string.
               //In openSYDE we always assume additional space for the "\0" terminator is required.
               //So in this case we replace the final character by '\0' and add a report string.
               if (rc_ElementRamView.u8_Type == KFX_DATA_TYPE_ASINT8)
               {
                  for (uint16_t u16_DataSetIndex = 0U; u16_DataSetIndex < rc_ElementRamView.GetNumDefaults();
                       u16_DataSetIndex++)
                  {
                     if (rc_ElementRamView.GetStringDefault(u16_DataSetIndex).Length() == rc_ElementRamView.GetSize())
                     {
                        //filled up to the max -> patch it
                        const C_SclString c_Info = "Datapool element \"" + c_List.c_Name + "." + rc_Element.c_Name +
                                                   "\" is a string array. Dataset \"" +
                                                   c_List.c_DataSets[u16_DataSetIndex].c_Name +
                                                   "\" contains value \"" +
                                                   rc_ElementRamView.GetStringDefault(u16_DataSetIndex) +
                                                   "\" without zero termination. "
                                                   "The final character was replaced by a zero termination.";
                        orc_ImportInformation.Add(c_Info);

                        rc_Element.c_DataSetValues[u16_DataSetIndex].SetValueArrS8Element(0,
                                                                                          rc_Element.GetSizeByte() - 1);
                     }
                  }
               }
            }
            orc_DataPool.c_Lists.push_back(c_List);
         }
      }

      orc_DataPool.u32_NvmSize = 0U;
      if (orc_DataPool.e_Type == C_OscNodeDataPool::eNVM)
      {
         //Set NvmSize to the number of bytes occupied by the sum of all imported NVM elements:
         for (uint16_t u16_List = 0U; u16_List < orc_DataPool.c_Lists.size(); u16_List++)
         {
            orc_DataPool.u32_NvmSize += orc_DataPool.c_Lists[u16_List].u32_NvmSize;
         }
      }

      osc_write_log_info("Loading RAMView project",
                         "Content of project \"" + orc_ProjectPath + "\" was imported to openSYDE data structures. "
                         "Number of imported lists: " + C_SclString::IntToStr(orc_DataPool.c_Lists.size()) + ".");
   }

   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility to set openSYDE type based on RAMView type

   Will set the following elements of orc_Element:
   * Type
   * IsArray
   * Number of array elements
   This will also preset the types for the "Content" elements for min/max/value/datasets

   \param[in]     orc_RamViewElement          KEFEX RAMView variable element
   \param[in,out] orc_Element                 openSYDE element with updated information

   \return
   C_NO_ERR     information added
   C_RANGE      unexpected type or size (type set to eUINT8, non-array)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscImportRamView::mh_ImportElementType(const stw::diag_lib::C_KFXVariableBase & orc_RamViewElement,
                                                 C_OscNodeDataPoolListElement & orc_Element)
{
   int32_t s32_Result = C_NO_ERR;

   switch (orc_RamViewElement.u8_Type)
   {
   case KFX_DATA_TYPE_SINT8:
   case KFX_DATA_TYPE_ASINT8:
      orc_Element.SetType(C_OscNodeDataPoolContent::eSINT8);
      break;
   case KFX_DATA_TYPE_UINT8:
   case KFX_DATA_TYPE_AUINT8:
      orc_Element.SetType(C_OscNodeDataPoolContent::eUINT8);
      break;
   case KFX_DATA_TYPE_SINT16:
   case KFX_DATA_TYPE_ASINT16:
      orc_Element.SetType(C_OscNodeDataPoolContent::eSINT16);
      break;
   case KFX_DATA_TYPE_UINT16:
   case KFX_DATA_TYPE_AUINT16:
      orc_Element.SetType(C_OscNodeDataPoolContent::eUINT16);
      break;
   case KFX_DATA_TYPE_SINT32:
   case KFX_DATA_TYPE_ASINT32:
      orc_Element.SetType(C_OscNodeDataPoolContent::eSINT32);
      break;
   case KFX_DATA_TYPE_UINT32:
   case KFX_DATA_TYPE_AUINT32:
      orc_Element.SetType(C_OscNodeDataPoolContent::eUINT32);
      break;
   case KFX_DATA_TYPE_SINT64:
   case KFX_DATA_TYPE_ASINT64:
      orc_Element.SetType(C_OscNodeDataPoolContent::eSINT64);
      break;
   case KFX_DATA_TYPE_UINT64:
   case KFX_DATA_TYPE_AUINT64:
      orc_Element.SetType(C_OscNodeDataPoolContent::eUINT64);
      break;
   case KFX_DATA_TYPE_FLOAT32:
   case KFX_DATA_TYPE_AFLOAT32:
      orc_Element.SetType(C_OscNodeDataPoolContent::eFLOAT32);
      break;
   case KFX_DATA_TYPE_FLOAT64:
   case KFX_DATA_TYPE_AFLOAT64:
      orc_Element.SetType(C_OscNodeDataPoolContent::eFLOAT64);
      break;
   default:
      s32_Result = C_RANGE;
      orc_Element.SetType(C_OscNodeDataPoolContent::eUINT8);
      break;
   }

   orc_Element.SetArray(orc_RamViewElement.IsArrayType());

   if (orc_RamViewElement.IsArrayType() == true)
   {
      orc_Element.SetArraySize(orc_RamViewElement.GetNumberOfArrayElements());
   }

   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility to import min/max values

   The functions expects that the type of orc_Element is already set to match the type of orc_RamViewElement.

   \param[in]     orc_RamViewElement          KEFEX RAMView variable element
   \param[in,out] orc_Element                 openSYDE element with updated information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscImportRamView::mh_ImportElementMinMax(const C_KFXVariableBase & orc_RamViewElement,
                                                C_OscNodeDataPoolListElement & orc_Element)
{
   if (orc_Element.GetArray() == false)
   {
      switch (orc_Element.GetType())
      {
      case C_OscNodeDataPoolContent::eUINT8:
         orc_Element.c_MinValue.SetValueU8(static_cast<uint8_t>(orc_RamViewElement.GetNumericMin()));
         orc_Element.c_MaxValue.SetValueU8(static_cast<uint8_t>(orc_RamViewElement.GetNumericMax()));
         break;
      case C_OscNodeDataPoolContent::eUINT16:
         orc_Element.c_MinValue.SetValueU16(static_cast<uint16_t>(orc_RamViewElement.GetNumericMin()));
         orc_Element.c_MaxValue.SetValueU16(static_cast<uint16_t>(orc_RamViewElement.GetNumericMax()));
         break;
      case C_OscNodeDataPoolContent::eUINT32:
         orc_Element.c_MinValue.SetValueU32(static_cast<uint32_t>(orc_RamViewElement.GetNumericMin()));
         orc_Element.c_MaxValue.SetValueU32(static_cast<uint32_t>(orc_RamViewElement.GetNumericMax()));
         break;
      case C_OscNodeDataPoolContent::eSINT8:
         orc_Element.c_MinValue.SetValueS8(static_cast<int8_t>(orc_RamViewElement.GetNumericMin()));
         orc_Element.c_MaxValue.SetValueS8(static_cast<int8_t>(orc_RamViewElement.GetNumericMax()));
         break;
      case C_OscNodeDataPoolContent::eSINT16:
         orc_Element.c_MinValue.SetValueS16(static_cast<int16_t>(orc_RamViewElement.GetNumericMin()));
         orc_Element.c_MaxValue.SetValueS16(static_cast<int16_t>(orc_RamViewElement.GetNumericMax()));
         break;
      case C_OscNodeDataPoolContent::eSINT32:
         orc_Element.c_MinValue.SetValueS32(static_cast<int32_t>(orc_RamViewElement.GetNumericMin()));
         orc_Element.c_MaxValue.SetValueS32(static_cast<int32_t>(orc_RamViewElement.GetNumericMax()));
         break;
      case C_OscNodeDataPoolContent::eSINT64:
         orc_Element.c_MinValue.SetValueS64(orc_RamViewElement.GetNumericMin());
         orc_Element.c_MaxValue.SetValueS64(orc_RamViewElement.GetNumericMax());
         break;
      case C_OscNodeDataPoolContent::eFLOAT32:
         orc_Element.c_MinValue.SetValueF32(static_cast<float32_t>(orc_RamViewElement.GetFloatMin()));
         orc_Element.c_MaxValue.SetValueF32(static_cast<float32_t>(orc_RamViewElement.GetFloatMax()));
         break;
      case C_OscNodeDataPoolContent::eFLOAT64:
         orc_Element.c_MinValue.SetValueF64(orc_RamViewElement.GetFloatMin());
         orc_Element.c_MaxValue.SetValueF64(orc_RamViewElement.GetFloatMax());
         break;
      case C_OscNodeDataPoolContent::eUINT64:
      default:
         tgl_assert(false); //uint64 is not supported in RAMView projects
         break;
      }
   }
   else
   {
      //arrays:
      switch (orc_Element.GetType())
      {
      case C_OscNodeDataPoolContent::eUINT8:
         for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
         {
            orc_Element.c_MinValue.SetValueArrU8Element(
               static_cast<uint8_t>(orc_RamViewElement.GetNumericMinFromArray(
                                       u32_ArrayIndex)), u32_ArrayIndex);
            orc_Element.c_MaxValue.SetValueArrU8Element(
               static_cast<uint8_t>(orc_RamViewElement.GetNumericMaxFromArray(
                                       u32_ArrayIndex)), u32_ArrayIndex);
         }
         break;
      case C_OscNodeDataPoolContent::eUINT16:
         for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
         {
            orc_Element.c_MinValue.SetValueArrU16Element(
               static_cast<uint16_t>(orc_RamViewElement.GetNumericMinFromArray(
                                        u32_ArrayIndex)), u32_ArrayIndex);
            orc_Element.c_MaxValue.SetValueArrU16Element(
               static_cast<uint16_t>(orc_RamViewElement.GetNumericMaxFromArray(
                                        u32_ArrayIndex)), u32_ArrayIndex);
         }
         break;
      case C_OscNodeDataPoolContent::eUINT32:
         for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
         {
            orc_Element.c_MinValue.SetValueArrU32Element(
               static_cast<uint32_t>(orc_RamViewElement.GetNumericMinFromArray(
                                        u32_ArrayIndex)), u32_ArrayIndex);
            orc_Element.c_MaxValue.SetValueArrU32Element(
               static_cast<uint32_t>(orc_RamViewElement.GetNumericMaxFromArray(
                                        u32_ArrayIndex)), u32_ArrayIndex);
         }
         break;
      case C_OscNodeDataPoolContent::eSINT8:
         for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
         {
            orc_Element.c_MinValue.SetValueArrS8Element(
               static_cast<int8_t>(orc_RamViewElement.GetNumericMinFromArray(u32_ArrayIndex)),
               u32_ArrayIndex);
            orc_Element.c_MaxValue.SetValueArrS8Element(
               static_cast<int8_t>(orc_RamViewElement.GetNumericMaxFromArray(u32_ArrayIndex)),
               u32_ArrayIndex);
         }
         break;
      case C_OscNodeDataPoolContent::eSINT16:
         for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
         {
            orc_Element.c_MinValue.SetValueArrS16Element(
               static_cast<int16_t>(orc_RamViewElement.GetNumericMinFromArray(
                                       u32_ArrayIndex)), u32_ArrayIndex);
            orc_Element.c_MaxValue.SetValueArrS16Element(
               static_cast<int16_t>(orc_RamViewElement.GetNumericMaxFromArray(
                                       u32_ArrayIndex)), u32_ArrayIndex);
         }
         break;
      case C_OscNodeDataPoolContent::eSINT32:
         for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
         {
            orc_Element.c_MinValue.SetValueArrS32Element(
               static_cast<int32_t>(orc_RamViewElement.GetNumericMinFromArray(
                                       u32_ArrayIndex)), u32_ArrayIndex);
            orc_Element.c_MaxValue.SetValueArrS32Element(
               static_cast<int32_t>(orc_RamViewElement.GetNumericMaxFromArray(
                                       u32_ArrayIndex)), u32_ArrayIndex);
         }
         break;
      case C_OscNodeDataPoolContent::eSINT64:
         for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
         {
            orc_Element.c_MinValue.SetValueArrS64Element(
               orc_RamViewElement.GetNumericMinFromArray(u32_ArrayIndex), u32_ArrayIndex);
            orc_Element.c_MaxValue.SetValueArrS64Element(
               orc_RamViewElement.GetNumericMaxFromArray(u32_ArrayIndex), u32_ArrayIndex);
         }
         break;
      case C_OscNodeDataPoolContent::eFLOAT32:
         for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
         {
            orc_Element.c_MinValue.SetValueArrF32Element(
               static_cast<float32_t>(orc_RamViewElement.GetFloatMinFromArray(
                                         u32_ArrayIndex)), u32_ArrayIndex);
            orc_Element.c_MaxValue.SetValueArrF32Element(
               static_cast<float32_t>(orc_RamViewElement.GetFloatMaxFromArray(
                                         u32_ArrayIndex)), u32_ArrayIndex);
         }
         break;
      case C_OscNodeDataPoolContent::eFLOAT64:
         for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
         {
            orc_Element.c_MinValue.SetValueArrF64Element(
               orc_RamViewElement.GetFloatMinFromArray(u32_ArrayIndex), u32_ArrayIndex);
            orc_Element.c_MaxValue.SetValueArrF64Element(
               orc_RamViewElement.GetFloatMaxFromArray(u32_ArrayIndex), u32_ArrayIndex);
         }
         break;
      case C_OscNodeDataPoolContent::eUINT64:
      default:
         tgl_assert(false); //uint64 is not supported in RAMView projects
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility to import Dataset values

   The functions expects that the
   * type of orc_Element is already set to match the type of orc_RamViewElement
   * number of Datasets in orc_Elements already matches the number in orc_RamViewElement

   \param[in]     orc_RamViewElement          KEFEX RAMView variable element
   \param[in,out] orc_Element                 openSYDE element with updated information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscImportRamView::mh_ImportElementDataSetValues(const C_KFXVariableBase & orc_RamViewElement,
                                                       C_OscNodeDataPoolListElement & orc_Element)
{
   for (uint16_t u16_DataSetIndex = 0U; u16_DataSetIndex < orc_RamViewElement.GetNumDefaults();
        u16_DataSetIndex++)
   {
      C_OscNodeDataPoolContent & rc_Content = orc_Element.c_DataSetValues[u16_DataSetIndex];

      if (orc_Element.GetArray() == false)
      {
         switch (orc_Element.GetType())
         {
         case C_OscNodeDataPoolContent::eUINT8:
            rc_Content.SetValueU8(static_cast<uint8_t>(orc_RamViewElement.GetNumericDefault(u16_DataSetIndex)));
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            rc_Content.SetValueU16(static_cast<uint16_t>(orc_RamViewElement.GetNumericDefault(u16_DataSetIndex)));
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            rc_Content.SetValueU32(static_cast<uint32_t>(orc_RamViewElement.GetNumericDefault(u16_DataSetIndex)));
            break;
         case C_OscNodeDataPoolContent::eSINT8:
            rc_Content.SetValueS8(static_cast<int8_t>(orc_RamViewElement.GetNumericDefault(u16_DataSetIndex)));
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            rc_Content.SetValueS16(static_cast<int16_t>(orc_RamViewElement.GetNumericDefault(u16_DataSetIndex)));
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            rc_Content.SetValueS32(static_cast<int32_t>(orc_RamViewElement.GetNumericDefault(u16_DataSetIndex)));
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            rc_Content.SetValueS64(orc_RamViewElement.GetNumericDefault(u16_DataSetIndex));
            break;
         case C_OscNodeDataPoolContent::eFLOAT32:
            rc_Content.SetValueF32(static_cast<float32_t>(orc_RamViewElement.GetFloatDefault(u16_DataSetIndex)));
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            rc_Content.SetValueF64(orc_RamViewElement.GetFloatDefault(u16_DataSetIndex));
            break;
         case C_OscNodeDataPoolContent::eUINT64:
         default:
            tgl_assert(false); //uint64 is not supported in RAMView projects
            break;
         }
      }
      else
      {
         //arrays:
         switch (orc_Element.GetType())
         {
         case C_OscNodeDataPoolContent::eUINT8:
            for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
            {
               rc_Content.SetValueArrU8Element(static_cast<uint8_t>(orc_RamViewElement.
                                                                    GetNumericDefaultFromArray(
                                                                       u32_ArrayIndex, u16_DataSetIndex)),
                                               u32_ArrayIndex);
            }
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
            {
               rc_Content.SetValueArrU16Element(static_cast<uint16_t>(orc_RamViewElement.
                                                                      GetNumericDefaultFromArray(
                                                                         u32_ArrayIndex, u16_DataSetIndex)),
                                                u32_ArrayIndex);
            }
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
            {
               rc_Content.SetValueArrU32Element(static_cast<uint32_t>(orc_RamViewElement.
                                                                      GetNumericDefaultFromArray(
                                                                         u32_ArrayIndex, u16_DataSetIndex)),
                                                u32_ArrayIndex);
            }
            break;
         case C_OscNodeDataPoolContent::eSINT8:
            for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
            {
               rc_Content.SetValueArrS8Element(static_cast<int8_t>(orc_RamViewElement.
                                                                   GetNumericDefaultFromArray(
                                                                      u32_ArrayIndex, u16_DataSetIndex)),
                                               u32_ArrayIndex);
            }
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
            {
               rc_Content.SetValueArrS16Element(static_cast<int16_t>(orc_RamViewElement.
                                                                     GetNumericDefaultFromArray(
                                                                        u32_ArrayIndex, u16_DataSetIndex)),
                                                u32_ArrayIndex);
            }
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
            {
               rc_Content.SetValueArrS32Element(static_cast<int32_t>(orc_RamViewElement.
                                                                     GetNumericDefaultFromArray(
                                                                        u32_ArrayIndex, u16_DataSetIndex)),
                                                u32_ArrayIndex);
            }
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
            {
               rc_Content.SetValueArrS64Element(orc_RamViewElement.GetNumericDefaultFromArray(
                                                   u32_ArrayIndex, u16_DataSetIndex),
                                                u32_ArrayIndex);
            }
            break;
         case C_OscNodeDataPoolContent::eFLOAT32:
            for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
            {
               rc_Content.SetValueArrF32Element(static_cast<float32_t>(orc_RamViewElement.
                                                                       GetFloatDefaultFromArray(
                                                                          u32_ArrayIndex,
                                                                          u16_DataSetIndex)),
                                                u32_ArrayIndex);
            }
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            for (uint32_t u32_ArrayIndex = 0U; u32_ArrayIndex < orc_Element.GetArraySize(); u32_ArrayIndex++)
            {
               rc_Content.SetValueArrF64Element(orc_RamViewElement.GetFloatDefaultFromArray(
                                                   u32_ArrayIndex,
                                                   u16_DataSetIndex),
                                                u32_ArrayIndex);
            }
            break;
         case C_OscNodeDataPoolContent::eUINT64:
         default:
            tgl_assert(false); //uint64 is not supported in RAMView projects
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load RAMView .def project

   Load information from RAMView .def project and associated files:
   * Data Pool list and variable definitions
   * Basic project options

   This implementation is based on pre-existing code from KEFEX Diaglib (C_KFXProject::OpenProject).
   The logic was simplified to match the use case to import from existing projects in the openSYDE context:
   * no dependency from protocol drivers
   * can only open .def style projects (no .ksp projects)
   * project passwords will be ignored (these are not considered a real protection in .def-style projects)

   Error reporting:
   We assume we have correct project files that load ok in RAMView.
   So we do not provide a fancy error detail string reporting engine here. Also we only provide one "could not open"
    error code.
   But we'll log to the OSC logger if there are any problems.

   \param[in]  orc_ProjectPath     Full path to .def file
   \param[out] orc_ProjectOptions  Loaded project information
   \param[out] orc_VariableLists   Loaded Data Pool definition

   \return
   C_NO_ERR     information read
   C_RD_WR      critical problems loading project information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscImportRamView::mh_LoadRamViewDefProject(const stw::scl::C_SclString & orc_ProjectPath,
                                                     stw::diag_lib::C_KFXProjectOptions & orc_ProjectOptions,
                                                     stw::diag_lib::C_KFXVariableLists & orc_VariableLists)
{
   int32_t s32_Return = C_NO_ERR;

   orc_VariableLists.SetLength(0);

   if (TglExtractFileExtension(orc_ProjectPath).LowerCase() != ".def")
   {
      osc_write_log_error("Loading RAMView project",
                          "File \"" + orc_ProjectPath + "\" does not have the file extension \".def\".");
      s32_Return = C_RD_WR;
   }
   else if (TglFileExists(orc_ProjectPath) == false)
   {
      osc_write_log_error("Loading RAMView project", "File \"" + orc_ProjectPath + "\" does not exist.");
      s32_Return = C_RD_WR;
   }
   else
   {
      //everything fine so far ...
   }

   if (s32_Return == C_NO_ERR)
   {
      C_OscChecksummedIniFile * pc_IniFile;
      bool q_Return;

      //.def file format:
      pc_IniFile = new C_OscChecksummedIniFile(orc_ProjectPath);
      q_Return = pc_IniFile->CheckCheckSum();
      if (q_Return == false)
      {
         osc_write_log_error("Loading RAMView project",
                             "File \"" + orc_ProjectPath + "\" has incorrect file checksum.");
         s32_Return = C_RD_WR;
      }

      if (s32_Return == C_NO_ERR)
      {
         //Load Device Parameter from *.def File
         //load communication protocol parameters
         orc_ProjectOptions.LoadConfigFromIni(*pc_IniFile);
         if (orc_ProjectOptions.c_DeviceName == "")
         {
            osc_write_log_error("Loading RAMView project",
                                "File \"" + orc_ProjectPath +
                                "\" does not seem to contain a DEVICENAME. Is this a proper RAMView project file ?");
            s32_Return = C_RD_WR;
         }
         else
         {
            //load default names
            C_KFXDEFProject::LoadDefaultNames(pc_IniFile, orc_VariableLists);
         }
      }
      delete pc_IniFile;
   }

   //scan for .ram files and read them in:
   if (s32_Return == C_NO_ERR)
   {
      C_SclDynamicArray<C_SclString> c_Warnings;
      C_SclString c_ErrorText;
      const C_SclString c_WorkDirectory = TglExtractFilePath(orc_ProjectPath);

      s32_Return = C_KFXDEFProject::LoadRAMFiles(c_WorkDirectory, orc_ProjectOptions.c_DeviceName, orc_VariableLists,
                                                 c_ErrorText, c_Warnings);
      //report warnings from loading .ram files in any case (makes it easier to locate problems for user):
      for (int32_t s32_Index = 0; s32_Index < c_Warnings.GetLength(); s32_Index++)
      {
         osc_write_log_warning("Loading RAMView project", c_Warnings[s32_Index]);
      }
      switch (s32_Return)
      {
      case C_NO_ERR:
         break;
      case C_WARN:
         osc_write_log_error("Loading RAMView project", "No variable lists found in project.");
         s32_Return = C_RD_WR;
         break;
      case C_CONFIG:
         osc_write_log_error("Loading RAMView project", "List index inconsistency.");
         s32_Return = C_RD_WR;
         break;
      case C_RD_WR:
         osc_write_log_error("Loading RAMView project", "Error reading or parsing .ram file. Detail: " + c_ErrorText);
         s32_Return = C_RD_WR;
         break;
      default:
         osc_write_log_error("Loading RAMView project", "Unexpected error parsing .ram files.");
         s32_Return = C_RD_WR;
         break;
      }
   }

   //load default file
   if (s32_Return == C_NO_ERR)
   {
      const C_SclString c_FilePath = TglChangeFileExtension(orc_ProjectPath, ".dat");

      //preset defaults to clearly defined values:
      orc_VariableLists.ClearDefaults();

      s32_Return = C_KFXDATFile::LoadDATAllLists(TglChangeFileExtension(orc_ProjectPath, ".dat"),
                                                 orc_ProjectOptions.c_DeviceName, orc_VariableLists, NULL);
      switch (s32_Return)
      {
      case C_NO_ERR:
         break;
      case C_WARN:
         //just a warning; .dat does not exactly match project: continue nevertheless
         osc_write_log_warning("Loading RAMView project",
                               ".dat file \"" + c_FilePath + "\" did not contain default values for all variables.");
         s32_Return = C_NO_ERR;
         break;
      case C_NOACT:
         osc_write_log_error("Loading RAMView project",
                             "Could not load default values from .dat file \"" + c_FilePath +
                             "\". Could not open file or file not valid.");
         s32_Return = C_RD_WR;
         break;
      case C_CONFIG:
         osc_write_log_error("Loading RAMView project",
                             "Could not load default values from .dat file \"" + c_FilePath +
                             "\". Unexpected file version or file is for different project.");
         s32_Return = C_RD_WR;
         break;
      default:
         osc_write_log_error("Loading RAMView project",
                             "Could not load default values from .dat file \"" + c_FilePath + "\". Unexpected error.");
         s32_Return = C_RD_WR;
         break;
      }
   }

   //load comment file
   if (s32_Return == C_NO_ERR)
   {
      const C_SclString c_FilePath = TglChangeFileExtension(orc_ProjectPath, ".rec");

      s32_Return = C_KFXDEFProject::LoadComments(c_FilePath,
                                                 orc_ProjectOptions.c_DeviceName, orc_VariableLists,
                                                 orc_ProjectOptions.ac_CommentDescriptions);
      if (s32_Return != C_NO_ERR)
      {
         //this is not critical; only report as warning
         switch (s32_Return)
         {
         case C_CONFIG:
            osc_write_log_warning("Loading RAMView project",
                                  "Could not load comments from .rec file \"" + c_FilePath +
                                  "\". File is for a different project.");
            break;
         case C_NOACT:
            osc_write_log_warning("Loading RAMView project",
                                  "Could not load comments from .rec file \"" + c_FilePath +
                                  "\". File does not exist.");
            break;
         case C_RD_WR:
            osc_write_log_warning("Loading RAMView project",
                                  "Could not load comments from .rec file \"" + c_FilePath +
                                  "\". Invalid language name in file.");
            break;
         case C_OVERFLOW:
            osc_write_log_warning("Loading RAMView project",
                                  "Could not load comments from .rec file \"" + c_FilePath +
                                  "\". Too many languages in file.");
            break;
         default:
            osc_write_log_warning("Loading RAMView project",
                                  "Could not load comments from .rec file \"" + c_FilePath + "\". Unexpected error.");
            break;
         }
         s32_Return = C_NO_ERR;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility: Adapt RAMView name to openSYDE conventions

   Naming rules for project, lists, variables, Datasets in RAMView is more relaxed.

   This function will:
   * Add a "_" prefix if orc_Name start with a digit
   * Eliminate spaces from orc_Name (just in case some creeped in somewhere)
   * Cut orc_Name after 31 characters
   * Append a note to "orc_Comment" with the original name

   The names are *not* checked for non-compliant characters.
   RAMView prevents entering such names. While it would be technically possible to have created project files with
    invalid characters from another source this is considered a minor risk. And even if we have such a scenario the
    names will be flagged in the openSYDE GUI after importing.

   \param[in,out]  orc_Name              in: original name; out: adapted name
   \param[in,out]  orc_Comment           in: original comment; out: comment with added "upgrade information" (if needed)
   \param[in]      orc_ElementDesignator Description of the object for which the name shall be adapted (for reporting)
   \param[in]      orc_ImportInformation List to which to add reporting information (if the name changes)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscImportRamView::mh_AdaptName(C_SclString & orc_Name, C_SclString & orc_Comment,
                                      const stw::scl::C_SclString & orc_ElementDesignator,
                                      stw::scl::C_SclStringList & orc_ImportInformation)
{
   const int32_t s32_C_ITEM_MAX_CHAR_COUNT = 31;
   C_SclString c_NewName = orc_Name;
   char_t cn_FirstCharacter;

   //eliminate all spaces:
   c_NewName.ReplaceAll(" ", "");

   //prefix with "_" if starting with a digit
   //isdigit behavior is undefined with values < 0
   cn_FirstCharacter = c_NewName.c_str()[0];
   if ((c_NewName.Length() >= 1) && (static_cast<int8_t>(cn_FirstCharacter) >= 0) &&
       (std::isdigit(cn_FirstCharacter) != 0))
   {
      c_NewName = "_" + c_NewName;
   }

   //cut string:
   if (c_NewName.Length() > s32_C_ITEM_MAX_CHAR_COUNT)
   {
      c_NewName.Delete(s32_C_ITEM_MAX_CHAR_COUNT + 1, c_NewName.Length());
   }

   if (orc_Name != c_NewName)
   {
      //Name was changed; add information to comment
      if (orc_Comment != "")
      {
         orc_Comment += "\n";
      }
      //Set comment before overwriting the original name
      orc_Comment += ("Original name (from import source): \"" + orc_Name + "\"");

      //add report:
      orc_ImportInformation.Add("Name of " + orc_ElementDesignator +
                                " \"" + orc_Name + "\" was changed to \"" + c_NewName + "\".");

      orc_Name = c_NewName;
   }
}
