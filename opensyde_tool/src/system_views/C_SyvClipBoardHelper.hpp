//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle clipboard load and save (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVCLIPBOARDHELPER_H
#define C_SYVCLIPBOARDHELPER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscXmlParser.hpp"
#include "C_PuiSvDashboard.hpp"
#include "C_UtiClipBoardHelper.hpp"
#include "C_PuiSvDbElementIdCrcGroup.hpp"
#include "C_PuiSvReadDataConfiguration.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvClipBoardHelper :
   public C_UtiClipBoardHelper
{
public:
   static void h_StoreDashboardToClipboard(const C_PuiSvDashboard & orc_Data,
                                           const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                                      C_PuiSvReadDataConfiguration> & orc_Rails,
                                           const QString & orc_GenericTagName);
   static int32_t h_LoadDashboardFromClipboard(C_PuiSvDashboard & orc_Data,
                                               QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                                    C_PuiSvReadDataConfiguration> & orc_Rails,
                                               QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                    C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups,
                                               const QString & orc_GenericTagName);

private:
   C_SyvClipBoardHelper(void);
   static void mh_StoreDashboardToClipboard(const C_PuiSvDashboard & orc_Data,
                                            const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                                       C_PuiSvReadDataConfiguration> & orc_Rails,
                                            const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                       C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups,
                                            const QString & orc_GenericTagName);
   static void mh_StoreElementIdGroups(const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                  C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups,
                                       const QString & orc_GenericTagName,
                                       stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_StoreElementIdGroup(const C_PuiSvDbNodeDataPoolListElementId & orc_ElementId,
                                      const C_PuiSvDbElementIdCrcGroup & orc_ElementIdGroup,
                                      stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadDashboardFromClipboard(C_PuiSvDashboard & orc_Data,
                                                QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                                     C_PuiSvReadDataConfiguration> & orc_Rails,
                                                QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                     C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups,
                                                const QString & orc_GenericTagName);
   static int32_t mh_LoadElementIdGroups(QMap<C_PuiSvDbNodeDataPoolListElementId,
                                              C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups,
                                         const QString & orc_GenericTagName,
                                         stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadElementIdGroup(C_PuiSvDbNodeDataPoolListElementId & orc_ElementId,
                                        C_PuiSvDbElementIdCrcGroup & orc_ElementIdGroup,
                                        stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static QMap<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvDbElementIdCrcGroup> mh_FillElementIdGroups(
      const C_PuiSvDashboard & orc_Data, const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                                    C_PuiSvReadDataConfiguration> & orc_Rails);
   static std::set<C_PuiSvDbNodeDataPoolListElementId> mh_GetAllIds(const C_PuiSvDashboard & orc_Data,
                                                                    const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                                                               C_PuiSvReadDataConfiguration> & orc_Rails);
   static int32_t mh_ValidateIds(const C_PuiSvDashboard & orc_Data,
                                 const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                            C_PuiSvReadDataConfiguration> & orc_Rails,
                                 const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                            C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
