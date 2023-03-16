//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Interpreted parameter set file reader/writer (implementation)

   Interpreted parameter set file reader/writer

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
#include "C_OscNodeDataPoolFiler.hpp"
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
/*! \brief   Load parameter set node

   Load parameter set node data from XML file
   pre-condition: the passed XML parser has the active node set to "opensyde-parameter-sets"
   post-condition: the passed XML parser has the active node set to the same "opensyde-parameter-sets"

   \param[out]     orc_Node                     data storage
   \param[in,out]  orc_XmlParser                XML with specified node active
   \param[in,out]  orq_MissingOptionalContent   Flag for indication of optional content missing
                                                Warning: flag is never set to false if optional content is present

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetInterpretedNodeFiler::h_LoadInterpretedNode(C_OscParamSetInterpretedNode & orc_Node,
                                                                 C_OscXmlParserBase & orc_XmlParser,
                                                                 bool & orq_MissingOptionalContent)
{
   int32_t s32_Retval = C_OscParamSetFilerBase::mh_LoadNodeName(orc_Node.c_Name, orc_XmlParser);

   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_OscParamSetDataPoolInfo> c_DataPoolInfos;
      s32_Retval = C_OscParamSetFilerBase::mh_LoadDataPoolInfos(c_DataPoolInfos, orc_XmlParser,
                                                                orq_MissingOptionalContent);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OscParamSetInterpretedNodeFiler::mh_LoadDataPools(orc_Node.c_DataPools, c_DataPoolInfos,
                                                                          orc_XmlParser);
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
   \param[in,out]  orc_XmlParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetInterpretedNodeFiler::h_SaveInterpretedNode(const C_OscParamSetInterpretedNode & orc_Node,
                                                              C_OscXmlParserBase & orc_XmlParser)
{
   std::vector<C_OscParamSetDataPoolInfo> c_Infos;
   C_OscParamSetFilerBase::mh_SaveNodeName(orc_Node.c_Name, orc_XmlParser);
   //Extract data pool info
   c_Infos.reserve(orc_Node.c_DataPools.size());
   for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < orc_Node.c_DataPools.size(); ++u32_ItDataPool)
   {
      const C_OscParamSetInterpretedDataPool & rc_DataPool = orc_Node.c_DataPools[u32_ItDataPool];
      c_Infos.push_back(rc_DataPool.c_DataPoolInfo);
   }
   C_OscParamSetFilerBase::mh_SaveDataPoolInfos(c_Infos, orc_XmlParser);
   C_OscParamSetInterpretedNodeFiler::mh_SaveDataPools(orc_Node.c_DataPools, orc_XmlParser);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscParamSetInterpretedNodeFiler::C_OscParamSetInterpretedNodeFiler(void) :
   C_OscParamSetFilerBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pools

   Load data pools data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]     orc_DataPools       data storage
   \param[in]      orc_DataPoolInfos   Loaded data pool infos
   \param[in,out]  orc_XmlParser       XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetInterpretedNodeFiler::mh_LoadDataPools(
   std::vector<C_OscParamSetInterpretedDataPool> & orc_DataPools,
   const std::vector<C_OscParamSetDataPoolInfo> & orc_DataPoolInfos, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_DataPools.clear();
   if (orc_XmlParser.SelectNodeChild("interpreted") == "interpreted")
   {
      C_SclString c_SelectedNode = orc_XmlParser.SelectNodeChild("datapool");

      if (c_SelectedNode == "datapool")
      {
         uint32_t u32_ItDataPoolInfo = 0;
         do
         {
            if (u32_ItDataPoolInfo < orc_DataPoolInfos.size())
            {
               C_OscParamSetInterpretedDataPool c_Item;
               //Info
               c_Item.c_DataPoolInfo = orc_DataPoolInfos[u32_ItDataPoolInfo];
               ++u32_ItDataPoolInfo;
               //Content
               s32_Retval = C_OscParamSetInterpretedNodeFiler::mh_LoadDataPool(c_Item, orc_XmlParser);
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
            c_SelectedNode = orc_XmlParser.SelectNodeNext("datapool");
         }
         while ((c_SelectedNode == "datapool") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "interpreted");
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
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
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
   \param[in,out]  orc_XmlParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetInterpretedNodeFiler::mh_SaveDataPools(
   const std::vector<C_OscParamSetInterpretedDataPool> & orc_DataPools, C_OscXmlParserBase & orc_XmlParser)
{
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("interpreted") == "interpreted");
   for (uint32_t u32_Index = 0U; u32_Index < orc_DataPools.size(); u32_Index++)
   {
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("datapool") == "datapool");
      C_OscParamSetInterpretedNodeFiler::mh_SaveDataPool(orc_DataPools[u32_Index], orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "interpreted");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pool

   Load data pool data from XML file
   pre-condition: the passed XML parser has the active node set to "datapool"
   post-condition: the passed XML parser has the active node set to the same "datapool"

   \param[in,out]  orc_DataPool     Data pool
   \param[in,out]  orc_XmlParser    XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetInterpretedNodeFiler::mh_LoadDataPool(C_OscParamSetInterpretedDataPool & orc_DataPool,
                                                           C_OscXmlParserBase & orc_XmlParser)
{
   const int32_t s32_Retval = C_OscParamSetInterpretedNodeFiler::mh_LoadLists(orc_DataPool.c_Lists, orc_XmlParser);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool

   Save data pool to XML file
   pre-condition: the passed XML parser has the active node set to "datapool"
   post-condition: the passed XML parser has the active node set to the same "datapool"

   \param[in]      orc_DataPool     Data pool
   \param[in,out]  orc_XmlParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetInterpretedNodeFiler::mh_SaveDataPool(const C_OscParamSetInterpretedDataPool & orc_DataPool,
                                                        C_OscXmlParserBase & orc_XmlParser)
{
   C_OscParamSetInterpretedNodeFiler::mh_SaveLists(orc_DataPool.c_Lists, orc_XmlParser);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load lists

   Load lists data from XML file
   pre-condition: the passed XML parser has the active node set to "datapool"
   post-condition: the passed XML parser has the active node set to the same "datapool"

   \param[in,out]  orc_Lists        Lists
   \param[in,out]  orc_XmlParser    XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetInterpretedNodeFiler::mh_LoadLists(std::vector<C_OscParamSetInterpretedList> & orc_Lists,
                                                        C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_Lists.clear();
   if (orc_XmlParser.SelectNodeChild("lists") == "lists")
   {
      C_SclString c_SelectedNode = orc_XmlParser.SelectNodeChild("list");

      if (c_SelectedNode == "list")
      {
         do
         {
            C_OscParamSetInterpretedList c_Item;
            s32_Retval = C_OscParamSetInterpretedNodeFiler::mh_LoadList(c_Item, orc_XmlParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_Lists.push_back(c_Item);
            }

            //Next
            c_SelectedNode = orc_XmlParser.SelectNodeNext("list");
         }
         while ((c_SelectedNode == "list") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "lists");
      }
      else
      {
         osc_write_log_error("Loading Dataset data",
                             "Could not find \"node\".\"interpreted\".\"datapool\".\"lists\".\"list\" node.");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "datapool");
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
   \param[in,out]  orc_XmlParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetInterpretedNodeFiler::mh_SaveLists(const std::vector<C_OscParamSetInterpretedList> & orc_Lists,
                                                     C_OscXmlParserBase & orc_XmlParser)
{
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("lists") == "lists");
   for (uint32_t u32_Index = 0U; u32_Index < orc_Lists.size(); u32_Index++)
   {
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("list") == "list");
      C_OscParamSetInterpretedNodeFiler::mh_SaveList(orc_Lists[u32_Index], orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "lists");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "datapool");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load list

   Load list data from XML file
   pre-condition: the passed XML parser has the active node set to "list"
   post-condition: the passed XML parser has the active node set to the same "list"

   \param[in,out]  orc_List         List
   \param[in,out]  orc_XmlParser    XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetInterpretedNodeFiler::mh_LoadList(C_OscParamSetInterpretedList & orc_List,
                                                       C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("name") == "name")
   {
      orc_List.c_Name = orc_XmlParser.GetNodeContent();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "list");
   }
   else
   {
      osc_write_log_error("Loading Dataset data", "Could not find \"list\".\"name\" node.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OscParamSetInterpretedNodeFiler::mh_LoadElements(orc_List.c_Elements, orc_XmlParser);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save list

   Save list to XML file
   pre-condition: the passed XML parser has the active node set to "list"
   post-condition: the passed XML parser has the active node set to the same "list"

   \param[in]      orc_List         List
   \param[in,out]  orc_XmlParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetInterpretedNodeFiler::mh_SaveList(const C_OscParamSetInterpretedList & orc_List,
                                                    C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateNodeChild("name", orc_List.c_Name);
   C_OscParamSetInterpretedNodeFiler::mh_SaveElements(orc_List.c_Elements, orc_XmlParser);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load elements

   Load elements data from XML file
   pre-condition: the passed XML parser has the active node set to "list"
   post-condition: the passed XML parser has the active node set to the same "list"

   \param[in,out]  orc_Elements     Elements
   \param[in,out]  orc_XmlParser    XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetInterpretedNodeFiler::mh_LoadElements(std::vector<C_OscParamSetInterpretedElement> & orc_Elements,
                                                           C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_Elements.clear();
   if (orc_XmlParser.SelectNodeChild("elements") == "elements")
   {
      C_SclString c_SelectedNode = orc_XmlParser.SelectNodeChild("element");

      if (c_SelectedNode == "element")
      {
         do
         {
            C_OscParamSetInterpretedElement c_Item;
            s32_Retval = C_OscParamSetInterpretedNodeFiler::mh_LoadElement(c_Item, orc_XmlParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_Elements.push_back(c_Item);
            }

            //Next
            c_SelectedNode = orc_XmlParser.SelectNodeNext("element");
         }
         while ((c_SelectedNode == "element") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "elements");
      }
      else
      {
         osc_write_log_error("Loading Dataset data", "Could not find \"elements\".\"element\" node.");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "list");
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
   \param[in,out]  orc_XmlParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetInterpretedNodeFiler::mh_SaveElements(
   const std::vector<C_OscParamSetInterpretedElement> & orc_Elements, C_OscXmlParserBase & orc_XmlParser)
{
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("elements") == "elements");
   for (uint32_t u32_Index = 0U; u32_Index < orc_Elements.size(); u32_Index++)
   {
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("element") == "element");
      C_OscParamSetInterpretedNodeFiler::mh_SaveElement(orc_Elements[u32_Index], orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "elements");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "list");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load element

   Load element data from XML file
   pre-condition: the passed XML parser has the active node set to "element"
   post-condition: the passed XML parser has the active node set to the same "element"

   \param[out]     orc_Element      data storage
   \param[in,out]  orc_XmlParser    XML with specified node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscParamSetInterpretedNodeFiler::mh_LoadElement(C_OscParamSetInterpretedElement & orc_Element,
                                                          C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("name") == "name")
   {
      orc_Element.c_Name = orc_XmlParser.GetNodeContent();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "element");
   }
   else
   {
      osc_write_log_error("Loading Dataset data", "Could not find \"element\".\"name\" node.");
      s32_Retval = C_CONFIG;
   }

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OscNodeDataPoolFiler::h_LoadDataPoolElementType(orc_Element.c_NvmValue, orc_XmlParser);
      if ((s32_Retval == C_NO_ERR) && (orc_XmlParser.SelectNodeChild("value") == "value"))
      {
         C_SclString c_Error;
         s32_Retval = C_OscNodeDataPoolFiler::h_LoadDataPoolElementValue(orc_Element.c_NvmValue, orc_XmlParser, true,
                                                                         &c_Error);
         if (s32_Retval != C_NO_ERR)
         {
            osc_write_log_error("Loading Dataset data",
                                "Invalid content of \"element\".\"value\" node: " +  c_Error);
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "element");
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
   \param[in,out]  orc_XmlParser    XML with specified node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetInterpretedNodeFiler::mh_SaveElement(const C_OscParamSetInterpretedElement & orc_Element,
                                                       C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateNodeChild("name", orc_Element.c_Name);
   C_OscNodeDataPoolFiler::h_SaveDataPoolElementType(orc_Element.c_NvmValue, orc_XmlParser);
   C_OscNodeDataPoolFiler::h_SaveDataPoolElementValue("value", orc_Element.c_NvmValue, orc_XmlParser);
}
