//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export code of an openSYDE node (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCEXPORTNODE_H
#define C_OSCEXPORTNODE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OscNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
///Code exporter for server code: Datapools, comm configuration, DPD and DPH initialization
class C_OscExportNode
{
public:
   static int32_t h_CreateSourceCode(const C_OscNode & orc_Node, const uint16_t ou16_ApplicationIndex,
                                     const stw::scl::C_SclString & orc_Path,
                                     std::vector<stw::scl::C_SclString> & orc_Files,
                                     const stw::scl::C_SclString & orc_ExportToolName = "",
                                     const stw::scl::C_SclString & orc_ExportToolVersion = "");

private:
   static int32_t mh_CreateOsyInitCode(const C_OscNode & orc_Node, const uint16_t ou16_ApplicationIndex,
                                       const stw::scl::C_SclString & orc_Path,
                                       std::vector<stw::scl::C_SclString> & orc_Files,
                                       const stw::scl::C_SclString & orc_ExportToolInfo = "");
   static int32_t mh_CreateDatapoolCode(const C_OscNode & orc_Node, const uint16_t ou16_ApplicationIndex,
                                        const stw::scl::C_SclString & orc_Path,
                                        std::vector<stw::scl::C_SclString> & orc_Files,
                                        const stw::scl::C_SclString & orc_ExportToolInfo = "");
   static int32_t mh_CreateCommStackCode(const C_OscNode & orc_Node, const uint16_t ou16_ApplicationIndex,
                                         const stw::scl::C_SclString & orc_Path,
                                         std::vector<stw::scl::C_SclString> & orc_Files,
                                         const stw::scl::C_SclString & orc_ExportToolInfo = "");
   static int32_t mh_CreateHalConfigCode(const C_OscNode & orc_Node, const uint16_t ou16_ApplicationIndex,
                                         const stw::scl::C_SclString & orc_Path,
                                         std::vector<stw::scl::C_SclString> & orc_Files,
                                         const stw::scl::C_SclString & orc_ExportToolInfo = "");
   static int32_t mh_CreateHalNvmData(const C_OscNode & orc_Node, const uint16_t ou16_ApplicationIndex,
                                      const stw::scl::C_SclString & orc_Path,
                                      std::vector<stw::scl::C_SclString> & orc_Files,
                                      const stw::scl::C_SclString & orc_ExportToolName,
                                      const stw::scl::C_SclString & orc_ExportToolVersion);

   static int32_t mh_CheckPrerequisites(const C_OscNode & orc_Node);
   static int32_t mh_GetAdaptedComDataPool(const C_OscNode & orc_Node, const uint32_t ou32_DataPoolIndex,
                                           C_OscNodeDataPool & orc_DataPool);
   static int32_t mh_GetHalDataPoolIndices(const stw::opensyde_core::C_OscNode & orc_Node, std::
                                           map<bool, int32_t> & orc_HalcDataPools);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
