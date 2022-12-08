//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief      Wrapper class for tinyxml2

   \class      stw::opensyde_core::C_OscXMLParser
   \brief      Wrapper class for tinyxml2

   Based on an pre-existing implementation for rapidxml.
   Documented and modified to fit into openSYDE structure and conventions.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCXMLPARSERHPP
#define C_OSCXMLPARSERHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <list>
#include "tinyxml2.h"
#include "stwtypes.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///one XML attribute with its value
class C_OscXmlAttribute
{
public:
   stw::scl::C_SclString c_Name;  ///< name of attribute
   stw::scl::C_SclString c_Value; ///< value of attribute
};

//----------------------------------------------------------------------------------------------------------------------

///General XML handling
class C_OscXmlParserBase
{
private:
   C_OscXmlParserBase(const C_OscXmlParserBase & orc_Source);               ///< not implemented: prevent copying
   C_OscXmlParserBase & operator = (const C_OscXmlParserBase & orc_Source); ///< not implemented: prevent assignment

   tinyxml2::XMLElement * mpc_CurrentNode;

protected:
   tinyxml2::XMLDocument mc_Document;

   void m_Init(void);

public:
   // set up class
   C_OscXmlParserBase(void);
   virtual ~C_OscXmlParserBase(void);

   // init node system - selects second node, if available; first node is declaration
   stw::scl::C_SclString SelectRoot(void);
   virtual int32_t SelectRootError(const stw::scl::C_SclString & orc_Name);

   // node navigation - returns current node
   // select first/next node (with name) after current node      (go forward)
   stw::scl::C_SclString SelectNodeNext(const stw::scl::C_SclString & orc_Name = "");
   // select first child node (with name) of current node        (go deeper)
   stw::scl::C_SclString SelectNodeChild(const stw::scl::C_SclString & orc_Name = "");
   virtual int32_t SelectNodeChildError(const stw::scl::C_SclString & orc_Name);
   // select parent node of current node                         (go up)
   stw::scl::C_SclString SelectNodeParent(void);

   //append new child node under the current node with content
   // if the document is still empty this will create the root node
   void CreateNodeChild(const stw::scl::C_SclString & orc_Name, const stw::scl::C_SclString & orc_Content = "");
   //append new child node under the current node and select it; returns new child node name
   stw::scl::C_SclString CreateAndSelectNodeChild(const stw::scl::C_SclString & orc_Name);
   // delete current node
   stw::scl::C_SclString DeleteNode(void);

   // node text content
   void SetNodeContent(const stw::scl::C_SclString & orc_Content);
   stw::scl::C_SclString GetNodeContent(void) const;

   // node attribute operations
   bool AttributeExists(const stw::scl::C_SclString & orc_Name) const;

   stw::scl::C_SclString GetCurrentNodeName(void) const;
   uint32_t GetFileLineForCurrentNode(void) const;

   // set attribute values
   void SetAttributeString(const stw::scl::C_SclString & orc_Name, const stw::scl::C_SclString & orc_Value);
   void SetAttributeSint32(const stw::scl::C_SclString & orc_Name, const int32_t os32_Value);
   void SetAttributeUint32(const stw::scl::C_SclString & orc_Name, const uint32_t ou32_Value);
   void SetAttributeSint64(const stw::scl::C_SclString & orc_Name, const int64_t os64_Value);
   void SetAttributeUint64(const stw::scl::C_SclString & orc_Name, const uint64_t ou64_Value);
   void SetAttributeBool(const stw::scl::C_SclString & orc_Name, const bool oq_Value);
   void SetAttributeFloat32(const stw::scl::C_SclString & orc_Name, const float32_t of32_Value);
   void SetAttributeFloat64(const stw::scl::C_SclString & orc_Name, const float64_t of64_Value);

   // get attribute values
   stw::scl::C_SclString GetAttributeString(const stw::scl::C_SclString & orc_Name,
                                            const stw::scl::C_SclString & orc_Default = "") const;
   int32_t GetAttributeSint32(const stw::scl::C_SclString & orc_Name, const int32_t os32_Default = 0L) const;
   uint32_t GetAttributeUint32(const stw::scl::C_SclString & orc_Name, const uint32_t ou32_Default = 0UL) const;
   int64_t GetAttributeSint64(const stw::scl::C_SclString & orc_Name, const int64_t os64_Default = 0LL) const;
   uint64_t GetAttributeUint64(const stw::scl::C_SclString & orc_Name, const uint64_t ou64_Default = 0ULL) const;
   bool GetAttributeBool(const stw::scl::C_SclString & orc_Name, const bool oq_Default = false) const;
   float32_t GetAttributeFloat32(const stw::scl::C_SclString & orc_Name, const float32_t of32_Default = 0.0F) const;
   float64_t GetAttributeFloat64(const stw::scl::C_SclString & orc_Name, const float64_t of64_Default = 0.0) const;

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

   //Base reporting functions
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

   // get all attributes
   std::vector<C_OscXmlAttribute> GetAttributes(void) const;
};

//----------------------------------------------------------------------------------------------------------------------

///file based parsing
class C_OscXmlParser :
   public C_OscXmlParserBase
{
private:
   //not implemented -> prevent copying
   C_OscXmlParser(const C_OscXmlParser & orc_Source);
   //not implemented -> prevent assignment
   C_OscXmlParser & operator = (const C_OscXmlParser & orc_Source); //lint !e1511 //we want to hide the base function

public:
   C_OscXmlParser(void);
   virtual ~C_OscXmlParser(void);

   // open xml file; create XML declaration
   virtual int32_t LoadFromFile(const stw::scl::C_SclString & orc_FileName);
   virtual int32_t SaveToFile(const stw::scl::C_SclString & orc_FileName);

   // parse xml string; create XML declaration
   int32_t LoadFromString(const stw::scl::C_SclString & orc_String);
   void SaveToString(stw::scl::C_SclString & orc_String) const;
};

//----------------------------------------------------------------------------------------------------------------------

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
