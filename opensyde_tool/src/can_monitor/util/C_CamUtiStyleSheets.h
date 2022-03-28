//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for stylesheets of openSYDE CAN Monitor (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMUTISTYLESHEETS_H
#define C_CAMUTISTYLESHEETS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamUtiStyleSheets
{
public:
   static QString h_GetStylesheet(void);

private:
   C_CamUtiStyleSheets();

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
   static const std::vector<QString> mhc_SCROLL_BAR_ELEMENTS_BRIGHT;
   static const std::vector<C_PropertyValueColorConfig> mhc_SCROLL_BAR_PROPERTIES_BRIGHT;
   static const std::vector<QString> mhc_SCROLL_BAR_ELEMENTS_DARK;
   static const std::vector<C_PropertyValueColorConfig> mhc_SCROLL_BAR_PROPERTIES_DARK;

   static void mh_AppendStylesheet(const QString & orc_File, QString & orc_Stylesheet);
   static void mh_AppendDynamicStylesheet(QString & orc_Stylesheet);
   static void mh_AppendScrollBarStyleSheets(QString & orc_Stylesheet);
   static void mh_AppendScrollBarStyleSheet(const std::vector<QString> & orc_ScrollBarElements,
                                            const std::vector<C_PropertyValueColorConfig> & orc_ScrollBarProperties,
                                            QString & orc_Stylesheet);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
