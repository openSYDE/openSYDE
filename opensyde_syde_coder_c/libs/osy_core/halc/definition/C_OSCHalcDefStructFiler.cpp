//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load HALC definition struct section

   Load HALC definition struct section

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>
#include <sstream>

#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCNodeDataPoolContentUtil.h"
#include "C_OSCHalcDefStructFiler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_scl::C_SCLString C_OSCHalcDefStructFiler::mhc_False = "FALSE";
const stw_scl::C_SCLString C_OSCHalcDefStructFiler::mhc_True = "TRUE";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO parameters

   \param[out]     orc_Structs            Storage
   \param[in,out]  orc_XMLParser          XML with default state
   \param[in]      orc_UseCases           All available channel use cases for this domain
   \param[in]      orc_CurrentNodeName    Current XML node name
   \param[in]      orc_SectionNodeName    XML node name of section to parse
   \param[in]      orc_GroupNodeName      XML node name of section elements to parse
   \param[in]      orc_SingleNodeName     XML node name of group elements to parse
   \param[in]      oq_RequireId           Flag if ID is required in this section
   \param[in]      oq_RequireSection      Flag if this entire section is required
   \param[in]      ou32_DomainNameLength  Domain name length

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefStructFiler::h_LoadStructs(std::vector<C_OSCHalcDefStruct> & orc_Structs,
                                              C_OSCXMLParserBase & orc_XMLParser,
                                              const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases,
                                              const stw_scl::C_SCLString & orc_CurrentNodeName,
                                              const stw_scl::C_SCLString & orc_SectionNodeName,
                                              const stw_scl::C_SCLString & orc_GroupNodeName,
                                              const stw_scl::C_SCLString & orc_SingleNodeName, const bool oq_RequireId,
                                              const bool oq_RequireSection, const uint32 ou32_DomainNameLength)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Structs.clear();
   if (orc_XMLParser.SelectNodeChild(orc_SectionNodeName) == orc_SectionNodeName)
   {
      stw_scl::C_SCLString c_NodeChannel = orc_XMLParser.SelectNodeChild(orc_GroupNodeName);
      if (c_NodeChannel == orc_GroupNodeName)
      {
         do
         {
            C_OSCHalcDefStruct c_Struct;
            s32_Retval = mh_LoadStruct(c_Struct, orc_XMLParser, orc_UseCases, orc_GroupNodeName, orc_SingleNodeName,
                                       oq_RequireId, ou32_DomainNameLength);
            if (s32_Retval == C_NO_ERR)
            {
               orc_Structs.push_back(c_Struct);
               c_NodeChannel = orc_XMLParser.SelectNodeNext(orc_GroupNodeName);
            }
         }
         while ((c_NodeChannel == orc_GroupNodeName) && (s32_Retval == C_NO_ERR));
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == orc_SectionNodeName);
         }
      }

      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == orc_CurrentNodeName);
      }
   }
   else
   {
      if (oq_RequireSection)
      {
         osc_write_log_error("Loading HALC definition",
                             stw_scl::C_SCLString(stw_scl::C_SCLString("Could not find \"") + orc_SectionNodeName +
                                                  "\" node.").c_str());
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save generic struct data

   \param[in]      orc_Structs            Generic struct data
   \param[in]      orc_UseCases           All available channel use cases for this domain
   \param[in,out]  orc_XMLParser          XML parser
   \param[in]      orc_CurrentNodeName    Current node name
   \param[in]      orc_SectionNodeName    Section node name
   \param[in]      orc_GroupNodeName      Group node name
   \param[in]      orc_SingleNodeName     Single node name

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefStructFiler::h_SaveStructs(const std::vector<C_OSCHalcDefStruct> & orc_Structs,
                                              const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases,
                                              C_OSCXMLParserBase & orc_XMLParser,
                                              const stw_scl::C_SCLString & orc_CurrentNodeName,
                                              const stw_scl::C_SCLString & orc_SectionNodeName,
                                              const stw_scl::C_SCLString & orc_GroupNodeName,
                                              const stw_scl::C_SCLString & orc_SingleNodeName)
{
   sint32 s32_Retval = C_NO_ERR;

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild(orc_SectionNodeName) == orc_SectionNodeName);
   for (uint32 u32_ItStruct = 0UL; (u32_ItStruct < orc_Structs.size()) && (s32_Retval == C_NO_ERR); ++u32_ItStruct)
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild(orc_GroupNodeName) == orc_GroupNodeName);
      s32_Retval = C_OSCHalcDefStructFiler::mh_SaveStruct(orc_Structs[u32_ItStruct], orc_UseCases, orc_XMLParser,
                                                          orc_GroupNodeName,
                                                          orc_SingleNodeName);
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == orc_SectionNodeName);
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == orc_CurrentNodeName);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse simplest data type values

   \param[in]      orc_TypeStr        Original type string
   \param[in,out]  orc_Content         Content to set
   \param[in,out]  orc_XMLParser       XML with default state
   \param[in]      orc_AttributeName   Attribute to look for value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcDefStructFiler::h_ParseSimplestTypeValue(const stw_scl::C_SCLString & orc_TypeStr,
                                                       C_OSCNodeDataPoolContent & orc_Content,
                                                       const C_OSCXMLParserBase & orc_XMLParser,
                                                       const stw_scl::C_SCLString & orc_AttributeName)
{
   switch (orc_Content.GetType())
   {
   case C_OSCNodeDataPoolContent::eUINT8:
      if (orc_TypeStr == "bool")
      {
         const stw_scl::C_SCLString c_ItemStr = orc_XMLParser.GetAttributeString(orc_AttributeName);
         if (c_ItemStr.LowerCase() == mhc_False.LowerCase())
         {
            orc_Content.SetValueU8(static_cast<uint8>(false));
         }
         else if (c_ItemStr.LowerCase() == mhc_True.LowerCase())
         {
            orc_Content.SetValueU8(static_cast<uint8>(true));
         }
         else
         {
            orc_Content.SetValueU8(static_cast<uint8>(orc_XMLParser.GetAttributeUint32(orc_AttributeName)));
         }
      }
      else
      {
         orc_Content.SetValueU8(static_cast<uint8>(orc_XMLParser.GetAttributeUint32(orc_AttributeName)));
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT16:
      orc_Content.SetValueU16(static_cast<uint16>(orc_XMLParser.GetAttributeUint32(orc_AttributeName)));
      break;
   case C_OSCNodeDataPoolContent::eUINT32:
      orc_Content.SetValueU32(orc_XMLParser.GetAttributeUint32(orc_AttributeName));
      break;
   case C_OSCNodeDataPoolContent::eUINT64:
      orc_Content.SetValueU64(orc_XMLParser.GetAttributeSint64(orc_AttributeName));
      break;
   case C_OSCNodeDataPoolContent::eSINT8:
      orc_Content.SetValueS8(static_cast<sint8>(orc_XMLParser.GetAttributeSint32(orc_AttributeName)));
      break;
   case C_OSCNodeDataPoolContent::eSINT16:
      orc_Content.SetValueS16(static_cast<sint16>(orc_XMLParser.GetAttributeSint32(orc_AttributeName)));
      break;
   case C_OSCNodeDataPoolContent::eSINT32:
      orc_Content.SetValueS32(orc_XMLParser.GetAttributeSint32(orc_AttributeName));
      break;
   case C_OSCNodeDataPoolContent::eSINT64:
      orc_Content.SetValueS64(orc_XMLParser.GetAttributeSint64(orc_AttributeName));
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32:
      orc_Content.SetValueF32(orc_XMLParser.GetAttributeFloat32(orc_AttributeName));
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64:
      orc_Content.SetValueF64(orc_XMLParser.GetAttributeFloat64(orc_AttributeName));
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse simplest data type values

   \param[in]   orc_TypeStr  Original type string
   \param[out]  ore_Type      Type value

   \return
   C_NO_ERR    data read
   C_CONFIG    String is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefStructFiler::h_GetTypeForSimplestTypeString(const stw_scl::C_SCLString & orc_TypeStr,
                                                               C_OSCNodeDataPoolContent::E_Type & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_TypeStr == "float64")
   {
      ore_Type = C_OSCNodeDataPoolContent::eFLOAT64;
   }
   else if (orc_TypeStr == "float32")
   {
      ore_Type = C_OSCNodeDataPoolContent::eFLOAT32;
   }
   else if (orc_TypeStr == "sint64")
   {
      ore_Type = C_OSCNodeDataPoolContent::eSINT64;
   }
   else if (orc_TypeStr == "sint32")
   {
      ore_Type = C_OSCNodeDataPoolContent::eSINT32;
   }
   else if (orc_TypeStr == "sint16")
   {
      ore_Type = C_OSCNodeDataPoolContent::eSINT16;
   }
   else if (orc_TypeStr == "sint8")
   {
      ore_Type = C_OSCNodeDataPoolContent::eSINT8;
   }
   else if (orc_TypeStr == "uint64")
   {
      ore_Type = C_OSCNodeDataPoolContent::eUINT64;
   }
   else if (orc_TypeStr == "uint32")
   {
      ore_Type = C_OSCNodeDataPoolContent::eUINT32;
   }
   else if (orc_TypeStr == "uint16")
   {
      ore_Type = C_OSCNodeDataPoolContent::eUINT16;
   }
   else if ((orc_TypeStr == "uint8") || (orc_TypeStr == "bool"))
   {
      ore_Type = C_OSCNodeDataPoolContent::eUINT8;
   }
   else
   {
      osc_write_log_error("Loading HALC definition", "Unexpected value for \"type\" attribute.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get type string

   \param[in]  oe_Type  Type

   \return
   Type string
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCHalcDefStructFiler::h_GetTypeString(const C_OSCNodeDataPoolContent::E_Type oe_Type)
{
   stw_scl::C_SCLString c_Retval;
   switch (oe_Type)
   {
   case C_OSCNodeDataPoolContent::eUINT8:
      c_Retval = "uint8";
      break;
   case C_OSCNodeDataPoolContent::eUINT16:
      c_Retval = "uint16";
      break;
   case C_OSCNodeDataPoolContent::eUINT32:
      c_Retval = "uint32";
      break;
   case C_OSCNodeDataPoolContent::eUINT64:
      c_Retval = "uint64";
      break;
   case C_OSCNodeDataPoolContent::eSINT8:
      c_Retval = "sint8";
      break;
   case C_OSCNodeDataPoolContent::eSINT16:
      c_Retval = "sint16";
      break;
   case C_OSCNodeDataPoolContent::eSINT32:
      c_Retval = "sint32";
      break;
   case C_OSCNodeDataPoolContent::eSINT64:
      c_Retval = "sint64";
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32:
      c_Retval = "float32";
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64:
      c_Retval = "float64";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save simple value type as string

   \param[in]      orc_Node         Attribute name
   \param[in,out]  orc_XMLParser    XML parser
   \param[in]      orc_Content      Content

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefStructFiler::h_SaveSimpleValueAsAttribute(const stw_scl::C_SCLString & orc_Node,
                                                             C_OSCXMLParserBase & orc_XMLParser,
                                                             const C_OSCNodeDataPoolContent & orc_Content)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_Content.GetArray() == false)
   {
      uint32 u32_Tmp;
      uint64 u64_Tmp;
      sint64 s64_Tmp;
      switch (orc_Content.GetType())
      {
      case C_OSCNodeDataPoolContent::eUINT8:
         u32_Tmp = orc_Content.GetValueU8();
         orc_XMLParser.SetAttributeUint32(orc_Node, u32_Tmp);
         break;
      case C_OSCNodeDataPoolContent::eUINT16:
         u32_Tmp = orc_Content.GetValueU16();
         orc_XMLParser.SetAttributeUint32(orc_Node, u32_Tmp);
         break;
      case C_OSCNodeDataPoolContent::eUINT32:
         u32_Tmp = orc_Content.GetValueU32();
         orc_XMLParser.SetAttributeUint32(orc_Node, u32_Tmp);
         break;
      case C_OSCNodeDataPoolContent::eUINT64:
         u64_Tmp = orc_Content.GetValueU64();
         orc_XMLParser.SetAttributeSint64(orc_Node, static_cast<sint64>(u64_Tmp));
         break;
      case C_OSCNodeDataPoolContent::eSINT8:
         s64_Tmp = orc_Content.GetValueS8();
         orc_XMLParser.SetAttributeSint64(orc_Node, s64_Tmp);
         break;
      case C_OSCNodeDataPoolContent::eSINT16:
         s64_Tmp = orc_Content.GetValueS16();
         orc_XMLParser.SetAttributeSint64(orc_Node, s64_Tmp);
         break;
      case C_OSCNodeDataPoolContent::eSINT32:
         s64_Tmp = orc_Content.GetValueS32();
         orc_XMLParser.SetAttributeSint64(orc_Node, s64_Tmp);
         break;
      case C_OSCNodeDataPoolContent::eSINT64:
         s64_Tmp = orc_Content.GetValueS64();
         orc_XMLParser.SetAttributeSint64(orc_Node, s64_Tmp);
         break;
      case C_OSCNodeDataPoolContent::eFLOAT32:
         orc_XMLParser.SetAttributeFloat32(orc_Node, orc_Content.GetValueF32());
         break;
      case C_OSCNodeDataPoolContent::eFLOAT64:
         orc_XMLParser.SetAttributeFloat64(orc_Node, orc_Content.GetValueF64());
         break;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
      osc_write_log_error("Saving HALC definition", "arrays not supported");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set type

   \param[in,out]  orc_XMLParser          XML parser
   \param[in,out]  orc_Content            Content
   \param[in,out]  orc_TypeStr            Type str
   \param[in,out]  orc_BaseTypeStr        Base type str
   \param[in]      orc_CurrentNodeName    Current node name
   \param[in]      orc_UseCaseForLog      Use case for log

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefStructFiler::h_SetType(C_OSCXMLParserBase & orc_XMLParser, C_OSCHalcDefContent & orc_Content,
                                          stw_scl::C_SCLString & orc_TypeStr, stw_scl::C_SCLString & orc_BaseTypeStr,
                                          const stw_scl::C_SCLString & orc_CurrentNodeName,
                                          const stw_scl::C_SCLString & orc_UseCaseForLog)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.AttributeExists("type"))
   {
      orc_TypeStr = orc_XMLParser.GetAttributeString("type");
   }
   else
   {
      osc_write_log_error("Loading HALC definition", "Could not find \"type\" attribute.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      orc_Content.SetArray(false);
      if (orc_TypeStr == "float64")
      {
         orc_Content.SetComplexType(C_OSCHalcDefContent::eCT_PLAIN);
         orc_Content.SetType(C_OSCNodeDataPoolContent::eFLOAT64);
      }
      else if (orc_TypeStr == "float32")
      {
         orc_Content.SetComplexType(C_OSCHalcDefContent::eCT_PLAIN);
         orc_Content.SetType(C_OSCNodeDataPoolContent::eFLOAT32);
      }
      else if (orc_TypeStr == "sint64")
      {
         orc_Content.SetComplexType(C_OSCHalcDefContent::eCT_PLAIN);
         orc_Content.SetType(C_OSCNodeDataPoolContent::eSINT64);
      }
      else if (orc_TypeStr == "sint32")
      {
         orc_Content.SetComplexType(C_OSCHalcDefContent::eCT_PLAIN);
         orc_Content.SetType(C_OSCNodeDataPoolContent::eSINT32);
      }
      else if (orc_TypeStr == "sint16")
      {
         orc_Content.SetComplexType(C_OSCHalcDefContent::eCT_PLAIN);
         orc_Content.SetType(C_OSCNodeDataPoolContent::eSINT16);
      }
      else if (orc_TypeStr == "sint8")
      {
         orc_Content.SetComplexType(C_OSCHalcDefContent::eCT_PLAIN);
         orc_Content.SetType(C_OSCNodeDataPoolContent::eSINT8);
      }
      else if (orc_TypeStr == "uint64")
      {
         orc_Content.SetComplexType(C_OSCHalcDefContent::eCT_PLAIN);
         orc_Content.SetType(C_OSCNodeDataPoolContent::eUINT64);
      }
      else if (orc_TypeStr == "uint32")
      {
         orc_Content.SetComplexType(C_OSCHalcDefContent::eCT_PLAIN);
         orc_Content.SetType(C_OSCNodeDataPoolContent::eUINT32);
      }
      else if (orc_TypeStr == "uint16")
      {
         orc_Content.SetComplexType(C_OSCHalcDefContent::eCT_PLAIN);
         orc_Content.SetType(C_OSCNodeDataPoolContent::eUINT16);
      }
      else if ((orc_TypeStr == "uint8") || (orc_TypeStr == "bool"))
      {
         orc_Content.SetComplexType(C_OSCHalcDefContent::eCT_PLAIN);
         orc_Content.SetType(C_OSCNodeDataPoolContent::eUINT8);
      }
      else if (orc_TypeStr == "enum")
      {
         if (orc_XMLParser.AttributeExists("base-type"))
         {
            orc_BaseTypeStr = orc_XMLParser.GetAttributeString("base-type");
         }
         else
         {
            osc_write_log_error("Loading HALC definition", "Could not find \"base-type\" attribute.");
            s32_Retval = C_CONFIG;
         }
         if (s32_Retval == C_NO_ERR)
         {
            C_OSCNodeDataPoolContent::E_Type e_Type;
            s32_Retval = C_OSCHalcDefStructFiler::h_GetTypeForSimplestTypeString(orc_BaseTypeStr, e_Type);
            orc_Content.SetType(e_Type);
            orc_Content.SetComplexType(C_OSCHalcDefContent::eCT_ENUM);
            if (s32_Retval == C_NO_ERR)
            {
               if ((e_Type == C_OSCNodeDataPoolContent::eFLOAT32) || (e_Type == C_OSCNodeDataPoolContent::eFLOAT64))
               {
                  osc_write_log_error(orc_UseCaseForLog.c_str(),
                                      "Float type not supported in \"base-type\" attribute.");
                  s32_Retval = C_CONFIG;
               }
            }
         }
      }
      else if (orc_TypeStr == "bitmask")
      {
         if (orc_XMLParser.AttributeExists("base-type"))
         {
            orc_BaseTypeStr = orc_XMLParser.GetAttributeString("base-type");
         }
         else
         {
            osc_write_log_error(orc_UseCaseForLog.c_str(), "Could not find \"base-type\" attribute.");
            s32_Retval = C_CONFIG;
         }
         if (s32_Retval == C_NO_ERR)
         {
            C_OSCNodeDataPoolContent::E_Type e_Type;
            s32_Retval = C_OSCHalcDefStructFiler::h_GetTypeForSimplestTypeString(orc_BaseTypeStr, e_Type);
            orc_Content.SetType(e_Type);
            orc_Content.SetComplexType(C_OSCHalcDefContent::eCT_BIT_MASK);
            if (s32_Retval == C_NO_ERR)
            {
               if ((((((e_Type == C_OSCNodeDataPoolContent::eFLOAT32) ||
                       (e_Type == C_OSCNodeDataPoolContent::eFLOAT64)) ||
                      (e_Type == C_OSCNodeDataPoolContent::eSINT8)) ||
                     (e_Type == C_OSCNodeDataPoolContent::eSINT16)) ||
                    (e_Type == C_OSCNodeDataPoolContent::eSINT32)) ||
                   (e_Type == C_OSCNodeDataPoolContent::eSINT64))
               {
                  osc_write_log_error(orc_UseCaseForLog.c_str(),
                                      "Float type and signed type not supported in \"base-type\" attribute.");
                  s32_Retval = C_CONFIG;
               }
            }
         }
      }
      else
      {
         osc_write_log_error(orc_UseCaseForLog.c_str(), "Unexpected value for \"type\" attribute.");
         s32_Retval = C_CONFIG;
      }
   }
   //Parse enum
   if (s32_Retval == C_NO_ERR)
   {
      stw_scl::C_SCLString c_EnumItemNode = orc_XMLParser.SelectNodeChild("enum-item");
      if (c_EnumItemNode == "enum-item")
      {
         if (orc_TypeStr != "enum")
         {
            osc_write_log_error("Loading HALC definition",
                                stw_scl::C_SCLString(stw_scl::C_SCLString("\"enum-item\" node found for type \"") +
                                                     orc_TypeStr + "\".").c_str());
            s32_Retval = C_CONFIG;
         }
         else
         {
            do
            {
               C_OSCNodeDataPoolContent c_Content = orc_Content;
               stw_scl::C_SCLString c_Display;
               if (orc_XMLParser.AttributeExists("display"))
               {
                  c_Display = orc_XMLParser.GetAttributeString("display");
               }
               else
               {
                  osc_write_log_error("Loading HALC definition",
                                      "\"enum-item\" node missing \"display\" attribute.");
                  s32_Retval = C_CONFIG;
               }
               if (s32_Retval == C_NO_ERR)
               {
                  if (orc_XMLParser.AttributeExists("value"))
                  {
                     h_ParseSimplestTypeValue(orc_BaseTypeStr, c_Content, orc_XMLParser, "value");
                  }
                  else
                  {
                     osc_write_log_error("Loading HALC definition",
                                         "\"enum-item\" node missing \"value\" attribute.");
                     s32_Retval = C_CONFIG;
                  }
               }
               if (s32_Retval == C_NO_ERR)
               {
                  if (orc_Content.AddEnumItem(c_Display, c_Content) != C_NO_ERR)
                  {
                     osc_write_log_error("Loading HALC definition",
                                         stw_scl::C_SCLString(stw_scl::C_SCLString("\"enum-item\" display name \"") +
                                                              c_Display +
                                                              "\" is duplicate").c_str());
                     s32_Retval = C_CONFIG;
                  }
               }
               if (s32_Retval == C_NO_ERR)
               {
                  c_EnumItemNode = orc_XMLParser.SelectNodeNext("enum-item");
               }
            }
            while ((c_EnumItemNode == "enum-item") && (s32_Retval == C_NO_ERR));
         }
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == orc_CurrentNodeName);
         }
      }
      else
      {
         if (orc_TypeStr == "enum")
         {
            osc_write_log_error("Loading HALC definition", "No \"enum-item\" node found for type enum.");
            s32_Retval = C_CONFIG;
         }
      }
   }
   //Parse bitmask
   if (s32_Retval == C_NO_ERR)
   {
      stw_scl::C_SCLString c_BitmaskItemNode = orc_XMLParser.SelectNodeChild("bitmask-selection");
      if (c_BitmaskItemNode == "bitmask-selection")
      {
         if (orc_TypeStr != "bitmask")
         {
            osc_write_log_error("Loading HALC definition",
                                stw_scl::C_SCLString(stw_scl::C_SCLString(
                                                        "\"bitmask-selection\" node found for type \"") + orc_TypeStr +
                                                     "\".").c_str());
            s32_Retval = C_CONFIG;
         }
         else
         {
            do
            {
               C_OSCHalcDefContentBitmaskItem c_BitmaskItem;
               if (orc_XMLParser.SelectNodeChild("comment") == "comment")
               {
                  c_BitmaskItem.c_Comment = orc_XMLParser.GetNodeContent();
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "bitmask-selection");
               }
               if (orc_XMLParser.AttributeExists("display"))
               {
                  c_BitmaskItem.c_Display = orc_XMLParser.GetAttributeString("display");
               }
               else
               {
                  osc_write_log_error("Loading HALC definition",
                                      "\"bitmask-selection\" node missing \"display\" attribute.");
                  s32_Retval = C_CONFIG;
               }
               if (s32_Retval == C_NO_ERR)
               {
                  if (orc_XMLParser.AttributeExists("initial-apply-value-setting"))
                  {
                     const stw_scl::C_SCLString c_Content = orc_XMLParser.GetAttributeString(
                        "initial-apply-value-setting");
                     if (c_Content.LowerCase() == mhc_False.LowerCase())
                     {
                        c_BitmaskItem.q_ApplyValueSetting = false;
                     }
                     else if (c_Content.LowerCase() == mhc_True.LowerCase())
                     {
                        c_BitmaskItem.q_ApplyValueSetting = true;
                     }
                     else
                     {
                        c_BitmaskItem.q_ApplyValueSetting = orc_XMLParser.GetAttributeBool(
                           "initial-apply-value-setting");
                     }
                  }
                  else
                  {
                     osc_write_log_error("Loading HALC definition",
                                         "\"bitmask-selection\" node missing \"initial-apply-value-setting\" attribute.");
                     s32_Retval = C_CONFIG;
                  }
               }
               if (s32_Retval == C_NO_ERR)
               {
                  if (orc_XMLParser.AttributeExists("value"))
                  {
                     const stw_scl::C_SCLString c_Content = orc_XMLParser.GetAttributeString("value");
                     if (c_BitmaskItem.SetValueByString(c_Content) !=
                         C_NO_ERR)
                     {
                        osc_write_log_error("Loading HALC definition",
                                            stw_scl::C_SCLString(stw_scl::C_SCLString(
                                                                    "\"value\" attribute content \"") +
                                                                 c_Content + "\" cannot be parsed.").c_str());
                        s32_Retval = C_CONFIG;
                     }
                  }
                  else
                  {
                     osc_write_log_error("Loading HALC definition",
                                         "\"bitmask-selection\" node missing \"value-mask\" attribute.");
                     s32_Retval = C_CONFIG;
                  }
               }
               if (s32_Retval == C_NO_ERR)
               {
                  //Add bitmask item
                  orc_Content.AddBitmaskItem(c_BitmaskItem);
                  //Select next one if any
                  c_BitmaskItemNode = orc_XMLParser.SelectNodeNext("bitmask-selection");
               }
            }
            while ((c_BitmaskItemNode == "bitmask-selection") && (s32_Retval == C_NO_ERR));
         }
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == orc_CurrentNodeName);
         }
      }
      else
      {
         if (orc_TypeStr == "bitmask")
         {
            osc_write_log_error("Loading HALC definition", "No \"bitmask-selection\" node found for type bitmask.");
            s32_Retval = C_CONFIG;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse attribute content

   \param[in,out]  orc_Content            Content to set
   \param[in,out]  orc_XMLParser          XML with default state
   \param[in]      orc_AttributeName      Attribute name to search for
   \param[in]      orc_Type               Type string
   \param[in]      orc_BaseType           Base type string
   \param[in]      oq_RequireAttribute    Flag if attribute is required
   \param[in]      orc_CurrentNodeName    Current node name

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefStructFiler::h_ParseAttributeIntoContent(C_OSCHalcDefContent & orc_Content,
                                                            const C_OSCXMLParserBase & orc_XMLParser,
                                                            const stw_scl::C_SCLString & orc_AttributeName,
                                                            const stw_scl::C_SCLString & orc_Type,
                                                            const stw_scl::C_SCLString & orc_BaseType,
                                                            const bool oq_RequireAttribute,
                                                            const stw_scl::C_SCLString & orc_CurrentNodeName)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.AttributeExists(orc_AttributeName))
   {
      if (orc_Type == "float64")
      {
         orc_Content.SetValueF64(orc_XMLParser.GetAttributeFloat64(orc_AttributeName));
      }
      else if (orc_Type == "float32")
      {
         orc_Content.SetValueF32(orc_XMLParser.GetAttributeFloat32(orc_AttributeName));
      }
      else if (orc_Type == "sint64")
      {
         orc_Content.SetValueS64(orc_XMLParser.GetAttributeSint64(orc_AttributeName));
      }
      else if (orc_Type == "sint32")
      {
         orc_Content.SetValueS32(orc_XMLParser.GetAttributeSint32(orc_AttributeName));
      }
      else if (orc_Type == "sint16")
      {
         orc_Content.SetValueS16(static_cast<sint16>(orc_XMLParser.GetAttributeSint32(orc_AttributeName)));
      }
      else if (orc_Type == "sint8")
      {
         orc_Content.SetValueS8(static_cast<sint8>(orc_XMLParser.GetAttributeSint32(orc_AttributeName)));
      }
      else if (orc_Type == "uint64")
      {
         orc_Content.SetValueU64(orc_XMLParser.GetAttributeSint64(orc_AttributeName));
      }
      else if (orc_Type == "uint32")
      {
         orc_Content.SetValueU32(orc_XMLParser.GetAttributeUint32(orc_AttributeName));
      }
      else if (orc_Type == "uint16")
      {
         orc_Content.SetValueU16(static_cast<uint16>(orc_XMLParser.GetAttributeUint32(orc_AttributeName)));
      }
      else if (orc_Type == "uint8")
      {
         orc_Content.SetValueU8(static_cast<uint8>(orc_XMLParser.GetAttributeUint32(orc_AttributeName)));
      }
      else if (orc_Type == "bool")
      {
         const stw_scl::C_SCLString c_ItemStr = orc_XMLParser.GetAttributeString(orc_AttributeName);
         if (c_ItemStr.LowerCase() == mhc_False.LowerCase())
         {
            orc_Content.SetValueU8(static_cast<uint8>(false));
         }
         else if (c_ItemStr.LowerCase() == mhc_True.LowerCase())
         {
            orc_Content.SetValueU8(static_cast<uint8>(true));
         }
         else
         {
            orc_Content.SetValueU8(static_cast<uint8>(orc_XMLParser.GetAttributeUint32(orc_AttributeName)));
         }
      }
      else if (orc_Type == "enum")
      {
         const stw_scl::C_SCLString c_ItemStr = orc_XMLParser.GetAttributeString(orc_AttributeName);
         //Try enum
         if (orc_Content.SetEnumValue(c_ItemStr) != C_NO_ERR)
         {
            //Try simple value
            h_ParseSimplestTypeValue(orc_BaseType, orc_Content, orc_XMLParser, orc_AttributeName);
         }
      }
      else if (orc_Type == "bitmask")
      {
         h_ParseSimplestTypeValue(orc_BaseType, orc_Content, orc_XMLParser, orc_AttributeName);
      }
      else
      {
         osc_write_log_error("Loading HALC definition",
                             stw_scl::C_SCLString(stw_scl::C_SCLString("Unexpected type \"") + orc_Type +
                                                  "\" in node \"" + orc_CurrentNodeName + "\".").c_str());
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      if (oq_RequireAttribute)
      {
         osc_write_log_error("Loading HALC definition",
                             stw_scl::C_SCLString(stw_scl::C_SCLString("Could not find \"") + orc_AttributeName +
                                                  "\" attribute in node \"" + orc_CurrentNodeName + "\".").c_str());
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcDefStructFiler::C_OSCHalcDefStructFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO parameter struct

   \param[out]     orc_Struct             Storage
   \param[in,out]  orc_XMLParser          XML with default state
   \param[in]      orc_UseCases           All available channel use cases for this domain
   \param[in]      orc_GroupNodeName      XML node name of section elements to parse
   \param[in]      orc_SingleNodeName     XML node name of group elements to parse
   \param[in]      oq_RequireId           Flag if ID is required in this section
   \param[in]      ou32_DomainNameLength  Domain name length

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefStructFiler::mh_LoadStruct(C_OSCHalcDefStruct & orc_Struct, C_OSCXMLParserBase & orc_XMLParser,
                                              const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases,
                                              const stw_scl::C_SCLString & orc_GroupNodeName,
                                              const stw_scl::C_SCLString & orc_SingleNodeName, const bool oq_RequireId,
                                              const uint32 ou32_DomainNameLength)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.AttributeExists("type"))
   {
      const stw_scl::C_SCLString c_Type = orc_XMLParser.GetAttributeString("type");
      if (c_Type == "struct")
      {
         if (orc_XMLParser.AttributeExists("id"))
         {
            orc_Struct.c_Id = orc_XMLParser.GetAttributeString("id");
         }
         else
         {
            if (oq_RequireId)
            {
               osc_write_log_error("Loading HALC definition", "Could not find \"id\" attribute.");
               s32_Retval = C_CONFIG;
            }
         }
         if (s32_Retval == C_NO_ERR)
         {
            if (orc_XMLParser.AttributeExists("display"))
            {
               orc_Struct.c_Display = orc_XMLParser.GetAttributeString("display");
               if ((orc_Struct.c_Display.Length() + ou32_DomainNameLength) >
                   C_OSCHalcDefStructFiler::hu32_MAX_ALLOWED_COMBINED_VARIABLE_LENGTH)
               {
                  osc_write_log_error("Loading HALC definition",
                                      stw_scl::C_SCLString(stw_scl::C_SCLString("Content of \"display\" attribute \"") +
                                                           orc_Struct.c_Display +
                                                           "\" is too long to be combined with domain name, maximum allowed characters of combined value: "
                                                           +
                                                           stw_scl::C_SCLString::IntToStr(C_OSCHalcDefStructFiler::
                                                                                          hu32_MAX_ALLOWED_COMBINED_VARIABLE_LENGTH)
                                                           +
                                                           " (Current: " +
                                                           stw_scl::C_SCLString::IntToStr(orc_Struct.c_Display.Length()
                                                                                          +
                                                                                          ou32_DomainNameLength) +
                                                           ")."));
                  s32_Retval = C_CONFIG;
               }
            }
            else
            {
               osc_write_log_error("Loading HALC definition", "Could not find \"display\" attribute.");
               s32_Retval = C_CONFIG;
            }
         }
         if (s32_Retval == C_NO_ERR)
         {
            if (orc_XMLParser.AttributeExists("availability"))
            {
               const stw_scl::C_SCLString c_Availability = orc_XMLParser.GetAttributeString("availability");
               s32_Retval = C_OSCHalcDefStructFiler::mh_ParseAttributeAvailability(
                  orc_Struct.c_UseCaseAvailabilities,
                  c_Availability, orc_UseCases);
            }
            else
            {
               osc_write_log_error("Loading HALC definition", "Could not find \"availability\" attribute.");
               s32_Retval = C_CONFIG;
            }
         }
         if (s32_Retval == C_NO_ERR)
         {
            stw_scl::C_SCLString c_NodeParameter;
            if (orc_XMLParser.SelectNodeChild("comment") == "comment")
            {
               orc_Struct.c_Comment = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == orc_GroupNodeName);
            }
            c_NodeParameter = orc_XMLParser.SelectNodeChild(orc_SingleNodeName);
            if (c_NodeParameter == orc_SingleNodeName)
            {
               do
               {
                  C_OSCHalcDefElement c_Element;
                  s32_Retval = mh_LoadDataElement(c_Element, orc_XMLParser, orc_UseCases,
                                                  orc_Struct.c_UseCaseAvailabilities,
                                                  orc_SingleNodeName, oq_RequireId, ou32_DomainNameLength);
                  if (s32_Retval == C_NO_ERR)
                  {
                     orc_Struct.c_StructElements.push_back(c_Element);
                     c_NodeParameter = orc_XMLParser.SelectNodeNext(orc_SingleNodeName);
                  }
               }
               while ((c_NodeParameter == orc_SingleNodeName) && (s32_Retval == C_NO_ERR));
               if (s32_Retval == C_NO_ERR)
               {
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == orc_GroupNodeName);
               }
            }
         }
      }
      else
      {
         s32_Retval = mh_LoadDataElement(orc_Struct, orc_XMLParser, orc_UseCases, orc_Struct.c_UseCaseAvailabilities,
                                         orc_GroupNodeName, oq_RequireId, ou32_DomainNameLength);
      }
   }
   else
   {
      osc_write_log_error("Loading HALC definition", "Could not find \"type\" attribute.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save generic struct data

   \param[in]      orc_Struct          Generic struct data
   \param[in]      orc_UseCases        All available channel use cases for this domain
   \param[in,out]  orc_XMLParser       XML parser
   \param[in]      orc_GroupNodeName   Group node name
   \param[in]      orc_SingleNodeName  Single node name

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefStructFiler::mh_SaveStruct(const C_OSCHalcDefStruct & orc_Struct,
                                              const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases,
                                              C_OSCXMLParserBase & orc_XMLParser,
                                              const stw_scl::C_SCLString & orc_GroupNodeName,
                                              const stw_scl::C_SCLString & orc_SingleNodeName)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_Struct.c_StructElements.size() > 0UL)
   {
      stw_scl::C_SCLString c_Availability;
      if (orc_Struct.c_Id.IsEmpty() == false)
      {
         orc_XMLParser.SetAttributeString("id", orc_Struct.c_Id);
      }
      orc_XMLParser.SetAttributeString("display", orc_Struct.c_Display);
      orc_XMLParser.SetAttributeString("type", "struct");
      s32_Retval = mh_GetAvailabilityString(orc_Struct.c_UseCaseAvailabilities, orc_UseCases, c_Availability);
      if (s32_Retval == C_NO_ERR)
      {
         orc_XMLParser.SetAttributeString("availability", c_Availability);
         tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("comment") == "comment");
         orc_XMLParser.SetNodeContent(orc_Struct.c_Comment);
         tgl_assert(orc_XMLParser.SelectNodeParent() == orc_GroupNodeName);
         for (uint32 u32_ItElem = 0UL; (u32_ItElem < orc_Struct.c_StructElements.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItElem)
         {
            tgl_assert(orc_XMLParser.CreateAndSelectNodeChild(orc_SingleNodeName) == orc_SingleNodeName);
            s32_Retval = mh_SaveDataElement(orc_Struct.c_StructElements[u32_ItElem], orc_UseCases, orc_XMLParser,
                                            orc_SingleNodeName);
            tgl_assert(orc_XMLParser.SelectNodeParent() == orc_GroupNodeName);
         }
      }
   }
   else
   {
      s32_Retval = mh_SaveDataElement(orc_Struct, orc_UseCases, orc_XMLParser, orc_GroupNodeName);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO parameter

   \param[out]     orc_Element            Storage
   \param[in,out]  orc_XMLParser          XML with default state
   \param[in]      orc_UseCases           All available channel use cases for this domain
   \param[in]      orc_BaseAvailability   Availability of base node
   \param[in]      orc_SingleNodeName     XML node name of group elements to parse
   \param[in]      oq_RequireId           Flag if ID is required in this section
   \param[in]      ou32_DomainNameLength  Domain name length

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefStructFiler::mh_LoadDataElement(C_OSCHalcDefElement & orc_Element,
                                                   C_OSCXMLParserBase & orc_XMLParser,
                                                   const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases,
                                                   const std::vector<stw_types::uint32> & orc_BaseAvailability,
                                                   const stw_scl::C_SCLString & orc_SingleNodeName,
                                                   const bool oq_RequireId, const uint32 ou32_DomainNameLength)
{
   sint32 s32_Retval = C_NO_ERR;

   stw_scl::C_SCLString c_TypeStr;
   stw_scl::C_SCLString c_BaseTypeStr;

   if (orc_XMLParser.SelectNodeChild("comment") == "comment")
   {
      orc_Element.c_Comment = orc_XMLParser.GetNodeContent();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == orc_SingleNodeName);
   }
   if (orc_XMLParser.AttributeExists("id"))
   {
      orc_Element.c_Id = orc_XMLParser.GetAttributeString("id");
   }
   else
   {
      if (oq_RequireId)
      {
         osc_write_log_error("Loading HALC definition", "Could not find \"id\" attribute.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.AttributeExists("display"))
      {
         orc_Element.c_Display = orc_XMLParser.GetAttributeString("display");
         if ((orc_Element.c_Display.Length() + ou32_DomainNameLength) >
             C_OSCHalcDefStructFiler::hu32_MAX_ALLOWED_COMBINED_VARIABLE_LENGTH)
         {
            osc_write_log_error("Loading HALC definition",
                                stw_scl::C_SCLString(stw_scl::C_SCLString("Content of \"display\" attribute \"") +
                                                     orc_Element.c_Display +
                                                     "\" is too long to be combined with domain name, maximum allowed characters of combined value: "
                                                     +
                                                     stw_scl::C_SCLString::IntToStr(C_OSCHalcDefStructFiler::
                                                                                    hu32_MAX_ALLOWED_COMBINED_VARIABLE_LENGTH)
                                                     +
                                                     " (Current: " +
                                                     stw_scl::C_SCLString::IntToStr(orc_Element.c_Display.Length() +
                                                                                    ou32_DomainNameLength) +
                                                     ")."));
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         osc_write_log_error("Loading HALC definition", "Could not find \"display\" attribute.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.AttributeExists("availability"))
      {
         const stw_scl::C_SCLString c_Availability = orc_XMLParser.GetAttributeString("availability");
         s32_Retval = C_OSCHalcDefStructFiler::mh_ParseAttributeAvailability(orc_Element.c_UseCaseAvailabilities,
                                                                             c_Availability, orc_UseCases);
      }
      else
      {
         if (orc_BaseAvailability.size() > 0UL)
         {
            orc_Element.c_UseCaseAvailabilities = orc_BaseAvailability;
         }
         else
         {
            //Assuming all
            s32_Retval = C_OSCHalcDefStructFiler::mh_ParseAttributeAvailability(orc_Element.c_UseCaseAvailabilities,
                                                                                "all", orc_UseCases);
         }
      }
   }
   //Parse content first
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = h_SetType(orc_XMLParser, orc_Element.c_InitialValue, c_TypeStr, c_BaseTypeStr, orc_SingleNodeName,
                             "Loading HALC definition");
      if (s32_Retval == C_NO_ERR)
      {
         //Initialize all the same
         orc_Element.c_MaxValue = orc_Element.c_InitialValue;
         orc_Element.c_MinValue = orc_Element.c_InitialValue;
      }
   }

   //Parse attributes last (might require enum values!)
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.AttributeExists("min-value"))
      {
         s32_Retval = h_ParseAttributeIntoContent(orc_Element.c_MinValue, orc_XMLParser, "min-value", c_TypeStr,
                                                  c_BaseTypeStr, true, orc_SingleNodeName);
      }
      else
      {
         //Default
         if ((c_TypeStr == "enum") || (c_TypeStr == "bitmask"))
         {
            mh_SetMinValForType(c_BaseTypeStr, orc_Element.c_MinValue);
         }
         else
         {
            mh_SetMinValForType(c_TypeStr, orc_Element.c_MinValue);
         }
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.AttributeExists("max-value"))
      {
         s32_Retval = h_ParseAttributeIntoContent(orc_Element.c_MaxValue, orc_XMLParser, "max-value", c_TypeStr,
                                                  c_BaseTypeStr, true, orc_SingleNodeName);
      }
      else
      {
         //Default
         if ((c_TypeStr == "enum") || (c_TypeStr == "bitmask"))
         {
            mh_SetMaxValForType(c_BaseTypeStr, orc_Element.c_MaxValue);
         }
         else
         {
            mh_SetMaxValForType(c_TypeStr, orc_Element.c_MaxValue);
         }
      }
   }
   //Parse init value last as this also requires min and max to already be set
   if (s32_Retval == C_NO_ERR)
   {
      //Default zero
      C_OSCNodeDataPoolContentUtil::h_ZeroContent(orc_Element.c_InitialValue);
      //Overwrite if attribute present
      s32_Retval = h_ParseAttributeIntoContent(orc_Element.c_InitialValue, orc_XMLParser, "initial-value", c_TypeStr,
                                               c_BaseTypeStr,
                                               orc_Element.GetComplexType() != C_OSCHalcDefContent::eCT_BIT_MASK,
                                               orc_SingleNodeName);
      if (s32_Retval == C_NO_ERR)
      {
         C_OSCNodeDataPoolContentUtil::E_ValueChangedTo e_Tmp;
         //Check if value in range
         s32_Retval = C_OSCNodeDataPoolContentUtil::h_SetValueInMinMaxRange(orc_Element.c_MinValue,
                                                                            orc_Element.c_MaxValue,
                                                                            orc_Element.c_InitialValue, e_Tmp,
                                                                            C_OSCNodeDataPoolContentUtil::eLEAVE_VALUE);
         if (s32_Retval != C_NO_ERR)
         {
            osc_write_log_error("Loading HALC definition",
                                "\"initial-value\" attribute value could not be set in min max range");
            s32_Retval = C_CONFIG;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save data element

   \param[in]      orc_Element         Element
   \param[in]      orc_UseCases        All available channel use cases for this domain
   \param[in,out]  orc_XMLParser       XML parser
   \param[in]      orc_SingleNodeName  Single node name

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefStructFiler::mh_SaveDataElement(const C_OSCHalcDefElement & orc_Element,
                                                   const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases,
                                                   C_OSCXMLParserBase & orc_XMLParser,
                                                   const stw_scl::C_SCLString & orc_SingleNodeName)
{
   sint32 s32_Retval = C_NO_ERR;
   const stw_scl::C_SCLString c_BaseType = h_GetTypeString(orc_Element.GetType());

   if (orc_Element.c_Id.IsEmpty() == false)
   {
      orc_XMLParser.SetAttributeString("id", orc_Element.c_Id);
   }
   orc_XMLParser.SetAttributeString("display", orc_Element.c_Display);
   //Comment before any other child elements
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("comment") == "comment");
   orc_XMLParser.SetNodeContent(orc_Element.c_Comment);
   tgl_assert(orc_XMLParser.SelectNodeParent() == orc_SingleNodeName);
   //Type
   switch (orc_Element.GetComplexType())
   {
   case C_OSCHalcDefContent::eCT_BIT_MASK:
      {
         const std::vector<C_OSCHalcDefContentBitmaskItem> & rc_BitmaskItems = orc_Element.GetBitmaskItems();
         orc_XMLParser.SetAttributeString("type", "bitmask");
         orc_XMLParser.SetAttributeString("base-type", c_BaseType);
         //Items
         for (uint32 u32_It = 0UL; (u32_It < rc_BitmaskItems.size()) && (s32_Retval == C_NO_ERR); ++u32_It)
         {
            const C_OSCHalcDefContentBitmaskItem & rc_Bitmask = rc_BitmaskItems[u32_It];
            std::stringstream c_Mask;
            tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("bitmask-selection") == "bitmask-selection");
            orc_XMLParser.SetAttributeString("display", rc_Bitmask.c_Display);
            orc_XMLParser.SetAttributeBool("initial-apply-value-setting", rc_Bitmask.q_ApplyValueSetting);
            c_Mask << "0x" << &std::hex << rc_Bitmask.u64_Value;
            orc_XMLParser.SetAttributeString("value", c_Mask.str().c_str());
            tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("comment") == "comment");
            orc_XMLParser.SetNodeContent(rc_Bitmask.c_Comment);
            tgl_assert(orc_XMLParser.SelectNodeParent() == "bitmask-selection");
            tgl_assert(orc_XMLParser.SelectNodeParent() == orc_SingleNodeName);
         }
      }
      break;
   case C_OSCHalcDefContent::eCT_ENUM:
      {
         const std::map<stw_scl::C_SCLString, C_OSCNodeDataPoolContent> & rc_EnumItems = orc_Element.GetEnumItems();
         orc_XMLParser.SetAttributeString("type", "enum");
         orc_XMLParser.SetAttributeString("base-type", c_BaseType);
         //Items
         for (std::map<stw_scl::C_SCLString, C_OSCNodeDataPoolContent>::const_iterator c_It = rc_EnumItems.begin();
              (c_It != rc_EnumItems.end()) && (s32_Retval == C_NO_ERR); ++c_It)
         {
            tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("enum-item") == "enum-item");
            orc_XMLParser.SetAttributeString("display", c_It->first);
            s32_Retval = h_SaveSimpleValueAsAttribute("value", orc_XMLParser, c_It->second);
            if (s32_Retval == C_NO_ERR)
            {
               tgl_assert(orc_XMLParser.SelectNodeParent() == orc_SingleNodeName);
            }
         }
      }
      break;
   case C_OSCHalcDefContent::eCT_PLAIN:
      orc_XMLParser.SetAttributeString("type", c_BaseType);
      break;
   }
   //Handle values
   s32_Retval = h_SaveSimpleValueAsAttribute("initial-value", orc_XMLParser, orc_Element.c_InitialValue);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = h_SaveSimpleValueAsAttribute("max-value", orc_XMLParser, orc_Element.c_MaxValue);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = h_SaveSimpleValueAsAttribute("min-value", orc_XMLParser, orc_Element.c_MinValue);
   }
   if (s32_Retval == C_NO_ERR)
   {
      stw_scl::C_SCLString c_Availability;
      s32_Retval = mh_GetAvailabilityString(orc_Element.c_UseCaseAvailabilities, orc_UseCases, c_Availability);
      if (s32_Retval == C_NO_ERR)
      {
         orc_XMLParser.SetAttributeString("availability", c_Availability);
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse availability string

   \param[out]  orc_Availability       Parsed availability output
   \param[in]   orc_AttributeContent   Availability string to parse
   \param[in]   orc_UseCases           All known use-cases

   \return
   C_NO_ERR    data read
   C_CONFIG    string invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefStructFiler::mh_ParseAttributeAvailability(std::vector<stw_types::uint32> & orc_Availability,
                                                              const stw_scl::C_SCLString & orc_AttributeContent,
                                                              const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_AttributeContent.LowerCase() == "all")
   {
      for (uint32 u32_ItUseCase = 0UL; u32_ItUseCase < orc_UseCases.size(); ++u32_ItUseCase)
      {
         orc_Availability.push_back(u32_ItUseCase);
      }
   }
   else
   {
      stw_scl::SCLDynamicArray<stw_scl::C_SCLString> c_SplittedString;
      orc_AttributeContent.Tokenize(",", c_SplittedString);
      for (sint32 s32_ItSplit = 0L; s32_ItSplit < c_SplittedString.GetLength(); ++s32_ItSplit)
      {
         const stw_scl::C_SCLString c_CurSplit = c_SplittedString[s32_ItSplit].Trim();
         bool q_Found = false;
         for (uint32 u32_ItUseCase = 0UL; u32_ItUseCase < orc_UseCases.size(); ++u32_ItUseCase)
         {
            const C_OSCHalcDefChannelUseCase & rc_CurUseCase = orc_UseCases[u32_ItUseCase];
            if (rc_CurUseCase.c_Id == c_CurSplit)
            {
               q_Found = true;
               orc_Availability.push_back(u32_ItUseCase);
               break;
            }
         }
         if (q_Found == false)
         {
            osc_write_log_error("Loading HALC definition",
                                stw_scl::C_SCLString(stw_scl::C_SCLString(
                                                        "Could not match \"availability\" attribute content \"")
                                                     +
                                                     c_CurSplit + "\" to any use case.").c_str());
            s32_Retval = C_CONFIG;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get availability string

   \param[in]      orc_Availability    Availability
   \param[in]      orc_UseCases        Use cases
   \param[in,out]  orc_Output          Output

   \return
   Availability string
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_OSCHalcDefStructFiler::mh_GetAvailabilityString(
   const std::vector<stw_types::uint32> & orc_Availability,
   const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases, stw_scl::C_SCLString & orc_Output)
{
   stw_types::sint32 s32_Retval = C_NO_ERR;
   orc_Output = "";
   for (uint32 u32_It = 0UL; u32_It < orc_Availability.size(); ++u32_It)
   {
      if (orc_Availability[u32_It] < orc_UseCases.size())
      {
         const C_OSCHalcDefChannelUseCase & rc_Availability = orc_UseCases[orc_Availability[u32_It]];
         if (orc_Output.IsEmpty() == false)
         {
            orc_Output += ",";
         }
         orc_Output += rc_Availability.c_Id;
      }
      else
      {
         osc_write_log_error("Saving HALC definition",
                             stw_scl::C_SCLString(stw_scl::C_SCLString(
                                                     "Availability index ") +
                                                  stw_scl::C_SCLString::IntToStr(orc_Availability[u32_It]) +
                                                  " no longer in range of use-case count " +
                                                  stw_scl::C_SCLString::IntToStr(orc_UseCases.size())).c_str());
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set maximum value for type

   \param[in]      orc_TypeStr   Type
   \param[in,out]  orc_Content   Content to set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcDefStructFiler::mh_SetMaxValForType(const stw_scl::C_SCLString & orc_TypeStr,
                                                  C_OSCNodeDataPoolContent & orc_Content)
{
   if (orc_TypeStr == "float64")
   {
      orc_Content.SetValueF64(std::numeric_limits<float64>::max());
   }
   else if (orc_TypeStr == "float32")
   {
      orc_Content.SetValueF32(std::numeric_limits<float32>::max());
   }
   else if (orc_TypeStr == "sint64")
   {
      orc_Content.SetValueS64(std::numeric_limits<sint64>::max());
   }
   else if (orc_TypeStr == "sint32")
   {
      orc_Content.SetValueS32(std::numeric_limits<sint32>::max());
   }
   else if (orc_TypeStr == "sint16")
   {
      orc_Content.SetValueS16(std::numeric_limits<sint16>::max());
   }
   else if (orc_TypeStr == "sint8")
   {
      orc_Content.SetValueS8(std::numeric_limits<sint8>::max());
   }
   else if (orc_TypeStr == "uint64")
   {
      orc_Content.SetValueU64(std::numeric_limits<uint64>::max());
   }
   else if (orc_TypeStr == "uint32")
   {
      orc_Content.SetValueU32(std::numeric_limits<uint32>::max());
   }
   else if (orc_TypeStr == "uint16")
   {
      orc_Content.SetValueU16(std::numeric_limits<uint16>::max());
   }
   else if (orc_TypeStr == "uint8")
   {
      orc_Content.SetValueU8(std::numeric_limits<uint8>::max());
   }
   else if (orc_TypeStr == "bool")
   {
      orc_Content.SetValueU8(1);
   }
   else
   {
      //Unexpected
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set minimum value for type

   \param[in]      orc_TypeStr   Type
   \param[in,out]  orc_Content   Content to set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcDefStructFiler::mh_SetMinValForType(const stw_scl::C_SCLString & orc_TypeStr,
                                                  C_OSCNodeDataPoolContent & orc_Content)
{
   if (orc_TypeStr == "float64")
   {
      orc_Content.SetValueF64(-std::numeric_limits<float64>::max());
   }
   else if (orc_TypeStr == "float32")
   {
      orc_Content.SetValueF32(-std::numeric_limits<float32>::max());
   }
   else if (orc_TypeStr == "sint64")
   {
      orc_Content.SetValueS64(std::numeric_limits<sint64>::min());
   }
   else if (orc_TypeStr == "sint32")
   {
      orc_Content.SetValueS32(std::numeric_limits<sint32>::min());
   }
   else if (orc_TypeStr == "sint16")
   {
      orc_Content.SetValueS16(std::numeric_limits<sint16>::min());
   }
   else if (orc_TypeStr == "sint8")
   {
      orc_Content.SetValueS8(std::numeric_limits<sint8>::min());
   }
   else if (orc_TypeStr == "uint64")
   {
      orc_Content.SetValueU64(std::numeric_limits<uint64>::min());
   }
   else if (orc_TypeStr == "uint32")
   {
      orc_Content.SetValueU32(std::numeric_limits<uint32>::min());
   }
   else if (orc_TypeStr == "uint16")
   {
      orc_Content.SetValueU16(std::numeric_limits<uint16>::min());
   }
   else if (orc_TypeStr == "uint8")
   {
      orc_Content.SetValueU8(std::numeric_limits<uint8>::min());
   }
   else if (orc_TypeStr == "bool")
   {
      orc_Content.SetValueU8(0);
   }
   else
   {
      //Unexpected
   }
}
