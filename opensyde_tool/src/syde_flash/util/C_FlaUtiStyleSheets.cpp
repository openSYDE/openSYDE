//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for stylesheets of SYDEflash (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFile>

#include "stwtypes.hpp"
#include "constants.hpp"

#include "C_FlaUtiStyleSheets.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
// Configuration for dynamic transparency color configuration of elements
const std::vector<QString> C_FlaUtiStyleSheets::mhc_SCROLL_AREA_ELEMENTS(
{
   "stw--opensyde_gui_elements--C_OgeTebMessageDetails",
   "stw--opensyde_gui--C_SyvDcConnectedNodeList"
}
   );
const std::vector<C_FlaUtiStyleSheets::C_PropertyValueColorConfig> C_FlaUtiStyleSheets::mhc_SCROLL_AREA_PROPERTIES(
{
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("", "background-color", static_cast<QColor>(Qt::transparent))
}
   );

const std::vector<QString> C_FlaUtiStyleSheets::mhc_SCROLL_BAR_SETTINGS_DARK_BLUE(
{
   "stw--opensyde_gui_elements--C_FlaOgeTebProgressLog QScrollBar"
}
   );

const std::vector<QString> C_FlaUtiStyleSheets::mhc_SCROLL_BAR_ELEMENTS_BRIGHT(
{
   "stw--opensyde_gui_elements--C_OgeTebMessageDetails QScrollBar",
   "stw--opensyde_gui_elements--C_OgeTransparentScrollArea QScrollBar",
   "stw--opensyde_gui--C_SyvDcConnectedNodeList QScrollBar"
}
   );

const std::vector<C_FlaUtiStyleSheets::C_PropertyValueColorConfig> C_FlaUtiStyleSheets::
mhc_SCROLL_BAR_PROPERTIES_SETTINGS_DARK_BLUE(
{
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "background-color", mc_STYLE_GUIDE_COLOR_11),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "border-color", mc_STYLE_GUIDE_COLOR_11),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:!pressed", "background-color", mc_STYLE_GUIDE_COLOR_10),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:!pressed", "border-color", mc_STYLE_GUIDE_COLOR_10),

   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:hover", "background-color", mc_STYLE_GUIDE_COLOR_11),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:hover", "border-color", mc_STYLE_GUIDE_COLOR_11),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:!hover", "background-color", mc_STYLE_GUIDE_COLOR_10),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:!hover", "border-color", mc_STYLE_GUIDE_COLOR_10)
}
   );

const std::vector<C_FlaUtiStyleSheets::C_PropertyValueColorConfig> C_FlaUtiStyleSheets::mhc_SCROLL_BAR_PROPERTIES_BRIGHT(
{
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig(":vertical", "background-color",
                                                   static_cast<QColor>(Qt::transparent)),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig(":horizontal", "background-color",
                                                   static_cast<QColor>(Qt::transparent)),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "background-color", mc_STYLE_GUIDE_COLOR_8),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "border-color", mc_STYLE_GUIDE_COLOR_8),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:hover:!pressed", "background-color",
                                                   mc_STYLE_GUIDE_COLOR_9),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:hover:!pressed", "border-color", mc_STYLE_GUIDE_COLOR_9),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle", "background-color", mc_STYLE_GUIDE_COLOR_10),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle", "border-color", mc_STYLE_GUIDE_COLOR_10),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("", "background-color", mc_STYLE_GUIDE_COLOR_11),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::add-page", "background-color", mc_STYLE_GUIDE_COLOR_11),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::sub-page", "background-color", mc_STYLE_GUIDE_COLOR_11),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("", "border-color", mc_STYLE_GUIDE_COLOR_11),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::add-page", "border-color", mc_STYLE_GUIDE_COLOR_11),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::sub-page", "border-color", mc_STYLE_GUIDE_COLOR_11)
}
   );
const std::vector<QString> C_FlaUtiStyleSheets::mhc_SCROLL_BAR_ELEMENTS_DARK(
{
   // insert dark scroll bar here
}
   );
const std::vector<C_FlaUtiStyleSheets::C_PropertyValueColorConfig> C_FlaUtiStyleSheets::mhc_SCROLL_BAR_PROPERTIES_DARK(
{
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig(":vertical", "background-color",
                                                   static_cast<QColor>(Qt::transparent)),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig(":horizontal", "background-color",
                                                   static_cast<QColor>(Qt::transparent)),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "background-color", mc_STYLE_GUIDE_COLOR_10),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "border-color", mc_STYLE_GUIDE_COLOR_10),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:hover:!pressed", "background-color",
                                                   mc_STYLE_GUIDE_COLOR_35),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle:hover:!pressed", "border-color", mc_STYLE_GUIDE_COLOR_35),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle", "background-color", mc_STYLE_GUIDE_COLOR_39),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::handle", "border-color", mc_STYLE_GUIDE_COLOR_39),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("", "background-color", mc_STYLE_GUIDE_COLOR_32),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::add-page", "background-color", mc_STYLE_GUIDE_COLOR_32),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::sub-page", "background-color", mc_STYLE_GUIDE_COLOR_32),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("", "border-color", mc_STYLE_GUIDE_COLOR_32),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::add-page", "border-color", mc_STYLE_GUIDE_COLOR_32),
   C_FlaUtiStyleSheets::C_PropertyValueColorConfig("::sub-page", "border-color", mc_STYLE_GUIDE_COLOR_32)
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
   QSS file
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_FlaUtiStyleSheets::h_GetStylesheet(void)
{
   QString c_Stylesheet;

   mh_AppendScrollBarStyleSheet(C_FlaUtiStyleSheets::mhc_SCROLL_AREA_ELEMENTS,
                                C_FlaUtiStyleSheets::mhc_SCROLL_AREA_PROPERTIES, c_Stylesheet);

   mh_AppendStylesheet("://styles/Color.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/Font.qss", c_Stylesheet);

   mh_AppendStylesheet("://styles/CheckBox.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/ComboBox.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/GroupBox.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/Label.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/LineEdit.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/Menu.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/PushButton.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/RadioButton.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/ScrollArea.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/ScrollBar.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/SpinBox.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/TableView.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/TextBrowser.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/TextEdit.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/ToolButton.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/TreeView.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/Widget.qss", c_Stylesheet);
   mh_AppendStylesheet("://styles/ListWidget.qss", c_Stylesheet);

   // Append the dynamic created stylesheet
   mh_AppendDynamicStylesheet(c_Stylesheet);

   return c_Stylesheet;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUtiStyleSheets::C_FlaUtiStyleSheets()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append stylesheet of file

   \param[in]     orc_File       QSS file
   \param[in,out] orc_Stylesheet Stylesheet
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUtiStyleSheets::mh_AppendStylesheet(const QString & orc_File, QString & orc_Stylesheet)
{
   QFile c_File(orc_File);

   c_File.open(QFile::ReadOnly);
   orc_Stylesheet += QLatin1String(c_File.readAll());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append dynamic created stylesheet

   \param[in,out] orc_Stylesheet Stylesheet
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUtiStyleSheets::mh_AppendDynamicStylesheet(QString & orc_Stylesheet)
{
   mh_AppendScrollBarStyleSheets(orc_Stylesheet);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply stylesheets for scroll bars

   \param[in,out] orc_Stylesheet Dynamic stylesheet document
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUtiStyleSheets::mh_AppendScrollBarStyleSheets(QString & orc_Stylesheet)
{
   mh_AppendScrollBarStyleSheet(C_FlaUtiStyleSheets::mhc_SCROLL_BAR_ELEMENTS_DARK,
                                C_FlaUtiStyleSheets::mhc_SCROLL_BAR_PROPERTIES_DARK, orc_Stylesheet);
   mh_AppendScrollBarStyleSheet(C_FlaUtiStyleSheets::mhc_SCROLL_BAR_ELEMENTS_BRIGHT,
                                C_FlaUtiStyleSheets::mhc_SCROLL_BAR_PROPERTIES_BRIGHT, orc_Stylesheet);
   mh_AppendScrollBarStyleSheet(C_FlaUtiStyleSheets::mhc_SCROLL_BAR_SETTINGS_DARK_BLUE,
                                C_FlaUtiStyleSheets::mhc_SCROLL_BAR_PROPERTIES_SETTINGS_DARK_BLUE, orc_Stylesheet);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply stylesheets for scroll bar

   \param[in]     orc_ScrollBarElements   Which elements to apply the stylesheets for
   \param[in]     orc_ScrollBarProperties Which stylesheets should be applied for these elements
   \param[in,out] orc_Stylesheet          Dynamic stylesheet document
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUtiStyleSheets::mh_AppendScrollBarStyleSheet(const std::vector<QString> & orc_ScrollBarElements,
                                                       const std::vector<C_PropertyValueColorConfig> & orc_ScrollBarProperties,
                                                       QString & orc_Stylesheet)
{
   if ((orc_ScrollBarElements.size() > 0) && (orc_ScrollBarProperties.size() > 0))
   {
      orc_Stylesheet += "\n";
      //Each property
      for (uint32_t u32_ItProperty = 0; u32_ItProperty < orc_ScrollBarProperties.size(); ++u32_ItProperty)
      {
         QString c_Value;
         const C_FlaUtiStyleSheets::C_PropertyValueColorConfig & rc_Property = orc_ScrollBarProperties[u32_ItProperty];
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
         c_Value += "\n}\n";
         orc_Stylesheet += c_Value;
      }
   }
}
