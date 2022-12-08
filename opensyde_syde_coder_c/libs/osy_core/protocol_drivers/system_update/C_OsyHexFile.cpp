//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Hex file class

   For details cf. documentation in .h file.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#include "precomp_headers.hpp" //pre-compiled headers

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OsyHexFile.hpp"
#include "C_SclString.hpp"
#include "CXFLECUInformation.hpp"
#include "TglUtils.hpp"

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::scl;
using namespace stw::diag_lib;

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   get address of signature block from hex file

   Identify address of signature block in file.
   The address of the first found block is returned.

   Possible approach when we have the flash packages: put the address into the package information.
    The signature generator has this information anyway.

   \param[out]     oru32_Address             address of signature block

   \return
   C_NO_ERR     everything OK; block found
   C_NOACT      no block found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OsyHexFile::GetSignatureBlockAddress(uint32_t & oru32_Address)
{
   int32_t s32_Return;

   oru32_Address = this->mu32_MinAdr;

   //lint -e{926}
   s32_Return = this->FindPattern(oru32_Address, 10, reinterpret_cast<const uint8_t *>(";zwm2KgUZ!"));
   if (s32_Return == -1)
   {
      s32_Return = C_NOACT;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   extract device ID from hex-file

   Scan through the hex-file and try to find the "application_info" structure.
   Then extract the device ID from it and return it.
   If the application_info structure with the device ID information is in the hex file more than once, AND
    the device ID in at least one of the copies is different, we fail.

   \param[out]    orc_DeviceId                     device ID found in hex-file

   \return
   C_NO_ERR     everything OK (device ID in orc_DeviceId)
   C_NOACT      device-ID not found
   C_CONFIG     ambiguous device-IDs in hex-file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OsyHexFile::ScanDeviceIdFromHexFile(C_SclString & orc_DeviceId)
{
   int32_t s32_Return;
   C_SclString c_DeviceId = "";

   C_SclDynamicArray<C_XFLECUInformation> c_InfoBlocks;
   s32_Return = this->GetECUInformationBlocks(c_InfoBlocks, 0x0U, false, false, true);
   if ((s32_Return == C_NO_ERR) && (c_InfoBlocks.GetLength() > 0))
   {
      int32_t s32_Index;
      for (s32_Index = 0; s32_Index < c_InfoBlocks.GetLength(); s32_Index++)
      {
         if (c_InfoBlocks[s32_Index].ContainsDeviceID() == true)
         {
            const C_SclString c_Help = c_InfoBlocks[s32_Index].GetDeviceID();
            if (c_DeviceId == "") //no device-ID seen yet ...
            {
               c_DeviceId = c_Help;
            }
            else
            {
               if (c_DeviceId != c_Help)
               {
                  s32_Return = C_CONFIG; //ambiguous device IDs !
                  break;
               }
            }
         }
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      if (c_DeviceId != "")
      {
         orc_DeviceId = c_DeviceId;
      }
      else
      {
         s32_Return = C_NOACT;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   extract application information block from hex-file

   Scan through the hex-file and try to find the "application_info" structure.
   Then extract the information from it and return it.
   Multiple instances of the application_info structure are considered an error
   or warning depending on the device names reported in all application blocks.

   \param[out]    orc_InfoBlock        application info block found in hex file

   \return
   C_NO_ERR     everything OK
   C_WARN       multiple application blocks detected in hex file but device names match
                  output in this case is the first found application block
   C_NOACT      no application information block detected in hex file
   C_OVERFLOW   multiple application information blocks detected in hex file and device names differ
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OsyHexFile::ScanApplicationInformationBlockFromHexFile(C_XFLECUInformation & orc_InfoBlock)
{
   int32_t s32_Return;

   C_SclDynamicArray<C_XFLECUInformation> c_InfoBlocks;
   s32_Return = this->GetECUInformationBlocks(c_InfoBlocks, 0x0U, false, false, true);
   tgl_assert(s32_Return == C_NO_ERR); //no plausible reasons documented

   if (c_InfoBlocks.GetLength() == 0)
   {
      s32_Return = C_NOACT;
   }
   else if (c_InfoBlocks.GetLength() > 1)
   {
      for (int32_t s32_Pos = 1; s32_Pos < c_InfoBlocks.GetLength(); s32_Pos++)
      {
         // compare every device name with first device name, this is enough because all must be equal
         if (c_InfoBlocks[0].GetDeviceID() != c_InfoBlocks[s32_Pos].GetDeviceID())
         {
            s32_Return = C_OVERFLOW;
         }
         else
         {
            orc_InfoBlock = c_InfoBlocks[0];
            s32_Return = C_WARN;
         }
      }
   }
   else
   {
      //one block found !
      orc_InfoBlock = c_InfoBlocks[0];
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}
