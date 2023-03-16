//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export parameter set image for an openSYDE node.

   See cpp file for detailed description

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCEXPORTPARAMSET_HPP
#define C_OSCEXPORTPARAMSET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"
#include "C_OscNode.hpp"
#include "C_OscParamSetHandler.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscExportParamSet
{
public:
   static stw::scl::C_SclString h_GetFileName(const C_OscNodeApplication & orc_DataBlock, const bool oq_IsSafe);
   static int32_t h_CreateParameterSetImage(const stw::scl::C_SclString & orc_Path, const C_OscNode & orc_Node,
                                            const uint16_t ou16_ApplicationIndex,
                                            std::vector<stw::scl::C_SclString> & orc_Files,
                                            const stw::scl::C_SclString & orc_ExportToolName = "",
                                            const stw::scl::C_SclString & orc_ExportToolVersion = "");

private:
   static int32_t mh_FillPsiStructure(const C_OscNode & orc_Node, const bool oq_IsSafe,
                                      const uint16_t ou16_ApplicationIndex, C_OscParamSetRawNode & orc_RawNode,
                                      C_OscParamSetInterpretedNode & orc_IntNode);
   static void mh_FillInterpretedDatapool(const stw::opensyde_core::C_OscNodeDataPool & orc_SdDataPool,
                                          stw::opensyde_core::C_OscParamSetInterpretedDataPool & orc_IntDataPool);
   static int32_t mh_FillRawEntries(const stw::opensyde_core::C_OscNodeDataPool & orc_SdDataPool,
                                    const std::vector<uint8_t> & orc_ConfigRawBytes,
                                    std::vector<C_OscParamSetRawEntry> & orc_Entries);
   static C_OscParamSetInterpretedFileInfoData mh_GetFileInfo(const stw::scl::C_SclString & orc_ExportToolName,
                                                              const stw::scl::C_SclString & orc_ExportToolVersion);
   static int32_t mh_WriteParameterSetImage(const C_OscParamSetRawNode & orc_RawNode,
                                            const C_OscParamSetInterpretedNode & orc_IntNode, const bool oq_IsSafe,
                                            const C_OscNodeApplication & orc_DataBlock,
                                            const stw::scl::C_SclString & orc_Path,
                                            std::vector<stw::scl::C_SclString> & orc_Files,
                                            const stw::scl::C_SclString & orc_ExportToolName,
                                            const stw::scl::C_SclString & orc_ExportToolVersion);

   static int32_t mh_InsertCrc16(std::vector<uint8_t> & orc_Bytes);
   static int32_t mh_GetConfigurationRawBytes(const stw::opensyde_core::C_OscNodeDataPool & orc_SdDataPool,
                                              std::vector<uint8_t> & orc_Bytes);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
