//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for stylesheets of openSYDE CAN Monitor (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFile>

#include "stwtypes.h"
#include "constants.h"

#include "C_CamUtiStyleSheets.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
// Configuration for dynamic transparency color configuration of elements
//lint -e{36,155}  c++11 feature
const std::vector<QString> C_CamUtiStyleSheets::mhc_ScrollAreaElements(
{
}
   );
//lint -e{36,155}  c++11 feature
const std::vector<C_CamUtiStyleSheets::C_PropertyValueColorConfig> C_CamUtiStyleSheets::mhc_ScrollAreaProperties(
{
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("", "background-color", QColor(Qt::transparent))
}
   );

//lint -e{36,155}  c++11 feature
const std::vector<QString> C_CamUtiStyleSheets::mhc_ScrollBarElementsBright(
{
   "stw_opensyde_gui--C_CamGenTableView QScrollBar",
   "stw_opensyde_gui--C_CamGenSigTableView QScrollBar",
   "stw_opensyde_gui_elements--C_OgeTebMessageDetails QScrollBar",
   "stw_opensyde_gui_elements--C_OgeTransparentScrollArea QScrollBar",
   "stw_opensyde_gui--C_CamMetWidget QScrollBar",
   "QAbstractItemView[View=\"CompleterDark\"] QScrollBar",
   "stw_opensyde_gui_elements--C_CamOgeCbxTable QAbstractItemView QScrollBar"
}
   );
//lint -e{36,155}  c++11 feature
const std::vector<C_CamUtiStyleSheets::C_PropertyValueColorConfig> C_CamUtiStyleSheets::mhc_ScrollBarPropertiesBright(
{
   C_CamUtiStyleSheets::C_PropertyValueColorConfig(":vertical", "background-color", QColor(Qt::transparent)),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig(":horizontal", "background-color", QColor(Qt::transparent)),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "background-color", mc_STYLE_GUIDE_COLOR_8),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "border-color", mc_STYLE_GUIDE_COLOR_8),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::handle:hover:!pressed", "background-color",
                                                   mc_STYLE_GUIDE_COLOR_9),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::handle:hover:!pressed", "border-color", mc_STYLE_GUIDE_COLOR_9),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::handle", "background-color", mc_STYLE_GUIDE_COLOR_10),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::handle", "border-color", mc_STYLE_GUIDE_COLOR_10),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("", "background-color", mc_STYLE_GUIDE_COLOR_11),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::add-page", "background-color", mc_STYLE_GUIDE_COLOR_11),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::sub-page", "background-color", mc_STYLE_GUIDE_COLOR_11),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("", "border-color", mc_STYLE_GUIDE_COLOR_11),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::add-page", "border-color", mc_STYLE_GUIDE_COLOR_11),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::sub-page", "border-color", mc_STYLE_GUIDE_COLOR_11)
}
   );
//lint -e{36,155}  c++11 feature
const std::vector<QString> C_CamUtiStyleSheets::mhc_ScrollBarElementsDark(
{
   // insert dark scroll bar here
}
   );
//lint -e{36,155}  c++11 feature
const std::vector<C_CamUtiStyleSheets::C_PropertyValueColorConfig> C_CamUtiStyleSheets::mhc_ScrollBarPropertiesDark(
{
   C_CamUtiStyleSheets::C_PropertyValueColorConfig(":vertical", "background-color", QColor(Qt::transparent)),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig(":horizontal", "background-color", QColor(Qt::transparent)),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "background-color", mc_STYLE_GUIDE_COLOR_10),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::handle:pressed", "border-color", mc_STYLE_GUIDE_COLOR_10),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::handle:hover:!pressed", "background-color",
                                                   mc_STYLE_GUIDE_COLOR_35),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::handle:hover:!pressed", "border-color", mc_STYLE_GUIDE_COLOR_35),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::handle", "background-color", mc_STYLE_GUIDE_COLOR_39),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::handle", "border-color", mc_STYLE_GUIDE_COLOR_39),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("", "background-color", mc_STYLE_GUIDE_COLOR_32),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::add-page", "background-color", mc_STYLE_GUIDE_COLOR_32),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::sub-page", "background-color", mc_STYLE_GUIDE_COLOR_32),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("", "border-color", mc_STYLE_GUIDE_COLOR_32),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::add-page", "border-color", mc_STYLE_GUIDE_COLOR_32),
   C_CamUtiStyleSheets::C_PropertyValueColorConfig("::sub-page", "border-color", mc_STYLE_GUIDE_COLOR_32)
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
QString C_CamUtiStyleSheets::h_GetStylesheet(void)
{
   QString c_Stylesheet;

   mh_AppendScrollBarStyleSheet(C_CamUtiStyleSheets::mhc_ScrollAreaElements,
                                C_CamUtiStyleSheets::mhc_ScrollAreaProperties, c_Stylesheet);

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

   // Append the dynamic created stylesheet
   mh_AppendDynamicStylesheet(c_Stylesheet);

   return c_Stylesheet;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamUtiStyleSheets::C_CamUtiStyleSheets()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append stylesheet of file

   \param[in]     orc_File       QSS file
   \param[in,out] orc_Stylesheet Stylesheet
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamUtiStyleSheets::mh_AppendStylesheet(const QString & orc_File, QString & orc_Stylesheet)
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
void C_CamUtiStyleSheets::mh_AppendDynamicStylesheet(QString & orc_Stylesheet)
{
   mh_AppendScrollBarStyleSheets(orc_Stylesheet);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply stylesheets for scroll bars

   \param[in,out] orc_Stylesheet Dynamic stylesheet document
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamUtiStyleSheets::mh_AppendScrollBarStyleSheets(QString & orc_Stylesheet)
{
   mh_AppendScrollBarStyleSheet(C_CamUtiStyleSheets::mhc_ScrollBarElementsDark,
                                C_CamUtiStyleSheets::mhc_ScrollBarPropertiesDark, orc_Stylesheet);
   mh_AppendScrollBarStyleSheet(C_CamUtiStyleSheets::mhc_ScrollBarElementsBright,
                                C_CamUtiStyleSheets::mhc_ScrollBarPropertiesBright, orc_Stylesheet);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply stylesheets for scroll bar

   \param[in]     orc_ScrollBarElements   Which elements to apply the stylesheets for
   \param[in]     orc_ScrollBarProperties Which stylesheets should be applied for these elements
   \param[in,out] orc_Stylesheet          Dynamic stylesheet document
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamUtiStyleSheets::mh_AppendScrollBarStyleSheet(const std::vector<QString> & orc_ScrollBarElements,
                                                       const std::vector<C_PropertyValueColorConfig> & orc_ScrollBarProperties,
                                                       QString & orc_Stylesheet)
{
   if ((orc_ScrollBarElements.size() > 0) && (orc_ScrollBarProperties.size() > 0))
   {
      orc_Stylesheet += "\n";
      //Each property
      for (uint32 u32_ItProperty = 0; u32_ItProperty < orc_ScrollBarProperties.size(); ++u32_ItProperty)
      {
         QString c_Value;
         const C_CamUtiStyleSheets::C_PropertyValueColorConfig & rc_Property = orc_ScrollBarProperties[u32_ItProperty];
         //Add elements
         for (uint32 u32_ItElement = 0; u32_ItElement < orc_ScrollBarElements.size(); ++u32_ItElement)
         {
            QString c_Entry;
            const QString & rc_Element = orc_ScrollBarElements[u32_ItElement];
            c_Entry = QString("%1%2").arg(rc_Element, rc_Property.c_PropertyName);
            if (u32_ItElement < (orc_ScrollBarElements.size() - 1UL))
            {
               c_Entry += ",";
            }
            c_Entry += "\n";
            orc_Stylesheet += c_Entry;
         }
         c_Value = "{\n";
         //Add config
         c_Value += QString("%1:rgba(%2,%3,%4,%5);").arg(rc_Property.c_ValueName,
                                                         QString::number(rc_Property.c_Color.red()),
                                                         QString::number(rc_Property.c_Color.green()),
                                                         QString::number(rc_Property.c_Color.blue()),
                                                         QString::number(rc_Property.c_Color.alpha()));
         c_Value += "\n}\n";
         orc_Stylesheet += c_Value;
      }
   }
}
