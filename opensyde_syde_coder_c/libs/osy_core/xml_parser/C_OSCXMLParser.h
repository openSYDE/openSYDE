//-----------------------------------------------------------------------------
/*!
   \file
   \brief      Wrapper class for tinyxml2

   \class      stw_opensyde_core::C_OSCXMLParser
   \brief      Wrapper class for tinyxml2

   Based on an pre-existing implementation for rapidxml.
   Documented and modfied to fit into openSYDE structure and conventions.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.09.2016  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCXMLPARSERH
#define C_OSCXMLPARSERH

/* -- Includes ------------------------------------------------------------- */
#include <list>
#include "tinyxml2.h"
#include "stwtypes.h"
#include "CSCLString.h"

/* -- Namespace ------------------------------------------------------------ */

namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

///one XML attribute with its value
class C_OSCXMLAttribute
{
public:
   stw_scl::C_SCLString c_Name;  ///< name of attribute
   stw_scl::C_SCLString c_Value; ///< value of attribute
};

//-----------------------------------------------------------------------------

///General xml handling
class C_OSCXMLParserBase
{
private:
   C_OSCXMLParserBase(const C_OSCXMLParserBase & orc_Souce);               ///< not implemented -> prevent copying
   C_OSCXMLParserBase & operator = (const C_OSCXMLParserBase & orc_Souce); ///< not implemented -> prevent assignment

   tinyxml2::XMLElement * mpc_CurrentNode;

protected:
   tinyxml2::XMLDocument mc_Document;

   void m_Init(void);

public:
   // set up class
   C_OSCXMLParserBase(void);
   virtual ~C_OSCXMLParserBase(void);

   // init node system - selects second node, if available; first node is declaration
   stw_scl::C_SCLString SelectRoot(void);

   // node navigation - returns current node
   // select first/next node (with name) after current node      (go forward)
   stw_scl::C_SCLString SelectNodeNext(const stw_scl::C_SCLString & orc_Name = "");
   // select first child node (with name) of current node        (go deeper)
   stw_scl::C_SCLString SelectNodeChild(const stw_scl::C_SCLString & orc_Name = "");
   // select parent node of current node                         (go up)
   stw_scl::C_SCLString SelectNodeParent(void);

   //append new child node under the current node with content
   // if the document is still empty this will create the root node
   void CreateNodeChild(const stw_scl::C_SCLString & orc_Name, const stw_scl::C_SCLString & orc_Content = "");
   //append new child node under the current node and select it; returns new child node name
   stw_scl::C_SCLString CreateAndSelectNodeChild(const stw_scl::C_SCLString & orc_Name);
   // delete current node
   stw_scl::C_SCLString DeleteNode(void);

   // node text content
   void SetNodeContent(const stw_scl::C_SCLString & orc_Content);
   stw_scl::C_SCLString GetNodeContent(void) const;

   // node attribute operations
   bool AttributeExists(const stw_scl::C_SCLString & orc_Name) const;

   stw_scl::C_SCLString GetCurrentNodeName(void) const
   {
      return (mpc_CurrentNode == NULL) ? "" : mpc_CurrentNode->Name();
   }

   // set attribute values
   void SetAttributeString(const stw_scl::C_SCLString & orc_Name, const stw_scl::C_SCLString & orc_Value);
   void SetAttributeSint32(const stw_scl::C_SCLString & orc_Name, const stw_types::sint32 os32_Value);
   void SetAttributeUint32(const stw_scl::C_SCLString & orc_Name, const stw_types::uint32 ou32_Value);
   void SetAttributeSint64(const stw_scl::C_SCLString & orc_Name, const stw_types::sint64 os64_Value);
   void SetAttributeBool(const stw_scl::C_SCLString & orc_Name, const bool oq_Value);
   void SetAttributeFloat32(const stw_scl::C_SCLString & orc_Name, const stw_types::float32 of32_Value);
   void SetAttributeFloat64(const stw_scl::C_SCLString & orc_Name, const stw_types::float64 of64_Value);

   // get attribute values
   stw_scl::C_SCLString GetAttributeString(const stw_scl::C_SCLString & orc_Name) const;
   stw_types::sint32 GetAttributeSint32(const stw_scl::C_SCLString & orc_Name) const;
   stw_types::uint32 GetAttributeUint32(const stw_scl::C_SCLString & orc_Name) const;
   stw_types::sint64 GetAttributeSint64(const stw_scl::C_SCLString & orc_Name) const;
   bool GetAttributeBool(const stw_scl::C_SCLString & orc_Name) const;
   stw_types::float32 GetAttributeFloat32(const stw_scl::C_SCLString & orc_Name) const;
   stw_types::float64 GetAttributeFloat64(const stw_scl::C_SCLString & orc_Name) const;

   // get all attributes
   std::vector<C_OSCXMLAttribute> GetAttributes(void) const;
};

//-----------------------------------------------------------------------------

///file based parsing
class C_OSCXMLParser :
   public C_OSCXMLParserBase
{
private:
   C_OSCXMLParser(const C_OSCXMLParser & orc_Souce);               ///< not implemented -> prevent copying
   C_OSCXMLParser & operator = (const C_OSCXMLParser & orc_Souce); ///< not implemented -> prevent assignment

public:
   C_OSCXMLParser(void);
   virtual ~C_OSCXMLParser(void);

   // open xml file; create XML declaration
   virtual stw_types::sint32 LoadFromFile(const stw_scl::C_SCLString & orc_FileName);
   virtual stw_types::sint32 SaveToFile(const stw_scl::C_SCLString & orc_FileName);
};

//-----------------------------------------------------------------------------

///string based parsing
class C_OSCXMLParserString :
   public C_OSCXMLParserBase
{
private:
   C_OSCXMLParserString(const C_OSCXMLParserString & orc_Souce);               ///< not implemented -> prevent copying
   C_OSCXMLParserString & operator = (const C_OSCXMLParserString & orc_Souce); ///< not implemented -> prevent
   ///< assignment

public:
   C_OSCXMLParserString(void);
   virtual ~C_OSCXMLParserString(void);

   // parse xml string; create XML declaration
   stw_types::sint32 LoadFromString(const stw_scl::C_SCLString & orc_String);
   void SaveToString(stw_scl::C_SCLString & orc_String) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
