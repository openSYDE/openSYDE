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
#include "C_OSCXMLParser.h"
#include "C_PuiSvDashboard.h"
#include "C_UtiClipBoardHelper.h"
#include "C_PuiSvDbElementIdCRCGroup.h"
#include "C_PuiSvReadDataConfiguration.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvClipBoardHelper :
   public C_UtiClipBoardHelper
{
public:
   static void h_StoreDashboardToClipboard(const C_PuiSvDashboard & orc_Data,
                                           const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                      C_PuiSvReadDataConfiguration> & orc_Rails,
                                           const QString & orc_GenericTagName);
   static stw_types::sint32 h_LoadDashboardFromClipboard(C_PuiSvDashboard & orc_Data,
                                                         QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                              C_PuiSvReadDataConfiguration> & orc_Rails,
                                                         QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                              C_PuiSvDbElementIdCRCGroup> & orc_ElementIDGroups,
                                                         const QString & orc_GenericTagName);

private:
   C_SyvClipBoardHelper(void);
   static void mh_StoreDashboardToClipboard(const C_PuiSvDashboard & orc_Data,
                                            const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                       C_PuiSvReadDataConfiguration> & orc_Rails,
                                            const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                       C_PuiSvDbElementIdCRCGroup> & orc_ElementIDGroups,
                                            const QString & orc_GenericTagName);
   static void mh_StoreElementIDGroups(const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                  C_PuiSvDbElementIdCRCGroup> & orc_ElementIDGroups,
                                       const QString & orc_GenericTagName,
                                       stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_StoreElementIDGroup(const C_PuiSvDbNodeDataPoolListElementId & orc_ElementID,
                                      const C_PuiSvDbElementIdCRCGroup & orc_ElementIDGroup,
                                      stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadDashboardFromClipboard(C_PuiSvDashboard & orc_Data,
                                                          QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                               C_PuiSvReadDataConfiguration> & orc_Rails,
                                                          QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                               C_PuiSvDbElementIdCRCGroup> & orc_ElementIDGroups,
                                                          const QString & orc_GenericTagName);
   static stw_types::sint32 mh_LoadElementIDGroups(QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                        C_PuiSvDbElementIdCRCGroup> & orc_ElementIDGroups,
                                                   const QString & orc_GenericTagName,
                                                   stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadElementIDGroup(C_PuiSvDbNodeDataPoolListElementId & orc_ElementID,
                                                  C_PuiSvDbElementIdCRCGroup & orc_ElementIDGroup,
                                                  stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static QMap<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvDbElementIdCRCGroup> mh_FillElementIDGroups(
      const C_PuiSvDashboard & orc_Data, const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                    C_PuiSvReadDataConfiguration> & orc_Rails);
   static std::set<C_PuiSvDbNodeDataPoolListElementId> mh_GetAllIds(const C_PuiSvDashboard & orc_Data,
                                                                    const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                                               C_PuiSvReadDataConfiguration> & orc_Rails);
   static stw_types::sint32 mh_ValidateIds(const C_PuiSvDashboard & orc_Data,
                                           const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                      C_PuiSvReadDataConfiguration> & orc_Rails,
                                           const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                      C_PuiSvDbElementIdCRCGroup> & orc_ElementIDGroups);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
