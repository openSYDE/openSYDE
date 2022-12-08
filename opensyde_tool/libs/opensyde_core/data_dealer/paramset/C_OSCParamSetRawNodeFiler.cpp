//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Raw parameter set file reader/writer (implementation)

   Raw parameter set file reader/writer

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstdio>
#include "TglFile.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_OscChecksummedXml.hpp"
#include "C_OscParamSetRawNodeFiler.hpp"
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
/*! \brief   Load parameter set node

   Load parameter set node data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]    orc_Node                   data storage
   \param[in,out] orc_XmlParser              XML with specified node active
   \param[in,out] orq_MissingOptionalContent Flag for indication of optional content missing
                                             Warning: flag is never set to false if optional content is present

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetRawNodeFiler::h_LoadRawNode(C_OscParamSetRawNode & orc_Node, C_OscXmlParserBase & orc_XmlParser,
                                                 bool & orq_MissingOptionalContent)
{
   int32_t s32_Retval = C_OscParamSetFilerBase::mh_LoadNodeName(orc_Node.c_Name, orc_XmlParser);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OscParamSetFilerBase::mh_LoadDataPoolInfos(orc_Node.c_DataPools, orc_XmlParser,
                                                                orq_MissingOptionalContent);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OscParamSetRawNodeFiler::mh_LoadEntries(orc_Node.c_Entries, orc_XmlParser);
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save parameter set node

   Save parameter set node to XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[in]     orc_Node      data storage
   \param[in,out] orc_XmlParser XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetRawNodeFiler::h_SaveRawNode(const C_OscParamSetRawNode & orc_Node, C_OscXmlParserBase & orc_XmlParser)
{
   C_OscParamSetFilerBase::mh_SaveNodeName(orc_Node.c_Name, orc_XmlParser);
   C_OscParamSetFilerBase::mh_SaveDataPoolInfos(orc_Node.c_DataPools, orc_XmlParser);
   C_OscParamSetRawNodeFiler::mh_SaveEntries(orc_Node.c_Entries, orc_XmlParser);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscParamSetRawNodeFiler::C_OscParamSetRawNodeFiler(void) :
   C_OscParamSetFilerBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load parameter set entries

   Load parameter set entries data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]    orc_Node      data storage
   \param[in,out] orc_XmlParser XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetRawNodeFiler::mh_LoadEntries(std::vector<C_OscParamSetRawEntry> & orc_Entries,
                                                  C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_Entries.clear();
   if (orc_XmlParser.SelectNodeChild("raw") == "raw")
   {
      C_SclString c_SelectedNode = orc_XmlParser.SelectNodeChild("raw-entry");

      if (c_SelectedNode == "raw-entry")
      {
         do
         {
            C_OscParamSetRawEntry c_Item;
            s32_Retval = C_OscParamSetRawNodeFiler::mh_LoadEntry(c_Item, orc_XmlParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_Entries.push_back(c_Item);
            }

            //Next
            c_SelectedNode = orc_XmlParser.SelectNodeNext("raw-entry");
         }
         while ((c_SelectedNode == "raw-entry") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "raw");
      }
      else
      {
         osc_write_log_error("Loading Dataset data", "Could not find \"node\".\"raw\".\"raw-entry\" node.");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save parameter set entries

   Save parameter set entries to XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[in]     orc_Node      data storage
   \param[in,out] orc_XmlParser XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetRawNodeFiler::mh_SaveEntries(const std::vector<C_OscParamSetRawEntry> & orc_Entries,
                                               C_OscXmlParserBase & orc_XmlParser)
{
   //Clean start
   if (orc_XmlParser.SelectNodeChild("raw") == "raw")
   {
      tgl_assert(orc_XmlParser.DeleteNode() == "raw");
      tgl_assert(orc_XmlParser.SelectRoot() == "opensyde-parameter-sets");
   }
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("raw") == "raw");
   for (uint32_t u32_Index = 0U; u32_Index < orc_Entries.size(); u32_Index++)
   {
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("raw-entry") == "raw-entry");
      C_OscParamSetRawNodeFiler::mh_SaveEntry(orc_Entries[u32_Index], orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "raw");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load parameter set entry

   Load parameter set entry data from XML file
   pre-condition: the passed XML parser has the active node set to "raw-entry"
   post-condition: the passed XML parser has the active node set to the same "raw-entry"

   \param[out]    orc_Node      data storage
   \param[in,out] orc_XmlParser XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetRawNodeFiler::mh_LoadEntry(C_OscParamSetRawEntry & orc_Entry, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("address") == "address")
   {
      try
      {
         orc_Entry.u32_StartAddress = static_cast<uint32_t>(orc_XmlParser.GetNodeContent().ToInt64());
      }
      catch (...)
      {
         osc_write_log_error("Loading Dataset data", "Node \"node\".\"raw\".\"raw-entry\".\"address\" contains non-integer value (" +
                             orc_XmlParser.GetNodeContent() + ").");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "raw-entry");
   }
   else
   {
      osc_write_log_error("Loading Dataset data", "Could not find \"node\".\"raw\".\"raw-entry\".\"address\" node.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("value") == "value")
      {
         const C_SclString c_Content = orc_XmlParser.GetNodeContent();
         C_SclDynamicArray<C_SclString> c_Tokens;
         c_Content.Tokenize(";", c_Tokens);
         orc_Entry.c_Bytes.reserve(c_Tokens.GetLength());
         for (int32_t s32_It = 0; (s32_It < c_Tokens.GetLength()) && (s32_Retval == C_NO_ERR); ++s32_It)
         {
            const C_SclString & rc_Token = c_Tokens[s32_It];
            try
            {
               orc_Entry.c_Bytes.push_back(static_cast<uint8_t>(rc_Token.ToInt()));
            }
            catch (...)
            {
               osc_write_log_error("Loading Dataset data", "Node \"node\".\"raw\".\"raw-entry\".\"value\" contains non-integer value (" +
                                   rc_Token + ").");

               s32_Retval = C_CONFIG;
            }
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "raw-entry");
      }
      else
      {
         osc_write_log_error("Loading Dataset data", "Could not find \"node\".\"raw\".\"raw-entry\".\"value\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("size") == "size")
      {
         uint32_t u32_Size = 0;
         try
         {
            u32_Size = static_cast<uint32_t>(orc_XmlParser.GetNodeContent().ToInt64());
         }
         catch (...)
         {
            s32_Retval = C_CONFIG;
         }
         if (s32_Retval == C_NO_ERR)
         {
            if (u32_Size != orc_Entry.c_Bytes.size())
            {
               s32_Retval = C_CONFIG;
            }
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "raw-entry");
      }
      else
      {
         osc_write_log_error("Loading Dataset data", "Could not find \"node\".\"raw\".\"raw-entry\".\"size\" node.");
         s32_Retval = C_CONFIG;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save parameter set entry

   Save parameter set entry to XML file
   pre-condition: the passed XML parser has the active node set to "raw-entry"
   post-condition: the passed XML parser has the active node set to the same "raw-entry"

   \param[in]     orc_Node      data storage
   \param[in,out] orc_XmlParser XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetRawNodeFiler::mh_SaveEntry(const C_OscParamSetRawEntry & orc_Entry,
                                             C_OscXmlParserBase & orc_XmlParser)
{
   C_SclString c_Bytes;

   orc_XmlParser.CreateNodeChild("address", C_SclString::IntToStr(orc_Entry.u32_StartAddress));
   orc_XmlParser.CreateNodeChild("size", C_SclString::IntToStr(orc_Entry.c_Bytes.size()));
   if (orc_Entry.c_Bytes.size() > 0)
   {
      c_Bytes = C_SclString::IntToStr(orc_Entry.c_Bytes[0]);
      for (uint32_t u32_It = 1; u32_It < orc_Entry.c_Bytes.size(); ++u32_It)
      {
         c_Bytes += ';';
         c_Bytes += C_SclString::IntToStr(orc_Entry.c_Bytes[u32_It]);
      }
   }
   else
   {
      c_Bytes = "";
   }
   orc_XmlParser.CreateNodeChild("value", c_Bytes);
}
