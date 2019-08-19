//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Raw parameter set file reader/writer (implementation)

   Raw parameter set file reader/writer

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cstdio>
#include "TGLFile.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_OSCChecksummedXML.h"
#include "C_OSCParamSetRawNodeFiler.h"

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
/*! \brief   Load parameter set node

   Load parameter set node data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]    orc_Node                   data storage
   \param[in,out] orc_XMLParser              XML with specified node active
   \param[in,out] orq_MissingOptionalContent Flag for indication of optional content missing
                                             Warning: flag is never set to false if optional content is present

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_OSCParamSetRawNodeFiler::h_LoadRawNode(C_OSCParamSetRawNode & orc_Node,
                                                           C_OSCXMLParserBase & orc_XMLParser,
                                                           bool & orq_MissingOptionalContent)
{
   stw_types::sint32 s32_Retval = C_OSCParamSetFilerBase::h_LoadNodeName(orc_Node.c_Name, orc_XMLParser);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OSCParamSetFilerBase::h_LoadDataPoolInfos(orc_Node.c_DataPools, orc_XMLParser,
                                                               orq_MissingOptionalContent);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OSCParamSetRawNodeFiler::mh_LoadEntries(orc_Node.c_Entries, orc_XMLParser);
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
   \param[in,out] orc_XMLParser XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetRawNodeFiler::h_SaveRawNode(const C_OSCParamSetRawNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser)
{
   C_OSCParamSetFilerBase::h_SaveNodeName(orc_Node.c_Name, orc_XMLParser);
   C_OSCParamSetFilerBase::h_SaveDataPoolInfos(orc_Node.c_DataPools, orc_XMLParser);
   C_OSCParamSetRawNodeFiler::mh_SaveEntries(orc_Node.c_Entries, orc_XMLParser);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCParamSetRawNodeFiler::C_OSCParamSetRawNodeFiler(void) :
   C_OSCParamSetFilerBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load parameter set entries

   Load parameter set entries data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]    orc_Node      data storage
   \param[in,out] orc_XMLParser XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetRawNodeFiler::mh_LoadEntries(std::vector<C_OSCParamSetRawEntry> & orc_Entries,
                                                 C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Entries.clear();
   if (orc_XMLParser.SelectNodeChild("raw") == "raw")
   {
      C_SCLString c_SelectedNode = orc_XMLParser.SelectNodeChild("raw-entry");

      if (c_SelectedNode == "raw-entry")
      {
         do
         {
            C_OSCParamSetRawEntry c_Item;
            s32_Retval = C_OSCParamSetRawNodeFiler::mh_LoadEntry(c_Item, orc_XMLParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_Entries.push_back(c_Item);
            }

            //Next
            c_SelectedNode = orc_XMLParser.SelectNodeNext("raw-entry");
         }
         while ((c_SelectedNode == "raw-entry") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "raw");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
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
   \param[in,out] orc_XMLParser XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetRawNodeFiler::mh_SaveEntries(const std::vector<C_OSCParamSetRawEntry> & orc_Entries,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   //Clean start
   if (orc_XMLParser.SelectNodeChild("raw") == "raw")
   {
      tgl_assert(orc_XMLParser.DeleteNode() == "raw");
      tgl_assert(orc_XMLParser.SelectRoot() == "opensyde-parameter-sets");
   }
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("raw") == "raw");
   for (uint32 u32_Index = 0U; u32_Index < orc_Entries.size(); u32_Index++)
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("raw-entry") == "raw-entry");
      C_OSCParamSetRawNodeFiler::mh_SaveEntry(orc_Entries[u32_Index], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "raw");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load parameter set entry

   Load parameter set entry data from XML file
   pre-condition: the passed XML parser has the active node set to "raw-entry"
   post-condition: the passed XML parser has the active node set to the same "raw-entry"

   \param[out]    orc_Node      data storage
   \param[in,out] orc_XMLParser XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetRawNodeFiler::mh_LoadEntry(C_OSCParamSetRawEntry & orc_Entry, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("address") == "address")
   {
      try
      {
         orc_Entry.u32_StartAddress = static_cast<uint32>(orc_XMLParser.GetNodeContent().ToInt64());
      }
      catch (...)
      {
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "raw-entry");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if ((orc_XMLParser.SelectNodeChild("value") == "value") && (s32_Retval == C_NO_ERR))
   {
      const C_SCLString c_Content = orc_XMLParser.GetNodeContent();
      SCLDynamicArray<C_SCLString> c_Tokens;
      c_Content.Tokenize(";", c_Tokens);
      orc_Entry.c_Bytes.reserve(c_Tokens.GetLength());
      for (sint32 s32_It = 0; (s32_It < c_Tokens.GetLength()) && (s32_Retval == C_NO_ERR); ++s32_It)
      {
         const C_SCLString & rc_Token = c_Tokens[s32_It];
         try
         {
            orc_Entry.c_Bytes.push_back(static_cast<uint8>(rc_Token.ToInt()));
         }
         catch (...)
         {
            s32_Retval = C_CONFIG;
         }
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "raw-entry");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if ((orc_XMLParser.SelectNodeChild("size") == "size") && (s32_Retval == C_NO_ERR))
   {
      uint32 u32_Size = 0;
      try
      {
         u32_Size = static_cast<uint32>(orc_XMLParser.GetNodeContent().ToInt64());
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
      tgl_assert(orc_XMLParser.SelectNodeParent() == "raw-entry");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save parameter set entry

   Save parameter set entry to XML file
   pre-condition: the passed XML parser has the active node set to "raw-entry"
   post-condition: the passed XML parser has the active node set to the same "raw-entry"

   \param[in]     orc_Node      data storage
   \param[in,out] orc_XMLParser XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetRawNodeFiler::mh_SaveEntry(const C_OSCParamSetRawEntry & orc_Entry,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   C_SCLString c_Bytes;

   orc_XMLParser.CreateNodeChild("address", C_SCLString::IntToStr(orc_Entry.u32_StartAddress));
   orc_XMLParser.CreateNodeChild("size", C_SCLString::IntToStr(orc_Entry.c_Bytes.size()));
   if (orc_Entry.c_Bytes.size() > 0)
   {
      c_Bytes = C_SCLString::IntToStr(orc_Entry.c_Bytes[0]);
      for (uint32 u32_It = 1; u32_It < orc_Entry.c_Bytes.size(); ++u32_It)
      {
         c_Bytes += ';';
         c_Bytes += C_SCLString::IntToStr(orc_Entry.c_Bytes[u32_It]);
      }
   }
   else
   {
      c_Bytes = "";
   }
   orc_XMLParser.CreateNodeChild("value", c_Bytes);
}
