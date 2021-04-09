//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Interpreted parameter set file reader/writer (implementation)

   Interpreted parameter set file reader/writer

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
#include "C_OSCNodeDataPoolFiler.h"
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
/*! \brief   Load parameter set node

   Load parameter set node data from XML file
   pre-condition: the passed XML parser has the active node set to "opensyde-parameter-sets"
   post-condition: the passed XML parser has the active node set to the same "opensyde-parameter-sets"

   \param[out]     orc_Node                     data storage
   \param[in,out]  orc_XMLParser                XML with specified node active
   \param[in,out]  orq_MissingOptionalContent   Flag for indication of optional content missing
                                                Warning: flag is never set to false if optional content is present

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_OSCParamSetInterpretedNodeFiler::h_LoadInterpretedNode(C_OSCParamSetInterpretedNode & orc_Node,
                                                                           C_OSCXMLParserBase & orc_XMLParser,
                                                                           bool & orq_MissingOptionalContent)
{
   stw_types::sint32 s32_Retval = C_OSCParamSetFilerBase::mh_LoadNodeName(orc_Node.c_Name, orc_XMLParser);
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_OSCParamSetDataPoolInfo> c_DataPoolInfos;
      s32_Retval = C_OSCParamSetFilerBase::mh_LoadDataPoolInfos(c_DataPoolInfos, orc_XMLParser,
                                                                orq_MissingOptionalContent);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OSCParamSetInterpretedNodeFiler::mh_LoadDataPools(orc_Node.c_DataPools, c_DataPoolInfos,
                                                                          orc_XMLParser);
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save parameter set node

   Save parameter set node to XML file
   pre-condition: the passed XML parser has the active node set to "opensyde-parameter-sets"
   post-condition: the passed XML parser has the active node set to the same "opensyde-parameter-sets"

   \param[in]      orc_Node         data storage
   \param[in,out]  orc_XMLParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetInterpretedNodeFiler::h_SaveInterpretedNode(const C_OSCParamSetInterpretedNode & orc_Node,
                                                              C_OSCXMLParserBase & orc_XMLParser)
{
   std::vector<C_OSCParamSetDataPoolInfo> c_Infos;
   C_OSCParamSetFilerBase::mh_SaveNodeName(orc_Node.c_Name, orc_XMLParser);
   //Extract data pool info
   c_Infos.reserve(orc_Node.c_DataPools.size());
   for (uint32 u32_ItDataPool = 0; u32_ItDataPool < orc_Node.c_DataPools.size(); ++u32_ItDataPool)
   {
      const C_OSCParamSetInterpretedDataPool & rc_DataPool = orc_Node.c_DataPools[u32_ItDataPool];
      c_Infos.push_back(rc_DataPool.c_DataPoolInfo);
   }
   C_OSCParamSetFilerBase::mh_SaveDataPoolInfos(c_Infos, orc_XMLParser);
   C_OSCParamSetInterpretedNodeFiler::mh_SaveDataPools(orc_Node.c_DataPools, orc_XMLParser);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCParamSetInterpretedNodeFiler::C_OSCParamSetInterpretedNodeFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pools

   Load data pools data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]     orc_DataPools       data storage
   \param[in]      orc_DataPoolInfos   Loaded data pool infos
   \param[in,out]  orc_XMLParser       XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetInterpretedNodeFiler::mh_LoadDataPools(
   std::vector<C_OSCParamSetInterpretedDataPool> & orc_DataPools,
   const std::vector<C_OSCParamSetDataPoolInfo> & orc_DataPoolInfos, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_DataPools.clear();
   if (orc_XMLParser.SelectNodeChild("interpreted") == "interpreted")
   {
      uint32 u32_ItDataPoolInfo = 0;
      C_SCLString c_SelectedNode = orc_XMLParser.SelectNodeChild("datapool");

      if (c_SelectedNode == "datapool")
      {
         do
         {
            if (u32_ItDataPoolInfo < orc_DataPoolInfos.size())
            {
               C_OSCParamSetInterpretedDataPool c_Item;
               //Info
               c_Item.c_DataPoolInfo = orc_DataPoolInfos[u32_ItDataPoolInfo];
               ++u32_ItDataPoolInfo;
               //Content
               s32_Retval = C_OSCParamSetInterpretedNodeFiler::mh_LoadDataPool(c_Item, orc_XMLParser);
               if (s32_Retval == C_NO_ERR)
               {
                  orc_DataPools.push_back(c_Item);
               }
            }
            else
            {
               osc_write_log_error("Loading Dataset data", "Information for too many data pools present.");
               s32_Retval = C_CONFIG;
            }

            //Next
            c_SelectedNode = orc_XMLParser.SelectNodeNext("datapool");
         }
         while ((c_SelectedNode == "datapool") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "interpreted");
      }
      else
      {
         osc_write_log_error("Loading Dataset data", "Could not find \"node\".\"interpreted\".\"datapool\" node.");
         s32_Retval = C_CONFIG;
      }
      //Check  number of found data pools
      if ((s32_Retval == C_NO_ERR) && (orc_DataPools.size() != orc_DataPoolInfos.size()))
      {
         osc_write_log_error("Loading Dataset data", "Number of Datapools described in file does not match number of "
                             "defined Datapools.");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
   }
   else
   {
      osc_write_log_error("Loading Dataset data", "Could not find \"node\".\"interpreted\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pools

   Save data pools to XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[in]      orc_DataPools    Data pools
   \param[in,out]  orc_XMLParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetInterpretedNodeFiler::mh_SaveDataPools(
   const std::vector<C_OSCParamSetInterpretedDataPool> & orc_DataPools, C_OSCXMLParserBase & orc_XMLParser)
{
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("interpreted") == "interpreted");
   for (uint32 u32_Index = 0U; u32_Index < orc_DataPools.size(); u32_Index++)
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("datapool") == "datapool");
      C_OSCParamSetInterpretedNodeFiler::mh_SaveDataPool(orc_DataPools[u32_Index], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "interpreted");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pool

   Load data pool data from XML file
   pre-condition: the passed XML parser has the active node set to "datapool"
   post-condition: the passed XML parser has the active node set to the same "datapool"

   \param[in,out]  orc_DataPool     Data pool
   \param[in,out]  orc_XMLParser    XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetInterpretedNodeFiler::mh_LoadDataPool(C_OSCParamSetInterpretedDataPool & orc_DataPool,
                                                          C_OSCXMLParserBase & orc_XMLParser)
{
   const sint32 s32_Retval = C_OSCParamSetInterpretedNodeFiler::mh_LoadLists(orc_DataPool.c_Lists, orc_XMLParser);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool

   Save data pool to XML file
   pre-condition: the passed XML parser has the active node set to "datapool"
   post-condition: the passed XML parser has the active node set to the same "datapool"

   \param[in]      orc_DataPool     Data pool
   \param[in,out]  orc_XMLParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetInterpretedNodeFiler::mh_SaveDataPool(const C_OSCParamSetInterpretedDataPool & orc_DataPool,
                                                        C_OSCXMLParserBase & orc_XMLParser)
{
   C_OSCParamSetInterpretedNodeFiler::mh_SaveLists(orc_DataPool.c_Lists, orc_XMLParser);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load lists

   Load lists data from XML file
   pre-condition: the passed XML parser has the active node set to "datapool"
   post-condition: the passed XML parser has the active node set to the same "datapool"

   \param[in,out]  orc_Lists        Lists
   \param[in,out]  orc_XMLParser    XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetInterpretedNodeFiler::mh_LoadLists(std::vector<C_OSCParamSetInterpretedList> & orc_Lists,
                                                       C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Lists.clear();
   if (orc_XMLParser.SelectNodeChild("lists") == "lists")
   {
      C_SCLString c_SelectedNode = orc_XMLParser.SelectNodeChild("list");

      if (c_SelectedNode == "list")
      {
         do
         {
            C_OSCParamSetInterpretedList c_Item;
            s32_Retval = C_OSCParamSetInterpretedNodeFiler::mh_LoadList(c_Item, orc_XMLParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_Lists.push_back(c_Item);
            }

            //Next
            c_SelectedNode = orc_XMLParser.SelectNodeNext("list");
         }
         while ((c_SelectedNode == "list") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "lists");
      }
      else
      {
         osc_write_log_error("Loading Dataset data",
                             "Could not find \"node\".\"interpreted\".\"datapool\".\"lists\".\"list\" node.");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "datapool");
   }
   else
   {
      osc_write_log_error("Loading Dataset data",
                          "Could not find \"node\".\"interpreted\".\"datapool\".\"lists\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save lists

   Save lists to XML file
   pre-condition: the passed XML parser has the active node set to "datapool"
   post-condition: the passed XML parser has the active node set to the same "datapool"

   \param[in]      orc_Lists        Lists
   \param[in,out]  orc_XMLParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetInterpretedNodeFiler::mh_SaveLists(const std::vector<C_OSCParamSetInterpretedList> & orc_Lists,
                                                     C_OSCXMLParserBase & orc_XMLParser)
{
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("lists") == "lists");
   for (uint32 u32_Index = 0U; u32_Index < orc_Lists.size(); u32_Index++)
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("list") == "list");
      C_OSCParamSetInterpretedNodeFiler::mh_SaveList(orc_Lists[u32_Index], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "lists");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "datapool");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load list

   Load list data from XML file
   pre-condition: the passed XML parser has the active node set to "list"
   post-condition: the passed XML parser has the active node set to the same "list"

   \param[in,out]  orc_List         List
   \param[in,out]  orc_XMLParser    XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetInterpretedNodeFiler::mh_LoadList(C_OSCParamSetInterpretedList & orc_List,
                                                      C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("name") == "name")
   {
      orc_List.c_Name = orc_XMLParser.GetNodeContent();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "list");
   }
   else
   {
      osc_write_log_error("Loading Dataset data", "Could not find \"list\".\"name\" node.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OSCParamSetInterpretedNodeFiler::mh_LoadElements(orc_List.c_Elements, orc_XMLParser);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save list

   Save list to XML file
   pre-condition: the passed XML parser has the active node set to "list"
   post-condition: the passed XML parser has the active node set to the same "list"

   \param[in]      orc_List         List
   \param[in,out]  orc_XMLParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetInterpretedNodeFiler::mh_SaveList(const C_OSCParamSetInterpretedList & orc_List,
                                                    C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateNodeChild("name", orc_List.c_Name);
   C_OSCParamSetInterpretedNodeFiler::mh_SaveElements(orc_List.c_Elements, orc_XMLParser);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load elements

   Load elements data from XML file
   pre-condition: the passed XML parser has the active node set to "list"
   post-condition: the passed XML parser has the active node set to the same "list"

   \param[in,out]  orc_Elements     Elements
   \param[in,out]  orc_XMLParser    XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetInterpretedNodeFiler::mh_LoadElements(std::vector<C_OSCParamSetInterpretedElement> & orc_Elements,
                                                          C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Elements.clear();
   if (orc_XMLParser.SelectNodeChild("elements") == "elements")
   {
      C_SCLString c_SelectedNode = orc_XMLParser.SelectNodeChild("element");

      if (c_SelectedNode == "element")
      {
         do
         {
            C_OSCParamSetInterpretedElement c_Item;
            s32_Retval = C_OSCParamSetInterpretedNodeFiler::mh_LoadElement(c_Item, orc_XMLParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_Elements.push_back(c_Item);
            }

            //Next
            c_SelectedNode = orc_XMLParser.SelectNodeNext("element");
         }
         while ((c_SelectedNode == "element") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "elements");
      }
      else
      {
         osc_write_log_error("Loading Dataset data", "Could not find \"elements\".\"element\" node.");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "list");
   }
   else
   {
      osc_write_log_error("Loading Dataset data", "Could not find \"list\".\"elements\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save elements

   Save elements to XML file
   pre-condition: the passed XML parser has the active node set to "list"
   post-condition: the passed XML parser has the active node set to the same "list"

   \param[in]      orc_Elements     Elements
   \param[in,out]  orc_XMLParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetInterpretedNodeFiler::mh_SaveElements(
   const std::vector<C_OSCParamSetInterpretedElement> & orc_Elements, C_OSCXMLParserBase & orc_XMLParser)
{
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("elements") == "elements");
   for (uint32 u32_Index = 0U; u32_Index < orc_Elements.size(); u32_Index++)
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("element") == "element");
      C_OSCParamSetInterpretedNodeFiler::mh_SaveElement(orc_Elements[u32_Index], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "elements");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "list");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load element

   Load element data from XML file
   pre-condition: the passed XML parser has the active node set to "element"
   post-condition: the passed XML parser has the active node set to the same "element"

   \param[out]     orc_Element      data storage
   \param[in,out]  orc_XMLParser    XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCParamSetInterpretedNodeFiler::mh_LoadElement(C_OSCParamSetInterpretedElement & orc_Element,
                                                         C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("name") == "name")
   {
      orc_Element.c_Name = orc_XMLParser.GetNodeContent();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "element");
   }
   else
   {
      osc_write_log_error("Loading Dataset data", "Could not find \"element\".\"name\" node.");
      s32_Retval = C_CONFIG;
   }

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OSCNodeDataPoolFiler::h_LoadDataPoolElementType(orc_Element.c_NvmValue, orc_XMLParser);
      if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("value") == "value"))
      {
         C_SCLString c_Error;
         s32_Retval = C_OSCNodeDataPoolFiler::h_LoadDataPoolElementValue(orc_Element.c_NvmValue, orc_XMLParser, true,
                                                                         &c_Error);
         if (s32_Retval != C_NO_ERR)
         {
            osc_write_log_error("Loading Dataset data",
                                "Invalid content of \"element\".\"value\" node: " +  c_Error);
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "element");
      }
      else
      {
         osc_write_log_error("Loading Dataset data", "Could not find \"element\".\"value\" node.");
         s32_Retval = C_CONFIG;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save element

   Save element to XML file
   pre-condition: the passed XML parser has the active node set to "element"
   post-condition: the passed XML parser has the active node set to the same "element"

   \param[in]      orc_Element      Element
   \param[in,out]  orc_XMLParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCParamSetInterpretedNodeFiler::mh_SaveElement(const C_OSCParamSetInterpretedElement & orc_Element,
                                                       C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateNodeChild("name", orc_Element.c_Name);
   C_OSCNodeDataPoolFiler::h_SaveDataPoolElementType(orc_Element.c_NvmValue, orc_XMLParser);
   C_OSCNodeDataPoolFiler::h_SaveDataPoolElementValue("value", orc_Element.c_NvmValue, orc_XMLParser);
}
