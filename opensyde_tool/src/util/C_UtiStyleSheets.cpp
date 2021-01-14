//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for stylesheets (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QTextDocument>
#include <QFile>

#include "constants.h"

#include "C_UtiStyleSheets.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
// Configuration for dynamic transparency color configuration of elements
const std::vector<QString> C_UtiStyleSheets::mhc_ScrollAreaElements(
{
   "stw_opensyde_gui--C_SdSearchTreeWidget",
   "stw_opensyde_gui_elements--C_OgeTedTable",
   "stw_opensyde_gui--C_SyvDaItPaListTreeWidget",
   "stw_opensyde_gui--C_SdNdeDbListWidget",
   "stw_opensyde_gui--C_SyvDaPeUpdateModeTreeWidget",
   "stw_opensyde_gui_elements--C_OgeTableWidgetComIF",
   "stw_opensyde_gui--C_SdNdeDpListsTreeWidget",
   "stw_opensyde_gui--C_SyvDaItPaArView",
   "stw_opensyde_gui--C_SyvDaItPaTableView",
   "stw_opensyde_gui--C_SdBueSignalTableView",
   "stw_opensyde_gui--C_SdBueMessageTableView",
   "stw_opensyde_gui--C_SdNdeHalcOvTableView",
   "stw_opensyde_gui--C_SyvDaPeUpdateModeTableView",
   "stw_opensyde_gui--C_SdNdeDpListTableView",
   "stw_opensyde_gui--C_SdNdeHalcConfigImportView",
   "stw_opensyde_gui--C_SdCodeGenerationView",
   "stw_opensyde_gui--C_TblTreDataElementView",
   "stw_opensyde_gui--C_SyvDaItTaView",
   "stw_opensyde_gui--C_SyvDaItPaTreeView",
   "stw_opensyde_gui--C_SdNdeDpListDataSetView",
   "stw_opensyde_gui--C_SdNdeDpListArrayEditView",
   "stw_opensyde_gui--C_SdNdeSfoResetMessageTableView",
   "stw_opensyde_gui--C_SdBueMessageSelectorTreeWidget",
   "stw_opensyde_gui--C_SdNdeHalcChannelTreeView",
   "stw_opensyde_gui--C_SdNdeHalcConfigTreeView",
   "stw_opensyde_gui_elements--C_OgeCbxTableHalc QAbstractItemView QScrollBar",
   "stw_opensyde_gui_elements--C_OgeCbxMultiSelectTableHalc QAbstractItemView QScrollBar",
   "stw_opensyde_gui_elements--C_OgeTedPropertiesComment",
   "stw_opensyde_gui_elements--C_OgeTebMessageDetails",
   "stw_opensyde_gui_elements--C_OgeTebReport",
   "stw_opensyde_gui--C_SebGraphicsView",
   "stw_opensyde_gui--C_SyvUpUpdatePackageListWidget",
   "stw_opensyde_gui--C_SyvUpUpdatePackageListNodeWidget",
   "stw_opensyde_gui--C_SyvDaItChartDataSelectorWidget",
   "stw_opensyde_gui_elements--C_OgeCbxText QAbstractItemView",
   "stw_opensyde_gui_elements--C_OgeSmoothScrollArea",
   "stw_opensyde_gui--C_SdBueNodeSelectorCheckBoxListWidget QAbstractScrollArea",
   "stw_opensyde_gui_elements--C_OgeCbxFontProperties QAbstractItemView",
   "stw_opensyde_gui_elements--C_OgeDarkScrollArea",
   "stw_opensyde_gui_elements--C_OgeTransparentScrollArea",
   "stw_opensyde_gui--C_SyvDcExistingNodeList",
   "stw_opensyde_gui--C_SyvDcConnectedNodeList",
   "stw_opensyde_gui--C_SdBueMlvGraphicsView",
   "stw_opensyde_gui_elements--C_OgeDarkScrollArea",
   "stw_opensyde_gui_logic--C_PopFileTableView",
   "stw_opensyde_gui_elements--C_OgeSaNaviBar",
   "stw_opensyde_gui--C_SdNdeDpSelectorAddListWidget"
}
   );
const std::vector<C_UtiStyleSheets::C_PropertyValueColorConfig> C_UtiStyleSheets::mhc_ScrollAreaProperties(
{
   C_UtiStyleSheets::C_PropertyValueColorConfig("", "background-color", Qt::transparent)
}
   );
const std::vector<C_UtiStyleSheets::C_ElementColorConfig> C_UtiStyleSheets::mhc_ElementColorsWithTransparency(
{
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"OPENSYDE_BRIGHT\"]", mc_STYLE_GUIDE_COLOR_32),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"OPENSYDE_DARK\"]", mc_STYLE_GUIDE_COLOR_32),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"OPENSYDE_2_BRIGHT\"]", mc_STYLE_GUIDE_COLOR_3),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"OPENSYDE_2_DARK\"]", mc_STYLE_GUIDE_COLOR_0),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"FLAT_BRIGHT\"]", mc_STYLE_GUIDE_COLOR_1),
   C_UtiStyleSheets::C_ElementColorConfig("stw_opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"FLAT_DARK\"]",
                                          mc_STYLE_GUIDE_COLOR_0),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"SKEUOMORPH_BRIGHT\"]", mc_STYLE_GUIDE_COLOR_34),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"SKEUOMORPH_DARK\"]", mc_STYLE_GUIDE_COLOR_0),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_BRIGHT\"][Selected=\"true\"]",
      mc_STYLE_GUIDE_COLOR_4),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_DARK\"][Selected=\"true\"]",
      mc_STYLE_GUIDE_COLOR_0),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_BRIGHT\"][Selected=\"false\"]",
      mc_STYLE_GUIDE_COLOR_34),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_DARK\"][Selected=\"false\"]",
      mc_STYLE_GUIDE_COLOR_2),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_2_BRIGHT\"][Selected=\"true\"]",
      mc_STYLE_GUIDE_COLOR_3),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_2_DARK\"][Selected=\"true\"]",
      mc_STYLE_GUIDE_COLOR_0),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_2_BRIGHT\"][Selected=\"false\"]",
      mc_STYLE_GUIDE_COLOR_6),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_2_DARK\"][Selected=\"false\"]",
      mc_STYLE_GUIDE_COLOR_0),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"FLAT_BRIGHT\"][Selected=\"true\"]",
      mc_STYLE_GUIDE_COLOR_1),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"FLAT_DARK\"][Selected=\"true\"]",
      mc_STYLE_GUIDE_COLOR_0),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"FLAT_BRIGHT\"][Selected=\"false\"]",
      mc_STYLE_GUIDE_COLOR_34),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"FLAT_DARK\"][Selected=\"false\"]",
      mc_STYLE_GUIDE_COLOR_36),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"SKEUOMORPH_BRIGHT\"][Selected=\"true\"]",
      mc_STYLE_GUIDE_COLOR_0),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"SKEUOMORPH_DARK\"][Selected=\"true\"]",
      mc_STYLE_GUIDE_COLOR_0),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"SKEUOMORPH_BRIGHT\"][Selected=\"false\"]",
      mc_STYLE_GUIDE_COLOR_34),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw_opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"SKEUOMORPH_DARK\"][Selected=\"false\"]",
      mc_STYLE_GUIDE_COLOR_37)
}
   );

const std::vector<QString> C_UtiStyleSheets::mhc_ScrollBarElementsBright(
{
   "stw_opensyde_gui--C_SyvDaItPaArView QScrollBar",
   "stw_opensyde_gui--C_SyvDaItPaTableView QScrollBar",
   "stw_opensyde_gui--C_SdSearchTreeWidget QScrollBar",
   "stw_opensyde_gui_elements--C_OgeTedTable QScrollBar",
   "stw_opensyde_gui--C_SdBueSignalTableView QScrollBar",
   "stw_opensyde_gui--C_SdBueMessageTableView QScrollBar",
   "stw_opensyde_gui--C_SdNdeHalcOvTableView QScrollBar",
   "stw_opensyde_gui--C_SyvDaItPaListTreeWidget QScrollBar",
   "stw_opensyde_gui--C_SdNdeDbListWidget QScrollBar[C_SdNdeDbWidget=\"true\"]",
   "stw_opensyde_gui--C_SyvDaPeUpdateModeTableView QScrollBar",
   "stw_opensyde_gui--C_SdNdeDpListTableView QScrollBar",
   "stw_opensyde_gui--C_SdNdeHalcConfigImportView QScrollBar",
   "stw_opensyde_gui--C_SdCodeGenerationView QScrollBar",
   "stw_opensyde_gui--C_TblTreDataElementView QScrollBar",
   "stw_opensyde_gui--C_SyvDaPeUpdateModeTreeWidget QScrollBar",
   "stw_opensyde_gui_elements--C_OgeTableWidgetComIF QScrollBar",
   "stw_opensyde_gui--C_SdNdeDpListsTreeWidget QScrollBar",
   "stw_opensyde_gui--C_SdNdeDpListDataSetView QScrollBar",
   "stw_opensyde_gui--C_SdNdeDpListArrayEditView QScrollBar",
   "stw_opensyde_gui--C_SdNdeSfoResetMessageTableView QScrollBar",
   "stw_opensyde_gui--C_SdBueMessageSelectorTreeWidget QScrollBar",
   "stw_opensyde_gui--C_SdNdeHalcChannelTreeView QScrollBar",
   "stw_opensyde_gui--C_SdNdeHalcConfigTreeView QScrollBar",
   "stw_opensyde_gui_elements--C_OgeCbxTableHalc QAbstractItemView QScrollBar",
   "stw_opensyde_gui_elements--C_OgeCbxMultiSelectTableHalc QAbstractItemView QScrollBar",
   "stw_opensyde_gui--C_SyvDaItPaTreeView QScrollBar[DarkMode=\"false\"]",
   "stw_opensyde_gui--C_SyvDaItTaView QScrollBar[DarkMode=\"false\"]",
   "stw_opensyde_gui_elements--C_OgeTedPropertiesComment QScrollBar",
   "stw_opensyde_gui_elements--C_OgeTebMessageDetails QScrollBar",
   "stw_opensyde_gui_elements--C_OgeTebReport QScrollBar",
   "stw_opensyde_gui--C_SebGraphicsView QScrollBar[DarkMode=\"false\"]",
   "stw_opensyde_gui--C_SdBueNodeSelectorCheckBoxListWidget QScrollBar",
   "stw_opensyde_gui--C_SyvUpUpdatePackageListNodeWidget QScrollBar",
   "stw_opensyde_gui--C_SyvDaItChartDataSelectorWidget QScrollBar[Style=\"OPENSYDE_BRIGHT\"]",
   "stw_opensyde_gui--C_SyvDaItChartDataSelectorWidget QScrollBar[Style=\"FLAT_BRIGHT\"]",
   "stw_opensyde_gui--C_SyvDaItChartDataSelectorWidget QScrollBar[Style=\"SKEUOMORPH_BRIGHT\"]",
   "stw_opensyde_gui--C_SyvDaItChartDataSelectorWidget QScrollBar[Style=\"OPENSYDE_2_BRIGHT\"]",
   "stw_opensyde_gui_elements--C_OgeCbxText QAbstractItemView QScrollBar",
   "stw_opensyde_gui_elements--C_OgeSmoothScrollArea[DarkMode=\"false\"] QScrollBar",
   "stw_opensyde_gui_elements--C_OgeCbxFontProperties QAbstractItemView QScrollBar",
   "stw_opensyde_gui_elements--C_OgeDarkScrollArea QScrollBar[DarkMode=\"false\"]",
   "stw_opensyde_gui_elements--C_OgeTransparentScrollArea QScrollBar",
   "stw_opensyde_gui--C_SyvDcExistingNodeList QScrollBar",
   "stw_opensyde_gui--C_SyvDcConnectedNodeList QScrollBar",
   "stw_opensyde_gui--C_SdNdeDpSelectorAddListWidget QScrollBar"
}
   );
const std::vector<C_UtiStyleSheets::C_PropertyValueColorConfig> C_UtiStyleSheets::mhc_ScrollBarPropertiesBright(
{
   C_UtiStyleSheets::C_PropertyValueColorConfig(":vertical", "background-color", Qt::transparent),
   C_UtiStyleSheets::C_PropertyValueColorConfig(":horizontal", "background-color", Qt::transparent),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "background-color", mc_STYLE_GUIDE_COLOR_8),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "border-color", mc_STYLE_GUIDE_COLOR_8),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle:hover:!pressed", "background-color", mc_STYLE_GUIDE_COLOR_9),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle:hover:!pressed", "border-color", mc_STYLE_GUIDE_COLOR_9),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle", "background-color", mc_STYLE_GUIDE_COLOR_10),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle", "border-color", mc_STYLE_GUIDE_COLOR_10),
   C_UtiStyleSheets::C_PropertyValueColorConfig("", "background-color", mc_STYLE_GUIDE_COLOR_11),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::add-page", "background-color", mc_STYLE_GUIDE_COLOR_11),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::sub-page", "background-color", mc_STYLE_GUIDE_COLOR_11),
   C_UtiStyleSheets::C_PropertyValueColorConfig("", "border-color", mc_STYLE_GUIDE_COLOR_11),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::add-page", "border-color", mc_STYLE_GUIDE_COLOR_11),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::sub-page", "border-color", mc_STYLE_GUIDE_COLOR_11)
}
   );
const std::vector<QString> C_UtiStyleSheets::mhc_ScrollBarElementsDark(
{
   "stw_opensyde_gui--C_SyvDaItPaTreeView QScrollBar[DarkMode=\"true\"]",
   "stw_opensyde_gui--C_SyvDaItTaView QScrollBar[DarkMode=\"true\"]",
   "stw_opensyde_gui--C_SebGraphicsView QScrollBar[DarkMode=\"true\"]",
   "stw_opensyde_gui_elements--C_OgeSmoothScrollArea QScrollBar[DarkMode=\"true\"]",
   "stw_opensyde_gui_elements--C_OgeDarkScrollArea QScrollBar[DarkMode=\"true\"]",
   "stw_opensyde_gui--C_SyvDaItChartDataSelectorWidget QScrollBar[Style=\"OPENSYDE_DARK\"]",
   "stw_opensyde_gui--C_SyvDaItChartDataSelectorWidget QScrollBar[Style=\"FLAT_DARK\"]",
   "stw_opensyde_gui--C_SyvDaItChartDataSelectorWidget QScrollBar[Style=\"SKEUOMORPH_DARK\"]",
   "stw_opensyde_gui--C_SyvDaItChartDataSelectorWidget QScrollBar[Style=\"OPENSYDE_2_DARK\"]"
}
   );
const std::vector<C_UtiStyleSheets::C_PropertyValueColorConfig> C_UtiStyleSheets::mhc_ScrollBarPropertiesDark(
{
   C_UtiStyleSheets::C_PropertyValueColorConfig(":vertical", "background-color", Qt::transparent),
   C_UtiStyleSheets::C_PropertyValueColorConfig(":horizontal", "background-color", Qt::transparent),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "background-color", mc_STYLE_GUIDE_COLOR_10),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "border-color", mc_STYLE_GUIDE_COLOR_10),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle:hover:!pressed", "background-color", mc_STYLE_GUIDE_COLOR_35),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle:hover:!pressed", "border-color", mc_STYLE_GUIDE_COLOR_35),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle", "background-color", mc_STYLE_GUIDE_COLOR_39),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle", "border-color", mc_STYLE_GUIDE_COLOR_39),
   C_UtiStyleSheets::C_PropertyValueColorConfig("", "background-color", mc_STYLE_GUIDE_COLOR_32),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::add-page", "background-color", mc_STYLE_GUIDE_COLOR_32),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::sub-page", "background-color", mc_STYLE_GUIDE_COLOR_32),
   C_UtiStyleSheets::C_PropertyValueColorConfig("", "border-color", mc_STYLE_GUIDE_COLOR_32),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::add-page", "border-color", mc_STYLE_GUIDE_COLOR_32),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::sub-page", "border-color", mc_STYLE_GUIDE_COLOR_32)
}
   );
const std::vector<QString> C_UtiStyleSheets::mhc_ScrollBarElementsMainNavi(
{
   "stw_opensyde_gui_logic--C_PopFileTableView QScrollBar",
   "stw_opensyde_gui_elements--C_OgeSaNaviBar QScrollBar"
}
   );
const std::vector<C_UtiStyleSheets::C_PropertyValueColorConfig> C_UtiStyleSheets::mhc_ScrollBarPropertiesMainNavi(
{
   C_UtiStyleSheets::C_PropertyValueColorConfig(":vertical", "background-color", Qt::transparent),
   C_UtiStyleSheets::C_PropertyValueColorConfig(":horizontal", "background-color", Qt::transparent),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "background-color", mc_STYLE_GUIDE_COLOR_12),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "border-color", mc_STYLE_GUIDE_COLOR_12),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle:hover:!pressed", "background-color", mc_STYLE_GUIDE_COLOR_11),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle:hover:!pressed", "border-color", mc_STYLE_GUIDE_COLOR_11),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle", "background-color", mc_STYLE_GUIDE_COLOR_10),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::handle", "border-color", mc_STYLE_GUIDE_COLOR_10),
   C_UtiStyleSheets::C_PropertyValueColorConfig("", "background-color", mc_STYLE_GUIDE_COLOR_9),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::add-page", "background-color", mc_STYLE_GUIDE_COLOR_9),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::sub-page", "background-color", mc_STYLE_GUIDE_COLOR_9),
   C_UtiStyleSheets::C_PropertyValueColorConfig("", "border-color", mc_STYLE_GUIDE_COLOR_9),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::add-page", "border-color", mc_STYLE_GUIDE_COLOR_9),
   C_UtiStyleSheets::C_PropertyValueColorConfig("::sub-page", "border-color", mc_STYLE_GUIDE_COLOR_9)
}
   );

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Combine all available stylesheets

   \return
   Combined string of all stylesheets
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UtiStyleSheets::h_GetStylesheet(void)
{
   QString c_Stylesheet;

   mh_AppendScrollBarStyleSheet(C_UtiStyleSheets::mhc_ScrollAreaElements, C_UtiStyleSheets::mhc_ScrollAreaProperties,
                                c_Stylesheet);

   mh_AppendStylesheet("://styles/Charts.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/CheckBox.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/Color.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/ComboBox.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/Font.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/Frame.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/GroupBox.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/Label.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/LineEdit.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/ListWidget.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/Menu.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/PlainTextEdit.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/ProgressBar.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/PushButton.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/RadioButton.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/ScrollArea.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/ScrollBar.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/Slider.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/SpinBox.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/Table.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/TabWidget.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/TextEdit.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/TreeWidget.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/Widget.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/TextBrowser.qss", c_Stylesheet);

   // Append the dynamic created stylesheet
   mh_AppendDynamicStylesheet(c_Stylesheet);

   return c_Stylesheet;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the background color of a styleshett

   Necessary format: "background-color: #0070c0;"

   \param[in]  orc_Style   Stylesheet string to analyse

   \return        Background color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_UtiStyleSheets::h_GetStyleSheetColor(const QString & orc_Style)
{
   const QString c_Category = "background-color: ";
   const sintn sn_Pos = orc_Style.indexOf(c_Category);
   QColor c_Color;

   c_Color.setNamedColor(orc_Style.mid(sn_Pos + c_Category.length(), 9));

   return c_Color;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the background color of a styleshett

   \param[in,out]  orc_Style  Stylesheet string to adapt
   \param[in]      orc_Color  New background color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UtiStyleSheets::h_SetStyleSheetBackgroundColor(QString & orc_Style, const QColor & orc_Color)
{
   mh_SetStyleSheetColor("background-color", orc_Style, orc_Color);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_UtiStyleSheets::C_UtiStyleSheets(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append stylesheet of file

   \param[in]      orc_File         QSS file
   \param[in,out]  orc_Stylesheet   Stylesheet
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UtiStyleSheets::mh_AppendStylesheet(const QString & orc_File, QString & orc_Stylesheet)
{
   QFile c_File(orc_File);

   c_File.open(QFile::ReadOnly);
   orc_Stylesheet += QLatin1String(c_File.readAll());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append dynamic created stylesheet

   \param[in,out]  orc_Stylesheet   Stylesheet
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UtiStyleSheets::mh_AppendDynamicStylesheet(QString & orc_Stylesheet)
{
   mh_AppendTransparentColorSteps(orc_Stylesheet);
   mh_AppendScrollBarStyleSheets(orc_Stylesheet);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append dynamic created color with transparent steps from 255 to 127

   The stylesheet can be used with the property Transparency with the number of the
   transparency value itself.

   \param[in,out]  orc_Stylesheet   Stylesheet
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UtiStyleSheets::mh_AppendTransparentColorSteps(QString & orc_Stylesheet)
{
   uint32 u32_Counter;
   const QString c_TransparencyPropertyStart = "[Transparency=\"";
   const QString c_TransparencyPropertyEnd = "\"] {\n";
   const QString c_End = "\n}\n";

   for (u32_Counter = 0; u32_Counter < mhc_ElementColorsWithTransparency.size(); ++u32_Counter)
   {
      // Prepare color for start
      QColor c_Color = mhc_ElementColorsWithTransparency[u32_Counter].c_Color;
      sintn sn_AlphaValue = msn_TRANSPARENCY_START;

      do
      {
         QString c_Line;
         QString c_ColorString;

         c_Color.setAlpha(sn_AlphaValue);

         // Build the stylesheet string
         c_Line = mhc_ElementColorsWithTransparency[u32_Counter].c_ElementName;
         c_Line += c_TransparencyPropertyStart;
         // Add the property value -> transparency value
         c_Line += QString::number(sn_AlphaValue);
         c_Line += c_TransparencyPropertyEnd;
         // Add the color itself
         mh_SetStyleSheetColor("color", c_ColorString, c_Color);
         c_Line += c_ColorString;
         // Finish the line
         c_Line += c_End;

         orc_Stylesheet += c_Line;

         // Prepare next transparent step
         ++sn_AlphaValue;
      }
      while (sn_AlphaValue <= msn_TRANSPARENCY_END);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply stylesheets for scroll bars

   \param[in,out]  orc_Stylesheet   Dynamic stylesheet document
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UtiStyleSheets::mh_AppendScrollBarStyleSheets(QString & orc_Stylesheet)
{
   mh_AppendScrollBarStyleSheet(C_UtiStyleSheets::mhc_ScrollBarElementsDark,
                                C_UtiStyleSheets::mhc_ScrollBarPropertiesDark, orc_Stylesheet);
   mh_AppendScrollBarStyleSheet(C_UtiStyleSheets::mhc_ScrollBarElementsBright,
                                C_UtiStyleSheets::mhc_ScrollBarPropertiesBright, orc_Stylesheet);
   mh_AppendScrollBarStyleSheet(C_UtiStyleSheets::mhc_ScrollBarElementsMainNavi,
                                C_UtiStyleSheets::mhc_ScrollBarPropertiesMainNavi, orc_Stylesheet);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply stylesheets for scroll bar

   \param[in]      orc_ScrollBarElements     Which elements to apply the stylesheets for
   \param[in]      orc_ScrollBarProperties   Which stylesheets should be applied for these elements
   \param[in,out]  orc_Stylesheet            Dynamic stylesheet document
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UtiStyleSheets::mh_AppendScrollBarStyleSheet(const std::vector<QString> & orc_ScrollBarElements,
                                                    const std::vector<C_UtiStyleSheets::C_PropertyValueColorConfig> & orc_ScrollBarProperties,
                                                    QString & orc_Stylesheet)
{
   if ((orc_ScrollBarElements.size() > 0) && (orc_ScrollBarProperties.size() > 0))
   {
      orc_Stylesheet += "\n";
      //Each property
      for (uint32 u32_ItProperty = 0; u32_ItProperty < orc_ScrollBarProperties.size(); ++u32_ItProperty)
      {
         QString c_Value;
         const C_UtiStyleSheets::C_PropertyValueColorConfig & rc_Property = orc_ScrollBarProperties[u32_ItProperty];
         //Add elements
         for (uint32 u32_ItElement = 0; u32_ItElement < orc_ScrollBarElements.size(); ++u32_ItElement)
         {
            QString c_Entry;
            const QString & rc_Element = orc_ScrollBarElements[u32_ItElement];
            c_Entry = static_cast<QString>("%1%2").arg(rc_Element, rc_Property.c_PropertyName);
            if (u32_ItElement < (orc_ScrollBarElements.size() - 1UL))
            {
               c_Entry += ",";
            }
            c_Entry += "\n";
            orc_Stylesheet += c_Entry;
         }
         c_Value = "{\n";
         //Add config
         c_Value += static_cast<QString>("%1:rgba(%2,%3,%4,%5);").arg(rc_Property.c_ValueName,
                                                                      QString::number(rc_Property.c_Color.red()),
                                                                      QString::number(rc_Property.c_Color.green()),
                                                                      QString::number(rc_Property.c_Color.blue()),
                                                                      QString::number(rc_Property.c_Color.alpha()));
         c_Value += "}\n";
         orc_Stylesheet += c_Value;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_UtiStyleSheets::mh_SetStyleSheetColor(const QString & orc_ColorType, QString & orc_Style,
                                             const QColor & orc_Color)
{
   const sintn sn_Pos = orc_Style.indexOf(orc_ColorType + ":");
   sintn sn_PosSemicolon;
   QString c_StyleColor;

   c_StyleColor = orc_ColorType + ": " + orc_Color.name(QColor::HexArgb);

   if (sn_Pos >= 0)
   {
      // search the semicolon of this part of the stylesheet
      sn_PosSemicolon = orc_Style.indexOf(";", sn_Pos);
      orc_Style.remove(sn_Pos, sn_PosSemicolon - sn_Pos);
      orc_Style.insert(sn_Pos, c_StyleColor);
   }
   else
   {
      orc_Style = c_StyleColor + ";";
   }
}
