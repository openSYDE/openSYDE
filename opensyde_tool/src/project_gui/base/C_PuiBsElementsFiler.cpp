//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base item filer (implementation)

   Base item filer

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPixmap>
#include <QBuffer>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_PuiBsElementsFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

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
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the any element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiBsElementsFiler::h_LoadBaseElements(C_PuiBsElements & orc_Elements, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Return = C_NO_ERR;

   //Text elements
   if (orc_XmlParser.SelectNodeChild("text-elements") == "text-elements")
   {
      s32_Return = C_PuiBsElementsFiler::h_LoadTextElements(orc_Elements.c_TextElements, orc_XmlParser);
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   //Boundaries
   if ((orc_XmlParser.SelectNodeChild("boundaries") == "boundaries") && (s32_Return == C_NO_ERR))
   {
      s32_Return = C_PuiBsElementsFiler::h_LoadBoundaries(orc_Elements.c_Boundaries, orc_XmlParser);
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   //Images
   if ((orc_XmlParser.SelectNodeChild("images") == "images") && (s32_Return == C_NO_ERR))
   {
      s32_Return = C_PuiBsElementsFiler::h_LoadImages(orc_Elements.c_Images, orc_XmlParser);
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   //Line arrows
   if ((orc_XmlParser.SelectNodeChild("line-arrows") == "line-arrows") && (s32_Return == C_NO_ERR))
   {
      s32_Return = C_PuiBsElementsFiler::h_LoadLineArrows(orc_Elements.c_LineArrows, orc_XmlParser);
      orc_XmlParser.SelectNodeParent();
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
   \param[in,out] orc_XmlParser XML parser with the "current" element set to any element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveBaseElements(const C_PuiBsElements & orc_Elements, C_OscXmlParserBase & orc_XmlParser)
{
   //Text elements
   orc_XmlParser.CreateAndSelectNodeChild("text-elements");
   C_PuiBsElementsFiler::h_SaveTextElements(orc_Elements.c_TextElements, orc_XmlParser);
   orc_XmlParser.SelectNodeParent(); //back up

   //Boundaries
   orc_XmlParser.CreateAndSelectNodeChild("boundaries");
   C_PuiBsElementsFiler::h_SaveBoundaries(orc_Elements.c_Boundaries, orc_XmlParser);
   orc_XmlParser.SelectNodeParent(); //back up

   //Images
   orc_XmlParser.CreateAndSelectNodeChild("images");
   C_PuiBsElementsFiler::h_SaveImages(orc_Elements.c_Images, orc_XmlParser);
   orc_XmlParser.SelectNodeParent(); //back up

   //Line arrows
   orc_XmlParser.CreateAndSelectNodeChild("line-arrows");
   C_PuiBsElementsFiler::h_SaveLineArrows(orc_Elements.c_LineArrows, orc_XmlParser);
   orc_XmlParser.SelectNodeParent(); //back up
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load text elements

   \param[in,out] orc_TextElements Text element data elements
   \param[in,out] orc_XmlParser    XML parser with the "current" element set to the "text-elements" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiBsElementsFiler::h_LoadTextElements(std::vector<C_PuiBsTextElement> & orc_TextElements,
                                                 C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_SelectedNode = orc_XmlParser.SelectNodeChild("text-element");

   orc_TextElements.clear();
   if (c_SelectedNode == "text-element")
   {
      do
      {
         C_PuiBsTextElement c_TextElement;
         s32_Retval = C_PuiBsElementsFiler::h_LoadTextElement(&c_TextElement, orc_XmlParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_TextElements.push_back(c_TextElement);
         }

         //Next
         c_SelectedNode = orc_XmlParser.SelectNodeNext("text-element");
      }
      while (c_SelectedNode == "text-element");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "text-elements");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save text elements

   \param[in]     orc_TextElements Text element data elements
   \param[in,out] orc_XmlParser    XML parser with the "current" element set to the "text-elements" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveTextElements(const std::vector<C_PuiBsTextElement> & orc_TextElements,
                                              C_OscXmlParserBase & orc_XmlParser)
{
   for (uint32_t u32_Index = 0U; u32_Index < orc_TextElements.size(); ++u32_Index)
   {
      orc_XmlParser.CreateAndSelectNodeChild("text-element");
      C_PuiBsElementsFiler::h_SaveTextElement(&orc_TextElements[u32_Index], orc_XmlParser);
      tgl_assert(orc_XmlParser.SelectNodeParent() == "text-elements"); //back up
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load boundaries

   \param[in,out] orc_Boundaries Boundary data elements
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "boundaries" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiBsElementsFiler::h_LoadBoundaries(std::vector<C_PuiBsBoundary> & orc_Boundaries,
                                               C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_SelectedNode = orc_XmlParser.SelectNodeChild("boundary");

   orc_Boundaries.clear();
   if (c_SelectedNode == "boundary")
   {
      do
      {
         C_PuiBsBoundary c_Item;
         s32_Retval = C_PuiBsElementsFiler::mh_LoadBoundary(c_Item, orc_XmlParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Boundaries.push_back(c_Item);
         }

         //Next
         c_SelectedNode = orc_XmlParser.SelectNodeNext("boundary");
      }
      while (c_SelectedNode == "boundary");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "boundaries");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save boundaries

   \param[in]     orc_Boundaries Boundary data elements
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "boundaries" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveBoundaries(const std::vector<C_PuiBsBoundary> & orc_Boundaries,
                                            C_OscXmlParserBase & orc_XmlParser)
{
   for (uint32_t u32_Index = 0U; u32_Index < orc_Boundaries.size(); ++u32_Index)
   {
      orc_XmlParser.CreateAndSelectNodeChild("boundary");
      C_PuiBsElementsFiler::mh_SaveBoundary(orc_Boundaries[u32_Index], orc_XmlParser);
      tgl_assert(orc_XmlParser.SelectNodeParent() == "boundaries"); //back up
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load images

   \param[in,out] orc_Images    Image data elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "images" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiBsElementsFiler::h_LoadImages(std::vector<C_PuiBsImage> & orc_Images, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_SelectedNode = orc_XmlParser.SelectNodeChild("image");

   orc_Images.clear();
   if (c_SelectedNode == "image")
   {
      do
      {
         C_PuiBsImage c_Item;
         s32_Retval = C_PuiBsElementsFiler::mh_LoadImage(c_Item, orc_XmlParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Images.push_back(c_Item);
         }

         //Next
         c_SelectedNode = orc_XmlParser.SelectNodeNext("image");
      }
      while (c_SelectedNode == "image");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "images");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save images

   \param[in]     orc_Images    Image data elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "images" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveImages(const std::vector<C_PuiBsImage> & orc_Images,
                                        C_OscXmlParserBase & orc_XmlParser)
{
   for (uint32_t u32_Index = 0U; u32_Index < orc_Images.size(); ++u32_Index)
   {
      orc_XmlParser.CreateAndSelectNodeChild("image");
      C_PuiBsElementsFiler::mh_SaveImage(orc_Images[u32_Index], orc_XmlParser);
      tgl_assert(orc_XmlParser.SelectNodeParent() == "images"); //back up
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load line arrows

   \param[in,out] orc_LineArrows Line arrow data elements
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "line-arrows" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiBsElementsFiler::h_LoadLineArrows(std::vector<C_PuiBsLineArrow> & orc_LineArrows,
                                               C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_SelectedNode = orc_XmlParser.SelectNodeChild("line-arrow");

   orc_LineArrows.clear();
   if (c_SelectedNode == "line-arrow")
   {
      do
      {
         C_PuiBsLineArrow c_Item;
         s32_Retval = C_PuiBsElementsFiler::mh_LoadLineArrow(c_Item, orc_XmlParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_LineArrows.push_back(c_Item);
         }

         //Next
         c_SelectedNode = orc_XmlParser.SelectNodeNext("line-arrow");
      }
      while (c_SelectedNode == "line-arrow");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "line-arrows");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save line arrows

   \param[in]     orc_LineArrows Line arrow data elements
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "line-arrows" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveLineArrows(const std::vector<C_PuiBsLineArrow> & orc_LineArrows,
                                            C_OscXmlParserBase & orc_XmlParser)
{
   for (uint32_t u32_Index = 0U; u32_Index < orc_LineArrows.size(); ++u32_Index)
   {
      orc_XmlParser.CreateAndSelectNodeChild("line-arrow");
      C_PuiBsElementsFiler::mh_SaveLineArrow(orc_LineArrows[u32_Index], orc_XmlParser);
      tgl_assert(orc_XmlParser.SelectNodeParent() == "line-arrows"); //back up
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load box base attributes

   \param[in,out] orc_BoxBase   Box base data element
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "box" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiBsElementsFiler::h_LoadBoxBase(C_PuiBsBox & orc_BoxBase, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Return = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("position") == "position")
   {
      orc_BoxBase.c_UiPosition.setX(orc_XmlParser.GetAttributeFloat64("x"));
      orc_BoxBase.c_UiPosition.setY(orc_XmlParser.GetAttributeFloat64("y"));
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XmlParser.SelectNodeParent(); //back up to "box"

   if (orc_XmlParser.SelectNodeChild("size") == "size")
   {
      orc_BoxBase.f64_Width = orc_XmlParser.GetAttributeFloat64("width");
      orc_BoxBase.f64_Height = orc_XmlParser.GetAttributeFloat64("height");
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XmlParser.SelectNodeParent(); //back up to "box"

   // Z Order
   if (orc_XmlParser.SelectNodeChild("zorder") == "zorder")
   {
      orc_BoxBase.f64_ZetOrder = orc_XmlParser.GetAttributeFloat64("value");
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XmlParser.SelectNodeParent(); //back up to "box"
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save box base attributes

   \param[in]     orc_BoxBase   Box base data element
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "box" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveBoxBase(const C_PuiBsBox & orc_BoxBase,
                                         stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   //Position
   orc_XmlParser.CreateAndSelectNodeChild("position");
   orc_XmlParser.SetAttributeFloat64("x", orc_BoxBase.c_UiPosition.x());
   orc_XmlParser.SetAttributeFloat64("y", orc_BoxBase.c_UiPosition.y());
   orc_XmlParser.SelectNodeParent();

   //Size
   orc_XmlParser.CreateAndSelectNodeChild("size");
   orc_XmlParser.SetAttributeFloat64("width", orc_BoxBase.f64_Width);
   orc_XmlParser.SetAttributeFloat64("height", orc_BoxBase.f64_Height);
   orc_XmlParser.SelectNodeParent();

   //Z Order
   orc_XmlParser.CreateAndSelectNodeChild("zorder");
   orc_XmlParser.SetAttributeFloat64("value", orc_BoxBase.f64_ZetOrder);
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load line base attributes

   \param[in,out] orc_LineBase  Line base data element
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "line" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiBsElementsFiler::h_LoadLineBase(C_PuiBsLineBase & orc_LineBase, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Return = C_NO_ERR;
   C_SclString c_Node;

   if (orc_XmlParser.SelectNodeChild("color") == "color")
   {
      h_LoadColor(orc_LineBase.c_UiColor, orc_XmlParser);
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   if (orc_XmlParser.SelectNodeNext("width") == "width")
   {
      orc_LineBase.s32_UiWidthPixels = orc_XmlParser.GetAttributeSint32("pixels");
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   // Z Order
   if (orc_XmlParser.SelectNodeNext("zorder") == "zorder")
   {
      orc_LineBase.f64_ZetOrder = orc_XmlParser.GetAttributeFloat64("value");
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   if (orc_XmlParser.SelectNodeNext("interaction-points") == "interaction-points")
   {
      //file through children:
      c_Node = orc_XmlParser.SelectNodeChild("interaction-point");

      orc_LineBase.c_UiInteractionPoints.clear();
      if (c_Node == "interaction-point")
      {
         do
         {
            const QPointF c_Point(orc_XmlParser.GetAttributeFloat64("x"), orc_XmlParser.GetAttributeFloat64(
                                     "y"));
            orc_LineBase.c_UiInteractionPoints.push_back(c_Point);
            c_Node = orc_XmlParser.SelectNodeNext("interaction-point");
         }
         while (c_Node == "interaction-point");
         orc_XmlParser.SelectNodeParent(); //back up
      }
      orc_XmlParser.SelectNodeParent(); //back to line
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
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "line" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveLineBase(const C_PuiBsLineBase & orc_LineBase,
                                          stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("color");
   h_SaveColor(orc_LineBase.c_UiColor, orc_XmlParser);

   orc_XmlParser.SelectNodeParent(); //back to "line"
   orc_XmlParser.CreateAndSelectNodeChild("width");
   orc_XmlParser.SetAttributeSint32("pixels", orc_LineBase.s32_UiWidthPixels);

   //Z Order
   orc_XmlParser.SelectNodeParent(); // back to "line"
   orc_XmlParser.CreateAndSelectNodeChild("zorder");
   orc_XmlParser.SetAttributeFloat64("value", orc_LineBase.f64_ZetOrder);

   orc_XmlParser.SelectNodeParent(); //back to "line"
   orc_XmlParser.CreateAndSelectNodeChild("interaction-points");

   for (uint32_t u32_Index = 0U; u32_Index < orc_LineBase.c_UiInteractionPoints.size(); u32_Index++)
   {
      orc_XmlParser.CreateAndSelectNodeChild("interaction-point");
      orc_XmlParser.SetAttributeFloat64("x", orc_LineBase.c_UiInteractionPoints[u32_Index].x());
      orc_XmlParser.SetAttributeFloat64("y", orc_LineBase.c_UiInteractionPoints[u32_Index].y());
      orc_XmlParser.SelectNodeParent(); //back to "interaction-points"
   }

   orc_XmlParser.SelectNodeParent(); //back to "line"
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load text element attributes

   \param[in,out] opc_TextElement Text element data element
   \param[in,out] orc_XmlParser   XML parser with the "current" element set to the "text-element" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiBsElementsFiler::h_LoadTextElement(C_PuiBsTextElement * const opc_TextElement,
                                                stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Return;

   if (opc_TextElement != NULL)
   {
      if (orc_XmlParser.SelectNodeChild("box") == "box")
      {
         s32_Return = h_LoadBoxBase(*opc_TextElement, orc_XmlParser);
      }
      else
      {
         s32_Return = C_CONFIG;
      }
      orc_XmlParser.SelectNodeParent(); //back up to "text-element"

      if (orc_XmlParser.SelectNodeChild("font-style") == "font-style")
      {
         mh_LoadFontStyle(opc_TextElement->c_UiFontStyle, orc_XmlParser);
      }
      else
      {
         s32_Return = C_CONFIG;
      }
      orc_XmlParser.SelectNodeParent(); //back up to "text-element"

      if (orc_XmlParser.SelectNodeChild("font-color") == "font-color")
      {
         if (orc_XmlParser.SelectNodeChild("bright") == "bright")
         {
            h_LoadColor(opc_TextElement->c_UiFontColorBright, orc_XmlParser);
            tgl_assert(orc_XmlParser.SelectNodeParent() == "font-color");
         }
         else
         {
            s32_Return = C_CONFIG;
         }
         if (orc_XmlParser.SelectNodeChild("dark") == "dark")
         {
            h_LoadColor(opc_TextElement->c_UiFontColorDark, orc_XmlParser);
            tgl_assert(orc_XmlParser.SelectNodeParent() == "font-color");
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
      orc_XmlParser.SelectNodeParent(); //back up to "text-element"

      if (orc_XmlParser.SelectNodeChild("text-content") == "text-content")
      {
         opc_TextElement->c_UiText = orc_XmlParser.GetNodeContent().c_str();
      }
      else
      {
         s32_Return = C_CONFIG;
      }
      orc_XmlParser.SelectNodeParent(); //back up to "text-element"
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
   \param[in,out] orc_XmlParser   XML parser with the "current" element set to the "text-element" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveTextElement(const C_PuiBsTextElement * const opc_TextElement,
                                             stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   if (opc_TextElement != NULL)
   {
      orc_XmlParser.CreateAndSelectNodeChild("box");
      h_SaveBoxBase(*opc_TextElement, orc_XmlParser);
      orc_XmlParser.SelectNodeParent(); //back to "text-element"
      orc_XmlParser.CreateAndSelectNodeChild("font-style");
      mh_SaveFontStyle(opc_TextElement->c_UiFontStyle, orc_XmlParser);
      orc_XmlParser.SelectNodeParent(); //back to "text-element"
      orc_XmlParser.CreateAndSelectNodeChild("font-color");
      orc_XmlParser.CreateAndSelectNodeChild("bright");
      h_SaveColor(opc_TextElement->c_UiFontColorBright, orc_XmlParser);
      orc_XmlParser.SelectNodeParent(); //back to "font-color"
      orc_XmlParser.CreateAndSelectNodeChild("dark");
      h_SaveColor(opc_TextElement->c_UiFontColorDark, orc_XmlParser);
      orc_XmlParser.SelectNodeParent(); //back to "font-color"
      orc_XmlParser.SelectNodeParent(); //back to "text-element"
      orc_XmlParser.CreateAndSelectNodeChild("text-content");
      orc_XmlParser.SetNodeContent(opc_TextElement->c_UiText.toStdString().c_str());
      orc_XmlParser.SelectNodeParent(); //back to "text-element"
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load color attributes

   \param[in,out] orc_Color     Color data element
   \param[in,out] orc_XmlParser XML parser with the "current" element set to a color element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_LoadColor(QColor & orc_Color, const stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   orc_Color = QColor(orc_XmlParser.GetAttributeSint32("r"),
                      orc_XmlParser.GetAttributeSint32("g"),
                      orc_XmlParser.GetAttributeSint32("b"),
                      orc_XmlParser.GetAttributeSint32("a"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save color attributes

   \param[in]     orc_Color     Color data element
   \param[in,out] orc_XmlParser XML parser with the "current" element set to a color element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::h_SaveColor(const QColor & orc_Color, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("r", orc_Color.red());
   orc_XmlParser.SetAttributeUint32("g", orc_Color.green());
   orc_XmlParser.SetAttributeUint32("b", orc_Color.blue());
   orc_XmlParser.SetAttributeUint32("a", orc_Color.alpha());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load boundary attributes

   \param[in,out] orc_Boundary      Boundary data element
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "boundary" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiBsElementsFiler::mh_LoadBoundary(C_PuiBsBoundary & orc_Boundary,
                                              stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Return;

   if (orc_XmlParser.SelectNodeChild("box") == "box")
   {
      s32_Return = h_LoadBoxBase(orc_Boundary, orc_XmlParser);
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XmlParser.SelectNodeParent(); //back up to "boundary"

   if (orc_XmlParser.SelectNodeChild("border-color") == "border-color")
   {
      if (orc_XmlParser.SelectNodeChild("bright") == "bright")
      {
         h_LoadColor(orc_Boundary.c_UiBorderColorBright, orc_XmlParser);
         tgl_assert(orc_XmlParser.SelectNodeParent() == "border-color");
      }
      else
      {
         s32_Return = C_CONFIG;
      }
      if (orc_XmlParser.SelectNodeChild("dark") == "dark")
      {
         h_LoadColor(orc_Boundary.c_UiBorderColorDark, orc_XmlParser);
         tgl_assert(orc_XmlParser.SelectNodeParent() == "border-color");
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
   orc_XmlParser.SelectNodeParent(); //back up to "boundary"

   if (orc_XmlParser.SelectNodeChild("border-width") == "border-width")
   {
      orc_Boundary.s32_UiBorderWidth = orc_XmlParser.GetAttributeSint32("pixels");
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XmlParser.SelectNodeParent(); //back up to "boundary"

   if (orc_XmlParser.SelectNodeChild("background-color") == "background-color")
   {
      if (orc_XmlParser.SelectNodeChild("bright") == "bright")
      {
         h_LoadColor(orc_Boundary.c_UiBackgroundColorBright, orc_XmlParser);
         tgl_assert(orc_XmlParser.SelectNodeParent() == "background-color");
      }
      else
      {
         s32_Return = C_CONFIG;
      }
      if (orc_XmlParser.SelectNodeChild("dark") == "dark")
      {
         h_LoadColor(orc_Boundary.c_UiBackgroundColorDark, orc_XmlParser);
         tgl_assert(orc_XmlParser.SelectNodeParent() == "background-color");
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
   orc_XmlParser.SelectNodeParent(); //back up to "boundary"
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save boundary

   \param[in]     orc_Boundary  Boundary data element
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "boundary" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::mh_SaveBoundary(const C_PuiBsBoundary & orc_Boundary,
                                           stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("box");
   h_SaveBoxBase(orc_Boundary, orc_XmlParser);
   orc_XmlParser.SelectNodeParent(); //back to "boundary"
   orc_XmlParser.CreateAndSelectNodeChild("border-color");
   orc_XmlParser.CreateAndSelectNodeChild("bright");
   h_SaveColor(orc_Boundary.c_UiBorderColorBright, orc_XmlParser);
   orc_XmlParser.SelectNodeParent(); //back to "border-color"
   orc_XmlParser.CreateAndSelectNodeChild("dark");
   h_SaveColor(orc_Boundary.c_UiBorderColorDark, orc_XmlParser);
   orc_XmlParser.SelectNodeParent(); //back to "border-color"
   orc_XmlParser.SelectNodeParent(); //back to "boundary"
   orc_XmlParser.CreateAndSelectNodeChild("border-width");
   orc_XmlParser.SetAttributeSint32("pixels", orc_Boundary.s32_UiBorderWidth);
   orc_XmlParser.SelectNodeParent(); //back to "boundary"
   orc_XmlParser.CreateAndSelectNodeChild("background-color");
   orc_XmlParser.CreateAndSelectNodeChild("bright");
   h_SaveColor(orc_Boundary.c_UiBackgroundColorBright, orc_XmlParser);
   orc_XmlParser.SelectNodeParent(); //back to "background-color"
   orc_XmlParser.CreateAndSelectNodeChild("dark");
   h_SaveColor(orc_Boundary.c_UiBackgroundColorDark, orc_XmlParser);
   orc_XmlParser.SelectNodeParent(); //back to "background-color"
   orc_XmlParser.SelectNodeParent(); //back to "boundary"
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load image attributes

   \param[in,out]  orc_Image        Image data element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "image" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiBsElementsFiler::mh_LoadImage(C_PuiBsImage & orc_Image,
                                           stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Return;

   if (orc_XmlParser.SelectNodeChild("box") == "box")
   {
      s32_Return = h_LoadBoxBase(orc_Image, orc_XmlParser);
      orc_XmlParser.SelectNodeParent(); //back up to "image"
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   // first load format, then the image, to know the format on image load
   if (orc_XmlParser.SelectNodeChild("image-format") == "image-format")
   {
      orc_Image.c_UiImageFormat = orc_XmlParser.GetNodeContent().c_str();
      orc_XmlParser.SelectNodeParent(); //back up to "image"
   }
   else
   {
      //Optional
      orc_Image.c_UiImageFormat = "png";
   }

   if (orc_XmlParser.SelectNodeChild("image-data") == "image-data")
   {
      const QString c_ImageBytes = orc_XmlParser.GetNodeContent().c_str();
      mh_StringToPixmap(c_ImageBytes, orc_Image.c_UiImagePixmap, orc_Image.c_UiImageFormat);
      orc_XmlParser.SelectNodeParent(); //back up to "image"
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save image

   \param[in]      orc_Image        Image data element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "image" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::mh_SaveImage(const C_PuiBsImage & orc_Image,
                                        stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   QString c_ImageBytes;

   orc_XmlParser.CreateAndSelectNodeChild("box");
   h_SaveBoxBase(orc_Image, orc_XmlParser);
   orc_XmlParser.SelectNodeParent(); //back to "image"
   orc_XmlParser.CreateAndSelectNodeChild("image-data");
   mh_PixmapToString(orc_Image.c_UiImagePixmap, orc_Image.c_UiImageFormat, c_ImageBytes);
   orc_XmlParser.SetNodeContent(c_ImageBytes.toStdString().c_str());
   orc_XmlParser.SelectNodeParent(); //back to "image"
   orc_XmlParser.CreateAndSelectNodeChild("image-format");
   orc_XmlParser.SetNodeContent(orc_Image.c_UiImageFormat.toStdString().c_str());
   orc_XmlParser.SelectNodeParent(); //back to "image"
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load line arrow attributes

   \param[in,out]  orc_LineArrow    Line arrow data element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "line-arrow" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiBsElementsFiler::mh_LoadLineArrow(C_PuiBsLineArrow & orc_LineArrow,
                                               stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Return;

   if (orc_XmlParser.SelectNodeChild("line") == "line")
   {
      s32_Return = h_LoadLineBase(orc_LineArrow, orc_XmlParser);
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XmlParser.SelectNodeParent(); //back to line-arrow
   if (orc_XmlParser.SelectNodeChild("start-arrow-head-type") == "start-arrow-head-type")
   {
      orc_LineArrow.e_StartArrowHeadType = C_PuiBsLineArrow::h_ArrowHeadTypeFromString(orc_XmlParser.GetNodeContent());
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XmlParser.SelectNodeParent(); //back to line-arrow
   if (orc_XmlParser.SelectNodeChild("end-arrow-head-type") == "end-arrow-head-type")
   {
      orc_LineArrow.e_EndArrowHeadType = C_PuiBsLineArrow::h_ArrowHeadTypeFromString(orc_XmlParser.GetNodeContent());
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XmlParser.SelectNodeParent(); //back to line-arrow
   if (orc_XmlParser.SelectNodeChild("line-type") == "line-type")
   {
      orc_LineArrow.e_LineType = C_PuiBsLineArrow::h_LineTypeFromString(orc_XmlParser.GetNodeContent());
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XmlParser.SelectNodeParent(); //back to line-arrow
   if (orc_XmlParser.SelectNodeChild("dark-color") == "dark-color")
   {
      h_LoadColor(orc_LineArrow.c_UiColorDark, orc_XmlParser);
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   orc_XmlParser.SelectNodeParent(); //back to line-arrow
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save line arrow

   \param[in]     orc_LineArrow Line arrow data element
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "line-arrow" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::mh_SaveLineArrow(const C_PuiBsLineArrow & orc_LineArrow,
                                            stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("line");
   h_SaveLineBase(orc_LineArrow, orc_XmlParser);
   orc_XmlParser.SelectNodeParent(); //back to "image"
   orc_XmlParser.CreateAndSelectNodeChild("start-arrow-head-type");
   orc_XmlParser.SetNodeContent(C_PuiBsLineArrow::h_ArrowHeadTypeToString(orc_LineArrow.e_StartArrowHeadType));
   orc_XmlParser.SelectNodeParent(); //back to "image"
   orc_XmlParser.CreateAndSelectNodeChild("end-arrow-head-type");
   orc_XmlParser.SetNodeContent(C_PuiBsLineArrow::h_ArrowHeadTypeToString(orc_LineArrow.e_EndArrowHeadType));
   orc_XmlParser.SelectNodeParent(); //back to "image"
   orc_XmlParser.CreateAndSelectNodeChild("line-type");
   orc_XmlParser.SetNodeContent(C_PuiBsLineArrow::h_LineTypeToString(orc_LineArrow.e_LineType));
   orc_XmlParser.SelectNodeParent(); //back to "image"
   orc_XmlParser.CreateAndSelectNodeChild("dark-color");
   h_SaveColor(orc_LineArrow.c_UiColorDark, orc_XmlParser);
   orc_XmlParser.SelectNodeParent(); //back to "image"
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load font style attributes

   \param[in,out] orc_FontStyle Font style data element
   \param[in,out] orc_XmlParser XML parser with the "current" element set to a color element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::mh_LoadFontStyle(QFont & orc_FontStyle,
                                            const stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   orc_FontStyle.fromString(orc_XmlParser.GetNodeContent().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save font style attributes

   \param[in]     orc_FontStyle Font style data element
   \param[in,out] orc_XmlParser XML parser with the "current" element set to a color element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::mh_SaveFontStyle(const QFont & orc_FontStyle,
                                            stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetNodeContent(orc_FontStyle.toString().toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert pixmap to string

   \param[in]   orc_Pixmap    Pixmap
   \param[in]   orc_Format    Image format
   \param[out]  orc_String    Byte string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::mh_PixmapToString(const QPixmap & orc_Pixmap, const QByteArray & orc_Format,
                                             QString & orc_String)
{
   QByteArray c_PixmapAsByteArray;
   QBuffer c_PixmapBuffer(&c_PixmapAsByteArray);
   // use PNG as default image format as we had PNG hard-coded in previous implementation
   const QByteArray c_Format = orc_Format.isEmpty() ? "png" : orc_Format;

   c_PixmapBuffer.open(QIODevice::WriteOnly);
   orc_Pixmap.save(&c_PixmapBuffer, c_Format);
   c_PixmapBuffer.close();

   orc_String = QString::fromUtf8(c_PixmapAsByteArray.toBase64());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert string to pixmap

   \param[in]   orc_String    Byte string
   \param[out]  orc_Pixmap    Pixmap
   \param[in]   orc_Format    Image format
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsElementsFiler::mh_StringToPixmap(const QString & orc_String, QPixmap & orc_Pixmap,
                                             const QByteArray & orc_Format)
{
   // use PNG as default image type as we had PNG hard-coded in previous implementation
   const QByteArray c_Format = orc_Format.isEmpty() ? "png" : orc_Format;

   orc_Pixmap.loadFromData(QByteArray::fromBase64(orc_String.toUtf8()), c_Format);
}
