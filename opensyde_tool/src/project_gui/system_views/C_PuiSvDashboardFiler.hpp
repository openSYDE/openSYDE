//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer helper for dashboard components
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDASHBOARDFILER_H
#define C_PUISVDASHBOARDFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OscXmlParser.hpp"
#include "C_PuiSvDashboard.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDashboardFiler
{
public:
   C_PuiSvDashboardFiler();

   static int32_t h_LoadDashboard(C_PuiSvDashboard & orc_Dashboard,
                                  stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser,
                                  const bool oq_IgnoreMostErrorCases = false);
   static void h_SaveDashboard(const C_PuiSvDashboard & orc_Dashboard,
                               stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_LoadUiIndex(C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                             stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveUiIndex(const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                             stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);

private:
   static int32_t mh_LoadCharts(std::vector<C_PuiSvDbChart> & orc_Widgets,
                                stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadTabChart(C_PuiSvDbTabChart & orc_Widget,
                                  stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadLabels(std::vector<C_PuiSvDbLabel> & orc_Widgets,
                                stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadParams(std::vector<C_PuiSvDbParam> & orc_Widgets,
                                stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_LoadParamExpandedItems(std::vector<C_PuiSvDbExpandedTreeIndex> & orc_Items,
                                         stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_LoadParamColumnPositionIndices(std::vector<int32_t> & orc_Items,
                                                 stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadPieCharts(std::vector<C_PuiSvDbPieChart> & orc_Widgets,
                                   stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadSpinBoxes(std::vector<C_PuiSvDbSpinBox> & orc_Widgets,
                                   stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadTables(std::vector<C_PuiSvDbTable> & orc_Widgets,
                                stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadSliders(std::vector<C_PuiSvDbSlider> & orc_Widgets,
                                 stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadProgressBars(std::vector<C_PuiSvDbProgressBar> & orc_Widgets,
                                      stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadToggles(std::vector<C_PuiSvDbToggle> & orc_Widgets,
                                 stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadWidgetBase(C_PuiSvDbWidgetBase & orc_Widget,
                                    stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadDataElementConfig(C_PuiSvDbNodeDataElementConfig & orc_Config,
                                           stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadDataScalingConfig(C_PuiSvDbDataElementScaling & orc_Config,
                                           stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadDataFormatterConfig(C_PuiSvDbDataElementDisplayFormatter & orc_Config,
                                             stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadParamDataSetIndices(std::vector<int32_t> & orc_Values,
                                             stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadParamValues(std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> & orc_Values,
                                     stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadParamTables(std::vector<std::vector<int32_t> > & orc_Values,
                                     stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadParamColumns(std::vector<int32_t> & orc_Values,
                                      stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadTabChartScreenRegion(std::vector<std::array<float64_t,
                                                                     4> > & orc_ScreenRegion,
                                              stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);

   static void mh_SaveCharts(const std::vector<C_PuiSvDbChart> & orc_Widgets,
                             stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveTabChart(const C_PuiSvDbTabChart & orc_Widget,
                               stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveLabels(const std::vector<C_PuiSvDbLabel> & orc_Widgets,
                             stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveParams(const std::vector<C_PuiSvDbParam> & orc_Widgets,
                             stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveParamExpandedItems(const std::vector<C_PuiSvDbExpandedTreeIndex> & orc_Items,
                                         stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveParamColumnPositionIndices(const std::vector<int32_t> & orc_Items,
                                                 stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SavePieCharts(const std::vector<C_PuiSvDbPieChart> & orc_Widgets,
                                stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveSpinBoxes(const std::vector<C_PuiSvDbSpinBox> & orc_Widgets,
                                stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveTables(const std::vector<C_PuiSvDbTable> & orc_Widgets,
                             stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveSliders(const std::vector<C_PuiSvDbSlider> & orc_Widgets,
                              stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveProgressBars(const std::vector<C_PuiSvDbProgressBar> & orc_Widgets,
                                   stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveToggles(const std::vector<C_PuiSvDbToggle> & orc_Widgets,
                              stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveWidgetBase(const C_PuiSvDbWidgetBase & orc_Widget,
                                 stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveDataElementConfig(const C_PuiSvDbNodeDataElementConfig & orc_Config,
                                        stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveDataScalingConfig(const C_PuiSvDbDataElementScaling & orc_Config,
                                        stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveDataFormatterConfig(const C_PuiSvDbDataElementDisplayFormatter & orc_Config,
                                          stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveTabChartScreenRegion(const std::vector<std::array<float64_t, 4> > & orc_ScreenRegion,
                                           stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);

   static QString mh_TabTypeToString(const C_PuiSvDashboard::E_TabType oe_Type);
   static QString mh_DisplayStyleToString(const C_PuiSvDbWidgetBase::E_Style oe_Style);
   static QString mh_ToggleTypeToString(const C_PuiSvDbToggle::E_Type oe_Type);
   static QString mh_SpinBoxTypeToString(const C_PuiSvDbSpinBox::E_Type oe_Type);
   static QString mh_LabelTypeToString(const C_PuiSvDbLabel::E_Type oe_Type);
   static QString mh_SliderTypeToString(const C_PuiSvDbSlider::E_Type oe_Type);
   static QString mh_ProgressBarTypeToString(const C_PuiSvDbProgressBar::E_Type oe_Type);
   static QString mh_ProgressBarAlignmentTypeToString(const C_PuiSvDbProgressBar::E_Alignment oe_Alignment);
   static QString mh_TabChartZoomModeToString(const C_PuiSvDbTabChart::E_SettingZoomMode oe_ZoomMode);
   static QString mh_TabChartVerticalAxisModeToString(
      const C_PuiSvDbTabChart::E_SettingVerticalAxisMode oe_VerticalAxisMode);
   static QString mh_WriteModeToString(const C_PuiSvDbWidgetBase::E_WriteMode oe_Mode);
   static QString mh_SourceTypeToString(const C_PuiSvDbNodeDataPoolListElementId::E_Type oe_Type);

   static int32_t mh_StringToDisplayStyle(const QString & orc_String, C_PuiSvDbWidgetBase::E_Style & ore_Style);
   static int32_t mh_StringToTabType(const QString & orc_String, C_PuiSvDashboard::E_TabType & ore_TabType);
   static int32_t mh_StringToToggleType(const QString & orc_String, C_PuiSvDbToggle::E_Type & ore_Type);
   static int32_t mh_StringToSpinBoxType(const QString & orc_String, C_PuiSvDbSpinBox::E_Type & ore_Type);
   static int32_t mh_StringToLabelType(const QString & orc_String, C_PuiSvDbLabel::E_Type & ore_Type);
   static int32_t mh_StringToSliderType(const QString & orc_String, C_PuiSvDbSlider::E_Type & ore_Type);
   static int32_t mh_StringToProgressBarType(const QString & orc_String, C_PuiSvDbProgressBar::E_Type & ore_Type);
   static int32_t mh_StringToProgressBarAlignmentType(const QString & orc_String,
                                                      C_PuiSvDbProgressBar::E_Alignment & ore_Alignment);
   static int32_t mh_StringToChartSettingZoomMode(const QString & orc_String,
                                                  C_PuiSvDbChart::E_SettingZoomMode & ore_ZoomMode);
   static int32_t mh_StringToTabChartSettingZoomMode(const QString & orc_String,
                                                     C_PuiSvDbTabChart::E_SettingZoomMode & ore_ZoomMode);
   static int32_t mh_StringToTabChartSettingVerticalAxisMode(const QString & orc_String,
                                                             C_PuiSvDbTabChart::E_SettingVerticalAxisMode & ore_VerticalAxisMode);
   static int32_t mh_StringToWriteMode(const QString & orc_String, C_PuiSvDbWidgetBase::E_WriteMode & ore_Mode);
   static int32_t mh_StringToSourceType(const QString & orc_String,
                                        C_PuiSvDbNodeDataPoolListElementId::E_Type & ore_Type);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
