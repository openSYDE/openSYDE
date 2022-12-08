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
#include "stwtypes.hpp"
#include "C_OscNode.hpp"
#include "C_PuiSvData.hpp"
#include "C_OscXmlParser.hpp"
#include "C_PuiSvDbWidgetBase.hpp"
#include "C_PuiSvLastKnownHalElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvHandlerFiler
{
public:
   C_PuiSvHandlerFiler(void);

   static int32_t h_LoadViews(std::vector<C_PuiSvData> & orc_Views,
                              const std::vector<stw::opensyde_core::C_OscNode> & orc_OscNodes,
                              stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser, const QDir * const opc_BasePath);
   static int32_t h_SaveViews(const std::vector<C_PuiSvData> & orc_Views,
                              stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser, const QDir * const opc_BasePath);
   static int32_t h_LoadReadRails(QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                       C_PuiSvReadDataConfiguration> & orc_Rails,
                                  stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveReadRails(const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                          C_PuiSvReadDataConfiguration> & orc_Rails,
                               stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadLastKnownHalcCrcs(std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                   C_PuiSvLastKnownHalElementId> & orc_Crcs,
                                          stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveLastKnownHalcCrcs(const std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                      C_PuiSvLastKnownHalElementId> & orc_Crcs,
                                       stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static QString h_GetViewFileName(const QString & orc_ViewName);
   static int32_t h_StringToPemFileStateDebugger(const QString & orc_String,
                                                 C_PuiSvNodeUpdate::E_StateDebugger & ore_State);
   static int32_t h_StringToPemFileStateSecurity(const QString & orc_String,
                                                 C_PuiSvNodeUpdate::E_StateSecurity & ore_State);
   static QString h_PemFileStateDebuggerToString(const C_PuiSvNodeUpdate::E_StateDebugger oe_State);
   static QString h_PemFileStateSecurityToString(const C_PuiSvNodeUpdate::E_StateSecurity oe_State);

private:
   static int32_t mh_LoadNodeActiveFlags(std::vector<uint8_t> & orc_NodeActiveFlags,
                                         stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadNodeUpdateInformation(std::vector<C_PuiSvNodeUpdate> & orc_NodeUpdateInformation,
                                               stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser,
                                               const std::vector<stw::opensyde_core::C_OscNode> & orc_OscNodes);
   static void mh_LoadNodeUpdateInformationPaths(std::vector<QString> & orc_Paths, const QString & orc_XmlTagBaseName,
                                                 stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadOneNodeUpdateInformation(C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                                  stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser,
                                                  const stw::opensyde_core::C_OscNode & orc_Node);
   static void mh_LoadNodeUpdateInformationParam(std::vector<C_PuiSvNodeUpdateParamInfo> & orc_Info,
                                                 stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_LoadNodeUpdateInformationSkipUpdateOfFiles(std::vector<bool> & orc_Flags,
                                                             stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadNodeUpdateInformationPem(C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                                  stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadNodeUpdateInformationPemStates(C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                                        stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadDashboards(std::vector<C_PuiSvDashboard> & orc_Dashboards,
                                    stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadViewFile(C_PuiSvData & orc_View, const QString & orc_FilePath,
                                  const std::vector<stw::opensyde_core::C_OscNode> & orc_OscNodes);
   static int32_t mh_LoadView(C_PuiSvData & orc_View, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser,
                              const std::vector<stw::opensyde_core::C_OscNode> & orc_OscNodes);
   static int32_t mh_LoadPc(C_PuiSvPc & orc_Pc, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_LoadDataElement(stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                                  const stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_StringToTransmissionMode(const QString & orc_String,
                                              C_PuiSvReadDataConfiguration::E_TransmissionMode & ore_Mode);
   static void mh_SaveNodeActiveFlags(const std::vector<uint8_t> & orc_NodeActiveFlags,
                                      stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveNodeUpdateInformation(const std::vector<C_PuiSvNodeUpdate> & orc_NodeUpdateInformation,
                                            stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveNodeUpdateInformationPaths(const std::vector<QString> & orc_Paths,
                                                 const QString & orc_XmlTagBaseName,
                                                 stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveNodeUpdateInformationParamInfo(const std::vector<C_PuiSvNodeUpdateParamInfo> & orc_Info,
                                                     stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveNodeUpdateInformationSkipUpdateOfFiles(const std::vector<bool> & orc_Flags,
                                                             stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveNodeUpdateInformationPem(const C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                               stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveDashboards(const std::vector<C_PuiSvDashboard> & orc_Dashboards,
                                 stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_SaveViewFile(const C_PuiSvData & orc_View, const QString & orc_FilePath);
   static void mh_SaveView(const C_PuiSvData & orc_View, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SavePc(const C_PuiSvPc & orc_Pc, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveDataElement(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                                  stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static QString mh_DeviceConfigModeToString(const C_PuiSvData::E_DeviceConfigurationMode oe_Mode);
   static QString mh_TransmissionModeToString(const C_PuiSvReadDataConfiguration::E_TransmissionMode oe_Mode);
   static C_PuiSvData::E_DeviceConfigurationMode mh_StringToDeviceConfigMode(const QString & orc_Input);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
