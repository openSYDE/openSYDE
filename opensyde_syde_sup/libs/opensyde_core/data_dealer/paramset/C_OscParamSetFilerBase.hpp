//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Parameter set file reader/writer base (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETFILERBASE_HPP
#define C_OSCPARAMSETFILERBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscParamSetDataPoolInfo.hpp"
#include "C_OscParamSetInterpretedFileInfoData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///filer class to read and write parameter set files
class C_OscParamSetFilerBase
{
public:
   static int32_t h_AddCrc(const stw::scl::C_SclString & orc_Path);
   static int32_t h_CheckFileVersion(C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveFileVersion(C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveFileInfo(C_OscXmlParserBase & orc_XmlParser,
                              const C_OscParamSetInterpretedFileInfoData & orc_FileInfo);
   static void h_LoadFileInfo(C_OscXmlParserBase & orc_XmlParser, C_OscParamSetInterpretedFileInfoData & orc_FileInfo,
                              bool & orq_MissingOptionalContent);

protected:
   C_OscParamSetFilerBase(void);

   static int32_t mh_LoadNodeName(stw::scl::C_SclString & orc_Name, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveNodeName(const stw::scl::C_SclString & orc_Name, C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadDataPoolInfos(std::vector<C_OscParamSetDataPoolInfo> & orc_DataPoolInfos,
                                       C_OscXmlParserBase & orc_XmlParser, bool & orq_MissingOptionalContent);
   static void mh_SaveDataPoolInfos(const std::vector<C_OscParamSetDataPoolInfo> & orc_DataPoolInfos,
                                    C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadDataPoolInfo(C_OscParamSetDataPoolInfo & orc_DataPoolInfo, C_OscXmlParserBase & orc_XmlParser,
                                      bool & orq_MissingOptionalContent);
   static void mh_SaveDataPoolInfo(const C_OscParamSetDataPoolInfo & orc_DataPoolInfo,
                                   C_OscXmlParserBase & orc_XmlParser);

private:
   static uint16_t mhu16_FileVersion;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
