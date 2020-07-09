//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for parameter set file operations (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETHANDLER_H
#define C_OSCPARAMSETHANDLER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCXMLParser.h"
#include "C_OSCParamSetInterpretedData.h"
#include "C_OSCParamSetRawNode.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///utility class providing access to parameter set files
class C_OSCParamSetHandler
{
public:
   C_OSCParamSetHandler(void);

   //File
   stw_types::sint32 CreateCleanFileWithoutCRC(const stw_scl::C_SCLString & orc_FilePath,
                                               const bool oq_InterpretedDataOnly = false);
   stw_types::sint32 ReadFile(const stw_scl::C_SCLString & orc_FilePath, const bool oq_IgnoreCrc,
                              const bool oq_InterpretedDataOnly = false, stw_types::uint16 * const opu16_FileCrc = NULL,
                              bool * const opq_MissingOptionalContent = NULL);
   static stw_types::sint32 h_UpdateCRCForFile(const stw_scl::C_SCLString & orc_FilePath);

   //Data
   void ClearContent(void);
   stw_types::sint32 AddRawDataForNode(const C_OSCParamSetRawNode & orc_Content);
   void AddInterpretedFileData(const C_OSCParamSetInterpretedFileInfoData & orc_FileInfo);
   stw_types::sint32 AddInterpretedDataForNode(const C_OSCParamSetInterpretedNode & orc_Content);
   const C_OSCParamSetRawNode * GetRawDataForNode(const stw_scl::C_SCLString & orc_NodeName) const;
   const C_OSCParamSetInterpretedData & GetInterpretedData(void) const;

   stw_types::uint32 GetNumberOfNodes(void) const;
   const C_OSCParamSetRawNode * GetRawDataForNode(const stw_types::uint32 ou32_NodeIndex) const;

private:
   C_OSCParamSetInterpretedData mc_Data;
   std::vector<C_OSCParamSetRawNode> mc_RawNodes;

   stw_types::sint32 m_LoadNodes(C_OSCXMLParser & orc_XMLParser, const bool oq_InterpretedDataOnly,
                                 bool & orq_MissingOptionalContent);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
