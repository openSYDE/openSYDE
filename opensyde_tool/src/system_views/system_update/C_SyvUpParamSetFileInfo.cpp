//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Display class for parameter set file information and comparison (implementation)

   Display class for parameter set file information and comparison

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_OSCParamSetHandler.h"
#include "C_SyvUpParamSetFileInfo.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvUpParamSetFileInfo::mhc_StartHeadingTD = "<td style=\"padding: 9px 9px 0 0;\">";
const QString C_SyvUpParamSetFileInfo::mhc_ContinueHeadingTD = "<td style=\"padding: 9px 9px 0 9px;\">";
const QString C_SyvUpParamSetFileInfo::mhc_StartDataTD = "<td style=\"padding: 0 9px 0 0;white-space: pre;\">";
const QString C_SyvUpParamSetFileInfo::mhc_ContinueDataTD = "<td style=\"padding: 0 9px 0 9px;\">";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in] orc_Path        Path for parameter set (usage: absolute path to load)
   \param[in] orc_StoragePath Path to return for param info structure (usage: relative path for saving)
   \param[in] ou32_NodeIndex  Node index for comparison
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpParamSetFileInfo::C_SyvUpParamSetFileInfo(const QString & orc_Path, const QString & orc_StoragePath,
                                                 const stw_types::uint32 ou32_NodeIndex) :
   mc_Path(orc_Path),
   mc_StoragePath(orc_StoragePath),
   mu32_NodeIndex(ou32_NodeIndex)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle file read step

   \return
   C_NO_ERR   data read
   C_RD_WR    specified file does not exist
              specified file is present but structure is invalid (e.g. invalid XML file; not checksum found)
   C_CHECKSUM specified file is present but checksum is invalid
   C_CONFIG   file does not contain essential information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpParamSetFileInfo::ReadFile(void)
{
   uint16 u16_FileCrc;
   bool q_OptionlContentMissing;
   C_OSCParamSetHandler c_FileHandler;

   const sint32 s32_Retval = c_FileHandler.ReadFile(
      this->mc_Path.toStdString().c_str(), false, true, &u16_FileCrc, &q_OptionlContentMissing);

   if (s32_Retval == C_NO_ERR)
   {
      this->mc_InterpretedFileInfo = c_FileHandler.GetInterpretedData();
      this->mc_ParamInfo.SetContent(this->mc_StoragePath, u16_FileCrc);
      this->mc_ComparisonResults.clear();
      m_Comparison(q_OptionlContentMissing);
      m_ConvertToHtmlString(q_OptionlContentMissing);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get read file info (check file read function return value for validity)

   \return
   Read file info (check file read function return value for validity)
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvNodeUpdateParamInfo & C_SyvUpParamSetFileInfo::GetParamInfo(void) const
{
   return this->mc_ParamInfo;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current path

   \return
   Current path
*/
//----------------------------------------------------------------------------------------------------------------------
const QString & C_SyvUpParamSetFileInfo::GetPath(void) const
{
   return this->mc_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get comparison result text

   \return
   Comparison result text
*/
//----------------------------------------------------------------------------------------------------------------------
const QString & C_SyvUpParamSetFileInfo::GetComparisonResultsHtml(void) const
{
   return this->mc_ComparisonHtml;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add comparison

   \param[in] oq_OptionlContentMissing Indicator if optional content missing
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpParamSetFileInfo::m_Comparison(const bool oq_OptionlContentMissing)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if ((pc_Node != NULL) && (this->mc_InterpretedFileInfo.c_InterpretedNodes.size() > 0UL))
   {
      const C_OSCParamSetInterpretedNode & rc_InterpretedNode = this->mc_InterpretedFileInfo.c_InterpretedNodes[0UL];
      //Node
      //Node name
      this->m_CompareString(rc_InterpretedNode.c_Name.c_str(),
                            pc_Node->c_Properties.c_Name.c_str(), C_GtGetText::h_GetText("Node name"), 0UL);
      for (uint32 u32_ItInDp = 0UL; u32_ItInDp < rc_InterpretedNode.c_DataPools.size(); ++u32_ItInDp)
      {
         //Datapools
         bool q_DpMatch = false;
         const C_OSCParamSetInterpretedDataPool & rc_InDp = rc_InterpretedNode.c_DataPools[u32_ItInDp];
         for (uint32 u32_ItNoDp = 0UL; u32_ItNoDp < pc_Node->c_DataPools.size(); ++u32_ItNoDp)
         {
            const C_OSCNodeDataPool & rc_NoDp = pc_Node->c_DataPools[u32_ItNoDp];
            if (rc_InDp.c_DataPoolInfo.c_Name == rc_NoDp.c_Name)
            {
               QString c_Value;
               uint32 u32_DpHash = 0UL;
               //Datapool
               q_DpMatch = true;
               //Datapool name
               this->m_CompareString(rc_InDp.c_DataPoolInfo.c_Name.c_str(),
                                     rc_NoDp.c_Name.c_str(), C_GtGetText::h_GetText("Datapool name"), 1UL);
               rc_NoDp.CalcDefinitionHash(u32_DpHash);
               //Datapool CRC
               this->m_CompareString(QString("0x%1").arg(rc_InDp.c_DataPoolInfo.u32_DataPoolCrc, 0, 16),
                                     QString("0x%1").arg(u32_DpHash, 0, 16), C_GtGetText::h_GetText("Datapool CRC"),
                                     1UL);
               //Datapool NVM start address
               if (oq_OptionlContentMissing)
               {
                  c_Value = C_GtGetText::h_GetText("unknown");
               }
               else
               {
                  c_Value = QString("%1").arg(rc_NoDp.u32_NvMStartAddress);
               }
               this->m_CompareString(QString("%1").arg(rc_InDp.c_DataPoolInfo.u32_NvMStartAddress),
                                     c_Value, C_GtGetText::h_GetText("Datapool NVM start address"), 1UL);
               //Datapool NVM size
               if (oq_OptionlContentMissing)
               {
                  c_Value = C_GtGetText::h_GetText("unknown");
               }
               else
               {
                  c_Value = QString("%1").arg(rc_NoDp.u32_NvMSize);
               }
               this->m_CompareString(QString("%1").arg(rc_InDp.c_DataPoolInfo.u32_NvMSize),
                                     c_Value, C_GtGetText::h_GetText("Datapool NVM size"), 1UL);
               //Datapool version
               this->m_CompareString(C_SyvUpParamSetFileInfo::mh_GetVersionString(rc_InDp.c_DataPoolInfo.au8_Version[0],
                                                                                  rc_InDp.c_DataPoolInfo.au8_Version[1],
                                                                                  rc_InDp.c_DataPoolInfo.au8_Version[2]),
                                     C_SyvUpParamSetFileInfo::mh_GetVersionString(rc_NoDp.au8_Version[0],
                                                                                  rc_NoDp.au8_Version[1],
                                                                                  rc_NoDp.au8_Version[2]),
                                     C_GtGetText::h_GetText("Datapool version"), 1UL);
               for (uint32 u32_ItInLi = 0UL; u32_ItInLi < rc_InDp.c_Lists.size(); ++u32_ItInLi)
               {
                  //Lists
                  bool q_LiMatch = false;
                  const C_OSCParamSetInterpretedList & rc_InLi = rc_InDp.c_Lists[u32_ItInLi];
                  for (uint32 u32_ItNoLi = 0UL; u32_ItNoLi < rc_NoDp.c_Lists.size(); ++u32_ItNoLi)
                  {
                     const C_OSCNodeDataPoolList & rc_NoLi = rc_NoDp.c_Lists[u32_ItNoLi];
                     if (rc_InLi.c_Name == rc_NoLi.c_Name)
                     {
                        //List
                        q_LiMatch = true;
                        //List name
                        this->m_CompareString(rc_InLi.c_Name.c_str(), rc_NoLi.c_Name.c_str(),
                                              C_GtGetText::h_GetText("List name"), 2UL);
                        //List elements
                        this->m_CompareString(QString("%1").arg(rc_InLi.c_Elements.size()),
                                              QString("%1").arg(rc_NoLi.c_Elements.size()),
                                              C_GtGetText::h_GetText("Number of elements in list"), 2UL);
                        break;
                     }
                  }
                  if (q_LiMatch == false)
                  {
                     this->m_DisplayNoMatch(rc_InLi.c_Name.c_str(), C_GtGetText::h_GetText("List name"), 2UL);
                  }
               }
               break;
            }
         }
         if (q_DpMatch == false)
         {
            this->m_DisplayNoMatch(rc_InDp.c_DataPoolInfo.c_Name.c_str(), C_GtGetText::h_GetText("Datapool name"), 1UL);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert comparison results to html

   \param[in] oq_OptionlContentMissing Indicator if optional content missing
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpParamSetFileInfo::m_ConvertToHtmlString(const bool oq_OptionlContentMissing)
{
   QString c_Value;

   this->mc_ComparisonHtml.clear();

   //File information
   this->mc_ComparisonHtml += "<h3>" + QString(C_GtGetText::h_GetText("Meta information")) + "</h3>";
   this->mc_ComparisonHtml += "<table>";

   if (oq_OptionlContentMissing)
   {
      c_Value = C_GtGetText::h_GetText("unknown");
   }
   else
   {
      c_Value = this->mc_InterpretedFileInfo.c_FileInfo.c_DateTime.c_str();
   }
   this->m_AddTableRow(C_GtGetText::h_GetText("Created:"), c_Value);
   if (oq_OptionlContentMissing)
   {
      c_Value = C_GtGetText::h_GetText("unknown");
   }
   else
   {
      c_Value = this->mc_InterpretedFileInfo.c_FileInfo.c_Creator.c_str();
   }
   this->m_AddTableRow(C_GtGetText::h_GetText("Creator:"), c_Value);
   if (oq_OptionlContentMissing)
   {
      c_Value = C_GtGetText::h_GetText("unknown");
   }
   else
   {
      c_Value = this->mc_InterpretedFileInfo.c_FileInfo.c_ToolName.c_str();
   }
   this->m_AddTableRow(C_GtGetText::h_GetText("Tool name:"), c_Value);
   if (oq_OptionlContentMissing)
   {
      c_Value = C_GtGetText::h_GetText("unknown");
   }
   else
   {
      c_Value = this->mc_InterpretedFileInfo.c_FileInfo.c_ToolVersion.c_str();
   }
   this->m_AddTableRow(C_GtGetText::h_GetText("Tool version:"), c_Value);
   if (oq_OptionlContentMissing)
   {
      c_Value = C_GtGetText::h_GetText("unknown");
   }
   else
   {
      c_Value = this->mc_InterpretedFileInfo.c_FileInfo.c_ProjectName.c_str();
   }
   this->m_AddTableRow(C_GtGetText::h_GetText("Project name:"), c_Value);
   if (oq_OptionlContentMissing)
   {
      c_Value = C_GtGetText::h_GetText("unknown");
   }
   else
   {
      c_Value = this->mc_InterpretedFileInfo.c_FileInfo.c_ProjectVersion.c_str();
   }
   this->m_AddTableRow(C_GtGetText::h_GetText("Project version:"), c_Value);
   if (oq_OptionlContentMissing)
   {
      c_Value = C_GtGetText::h_GetText("unknown");
   }
   else
   {
      c_Value = this->mc_InterpretedFileInfo.c_FileInfo.c_UserComment.c_str();
   }
   this->m_AddTableRow(C_GtGetText::h_GetText("Comment:"), c_Value);

   this->mc_ComparisonHtml += "</table>";

   //Comparison
   this->mc_ComparisonHtml += "<h3>" + QString(C_GtGetText::h_GetText("Comparison result")) + "</h3>";
   this->mc_ComparisonHtml += "<p>" +
                              QString(C_GtGetText::h_GetText("Compare selected file with current SYSTEM DEFINITION.")) +
                              "</p>";
   this->mc_ComparisonHtml += "<table>";

   //Table headings
   this->mc_ComparisonHtml += "<tr>";

   //Description
   this->mc_ComparisonHtml += C_SyvUpParamSetFileInfo::mhc_StartHeadingTD;
   this->mc_ComparisonHtml += "<b>";
   this->mc_ComparisonHtml += C_GtGetText::h_GetText("Description");
   this->mc_ComparisonHtml += "</b>";
   this->mc_ComparisonHtml += "</td>";

   //File
   this->mc_ComparisonHtml += C_SyvUpParamSetFileInfo::mhc_ContinueHeadingTD;
   this->mc_ComparisonHtml += "<b>";
   this->mc_ComparisonHtml += C_GtGetText::h_GetText("File value");
   this->mc_ComparisonHtml += "</b>";
   this->mc_ComparisonHtml += "</td>";

   //SD
   this->mc_ComparisonHtml += C_SyvUpParamSetFileInfo::mhc_ContinueHeadingTD;
   this->mc_ComparisonHtml += "<b>";
   this->mc_ComparisonHtml += C_GtGetText::h_GetText("SYSTEM DEFINITION value");
   this->mc_ComparisonHtml += "</b>";
   this->mc_ComparisonHtml += "</td>";

   //Result
   this->mc_ComparisonHtml += C_SyvUpParamSetFileInfo::mhc_ContinueHeadingTD;
   this->mc_ComparisonHtml += "<b>";
   this->mc_ComparisonHtml += C_GtGetText::h_GetText("Comparison result");
   this->mc_ComparisonHtml += "</b>";
   this->mc_ComparisonHtml += "</td>";

   this->mc_ComparisonHtml += "</tr>";

   //Table content
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_ComparisonResults.size(); ++u32_ItItem)
   {
      const C_SyvUpParamSetFileInfoComparisonDescription & rc_Descritpion = this->mc_ComparisonResults[u32_ItItem];
      this->mc_ComparisonHtml += "<tr>";

      //Description
      this->mc_ComparisonHtml += C_SyvUpParamSetFileInfo::mhc_StartDataTD;
      for (uint32 u32_It = 0UL; u32_It < rc_Descritpion.u32_LayerNum; ++u32_It)
      {
         this->mc_ComparisonHtml += "   ";
      }
      this->mc_ComparisonHtml += rc_Descritpion.c_Description;
      this->mc_ComparisonHtml += "</td>";

      //File
      this->mc_ComparisonHtml += C_SyvUpParamSetFileInfo::mhc_ContinueDataTD;
      this->mc_ComparisonHtml += rc_Descritpion.c_FileValue;
      this->mc_ComparisonHtml += "</td>";

      //SD
      this->mc_ComparisonHtml += C_SyvUpParamSetFileInfo::mhc_ContinueDataTD;
      if (rc_Descritpion.e_ResultType == C_SyvUpParamSetFileInfoComparisonDescription::eRT_NOT_FOUND)
      {
         this->mc_ComparisonHtml += "<b>";
      }
      this->mc_ComparisonHtml += rc_Descritpion.c_SDValue;
      if (rc_Descritpion.e_ResultType == C_SyvUpParamSetFileInfoComparisonDescription::eRT_NOT_FOUND)
      {
         this->mc_ComparisonHtml += "</b>";
      }
      this->mc_ComparisonHtml += "</td>";

      //Result
      this->mc_ComparisonHtml += C_SyvUpParamSetFileInfo::mhc_ContinueDataTD;
      if (rc_Descritpion.e_ResultType == C_SyvUpParamSetFileInfoComparisonDescription::eRT_NO_MATCH)
      {
         this->mc_ComparisonHtml += "<b>";
      }
      this->mc_ComparisonHtml += rc_Descritpion.GetResultText();
      if (rc_Descritpion.e_ResultType == C_SyvUpParamSetFileInfoComparisonDescription::eRT_NO_MATCH)
      {
         this->mc_ComparisonHtml += "</b>";
      }
      this->mc_ComparisonHtml += "</td>";

      this->mc_ComparisonHtml += "</tr>";
   }
   this->mc_ComparisonHtml += "</table>";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Display no match found result

   \param[in] orc_Item        Item which was searched for
   \param[in] orc_Description Description for the searched item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpParamSetFileInfo::m_DisplayNoMatch(const QString & orc_Item, const QString & orc_Description,
                                               const uint32 ou32_LayerNum)
{
   C_SyvUpParamSetFileInfoComparisonDescription c_Description;

   //Comparison type
   c_Description.c_Description = orc_Description;
   c_Description.u32_LayerNum = ou32_LayerNum;

   //Value
   c_Description.c_FileValue = orc_Item;
   c_Description.c_SDValue = C_GtGetText::h_GetText("not found");

   //Result
   c_Description.e_ResultType = C_SyvUpParamSetFileInfoComparisonDescription::eRT_NOT_FOUND;

   //Add
   this->mc_ComparisonResults.push_back(c_Description);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compare two strings and display result

   \param[in] orc_Str1        First string to compare
   \param[in] orc_Str2        Second string to compare
   \param[in] orc_Description Description for the compared items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpParamSetFileInfo::m_CompareString(const QString & orc_StrFile, const QString & orc_StrSD,
                                              const QString & orc_Description, const uint32 ou32_LayerNum)
{
   C_SyvUpParamSetFileInfoComparisonDescription c_Description;

   //Result
   if (orc_StrFile.compare(orc_StrSD) == 0)
   {
      c_Description.e_ResultType = C_SyvUpParamSetFileInfoComparisonDescription::eRT_MATCH;
   }
   else
   {
      c_Description.e_ResultType = C_SyvUpParamSetFileInfoComparisonDescription::eRT_NO_MATCH;
   }

   //Comparison type and value
   c_Description.c_Description = orc_Description;
   c_Description.u32_LayerNum = ou32_LayerNum;

   //Value
   c_Description.c_FileValue = orc_StrFile;
   c_Description.c_SDValue = orc_StrSD;

   //Add
   this->mc_ComparisonResults.push_back(c_Description);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get formatted version string

   \param[in] ou32_VersionByte1 Version byte 1
   \param[in] ou32_VersionByte2 Version byte 2
   \param[in] ou32_VersionByte3 Version byte 3

   \return
   Formatted version string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpParamSetFileInfo::mh_GetVersionString(const uint32 ou32_VersionByte1, const uint32 ou32_VersionByte2,
                                                     const uint32 ou32_VersionByte3)
{
   const QString c_Retval =
      QString("v%1.%2r%3").arg(ou32_VersionByte1, 2, 10, QChar('0')).arg(ou32_VersionByte2, 2, 10, QChar('0')).arg(
         ou32_VersionByte3, 2, 10, QChar('0'));

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add table row entry

   \param[in] orc_Heading Entry heading
   \param[in] orc_Content Entry content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpParamSetFileInfo::m_AddTableRow(const QString & orc_Heading, const QString & orc_Content)
{
   this->mc_ComparisonHtml += "<tr>";
   this->mc_ComparisonHtml += C_SyvUpParamSetFileInfo::mhc_StartDataTD;
   this->mc_ComparisonHtml += orc_Heading;
   this->mc_ComparisonHtml += "</td>";
   this->mc_ComparisonHtml += C_SyvUpParamSetFileInfo::mhc_ContinueDataTD;
   this->mc_ComparisonHtml += orc_Content;
   this->mc_ComparisonHtml += "</td>";
   this->mc_ComparisonHtml += "</tr>";
}
