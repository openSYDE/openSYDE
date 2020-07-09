//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI topology filer (implementation)

   UI topology filer

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QBuffer>
#include <QFileInfo>
#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_PuiSdHandler.h"
#include "C_OSCNodeFiler.h"
#include "C_OSCSystemDefinitionFiler.h"
#include "C_OSCSystemFilerUtil.h"
#include "C_OSCLoggingHandler.h"
#include "C_PuiSdHandlerFiler.h"
#include "C_PuiBsElementsFiler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_scl;
using namespace stw_opensyde_gui_logic;
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
C_PuiSdHandlerFiler::C_PuiSdHandlerFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pools

   \param[in,out]  orc_DataPools    Data pool elements (Cleared if necessary)
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "node" element
   \param[in]      opc_BasePath     Base path (Optional for save to string)

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadDataPools(std::vector<C_PuiSdNodeDataPool> & orc_DataPools,
                                            C_OSCXMLParserBase & orc_XMLParser, const QDir * const opc_BasePath)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last data pools
   orc_DataPools.clear();
   if (orc_XMLParser.SelectNodeChild("data-pools") == "data-pools")
   {
      C_SCLString c_CurrentDataPoolNode;
      uint32 u32_ExpectedSize = 0UL;
      const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
         orc_DataPools.reserve(u32_ExpectedSize);
      }

      c_CurrentDataPoolNode = orc_XMLParser.SelectNodeChild("data-pool");
      if (c_CurrentDataPoolNode == "data-pool")
      {
         do
         {
            C_PuiSdNodeDataPool c_Datapool;
            if (opc_BasePath != NULL)
            {
               const QString c_FilePath = opc_BasePath->absoluteFilePath(orc_XMLParser.GetNodeContent().c_str());
               s32_Retval = mh_LoadDatapoolFile(c_Datapool, c_FilePath);
            }
            else
            {
               s32_Retval = h_LoadDataPool(c_Datapool, orc_XMLParser);
            }
            orc_DataPools.push_back(c_Datapool);
            //Next
            c_CurrentDataPoolNode = orc_XMLParser.SelectNodeNext("data-pool");
         }
         while ((s32_Retval == C_NO_ERR) && (c_CurrentDataPoolNode == "data-pool"));
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pools");
         }
      }
      //Compare length
      if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
      {
         if (u32_ExpectedSize != orc_DataPools.size())
         {
            C_SCLString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected UI Datapool count, expected: %i, got %i", u32_ExpectedSize,
                                 orc_DataPools.size());
            osc_write_log_warning("Load file", c_Tmp.c_str());
         }
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
/*! \brief   Load data pool

   \param[in,out]  orc_DataPool     Data pool element
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "data-pool" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadDataPool(C_PuiSdNodeDataPool & orc_DataPool, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("lists") == "lists")
   {
      s32_Retval = h_LoadDataPoolLists(orc_DataPool.c_DataPoolLists, orc_XMLParser);
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool");
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
   \param[in,out]  orc_XMLParser       XML parser with the "current" element set to the "lists" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadDataPoolLists(std::vector<C_PuiSdNodeDataPoolList> & orc_DataPoolLists,
                                                C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_CurrentDataPoolListNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_DataPoolLists.reserve(u32_ExpectedSize);
   }

   c_CurrentDataPoolListNode = orc_XMLParser.SelectNodeChild("list");

   orc_DataPoolLists.clear();
   if (c_CurrentDataPoolListNode == "list")
   {
      do
      {
         C_PuiSdNodeDataPoolList c_List;
         s32_Retval = h_LoadDataPoolList(c_List, orc_XMLParser);
         orc_DataPoolLists.push_back(c_List);

         //Next
         c_CurrentDataPoolListNode = orc_XMLParser.SelectNodeNext("list");
      }
      while ((s32_Retval == C_NO_ERR) && (c_CurrentDataPoolListNode == "list"));
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "lists");
      }
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_DataPoolLists.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI list count, expected: %i, got %i", u32_ExpectedSize,
                              orc_DataPoolLists.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pool list

   \param[in,out]  orc_DataPoolList    Data pool list element
   \param[in,out]  orc_XMLParser       XML parser with the "current" element set to the "list" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadDataPoolList(C_PuiSdNodeDataPoolList & orc_DataPoolList,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("data-elements") == "data-elements")
   {
      h_LoadDataPoolListElements(orc_DataPoolList.c_DataPoolListElements, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "list");
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
   \param[in,out]  orc_XMLParser             XML parser with the "current" element set to the "data-elements" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadDataPoolListElements(
   std::vector<C_PuiSdNodeDataPoolListElement> & orc_DataPoolListElements, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_CurrentDataPoolListElementNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_DataPoolListElements.reserve(u32_ExpectedSize);
   }

   c_CurrentDataPoolListElementNode = orc_XMLParser.SelectNodeChild("data-element");

   orc_DataPoolListElements.clear();
   if (c_CurrentDataPoolListElementNode == "data-element")
   {
      do
      {
         C_PuiSdNodeDataPoolListElement c_Element;
         h_LoadDataPoolListElement(c_Element, orc_XMLParser);
         orc_DataPoolListElements.push_back(c_Element);

         //Next
         c_CurrentDataPoolListElementNode = orc_XMLParser.SelectNodeNext("data-element");
      }
      while (c_CurrentDataPoolListElementNode == "data-element");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-elements");
   }
   //Compare length
   if (q_ExpectedSizeHere == true)
   {
      if (u32_ExpectedSize != orc_DataPoolListElements.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI data element count, expected: %i, got %i", u32_ExpectedSize,
                              orc_DataPoolListElements.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pool list element

   \param[in,out]  orc_DataPoolListElement   Data pool list element
   \param[in,out]  orc_XMLParser             XML parser with the "current" element set to the "data-element" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_LoadDataPoolListElement(C_PuiSdNodeDataPoolListElement & orc_DataPoolListElement,
                                                    const C_OSCXMLParserBase & orc_XMLParser)
{
   orc_DataPoolListElement.q_AutoMinMaxActive = orc_XMLParser.GetAttributeBool("auto_min_max_active");
   orc_DataPoolListElement.q_InterpretAsString = orc_XMLParser.GetAttributeBool("interpret_as_string");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pools

   \param[in]      orc_UIDataPools     UI data pool elements
   \param[in]      opc_OSCDataPools    OSC data pool elements (Optional for save to string)
   \param[in]      opc_BasePath        Base path (Optional for save to string)
   \param[in,out]  orc_XMLParser       XML parser with the "current" element set to the "node" element

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_SaveDataPools(const std::vector<C_PuiSdNodeDataPool> & orc_UIDataPools,
                                            const std::vector<C_OSCNodeDataPool> * const opc_OSCDataPools,
                                            const QDir * const opc_BasePath, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if ((opc_OSCDataPools != NULL) && (opc_BasePath != NULL))
   {
      if (orc_UIDataPools.size() == opc_OSCDataPools->size())
      {
         orc_XMLParser.CreateAndSelectNodeChild("data-pools");
         orc_XMLParser.SetAttributeUint32("length", orc_UIDataPools.size());
         for (uint32 u32_ItDataPool = 0; (u32_ItDataPool < orc_UIDataPools.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItDataPool)
         {
            const C_OSCNodeDataPool & rc_OSCDatapool = (*opc_OSCDataPools)[u32_ItDataPool];
            const QString c_FileName = C_PuiSdHandlerFiler::h_GetDatapoolUiFileName(rc_OSCDatapool.c_Name);
            orc_XMLParser.CreateAndSelectNodeChild("data-pool");
            s32_Retval =
               h_SaveDataPoolFile(orc_UIDataPools[u32_ItDataPool], opc_BasePath->absoluteFilePath(c_FileName));
            //Store file name
            orc_XMLParser.SetNodeContent(c_FileName.toStdString().c_str());
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pools");
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
      }
      else
      {
         s32_Retval = C_CONFIG;
         osc_write_log_error("Saving system definition UI", "datapool vector sizes don't match");
      }
   }
   else
   {
      orc_XMLParser.CreateAndSelectNodeChild("data-pools");
      orc_XMLParser.SetAttributeUint32("length", orc_UIDataPools.size());
      for (uint32 u32_ItDataPool = 0; u32_ItDataPool < orc_UIDataPools.size(); ++u32_ItDataPool)
      {
         orc_XMLParser.CreateAndSelectNodeChild("data-pool");
         h_SaveDataPool(orc_UIDataPools[u32_ItDataPool], orc_XMLParser);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pools");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
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
sint32 C_PuiSdHandlerFiler::h_SaveDataPoolFile(const C_PuiSdNodeDataPool & orc_DataPool, const QString & orc_FilePath)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForNewFile(c_XMLParser, orc_FilePath.toStdString().c_str(),
                                                                   "opensyde-dp-ui-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XMLParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XMLParser.CreateAndSelectNodeChild("data-pool") == "data-pool");
      //node
      C_PuiSdHandlerFiler::h_SaveDataPool(orc_DataPool, c_XMLParser);
      //Don't forget to save!
      if (c_XMLParser.SaveToFile(orc_FilePath.toStdString().c_str()) != C_NO_ERR)
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
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "data-pool" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveDataPool(const C_PuiSdNodeDataPool & orc_DataPool, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("lists");

   h_SaveDataPoolLists(orc_DataPool.c_DataPoolLists, orc_XMLParser);

   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool lists

   \param[in]      orc_DataPoolLists   Data pool lists
   \param[in,out]  orc_XMLParser       XML parser with the "current" element set to the "lists" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveDataPoolLists(const std::vector<C_PuiSdNodeDataPoolList> & orc_DataPoolLists,
                                              C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_DataPoolLists.size());
   for (uint32 u32_ItDataPoolList = 0; u32_ItDataPoolList < orc_DataPoolLists.size();
        ++u32_ItDataPoolList)
   {
      orc_XMLParser.CreateAndSelectNodeChild("list");
      h_SaveDataPoolList(orc_DataPoolLists[u32_ItDataPoolList], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "lists");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool list

   \param[in]      orc_DataPoolList    Data pool list element
   \param[in,out]  orc_XMLParser       XML parser with the "current" element set to the "list" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveDataPoolList(const C_PuiSdNodeDataPoolList & orc_DataPoolList,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("data-elements");
   h_SaveDataPoolListElements(orc_DataPoolList.c_DataPoolListElements, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "list");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool list elements

   \param[in]      orc_DataPoolListElements  Data pool list elements
   \param[in,out]  orc_XMLParser             XML parser with the "current" element set to the "data-elements" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveDataPoolListElements(
   const std::vector<C_PuiSdNodeDataPoolListElement> & orc_DataPoolListElements, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_DataPoolListElements.size());
   for (uint32 u32_ItDataPoolListElement = 0;
        u32_ItDataPoolListElement < orc_DataPoolListElements.size();
        ++u32_ItDataPoolListElement)
   {
      orc_XMLParser.CreateAndSelectNodeChild("data-element");
      h_SaveDataPoolListElement(orc_DataPoolListElements[u32_ItDataPoolListElement], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-elements");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool list element

   \param[in]      orc_DataPoolListElement   Data pool list element
   \param[in,out]  orc_XMLParser             XML parser with the "current" element set to the "data-element" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveDataPoolListElement(const C_PuiSdNodeDataPoolListElement & orc_DataPoolListElement,
                                                    C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeBool("auto_min_max_active", orc_DataPoolListElement.q_AutoMinMaxActive);
   orc_XMLParser.SetAttributeBool("interpret_as_string", orc_DataPoolListElement.q_InterpretAsString);
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
sint32 C_PuiSdHandlerFiler::h_LoadSharedDatapoolsFile(const QString & orc_FilePath,
                                                      C_PuiSdSharedDatapools & orc_SharedDatapools)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForExistingFile(c_XMLParser,
                                                                        orc_FilePath.toStdString().c_str(),
                                                                        "opensyde-shared-datapools-ui-definition");

   orc_SharedDatapools.c_SharedDatapools.clear();

   if (s32_Retval == C_NO_ERR)
   {
      if (c_XMLParser.SelectNodeChild("shared_datapools_groups") == "shared_datapools_groups")
      {
         C_PuiSdHandlerFiler::h_LoadSharedDatapoolsGroups(orc_SharedDatapools, c_XMLParser);
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
   \param[in,out]  orc_XMLParser          XML parser with the "current" element set to the "shared_datapools_groups" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_LoadSharedDatapoolsGroups(C_PuiSdSharedDatapools & orc_SharedDatapools,
                                                      C_OSCXMLParserBase & orc_XMLParser)
{
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_SharedDatapools.c_SharedDatapools.reserve(u32_ExpectedSize);
   }

   if (orc_XMLParser.SelectNodeChild("group") == "group")
   {
      C_SCLString c_CurrentNode;

      do
      {
         std::vector<stw_opensyde_core::C_OSCNodeDataPoolId> c_Group;

         C_PuiSdHandlerFiler::h_LoadSharedDatapoolsGroup(c_Group, orc_XMLParser);

         if (c_Group.size() > 0)
         {
            orc_SharedDatapools.c_SharedDatapools.push_back(c_Group);
         }

         //Next
         c_CurrentNode = orc_XMLParser.SelectNodeNext("group");
      }
      while (c_CurrentNode == "group");

      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "shared_datapools_groups");
   }

   //Compare length
   if (q_ExpectedSizeHere == true)
   {
      if (u32_ExpectedSize != orc_SharedDatapools.c_SharedDatapools.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI shared Datapools group count, expected: %i, got %i", u32_ExpectedSize,
                              orc_SharedDatapools.c_SharedDatapools.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   short description of function

   long description of function within several lines

   \param[out]     orc_Group        Read shared Datapool group
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "shared_datapools_groups" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_LoadSharedDatapoolsGroup(std::vector<C_OSCNodeDataPoolId> & orc_Group,
                                                     C_OSCXMLParserBase & orc_XMLParser)
{
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_Group.reserve(u32_ExpectedSize);
   }

   if (orc_XMLParser.SelectNodeChild("datapool-id") == "datapool-id")
   {
      C_SCLString c_CurrentNode;

      do
      {
         stw_opensyde_core::C_OSCNodeDataPoolId c_DatapoolId;

         c_DatapoolId.u32_NodeIndex = orc_XMLParser.GetAttributeUint32("node");
         c_DatapoolId.u32_DataPoolIndex = orc_XMLParser.GetAttributeUint32("datapool");

         orc_Group.push_back(c_DatapoolId);

         //Next
         c_CurrentNode = orc_XMLParser.SelectNodeNext("datapool-id");
      }
      while (c_CurrentNode == "datapool-id");

      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "group");
   }

   //Compare length
   if (q_ExpectedSizeHere == true)
   {
      if (u32_ExpectedSize != orc_Group.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI shared Datapool Ids count in group, expected: %i, got %i",
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
sint32 C_PuiSdHandlerFiler::h_SaveSharedDatapoolsFile(const QString & orc_FilePath,
                                                      const C_PuiSdSharedDatapools & orc_SharedDatapools)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForNewFile(c_XMLParser, orc_FilePath.toStdString().c_str(),
                                                                   "opensyde-shared-datapools-ui-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XMLParser.CreateNodeChild("file-version", "1");

      // Shared Datapool groups
      tgl_assert(c_XMLParser.CreateAndSelectNodeChild("shared_datapools_groups") == "shared_datapools_groups");
      C_PuiSdHandlerFiler::h_SaveSharedDatapoolsGroups(orc_SharedDatapools, c_XMLParser);

      //Don't forget to save!
      if (c_XMLParser.SaveToFile(orc_FilePath.toStdString().c_str()) != C_NO_ERR)
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
   \param[in,out]  orc_XMLParser          XML parser with the "current" element set to the "shared_datapools_groups" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveSharedDatapoolsGroups(const C_PuiSdSharedDatapools & orc_SharedDatapools,
                                                      C_OSCXMLParserBase & orc_XMLParser)
{
   uint32 u32_GroupCounter;
   uint32 u32_DatapoolIdCounter;

   orc_XMLParser.SetAttributeUint32("length", orc_SharedDatapools.c_SharedDatapools.size());

   // Iterate over all different groups
   for (u32_GroupCounter = 0U; u32_GroupCounter < orc_SharedDatapools.c_SharedDatapools.size(); ++u32_GroupCounter)
   {
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolId> & rc_Group =
         orc_SharedDatapools.c_SharedDatapools[u32_GroupCounter];

      orc_XMLParser.CreateAndSelectNodeChild("group");
      orc_XMLParser.SetAttributeUint32("length", rc_Group.size());

      // All Datapool Ids of the group
      for (u32_DatapoolIdCounter = 0U; u32_DatapoolIdCounter < rc_Group.size(); ++u32_DatapoolIdCounter)
      {
         orc_XMLParser.CreateAndSelectNodeChild("datapool-id");
         orc_XMLParser.SetAttributeUint32("node", rc_Group[u32_DatapoolIdCounter].u32_NodeIndex);
         orc_XMLParser.SetAttributeUint32("datapool", rc_Group[u32_DatapoolIdCounter].u32_DataPoolIndex);

         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "group");
      }

      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "shared_datapools_groups");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can protocols

   \param[in,out]  orc_CanProtocols    Can protocols (Cleared if necessary)
   \param[in,out]  orc_XMLParser       XML parser with the "current" element set to the "node" element
   \param[in]      opc_BasePath        Base path (Optional for save to string)

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadCanProtocols(std::vector<C_PuiSdNodeCanProtocol> & orc_CanProtocols,
                                               C_OSCXMLParserBase & orc_XMLParser, const QDir * const opc_BasePath)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last can protocols
   orc_CanProtocols.clear();
   if (orc_XMLParser.SelectNodeChild("com-protocols") == "com-protocols")
   {
      C_SCLString c_CurrentCanProtocolNode;
      uint32 u32_ExpectedSize = 0UL;
      const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
         orc_CanProtocols.reserve(u32_ExpectedSize);
      }

      c_CurrentCanProtocolNode = orc_XMLParser.SelectNodeChild("com-protocol");
      if (c_CurrentCanProtocolNode == "com-protocol")
      {
         do
         {
            C_PuiSdNodeCanProtocol c_CanProtocol;
            if (opc_BasePath != NULL)
            {
               const QString c_FilePath = opc_BasePath->absoluteFilePath(orc_XMLParser.GetNodeContent().c_str());
               s32_Retval = mh_LoadCommFile(c_CanProtocol, c_FilePath);
            }
            else
            {
               s32_Retval = h_LoadCanProtocol(c_CanProtocol, orc_XMLParser);
            }
            orc_CanProtocols.push_back(c_CanProtocol);

            //Next
            c_CurrentCanProtocolNode = orc_XMLParser.SelectNodeNext("com-protocol");
         }
         while ((s32_Retval == C_NO_ERR) && (c_CurrentCanProtocolNode == "com-protocol"));
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocols");
         }
      }
      //Compare length
      if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
      {
         if (u32_ExpectedSize != orc_CanProtocols.size())
         {
            C_SCLString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected UI protocol count, expected: %i, got %i", u32_ExpectedSize,
                                 orc_CanProtocols.size());
            osc_write_log_warning("Load file", c_Tmp.c_str());
         }
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
/*! \brief   Load can protocol

   \param[in,out]  orc_CanProtocol  Can protocol element
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "com-protocol" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadCanProtocol(C_PuiSdNodeCanProtocol & orc_CanProtocol,
                                              C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("com-message-containers") == "com-message-containers")
   {
      s32_Retval = h_LoadCanMessageContainers(orc_CanProtocol.c_ComMessages, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocol");
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
   \param[in,out]  orc_XMLParser             XML parser with the "current" element set
                                           to the "com-message-containers" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadCanMessageContainers(
   std::vector<C_PuiSdNodeCanMessageContainer> & orc_CanMessageContainers, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_CurrentCanMessageContainerNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_CanMessageContainers.reserve(u32_ExpectedSize);
   }

   c_CurrentCanMessageContainerNode = orc_XMLParser.SelectNodeChild("com-message-container");

   orc_CanMessageContainers.clear();
   if (c_CurrentCanMessageContainerNode == "com-message-container")
   {
      do
      {
         if (s32_Retval == C_NO_ERR)
         {
            C_PuiSdNodeCanMessageContainer c_CanMessageContainer;
            s32_Retval =
               h_LoadCanMessageContainer(c_CanMessageContainer, orc_XMLParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_CanMessageContainers.push_back(c_CanMessageContainer);
            }
         }
         //Next
         c_CurrentCanMessageContainerNode = orc_XMLParser.SelectNodeNext("com-message-container");
      }
      while (c_CurrentCanMessageContainerNode == "com-message-container");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-containers");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_CanMessageContainers.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected data element count, expected: %i, got %i", u32_ExpectedSize,
                              orc_CanMessageContainers.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can message container

   \param[in,out]  orc_CanMessageContainer   Can message container element
   \param[in,out]  orc_XMLParser             XML parser with the "current" element set
                                             to the "com-message-container " element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadCanMessageContainer(C_PuiSdNodeCanMessageContainer & orc_CanMessageContainer,
                                                      C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval;

   if (orc_XMLParser.SelectNodeChild("tx-messages") == "tx-messages")
   {
      s32_Retval = h_LoadCanMessages(orc_CanMessageContainer.c_TxMessages, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-container");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if ((orc_XMLParser.SelectNodeChild("rx-messages") == "rx-messages") && (s32_Retval == C_NO_ERR))
   {
      s32_Retval = h_LoadCanMessages(orc_CanMessageContainer.c_RxMessages, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-container");
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
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to unknown element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadCanMessages(std::vector<C_PuiSdNodeCanMessage> & orc_CanMessages,
                                              C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_CurrentCanMessageNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_CanMessages.reserve(u32_ExpectedSize);
   }

   c_CurrentCanMessageNode = orc_XMLParser.SelectNodeChild("com-message");

   orc_CanMessages.clear();
   if (c_CurrentCanMessageNode == "com-message")
   {
      do
      {
         if (s32_Retval == C_NO_ERR)
         {
            C_PuiSdNodeCanMessage c_CanMessage;
            s32_Retval = h_LoadCanMessage(c_CanMessage, orc_XMLParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_CanMessages.push_back(c_CanMessage);
            }
         }
         //Next
         c_CurrentCanMessageNode = orc_XMLParser.SelectNodeNext("com-message");
      }
      while (c_CurrentCanMessageNode == "com-message");
      //Return
      orc_XMLParser.SelectNodeParent();
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_CanMessages.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI messages count, expected: %i, got %i", u32_ExpectedSize,
                              orc_CanMessages.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can message

   \param[in,out]  orc_CanMessage   Can message element
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "com-message" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadCanMessage(C_PuiSdNodeCanMessage & orc_CanMessage, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   bool q_UseAutoReceiveTimeout;
   bool q_ReceiveTimeoutDisabled;

   //Attributes
   if (orc_XMLParser.AttributeExists("use-auto-receive-timeout") == true)
   {
      q_UseAutoReceiveTimeout = orc_XMLParser.GetAttributeBool("use-auto-receive-timeout");
   }
   else
   {
      q_UseAutoReceiveTimeout = true;
   }

   if (orc_XMLParser.AttributeExists("receive-timeout-disabled") == true)
   {
      q_ReceiveTimeoutDisabled = orc_XMLParser.GetAttributeBool("receive-timeout-disabled");
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
   if (orc_XMLParser.SelectNodeChild("com-signals") == "com-signals")
   {
      h_LoadCanSignals(orc_CanMessage.c_Signals, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message");
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
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "com-signals" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_LoadCanSignals(std::vector<C_PuiSdNodeCanSignal> & orc_CanSignals,
                                           C_OSCXMLParserBase & orc_XMLParser)
{
   C_SCLString c_CurrentDataPoolListElementNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_CanSignals.reserve(u32_ExpectedSize);
   }

   c_CurrentDataPoolListElementNode = orc_XMLParser.SelectNodeChild("com-signal");

   if (c_CurrentDataPoolListElementNode == "com-signal")
   {
      do
      {
         C_PuiSdNodeCanSignal c_CanSignal;
         h_LoadCanSignal(c_CanSignal, orc_XMLParser);
         orc_CanSignals.push_back(c_CanSignal);

         //Next
         c_CurrentDataPoolListElementNode = orc_XMLParser.SelectNodeNext("com-signal");
      }
      while (c_CurrentDataPoolListElementNode == "com-signal");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-signals");
   }
   //Compare length
   if (q_ExpectedSizeHere == true)
   {
      if (u32_ExpectedSize != orc_CanSignals.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected data element count, expected: %i, got %i", u32_ExpectedSize,
                              orc_CanSignals.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can signal element

   \param[in,out]  orc_CanSignal    Can signal element
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "com-signal" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_LoadCanSignal(C_PuiSdNodeCanSignal & orc_CanSignal,
                                          const C_OSCXMLParserBase & orc_XMLParser)
{
   orc_CanSignal.u8_ColorIndex = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("color-index"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can protocols

   \param[in]      orc_UICanProtocols     UI can protocols
   \param[in]      opc_OSCCanProtocols    OSC can protocols (Optional for save to string)
   \param[in]      opc_OSCDatapools       OSC datapools (Optional for save to string)
   \param[in]      opc_BasePath           Base path (Optional for save to string)
   \param[in,out]  orc_XMLParser          XML parser with the "current" element set to the "node" element

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_SaveCanProtocols(const std::vector<C_PuiSdNodeCanProtocol> & orc_UICanProtocols,
                                               const std::vector<C_OSCCanProtocol> * const opc_OSCCanProtocols,
                                               const std::vector<C_OSCNodeDataPool> * const opc_OSCDatapools,
                                               const QDir * const opc_BasePath, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (((opc_OSCCanProtocols != NULL) && (opc_OSCDatapools != NULL)) && (opc_BasePath != NULL))
   {
      if (orc_UICanProtocols.size() == opc_OSCCanProtocols->size())
      {
         orc_XMLParser.CreateAndSelectNodeChild("com-protocols");
         orc_XMLParser.SetAttributeUint32("length", orc_UICanProtocols.size());
         for (uint32 u32_ItCanProtocol = 0; (u32_ItCanProtocol < orc_UICanProtocols.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItCanProtocol)
         {
            const C_OSCCanProtocol & rc_OSCProtocol = (*opc_OSCCanProtocols)[u32_ItCanProtocol];
            if (rc_OSCProtocol.u32_DataPoolIndex < opc_OSCDatapools->size())
            {
               const C_OSCNodeDataPool & rc_Datapool = (*opc_OSCDatapools)[rc_OSCProtocol.u32_DataPoolIndex];
               const QString c_FileName = C_PuiSdHandlerFiler::h_GetCommUiFileName(rc_Datapool.c_Name);
               orc_XMLParser.CreateAndSelectNodeChild("com-protocol");
               s32_Retval =
                  h_SaveCanProtocolFile(orc_UICanProtocols[u32_ItCanProtocol], opc_BasePath->absoluteFilePath(
                                           c_FileName));
               //Store file name
               orc_XMLParser.SetNodeContent(c_FileName.toStdString().c_str());
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocols");
            }
            else
            {
               s32_Retval = C_CONFIG;
               osc_write_log_error("Saving system definition UI", "protocol datapool index invalid");
            }
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
      }
      else
      {
         s32_Retval = C_CONFIG;
         osc_write_log_error("Saving system definition UI", "protocol vector sizes don't match");
      }
   }
   else
   {
      orc_XMLParser.CreateAndSelectNodeChild("com-protocols");
      orc_XMLParser.SetAttributeUint32("length", orc_UICanProtocols.size());
      for (uint32 u32_ItCanProtocol = 0; u32_ItCanProtocol < orc_UICanProtocols.size(); ++u32_ItCanProtocol)
      {
         orc_XMLParser.CreateAndSelectNodeChild("com-protocol");
         h_SaveCanProtocol(orc_UICanProtocols[u32_ItCanProtocol], orc_XMLParser);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocols");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
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
sint32 C_PuiSdHandlerFiler::h_SaveCanProtocolFile(const C_PuiSdNodeCanProtocol & orc_CanProtocol,
                                                  const QString & orc_FilePath)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForNewFile(c_XMLParser, orc_FilePath.toStdString().c_str(),
                                                                   "opensyde-comm-ui-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XMLParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XMLParser.CreateAndSelectNodeChild("com-protocol") == "com-protocol");
      //node
      C_PuiSdHandlerFiler::h_SaveCanProtocol(orc_CanProtocol, c_XMLParser);
      //Don't forget to save!
      if (c_XMLParser.SaveToFile(orc_FilePath.toStdString().c_str()) != C_NO_ERR)
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
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "com-protocol" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveCanProtocol(const C_PuiSdNodeCanProtocol & orc_CanProtocol,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("com-message-containers");

   h_SaveCanMessageContainers(orc_CanProtocol.c_ComMessages, orc_XMLParser);

   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocol");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can message containers

   \param[in]      orc_CanMessageContainers  Can message containers
   \param[in,out]  orc_XMLParser             XML parser with the "current" element set
                                           to the "com-message-containers" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveCanMessageContainers(
   const std::vector<C_PuiSdNodeCanMessageContainer> & orc_CanMessageContainers, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_CanMessageContainers.size());
   for (uint32 u32_ItCanMessageContainer = 0; u32_ItCanMessageContainer < orc_CanMessageContainers.size();
        ++u32_ItCanMessageContainer)
   {
      orc_XMLParser.CreateAndSelectNodeChild("com-message-container");
      h_SaveCanMessageContainer(orc_CanMessageContainers[u32_ItCanMessageContainer], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-containers");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can message container element

   \param[in]      orc_CanMessageContainer   Can message container element
   \param[in,out]  orc_XMLParser             XML parser with the "current" element set
                                           to the "com-message-container" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveCanMessageContainer(const C_PuiSdNodeCanMessageContainer & orc_CanMessageContainer,
                                                    C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("tx-messages");
   h_SaveCanMessages(orc_CanMessageContainer.c_TxMessages, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-container");
   orc_XMLParser.CreateAndSelectNodeChild("rx-messages");
   h_SaveCanMessages(orc_CanMessageContainer.c_RxMessages, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-container");
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can messages

   \param[in]      orc_CanMessages  Can messages
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to unknown element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveCanMessages(const std::vector<C_PuiSdNodeCanMessage> & orc_CanMessages,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_CanMessages.size());
   for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_CanMessages.size();
        ++u32_ItMessage)
   {
      orc_XMLParser.CreateAndSelectNodeChild("com-message");
      h_SaveCanMessage(orc_CanMessages[u32_ItMessage], orc_XMLParser);
      //Return
      orc_XMLParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can message element

   \param[in]      orc_CanMessage   Can message element
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "com-message" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveCanMessage(const C_PuiSdNodeCanMessage & orc_CanMessage,
                                           C_OSCXMLParserBase & orc_XMLParser)
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
   orc_XMLParser.SetAttributeBool("use-auto-receive-timeout", q_UseAutoReceiveTimeout);
   orc_XMLParser.SetAttributeBool("receive-timeout-disabled", q_ReceiveTimeoutDisabled);
   //Signals
   orc_XMLParser.CreateAndSelectNodeChild("com-signals");
   h_SaveCanSignals(orc_CanMessage.c_Signals, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can signals

   \param[in]      orc_CanSignals   Can signals
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "com-signals" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveCanSignals(const std::vector<C_PuiSdNodeCanSignal> & orc_CanSignals,
                                           C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_CanSignals.size());
   for (uint32 u32_ItDataPoolListElement = 0;
        u32_ItDataPoolListElement < orc_CanSignals.size();
        ++u32_ItDataPoolListElement)
   {
      orc_XMLParser.CreateAndSelectNodeChild("com-signal");
      h_SaveCanSignal(orc_CanSignals[u32_ItDataPoolListElement], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-signals");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can signal element

   \param[in]      orc_CanSignal    Can signal element
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "com-signal" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveCanSignal(const C_PuiSdNodeCanSignal & orc_CanSignal,
                                          C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("color-index", orc_CanSignal.u8_ColorIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load UI nodes

   Load UI node information from XML structure.
   Also uses base class function to load basic information.
   Postcondition: XMLParser has the node "nodes" selected.

   \param[out]     orc_Nodes        UI node data containers
   \param[in,out]  orc_XMLParser    XMLParser with the "current" element set to the "nodes" element
   \param[in]      opc_BasePath     Base path

   \return
   C_NO_ERR    everything ok
   else        error occured while loading
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadNodes(std::vector<C_PuiSdNode> & orc_Nodes, C_OSCXMLParserBase & orc_XMLParser,
                                        const QDir * const opc_BasePath)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_SelectedNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_Nodes.reserve(u32_ExpectedSize);
   }

   c_SelectedNode = orc_XMLParser.SelectNodeChild("node");

   orc_Nodes.clear();
   if (c_SelectedNode == "node")
   {
      do
      {
         C_PuiSdNode c_Node;
         if (opc_BasePath != NULL)
         {
            const QString c_FilePath = orc_XMLParser.GetNodeContent().c_str();
            const QString c_FilePathCombined = opc_BasePath->absoluteFilePath(c_FilePath);
            //Dir for sub folder
            const QFileInfo c_FileInfo(c_FilePathCombined);
            const QDir c_Dir = c_FileInfo.dir();
            s32_Retval = mh_LoadNodeFile(c_Node, c_FilePathCombined, &c_Dir);
         }
         else
         {
            s32_Retval = mh_LoadNode(c_Node, orc_XMLParser, opc_BasePath);
         }
         if (s32_Retval == C_NO_ERR)
         {
            orc_Nodes.push_back(c_Node);
         }

         //Next
         c_SelectedNode = orc_XMLParser.SelectNodeNext("node");
      }
      while (c_SelectedNode == "node");
      //Return (no check to allow reuse)
      orc_XMLParser.SelectNodeParent();
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Nodes.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI nodes count, expected: %i, got %i", u32_ExpectedSize,
                              orc_Nodes.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save UI nodes

   Save UI node information to XML structure
   Also uses base class function to save basic information.
   Postcondition: XMLParser has the node "nodes" selected.

   \param[in]      orc_Nodes        UI node data containers
   \param[in,out]  orc_XMLParser    XMLParser with the "current" element set to the "nodes" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveNodes(const std::vector<C_PuiSdNode> & orc_Nodes, C_OSCXMLParserString & orc_XMLParser)
{
   if (orc_Nodes.size() > 0)
   {
      orc_XMLParser.SetAttributeUint32("length", orc_Nodes.size());
      for (uint32 u32_Index = 0U; u32_Index < orc_Nodes.size(); u32_Index++)
      {
         orc_XMLParser.CreateAndSelectNodeChild("node");
         tgl_assert(C_PuiSdHandlerFiler::mh_SaveNode(orc_Nodes[u32_Index], NULL, NULL, orc_XMLParser) == C_NO_ERR);
         //Back up (don't check to allow reuse)
         orc_XMLParser.SelectNodeParent();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load GUI buses

   Load GUI-specific information about bus.
   Also uses base class function to load basic information.

   \param[in,out]  orc_Buses        UI bus data containers
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "buses" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadBuses(std::vector<C_PuiSdBus> & orc_Buses, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_SelectedNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_Buses.reserve(u32_ExpectedSize);
   }

   c_SelectedNode = orc_XMLParser.SelectNodeChild("bus");

   orc_Buses.clear();
   if (c_SelectedNode == "bus")
   {
      do
      {
         C_PuiSdBus c_Bus;
         s32_Retval = C_PuiSdHandlerFiler::mh_LoadBus(c_Bus, orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Buses.push_back(c_Bus);
         }

         //Next
         c_SelectedNode = orc_XMLParser.SelectNodeNext("bus");
      }
      while ((s32_Retval == C_NO_ERR) && (c_SelectedNode == "bus"));
      if (s32_Retval == C_NO_ERR)
      {
         //Return (no check to allow reuse)
         orc_XMLParser.SelectNodeParent();
      }
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Buses.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected bus count, expected: %i, got %i", u32_ExpectedSize,
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
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "buses" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveBuses(const std::vector<C_PuiSdBus> & orc_Buses, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_Buses.size());
   if (orc_Buses.size() > 0)
   {
      for (uint32 u32_Index = 0U; u32_Index < orc_Buses.size(); u32_Index++)
      {
         orc_XMLParser.CreateAndSelectNodeChild("bus");
         C_PuiSdHandlerFiler::mh_SaveBus(orc_Buses[u32_Index], orc_XMLParser);
         //Back up (don't check to allow reuse)
         orc_XMLParser.SelectNodeParent();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load text elements

   \param[in,out]  orc_BusTextElements    Text element data elements
   \param[in,out]  orc_XMLParser          XML parser with the "current" element set to the "bus-text-elements" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadBusTextElements(std::vector<C_PuiSdTextElementBus> & orc_BusTextElements,
                                                  C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_SelectedNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_BusTextElements.reserve(u32_ExpectedSize);
   }

   c_SelectedNode = orc_XMLParser.SelectNodeChild("bus-text-element");

   orc_BusTextElements.clear();
   if (c_SelectedNode == "bus-text-element")
   {
      do
      {
         C_PuiSdTextElementBus c_TextElement;
         s32_Retval = C_PuiSdHandlerFiler::mh_LoadTextElement(&c_TextElement, orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_BusTextElements.push_back(c_TextElement);
         }

         //Next
         c_SelectedNode = orc_XMLParser.SelectNodeNext("bus-text-element");
      }
      while (c_SelectedNode == "bus-text-element");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "bus-text-elements");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_BusTextElements.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected bus text element count, expected: %i, got %i", u32_ExpectedSize,
                              orc_BusTextElements.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save text elements

   \param[in]      orc_BusTextElements    Text element data elements
   \param[in,out]  orc_XMLParser          XML parser with the "current" element set to the "bus-text-elements" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::h_SaveBusTextElements(const std::vector<C_PuiSdTextElementBus> & orc_BusTextElements,
                                                C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_BusTextElements.size());
   for (uint32 u32_Index = 0U; u32_Index < orc_BusTextElements.size(); ++u32_Index)
   {
      orc_XMLParser.CreateAndSelectNodeChild("bus-text-element");
      C_PuiSdHandlerFiler::mh_SaveTextElement(&orc_BusTextElements[u32_Index], orc_XMLParser);
      tgl_assert(orc_XMLParser.SelectNodeParent() == "bus-text-elements"); //back up
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save UI system definition

   Save UI system definition to XML file

   \param[in]  orc_FilePath               File path for xml
   \param[in]  orc_OSCSystemDefinition    OSC storage
   \param[in]  orc_UiNodes                UI nodes data storage
   \param[in]  orc_UiBuses                UI buses data storage
   \param[in]  orc_BusTextElements        UI bus text elements data storage
   \param[in]  orc_Elements               UI generic elements data storage

   \return
   C_NO_ERR   data saved
   C_RD_WR    file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_SaveSystemDefinitionUiFile(const QString & orc_FilePath,
                                                         const C_OSCSystemDefinition & orc_OSCSystemDefinition,
                                                         const std::vector<C_PuiSdNode> & orc_UiNodes,
                                                         const std::vector<C_PuiSdBus> & orc_UiBuses,
                                                         const std::vector<C_PuiSdTextElementBus> & orc_BusTextElements,
                                                         const C_PuiBsElements & orc_Elements)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForNewFile(c_XMLParser, orc_FilePath.toStdString().c_str(),
                                                                   "opensyde-system-ui-definition");

   if (s32_Retval == C_NO_ERR)
   {
      const QFileInfo c_Info(orc_FilePath);

      //Version
      c_XMLParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XMLParser.CreateAndSelectNodeChild("nodes") == "nodes");
      c_XMLParser.SetAttributeUint32("length", orc_OSCSystemDefinition.c_Nodes.size());
      if (orc_OSCSystemDefinition.c_Nodes.size() == orc_UiNodes.size())
      {
         for (uint32 u32_ItNode = 0UL;
              (u32_ItNode < orc_OSCSystemDefinition.c_Nodes.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItNode)
         {
            const C_OSCNode & rc_OSCNode = orc_OSCSystemDefinition.c_Nodes[u32_ItNode];
            const C_PuiSdNode & rc_UINode = orc_UiNodes[u32_ItNode];
            const QString c_FolderName =
               C_OSCNodeFiler::h_GetFolderName(rc_OSCNode.c_Properties.c_Name.c_str()).c_str();
            QDir c_Folder = c_Info.dir();
            //Change to folder
            c_Folder.cd(c_FolderName);
            //file
            const QString c_FileName = C_PuiSdHandlerFiler::h_GetNodeUiFileName();
            s32_Retval =
               C_PuiSdHandlerFiler::mh_SaveNodeFile(rc_UINode, rc_OSCNode, c_Folder.absoluteFilePath(
                                                       c_FileName), &c_Folder);
            //Store file name
            c_XMLParser.CreateNodeChild("node", QString(c_FolderName + "/" + c_FileName).toStdString().c_str());
         }
      }
      else
      {
         s32_Retval = C_RD_WR;
         osc_write_log_error("Saving system definition UI", "Node count out of synchronization.");
      }
      if (s32_Retval == C_NO_ERR)
      {
         tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-ui-definition"); //back up

         //Bus
         c_XMLParser.CreateAndSelectNodeChild("buses");
         C_PuiSdHandlerFiler::h_SaveBuses(orc_UiBuses, c_XMLParser);
         tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-ui-definition"); //back up

         //GUI items

         //Bus text elements
         c_XMLParser.CreateAndSelectNodeChild("bus-text-elements");
         C_PuiSdHandlerFiler::h_SaveBusTextElements(orc_BusTextElements, c_XMLParser);
         tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-ui-definition"); //back up

         //Base elements
         C_PuiBsElementsFiler::h_SaveBaseElements(orc_Elements, c_XMLParser);
      }
      //Don't forget to save!
      if (c_XMLParser.SaveToFile(orc_FilePath.toStdString().c_str()) != C_NO_ERR)
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

   \return
   C_NO_ERR   data was read from file
   C_NOACT    could not read data from file
   C_RANGE    file not found
   C_CONFIG   root node not found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::h_LoadSystemDefinitionUiFile(const QString & orc_FilePath,
                                                         std::vector<C_PuiSdNode> & orc_UiNodes,
                                                         std::vector<C_PuiSdBus> & orc_UiBuses,
                                                         std::vector<C_PuiSdTextElementBus> & orc_BusTextElements,
                                                         stw_opensyde_gui_logic::C_PuiBsElements & orc_Elements)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForExistingFile(c_XMLParser,
                                                                        orc_FilePath.toStdString().c_str(),
                                                                        "opensyde-system-ui-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Nodes
      if (c_XMLParser.SelectNodeChild("nodes") == "nodes")
      {
         const QFileInfo c_FileInfo(orc_FilePath);
         const QDir c_BasePath = c_FileInfo.dir();
         s32_Retval = C_PuiSdHandlerFiler::h_LoadNodes(orc_UiNodes, c_XMLParser, &c_BasePath);
         if (s32_Retval == C_NO_ERR)
         {
            tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-ui-definition");
            //Buses
            if (c_XMLParser.SelectNodeChild("buses") == "buses")
            {
               s32_Retval = C_PuiSdHandlerFiler::h_LoadBuses(orc_UiBuses, c_XMLParser);
               if (s32_Retval == C_NO_ERR)
               {
                  tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-ui-definition");
                  //Buse text elements
                  if (c_XMLParser.SelectNodeChild("bus-text-elements") == "bus-text-elements")
                  {
                     s32_Retval = C_PuiSdHandlerFiler::h_LoadBusTextElements(orc_BusTextElements, c_XMLParser);
                     if (s32_Retval == C_NO_ERR)
                     {
                        tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-ui-definition");
                        //Generic elements
                        s32_Retval = C_PuiBsElementsFiler::h_LoadBaseElements(orc_Elements, c_XMLParser);
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
QString C_PuiSdHandlerFiler::h_GetCommUiFileName(const C_SCLString & orc_DatapoolName)
{
   return C_SCLString("comm_" + C_OSCSystemFilerUtil::h_PrepareItemNameForFileName(orc_DatapoolName) +
                      "_ui.xml").c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get automatically generated file name

   \param[in]  orc_DatapoolName  Datapool name

   \return
   Automatically generated file name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdHandlerFiler::h_GetDatapoolUiFileName(const C_SCLString & orc_DatapoolName)
{
   return C_SCLString("dp_" + C_OSCSystemFilerUtil::h_PrepareItemNameForFileName(orc_DatapoolName) +
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
/*! \brief   Load node UI part

   \param[in,out]  orc_Node      Node UI data storage
   \param[in]      orc_FilePath  File path
   \param[in]      opc_BasePath  Base path (Optional for save to string)

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::mh_LoadNodeFile(C_PuiSdNode & orc_Node, const QString & orc_FilePath,
                                            const QDir * const opc_BasePath)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForExistingFile(c_XMLParser,
                                                                        orc_FilePath.toStdString().c_str(),
                                                                        "opensyde-node-ui-definition");

   if (s32_Retval == C_NO_ERR)
   {
      if (c_XMLParser.SelectNodeChild("node") == "node")
      {
         s32_Retval = C_PuiSdHandlerFiler::mh_LoadNode(orc_Node, c_XMLParser, opc_BasePath);
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
/*! \brief   Load datapool UI part

   \param[in,out]  orc_DataPool  Datapool UI data storage
   \param[in]      orc_FilePath  File path

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::mh_LoadDatapoolFile(C_PuiSdNodeDataPool & orc_DataPool, const QString & orc_FilePath)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForExistingFile(c_XMLParser,
                                                                        orc_FilePath.toStdString().c_str(),
                                                                        "opensyde-dp-ui-definition");

   if (s32_Retval == C_NO_ERR)
   {
      if (c_XMLParser.SelectNodeChild("data-pool") == "data-pool")
      {
         s32_Retval = C_PuiSdHandlerFiler::h_LoadDataPool(orc_DataPool, c_XMLParser);
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

   \param[in,out]  orc_UICanProtocol   Comm definition UI data storage
   \param[in]      orc_FilePath        File path

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::mh_LoadCommFile(C_PuiSdNodeCanProtocol & orc_UICanProtocol, const QString & orc_FilePath)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForExistingFile(c_XMLParser,
                                                                        orc_FilePath.toStdString().c_str(),
                                                                        "opensyde-comm-ui-definition");

   if (s32_Retval == C_NO_ERR)
   {
      if (c_XMLParser.SelectNodeChild("com-protocol") == "com-protocol")
      {
         s32_Retval = C_PuiSdHandlerFiler::h_LoadCanProtocol(orc_UICanProtocol, c_XMLParser);
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
   \param[in,out]  orc_XMLParser    XMLParser with the "current" element set to the "node" element
   \param[in]      opc_BasePath     Base path (Optional for save to string)

   \return
   C_NO_ERR    everything ok
   else        error occured while loading
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::mh_LoadNode(C_PuiSdNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser,
                                        const QDir * const opc_BasePath)
{
   sint32 s32_Return = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("box") == "box")
   {
      s32_Return = C_PuiBsElementsFiler::h_LoadBoxBase(orc_Node, orc_XMLParser);
      orc_XMLParser.SelectNodeParent(); //back up to "node"
   }
   if (s32_Return == C_NO_ERR)
   {
      s32_Return = h_LoadCanProtocols(orc_Node.c_UICanProtocols, orc_XMLParser, opc_BasePath);
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = h_LoadDataPools(orc_Node.c_UIDataPools, orc_XMLParser, opc_BasePath);

         if ((s32_Return == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("busconnections") == "busconnections"))
         {
            orc_Node.c_UIBusConnections.clear();

            //go through list:
            if (orc_XMLParser.SelectNodeChild("connection") == "connection")
            {
               do
               {
                  //check type and busnumber just to verify the sequence matches the one in core:
                  C_PuiSdNodeConnectionId c_Connection;
                  C_SCLString c_Text;
                  c_Text = orc_XMLParser.GetAttributeString("type");
                  if (c_Text == "ethernet")
                  {
                     c_Connection.e_InterfaceType = C_OSCSystemBus::eETHERNET;
                  }
                  else if (c_Text == "can")
                  {
                     c_Connection.e_InterfaceType = C_OSCSystemBus::eCAN;
                  }
                  else
                  {
                     s32_Return = C_CONFIG;
                  }
                  if (s32_Return == C_NO_ERR)
                  {
                     c_Connection.u8_InterfaceNumber =
                        static_cast<uint8>(orc_XMLParser.GetAttributeUint32("busnumber"));

                     {
                        C_PuiSdNodeConnection c_UIConnection;

                        //Set ID
                        c_UIConnection.c_ConnectionID = c_Connection;

                        //sequence OK; now get points:
                        if (orc_XMLParser.SelectNodeChild("interaction-points") == "interaction-points")
                        {
                           //file through children:
                           c_Text = orc_XMLParser.SelectNodeChild("interaction-point");

                           if (c_Text == "interaction-point")
                           {
                              do
                              {
                                 QPointF c_Point(orc_XMLParser.GetAttributeFloat64(
                                                    "x"), orc_XMLParser.GetAttributeFloat64(
                                                    "y"));
                                 c_UIConnection.c_UINodeConnectionInteractionPoints.push_back(c_Point);
                                 c_Text = orc_XMLParser.SelectNodeNext("interaction-point");
                              }
                              while (c_Text == "interaction-point");
                              tgl_assert(orc_XMLParser.SelectNodeParent() == "interaction-points");
                           }

                           orc_Node.c_UIBusConnections.push_back(c_UIConnection);
                           tgl_assert(orc_XMLParser.SelectNodeParent() == "connection");
                        }
                        else
                        {
                           s32_Return = C_CONFIG;
                        }
                     }
                  }
               }
               while ((s32_Return == C_NO_ERR) && (orc_XMLParser.SelectNodeNext("connection") == "connection"));
               //any more connections ?

               tgl_assert(orc_XMLParser.SelectNodeParent() == "busconnections"); //back to busconnections
            }
            orc_XMLParser.SelectNodeParent(); //back to node
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

   \param[in]      orc_UINode    UI data storage
   \param[in]      orc_OSCNode   OSC data storage
   \param[in,out]  orc_FilePath  File path for xml
   \param[in]      opc_BasePath  Base path (Optional for save to string)

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::mh_SaveNodeFile(const C_PuiSdNode & orc_UINode, const C_OSCNode & orc_OSCNode,
                                            const QString & orc_FilePath, const QDir * const opc_BasePath)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForNewFile(c_XMLParser, orc_FilePath.toStdString().c_str(),
                                                                   "opensyde-node-ui-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XMLParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XMLParser.CreateAndSelectNodeChild("node") == "node");
      //node
      s32_Retval = C_PuiSdHandlerFiler::mh_SaveNode(orc_UINode, &orc_OSCNode, opc_BasePath, c_XMLParser);
      if (s32_Retval == C_NO_ERR)
      {
         //Don't forget to save!
         if (c_XMLParser.SaveToFile(orc_FilePath.toStdString().c_str()) != C_NO_ERR)
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

   \param[in]      orc_UINode       UI node data container
   \param[in]      opc_OSCNode      OSC node data container
   \param[in]      opc_BasePath     Base path (Optional for save to string)
   \param[in,out]  orc_XMLParser    XMLParser with the "current" element set to the "node" element

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFiler::mh_SaveNode(const C_PuiSdNode & orc_UINode, const C_OSCNode * const opc_OSCNode,
                                        const QDir * const opc_BasePath, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval;

   if (opc_OSCNode != NULL)
   {
      s32_Retval = h_SaveCanProtocols(orc_UINode.c_UICanProtocols, &opc_OSCNode->c_ComProtocols,
                                      &opc_OSCNode->c_DataPools, opc_BasePath, orc_XMLParser);
   }
   else
   {
      s32_Retval = h_SaveCanProtocols(orc_UINode.c_UICanProtocols, NULL, NULL, opc_BasePath, orc_XMLParser);
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (opc_OSCNode != NULL)
      {
         s32_Retval = h_SaveDataPools(orc_UINode.c_UIDataPools, &opc_OSCNode->c_DataPools, opc_BasePath, orc_XMLParser);
      }
      else
      {
         s32_Retval = h_SaveDataPools(orc_UINode.c_UIDataPools, NULL, opc_BasePath, orc_XMLParser);
      }
      if (s32_Retval == C_NO_ERR)
      {
         orc_XMLParser.CreateAndSelectNodeChild("box");
         C_PuiBsElementsFiler::h_SaveBoxBase(orc_UINode, orc_XMLParser);
         orc_XMLParser.SelectNodeParent();

         orc_XMLParser.CreateAndSelectNodeChild("busconnections");
         for (uint16 u16_Bus = 0U; u16_Bus < orc_UINode.c_UIBusConnections.size(); u16_Bus++)
         {
            const C_PuiSdNodeConnection & rc_Bus = orc_UINode.c_UIBusConnections[u16_Bus];

            orc_XMLParser.CreateAndSelectNodeChild("connection");

            if (rc_Bus.c_ConnectionID.e_InterfaceType == C_OSCSystemBus::eCAN)
            {
               orc_XMLParser.SetAttributeString("type", "can");
            }
            else if (rc_Bus.c_ConnectionID.e_InterfaceType == C_OSCSystemBus::eETHERNET)
            {
               orc_XMLParser.SetAttributeString("type", "ethernet");
            }
            else
            {
               //weird ...
            }
            orc_XMLParser.SetAttributeUint32("busnumber", rc_Bus.c_ConnectionID.u8_InterfaceNumber);

            orc_XMLParser.CreateAndSelectNodeChild("interaction-points");

            for (uint16 u16_Point = 0U;
                 u16_Point < rc_Bus.c_UINodeConnectionInteractionPoints.size(); u16_Point++)
            {
               orc_XMLParser.CreateAndSelectNodeChild("interaction-point");
               orc_XMLParser.SetAttributeFloat64("x", rc_Bus.c_UINodeConnectionInteractionPoints[u16_Point].x());
               orc_XMLParser.SetAttributeFloat64("y", rc_Bus.c_UINodeConnectionInteractionPoints[u16_Point].y());
               orc_XMLParser.SelectNodeParent(); //back to interaction-points
            }

            tgl_assert(orc_XMLParser.SelectNodeParent() == "connection");     //back to connection
            tgl_assert(orc_XMLParser.SelectNodeParent() == "busconnections"); //back to busconnections
         }

         tgl_assert(orc_XMLParser.SelectNodeParent() == "node"); //back to "node"
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load GUI bus information

   Load GUI-specific information about bus.
   Also uses base class function to load basic information.

   \param[in,out]  orc_Bus          UI bus data container
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "bus" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_PuiSdHandlerFiler::mh_LoadBus(C_PuiSdBus & orc_Bus, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return = C_NO_ERR;

   //GUI information
   if (orc_XMLParser.SelectNodeChild("line") == "line")
   {
      s32_Return = C_PuiBsElementsFiler::h_LoadLineBase(orc_Bus, orc_XMLParser);
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   if (orc_XMLParser.SelectNodeNext("color-middle-line") == "color-middle-line")
   {
      C_PuiBsElementsFiler::h_LoadColor(orc_Bus.c_UIColorMiddleLine, orc_XMLParser);
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   orc_XMLParser.SelectNodeParent(); //back to bus

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save GUI bus information

   Save GUI-specific information about bus.
   Also uses base class function to save basic information.

   \param[in]      orc_Bus          UI bus data container
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "bus" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::mh_SaveBus(const C_PuiSdBus & orc_Bus, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("line");
   C_PuiBsElementsFiler::h_SaveLineBase(orc_Bus, orc_XMLParser);
   orc_XMLParser.SelectNodeParent(); //back to "gui"

   orc_XMLParser.CreateAndSelectNodeChild("color-middle-line");
   C_PuiBsElementsFiler::h_SaveColor(orc_Bus.c_UIColorMiddleLine, orc_XMLParser);

   orc_XMLParser.SelectNodeParent(); //back to "bus"
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load text element attributes

   \param[in,out]  opc_TextElement  Text element data element
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "text-element" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_PuiSdHandlerFiler::mh_LoadTextElement(C_PuiBsTextElement * const opc_TextElement,
                                                          stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return;
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_PuiSdTextElementBus * const opc_BusTextElement =
      dynamic_cast<C_PuiSdTextElementBus * const>(opc_TextElement);

   s32_Return = C_PuiBsElementsFiler::h_LoadTextElement(opc_TextElement, orc_XMLParser);
   //Check bus
   if (opc_BusTextElement != NULL)
   {
      opc_BusTextElement->u32_BusIndex = orc_XMLParser.GetAttributeUint32("bus-index");
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save text element

   \param[in]      opc_TextElement  Text element data element
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "text-element" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFiler::mh_SaveTextElement(const C_PuiBsTextElement * const opc_TextElement,
                                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const C_PuiSdTextElementBus * const opc_BusTextElement =
      dynamic_cast<const C_PuiSdTextElementBus * const>(opc_TextElement);

   C_PuiBsElementsFiler::h_SaveTextElement(opc_TextElement, orc_XMLParser);
   //Check bus
   if (opc_BusTextElement != NULL)
   {
      orc_XMLParser.SetAttributeUint32("bus-index", opc_BusTextElement->u32_BusIndex);
   }
}
