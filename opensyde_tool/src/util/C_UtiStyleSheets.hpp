//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for stylesheets (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_UTISTYLESHEETS_HPP
#define C_UTISTYLESHEETS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QColor>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UtiStyleSheets
{
public:
   static QString h_GetStylesheet(void);

   static QColor h_GetStyleSheetColor(const QString & orc_Style);
   static void h_SetStyleSheetBackgroundColor(QString & orc_Style, const QColor & orc_Color);

private:
   C_UtiStyleSheets(void);

   class C_ElementColorConfig
   {
   public:
      C_ElementColorConfig(const QString & orc_ElementName, const QColor & orc_Color) :
         c_ElementName(orc_ElementName),
         c_Color(orc_Color)
      {
      }

      const QString c_ElementName;
      const QColor c_Color;
   };

   class C_PropertyValueColorConfig
   {
   public:
      C_PropertyValueColorConfig(const QString & orc_PropertyName, const QString & orc_ValueName,
                                 const QColor & orc_Color) :
         c_PropertyName(orc_PropertyName),
         c_ValueName(orc_ValueName),
         c_Color(orc_Color)
      {
      }

      const QString c_PropertyName;
      const QString c_ValueName;
      const QColor c_Color;
   };

   static const std::vector<QString> mhc_SCROLL_AREA_ELEMENTS;
   static const std::vector<C_PropertyValueColorConfig> mhc_SCROLL_AREA_PROPERTIES;
   static const std::vector<C_ElementColorConfig> mhc_ELEMENT_COLORS_WITH_TRANSPARENCY;
   static const std::vector<QString> mhc_SCROLL_BAR_ELEMENTS_BRIGHT;
   static const std::vector<C_PropertyValueColorConfig> mhc_SCROLL_BAR_PROPERTIES_BRIGHT;
   static const std::vector<QString> mhc_SCROLL_BAR_ELEMENTS_DARK;
   static const std::vector<C_PropertyValueColorConfig> mhc_SCROLL_BAR_PROPERTIES_DARK;
   static const std::vector<QString> mhc_SCROLL_BAR_ELEMENTS_MAIN_NAVI;
   static const std::vector<C_PropertyValueColorConfig> mhc_SCROLL_BAR_PROPERTIES_MAIN_NAVI;

   static void mh_AppendStylesheet(const QString & orc_File, QString & orc_Stylesheet);
   static void mh_AppendDynamicStylesheet(QString & orc_Stylesheet);
   static void mh_AppendTransparentColorSteps(QString & orc_Stylesheet);
   static void mh_AppendScrollBarStyleSheets(QString & orc_Stylesheet);
   static void mh_AppendScrollBarStyleSheet(const std::vector<QString> & orc_ScrollBarElements,
                                            const std::vector<C_PropertyValueColorConfig> & orc_ScrollBarProperties,
                                            QString & orc_Stylesheet);

   static void mh_SetStyleSheetColor(const QString & orc_ColorType, QString & orc_Style, const QColor & orc_Color);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
