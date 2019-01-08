//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for stylesheets (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.09.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_UTISTYLESHEETS_H
#define C_UTISTYLESHEETS_H

/* -- Includes ------------------------------------------------------------- */
#include <QString>
#include <QColor>

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_UtiStyleSheets
{
public:
   static QString h_GetStylesheet(void);
   static QString h_ToStyledHtml(const QString & orc_Html);

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

   static const std::vector<QString> mhc_ScrollAreaElements;
   static const std::vector<C_PropertyValueColorConfig> mhc_ScrollAreaProperties;
   static const std::vector<C_ElementColorConfig> mhc_ElementColorsWithTransparency;
   static const std::vector<QString> mhc_ScrollBarElementsBright;
   static const std::vector<C_PropertyValueColorConfig> mhc_ScrollBarPropertiesBright;
   static const std::vector<QString> mhc_ScrollBarElementsDark;
   static const std::vector<C_PropertyValueColorConfig> mhc_ScrollBarPropertiesDark;
   static const std::vector<QString> mhc_ScrollBarElementsMainNavi;
   static const std::vector<C_PropertyValueColorConfig> mhc_ScrollBarPropertiesMainNavi;

   static void mh_AppendStylesheet(const QString & orc_File, QString & orc_Stylesheet);
   static void mh_AppendDynamicStylesheet(QString & orc_Stylesheet);
   static void mh_AppendTransparentColorSteps(QString & orc_Stylesheet);
   static void mh_AppendScrollBarStyleSheets(QString & orc_Stylesheet);
   static void mh_AppendScrollBarStyleSheet(const std::vector<QString> & orc_ScrollBarElements,
                                            const std::vector<C_PropertyValueColorConfig> & orc_ScrollBarProperties,
                                            QString & orc_Stylesheet);

   static void mh_SetStyleSheetColor(const QString & orc_ColorType, QString & orc_Style, const QColor & orc_Color);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
