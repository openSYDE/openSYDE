//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Parameter set file reader/writer base (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     12.10.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCPARAMSETFILERBASE_H
#define C_OSCPARAMSETFILERBASE_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCXMLParser.h"
#include "C_OSCParamSetDataPoolInfo.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

///filer class to read and write parameter set files
class C_OSCParamSetFilerBase
{
public:
   static stw_types::sint32 h_AddCRC(const stw_scl::C_SCLString & orc_Path);
   static stw_types::sint32 h_CheckFileVersion(C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveFileVersion(C_OSCXMLParserBase & orc_XMLParser);

protected:
   C_OSCParamSetFilerBase(void);

   static stw_types::sint32 h_LoadNodeName(stw_scl::C_SCLString & orc_Name, C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNodeName(const stw_scl::C_SCLString & orc_Name, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPoolInfos(std::vector<C_OSCParamSetDataPoolInfo> & orc_DataPoolInfos,
                                                C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPoolInfos(const std::vector<C_OSCParamSetDataPoolInfo> & orc_DataPoolInfos,
                                   C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPoolInfo(C_OSCParamSetDataPoolInfo & orc_DataPoolInfo,
                                               C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPoolInfo(const C_OSCParamSetDataPoolInfo & orc_DataPoolInfo,
                                  C_OSCXMLParserBase & orc_XMLParser);

private:
   static stw_types::uint16 mhu16_FileVersion;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
