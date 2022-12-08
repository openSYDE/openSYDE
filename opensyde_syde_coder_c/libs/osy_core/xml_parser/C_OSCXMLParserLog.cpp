//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Wrapper for C_OscXMLParser including error logging

   Wrapper for C_OscXMLParser including error logging.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscXmlParserLog.hpp"
#include "C_OscLoggingHandler.hpp"

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
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscXmlParserLog::C_OscXmlParserLog() :
   C_OscXmlParser()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set log heading

   \param[in]  orc_Text    Text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserLog::SetLogHeading(const C_SclString & orc_Text)
{
   this->mc_LogHeading = orc_Text;
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
int32_t C_OscXmlParserLog::SelectRootError(const C_SclString & orc_Name)
{
   const int32_t s32_Retval = C_OscXmlParser::SelectRootError(orc_Name);

   if (s32_Retval != C_NO_ERR)
   {
      m_ReportErrorForRootNodeMissing(orc_Name);
   }

   return s32_Retval;
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
int32_t C_OscXmlParserLog::SelectNodeChildError(const C_SclString & orc_Name)
{
   const int32_t s32_Retval = C_OscXmlParser::SelectNodeChildError(orc_Name);

   if (s32_Retval != C_NO_ERR)
   {
      ReportErrorForNodeMissing(orc_Name);
   }

   return s32_Retval;
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
int32_t C_OscXmlParserLog::GetAttributeStringError(const C_SclString & orc_Name, C_SclString & orc_Value) const
{
   const int32_t s32_Retval = C_OscXmlParser::GetAttributeStringError(orc_Name, orc_Value);

   if (s32_Retval != C_NO_ERR)
   {
      m_ReportErrorForAttributeMissing(orc_Name);
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
int32_t C_OscXmlParserLog::GetAttributeSint32Error(const C_SclString & orc_Name, int32_t & ors32_Value) const
{
   const int32_t s32_Retval = C_OscXmlParser::GetAttributeSint32Error(orc_Name, ors32_Value);

   if (s32_Retval != C_NO_ERR)
   {
      m_ReportErrorForAttributeMissing(orc_Name);
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
int32_t C_OscXmlParserLog::GetAttributeUint32Error(const C_SclString & orc_Name, uint32_t & oru32_Value) const
{
   const int32_t s32_Retval = C_OscXmlParser::GetAttributeUint32Error(orc_Name, oru32_Value);

   if (s32_Retval != C_NO_ERR)
   {
      m_ReportErrorForAttributeMissing(orc_Name);
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
int32_t C_OscXmlParserLog::GetAttributeSint64Error(const C_SclString & orc_Name, int64_t & ors64_Value) const
{
   const int32_t s32_Retval = C_OscXmlParser::GetAttributeSint64Error(orc_Name, ors64_Value);

   if (s32_Retval != C_NO_ERR)
   {
      m_ReportErrorForAttributeMissing(orc_Name);
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
int32_t C_OscXmlParserLog::GetAttributeUint64Error(const C_SclString & orc_Name, uint64_t & oru64_Value) const
{
   const int32_t s32_Retval = C_OscXmlParser::GetAttributeUint64Error(orc_Name, oru64_Value);

   if (s32_Retval != C_NO_ERR)
   {
      m_ReportErrorForAttributeMissing(orc_Name);
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
int32_t C_OscXmlParserLog::GetAttributeBoolError(const C_SclString & orc_Name, bool & orq_Value) const
{
   const int32_t s32_Retval = C_OscXmlParser::GetAttributeBoolError(orc_Name, orq_Value);

   if (s32_Retval != C_NO_ERR)
   {
      m_ReportErrorForAttributeMissing(orc_Name);
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
int32_t C_OscXmlParserLog::GetAttributeFloat32Error(const C_SclString & orc_Name, float32_t & orf32_Value) const
{
   const int32_t s32_Retval = C_OscXmlParser::GetAttributeFloat32Error(orc_Name, orf32_Value);

   if (s32_Retval != C_NO_ERR)
   {
      m_ReportErrorForAttributeMissing(orc_Name);
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
int32_t C_OscXmlParserLog::GetAttributeFloat64Error(const C_SclString & orc_Name, float64_t & orf64_Value) const
{
   const int32_t s32_Retval = C_OscXmlParser::GetAttributeFloat64Error(orc_Name, orf64_Value);

   if (s32_Retval != C_NO_ERR)
   {
      m_ReportErrorForAttributeMissing(orc_Name);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for node content, starting with error message

   \param[in]  orc_ErrorMessage  Error message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserLog::ReportErrorForNodeContentAppendXmlContext(const C_SclString & orc_ErrorMessage)
const
{
   if (this->mc_LogHeading != "")
   {
      const C_SclString c_XmlLineInfoText = m_GetCurrentXmlLineInfoText();
      osc_write_log_error(this->mc_LogHeading,
                          orc_ErrorMessage + " in node \"" + this->GetCurrentNodeName() + "\"" + c_XmlLineInfoText +
                          ".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for attribute content, starting with error message

   \param[in]  orc_Attribute     Attribute
   \param[in]  orc_ErrorMessage  Error message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserLog::ReportErrorForAttributeContentAppendXmlContext(const C_SclString & orc_Attribute,
                                                                       const C_SclString & orc_ErrorMessage)
const
{
   if (this->mc_LogHeading != "")
   {
      const C_SclString c_XmlLineInfoText = m_GetCurrentXmlLineInfoText();
      osc_write_log_error(this->mc_LogHeading,
                          orc_ErrorMessage + " in attribute \"" + orc_Attribute + "\" in node \"" + this->GetCurrentNodeName() +
                          "\"" + c_XmlLineInfoText + ".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for node content, finish with error message

   \param[in]  orc_ErrorMessage  Error message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserLog::ReportErrorForNodeContentStartingWithXmlContext(const C_SclString & orc_ErrorMessage)
const
{
   if (this->mc_LogHeading != "")
   {
      const C_SclString c_XmlLineInfoText = m_GetCurrentXmlLineInfoText();
      osc_write_log_error(this->mc_LogHeading,
                          "Node \"" + this->GetCurrentNodeName() + "\"" + c_XmlLineInfoText + " " + orc_ErrorMessage +
                          ".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for attribute content, finish with error message

   \param[in]  orc_Attribute     Attribute
   \param[in]  orc_ErrorMessage  Error message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserLog::ReportErrorForAttributeContentStartingWithXmlContext(const C_SclString & orc_Attribute,
                                                                             const C_SclString & orc_ErrorMessage)
const
{
   if (this->mc_LogHeading != "")
   {
      const C_SclString c_XmlLineInfoText = m_GetCurrentXmlLineInfoText();
      osc_write_log_error(this->mc_LogHeading,
                          "Attribute \"" + orc_Attribute + "\" in node \"" + this->GetCurrentNodeName() +
                          "\"" + c_XmlLineInfoText + " " + orc_ErrorMessage +  ".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for root node missing

   \param[in]  orc_RootNodeName  Root node name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserLog::m_ReportErrorForRootNodeMissing(const C_SclString & orc_RootNodeName) const
{
   if (this->mc_LogHeading != "")
   {
      C_SclString c_FoundRootNodeText;
      if (this->GetCurrentNodeName() != "")
      {
         c_FoundRootNodeText = " (Found \"" + this->GetCurrentNodeName() + "\" instead)";
      }
      osc_write_log_error(this->mc_LogHeading,
                          "Could not find root node \"" + orc_RootNodeName + "\" in XML file" + c_FoundRootNodeText +
                          ".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for node missing

   \param[in]  orc_MissingNodeName  Missing node name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserLog::ReportErrorForNodeMissing(const C_SclString & orc_MissingNodeName) const
{
   if (this->mc_LogHeading != "")
   {
      const C_SclString c_XmlLineInfoText = m_GetCurrentXmlLineInfoText();
      osc_write_log_error(this->mc_LogHeading,
                          "Could not find node \"" + orc_MissingNodeName + "\" in node \"" + this->GetCurrentNodeName() +
                          "\"" + c_XmlLineInfoText + ".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for attribute missing

   \param[in]  orc_AttributeName    Attribute name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXmlParserLog::m_ReportErrorForAttributeMissing(const C_SclString & orc_AttributeName) const
{
   if (this->mc_LogHeading != "")
   {
      const C_SclString c_XmlLineInfoText = m_GetCurrentXmlLineInfoText();
      osc_write_log_error(this->mc_LogHeading,
                          "Could not find attribute \"" + orc_AttributeName + "\" in node \"" + this->GetCurrentNodeName() +
                          "\"" + c_XmlLineInfoText + ".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current XML line info text

   \return
   Current XML line info text
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscXmlParserLog::m_GetCurrentXmlLineInfoText() const
{
   C_SclString c_Retval;
   const uint32_t u32_Line = this->GetFileLineForCurrentNode();

   if (u32_Line != 0)
   {
      c_Retval = " (XML line " + C_SclString::IntToStr(u32_Line) + ")";
   }

   return c_Retval;
}
