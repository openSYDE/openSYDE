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

   uint32_t u32_FileHash;                      ///< Content of loaded EDS file, parsed for GUI usage
   stw::scl::C_SclString c_FileContentForSave; ///< Content of loaded EDS file to use when saving to file
   C_OscCanOpenEdsFileInfoBlock c_FileInfo;
   C_OscCanOpenEdsDeviceInfoBlock c_DeviceInfo;

   void CalcHash(uint32_t & oru32_HashValue) const;
   int32_t LoadFromFile(const stw::scl::C_SclString & orc_FilePath, stw::scl::C_SclString & orc_LastError);

   uint8_t GetGranularity(void) const;

private:
   void m_InitHash(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
