//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Base item filer (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUIBSELEMENTSFILER_H
#define C_PUIBSELEMENTSFILER_H

/* -- Includes ------------------------------------------------------------- */
#include "C_PuiBsElements.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiBsElementsFiler
{
public:
   C_PuiBsElementsFiler(void);
   static stw_types::sint32 h_LoadBaseElements(C_PuiBsElements & orc_Elements,
                                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveBaseElements(const C_PuiBsElements & orc_Elements,
                                  stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadTextElements(std::vector<C_PuiBsTextElement> & orc_TextElements,
                                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveTextElements(const std::vector<C_PuiBsTextElement> & orc_TextElements,
                                  stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadBoundaries(std::vector<C_PuiBsBoundary> & orc_Boundaries,
                                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveBoundaries(const std::vector<C_PuiBsBoundary> & orc_Boundaries,
                                stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadImages(std::vector<C_PuiBsImage> & orc_Images,
                                         stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveImages(const std::vector<C_PuiBsImage> & orc_Images,
                            stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadLineArrows(std::vector<C_PuiBsLineArrow> & orc_LineArrows,
                                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveLineArrows(const std::vector<C_PuiBsLineArrow> & orc_LineArrows,
                                stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadBoxBase(C_PuiBsBox & orc_BoxBase,
                                          stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveBoxBase(const C_PuiBsBox & orc_BoxBase, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadLineBase(C_PuiBsLineBase & orc_LineBase,
                                           stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveLineBase(const C_PuiBsLineBase & orc_LineBase,
                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);

   static stw_types::sint32 h_LoadTextElement(C_PuiBsTextElement * const opc_TextElement,
                                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveTextElement(const C_PuiBsTextElement * const opc_TextElement,
                                 stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);

   static void h_LoadColor(QColor & orc_Color, const stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveColor(const QColor & orc_Color, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);

private:
   static stw_types::sint32 mh_LoadBoundary(C_PuiBsBoundary & orc_Boundary,
                                            stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveBoundary(const C_PuiBsBoundary & orc_Boundary,
                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadImage(C_PuiBsImage & orc_Image,
                                         stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveImage(const C_PuiBsImage & orc_Image, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadLineArrow(C_PuiBsLineArrow & orc_LineArrow,
                                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveLineArrow(const C_PuiBsLineArrow & orc_LineArrow,
                                stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);

   static void mh_LoadFontStyle(QFont & orc_FontStyle, const stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveFontStyle(const QFont & orc_FontStyle, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_PixmapToString(const QPixmap & orc_Pixmap, QString & orc_String);
   static void mh_StringToPixmap(const QString & orc_String, QPixmap & orc_Pixmap);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
