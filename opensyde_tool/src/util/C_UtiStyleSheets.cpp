//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for stylesheets (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QTextDocument>
#include <QFile>

#include "constants.hpp"

#include "C_UtiStyleSheets.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
// Configuration for dynamic transparency color configuration of elements
const std::vector<QString> C_UtiStyleSheets::mhc_SCROLL_AREA_ELEMENTS(
{
   "stw--opensyde_gui--C_SdSearchTreeWidget",
   "stw--opensyde_gui_elements--C_OgeTedTable",
   "stw--opensyde_gui--C_SyvDaItPaListTreeWidget",
   "stw--opensyde_gui--C_SdNdeDbListWidget",
   "stw--opensyde_gui--C_SyvDaPeUpdateModeTreeWidget",
   "stw--opensyde_gui_elements--C_OgeTableWidgetComIf",
   "stw--opensyde_gui--C_SdNdeDpListsTreeWidget",
   "stw--opensyde_gui--C_SyvDaItPaArView",
   "stw--opensyde_gui--C_SyvDaItPaTableView",
   "stw--opensyde_gui--C_SdBueSignalTableView",
   "stw--opensyde_gui--C_SdBueMessageTableView",
   "stw--opensyde_gui--C_SdNdeHalcOvTableView",
   "stw--opensyde_gui--C_SdNdeCoOverviewTableView",
   "stw--opensyde_gui--C_SdNdeCoPdoTableView",
   "stw--opensyde_gui--C_SyvDaPeUpdateModeTableView",
   "stw--opensyde_gui--C_SdNdeDpListTableView",
   "stw--opensyde_gui--C_SdNdeHalcConfigImportView",
   "stw--opensyde_gui_elements--C_OgeTreeViewCheckable",
   "stw--opensyde_gui--C_TblTreDataElementView",
   "stw--opensyde_gui--C_SyvDaItTaView",
   "stw--opensyde_gui--C_SyvDaItPaTreeView",
   "stw--opensyde_gui--C_SdNdeDpListDataSetView",
   "stw--opensyde_gui--C_SdNdeDpListArrayEditView",
   "stw--opensyde_gui--C_SdNdeSfoResetMessageTableView",
   "stw--opensyde_gui--C_SdBueMessageSelectorTreeWidget",
   "stw--opensyde_gui--C_SdNdeCoConfigTreeView",
   "stw--opensyde_gui--C_SdNdeHalcChannelTreeView",
   "stw--opensyde_gui--C_SdNdeHalcConfigTreeView",
   "stw--opensyde_gui_elements--C_OgeCbxTableHalc QAbstractItemView QScrollBar",
   "stw--opensyde_gui_elements--C_OgeCbxMultiSelectTableHalc QAbstractItemView QScrollBar",
   "stw--opensyde_gui_elements--C_OgeTedPropertiesComment",
   "stw--opensyde_gui_elements--C_OgeTebMessageDetails",
   "stw--opensyde_gui_elements--C_OgeTebReport",
   "stw--opensyde_gui--C_SebGraphicsView",
   "stw--opensyde_gui--C_SyvUpPacListWidget",
   "stw--opensyde_gui--C_SyvUpPacSectionNodeWidget",
   "stw--opensyde_gui--C_SyvDaChaDataSelectorWidget",
   "stw--opensyde_gui_elements--C_OgeCbxText QAbstractItemView",
   "stw--opensyde_gui_elements--C_OgeSmoothScrollArea",
   "stw--opensyde_gui--C_SdBueNodeSelectorCheckBoxListWidget QAbstractScrollArea",
   "stw--opensyde_gui_elements--C_OgeCbxFontProperties QAbstractItemView",
   "stw--opensyde_gui_elements--C_OgeDarkScrollArea",
   "stw--opensyde_gui_elements--C_OgeTransparentScrollArea",
   "stw--opensyde_gui--C_SyvDcExistingNodeList",
   "stw--opensyde_gui--C_SyvDcConnectedNodeList",
   "stw--opensyde_gui--C_SdBueMlvGraphicsView",
   "stw--opensyde_gui_elements--C_OgeDarkScrollArea",
   "stw--opensyde_gui_logic--C_PopFileTableView",
   "stw--opensyde_gui_elements--C_OgeSaNaviBar",
   "stw--opensyde_gui--C_SdNdeDpSelectorAddListWidget",
   "stw--opensyde_gui--C_SdNdeDalLogJobDataSelectionTableView",
   "stw--opensyde_gui--C_SdNdeDalLogJobsListView"
}
   );
const std::vector<C_UtiStyleSheets::C_PropertyValueColorConfig> C_UtiStyleSheets::mhc_SCROLL_AREA_PROPERTIES(
{
   C_UtiStyleSheets::C_PropertyValueColorConfig("", "background-color", Qt::transparent)
}
   );
const std::vector<C_UtiStyleSheets::C_ElementColorConfig> C_UtiStyleSheets::mhc_ELEMENT_COLORS_WITH_TRANSPARENCY(
{
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"OPENSYDE_BRIGHT\"]", mc_STYLE_GUIDE_COLOR_32),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"OPENSYDE_DARK\"]", mc_STYLE_GUIDE_COLOR_32),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"OPENSYDE_2_BRIGHT\"]", mc_STYLE_GUIDE_COLOR_3),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"OPENSYDE_2_DARK\"]", mc_STYLE_GUIDE_COLOR_0),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"FLAT_BRIGHT\"]", mc_STYLE_GUIDE_COLOR_1),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"FLAT_DARK\"]",
      mc_STYLE_GUIDE_COLOR_0),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"SKEUOMORPH_BRIGHT\"]", mc_STYLE_GUIDE_COLOR_34),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardLabelValue[Style=\"SKEUOMORPH_DARK\"]", mc_STYLE_GUIDE_COLOR_0),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_BRIGHT\"][Selected=\"true\"][Active=\"true\"]",
      mc_STYLE_GUIDE_COLOR_4),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_DARK\"][Selected=\"true\"][Active=\"true\"]",
      mc_STYLE_GUIDE_COLOR_0),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_BRIGHT\"][Selected=\"false\"][Active=\"true\"]",
      mc_STYLE_GUIDE_COLOR_34),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_DARK\"][Selected=\"false\"][Active=\"true\"]",
      mc_STYLE_GUIDE_COLOR_10),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_BRIGHT\"][Selected=\"true\"][Active=\"false\"]",
      mc_STYLE_GUIDE_COLOR_8),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_DARK\"][Selected=\"true\"][Active=\"false\"]",
      mc_STYLE_GUIDE_COLOR_8),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_BRIGHT\"][Selected=\"false\"][Active=\"false\"]",
      mc_STYLE_GUIDE_COLOR_8),
   C_UtiStyleSheets::C_ElementColorConfig(
      "stw--opensyde_gui_elements--C_OgeLabDashboardChart#pc_LabelValue[Style=\"OPENSYDE_DARK\"][Selected=\"false\"][Active=\"false\"]",
      mc_STYLE_GUIDE_COLOR_8)
}
   );

const std::vector<QString> C_UtiStyleSheets::mhc_SCROLL_BAR_ELEMENTS_BRIGHT(
{
   "stw--opensyde_gui--C_SyvDaItPaArView QScrollBar",
   "stw--opensyde_gui--C_SyvDaItPaTableView QScrollBar",
   "stw--opensyde_gui--C_SdSearchTreeWidget QScrollBar",
   "stw--opensyde_gui_elements--C_OgeTedTable QScrollBar",
   "stw--opensyde_gui--C_SdBueSignalTableView QScrollBar",
   "stw--opensyde_gui--C_SdBueMessageTableView QScrollBar",
   "stw--opensyde_gui--C_SdNdeHalcOvTableView QScrollBar",
   "stw--opensyde_gui--C_SdNdeCoOverviewTableView QScrollBar",
   "stw--opensyde_gui--C_SdNdeCoPdoTableView QScrollBar",
   "stw--opensyde_gui--C_SyvDaItPaListTreeWidget QScrollBar",
   "stw--opensyde_gui--C_SdNdeDbListWidget QScrollBar[C_SdNdeDbWidget=\"true\"]",
   "stw--opensyde_gui--C_SyvDaPeUpdateModeTableView QScrollBar",
   "stw--opensyde_gui--C_SdNdeDpListTableView QScrollBar",
   "stw--opensyde_gui--C_SdNdeHalcConfigImportView QScrollBar",
   "stw--opensyde_gui_elements--C_OgeTreeViewCheckable QScrollBar",
   "stw--opensyde_gui--C_TblTreDataElementView QScrollBar",
   "stw--opensyde_gui--C_SyvDaPeUpdateModeTreeWidget QScrollBar",
   "stw--opensyde_gui_elements--C_OgeTableWidgetComIf QScrollBar",
   "stw--opensyde_gui--C_SdNdeDpListsTreeWidget QScrollBar",
   "stw--opensyde_gui--C_SdNdeDpListDataSetView QScrollBar",
   "stw--opensyde_gui--C_SdNdeDpListArrayEditView QScrollBar",
   "stw--opensyde_gui--C_SdNdeSfoResetMessageTableView QScrollBar",
   "stw--opensyde_gui--C_SdBueMessageSelectorTreeWidget QScrollBar",
   "stw--opensyde_gui--C_SdNdeCoConfigTreeView QScrollBar",
   "stw--opensyde_gui--C_SdNdeHalcChannelTreeView QScrollBar",
   "stw--opensyde_gui--C_SdNdeHalcConfigTreeView QScrollBar",
   "stw--opensyde_gui_elements--C_OgeCbxTableHalc QAbstractItemView QScrollBar",
   "stw--opensyde_gui_elements--C_OgeCbxMultiSelectTableHalc QAbstractItemView QScrollBar",
   "stw--opensyde_gui--C_SyvDaItPaTreeView QScrollBar[DarkMode=\"false\"]",
   "stw--opensyde_gui--C_SyvDaItTaView QScrollBar[DarkMode=\"false\"]",
   "stw--opensyde_gui_elements--C_OgeTedPropertiesComment QScrollBar",
   "stw--opensyde_gui_elements--C_OgeTebMessageDetails QScrollBar",
   "stw--opensyde_gui_elements--C_OgeTebReport QScrollBar",
   "stw--opensyde_gui--C_SebGraphicsView QScrollBar[DarkMode=\"false\"]",
   "stw--opensyde_gui--C_SdBueNodeSelectorCheckBoxListWidget QScrollBar",
   "stw--opensyde_gui--C_SyvUpPacSectionNodeWidget QScrollBar",
   "stw--opensyde_gui--C_SyvDaChaDataSelectorWidget QScrollBar[Style=\"OPENSYDE_BRIGHT\"]",
   "stw--opensyde_gui--C_SyvDaChaDataSelectorWidget QScrollBar[Style=\"FLAT_BRIGHT\"]",
   "stw--opensyde_gui--C_SyvDaChaDataSelectorWidget QScrollBar[Style=\"SKEUOMORPH_BRIGHT\"]",
   "stw--opensyde_gui--C_SyvDaChaDataSelectorWidget QScrollBar[Style=\"OPENSYDE_2_BRIGHT\"]",
   "stw--opensyde_gui_elements--C_OgeCbxText QAbstractItemView QScrollBar",
   "stw--opensyde_gui_elements--C_OgeSmoothScrollArea[DarkMode=\"false\"] QScrollBar",
   "stw--opensyde_gui_elements--C_OgeCbxFontProperties QAbstractItemView QScrollBar",
   "stw--opensyde_gui_elements--C_OgeDarkScrollArea QScrollBar[DarkMode=\"false\"]",
   "stw--opensyde_gui_elements--C_OgeTransparentScrollArea QScrollBar",
   "stw--opensyde_gui--C_SyvDcExistingNodeList QScrollBar",
   "stw--opensyde_gui--C_SyvDcConnectedNodeList QScrollBar",
   "stw--opensyde_gui--C_SdNdeDpSelectorAddListWidget QScrollBar",
   "stw--opensyde_gui--C_SdNdeDalLogJobDataSelectionTableView QScrollBar",
   "stw--opensyde_gui--C_SdNdeDalLogJobsListView QScrollbar"
}
   );
const std::vector<C_UtiStyleSheets::C_PropertyValueColorConfig> C_UtiStyleSheets::mhc_SCROLL_BAR_PROPERTIES_BRIGHT(
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
const std::vector<QString> C_UtiStyleSheets::mhc_SCROLL_BAR_ELEMENTS_DARK(
{
   "stw--opensyde_gui--C_SyvDaItPaTreeView QScrollBar[DarkMode=\"true\"]",
   "stw--opensyde_gui--C_SyvDaItTaView QScrollBar[DarkMode=\"true\"]",
   "stw--opensyde_gui--C_SebGraphicsView QScrollBar[DarkMode=\"true\"]",
   "stw--opensyde_gui_elements--C_OgeSmoothScrollArea QScrollBar[DarkMode=\"true\"]",
   "stw--opensyde_gui_elements--C_OgeDarkScrollArea QScrollBar[DarkMode=\"true\"]",
   "stw--opensyde_gui--C_SyvDaChaDataSelectorWidget QScrollBar[Style=\"OPENSYDE_DARK\"]",
   "stw--opensyde_gui--C_SyvDaChaDataSelectorWidget QScrollBar[Style=\"FLAT_DARK\"]",
   "stw--opensyde_gui--C_SyvDaChaDataSelectorWidget QScrollBar[Style=\"SKEUOMORPH_DARK\"]",
   "stw--opensyde_gui--C_SyvDaChaDataSelectorWidget QScrollBar[Style=\"OPENSYDE_2_DARK\"]"
}
   );
const std::vector<C_UtiStyleSheets::C_PropertyValueColorConfig> C_UtiStyleSheets::mhc_SCROLL_BAR_PROPERTIES_DARK(
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
const std::vector<QString> C_UtiStyleSheets::mhc_SCROLL_BAR_ELEMENTS_MAIN_NAVI(
{
   "stw--opensyde_gui_logic--C_PopFileTableView QScrollBar",
   "stw--opensyde_gui_elements--C_OgeSaNaviBar QScrollBar"
}
   );
const std::vector<C_UtiStyleSheets::C_PropertyValueColorConfig> C_UtiStyleSheets::mhc_SCROLL_BAR_PROPERTIES_MAIN_NAVI(
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

   mh_AppendScrollBarStyleSheet(C_UtiStyleSheets::mhc_SCROLL_AREA_ELEMENTS,
                                C_UtiStyleSheets::mhc_SCROLL_AREA_PROPERTIES,
                                c_Stylesheet);

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
   const int32_t s32_Pos = orc_Style.indexOf(c_Category);
   QColor c_Color;

   c_Color.setNamedColor(orc_Style.mid(s32_Pos + c_Category.length(), 9));

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
   uint32_t u32_Counter;
   const QString c_TransparencyPropertyStart = "[Transparency=\"";
   const QString c_TransparencyPropertyEnd = "\"] {\n";
   const QString c_End = "\n}\n";

   for (u32_Counter = 0; u32_Counter < mhc_ELEMENT_COLORS_WITH_TRANSPARENCY.size(); ++u32_Counter)
   {
      // Prepare color for start
      QColor c_Color = mhc_ELEMENT_COLORS_WITH_TRANSPARENCY[u32_Counter].c_Color;
      int32_t s32_AlphaValue = ms32_TRANSPARENCY_START;

      do
      {
         QString c_Line;
         QString c_ColorString;

         c_Color.setAlpha(s32_AlphaValue);

         // Build the stylesheet string
         c_Line = mhc_ELEMENT_COLORS_WITH_TRANSPARENCY[u32_Counter].c_ElementName;
         c_Line += c_TransparencyPropertyStart;
         // Add the property value -> transparency value
         c_Line += QString::number(s32_AlphaValue);
         c_Line += c_TransparencyPropertyEnd;
         // Add the color itself
         mh_SetStyleSheetColor("color", c_ColorString, c_Color);
         c_Line += c_ColorString;
         // Finish the line
         c_Line += c_End;

         orc_Stylesheet += c_Line;

         // Prepare next transparent step
         ++s32_AlphaValue;
      }
      while (s32_AlphaValue <= ms32_TRANSPARENCY_END);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply stylesheets for scroll bars

   \param[in,out]  orc_Stylesheet   Dynamic stylesheet document
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UtiStyleSheets::mh_AppendScrollBarStyleSheets(QString & orc_Stylesheet)
{
   mh_AppendScrollBarStyleSheet(C_UtiStyleSheets::mhc_SCROLL_BAR_ELEMENTS_DARK,
                                C_UtiStyleSheets::mhc_SCROLL_BAR_PROPERTIES_DARK, orc_Stylesheet);
   mh_AppendScrollBarStyleSheet(C_UtiStyleSheets::mhc_SCROLL_BAR_ELEMENTS_BRIGHT,
                                C_UtiStyleSheets::mhc_SCROLL_BAR_PROPERTIES_BRIGHT, orc_Stylesheet);
   mh_AppendScrollBarStyleSheet(C_UtiStyleSheets::mhc_SCROLL_BAR_ELEMENTS_MAIN_NAVI,
                                C_UtiStyleSheets::mhc_SCROLL_BAR_PROPERTIES_MAIN_NAVI, orc_Stylesheet);
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
      for (uint32_t u32_ItProperty = 0; u32_ItProperty < orc_ScrollBarProperties.size(); ++u32_ItProperty)
      {
         QString c_Value;
         const C_UtiStyleSheets::C_PropertyValueColorConfig & rc_Property = orc_ScrollBarProperties[u32_ItProperty];
         //Add elements
         for (uint32_t u32_ItElement = 0; u32_ItElement < orc_ScrollBarElements.size(); ++u32_ItElement)
         {
            QString c_Entry;
            const QString & rc_Element = orc_ScrollBarElements[u32_ItElement];
            c_Entry = static_cast<QString>("%1%2").arg(rc_Element, rc_Property.c_PropertyName);
            if (u32_ItElement < (static_cast<uint32_t>(orc_ScrollBarElements.size()) - 1UL))
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
   const int32_t s32_Pos = orc_Style.indexOf(orc_ColorType + ":");
   QString c_StyleColor;

   c_StyleColor = orc_ColorType + ": " + orc_Color.name(QColor::HexArgb);

   if (s32_Pos >= 0)
   {
      // search the semicolon of this part of the stylesheet
      const int32_t s32_PosSemicolon = orc_Style.indexOf(";", s32_Pos);
      orc_Style.remove(s32_Pos, s32_PosSemicolon - s32_Pos);
      orc_Style.insert(s32_Pos, c_StyleColor);
   }
   else
   {
      orc_Style = c_StyleColor + ";";
   }
}
