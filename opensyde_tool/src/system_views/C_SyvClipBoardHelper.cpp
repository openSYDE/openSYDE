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

#include "TglUtils.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SyvClipBoardHelper.hpp"
#include "C_PuiSvHandlerFiler.hpp"
#include "C_PuiSvDashboardFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

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
                                                       const QMap<C_OscNodeDataPoolListElementId,
                                                                  C_PuiSvReadDataConfiguration> & orc_Rails,
                                                       const QString & orc_GenericTagName)
{
   const QMap<C_PuiSvDbNodeDataPoolListElementId,
              C_PuiSvDbElementIdCrcGroup> c_ElementIdGroups = C_SyvClipBoardHelper::mh_FillElementIdGroups(orc_Data,
                                                                                                           orc_Rails);

   C_SyvClipBoardHelper::mh_StoreDashboardToClipboard(orc_Data, orc_Rails, c_ElementIdGroups, orc_GenericTagName);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system view dashboard from clip board

   \param[out]     orc_Data               System view dashboard
   \param[in]      orc_Rails              System view dashboard rails
   \param[in,out]  orc_ElementIdGroups    Element ID groups
   \param[in]      orc_GenericTagName     Generic content tag name

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvClipBoardHelper::h_LoadDashboardFromClipboard(C_PuiSvDashboard & orc_Data,
                                                           QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                                                C_PuiSvReadDataConfiguration> & orc_Rails,
                                                           QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                                C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups,
                                                           const QString & orc_GenericTagName)
{
   int32_t s32_Retval =
      C_SyvClipBoardHelper::mh_LoadDashboardFromClipboard(orc_Data, orc_Rails, orc_ElementIdGroups, orc_GenericTagName);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_SyvClipBoardHelper::mh_ValidateIds(orc_Data, orc_Rails, orc_ElementIdGroups);
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
   \param[in]  orc_ElementIdGroups  Element ID groups
   \param[in]  orc_GenericTagName   Generic content tag name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvClipBoardHelper::mh_StoreDashboardToClipboard(const C_PuiSvDashboard & orc_Data,
                                                        const QMap<C_OscNodeDataPoolListElementId,
                                                                   C_PuiSvReadDataConfiguration> & orc_Rails,
                                                        const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                                   C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups,
                                                        const QString & orc_GenericTagName)
{
   stw::scl::C_SclString c_XmlContent;
   C_OscXmlParser c_StringXml;

   c_StringXml.CreateAndSelectNodeChild(orc_GenericTagName.toStdString().c_str());
   c_StringXml.CreateAndSelectNodeChild("rail-assignments");
   C_PuiSvHandlerFiler::h_SaveReadRails(orc_Rails, c_StringXml);
   c_StringXml.SelectNodeParent();
   //System definition compatibility
   c_StringXml.CreateAndSelectNodeChild("gui-only");
   C_PuiSvDashboardFiler::h_SaveDashboard(orc_Data, c_StringXml);
   c_StringXml.SelectNodeParent();
   C_SyvClipBoardHelper::mh_StoreElementIdGroups(orc_ElementIdGroups, orc_GenericTagName, c_StringXml);
   c_StringXml.SaveToString(c_XmlContent);

   mh_SetClipBoard(c_XmlContent.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Store element ID groups

   \param[in]      orc_ElementIdGroups    Element ID groups
   \param[in]      orc_GenericTagName     Generic tag name
   \param[in,out]  orc_XmlParser          XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvClipBoardHelper::mh_StoreElementIdGroups(const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                              C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups,
                                                   const QString & orc_GenericTagName,
                                                   C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("element-id-groups");
   for (QMap<C_PuiSvDbNodeDataPoolListElementId,
             C_PuiSvDbElementIdCrcGroup>::ConstIterator c_It = orc_ElementIdGroups.cbegin();
        c_It != orc_ElementIdGroups.cend(); ++c_It)
   {
      C_SyvClipBoardHelper::mh_StoreElementIdGroup(c_It.key(), c_It.value(), orc_XmlParser);
   }
   tgl_assert(orc_XmlParser.SelectNodeParent() == orc_GenericTagName.toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Store element ID group

   \param[in]      orc_ElementId       Element ID
   \param[in]      orc_ElementIdGroup  Element ID group
   \param[in,out]  orc_XmlParser       XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvClipBoardHelper::mh_StoreElementIdGroup(const C_PuiSvDbNodeDataPoolListElementId & orc_ElementId,
                                                  const C_PuiSvDbElementIdCrcGroup & orc_ElementIdGroup,
                                                  C_OscXmlParserBase & orc_XmlParser)
{
   tgl_assert(orc_ElementId == orc_ElementIdGroup.GetElementId());
   orc_XmlParser.CreateAndSelectNodeChild("element-id-group");
   orc_XmlParser.CreateAndSelectNodeChild("index");
   C_PuiSvDashboardFiler::h_SaveUiIndex(orc_ElementId, orc_XmlParser);
   tgl_assert(orc_XmlParser.SelectNodeParent() == "element-id-group");
   orc_XmlParser.SetAttributeUint32("crc", orc_ElementIdGroup.GetCrc());
   tgl_assert(orc_XmlParser.SelectNodeParent() == "element-id-groups");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system view dashboard from clip board

   \param[out]     orc_Data               System view dashboard
   \param[out]     orc_Rails              System view dashboard rails
   \param[in,out]  orc_ElementIdGroups    Element ID groups
   \param[in]      orc_GenericTagName     Generic content tag name

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvClipBoardHelper::mh_LoadDashboardFromClipboard(C_PuiSvDashboard & orc_Data,
                                                            QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                                                 C_PuiSvReadDataConfiguration> & orc_Rails,
                                                            QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                                 C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups,
                                                            const QString & orc_GenericTagName)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscXmlParser c_StringXml;

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
            C_SyvClipBoardHelper::mh_LoadElementIdGroups(orc_ElementIdGroups, orc_GenericTagName, c_StringXml);
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

   \param[in,out]  orc_ElementIdGroups    Element ID groups
   \param[in]      orc_GenericTagName     Generic tag name
   \param[in,out]  orc_XmlParser          XML parser

   \return
   STW error codes

   \retval   C_NO_ERR   Found and loaded
   \retval   C_CONFIG   Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvClipBoardHelper::mh_LoadElementIdGroups(QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                          C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups,
                                                     const QString & orc_GenericTagName,
                                                     C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = orc_XmlParser.SelectNodeChildError("element-id-groups");

   if  (s32_Retval == C_NO_ERR)
   {
      stw::scl::C_SclString c_CurrentGroupNode = orc_XmlParser.SelectNodeChild("element-id-group");

      if (c_CurrentGroupNode == "element-id-group")
      {
         do
         {
            C_PuiSvDbNodeDataPoolListElementId c_Id;
            C_PuiSvDbElementIdCrcGroup c_GroupData;
            s32_Retval = C_SyvClipBoardHelper::mh_LoadElementIdGroup(c_Id, c_GroupData, orc_XmlParser);
            //Insert
            orc_ElementIdGroups.insert(c_Id, c_GroupData);
            //Next
            c_CurrentGroupNode = orc_XmlParser.SelectNodeNext("element-id-group");
         }
         while ((c_CurrentGroupNode == "element-id-group") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "element-id-groups");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == orc_GenericTagName.toStdString().c_str());
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load element ID group

   \param[in,out]  orc_ElementId       Element ID
   \param[in,out]  orc_ElementIdGroup  Element ID group
   \param[in,out]  orc_XmlParser       XML parser

   \return
   STW error codes

   \retval   C_NO_ERR   Found and loaded
   \retval   C_CONFIG   Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvClipBoardHelper::mh_LoadElementIdGroup(C_PuiSvDbNodeDataPoolListElementId & orc_ElementId,
                                                    C_PuiSvDbElementIdCrcGroup & orc_ElementIdGroup,
                                                    C_OscXmlParserBase & orc_XmlParser)
{
   uint32_t u32_Crc;
   int32_t s32_Retval = orc_XmlParser.GetAttributeUint32Error("crc", u32_Crc);

   orc_ElementIdGroup.SetCrc(u32_Crc);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("index");
      if (s32_Retval == C_NO_ERR)
      {
         C_PuiSvDashboardFiler::h_LoadUiIndex(orc_ElementId, orc_XmlParser);

         orc_ElementIdGroup.SetElementId(orc_ElementId);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "element-id-group");
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
QMap<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvDbElementIdCrcGroup> C_SyvClipBoardHelper::mh_FillElementIdGroups(
   const C_PuiSvDashboard & orc_Data, const QMap<C_OscNodeDataPoolListElementId,
                                                 C_PuiSvReadDataConfiguration> & orc_Rails)
{
   QMap<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvDbElementIdCrcGroup> c_Retval;
   const std::set<C_PuiSvDbNodeDataPoolListElementId> c_Ids = C_SyvClipBoardHelper::mh_GetAllIds(orc_Data, orc_Rails);
   for (std::set<C_PuiSvDbNodeDataPoolListElementId>::const_iterator c_It = c_Ids.cbegin(); c_It != c_Ids.cend();
        ++c_It)
   {
      C_PuiSvDbElementIdCrcGroup c_Tmp;
      c_Tmp.SetElementId(*c_It);
      tgl_assert(c_Tmp.UpdateCrc() == C_NO_ERR);
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
                                                                                const QMap<C_OscNodeDataPoolListElementId,
                                                                                           C_PuiSvReadDataConfiguration> & orc_Rails)
{
   std::set<C_PuiSvDbNodeDataPoolListElementId> c_Retval;
   orc_Data.GetAllRegisteredDashboardElementsGuiId(c_Retval);
   for (QMap<C_OscNodeDataPoolListElementId,
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
   \param[in]  orc_ElementIdGroups  Element ID groups

   \return
   STW error codes

   \retval   C_NO_ERR   All found
   \retval   C_CONFIG   At least one missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvClipBoardHelper::mh_ValidateIds(const C_PuiSvDashboard & orc_Data,
                                             const QMap<C_OscNodeDataPoolListElementId,
                                                        C_PuiSvReadDataConfiguration> & orc_Rails,
                                             const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                        C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups)
{
   int32_t s32_Retval = C_NO_ERR;
   const std::set<C_PuiSvDbNodeDataPoolListElementId> c_Ids = C_SyvClipBoardHelper::mh_GetAllIds(orc_Data, orc_Rails);

   //Check
   for (std::set<C_PuiSvDbNodeDataPoolListElementId>::const_iterator c_It = c_Ids.cbegin(); c_It != c_Ids.cend();
        ++c_It)
   {
      if (!orc_ElementIdGroups.contains(*c_It))
      {
         s32_Retval = C_CONFIG;
      }
   }

   return s32_Retval;
}
