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
   static stw_types::sint32 h_LoadDashboard(C_PuiSvDashboard & orc_Dashboard,
                                            stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser,
                                            const bool oq_IgnoreMostErrorCases = false);
   static void h_SaveDashboard(const C_PuiSvDashboard & orc_Dashboard,
                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
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
   static stw_types::sint32 mh_LoadDashboards(std::vector<C_PuiSvDashboard> & orc_Dashboards,
                                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadViewFile(C_PuiSvData & orc_View, const QString & orc_FilePath,
                                            const std::vector<stw_opensyde_core::C_OSCNode> & orc_OSCNodes);
   static stw_types::sint32 mh_LoadView(C_PuiSvData & orc_View, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser,
                                        const std::vector<stw_opensyde_core::C_OSCNode> & orc_OSCNodes);
   static stw_types::sint32 mh_LoadPc(C_PuiSvPc & orc_Pc, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadCharts(std::vector<C_PuiSvDbChart> & orc_Widgets,
                                          stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadTabChart(C_PuiSvDbTabChart & orc_Widget,
                                            stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadLabels(std::vector<C_PuiSvDbLabel> & orc_Widgets,
                                          stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadParams(std::vector<C_PuiSvDbParam> & orc_Widgets,
                                          stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_LoadParamExpandedItems(std::vector<C_PuiSvDbExpandedTreeIndex> & orc_Items,
                                         stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_LoadParamColumnPositionIndices(std::vector<stw_types::sint32> & orc_Items,
                                                 stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadPieCharts(std::vector<C_PuiSvDbPieChart> & orc_Widgets,
                                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadSpinBoxes(std::vector<C_PuiSvDbSpinBox> & orc_Widgets,
                                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadTables(std::vector<C_PuiSvDbTable> & orc_Widgets,
                                          stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadSliders(std::vector<C_PuiSvDbSlider> & orc_Widgets,
                                           stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadProgressBars(std::vector<C_PuiSvDbProgressBar> & orc_Widgets,
                                                stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadToggles(std::vector<C_PuiSvDbToggle> & orc_Widgets,
                                           stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadWidgetBase(C_PuiSvDbWidgetBase & orc_Widget,
                                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_LoadUiIndex(C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadTabChartScreenRegion(std::vector<std::array<stw_types::float64,
                                                                               4> > & orc_ScreenRegion,
                                                        stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadParamDataSetIndices(std::vector<stw_types::sint32> & orc_Values,
                                                       stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadParamValues(std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> & orc_Values,
                                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadParamTables(std::vector<std::vector<stw_types::sint32> > & orc_Values,
                                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadParamColumns(std::vector<stw_types::sint32> & orc_Values,
                                                stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_LoadDataElement(stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                                  const stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_StringToWriteMode(const QString & orc_String,
                                                 C_PuiSvDbWidgetBase::E_WriteMode & ore_Mode);
   static stw_types::sint32 mh_StringToDisplayStyle(const QString & orc_String,
                                                    C_PuiSvDbWidgetBase::E_Style & ore_Style);
   static stw_types::sint32 mh_StringToTabType(const QString & orc_String, C_PuiSvDashboard::E_TabType & ore_TabType);
   static stw_types::sint32 mh_StringToToggleType(const QString & orc_String, C_PuiSvDbToggle::E_Type & ore_Type);
   static stw_types::sint32 mh_StringToSpinBoxType(const QString & orc_String, C_PuiSvDbSpinBox::E_Type & ore_Type);
   static stw_types::sint32 mh_StringToLabelType(const QString & orc_String, C_PuiSvDbLabel::E_Type & ore_Type);
   static stw_types::sint32 mh_StringToSliderType(const QString & orc_String, C_PuiSvDbSlider::E_Type & ore_Type);
   static stw_types::sint32 mh_StringToProgressBarType(const QString & orc_String,
                                                       C_PuiSvDbProgressBar::E_Type & ore_Type);
   static stw_types::sint32 mh_StringToProgressBarAlignmentType(const QString & orc_String,
                                                                C_PuiSvDbProgressBar::E_Alignment & ore_Alignment);
   static stw_types::sint32 mh_StringToTransmissionMode(const QString & orc_String,
                                                        C_PuiSvReadDataConfiguration::E_TransmissionMode & ore_Mode);
   static stw_types::sint32 mh_StringToSourceType(const QString & orc_String,
                                                  C_PuiSvDbNodeDataPoolListElementId::E_Type & ore_Type);
   static stw_types::sint32 mh_StringToChartSettingZoomMode(const QString & orc_String,
                                                            C_PuiSvDbChart::E_SettingZoomMode & ore_ZoomMode);
   static stw_types::sint32 mh_StringToTabChartSettingZoomMode(const QString & orc_String,
                                                               C_PuiSvDbTabChart::E_SettingZoomMode & ore_ZoomMode);
   static stw_types::sint32 mh_StringToTabChartSettingYAxisMode(const QString & orc_String,
                                                                C_PuiSvDbTabChart::E_SettingYAxisMode & ore_YAxisMode);
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
   static void mh_SaveDashboards(const std::vector<C_PuiSvDashboard> & orc_Dashboards,
                                 stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_SaveViewFile(const C_PuiSvData & orc_View, const QString & orc_FilePath);
   static void mh_SaveView(const C_PuiSvData & orc_View, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SavePc(const C_PuiSvPc & orc_Pc, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveCharts(const std::vector<C_PuiSvDbChart> & orc_Widgets,
                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveTabChart(const C_PuiSvDbTabChart & orc_Widget,
                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveLabels(const std::vector<C_PuiSvDbLabel> & orc_Widgets,
                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveParams(const std::vector<C_PuiSvDbParam> & orc_Widgets,
                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveParamExpandedItems(const std::vector<C_PuiSvDbExpandedTreeIndex> & orc_Items,
                                         stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveParamColumnPositionIndices(const std::vector<stw_types::sint32> & orc_Items,
                                                 stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SavePieCharts(const std::vector<C_PuiSvDbPieChart> & orc_Widgets,
                                stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveSpinBoxes(const std::vector<C_PuiSvDbSpinBox> & orc_Widgets,
                                stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveTables(const std::vector<C_PuiSvDbTable> & orc_Widgets,
                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveSliders(const std::vector<C_PuiSvDbSlider> & orc_Widgets,
                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveProgressBars(const std::vector<C_PuiSvDbProgressBar> & orc_Widgets,
                                   stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveToggles(const std::vector<C_PuiSvDbToggle> & orc_Widgets,
                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveWidgetBase(const C_PuiSvDbWidgetBase & orc_Widget,
                                 stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveUiIndex(const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveDataElement(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                                  stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveTabChartScreenRegion(const std::vector<std::array<stw_types::float64, 4> > & orc_ScreenRegion,
                                           stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static QString mh_DeviceConfigModeToString(const C_PuiSvData::E_DeviceConfigurationMode oe_Mode);
   static QString mh_WriteModeToString(const C_PuiSvDbWidgetBase::E_WriteMode oe_Mode);
   static QString mh_DisplayStyleToString(const C_PuiSvDbWidgetBase::E_Style oe_Style);
   static QString mh_ToggleTypeToString(const C_PuiSvDbToggle::E_Type oe_Type);
   static QString mh_TabTypeToString(const C_PuiSvDashboard::E_TabType oe_Type);
   static QString mh_SpinBoxTypeToString(const C_PuiSvDbSpinBox::E_Type oe_Type);
   static QString mh_LabelTypeToString(const C_PuiSvDbLabel::E_Type oe_Type);
   static QString mh_SliderTypeToString(const C_PuiSvDbSlider::E_Type oe_Type);
   static QString mh_ProgressBarTypeToString(const C_PuiSvDbProgressBar::E_Type oe_Type);
   static QString mh_ProgressBarAlignmentTypeToString(const C_PuiSvDbProgressBar::E_Alignment oe_Alignment);
   static QString mh_TransmissionModeToString(const C_PuiSvReadDataConfiguration::E_TransmissionMode oe_Mode);
   static QString mh_SourceTypeToString(const C_PuiSvDbNodeDataPoolListElementId::E_Type oe_Type);
   static C_PuiSvData::E_DeviceConfigurationMode mh_StringToDeviceConfigMode(const QString & orc_Input);
   static QString mh_ChartZoomModeToString(const C_PuiSvDbChart::E_SettingZoomMode oe_ZoomMode);
   static QString mh_TabChartZoomModeToString(const C_PuiSvDbTabChart::E_SettingZoomMode oe_ZoomMode);
   static QString mh_TabChartYAxisModeToString(const C_PuiSvDbTabChart::E_SettingYAxisMode oe_YAxisMode);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
