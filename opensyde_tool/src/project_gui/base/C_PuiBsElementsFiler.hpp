//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base item filer (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUIBSELEMENTSFILER_H
#define C_PUIBSELEMENTSFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiBsElements.hpp"
#include "C_OscXmlParser.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsElementsFiler
{
public:
   C_PuiBsElementsFiler(void);
   static int32_t h_LoadBaseElements(C_PuiBsElements & orc_Elements,
                                     stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveBaseElements(const C_PuiBsElements & orc_Elements,
                                  stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadTextElements(std::vector<C_PuiBsTextElement> & orc_TextElements,
                                     stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveTextElements(const std::vector<C_PuiBsTextElement> & orc_TextElements,
                                  stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadBoundaries(std::vector<C_PuiBsBoundary> & orc_Boundaries,
                                   stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveBoundaries(const std::vector<C_PuiBsBoundary> & orc_Boundaries,
                                stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadImages(std::vector<C_PuiBsImage> & orc_Images,
                               stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveImages(const std::vector<C_PuiBsImage> & orc_Images,
                            stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadLineArrows(std::vector<C_PuiBsLineArrow> & orc_LineArrows,
                                   stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveLineArrows(const std::vector<C_PuiBsLineArrow> & orc_LineArrows,
                                stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadBoxBase(C_PuiBsBox & orc_BoxBase, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveBoxBase(const C_PuiBsBox & orc_BoxBase, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadLineBase(C_PuiBsLineBase & orc_LineBase,
                                 stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveLineBase(const C_PuiBsLineBase & orc_LineBase,
                              stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);

   static int32_t h_LoadTextElement(C_PuiBsTextElement * const opc_TextElement,
                                    stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveTextElement(const C_PuiBsTextElement * const opc_TextElement,
                                 stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);

   static void h_LoadColor(QColor & orc_Color, const stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveColor(const QColor & orc_Color, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);

private:
   static int32_t mh_LoadBoundary(C_PuiBsBoundary & orc_Boundary,
                                  stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveBoundary(const C_PuiBsBoundary & orc_Boundary,
                               stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadImage(C_PuiBsImage & orc_Image, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveImage(const C_PuiBsImage & orc_Image, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadLineArrow(C_PuiBsLineArrow & orc_LineArrow,
                                   stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveLineArrow(const C_PuiBsLineArrow & orc_LineArrow,
                                stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);

   static void mh_LoadFontStyle(QFont & orc_FontStyle, const stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveFontStyle(const QFont & orc_FontStyle, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void mh_PixmapToString(const QPixmap & orc_Pixmap, QString & orc_String);
   static void mh_StringToPixmap(const QString & orc_String, QPixmap & orc_Pixmap);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
