//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Wrapper for C_OSCXMLParser including error logging
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCXMLPARSERLOG_H
#define C_OSCXMLPARSERLOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCXMLParser.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCXMLParserLog :
   public C_OSCXMLParser
{
public:
   C_OSCXMLParserLog();

   void SetLogHeading(const stw_scl::C_SCLString & orc_Text);

   // init node system - selects second node, if available; first node is declaration
   virtual stw_types::sint32 SelectRootError(const stw_scl::C_SCLString & orc_Name);

   // select first child node (with name) of current node        (go deeper)
   virtual stw_types::sint32 SelectNodeChildError(const stw_scl::C_SCLString & orc_Name);

   // get attribute values (includes error check)
   virtual stw_types::sint32 GetAttributeStringError(const stw_scl::C_SCLString & orc_Name,
                                                     stw_scl::C_SCLString & orc_Value) const;
   virtual stw_types::sint32 GetAttributeSint32Error(const stw_scl::C_SCLString & orc_Name,
                                                     stw_types::sint32 & ors32_Value) const;
   virtual stw_types::sint32 GetAttributeUint32Error(const stw_scl::C_SCLString & orc_Name,
                                                     stw_types::uint32 & oru32_Value) const;
   virtual stw_types::sint32 GetAttributeSint64Error(const stw_scl::C_SCLString & orc_Name,
                                                     stw_types::sint64 & ors64_Value) const;
   virtual stw_types::sint32 GetAttributeUint64Error(const stw_scl::C_SCLString & orc_Name,
                                                     stw_types::uint64 & oru64_Value) const;
   virtual stw_types::sint32 GetAttributeBoolError(const stw_scl::C_SCLString & orc_Name, bool & orq_Value) const;
   virtual stw_types::sint32 GetAttributeFloat32Error(const stw_scl::C_SCLString & orc_Name,
                                                      stw_types::float32 & orf32_Value) const;
   virtual stw_types::sint32 GetAttributeFloat64Error(const stw_scl::C_SCLString & orc_Name,
                                                      stw_types::float64 & orf64_Value) const;

   //Base error reporting functions
   virtual void ReportErrorForNodeContentAppendXMLContext(const stw_scl::C_SCLString & orc_ErrorMessage)
   const;
   virtual void ReportErrorForAttributeContentAppendXMLContext(const stw_scl::C_SCLString & orc_Attribute,
                                                               const stw_scl::C_SCLString & orc_ErrorMessage)
   const;
   virtual void ReportErrorForNodeContentStartingWithXMLContext(const stw_scl::C_SCLString & orc_ErrorMessage)
   const;
   virtual void ReportErrorForAttributeContentStartingWithXMLContext(const stw_scl::C_SCLString & orc_Attribute,
                                                                     const stw_scl::C_SCLString & orc_ErrorMessage)
   const;
   virtual void ReportErrorForNodeMissing(const stw_scl::C_SCLString & orc_MissingNodeName) const;

private:
   stw_scl::C_SCLString mc_LogHeading;

   void m_ReportErrorForRootNodeMissing(const stw_scl::C_SCLString & orc_RootNodeName) const;
   void m_ReportErrorForAttributeMissing(const stw_scl::C_SCLString & orc_AttributeName) const;
   stw_scl::C_SCLString m_GetCurrentXMLLineInfoText(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
