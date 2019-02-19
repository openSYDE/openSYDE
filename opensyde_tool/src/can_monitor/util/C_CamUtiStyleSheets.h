//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for stylesheets of openSYDE CAN Monitor (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.11.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMUTISTYLESHEETS_H
#define C_CAMUTISTYLESHEETS_H

/* -- Includes ------------------------------------------------------------- */
#include <QString>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

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

   static const std::vector<QString> mhc_ScrollAreaElements;
   static const std::vector<C_PropertyValueColorConfig> mhc_ScrollAreaProperties;
   static const std::vector<QString> mhc_ScrollBarElementsBright;
   static const std::vector<C_PropertyValueColorConfig> mhc_ScrollBarPropertiesBright;
   static const std::vector<QString> mhc_ScrollBarElementsDark;
   static const std::vector<C_PropertyValueColorConfig> mhc_ScrollBarPropertiesDark;

   static void mh_AppendStylesheet(const QString & orc_File, QString & orc_Stylesheet);
   static void mh_AppendDynamicStylesheet(QString & orc_Stylesheet);
   static void mh_AppendScrollBarStyleSheets(QString & orc_Stylesheet);
   static void mh_AppendScrollBarStyleSheet(const std::vector<QString> & orc_ScrollBarElements,
                                            const std::vector<C_PropertyValueColorConfig> & orc_ScrollBarProperties,
                                            QString & orc_Stylesheet);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
