//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view filer (V2 format) (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVHANDLERFILER_H
#define C_PUISVHANDLERFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <QDir>
#include "stwtypes.h"
#include "C_OSCNode.h"
#include "C_PuiSvData.h"
#include "C_OSCXMLParser.h"
#include "C_PuiSvDbWidgetBase.h"
#include "C_PuiSvLastKnownHalElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvHandlerFiler
{
public:
   C_PuiSvHandlerFiler(void);

   static stw_types::sint32 h_LoadViews(std::vector<C_PuiSvData> & orc_Views,
                                        const std::vector<stw_opensyde_core::C_OSCNode> & orc_OSCNodes,
                                        stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser,
                                        const QDir * const opc_BasePath);
   static stw_types::sint32 h_SaveViews(const std::vector<C_PuiSvData> & orc_Views,
                                        stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser,
                                        const QDir * const opc_BasePath);
   static stw_types::sint32 h_LoadReadRails(QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                 C_PuiSvReadDataConfiguration> & orc_Rails,
                                            stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveReadRails(const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                          C_PuiSvReadDataConfiguration> & orc_Rails,
                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadLastKnownHalcCrcs(std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                             C_PuiSvLastKnownHalElementId> & orc_Crcs,
                                                    stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveLastKnownHalcCrcs(const std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                      C_PuiSvLastKnownHalElementId> & orc_Crcs,
                                       stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static QString h_GetViewFileName(const QString & orc_ViewName);
   static stw_types::sint32 h_StringToPemFileStateDebugger(const QString & orc_String,
                                                           C_PuiSvNodeUpdate::E_StateDebugger & ore_State);
   static stw_types::sint32 h_StringToPemFileStateSecurity(const QString & orc_String,
                                                           C_PuiSvNodeUpdate::E_StateSecurity & ore_State);
   static QString h_PemFileStateDebuggerToString(const C_PuiSvNodeUpdate::E_StateDebugger oe_State);
   static QString h_PemFileStateSecurityToString(const C_PuiSvNodeUpdate::E_StateSecurity oe_State);

private:
   static stw_types::sint32 mh_LoadNodeActiveFlags(std::vector<stw_types::uint8> & orc_NodeActiveFlags,
                                                   stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadNodeUpdateInformation(std::vector<C_PuiSvNodeUpdate> & orc_NodeUpdateInformation,
                                                         stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser,
                                                         const std::vector<stw_opensyde_core::C_OSCNode> & orc_OSCNodes);
   static void mh_LoadNodeUpdateInformationPaths(std::vector<QString> & orc_Paths, const QString & orc_XMLTagBaseName,
                                                 stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadOneNodeUpdateInformation(C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                                            stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser,
                                                            const stw_opensyde_core::C_OSCNode & orc_Node);
   static void mh_LoadNodeUpdateInformationParam(std::vector<C_PuiSvNodeUpdateParamInfo> & orc_Info,
                                                 stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_LoadNodeUpdateInformationSkipUpdateOfFiles(std::vector<bool> & orc_Flags,
                                                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadNodeUpdateInformationPem(C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                                            stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadNodeUpdateInformationPemStates(C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                                                  stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadDashboards(std::vector<C_PuiSvDashboard> & orc_Dashboards,
                                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadViewFile(C_PuiSvData & orc_View, const QString & orc_FilePath,
                                            const std::vector<stw_opensyde_core::C_OSCNode> & orc_OSCNodes);
   static stw_types::sint32 mh_LoadView(C_PuiSvData & orc_View, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser,
                                        const std::vector<stw_opensyde_core::C_OSCNode> & orc_OSCNodes);
   static stw_types::sint32 mh_LoadPc(C_PuiSvPc & orc_Pc, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_LoadDataElement(stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                                  const stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_StringToTransmissionMode(const QString & orc_String,
                                                        C_PuiSvReadDataConfiguration::E_TransmissionMode & ore_Mode);
   static void mh_SaveNodeActiveFlags(const std::vector<stw_types::uint8> & orc_NodeActiveFlags,
                                      stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveNodeUpdateInformation(const std::vector<C_PuiSvNodeUpdate> & orc_NodeUpdateInformation,
                                            stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveNodeUpdateInformationPaths(const std::vector<QString> & orc_Paths,
                                                 const QString & orc_XMLTagBaseName,
                                                 stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveNodeUpdateInformationParamInfo(const std::vector<C_PuiSvNodeUpdateParamInfo> & orc_Info,
                                                     stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveNodeUpdateInformationSkipUpdateOfFiles(const std::vector<bool> & orc_Flags,
                                                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveNodeUpdateInformationPem(const C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveDashboards(const std::vector<C_PuiSvDashboard> & orc_Dashboards,
                                 stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_SaveViewFile(const C_PuiSvData & orc_View, const QString & orc_FilePath);
   static void mh_SaveView(const C_PuiSvData & orc_View, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SavePc(const C_PuiSvPc & orc_Pc, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveDataElement(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                                  stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static QString mh_DeviceConfigModeToString(const C_PuiSvData::E_DeviceConfigurationMode oe_Mode);
   static QString mh_TransmissionModeToString(const C_PuiSvReadDataConfiguration::E_TransmissionMode oe_Mode);
   static C_PuiSvData::E_DeviceConfigurationMode mh_StringToDeviceConfigMode(const QString & orc_Input);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
