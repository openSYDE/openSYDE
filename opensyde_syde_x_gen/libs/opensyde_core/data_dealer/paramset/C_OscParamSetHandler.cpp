//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for parameter set file operations (implementation)

   Handler class for parameter set file operations

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "TglFile.hpp"
#include "stwerrors.hpp"
#include "C_OscChecksummedXml.hpp"
#include "C_OscParamSetHandler.hpp"
#include "C_OscParamSetRawNodeFiler.hpp"
#include "C_OscParamSetInterpretedNodeFiler.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::tgl;

using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscParamSetHandler::C_OscParamSetHandler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create clean file from internally stored content without adding a CRC

   \param[in] orc_FilePath Parameter file path
   \param[in] oq_InterpretedDataOnly   Flag to load only the interpreted data and not the raw data

   \return
   C_NO_ERR   data saved
   C_RANGE    file already exists
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetHandler::CreateCleanFileWithoutCrc(const C_SclString & orc_FilePath,
                                                        const bool oq_InterpretedDataOnly) const
{
   int32_t s32_Return = C_NO_ERR;

   if (TglFileExists(orc_FilePath.c_str()) == false)
   {
      if ((oq_InterpretedDataOnly == true) ||
          (this->mc_RawNodes.size() == this->mc_Data.c_InterpretedNodes.size()))
      {
         C_OscXmlParser c_XmlParser;
         tgl_assert(c_XmlParser.CreateAndSelectNodeChild("opensyde-parameter-sets") == "opensyde-parameter-sets");
         C_OscParamSetFilerBase::h_SaveFileVersion(c_XmlParser);
         C_OscParamSetFilerBase::h_SaveFileInfo(c_XmlParser, this->mc_Data.c_FileInfo);
         tgl_assert(c_XmlParser.CreateAndSelectNodeChild("nodes") == "nodes");
         for (uint32_t u32_ItNode = 0; u32_ItNode < this->mc_Data.c_InterpretedNodes.size(); ++u32_ItNode)
         {
            tgl_assert(c_XmlParser.CreateAndSelectNodeChild("node") == "node");
            if (oq_InterpretedDataOnly == false)
            {
               C_OscParamSetRawNodeFiler::h_SaveRawNode(this->mc_RawNodes[u32_ItNode], c_XmlParser);
            }
            C_OscParamSetInterpretedNodeFiler::h_SaveInterpretedNode(this->mc_Data.c_InterpretedNodes[u32_ItNode],
                                                                     c_XmlParser);
            //Return
            tgl_assert(c_XmlParser.SelectNodeParent() == "nodes");
         }

         s32_Return = c_XmlParser.SaveToFile(orc_FilePath);
         if (s32_Return != C_NO_ERR)
         {
            s32_Return = C_RD_WR;
         }
      }
      else
      {
         tgl_assert(false);
      }
   }
   else
   {
      s32_Return = C_RANGE;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read file and update internally stored content (cleared at start)

   \param[in]     orc_FilePath               Parameter file path
   \param[in]     oq_IgnoreCrc               Flag to ignore CRC if set to true
   \param[in]     oq_InterpretedDataOnly     Flag to load only the interpreted data and not the raw data
   \param[in,out] opu16_FileCrc              Optional storage for read file CRC
                                             (only set if no err and oq_IgnoreCrc is false)
   \param[in,out] opq_MissingOptionalContent Optional flag for indication of optional content missing
                                             Warning: only valid if C_NO_ERR

   \return
   C_NO_ERR   data read
   C_RD_WR    specified file does not exist
              specified file is present but structure is invalid (e.g. invalid XML file; not checksum found)
   C_CHECKSUM specified file is present but checksum is invalid
   C_CONFIG   file does not contain essential information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetHandler::ReadFile(const C_SclString & orc_FilePath, const bool oq_IgnoreCrc,
                                       const bool oq_InterpretedDataOnly, uint16_t * const opu16_FileCrc,
                                       bool * const opq_MissingOptionalContent)
{
   int32_t s32_Retval = C_NO_ERR;

   this->ClearContent();
   if (TglFileExists(orc_FilePath) == true)
   {
      C_OscXmlParser * pc_Parser;
      if (oq_IgnoreCrc == true)
      {
         pc_Parser = new C_OscXmlParser();
      }
      else
      {
         pc_Parser = new C_OscChecksummedXml();
      }

      s32_Retval = pc_Parser->LoadFromFile(orc_FilePath);
      if (s32_Retval == C_NO_ERR)
      {
         if (pc_Parser->SelectRoot() == "opensyde-parameter-sets")
         {
            s32_Retval = C_OscParamSetRawNodeFiler::h_CheckFileVersion(*pc_Parser);
            if (s32_Retval == C_NO_ERR)
            {
               bool q_MissingOptionalContent = false;
               C_OscParamSetRawNodeFiler::h_LoadFileInfo(*pc_Parser, this->mc_Data.c_FileInfo,
                                                         q_MissingOptionalContent);
               s32_Retval = this->m_LoadNodes(*pc_Parser, oq_InterpretedDataOnly, q_MissingOptionalContent);
               if ((opu16_FileCrc != NULL) && (s32_Retval == C_NO_ERR))
               {
                  *opu16_FileCrc = static_cast<uint16_t>(pc_Parser->GetAttributeUint32("file_crc"));
               }
               if (opq_MissingOptionalContent != NULL)
               {
                  *opq_MissingOptionalContent = q_MissingOptionalContent;
               }
            }
         }
         else
         {
            osc_write_log_error("Loading Dataset data", "Could not find root element \"opensyde-parameter-sets\".");
            s32_Retval = C_RD_WR;
         }
      }
      else
      {
         if (s32_Retval != C_CHECKSUM)
         {
            s32_Retval = C_RD_WR;
         }
      }
      delete pc_Parser;
   }
   else
   {
      s32_Retval = C_RD_WR;
   }
   if (s32_Retval != C_NO_ERR)
   {
      const C_SclString c_Text = "Could not load file \"" + orc_FilePath + "\". Error code: " +
                                 C_SclString::IntToStr(s32_Retval);
      osc_write_log_error("Loading Dataset data", c_Text);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update/add CRC for provided file

   \param[in] orc_FilePath Parameter file path

   \return
   C_NO_ERR CRC updated
   C_CONFIG Unexpected XML format
   C_RD_WR  Error accessing file system
   C_RANGE  File does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetHandler::h_UpdateCrcForFile(const C_SclString & orc_FilePath)
{
   return C_OscParamSetFilerBase::h_AddCrc(orc_FilePath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear internally stored content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetHandler::ClearContent(void)
{
   this->mc_RawNodes.clear();
   this->mc_Data.Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add raw data for node (Node name used as ID)

   \param[in] orc_Content Raw data for node (Node name used as ID)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetHandler::AddRawDataForNode(const C_OscParamSetRawNode & orc_Content)
{
   int32_t s32_Retval = C_NO_ERR;

   for (uint32_t u32_ItRawNode = 0; u32_ItRawNode < this->mc_RawNodes.size(); ++u32_ItRawNode)
   {
      const C_OscParamSetRawNode & rc_CurRawNode = this->mc_RawNodes[u32_ItRawNode];
      if (rc_CurRawNode.c_Name == orc_Content.c_Name)
      {
         s32_Retval = C_RANGE;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      this->mc_RawNodes.push_back(orc_Content);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add some general information for one param set file

   \param[in] orc_FileInfo Optional general file information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetHandler::AddInterpretedFileData(const C_OscParamSetInterpretedFileInfoData & orc_FileInfo)
{
   this->mc_Data.AddInterpretedFileData(orc_FileInfo);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add interpreted data for node (Node name used as ID)

   \param[in] orc_Content Interpreted data for node (Node name used as ID)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetHandler::AddInterpretedDataForNode(const C_OscParamSetInterpretedNode & orc_Content)
{
   int32_t s32_Retval = C_NO_ERR;

   for (uint32_t u32_ItInterpretedNode = 0; u32_ItInterpretedNode < this->mc_Data.c_InterpretedNodes.size();
        ++u32_ItInterpretedNode)
   {
      const C_OscParamSetInterpretedNode & rc_CurInterpretedNode =
         this->mc_Data.c_InterpretedNodes[u32_ItInterpretedNode];
      if (rc_CurInterpretedNode.c_Name == orc_Content.c_Name)
      {
         s32_Retval = C_RANGE;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      this->mc_Data.c_InterpretedNodes.push_back(orc_Content);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get raw node data for specified node

   \param[in] orc_NodeName Node name to look for

   \return
   NULL Raw node data not found
   Else Valid raw node data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscParamSetRawNode * C_OscParamSetHandler::GetRawDataForNode(const C_SclString & orc_NodeName) const
{
   const C_OscParamSetRawNode * pc_Retval = NULL;

   for (uint32_t u32_ItRawNode = 0; u32_ItRawNode < this->mc_RawNodes.size(); ++u32_ItRawNode)
   {
      const C_OscParamSetRawNode & rc_CurRawNode = this->mc_RawNodes[u32_ItRawNode];
      if (rc_CurRawNode.c_Name == orc_NodeName)
      {
         pc_Retval = &this->mc_RawNodes[u32_ItRawNode];
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get interpreted data

   \return
   Pointer to vector with interpreted data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscParamSetInterpretedData & C_OscParamSetHandler::GetInterpretedData(void) const
{
   return this->mc_Data;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number of nodes

   Get number of nodes that the file contains data for

   \return
   Number of nodes
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscParamSetHandler::GetNumberOfNodes(void) const
{
   return static_cast<uint32_t>(this->mc_RawNodes.size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get raw data for one node

   Get raw data for specified node.

   \param[in]  ou32_NodeIndex   index of node

   \return
   NULL  Raw node data not found
   Else  Valid raw node data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscParamSetRawNode * C_OscParamSetHandler::GetRawDataForNode(const uint32_t ou32_NodeIndex) const
{
   const C_OscParamSetRawNode * pc_Result = NULL;

   if (ou32_NodeIndex < this->mc_RawNodes.size())
   {
      pc_Result = &this->mc_RawNodes[ou32_NodeIndex];
   }
   return pc_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node data

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "opensyde-parameter-sets"
   post-condition: the passed XML parser has the active node set to the same "opensyde-parameter-sets"

   \param[in,out] orc_XmlParser              XML with specified node active
   \param[in]     oq_InterpretedDataOnly     Flag to load only the interpreted data and not the raw data
   \param[in,out] orq_MissingOptionalContent Flag for indication of optional content missing
                                             Warning: flag is never set to false if optional content is present

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetHandler::m_LoadNodes(C_OscXmlParser & orc_XmlParser, const bool oq_InterpretedDataOnly,
                                          bool & orq_MissingOptionalContent)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("nodes") == "nodes")
   {
      C_SclString c_SelectedNode = orc_XmlParser.SelectNodeChild("node");

      if (c_SelectedNode == "node")
      {
         do
         {
            C_OscParamSetInterpretedNode c_InterpretedItem;
            C_OscParamSetRawNode c_RawItem;
            //Content
            if (oq_InterpretedDataOnly == false)
            {
               s32_Retval = C_OscParamSetRawNodeFiler::h_LoadRawNode(c_RawItem, orc_XmlParser,
                                                                     orq_MissingOptionalContent);
               if (s32_Retval == C_NO_ERR)
               {
                  this->mc_RawNodes.push_back(c_RawItem);
               }
            }
            else
            {
               // Do not load the raw data
               s32_Retval = C_NO_ERR;
            }

            if (s32_Retval == C_NO_ERR)
            {
               s32_Retval = C_OscParamSetInterpretedNodeFiler::h_LoadInterpretedNode(c_InterpretedItem, orc_XmlParser,
                                                                                     orq_MissingOptionalContent);
               if (s32_Retval == C_NO_ERR)
               {
                  this->mc_Data.c_InterpretedNodes.push_back(c_InterpretedItem);
               }
            }

            //Next
            c_SelectedNode = orc_XmlParser.SelectNodeNext("node");
         }
         while ((c_SelectedNode == "node") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "nodes");
      }
      else
      {
         osc_write_log_error("Loading Dataset data", "Could not find \"nodes\".\"node\" node.");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-parameter-sets");
   }
   else
   {
      osc_write_log_error("Loading Dataset data", "Could not find \"nodes\".");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}
