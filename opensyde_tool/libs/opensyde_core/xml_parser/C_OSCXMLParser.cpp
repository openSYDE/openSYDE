//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief      Wrapper class for tinyxml2

   cf. .h file header for details

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h" //pre-compiled headers

#include <fstream>
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCXMLParser.h"
#include "tinyxml2.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Constructor

   Open XML file.
   Will throw on error.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCXMLParserBase::C_OSCXMLParserBase(void)
{
   mpc_CurrentNode = NULL;
   m_Init();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init of common xml structure
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserBase::m_Init(void)
{
   //empty file ?
   if (mc_Document.RootElement() == NULL)
   {
      //create header:
      tinyxml2::XMLDeclaration * const pc_Declaration = mc_Document.NewDeclaration(
         "xml version=\"1.0\" encoding=\"utf-8\"");
      mc_Document.InsertFirstChild(pc_Declaration);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Destructor

   Will try to save the XML file and will throw on error.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCXMLParserBase::~C_OSCXMLParserBase(void)
{
   mpc_CurrentNode = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Constructor

   Set up empty document

   Open XML file.
   Will throw on error.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCXMLParser::C_OSCXMLParser(void) :
   C_OSCXMLParserBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Destructor

   Clean up ...
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCXMLParser::~C_OSCXMLParser(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open XML data from file

   Open XML file.
   If the file could not be opened the function will return an error and prepare an empty XML structure.

   \param[in]   orc_FileName   path to XML file to open

   \return
   C_NO_ERR   data was read from file
   C_NOACT    could not read data from file
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCXMLParser::LoadFromFile(const C_SCLString & orc_FileName)
{
   tinyxml2::XMLError e_Error;
   sint32 s32_Return = C_NO_ERR;
   mc_Document.Clear();

   e_Error = mc_Document.LoadFile(orc_FileName.c_str());
   if (e_Error != tinyxml2::XML_SUCCESS)
   {
      s32_Return = C_NOACT;
      //prepare empty file structure:
      m_Init();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Write XML data to file

   A pre-existing file will be replaced.

   \param[in]   orc_FileName   path to XML file to write to

   \return
   C_NO_ERR   data was written to file
   C_NOACT    could not write data to file
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCXMLParser::SaveToFile(const C_SCLString & orc_FileName)
{
   sint32 s32_Return = C_NO_ERR;

   const tinyxml2::XMLError e_Error = mc_Document.SaveFile(orc_FileName.c_str());

   if (e_Error != tinyxml2::XML_SUCCESS)
   {
      s32_Return = C_NOACT;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select root node as active element

   Set the document's root node as active element.

   \return
   name of root element ("" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCXMLParserBase::SelectRoot(void)
{
   C_SCLString c_RootName;

   mpc_CurrentNode = mc_Document.RootElement();
   if (mpc_CurrentNode != NULL)
   {
      c_RootName = mpc_CurrentNode->Name();
   }
   return c_RootName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select next node as active element

   Select the next node on the same level as the current node as active element.

   \param[in]  orc_Name   if != "": select first node with this name
                          if == "": select first node with any name

   \return
   name of selected element ("" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCXMLParserBase::SelectNodeNext(const C_SCLString & orc_Name)
{
   C_SCLString c_Name;

   tinyxml2::XMLElement * const pc_Save = mpc_CurrentNode;

   if (mpc_CurrentNode != NULL)
   {
      if (orc_Name != "")
      {
         mpc_CurrentNode = mpc_CurrentNode->NextSiblingElement(orc_Name.c_str());
      }
      else
      {
         mpc_CurrentNode = mpc_CurrentNode->NextSiblingElement();
      }
   }
   if (mpc_CurrentNode != NULL)
   {
      c_Name = mpc_CurrentNode->Name();
   }
   else
   {
      //Restore last known state?
      mpc_CurrentNode = pc_Save;
   }
   return c_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select next node as active element

   Select the next node on the same level as the current node as active element.

   \param[in]  orc_Name   if != "": select first child node with this name
                          if == "": select first child node with any name

   \return
   name of selected element ("" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCXMLParserBase::SelectNodeChild(const C_SCLString & orc_Name)
{
   C_SCLString c_Name;

   tinyxml2::XMLElement * pc_Element = NULL;

   if (mpc_CurrentNode != NULL)
   {
      pc_Element = mpc_CurrentNode;
   }
   else
   {
      pc_Element = mc_Document.RootElement();
   }

   if (pc_Element != NULL)
   {
      if (orc_Name == "")
      {
         pc_Element = pc_Element->FirstChildElement();
      }
      else
      {
         pc_Element = pc_Element->FirstChildElement(orc_Name.c_str());
      }
   }

   if (pc_Element != NULL)
   {
      mpc_CurrentNode = pc_Element;
      c_Name = mpc_CurrentNode->Name();
   }
   return c_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select parent of active node as active element

   Select the parent node of the current node as active element.

   \return
   name of selected element ("" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCXMLParserBase::SelectNodeParent(void)
{
   C_SCLString c_Name;

   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode = mpc_CurrentNode->Parent()->ToElement();
   }
   if (mpc_CurrentNode != NULL)
   {
      c_Name = mpc_CurrentNode->Name();
   }
   return c_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get content of selected node

   Return content of selected node as string.
   Example:
   if the active node contains "<element>text</element>"
    this function will return "text".

   \return
   Content of selected element ("" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCXMLParserBase::GetNodeContent(void) const
{
   C_SCLString c_Content;

   if (mpc_CurrentNode != NULL)
   {
      const charn * const pcn_Content = mpc_CurrentNode->GetText();
      if (pcn_Content != NULL)
      {
         c_Content = pcn_Content;
      }
   }

   return c_Content;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check whether specified attribute exists

   Check whether the specified attribute exists in the current element.

   \param[in]   orc_Name   name of attribute to check for

   \return
   true   attribute exists
   false  attribute does not exists (or: no element selected)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCXMLParserBase::AttributeExists(const C_SCLString & orc_Name) const
{
   bool q_Return = false;

   if (mpc_CurrentNode != NULL)
   {
      const charn * const pcn_Text = mpc_CurrentNode->Attribute(orc_Name.c_str());
      if (pcn_Text != NULL)
      {
         q_Return = true;
      }
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Return one attribute value of selected node as string.

   \param[in]  orc_Name   name of attribute

   \return
   Content of selected attribute ("" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCXMLParserBase::GetAttributeString(const C_SCLString & orc_Name) const
{
   C_SCLString c_Value;

   if (mpc_CurrentNode != NULL)
   {
      const charn * const pcn_Text = mpc_CurrentNode->Attribute(orc_Name.c_str());
      if (pcn_Text != NULL)
      {
         c_Value = pcn_Text;
      }
   }
   return c_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Return one attribute value of selected node as sint32 value.
   Can handle "0x" notation to interpret hex values.

   \param[in]  orc_Name   name of attribute

   \return
   value (zero on error)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCXMLParserBase::GetAttributeSint32(const C_SCLString & orc_Name) const
{
   sintn sn_Value = 0;
   C_SCLString c_Text;

   //do not use XMLElement::Query function: it can not handle hexadecimal values with "0x"
   c_Text = this->GetAttributeString(orc_Name);
   if (c_Text != "")
   {
      try
      {
         sn_Value = static_cast<sintn>(c_Text.ToInt());
      }
      catch (...)
      {
      }
   }
   return sn_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Return one attribute value of selected node as uint32 value.
   Can handle "0x" notation to interpret hex values.

   \param[in]  orc_Name   name of attribute

   \return
   value (zero on error)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCXMLParserBase::GetAttributeUint32(const C_SCLString & orc_Name) const
{
   uintn un_Value = 0;
   C_SCLString c_Text;

   //do not use XMLElement::Query function: it can not handle hexadecimal values with "0x"
   c_Text = this->GetAttributeString(orc_Name);
   if (c_Text != "")
   {
      try
      {
         un_Value = static_cast<uintn>(c_Text.ToInt());
      }
      catch (...)
      {
      }
   }
   return un_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Return one attribute value of selected node as sint64 value.
   Can handle "0x" notation to interpret hex values.

   \param[in]  orc_Name   name of attribute

   \return
   value (zero on error)
*/
//----------------------------------------------------------------------------------------------------------------------
sint64 C_OSCXMLParserBase::GetAttributeSint64(const C_SCLString & orc_Name) const
{
   sint64 s64_Value = 0;
   C_SCLString c_Text;

   //do not use XMLElement::Query function: it can not handle hexadecimal values with "0x"
   c_Text = this->GetAttributeString(orc_Name);
   if (c_Text != "")
   {
      try
      {
         s64_Value = c_Text.ToInt64();
      }
      catch (...)
      {
      }
   }
   return s64_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Return one attribute value of selected node as sint64 value.
   Can handle "0x" notation to interpret hex values.

   \param[in]  orc_Name    name of attribute

   \return
   value (zero on error)
*/
//----------------------------------------------------------------------------------------------------------------------
uint64 C_OSCXMLParserBase::GetAttributeUint64(const C_SCLString & orc_Name) const
{
   uint64 u64_Value = 0;
   C_SCLString c_Text;

   //do not use XMLElement::Query function: it can not handle hexadecimal values with "0x"
   c_Text = this->GetAttributeString(orc_Name);
   if (c_Text != "")
   {
      if ((c_Text.Length() >= 2) && ((c_Text[1] == '0') && (c_Text[2] == 'x')))
      {
         std::istringstream c_Stream(c_Text.SubString(3UL, c_Text.Length() - 2UL).c_str());
         c_Stream >> std::hex >> u64_Value;
      }
      else
      {
         std::istringstream c_Stream(c_Text.c_str());
         c_Stream >> u64_Value;
      }
   }
   return u64_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Return one attribute value of selected node as bool value.
   "0" resp. "1" and "false" resp. "true" are accepted as valid values.

   \param[in]  orc_Name   name of attribute

   \return
   true   attribute value is true
   false  attribute value is false (also returned on error)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCXMLParserBase::GetAttributeBool(const C_SCLString & orc_Name) const
{
   bool q_Value = false;

   if (mpc_CurrentNode != NULL)
   {
      const tinyxml2::XMLError e_Error = mpc_CurrentNode->QueryBoolAttribute(orc_Name.c_str(), &q_Value);
      if (e_Error != tinyxml2::XML_SUCCESS)
      {
         q_Value = false;
      }
   }
   return q_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Return one attribute value of selected node as float32 value.

   \param[in]  orc_Name   name of attribute

   \return
   value (0.0F on error)
*/
//----------------------------------------------------------------------------------------------------------------------
float32 C_OSCXMLParserBase::GetAttributeFloat32(const C_SCLString & orc_Name) const
{
   float32 f32_Value = 0.0F;

   if (mpc_CurrentNode != NULL)
   {
      const tinyxml2::XMLError e_Error = mpc_CurrentNode->QueryFloatAttribute(orc_Name.c_str(), &f32_Value);
      if (e_Error != tinyxml2::XML_SUCCESS)
      {
         f32_Value = 0.0F;
      }
   }
   return f32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Return one attribute value of selected node as float64 value.

   \param[in]  orc_Name   name of attribute

   \return
   value (0.0 on error)
*/
//----------------------------------------------------------------------------------------------------------------------
float64 C_OSCXMLParserBase::GetAttributeFloat64(const C_SCLString & orc_Name) const
{
   float64 f64_Value = 0.0;

   if (mpc_CurrentNode != NULL)
   {
      const tinyxml2::XMLError e_Error = mpc_CurrentNode->QueryDoubleAttribute(orc_Name.c_str(), &f64_Value);
      if (e_Error != tinyxml2::XML_SUCCESS)
      {
         f64_Value = 0.0;
      }
   }
   return f64_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all attribute values of selected node

   Return all attribute values of selected node as vector of C_OSCXMLAttribute.

   \return
   Content of selected attributes (empty vector on error)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OSCXMLAttribute> C_OSCXMLParserBase::GetAttributes(void) const
{
   std::vector<C_OSCXMLAttribute> c_AttributeList;
   if (mpc_CurrentNode != NULL)
   {
      const tinyxml2::XMLAttribute * pc_Attribute = mpc_CurrentNode->FirstAttribute();
      while (pc_Attribute != NULL)
      {
         C_OSCXMLAttribute c_Data;
         c_Data.c_Name = pc_Attribute->Name();
         c_Data.c_Value = pc_Attribute->Value();
         c_AttributeList.push_back(c_Data);

         pc_Attribute = pc_Attribute->Next();
      }
   }

   return c_AttributeList;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create a node under the currently selected node.

   Add a new node under the currently selected node.
   The current selection will not be changed.
   If no node is currently selected the new node will be added at the end of the document.

   \param[in]  orc_Name     name of new node
   \param[in]  orc_Content  content of new node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserBase::CreateNodeChild(const C_SCLString & orc_Name, const C_SCLString & orc_Content)
{
   tinyxml2::XMLElement * const pc_Node = mc_Document.NewElement("");
   if (orc_Content != "")
   {
      pc_Node->SetText(orc_Content.c_str());
   }
   pc_Node->SetName(orc_Name.c_str()); //does not seem to be the same as in the constructor (?)
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->InsertEndChild(pc_Node);
   }
   else
   {
      mc_Document.InsertEndChild(pc_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create a node under the currently selected node and select it.

   Add a new node under the currently selected node.
   The current selection will be changed to the new node.
   If no node is currently selected the new node will be added at the end of the document.

   \param[in]  orc_Name     name of new node

   \return   name of the new node
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCXMLParserBase::CreateAndSelectNodeChild(const C_SCLString & orc_Name)
{
   C_SCLString c_Name;

   this->CreateNodeChild(orc_Name);
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode = mpc_CurrentNode->LastChildElement();
   }
   else
   {
      //new node added at the end:
      mpc_CurrentNode = mc_Document.LastChildElement();
   }

   if (mpc_CurrentNode != NULL)
   {
      c_Name = mpc_CurrentNode->Name();
   }
   return c_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete the current node.

   Remove the currently selected node.
   The selection will be set to NULL (= nothing selected).

   \return  if a node was selected: name of deleted node (otherwise "")
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCXMLParserBase::DeleteNode(void)
{
   C_SCLString c_Name;

   if (mpc_CurrentNode != NULL)
   {
      c_Name = mpc_CurrentNode->Name();
      mc_Document.DeleteNode(mpc_CurrentNode);
      mpc_CurrentNode = NULL;
   }

   return c_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set content of currently selected node.

   Set content of currently selected node from a string.

   \param[in]  orc_Content   new content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserBase::SetNodeContent(const C_SCLString & orc_Content)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetText(orc_Content.c_str());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set string content of one attribute of currently selected node.

   Set content of one attribute of currently selected node from a string.

   \param[in]  orc_Name   name of attribute
   \param[in]  orc_Value  new value of attribute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserBase::SetAttributeString(const C_SCLString & orc_Name, const C_SCLString & orc_Value)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetAttribute(orc_Name.c_str(), orc_Value.c_str());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set sint32 content of attribute of currently selected node.

   Set content of one attribute of currently selected node from an sint32.

   \param[in]  orc_Name    name of attribute
   \param[in]  os32_Value  new value of attribute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserBase::SetAttributeSint32(const C_SCLString & orc_Name, const sint32 os32_Value)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetAttribute(orc_Name.c_str(), static_cast<sintn>(os32_Value));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set uint32 content of attribute of currently selected node.

   Set content of one attribute of currently selected node from a uint32.

   \param[in]  orc_Name    name of attribute
   \param[in]  ou32_Value  new value of attribute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserBase::SetAttributeUint32(const C_SCLString & orc_Name, const uint32 ou32_Value)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetAttribute(orc_Name.c_str(), static_cast<uintn>(ou32_Value));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set sint64 content of attribute of currently selected node.

   Set content of one attribute of currently selected node from an sint64.

   \param[in]  orc_Name    name of attribute
   \param[in]  os64_Value  new value of attribute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserBase::SetAttributeSint64(const C_SCLString & orc_Name, const sint64 os64_Value)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetAttribute(orc_Name.c_str(), static_cast<int64_t>(os64_Value));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set uint64 content of attribute of currently selected node.

   Set content of one attribute of currently selected node from an sint64.

   \param[in]  orc_Name    name of attribute
   \param[in]  ou64_Value  new value of attribute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserBase::SetAttributeUint64(const C_SCLString & orc_Name, const uint64 ou64_Value)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetAttribute(orc_Name.c_str(), static_cast<uint64_t>(ou64_Value));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set bool content of attribute of currently selected node.

   Set content of one attribute of currently selected node from a bool.
   true will be written as "true"
   false will be written as "false"

   \param[in]  orc_Name    name of attribute
   \param[in]  oq_Value    new value of attribute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserBase::SetAttributeBool(const C_SCLString & orc_Name, const bool oq_Value)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetAttribute(orc_Name.c_str(), oq_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set float32 content of attribute of currently selected node.

   Set content of one attribute of currently selected node from a float32.

   \param[in]  orc_Name      name of attribute
   \param[in]  of32_Value    new value of attribute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserBase::SetAttributeFloat32(const C_SCLString & orc_Name, const float32 of32_Value)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetAttribute(orc_Name.c_str(), of32_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set float64 content of attribute of currently selected node.

   Set content of one attribute of currently selected node from a float64.

   \param[in]  orc_Name      name of attribute
   \param[in]  of64_Value    new value of attribute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserBase::SetAttributeFloat64(const C_SCLString & orc_Name, const float64 of64_Value)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetAttribute(orc_Name.c_str(), of64_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCXMLParserString::C_OSCXMLParserString(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCXMLParserString::~C_OSCXMLParserString(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open XML data from string

   Parse XML data from string.
   If the data cannot be parsed the function will return an error and prepare an empty XML structure.

   \param[in]   orc_String   string containing XML data

   \return
   C_NO_ERR   data was read from string
   C_NOACT    could not parse data from string
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCXMLParserString::LoadFromString(const C_SCLString & orc_String)
{
   tinyxml2::XMLError e_Error;
   sint32 s32_Return = C_NO_ERR;
   mc_Document.Clear();

   e_Error = this->mc_Document.Parse(orc_String.c_str(), orc_String.Length());
   if (e_Error != tinyxml2::XML_SUCCESS)
   {
      s32_Return = C_NOACT;
      //prepare empty file structure:
      m_Init();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Write XML data to string

   Write XML data to string

   \param[out]   orc_String   Resulting XML data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserString::SaveToString(C_SCLString & orc_String) const
{
   tinyxml2::XMLPrinter c_Printer;
   this->mc_Document.Print(&c_Printer);
   orc_String = static_cast<C_SCLString>(c_Printer.CStr());
}
