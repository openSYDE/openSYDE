//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Wrapper for C_OscXMLParser including error logging
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCXMLPARSERLOG_HPP
#define C_OSCXMLPARSERLOG_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscXmlParser.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscXmlParserLog :
   public C_OscXmlParser
{
public:
   C_OscXmlParserLog();

   void SetLogHeading(const stw::scl::C_SclString & orc_Text);

   // init node system - selects second node, if available; first node is declaration
   virtual int32_t SelectRootError(const stw::scl::C_SclString & orc_Name);

   // select first child node (with name) of current node        (go deeper)
   virtual int32_t SelectNodeChildError(const stw::scl::C_SclString & orc_Name);

   // get attribute values (includes error check)
   virtual int32_t GetAttributeStringError(const stw::scl::C_SclString & orc_Name,
                                           stw::scl::C_SclString & orc_Value) const;
   virtual int32_t GetAttributeSint32Error(const stw::scl::C_SclString & orc_Name, int32_t & ors32_Value) const;
   virtual int32_t GetAttributeUint32Error(const stw::scl::C_SclString & orc_Name, uint32_t & oru32_Value) const;
   virtual int32_t GetAttributeSint64Error(const stw::scl::C_SclString & orc_Name, int64_t & ors64_Value) const;
   virtual int32_t GetAttributeUint64Error(const stw::scl::C_SclString & orc_Name, uint64_t & oru64_Value) const;
   virtual int32_t GetAttributeBoolError(const stw::scl::C_SclString & orc_Name, bool & orq_Value) const;
   virtual int32_t GetAttributeFloat32Error(const stw::scl::C_SclString & orc_Name, float32_t & orf32_Value) const;
   virtual int32_t GetAttributeFloat64Error(const stw::scl::C_SclString & orc_Name, float64_t & orf64_Value) const;

   //Base error reporting functions
   virtual void ReportErrorForNodeContentAppendXmlContext(const stw::scl::C_SclString & orc_ErrorMessage)
   const;
   virtual void ReportErrorForAttributeContentAppendXmlContext(const stw::scl::C_SclString & orc_Attribute,
                                                               const stw::scl::C_SclString & orc_ErrorMessage)
   const;
   virtual void ReportErrorForNodeContentStartingWithXmlContext(const stw::scl::C_SclString & orc_ErrorMessage)
   const;
   virtual void ReportErrorForAttributeContentStartingWithXmlContext(const stw::scl::C_SclString & orc_Attribute,
                                                                     const stw::scl::C_SclString & orc_ErrorMessage)
   const;
   virtual void ReportErrorForNodeMissing(const stw::scl::C_SclString & orc_MissingNodeName) const;

private:
   stw::scl::C_SclString mc_LogHeading;

   void m_ReportErrorForRootNodeMissing(const stw::scl::C_SclString & orc_RootNodeName) const;
   void m_ReportErrorForAttributeMissing(const stw::scl::C_SclString & orc_AttributeName) const;
   stw::scl::C_SclString m_GetCurrentXmlLineInfoText(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
