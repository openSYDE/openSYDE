//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Parameter set file reader/writer base (implementation)

   Parameter set file reader/writer base

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "TGLFile.h"
#include "TGLUtils.h"
#include "C_OSCChecksummedXML.h"
#include "C_OSCParamSetFilerBase.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
stw_types::uint16 C_OSCParamSetFilerBase::mhu16_FileVersion = 1;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add CRC to specified file

   \param[in] orc_Path File path

   \return
   C_NO_ERR CRC updated
   C_CONFIG Unexpected XML format
   C_RD_WR  Error accessing file system
   C_RANGE  File does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetFilerBase::h_AddCRC(const C_SCLString & orc_Path)
{
   sint32 s32_Return = C_NO_ERR;

   if (TGL_FileExists(orc_Path) == true)
   {
      C_OSCChecksummedXML c_XMLParser;
      s32_Return = c_XMLParser.LoadFromFile(orc_Path);
      //ignore missing and incorrect CRC; we want to set it
      if ((s32_Return == C_NO_ERR) || (s32_Return == C_CHECKSUM) || (s32_Return == C_RD_WR))
      {
         //do we have the correct type of file ?
         if (c_XMLParser.SelectRoot() != "opensyde-parameter-sets")
         {
            s32_Return = C_CONFIG;
         }
         else
         {
            s32_Return = c_XMLParser.SaveToFile(orc_Path);
         }
      }
      if (s32_Return == C_NOACT)
      {
         s32_Return = C_RD_WR;
      }
   }
   else
   {
      s32_Return = C_RANGE;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check parameter set file version

   Check parameter set file version from XML file
   pre-condition: the passed XML parser has the active node set to "opensyde-parameter-sets"
   post-condition: the passed XML parser has the active node set to the same "opensyde-parameter-sets"

   \param[in,out] orc_XMLParser XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetFilerBase::h_CheckFileVersion(C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("file-version") == "file-version")
   {
      //nothing to do with the file version yet ...
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-parameter-sets");
   }
   else
   {
      osc_write_log_error("Loading Dataset data", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save parameter set file version

   Save parameter set file version to XML file
   pre-condition: the passed XML parser has the active node set to "opensyde-parameter-sets"
   post-condition: the passed XML parser has the active node set to the same "opensyde-parameter-sets"

   \param[in,out] orc_XMLParser XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetFilerBase::h_SaveFileVersion(C_OSCXMLParserBase & orc_XMLParser)
{
   if (orc_XMLParser.SelectNodeChild("file-version") != "file-version")
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("file-version") == "file-version");
   }
   orc_XMLParser.SetNodeContent("0x" + C_SCLString::IntToHex(C_OSCParamSetFilerBase::mhu16_FileVersion, 4));
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-parameter-sets");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save parameter set file info

   Save parameter set file info to XML file
   pre-condition: the passed XML parser has the active node set to "opensyde-parameter-sets"
   post-condition: the passed XML parser has the active node set to the same "opensyde-parameter-sets"

   \param[in,out] orc_XMLParser XML with specified node active
   \param[in]     orc_FileInfo  File info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetFilerBase::h_SaveFileInfo(C_OSCXMLParserBase & orc_XMLParser,
                                            const C_OSCParamSetInterpretedFileInfoData & orc_FileInfo)
{
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("file-info") == "file-info");
   orc_XMLParser.CreateNodeChild("datetime", orc_FileInfo.c_DateTime);
   orc_XMLParser.CreateNodeChild("creator", orc_FileInfo.c_Creator);
   orc_XMLParser.CreateNodeChild("toolname", orc_FileInfo.c_ToolName);
   orc_XMLParser.CreateNodeChild("toolversion", orc_FileInfo.c_ToolVersion);
   orc_XMLParser.CreateNodeChild("projectname", orc_FileInfo.c_ProjectName);
   orc_XMLParser.CreateNodeChild("projectversion", orc_FileInfo.c_ProjectVersion);
   orc_XMLParser.CreateNodeChild("comment", orc_FileInfo.c_UserComment);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-parameter-sets");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load parameter set file info

   Load parameter set file info from XML file
   pre-condition: the passed XML parser has the active node set to "opensyde-parameter-sets"
   post-condition: the passed XML parser has the active node set to the same "opensyde-parameter-sets"

   \param[in,out] orc_XMLParser              XML with specified node active
   \param[out]    orc_FileInfo               Read file info
   \param[in,out] orq_MissingOptionalContent Flag for indication of optional content missing
                                             Warning: flag is never set to false if optional content is present
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetFilerBase::h_LoadFileInfo(C_OSCXMLParserBase & orc_XMLParser,
                                            C_OSCParamSetInterpretedFileInfoData & orc_FileInfo,
                                            bool & orq_MissingOptionalContent)
{
   //Default
   orc_FileInfo.Clear();
   //Read and overwrite if available
   if (orc_XMLParser.SelectNodeChild("file-info") == "file-info")
   {
      if (orc_XMLParser.SelectNodeChild("datetime") == "datetime")
      {
         orc_FileInfo.c_DateTime = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "file-info");
      }
      else
      {
         orq_MissingOptionalContent = true;
      }
      if (orc_XMLParser.SelectNodeChild("creator") == "creator")
      {
         orc_FileInfo.c_Creator = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "file-info");
      }
      else
      {
         orq_MissingOptionalContent = true;
      }
      if (orc_XMLParser.SelectNodeChild("toolname") == "toolname")
      {
         orc_FileInfo.c_ToolName = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "file-info");
      }
      else
      {
         orq_MissingOptionalContent = true;
      }
      if (orc_XMLParser.SelectNodeChild("toolversion") == "toolversion")
      {
         orc_FileInfo.c_ToolVersion = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "file-info");
      }
      else
      {
         orq_MissingOptionalContent = true;
      }
      if (orc_XMLParser.SelectNodeChild("projectname") == "projectname")
      {
         orc_FileInfo.c_ProjectName = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "file-info");
      }
      else
      {
         orq_MissingOptionalContent = true;
      }
      if (orc_XMLParser.SelectNodeChild("projectversion") == "projectversion")
      {
         orc_FileInfo.c_ProjectVersion = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "file-info");
      }
      else
      {
         orq_MissingOptionalContent = true;
      }
      if (orc_XMLParser.SelectNodeChild("comment") == "comment")
      {
         orc_FileInfo.c_UserComment = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "file-info");
      }
      else
      {
         orq_MissingOptionalContent = true;
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-parameter-sets");
   }
   else
   {
      orq_MissingOptionalContent = true;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCParamSetFilerBase::C_OSCParamSetFilerBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load parameter set node name

   Load parameter set node name data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]    orc_Name      node name
   \param[in,out] orc_XMLParser XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetFilerBase::mh_LoadNodeName(stw_scl::C_SCLString & orc_Name, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("name") == "name")
   {
      orc_Name = orc_XMLParser.GetNodeContent();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
   }
   else
   {
      osc_write_log_error("Loading Dataset data", "Could not find \"node\".\"name\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save parameter set node name

   Save parameter set node name to XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[in]     orc_Name      data storage
   \param[in,out] orc_XMLParser XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetFilerBase::mh_SaveNodeName(const stw_scl::C_SCLString & orc_Name, C_OSCXMLParserBase & orc_XMLParser)
{
   if (orc_XMLParser.SelectNodeChild("name") == "name")
   {
      orc_XMLParser.SetNodeContent(orc_Name);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
   }
   else
   {
      orc_XMLParser.CreateNodeChild("name", orc_Name);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load parameter set data pool infos

   Load parameter set data pool infos data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]    orc_DataPoolInfos          data storage
   \param[in,out] orc_XMLParser              XML with specified node active
   \param[in,out] orq_MissingOptionalContent Flag for indication of optional content missing
                                             Warning: flag is never set to false if optional content is present

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetFilerBase::mh_LoadDataPoolInfos(std::vector<C_OSCParamSetDataPoolInfo> & orc_DataPoolInfos,
                                                    C_OSCXMLParserBase & orc_XMLParser,
                                                    bool & orq_MissingOptionalContent)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_DataPoolInfos.clear();
   if (orc_XMLParser.SelectNodeChild("datapools") == "datapools")
   {
      C_SCLString c_SelectedNode = orc_XMLParser.SelectNodeChild("datapool");

      if (c_SelectedNode == "datapool")
      {
         do
         {
            C_OSCParamSetDataPoolInfo c_Item;
            s32_Retval = C_OSCParamSetFilerBase::mh_LoadDataPoolInfo(c_Item, orc_XMLParser, orq_MissingOptionalContent);
            if (s32_Retval == C_NO_ERR)
            {
               orc_DataPoolInfos.push_back(c_Item);
            }

            //Next
            c_SelectedNode = orc_XMLParser.SelectNodeNext("datapool");
         }
         while ((c_SelectedNode == "datapool") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "datapools");
      }
      else
      {
         osc_write_log_error("Loading Dataset data", "Could not find \"datapools\".\"datapool\" node.");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
   }
   else
   {
      osc_write_log_error("Loading Dataset data", "Could not find \"datapools\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save parameter set data pool information

   Save parameter set data pool information to XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[in]     orc_DataPoolInfos data pool information to store
   \param[in,out] orc_XMLParser     XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetFilerBase::mh_SaveDataPoolInfos(const std::vector<C_OSCParamSetDataPoolInfo> & orc_DataPoolInfos,
                                                  C_OSCXMLParserBase & orc_XMLParser)
{
   //Avoid adding the same datapools twice
   if (orc_XMLParser.SelectNodeChild("datapools") != "datapools")
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("datapools") == "datapools");
      for (uint32 u32_ItDataPool = 0; u32_ItDataPool < orc_DataPoolInfos.size(); ++u32_ItDataPool)
      {
         tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("datapool") == "datapool");
         C_OSCParamSetFilerBase::mh_SaveDataPoolInfo(orc_DataPoolInfos[u32_ItDataPool], orc_XMLParser);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "datapools");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
   }
   else
   {
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load parameter set data pool info

   Load parameter set data pool info data from XML file
   pre-condition: the passed XML parser has the active node set to "datapool"
   post-condition: the passed XML parser has the active node set to the same "datapool"

   \param[out]    orc_DataPoolInfo           data storage
   \param[in,out] orc_XMLParser              XML with specified node active
   \param[in,out] orq_MissingOptionalContent Flag for indication of optional content missing
                                             Warning: flag is never set to false if optional content is present

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetFilerBase::mh_LoadDataPoolInfo(C_OSCParamSetDataPoolInfo & orc_DataPoolInfo,
                                                   C_OSCXMLParserBase & orc_XMLParser,
                                                   bool & orq_MissingOptionalContent)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.AttributeExists("crc") == true)
   {
      orc_DataPoolInfo.u32_DataPoolCrc = orc_XMLParser.GetAttributeUint32("crc");
   }
   else
   {
      osc_write_log_error("Loading Dataset data", "Could not find \"node\".\"datapool\".\"crc\" attribute.");
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("nvm-size") == true)
   {
      orc_DataPoolInfo.u32_NvMSize = orc_XMLParser.GetAttributeUint32("nvm-size");
   }
   else
   {
      orq_MissingOptionalContent = true;
   }
   if (orc_XMLParser.AttributeExists("nvm-start-address") == true)
   {
      orc_DataPoolInfo.u32_NvMStartAddress = orc_XMLParser.GetAttributeUint32("nvm-start-address");
   }
   else
   {
      orq_MissingOptionalContent = true;
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("name") == "name")
      {
         orc_DataPoolInfo.c_Name = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "datapool");
      }
      else
      {
         osc_write_log_error("Loading Dataset data", "Could not find \"node\".\"datapool\".\"name\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("version") == "version")
      {
         if (orc_XMLParser.AttributeExists("major") == true)
         {
            orc_DataPoolInfo.au8_Version[0] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("major"));
         }
         else
         {
            osc_write_log_error("Loading Dataset data",
                                "Could not find \"node\".\"datapool\".\"version\".\"major\" attribute.");
            s32_Retval = C_CONFIG;
         }
         if ((orc_XMLParser.AttributeExists("minor") == true) && (s32_Retval == C_NO_ERR))
         {
            orc_DataPoolInfo.au8_Version[1] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("minor"));
         }
         else
         {
            osc_write_log_error("Loading Dataset data",
                                "Could not find \"node\".\"datapool\".\"version\".\"minor\" attribute.");
            s32_Retval = C_CONFIG;
         }
         if ((orc_XMLParser.AttributeExists("release") == true) && (s32_Retval == C_NO_ERR))
         {
            orc_DataPoolInfo.au8_Version[2] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("release"));
         }
         else
         {
            osc_write_log_error("Loading Dataset data",
                                "Could not find \"node\".\"datapool\".\"version\".\"release\" attribute.");
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "datapool");
      }
      else
      {
         osc_write_log_error("Loading Dataset data", "Could not find \"node\".\"datapool\".\"version\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save parameter set data pool info

   Save parameter set data pool info to XML file
   pre-condition: the passed XML parser has the active node set to "datapool"
   post-condition: the passed XML parser has the active node set to the same "datapool"

   \param[in]     orc_DataPoolInfo information to store
   \param[in,out] orc_XMLParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetFilerBase::mh_SaveDataPoolInfo(const C_OSCParamSetDataPoolInfo & orc_DataPoolInfo,
                                                 C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("crc", orc_DataPoolInfo.u32_DataPoolCrc);
   orc_XMLParser.SetAttributeUint32("nvm-start-address", orc_DataPoolInfo.u32_NvMStartAddress);
   orc_XMLParser.SetAttributeUint32("nvm-size", orc_DataPoolInfo.u32_NvMSize);
   orc_XMLParser.CreateNodeChild("name", orc_DataPoolInfo.c_Name);
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("version") == "version");
   orc_XMLParser.SetAttributeUint32("major", orc_DataPoolInfo.au8_Version[0]);
   orc_XMLParser.SetAttributeUint32("minor", orc_DataPoolInfo.au8_Version[1]);
   orc_XMLParser.SetAttributeUint32("release", orc_DataPoolInfo.au8_Version[2]);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "datapool");
}
