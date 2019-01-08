//-----------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Hex file class

   openSYDE utility layer on top of the generic hex file class

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.09.2017  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSYHEXFILEH
#define C_OSYHEXFILEH

#include "stwtypes.h"
#include "CXFLHexFile.h"
#include "CSCLString.h"
#include "SCLDynamicArray.h"

//---------------------------------------------------------------------------

namespace stw_opensyde_core
{
//---------------------------------------------------------------------------
///Comfort layer on top of the hex file class
class C_OsyHexFile :
   public stw_diag_lib::C_XFLHexFile
{
public:
   stw_types::sint32 GetSignatureBlockAddress(stw_types::uint32 & oru32_Address);
   stw_types::sint32 ScanDeviceIdFromHexFile(stw_scl::C_SCLString & orc_DeviceID);
   stw_types::sint32 ScanApplicationInformationBlockFromHexFile(stw_diag_lib::C_XFLECUInformation & orc_InfoBlock);
};

//---------------------------------------------------------------------------
}

#endif
