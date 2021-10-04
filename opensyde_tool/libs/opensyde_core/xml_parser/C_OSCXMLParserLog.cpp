//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Wrapper for C_OSCXMLParser including error logging

   Wrapper for C_OSCXMLParser including error logging.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCXMLParserLog.h"
#include "C_OSCLoggingHandler.h"

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
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCXMLParserLog::C_OSCXMLParserLog()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set log heading

   \param[in]  orc_Text    Text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserLog::SetLogHeading(const C_SCLString & orc_Text)
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
sint32 C_OSCXMLParserLog::SelectRootError(const C_SCLString & orc_Name)
{
   const sint32 s32_Retval = C_OSCXMLParser::SelectRootError(orc_Name);

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
sint32 C_OSCXMLParserLog::SelectNodeChildError(const C_SCLString & orc_Name)
{
   const sint32 s32_Retval = C_OSCXMLParser::SelectNodeChildError(orc_Name);

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
sint32 C_OSCXMLParserLog::GetAttributeStringError(const C_SCLString & orc_Name, C_SCLString & orc_Value) const
{
   const sint32 s32_Retval = C_OSCXMLParser::GetAttributeStringError(orc_Name, orc_Value);

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
sint32 C_OSCXMLParserLog::GetAttributeSint32Error(const C_SCLString & orc_Name, sint32 & ors32_Value) const
{
   const sint32 s32_Retval = C_OSCXMLParser::GetAttributeSint32Error(orc_Name, ors32_Value);

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
sint32 C_OSCXMLParserLog::GetAttributeUint32Error(const C_SCLString & orc_Name, uint32 & oru32_Value) const
{
   const sint32 s32_Retval = C_OSCXMLParser::GetAttributeUint32Error(orc_Name, oru32_Value);

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
sint32 C_OSCXMLParserLog::GetAttributeSint64Error(const C_SCLString & orc_Name, sint64 & ors64_Value) const
{
   const sint32 s32_Retval = C_OSCXMLParser::GetAttributeSint64Error(orc_Name, ors64_Value);

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
sint32 C_OSCXMLParserLog::GetAttributeUint64Error(const C_SCLString & orc_Name, uint64 & oru64_Value) const
{
   const sint32 s32_Retval = C_OSCXMLParser::GetAttributeUint64Error(orc_Name, oru64_Value);

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
sint32 C_OSCXMLParserLog::GetAttributeBoolError(const C_SCLString & orc_Name, bool & orq_Value) const
{
   const sint32 s32_Retval = C_OSCXMLParser::GetAttributeBoolError(orc_Name, orq_Value);

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
sint32 C_OSCXMLParserLog::GetAttributeFloat32Error(const C_SCLString & orc_Name, float32 & orf32_Value) const
{
   const sint32 s32_Retval = C_OSCXMLParser::GetAttributeFloat32Error(orc_Name, orf32_Value);

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
sint32 C_OSCXMLParserLog::GetAttributeFloat64Error(const C_SCLString & orc_Name, float64 & orf64_Value) const
{
   const sint32 s32_Retval = C_OSCXMLParser::GetAttributeFloat64Error(orc_Name, orf64_Value);

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
void C_OSCXMLParserLog::ReportErrorForNodeContentAppendXMLContext(const C_SCLString & orc_ErrorMessage)
const
{
   if (this->mc_LogHeading != "")
   {
      const C_SCLString c_XMLLineInfoText = m_GetCurrentXMLLineInfoText();
      osc_write_log_error(this->mc_LogHeading,
                          orc_ErrorMessage + " in node \"" + this->GetCurrentNodeName() + "\"" + c_XMLLineInfoText +
                          ".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for attribute content, starting with error message

   \param[in]  orc_Attribute     Attribute
   \param[in]  orc_ErrorMessage  Error message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserLog::ReportErrorForAttributeContentAppendXMLContext(const C_SCLString & orc_Attribute,
                                                                       const C_SCLString & orc_ErrorMessage)
const
{
   if (this->mc_LogHeading != "")
   {
      const C_SCLString c_XMLLineInfoText = m_GetCurrentXMLLineInfoText();
      osc_write_log_error(this->mc_LogHeading,
                          orc_ErrorMessage + " in attribute \"" + orc_Attribute + "\" in node \"" + this->GetCurrentNodeName() +
                          "\"" + c_XMLLineInfoText + ".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for node content, finish with error message

   \param[in]  orc_ErrorMessage  Error message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserLog::ReportErrorForNodeContentStartingWithXMLContext(const C_SCLString & orc_ErrorMessage)
const
{
   if (this->mc_LogHeading != "")
   {
      const C_SCLString c_XMLLineInfoText = m_GetCurrentXMLLineInfoText();
      osc_write_log_error(this->mc_LogHeading,
                          "Node \"" + this->GetCurrentNodeName() + "\"" + c_XMLLineInfoText + " " + orc_ErrorMessage +
                          ".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for attribute content, finish with error message

   \param[in]  orc_Attribute     Attribute
   \param[in]  orc_ErrorMessage  Error message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserLog::ReportErrorForAttributeContentStartingWithXMLContext(const C_SCLString & orc_Attribute,
                                                                             const C_SCLString & orc_ErrorMessage)
const
{
   if (this->mc_LogHeading != "")
   {
      const C_SCLString c_XMLLineInfoText = m_GetCurrentXMLLineInfoText();
      osc_write_log_error(this->mc_LogHeading,
                          "Attribute \"" + orc_Attribute + "\" in node \"" + this->GetCurrentNodeName() +
                          "\"" + c_XMLLineInfoText + " " + orc_ErrorMessage +  ".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for root node missing

   \param[in]  orc_RootNodeName  Root node name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserLog::m_ReportErrorForRootNodeMissing(const C_SCLString & orc_RootNodeName) const
{
   if (this->mc_LogHeading != "")
   {
      C_SCLString c_FoundRootNodeText;
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
void C_OSCXMLParserLog::ReportErrorForNodeMissing(const C_SCLString & orc_MissingNodeName) const
{
   if (this->mc_LogHeading != "")
   {
      const C_SCLString c_XMLLineInfoText = m_GetCurrentXMLLineInfoText();
      osc_write_log_error(this->mc_LogHeading,
                          "Could not find node \"" + orc_MissingNodeName + "\" in node \"" + this->GetCurrentNodeName() +
                          "\"" + c_XMLLineInfoText + ".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report error for attribute missing

   \param[in]  orc_AttributeName    Attribute name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCXMLParserLog::m_ReportErrorForAttributeMissing(const C_SCLString & orc_AttributeName) const
{
   if (this->mc_LogHeading != "")
   {
      const C_SCLString c_XMLLineInfoText = m_GetCurrentXMLLineInfoText();
      osc_write_log_error(this->mc_LogHeading,
                          "Could not find attribute \"" + orc_AttributeName + "\" in node \"" + this->GetCurrentNodeName() +
                          "\"" + c_XMLLineInfoText + ".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current XML line info text

   \return
   Current XML line info text
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCXMLParserLog::m_GetCurrentXMLLineInfoText() const
{
   C_SCLString c_Retval;
   const sintn sn_Line = this->GetFileLineForCurrentNode();

   if (sn_Line != 0)
   {
      c_Retval = " (XML line " + C_SCLString::IntToStr(this->GetFileLineForCurrentNode()) + ")";
   }

   return c_Retval;
}
