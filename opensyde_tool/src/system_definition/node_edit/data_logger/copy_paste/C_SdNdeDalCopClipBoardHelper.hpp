//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle datalogger clipboard load and save
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALCOPCLIPBOARDHELPER_HPP
#define C_SDNDEDALCOPCLIPBOARDHELPER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <set>
#include <vector>

#include <QMap>

#include "C_OscXmlParser.hpp"
#include "C_OscDataLoggerJob.hpp"
#include "C_UtiClipBoardHelper.hpp"
#include "C_SdNdeDalCopElementIdCrcGroup.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalCopClipBoardHelper :
   public C_UtiClipBoardHelper
{
public:
   static void h_StoreDataloggerToClipboard(const std::vector<stw::opensyde_core::C_OscDataLoggerJob> & orc_Data);
   static int32_t h_LoadDataloggerFromClipboardAndRemoveInvalidIds(const uint32_t ou32_NodeIndex,
                                                                   std::vector<stw::opensyde_core::C_OscDataLoggerJob> & orc_Data, bool & orq_IdsChanged);
   static int32_t h_LoadDataloggerFromClipboardRemoveInvalidIdsAndReportChanges(const uint32_t ou32_NodeIndex,
                                                                                std::vector<stw::opensyde_core::C_OscDataLoggerJob> & orc_Data, QWidget * const opc_Parent);

private:
   static const QString mhc_CLIP_BOARD_BASE_TAG_NAME;

   C_SdNdeDalCopClipBoardHelper();
   static void mh_StoreDataloggerToClipboard(const std::vector<stw::opensyde_core::C_OscDataLoggerJob> & orc_Data,
                                             const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                                        C_SdNdeDalCopElementIdCrcGroup> & orc_ElementIdGroups,
                                             const QString & orc_GenericTagName);
   static void mh_StoreElementIdGroups(const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                                  C_SdNdeDalCopElementIdCrcGroup> & orc_ElementIdGroups,
                                       const QString & orc_GenericTagName,
                                       stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_StoreElementIdGroup(const stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_ElementId,
                                      const C_SdNdeDalCopElementIdCrcGroup & orc_ElementIdGroup,
                                      stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadDataloggerFromClipboard(std::vector<stw::opensyde_core::C_OscDataLoggerJob> & orc_Data,
                                                 QMap<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                                      C_SdNdeDalCopElementIdCrcGroup> & orc_ElementIdGroups,
                                                 const QString & orc_GenericTagName);
   static int32_t mh_LoadElementIdGroups(QMap<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                              C_SdNdeDalCopElementIdCrcGroup> & orc_ElementIdGroups,
                                         const QString & orc_GenericTagName,
                                         stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadElementIdGroup(stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_ElementId,
                                        C_SdNdeDalCopElementIdCrcGroup & orc_ElementIdGroup,
                                        stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static QMap<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
               C_SdNdeDalCopElementIdCrcGroup> mh_FillElementIdGroups(
      const std::vector<stw::opensyde_core::C_OscDataLoggerJob> & orc_Data);
   static std::set<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId> mh_GetAllIds(
      const std::vector<stw::opensyde_core::C_OscDataLoggerJob> & orc_Data);
   static void mh_GetAllIds(const stw::opensyde_core::C_OscDataLoggerJob & orc_Data,
                            std::set<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId> & orc_Ids);
   static int32_t mh_ValidateIds(const std::vector<stw::opensyde_core::C_OscDataLoggerJob> & orc_Data,
                                 const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                            C_SdNdeDalCopElementIdCrcGroup> & orc_ElementIdGroups);
   static bool mh_ValidateCrcs(std::vector<stw::opensyde_core::C_OscDataLoggerJob> & orc_Data,
                               const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                          C_SdNdeDalCopElementIdCrcGroup> & orc_ElementIdGroups);
   static void mh_RemoveAllReferencesToElementId(std::vector<stw::opensyde_core::C_OscDataLoggerJob> & orc_Data,
                                                 const stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_Id);
   static void mh_RemoveAllReferencesToElementId(stw::opensyde_core::C_OscDataLoggerJob & orc_Data,
                                                 const stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_Id);
   static int32_t mh_LoadDataloggerFromClipboard(std::vector<stw::opensyde_core::C_OscDataLoggerJob> & orc_Data,
                                                 QMap<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                                      C_SdNdeDalCopElementIdCrcGroup> & orc_ElementIdGroups);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
