//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       header for class C_XFLHexFile

   Comfort layer on top of the hex file class

   \copyright   Copyright 2002 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CXFLHexfileH
#define CXFLHexfileH

#include "stwtypes.h"
#include "CHexFile.h"
#include "CXFLECUInformation.h"
#include "CSCLString.h"
#include "SCLDynamicArray.h"
#include "DiagLib_config.h"

//----------------------------------------------------------------------------------------------------------------------

namespace stw_diag_lib
{

//----------------------------------------------------------------------------------------------------------------------
///Comfort layer on top of the hex file class
class STW_DIAGLIB_EXPORT C_XFLHexFile : public stw_hex_file::C_HexFile
{
public:
   stw_scl::C_SCLString ErrorCodeToErrorText(const stw_types::uint32 ou32_ErrorCode) const;

   //STW specific: find magic block(s) and return all information contained there:
   stw_types::sint32 GetECUInformationBlocks(stw_scl::SCLDynamicArray <C_XFLECUInformation> & orc_InfoBlocks,
                                             const stw_types::uint32 ou32_SearchStartAddress,
                                             const bool oq_OnlyOneBlock, const bool oq_ExactAddressMatch,
                                             const bool oq_Block0Only);
   stw_types::sint32 CalcFileChecksum(stw_types::uint32 & oru32_Checksum);
};

//----------------------------------------------------------------------------------------------------------------------

}

#endif
