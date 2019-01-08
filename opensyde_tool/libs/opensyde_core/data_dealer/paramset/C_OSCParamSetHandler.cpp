//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Handler class for parameter set file operations (implementation)

   Handler class for parameter set file operations

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.10.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "TGLUtils.h"
#include "TGLFile.h"
#include "stwerrors.h"
#include "C_OSCChecksummedXML.h"
#include "C_OSCParamSetHandler.h"
#include "C_OSCParamSetRawNodeFiler.h"
#include "C_OSCParamSetInterpretedNodeFiler.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */
C_OSCParamSetHandler C_OSCParamSetHandler::mhc_Singleton;

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Create clean file from internally stored content without adding a CRC

   \param[in] orc_FilePath Parameter file path
   \param[in] oq_InterpretedDataOnly   Flag to load only the interpreted data and not the raw data

   \return
   C_NO_ERR   data saved
   C_RANGE    file already exists
   C_CONFIG   Internal data invalid
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)

   \created     24.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCParamSetHandler::CreateCleanFileWithoutCRC(const C_SCLString & orc_FilePath,
                                                       const bool oq_InterpretedDataOnly)
{
   sint32 s32_Return = C_NO_ERR;

   if (TGL_FileExists(orc_FilePath.c_str()) == false)
   {
      if ((oq_InterpretedDataOnly == true) ||
          (this->mc_RawNodes.size() == this->mc_InterpretedNodes.size()))
      {
         C_OSCXMLParser c_XMLParser;
         tgl_assert(c_XMLParser.CreateAndSelectNodeChild("opensyde-parameter-sets") == "opensyde-parameter-sets");
         C_OSCParamSetFilerBase::h_SaveFileVersion(c_XMLParser);
         tgl_assert(c_XMLParser.CreateAndSelectNodeChild("nodes") == "nodes");
         for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_InterpretedNodes.size(); ++u32_ItNode)
         {
            tgl_assert(c_XMLParser.CreateAndSelectNodeChild("node") == "node");
            if (oq_InterpretedDataOnly == false)
            {
               C_OSCParamSetRawNodeFiler::h_SaveRawNode(this->mc_RawNodes[u32_ItNode], c_XMLParser);
            }
            C_OSCParamSetInterpretedNodeFiler::h_SaveInterpretedNode(this->mc_InterpretedNodes[u32_ItNode],
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
         s32_Return = C_CONFIG;
      }
   }
   else
   {
      s32_Return = C_RANGE;
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Read file and update internally stored content (cleared at start)

   \param[in] orc_FilePath             Parameter file path
   \param[in] oq_IgnoreCrc             Flag to ignore CRC if set to true
   \param[in] oq_InterpretedDataOnly   Flag to load only the interpreted data and not the raw data

   \return
   C_NO_ERR   data read
   C_RD_WR    specified file does not exist
              specified file is present but structure is invalid (e.g. invalid XML file; not checksum found)
   C_CHECKSUM specified file is present but checksum is invalid
   C_CONFIG   file does not contain essential information

   \created     24.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCParamSetHandler::ReadFile(const C_SCLString & orc_FilePath, const bool oq_IgnoreCrc,
                                      const bool oq_InterpretedDataOnly)
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
               s32_Retval = this->m_LoadNodes(*pc_Parser, oq_InterpretedDataOnly);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Update/add CRC for provided file

   \param[in] orc_FilePath Parameter file path

   \return
   C_NO_ERR CRC updated
   C_CONFIG Unexpected XML format
   C_RD_WR  Error accessing file system
   C_RANGE  File does not exist

   \created     24.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCParamSetHandler::UpdateCRCForFile(const C_SCLString & orc_FilePath) const
{
   return C_OSCParamSetFilerBase::h_AddCRC(orc_FilePath);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clear internally stored content

   \created     24.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCParamSetHandler::ClearContent(void)
{
   this->mc_RawNodes.clear();
   this->mc_InterpretedNodes.clear();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set raw data for node (Node name used as ID)

   \param[in] orc_Content Raw data for node (Node name used as ID)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     24.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCParamSetHandler::SetRawDataForNode(const C_OSCParamSetRawNode & orc_Content)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set interpreted data for node (Node name used as ID)

   \param[in] orc_Content Interpreted data for node (Node name used as ID)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     24.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCParamSetHandler::SetInterpretedDataForNode(const C_OSCParamSetInterpretedNode & orc_Content)
{
   sint32 s32_Retval = C_NO_ERR;

   for (uint32 u32_ItInterpretedNode = 0; u32_ItInterpretedNode < this->mc_InterpretedNodes.size();
        ++u32_ItInterpretedNode)
   {
      const C_OSCParamSetInterpretedNode & rc_CurInterpretedNode = this->mc_InterpretedNodes[u32_ItInterpretedNode];
      if (rc_CurInterpretedNode.c_Name == orc_Content.c_Name)
      {
         s32_Retval = C_RANGE;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      this->mc_InterpretedNodes.push_back(orc_Content);
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get raw node data for specified node

   \param[in] orc_NodeName Node name to look for

   \return
   NULL Raw node data not found
   Else Valid raw node data

   \created     24.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get interpreted node data for specified node

   \param[in] orc_NodeName Node name to look for

   \return
   NULL Interpreted node data not found
   Else Valid interpreted node data

   \created     24.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCParamSetInterpretedNode * C_OSCParamSetHandler::GetInterpretedDataForNode(const C_SCLString & orc_NodeName)
const
{
   const C_OSCParamSetInterpretedNode * pc_Retval = NULL;

   for (uint32 u32_ItInterpretedNode = 0; u32_ItInterpretedNode < this->mc_InterpretedNodes.size();
        ++u32_ItInterpretedNode)
   {
      const C_OSCParamSetInterpretedNode & rc_CurInterpretedNode = this->mc_InterpretedNodes[u32_ItInterpretedNode];
      if (rc_CurInterpretedNode.c_Name == orc_NodeName)
      {
         pc_Retval = &this->mc_InterpretedNodes[u32_ItInterpretedNode];
      }
   }

   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get interpreted data

   \return
   Pointer to vector with interpreted data

   \created     16.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
const std::vector<C_OSCParamSetInterpretedNode> * C_OSCParamSetHandler::GetInterpretedData(void) const
{
   return &this->mc_InterpretedNodes;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get singleton instance

   \return
   Singleton instance

   \created     24.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCParamSetHandler & C_OSCParamSetHandler::h_GetInstance(void)
{
   return mhc_Singleton;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     24.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCParamSetHandler::C_OSCParamSetHandler(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load node data

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "opensyde-parameter-sets"
   post-condition: the passed XML parser has the active node set to the same "opensyde-parameter-sets"

   \param[in,out] orc_XMLParser            XML with specified node active
   \param[in]     oq_InterpretedDataOnly   Flag to load only the interpreted data and not the raw data

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete

   \created     24.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCParamSetHandler::m_LoadNodes(C_OSCXMLParser & orc_XMLParser, const bool oq_InterpretedDataOnly)
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
               s32_Retval = C_OSCParamSetRawNodeFiler::h_LoadRawNode(c_RawItem, orc_XMLParser);
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
               s32_Retval = C_OSCParamSetInterpretedNodeFiler::h_LoadInterpretedNode(c_InterpretedItem, orc_XMLParser);
               if (s32_Retval == C_NO_ERR)
               {
                  this->mc_InterpretedNodes.push_back(c_InterpretedItem);
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
