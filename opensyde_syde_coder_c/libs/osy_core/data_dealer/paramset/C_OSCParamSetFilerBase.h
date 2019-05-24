//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Parameter set file reader/writer base (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETFILERBASE_H
#define C_OSCPARAMSETFILERBASE_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCXMLParser.h"
#include "C_OSCParamSetDataPoolInfo.h"
#include "C_OSCParamSetInterpretedFileInfoData.h"

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
   static void h_SaveFileInfo(C_OSCXMLParserBase & orc_XMLParser,
                              const C_OSCParamSetInterpretedFileInfoData & orc_FileInfo);
   static void h_LoadFileInfo(C_OSCXMLParserBase & orc_XMLParser,
                              C_OSCParamSetInterpretedFileInfoData & orc_FileInfo);

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
