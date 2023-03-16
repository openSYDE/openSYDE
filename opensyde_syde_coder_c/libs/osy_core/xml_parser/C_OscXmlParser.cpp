//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief      Wrapper class for tinyxml2

   cf. .h file header for details

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers

#include <fstream>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscXmlParser.hpp"
#include "tinyxml2.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;

using namespace stw::errors;
using namespace stw::opensyde_core;

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
C_OscXmlParserBase::C_OscXmlParserBase(void)
{
   mpc_CurrentNode = NULL;
   m_Init();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init of common xml structure
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserBase::m_Init(void)
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
C_OscXmlParserBase::~C_OscXmlParserBase(void)
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
C_OscXmlParser::C_OscXmlParser(void) :
   C_OscXmlParserBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Destructor

   Clean up ...
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscXmlParser::~C_OscXmlParser(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open XML data from file

   Open XML file.
   If the file could not be opened the function will return an error and prepare an empty XML structure.

   \param[in]  orc_FileName   path to XML file to open

   \return
   C_NO_ERR   data was read from file
   C_NOACT    could not read data from file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXmlParser::LoadFromFile(const C_SclString & orc_FileName)
{
   tinyxml2::XMLError e_Error;
   int32_t s32_Return = C_NO_ERR;
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

   \param[in]  orc_FileName   path to XML file to write to

   \return
   C_NO_ERR   data was written to file
   C_NOACT    could not write data to file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXmlParser::SaveToFile(const C_SclString & orc_FileName)
{
   int32_t s32_Return = C_NO_ERR;

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
C_SclString C_OscXmlParserBase::SelectRoot(void)
{
   C_SclString c_RootName;

   mpc_CurrentNode = mc_Document.RootElement();
   if (mpc_CurrentNode != NULL)
   {
      c_RootName = mpc_CurrentNode->Name();
   }
   return c_RootName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select root node as active element

   Set the document's root node as active element.

   \param[in,out]  orc_Name   Root node name

   \return
   Result of root element selection

   \retval   C_NO_ERR   Root found
   \retval   C_CONFIG   Root not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXmlParserBase::SelectRootError(const C_SclString & orc_Name)
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->SelectRoot() != orc_Name)
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select next node as active element

   Select the next node on the same level as the current node as active element.

   \param[in]  orc_Name    if != "": select first node with this name
                           if == "": select first node with any name

   \return
   name of selected element ("" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscXmlParserBase::SelectNodeNext(const C_SclString & orc_Name)
{
   C_SclString c_Name;

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

   \param[in]  orc_Name    if != "": select first child node with this name
                           if == "": select first child node with any name

   \return
   name of selected element ("" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscXmlParserBase::SelectNodeChild(const C_SclString & orc_Name)
{
   C_SclString c_Name;

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
/*! \brief  Select next node as active element

   Select the next node on the same level as the current node as active element.

   \param[in]  orc_Name    Select first child node with this name

   \return
   Result of node switch

   \retval   C_NO_ERR   Node switch success
   \retval   C_CONFIG   Node switch failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXmlParserBase::SelectNodeChildError(const C_SclString & orc_Name)
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->SelectNodeChild(orc_Name) != orc_Name)
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select parent of active node as active element

   Select the parent node of the current node as active element.

   \return
   name of selected element ("" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscXmlParserBase::SelectNodeParent(void)
{
   C_SclString c_Name;

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
C_SclString C_OscXmlParserBase::GetNodeContent(void) const
{
   C_SclString c_Content;

   if (mpc_CurrentNode != NULL)
   {
      const char_t * const pcn_Content = mpc_CurrentNode->GetText();
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

   \param[in]  orc_Name    name of attribute to check for

   \return
   true   attribute exists
   false  attribute does not exists (or: no element selected)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscXmlParserBase::AttributeExists(const C_SclString & orc_Name) const
{
   bool q_Return = false;

   if (mpc_CurrentNode != NULL)
   {
      const char_t * const pcn_Text = mpc_CurrentNode->Attribute(orc_Name.c_str());
      if (pcn_Text != NULL)
      {
         q_Return = true;
      }
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current node name

   \return
   Current node name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscXmlParserBase::GetCurrentNodeName(void) const
{
   return (mpc_CurrentNode == NULL) ? "" : mpc_CurrentNode->Name();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get file line for current node

   \return
   File line for current node
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscXmlParserBase::GetFileLineForCurrentNode(void) const
{
   return (mpc_CurrentNode == NULL) ? 0U : static_cast<uint32_t>(mpc_CurrentNode->GetLineNum());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Return one attribute value of selected node as string.

   \param[in]  orc_Name       name of attribute
   \param[in]  orc_Default    Default if attribute is not found

   \return
   Content of selected attribute ("" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscXmlParserBase::GetAttributeString(const C_SclString & orc_Name, const C_SclString & orc_Default) const
{
   C_SclString c_Value = orc_Default;

   if (mpc_CurrentNode != NULL)
   {
      const char_t * const pcn_Text = mpc_CurrentNode->Attribute(orc_Name.c_str());
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

   \param[in]  orc_Name       name of attribute
   \param[in]  os32_Default   Default if attribute is not found or conversion of node content from string fails

   \return
   value (zero on error)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXmlParserBase::GetAttributeSint32(const C_SclString & orc_Name, const int32_t os32_Default) const
{
   int32_t s32_Value = os32_Default;
   C_SclString c_Text;

   //do not use XMLElement::Query function: it can not handle hexadecimal values with "0x"
   c_Text = this->GetAttributeString(orc_Name);
   if (c_Text != "")
   {
      try
      {
         s32_Value = c_Text.ToInt();
      }
      catch (...)
      {
      }
   }
   return s32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Return one attribute value of selected node as uint32 value.
   Can handle "0x" notation to interpret hex values.

   \param[in]  orc_Name       name of attribute
   \param[in]  ou32_Default   Default if attribute is not found or conversion of node content from string fails

   \return
   value (zero on error)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscXmlParserBase::GetAttributeUint32(const C_SclString & orc_Name, const uint32_t ou32_Default) const
{
   uint32_t u32_Value = ou32_Default;
   C_SclString c_Text;

   //do not use XMLElement::Query function: it can not handle hexadecimal values with "0x"
   c_Text = this->GetAttributeString(orc_Name);
   if (c_Text != "")
   {
      try
      {
         u32_Value = static_cast<uint32_t>(c_Text.ToInt());
      }
      catch (...)
      {
      }
   }
   return u32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Return one attribute value of selected node as sint64 value.
   Can handle "0x" notation to interpret hex values.

   \param[in]  orc_Name       name of attribute
   \param[in]  os64_Default   Default if attribute is not found or conversion of node content from string fails

   \return
   value (zero on error)
*/
//----------------------------------------------------------------------------------------------------------------------
int64_t C_OscXmlParserBase::GetAttributeSint64(const C_SclString & orc_Name, const int64_t os64_Default) const
{
   int64_t s64_Value = os64_Default;
   C_SclString c_Text;

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

   Return one attribute value of selected node as uint64 value.
   Can handle "0x" notation to interpret hex values.

   \param[in]  orc_Name       name of attribute
   \param[in]  ou64_Default   Default if attribute is not found or conversion of node content from string fails

   \return
   value (zero on error)
*/
//----------------------------------------------------------------------------------------------------------------------
uint64_t C_OscXmlParserBase::GetAttributeUint64(const C_SclString & orc_Name, const uint64_t ou64_Default) const
{
   uint64_t u64_Value = ou64_Default;
   C_SclString c_Text;

   //do not use XMLElement::Query function: it can not handle hexadecimal values with "0x"
   c_Text = this->GetAttributeString(orc_Name);
   if (c_Text != "")
   {
      if ((c_Text.Length() >= 2) && ((c_Text[1] == '0') && (c_Text[2] == 'x')))
      {
         std::istringstream c_Stream(c_Text.SubString(3UL, c_Text.Length() - 2UL).c_str());
         c_Stream >> std::hex >> u64_Value;
         if (c_Stream.fail() == true)
         {
            u64_Value = ou64_Default;
         }
      }
      else
      {
         std::istringstream c_Stream(c_Text.c_str());
         c_Stream >> u64_Value;
         if (c_Stream.fail() == true)
         {
            u64_Value = ou64_Default;
         }
      }
   }
   return u64_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Return one attribute value of selected node as bool value.
   "0" resp. "1" and "false" resp. "true" are accepted as valid values.

   \param[in]  orc_Name    name of attribute
   \param[in]  oq_Default  Default if attribute is not found or conversion of node content from string fails

   \return
   true   attribute value is true
   false  attribute value is false (also returned on error)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscXmlParserBase::GetAttributeBool(const C_SclString & orc_Name, const bool oq_Default) const
{
   bool q_Value = oq_Default;

   if (mpc_CurrentNode != NULL)
   {
      const tinyxml2::XMLError e_Error = mpc_CurrentNode->QueryBoolAttribute(orc_Name.c_str(), &q_Value);
      if (e_Error != tinyxml2::XML_SUCCESS)
      {
         q_Value = oq_Default;
      }
   }
   return q_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Return one attribute value of selected node as float32 value.

   \param[in]  orc_Name       name of attribute
   \param[in]  of32_Default   Default if attribute is not found or conversion of node content from string fails

   \return
   value (0.0F on error)
*/
//----------------------------------------------------------------------------------------------------------------------
float32_t C_OscXmlParserBase::GetAttributeFloat32(const C_SclString & orc_Name, const float32_t of32_Default) const
{
   float32_t f32_Value = of32_Default;

   if (mpc_CurrentNode != NULL)
   {
      const tinyxml2::XMLError e_Error = mpc_CurrentNode->QueryFloatAttribute(orc_Name.c_str(), &f32_Value);
      if (e_Error != tinyxml2::XML_SUCCESS)
      {
         f32_Value = of32_Default;
      }
   }
   return f32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Return one attribute value of selected node as float64 value.

   \param[in]  orc_Name       name of attribute
   \param[in]  of64_Default   Default if attribute is not found or conversion of node content from string fails

   \return
   value (0.0 on error)
*/
//----------------------------------------------------------------------------------------------------------------------
float64_t C_OscXmlParserBase::GetAttributeFloat64(const C_SclString & orc_Name, const float64_t of64_Default) const
{
   float64_t f64_Value = of64_Default;

   if (mpc_CurrentNode != NULL)
   {
      const tinyxml2::XMLError e_Error = mpc_CurrentNode->QueryDoubleAttribute(orc_Name.c_str(), &f64_Value);
      if (e_Error != tinyxml2::XML_SUCCESS)
      {
         f64_Value = of64_Default;
      }
   }
   return f64_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Get one attribute value of selected node as string.

   \param[in]  orc_Name    name of attribute
   \param[in]  orc_Value   Content of selected attribute ("" on error)

   \return
   Result of attribute check

   \retval   C_NO_ERR   Attribute exists
   \retval   C_CONFIG   Attribute missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXmlParserBase::GetAttributeStringError(const C_SclString & orc_Name, C_SclString & orc_Value) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->AttributeExists(orc_Name))
   {
      orc_Value = this->GetAttributeString(orc_Name);
   }
   else
   {
      orc_Value = "";
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Get one attribute value of selected node as sint32.
   Can handle "0x" notation to interpret hex values.

   \param[in]  orc_Name       name of attribute
   \param[in]  ors32_Value    Content of selected attribute (0 on error)

   \return
   Result of attribute check

   \retval   C_NO_ERR   Attribute exists
   \retval   C_CONFIG   Attribute missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXmlParserBase::GetAttributeSint32Error(const C_SclString & orc_Name, int32_t & ors32_Value) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->AttributeExists(orc_Name))
   {
      ors32_Value = this->GetAttributeSint32(orc_Name);
   }
   else
   {
      ors32_Value = 0L;
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Get one attribute value of selected node as uint32.
   Can handle "0x" notation to interpret hex values.

   \param[in]  orc_Name       name of attribute
   \param[in]  oru32_Value    Content of selected attribute (0 on error)

   \return
   Result of attribute check

   \retval   C_NO_ERR   Attribute exists
   \retval   C_CONFIG   Attribute missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXmlParserBase::GetAttributeUint32Error(const C_SclString & orc_Name, uint32_t & oru32_Value) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->AttributeExists(orc_Name))
   {
      oru32_Value = this->GetAttributeUint32(orc_Name);
   }
   else
   {
      oru32_Value = 0UL;
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Get one attribute value of selected node as sint64.
   Can handle "0x" notation to interpret hex values.

   \param[in]  orc_Name       name of attribute
   \param[in]  ors64_Value    Content of selected attribute (0 on error)

   \return
   Result of attribute check

   \retval   C_NO_ERR   Attribute exists
   \retval   C_CONFIG   Attribute missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXmlParserBase::GetAttributeSint64Error(const C_SclString & orc_Name, int64_t & ors64_Value) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->AttributeExists(orc_Name))
   {
      ors64_Value = this->GetAttributeSint64(orc_Name);
   }
   else
   {
      ors64_Value = 0LL;
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Get one attribute value of selected node as uint64.
   Can handle "0x" notation to interpret hex values.

   \param[in]  orc_Name       name of attribute
   \param[in]  oru64_Value    Content of selected attribute (0 on error)

   \return
   Result of attribute check

   \retval   C_NO_ERR   Attribute exists
   \retval   C_CONFIG   Attribute missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXmlParserBase::GetAttributeUint64Error(const C_SclString & orc_Name, uint64_t & oru64_Value) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->AttributeExists(orc_Name))
   {
      oru64_Value = this->GetAttributeUint64(orc_Name);
   }
   else
   {
      oru64_Value = 0ULL;
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Get one attribute value of selected node as sint64.
   "0" resp. "1" and "false" resp. "true" are accepted as valid values.

   \param[in]  orc_Name    name of attribute
   \param[in]  orq_Value   Content of selected attribute (false on error)

   \return
   Result of attribute check

   \retval   C_NO_ERR   Attribute exists
   \retval   C_CONFIG   Attribute missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXmlParserBase::GetAttributeBoolError(const C_SclString & orc_Name, bool & orq_Value) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->AttributeExists(orc_Name))
   {
      orq_Value = this->GetAttributeBool(orc_Name);
   }
   else
   {
      orq_Value = false;
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Get one attribute value of selected node as float32.

   \param[in]  orc_Name       name of attribute
   \param[in]  orf32_Value    Content of selected attribute (0 on error)

   \return
   Result of attribute check

   \retval   C_NO_ERR   Attribute exists
   \retval   C_CONFIG   Attribute missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXmlParserBase::GetAttributeFloat32Error(const C_SclString & orc_Name, float32_t & orf32_Value) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->AttributeExists(orc_Name))
   {
      orf32_Value = this->GetAttributeFloat32(orc_Name);
   }
   else
   {
      orf32_Value = 0.0F;
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get attribute value of selected node

   Get one attribute value of selected node as float64.

   \param[in]  orc_Name       name of attribute
   \param[in]  orf64_Value    Content of selected attribute (0 on error)

   \return
   Result of attribute check

   \retval   C_NO_ERR   Attribute exists
   \retval   C_CONFIG   Attribute missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXmlParserBase::GetAttributeFloat64Error(const C_SclString & orc_Name, float64_t & orf64_Value) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->AttributeExists(orc_Name))
   {
      orf64_Value = this->GetAttributeFloat64(orc_Name);
   }
   else
   {
      orf64_Value = 0.0;
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for node content, starting with error message

   \param[in]  orc_ErrorMessage  Error message
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} intentionally no functionality in default implementation
void C_OscXmlParserBase::ReportErrorForNodeContentAppendXmlContext(const C_SclString & orc_ErrorMessage)
const
{
   //Does not report in the base implementation as base class does not handle error reporting
   //Also e.g in ParseFromString case errors in log file might be misleading
   (void)orc_ErrorMessage;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for attribute content, starting with error message

   \param[in]  orc_Attribute     Attribute
   \param[in]  orc_ErrorMessage  Error message
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} intentionally no functionality in default implementation
void C_OscXmlParserBase::ReportErrorForAttributeContentAppendXmlContext(const C_SclString & orc_Attribute,
                                                                        const C_SclString & orc_ErrorMessage)
const
{
   //Does not report in the base implementation as base class does not handle error reporting
   //Also e.g in ParseFromString case errors in log file might be misleading
   (void)orc_Attribute;
   (void)orc_ErrorMessage;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for node content, finish with error message

   \param[in]  orc_ErrorMessage  Error message
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} intentionally no functionality in default implementation
void C_OscXmlParserBase::ReportErrorForNodeContentStartingWithXmlContext(const C_SclString & orc_ErrorMessage)
const
{
   //Does not report in the base implementation as base class does not handle error reporting
   //Also e.g in ParseFromString case errors in log file might be misleading
   (void)orc_ErrorMessage;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for attribute content, finish with error message

   \param[in]  orc_Attribute     Attribute
   \param[in]  orc_ErrorMessage  Error message
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} intentionally no functionality in default implementation
void C_OscXmlParserBase::ReportErrorForAttributeContentStartingWithXmlContext(const C_SclString & orc_Attribute,
                                                                              const C_SclString & orc_ErrorMessage)
const
{
   //Does not report in the base implementation as base class does not handle error reporting
   //Also e.g in ParseFromString case errors in log file might be misleading
   (void)orc_Attribute;
   (void)orc_ErrorMessage;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for node missing

   \param[in]  orc_MissingNodeName  Missing node name
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} intentionally no functionality in default implementation
void C_OscXmlParserBase::ReportErrorForNodeMissing(const C_SclString & orc_MissingNodeName) const
{
   //Does not report in the base implementation as base class does not handle error reporting
   //Also e.g in ParseFromString case errors in log file might be misleading
   (void)orc_MissingNodeName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all attribute values of selected node

   Return all attribute values of selected node as vector of C_OscXMLAttribute.

   \return
   Content of selected attributes (empty vector on error)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OscXmlAttribute> C_OscXmlParserBase::GetAttributes(void) const
{
   std::vector<C_OscXmlAttribute> c_AttributeList;
   if (mpc_CurrentNode != NULL)
   {
      const tinyxml2::XMLAttribute * pc_Attribute = mpc_CurrentNode->FirstAttribute();
      while (pc_Attribute != NULL)
      {
         C_OscXmlAttribute c_Data;
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

   \param[in]  orc_Name       name of new node
   \param[in]  orc_Content    content of new node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserBase::CreateNodeChild(const C_SclString & orc_Name, const C_SclString & orc_Content)
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

   \param[in]  orc_Name    name of new node

   \return   name of the new node
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscXmlParserBase::CreateAndSelectNodeChild(const C_SclString & orc_Name)
{
   C_SclString c_Name;

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
C_SclString C_OscXmlParserBase::DeleteNode(void)
{
   C_SclString c_Name;

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

   \param[in]  orc_Content    new content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserBase::SetNodeContent(const C_SclString & orc_Content)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetText(orc_Content.c_str());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set string content of one attribute of currently selected node.

   Set content of one attribute of currently selected node from a string.

   \param[in]  orc_Name    name of attribute
   \param[in]  orc_Value   new value of attribute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserBase::SetAttributeString(const C_SclString & orc_Name, const C_SclString & orc_Value)
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
void C_OscXmlParserBase::SetAttributeSint32(const C_SclString & orc_Name, const int32_t os32_Value)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetAttribute(orc_Name.c_str(), os32_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set uint32 content of attribute of currently selected node.

   Set content of one attribute of currently selected node from a uint32.

   \param[in]  orc_Name    name of attribute
   \param[in]  ou32_Value  new value of attribute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserBase::SetAttributeUint32(const C_SclString & orc_Name, const uint32_t ou32_Value)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetAttribute(orc_Name.c_str(), ou32_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set sint64 content of attribute of currently selected node.

   Set content of one attribute of currently selected node from an sint64.

   \param[in]  orc_Name    name of attribute
   \param[in]  os64_Value  new value of attribute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserBase::SetAttributeSint64(const C_SclString & orc_Name, const int64_t os64_Value)
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
void C_OscXmlParserBase::SetAttributeUint64(const C_SclString & orc_Name, const uint64_t ou64_Value)
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
void C_OscXmlParserBase::SetAttributeBool(const C_SclString & orc_Name, const bool oq_Value)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetAttribute(orc_Name.c_str(), oq_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set float32 content of attribute of currently selected node.

   Set content of one attribute of currently selected node from a float32.

   \param[in]  orc_Name    name of attribute
   \param[in]  of32_Value  new value of attribute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserBase::SetAttributeFloat32(const C_SclString & orc_Name, const float32_t of32_Value)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetAttribute(orc_Name.c_str(), of32_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set float64 content of attribute of currently selected node.

   Set content of one attribute of currently selected node from a float64.

   \param[in]  orc_Name    name of attribute
   \param[in]  of64_Value  new value of attribute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserBase::SetAttributeFloat64(const C_SclString & orc_Name, const float64_t of64_Value)
{
   if (mpc_CurrentNode != NULL)
   {
      mpc_CurrentNode->SetAttribute(orc_Name.c_str(), of64_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open XML data from string

   Parse XML data from string.
   If the data cannot be parsed the function will return an error and prepare an empty XML structure.

   \param[in]  orc_String  string containing XML data

   \return
   C_NO_ERR   data was read from string
   C_NOACT    could not parse data from string
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXmlParser::LoadFromString(const C_SclString & orc_String)
{
   tinyxml2::XMLError e_Error;
   int32_t s32_Return = C_NO_ERR;
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

   \param[out]  orc_String    Resulting XML data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParser::SaveToString(C_SclString & orc_String) const
{
   tinyxml2::XMLPrinter c_Printer;
   this->mc_Document.Print(&c_Printer);
   orc_String = static_cast<C_SclString>(c_Printer.CStr());
}
