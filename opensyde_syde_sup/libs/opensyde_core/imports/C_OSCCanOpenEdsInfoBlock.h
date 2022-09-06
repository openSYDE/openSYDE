//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for info block handling of EDS/DCF files
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANOPENEDSINFOBLOCK_H
#define C_OSCCANOPENEDSINFOBLOCK_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCCanOpenEdsFileInfoBlock.h"
#include "C_OSCCanOpenEdsDeviceInfoBlock.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCCanOpenEdsInfoBlock
{
public:
   C_OSCCanOpenEdsInfoBlock();

   stw_types::uint32 u32_FileHash;            ///< Content of loaded EDS file, parsed for GUI usage
   stw_scl::C_SCLString c_FileContentForSave; ///< Content of loaded EDS file to use when saving to file
   C_OSCCanOpenEdsFileInfoBlock c_FileInfo;
   C_OSCCanOpenEdsDeviceInfoBlock c_DeviceInfo;

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   stw_types::sint32 LoadFromFile(const stw_scl::C_SCLString & orc_FilePath, stw_scl::C_SCLString & orc_LastError);

   stw_types::uint8 GetGranularity(void) const;

private:
   void m_InitHash(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
