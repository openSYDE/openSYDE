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

#include "stwtypes.h"

#include "CSCLStringList.h"
#include "C_OSCNode.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
///Code exporter for server side DPD and DPH init structures
class C_OSCExportOsyInit
{
public:
   C_OSCExportOsyInit(void);
   virtual ~C_OSCExportOsyInit(void);

   static stw_types::sint32 h_CreateSourceCode(const stw_scl::C_SCLString & orc_FilePath, const C_OSCNode & orc_Node,
                                               const bool oq_RunsDpd, const stw_types::uint16 ou16_ApplicationIndex,
                                               const stw_scl::C_SCLString & orc_ExportToolInfo = "");

   //Minimum buffer size required for DPD-services
   //greatest size for openSYDE server: routine control DP meta data)
   //ETH-Header + Service: (8+2+2) + 5 + (1+3) + (1+1+32)
   static const stw_types::uint8 hu8_MIN_SIZE_DPD_BUF_INSTANCE = 55U;

private:
   static bool mh_IsDpdInitRequired(const C_OSCNodeComInterfaceSettings & orc_Settings);
   static stw_types::uint32 mh_GetSizeOfLargestDataPoolElement(const std::vector<C_OSCNodeDataPool> & orc_DataPools);
   static bool mh_IsDpKnownToApp(const stw_types::uint8 ou8_DataPoolIndex,
                                 const stw_types::uint16 ou16_ApplicationIndex, const C_OSCNode & orc_Node,
                                 const bool oq_RunsDpd);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
