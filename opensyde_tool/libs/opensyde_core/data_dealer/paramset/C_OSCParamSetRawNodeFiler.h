//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Raw parameter set file reader/writer (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     12.10.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCPARAMSETRAWNODEFILER_H
#define C_OSCPARAMSETRAWNODEFILER_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "C_OSCParamSetRawNode.h"
#include "C_OSCParamSetFilerBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
///Filer for C_OSCParamSetRawNode
class C_OSCParamSetRawNodeFiler :
   public C_OSCParamSetFilerBase
{
public:
   static stw_types::sint32 h_LoadRawNode(C_OSCParamSetRawNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveRawNode(const C_OSCParamSetRawNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser);

private:
   C_OSCParamSetRawNodeFiler(void);

   static stw_types::sint32 mh_LoadEntries(std::vector<C_OSCParamSetRawEntry> & orc_Entries,
                                           C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveEntries(const std::vector<C_OSCParamSetRawEntry> & orc_Entries,
                              C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadEntry(C_OSCParamSetRawEntry & orc_Entry, C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveEntry(const C_OSCParamSetRawEntry & orc_Entry, C_OSCXMLParserBase & orc_XMLParser);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
