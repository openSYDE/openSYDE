//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Raw parameter set file reader/writer (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETRAWNODEFILER_H
#define C_OSCPARAMSETRAWNODEFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OscParamSetRawNode.hpp"
#include "C_OscParamSetFilerBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
///Filer for C_OscParamSetRawNode
class C_OscParamSetRawNodeFiler :
   public C_OscParamSetFilerBase
{
public:
   static int32_t h_LoadRawNode(C_OscParamSetRawNode & orc_Node, C_OscXmlParserBase & orc_XmlParser,
                                bool & orq_MissingOptionalContent);
   static void h_SaveRawNode(const C_OscParamSetRawNode & orc_Node, C_OscXmlParserBase & orc_XmlParser);

private:
   C_OscParamSetRawNodeFiler(void);

   static int32_t mh_LoadEntries(std::vector<C_OscParamSetRawEntry> & orc_Entries, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveEntries(const std::vector<C_OscParamSetRawEntry> & orc_Entries,
                              C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadEntry(C_OscParamSetRawEntry & orc_Entry, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveEntry(const C_OscParamSetRawEntry & orc_Entry, C_OscXmlParserBase & orc_XmlParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
