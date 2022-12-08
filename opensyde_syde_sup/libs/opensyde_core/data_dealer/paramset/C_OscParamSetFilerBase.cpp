//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Parameter set file reader/writer base (implementation)

   Parameter set file reader/writer base

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "TglFile.hpp"
#include "TglUtils.hpp"
#include "C_OscChecksummedXml.hpp"
#include "C_OscParamSetFilerBase.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::tgl;

using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
uint16_t C_OscParamSetFilerBase::mhu16_FileVersion = 1;

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
int32_t C_OscParamSetFilerBase::h_AddCrc(const C_SclString & orc_Path)
{
   int32_t s32_Return = C_NO_ERR;

   if (TglFileExists(orc_Path) == true)
   {
      C_OscChecksummedXml c_XmlParser;
      s32_Return = c_XmlParser.LoadFromFile(orc_Path);
      //ignore missing and incorrect CRC; we want to set it
      if ((s32_Return == C_NO_ERR) || (s32_Return == C_CHECKSUM) || (s32_Return == C_RD_WR))
      {
         //do we have the correct type of file ?
         if (c_XmlParser.SelectRoot() != "opensyde-parameter-sets")
         {
            s32_Return = C_CONFIG;
         }
         else
         {
            s32_Return = c_XmlParser.SaveToFile(orc_Path);
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

   \param[in,out] orc_XmlParser XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetFilerBase::h_CheckFileVersion(C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("file-version") == "file-version")
   {
      uint16_t u16_FileVersion = 0U;
      try
      {
         u16_FileVersion = static_cast<uint16_t>(orc_XmlParser.GetNodeContent().ToInt());
      }
      catch (...)
      {
         osc_write_log_error("Loading Dataset data", "\"file-version\" could not be converted to a number.");
         s32_Retval = C_CONFIG;
      }

      //is the file version one we know ?
      if (s32_Retval == C_NO_ERR)
      {
         osc_write_log_info("Loading Dataset data", "Value of \"file-version\": " +
                            C_SclString::IntToStr(u16_FileVersion));
         //Check file version
         if (u16_FileVersion != mhu16_FileVersion)
         {
            osc_write_log_error("Loading Dataset data",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-parameter-sets");
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

   \param[in,out] orc_XmlParser XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetFilerBase::h_SaveFileVersion(C_OscXmlParserBase & orc_XmlParser)
{
   if (orc_XmlParser.SelectNodeChild("file-version") != "file-version")
   {
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("file-version") == "file-version");
   }
   orc_XmlParser.SetNodeContent("0x" + C_SclString::IntToHex(C_OscParamSetFilerBase::mhu16_FileVersion, 4));
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-parameter-sets");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save parameter set file info

   Save parameter set file info to XML file
   pre-condition: the passed XML parser has the active node set to "opensyde-parameter-sets"
   post-condition: the passed XML parser has the active node set to the same "opensyde-parameter-sets"

   \param[in,out] orc_XmlParser XML with specified node active
   \param[in]     orc_FileInfo  File info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetFilerBase::h_SaveFileInfo(C_OscXmlParserBase & orc_XmlParser,
                                            const C_OscParamSetInterpretedFileInfoData & orc_FileInfo)
{
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("file-info") == "file-info");
   orc_XmlParser.CreateNodeChild("datetime", orc_FileInfo.c_DateTime);
   orc_XmlParser.CreateNodeChild("creator", orc_FileInfo.c_Creator);
   orc_XmlParser.CreateNodeChild("toolname", orc_FileInfo.c_ToolName);
   orc_XmlParser.CreateNodeChild("toolversion", orc_FileInfo.c_ToolVersion);
   orc_XmlParser.CreateNodeChild("projectname", orc_FileInfo.c_ProjectName);
   orc_XmlParser.CreateNodeChild("projectversion", orc_FileInfo.c_ProjectVersion);
   orc_XmlParser.CreateNodeChild("comment", orc_FileInfo.c_UserComment);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-parameter-sets");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load parameter set file info

   Load parameter set file info from XML file
   pre-condition: the passed XML parser has the active node set to "opensyde-parameter-sets"
   post-condition: the passed XML parser has the active node set to the same "opensyde-parameter-sets"

   \param[in,out] orc_XmlParser              XML with specified node active
   \param[out]    orc_FileInfo               Read file info
   \param[in,out] orq_MissingOptionalContent Flag for indication of optional content missing
                                             Warning: flag is never set to false if optional content is present
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetFilerBase::h_LoadFileInfo(C_OscXmlParserBase & orc_XmlParser,
                                            C_OscParamSetInterpretedFileInfoData & orc_FileInfo,
                                            bool & orq_MissingOptionalContent)
{
   //Default
   orc_FileInfo.Clear();
   //Read and overwrite if available
   if (orc_XmlParser.SelectNodeChild("file-info") == "file-info")
   {
      if (orc_XmlParser.SelectNodeChild("datetime") == "datetime")
      {
         orc_FileInfo.c_DateTime = orc_XmlParser.GetNodeContent();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "file-info");
      }
      else
      {
         orq_MissingOptionalContent = true;
      }
      if (orc_XmlParser.SelectNodeChild("creator") == "creator")
      {
         orc_FileInfo.c_Creator = orc_XmlParser.GetNodeContent();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "file-info");
      }
      else
      {
         orq_MissingOptionalContent = true;
      }
      if (orc_XmlParser.SelectNodeChild("toolname") == "toolname")
      {
         orc_FileInfo.c_ToolName = orc_XmlParser.GetNodeContent();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "file-info");
      }
      else
      {
         orq_MissingOptionalContent = true;
      }
      if (orc_XmlParser.SelectNodeChild("toolversion") == "toolversion")
      {
         orc_FileInfo.c_ToolVersion = orc_XmlParser.GetNodeContent();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "file-info");
      }
      else
      {
         orq_MissingOptionalContent = true;
      }
      if (orc_XmlParser.SelectNodeChild("projectname") == "projectname")
      {
         orc_FileInfo.c_ProjectName = orc_XmlParser.GetNodeContent();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "file-info");
      }
      else
      {
         orq_MissingOptionalContent = true;
      }
      if (orc_XmlParser.SelectNodeChild("projectversion") == "projectversion")
      {
         orc_FileInfo.c_ProjectVersion = orc_XmlParser.GetNodeContent();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "file-info");
      }
      else
      {
         orq_MissingOptionalContent = true;
      }
      if (orc_XmlParser.SelectNodeChild("comment") == "comment")
      {
         orc_FileInfo.c_UserComment = orc_XmlParser.GetNodeContent();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "file-info");
      }
      else
      {
         orq_MissingOptionalContent = true;
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-parameter-sets");
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
C_OscParamSetFilerBase::C_OscParamSetFilerBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load parameter set node name

   Load parameter set node name data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]    orc_Name      node name
   \param[in,out] orc_XmlParser XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetFilerBase::mh_LoadNodeName(stw::scl::C_SclString & orc_Name, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("name") == "name")
   {
      orc_Name = orc_XmlParser.GetNodeContent();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
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
   \param[in,out] orc_XmlParser XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetFilerBase::mh_SaveNodeName(const stw::scl::C_SclString & orc_Name, C_OscXmlParserBase & orc_XmlParser)
{
   if (orc_XmlParser.SelectNodeChild("name") == "name")
   {
      orc_XmlParser.SetNodeContent(orc_Name);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
   }
   else
   {
      orc_XmlParser.CreateNodeChild("name", orc_Name);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load parameter set data pool infos

   Load parameter set data pool infos data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]    orc_DataPoolInfos          data storage
   \param[in,out] orc_XmlParser              XML with specified node active
   \param[in,out] orq_MissingOptionalContent Flag for indication of optional content missing
                                             Warning: flag is never set to false if optional content is present

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetFilerBase::mh_LoadDataPoolInfos(std::vector<C_OscParamSetDataPoolInfo> & orc_DataPoolInfos,
                                                     C_OscXmlParserBase & orc_XmlParser,
                                                     bool & orq_MissingOptionalContent)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_DataPoolInfos.clear();
   if (orc_XmlParser.SelectNodeChild("datapools") == "datapools")
   {
      C_SclString c_SelectedNode = orc_XmlParser.SelectNodeChild("datapool");

      if (c_SelectedNode == "datapool")
      {
         do
         {
            C_OscParamSetDataPoolInfo c_Item;
            s32_Retval = C_OscParamSetFilerBase::mh_LoadDataPoolInfo(c_Item, orc_XmlParser, orq_MissingOptionalContent);
            if (s32_Retval == C_NO_ERR)
            {
               orc_DataPoolInfos.push_back(c_Item);
            }

            //Next
            c_SelectedNode = orc_XmlParser.SelectNodeNext("datapool");
         }
         while ((c_SelectedNode == "datapool") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "datapools");
      }
      else
      {
         osc_write_log_error("Loading Dataset data", "Could not find \"datapools\".\"datapool\" node.");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
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
   \param[in,out] orc_XmlParser     XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetFilerBase::mh_SaveDataPoolInfos(const std::vector<C_OscParamSetDataPoolInfo> & orc_DataPoolInfos,
                                                  C_OscXmlParserBase & orc_XmlParser)
{
   //Avoid adding the same datapools twice
   if (orc_XmlParser.SelectNodeChild("datapools") != "datapools")
   {
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("datapools") == "datapools");
      for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < orc_DataPoolInfos.size(); ++u32_ItDataPool)
      {
         tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("datapool") == "datapool");
         C_OscParamSetFilerBase::mh_SaveDataPoolInfo(orc_DataPoolInfos[u32_ItDataPool], orc_XmlParser);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "datapools");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
   }
   else
   {
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load parameter set data pool info

   Load parameter set data pool info data from XML file
   pre-condition: the passed XML parser has the active node set to "datapool"
   post-condition: the passed XML parser has the active node set to the same "datapool"

   \param[out]    orc_DataPoolInfo           data storage
   \param[in,out] orc_XmlParser              XML with specified node active
   \param[in,out] orq_MissingOptionalContent Flag for indication of optional content missing
                                             Warning: flag is never set to false if optional content is present

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetFilerBase::mh_LoadDataPoolInfo(C_OscParamSetDataPoolInfo & orc_DataPoolInfo,
                                                    C_OscXmlParserBase & orc_XmlParser,
                                                    bool & orq_MissingOptionalContent)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.AttributeExists("crc") == true)
   {
      orc_DataPoolInfo.u32_DataPoolCrc = orc_XmlParser.GetAttributeUint32("crc");
   }
   else
   {
      osc_write_log_error("Loading Dataset data", "Could not find \"node\".\"datapool\".\"crc\" attribute.");
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("nvm-size") == true)
   {
      orc_DataPoolInfo.u32_NvmSize = orc_XmlParser.GetAttributeUint32("nvm-size");
   }
   else
   {
      orq_MissingOptionalContent = true;
   }
   if (orc_XmlParser.AttributeExists("nvm-start-address") == true)
   {
      orc_DataPoolInfo.u32_NvmStartAddress = orc_XmlParser.GetAttributeUint32("nvm-start-address");
   }
   else
   {
      orq_MissingOptionalContent = true;
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("name") == "name")
      {
         orc_DataPoolInfo.c_Name = orc_XmlParser.GetNodeContent();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "datapool");
      }
      else
      {
         osc_write_log_error("Loading Dataset data", "Could not find \"node\".\"datapool\".\"name\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("version") == "version")
      {
         if (orc_XmlParser.AttributeExists("major") == true)
         {
            orc_DataPoolInfo.au8_Version[0] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("major"));
         }
         else
         {
            osc_write_log_error("Loading Dataset data",
                                "Could not find \"node\".\"datapool\".\"version\".\"major\" attribute.");
            s32_Retval = C_CONFIG;
         }
         if ((orc_XmlParser.AttributeExists("minor") == true) && (s32_Retval == C_NO_ERR))
         {
            orc_DataPoolInfo.au8_Version[1] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("minor"));
         }
         else
         {
            osc_write_log_error("Loading Dataset data",
                                "Could not find \"node\".\"datapool\".\"version\".\"minor\" attribute.");
            s32_Retval = C_CONFIG;
         }
         if ((orc_XmlParser.AttributeExists("release") == true) && (s32_Retval == C_NO_ERR))
         {
            orc_DataPoolInfo.au8_Version[2] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("release"));
         }
         else
         {
            osc_write_log_error("Loading Dataset data",
                                "Could not find \"node\".\"datapool\".\"version\".\"release\" attribute.");
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "datapool");
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
   \param[in,out] orc_XmlParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetFilerBase::mh_SaveDataPoolInfo(const C_OscParamSetDataPoolInfo & orc_DataPoolInfo,
                                                 C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("crc", orc_DataPoolInfo.u32_DataPoolCrc);
   orc_XmlParser.SetAttributeUint32("nvm-start-address", orc_DataPoolInfo.u32_NvmStartAddress);
   orc_XmlParser.SetAttributeUint32("nvm-size", orc_DataPoolInfo.u32_NvmSize);
   orc_XmlParser.CreateNodeChild("name", orc_DataPoolInfo.c_Name);
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("version") == "version");
   orc_XmlParser.SetAttributeUint32("major", orc_DataPoolInfo.au8_Version[0]);
   orc_XmlParser.SetAttributeUint32("minor", orc_DataPoolInfo.au8_Version[1]);
   orc_XmlParser.SetAttributeUint32("release", orc_DataPoolInfo.au8_Version[2]);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "datapool");
}
