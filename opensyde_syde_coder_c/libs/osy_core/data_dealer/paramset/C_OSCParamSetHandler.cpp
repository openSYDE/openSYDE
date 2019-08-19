//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for parameter set file operations (implementation)

   Handler class for parameter set file operations

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "TGLUtils.h"
#include "TGLFile.h"
#include "stwerrors.h"
#include "C_OSCChecksummedXML.h"
#include "C_OSCParamSetHandler.h"
#include "C_OSCParamSetRawNodeFiler.h"
#include "C_OSCParamSetInterpretedNodeFiler.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_tgl;
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
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCParamSetHandler::C_OSCParamSetHandler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create clean file from internally stored content without adding a CRC

   \param[in] orc_FilePath Parameter file path
   \param[in] oq_InterpretedDataOnly   Flag to load only the interpreted data and not the raw data

   \return
   C_NO_ERR   data saved
   C_RANGE    file already exists
   C_CONFIG   Internal data invalid
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetHandler::CreateCleanFileWithoutCRC(const C_SCLString & orc_FilePath,
                                                       const bool oq_InterpretedDataOnly)
{
   sint32 s32_Return = C_NO_ERR;

   if (TGL_FileExists(orc_FilePath.c_str()) == false)
   {
      if ((oq_InterpretedDataOnly == true) ||
          (this->mc_RawNodes.size() == this->mc_Data.c_InterpretedNodes.size()))
      {
         C_OSCXMLParser c_XMLParser;
         tgl_assert(c_XMLParser.CreateAndSelectNodeChild("opensyde-parameter-sets") == "opensyde-parameter-sets");
         C_OSCParamSetFilerBase::h_SaveFileVersion(c_XMLParser);
         C_OSCParamSetFilerBase::h_SaveFileInfo(c_XMLParser, this->mc_Data.c_FileInfo);
         tgl_assert(c_XMLParser.CreateAndSelectNodeChild("nodes") == "nodes");
         for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_Data.c_InterpretedNodes.size(); ++u32_ItNode)
         {
            tgl_assert(c_XMLParser.CreateAndSelectNodeChild("node") == "node");
            if (oq_InterpretedDataOnly == false)
            {
               C_OSCParamSetRawNodeFiler::h_SaveRawNode(this->mc_RawNodes[u32_ItNode], c_XMLParser);
            }
            C_OSCParamSetInterpretedNodeFiler::h_SaveInterpretedNode(this->mc_Data.c_InterpretedNodes[u32_ItNode],
                                                                     c_XMLParser);
            //Return
            tgl_assert(c_XMLParser.SelectNodeParent() == "nodes");
         }

         s32_Return = c_XMLParser.SaveToFile(orc_FilePath);
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
sint32 C_OSCParamSetHandler::ReadFile(const C_SCLString & orc_FilePath, const bool oq_IgnoreCrc,
                                      const bool oq_InterpretedDataOnly, uint16 * const opu16_FileCrc,
                                      bool * const opq_MissingOptionalContent)
{
   sint32 s32_Retval = C_NO_ERR;

   this->ClearContent();
   if (TGL_FileExists(orc_FilePath) == true)
   {
      C_OSCXMLParser * pc_Parser;
      if (oq_IgnoreCrc == true)
      {
         pc_Parser = new C_OSCXMLParser();
      }
      else
      {
         pc_Parser = new C_OSCChecksummedXML();
      }

      s32_Retval = pc_Parser->LoadFromFile(orc_FilePath);
      if (s32_Retval == C_NO_ERR)
      {
         if (pc_Parser->SelectRoot() == "opensyde-parameter-sets")
         {
            s32_Retval = C_OSCParamSetRawNodeFiler::h_CheckFileVersion(*pc_Parser);
            if (s32_Retval == C_NO_ERR)
            {
               bool q_MissingOptionalContent = false;
               C_OSCParamSetRawNodeFiler::h_LoadFileInfo(*pc_Parser, this->mc_Data.c_FileInfo,
                                                         q_MissingOptionalContent);
               s32_Retval = this->m_LoadNodes(*pc_Parser, oq_InterpretedDataOnly, q_MissingOptionalContent);
               if ((opu16_FileCrc != NULL) && (s32_Retval == C_NO_ERR))
               {
                  *opu16_FileCrc = static_cast<uint16>(pc_Parser->GetAttributeUint32("file_crc"));
               }
               if (opq_MissingOptionalContent != NULL)
               {
                  *opq_MissingOptionalContent = q_MissingOptionalContent;
               }
            }
         }
         else
         {
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
sint32 C_OSCParamSetHandler::UpdateCRCForFile(const C_SCLString & orc_FilePath) const
{
   return C_OSCParamSetFilerBase::h_AddCRC(orc_FilePath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear internally stored content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetHandler::ClearContent(void)
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
sint32 C_OSCParamSetHandler::AddRawDataForNode(const C_OSCParamSetRawNode & orc_Content)
{
   sint32 s32_Retval = C_NO_ERR;

   for (uint32 u32_ItRawNode = 0; u32_ItRawNode < this->mc_RawNodes.size(); ++u32_ItRawNode)
   {
      const C_OSCParamSetRawNode & rc_CurRawNode = this->mc_RawNodes[u32_ItRawNode];
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
void C_OSCParamSetHandler::AddInterpretedFileData(const C_OSCParamSetInterpretedFileInfoData & orc_FileInfo)
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
sint32 C_OSCParamSetHandler::AddInterpretedDataForNode(const C_OSCParamSetInterpretedNode & orc_Content)
{
   sint32 s32_Retval = C_NO_ERR;

   for (uint32 u32_ItInterpretedNode = 0; u32_ItInterpretedNode < this->mc_Data.c_InterpretedNodes.size();
        ++u32_ItInterpretedNode)
   {
      const C_OSCParamSetInterpretedNode & rc_CurInterpretedNode =
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
const C_OSCParamSetRawNode * C_OSCParamSetHandler::GetRawDataForNode(const C_SCLString & orc_NodeName) const
{
   const C_OSCParamSetRawNode * pc_Retval = NULL;

   for (uint32 u32_ItRawNode = 0; u32_ItRawNode < this->mc_RawNodes.size(); ++u32_ItRawNode)
   {
      const C_OSCParamSetRawNode & rc_CurRawNode = this->mc_RawNodes[u32_ItRawNode];
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
const C_OSCParamSetInterpretedData & C_OSCParamSetHandler::GetInterpretedData(void) const
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
uint32 C_OSCParamSetHandler::GetNumberOfNodes(void) const
{
   return this->mc_RawNodes.size();
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
const C_OSCParamSetRawNode * C_OSCParamSetHandler::GetRawDataForNode(const uint32 ou32_NodeIndex) const
{
   const C_OSCParamSetRawNode * pc_Result = NULL;

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

   \param[in,out] orc_XMLParser              XML with specified node active
   \param[in]     oq_InterpretedDataOnly     Flag to load only the interpreted data and not the raw data
   \param[in,out] orq_MissingOptionalContent Flag for indication of optional content missing
                                             Warning: flag is never set to false if optional content is present

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetHandler::m_LoadNodes(C_OSCXMLParser & orc_XMLParser, const bool oq_InterpretedDataOnly,
                                         bool & orq_MissingOptionalContent)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("nodes") == "nodes")
   {
      C_SCLString c_SelectedNode = orc_XMLParser.SelectNodeChild("node");

      if (c_SelectedNode == "node")
      {
         do
         {
            C_OSCParamSetInterpretedNode c_InterpretedItem;
            C_OSCParamSetRawNode c_RawItem;
            //Content
            if (oq_InterpretedDataOnly == false)
            {
               s32_Retval = C_OSCParamSetRawNodeFiler::h_LoadRawNode(c_RawItem, orc_XMLParser,
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
               s32_Retval = C_OSCParamSetInterpretedNodeFiler::h_LoadInterpretedNode(c_InterpretedItem, orc_XMLParser,
                                                                                     orq_MissingOptionalContent);
               if (s32_Retval == C_NO_ERR)
               {
                  this->mc_Data.c_InterpretedNodes.push_back(c_InterpretedItem);
               }
            }

            //Next
            c_SelectedNode = orc_XMLParser.SelectNodeNext("node");
         }
         while ((c_SelectedNode == "node") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "nodes");
      }
      else
      {
         osc_write_log_error("Loading Dataset data", "Could not find \"nodes\".\"node\" node.");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-parameter-sets");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}
