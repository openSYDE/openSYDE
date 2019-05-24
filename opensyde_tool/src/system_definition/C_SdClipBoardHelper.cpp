//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle clipboard load and save (implementation)

   Handle clipboard load and save

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QClipboard>
#include <QApplication>
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_SdClipBoardHelper.h"
#include "C_OSCXMLParser.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSdHandlerFiler.h"
#include "C_PuiBsElementsFiler.h"
#include "C_OSCNodeDataPoolFiler.h"
#include "C_OSCNodeFiler.h"
#include "TGLUtils.h"
#include "C_OSCNodeCommFiler.h"
#include "C_OSCSystemDefinitionFiler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_errors;
using namespace stw_tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store node data pool to clip board

   \param[in] orc_OSCContent  OSC content
   \param[in] orc_UIContent   UI content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreDataPool(const C_OSCNodeDataPool & orc_OSCContent,
                                          const C_PuiSdNodeDataPool & orc_UIContent)
{
   stw_scl::C_SCLString c_XMLContent;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("gui");
   c_StringXml.CreateAndSelectNodeChild("data-pool");

   C_PuiSdHandlerFiler::h_SaveDataPool(orc_UIContent, c_StringXml);

   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui");
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
   c_StringXml.CreateAndSelectNodeChild("core");
   c_StringXml.CreateAndSelectNodeChild("data-pool");

   C_OSCNodeDataPoolFiler::h_SaveDataPool(orc_OSCContent, c_StringXml);

   c_StringXml.SaveToString(c_XMLContent);
   mh_SetClipBoard(c_XMLContent.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node data pool from clip board

   \param[out] orc_OSCContent  OSC content
   \param[out] orc_UIContent   UI content

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdClipBoardHelper::h_LoadToDataPool(C_OSCNodeDataPool & orc_OSCContent, C_PuiSdNodeDataPool & orc_UIContent)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.LoadFromString(mh_GetClipBoard().toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("gui") == "gui")
      {
         if (c_StringXml.SelectNodeChild("data-pool") == "data-pool")
         {
            s32_Retval = C_PuiSdHandlerFiler::h_LoadDataPool(orc_UIContent, c_StringXml);
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(c_StringXml.SelectNodeParent() == "gui");
               //Return
               tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
               if (c_StringXml.SelectNodeChild("core") == "core")
               {
                  if (c_StringXml.SelectNodeChild("data-pool") == "data-pool")
                  {
                     s32_Retval = C_OSCNodeDataPoolFiler::h_LoadDataPool(orc_OSCContent, c_StringXml);
                  }
                  else
                  {
                     s32_Retval = C_CONFIG;
                  }
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store node data pool lists to clip board

   \param[in] orc_OSCContent OSC content
   \param[in] orc_UIContent  UI content
   \param[in] ore_Type       Data pool type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreDataPoolLists(const std::vector<C_OSCNodeDataPoolList> & orc_OSCContent,
                                               const std::vector<C_PuiSdNodeDataPoolList> & orc_UIContent,
                                               const C_OSCNodeDataPool::E_Type & ore_Type)
{
   stw_scl::C_SCLString c_XMLContent;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("gui");
   c_StringXml.CreateAndSelectNodeChild("type");

   c_StringXml.SetNodeContent(C_OSCNodeDataPoolFiler::h_DataPoolToString(ore_Type));

   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui");
   c_StringXml.CreateAndSelectNodeChild("lists");

   C_PuiSdHandlerFiler::h_SaveDataPoolLists(orc_UIContent, c_StringXml);

   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui");
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
   c_StringXml.CreateAndSelectNodeChild("core");
   c_StringXml.CreateAndSelectNodeChild("lists");

   C_OSCNodeDataPoolFiler::h_SaveDataPoolLists(orc_OSCContent, c_StringXml);

   c_StringXml.SaveToString(c_XMLContent);
   mh_SetClipBoard(c_XMLContent.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node data pool lists from clip board

   \param[out] orc_OSCContent OSC content
   \param[out] orc_UIContent  UI content
   \param[out] ore_Type       Data pool type

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdClipBoardHelper::h_LoadToDataPoolLists(std::vector<C_OSCNodeDataPoolList> & orc_OSCContent,
                                                  std::vector<C_PuiSdNodeDataPoolList> & orc_UIContent,
                                                  C_OSCNodeDataPool::E_Type & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.LoadFromString(mh_GetClipBoard().toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("gui") == "gui")
      {
         if (c_StringXml.SelectNodeChild("type") == "type")
         {
            s32_Retval = C_OSCNodeDataPoolFiler::h_StringToDataPool(c_StringXml.GetNodeContent(), ore_Type);
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(c_StringXml.SelectNodeParent() == "gui");
               if (c_StringXml.SelectNodeChild("lists") == "lists")
               {
                  s32_Retval = C_PuiSdHandlerFiler::h_LoadDataPoolLists(orc_UIContent, c_StringXml);
                  if (s32_Retval == C_NO_ERR)
                  {
                     //Return
                     tgl_assert(c_StringXml.SelectNodeParent() == "gui");
                     //Return
                     tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
                     if (c_StringXml.SelectNodeChild("core") == "core")
                     {
                        if (c_StringXml.SelectNodeChild("lists") == "lists")
                        {
                           s32_Retval = C_OSCNodeDataPoolFiler::h_LoadDataPoolLists(orc_OSCContent, c_StringXml);
                        }
                        else
                        {
                           s32_Retval = C_CONFIG;
                        }
                     }
                     else
                     {
                        s32_Retval = C_CONFIG;
                     }
                  }
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store node data pool list elements to clip board

   \param[in] orc_OSCContent OSC content
   \param[in] orc_UIContent  UI content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreDataPoolListElementsToClipBoard(
   const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCContent,
   const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UIContent)
{
   QString c_Tmp;

   h_StoreDataPoolListElementsToString(orc_OSCContent, orc_UIContent, c_Tmp);
   mh_SetClipBoard(c_Tmp);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node data pool list elements from clip board

   \param[in] orc_OSCContent OSC content
   \param[in] orc_UIContent  UI content

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdClipBoardHelper::h_LoadToDataPoolListElementsFromClipBoard(
   std::vector<C_OSCNodeDataPoolListElement> & orc_OSCContent,
   std::vector<C_PuiSdNodeDataPoolListElement> & orc_UIContent)
{
   return h_LoadToDataPoolListElementsFromString(orc_OSCContent, orc_UIContent, mh_GetClipBoard());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store node data pool list elements to string

   \param[in]  orc_OSCContent OSC content
   \param[in]  orc_UIContent  UI content
   \param[out] orc_Output     String output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreDataPoolListElementsToString(
   const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCContent,
   const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UIContent, QString & orc_Output)
{
   stw_scl::C_SCLString c_XMLContent;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("gui");
   c_StringXml.CreateAndSelectNodeChild("data-elements");

   C_PuiSdHandlerFiler::h_SaveDataPoolListElements(orc_UIContent, c_StringXml);

   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui");
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
   c_StringXml.CreateAndSelectNodeChild("core");
   c_StringXml.CreateAndSelectNodeChild("data-elements");

   C_OSCNodeDataPoolFiler::h_SaveDataPoolListElements(orc_OSCContent, c_StringXml);

   c_StringXml.SaveToString(c_XMLContent);
   orc_Output = c_XMLContent.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node data pool list elements from string

   \param[in] orc_OSCContent OSC content
   \param[in] orc_UIContent  UI content
   \param[in] orc_Input      String input

   \return
   C_NO_ERR Found and loaded
   C_CONFIG String invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdClipBoardHelper::h_LoadToDataPoolListElementsFromString(
   std::vector<C_OSCNodeDataPoolListElement> & orc_OSCContent,
   std::vector<C_PuiSdNodeDataPoolListElement> & orc_UIContent, const QString & orc_Input)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.LoadFromString(orc_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("gui") == "gui")
      {
         if (c_StringXml.SelectNodeChild("data-elements") == "data-elements")
         {
            s32_Retval = C_PuiSdHandlerFiler::h_LoadDataPoolListElements(orc_UIContent, c_StringXml);
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(c_StringXml.SelectNodeParent() == "gui");
               //Return
               tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
               if (c_StringXml.SelectNodeChild("core") == "core")
               {
                  if (c_StringXml.SelectNodeChild("data-elements") == "data-elements")
                  {
                     s32_Retval = C_OSCNodeDataPoolFiler::h_LoadDataPoolListElements(orc_OSCContent, c_StringXml);
                  }
                  else
                  {
                     s32_Retval = C_CONFIG;
                  }
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store indices to string

   \param[in]  orc_Indices Indices
   \param[out] orc_Output  String output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreIndicesToString(const std::vector<uint32> & orc_Indices, QString & orc_Output)
{
   stw_scl::C_SCLString c_XMLContent;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("indices");
   for (uint32 u32_Index = 0; u32_Index < static_cast<uint32>(orc_Indices.size()); ++u32_Index)
   {
      c_StringXml.CreateAndSelectNodeChild("index");
      c_StringXml.SetAttributeUint32("number", orc_Indices.at(u32_Index));
      //Return
      tgl_assert(c_StringXml.SelectNodeParent() == "indices");
   }
   c_StringXml.SaveToString(c_XMLContent);
   orc_Output = c_XMLContent.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load indices from string

   \param[out] orc_Indices Indices
   \param[in]  orc_Input   String input

   \return
   C_NO_ERR Found and loaded
   C_CONFIG String invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdClipBoardHelper::h_LoadIndicesFromString(std::vector<uint32> & orc_Indices, const QString & orc_Input)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.LoadFromString(orc_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("indices") == "indices")
      {
         stw_scl::C_SCLString c_Node = c_StringXml.SelectNodeChild("index");
         while (c_Node == "index")
         {
            orc_Indices.push_back(c_StringXml.GetAttributeUint32("number"));
            //Next
            c_Node = c_StringXml.SelectNodeNext("index");
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store node data pool list data sets to clip board

   \param[out] orc_OSCNames         Data set names
   \param[out] orc_OSCDataSetValues Data set values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreDataPoolListDataSetsToClipBoard(
   const std::vector<C_OSCNodeDataPoolDataSet> & orc_OSCNames,
   const std::vector<std::vector<C_OSCNodeDataPoolContent> > & orc_OSCDataSetValues)
{
   QString c_Tmp;

   h_StoreDataPoolListDataSetsToString(orc_OSCNames, orc_OSCDataSetValues, c_Tmp);
   mh_SetClipBoard(c_Tmp);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data sets from clipboard

   \param[out] orc_OSCNames         Data set names
   \param[out] orc_OSCDataSetValues Data set values
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdClipBoardHelper::h_LoadToDataPoolListDataSetsFromClipBoard(
   std::vector<C_OSCNodeDataPoolDataSet> & orc_OSCNames,
   std::vector<std::vector<C_OSCNodeDataPoolContent> > & orc_OSCDataSetValues)
{
   return h_LoadToDataPoolListDataSetsFromString(orc_OSCNames, orc_OSCDataSetValues, mh_GetClipBoard());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store node data pool list data sets to string

   \param[out] orc_OSCNames         Data set names
   \param[out] orc_OSCDataSetValues Data set values
   \param[out] orc_Output           String output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreDataPoolListDataSetsToString(
   const std::vector<C_OSCNodeDataPoolDataSet> & orc_OSCNames,
   const std::vector<std::vector<C_OSCNodeDataPoolContent> > & orc_OSCDataSetValues, QString & orc_Output)
{
   stw_scl::C_SCLString c_XMLContent;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("core");
   c_StringXml.CreateAndSelectNodeChild("data-sets");

   C_OSCNodeDataPoolFiler::h_SaveDataPoolListDataSets(orc_OSCNames, c_StringXml);

   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "core");
   c_StringXml.CreateAndSelectNodeChild("data-sets-2");
   for (uint32 u32_ItDataSet = 0; u32_ItDataSet < orc_OSCDataSetValues.size(); ++u32_ItDataSet)
   {
      const std::vector<C_OSCNodeDataPoolContent> & rc_DataSetValues = orc_OSCDataSetValues[u32_ItDataSet];
      c_StringXml.CreateAndSelectNodeChild("data-set-values");
      c_StringXml.SetAttributeUint32("index", u32_ItDataSet);
      for (uint32 u32_ItType = 0; u32_ItType < rc_DataSetValues.size(); ++u32_ItType)
      {
         c_StringXml.CreateAndSelectNodeChild("value");
         C_OSCNodeDataPoolFiler::h_SaveDataPoolContentV1(rc_DataSetValues[u32_ItType], c_StringXml);
         tgl_assert(c_StringXml.SelectNodeParent() == "data-set-values");
      }

      tgl_assert(c_StringXml.SelectNodeParent() == "data-sets-2");
   }

   c_StringXml.SaveToString(c_XMLContent);
   orc_Output = c_XMLContent.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data sets from clipboard

   \param[out] orc_OSCNames         Data set names
   \param[out] orc_OSCDataSetValues Data set values
   \param[in]  orc_Input            String input
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdClipBoardHelper::h_LoadToDataPoolListDataSetsFromString(std::vector<C_OSCNodeDataPoolDataSet> & orc_OSCNames,
                                                                   std::vector<std::vector<C_OSCNodeDataPoolContent> > & orc_OSCDataSetValues,
                                                                   const QString & orc_Input)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.LoadFromString(orc_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("core") == "core")
      {
         if (c_StringXml.SelectNodeChild("data-sets") == "data-sets")
         {
            s32_Retval = C_OSCNodeDataPoolFiler::h_LoadDataPoolListDataSets(orc_OSCNames, c_StringXml);
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(c_StringXml.SelectNodeParent() == "core");
               if (c_StringXml.SelectNodeChild("data-sets-2") == "data-sets-2")
               {
                  stw_scl::C_SCLString c_CurNodeDataSetValue = c_StringXml.SelectNodeChild("data-set-values");

                  if (c_CurNodeDataSetValue == "data-set-values")
                  {
                     do
                     {
                        std::vector<C_OSCNodeDataPoolContent> c_CurDataSetValue;

                        if (s32_Retval == C_NO_ERR)
                        {
                           stw_scl::C_SCLString c_CurNodeDataSetValueValue;
                           //Check if index as expected (in order)
                           tgl_assert(c_StringXml.GetAttributeUint32("index") == orc_OSCDataSetValues.size());
                           //Load content
                           c_CurNodeDataSetValueValue = c_StringXml.SelectNodeChild("value");

                           if (c_CurNodeDataSetValueValue == "value")
                           {
                              do
                              {
                                 C_OSCNodeDataPoolContent c_CurDataSetValueValue;

                                 if (s32_Retval == C_NO_ERR)
                                 {
                                    //Load content
                                    s32_Retval = C_OSCNodeDataPoolFiler::h_LoadDataPoolContentV1(
                                       c_CurDataSetValueValue,
                                       c_StringXml);
                                 }

                                 //Append
                                 c_CurDataSetValue.push_back(c_CurDataSetValueValue);

                                 //Next
                                 c_CurNodeDataSetValueValue = c_StringXml.SelectNodeNext("value");
                              }
                              while (c_CurNodeDataSetValueValue == "value");

                              //Finished, back to parent
                              tgl_assert(c_StringXml.SelectNodeParent() == "data-set-values");
                           }
                        }

                        //Append
                        orc_OSCDataSetValues.push_back(c_CurDataSetValue);

                        //Next
                        c_CurNodeDataSetValue = c_StringXml.SelectNodeNext("data-set-values");
                     }
                     while (c_CurNodeDataSetValue == "data-set-values");
                  }
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store messages to clipboard

   \param[in] orc_Messages                Message data
   \param[in] orc_OSCSignalCommons        Signal common osc data
   \param[in] orc_UISignalCommons         Signal common ui data
   \param[in] orc_UISignals               Signal ui data
   \param[in] orc_OwnerNodeName           Owner node names
   \param[in] orc_OwnerNodeInterfaceIndex Owner node interface index
   \param[in] orc_OwnerIsTxFlag           Owner has message as TX flags
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreMessages(const std::vector<C_OSCCanMessage> & orc_Messages,
                                          const std::vector<std::vector<C_OSCNodeDataPoolListElement> > & orc_OSCSignalCommons, const std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > & orc_UISignalCommons, const std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UISignals, const std::vector<std::vector<QString> > & orc_OwnerNodeName, std::vector<std::vector<uint32> > & orc_OwnerNodeInterfaceIndex,
                                          const std::vector<std::vector<bool> > & orc_OwnerIsTxFlag)
{
   QString c_String;

   stw_scl::C_SCLString c_XMLContent;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("core");
   c_StringXml.CreateAndSelectNodeChild("message");
   C_OSCNodeCommFiler::h_SaveNodeComMessages(orc_Messages, c_StringXml);
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "core");
   c_StringXml.CreateAndSelectNodeChild("message-common");
   for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_OSCSignalCommons.size(); ++u32_ItMessage)
   {
      c_StringXml.CreateAndSelectNodeChild("data-elements");
      C_OSCNodeDataPoolFiler::h_SaveDataPoolListElements(orc_OSCSignalCommons[u32_ItMessage], c_StringXml);
      //Return
      tgl_assert(c_StringXml.SelectNodeParent() == "message-common");
   }
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "core");
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
   c_StringXml.CreateAndSelectNodeChild("gui");
   c_StringXml.CreateAndSelectNodeChild("message-common");
   for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_UISignalCommons.size(); ++u32_ItMessage)
   {
      c_StringXml.CreateAndSelectNodeChild("data-elements");
      C_PuiSdHandlerFiler::h_SaveDataPoolListElements(orc_UISignalCommons[u32_ItMessage], c_StringXml);
      //Return
      tgl_assert(c_StringXml.SelectNodeParent() == "message-common");
   }
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui");
   c_StringXml.CreateAndSelectNodeChild("message");
   for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_UISignals.size(); ++u32_ItMessage)
   {
      c_StringXml.CreateAndSelectNodeChild("com-signals");
      C_PuiSdHandlerFiler::h_SaveCanSignals(orc_UISignals[u32_ItMessage], c_StringXml);
      //Return
      tgl_assert(c_StringXml.SelectNodeParent() == "message");
   }
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui");
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");

   //Owner
   tgl_assert((orc_OwnerNodeName.size() == orc_OwnerNodeInterfaceIndex.size()) &&
              (orc_OwnerNodeName.size() == orc_OwnerIsTxFlag.size()));
   if ((orc_OwnerNodeName.size() == orc_OwnerNodeInterfaceIndex.size()) &&
       (orc_OwnerNodeName.size() == orc_OwnerIsTxFlag.size()))
   {
      c_StringXml.CreateAndSelectNodeChild("parents");
      for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_OwnerNodeName.size(); ++u32_ItMessage)
      {
         const std::vector<QString>  & rc_OwnerNodeName = orc_OwnerNodeName[u32_ItMessage];
         const std::vector<uint32>  & rc_OwnerNodeInterfaceIndex = orc_OwnerNodeInterfaceIndex[u32_ItMessage];
         const std::vector<bool>  & rc_OwnerIsTxFlag = orc_OwnerIsTxFlag[u32_ItMessage];
         c_StringXml.CreateAndSelectNodeChild("message-parents");
         tgl_assert((rc_OwnerNodeName.size() == rc_OwnerNodeInterfaceIndex.size()) &&
                    (rc_OwnerNodeName.size() == rc_OwnerIsTxFlag.size()));
         if ((rc_OwnerNodeName.size() == rc_OwnerNodeInterfaceIndex.size()) &&
             (rc_OwnerNodeName.size() == rc_OwnerIsTxFlag.size()))
         {
            for (uint32 u32_ItOwner = 0; u32_ItOwner < rc_OwnerNodeName.size(); ++u32_ItOwner)
            {
               const QString & rc_CurName = rc_OwnerNodeName[u32_ItOwner];
               c_StringXml.CreateAndSelectNodeChild("message-parent");
               c_StringXml.SetAttributeUint32("interface-index", rc_OwnerNodeInterfaceIndex[u32_ItOwner]);
               c_StringXml.SetAttributeBool("message-was-tx", rc_OwnerIsTxFlag[u32_ItOwner]);
               c_StringXml.CreateNodeChild("name", rc_CurName.toStdString().c_str());
               //Return
               tgl_assert(c_StringXml.SelectNodeParent() == "message-parents");
            }
         }
         //Return
         tgl_assert(c_StringXml.SelectNodeParent() == "parents");
      }
   }

   //Final step
   c_StringXml.SaveToString(c_XMLContent);
   c_String = c_XMLContent.c_str();
   C_SdClipBoardHelper::mh_SetClipBoard(c_String);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore messages from clipboard

   \param[out] orc_Messages                Message data
   \param[out] orc_OSCSignalCommons        Signal common osc data
   \param[out] orc_UISignalCommons         Signal common ui data
   \param[out] orc_UISignals               Signal ui data
   \param[out] orc_OwnerNodeName           Owner node names
   \param[out] orc_OwnerNodeInterfaceIndex Owner node interface index
   \param[out] orc_OwnerIsTxFlag           Owner has message as TX flags

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdClipBoardHelper::h_LoadMessages(std::vector<C_OSCCanMessage> & orc_Messages,
                                           std::vector<std::vector<C_OSCNodeDataPoolListElement> > & orc_OSCSignalCommons, std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > & orc_UISignalCommons, std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UISignals, std::vector<std::vector<QString> > & orc_OwnerNodeName, std::vector<std::vector<uint32> > & orc_OwnerNodeInterfaceIndex,
                                           std::vector<std::vector<bool> > & orc_OwnerIsTxFlag)
{
   sint32 s32_Retval = C_NO_ERR;
   const QString c_Input = C_SdClipBoardHelper::mh_GetClipBoard();
   C_OSCXMLParserString c_StringXml;

   c_StringXml.LoadFromString(c_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("core") == "core")
      {
         if (c_StringXml.SelectNodeChild("message") == "message")
         {
            s32_Retval = C_OSCNodeCommFiler::h_LoadNodeComMessages(orc_Messages, c_StringXml);
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(c_StringXml.SelectNodeParent() == "core");
               if (c_StringXml.SelectNodeChild("message-common") == "message-common")
               {
                  stw_scl::C_SCLString c_CurrentNode = c_StringXml.SelectNodeChild("data-elements");
                  if (c_CurrentNode == "data-elements")
                  {
                     std::vector<C_OSCNodeDataPoolListElement> c_Tmp;
                     do
                     {
                        c_Tmp.clear();
                        C_OSCNodeDataPoolFiler::h_LoadDataPoolListElements(c_Tmp, c_StringXml);
                        orc_OSCSignalCommons.push_back(c_Tmp);
                        c_CurrentNode = c_StringXml.SelectNodeNext("data-elements");
                     }
                     while (c_CurrentNode == "data-elements");
                     //Return
                     tgl_assert(c_StringXml.SelectNodeParent() == "message-common");
                  }
                  //Return
                  tgl_assert(c_StringXml.SelectNodeParent() == "core");
                  //Return
                  tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
                  if (c_StringXml.SelectNodeChild("gui") == "gui")
                  {
                     if (c_StringXml.SelectNodeChild("message-common") == "message-common")
                     {
                        c_CurrentNode = c_StringXml.SelectNodeChild("data-elements");
                        if (c_CurrentNode == "data-elements")
                        {
                           std::vector<C_PuiSdNodeDataPoolListElement> c_Tmp;
                           do
                           {
                              c_Tmp.clear();
                              C_PuiSdHandlerFiler::h_LoadDataPoolListElements(c_Tmp, c_StringXml);
                              orc_UISignalCommons.push_back(c_Tmp);
                              c_CurrentNode = c_StringXml.SelectNodeNext("data-elements");
                           }
                           while (c_CurrentNode == "data-elements");
                           //Return
                           tgl_assert(c_StringXml.SelectNodeParent() == "message-common");
                        }
                        //Return
                        tgl_assert(c_StringXml.SelectNodeParent() == "gui");
                        if (c_StringXml.SelectNodeChild("message") == "message")
                        {
                           c_CurrentNode = c_StringXml.SelectNodeChild("com-signals");
                           if (c_CurrentNode == "com-signals")
                           {
                              std::vector<C_PuiSdNodeCanSignal> c_Tmp;
                              do
                              {
                                 c_Tmp.clear();
                                 C_PuiSdHandlerFiler::h_LoadCanSignals(c_Tmp, c_StringXml);
                                 orc_UISignals.push_back(c_Tmp);
                                 c_CurrentNode = c_StringXml.SelectNodeNext("com-signals");
                              }
                              while (c_CurrentNode == "com-signals");
                              //Return
                              tgl_assert(c_StringXml.SelectNodeParent() == "message");
                              //Return
                              tgl_assert(c_StringXml.SelectNodeParent() == "gui");
                              //Return
                              tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
                              if (c_StringXml.SelectNodeChild("parents") == "parents")
                              {
                                 c_CurrentNode = c_StringXml.SelectNodeChild("message-parents");
                                 if (c_CurrentNode == "message-parents")
                                 {
                                    do
                                    {
                                       std::vector<QString>  c_OwnerNodeName;
                                       std::vector<uint32>  c_OwnerNodeInterfaceIndex;
                                       std::vector<bool>  c_OwnerIsTxFlag;
                                       stw_scl::C_SCLString c_CurrentNode2 = c_StringXml.SelectNodeChild(
                                          "message-parent");
                                       if (c_CurrentNode2 == "message-parent")
                                       {
                                          do
                                          {
                                             //Read
                                             c_OwnerNodeInterfaceIndex.push_back(c_StringXml.GetAttributeUint32(
                                                                                    "interface-index"));
                                             c_OwnerIsTxFlag.push_back(c_StringXml.GetAttributeBool("message-was-tx"));
                                             if (c_StringXml.SelectNodeChild("name") == "name")
                                             {
                                                c_OwnerNodeName.push_back(c_StringXml.GetNodeContent().c_str());
                                                //Return
                                                tgl_assert(c_StringXml.SelectNodeParent() == "message-parent");
                                             }
                                             //Next
                                             c_CurrentNode2 = c_StringXml.SelectNodeNext("message-parent");
                                          }
                                          while (c_CurrentNode2 == "message-parent");
                                          //Return
                                          tgl_assert(c_StringXml.SelectNodeParent() == "message-parents");
                                       }
                                       orc_OwnerNodeName.push_back(c_OwnerNodeName);
                                       orc_OwnerNodeInterfaceIndex.push_back(c_OwnerNodeInterfaceIndex);
                                       orc_OwnerIsTxFlag.push_back(c_OwnerIsTxFlag);
                                       c_CurrentNode = c_StringXml.SelectNodeNext("message-parents");
                                    }
                                    while (c_CurrentNode == "message-parents");
                                 }
                              }
                           }
                        }
                        else
                        {
                           s32_Retval = C_CONFIG;
                        }
                     }
                     else
                     {
                        s32_Retval = C_CONFIG;
                     }
                  }
                  else
                  {
                     s32_Retval = C_CONFIG;
                  }
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store signals to clipboard

   \param[in] orc_Signals          Signals data
   \param[in] orc_OSCSignalCommons Signal common osc data
   \param[in] orc_UISignalCommons  Signal common ui data
   \param[in] orc_UISignals        Signal ui data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreSignalsToClipboard(const std::vector<C_OSCCanSignal> & orc_Signals,
                                                    const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                                    const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals)
{
   QString c_String;

   mh_StoreSignalsToString(orc_Signals, orc_OSCSignalCommons, orc_UISignalCommons, orc_UISignals, c_String);
   C_SdClipBoardHelper::mh_SetClipBoard(c_String);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore signals from clipboard

   \param[out] orc_Signals          Signals data
   \param[out] orc_OSCSignalCommons Signal common osc data
   \param[out] orc_UISignalCommons  Signal common ui data
   \param[out] orc_UISignals        Signal ui data

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdClipBoardHelper::h_LoadSignalsFromClipboard(std::vector<C_OSCCanSignal> & orc_Signals,
                                                       std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons,
                                                       std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                                       std::vector<C_PuiSdNodeCanSignal> & orc_UISignals)
{
   const QString c_Input = C_SdClipBoardHelper::mh_GetClipBoard();
   sint32 s32_Retval = mh_LoadSignalsFromString(c_Input, orc_Signals, orc_OSCSignalCommons, orc_UISignalCommons,
                                                orc_UISignals);

   if (s32_Retval != C_NO_ERR)
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store message ids to string

   \param[in]  orc_MessageIds Message identification indices
   \param[out] orc_Output     String output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreMessageIndexToString(
   const std::vector<C_OSCCanMessageIdentificationIndices> & orc_MessageIds, QString & orc_Output)
{
   stw_scl::C_SCLString c_XMLContent;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("core");
   c_StringXml.CreateAndSelectNodeChild("message-ids");
   for (uint32 u32_ItMessageId = 0; u32_ItMessageId < orc_MessageIds.size(); ++u32_ItMessageId)
   {
      const C_OSCCanMessageIdentificationIndices & rc_CurrentElement = orc_MessageIds[u32_ItMessageId];
      c_StringXml.CreateAndSelectNodeChild("message-id");
      c_StringXml.SetAttributeUint32("node-index", rc_CurrentElement.u32_NodeIndex);
      c_StringXml.SetAttributeUint32("interface-index", rc_CurrentElement.u32_InterfaceIndex);
      c_StringXml.SetAttributeUint32("message-index", rc_CurrentElement.u32_MessageIndex);
      c_StringXml.SetAttributeBool("message-tx-flag", rc_CurrentElement.q_MessageIsTx);
      c_StringXml.CreateNodeChild("protocol-type",
                                  C_OSCNodeCommFiler::h_CommunicationProtocolToString(rc_CurrentElement.e_ComProtocol));
      //Return
      tgl_assert(c_StringXml.SelectNodeParent() == "message-ids");
   }

   //Final step
   c_StringXml.SaveToString(c_XMLContent);
   orc_Output = c_XMLContent.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore message ids from string

   \param[in]  orc_Input      String input
   \param[out] orc_MessageIds Message identification indices

   \return
   C_NO_ERR Found and loaded
   C_RANGE  String invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdClipBoardHelper::h_LoadMessageIndexFromString(const QString & orc_Input,
                                                         std::vector<C_OSCCanMessageIdentificationIndices> & orc_MessageIds)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.LoadFromString(orc_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("core") == "core")
      {
         if (c_StringXml.SelectNodeChild("message-ids") == "message-ids")
         {
            stw_scl::C_SCLString c_CurrentNode = c_StringXml.SelectNodeChild("message-id");
            while (c_CurrentNode == "message-id")
            {
               C_OSCCanMessageIdentificationIndices c_MessageId;

               c_MessageId.u32_NodeIndex = c_StringXml.GetAttributeUint32("node-index");
               c_MessageId.u32_InterfaceIndex = c_StringXml.GetAttributeUint32("interface-index");
               c_MessageId.u32_MessageIndex = c_StringXml.GetAttributeUint32("message-index");
               c_MessageId.q_MessageIsTx = c_StringXml.GetAttributeBool("message-tx-flag");

               if (c_StringXml.SelectNodeChild("protocol-type") == "protocol-type")
               {
                  if (C_OSCNodeCommFiler::h_StringToCommunicationProtocol(c_StringXml.GetNodeContent(),
                                                                          c_MessageId.e_ComProtocol) != C_NO_ERR)
                  {
                     s32_Retval = C_RANGE;
                  }
                  //Return
                  tgl_assert(c_StringXml.SelectNodeParent() == "message-id");
               }
               else
               {
                  s32_Retval = C_RANGE;
               }

               orc_MessageIds.push_back(c_MessageId);

               c_CurrentNode = c_StringXml.SelectNodeNext("message-id");
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store data snapshot to clipboard

   \param[in] orc_Data Snapshot data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreDataSnapShotToClipboard(const C_SdTopologyDataSnapshot & orc_Data)
{
   QString c_String;

   stw_scl::C_SCLString c_XMLContent;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("opensyde-system-definition");
   c_StringXml.CreateAndSelectNodeChild("nodes-core");
   C_OSCSystemDefinitionFiler::h_SaveNodes(orc_Data.c_OSCNodes, c_StringXml, "", NULL);
   tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
   c_StringXml.CreateAndSelectNodeChild("nodes-ui");
   C_PuiSdHandlerFiler::h_SaveNodes(orc_Data.c_UINodes, c_StringXml);
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
   c_StringXml.CreateAndSelectNodeChild("buses-core");
   C_OSCSystemDefinitionFiler::h_SaveBuses(orc_Data.c_OSCBuses, c_StringXml);
   tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
   c_StringXml.CreateAndSelectNodeChild("buses-ui");
   C_PuiSdHandlerFiler::h_SaveBuses(orc_Data.c_UIBuses, c_StringXml);
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
   c_StringXml.CreateAndSelectNodeChild("gui-only");
   c_StringXml.CreateAndSelectNodeChild("bus-text-elements");
   C_PuiSdHandlerFiler::h_SaveBusTextElements(orc_Data.c_BusTextElements, c_StringXml);
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui-only");

   C_PuiBsElementsFiler::h_SaveBaseElements(orc_Data, c_StringXml);

   //Final step
   c_StringXml.SaveToString(c_XMLContent);
   c_String = c_XMLContent.c_str();
   C_SdClipBoardHelper::mh_SetClipBoard(c_String);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore data snapshot from clipboard

   \param[out] orc_Data Snapshot data

   \return
   C_NO_ERR Found and loaded
   C_RANGE  Clip board invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdClipBoardHelper::h_LoadDataSnapShotFromClipboard(C_SdTopologyDataSnapshot & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;
   const QString c_Input = C_SdClipBoardHelper::mh_GetClipBoard();
   C_OSCXMLParserString c_StringXml;

   c_StringXml.LoadFromString(c_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "opensyde-system-definition")
   {
      if (c_StringXml.SelectNodeChild("nodes-core") == "nodes-core")
      {
         s32_Retval = C_OSCSystemDefinitionFiler::h_LoadNodes(orc_Data.c_OSCNodes, c_StringXml,
                                                              C_OSCSystemDefinition::hc_Devices, "", true, false);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
         }
      }
      if ((c_StringXml.SelectNodeChild("nodes-ui") == "nodes-ui") && (s32_Retval == C_NO_ERR))
      {
         s32_Retval = C_PuiSdHandlerFiler::h_LoadNodes(orc_Data.c_UINodes, c_StringXml, NULL);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
         }
      }
      if ((c_StringXml.SelectNodeChild("buses-core") == "buses-core") && (s32_Retval == C_NO_ERR))
      {
         s32_Retval = C_OSCSystemDefinitionFiler::h_LoadBuses(orc_Data.c_OSCBuses, c_StringXml);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
         }
      }
      if ((c_StringXml.SelectNodeChild("buses-ui") == "buses-ui") && (s32_Retval == C_NO_ERR))
      {
         s32_Retval = C_PuiSdHandlerFiler::h_LoadBuses(orc_Data.c_UIBuses, c_StringXml);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
         }
      }
      if ((c_StringXml.SelectNodeChild("gui-only") == "gui-only") && (s32_Retval == C_NO_ERR))
      {
         if (c_StringXml.SelectNodeChild("bus-text-elements") == "bus-text-elements")
         {
            s32_Retval =
               C_PuiSdHandlerFiler::h_LoadBusTextElements(orc_Data.c_BusTextElements, c_StringXml);
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(c_StringXml.SelectNodeParent() == "gui-only");
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = C_PuiBsElementsFiler::h_LoadBaseElements(orc_Data, c_StringXml);
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdClipBoardHelper::C_SdClipBoardHelper(void) :
   C_UtiClipBoardHelper()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store signals to string

   \param[in]  orc_Signals          Signals data
   \param[in]  orc_OSCSignalCommons Signal common osc data
   \param[in]  orc_UISignalCommons  Signal common ui data
   \param[in]  orc_UISignals        Signal ui data
   \param[out] orc_Output           String output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::mh_StoreSignalsToString(const std::vector<C_OSCCanSignal> & orc_Signals,
                                                  const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons, const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals,
                                                  QString & orc_Output)
{
   stw_scl::C_SCLString c_XMLContent;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("core");
   c_StringXml.CreateAndSelectNodeChild("com-signals");
   C_OSCNodeCommFiler::h_SaveNodeComSignals(orc_Signals, c_StringXml);
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "core");
   c_StringXml.CreateAndSelectNodeChild("data-elements");
   C_OSCNodeDataPoolFiler::h_SaveDataPoolListElements(orc_OSCSignalCommons, c_StringXml);
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "core");
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
   c_StringXml.CreateAndSelectNodeChild("gui");
   c_StringXml.CreateAndSelectNodeChild("data-elements");
   C_PuiSdHandlerFiler::h_SaveDataPoolListElements(orc_UISignalCommons, c_StringXml);

   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui");
   c_StringXml.CreateAndSelectNodeChild("com-signals");
   C_PuiSdHandlerFiler::h_SaveCanSignals(orc_UISignals, c_StringXml);

   //Final step
   c_StringXml.SaveToString(c_XMLContent);
   orc_Output = c_XMLContent.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore signals from string

   \param[in]  orc_Input            String input
   \param[out] orc_Signals          Signals data
   \param[out] orc_OSCSignalCommons Signal common osc data
   \param[out] orc_UISignalCommons  Signal common ui data
   \param[out] orc_UISignals        Signal ui data

   \return
   C_NO_ERR Found and loaded
   C_RANGE String invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdClipBoardHelper::mh_LoadSignalsFromString(const QString & orc_Input,
                                                     std::vector<C_OSCCanSignal> & orc_Signals,
                                                     std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons,
                                                     std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                                     std::vector<C_PuiSdNodeCanSignal> & orc_UISignals)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.LoadFromString(orc_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("core") == "core")
      {
         if (c_StringXml.SelectNodeChild("com-signals") == "com-signals")
         {
            s32_Retval = C_OSCNodeCommFiler::h_LoadNodeComSignals(orc_Signals, c_StringXml);
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(c_StringXml.SelectNodeParent() == "core");
               if (c_StringXml.SelectNodeChild("data-elements") == "data-elements")
               {
                  C_OSCNodeDataPoolFiler::h_LoadDataPoolListElements(orc_OSCSignalCommons, c_StringXml);
                  //Return
                  tgl_assert(c_StringXml.SelectNodeParent() == "core");
                  //Return
                  tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
                  if (c_StringXml.SelectNodeChild("gui") == "gui")
                  {
                     if (c_StringXml.SelectNodeChild("data-elements") == "data-elements")
                     {
                        C_PuiSdHandlerFiler::h_LoadDataPoolListElements(orc_UISignalCommons, c_StringXml);
                        //Return
                        tgl_assert(c_StringXml.SelectNodeParent() == "gui");
                        if (c_StringXml.SelectNodeChild("com-signals") == "com-signals")
                        {
                           C_PuiSdHandlerFiler::h_LoadCanSignals(orc_UISignals, c_StringXml);
                        }
                        else
                        {
                           s32_Retval = C_RANGE;
                        }
                     }
                     else
                     {
                        s32_Retval = C_RANGE;
                     }
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}
