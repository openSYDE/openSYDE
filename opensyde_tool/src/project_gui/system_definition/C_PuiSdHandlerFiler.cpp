//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI topology filer (implementation)

   UI topology filer

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QBuffer>
#include <QFileInfo>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscNodeFiler.hpp"
#include "C_OscSystemDefinitionFiler.hpp"
#include "C_OscSystemFilerUtil.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_PuiSdHandlerFiler.hpp"
#include "C_PuiBsElementsFiler.hpp"
#include "C_OscDataLoggerJobFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::tgl;
using namespace stw::scl;
using namespace stw::opensyde_gui_logic;
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
C_PuiSdHandlerFiler::C_PuiSdHandlerFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pools

   \param[in,out]  orc_DataPools       Data pool elements (Cleared if necessary)
   \param[in,out]  orc_XmlParser       XML parser with the "current" element set to the "node" element
   \param[in]      opc_BasePath        Base path (Optional for save to string)
   \param[in,out]  opc_OscDataPools    Core data pools

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadDataPools(std::vector<C_PuiSdNodeDataPool> & orc_DataPools,
                                             C_OscXmlParserBase & orc_XmlParser, const QDir * const opc_BasePath,
                                             std::vector<C_OscNodeDataPool> * const opc_OscDataPools)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear last data pools
   orc_DataPools.clear();
   if (orc_XmlParser.SelectNodeChild("data-pools") == "data-pools")
   {
      C_SclString c_CurrentDataPoolNode;
      uint32_t u32_ExpectedSize = 0UL;
      const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
         orc_DataPools.reserve(u32_ExpectedSize);
      }

      c_CurrentDataPoolNode = orc_XmlParser.SelectNodeChild("data-pool");
      if (c_CurrentDataPoolNode == "data-pool")
      {
         uint32_t u32_ItDp = 0UL;
         do
         {
            C_OscNodeDataPool * const pc_OscDp = mh_GetArrayElemIfAvailable(opc_OscDataPools, u32_ItDp);
            C_PuiSdNodeDataPool c_Datapool;
            if (opc_BasePath != NULL)
            {
               const QString c_FilePath = opc_BasePath->absoluteFilePath(orc_XmlParser.GetNodeContent().c_str());
               s32_Retval = mh_LoadDatapoolFile(c_Datapool, c_FilePath, pc_OscDp);
            }
            else
            {
               s32_Retval = h_LoadDataPool(c_Datapool, orc_XmlParser, pc_OscDp);
            }
            orc_DataPools.push_back(c_Datapool);
            //Next
            c_CurrentDataPoolNode = orc_XmlParser.SelectNodeNext("data-pool");
            ++u32_ItDp;
         }
         while ((s32_Retval == C_NO_ERR) && (c_CurrentDataPoolNode == "data-pool"));
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pools");
         }
      }
      //Compare length
      if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
      {
         if (u32_ExpectedSize != orc_DataPools.size())
         {
            C_SclString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected UI Datapool count, expected: %u, got %zu", u32_ExpectedSize,
                                 orc_DataPools.size());
            osc_write_log_warning("Load file", c_Tmp.c_str());
         }
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
/*! \brief   Load data pool

   \param[in,out]  orc_DataPool     Data pool element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "data-pool" element
   \param[in,out]  opc_OscDataPool  Core data pool

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadDataPool(C_PuiSdNodeDataPool & orc_DataPool, C_OscXmlParserBase & orc_XmlParser,
                                            C_OscNodeDataPool * const opc_OscDataPool)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("lists") == "lists")
   {
      std::vector<C_OscNodeDataPoolList> * pc_OscLists = NULL;
      if (opc_OscDataPool != NULL)
      {
         pc_OscLists = &opc_OscDataPool->c_Lists;
      }
      s32_Retval = h_LoadDataPoolLists(orc_DataPool.c_DataPoolLists, orc_XmlParser, pc_OscLists);
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool");
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pool lists

   \param[in,out]  orc_DataPoolLists   Data pool lists element
   \param[in,out]  orc_XmlParser       XML parser with the "current" element set to the "lists" element
   \param[in,out]  opc_OscLists        Core lists

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadDataPoolLists(std::vector<C_PuiSdNodeDataPoolList> & orc_DataPoolLists,
                                                 C_OscXmlParserBase & orc_XmlParser,
                                                 std::vector<C_OscNodeDataPoolList> * const opc_OscLists)
{
   int32_t s32_Retval = C_NO_ERR;

   C_SclString c_CurrentDataPoolListNode;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_DataPoolLists.reserve(u32_ExpectedSize);
   }

   c_CurrentDataPoolListNode = orc_XmlParser.SelectNodeChild("list");

   orc_DataPoolLists.clear();
   if (c_CurrentDataPoolListNode == "list")
   {
      uint32_t u32_ItList = 0UL;
      do
      {
         C_OscNodeDataPoolList * const pc_OscList = mh_GetArrayElemIfAvailable(opc_OscLists, u32_ItList);
         C_PuiSdNodeDataPoolList c_List;
         s32_Retval = h_LoadDataPoolList(c_List, orc_XmlParser, pc_OscList);
         orc_DataPoolLists.push_back(c_List);

         //Next
         c_CurrentDataPoolListNode = orc_XmlParser.SelectNodeNext("list");
         ++u32_ItList;
      }
      while ((s32_Retval == C_NO_ERR) && (c_CurrentDataPoolListNode == "list"));
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "lists");
      }
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_DataPoolLists.size())
      {
         C_SclString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI list count, expected: %u, got %zu", u32_ExpectedSize,
                              orc_DataPoolLists.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pool list

   \param[in,out]  orc_DataPoolList    Data pool list element
   \param[in,out]  orc_XmlParser       XML parser with the "current" element set to the "list" element
   \param[in,out]  opc_OscList         Core list

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadDataPoolList(C_PuiSdNodeDataPoolList & orc_DataPoolList,
                                                C_OscXmlParserBase & orc_XmlParser,
                                                C_OscNodeDataPoolList * const opc_OscList)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("data-elements") == "data-elements")
   {
      std::vector<C_OscNodeDataPoolListElement> * pc_OscElements = NULL;
      if (opc_OscList != NULL)
      {
         pc_OscElements = &opc_OscList->c_Elements;
      }
      h_LoadDataPoolListElements(orc_DataPoolList.c_DataPoolListElements, orc_XmlParser, pc_OscElements);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "list");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pool list elements

   \param[in,out]  orc_DataPoolListElements  Data pool list elements
   \param[in,out]  orc_XmlParser             XML parser with the "current" element set to the "data-elements" element
   \param[in,out]  opc_OscElements           Core elements

   \return
   C_NO_ERR    information loaded
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadDataPoolListElements(
   std::vector<C_PuiSdNodeDataPoolListElement> & orc_DataPoolListElements, C_OscXmlParserBase & orc_XmlParser,
   std::vector<C_OscNodeDataPoolListElement> * const opc_OscElements)
{
   const int32_t s32_RETVAL = C_NO_ERR;

   C_SclString c_CurrentDataPoolListElementNode;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_DataPoolListElements.reserve(u32_ExpectedSize);
   }

   c_CurrentDataPoolListElementNode = orc_XmlParser.SelectNodeChild("data-element");

   orc_DataPoolListElements.clear();
   if (c_CurrentDataPoolListElementNode == "data-element")
   {
      uint32_t u32_ItEl = 0UL;
      do
      {
         C_OscNodeDataPoolListElement * const pc_OscEl = mh_GetArrayElemIfAvailable(opc_OscElements, u32_ItEl);
         C_PuiSdNodeDataPoolListElement c_Element;
         h_LoadDataPoolListElement(c_Element, orc_XmlParser, pc_OscEl);
         orc_DataPoolListElements.push_back(c_Element);

         //Next
         c_CurrentDataPoolListElementNode = orc_XmlParser.SelectNodeNext("data-element");
         ++u32_ItEl;
      }
      while (c_CurrentDataPoolListElementNode == "data-element");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-elements");
   }
   //Compare length
   if (q_ExpectedSizeHere == true)
   {
      if (u32_ExpectedSize != orc_DataPoolListElements.size())
      {
         C_SclString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI data element count, expected: %u, got %zu", u32_ExpectedSize,
                              orc_DataPoolListElements.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_RETVAL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pool list element

   \param[in,out]  orc_DataPoolListElement   Data pool list element
   \param[in,out]  orc_XmlParser             XML parser with the "current" element set to the "data-element" element
   \param[in,out]  opc_OscElement            Core element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_LoadDataPoolListElement(C_PuiSdNodeDataPoolListElement & orc_DataPoolListElement,
                                                    const C_OscXmlParserBase & orc_XmlParser,
                                                    C_OscNodeDataPoolListElement * const opc_OscElement)
{
   orc_DataPoolListElement.q_AutoMinMaxActive = orc_XmlParser.GetAttributeBool("auto_min_max_active");
   if ((opc_OscElement != NULL) && (orc_XmlParser.AttributeExists("interpret_as_string")))
   {
      opc_OscElement->q_InterpretAsString = orc_XmlParser.GetAttributeBool("interpret_as_string");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pools

   \param[in]      orc_UiDataPools     UI data pool elements
   \param[in]      opc_OscDataPools    OSC data pool elements (Optional for save to string)
   \param[in]      opc_BasePath        Base path (Optional for save to string)
   \param[in,out]  orc_XmlParser       XML parser with the "current" element set to the "node" element

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_SaveDataPools(const std::vector<C_PuiSdNodeDataPool> & orc_UiDataPools,
                                             const std::vector<C_OscNodeDataPool> * const opc_OscDataPools,
                                             const QDir * const opc_BasePath, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if ((opc_OscDataPools != NULL) && (opc_BasePath != NULL))
   {
      if (orc_UiDataPools.size() == opc_OscDataPools->size())
      {
         orc_XmlParser.CreateAndSelectNodeChild("data-pools");
         orc_XmlParser.SetAttributeUint32("length", orc_UiDataPools.size());
         for (uint32_t u32_ItDataPool = 0; (u32_ItDataPool < orc_UiDataPools.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItDataPool)
         {
            const C_OscNodeDataPool & rc_OscDatapool = (*opc_OscDataPools)[u32_ItDataPool];
            const QString c_FileName = C_PuiSdHandlerFiler::h_GetDatapoolUiFileName(rc_OscDatapool.c_Name);
            orc_XmlParser.CreateAndSelectNodeChild("data-pool");
            s32_Retval =
               h_SaveDataPoolFile(orc_UiDataPools[u32_ItDataPool], opc_BasePath->absoluteFilePath(c_FileName));
            //Store file name
            orc_XmlParser.SetNodeContent(c_FileName.toStdString().c_str());
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pools");
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
      }
      else
      {
         s32_Retval = C_CONFIG;
         osc_write_log_error("Saving system definition UI", "datapool vector sizes don't match");
      }
   }
   else
   {
      orc_XmlParser.CreateAndSelectNodeChild("data-pools");
      orc_XmlParser.SetAttributeUint32("length", orc_UiDataPools.size());
      for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < orc_UiDataPools.size(); ++u32_ItDataPool)
      {
         orc_XmlParser.CreateAndSelectNodeChild("data-pool");
         h_SaveDataPool(orc_UiDataPools[u32_ItDataPool], orc_XmlParser);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pools");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save datapool

   Save datapool to XML file

   \param[in]      orc_DataPool  data storage
   \param[in,out]  orc_FilePath  File path for xml

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_SaveDataPoolFile(const C_PuiSdNodeDataPool & orc_DataPool, const QString & orc_FilePath)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForNewFile(c_XmlParser, orc_FilePath.toStdString().c_str(),
                                                                    "opensyde-dp-ui-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XmlParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XmlParser.CreateAndSelectNodeChild("data-pool") == "data-pool");
      //node
      C_PuiSdHandlerFiler::h_SaveDataPool(orc_DataPool, c_XmlParser);
      //Don't forget to save!
      if (c_XmlParser.SaveToFile(orc_FilePath.toStdString().c_str()) != C_NO_ERR)
      {
         osc_write_log_error("Saving system definition UI", "Could not create file for datapool.");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool

   \param[in]      orc_DataPool     Data pool element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "data-pool" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveDataPool(const C_PuiSdNodeDataPool & orc_DataPool, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("lists");

   h_SaveDataPoolLists(orc_DataPool.c_DataPoolLists, orc_XmlParser);

   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool lists

   \param[in]      orc_DataPoolLists   Data pool lists
   \param[in,out]  orc_XmlParser       XML parser with the "current" element set to the "lists" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveDataPoolLists(const std::vector<C_PuiSdNodeDataPoolList> & orc_DataPoolLists,
                                              C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("length", orc_DataPoolLists.size());
   for (uint32_t u32_ItDataPoolList = 0; u32_ItDataPoolList < orc_DataPoolLists.size();
        ++u32_ItDataPoolList)
   {
      orc_XmlParser.CreateAndSelectNodeChild("list");
      h_SaveDataPoolList(orc_DataPoolLists[u32_ItDataPoolList], orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "lists");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool list

   \param[in]      orc_DataPoolList    Data pool list element
   \param[in,out]  orc_XmlParser       XML parser with the "current" element set to the "list" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveDataPoolList(const C_PuiSdNodeDataPoolList & orc_DataPoolList,
                                             C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("data-elements");
   h_SaveDataPoolListElements(orc_DataPoolList.c_DataPoolListElements, orc_XmlParser);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "list");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool list elements

   \param[in]      orc_DataPoolListElements  Data pool list elements
   \param[in,out]  orc_XmlParser             XML parser with the "current" element set to the "data-elements" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveDataPoolListElements(
   const std::vector<C_PuiSdNodeDataPoolListElement> & orc_DataPoolListElements, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("length", orc_DataPoolListElements.size());
   for (uint32_t u32_ItDataPoolListElement = 0;
        u32_ItDataPoolListElement < orc_DataPoolListElements.size();
        ++u32_ItDataPoolListElement)
   {
      orc_XmlParser.CreateAndSelectNodeChild("data-element");
      h_SaveDataPoolListElement(orc_DataPoolListElements[u32_ItDataPoolListElement], orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-elements");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool list element

   \param[in]      orc_DataPoolListElement   Data pool list element
   \param[in,out]  orc_XmlParser             XML parser with the "current" element set to the "data-element" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveDataPoolListElement(const C_PuiSdNodeDataPoolListElement & orc_DataPoolListElement,
                                                    C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("auto_min_max_active", orc_DataPoolListElement.q_AutoMinMaxActive);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Loads the configuration of the shared Datapools

   \param[in]   orc_FilePath           File path for xml
   \param[out]  orc_SharedDatapools    Shared Datapool configuration

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadSharedDatapoolsFile(const QString & orc_FilePath,
                                                       C_PuiSdSharedDatapools & orc_SharedDatapools)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForExistingFile(c_XmlParser,
                                                                         orc_FilePath.toStdString().c_str(),
                                                                         "opensyde-shared-datapools-ui-definition");

   orc_SharedDatapools.c_SharedDatapools.clear();

   //File version
   if (c_XmlParser.SelectNodeChild("file-version") == "file-version")
   {
      uint16_t u16_FileVersion = 0U;
      try
      {
         u16_FileVersion = static_cast<uint16_t>(c_XmlParser.GetNodeContent().ToInt());
      }
      catch (...)
      {
         osc_write_log_error("Loading shared Datapools UI", "\"file-version\" could not be converted to a number.");
         s32_Retval = C_CONFIG;
      }

      //is the file version one we know ?
      if (s32_Retval == C_NO_ERR)
      {
         osc_write_log_info("Loading shared Datapools UI", "Value of \"file-version\": " +
                            C_SclString::IntToStr(u16_FileVersion));
         //Check file version
         if (u16_FileVersion != 1U)
         {
            osc_write_log_error("Loading shared Datapools UI",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      c_XmlParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading shared Datapools UI", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (c_XmlParser.SelectNodeChild("shared_datapools_groups") == "shared_datapools_groups")
      {
         C_PuiSdHandlerFiler::h_LoadSharedDatapoolsGroups(orc_SharedDatapools, c_XmlParser);
      }
      else
      {
         osc_write_log_error("Loading shared Datapools UI", "Could not find \"shared_datapools_groups\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   short description of function

   long description of function within several lines

   \param[out]     orc_SharedDatapools    Shared Datapool configuration
   \param[in,out]  orc_XmlParser          XML parser with the "current" element set to the "shared_datapools_groups" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_LoadSharedDatapoolsGroups(C_PuiSdSharedDatapools & orc_SharedDatapools,
                                                      C_OscXmlParserBase & orc_XmlParser)
{
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_SharedDatapools.c_SharedDatapools.reserve(u32_ExpectedSize);
   }

   if (orc_XmlParser.SelectNodeChild("group") == "group")
   {
      C_SclString c_CurrentNode;

      do
      {
         std::vector<stw::opensyde_core::C_OscNodeDataPoolId> c_Group;

         C_PuiSdHandlerFiler::h_LoadSharedDatapoolsGroup(c_Group, orc_XmlParser);

         if (c_Group.size() > 0)
         {
            orc_SharedDatapools.c_SharedDatapools.push_back(c_Group);
         }

         //Next
         c_CurrentNode = orc_XmlParser.SelectNodeNext("group");
      }
      while (c_CurrentNode == "group");

      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "shared_datapools_groups");
   }

   //Compare length
   if (q_ExpectedSizeHere == true)
   {
      if (u32_ExpectedSize != orc_SharedDatapools.c_SharedDatapools.size())
      {
         C_SclString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI shared Datapools group count, expected: %u, got %zu", u32_ExpectedSize,
                              orc_SharedDatapools.c_SharedDatapools.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   short description of function

   long description of function within several lines

   \param[out]     orc_Group        Read shared Datapool group
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "shared_datapools_groups" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_LoadSharedDatapoolsGroup(std::vector<C_OscNodeDataPoolId> & orc_Group,
                                                     C_OscXmlParserBase & orc_XmlParser)
{
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_Group.reserve(u32_ExpectedSize);
   }

   if (orc_XmlParser.SelectNodeChild("datapool-id") == "datapool-id")
   {
      C_SclString c_CurrentNode;

      do
      {
         stw::opensyde_core::C_OscNodeDataPoolId c_DatapoolId;

         c_DatapoolId.u32_NodeIndex = orc_XmlParser.GetAttributeUint32("node");
         c_DatapoolId.u32_DataPoolIndex = orc_XmlParser.GetAttributeUint32("datapool");

         orc_Group.push_back(c_DatapoolId);

         //Next
         c_CurrentNode = orc_XmlParser.SelectNodeNext("datapool-id");
      }
      while (c_CurrentNode == "datapool-id");

      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "group");
   }

   //Compare length
   if (q_ExpectedSizeHere == true)
   {
      if (u32_ExpectedSize != orc_Group.size())
      {
         C_SclString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI shared Datapool Ids count in group, expected: %u, got %zu",
                              u32_ExpectedSize,
                              orc_Group.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Saves the configuration of the shared Datapools

   \param[in]  orc_FilePath         File path for xml
   \param[in]  orc_SharedDatapools  Shared Datapool configuration

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_SaveSharedDatapoolsFile(const QString & orc_FilePath,
                                                       const C_PuiSdSharedDatapools & orc_SharedDatapools)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForNewFile(c_XmlParser, orc_FilePath.toStdString().c_str(),
                                                                    "opensyde-shared-datapools-ui-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XmlParser.CreateNodeChild("file-version", "1");

      // Shared Datapool groups
      tgl_assert(c_XmlParser.CreateAndSelectNodeChild("shared_datapools_groups") == "shared_datapools_groups");
      C_PuiSdHandlerFiler::h_SaveSharedDatapoolsGroups(orc_SharedDatapools, c_XmlParser);

      //Don't forget to save!
      if (c_XmlParser.SaveToFile(orc_FilePath.toStdString().c_str()) != C_NO_ERR)
      {
         osc_write_log_error("Saving shared Datapools UI", "Could not create file for configuration.");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Saves a shared Datapool group

   \param[in]      orc_SharedDatapools    Shared Datapool configuration
   \param[in,out]  orc_XmlParser          XML parser with the "current" element set to the "shared_datapools_groups" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveSharedDatapoolsGroups(const C_PuiSdSharedDatapools & orc_SharedDatapools,
                                                      C_OscXmlParserBase & orc_XmlParser)
{
   uint32_t u32_GroupCounter;
   uint32_t u32_DatapoolIdCounter;

   orc_XmlParser.SetAttributeUint32("length", orc_SharedDatapools.c_SharedDatapools.size());

   // Iterate over all different groups
   for (u32_GroupCounter = 0U; u32_GroupCounter < orc_SharedDatapools.c_SharedDatapools.size(); ++u32_GroupCounter)
   {
      const std::vector<stw::opensyde_core::C_OscNodeDataPoolId> & rc_Group =
         orc_SharedDatapools.c_SharedDatapools[u32_GroupCounter];

      orc_XmlParser.CreateAndSelectNodeChild("group");
      orc_XmlParser.SetAttributeUint32("length", rc_Group.size());

      // All Datapool Ids of the group
      for (u32_DatapoolIdCounter = 0U; u32_DatapoolIdCounter < rc_Group.size(); ++u32_DatapoolIdCounter)
      {
         orc_XmlParser.CreateAndSelectNodeChild("datapool-id");
         orc_XmlParser.SetAttributeUint32("node", rc_Group[u32_DatapoolIdCounter].u32_NodeIndex);
         orc_XmlParser.SetAttributeUint32("datapool", rc_Group[u32_DatapoolIdCounter].u32_DataPoolIndex);

         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "group");
      }

      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "shared_datapools_groups");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can protocols

   \param[in,out]  orc_CanProtocols    Can protocols (Cleared if necessary)
   \param[in,out]  orc_XmlParser       XML parser with the "current" element set to the "node" element
   \param[in]      opc_BasePath        Base path (Optional for save to string)

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadCanProtocols(std::vector<C_PuiSdNodeCanProtocol> & orc_CanProtocols,
                                                C_OscXmlParserBase & orc_XmlParser, const QDir * const opc_BasePath)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear last can protocols
   orc_CanProtocols.clear();
   if (orc_XmlParser.SelectNodeChild("com-protocols") == "com-protocols")
   {
      C_SclString c_CurrentCanProtocolNode;
      uint32_t u32_ExpectedSize = 0UL;
      const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
         orc_CanProtocols.reserve(u32_ExpectedSize);
      }

      c_CurrentCanProtocolNode = orc_XmlParser.SelectNodeChild("com-protocol");
      if (c_CurrentCanProtocolNode == "com-protocol")
      {
         do
         {
            C_PuiSdNodeCanProtocol c_CanProtocol;
            if (opc_BasePath != NULL)
            {
               const QString c_FilePath = opc_BasePath->absoluteFilePath(orc_XmlParser.GetNodeContent().c_str());
               s32_Retval = mh_LoadCommFile(c_CanProtocol, c_FilePath);
            }
            else
            {
               s32_Retval = h_LoadCanProtocol(c_CanProtocol, orc_XmlParser);
            }
            orc_CanProtocols.push_back(c_CanProtocol);

            //Next
            c_CurrentCanProtocolNode = orc_XmlParser.SelectNodeNext("com-protocol");
         }
         while ((s32_Retval == C_NO_ERR) && (c_CurrentCanProtocolNode == "com-protocol"));
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "com-protocols");
         }
      }
      //Compare length
      if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
      {
         if (u32_ExpectedSize != orc_CanProtocols.size())
         {
            C_SclString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected UI protocol count, expected: %u, got %zu", u32_ExpectedSize,
                                 orc_CanProtocols.size());
            osc_write_log_warning("Load file", c_Tmp.c_str());
         }
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
/*! \brief   Load can protocol

   \param[in,out]  orc_CanProtocol  Can protocol element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "com-protocol" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadCanProtocol(C_PuiSdNodeCanProtocol & orc_CanProtocol,
                                               C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("com-message-containers") == "com-message-containers")
   {
      s32_Retval = h_LoadCanMessageContainers(orc_CanProtocol.c_ComMessages, orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "com-protocol");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can message containers

   \param[in,out]  orc_CanMessageContainers  Can message containers
   \param[in,out]  orc_XmlParser             XML parser with the "current" element set
                                             to the "com-message-containers" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadCanMessageContainers(
   std::vector<C_PuiSdNodeCanMessageContainer> & orc_CanMessageContainers, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   C_SclString c_CurrentCanMessageContainerNode;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_CanMessageContainers.reserve(u32_ExpectedSize);
   }

   c_CurrentCanMessageContainerNode = orc_XmlParser.SelectNodeChild("com-message-container");

   orc_CanMessageContainers.clear();
   if (c_CurrentCanMessageContainerNode == "com-message-container")
   {
      do
      {
         if (s32_Retval == C_NO_ERR)
         {
            C_PuiSdNodeCanMessageContainer c_CanMessageContainer;
            s32_Retval =
               h_LoadCanMessageContainer(c_CanMessageContainer, orc_XmlParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_CanMessageContainers.push_back(c_CanMessageContainer);
            }
         }
         //Next
         c_CurrentCanMessageContainerNode = orc_XmlParser.SelectNodeNext("com-message-container");
      }
      while (c_CurrentCanMessageContainerNode == "com-message-container");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "com-message-containers");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_CanMessageContainers.size())
      {
         C_SclString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected data element count, expected: %u, got %zu", u32_ExpectedSize,
                              orc_CanMessageContainers.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can message container

   \param[in,out]  orc_CanMessageContainer   Can message container element
   \param[in,out]  orc_XmlParser             XML parser with the "current" element set
                                             to the "com-message-container " element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadCanMessageContainer(C_PuiSdNodeCanMessageContainer & orc_CanMessageContainer,
                                                       C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval;

   if (orc_XmlParser.SelectNodeChild("tx-messages") == "tx-messages")
   {
      s32_Retval = h_LoadCanMessages(orc_CanMessageContainer.c_TxMessages, orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "com-message-container");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if ((orc_XmlParser.SelectNodeChild("rx-messages") == "rx-messages") && (s32_Retval == C_NO_ERR))
   {
      s32_Retval = h_LoadCanMessages(orc_CanMessageContainer.c_RxMessages, orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "com-message-container");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can messages

   \param[in,out]  orc_CanMessages  Can messages
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to unknown element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadCanMessages(std::vector<C_PuiSdNodeCanMessage> & orc_CanMessages,
                                               C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   C_SclString c_CurrentCanMessageNode;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_CanMessages.reserve(u32_ExpectedSize);
   }

   c_CurrentCanMessageNode = orc_XmlParser.SelectNodeChild("com-message");

   orc_CanMessages.clear();
   if (c_CurrentCanMessageNode == "com-message")
   {
      do
      {
         if (s32_Retval == C_NO_ERR)
         {
            C_PuiSdNodeCanMessage c_CanMessage;
            s32_Retval = h_LoadCanMessage(c_CanMessage, orc_XmlParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_CanMessages.push_back(c_CanMessage);
            }
         }
         //Next
         c_CurrentCanMessageNode = orc_XmlParser.SelectNodeNext("com-message");
      }
      while (c_CurrentCanMessageNode == "com-message");
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_CanMessages.size())
      {
         C_SclString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI messages count, expected: %u, got %zu", u32_ExpectedSize,
                              orc_CanMessages.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can message

   \param[in,out]  orc_CanMessage   Can message element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "com-message" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadCanMessage(C_PuiSdNodeCanMessage & orc_CanMessage,
                                              C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   bool q_UseAutoReceiveTimeout;
   bool q_ReceiveTimeoutDisabled;

   //Attributes
   if (orc_XmlParser.AttributeExists("use-auto-receive-timeout") == true)
   {
      q_UseAutoReceiveTimeout = orc_XmlParser.GetAttributeBool("use-auto-receive-timeout");
   }
   else
   {
      q_UseAutoReceiveTimeout = true;
   }

   if (orc_XmlParser.AttributeExists("receive-timeout-disabled") == true)
   {
      q_ReceiveTimeoutDisabled = orc_XmlParser.GetAttributeBool("receive-timeout-disabled");
   }
   else
   {
      q_ReceiveTimeoutDisabled = false;
   }

   if (q_ReceiveTimeoutDisabled == true)
   {
      orc_CanMessage.e_ReceiveTimeoutMode = C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_DISABLED;
   }
   else if (q_UseAutoReceiveTimeout == true)
   {
      orc_CanMessage.e_ReceiveTimeoutMode = C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_AUTO;
   }
   else
   {
      orc_CanMessage.e_ReceiveTimeoutMode = C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_CUSTOM;
   }

   //Signals
   if (orc_XmlParser.SelectNodeChild("com-signals") == "com-signals")
   {
      h_LoadCanSignals(orc_CanMessage.c_Signals, orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "com-message");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can signals

   \param[in,out]  orc_CanSignals   Can signals
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "com-signals" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_LoadCanSignals(std::vector<C_PuiSdNodeCanSignal> & orc_CanSignals,
                                           C_OscXmlParserBase & orc_XmlParser)
{
   C_SclString c_CurrentDataPoolListElementNode;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_CanSignals.reserve(u32_ExpectedSize);
   }

   c_CurrentDataPoolListElementNode = orc_XmlParser.SelectNodeChild("com-signal");

   if (c_CurrentDataPoolListElementNode == "com-signal")
   {
      do
      {
         C_PuiSdNodeCanSignal c_CanSignal;
         h_LoadCanSignal(c_CanSignal, orc_XmlParser);
         orc_CanSignals.push_back(c_CanSignal);

         //Next
         c_CurrentDataPoolListElementNode = orc_XmlParser.SelectNodeNext("com-signal");
      }
      while (c_CurrentDataPoolListElementNode == "com-signal");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "com-signals");
   }
   //Compare length
   if (q_ExpectedSizeHere == true)
   {
      if (u32_ExpectedSize != orc_CanSignals.size())
      {
         C_SclString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected data element count, expected: %u, got %zu", u32_ExpectedSize,
                              orc_CanSignals.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can signal element

   \param[in,out]  orc_CanSignal    Can signal element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "com-signal" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_LoadCanSignal(C_PuiSdNodeCanSignal & orc_CanSignal,
                                          const C_OscXmlParserBase & orc_XmlParser)
{
   orc_CanSignal.u8_ColorIndex = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("color-index"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can protocols

   \param[in]      orc_UiCanProtocols     UI can protocols
   \param[in]      opc_OscCanProtocols    OSC can protocols (Optional for save to string)
   \param[in]      opc_OscDatapools       OSC datapools (Optional for save to string)
   \param[in]      opc_BasePath           Base path (Optional for save to string)
   \param[in,out]  orc_XmlParser          XML parser with the "current" element set to the "node" element

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_SaveCanProtocols(const std::vector<C_PuiSdNodeCanProtocol> & orc_UiCanProtocols,
                                                const std::vector<C_OscCanProtocol> * const opc_OscCanProtocols,
                                                const std::vector<C_OscNodeDataPool> * const opc_OscDatapools,
                                                const QDir * const opc_BasePath, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (((opc_OscCanProtocols != NULL) && (opc_OscDatapools != NULL)) && (opc_BasePath != NULL))
   {
      if (orc_UiCanProtocols.size() == opc_OscCanProtocols->size())
      {
         orc_XmlParser.CreateAndSelectNodeChild("com-protocols");
         orc_XmlParser.SetAttributeUint32("length", orc_UiCanProtocols.size());
         for (uint32_t u32_ItCanProtocol = 0;
              (u32_ItCanProtocol < orc_UiCanProtocols.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItCanProtocol)
         {
            const C_OscCanProtocol & rc_OscProtocol = (*opc_OscCanProtocols)[u32_ItCanProtocol];
            if (rc_OscProtocol.u32_DataPoolIndex < opc_OscDatapools->size())
            {
               const C_OscNodeDataPool & rc_Datapool = (*opc_OscDatapools)[rc_OscProtocol.u32_DataPoolIndex];
               const QString c_FileName = C_PuiSdHandlerFiler::h_GetCommUiFileName(rc_Datapool.c_Name);
               orc_XmlParser.CreateAndSelectNodeChild("com-protocol");
               s32_Retval =
                  h_SaveCanProtocolFile(orc_UiCanProtocols[u32_ItCanProtocol], opc_BasePath->absoluteFilePath(
                                           c_FileName));
               //Store file name
               orc_XmlParser.SetNodeContent(c_FileName.toStdString().c_str());
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "com-protocols");
            }
            else
            {
               s32_Retval = C_CONFIG;
               osc_write_log_error("Saving system definition UI", "protocol datapool index invalid");
            }
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
      }
      else
      {
         s32_Retval = C_CONFIG;
         osc_write_log_error("Saving system definition UI", "protocol vector sizes don't match");
      }
   }
   else
   {
      orc_XmlParser.CreateAndSelectNodeChild("com-protocols");
      orc_XmlParser.SetAttributeUint32("length", orc_UiCanProtocols.size());
      for (uint32_t u32_ItCanProtocol = 0; u32_ItCanProtocol < orc_UiCanProtocols.size(); ++u32_ItCanProtocol)
      {
         orc_XmlParser.CreateAndSelectNodeChild("com-protocol");
         h_SaveCanProtocol(orc_UiCanProtocols[u32_ItCanProtocol], orc_XmlParser);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "com-protocols");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save comm protocol ui

   Save comm protocol ui to XML file

   \param[in]      orc_CanProtocol  data storage
   \param[in,out]  orc_FilePath     File path for xml

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_SaveCanProtocolFile(const C_PuiSdNodeCanProtocol & orc_CanProtocol,
                                                   const QString & orc_FilePath)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForNewFile(c_XmlParser, orc_FilePath.toStdString().c_str(),
                                                                    "opensyde-comm-ui-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XmlParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XmlParser.CreateAndSelectNodeChild("com-protocol") == "com-protocol");
      //node
      C_PuiSdHandlerFiler::h_SaveCanProtocol(orc_CanProtocol, c_XmlParser);
      //Don't forget to save!
      if (c_XmlParser.SaveToFile(orc_FilePath.toStdString().c_str()) != C_NO_ERR)
      {
         osc_write_log_error("Saving system definition UI", "Could not create file for comm protocol.");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can protocol

   \param[in]      orc_CanProtocol  Can protocol
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "com-protocol" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveCanProtocol(const C_PuiSdNodeCanProtocol & orc_CanProtocol,
                                            C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("com-message-containers");

   h_SaveCanMessageContainers(orc_CanProtocol.c_ComMessages, orc_XmlParser);

   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "com-protocol");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can message containers

   \param[in]      orc_CanMessageContainers  Can message containers
   \param[in,out]  orc_XmlParser             XML parser with the "current" element set
                                           to the "com-message-containers" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveCanMessageContainers(
   const std::vector<C_PuiSdNodeCanMessageContainer> & orc_CanMessageContainers, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("length", orc_CanMessageContainers.size());
   for (uint32_t u32_ItCanMessageContainer = 0; u32_ItCanMessageContainer < orc_CanMessageContainers.size();
        ++u32_ItCanMessageContainer)
   {
      orc_XmlParser.CreateAndSelectNodeChild("com-message-container");
      h_SaveCanMessageContainer(orc_CanMessageContainers[u32_ItCanMessageContainer], orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "com-message-containers");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can message container element

   \param[in]      orc_CanMessageContainer   Can message container element
   \param[in,out]  orc_XmlParser             XML parser with the "current" element set
                                           to the "com-message-container" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveCanMessageContainer(const C_PuiSdNodeCanMessageContainer & orc_CanMessageContainer,
                                                    C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("tx-messages");
   h_SaveCanMessages(orc_CanMessageContainer.c_TxMessages, orc_XmlParser);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "com-message-container");
   orc_XmlParser.CreateAndSelectNodeChild("rx-messages");
   h_SaveCanMessages(orc_CanMessageContainer.c_RxMessages, orc_XmlParser);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "com-message-container");
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can messages

   \param[in]      orc_CanMessages  Can messages
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to unknown element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveCanMessages(const std::vector<C_PuiSdNodeCanMessage> & orc_CanMessages,
                                            C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("length", orc_CanMessages.size());
   for (uint32_t u32_ItMessage = 0; u32_ItMessage < orc_CanMessages.size();
        ++u32_ItMessage)
   {
      orc_XmlParser.CreateAndSelectNodeChild("com-message");
      h_SaveCanMessage(orc_CanMessages[u32_ItMessage], orc_XmlParser);
      //Return
      orc_XmlParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can message element

   \param[in]      orc_CanMessage   Can message element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "com-message" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveCanMessage(const C_PuiSdNodeCanMessage & orc_CanMessage,
                                           C_OscXmlParserBase & orc_XmlParser)
{
   bool q_UseAutoReceiveTimeout;
   bool q_ReceiveTimeoutDisabled;

   if (orc_CanMessage.e_ReceiveTimeoutMode == C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_DISABLED)
   {
      q_ReceiveTimeoutDisabled = true;
      q_UseAutoReceiveTimeout = true;
   }
   else if (orc_CanMessage.e_ReceiveTimeoutMode == C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_CUSTOM)
   {
      q_ReceiveTimeoutDisabled = false;
      q_UseAutoReceiveTimeout = false;
   }
   else
   {
      q_ReceiveTimeoutDisabled = false;
      q_UseAutoReceiveTimeout = true;
   }

   //Attributes
   orc_XmlParser.SetAttributeBool("use-auto-receive-timeout", q_UseAutoReceiveTimeout);
   orc_XmlParser.SetAttributeBool("receive-timeout-disabled", q_ReceiveTimeoutDisabled);
   //Signals
   orc_XmlParser.CreateAndSelectNodeChild("com-signals");
   h_SaveCanSignals(orc_CanMessage.c_Signals, orc_XmlParser);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "com-message");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can signals

   \param[in]      orc_CanSignals   Can signals
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "com-signals" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveCanSignals(const std::vector<C_PuiSdNodeCanSignal> & orc_CanSignals,
                                           C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("length", orc_CanSignals.size());
   for (uint32_t u32_ItDataPoolListElement = 0;
        u32_ItDataPoolListElement < orc_CanSignals.size();
        ++u32_ItDataPoolListElement)
   {
      orc_XmlParser.CreateAndSelectNodeChild("com-signal");
      h_SaveCanSignal(orc_CanSignals[u32_ItDataPoolListElement], orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "com-signals");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can signal element

   \param[in]      orc_CanSignal    Can signal element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "com-signal" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveCanSignal(const C_PuiSdNodeCanSignal & orc_CanSignal,
                                          C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("color-index", orc_CanSignal.u8_ColorIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load UI nodes

   Load UI node information from XML structure.
   Also uses base class function to load basic information.
   Postcondition: XMLParser has the node "nodes" selected.

   \param[out]     orc_Nodes        UI node data containers
   \param[in,out]  orc_XmlParser    XMLParser with the "current" element set to the "nodes" element
   \param[in]      opc_BasePath     Base path
   \param[in,out]  opc_OscNodes     Core nodes

   \return
   C_NO_ERR    everything ok
   else        error occured while loading
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadNodes(std::vector<C_PuiSdNode> & orc_Nodes, C_OscXmlParserBase & orc_XmlParser,
                                         const QDir * const opc_BasePath, std::vector<C_OscNode> * const opc_OscNodes)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_SelectedNode;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_Nodes.reserve(u32_ExpectedSize);
   }

   c_SelectedNode = orc_XmlParser.SelectNodeChild("node");

   orc_Nodes.clear();
   if (c_SelectedNode == "node")
   {
      uint32_t u32_ItNode = 0UL;
      do
      {
         C_OscNode * const pc_OscNode = mh_GetArrayElemIfAvailable(opc_OscNodes, u32_ItNode);
         C_PuiSdNode c_Node;
         if (opc_BasePath != NULL)
         {
            const QString c_FilePath = orc_XmlParser.GetNodeContent().c_str();
            const QString c_FilePathCombined = opc_BasePath->absoluteFilePath(c_FilePath);
            //Dir for sub folder
            const QFileInfo c_FileInfo(c_FilePathCombined);
            const QDir c_Dir = c_FileInfo.dir();
            s32_Retval = h_LoadNodeFile(c_Node, c_FilePathCombined, &c_Dir, pc_OscNode);
         }
         else
         {
            s32_Retval = mh_LoadNode(c_Node, orc_XmlParser, opc_BasePath, pc_OscNode);
         }
         if (s32_Retval == C_NO_ERR)
         {
            orc_Nodes.push_back(c_Node);
         }

         //Next
         c_SelectedNode = orc_XmlParser.SelectNodeNext("node");
         ++u32_ItNode;
      }
      while (c_SelectedNode == "node");
      //Return (no check to allow reuse)
      orc_XmlParser.SelectNodeParent();
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Nodes.size())
      {
         C_SclString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI nodes count, expected: %u, got %zu", u32_ExpectedSize,
                              orc_Nodes.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node UI part

   \param[in,out]  orc_Node      Node UI data storage
   \param[in]      orc_FilePath  File path
   \param[in]      opc_BasePath  Base path (Optional for save to string)
   \param[in,out]  opc_OscNode   Core node

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadNodeFile(C_PuiSdNode & orc_Node, const QString & orc_FilePath,
                                            const QDir * const opc_BasePath, C_OscNode * const opc_OscNode)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForExistingFile(c_XmlParser,
                                                                         orc_FilePath.toStdString().c_str(),
                                                                         "opensyde-node-ui-definition");

   //File version
   if (c_XmlParser.SelectNodeChild("file-version") == "file-version")
   {
      uint16_t u16_FileVersion = 0U;
      try
      {
         u16_FileVersion = static_cast<uint16_t>(c_XmlParser.GetNodeContent().ToInt());
      }
      catch (...)
      {
         osc_write_log_error("Loading UI node", "\"file-version\" could not be converted to a number.");
         s32_Retval = C_CONFIG;
      }

      //is the file version one we know ?
      if (s32_Retval == C_NO_ERR)
      {
         osc_write_log_info("Loading UI node", "Value of \"file-version\": " +
                            C_SclString::IntToStr(u16_FileVersion));
         //Check file version
         if (u16_FileVersion != 1U)
         {
            osc_write_log_error("Loading UI node",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      c_XmlParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading UI node", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (c_XmlParser.SelectNodeChild("node") == "node")
      {
         s32_Retval = C_PuiSdHandlerFiler::mh_LoadNode(orc_Node, c_XmlParser, opc_BasePath, opc_OscNode);
      }
      else
      {
         osc_write_log_error("Loading UI node", "Could not find \"node\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save UI nodes

   Save UI node information to XML structure
   Also uses base class function to save basic information.
   Postcondition: XMLParser has the node "nodes" selected.

   \param[in]      orc_Nodes        UI node data containers
   \param[in,out]  orc_XmlParser    XMLParser with the "current" element set to the "nodes" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveNodes(const std::vector<C_PuiSdNode> & orc_Nodes, C_OscXmlParser & orc_XmlParser)
{
   if (orc_Nodes.size() > 0)
   {
      orc_XmlParser.SetAttributeUint32("length", orc_Nodes.size());
      for (uint32_t u32_Index = 0U; u32_Index < orc_Nodes.size(); u32_Index++)
      {
         orc_XmlParser.CreateAndSelectNodeChild("node");
         tgl_assert(C_PuiSdHandlerFiler::mh_SaveNode(orc_Nodes[u32_Index], NULL, NULL, orc_XmlParser) == C_NO_ERR);
         //Back up (don't check to allow reuse)
         orc_XmlParser.SelectNodeParent();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load GUI buses

   Load GUI-specific information about bus.
   Also uses base class function to load basic information.

   \param[in,out]  orc_Buses        UI bus data containers
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "buses" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadBuses(std::vector<C_PuiSdBus> & orc_Buses, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_SelectedNode;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_Buses.reserve(u32_ExpectedSize);
   }

   c_SelectedNode = orc_XmlParser.SelectNodeChild("bus");

   orc_Buses.clear();
   if (c_SelectedNode == "bus")
   {
      do
      {
         C_PuiSdBus c_Bus;
         s32_Retval = C_PuiSdHandlerFiler::mh_LoadBus(c_Bus, orc_XmlParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Buses.push_back(c_Bus);
         }

         //Next
         c_SelectedNode = orc_XmlParser.SelectNodeNext("bus");
      }
      while ((s32_Retval == C_NO_ERR) && (c_SelectedNode == "bus"));
      if (s32_Retval == C_NO_ERR)
      {
         //Return (no check to allow reuse)
         orc_XmlParser.SelectNodeParent();
      }
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Buses.size())
      {
         C_SclString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected bus count, expected: %u, got %zu", u32_ExpectedSize,
                              orc_Buses.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save GUI buses

   Save GUI-specific information about bus.
   Also uses base class function to save basic information.

   \param[in]      orc_Buses        UI bus data containers
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "buses" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveBuses(const std::vector<C_PuiSdBus> & orc_Buses, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("length", orc_Buses.size());
   if (orc_Buses.size() > 0)
   {
      for (uint32_t u32_Index = 0U; u32_Index < orc_Buses.size(); u32_Index++)
      {
         orc_XmlParser.CreateAndSelectNodeChild("bus");
         C_PuiSdHandlerFiler::mh_SaveBus(orc_Buses[u32_Index], orc_XmlParser);
         //Back up (don't check to allow reuse)
         orc_XmlParser.SelectNodeParent();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load text elements

   \param[in,out]  orc_BusTextElements    Text element data elements
   \param[in,out]  orc_XmlParser          XML parser with the "current" element set to the "bus-text-elements" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadBusTextElements(std::vector<C_PuiSdTextElementBus> & orc_BusTextElements,
                                                   C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_SelectedNode;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_BusTextElements.reserve(u32_ExpectedSize);
   }

   c_SelectedNode = orc_XmlParser.SelectNodeChild("bus-text-element");

   orc_BusTextElements.clear();
   if (c_SelectedNode == "bus-text-element")
   {
      do
      {
         C_PuiSdTextElementBus c_TextElement;
         s32_Retval = C_PuiSdHandlerFiler::mh_LoadTextElement(&c_TextElement, orc_XmlParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_BusTextElements.push_back(c_TextElement);
         }

         //Next
         c_SelectedNode = orc_XmlParser.SelectNodeNext("bus-text-element");
      }
      while (c_SelectedNode == "bus-text-element");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "bus-text-elements");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_BusTextElements.size())
      {
         C_SclString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected bus text element count, expected: %u, got %zu", u32_ExpectedSize,
                              orc_BusTextElements.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save text elements

   \param[in]      orc_BusTextElements    Text element data elements
   \param[in,out]  orc_XmlParser          XML parser with the "current" element set to the "bus-text-elements" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveBusTextElements(const std::vector<C_PuiSdTextElementBus> & orc_BusTextElements,
                                                C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("length", orc_BusTextElements.size());
   for (uint32_t u32_Index = 0U; u32_Index < orc_BusTextElements.size(); ++u32_Index)
   {
      orc_XmlParser.CreateAndSelectNodeChild("bus-text-element");
      C_PuiSdHandlerFiler::mh_SaveTextElement(&orc_BusTextElements[u32_Index], orc_XmlParser);
      tgl_assert(orc_XmlParser.SelectNodeParent() == "bus-text-elements"); //back up
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load last known halc crcs

   \param[in,out]  orc_Crcs         Crcs
   \param[in,out]  orc_XmlParser    XML parser

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadLastKnownHalcCrcs(std::map<C_OscNodeDataPoolListElementOptArrayId,
                                                              C_PuiSdLastKnownHalElementId> & orc_Crcs,
                                                     C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_Crcs.clear();
   if (orc_XmlParser.SelectNodeChild("last-known-halc-crcs") == "last-known-halc-crcs")
   {
      C_SclString c_CurrentNode = orc_XmlParser.SelectNodeChild("last-known-halc-crc");

      if (c_CurrentNode == "last-known-halc-crc")
      {
         do
         {
            C_OscNodeDataPoolListElementOptArrayId c_Id;
            s32_Retval = C_OscDataLoggerJobFiler::h_LoadDataElementOptArrayId(c_Id, orc_XmlParser);
            if (s32_Retval == C_NO_ERR)
            {
               if (orc_XmlParser.AttributeExists("crc"))
               {
                  const uint32_t u32_Crc = orc_XmlParser.GetAttributeUint32("crc");
                  QString c_DpName;
                  if (orc_XmlParser.SelectNodeChild("hal-data-pool-name") == "hal-data-pool-name")
                  {
                     c_DpName = orc_XmlParser.GetNodeContent().c_str();
                     //Return
                     tgl_assert(orc_XmlParser.SelectNodeParent() == "last-known-halc-crc");
                  }
                  else
                  {
                     const C_OscNodeDataPool * const pc_Dp = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(
                        c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex);
                     if (pc_Dp != NULL)
                     {
                        c_DpName = pc_Dp->c_Name.c_str();
                     }
                  }
                  //Insert
                  orc_Crcs[c_Id] = C_PuiSdLastKnownHalElementId(u32_Crc, c_DpName);
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
            //Next
            c_CurrentNode = orc_XmlParser.SelectNodeNext("last-known-halc-crc");
         }
         while ((c_CurrentNode == "last-known-halc-crc") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "last-known-halc-crcs");
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save last known halc crcs

   \param[in]      orc_Crcs         Crcs
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveLastKnownHalcCrcs(const std::map<C_OscNodeDataPoolListElementOptArrayId,
                                                                 C_PuiSdLastKnownHalElementId> & orc_Crcs,
                                                  C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("last-known-halc-crcs");
   for (std::map<C_OscNodeDataPoolListElementOptArrayId, C_PuiSdLastKnownHalElementId>::const_iterator c_It =
           orc_Crcs.begin();
        c_It != orc_Crcs.end(); ++c_It)
   {
      orc_XmlParser.CreateAndSelectNodeChild("last-known-halc-crc");
      C_OscDataLoggerJobFiler::h_SaveDataElementOptArrayId(c_It->first, orc_XmlParser);
      orc_XmlParser.SetAttributeUint32("crc", c_It->second.u32_Crc);
      orc_XmlParser.CreateNodeChild("hal-data-pool-name", c_It->second.c_HalDpName.toStdString().c_str());

      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "last-known-halc-crcs");
   }
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save UI system definition

   Save UI system definition to XML file

   \param[in]  orc_FilePath               File path for xml
   \param[in]  orc_OscSystemDefinition    OSC storage
   \param[in]  orc_UiNodes                UI nodes data storage
   \param[in]  orc_UiBuses                UI buses data storage
   \param[in]  orc_BusTextElements        UI bus text elements data storage
   \param[in]  orc_Elements               UI generic elements data storage
   \param[in]  orc_LastKnownHalcCrcs      Last known halc crcs

   \return
   C_NO_ERR   data saved
   C_RD_WR    file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_SaveSystemDefinitionUiFile(const QString & orc_FilePath,
                                                          const C_OscSystemDefinition & orc_OscSystemDefinition,
                                                          const std::vector<C_PuiSdNode> & orc_UiNodes,
                                                          const std::vector<C_PuiSdBus> & orc_UiBuses,
                                                          const std::vector<C_PuiSdTextElementBus> & orc_BusTextElements, const C_PuiBsElements & orc_Elements, const std::map<C_OscNodeDataPoolListElementOptArrayId,
                                                                                                                                                                               C_PuiSdLastKnownHalElementId> & orc_LastKnownHalcCrcs)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForNewFile(c_XmlParser, orc_FilePath.toStdString().c_str(),
                                                                    "opensyde-system-ui-definition");

   if (s32_Retval == C_NO_ERR)
   {
      const QFileInfo c_Info(orc_FilePath);

      //Version
      c_XmlParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XmlParser.CreateAndSelectNodeChild("nodes") == "nodes");
      c_XmlParser.SetAttributeUint32("length", orc_OscSystemDefinition.c_Nodes.size());
      if (orc_OscSystemDefinition.c_Nodes.size() == orc_UiNodes.size())
      {
         for (uint32_t u32_ItNode = 0UL;
              (u32_ItNode < orc_OscSystemDefinition.c_Nodes.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItNode)
         {
            const C_OscNode & rc_OscNode = orc_OscSystemDefinition.c_Nodes[u32_ItNode];
            const C_PuiSdNode & rc_UiNode = orc_UiNodes[u32_ItNode];
            const QString c_FolderName =
               C_OscNodeFiler::h_GetFolderName(rc_OscNode.c_Properties.c_Name.c_str()).c_str();
            QDir c_Folder = c_Info.dir();
            //Change to folder
            c_Folder.cd(c_FolderName);
            //file
            const QString c_FileName = C_PuiSdHandlerFiler::h_GetNodeUiFileName();
            s32_Retval =
               C_PuiSdHandlerFiler::mh_SaveNodeFile(rc_UiNode, rc_OscNode, c_Folder.absoluteFilePath(
                                                       c_FileName), &c_Folder);
            //Store file name
            c_XmlParser.CreateNodeChild("node",
                                        static_cast<QString>(c_FolderName + "/" + c_FileName).toStdString().c_str());
         }
      }
      else
      {
         s32_Retval = C_RD_WR;
         osc_write_log_error("Saving system definition UI", "Node count out of synchronization.");
      }
      if (s32_Retval == C_NO_ERR)
      {
         tgl_assert(c_XmlParser.SelectNodeParent() == "opensyde-system-ui-definition"); //back up

         //Bus
         c_XmlParser.CreateAndSelectNodeChild("buses");
         C_PuiSdHandlerFiler::h_SaveBuses(orc_UiBuses, c_XmlParser);
         tgl_assert(c_XmlParser.SelectNodeParent() == "opensyde-system-ui-definition"); //back up

         //GUI items

         //Bus text elements
         c_XmlParser.CreateAndSelectNodeChild("bus-text-elements");
         C_PuiSdHandlerFiler::h_SaveBusTextElements(orc_BusTextElements, c_XmlParser);
         tgl_assert(c_XmlParser.SelectNodeParent() == "opensyde-system-ui-definition"); //back up

         //Base elements
         C_PuiBsElementsFiler::h_SaveBaseElements(orc_Elements, c_XmlParser);
         C_PuiSdHandlerFiler::h_SaveLastKnownHalcCrcs(orc_LastKnownHalcCrcs, c_XmlParser);
      }
      //Don't forget to save!
      if (c_XmlParser.SaveToFile(orc_FilePath.toStdString().c_str()) != C_NO_ERR)
      {
         osc_write_log_error("Saving system definition ui", "Could not create file for ui part.");
         s32_Retval = C_RD_WR;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_RD_WR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system definition for UI

   \param[in]      orc_FilePath           File path for xml
   \param[in,out]  orc_UiNodes            UI nodes data storage
   \param[in,out]  orc_UiBuses            UI buses data storage
   \param[in,out]  orc_BusTextElements    UI bus text elements data storage
   \param[in,out]  orc_Elements           UI generic elements data storage
   \param[in,out]  orc_LastKnownHalcCrcs  Last known halc crcs
   \param[in,out]  opc_OscNodes           Core nodes

   \return
   C_NO_ERR   data was read from file
   C_NOACT    could not read data from file
   C_RANGE    file not found
   C_CONFIG   root node not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::h_LoadSystemDefinitionUiFile(const QString & orc_FilePath,
                                                          std::vector<C_PuiSdNode> & orc_UiNodes,
                                                          std::vector<C_PuiSdBus> & orc_UiBuses,
                                                          std::vector<C_PuiSdTextElementBus> & orc_BusTextElements,
                                                          stw::opensyde_gui_logic::C_PuiBsElements & orc_Elements,
                                                          std::map<C_OscNodeDataPoolListElementOptArrayId,
                                                                   C_PuiSdLastKnownHalElementId> & orc_LastKnownHalcCrcs,
                                                          std::vector<C_OscNode> * const opc_OscNodes)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForExistingFile(c_XmlParser,
                                                                         orc_FilePath.toStdString().c_str(),
                                                                         "opensyde-system-ui-definition");

   //File version
   if (c_XmlParser.SelectNodeChild("file-version") == "file-version")
   {
      uint16_t u16_FileVersion = 0U;
      try
      {
         u16_FileVersion = static_cast<uint16_t>(c_XmlParser.GetNodeContent().ToInt());
      }
      catch (...)
      {
         osc_write_log_error("Loading UI system definition", "\"file-version\" could not be converted to a number.");
         s32_Retval = C_CONFIG;
      }

      //is the file version one we know ?
      if (s32_Retval == C_NO_ERR)
      {
         osc_write_log_info("Loading UI system definition", "Value of \"file-version\": " +
                            C_SclString::IntToStr(u16_FileVersion));
         //Check file version
         if (u16_FileVersion != 1U)
         {
            osc_write_log_error("Loading UI system definition",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      c_XmlParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading UI system definition", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }

   if (s32_Retval == C_NO_ERR)
   {
      //Nodes
      if (c_XmlParser.SelectNodeChild("nodes") == "nodes")
      {
         const QFileInfo c_FileInfo(orc_FilePath);
         const QDir c_BasePath = c_FileInfo.dir();
         s32_Retval = C_PuiSdHandlerFiler::h_LoadNodes(orc_UiNodes, c_XmlParser, &c_BasePath, opc_OscNodes);
         if (s32_Retval == C_NO_ERR)
         {
            tgl_assert(c_XmlParser.SelectNodeParent() == "opensyde-system-ui-definition");
            //Buses
            if (c_XmlParser.SelectNodeChild("buses") == "buses")
            {
               s32_Retval = C_PuiSdHandlerFiler::h_LoadBuses(orc_UiBuses, c_XmlParser);
               if (s32_Retval == C_NO_ERR)
               {
                  tgl_assert(c_XmlParser.SelectNodeParent() == "opensyde-system-ui-definition");
                  //Bus text elements
                  if (c_XmlParser.SelectNodeChild("bus-text-elements") == "bus-text-elements")
                  {
                     s32_Retval = C_PuiSdHandlerFiler::h_LoadBusTextElements(orc_BusTextElements, c_XmlParser);
                     if (s32_Retval == C_NO_ERR)
                     {
                        tgl_assert(c_XmlParser.SelectNodeParent() == "opensyde-system-ui-definition");
                        //Generic elements
                        s32_Retval = C_PuiBsElementsFiler::h_LoadBaseElements(orc_Elements, c_XmlParser);
                        if (s32_Retval == C_NO_ERR)
                        {
                           s32_Retval =
                              C_PuiSdHandlerFiler::h_LoadLastKnownHalcCrcs(orc_LastKnownHalcCrcs, c_XmlParser);
                        }
                     }
                  }
                  else
                  {
                     osc_write_log_error("Loading UI system definition", "Could not find \"bus-text-elements\" node.");
                     s32_Retval = C_CONFIG;
                  }
               }
            }
            else
            {
               osc_write_log_error("Loading UI system definition", "Could not find \"buses\" node.");
               s32_Retval = C_CONFIG;
            }
         }
      }
      else
      {
         osc_write_log_error("Loading UI system definition", "Could not find \"nodes\" node.");
         s32_Retval = C_CONFIG;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get automatically generated file name

   \return
   Automatically generated file name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdHandlerFiler::h_GetNodeUiFileName(void)
{
   return "node_ui.xml";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get automatically generated file name

   \param[in]  orc_DatapoolName  Datapool name

   \return
   Automatically generated file name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdHandlerFiler::h_GetCommUiFileName(const C_SclString & orc_DatapoolName)
{
   return ("comm_" + C_OscSystemFilerUtil::h_PrepareItemNameForFileName(orc_DatapoolName) +
           "_ui.xml").c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get automatically generated file name

   \param[in]  orc_DatapoolName  Datapool name

   \return
   Automatically generated file name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdHandlerFiler::h_GetDatapoolUiFileName(const C_SclString & orc_DatapoolName)
{
   return ("dp_" + C_OscSystemFilerUtil::h_PrepareItemNameForFileName(orc_DatapoolName) +
           "_ui.xml").c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get UI system definition file name

   \param[in]  orc_SystemDefinitionCoreFilePath    File path to system definition core file

   \return
   UI system definition file name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdHandlerFiler::h_GetSystemDefinitionUiFilePath(const QString & orc_SystemDefinitionCoreFilePath)
{
   const QFileInfo c_Info(orc_SystemDefinitionCoreFilePath);

   return c_Info.dir().absoluteFilePath(c_Info.completeBaseName() + "_ui.xml");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get UI shared Datapool configuration file name

   \param[in]  orc_SystemDefinitionCoreFilePath    File path to system definition core file

   \return
   UI shared Datapool configuration file name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdHandlerFiler::h_GetSharedDatapoolUiFilePath(const QString & orc_SystemDefinitionCoreFilePath)
{
   const QFileInfo c_Info(orc_SystemDefinitionCoreFilePath);

   return c_Info.dir().absoluteFilePath("shared_datapools_ui.xml");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load datapool UI part

   \param[in,out]  orc_DataPool     Datapool UI data storage
   \param[in]      orc_FilePath     File path
   \param[in,out]  opc_OscDataPool  Core data pool

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::mh_LoadDatapoolFile(C_PuiSdNodeDataPool & orc_DataPool, const QString & orc_FilePath,
                                                 C_OscNodeDataPool * const opc_OscDataPool)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForExistingFile(c_XmlParser,
                                                                         orc_FilePath.toStdString().c_str(),
                                                                         "opensyde-dp-ui-definition");

   //File version
   if (c_XmlParser.SelectNodeChild("file-version") == "file-version")
   {
      uint16_t u16_FileVersion = 0U;
      try
      {
         u16_FileVersion = static_cast<uint16_t>(c_XmlParser.GetNodeContent().ToInt());
      }
      catch (...)
      {
         osc_write_log_error("Loading UI datapool", "\"file-version\" could not be converted to a number.");
         s32_Retval = C_CONFIG;
      }

      //is the file version one we know ?
      if (s32_Retval == C_NO_ERR)
      {
         osc_write_log_info("Loading UI datapool", "Value of \"file-version\": " +
                            C_SclString::IntToStr(u16_FileVersion));
         //Check file version
         if (u16_FileVersion != 1U)
         {
            osc_write_log_error("Loading UI datapool",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      c_XmlParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading UI datapool", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (c_XmlParser.SelectNodeChild("data-pool") == "data-pool")
      {
         s32_Retval = C_PuiSdHandlerFiler::h_LoadDataPool(orc_DataPool, c_XmlParser, opc_OscDataPool);
      }
      else
      {
         osc_write_log_error("Loading UI datapool", "Could not find \"data-pool\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load comm definition UI part

   \param[in,out]  orc_UiCanProtocol   Comm definition UI data storage
   \param[in]      orc_FilePath        File path

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::mh_LoadCommFile(C_PuiSdNodeCanProtocol & orc_UiCanProtocol, const QString & orc_FilePath)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForExistingFile(c_XmlParser,
                                                                         orc_FilePath.toStdString().c_str(),
                                                                         "opensyde-comm-ui-definition");

   //File version
   if (c_XmlParser.SelectNodeChild("file-version") == "file-version")
   {
      uint16_t u16_FileVersion = 0U;
      try
      {
         u16_FileVersion = static_cast<uint16_t>(c_XmlParser.GetNodeContent().ToInt());
      }
      catch (...)
      {
         osc_write_log_error("Loading UI comm definition", "\"file-version\" could not be converted to a number.");
         s32_Retval = C_CONFIG;
      }

      //is the file version one we know ?
      if (s32_Retval == C_NO_ERR)
      {
         osc_write_log_info("Loading UI comm definition", "Value of \"file-version\": " +
                            C_SclString::IntToStr(u16_FileVersion));
         //Check file version
         if (u16_FileVersion != 1U)
         {
            osc_write_log_error("Loading UI comm definition",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      c_XmlParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading UI comm definition", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (c_XmlParser.SelectNodeChild("com-protocol") == "com-protocol")
      {
         s32_Retval = C_PuiSdHandlerFiler::h_LoadCanProtocol(orc_UiCanProtocol, c_XmlParser);
      }
      else
      {
         osc_write_log_error("Loading UI comm definition", "Could not find \"com-protocol\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load UI node information

   Load UI node information from XML structure.
   Also uses base class function to load basic information.
   Postcondition: XMLParser has the node "node" selected.

   \param[out]     orc_Node         UI node data container
   \param[in,out]  orc_XmlParser    XMLParser with the "current" element set to the "node" element
   \param[in]      opc_BasePath     Base path (Optional for save to string)
   \param[in,out]  opc_OscNode      Core node

   \return
   C_NO_ERR    everything ok
   else        error occured while loading
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::mh_LoadNode(C_PuiSdNode & orc_Node, C_OscXmlParserBase & orc_XmlParser,
                                         const QDir * const opc_BasePath, C_OscNode * const opc_OscNode)
{
   int32_t s32_Return = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("box") == "box")
   {
      s32_Return = C_PuiBsElementsFiler::h_LoadBoxBase(orc_Node, orc_XmlParser);
      orc_XmlParser.SelectNodeParent(); //back up to "node"
   }
   if (s32_Return == C_NO_ERR)
   {
      s32_Return = h_LoadCanProtocols(orc_Node.c_UiCanProtocols, orc_XmlParser, opc_BasePath);
      if (s32_Return == C_NO_ERR)
      {
         std::vector<C_OscNodeDataPool> * pc_OscDataPools = NULL;
         if (opc_OscNode != NULL)
         {
            pc_OscDataPools = &opc_OscNode->c_DataPools;
         }
         s32_Return = h_LoadDataPools(orc_Node.c_UiDataPools, orc_XmlParser, opc_BasePath, pc_OscDataPools);

         if ((s32_Return == C_NO_ERR) && (orc_XmlParser.SelectNodeChild("busconnections") == "busconnections"))
         {
            orc_Node.c_UiBusConnections.clear();

            //go through list:
            if (orc_XmlParser.SelectNodeChild("connection") == "connection")
            {
               do
               {
                  //check type and busnumber just to verify the sequence matches the one in core:
                  C_PuiSdNodeConnectionId c_Connection;
                  C_SclString c_Text;
                  c_Text = orc_XmlParser.GetAttributeString("type");
                  if (c_Text == "ethernet")
                  {
                     c_Connection.e_InterfaceType = C_OscSystemBus::eETHERNET;
                  }
                  else if (c_Text == "can")
                  {
                     c_Connection.e_InterfaceType = C_OscSystemBus::eCAN;
                  }
                  else
                  {
                     s32_Return = C_CONFIG;
                  }
                  if (s32_Return == C_NO_ERR)
                  {
                     c_Connection.u8_InterfaceNumber =
                        static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("busnumber"));

                     {
                        C_PuiSdNodeConnection c_UiConnection;

                        //Set ID
                        c_UiConnection.c_ConnectionId = c_Connection;

                        //sequence OK; now get points:
                        if (orc_XmlParser.SelectNodeChild("interaction-points") == "interaction-points")
                        {
                           //file through children:
                           c_Text = orc_XmlParser.SelectNodeChild("interaction-point");

                           if (c_Text == "interaction-point")
                           {
                              do
                              {
                                 const QPointF c_Point(orc_XmlParser.GetAttributeFloat64("x"),
                                                       orc_XmlParser.GetAttributeFloat64("y"));
                                 c_UiConnection.c_UiNodeConnectionInteractionPoints.push_back(c_Point);
                                 c_Text = orc_XmlParser.SelectNodeNext("interaction-point");
                              }
                              while (c_Text == "interaction-point");
                              tgl_assert(orc_XmlParser.SelectNodeParent() == "interaction-points");
                           }

                           orc_Node.c_UiBusConnections.push_back(c_UiConnection);
                           tgl_assert(orc_XmlParser.SelectNodeParent() == "connection");
                        }
                        else
                        {
                           s32_Return = C_CONFIG;
                        }
                     }
                  }
               }
               while ((s32_Return == C_NO_ERR) && (orc_XmlParser.SelectNodeNext("connection") == "connection"));
               //any more connections ?

               tgl_assert(orc_XmlParser.SelectNodeParent() == "busconnections"); //back to busconnections
            }
            orc_XmlParser.SelectNodeParent(); //back to node
         }
         else
         {
            s32_Return = C_CONFIG;
         }
      }
   }

   //also load information for base class:
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node

   Save node to XML file

   \param[in]      orc_UiNode    UI data storage
   \param[in]      orc_OscNode   OSC data storage
   \param[in,out]  orc_FilePath  File path for xml
   \param[in]      opc_BasePath  Base path (Optional for save to string)

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::mh_SaveNodeFile(const C_PuiSdNode & orc_UiNode, const C_OscNode & orc_OscNode,
                                             const QString & orc_FilePath, const QDir * const opc_BasePath)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForNewFile(c_XmlParser, orc_FilePath.toStdString().c_str(),
                                                                    "opensyde-node-ui-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XmlParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XmlParser.CreateAndSelectNodeChild("node") == "node");
      //node
      s32_Retval = C_PuiSdHandlerFiler::mh_SaveNode(orc_UiNode, &orc_OscNode, opc_BasePath, c_XmlParser);
      if (s32_Retval == C_NO_ERR)
      {
         //Don't forget to save!
         if (c_XmlParser.SaveToFile(orc_FilePath.toStdString().c_str()) != C_NO_ERR)
         {
            osc_write_log_error("Saving system definition UI", "Could not create file for node.");
            s32_Retval = C_CONFIG;
         }
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save UI node information

   Save UI node information to XML structure
   Also uses base class function to save basic information.
   Postcondition: XMLParser has the node "node" selected.

   \param[in]      orc_UiNode       UI node data container
   \param[in]      opc_OscNode      OSC node data container
   \param[in]      opc_BasePath     Base path (Optional for save to string)
   \param[in,out]  orc_XmlParser    XMLParser with the "current" element set to the "node" element

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::mh_SaveNode(const C_PuiSdNode & orc_UiNode, const C_OscNode * const opc_OscNode,
                                         const QDir * const opc_BasePath, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval;

   if (opc_OscNode != NULL)
   {
      s32_Retval = h_SaveCanProtocols(orc_UiNode.c_UiCanProtocols, &opc_OscNode->c_ComProtocols,
                                      &opc_OscNode->c_DataPools, opc_BasePath, orc_XmlParser);
   }
   else
   {
      s32_Retval = h_SaveCanProtocols(orc_UiNode.c_UiCanProtocols, NULL, NULL, opc_BasePath, orc_XmlParser);
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (opc_OscNode != NULL)
      {
         s32_Retval = h_SaveDataPools(orc_UiNode.c_UiDataPools, &opc_OscNode->c_DataPools, opc_BasePath, orc_XmlParser);
      }
      else
      {
         s32_Retval = h_SaveDataPools(orc_UiNode.c_UiDataPools, NULL, opc_BasePath, orc_XmlParser);
      }
      if (s32_Retval == C_NO_ERR)
      {
         orc_XmlParser.CreateAndSelectNodeChild("box");
         C_PuiBsElementsFiler::h_SaveBoxBase(orc_UiNode, orc_XmlParser);
         orc_XmlParser.SelectNodeParent();

         orc_XmlParser.CreateAndSelectNodeChild("busconnections");
         for (uint16_t u16_Bus = 0U; u16_Bus < orc_UiNode.c_UiBusConnections.size(); u16_Bus++)
         {
            const C_PuiSdNodeConnection & rc_Bus = orc_UiNode.c_UiBusConnections[u16_Bus];

            orc_XmlParser.CreateAndSelectNodeChild("connection");

            if (rc_Bus.c_ConnectionId.e_InterfaceType == C_OscSystemBus::eCAN)
            {
               orc_XmlParser.SetAttributeString("type", "can");
            }
            else if (rc_Bus.c_ConnectionId.e_InterfaceType == C_OscSystemBus::eETHERNET)
            {
               orc_XmlParser.SetAttributeString("type", "ethernet");
            }
            else
            {
               //weird ...
            }
            orc_XmlParser.SetAttributeUint32("busnumber", rc_Bus.c_ConnectionId.u8_InterfaceNumber);

            orc_XmlParser.CreateAndSelectNodeChild("interaction-points");

            for (uint16_t u16_Point = 0U;
                 u16_Point < rc_Bus.c_UiNodeConnectionInteractionPoints.size(); u16_Point++)
            {
               orc_XmlParser.CreateAndSelectNodeChild("interaction-point");
               orc_XmlParser.SetAttributeFloat64("x", rc_Bus.c_UiNodeConnectionInteractionPoints[u16_Point].x());
               orc_XmlParser.SetAttributeFloat64("y", rc_Bus.c_UiNodeConnectionInteractionPoints[u16_Point].y());
               orc_XmlParser.SelectNodeParent(); //back to interaction-points
            }

            tgl_assert(orc_XmlParser.SelectNodeParent() == "connection");     //back to connection
            tgl_assert(orc_XmlParser.SelectNodeParent() == "busconnections"); //back to busconnections
         }

         tgl_assert(orc_XmlParser.SelectNodeParent() == "node"); //back to "node"
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load GUI bus information

   Load GUI-specific information about bus.
   Also uses base class function to load basic information.

   \param[in,out]  orc_Bus          UI bus data container
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "bus" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::mh_LoadBus(C_PuiSdBus & orc_Bus, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Return = C_NO_ERR;

   //GUI information
   if (orc_XmlParser.SelectNodeChild("line") == "line")
   {
      s32_Return = C_PuiBsElementsFiler::h_LoadLineBase(orc_Bus, orc_XmlParser);
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   if (orc_XmlParser.SelectNodeNext("color-middle-line") == "color-middle-line")
   {
      C_PuiBsElementsFiler::h_LoadColor(orc_Bus.c_UiColorMiddleLine, orc_XmlParser);
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   orc_XmlParser.SelectNodeParent(); //back to bus

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save GUI bus information

   Save GUI-specific information about bus.
   Also uses base class function to save basic information.

   \param[in]      orc_Bus          UI bus data container
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "bus" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::mh_SaveBus(const C_PuiSdBus & orc_Bus, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("line");
   C_PuiBsElementsFiler::h_SaveLineBase(orc_Bus, orc_XmlParser);
   orc_XmlParser.SelectNodeParent(); //back to "gui"

   orc_XmlParser.CreateAndSelectNodeChild("color-middle-line");
   C_PuiBsElementsFiler::h_SaveColor(orc_Bus.c_UiColorMiddleLine, orc_XmlParser);

   orc_XmlParser.SelectNodeParent(); //back to "bus"
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load text element attributes

   \param[in,out]  opc_TextElement  Text element data element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "text-element" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerFiler::mh_LoadTextElement(C_PuiBsTextElement * const opc_TextElement,
                                                stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Return;

   C_PuiSdTextElementBus * const opc_BusTextElement =
      dynamic_cast<C_PuiSdTextElementBus * const>(opc_TextElement);

   s32_Return = C_PuiBsElementsFiler::h_LoadTextElement(opc_TextElement, orc_XmlParser);
   //Check bus
   if (opc_BusTextElement != NULL)
   {
      opc_BusTextElement->u32_BusIndex = orc_XmlParser.GetAttributeUint32("bus-index");
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save text element

   \param[in]      opc_TextElement  Text element data element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "text-element" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::mh_SaveTextElement(const C_PuiBsTextElement * const opc_TextElement,
                                             stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   const C_PuiSdTextElementBus * const opc_BusTextElement =
      dynamic_cast<const C_PuiSdTextElementBus * const>(opc_TextElement);

   C_PuiBsElementsFiler::h_SaveTextElement(opc_TextElement, orc_XmlParser);
   //Check bus
   if (opc_BusTextElement != NULL)
   {
      orc_XmlParser.SetAttributeUint32("bus-index", opc_BusTextElement->u32_BusIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get array elem if available

   \param[in,out]  opc_Vector    Vector
   \param[in]      ou32_Index    Index

   \return
   Array elem if available
*/
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T * C_PuiSdHandlerFiler::mh_GetArrayElemIfAvailable(std::vector<T> * const opc_Vector, const uint32_t ou32_Index)
{
   T * pc_El = NULL;

   if (opc_Vector != NULL)
   {
      if (ou32_Index < opc_Vector->size())
      {
         pc_El = &((*opc_Vector)[ou32_Index]);
      }
   }
   return pc_El;
}
