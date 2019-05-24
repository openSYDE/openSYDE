//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base item filer (implementation)

   Base item filer

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPixmap>
#include <QBuffer>
#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_PuiBsElementsFiler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiBsElementsFiler::C_PuiBsElementsFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load text elements

   \param[in,out] orc_Elements  Base data elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the any element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiBsElementsFiler::h_LoadBaseElements(C_PuiBsElements & orc_Elements, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return = C_NO_ERR;

   //Text elements
   if (orc_XMLParser.SelectNodeChild("text-elements") == "text-elements")
   {
      s32_Return = C_PuiBsElementsFiler::h_LoadTextElements(orc_Elements.c_TextElements, orc_XMLParser);
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   //Boundaries
   if ((orc_XMLParser.SelectNodeChild("boundaries") == "boundaries") && (s32_Return == C_NO_ERR))
   {
      s32_Return = C_PuiBsElementsFiler::h_LoadBoundaries(orc_Elements.c_Boundaries, orc_XMLParser);
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   //Images
   if ((orc_XMLParser.SelectNodeChild("images") == "images") && (s32_Return == C_NO_ERR))
   {
      s32_Return = C_PuiBsElementsFiler::h_LoadImages(orc_Elements.c_Images, orc_XMLParser);
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   //Line arrows
   if ((orc_XMLParser.SelectNodeChild("line-arrows") == "line-arrows") && (s32_Return == C_NO_ERR))
   {
      s32_Return = C_PuiBsElementsFiler::h_LoadLineArrows(orc_Elements.c_LineArrows, orc_XMLParser);
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save boundaries

   \param[in]     orc_Elements  Base data elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to any element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveBaseElements(const C_PuiBsElements & orc_Elements, C_OSCXMLParserBase & orc_XMLParser)
{
   //Text elements
   orc_XMLParser.CreateAndSelectNodeChild("text-elements");
   C_PuiBsElementsFiler::h_SaveTextElements(orc_Elements.c_TextElements, orc_XMLParser);
   orc_XMLParser.SelectNodeParent(); //back up

   //Boundaries
   orc_XMLParser.CreateAndSelectNodeChild("boundaries");
   C_PuiBsElementsFiler::h_SaveBoundaries(orc_Elements.c_Boundaries, orc_XMLParser);
   orc_XMLParser.SelectNodeParent(); //back up

   //Images
   orc_XMLParser.CreateAndSelectNodeChild("images");
   C_PuiBsElementsFiler::h_SaveImages(orc_Elements.c_Images, orc_XMLParser);
   orc_XMLParser.SelectNodeParent(); //back up

   //Line arrows
   orc_XMLParser.CreateAndSelectNodeChild("line-arrows");
   C_PuiBsElementsFiler::h_SaveLineArrows(orc_Elements.c_LineArrows, orc_XMLParser);
   orc_XMLParser.SelectNodeParent(); //back up
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load text elements

   \param[in,out] orc_TextElements Text element data elements
   \param[in,out] orc_XMLParser    XML parser with the "current" element set to the "text-elements" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiBsElementsFiler::h_LoadTextElements(std::vector<C_PuiBsTextElement> & orc_TextElements,
                                                C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_SelectedNode = orc_XMLParser.SelectNodeChild("text-element");

   orc_TextElements.clear();
   if (c_SelectedNode == "text-element")
   {
      do
      {
         C_PuiBsTextElement c_TextElement;
         s32_Retval = C_PuiBsElementsFiler::h_LoadTextElement(&c_TextElement, orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_TextElements.push_back(c_TextElement);
         }

         //Next
         c_SelectedNode = orc_XMLParser.SelectNodeNext("text-element");
      }
      while (c_SelectedNode == "text-element");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "text-elements");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save text elements

   \param[in]     orc_TextElements Text element data elements
   \param[in,out] orc_XMLParser    XML parser with the "current" element set to the "text-elements" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveTextElements(const std::vector<C_PuiBsTextElement> & orc_TextElements,
                                              C_OSCXMLParserBase & orc_XMLParser)
{
   for (uint32 u32_Index = 0U; u32_Index < orc_TextElements.size(); ++u32_Index)
   {
      orc_XMLParser.CreateAndSelectNodeChild("text-element");
      C_PuiBsElementsFiler::h_SaveTextElement(&orc_TextElements[u32_Index], orc_XMLParser);
      tgl_assert(orc_XMLParser.SelectNodeParent() == "text-elements"); //back up
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load boundaries

   \param[in,out] orc_Boundaries Boundary data elements
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "boundaries" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiBsElementsFiler::h_LoadBoundaries(std::vector<C_PuiBsBoundary> & orc_Boundaries,
                                              C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_SelectedNode = orc_XMLParser.SelectNodeChild("boundary");

   orc_Boundaries.clear();
   if (c_SelectedNode == "boundary")
   {
      do
      {
         C_PuiBsBoundary c_Item;
         s32_Retval = C_PuiBsElementsFiler::mh_LoadBoundary(c_Item, orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Boundaries.push_back(c_Item);
         }

         //Next
         c_SelectedNode = orc_XMLParser.SelectNodeNext("boundary");
      }
      while (c_SelectedNode == "boundary");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "boundaries");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save boundaries

   \param[in]     orc_Boundaries Boundary data elements
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "boundaries" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveBoundaries(const std::vector<C_PuiBsBoundary> & orc_Boundaries,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   for (uint32 u32_Index = 0U; u32_Index < orc_Boundaries.size(); ++u32_Index)
   {
      orc_XMLParser.CreateAndSelectNodeChild("boundary");
      C_PuiBsElementsFiler::mh_SaveBoundary(orc_Boundaries[u32_Index], orc_XMLParser);
      tgl_assert(orc_XMLParser.SelectNodeParent() == "boundaries"); //back up
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load images

   \param[in,out] orc_Images    Image data elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "images" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiBsElementsFiler::h_LoadImages(std::vector<C_PuiBsImage> & orc_Images, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_SelectedNode = orc_XMLParser.SelectNodeChild("image");

   orc_Images.clear();
   if (c_SelectedNode == "image")
   {
      do
      {
         C_PuiBsImage c_Item;
         s32_Retval = C_PuiBsElementsFiler::mh_LoadImage(c_Item, orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Images.push_back(c_Item);
         }

         //Next
         c_SelectedNode = orc_XMLParser.SelectNodeNext("image");
      }
      while (c_SelectedNode == "image");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "images");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save images

   \param[in]     orc_Images    Image data elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "images" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveImages(const std::vector<C_PuiBsImage> & orc_Images,
                                        C_OSCXMLParserBase & orc_XMLParser)
{
   for (uint32 u32_Index = 0U; u32_Index < orc_Images.size(); ++u32_Index)
   {
      orc_XMLParser.CreateAndSelectNodeChild("image");
      C_PuiBsElementsFiler::mh_SaveImage(orc_Images[u32_Index], orc_XMLParser);
      tgl_assert(orc_XMLParser.SelectNodeParent() == "images"); //back up
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load line arrows

   \param[in,out] orc_LineArrows Line arrow data elements
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "line-arrows" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiBsElementsFiler::h_LoadLineArrows(std::vector<C_PuiBsLineArrow> & orc_LineArrows,
                                              C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_SelectedNode = orc_XMLParser.SelectNodeChild("line-arrow");

   orc_LineArrows.clear();
   if (c_SelectedNode == "line-arrow")
   {
      do
      {
         C_PuiBsLineArrow c_Item;
         s32_Retval = C_PuiBsElementsFiler::mh_LoadLineArrow(c_Item, orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_LineArrows.push_back(c_Item);
         }

         //Next
         c_SelectedNode = orc_XMLParser.SelectNodeNext("line-arrow");
      }
      while (c_SelectedNode == "line-arrow");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "line-arrows");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save line arrows

   \param[in]     orc_LineArrows Line arrow data elements
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "line-arrows" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveLineArrows(const std::vector<C_PuiBsLineArrow> & orc_LineArrows,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   for (uint32 u32_Index = 0U; u32_Index < orc_LineArrows.size(); ++u32_Index)
   {
      orc_XMLParser.CreateAndSelectNodeChild("line-arrow");
      C_PuiBsElementsFiler::mh_SaveLineArrow(orc_LineArrows[u32_Index], orc_XMLParser);
      tgl_assert(orc_XMLParser.SelectNodeParent() == "line-arrows"); //back up
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load box base attributes

   \param[in,out] orc_BoxBase   Box base data element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "box" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiBsElementsFiler::h_LoadBoxBase(C_PuiBsBox & orc_BoxBase, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("position") == "position")
   {
      orc_BoxBase.c_UIPosition.setX(orc_XMLParser.GetAttributeFloat64("x"));
      orc_BoxBase.c_UIPosition.setY(orc_XMLParser.GetAttributeFloat64("y"));
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XMLParser.SelectNodeParent(); //back up to "box"

   if (orc_XMLParser.SelectNodeChild("size") == "size")
   {
      orc_BoxBase.f64_Width = orc_XMLParser.GetAttributeFloat64("width");
      orc_BoxBase.f64_Height = orc_XMLParser.GetAttributeFloat64("height");
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XMLParser.SelectNodeParent(); //back up to "box"

   // Z Order
   if (orc_XMLParser.SelectNodeChild("zorder") == "zorder")
   {
      orc_BoxBase.f64_ZOrder = orc_XMLParser.GetAttributeFloat64("value");
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XMLParser.SelectNodeParent(); //back up to "box"
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save box base attributes

   \param[in]     orc_BoxBase   Box base data element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "box" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveBoxBase(const C_PuiBsBox & orc_BoxBase,
                                         stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   //Position
   orc_XMLParser.CreateAndSelectNodeChild("position");
   orc_XMLParser.SetAttributeFloat64("x", orc_BoxBase.c_UIPosition.x());
   orc_XMLParser.SetAttributeFloat64("y", orc_BoxBase.c_UIPosition.y());
   orc_XMLParser.SelectNodeParent();

   //Size
   orc_XMLParser.CreateAndSelectNodeChild("size");
   orc_XMLParser.SetAttributeFloat64("width", orc_BoxBase.f64_Width);
   orc_XMLParser.SetAttributeFloat64("height", orc_BoxBase.f64_Height);
   orc_XMLParser.SelectNodeParent();

   //Z Order
   orc_XMLParser.CreateAndSelectNodeChild("zorder");
   orc_XMLParser.SetAttributeFloat64("value", orc_BoxBase.f64_ZOrder);
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load line base attributes

   \param[in,out] orc_LineBase  Line base data element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "line" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiBsElementsFiler::h_LoadLineBase(C_PuiBsLineBase & orc_LineBase, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return = C_NO_ERR;
   C_SCLString c_Node;

   if (orc_XMLParser.SelectNodeChild("color") == "color")
   {
      h_LoadColor(orc_LineBase.c_UIColor, orc_XMLParser);
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   if (orc_XMLParser.SelectNodeNext("width") == "width")
   {
      orc_LineBase.s32_UIWidthPixels = orc_XMLParser.GetAttributeSint32("pixels");
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   // Z Order
   if (orc_XMLParser.SelectNodeNext("zorder") == "zorder")
   {
      orc_LineBase.f64_ZOrder = orc_XMLParser.GetAttributeFloat64("value");
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   if (orc_XMLParser.SelectNodeNext("interaction-points") == "interaction-points")
   {
      //file through children:
      c_Node = orc_XMLParser.SelectNodeChild("interaction-point");

      orc_LineBase.c_UIInteractionPoints.clear();
      if (c_Node == "interaction-point")
      {
         do
         {
            QPointF c_Point(orc_XMLParser.GetAttributeFloat64("x"), orc_XMLParser.GetAttributeFloat64(
                               "y"));
            orc_LineBase.c_UIInteractionPoints.push_back(c_Point);
            c_Node = orc_XMLParser.SelectNodeNext("interaction-point");
         }
         while (c_Node == "interaction-point");
         orc_XMLParser.SelectNodeParent(); //back up
      }
      orc_XMLParser.SelectNodeParent(); //back to line
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save line base attributes

   \param[in]     orc_LineBase  Line base data element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "line" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveLineBase(const C_PuiBsLineBase & orc_LineBase,
                                          stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("color");
   h_SaveColor(orc_LineBase.c_UIColor, orc_XMLParser);

   orc_XMLParser.SelectNodeParent(); //back to "line"
   orc_XMLParser.CreateAndSelectNodeChild("width");
   orc_XMLParser.SetAttributeSint32("pixels", orc_LineBase.s32_UIWidthPixels);

   //Z Order
   orc_XMLParser.SelectNodeParent(); // back to "line"
   orc_XMLParser.CreateAndSelectNodeChild("zorder");
   orc_XMLParser.SetAttributeFloat64("value", orc_LineBase.f64_ZOrder);

   orc_XMLParser.SelectNodeParent(); //back to "line"
   orc_XMLParser.CreateAndSelectNodeChild("interaction-points");

   for (uint32 u32_Index = 0U; u32_Index < orc_LineBase.c_UIInteractionPoints.size(); u32_Index++)
   {
      orc_XMLParser.CreateAndSelectNodeChild("interaction-point");
      orc_XMLParser.SetAttributeFloat64("x", orc_LineBase.c_UIInteractionPoints[u32_Index].x());
      orc_XMLParser.SetAttributeFloat64("y", orc_LineBase.c_UIInteractionPoints[u32_Index].y());
      orc_XMLParser.SelectNodeParent(); //back to "interaction-points"
   }

   orc_XMLParser.SelectNodeParent(); //back to "line"
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load text element attributes

   \param[in,out] opc_TextElement Text element data element
   \param[in,out] orc_XMLParser   XML parser with the "current" element set to the "text-element" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_PuiBsElementsFiler::h_LoadTextElement(C_PuiBsTextElement * const opc_TextElement,
                                                          stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return;

   if (opc_TextElement != NULL)
   {
      if (orc_XMLParser.SelectNodeChild("box") == "box")
      {
         s32_Return = h_LoadBoxBase(*opc_TextElement, orc_XMLParser);
      }
      else
      {
         s32_Return = C_CONFIG;
      }
      orc_XMLParser.SelectNodeParent(); //back up to "text-element"

      if (orc_XMLParser.SelectNodeChild("font-style") == "font-style")
      {
         mh_LoadFontStyle(opc_TextElement->c_UIFontStyle, orc_XMLParser);
      }
      else
      {
         s32_Return = C_CONFIG;
      }
      orc_XMLParser.SelectNodeParent(); //back up to "text-element"

      if (orc_XMLParser.SelectNodeChild("font-color") == "font-color")
      {
         if (orc_XMLParser.SelectNodeChild("bright") == "bright")
         {
            h_LoadColor(opc_TextElement->c_UIFontColorBright, orc_XMLParser);
            tgl_assert(orc_XMLParser.SelectNodeParent() == "font-color");
         }
         else
         {
            s32_Return = C_CONFIG;
         }
         if (orc_XMLParser.SelectNodeChild("dark") == "dark")
         {
            h_LoadColor(opc_TextElement->c_UIFontColorDark, orc_XMLParser);
            tgl_assert(orc_XMLParser.SelectNodeParent() == "font-color");
         }
         else
         {
            s32_Return = C_CONFIG;
         }
      }
      else
      {
         s32_Return = C_CONFIG;
      }
      orc_XMLParser.SelectNodeParent(); //back up to "text-element"

      if (orc_XMLParser.SelectNodeChild("text-content") == "text-content")
      {
         opc_TextElement->c_UIText = orc_XMLParser.GetNodeContent().c_str();
      }
      else
      {
         s32_Return = C_CONFIG;
      }
      orc_XMLParser.SelectNodeParent(); //back up to "text-element"
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save text element

   \param[in]     opc_TextElement Text element data element
   \param[in,out] orc_XMLParser   XML parser with the "current" element set to the "text-element" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveTextElement(const C_PuiBsTextElement * const opc_TextElement,
                                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   if (opc_TextElement != NULL)
   {
      orc_XMLParser.CreateAndSelectNodeChild("box");
      h_SaveBoxBase(*opc_TextElement, orc_XMLParser);
      orc_XMLParser.SelectNodeParent(); //back to "text-element"
      orc_XMLParser.CreateAndSelectNodeChild("font-style");
      mh_SaveFontStyle(opc_TextElement->c_UIFontStyle, orc_XMLParser);
      orc_XMLParser.SelectNodeParent(); //back to "text-element"
      orc_XMLParser.CreateAndSelectNodeChild("font-color");
      orc_XMLParser.CreateAndSelectNodeChild("bright");
      h_SaveColor(opc_TextElement->c_UIFontColorBright, orc_XMLParser);
      orc_XMLParser.SelectNodeParent(); //back to "font-color"
      orc_XMLParser.CreateAndSelectNodeChild("dark");
      h_SaveColor(opc_TextElement->c_UIFontColorDark, orc_XMLParser);
      orc_XMLParser.SelectNodeParent(); //back to "font-color"
      orc_XMLParser.SelectNodeParent(); //back to "text-element"
      orc_XMLParser.CreateAndSelectNodeChild("text-content");
      orc_XMLParser.SetNodeContent(opc_TextElement->c_UIText.toStdString().c_str());
      orc_XMLParser.SelectNodeParent(); //back to "text-element"
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load color attributes

   \param[in,out] orc_Color     Color data element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to a color element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_LoadColor(QColor & orc_Color, const stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   orc_Color = QColor(orc_XMLParser.GetAttributeSint32("r"),
                      orc_XMLParser.GetAttributeSint32("g"),
                      orc_XMLParser.GetAttributeSint32("b"),
                      orc_XMLParser.GetAttributeSint32("a"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save color attributes

   \param[in]     orc_Color     Color data element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to a color element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveColor(const QColor & orc_Color, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("r", orc_Color.red());
   orc_XMLParser.SetAttributeUint32("g", orc_Color.green());
   orc_XMLParser.SetAttributeUint32("b", orc_Color.blue());
   orc_XMLParser.SetAttributeUint32("a", orc_Color.alpha());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load boundary attributes

   \param[in,out] orc_Boundary      Boundary data element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "boundary" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_PuiBsElementsFiler::mh_LoadBoundary(C_PuiBsBoundary & orc_Boundary,
                                                        stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return;

   if (orc_XMLParser.SelectNodeChild("box") == "box")
   {
      s32_Return = h_LoadBoxBase(orc_Boundary, orc_XMLParser);
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XMLParser.SelectNodeParent(); //back up to "boundary"

   if (orc_XMLParser.SelectNodeChild("border-color") == "border-color")
   {
      if (orc_XMLParser.SelectNodeChild("bright") == "bright")
      {
         h_LoadColor(orc_Boundary.c_UIBorderColorBright, orc_XMLParser);
         tgl_assert(orc_XMLParser.SelectNodeParent() == "border-color");
      }
      else
      {
         s32_Return = C_CONFIG;
      }
      if (orc_XMLParser.SelectNodeChild("dark") == "dark")
      {
         h_LoadColor(orc_Boundary.c_UIBorderColorDark, orc_XMLParser);
         tgl_assert(orc_XMLParser.SelectNodeParent() == "border-color");
      }
      else
      {
         s32_Return = C_CONFIG;
      }
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XMLParser.SelectNodeParent(); //back up to "boundary"

   if (orc_XMLParser.SelectNodeChild("border-width") == "border-width")
   {
      orc_Boundary.s32_UIBorderWidth = orc_XMLParser.GetAttributeSint32("pixels");
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XMLParser.SelectNodeParent(); //back up to "boundary"

   if (orc_XMLParser.SelectNodeChild("background-color") == "background-color")
   {
      if (orc_XMLParser.SelectNodeChild("bright") == "bright")
      {
         h_LoadColor(orc_Boundary.c_UIBackgroundColorBright, orc_XMLParser);
         tgl_assert(orc_XMLParser.SelectNodeParent() == "background-color");
      }
      else
      {
         s32_Return = C_CONFIG;
      }
      if (orc_XMLParser.SelectNodeChild("dark") == "dark")
      {
         h_LoadColor(orc_Boundary.c_UIBackgroundColorDark, orc_XMLParser);
         tgl_assert(orc_XMLParser.SelectNodeParent() == "background-color");
      }
      else
      {
         s32_Return = C_CONFIG;
      }
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XMLParser.SelectNodeParent(); //back up to "boundary"
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save boundary

   \param[in]     orc_Boundary  Boundary data element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "boundary" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::mh_SaveBoundary(const C_PuiBsBoundary & orc_Boundary,
                                           stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("box");
   h_SaveBoxBase(orc_Boundary, orc_XMLParser);
   orc_XMLParser.SelectNodeParent(); //back to "boundary"
   orc_XMLParser.CreateAndSelectNodeChild("border-color");
   orc_XMLParser.CreateAndSelectNodeChild("bright");
   h_SaveColor(orc_Boundary.c_UIBorderColorBright, orc_XMLParser);
   orc_XMLParser.SelectNodeParent(); //back to "border-color"
   orc_XMLParser.CreateAndSelectNodeChild("dark");
   h_SaveColor(orc_Boundary.c_UIBorderColorDark, orc_XMLParser);
   orc_XMLParser.SelectNodeParent(); //back to "border-color"
   orc_XMLParser.SelectNodeParent(); //back to "boundary"
   orc_XMLParser.CreateAndSelectNodeChild("border-width");
   orc_XMLParser.SetAttributeSint32("pixels", orc_Boundary.s32_UIBorderWidth);
   orc_XMLParser.SelectNodeParent(); //back to "boundary"
   orc_XMLParser.CreateAndSelectNodeChild("background-color");
   orc_XMLParser.CreateAndSelectNodeChild("bright");
   h_SaveColor(orc_Boundary.c_UIBackgroundColorBright, orc_XMLParser);
   orc_XMLParser.SelectNodeParent(); //back to "background-color"
   orc_XMLParser.CreateAndSelectNodeChild("dark");
   h_SaveColor(orc_Boundary.c_UIBackgroundColorDark, orc_XMLParser);
   orc_XMLParser.SelectNodeParent(); //back to "background-color"
   orc_XMLParser.SelectNodeParent(); //back to "boundary"
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load image attributes

   \param[in,out] orc_Image     Image data element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "image" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_PuiBsElementsFiler::mh_LoadImage(C_PuiBsImage & orc_Image,
                                                     stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return;

   if (orc_XMLParser.SelectNodeChild("box") == "box")
   {
      s32_Return = h_LoadBoxBase(orc_Image, orc_XMLParser);
      orc_XMLParser.SelectNodeParent(); //back up to "image"
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   if (orc_XMLParser.SelectNodeChild("image-data") == "image-data")
   {
      const QString c_ImageBytes = orc_XMLParser.GetNodeContent().c_str();
      mh_StringToPixmap(c_ImageBytes, orc_Image.c_UIImagePixmap);
      orc_XMLParser.SelectNodeParent(); //back up to "image"
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save image

   \param[in]     orc_Image     Image data element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "image" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::mh_SaveImage(const C_PuiBsImage & orc_Image,
                                        stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   QString c_ImageBytes;

   orc_XMLParser.CreateAndSelectNodeChild("box");
   h_SaveBoxBase(orc_Image, orc_XMLParser);
   orc_XMLParser.SelectNodeParent(); //back to "image"
   orc_XMLParser.CreateAndSelectNodeChild("image-data");
   mh_PixmapToString(orc_Image.c_UIImagePixmap, c_ImageBytes);
   orc_XMLParser.SetNodeContent(c_ImageBytes.toStdString().c_str());
   orc_XMLParser.SelectNodeParent(); //back to "image"
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load line arrow attributes

   \param[in,out] orc_LineArrow Line arrow data element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "line-arrow" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_PuiBsElementsFiler::mh_LoadLineArrow(C_PuiBsLineArrow & orc_LineArrow,
                                                         stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return;

   if (orc_XMLParser.SelectNodeChild("line") == "line")
   {
      s32_Return = h_LoadLineBase(orc_LineArrow, orc_XMLParser);
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XMLParser.SelectNodeParent(); //back to line-arrow
   if (orc_XMLParser.SelectNodeChild("start-arrow-head-type") == "start-arrow-head-type")
   {
      orc_LineArrow.e_StartArrowHeadType = C_PuiBsLineArrow::ArrowHeadTypeFromString(orc_XMLParser.GetNodeContent());
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XMLParser.SelectNodeParent(); //back to line-arrow
   if (orc_XMLParser.SelectNodeChild("end-arrow-head-type") == "end-arrow-head-type")
   {
      orc_LineArrow.e_EndArrowHeadType = C_PuiBsLineArrow::ArrowHeadTypeFromString(orc_XMLParser.GetNodeContent());
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XMLParser.SelectNodeParent(); //back to line-arrow
   if (orc_XMLParser.SelectNodeChild("line-type") == "line-type")
   {
      orc_LineArrow.e_LineType = C_PuiBsLineArrow::LineTypeFromString(orc_XMLParser.GetNodeContent());
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XMLParser.SelectNodeParent(); //back to line-arrow
   if (orc_XMLParser.SelectNodeChild("dark-color") == "dark-color")
   {
      h_LoadColor(orc_LineArrow.c_UIColorDark, orc_XMLParser);
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XMLParser.SelectNodeParent(); //back to line-arrow
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save line arrow

   \param[in]     orc_LineArrow Line arrow data element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "line-arrow" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::mh_SaveLineArrow(const C_PuiBsLineArrow & orc_LineArrow,
                                            stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("line");
   h_SaveLineBase(orc_LineArrow, orc_XMLParser);
   orc_XMLParser.SelectNodeParent(); //back to "image"
   orc_XMLParser.CreateAndSelectNodeChild("start-arrow-head-type");
   orc_XMLParser.SetNodeContent(C_PuiBsLineArrow::ArrowHeadTypeToString(orc_LineArrow.e_StartArrowHeadType));
   orc_XMLParser.SelectNodeParent(); //back to "image"
   orc_XMLParser.CreateAndSelectNodeChild("end-arrow-head-type");
   orc_XMLParser.SetNodeContent(C_PuiBsLineArrow::ArrowHeadTypeToString(orc_LineArrow.e_EndArrowHeadType));
   orc_XMLParser.SelectNodeParent(); //back to "image"
   orc_XMLParser.CreateAndSelectNodeChild("line-type");
   orc_XMLParser.SetNodeContent(C_PuiBsLineArrow::LineTypeToString(orc_LineArrow.e_LineType));
   orc_XMLParser.SelectNodeParent(); //back to "image"
   orc_XMLParser.CreateAndSelectNodeChild("dark-color");
   h_SaveColor(orc_LineArrow.c_UIColorDark, orc_XMLParser);
   orc_XMLParser.SelectNodeParent(); //back to "image"
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load font style attributes

   \param[in,out] orc_FontStyle Font style data element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to a color element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::mh_LoadFontStyle(QFont & orc_FontStyle,
                                            const stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   orc_FontStyle.fromString(orc_XMLParser.GetNodeContent().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save font style attributes

   \param[in]     orc_FontStyle Font style data element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to a color element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::mh_SaveFontStyle(const QFont & orc_FontStyle,
                                            stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetNodeContent(orc_FontStyle.toString().toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert pixmap to string

   \param[in]     orc_Pixmap Pixmap
   \param[out]    orc_String Byte string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::mh_PixmapToString(const QPixmap & orc_Pixmap, QString & orc_String)
{
   QByteArray c_PixmapAsByteArray;
   //lint -e{10,40,522}  Qt feature
   QBuffer c_PixmapBuffer(&c_PixmapAsByteArray);

   //lint -e{10,40,746,1013,1055}  Qt feature
   c_PixmapBuffer.open(QIODevice::WriteOnly);
   //lint -e{40,1025,1703}  Qt feature
   orc_Pixmap.save(&c_PixmapBuffer, "PNG");
   //lint -e{10,40,746,1013,1055}  Qt feature
   c_PixmapBuffer.close();

   orc_String = QString::fromUtf8(c_PixmapAsByteArray.toBase64());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert string to pixmap

   \param[in]     orc_String Byte string
   \param[out]    orc_Pixmap Pixmap
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::mh_StringToPixmap(const QString & orc_String, QPixmap & orc_Pixmap)
{
   orc_Pixmap.loadFromData(QByteArray::fromBase64(orc_String.toUtf8()), "PNG");
}
