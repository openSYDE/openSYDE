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

#include "TGLUtils.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_SyvClipBoardHelper.h"
#include "C_PuiSvHandlerFiler.h"
#include "C_PuiSvDashboardFiler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store system view dashboard to clip board

   \param[in]  orc_Data             System view dashboard
   \param[in]  orc_Rails            System view dashboard rails
   \param[in]  orc_GenericTagName   Generic content tag name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvClipBoardHelper::h_StoreDashboardToClipboard(const C_PuiSvDashboard & orc_Data,
                                                       const QMap<C_OSCNodeDataPoolListElementId,
                                                                  C_PuiSvReadDataConfiguration> & orc_Rails,
                                                       const QString & orc_GenericTagName)
{
   const QMap<C_PuiSvDbNodeDataPoolListElementId,
              C_PuiSvDbElementIdCRCGroup> c_ElementIDGroups = C_SyvClipBoardHelper::mh_FillElementIDGroups(orc_Data,
                                                                                                           orc_Rails);

   C_SyvClipBoardHelper::mh_StoreDashboardToClipboard(orc_Data, orc_Rails, c_ElementIDGroups, orc_GenericTagName);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system view dashboard from clip board

   \param[out]     orc_Data               System view dashboard
   \param[in]      orc_Rails              System view dashboard rails
   \param[in,out]  orc_ElementIDGroups    Element ID groups
   \param[in]      orc_GenericTagName     Generic content tag name

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvClipBoardHelper::h_LoadDashboardFromClipboard(C_PuiSvDashboard & orc_Data,
                                                          QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                               C_PuiSvReadDataConfiguration> & orc_Rails,
                                                          QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                               C_PuiSvDbElementIdCRCGroup> & orc_ElementIDGroups,
                                                          const QString & orc_GenericTagName)
{
   sint32 s32_Retval =
      C_SyvClipBoardHelper::mh_LoadDashboardFromClipboard(orc_Data, orc_Rails, orc_ElementIDGroups, orc_GenericTagName);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_SyvClipBoardHelper::mh_ValidateIds(orc_Data, orc_Rails, orc_ElementIDGroups);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvClipBoardHelper::C_SyvClipBoardHelper(void) :
   C_UtiClipBoardHelper()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store system view dashboard to clip board

   \param[in]  orc_Data             System view dashboard
   \param[in]  orc_Rails            System view dashboard rails
   \param[in]  orc_ElementIDGroups  Element ID groups
   \param[in]  orc_GenericTagName   Generic content tag name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvClipBoardHelper::mh_StoreDashboardToClipboard(const C_PuiSvDashboard & orc_Data,
                                                        const QMap<C_OSCNodeDataPoolListElementId,
                                                                   C_PuiSvReadDataConfiguration> & orc_Rails,
                                                        const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                                   C_PuiSvDbElementIdCRCGroup> & orc_ElementIDGroups,
                                                        const QString & orc_GenericTagName)
{
   stw_scl::C_SCLString c_XMLContent;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.CreateAndSelectNodeChild(orc_GenericTagName.toStdString().c_str());
   c_StringXml.CreateAndSelectNodeChild("rail-assignments");
   C_PuiSvHandlerFiler::h_SaveReadRails(orc_Rails, c_StringXml);
   c_StringXml.SelectNodeParent();
   //System definition compatibility
   c_StringXml.CreateAndSelectNodeChild("gui-only");
   C_PuiSvDashboardFiler::h_SaveDashboard(orc_Data, c_StringXml);
   c_StringXml.SelectNodeParent();
   C_SyvClipBoardHelper::mh_StoreElementIDGroups(orc_ElementIDGroups, orc_GenericTagName, c_StringXml);
   c_StringXml.SaveToString(c_XMLContent);

   mh_SetClipBoard(c_XMLContent.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Store element ID groups

   \param[in]      orc_ElementIDGroups    Element ID groups
   \param[in]      orc_GenericTagName     Generic tag name
   \param[in,out]  orc_XMLParser          XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvClipBoardHelper::mh_StoreElementIDGroups(const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                              C_PuiSvDbElementIdCRCGroup> & orc_ElementIDGroups,
                                                   const QString & orc_GenericTagName,
                                                   C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("element-id-groups");
   for (QMap<C_PuiSvDbNodeDataPoolListElementId,
             C_PuiSvDbElementIdCRCGroup>::ConstIterator c_It = orc_ElementIDGroups.cbegin();
        c_It != orc_ElementIDGroups.cend(); ++c_It)
   {
      C_SyvClipBoardHelper::mh_StoreElementIDGroup(c_It.key(), c_It.value(), orc_XMLParser);
   }
   tgl_assert(orc_XMLParser.SelectNodeParent() == orc_GenericTagName.toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Store element ID group

   \param[in]      orc_ElementID       Element ID
   \param[in]      orc_ElementIDGroup  Element ID group
   \param[in,out]  orc_XMLParser       XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvClipBoardHelper::mh_StoreElementIDGroup(const C_PuiSvDbNodeDataPoolListElementId & orc_ElementID,
                                                  const C_PuiSvDbElementIdCRCGroup & orc_ElementIDGroup,
                                                  C_OSCXMLParserBase & orc_XMLParser)
{
   tgl_assert(orc_ElementID == orc_ElementIDGroup.GetElementId());
   orc_XMLParser.CreateAndSelectNodeChild("element-id-group");
   orc_XMLParser.CreateAndSelectNodeChild("index");
   C_PuiSvDashboardFiler::h_SaveUiIndex(orc_ElementID, orc_XMLParser);
   tgl_assert(orc_XMLParser.SelectNodeParent() == "element-id-group");
   orc_XMLParser.SetAttributeUint32("crc", orc_ElementIDGroup.GetCRC());
   tgl_assert(orc_XMLParser.SelectNodeParent() == "element-id-groups");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system view dashboard from clip board

   \param[out]     orc_Data               System view dashboard
   \param[out]     orc_Rails              System view dashboard rails
   \param[in,out]  orc_ElementIDGroups    Element ID groups
   \param[in]      orc_GenericTagName     Generic content tag name

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvClipBoardHelper::mh_LoadDashboardFromClipboard(C_PuiSvDashboard & orc_Data,
                                                           QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                                C_PuiSvReadDataConfiguration> & orc_Rails,
                                                           QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                                C_PuiSvDbElementIdCRCGroup> & orc_ElementIDGroups,
                                                           const QString & orc_GenericTagName)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.LoadFromString(mh_GetClipBoard().toStdString().c_str());

   if (c_StringXml.SelectRoot() == orc_GenericTagName.toStdString().c_str())
   {
      if (c_StringXml.SelectNodeChild("gui-only") == "gui-only")
      {
         s32_Retval = C_PuiSvDashboardFiler::h_LoadDashboard(orc_Data, c_StringXml, true);
         tgl_assert(c_StringXml.SelectNodeParent() == orc_GenericTagName.toStdString().c_str());
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      if ((s32_Retval == C_NO_ERR) && (c_StringXml.SelectNodeChild("rail-assignments") == "rail-assignments"))
      {
         s32_Retval = C_PuiSvHandlerFiler::h_LoadReadRails(orc_Rails, c_StringXml);
         tgl_assert(c_StringXml.SelectNodeParent() == orc_GenericTagName.toStdString().c_str());
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            C_SyvClipBoardHelper::mh_LoadElementIDGroups(orc_ElementIDGroups, orc_GenericTagName, c_StringXml);
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
/*! \brief  Load element ID groups

   \param[in,out]  orc_ElementIDGroups    Element ID groups
   \param[in]      orc_GenericTagName     Generic tag name
   \param[in,out]  orc_XMLParser          XML parser

   \return
   STW error codes

   \retval   C_NO_ERR   Found and loaded
   \retval   C_CONFIG   Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvClipBoardHelper::mh_LoadElementIDGroups(QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                         C_PuiSvDbElementIdCRCGroup> & orc_ElementIDGroups,
                                                    const QString & orc_GenericTagName,
                                                    C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = orc_XMLParser.SelectNodeChildError("element-id-groups");

   if  (s32_Retval == C_NO_ERR)
   {
      stw_scl::C_SCLString c_CurrentGroupNode = orc_XMLParser.SelectNodeChild("element-id-group");

      if (c_CurrentGroupNode == "element-id-group")
      {
         do
         {
            C_PuiSvDbNodeDataPoolListElementId c_Id;
            C_PuiSvDbElementIdCRCGroup c_GroupData;
            s32_Retval = C_SyvClipBoardHelper::mh_LoadElementIDGroup(c_Id, c_GroupData, orc_XMLParser);
            //Insert
            orc_ElementIDGroups.insert(c_Id, c_GroupData);
            //Next
            c_CurrentGroupNode = orc_XMLParser.SelectNodeNext("element-id-group");
         }
         while ((c_CurrentGroupNode == "element-id-group") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "element-id-groups");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == orc_GenericTagName.toStdString().c_str());
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load element ID group

   \param[in,out]  orc_ElementID       Element ID
   \param[in,out]  orc_ElementIDGroup  Element ID group
   \param[in,out]  orc_XMLParser       XML parser

   \return
   STW error codes

   \retval   C_NO_ERR   Found and loaded
   \retval   C_CONFIG   Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvClipBoardHelper::mh_LoadElementIDGroup(C_PuiSvDbNodeDataPoolListElementId & orc_ElementID,
                                                   C_PuiSvDbElementIdCRCGroup & orc_ElementIDGroup,
                                                   C_OSCXMLParserBase & orc_XMLParser)
{
   uint32 u32_Crc;
   sint32 s32_Retval = orc_XMLParser.GetAttributeUint32Error("crc", u32_Crc);

   orc_ElementIDGroup.SetCRC(u32_Crc);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XMLParser.SelectNodeChildError("index");
      if (s32_Retval == C_NO_ERR)
      {
         C_PuiSvDashboardFiler::h_LoadUiIndex(orc_ElementID, orc_XMLParser);

         orc_ElementIDGroup.SetElementId(orc_ElementID);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "element-id-group");
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill element ID groups

   \param[in]  orc_Data    System view dashboard
   \param[in]  orc_Rails   System view dashboard rails

   \return
   All element ID groups
*/
//----------------------------------------------------------------------------------------------------------------------
QMap<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvDbElementIdCRCGroup> C_SyvClipBoardHelper::mh_FillElementIDGroups(
   const C_PuiSvDashboard & orc_Data, const QMap<C_OSCNodeDataPoolListElementId,
                                                 C_PuiSvReadDataConfiguration> & orc_Rails)
{
   QMap<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvDbElementIdCRCGroup> c_Retval;
   const std::set<C_PuiSvDbNodeDataPoolListElementId> c_Ids = C_SyvClipBoardHelper::mh_GetAllIds(orc_Data, orc_Rails);
   for (std::set<C_PuiSvDbNodeDataPoolListElementId>::const_iterator c_It = c_Ids.cbegin(); c_It != c_Ids.cend();
        ++c_It)
   {
      C_PuiSvDbElementIdCRCGroup c_Tmp;
      c_Tmp.SetElementId(*c_It);
      tgl_assert(c_Tmp.UpdateCRC() == C_NO_ERR);
      c_Retval.insert(*c_It, c_Tmp);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all ids

   \param[in]  orc_Data    System view dashboard
   \param[in]  orc_Rails   System view dashboard rails

   \return
   All IDs
*/
//----------------------------------------------------------------------------------------------------------------------
std::set<C_PuiSvDbNodeDataPoolListElementId> C_SyvClipBoardHelper::mh_GetAllIds(const C_PuiSvDashboard & orc_Data,
                                                                                const QMap<C_OSCNodeDataPoolListElementId,
                                                                                           C_PuiSvReadDataConfiguration> & orc_Rails)
{
   std::set<C_PuiSvDbNodeDataPoolListElementId> c_Retval;
   orc_Data.GetAllRegisteredDashboardElementsGuiId(c_Retval);
   for (QMap<C_OSCNodeDataPoolListElementId,
             C_PuiSvReadDataConfiguration>::ConstIterator c_It = orc_Rails.cbegin(); c_It != orc_Rails.cend(); ++c_It)
   {
      const C_PuiSvDbNodeDataPoolListElementId c_Tmp(
         c_It.key(), C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false, 0UL);
      c_Retval.insert(c_Tmp);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Validate ids

   \param[in]  orc_Data             System view dashboard
   \param[in]  orc_Rails            System view dashboard rails
   \param[in]  orc_ElementIDGroups  Element ID groups

   \return
   STW error codes

   \retval   C_NO_ERR   All found
   \retval   C_CONFIG   At least one missing
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvClipBoardHelper::mh_ValidateIds(const C_PuiSvDashboard & orc_Data,
                                            const QMap<C_OSCNodeDataPoolListElementId,
                                                       C_PuiSvReadDataConfiguration> & orc_Rails,
                                            const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                       C_PuiSvDbElementIdCRCGroup> & orc_ElementIDGroups)
{
   sint32 s32_Retval = C_NO_ERR;
   const std::set<C_PuiSvDbNodeDataPoolListElementId> c_Ids = C_SyvClipBoardHelper::mh_GetAllIds(orc_Data, orc_Rails);

   //Check
   for (std::set<C_PuiSvDbNodeDataPoolListElementId>::const_iterator c_It = c_Ids.cbegin(); c_It != c_Ids.cend();
        ++c_It)
   {
      if (!orc_ElementIDGroups.contains(*c_It))
      {
         s32_Retval = C_CONFIG;
      }
   }

   return s32_Retval;
}
