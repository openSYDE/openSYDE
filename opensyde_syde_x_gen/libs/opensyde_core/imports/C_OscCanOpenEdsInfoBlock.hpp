//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for info block handling of EDS/DCF files
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANOPENEDSINFOBLOCK_HPP
#define C_OSCCANOPENEDSINFOBLOCK_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscCanOpenEdsFileInfoBlock.hpp"
#include "C_OscCanOpenEdsDeviceInfoBlock.hpp"
#include "C_SclIniFile.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanOpenEdsInfoBlock
{
public:
   C_OscCanOpenEdsInfoBlock();

   C_OscCanOpenEdsFileInfoBlock c_FileInfo;
   C_OscCanOpenEdsDeviceInfoBlock c_DeviceInfo;

   void CalcHash(uint32_t & oru32_HashValue) const;
   int32_t LoadFromFile(stw::scl::C_SclIniFile & orc_File, stw::scl::C_SclString & orc_LastError);

   uint8_t GetGranularity(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
