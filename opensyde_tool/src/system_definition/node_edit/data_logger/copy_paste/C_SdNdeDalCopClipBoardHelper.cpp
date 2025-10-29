//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle datalogger clipboard load and save

   Handle datalogger clipboard load and save

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_OscDataLoggerJobFiler.hpp"
#include "C_SdNdeDalCopClipBoardHelper.hpp"
#include "C_PuiSdHandlerDataLoggerLogic.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SdNdeDalCopClipBoardHelper::mhc_CLIP_BOARD_BASE_TAG_NAME = "opensyde-data-loggers";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Store datalogger to clipboard

   \param[in]  orc_Data    Data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalCopClipBoardHelper::h_StoreDataloggerToClipboard(const std::vector<C_OscDataLoggerJob> & orc_Data)
{
   const QMap<C_OscNodeDataPoolListElementOptArrayId,
              C_SdNdeDalCopElementIdCrcGroup> c_ElementIdGroups = mh_FillElementIdGroups(orc_Data);

   mh_StoreDataloggerToClipboard(orc_Data, c_ElementIdGroups, mhc_CLIP_BOARD_BASE_TAG_NAME);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load datalogger from clipboard and remove invalid ids

   \param[in]      ou32_NodeIndex   Node index
   \param[in,out]  orc_Data         Data
   \param[out]     orq_IdsChanged   Ids changed

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDalCopClipBoardHelper::h_LoadDataloggerFromClipboardAndRemoveInvalidIds(const uint32_t ou32_NodeIndex,
                                                                                       std::vector<C_OscDataLoggerJob> & orc_Data,
                                                                                       bool & orq_IdsChanged)
{
   QMap<C_OscNodeDataPoolListElementOptArrayId,
        C_SdNdeDalCopElementIdCrcGroup> c_ElementIdGroups;
   const int32_t s32_Retval =
      mh_LoadDataloggerFromClipboard(orc_Data, c_ElementIdGroups);
   orq_IdsChanged = mh_ValidateCrcs(orc_Data, c_ElementIdGroups);
   orq_IdsChanged = orq_IdsChanged ||
                    C_PuiSdHandlerDataLoggerLogic::h_RemoveAllIdsForInvalidRoutesForOneNode(ou32_NodeIndex, orc_Data);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load datalogger from clipboard remove invalid ids and report changes

   \param[in]      ou32_NodeIndex   Node index
   \param[in,out]  orc_Data         Data
   \param[in,out]  opc_Parent       Parent

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDalCopClipBoardHelper::h_LoadDataloggerFromClipboardRemoveInvalidIdsAndReportChanges(
   const uint32_t ou32_NodeIndex, std::vector<C_OscDataLoggerJob> & orc_Data, QWidget * const opc_Parent)
{
   bool q_IdsChanged;
   const int32_t s32_Retval = h_LoadDataloggerFromClipboardAndRemoveInvalidIds(ou32_NodeIndex, orc_Data, q_IdsChanged);

   if (q_IdsChanged && (s32_Retval == C_NO_ERR))
   {
      C_OgeWiCustomMessage c_Message(opc_Parent);
      c_Message.SetType(C_OgeWiCustomMessage::eINFORMATION);
      c_Message.SetHeading(C_GtGetText::h_GetText("Log Job paste"));
      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "Some data elements were not found in your SYSTEM DEFINITION\n"
                                  "or can no longer be reached by this node.\n"
                                  "These were automatically removed from the pasted Log Job."));
      c_Message.SetCustomMinHeight(210, 210);
      c_Message.Execute();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalCopClipBoardHelper::C_SdNdeDalCopClipBoardHelper() :
   C_UtiClipBoardHelper()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store system view dashboard to clip board

   \param[in]  orc_Data             Data
   \param[in]  orc_ElementIdGroups  Element ID groups
   \param[in]  orc_GenericTagName   Generic content tag name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalCopClipBoardHelper::mh_StoreDataloggerToClipboard(const std::vector<C_OscDataLoggerJob> & orc_Data,
                                                                 const QMap<C_OscNodeDataPoolListElementOptArrayId,
                                                                            C_SdNdeDalCopElementIdCrcGroup> & orc_ElementIdGroups,
                                                                 const QString & orc_GenericTagName)
{
   stw::scl::C_SclString c_XmlContent;
   C_OscXmlParser c_StringXml;

   c_StringXml.CreateAndSelectNodeChild(orc_GenericTagName.toStdString().c_str());
   C_OscDataLoggerJobFiler::h_SaveData(orc_Data, c_StringXml);
   C_SdNdeDalCopClipBoardHelper::mh_StoreElementIdGroups(orc_ElementIdGroups, orc_GenericTagName, c_StringXml);
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
void C_SdNdeDalCopClipBoardHelper::mh_StoreElementIdGroups(const QMap<C_OscNodeDataPoolListElementOptArrayId,
                                                                      C_SdNdeDalCopElementIdCrcGroup> & orc_ElementIdGroups, const QString & orc_GenericTagName,
                                                           C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("element-id-groups");
   for (QMap<C_OscNodeDataPoolListElementOptArrayId,
             C_SdNdeDalCopElementIdCrcGroup>::ConstIterator c_It = orc_ElementIdGroups.cbegin();
        c_It != orc_ElementIdGroups.cend(); ++c_It)
   {
      C_SdNdeDalCopClipBoardHelper::mh_StoreElementIdGroup(c_It.key(), c_It.value(), orc_XmlParser);
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
void C_SdNdeDalCopClipBoardHelper::mh_StoreElementIdGroup(const C_OscNodeDataPoolListElementOptArrayId & orc_ElementId,
                                                          const C_SdNdeDalCopElementIdCrcGroup & orc_ElementIdGroup,
                                                          C_OscXmlParserBase & orc_XmlParser)
{
   tgl_assert(orc_ElementId == orc_ElementIdGroup.GetElementId());
   orc_XmlParser.CreateAndSelectNodeChild("element-id-group");
   orc_XmlParser.CreateAndSelectNodeChild("index");
   C_OscDataLoggerJobFiler::h_SaveDataElementOptArrayId(orc_ElementId, orc_XmlParser);
   tgl_assert(orc_XmlParser.SelectNodeParent() == "element-id-group");
   orc_XmlParser.SetAttributeUint32("crc", orc_ElementIdGroup.GetCrc());
   tgl_assert(orc_XmlParser.SelectNodeParent() == "element-id-groups");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system view dashboard from clip board

   \param[out]     orc_Data               DataData
   \param[in,out]  orc_ElementIdGroups    Element ID groups
   \param[in]      orc_GenericTagName     Generic content tag name

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDalCopClipBoardHelper::mh_LoadDataloggerFromClipboard(std::vector<C_OscDataLoggerJob> & orc_Data,
                                                                     QMap<C_OscNodeDataPoolListElementOptArrayId,
                                                                          C_SdNdeDalCopElementIdCrcGroup> & orc_ElementIdGroups,
                                                                     const QString & orc_GenericTagName)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscXmlParser c_StringXml;

   c_StringXml.LoadFromString(mh_GetClipBoard().toStdString().c_str());

   if (c_StringXml.SelectRoot() == orc_GenericTagName.toStdString().c_str())
   {
      s32_Retval = C_OscDataLoggerJobFiler::h_LoadData(orc_Data, c_StringXml);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            C_SdNdeDalCopClipBoardHelper::mh_LoadElementIdGroups(orc_ElementIdGroups, orc_GenericTagName, c_StringXml);
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
int32_t C_SdNdeDalCopClipBoardHelper::mh_LoadElementIdGroups(QMap<C_OscNodeDataPoolListElementOptArrayId,
                                                                  C_SdNdeDalCopElementIdCrcGroup> & orc_ElementIdGroups,
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
            C_OscNodeDataPoolListElementOptArrayId c_Id;
            C_SdNdeDalCopElementIdCrcGroup c_GroupData;
            s32_Retval = C_SdNdeDalCopClipBoardHelper::mh_LoadElementIdGroup(c_Id, c_GroupData, orc_XmlParser);
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
int32_t C_SdNdeDalCopClipBoardHelper::mh_LoadElementIdGroup(C_OscNodeDataPoolListElementOptArrayId & orc_ElementId,
                                                            C_SdNdeDalCopElementIdCrcGroup & orc_ElementIdGroup,
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
         C_OscDataLoggerJobFiler::h_LoadDataElementOptArrayId(orc_ElementId, orc_XmlParser);

         orc_ElementIdGroup.SetElementId(orc_ElementId);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "element-id-group");
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill element ID groups

   \param[in]  orc_Data    Data

   \return
   All element ID groups
*/
//----------------------------------------------------------------------------------------------------------------------
QMap<C_OscNodeDataPoolListElementOptArrayId,
     C_SdNdeDalCopElementIdCrcGroup> C_SdNdeDalCopClipBoardHelper::mh_FillElementIdGroups(
   const std::vector<C_OscDataLoggerJob> & orc_Data)
{
   QMap<C_OscNodeDataPoolListElementOptArrayId,
        C_SdNdeDalCopElementIdCrcGroup> c_Retval;
   const std::set<C_OscNodeDataPoolListElementOptArrayId> c_Ids = C_SdNdeDalCopClipBoardHelper::mh_GetAllIds(orc_Data);
   for (std::set<C_OscNodeDataPoolListElementOptArrayId>::const_iterator c_It = c_Ids.cbegin(); c_It != c_Ids.cend();
        ++c_It)
   {
      C_SdNdeDalCopElementIdCrcGroup c_Tmp;
      c_Tmp.SetElementId(*c_It);
      tgl_assert(c_Tmp.UpdateCrc() == C_NO_ERR);
      c_Retval.insert(*c_It, c_Tmp);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all ids

   \param[in]  orc_Data    Data

   \return
   All IDs
*/
//----------------------------------------------------------------------------------------------------------------------
std::set<C_OscNodeDataPoolListElementOptArrayId> C_SdNdeDalCopClipBoardHelper::mh_GetAllIds(
   const std::vector<C_OscDataLoggerJob> & orc_Data)
{
   std::set<C_OscNodeDataPoolListElementOptArrayId> c_Retval;
   for (uint32_t u32_ItLogger = 0UL; u32_ItLogger < orc_Data.size(); ++u32_ItLogger)
   {
      mh_GetAllIds(orc_Data[u32_ItLogger], c_Retval);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all ids

   \param[in]      orc_Data   Data
   \param[in,out]  orc_Ids    Ids
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalCopClipBoardHelper::mh_GetAllIds(const C_OscDataLoggerJob & orc_Data,
                                                std::set<C_OscNodeDataPoolListElementOptArrayId> & orc_Ids)
{
   for (uint32_t u32_ItDataEl = 0UL; u32_ItDataEl < orc_Data.c_ConfiguredDataElements.size(); ++u32_ItDataEl)
   {
      const C_OscDataLoggerDataElementReference & rc_DataEl = orc_Data.c_ConfiguredDataElements[u32_ItDataEl];
      orc_Ids.insert(rc_DataEl.c_ConfiguredElementId);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Validate ids

   \param[in]  orc_Data             Data
   \param[in]  orc_ElementIdGroups  Element ID groups

   \return
   STW error codes

   \retval   C_NO_ERR   All found
   \retval   C_CONFIG   At least one missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDalCopClipBoardHelper::mh_ValidateIds(const std::vector<C_OscDataLoggerJob> & orc_Data,
                                                     const QMap<C_OscNodeDataPoolListElementOptArrayId,
                                                                C_SdNdeDalCopElementIdCrcGroup> & orc_ElementIdGroups)
{
   int32_t s32_Retval = C_NO_ERR;
   const std::set<C_OscNodeDataPoolListElementOptArrayId> c_Ids = C_SdNdeDalCopClipBoardHelper::mh_GetAllIds(orc_Data);

   //Check
   for (std::set<C_OscNodeDataPoolListElementOptArrayId>::const_iterator c_It = c_Ids.cbegin(); c_It != c_Ids.cend();
        ++c_It)
   {
      if (!orc_ElementIdGroups.contains(*c_It))
      {
         s32_Retval = C_CONFIG;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Validate CRCs

   \param[in,out]  orc_Data               System view dashboard
   \param[in]      orc_ElementIdGroups    Element ID groups

   \return
   Flags

   \retval   True    Changes happened
   \retval   False   Nothing changed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalCopClipBoardHelper::mh_ValidateCrcs(std::vector<C_OscDataLoggerJob> & orc_Data,
                                                   const QMap<C_OscNodeDataPoolListElementOptArrayId,
                                                              C_SdNdeDalCopElementIdCrcGroup> & orc_ElementIdGroups)
{
   bool q_Retval = false;

   for (QMap<C_OscNodeDataPoolListElementOptArrayId,
             C_SdNdeDalCopElementIdCrcGroup>::ConstIterator c_It = orc_ElementIdGroups.cbegin();
        c_It != orc_ElementIdGroups.cend(); ++c_It)
   {
      if (c_It.value().CheckCrc() != C_NO_ERR)
      {
         mh_RemoveAllReferencesToElementId(orc_Data, c_It.key());
         q_Retval = true;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove all references to element id

   \param[in,out]  orc_Data   Data
   \param[in]      orc_Id     Id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalCopClipBoardHelper::mh_RemoveAllReferencesToElementId(std::vector<C_OscDataLoggerJob> & orc_Data,
                                                                     const C_OscNodeDataPoolListElementOptArrayId & orc_Id)
{
   for (uint32_t u32_It = 0UL; u32_It < orc_Data.size(); ++u32_It)
   {
      mh_RemoveAllReferencesToElementId(orc_Data[u32_It], orc_Id);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove all references to element id

   \param[in,out]  orc_Data   Data
   \param[in]      orc_Id     Id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalCopClipBoardHelper::mh_RemoveAllReferencesToElementId(C_OscDataLoggerJob & orc_Data,
                                                                     const C_OscNodeDataPoolListElementOptArrayId & orc_Id)
{
   if (orc_Data.c_Properties.c_AdditionalTriggerProperties.c_ElementId == orc_Id)
   {
      orc_Data.c_Properties.c_AdditionalTriggerProperties.q_Enable = false;
   }
   for (uint32_t u32_It = 0UL; u32_It < orc_Data.c_ConfiguredDataElements.size();)
   {
      const C_OscDataLoggerDataElementReference & rc_El = orc_Data.c_ConfiguredDataElements[u32_It];
      if (rc_El.c_ConfiguredElementId == orc_Id)
      {
         orc_Data.c_ConfiguredDataElements.erase(orc_Data.c_ConfiguredDataElements.begin() + u32_It);
      }
      else
      {
         ++u32_It;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load datalogger from clipboard

   \param[in,out]  orc_Data               Data
   \param[in,out]  orc_ElementIdGroups    Element id groups

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDalCopClipBoardHelper::mh_LoadDataloggerFromClipboard(std::vector<C_OscDataLoggerJob> & orc_Data,
                                                                     QMap<C_OscNodeDataPoolListElementOptArrayId,
                                                                          C_SdNdeDalCopElementIdCrcGroup> & orc_ElementIdGroups)
{
   int32_t s32_Retval =
      mh_LoadDataloggerFromClipboard(orc_Data, orc_ElementIdGroups, mhc_CLIP_BOARD_BASE_TAG_NAME);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_ValidateIds(orc_Data, orc_ElementIdGroups);
   }

   return s32_Retval;
}
