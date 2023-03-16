//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Hex file class

   openSYDE utility layer on top of the generic hex file class

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSYHEXFILEHPP
#define C_OSYHEXFILEHPP

#include "stwtypes.hpp"
#include "CXFLHexFile.hpp"
#include "C_SclString.hpp"
#include "C_SclDynamicArray.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace stw
{
namespace opensyde_core
{
//----------------------------------------------------------------------------------------------------------------------
///Comfort layer on top of the hex file class
class C_OsyHexFile :
   public stw::diag_lib::C_XFLHexFile
{
public:
   int32_t GetSignatureBlockAddress(uint32_t & oru32_Address);
   int32_t ScanDeviceIdFromHexFile(stw::scl::C_SclString & orc_DeviceId);
   int32_t ScanApplicationInformationBlockFromHexFile(stw::diag_lib::C_XFLECUInformation & orc_InfoBlock);
};

//----------------------------------------------------------------------------------------------------------------------
}
}

#endif
