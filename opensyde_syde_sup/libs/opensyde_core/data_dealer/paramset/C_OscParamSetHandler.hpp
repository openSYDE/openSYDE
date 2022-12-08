//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for parameter set file operations (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETHANDLER_HPP
#define C_OSCPARAMSETHANDLER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscParamSetInterpretedData.hpp"
#include "C_OscParamSetRawNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///utility class providing access to parameter set files
class C_OscParamSetHandler
{
public:
   C_OscParamSetHandler(void);

   //File
   int32_t CreateCleanFileWithoutCrc(const stw::scl::C_SclString & orc_FilePath,
                                     const bool oq_InterpretedDataOnly = false);
   int32_t ReadFile(const stw::scl::C_SclString & orc_FilePath, const bool oq_IgnoreCrc,
                    const bool oq_InterpretedDataOnly = false, uint16_t * const opu16_FileCrc = NULL,
                    bool * const opq_MissingOptionalContent = NULL);
   static int32_t h_UpdateCrcForFile(const stw::scl::C_SclString & orc_FilePath);

   //Data
   void ClearContent(void);
   int32_t AddRawDataForNode(const C_OscParamSetRawNode & orc_Content);
   void AddInterpretedFileData(const C_OscParamSetInterpretedFileInfoData & orc_FileInfo);
   int32_t AddInterpretedDataForNode(const C_OscParamSetInterpretedNode & orc_Content);
   const C_OscParamSetRawNode * GetRawDataForNode(const stw::scl::C_SclString & orc_NodeName) const;
   const C_OscParamSetInterpretedData & GetInterpretedData(void) const;

   uint32_t GetNumberOfNodes(void) const;
   const C_OscParamSetRawNode * GetRawDataForNode(const uint32_t ou32_NodeIndex) const;

private:
   C_OscParamSetInterpretedData mc_Data;
   std::vector<C_OscParamSetRawNode> mc_RawNodes;

   int32_t m_LoadNodes(C_OscXmlParser & orc_XmlParser, const bool oq_InterpretedDataOnly,
                       bool & orq_MissingOptionalContent);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
