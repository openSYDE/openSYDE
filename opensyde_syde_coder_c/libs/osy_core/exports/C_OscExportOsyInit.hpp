//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export initialization module for DPD and DPH.

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCEXPORTOSYINITH
#define C_OSCEXPORTOSYINITH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.hpp"

#include "C_SclStringList.hpp"
#include "C_OscNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
///Code exporter for server side DPD and DPH init structures
class C_OscExportOsyInit
{
public:
   static stw::scl::C_SclString h_GetFileName(void);
   static int32_t h_CreateSourceCode(const stw::scl::C_SclString & orc_FilePath, const C_OscNode & orc_Node,
                                     const bool oq_RunsDpd, const uint16_t ou16_ApplicationIndex,
                                     const stw::scl::C_SclString & orc_ExportToolInfo = "");

   //Minimum buffer size required for DPD-services
   //greatest size for openSYDE server: WriteDataByIdentifier::SetKey
   //Calculation: (8 + 2 + 2) [ETH-Header] + 3 [WriteDataByIdentifierHeader] + (128 + 4 + 20) [SecurityKeyPayload]
   static const uint8_t hu8_MIN_SIZE_DPD_BUF_INSTANCE = 167U;

private:
   static bool mh_IsDpdInitRequired(const C_OscNodeComInterfaceSettings & orc_Settings);
   static uint32_t mh_GetSizeOfLargestDataPoolElement(const std::vector<C_OscNodeDataPool> & orc_DataPools);
   static bool mh_IsDpKnownToApp(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ApplicationIndex,
                                 const C_OscNode & orc_Node, const bool oq_RunsDpd);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
