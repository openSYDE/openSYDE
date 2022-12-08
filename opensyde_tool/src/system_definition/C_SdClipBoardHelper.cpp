//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle clipboard load and save (implementation)

   Handle clipboard load and save

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QClipboard>
#include <QApplication>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SdClipBoardHelper.hpp"
#include "C_OscXmlParser.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSdHandlerFiler.hpp"
#include "C_PuiBsElementsFiler.hpp"
#include "C_OscNodeDataPoolFiler.hpp"
#include "C_OscNodeFiler.hpp"
#include "TglUtils.hpp"
#include "C_OscNodeCommFiler.hpp"
#include "C_OscNodeSquadFiler.hpp"
#include "C_OscSystemDefinitionFiler.hpp"
#include "C_OscHalcConfigStandaloneFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;
using namespace stw::errors;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store node data pool to clip board

   \param[in]  orc_OscContent    OSC content
   \param[in]  orc_UiContent     UI content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreDataPool(const C_OscNodeDataPool & orc_OscContent,
                                          const C_PuiSdNodeDataPool & orc_UiContent)
{
   stw::scl::C_SclString c_XmlContent;
   C_OscXmlParser c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("gui");
   c_StringXml.CreateAndSelectNodeChild("data-pool");

   C_PuiSdHandlerFiler::h_SaveDataPool(orc_UiContent, c_StringXml);

   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui");
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
   c_StringXml.CreateAndSelectNodeChild("core");
   c_StringXml.CreateAndSelectNodeChild("data-pool");

   C_OscNodeDataPoolFiler::h_SaveDataPool(orc_OscContent, c_StringXml);

   c_StringXml.SaveToString(c_XmlContent);
   mh_SetClipBoard(c_XmlContent.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node data pool from clip board

   \param[out]  orc_OscContent   OSC content
   \param[out]  orc_UiContent    UI content

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdClipBoardHelper::h_LoadToDataPool(C_OscNodeDataPool & orc_OscContent, C_PuiSdNodeDataPool & orc_UiContent)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscXmlParser c_StringXml;

   c_StringXml.LoadFromString(mh_GetClipBoard().toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("gui") == "gui")
      {
         if (c_StringXml.SelectNodeChild("data-pool") == "data-pool")
         {
            s32_Retval = C_PuiSdHandlerFiler::h_LoadDataPool(orc_UiContent, c_StringXml);
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
                     s32_Retval = C_OscNodeDataPoolFiler::h_LoadDataPool(orc_OscContent, c_StringXml);
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

   \param[in]  orc_OscContent    OSC content
   \param[in]  orc_UiContent     UI content
   \param[in]  ore_Type          Data pool type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreDataPoolLists(const std::vector<C_OscNodeDataPoolList> & orc_OscContent,
                                               const std::vector<C_PuiSdNodeDataPoolList> & orc_UiContent,
                                               const C_OscNodeDataPool::E_Type & ore_Type)
{
   stw::scl::C_SclString c_XmlContent;
   C_OscXmlParser c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("gui");
   c_StringXml.CreateAndSelectNodeChild("type");

   c_StringXml.SetNodeContent(C_OscNodeDataPoolFiler::h_DataPoolToString(ore_Type));

   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui");
   c_StringXml.CreateAndSelectNodeChild("lists");

   C_PuiSdHandlerFiler::h_SaveDataPoolLists(orc_UiContent, c_StringXml);

   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui");
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
   c_StringXml.CreateAndSelectNodeChild("core");
   c_StringXml.CreateAndSelectNodeChild("lists");

   C_OscNodeDataPoolFiler::h_SaveDataPoolLists(orc_OscContent, c_StringXml);

   c_StringXml.SaveToString(c_XmlContent);
   mh_SetClipBoard(c_XmlContent.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node data pool lists from clip board

   \param[out]  orc_OscContent   OSC content
   \param[out]  orc_UiContent    UI content
   \param[out]  ore_Type         Data pool type

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdClipBoardHelper::h_LoadToDataPoolLists(std::vector<C_OscNodeDataPoolList> & orc_OscContent,
                                                   std::vector<C_PuiSdNodeDataPoolList> & orc_UiContent,
                                                   C_OscNodeDataPool::E_Type & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscXmlParser c_StringXml;

   c_StringXml.LoadFromString(mh_GetClipBoard().toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("gui") == "gui")
      {
         if (c_StringXml.SelectNodeChild("type") == "type")
         {
            s32_Retval = C_OscNodeDataPoolFiler::h_StringToDataPool(c_StringXml.GetNodeContent(), ore_Type);
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(c_StringXml.SelectNodeParent() == "gui");
               if (c_StringXml.SelectNodeChild("lists") == "lists")
               {
                  s32_Retval = C_PuiSdHandlerFiler::h_LoadDataPoolLists(orc_UiContent, c_StringXml);
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
                           s32_Retval = C_OscNodeDataPoolFiler::h_LoadDataPoolLists(orc_OscContent, c_StringXml);
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

   \param[in]  orc_OscContent    OSC content
   \param[in]  orc_UiContent     UI content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreDataPoolListElementsToClipBoard(
   const std::vector<C_OscNodeDataPoolListElement> & orc_OscContent,
   const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiContent)
{
   QString c_Tmp;

   h_StoreDataPoolListElementsToString(orc_OscContent, orc_UiContent, c_Tmp);
   mh_SetClipBoard(c_Tmp);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node data pool list elements from clip board

   \param[in]  orc_OscContent    OSC content
   \param[in]  orc_UiContent     UI content

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdClipBoardHelper::h_LoadToDataPoolListElementsFromClipBoard(
   std::vector<C_OscNodeDataPoolListElement> & orc_OscContent,
   std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiContent)
{
   return h_LoadToDataPoolListElementsFromString(orc_OscContent, orc_UiContent, mh_GetClipBoard());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store node data pool list elements to string

   \param[in]   orc_OscContent   OSC content
   \param[in]   orc_UiContent    UI content
   \param[out]  orc_Output       String output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreDataPoolListElementsToString(
   const std::vector<C_OscNodeDataPoolListElement> & orc_OscContent,
   const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiContent, QString & orc_Output)
{
   stw::scl::C_SclString c_XmlContent;
   C_OscXmlParser c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("gui");
   c_StringXml.CreateAndSelectNodeChild("data-elements");

   C_PuiSdHandlerFiler::h_SaveDataPoolListElements(orc_UiContent, c_StringXml);

   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui");
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
   c_StringXml.CreateAndSelectNodeChild("core");
   c_StringXml.CreateAndSelectNodeChild("data-elements");

   C_OscNodeDataPoolFiler::h_SaveDataPoolListElements(orc_OscContent, c_StringXml);

   c_StringXml.SaveToString(c_XmlContent);
   orc_Output = c_XmlContent.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node data pool list elements from string

   \param[in]  orc_OscContent    OSC content
   \param[in]  orc_UiContent     UI content
   \param[in]  orc_Input         String input

   \return
   C_NO_ERR Found and loaded
   C_CONFIG String invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdClipBoardHelper::h_LoadToDataPoolListElementsFromString(
   std::vector<C_OscNodeDataPoolListElement> & orc_OscContent,
   std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiContent, const QString & orc_Input)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscXmlParser c_StringXml;

   c_StringXml.LoadFromString(orc_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("gui") == "gui")
      {
         if (c_StringXml.SelectNodeChild("data-elements") == "data-elements")
         {
            s32_Retval = C_PuiSdHandlerFiler::h_LoadDataPoolListElements(orc_UiContent, c_StringXml);
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
                     s32_Retval = C_OscNodeDataPoolFiler::h_LoadDataPoolListElements(orc_OscContent, c_StringXml);
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

   \param[in]   orc_Indices   Indices
   \param[out]  orc_Output    String output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreIndicesToString(const std::vector<uint32_t> & orc_Indices, QString & orc_Output)
{
   stw::scl::C_SclString c_XmlContent;
   C_OscXmlParser c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("indices");
   for (uint32_t u32_Index = 0; u32_Index < static_cast<uint32_t>(orc_Indices.size()); ++u32_Index)
   {
      c_StringXml.CreateAndSelectNodeChild("index");
      c_StringXml.SetAttributeUint32("number", orc_Indices.at(u32_Index));
      //Return
      tgl_assert(c_StringXml.SelectNodeParent() == "indices");
   }
   c_StringXml.SaveToString(c_XmlContent);
   orc_Output = c_XmlContent.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load indices from string

   \param[out]  orc_Indices   Indices
   \param[in]   orc_Input     String input

   \return
   C_NO_ERR Found and loaded
   C_CONFIG String invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdClipBoardHelper::h_LoadIndicesFromString(std::vector<uint32_t> & orc_Indices, const QString & orc_Input)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscXmlParser c_StringXml;

   c_StringXml.LoadFromString(orc_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("indices") == "indices")
      {
         stw::scl::C_SclString c_Node = c_StringXml.SelectNodeChild("index");
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

   \param[out]  orc_OscNames           Data set names
   \param[out]  orc_OscDataSetValues   Data set values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreDataPoolListDataSetsToClipBoard(
   const std::vector<C_OscNodeDataPoolDataSet> & orc_OscNames,
   const std::vector<std::vector<C_OscNodeDataPoolContent> > & orc_OscDataSetValues)
{
   QString c_Tmp;

   h_StoreDataPoolListDataSetsToString(orc_OscNames, orc_OscDataSetValues, c_Tmp);
   mh_SetClipBoard(c_Tmp);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data sets from clipboard

   \param[out]  orc_OscNames           Data set names
   \param[out]  orc_OscDataSetValues   Data set values

   \return
   C_NO_ERR   data read
   C_CONFIG   content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdClipBoardHelper::h_LoadToDataPoolListDataSetsFromClipBoard(
   std::vector<C_OscNodeDataPoolDataSet> & orc_OscNames,
   std::vector<std::vector<C_OscNodeDataPoolContent> > & orc_OscDataSetValues)
{
   return h_LoadToDataPoolListDataSetsFromString(orc_OscNames, orc_OscDataSetValues, mh_GetClipBoard());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store node data pool list data sets to string

   \param[out]  orc_OscNames           Data set names
   \param[out]  orc_OscDataSetValues   Data set values
   \param[out]  orc_Output             String output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreDataPoolListDataSetsToString(
   const std::vector<C_OscNodeDataPoolDataSet> & orc_OscNames,
   const std::vector<std::vector<C_OscNodeDataPoolContent> > & orc_OscDataSetValues, QString & orc_Output)
{
   stw::scl::C_SclString c_XmlContent;
   C_OscXmlParser c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("core");
   c_StringXml.CreateAndSelectNodeChild("data-sets");

   C_OscNodeDataPoolFiler::h_SaveDataPoolListDataSets(orc_OscNames, c_StringXml);

   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "core");
   c_StringXml.CreateAndSelectNodeChild("data-sets-2");
   for (uint32_t u32_ItDataSet = 0; u32_ItDataSet < orc_OscDataSetValues.size(); ++u32_ItDataSet)
   {
      const std::vector<C_OscNodeDataPoolContent> & rc_DataSetValues = orc_OscDataSetValues[u32_ItDataSet];
      c_StringXml.CreateAndSelectNodeChild("data-set-values");
      c_StringXml.SetAttributeUint32("index", u32_ItDataSet);
      for (uint32_t u32_ItType = 0; u32_ItType < rc_DataSetValues.size(); ++u32_ItType)
      {
         c_StringXml.CreateAndSelectNodeChild("value");
         C_OscNodeDataPoolFiler::h_SaveDataPoolContentV1(rc_DataSetValues[u32_ItType], c_StringXml);
         tgl_assert(c_StringXml.SelectNodeParent() == "data-set-values");
      }

      tgl_assert(c_StringXml.SelectNodeParent() == "data-sets-2");
   }

   c_StringXml.SaveToString(c_XmlContent);
   orc_Output = c_XmlContent.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data sets from clipboard

   \param[out]  orc_OscNames           Data set names
   \param[out]  orc_OscDataSetValues   Data set values
   \param[in]   orc_Input              String input

   \return
   C_NO_ERR   data read
   C_CONFIG   content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdClipBoardHelper::h_LoadToDataPoolListDataSetsFromString(
   std::vector<C_OscNodeDataPoolDataSet> & orc_OscNames,
   std::vector<std::vector<C_OscNodeDataPoolContent> > & orc_OscDataSetValues, const QString & orc_Input)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscXmlParser c_StringXml;

   c_StringXml.LoadFromString(orc_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("core") == "core")
      {
         if (c_StringXml.SelectNodeChild("data-sets") == "data-sets")
         {
            s32_Retval = C_OscNodeDataPoolFiler::h_LoadDataPoolListDataSets(orc_OscNames, c_StringXml);
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(c_StringXml.SelectNodeParent() == "core");
               if (c_StringXml.SelectNodeChild("data-sets-2") == "data-sets-2")
               {
                  stw::scl::C_SclString c_CurNodeDataSetValue = c_StringXml.SelectNodeChild("data-set-values");

                  if (c_CurNodeDataSetValue == "data-set-values")
                  {
                     do
                     {
                        std::vector<C_OscNodeDataPoolContent> c_CurDataSetValue;

                        if (s32_Retval == C_NO_ERR)
                        {
                           stw::scl::C_SclString c_CurNodeDataSetValueValue;
                           //Check if index as expected (in order)
                           tgl_assert(c_StringXml.GetAttributeUint32("index") == orc_OscDataSetValues.size());
                           //Load content
                           c_CurNodeDataSetValueValue = c_StringXml.SelectNodeChild("value");

                           if (c_CurNodeDataSetValueValue == "value")
                           {
                              do
                              {
                                 C_OscNodeDataPoolContent c_CurDataSetValueValue;

                                 if (s32_Retval == C_NO_ERR)
                                 {
                                    //Load content
                                    s32_Retval = C_OscNodeDataPoolFiler::h_LoadDataPoolContentV1(
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
                        orc_OscDataSetValues.push_back(c_CurDataSetValue);

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

   \param[in]  orc_Messages                  Message data
   \param[in]  orc_OscSignalCommons          Signal common osc data
   \param[in]  orc_UiSignalCommons           Signal common ui data
   \param[in]  orc_UiSignals                 Signal ui data
   \param[in]  orc_OwnerNodeName             Owner node names
   \param[in]  orc_OwnerNodeInterfaceIndex   Owner node interface index
   \param[in]  orc_OwnerNodeDatapoolIndex    Owner node Datapool index
   \param[in]  orc_OwnerIsTxFlag             Owner has message as Tx flags
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreMessages(const std::vector<C_OscCanMessage> & orc_Messages,
                                          const std::vector<std::vector<C_OscNodeDataPoolListElement> > & orc_OscSignalCommons, const std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > & orc_UiSignalCommons, const std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UiSignals, const std::vector<std::vector<QString> > & orc_OwnerNodeName, const std::vector<std::vector<uint32_t> > & orc_OwnerNodeInterfaceIndex, const std::vector<std::vector<uint32_t> > & orc_OwnerNodeDatapoolIndex,
                                          const std::vector<std::vector<bool> > & orc_OwnerIsTxFlag)
{
   QString c_String;

   stw::scl::C_SclString c_XmlContent;
   C_OscXmlParser c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("core");
   c_StringXml.CreateAndSelectNodeChild("message");
   C_OscNodeCommFiler::h_SaveNodeComMessages(orc_Messages, c_StringXml);
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "core");
   c_StringXml.CreateAndSelectNodeChild("message-common");
   for (uint32_t u32_ItMessage = 0; u32_ItMessage < orc_OscSignalCommons.size(); ++u32_ItMessage)
   {
      c_StringXml.CreateAndSelectNodeChild("data-elements");
      C_OscNodeDataPoolFiler::h_SaveDataPoolListElements(orc_OscSignalCommons[u32_ItMessage], c_StringXml);
      //Return
      tgl_assert(c_StringXml.SelectNodeParent() == "message-common");
   }
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "core");
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
   c_StringXml.CreateAndSelectNodeChild("gui");
   c_StringXml.CreateAndSelectNodeChild("message-common");
   for (uint32_t u32_ItMessage = 0; u32_ItMessage < orc_UiSignalCommons.size(); ++u32_ItMessage)
   {
      c_StringXml.CreateAndSelectNodeChild("data-elements");
      C_PuiSdHandlerFiler::h_SaveDataPoolListElements(orc_UiSignalCommons[u32_ItMessage], c_StringXml);
      //Return
      tgl_assert(c_StringXml.SelectNodeParent() == "message-common");
   }
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui");
   c_StringXml.CreateAndSelectNodeChild("message");
   for (uint32_t u32_ItMessage = 0; u32_ItMessage < orc_UiSignals.size(); ++u32_ItMessage)
   {
      c_StringXml.CreateAndSelectNodeChild("com-signals");
      C_PuiSdHandlerFiler::h_SaveCanSignals(orc_UiSignals[u32_ItMessage], c_StringXml);
      //Return
      tgl_assert(c_StringXml.SelectNodeParent() == "message");
   }
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui");
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");

   //Owner
   tgl_assert((orc_OwnerNodeName.size() == orc_OwnerNodeInterfaceIndex.size()) &&
              (orc_OwnerNodeName.size() == orc_OwnerNodeDatapoolIndex.size()) &&
              (orc_OwnerNodeName.size() == orc_OwnerIsTxFlag.size()));
   if ((orc_OwnerNodeName.size() == orc_OwnerNodeInterfaceIndex.size()) &&
       (orc_OwnerNodeName.size() == orc_OwnerNodeDatapoolIndex.size()) &&
       (orc_OwnerNodeName.size() == orc_OwnerIsTxFlag.size()))
   {
      c_StringXml.CreateAndSelectNodeChild("parents");
      for (uint32_t u32_ItMessage = 0; u32_ItMessage < orc_OwnerNodeName.size(); ++u32_ItMessage)
      {
         const std::vector<QString>  & rc_OwnerNodeName = orc_OwnerNodeName[u32_ItMessage];
         const std::vector<uint32_t>  & rc_OwnerNodeInterfaceIndex = orc_OwnerNodeInterfaceIndex[u32_ItMessage];
         const std::vector<uint32_t>  & rc_OwnerNodeDatapoolIndex = orc_OwnerNodeDatapoolIndex[u32_ItMessage];
         const std::vector<bool>  & rc_OwnerIsTxFlag = orc_OwnerIsTxFlag[u32_ItMessage];
         c_StringXml.CreateAndSelectNodeChild("message-parents");
         tgl_assert((rc_OwnerNodeName.size() == rc_OwnerNodeInterfaceIndex.size()) &&
                    (rc_OwnerNodeName.size() == rc_OwnerNodeDatapoolIndex.size()) &&
                    (rc_OwnerNodeName.size() == rc_OwnerIsTxFlag.size()));
         if ((rc_OwnerNodeName.size() == rc_OwnerNodeInterfaceIndex.size()) &&
             (rc_OwnerNodeName.size() == rc_OwnerNodeDatapoolIndex.size()) &&
             (rc_OwnerNodeName.size() == rc_OwnerIsTxFlag.size()))
         {
            for (uint32_t u32_ItOwner = 0; u32_ItOwner < rc_OwnerNodeName.size(); ++u32_ItOwner)
            {
               const QString & rc_CurName = rc_OwnerNodeName[u32_ItOwner];
               c_StringXml.CreateAndSelectNodeChild("message-parent");
               c_StringXml.SetAttributeUint32("interface-index", rc_OwnerNodeInterfaceIndex[u32_ItOwner]);
               c_StringXml.SetAttributeUint32("datapool-index", rc_OwnerNodeDatapoolIndex[u32_ItOwner]);
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
   c_StringXml.SaveToString(c_XmlContent);
   c_String = c_XmlContent.c_str();
   C_SdClipBoardHelper::mh_SetClipBoard(c_String);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore messages from clipboard

   \param[out]  orc_Messages                 Message data
   \param[out]  orc_OscSignalCommons         Signal common osc data
   \param[out]  orc_UiSignalCommons          Signal common ui data
   \param[out]  orc_UiSignals                Signal ui data
   \param[out]  orc_OwnerNodeName            Owner node names
   \param[out]  orc_OwnerNodeInterfaceIndex  Owner node interface index
   \param[out]  orc_OwnerNodeDatapoolIndex   Owner node Datapool index
   \param[out]  orc_OwnerIsTxFlag            Owner has message as Tx flags

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdClipBoardHelper::h_LoadMessages(std::vector<C_OscCanMessage> & orc_Messages,
                                            std::vector<std::vector<C_OscNodeDataPoolListElement> > & orc_OscSignalCommons, std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > & orc_UiSignalCommons, std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UiSignals, std::vector<std::vector<QString> > & orc_OwnerNodeName, std::vector<std::vector<uint32_t> > & orc_OwnerNodeInterfaceIndex, std::vector<std::vector<uint32_t> > & orc_OwnerNodeDatapoolIndex,
                                            std::vector<std::vector<bool> > & orc_OwnerIsTxFlag)
{
   int32_t s32_Retval = C_NO_ERR;
   const QString c_Input = C_SdClipBoardHelper::mh_GetClipBoard();
   C_OscXmlParser c_StringXml;

   c_StringXml.LoadFromString(c_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("core") == "core")
      {
         if (c_StringXml.SelectNodeChild("message") == "message")
         {
            s32_Retval = C_OscNodeCommFiler::h_LoadNodeComMessages(orc_Messages, c_StringXml);
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(c_StringXml.SelectNodeParent() == "core");
               if (c_StringXml.SelectNodeChild("message-common") == "message-common")
               {
                  stw::scl::C_SclString c_CurrentNode = c_StringXml.SelectNodeChild("data-elements");
                  if (c_CurrentNode == "data-elements")
                  {
                     std::vector<C_OscNodeDataPoolListElement> c_Tmp;
                     do
                     {
                        c_Tmp.clear();
                        C_OscNodeDataPoolFiler::h_LoadDataPoolListElements(c_Tmp, c_StringXml);
                        orc_OscSignalCommons.push_back(c_Tmp);
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
                              orc_UiSignalCommons.push_back(c_Tmp);
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
                                 orc_UiSignals.push_back(c_Tmp);
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
                                       std::vector<uint32_t>  c_OwnerNodeInterfaceIndex;
                                       std::vector<uint32_t>  c_OwnerNodeDatapoolIndex;
                                       std::vector<bool>  c_OwnerIsTxFlag;
                                       stw::scl::C_SclString c_CurrentNode2 = c_StringXml.SelectNodeChild(
                                          "message-parent");
                                       if (c_CurrentNode2 == "message-parent")
                                       {
                                          do
                                          {
                                             //Read
                                             c_OwnerNodeInterfaceIndex.push_back(c_StringXml.GetAttributeUint32(
                                                                                    "interface-index"));
                                             c_OwnerNodeDatapoolIndex.push_back(c_StringXml.GetAttributeUint32(
                                                                                   "datapool-index"));
                                             c_OwnerIsTxFlag.push_back(c_StringXml.GetAttributeBool("message-was-tx"));
                                             if (c_StringXml.SelectNodeChild("name") == "name")
                                             {
                                                c_OwnerNodeName.emplace_back(c_StringXml.GetNodeContent().c_str());
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
                                       orc_OwnerNodeDatapoolIndex.push_back(c_OwnerNodeDatapoolIndex);
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

   \param[in]  orc_Signals             Signals data
   \param[in]  orc_OscSignalCommons    Signal common osc data
   \param[in]  orc_UiSignalCommons     Signal common ui data
   \param[in]  orc_UiSignals           Signal ui data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreSignalsToClipboard(const std::vector<C_OscCanSignal> & orc_Signals,
                                                    const std::vector<C_OscNodeDataPoolListElement> & orc_OscSignalCommons, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons,
                                                    const std::vector<C_PuiSdNodeCanSignal> & orc_UiSignals)
{
   QString c_String;

   mh_StoreSignalsToString(orc_Signals, orc_OscSignalCommons, orc_UiSignalCommons, orc_UiSignals, c_String);
   C_SdClipBoardHelper::mh_SetClipBoard(c_String);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore signals from clipboard

   \param[out]  orc_Signals            Signals data
   \param[out]  orc_OscSignalCommons   Signal common osc data
   \param[out]  orc_UiSignalCommons    Signal common ui data
   \param[out]  orc_UiSignals          Signal ui data

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdClipBoardHelper::h_LoadSignalsFromClipboard(std::vector<C_OscCanSignal> & orc_Signals,
                                                        std::vector<C_OscNodeDataPoolListElement> & orc_OscSignalCommons, std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons,
                                                        std::vector<C_PuiSdNodeCanSignal> & orc_UiSignals)
{
   const QString c_Input = C_SdClipBoardHelper::mh_GetClipBoard();
   int32_t s32_Retval = mh_LoadSignalsFromString(c_Input, orc_Signals, orc_OscSignalCommons, orc_UiSignalCommons,
                                                 orc_UiSignals);

   if (s32_Retval != C_NO_ERR)
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store message ids to string

   \param[in]   orc_MessageIds   Message identification indices
   \param[out]  orc_Output       String output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreMessageIndexToString(
   const std::vector<C_OscCanMessageIdentificationIndices> & orc_MessageIds, QString & orc_Output)
{
   stw::scl::C_SclString c_XmlContent;
   C_OscXmlParser c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("core");
   c_StringXml.CreateAndSelectNodeChild("message-ids");
   for (uint32_t u32_ItMessageId = 0; u32_ItMessageId < orc_MessageIds.size(); ++u32_ItMessageId)
   {
      const C_OscCanMessageIdentificationIndices & rc_CurrentElement = orc_MessageIds[u32_ItMessageId];
      c_StringXml.CreateAndSelectNodeChild("message-id");
      c_StringXml.SetAttributeUint32("node-index", rc_CurrentElement.u32_NodeIndex);
      c_StringXml.SetAttributeUint32("interface-index", rc_CurrentElement.u32_InterfaceIndex);
      c_StringXml.SetAttributeUint32("message-index", rc_CurrentElement.u32_MessageIndex);
      c_StringXml.SetAttributeUint32("datapool-index", rc_CurrentElement.u32_DatapoolIndex);
      c_StringXml.SetAttributeBool("message-tx-flag", rc_CurrentElement.q_MessageIsTx);
      c_StringXml.CreateNodeChild("protocol-type",
                                  C_OscNodeCommFiler::h_CommunicationProtocolToString(rc_CurrentElement.e_ComProtocol));
      //Return
      tgl_assert(c_StringXml.SelectNodeParent() == "message-ids");
   }

   //Final step
   c_StringXml.SaveToString(c_XmlContent);
   orc_Output = c_XmlContent.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore message ids from string

   \param[in]   orc_Input        String input
   \param[out]  orc_MessageIds   Message identification indices

   \return
   C_NO_ERR Found and loaded
   C_RANGE  String invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdClipBoardHelper::h_LoadMessageIndexFromString(const QString & orc_Input,
                                                          std::vector<C_OscCanMessageIdentificationIndices> & orc_MessageIds)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscXmlParser c_StringXml;

   c_StringXml.LoadFromString(orc_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("core") == "core")
      {
         if (c_StringXml.SelectNodeChild("message-ids") == "message-ids")
         {
            stw::scl::C_SclString c_CurrentNode = c_StringXml.SelectNodeChild("message-id");
            while (c_CurrentNode == "message-id")
            {
               C_OscCanMessageIdentificationIndices c_MessageId;

               c_MessageId.u32_NodeIndex = c_StringXml.GetAttributeUint32("node-index");
               c_MessageId.u32_InterfaceIndex = c_StringXml.GetAttributeUint32("interface-index");
               c_MessageId.u32_MessageIndex = c_StringXml.GetAttributeUint32("message-index");
               c_MessageId.u32_DatapoolIndex = c_StringXml.GetAttributeUint32("datapool-index");
               c_MessageId.q_MessageIsTx = c_StringXml.GetAttributeBool("message-tx-flag");

               if (c_StringXml.SelectNodeChild("protocol-type") == "protocol-type")
               {
                  if (C_OscNodeCommFiler::h_StringToCommunicationProtocol(c_StringXml.GetNodeContent(),
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

   \param[in]  orc_Data    Snapshot data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreDataSnapShotToClipboard(const C_SdTopologyDataSnapshot & orc_Data)
{
   QString c_String;

   stw::scl::C_SclString c_XmlContent;
   C_OscXmlParser c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("opensyde-system-definition");
   C_OscNodeSquadFiler::h_SaveNodeGroups(orc_Data.c_OscNodeGroups, c_StringXml);
   c_StringXml.CreateAndSelectNodeChild("nodes-core");
   C_OscSystemDefinitionFiler::h_SaveNodes(orc_Data.c_OscNodes, c_StringXml, "", NULL);
   tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
   c_StringXml.CreateAndSelectNodeChild("nodes-ui");
   C_PuiSdHandlerFiler::h_SaveNodes(orc_Data.c_UiNodes, c_StringXml);
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
   c_StringXml.CreateAndSelectNodeChild("buses-core");
   C_OscSystemDefinitionFiler::h_SaveBuses(orc_Data.c_OscBuses, c_StringXml);
   tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
   c_StringXml.CreateAndSelectNodeChild("buses-ui");
   C_PuiSdHandlerFiler::h_SaveBuses(orc_Data.c_UiBuses, c_StringXml);
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
   c_StringXml.CreateAndSelectNodeChild("gui-only");
   c_StringXml.CreateAndSelectNodeChild("bus-text-elements");
   C_PuiSdHandlerFiler::h_SaveBusTextElements(orc_Data.c_BusTextElements, c_StringXml);
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui-only");

   C_PuiBsElementsFiler::h_SaveBaseElements(orc_Data, c_StringXml);

   //Final step
   c_StringXml.SaveToString(c_XmlContent);
   c_String = c_XmlContent.c_str();
   C_SdClipBoardHelper::mh_SetClipBoard(c_String);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore data snapshot from clipboard

   \param[out]  orc_Data   Snapshot data

   \return
   C_NO_ERR Found and loaded
   C_RANGE  Clip board invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdClipBoardHelper::h_LoadDataSnapShotFromClipboard(C_SdTopologyDataSnapshot & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;
   const QString c_Input = C_SdClipBoardHelper::mh_GetClipBoard();
   C_OscXmlParser c_StringXml;

   c_StringXml.LoadFromString(c_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "opensyde-system-definition")
   {
      C_OscNodeSquadFiler::h_LoadNodeGroups(orc_Data.c_OscNodeGroups, c_StringXml);
      if (c_StringXml.SelectNodeChild("nodes-core") == "nodes-core")
      {
         s32_Retval = C_OscSystemDefinitionFiler::h_LoadNodes(orc_Data.c_OscNodes, c_StringXml,
                                                              C_OscSystemDefinition::hc_Devices, "", true, false);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
         }
      }
      if ((c_StringXml.SelectNodeChild("nodes-ui") == "nodes-ui") && (s32_Retval == C_NO_ERR))
      {
         s32_Retval = C_PuiSdHandlerFiler::h_LoadNodes(orc_Data.c_UiNodes, c_StringXml, NULL);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
         }
      }
      if ((c_StringXml.SelectNodeChild("buses-core") == "buses-core") && (s32_Retval == C_NO_ERR))
      {
         s32_Retval = C_OscSystemDefinitionFiler::h_LoadBuses(orc_Data.c_OscBuses, c_StringXml);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(c_StringXml.SelectNodeParent() == "opensyde-system-definition");
         }
      }
      if ((c_StringXml.SelectNodeChild("buses-ui") == "buses-ui") && (s32_Retval == C_NO_ERR))
      {
         s32_Retval = C_PuiSdHandlerFiler::h_LoadBuses(orc_Data.c_UiBuses, c_StringXml);
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
/*! \brief   Store HALC item configuration to clipboard

   \param[in]  orc_Data    Snapshot data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::h_StoreHalcItemConfigToClipboard(const C_OscHalcConfigStandalone & orc_Data)
{
   int32_t s32_Retval;
   QString c_String;

   stw::scl::C_SclString c_XmlContent;
   C_OscXmlParser c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");

   s32_Retval = C_OscHalcConfigStandaloneFiler::h_SaveDataStandalone(orc_Data, c_StringXml);

   if (s32_Retval == C_NO_ERR)
   {
      //Final step
      c_StringXml.SaveToString(c_XmlContent);
      c_String = c_XmlContent.c_str();
      C_SdClipBoardHelper::mh_SetClipBoard(c_String);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore HALC item configuration from clipboard

   \param[out]  orc_Data   Snapshot data

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clip board invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdClipBoardHelper::h_LoadHalcItemConfigFromClipboard(C_OscHalcConfigStandalone & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;
   const QString c_Input = C_SdClipBoardHelper::mh_GetClipBoard();
   C_OscXmlParser c_StringXml;

   c_StringXml.LoadFromString(c_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      s32_Retval = C_OscHalcConfigStandaloneFiler::h_LoadDataStandalone(orc_Data, c_StringXml);
   }
   else
   {
      s32_Retval = C_CONFIG;
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

   \param[in]   orc_Signals            Signals data
   \param[in]   orc_OscSignalCommons   Signal common osc data
   \param[in]   orc_UiSignalCommons    Signal common ui data
   \param[in]   orc_UiSignals          Signal ui data
   \param[out]  orc_Output             String output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdClipBoardHelper::mh_StoreSignalsToString(const std::vector<C_OscCanSignal> & orc_Signals,
                                                  const std::vector<C_OscNodeDataPoolListElement> & orc_OscSignalCommons, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons, const std::vector<C_PuiSdNodeCanSignal> & orc_UiSignals,
                                                  QString & orc_Output)
{
   stw::scl::C_SclString c_XmlContent;
   C_OscXmlParser c_StringXml;

   c_StringXml.CreateAndSelectNodeChild("clip-board");
   c_StringXml.CreateAndSelectNodeChild("core");
   c_StringXml.CreateAndSelectNodeChild("com-signals");
   C_OscNodeCommFiler::h_SaveNodeComSignals(orc_Signals, c_StringXml);
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "core");
   c_StringXml.CreateAndSelectNodeChild("data-elements");
   C_OscNodeDataPoolFiler::h_SaveDataPoolListElements(orc_OscSignalCommons, c_StringXml);
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "core");
   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
   c_StringXml.CreateAndSelectNodeChild("gui");
   c_StringXml.CreateAndSelectNodeChild("data-elements");
   C_PuiSdHandlerFiler::h_SaveDataPoolListElements(orc_UiSignalCommons, c_StringXml);

   //Return
   tgl_assert(c_StringXml.SelectNodeParent() == "gui");
   c_StringXml.CreateAndSelectNodeChild("com-signals");
   C_PuiSdHandlerFiler::h_SaveCanSignals(orc_UiSignals, c_StringXml);

   //Final step
   c_StringXml.SaveToString(c_XmlContent);
   orc_Output = c_XmlContent.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore signals from string

   \param[in]   orc_Input              String input
   \param[out]  orc_Signals            Signals data
   \param[out]  orc_OscSignalCommons   Signal common osc data
   \param[out]  orc_UiSignalCommons    Signal common ui data
   \param[out]  orc_UiSignals          Signal ui data

   \return
   C_NO_ERR Found and loaded
   C_RANGE String invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdClipBoardHelper::mh_LoadSignalsFromString(const QString & orc_Input,
                                                      std::vector<C_OscCanSignal> & orc_Signals,
                                                      std::vector<C_OscNodeDataPoolListElement> & orc_OscSignalCommons,
                                                      std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons,
                                                      std::vector<C_PuiSdNodeCanSignal> & orc_UiSignals)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscXmlParser c_StringXml;

   c_StringXml.LoadFromString(orc_Input.toStdString().c_str());

   if (c_StringXml.SelectRoot() == "clip-board")
   {
      if (c_StringXml.SelectNodeChild("core") == "core")
      {
         if (c_StringXml.SelectNodeChild("com-signals") == "com-signals")
         {
            s32_Retval = C_OscNodeCommFiler::h_LoadNodeComSignals(orc_Signals, c_StringXml);
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(c_StringXml.SelectNodeParent() == "core");
               if (c_StringXml.SelectNodeChild("data-elements") == "data-elements")
               {
                  C_OscNodeDataPoolFiler::h_LoadDataPoolListElements(orc_OscSignalCommons, c_StringXml);
                  //Return
                  tgl_assert(c_StringXml.SelectNodeParent() == "core");
                  //Return
                  tgl_assert(c_StringXml.SelectNodeParent() == "clip-board");
                  if (c_StringXml.SelectNodeChild("gui") == "gui")
                  {
                     if (c_StringXml.SelectNodeChild("data-elements") == "data-elements")
                     {
                        C_PuiSdHandlerFiler::h_LoadDataPoolListElements(orc_UiSignalCommons, c_StringXml);
                        //Return
                        tgl_assert(c_StringXml.SelectNodeParent() == "gui");
                        if (c_StringXml.SelectNodeChild("com-signals") == "com-signals")
                        {
                           C_PuiSdHandlerFiler::h_LoadCanSignals(orc_UiSignals, c_StringXml);
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
